using NFCLoginSystem.Models;
using NFCLoginSystem.Services;
using System;
using System.Windows.Forms;

namespace NFCLoginSystem.Forms
{
    public partial class EditUserForm : Form
    {
        private readonly DatabaseService _databaseService;
        private readonly User _user;

        public EditUserForm(User user)
        {
            InitializeComponent();
            _databaseService = new DatabaseService();
            _user = user;
            LoadUserData();
        }

        private void LoadUserData()
        {
            txtUsername.Text = _user.Username;
            txtDisplayName.Text = _user.DisplayName;
            txtNFCCardId.Text = _user.NFCCardId ?? "";
            chkIsActive.Checked = _user.IsActive;
            chkIsAdmin.Checked = _user.IsAdmin;
            lblCreatedAt.Text = $"创建时间: {_user.CreatedAt:yyyy-MM-dd HH:mm:ss}";
            lblLastLogin.Text = $"最后登录: {(_user.LastLogin == DateTime.MinValue ? "从未登录" : _user.LastLogin.ToString("yyyy-MM-dd HH:mm:ss"))}";
            
            // 用户名不能修改
            txtUsername.Enabled = false;
        }

        private void btnSave_Click(object sender, EventArgs e)
        {
            try
            {
                string displayName = txtDisplayName.Text.Trim();
                string nfcCardId = txtNFCCardId.Text.Trim();
                bool isActive = chkIsActive.Checked;
                bool isAdmin = chkIsAdmin.Checked;
                string newPassword = txtNewPassword.Text;

                // 验证输入
                if (string.IsNullOrEmpty(displayName))
                {
                    MessageBox.Show("请输入显示名称", "提示", MessageBoxButtons.OK, MessageBoxIcon.Warning);
                    txtDisplayName.Focus();
                    return;
                }

                // 检查NFC卡ID是否已绑定到其他用户
                if (!string.IsNullOrEmpty(nfcCardId) && nfcCardId != _user.NFCCardId)
                {
                    var existingNFCUser = _databaseService.GetUserByNFCCardId(nfcCardId);
                    if (existingNFCUser != null)
                    {
                        MessageBox.Show("该NFC卡已绑定到其他用户", "提示", MessageBoxButtons.OK, MessageBoxIcon.Warning);
                        txtNFCCardId.Focus();
                        return;
                    }
                }

                // 如果输入了新密码，验证密码强度
                if (!string.IsNullOrEmpty(newPassword))
                {
                    if (newPassword.Length < 6)
                    {
                        MessageBox.Show("新密码长度至少为6个字符", "提示", MessageBoxButtons.OK, MessageBoxIcon.Warning);
                        txtNewPassword.Focus();
                        return;
                    }

                    if (newPassword != txtConfirmPassword.Text)
                    {
                        MessageBox.Show("两次输入的新密码不一致", "提示", MessageBoxButtons.OK, MessageBoxIcon.Warning);
                        txtConfirmPassword.Focus();
                        return;
                    }
                }

                // 更新用户信息
                _user.DisplayName = displayName;
                _user.NFCCardId = string.IsNullOrEmpty(nfcCardId) ? null : nfcCardId;
                _user.IsActive = isActive;
                _user.IsAdmin = isAdmin;

                // 更新用户信息到数据库
                _databaseService.UpdateUser(_user);

                // 如果提供了新密码，更新密码
                if (!string.IsNullOrEmpty(newPassword))
                {
                    string newPasswordHash = AuthenticationService.HashPassword(newPassword);
                    _databaseService.UpdateUserPassword(_user.Id, newPasswordHash);
                }

                MessageBox.Show("用户信息更新成功！", "成功", MessageBoxButtons.OK, MessageBoxIcon.Information);
                DialogResult = DialogResult.OK;
                Close();
            }
            catch (Exception ex)
            {
                MessageBox.Show($"更新用户信息失败: {ex.Message}", "错误", MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
        }

        private void btnCancel_Click(object sender, EventArgs e)
        {
            DialogResult = DialogResult.Cancel;
            Close();
        }

        private void btnClearNFCCard_Click(object sender, EventArgs e)
        {
            txtNFCCardId.Clear();
        }

        private void btnGenerateNFCCard_Click(object sender, EventArgs e)
        {
            // 生成随机的NFC卡ID（模拟）
            string randomCardId = GenerateRandomCardId();
            txtNFCCardId.Text = randomCardId;
        }

        private string GenerateRandomCardId()
        {
            var random = new Random();
            var bytes = new byte[4];
            random.NextBytes(bytes);
            return BitConverter.ToString(bytes).Replace("-", "");
        }

        private void txtNewPassword_TextChanged(object sender, EventArgs e)
        {
            UpdatePasswordStrength();
        }

        private void UpdatePasswordStrength()
        {
            string password = txtNewPassword.Text;
            
            if (string.IsNullOrEmpty(password))
            {
                lblPasswordStrength.Text = "";
                lblPasswordStrength.ForeColor = SystemColors.ControlText;
                return;
            }

            int strength = CalculatePasswordStrength(password);
            
            if (strength < 3)
            {
                lblPasswordStrength.Text = "弱";
                lblPasswordStrength.ForeColor = Color.Red;
            }
            else if (strength < 5)
            {
                lblPasswordStrength.Text = "中等";
                lblPasswordStrength.ForeColor = Color.Orange;
            }
            else
            {
                lblPasswordStrength.Text = "强";
                lblPasswordStrength.ForeColor = Color.Green;
            }
        }

        private int CalculatePasswordStrength(string password)
        {
            int strength = 0;

            // 长度检查
            if (password.Length >= 6) strength++;
            if (password.Length >= 8) strength++;
            if (password.Length >= 12) strength++;

            // 字符类型检查
            if (System.Text.RegularExpressions.Regex.IsMatch(password, "[a-z]")) strength++;
            if (System.Text.RegularExpressions.Regex.IsMatch(password, "[A-Z]")) strength++;
            if (System.Text.RegularExpressions.Regex.IsMatch(password, "[0-9]")) strength++;
            if (System.Text.RegularExpressions.Regex.IsMatch(password, "[^a-zA-Z0-9]")) strength++;

            return strength;
        }
    }
}