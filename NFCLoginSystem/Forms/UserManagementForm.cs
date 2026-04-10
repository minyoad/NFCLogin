using NFCLoginSystem.Models;
using NFCLoginSystem.Services;
using System;
using System.Collections.Generic;
using System.Drawing;
using System.Windows.Forms;

namespace NFCLoginSystem.Forms
{
    public partial class UserManagementForm : Form
    {
        private readonly DatabaseService _databaseService;
        private readonly AuthenticationService _authService;
        private List<User> _users = new();

        public UserManagementForm(AuthenticationService authService)
        {
            InitializeComponent();
            
            _databaseService = new DatabaseService();
            _authService = authService;
            
            LoadUsers();
            SetupDataGridView();
        }

        private void SetupDataGridView()
        {
            dataGridViewUsers.AutoGenerateColumns = false;
            dataGridViewUsers.AllowUserToAddRows = false;
            dataGridViewUsers.SelectionMode = DataGridViewSelectionMode.FullRowSelect;
            dataGridViewUsers.MultiSelect = false;

            // 添加列
            dataGridViewUsers.Columns.Clear();
            
            dataGridViewUsers.Columns.Add(new DataGridViewTextBoxColumn
            {
                Name = "Username",
                HeaderText = "用户名",
                DataPropertyName = "Username",
                Width = 120
            });

            dataGridViewUsers.Columns.Add(new DataGridViewTextBoxColumn
            {
                Name = "DisplayName",
                HeaderText = "显示名称",
                DataPropertyName = "DisplayName",
                Width = 120
            });

            dataGridViewUsers.Columns.Add(new DataGridViewTextBoxColumn
            {
                Name = "NFCCardId",
                HeaderText = "NFC卡ID",
                DataPropertyName = "NFCCardId",
                Width = 120
            });

            dataGridViewUsers.Columns.Add(new DataGridViewCheckBoxColumn
            {
                Name = "IsActive",
                HeaderText = "激活状态",
                DataPropertyName = "IsActive",
                Width = 80
            });

            dataGridViewUsers.Columns.Add(new DataGridViewCheckBoxColumn
            {
                Name = "IsAdmin",
                HeaderText = "管理员",
                DataPropertyName = "IsAdmin",
                Width = 80
            });

            dataGridViewUsers.Columns.Add(new DataGridViewTextBoxColumn
            {
                Name = "CreatedAt",
                HeaderText = "创建时间",
                DataPropertyName = "CreatedAt",
                Width = 150,
                DefaultCellStyle = { Format = "yyyy-MM-dd HH:mm:ss" }
            });

            dataGridViewUsers.Columns.Add(new DataGridViewTextBoxColumn
            {
                Name = "LastLogin",
                HeaderText = "最后登录",
                DataPropertyName = "LastLogin",
                Width = 150,
                DefaultCellStyle = { Format = "yyyy-MM-dd HH:mm:ss" }
            });
        }

        private void LoadUsers()
        {
            try
            {
                _users = _databaseService.GetAllUsers();
                dataGridViewUsers.DataSource = null;
                dataGridViewUsers.DataSource = _users;
                lblTotalUsers.Text = $"总用户数: {_users.Count}";
            }
            catch (Exception ex)
            {
                MessageBox.Show($"加载用户列表失败: {ex.Message}", "错误", 
                    MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
        }

        private void btnAddUser_Click(object sender, EventArgs e)
        {
            var addUserForm = new AddUserForm();
            if (addUserForm.ShowDialog() == DialogResult.OK)
            {
                LoadUsers();
            }
        }

        private void btnEditUser_Click(object sender, EventArgs e)
        {
            if (dataGridViewUsers.SelectedRows.Count == 0)
            {
                MessageBox.Show("请选择要编辑的用户", "提示", 
                    MessageBoxButtons.OK, MessageBoxIcon.Warning);
                return;
            }

            var selectedUser = dataGridViewUsers.SelectedRows[0].DataBoundItem as User;
            if (selectedUser != null)
            {
                var editUserForm = new EditUserForm(selectedUser);
                if (editUserForm.ShowDialog() == DialogResult.OK)
                {
                    LoadUsers();
                }
            }
        }

        private void btnDeleteUser_Click(object sender, EventArgs e)
        {
            if (dataGridViewUsers.SelectedRows.Count == 0)
            {
                MessageBox.Show("请选择要删除的用户", "提示", 
                    MessageBoxButtons.OK, MessageBoxIcon.Warning);
                return;
            }

            var selectedUser = dataGridViewUsers.SelectedRows[0].DataBoundItem as User;
            if (selectedUser != null)
            {
                if (selectedUser.Username == _authService.CurrentUser?.Username)
                {
                    MessageBox.Show("不能删除当前登录的用户", "提示", 
                        MessageBoxButtons.OK, MessageBoxIcon.Warning);
                    return;
                }

                var result = MessageBox.Show(
                    $"确定要删除用户 '{selectedUser.Username}' 吗？\n\n此操作不可撤销。", 
                    "确认删除", 
                    MessageBoxButtons.YesNo, 
                    MessageBoxIcon.Question);

                if (result == DialogResult.Yes)
                {
                    try
                    {
                        // 删除用户及其相关数据
                        _databaseService.DeleteUser(selectedUser.Id);
                        MessageBox.Show("用户删除成功", "成功", 
                            MessageBoxButtons.OK, MessageBoxIcon.Information);
                        LoadUsers();
                    }
                    catch (Exception ex)
                    {
                        MessageBox.Show($"删除用户失败: {ex.Message}", "错误", 
                            MessageBoxButtons.OK, MessageBoxIcon.Error);
                    }
                }
            }
        }

        private void btnRefresh_Click(object sender, EventArgs e)
        {
            LoadUsers();
        }

        private void txtSearch_TextChanged(object sender, EventArgs e)
        {
            var searchText = txtSearch.Text.ToLower();
            
            if (string.IsNullOrEmpty(searchText))
            {
                dataGridViewUsers.DataSource = _users;
            }
            else
            {
                var filteredUsers = _users.FindAll(user => 
                    user.Username.ToLower().Contains(searchText) ||
                    user.DisplayName.ToLower().Contains(searchText) ||
                    (user.NFCCardId?.ToLower().Contains(searchText) ?? false));
                
                dataGridViewUsers.DataSource = null;
                dataGridViewUsers.DataSource = filteredUsers;
            }
        }

        private void dataGridViewUsers_SelectionChanged(object sender, EventArgs e)
        {
            btnEditUser.Enabled = dataGridViewUsers.SelectedRows.Count > 0;
            btnDeleteUser.Enabled = dataGridViewUsers.SelectedRows.Count > 0;
        }

        private void dataGridViewUsers_CellDoubleClick(object sender, DataGridViewCellEventArgs e)
        {
            if (e.RowIndex >= 0)
            {
                btnEditUser.PerformClick();
            }
        }

        private void btnClose_Click(object sender, EventArgs e)
        {
            Close();
        }
    }
}