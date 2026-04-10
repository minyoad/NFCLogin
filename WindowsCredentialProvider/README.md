# NFC Credential Provider for Windows

这是一个Windows凭据提供程序，支持通过NFC卡片进行系统级登录。

## 功能特性

- 🔐 系统级Windows登录集成
- 📱 NFC卡片识别和绑定
- 👥 多用户支持
- 🔑 本地用户账户管理
- 🎯 MIFARE Classic (M1) 卡片支持
- 🔒 安全的UID绑定机制
- 📝 完整的日志记录

## 系统要求

- Windows 10/11 (64位)
- Visual Studio 2022 (或更高版本)
- .NET Framework 4.8 或更高版本
- 管理员权限
- NFC读卡器（支持串口通信）
- MIFARE Classic 卡片

## 项目结构

```
WindowsCredentialProvider/
├── NFCCredentialProvider/          # 主凭据提供程序项目
│   ├── NFCCredentialProvider.cpp   # 主实现文件
│   ├── NFCCredentialProvider.h     # 头文件
│   ├── NFCCredentialProviderCredential.cpp  # 凭据处理
│   ├── NFCCredentialProviderCredential.h    # 凭据头文件
│   ├── AccountManager.cpp          # 账户管理器
│   ├── AccountManager.h            # 账户管理器头文件
│   ├── NFCManager.cpp              # NFC管理器
│   ├── NFCManager.h                # NFC管理器头文件
│   └── NFCCredentialProvider.sln   # Visual Studio解决方案
├── TestNFCLogin.cpp                # 测试程序
├── build_all.bat                   # 完整构建脚本
├── build_test.bat                  # 测试程序构建脚本
├── install.bat                     # 安装脚本
└── uninstall.bat                   # 卸载脚本
```

## 构建步骤

### 1. 环境准备

确保已安装：
- Visual Studio 2022 Community 或更高版本
- Windows SDK
- C++开发工具

### 2. 构建项目

以管理员身份运行：

```batch
build_all.bat
```

这将构建：
- 主凭据提供程序DLL
- 测试程序

### 3. 安装

以管理员身份运行：

```batch
install.bat
```

安装程序将：
- 注册DLL文件
- 添加注册表项
- 创建必要的目录
- 设置权限
- 重启相关服务

### 4. 测试

运行测试程序验证NFC功能：

```batch
TestNFCLogin.exe
```

## 使用方法

### 首次使用

1. 重启计算机
2. 在Windows登录界面，系统会自动检测NFC卡片
3. 放置已绑定的NFC卡片到读卡器上
4. 系统将自动登录对应用户

### 用户绑定

使用测试程序创建新用户并绑定NFC卡片：

1. 运行 `TestNFCLogin.exe`
2. 放置NFC卡片
3. 选择创建新用户
4. 输入用户名、密码和全名
5. 系统将自动创建用户并绑定卡片

### 卸载

以管理员身份运行：

```batch
uninstall.bat
```

## 配置

### NFC读卡器配置

默认使用串口COM3，可以在NFCManager.cpp中修改：

```cpp
const std::wstring NFCManager::DEFAULT_COM_PORT = L"COM3";
const DWORD NFCManager::DEFAULT_BAUD_RATE = 9600;
```

### 数据存储

用户绑定数据存储在：`C:\NFCLogin\data\`
日志文件存储在：`C:\NFCLogin\logs\`

### 注册表项

凭据提供程序注册在：
`HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows\CurrentVersion\Authentication\Credential Providers\{7A8A8F2E-4C3D-4F1B-9E2A-3C4D5F6A7B8C}`

## 故障排除

### 常见问题

1. **安装失败**
   - 确保以管理员身份运行安装脚本
   - 检查Visual Studio是否正确安装
   - 查看install.log日志文件

2. **NFC读卡器无法识别**
   - 检查串口连接
   - 确认COM端口设置正确
   - 验证读卡器驱动程序

3. **登录失败**
   - 检查用户是否正确绑定
   - 查看日志文件了解详细错误
   - 确保NFC卡片已正确绑定

4. **系统不稳定**
   - 立即卸载凭据提供程序
   - 重启计算机
   - 联系技术支持

### 日志文件

- 安装日志：`install.log`
- 卸载日志：`uninstall.log`
- 运行日志：`C:\NFCLogin\logs\credential_provider.log`

## 安全注意事项

⚠️ **重要安全提示：**

1. 这是一个系统级组件，安装前请确保了解其功能
2. 始终在测试环境中先进行验证
3. 保持系统备份
4. 不要在生产环境中直接使用
5. 定期更新和维护

## 开发说明

### 核心组件

- **NFCCredentialProvider**: 主凭据提供程序实现
- **AccountManager**: 处理用户账户和NFC绑定
- **NFCManager**: 管理NFC读卡器通信

### 扩展开发

如需添加新的卡片类型：
1. 在NFCManager.h中添加卡片类型
2. 实现对应的读取逻辑
3. 更新用户界面

### 调试

使用DebugView或类似工具查看调试输出：
- 所有重要操作都有日志记录
- 使用 `LogMessage()` 函数添加自定义日志

## 许可证

本项目仅供学习和研究使用。在商业环境中使用前，请确保遵守相关法律法规。

## 技术支持

如有问题，请：
1. 查看日志文件
2. 检查常见问题解答
3. 在测试环境中重现问题
4. 提供详细的错误信息

---

**注意**: 这是一个实验性项目，请在充分了解风险后使用。