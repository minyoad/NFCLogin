@echo off
setlocal enabledelayedexpansion

echo ========================================
echo 构建NFC Credential Provider项目
echo ========================================
echo.

:: 检查管理员权限
net session >nul 2>&1
if %errorlevel% neq 0 (
    echo 错误：需要管理员权限运行此构建脚本
    echo 请右键点击此脚本，选择"以管理员身份运行"
    pause
    exit /b 1
)

:: 设置Visual Studio环境变量
echo 1. 设置Visual Studio环境...
call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat" >nul 2>&1
if %errorlevel% neq 0 (
    echo 错误：无法找到Visual Studio 2022
    echo 请确保已安装Visual Studio 2022 Community版本
    pause
    exit /b 1
)

:: 设置项目路径
set "SOLUTION=NFCCredentialProvider\NFCCredentialProvider.sln"
set "CONFIG=Release"
set "PLATFORM=x64"

echo 2. 构建解决方案...
echo 解决方案：%SOLUTION%
echo 配置：%CONFIG%
echo 平台：%PLATFORM%
echo.

:: 使用MSBuild构建项目
msbuild "%SOLUTION%" /p:Configuration=%CONFIG% /p:Platform=%PLATFORM% /m

if %errorlevel% equ 0 (
    echo.
    echo ========================================
    echo 主项目构建成功！
    echo ========================================
    echo.
) else (
    echo.
    echo ========================================
    echo 主项目构建失败！
    echo ========================================
    echo.
    pause
    exit /b 1
)

:: 构建测试程序
echo 3. 构建测试程序...
call build_test.bat

if %errorlevel% equ 0 (
    echo.
    echo ========================================
    echo 测试程序构建成功！
    echo ========================================
    echo.
) else (
    echo.
    echo ========================================
    echo 测试程序构建失败！
    echo ========================================
    echo.
)

:: 显示构建结果
echo 4. 构建结果：
echo.
echo 生成的文件：
dir /s /b *.dll *.exe | findstr /i "release\\|testnfc"
echo.
echo ========================================
echo 构建完成！
echo ========================================
echo.
echo 下一步：
echo 1. 运行 install.bat 安装Credential Provider
echo 2. 运行 TestNFCLogin.exe 测试NFC功能
echo 3. 重启计算机使更改生效
echo.
pause