@echo off
chcp 65001 >nul 2>&1
echo NFC登录系统 - 构建测试
echo =========================
echo.

REM 检查.NET SDK
echo 检查.NET SDK...
dotnet --version
if %errorlevel% neq 0 (
    echo 错误: 未找到.NET SDK
    echo 请访问 https://dotnet.microsoft.com/download/dotnet/8.0 下载并安装.NET 8.0 SDK
    pause
    exit /b 1
)

echo.
echo 正在构建项目...
dotnet restore NFCLoginSystem.sln
if %errorlevel% neq 0 (
    echo 错误: 项目还原失败
    pause
    exit /b 1
)

dotnet build NFCLoginSystem.sln --configuration Release
if %errorlevel% neq 0 (
    echo 错误: 项目构建失败
    pause
    exit /b 1
)

echo.
echo 构建成功！
echo 可执行文件位置: NFCLoginSystem\bin\Release\net8.0-windows\win-x64\NFCLoginSystem.exe
echo.
pause