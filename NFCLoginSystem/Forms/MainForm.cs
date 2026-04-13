using System;
using System.Windows.Forms;
using NFCLoginSystem.Services;

namespace NFCLoginSystem.Forms
{
    public partial class MainForm : Form
    {
        private readonly AuthenticationService _authService;

        public MainForm(AuthenticationService authService)
        {
            _authService = authService;
            SetupUI();
        }

        private void SetupUI()
        {
            this.Text = $"欢迎, {_authService.CurrentUser?.DisplayName}";

            // 根据用户是否为管理员显示不同内容
            if (_authService.IsCurrentUserAdmin())
            {
                // 创建用户管理按钮
                var btnUserManagement = new Button
                {
                    Text = "用户管理",
                    Location = new System.Drawing.Point(50, 50),
                    Size = new System.Drawing.Size(150, 50)
                };
                btnUserManagement.Click += BtnUserManagement_Click;
                this.Controls.Add(btnUserManagement);
            }
            else
            {
                // 为普通用户显示欢迎信息
                var lblWelcome = new Label
                {
                    Text = "您已成功登录。",
                    Location = new System.Drawing.Point(50, 50),
                    AutoSize = true
                };
                this.Controls.Add(lblWelcome);
            }

            // 创建退出登录按钮
            var btnLogout = new Button
            {
                Text = "退出登录",
                Location = new System.Drawing.Point(50, 120),
                Size = new System.Drawing.Size(150, 50)
            };
            btnLogout.Click += BtnLogout_Click;
            this.Controls.Add(btnLogout);
        }

        private void BtnUserManagement_Click(object? sender, EventArgs e)
        {
            // 隐藏主窗体，显示用户管理窗体
            this.Hide();
            var userManagementForm = new UserManagementForm(_authService);
            userManagementForm.ShowDialog();
            // 用户管理窗体关闭后，重新显示主窗体
            this.Show();
        }

        private void BtnLogout_Click(object? sender, EventArgs e)
        {
            _authService.Logout();
            this.Close(); // 关闭主窗体，将返回到登录窗体
        }
    }
}