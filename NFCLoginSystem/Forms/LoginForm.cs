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
        private readonly NFCSerialService _nfcService;
        private bool _nfcReading = false;

        public LoginForm()
        {
            InitializeComponent();
            
            _databaseService = new DatabaseService();
            _authService = new AuthenticationService(_databaseService);
            _nfcService = new NFCSerialService();
            
            SetupNFCService();
            LoadAvailablePorts();
        }

        private void SetupNFCService()
        {
            _nfcService.CardDetected += OnNFCCardDetected;
            _nfcService.ErrorOccurred += OnNFCError;
        }

        private void LoadAvailablePorts()
        {
            try
            {
                var ports = _nfcService.GetAvailablePorts();
                cmbSerialPorts.Items.Clear();
                cmbSerialPorts.Items.AddRange(ports);
                
                if (ports.Length > 0)
                {
                    cmbSerialPorts.SelectedIndex = 0;
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show($"加载串口失败: {ex.Message}", "错误", MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
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

        private void btnConnectNFC_Click(object sender, EventArgs e)
        {
            try
            {
                if (cmbSerialPorts.SelectedItem == null)
                {
                    MessageBox.Show("请选择串口", "提示", 
                        MessageBoxButtons.OK, MessageBoxIcon.Warning);
                    return;
                }

                string selectedPort = cmbSerialPorts.SelectedItem.ToString()!;
                
                if (_nfcReading)
                {
                    StopNFCReading();
                }
                else
                {
                    StartNFCReading(selectedPort);
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show($"NFC连接失败: {ex.Message}", "错误", 
                    MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
        }

        private void StartNFCReading(string portName)
        {
            try
            {
                _nfcService.Disconnect();
                
                var nfcService = new NFCSerialService(portName);
                nfcService.CardDetected += OnNFCCardDetected;
                nfcService.ErrorOccurred += OnNFCError;
                
                if (nfcService.Connect())
                {
                    _nfcReading = true;
                    btnConnectNFC.Text = "断开NFC";
                    lblNFCStatus.Text = "NFC读卡器已连接";
                    lblNFCStatus.ForeColor = Color.Green;
                    nfcService.StartReading();
                }
                else
                {
                    MessageBox.Show("无法连接到NFC读卡器", "错误", 
                        MessageBoxButtons.OK, MessageBoxIcon.Error);
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show($"启动NFC读取失败: {ex.Message}", "错误", 
                    MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
        }

        private void StopNFCReading()
        {
            try
            {
                _nfcReading = false;
                _nfcService.StopReading();
                _nfcService.Disconnect();
                
                btnConnectNFC.Text = "连接NFC";
                lblNFCStatus.Text = "NFC读卡器未连接";
                lblNFCStatus.ForeColor = Color.Red;
            }
            catch (Exception ex)
            {
                MessageBox.Show($"停止NFC读取失败: {ex.Message}", "错误", 
                    MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
        }

        private void btnRefreshPorts_Click(object sender, EventArgs e)
        {
            LoadAvailablePorts();
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
                if (_nfcReading)
                {
                    StopNFCReading();
                }
                _nfcService.Dispose();
            }
            catch (Exception ex)
            {
                // 忽略关闭时的错误
            }
        }

        private void btnUserManagement_Click(object sender, EventArgs e)
        {
            try
            {
                // 显示登录对话框进行管理员身份验证
                var loginForm = new LoginForm();
                if (loginForm.ShowDialog() == DialogResult.OK)
                {
                    // 检查当前用户是否是管理员
                    if (_authService.CurrentUser != null && _authService.CurrentUser.IsAdmin)
                    {
                        var userManagementForm = new UserManagementForm(_authService);
                        userManagementForm.ShowDialog();
                    }
                    else
                    {
                        MessageBox.Show("只有管理员才能访问用户管理功能", "权限不足", 
                            MessageBoxButtons.OK, MessageBoxIcon.Warning);
                    }
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show($"打开用户管理失败: {ex.Message}", "错误", 
                    MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
        }
    }
}