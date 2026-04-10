namespace NFCLoginSystem.Forms
{
    partial class AddUserForm
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
            this.lblPassword = new System.Windows.Forms.Label();
            this.txtPassword = new System.Windows.Forms.TextBox();
            this.lblConfirmPassword = new System.Windows.Forms.Label();
            this.txtConfirmPassword = new System.Windows.Forms.TextBox();
            this.lblDisplayName = new System.Windows.Forms.Label();
            this.txtDisplayName = new System.Windows.Forms.TextBox();
            this.lblNFCCardId = new System.Windows.Forms.Label();
            this.txtNFCCardId = new System.Windows.Forms.TextBox();
            this.chkIsAdmin = new System.Windows.Forms.CheckBox();
            this.btnSave = new System.Windows.Forms.Button();
            this.btnCancel = new System.Windows.Forms.Button();
            this.btnGenerateNFCCard = new System.Windows.Forms.Button();
            this.lblPasswordStrength = new System.Windows.Forms.Label();
            this.groupBoxUserInfo = new System.Windows.Forms.GroupBox();
            this.groupBoxNFC = new System.Windows.Forms.GroupBox();
            this.groupBoxUserInfo.SuspendLayout();
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
            this.txtUsername.Size = new System.Drawing.Size(200, 26);
            this.txtUsername.TabIndex = 1;
            // 
            // lblPassword
            // 
            this.lblPassword.AutoSize = true;
            this.lblPassword.Font = new System.Drawing.Font("Microsoft Sans Serif", 10F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.lblPassword.Location = new System.Drawing.Point(40, 75);
            this.lblPassword.Name = "lblPassword";
            this.lblPassword.Size = new System.Drawing.Size(69, 20);
            this.lblPassword.TabIndex = 2;
            this.lblPassword.Text = "密码:";
            // 
            // txtPassword
            // 
            this.txtPassword.Font = new System.Drawing.Font("Microsoft Sans Serif", 10F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.txtPassword.Location = new System.Drawing.Point(120, 72);
            this.txtPassword.Name = "txtPassword";
            this.txtPassword.PasswordChar = '*';
            this.txtPassword.Size = new System.Drawing.Size(200, 26);
            this.txtPassword.TabIndex = 3;
            this.txtPassword.TextChanged += new System.EventHandler(this.txtPassword_TextChanged);
            // 
            // lblConfirmPassword
            // 
            this.lblConfirmPassword.AutoSize = true;
            this.lblConfirmPassword.Font = new System.Drawing.Font("Microsoft Sans Serif", 10F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.lblConfirmPassword.Location = new System.Drawing.Point(20, 115);
            this.lblConfirmPassword.Name = "lblConfirmPassword";
            this.lblConfirmPassword.Size = new System.Drawing.Size(89, 20);
            this.lblConfirmPassword.TabIndex = 4;
            this.lblConfirmPassword.Text = "确认密码:";
            // 
            // txtConfirmPassword
            // 
            this.txtConfirmPassword.Font = new System.Drawing.Font("Microsoft Sans Serif", 10F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.txtConfirmPassword.Location = new System.Drawing.Point(120, 112);
            this.txtConfirmPassword.Name = "txtConfirmPassword";
            this.txtConfirmPassword.PasswordChar = '*';
            this.txtConfirmPassword.Size = new System.Drawing.Size(200, 26);
            this.txtConfirmPassword.TabIndex = 5;
            // 
            // lblDisplayName
            // 
            this.lblDisplayName.AutoSize = true;
            this.lblDisplayName.Font = new System.Drawing.Font("Microsoft Sans Serif", 10F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.lblDisplayName.Location = new System.Drawing.Point(20, 155);
            this.lblDisplayName.Name = "lblDisplayName";
            this.lblDisplayName.Size = new System.Drawing.Size(89, 20);
            this.lblDisplayName.TabIndex = 6;
            this.lblDisplayName.Text = "显示名称:";
            // 
            // txtDisplayName
            // 
            this.txtDisplayName.Font = new System.Drawing.Font("Microsoft Sans Serif", 10F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.txtDisplayName.Location = new System.Drawing.Point(120, 152);
            this.txtDisplayName.Name = "txtDisplayName";
            this.txtDisplayName.Size = new System.Drawing.Size(200, 26);
            this.txtDisplayName.TabIndex = 7;
            // 
            // lblNFCCardId
            // 
            this.lblNFCCardId.AutoSize = true;
            this.lblNFCCardId.Font = new System.Drawing.Font("Microsoft Sans Serif", 10F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.lblNFCCardId.Location = new System.Drawing.Point(20, 35);
            this.lblNFCCardId.Name = "lblNFCCardId";
            this.lblNFCCardId.Size = new System.Drawing.Size(89, 20);
            this.lblNFCCardId.TabIndex = 8;
            this.lblNFCCardId.Text = "NFC卡ID:";
            // 
            // txtNFCCardId
            // 
            this.txtNFCCardId.Font = new System.Drawing.Font("Microsoft Sans Serif", 10F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.txtNFCCardId.Location = new System.Drawing.Point(120, 32);
            this.txtNFCCardId.Name = "txtNFCCardId";
            this.txtNFCCardId.Size = new System.Drawing.Size(200, 26);
            this.txtNFCCardId.TabIndex = 9;
            // 
            // chkIsAdmin
            // 
            this.chkIsAdmin.AutoSize = true;
            this.chkIsAdmin.Font = new System.Drawing.Font("Microsoft Sans Serif", 10F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.chkIsAdmin.Location = new System.Drawing.Point(120, 192);
            this.chkIsAdmin.Name = "chkIsAdmin";
            this.chkIsAdmin.Size = new System.Drawing.Size(126, 24);
            this.chkIsAdmin.TabIndex = 8;
            this.chkIsAdmin.Text = "管理员权限";
            this.chkIsAdmin.UseVisualStyleBackColor = true;
            // 
            // btnSave
            // 
            this.btnSave.Font = new System.Drawing.Font("Microsoft Sans Serif", 10F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.btnSave.Location = new System.Drawing.Point(120, 320);
            this.btnSave.Name = "btnSave";
            this.btnSave.Size = new System.Drawing.Size(100, 35);
            this.btnSave.TabIndex = 9;
            this.btnSave.Text = "保存";
            this.btnSave.UseVisualStyleBackColor = true;
            this.btnSave.Click += new System.EventHandler(this.btnSave_Click);
            // 
            // btnCancel
            // 
            this.btnCancel.Font = new System.Drawing.Font("Microsoft Sans Serif", 10F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.btnCancel.Location = new System.Drawing.Point(250, 320);
            this.btnCancel.Name = "btnCancel";
            this.btnCancel.Size = new System.Drawing.Size(100, 35);
            this.btnCancel.TabIndex = 10;
            this.btnCancel.Text = "取消";
            this.btnCancel.UseVisualStyleBackColor = true;
            this.btnCancel.Click += new System.EventHandler(this.btnCancel_Click);
            // 
            // btnGenerateNFCCard
            // 
            this.btnGenerateNFCCard.Font = new System.Drawing.Font("Microsoft Sans Serif", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.btnGenerateNFCCard.Location = new System.Drawing.Point(340, 30);
            this.btnGenerateNFCCard.Name = "btnGenerateNFCCard";
            this.btnGenerateNFCCard.Size = new System.Drawing.Size(100, 30);
            this.btnGenerateNFCCard.TabIndex = 10;
            this.btnGenerateNFCCard.Text = "生成卡ID";
            this.btnGenerateNFCCard.UseVisualStyleBackColor = true;
            this.btnGenerateNFCCard.Click += new System.EventHandler(this.btnGenerateNFCCard_Click);
            // 
            // lblPasswordStrength
            // 
            this.lblPasswordStrength.AutoSize = true;
            this.lblPasswordStrength.Font = new System.Drawing.Font("Microsoft Sans Serif", 9F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.lblPasswordStrength.Location = new System.Drawing.Point(340, 75);
            this.lblPasswordStrength.Name = "lblPasswordStrength";
            this.lblPasswordStrength.Size = new System.Drawing.Size(0, 18);
            this.lblPasswordStrength.TabIndex = 11;
            // 
            // groupBoxUserInfo
            // 
            this.groupBoxUserInfo.Controls.Add(this.lblUsername);
            this.groupBoxUserInfo.Controls.Add(this.chkIsAdmin);
            this.groupBoxUserInfo.Controls.Add(this.txtUsername);
            this.groupBoxUserInfo.Controls.Add(this.lblPassword);
            this.groupBoxUserInfo.Controls.Add(this.txtPassword);
            this.groupBoxUserInfo.Controls.Add(this.lblConfirmPassword);
            this.groupBoxUserInfo.Controls.Add(this.txtConfirmPassword);
            this.groupBoxUserInfo.Controls.Add(this.lblDisplayName);
            this.groupBoxUserInfo.Controls.Add(this.txtDisplayName);
            this.groupBoxUserInfo.Controls.Add(this.lblPasswordStrength);
            this.groupBoxUserInfo.Font = new System.Drawing.Font("Microsoft Sans Serif", 10F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.groupBoxUserInfo.Location = new System.Drawing.Point(20, 20);
            this.groupBoxUserInfo.Name = "groupBoxUserInfo";
            this.groupBoxUserInfo.Size = new System.Drawing.Size(460, 240);
            this.groupBoxUserInfo.TabIndex = 12;
            this.groupBoxUserInfo.TabStop = false;
            this.groupBoxUserInfo.Text = "用户信息";
            // 
            // groupBoxNFC
            // 
            this.groupBoxNFC.Controls.Add(this.lblNFCCardId);
            this.groupBoxNFC.Controls.Add(this.txtNFCCardId);
            this.groupBoxNFC.Controls.Add(this.btnGenerateNFCCard);
            this.groupBoxNFC.Font = new System.Drawing.Font("Microsoft Sans Serif", 10F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.groupBoxNFC.Location = new System.Drawing.Point(20, 270);
            this.groupBoxNFC.Name = "groupBoxNFC";
            this.groupBoxNFC.Size = new System.Drawing.Size(460, 80);
            this.groupBoxNFC.TabIndex = 13;
            this.groupBoxNFC.TabStop = false;
            this.groupBoxNFC.Text = "NFC卡绑定";
            // 
            // AddUserForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(8F, 16F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(500, 400);
            this.Controls.Add(this.groupBoxNFC);
            this.Controls.Add(this.groupBoxUserInfo);
            this.Controls.Add(this.btnCancel);
            this.Controls.Add(this.btnSave);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle;
            this.MaximizeBox = false;
            this.Name = "AddUserForm";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
            this.Text = "添加用户";
            this.groupBoxUserInfo.ResumeLayout(false);
            this.groupBoxUserInfo.PerformLayout();
            this.groupBoxNFC.ResumeLayout(false);
            this.groupBoxNFC.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();
        }

        #endregion

        private System.Windows.Forms.Label lblUsername;
        private System.Windows.Forms.TextBox txtUsername;
        private System.Windows.Forms.Label lblPassword;
        private System.Windows.Forms.TextBox txtPassword;
        private System.Windows.Forms.Label lblConfirmPassword;
        private System.Windows.Forms.TextBox txtConfirmPassword;
        private System.Windows.Forms.Label lblDisplayName;
        private System.Windows.Forms.TextBox txtDisplayName;
        private System.Windows.Forms.Label lblNFCCardId;
        private System.Windows.Forms.TextBox txtNFCCardId;
        private System.Windows.Forms.CheckBox chkIsAdmin;
        private System.Windows.Forms.Button btnSave;
        private System.Windows.Forms.Button btnCancel;
        private System.Windows.Forms.Button btnGenerateNFCCard;
        private System.Windows.Forms.Label lblPasswordStrength;
        private System.Windows.Forms.GroupBox groupBoxUserInfo;
        private System.Windows.Forms.GroupBox groupBoxNFC;
    }
}