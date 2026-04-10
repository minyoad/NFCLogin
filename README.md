# NFC登录系统

一个基于Windows Forms的登录系统，支持多用户管理、NFC卡登录和GitHub Actions自动编译。

## 功能特性

- ✅ **多用户支持** - 支持创建和管理多个用户账户
- ✅ **NFC卡登录** - 支持刷NFC卡直接登录系统
- ✅ **密码登录** - 传统用户名密码登录方式
- ✅ **用户管理** - 管理员可以添加、编辑、删除用户
- ✅ **安全特性** - 密码哈希存储、账户锁定机制
- ✅ **GitHub Actions** - 自动化构建和发布

## 系统要求

- Windows 10/11
- .NET 8.0 SDK
- NFC读卡器（支持串口通信）
- Visual Studio 2022 或更高版本（可选）

## 快速开始

### 1. 安装.NET 8.0 SDK

访问 [.NET 8.0 下载页面](https://dotnet.microsoft.com/download/dotnet/8.0) 并安装SDK。

### 2. 克隆项目

```bash
git clone https://github.com/your-username/NFCLogin.git
cd NFCLogin
```

### 3. 构建项目

```bash
dotnet restore NFCLoginSystem.sln
dotnet build NFCLoginSystem.sln --configuration Release
```

### 4. 运行应用

```bash
dotnet run --project NFCLoginSystem\NFCLoginSystem.csproj
```

## 项目结构

```
NFCLogin/
├── NFCLoginSystem/
│   ├── Forms/                    # Windows Forms界面
│   │   ├── LoginForm.cs        # 登录界面
│   │   ├── UserManagementForm.cs # 用户管理界面
│   │   ├── AddUserForm.cs      # 添加用户界面
│   │   └── EditUserForm.cs     # 编辑用户界面
│   ├── Models/                   # 数据模型
│   │   ├── User.cs             # 用户模型
│   │   └── LoginSession.cs     # 登录会话模型
│   ├── Services/                 # 业务逻辑服务
│   │   ├── AuthenticationService.cs # 认证服务
│   │   ├── DatabaseService.cs  # 数据库服务
│   │   └── NFCSerialService.cs # NFC串口服务
│   └── Program.cs               # 程序入口点
├── .github/workflows/            # GitHub Actions工作流
│   └── build.yml               # 构建配置
└── test_build.bat              # 构建测试脚本
```

## 功能详情

### 用户管理

- **添加用户**: 支持设置用户名、显示名称、密码、NFC卡绑定
- **编辑用户**: 修改用户信息、重置密码、NFC卡管理
- **删除用户**: 管理员可以删除非当前登录用户
- **用户搜索**: 支持按用户名、显示名称、NFC卡ID搜索

### 安全特性

- **密码哈希**: 使用SHA-256算法存储密码哈希
- **账户锁定**: 连续5次失败登录后锁定账户15分钟
- **管理员权限**: 区分普通用户和管理员权限

### NFC功能

- **串口通信**: 通过串口与NFC读卡器通信
- **自动检测**: 实时检测NFC卡片
- **快速登录**: 刷卡即可快速登录系统

## 数据库

使用SQLite本地数据库存储用户信息和登录会话，数据库文件位于应用程序目录。

### 用户表结构

- `Id`: 用户ID（主键）
- `Username`: 用户名（唯一）
- `PasswordHash`: 密码哈希
- `DisplayName`: 显示名称
- `NFCCardId`: NFC卡ID
- `IsActive`: 激活状态
- `IsAdmin`: 管理员标志
- `FailedLoginAttempts`: 失败登录次数
- `LastFailedLogin`: 最后失败登录时间
- `CreatedAt`: 创建时间
- `LastLogin`: 最后登录时间

## GitHub Actions

项目配置了GitHub Actions工作流，支持自动构建和发布：

- **自动构建**: 每次推送代码时自动构建项目
- **多平台支持**: 构建Windows x64版本
- **发布工件**: 自动上传构建产物

## 开发指南

### 添加新功能

1. 创建新的Form类（如果需要新界面）
2. 在Services层添加相应的业务逻辑
3. 更新数据库模型（如果需要）
4. 在主界面中添加相应的菜单或按钮

### 扩展NFC支持

目前系统使用串口通信与NFC读卡器交互。要支持其他类型的读卡器：

1. 创建新的NFC服务类实现INFCSerialService接口
2. 在配置中选择合适的NFC服务实现
3. 更新相关的UI界面

### 数据库迁移

如果需要修改数据库结构：

1. 修改DatabaseService.cs中的表创建SQL
2. 添加数据库版本管理逻辑
3. 实现数据迁移脚本

## 故障排除

### 构建失败

1. 确保已安装.NET 8.0 SDK
2. 检查项目文件中的依赖包版本
3. 运行 `dotnet restore` 还原依赖

### NFC读卡器无法识别

1. 检查串口连接是否正确
2. 确认串口号和波特率设置
3. 检查NFC读卡器驱动是否安装

### 登录失败

1. 检查用户名和密码是否正确
2. 确认账户未被锁定
3. 检查数据库连接是否正常

## 贡献指南

欢迎提交Issue和Pull Request来改进这个项目。

### 提交Issue

- 描述清楚遇到的问题
- 提供复现步骤
- 包含系统环境信息

### 提交Pull Request

1. Fork项目到个人仓库
2. 创建功能分支
3. 提交代码变更
4. 创建Pull Request

## 许可证

本项目采用MIT许可证，详见LICENSE文件。

## 联系方式

如有问题或建议，请通过以下方式联系：

- 提交GitHub Issue
- 发送邮件至项目维护者

---

**注意**: 这是一个演示项目，在生产环境中使用时需要进一步加强安全措施，如使用更强的加密算法、实现更完善的权限管理等。