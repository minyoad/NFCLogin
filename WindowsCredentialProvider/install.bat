@echo off
echo ========================================
echo NFC Credential Provider Installer
echo ========================================
echo.

:: Check for admin rights
net session >nul 2>&1
if %errorlevel% neq 0 (
    echo ERROR: Administrator privileges are required.
    echo Please right-click this script and select "Run as administrator".
    pause
    exit /b 1
)

set "DLL_NAME=NFCCredentialProvider.dll"
set "SOURCE_PATH=.\NFCCredentialProvider\x64\Release\%DLL_NAME%"
set "SYS32_PATH=%SystemRoot%\System32\%DLL_NAME%"

echo Installing NFC Credential Provider...

:: 1. Copy DLL to System32
echo 1. Copying %DLL_NAME% to System32...
copy /Y "%SOURCE_PATH%" "%SYS32_PATH%"
if %errorlevel% neq 0 (
    echo ERROR: Failed to copy DLL. Make sure the project has been built first.
    echo You can build it by running build_all.bat.
    pause
    exit /b 1
)

:: 2. Register the DLL
echo 2. Registering the DLL...
regsvr32 /s "%SYS32_PATH%"
if %errorlevel% neq 0 (
    echo ERROR: Failed to register the DLL.
    pause
    exit /b 1
)

echo.
echo ========================================
echo Installation successful!
echo ========================================
echo.
echo IMPORTANT: You must restart your computer for the changes to take effect.
echo After restarting, the NFC login option should appear on the Windows login screen.
echo.
pause