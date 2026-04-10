using NFCLoginSystem.Services;
using System;
using System.Windows.Forms;

namespace NFCLoginSystem.Forms
{
    public partial class AddUserForm : Form
    {
        private readonly DatabaseService _databaseService;

        public AddUserForm()
        {
            InitializeComponent();
            _databaseService = new DatabaseService();
        }

        private void btnSave_Click(object sender, EventArgs e)
        {
            try
            {
                string username = txtUsername.Text.Trim();
                string password = txtPassword.Text;
                string confirmPassword = txtConfirmPassword.Text;
                string displayName = txtDisplayName.Text.Trim();
                string nfcCardId = txtNFCCardId.Text.Trim();
                bool isAdmin = chkIsAdmin.Checked;

                // 验证输入
                if (string.IsNullOrEmpty(username))
                {
                    MessageBox.Show("请输入用户名", "提示", MessageBoxButtons.OK, MessageBoxIcon.Warning);
                    txtUsername.Focus();
                    return;
                }

                if (username.Length < 3)
                {
                    MessageBox.Show("用户名长度至少为3个字符", "提示", MessageBoxButtons.OK, MessageBoxIcon.Warning);
                    txtUsername.Focus();
                    return;
                }

                if (string.IsNullOrEmpty(password))
                {
                    MessageBox.Show("请输入密码", "提示", MessageBoxButtons.OK, MessageBoxIcon.Warning);
                    txtPassword.Focus();
                    return;
                }

                if (password.Length < 6)
                {
                    MessageBox.Show("密码长度至少为6个字符", "提示", MessageBoxButtons.OK, MessageBoxIcon.Warning);
                    txtPassword.Focus();
                    return;
                }

                if (password != confirmPassword)
                {
                    MessageBox.Show("两次输入的密码不一致", "提示", MessageBoxButtons.OK, MessageBoxIcon.Warning);
                    txtConfirmPassword.Focus();
                    return;
                }

                if (string.IsNullOrEmpty(displayName))
                {
                    MessageBox.Show("请输入显示名称", "提示", MessageBoxButtons.OK, MessageBoxIcon.Warning);
                    txtDisplayName.Focus();
                    return;
                }

                // 检查用户名是否已存在
                var existingUser = _databaseService.GetUserByUsername(username);
                if (existingUser != null)
                {
                    MessageBox.Show("用户名已存在，请选择其他用户名", "提示", MessageBoxButtons.OK, MessageBoxIcon.Warning);
                    txtUsername.Focus();
                    return;
                }

                // 检查NFC卡ID是否已绑定
                if (!string.IsNullOrEmpty(nfcCardId))
                {
                    var existingNFCUser = _databaseService.GetUserByNFCCardId(nfcCardId);
                    if (existingNFCUser != null)
                    {
                        MessageBox.Show("该NFC卡已绑定到其他用户", "提示", MessageBoxButtons.OK, MessageBoxIcon.Warning);
                        txtNFCCardId.Focus();
                        return;
                    }
                }

                // 创建用户
                string passwordHash = AuthenticationService.HashPassword(password);
                _databaseService.CreateUser(username, passwordHash, displayName, isAdmin);

                // 如果提供了NFC卡ID，更新用户信息
                if (!string.IsNullOrEmpty(nfcCardId))
                {
                    var newUser = _databaseService.GetUserByUsername(username);
                    if (newUser != null)
                    {
                        _databaseService.UpdateUserNFCCard(newUser.Id, nfcCardId);
                    }
                }

                MessageBox.Show("用户创建成功！", "成功", MessageBoxButtons.OK, MessageBoxIcon.Information);
                DialogResult = DialogResult.OK;
                Close();
            }
            catch (Exception ex)
            {
                MessageBox.Show($"创建用户失败: {ex.Message}", "错误", MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
        }

        private void btnCancel_Click(object sender, EventArgs e)
        {
            DialogResult = DialogResult.Cancel;
            Close();
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

        private void txtPassword_TextChanged(object sender, EventArgs e)
        {
            UpdatePasswordStrength();
        }

        private void UpdatePasswordStrength()
        {
            string password = txtPassword.Text;
            
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