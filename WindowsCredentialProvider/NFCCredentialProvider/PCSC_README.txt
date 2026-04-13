NFC PCSC实现说明
===================

概述
----
本实现使用Windows PCSC (Personal Computer/Smart Card) API替代原有的串口通信，提供更稳定、标准的NFC卡片读取功能。

主要特性
--------
- 使用Windows Smart Card API (winscard.h)
- 支持标准APDU命令
- 自动检测读卡器和卡片
- 支持多种NFC卡片类型
- 符合Windows登录安全要求

技术实现
--------
### 核心组件
- **NFCManager.h/NFCManager.cpp**: NFC管理器类，封装PCSC API调用
- **TestPCSC.cpp**: 功能测试程序
- **build_test.bat**: 编译脚本

### 关键API函数
- `SCardEstablishContext()`: 建立PCSC上下文
- `SCardListReaders()`: 获取读卡器列表
- `SCardConnect()`: 连接卡片
- `SCardTransmit()`: 发送APDU命令
- `SCardStatus()`: 获取卡片状态

### 标准APDU命令
- **获取UID**: `FF CA 00 00 00`
- **卡片状态检测**: 使用SCardStatus()函数

使用方法
--------
### 编译说明
- 需要Visual Studio 2019或更高版本
- 使用x64或x86架构编译
- 链接库: winscard.lib (Windows Smart Card API)
- C++17标准支持

### 测试步骤
1. 运行 `build_test.bat` 编译测试程序
2. 插入NFC读卡器
3. 运行 `TestPCSC.exe`
4. 将NFC卡片靠近读卡器

### 集成到Credential Provider
1. 确保Credential Provider项目链接winscard.lib
2. 包含NFCManager.h头文件
3. 创建NFCManager实例并调用相关方法

错误处理
--------
- 完善的错误代码映射
- 详细的错误信息描述
- 支持多语言错误提示

安全考虑
--------
- 使用标准PCSC API，符合Windows安全要求
- 支持卡片认证和加密
- 最小权限原则

兼容性
------
- Windows 10/11
- 支持所有符合PCSC标准的NFC读卡器
- 支持MIFARE Classic、MIFARE Ultralight、NTAG等常见卡片

提交到GitHub
-----------
代码已完成，可以提交到GitHub进行CI/CD测试。
建议的GitHub Actions工作流：
1. 编译测试程序
2. 运行基本功能测试
3. 验证PCSC API集成

注意事项
--------
- 确保目标系统已安装PCSC服务
- 读卡器驱动程序需要正确安装
- 某些卡片可能需要特定的APDU命令序列