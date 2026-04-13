using System;
using System.Windows.Forms;
using NFCLoginSystem.Services;
using System.Security.Principal;

namespace NFCLoginSystem.Forms
{
    public partial class MainForm : Form
    {
        private readonly DatabaseService _dbService;
        private readonly string _currentWindowsUser;
        private Label _nfcStatusLabel;

        public MainForm()
        {
            _dbService = new DatabaseService();
            _currentWindowsUser = WindowsIdentity.GetCurrent().Name;
            InitializeComponent();
            LoadNfcStatus();
        }

        private void InitializeComponent()
        {
            this.SuspendLayout();
            
            // Window settings
            this.Text = "NFC 卡管理";
            this.Size = new System.Drawing.Size(400, 300);
            this.StartPosition = FormStartPosition.CenterScreen;

            // Welcome label
            var welcomeLabel = new Label
            {
                Text = $"当前用户: {_currentWindowsUser}",
                Location = new System.Drawing.Point(20, 20),
                AutoSize = true
            };
            this.Controls.Add(welcomeLabel);

            // NFC Status label
            _nfcStatusLabel = new Label
            {
                Location = new System.Drawing.Point(20, 60),
                AutoSize = true
            };
            this.Controls.Add(_nfcStatusLabel);

            // Bind/Update NFC Card button
            var bindButton = new Button
            {
                Text = "绑定/更新NFC卡",
                Location = new System.Drawing.Point(20, 100),
                Size = new System.Drawing.Size(150, 40)
            };
            bindButton.Click += BindButton_Click;
            this.Controls.Add(bindButton);

            // Unbind NFC Card button
            var unbindButton = new Button
            {
                Text = "解绑NFC卡",
                Location = new System.Drawing.Point(200, 100),
                Size = new System.Drawing.Size(150, 40)
            };
            unbindButton.Click += UnbindButton_Click;
            this.Controls.Add(unbindButton);
            
            this.ResumeLayout(false);
            this.PerformLayout();
        }

        private void LoadNfcStatus()
        {
            var user = _dbService.GetUserByUsername(_currentWindowsUser);
            if (user != null && !string.IsNullOrEmpty(user.NFCCardId))
            {
                _nfcStatusLabel.Text = $"NFC卡已绑定: {user.NFCCardId}";
            }
            else
            {
                _nfcStatusLabel.Text = "NFC卡未绑定";
            }
        }

        private void BindButton_Click(object sender, EventArgs e)
        {
            using (var bindForm = new BindNFCForm(_currentWindowsUser))
            {
                bindForm.ShowDialog();
            }
            LoadNfcStatus(); // Refresh status after binding
        }

        private void UnbindButton_Click(object sender, EventArgs e)
        {
            var user = _dbService.GetUserByUsername(_currentWindowsUser);
            if (user != null && !string.IsNullOrEmpty(user.NFCCardId))
            {
                _dbService.UpdateUserNFCCard(_currentWindowsUser, null);
                MessageBox.Show("NFC卡已成功解绑。", "成功", MessageBoxButtons.OK, MessageBoxIcon.Information);
                LoadNfcStatus(); // Refresh status
            }
            else
            {
                MessageBox.Show("当前用户没有绑定NFC卡。", "提示", MessageBoxButtons.OK, MessageBoxIcon.Information);
            }
        }
    }
}