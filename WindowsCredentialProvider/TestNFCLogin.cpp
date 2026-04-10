// 这是一个简单的测试程序，用于验证NFC登录功能

#include <windows.h>
#include <iostream>
#include <string>
#include <fstream>
#include "AccountManager.h"
#include "NFCManager.h"

void TestNFCLogin() {
    std::cout << "=== NFC登录测试工具 ===" << std::endl;
    
    // 初始化账户管理器
    AccountManager accountManager;
    HRESULT hr = accountManager.Initialize();
    if (FAILED(hr)) {
        std::cout << "账户管理器初始化失败" << std::endl;
        return;
    }
    
    // 初始化NFC管理器
    NFCManager nfcManager;
    hr = nfcManager.Initialize();
    if (FAILED(hr)) {
        std::cout << "NFC管理器初始化失败" << std::endl;
        return;
    }
    
    std::cout << "请放置NFC卡片..." << std::endl;
    
    // 等待卡片
    hr = nfcManager.WaitForCard(10000); // 10秒超时
    if (FAILED(hr)) {
        std::cout << "等待卡片超时" << std::endl;
        return;
    }
    
    // 读取UID
    std::string uid;
    hr = nfcManager.ReadCardUID(uid);
    if (FAILED(hr)) {
        std::cout << "读取UID失败" << std::endl;
        return;
    }
    
    std::cout << "检测到卡片，UID: " << uid << std::endl;
    
    // 查找用户
    std::wstring username;
    hr = accountManager.FindUserByNFCCardUID(uid, username);
    if (FAILED(hr) || username.empty()) {
        std::cout << "未找到绑定此卡片的用户" << std::endl;
        
        // 询问是否创建新用户
        std::cout << "是否创建新用户？(y/n): ";
        char response;
        std::cin >> response;
        
        if (response == 'y' || response == 'Y') {
            std::wstring newUsername;
            std::wstring newPassword;
            std::wstring fullName;
            
            std::wcout << L"请输入用户名: ";
            std::wcin >> newUsername;
            std::wcout << L"请输入密码: ";
            std::wcin >> newPassword;
            std::wcout << L"请输入全名: ";
            std::wcin >> fullName;
            
            // 创建用户并绑定卡片
            hr = accountManager.CreateLocalUser(newUsername, newPassword, fullName, uid);
            if (SUCCEEDED(hr)) {
                std::cout << "用户创建成功并绑定卡片" << std::endl;
                username = newUsername;
            } else {
                std::cout << "用户创建失败" << std::endl;
                return;
            }
        } else {
            return;
        }
    }
    
    std::cout << "找到用户: " << std::string(username.begin(), username.end()) << std::endl;
    
    // 验证用户凭据
    std::wcout << L"请输入密码进行验证: ";
    std::wstring password;
    std::wcin >> password;
    
    hr = accountManager.ValidateLocalUserCredentials(username, password);
    if (SUCCEEDED(hr)) {
        std::cout << "验证成功！可以登录" << std::endl;
    } else {
        std::cout << "验证失败！密码错误" << std::endl;
    }
}

int main() {
    try {
        TestNFCLogin();
    } catch (const std::exception& e) {
        std::cout << "异常: " << e.what() << std::endl;
    }
    
    std::cout << "按任意键退出..." << std::endl;
    std::cin.get();
    return 0;
}