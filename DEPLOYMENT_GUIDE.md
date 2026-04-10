# 🚀 NFC登录系统部署指南

## 📋 项目概述

这是一个完整的Windows NFC登录系统，包含：
- ✅ WinForms用户管理界面
- ✅ Windows系统级凭据提供程序
- ✅ GitHub Actions自动编译
- ✅ MIFARE Classic (M1) NFC卡片支持
- ✅ 本地用户账户管理

## 🎯 快速部署步骤

### 步骤1：提交到GitHub

1. 运行提交脚本：
   ```batch
   commit_to_github.bat
   ```

2. 按照提示完成GitHub仓库创建和推送：
   ```bash
   git remote add origin https://github.com/你的用户名/你的仓库名.git
   git push -u origin main
   ```

### 步骤2：GitHub Actions自动编译

1. 推送代码后，访问你的GitHub仓库
2. 点击"Actions"标签页
3. 等待构建完成（大约2-3分钟）
4. 下载构建产物：
   - `nfc-login-winforms-artifact` - WinForms应用程序
   - `nfc-credential-provider-artifact` - 凭据提供程序DLL

### 步骤3：本地安装和测试

1. **构建项目**（如果本地构建）：
   ```batch
   cd WindowsCredentialProvider
   build_all.bat
   ```

2. **安装凭据提供程序**（需要管理员权限）：
   ```batch
   install.bat
   ```

3. **重启计算机**

4. **测试NFC功能**：
   - 在Windows登录界面应该能看到NFC登录选项
   - 使用MIFARE Classic卡片测试登录

## 🔧 本地测试工具

使用测试工具验证功能：
```batch
TestNFCLogin.exe
```

## 🛡️ 安全提醒

⚠️ **重要警告**：
- 这是一个系统级组件，会修改Windows登录行为
- 安装前请确保系统已备份
- 建议在测试环境中先验证功能
- 不要在生产环境中直接使用未经充分测试的版本

## 📁 项目文件结构

```
NFCLogin/
├── NFCLoginSystem/                    # WinForms应用程序
│   ├── Models/
│   ├── Services/
│   ├── Forms/
│   └── app.manifest
├── WindowsCredentialProvider/         # 凭据提供程序
│   ├── NFCCredentialProvider/
│   ├── build_all.bat
│   ├── install.bat
│   ├── uninstall.bat
│   └── TestNFCLogin.cpp
├── .github/workflows/                  # GitHub Actions
│   └── build.yml
├── commit_to_github.bat               # Git提交助手
├── run_as_admin.bat                   # 管理员权限运行
└── 文档和脚本文件
```

## 🔄 卸载说明

如果需要移除凭据提供程序：
```batch
cd WindowsCredentialProvider
uninstall.bat
```

然后重启计算机。

## 📞 技术支持

如果遇到问题：
1. 检查事件查看器中的系统日志
2. 确认NFC读卡器驱动已正确安装
3. 验证MIFARE Classic卡片是否正常工作
4. 查看GitHub Actions构建日志

## 🎉 恭喜！

您的NFC登录系统已经准备就绪！按照以上步骤，您可以：
- ✅ 提交代码到GitHub
- ✅ 自动编译WinForms和凭据提供程序
- ✅ 在本地安装和测试NFC登录功能

开始享受无密码的NFC登录体验吧！