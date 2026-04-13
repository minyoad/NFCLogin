using NFCLoginSystem.Models;
using NFCLoginSystem.Services;
using System;
using System.Collections.Generic;
using System.Drawing;
using System.Windows.Forms;
using System.DirectoryServices.AccountManagement;

namespace NFCLoginSystem.Forms
{
    public partial class UserManagementForm : Form
    {
        private readonly DatabaseService _databaseService;
        private readonly AuthenticationService _authService;
        private List<UserPrincipal> _users = new();
        private List<object> _displayUsers = new();

        public UserManagementForm(AuthenticationService authService)
        {
            InitializeComponent();
            
            _databaseService = new DatabaseService();
            _authService = authService;
            
            btnEditUser.Enabled = false; // 暂时禁用编辑功能

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
                DataPropertyName = "SamAccountName",
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
                DataPropertyName = "NFCCardId", // This will be custom loaded
                Width = 120
            });

            dataGridViewUsers.Columns.Add(new DataGridViewCheckBoxColumn
            {
                Name = "IsActive",
                HeaderText = "激活状态",
                DataPropertyName = "Enabled",
                Width = 80
            });

            dataGridViewUsers.Columns.Add(new DataGridViewTextBoxColumn
            {
                Name = "LastLogin",
                HeaderText = "最后登录",
                DataPropertyName = "LastLogon",
                Width = 150,
                DefaultCellStyle = { Format = "yyyy-MM-dd HH:mm:ss" }
            });
        }

        private void LoadUsers()
        {
            try
            {
                using (var context = new PrincipalContext(ContextType.Machine))
                {
                    var userPrincipal = new UserPrincipal(context);
                    using (var searcher = new PrincipalSearcher(userPrincipal))
                    {
                        _users = searcher.FindAll().Cast<UserPrincipal>().ToList();
                    }
                }

                // Now, we need a way to display this data along with NFC card IDs.
                // For now, let's just display the windows users.
                // We will create a custom view model later.
                var displayUsers = _users.Select(u => new 
                {
                    u.SamAccountName,
                    u.DisplayName,
                    u.Enabled,
                    u.LastLogon,
                    NFCCardId = _databaseService.GetUserByUsername(u.SamAccountName)?.NFCCardId ?? ""
                }).Cast<object>().ToList();

                _displayUsers = displayUsers;

                dataGridViewUsers.DataSource = null;
                dataGridViewUsers.DataSource = _displayUsers;
                lblTotalUsers.Text = $"总用户数: {_users.Count}";
            }
            catch (Exception ex)
            {
                MessageBox.Show($"加载系统用户列表失败: {ex.Message}\n\n请确保程序以管理员权限运行。", "错误", 
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
            MessageBox.Show("编辑系统用户功能正在开发中。", "提示", 
                MessageBoxButtons.OK, MessageBoxIcon.Information);
        }

        private void btnDeleteUser_Click(object sender, EventArgs e)
        {
            if (dataGridViewUsers.SelectedRows.Count == 0)
            {
                MessageBox.Show("请选择要删除的用户", "提示", 
                    MessageBoxButtons.OK, MessageBoxIcon.Warning);
                return;
            }

            var selectedRow = dataGridViewUsers.SelectedRows[0];
            var username = selectedRow.Cells["Username"].Value as string;

            if (string.IsNullOrEmpty(username))
            {
                MessageBox.Show("无法获取所选用户的用户名。", "错误",
                    MessageBoxButtons.OK, MessageBoxIcon.Error);
                return;
            }

            if (username.Equals(_authService.CurrentUser?.SamAccountName, StringComparison.OrdinalIgnoreCase))
            {
                MessageBox.Show("不能删除当前登录的用户", "提示", 
                    MessageBoxButtons.OK, MessageBoxIcon.Warning);
                return;
            }

            var result = MessageBox.Show(
                $"确定要删除Windows系统用户 '{username}' 吗？\n\n此操作将永久删除该系统账户及其所有相关数据，且不可撤销。", 
                "确认删除", 
                MessageBoxButtons.YesNo, 
                MessageBoxIcon.Question);

            if (result == DialogResult.Yes)
            {
                try
                {
                    using (var context = new PrincipalContext(ContextType.Machine))
                    {
                        var user = UserPrincipal.FindByIdentity(context, IdentityType.SamAccountName, username);
                        if (user != null)
                        {
                            user.Delete();
                        }
                    }

                    // 删除数据库中的NFC绑定关系
                    _databaseService.DeleteUserMapping(username);

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

        private void btnRefresh_Click(object sender, EventArgs e)
        {
            LoadUsers();
        }

        private void txtSearch_TextChanged(object sender, EventArgs e)
        {
            var searchText = txtSearch.Text.ToLower();
            
            if (string.IsNullOrEmpty(searchText))
            {
                dataGridViewUsers.DataSource = null;
                dataGridViewUsers.DataSource = _displayUsers;
            }
            else
            {
                var filteredUsers = _displayUsers.FindAll(u =>
                {
                    dynamic user = u;
                    string username = user.SamAccountName.ToLower();
                    string displayName = user.DisplayName?.ToLower() ?? "";
                    string nfcCardId = user.NFCCardId?.ToLower() ?? "";

                    return username.Contains(searchText) ||
                           displayName.Contains(searchText) ||
                           nfcCardId.Contains(searchText);
                });
                
                dataGridViewUsers.DataSource = null;
                dataGridViewUsers.DataSource = filteredUsers;
            }
        }

        private void dataGridViewUsers_SelectionChanged(object sender, EventArgs e)
        {
            bool hasSelection = dataGridViewUsers.SelectedRows.Count > 0;
            btnDeleteUser.Enabled = hasSelection;
            btnBindNFC.Enabled = hasSelection;
        }

        private void btnBindNFC_Click(object sender, EventArgs e)
        {
            if (dataGridViewUsers.SelectedRows.Count == 0)
            {
                MessageBox.Show("请选择要绑定NFC卡的用户", "提示",
                    MessageBoxButtons.OK, MessageBoxIcon.Warning);
                return;
            }

            var selectedRow = dataGridViewUsers.SelectedRows[0];
            // Since the DataBoundItem is an anonymous type, we need to get the username differently.
            var username = selectedRow.Cells["Username"].Value as string;

            if (string.IsNullOrEmpty(username))
            {
                MessageBox.Show("无法获取所选用户的用户名。", "错误",
                    MessageBoxButtons.OK, MessageBoxIcon.Error);
                return;
            }

            var bindingForm = new BindNFCForm();
            if (bindingForm.ShowDialog() == DialogResult.OK)
            {
                string? cardId = bindingForm.CardId;
                if (string.IsNullOrEmpty(cardId))
                {
                    MessageBox.Show("未能从绑定窗口获取到有效的NFC卡ID。请重试。", "绑定失败",
                        MessageBoxButtons.OK, MessageBoxIcon.Warning);
                    return;
                }
                
                try
                {
                    // 检查该NFC卡是否已被其他用户绑定
                    var existingUser = _databaseService.GetUserByNFCCardId(cardId);
                    if (existingUser != null && existingUser.Username != username)
                    {
                        MessageBox.Show($"此NFC卡已被用户 '{existingUser.Username}' 绑定。", "绑定失败",
                            MessageBoxButtons.OK, MessageBoxIcon.Error);
                        return;
                    }

                    _databaseService.UpdateUserNFCCard(username, cardId);
                    MessageBox.Show("NFC卡绑定成功！", "成功",
                        MessageBoxButtons.OK, MessageBoxIcon.Information);
                    LoadUsers();
                }
                catch (Exception ex)
                {
                    MessageBox.Show($"绑定NFC卡时发生意外错误: \n\n{ex.ToString()}", "严重错误",
                        MessageBoxButtons.OK, MessageBoxIcon.Error);
                }
            }
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