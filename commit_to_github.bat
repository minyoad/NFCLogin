@echo off
echo ========================================
echo GitHub Commit Helper for NFC Login System
echo ========================================
echo.

:: 检查Git是否安装
where git >nul 2>&1
if %errorlevel% neq 0 (
    echo ERROR: Git is not installed or not in PATH.
    echo Please install Git and ensure it's available in your system PATH.
    pause
    exit /b 1
)

:: 初始化Git仓库（如果不存在）
if not exist ".git" (
    echo 1. Initializing Git repository...
    git init
    if %errorlevel% neq 0 (
        echo ERROR: Failed to initialize Git repository.
        pause
        exit /b 1
    )
)

:: 添加所有文件
echo 2. Adding all files to staging area...
git add .
if %errorlevel% neq 0 (
    echo WARNING: There were issues adding some files.
)

:: 检查是否有更改需要提交
git diff --cached --quiet
if %errorlevel% equ 0 (
    echo No changes to commit.
    echo.
    echo If you want to force a commit, you can run:
    echo   git commit --allow-empty -m "Empty commit for testing"
    pause
    exit /b 0
)

:: 获取提交信息
echo.
echo Please enter a commit message (or press Enter for default):
set /p commit_message=
if "%commit_message%"=="" set "commit_message=NFC Login System - Complete implementation with Windows Credential Provider"

:: 提交更改
echo 3. Committing changes...
git commit -m "%commit_message%"
if %errorlevel% neq 0 (
    echo ERROR: Commit failed.
    pause
    exit /b 1
)

echo.
echo ========================================
echo Commit successful!
echo ========================================
echo.
echo Next steps to push to GitHub:
echo 1. Create a new repository on GitHub
echo 2. Add remote repository: git remote add origin https://github.com/YOUR_USERNAME/YOUR_REPO.git
echo 3. Push to GitHub: git push -u origin main
echo.
echo Example commands:
echo   git remote add origin https://github.com/yourusername/nfc-login-system.git
echo   git push -u origin main
echo.
pause