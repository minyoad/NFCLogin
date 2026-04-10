@echo off
setlocal enabledelayedexpansion

echo ========================================
echo 编译NFC测试程序
echo ========================================
echo.

:: 检查管理员权限
net session >nul 2>&1
if %errorlevel% neq 0 (
    echo 错误：需要管理员权限运行此编译脚本
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

:: 设置编译参数
set "SOURCE=TestNFCLogin.cpp"
set "OUTPUT=TestNFCLogin.exe"
set "INCLUDE_PATH=NFCCredentialProvider"
set "LIB_PATH=NFCCredentialProvider\x64\Release"

echo 2. 编译测试程序...
echo 源文件：%SOURCE%
echo 输出文件：%OUTPUT%
echo.

:: 编译命令
cl /EHsc /W3 /O2 /DUNICODE /D_UNICODE^
   /I"%INCLUDE_PATH%"^
   /Fe"%OUTPUT%"^
   "%SOURCE%"^
   /link /LIBPATH:"%LIB_PATH%"^
   advapi32.lib netapi32.lib user32.lib kernel32.lib

if %errorlevel% equ 0 (
    echo.
    echo ========================================
    echo 编译成功！
    echo ========================================
    echo.
    echo 可执行文件已生成：%OUTPUT%
    echo.
    echo 使用方法：
    echo 1. 确保NFC读卡器已连接
    echo 2. 运行 %OUTPUT%
    echo 3. 按照提示放置NFC卡片进行测试
    echo.
) else (
    echo.
    echo ========================================
    echo 编译失败！
    echo ========================================
    echo.
    echo 请检查错误信息并确保：
    echo 1. Visual Studio 2022已正确安装
    echo 2. 所有依赖文件都存在
    echo 3. 项目已正确配置
    echo.
)

pause