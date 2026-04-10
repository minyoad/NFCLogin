@echo off
echo 正在以管理员权限运行NFC登录系统...
echo ========================================

REM 检查是否已经以管理员权限运行
net session >nul 2>&1
if %errorlevel% neq 0 (
    echo 需要管理员权限，正在请求提升...
    
    REM 创建VBS脚本来请求管理员权限
    echo Set UAC = CreateObject^("Shell.Application"^) > "%temp%\getadmin.vbs"
    echo UAC.ShellExecute "%~f0", "", "", "runas", 1 >> "%temp%\getadmin.vbs"
    
    REM 运行VBS脚本
    "%temp%\getadmin.vbs"
    del "%temp%\getadmin.vbs"
    exit /b
)

echo 已获得管理员权限
echo.
echo 正在启动NFC登录系统...
echo.

REM 运行应用程序
cd /d "%~dp0"
dotnet run --project NFCLoginSystem\NFCLoginSystem.csproj

if %errorlevel% neq 0 (
    echo.
    echo 启动失败，请检查错误信息
    pause
)

pause