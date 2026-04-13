using NFCLoginSystem.Services;
using System;
using System.Drawing;
using System.Windows.Forms;

namespace NFCLoginSystem.Forms
{
    public partial class LoginForm : Form
    {
        private readonly DatabaseService _databaseService;
        private readonly AuthenticationService _authService;
        private NFCPCSCService? _nfcService;

        public LoginForm()
        {
            InitializeComponent();
            
            _databaseService = new DatabaseService();
            _authService = new AuthenticationService(_databaseService);
            
            try
            {
                _nfcService = new NFCPCSCService();
                SetupNFCService();
                _nfcService.StartMonitoring();
            }
            catch (Exception ex)
            {
                MessageBox.Show($"NFC服务初始化失败: {ex.Message}", "错误", 
                    MessageBoxButtons.OK, MessageBoxIcon.Error);
                lblNFCStatus.Text = "NFC服务不可用";
                lblNFCStatus.ForeColor = Color.Red;
            }
            
            // 设置默认图像以避免空引用异常
            pictureBoxLogo.Image = SystemIcons.Information.ToBitmap();
        }

        private void SetupNFCService()
        {
            if (_nfcService == null) return;
            _nfcService.CardDetected += OnNFCCardDetected;
            _nfcService.ErrorOccurred += OnNFCError;
            _nfcService.ReaderStatusChanged += OnReaderStatusChanged;
        }

        private void OnReaderStatusChanged(object? sender, string status)
        {
            if (InvokeRequired)
            {
                Invoke(new Action(() => OnReaderStatusChanged(sender, status)));
                return;
            }

            lblNFCStatus.Text = status;
            lblNFCStatus.ForeColor = status.Contains("未连接") || status.Contains("不可用") ? Color.Red : Color.Green;
        }

        private void OnNFCCardDetected(object? sender, string cardId)
        {
            if (InvokeRequired)
            {
                Invoke(new Action(() => OnNFCCardDetected(sender, cardId)));
                return;
            }

            try
            {
                lblNFCStatus.Text = $"检测到NFC卡: {cardId}";
                lblNFCStatus.ForeColor = Color.Green;

                var result = _authService.LoginWithNFC(cardId);
                
                if (result.Success)
                {
                    MessageBox.Show($"NFC登录成功！欢迎 {result.User?.DisplayName}", "登录成功", 
                        MessageBoxButtons.OK, MessageBoxIcon.Information);
                    
                    DialogResult = DialogResult.OK;
                    Close();
                }
                else
                {
                    MessageBox.Show(result.Message, "登录失败", 
                        MessageBoxButtons.OK, MessageBoxIcon.Warning);
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show($"NFC登录处理失败: {ex.Message}", "错误", 
                    MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
        }

        private void OnNFCError(object? sender, string errorMessage)
        {
            if (InvokeRequired)
            {
                Invoke(new Action(() => OnNFCError(sender, errorMessage)));
                return;
            }

            MessageBox.Show(errorMessage, "NFC错误", 
                MessageBoxButtons.OK, MessageBoxIcon.Error);
        }

        private void btnLogin_Click(object sender, EventArgs e)
        {
            try
            {
                string username = txtUsername.Text.Trim();
                string password = txtPassword.Text;

                if (string.IsNullOrEmpty(username))
                {
                    MessageBox.Show("请输入用户名", "提示", 
                        MessageBoxButtons.OK, MessageBoxIcon.Warning);
                    txtUsername.Focus();
                    return;
                }

                if (string.IsNullOrEmpty(password))
                {
                    MessageBox.Show("请输入密码", "提示", 
                        MessageBoxButtons.OK, MessageBoxIcon.Warning);
                    txtPassword.Focus();
                    return;
                }

                var result = _authService.Login(username, password);
                
                if (result.Success)
                {
                    MessageBox.Show($"登录成功！欢迎 {result.User?.DisplayName}", "登录成功", 
                        MessageBoxButtons.OK, MessageBoxIcon.Information);
                    
                    DialogResult = DialogResult.OK;
                    Close();
                }
                else
                {
                    MessageBox.Show(result.Message, "登录失败", 
                        MessageBoxButtons.OK, MessageBoxIcon.Warning);
                    txtPassword.Clear();
                    txtPassword.Focus();
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show($"登录失败: {ex.Message}", "错误", 
                    MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
        }

        private void txtPassword_KeyDown(object sender, KeyEventArgs e)
        {
            if (e.KeyCode == Keys.Enter)
            {
                btnLogin.PerformClick();
                e.Handled = true;
            }
        }

        private void LoginForm_FormClosing(object sender, FormClosingEventArgs e)
        {
            try
            {
                if (_nfcService != null)
                {
                    _nfcService.StopMonitoring();
                    _nfcService.Dispose();
                }
            }
            catch (Exception)
            {
                // 忽略关闭时的错误
            }
        }

        private void btnUserManagement_Click(object sender, EventArgs e)
        {
            // This button should only be visible/enabled after a successful login.
            // We will assume the main form handles the visibility of this button.

            // To access user management, a user must be logged in and be an admin.
            if (_authService.CurrentUser == null)
            {
                MessageBox.Show("请先登录。", "提示", 
                    MessageBoxButtons.OK, MessageBoxIcon.Information);
                return;
            }

            if (!_authService.CurrentUser.IsAdmin)
            {
                MessageBox.Show("只有管理员才能访问用户管理功能。", "权限不足", 
                    MessageBoxButtons.OK, MessageBoxIcon.Warning);
                return;
            }

            try
            {
                // Hide the login form and show the user management form.
                this.Hide();
                var userManagementForm = new UserManagementForm(_authService);
                userManagementForm.ShowDialog();
                // After the user management form is closed, show the login form again.
                this.Show();
            }
            catch (Exception ex)
            {
                MessageBox.Show($"打开用户管理失败: {ex.Message}", "错误", 
                    MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
        }
    }
}