@echo off
echo ========================================
echo GitHub提交助手
echo ========================================
echo.

:: 检查Git是否安装
where git >nul 2>&1
if %errorlevel% neq 0 (
    echo 错误：Git未安装或未添加到PATH
    echo 请安装Git并确保其可用
    pause
    exit /b 1
)

:: 初始化Git仓库（如果不存在）
if not exist ".git" (
    echo 1. 初始化Git仓库...
    git init
    if %errorlevel% neq 0 (
        echo 错误：Git仓库初始化失败
        pause
        exit /b 1
    )
)

:: 添加所有文件
echo 2. 添加所有文件到暂存区...
git add .
if %errorlevel% neq 0 (
    echo 警告：添加文件时遇到问题
)

:: 检查是否有更改需要提交
git diff --cached --quiet
if %errorlevel% equ 0 (
    echo 没有需要提交的更改
    pause
    exit /b 0
)

:: 获取提交信息
echo.
echo 请输入提交信息（或按回车使用默认信息）:
set /p commit_message=
if "%commit_message%"=="" set "commit_message=NFC登录系统 - 完整功能实现"

:: 提交更改
echo 3. 提交更改...
git commit -m "%commit_message%"
if %errorlevel% neq 0 (
    echo 错误：提交失败
    pause
    exit /b 1
)

echo.
echo ========================================
echo 提交成功！
echo ========================================
echo.
echo 下一步操作：
echo 1. 添加远程仓库：git remote add origin https://github.com/your-username/your-repo.git
echo 2. 推送到GitHub：git push -u origin main
echo.
echo 注意：请替换your-username和your-repo.git为您的实际仓库地址
echo.
pause