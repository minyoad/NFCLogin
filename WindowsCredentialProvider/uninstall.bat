@echo off
echo ========================================
echo NFC Credential Provider Uninstaller
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
set "SYS32_PATH=%SystemRoot%\System32\%DLL_NAME%"
set "PROVIDER_GUID={B4D3B5C1-5A3E-4B8F-9C7D-2E4F8A6B3C1D}"
set "REG_KEY_PROVIDER=HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows\CurrentVersion\Authentication\Credential Providers\%PROVIDER_GUID%"
set "REG_KEY_CLSID=HKEY_LOCAL_MACHINE\SOFTWARE\Classes\CLSID\%PROVIDER_GUID%"

echo Uninstalling NFC Credential Provider...

:: 1. Unregister the DLL
if exist "%SYS32_PATH%" (
    echo 1. Unregistering the DLL...
    regsvr32 /u /s "%SYS32_PATH%"
)

:: 2. Delete the DLL file
if exist "%SYS32_PATH%" (
    echo 2. Deleting the DLL file...
    takeown /f "%SYS32_PATH%" >nul 2>&1
    icacls "%SYS32_PATH%" /grant administrators:F >nul 2>&1
    del /f /q "%SYS32_PATH%"
)

:: 3. Clean up registry keys
echo 3. Cleaning up registry...
reg delete "%REG_KEY_PROVIDER%" /f >nul 2>&1
reg delete "%REG_KEY_CLSID%" /f >nul 2>&1

echo.
echo ========================================
echo Uninstallation successful!
echo ========================================
echo.
echo IMPORTANT: Please restart your computer to complete the uninstallation.
echo.
pause