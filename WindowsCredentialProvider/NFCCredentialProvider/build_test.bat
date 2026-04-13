@echo off
echo Building NFC PCSC Test...

REM 设置Visual Studio环境（根据你的安装路径调整）
call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvars64.bat"

REM 编译测试程序
cl /EHsc /std:c++17 /W4 TestPCSC.cpp NFCManager.cpp /link winscard.lib setupapi.lib

if %ERRORLEVEL% EQU 0 (
    echo Build successful!
    echo.
    echo You can now run TestPCSC.exe to test NFC functionality
) else (
    echo Build failed!
)

pause