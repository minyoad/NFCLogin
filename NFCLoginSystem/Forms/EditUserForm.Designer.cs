namespace NFCLoginSystem.Forms
{
    partial class EditUserForm
    {
        private System.ComponentModel.IContainer components = null;

        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows 窗体设计器生成的代码

        private void InitializeComponent()
        {
            this.lblUsername = new System.Windows.Forms.Label();
            this.txtUsername = new System.Windows.Forms.TextBox();
            this.lblDisplayName = new System.Windows.Forms.Label();
            this.txtDisplayName = new System.Windows.Forms.TextBox();
            this.lblNFCCardId = new System.Windows.Forms.Label();
            this.txtNFCCardId = new System.Windows.Forms.TextBox();
            this.chkIsActive = new System.Windows.Forms.CheckBox();
            this.chkIsAdmin = new System.Windows.Forms.CheckBox();
            this.btnSave = new System.Windows.Forms.Button();
            this.btnCancel = new System.Windows.Forms.Button();
            this.btnClearNFCCard = new System.Windows.Forms.Button();
            this.btnGenerateNFCCard = new System.Windows.Forms.Button();
            this.lblCreatedAt = new System.Windows.Forms.Label();
            this.lblLastLogin = new System.Windows.Forms.Label();
            this.groupBoxUserInfo = new System.Windows.Forms.GroupBox();
            this.groupBoxPassword = new System.Windows.Forms.GroupBox();
            this.lblNewPassword = new System.Windows.Forms.Label();
            this.txtNewPassword = new System.Windows.Forms.TextBox();
            this.lblConfirmPassword = new System.Windows.Forms.Label();
            this.txtConfirmPassword = new System.Windows.Forms.TextBox();
            this.lblPasswordStrength = new System.Windows.Forms.Label();
            this.groupBoxNFC = new System.Windows.Forms.GroupBox();
            this.groupBoxUserInfo.SuspendLayout();
            this.groupBoxPassword.SuspendLayout();
            this.groupBoxNFC.SuspendLayout();
            this.SuspendLayout();
            // 
            // lblUsername
            // 
            this.lblUsername.AutoSize = true;
            this.lblUsername.Font = new System.Drawing.Font("Microsoft Sans Serif", 10F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.lblUsername.Location = new System.Drawing.Point(20, 35);
            this.lblUsername.Name = "lblUsername";
            this.lblUsername.Size = new System.Drawing.Size(89, 20);
            this.lblUsername.TabIndex = 0;
            this.lblUsername.Text = "用户名:";
            // 
            // txtUsername
            // 
            this.txtUsername.Font = new System.Drawing.Font("Microsoft Sans Serif", 10F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.txtUsername.Location = new System.Drawing.Point(120, 32);
            this.txtUsername.Name = "txtUsername";
            this.txtUsername.ReadOnly = true;
            this.txtUsername.Size = new System.Drawing.Size(200, 26);
            this.txtUsername.TabIndex = 1;
            // 
            // lblDisplayName
            // 
            this.lblDisplayName.AutoSize = true;
            this.lblDisplayName.Font = new System.Drawing.Font("Microsoft Sans Serif", 10F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.lblDisplayName.Location = new System.Drawing.Point(20, 75);
            this.lblDisplayName.Name = "lblDisplayName";
            this.lblDisplayName.Size = new System.Drawing.Size(89, 20);
            this.lblDisplayName.TabIndex = 2;
            this.lblDisplayName.Text = "显示名称:";
            // 
            // txtDisplayName
            // 
            this.txtDisplayName.Font = new System.Drawing.Font("Microsoft Sans Serif", 10F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.txtDisplayName.Location = new System.Drawing.Point(120, 72);
            this.txtDisplayName.Name = "txtDisplayName";
            this.txtDisplayName.Size = new System.Drawing.Size(200, 26);
            this.txtDisplayName.TabIndex = 3;
            // 
            // lblNFCCardId
            // 
            this.lblNFCCardId.AutoSize = true;
            this.lblNFCCardId.Font = new System.Drawing.Font("Microsoft Sans Serif", 10F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.lblNFCCardId.Location = new System.Drawing.Point(20, 35);
            this.lblNFCCardId.Name = "lblNFCCardId";
            this.lblNFCCardId.Size = new System.Drawing.Size(89, 20);
            this.lblNFCCardId.TabIndex = 4;
            this.lblNFCCardId.Text = "NFC卡ID:";
            // 
            // txtNFCCardId
            // 
            this.txtNFCCardId.Font = new System.Drawing.Font("Microsoft Sans Serif", 10F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.txtNFCCardId.Location = new System.Drawing.Point(120, 32);
            this.txtNFCCardId.Name = "txtNFCCardId";
            this.txtNFCCardId.Size = new System.Drawing.Size(200, 26);
            this.txtNFCCardId.TabIndex = 5;
            // 
            // chkIsActive
            // 
            this.chkIsActive.AutoSize = true;
            this.chkIsActive.Font = new System.Drawing.Font("Microsoft Sans Serif", 10F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.chkIsActive.Location = new System.Drawing.Point(120, 115);
            this.chkIsActive.Name = "chkIsActive";
            this.chkIsActive.Size = new System.Drawing.Size(86, 24);
            this.chkIsActive.TabIndex = 6;
            this.chkIsActive.Text = "激活状态";
            this.chkIsActive.UseVisualStyleBackColor = true;
            // 
            // chkIsAdmin
            // 
            this.chkIsAdmin.AutoSize = true;
            this.chkIsAdmin.Font = new System.Drawing.Font("Microsoft Sans Serif", 10F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.chkIsAdmin.Location = new System.Drawing.Point(120, 145);
            this.chkIsAdmin.Name = "chkIsAdmin";
            this.chkIsAdmin.Size = new System.Drawing.Size(126, 24);
            this.chkIsAdmin.TabIndex = 7;
            this.chkIsAdmin.Text = "管理员权限";
            this.chkIsAdmin.UseVisualStyleBackColor = true;
            // 
            // btnSave
            // 
            this.btnSave.Font = new System.Drawing.Font("Microsoft Sans Serif", 10F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.btnSave.Location = new System.Drawing.Point(120, 480);
            this.btnSave.Name = "btnSave";
            this.btnSave.Size = new System.Drawing.Size(100, 35);
            this.btnSave.TabIndex = 8;
            this.btnSave.Text = "保存";
            this.btnSave.UseVisualStyleBackColor = true;
            this.btnSave.Click += new System.EventHandler(this.btnSave_Click);
            // 
            // btnCancel
            // 
            this.btnCancel.Font = new System.Drawing.Font("Microsoft Sans Serif", 10F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.btnCancel.Location = new System.Drawing.Point(250, 480);
            this.btnCancel.Name = "btnCancel";
            this.btnCancel.Size = new System.Drawing.Size(100, 35);
            this.btnCancel.TabIndex = 9;
            this.btnCancel.Text = "取消";
            this.btnCancel.UseVisualStyleBackColor = true;
            this.btnCancel.Click += new System.EventHandler(this.btnCancel_Click);
            // 
            // btnClearNFCCard
            // 
            this.btnClearNFCCard.Font = new System.Drawing.Font("Microsoft Sans Serif", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.btnClearNFCCard.Location = new System.Drawing.Point(340, 70);
            this.btnClearNFCCard.Name = "btnClearNFCCard";
            this.btnClearNFCCard.Size = new System.Drawing.Size(100, 30);
            this.btnClearNFCCard.TabIndex = 6;
            this.btnClearNFCCard.Text = "清除卡ID";
            this.btnClearNFCCard.UseVisualStyleBackColor = true;
            this.btnClearNFCCard.Click += new System.EventHandler(this.btnClearNFCCard_Click);
            // 
            // btnGenerateNFCCard
            // 
            this.btnGenerateNFCCard.Font = new System.Drawing.Font("Microsoft Sans Serif", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.btnGenerateNFCCard.Location = new System.Drawing.Point(340, 30);
            this.btnGenerateNFCCard.Name = "btnGenerateNFCCard";
            this.btnGenerateNFCCard.Size = new System.Drawing.Size(100, 30);
            this.btnGenerateNFCCard.TabIndex = 7;
            this.btnGenerateNFCCard.Text = "生成卡ID";
            this.btnGenerateNFCCard.UseVisualStyleBackColor = true;
            this.btnGenerateNFCCard.Click += new System.EventHandler(this.btnGenerateNFCCard_Click);
            // 
            // lblCreatedAt
            // 
            this.lblCreatedAt.AutoSize = true;
            this.lblCreatedAt.Font = new System.Drawing.Font("Microsoft Sans Serif", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.lblCreatedAt.Location = new System.Drawing.Point(20, 185);
            this.lblCreatedAt.Name = "lblCreatedAt";
            this.lblCreatedAt.Size = new System.Drawing.Size(144, 18);
            this.lblCreatedAt.TabIndex = 10;
            this.lblCreatedAt.Text = "创建时间: 2024-01-01";
            // 
            // lblLastLogin
            // 
            this.lblLastLogin.AutoSize = true;
            this.lblLastLogin.Font = new System.Drawing.Font("Microsoft Sans Serif", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.lblLastLogin.Location = new System.Drawing.Point(20, 210);
            this.lblLastLogin.Name = "lblLastLogin";
            this.lblLastLogin.Size = new System.Drawing.Size(144, 18);
            this.lblLastLogin.TabIndex = 11;
            this.lblLastLogin.Text = "最后登录: 从未登录";
            // 
            // groupBoxUserInfo
            // 
            this.groupBoxUserInfo.Controls.Add(this.lblUsername);
            this.groupBoxUserInfo.Controls.Add(this.txtUsername);
            this.groupBoxUserInfo.Controls.Add(this.lblDisplayName);
            this.groupBoxUserInfo.Controls.Add(this.txtDisplayName);
            this.groupBoxUserInfo.Controls.Add(this.chkIsActive);
            this.groupBoxUserInfo.Controls.Add(this.chkIsAdmin);
            this.groupBoxUserInfo.Controls.Add(this.lblCreatedAt);
            this.groupBoxUserInfo.Controls.Add(this.lblLastLogin);
            this.groupBoxUserInfo.Font = new System.Drawing.Font("Microsoft Sans Serif", 10F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.groupBoxUserInfo.Location = new System.Drawing.Point(20, 20);
            this.groupBoxUserInfo.Name = "groupBoxUserInfo";
            this.groupBoxUserInfo.Size = new System.Drawing.Size(460, 250);
            this.groupBoxUserInfo.TabIndex = 12;
            this.groupBoxUserInfo.TabStop = false;
            this.groupBoxUserInfo.Text = "用户信息";
            // 
            // groupBoxPassword
            // 
            this.groupBoxPassword.Controls.Add(this.lblNewPassword);
            this.groupBoxPassword.Controls.Add(this.txtNewPassword);
            this.groupBoxPassword.Controls.Add(this.lblConfirmPassword);
            this.groupBoxPassword.Controls.Add(this.txtConfirmPassword);
            this.groupBoxPassword.Controls.Add(this.lblPasswordStrength);
            this.groupBoxPassword.Font = new System.Drawing.Font("Microsoft Sans Serif", 10F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.groupBoxPassword.Location = new System.Drawing.Point(20, 280);
            this.groupBoxPassword.Name = "groupBoxPassword";
            this.groupBoxPassword.Size = new System.Drawing.Size(460, 180);
            this.groupBoxPassword.TabIndex = 13;
            this.groupBoxPassword.TabStop = false;
            this.groupBoxPassword.Text = "修改密码（可选）";
            // 
            // lblNewPassword
            // 
            this.lblNewPassword.AutoSize = true;
            this.lblNewPassword.Font = new System.Drawing.Font("Microsoft Sans Serif", 10F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.lblNewPassword.Location = new System.Drawing.Point(20, 35);
            this.lblNewPassword.Name = "lblNewPassword";
            this.lblNewPassword.Size = new System.Drawing.Size(69, 20);
            this.lblNewPassword.TabIndex = 0;
            this.lblNewPassword.Text = "新密码:";
            // 
            // txtNewPassword
            // 
            this.txtNewPassword.Font = new System.Drawing.Font("Microsoft Sans Serif", 10F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.txtNewPassword.Location = new System.Drawing.Point(120, 32);
            this.txtNewPassword.Name = "txtNewPassword";
            this.txtNewPassword.PasswordChar = '*';
            this.txtNewPassword.Size = new System.Drawing.Size(200, 26);
            this.txtNewPassword.TabIndex = 1;
            this.txtNewPassword.TextChanged += new System.EventHandler(this.txtNewPassword_TextChanged);
            // 
            // lblConfirmPassword
            // 
            this.lblConfirmPassword.AutoSize = true;
            this.lblConfirmPassword.Font = new System.Drawing.Font("Microsoft Sans Serif", 10F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.lblConfirmPassword.Location = new System.Drawing.Point(20, 75);
            this.lblConfirmPassword.Name = "lblConfirmPassword";
            this.lblConfirmPassword.Size = new System.Drawing.Size(89, 20);
            this.lblConfirmPassword.TabIndex = 2;
            this.lblConfirmPassword.Text = "确认密码:";
            // 
            // txtConfirmPassword
            // 
            this.txtConfirmPassword.Font = new System.Drawing.Font("Microsoft Sans Serif", 10F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.txtConfirmPassword.Location = new System.Drawing.Point(120, 72);
            this.txtConfirmPassword.Name = "txtConfirmPassword";
            this.txtConfirmPassword.PasswordChar = '*';
            this.txtConfirmPassword.Size = new System.Drawing.Size(200, 26);
            this.txtConfirmPassword.TabIndex = 3;
            // 
            // lblPasswordStrength
            // 
            this.lblPasswordStrength.AutoSize = true;
            this.lblPasswordStrength.Font = new System.Drawing.Font("Microsoft Sans Serif", 9F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.lblPasswordStrength.Location = new System.Drawing.Point(340, 35);
            this.lblPasswordStrength.Name = "lblPasswordStrength";
            this.lblPasswordStrength.Size = new System.Drawing.Size(0, 18);
            this.lblPasswordStrength.TabIndex = 4;
            // 
            // groupBoxNFC
            // 
            this.groupBoxNFC.Controls.Add(this.lblNFCCardId);
            this.groupBoxNFC.Controls.Add(this.txtNFCCardId);
            this.groupBoxNFC.Controls.Add(this.btnClearNFCCard);
            this.groupBoxNFC.Controls.Add(this.btnGenerateNFCCard);
            this.groupBoxNFC.Font = new System.Drawing.Font("Microsoft Sans Serif", 10F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.groupBoxNFC.Location = new System.Drawing.Point(20, 470);
            this.groupBoxNFC.Name = "groupBoxNFC";
            this.groupBoxNFC.Size = new System.Drawing.Size(460, 120);
            this.groupBoxNFC.TabIndex = 14;
            this.groupBoxNFC.TabStop = false;
            this.groupBoxNFC.Text = "NFC卡绑定";
            // 
            // EditUserForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(8F, 16F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(500, 620);
            this.Controls.Add(this.groupBoxNFC);
            this.Controls.Add(this.groupBoxPassword);
            this.Controls.Add(this.groupBoxUserInfo);
            this.Controls.Add(this.btnCancel);
            this.Controls.Add(this.btnSave);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle;
            this.MaximizeBox = false;
            this.Name = "EditUserForm";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
            this.Text = "编辑用户";
            this.groupBoxUserInfo.ResumeLayout(false);
            this.groupBoxUserInfo.PerformLayout();
            this.groupBoxPassword.ResumeLayout(false);
            this.groupBoxPassword.PerformLayout();
            this.groupBoxNFC.ResumeLayout(false);
            this.groupBoxNFC.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();
        }

        #endregion

        private System.Windows.Forms.Label lblUsername;
        private System.Windows.Forms.TextBox txtUsername;
        private System.Windows.Forms.Label lblDisplayName;
        private System.Windows.Forms.TextBox txtDisplayName;
        private System.Windows.Forms.Label lblNFCCardId;
        private System.Windows.Forms.TextBox txtNFCCardId;
        private System.Windows.Forms.CheckBox chkIsActive;
        private System.Windows.Forms.CheckBox chkIsAdmin;
        private System.Windows.Forms.Button btnSave;
        private System.Windows.Forms.Button btnCancel;
        private System.Windows.Forms.Button btnClearNFCCard;
        private System.Windows.Forms.Button btnGenerateNFCCard;
        private System.Windows.Forms.Label lblCreatedAt;
        private System.Windows.Forms.Label lblLastLogin;
        private System.Windows.Forms.GroupBox groupBoxUserInfo;
        private System.Windows.Forms.GroupBox groupBoxPassword;
        private System.Windows.Forms.Label lblNewPassword;
        private System.Windows.Forms.TextBox txtNewPassword;
        private System.Windows.Forms.Label lblConfirmPassword;
        private System.Windows.Forms.TextBox txtConfirmPassword;
        private System.Windows.Forms.Label lblPasswordStrength;
        private System.Windows.Forms.GroupBox groupBoxNFC;
    }
}