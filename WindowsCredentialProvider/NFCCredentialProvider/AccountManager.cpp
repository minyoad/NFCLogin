#include "AccountManager.h"
#include "NFCManager.h"
#include <windows.h>
#include <lm.h>
#include <lmapibuf.h>
#include <wincred.h>
#include <shlobj.h>
#include <iomanip>

#pragma comment(lib, "netapi32.lib")
#pragma comment(lib, "advapi32.lib")

AccountManager::AccountManager() : m_dbHandle(nullptr) {
}

AccountManager::~AccountManager() {
    if (m_dbHandle) {
        // 关闭数据库连接
        m_dbHandle = nullptr;
    }
}

HRESULT AccountManager::Initialize() {
    // 获取本地应用数据路径
    wchar_t appDataPath[MAX_PATH];
    if (SUCCEEDED(SHGetFolderPathW(nullptr, CSIDL_LOCAL_APPDATA, nullptr, 0, appDataPath))) {
        m_dbPath = std::wstring(appDataPath) + L"\\NFCLogin\\accounts.db";
        
        // 确保目录存在
        std::wstring dirPath = m_dbPath.substr(0, m_dbPath.find_last_of(L"\\"));
        CreateDirectoryW(dirPath.c_str(), nullptr);
        
        return InitializeDatabase();
    }
    
    return E_FAIL;
}

HRESULT AccountManager::InitializeDatabase() {
    // 这里应该初始化SQLite数据库
    // 现在创建简单的文件存储
    std::wofstream file(m_dbPath);
    if (file.is_open()) {
        file << L"# NFC Login Account Database" << std::endl;
        file.close();
        return S_OK;
    }
    
    return E_FAIL;
}

HRESULT AccountManager::FindUserByNFCCardUID(const std::string& uid, std::wstring& username) {
    if (uid.empty()) {
        return E_INVALIDARG;
    }
    
    // 从数据库查找用户
    // 简化实现：从文件读取映射
    std::wifstream file(m_dbPath);
    if (file.is_open()) {
        std::wstring line;
        while (std::getline(file, line)) {
            if (line.find(L"UID:") == 0) {
                size_t uidStart = line.find(L":") + 1;
                size_t uidEnd = line.find(L" ", uidStart);
                std::wstring storedUID = line.substr(uidStart, uidEnd - uidStart);
                
                std::string storedUIDStr = WStringToString(storedUID);
                if (storedUIDStr == uid) {
                    size_t userStart = line.find(L"USER:") + 5;
                    username = line.substr(userStart);
                    file.close();
                    return S_OK;
                }
            }
        }
        file.close();
    }
    
    return S_FALSE; // 未找到用户
}

HRESULT AccountManager::ValidateLocalUserCredentials(const std::wstring& username, const std::wstring& password) {
    if (username.empty() || password.empty()) {
        return E_INVALIDARG;
    }
    
    // 验证Windows本地用户凭据
    HANDLE hToken = nullptr;
    BOOL result = LogonUserW(
        username.c_str(),
        L".", // 本地计算机
        password.c_str(),
        LOGON32_LOGON_INTERACTIVE,
        LOGON32_PROVIDER_DEFAULT,
        &hToken
    );
    
    if (result) {
        CloseHandle(hToken);
        return S_OK;
    }
    
    return HRESULT_FROM_WIN32(GetLastError());
}

HRESULT AccountManager::CreateLocalUser(const std::wstring& username, const std::wstring& password, 
                                      const std::wstring& fullName, const std::string& nfcCardUID) {
    if (!ValidateUsername(username) || !ValidatePassword(password)) {
        return E_INVALIDARG;
    }
    
    // 检查用户是否已存在
    bool exists = false;
    HRESULT hr = UserExists(username, exists);
    if (SUCCEEDED(hr) && exists) {
        return HRESULT_FROM_WIN32(ERROR_USER_EXISTS);
    }
    
    // 创建本地用户
    USER_INFO_1 ui1 = {0};
    ui1.usri1_name = const_cast<LPWSTR>(username.c_str());
    ui1.usri1_password = const_cast<LPWSTR>(password.c_str());
    ui1.usri1_priv = USER_PRIV_USER;
    ui1.usri1_home_dir = nullptr;
    ui1.usri1_comment = const_cast<LPWSTR>(L"NFC Login User");
    ui1.usri1_flags = UF_NORMAL_ACCOUNT | UF_SCRIPT;
    ui1.usri1_script_path = nullptr;
    
    DWORD dwError = 0;
    NET_API_STATUS status = NetUserAdd(nullptr, 1, (LPBYTE)&ui1, &dwError);
    
    if (status == NERR_Success) {
        // 设置用户全名
        USER_INFO_1011 ui1011 = {0};
        ui1011.usri1011_full_name = const_cast<LPWSTR>(fullName.c_str());
        NetUserSetInfo(nullptr, username.c_str(), 1011, (LPBYTE)&ui1011, &dwError);
        
        // 绑定NFC卡
        if (!nfcCardUID.empty()) {
            hr = BindNFCCardToUser(username, nfcCardUID);
        } else {
            hr = S_OK;
        }
        
        return hr;
    }
    
    return HRESULT_FROM_WIN32(status);
}

HRESULT AccountManager::BindNFCCardToUser(const std::wstring& username, const std::string& nfcCardUID) {
    if (!ValidateNFCCardUID(nfcCardUID)) {
        return E_INVALIDARG;
    }
    
    // 检查NFC卡是否已被绑定
    std::wstring existingUser;
    HRESULT hr = FindUserByNFCCardUID(nfcCardUID, existingUser);
    if (SUCCEEDED(hr) && !existingUser.empty() && existingUser != username) {
        return HRESULT_FROM_WIN32(ERROR_ALREADY_EXISTS);
    }
    
    // 保存绑定信息
    std::wofstream file(m_dbPath, std::ios::app);
    if (file.is_open()) {
        file << L"UID:" << StringToWString(nfcCardUID).c_str() 
             << L" USER:" << username.c_str() 
             << L" TIMESTAMP:" << GetCurrentTimestamp().c_str() 
             << std::endl;
        file.close();
        return S_OK;
    }
    
    return E_FAIL;
}

HRESULT AccountManager::UnbindNFCCardFromUser(const std::wstring& username) {
    // 读取所有绑定信息
    std::vector<std::wstring> lines;
    std::wifstream inFile(m_dbPath);
    if (inFile.is_open()) {
        std::wstring line;
        while (std::getline(inFile, line)) {
            if (line.find(L"USER:" + username) == std::wstring::npos) {
                lines.push_back(line);
            }
        }
        inFile.close();
    }
    
    // 写回文件（排除指定用户的绑定）
    std::wofstream outFile(m_dbPath);
    if (outFile.is_open()) {
        for (const auto& line : lines) {
            outFile << line << std::endl;
        }
        outFile.close();
        return S_OK;
    }
    
    return E_FAIL;
}

HRESULT AccountManager::UserExists(const std::wstring& username, bool& exists) {
    exists = false;
    
    USER_INFO_0* pUserInfo = nullptr;
    NET_API_STATUS status = NetUserGetInfo(nullptr, username.c_str(), 0, (LPBYTE*)&pUserInfo);
    
    if (status == NERR_Success) {
        exists = true;
        NetApiBufferFree(pUserInfo);
        return S_OK;
    } else if (status == NERR_UserNotFound) {
        exists = false;
        return S_OK;
    }
    
    return HRESULT_FROM_WIN32(status);
}

HRESULT AccountManager::GetUserInfo(const std::wstring& username, std::wstring& fullName, bool& isActive) {
    USER_INFO_1011* pUserInfo = nullptr;
    NET_API_STATUS status = NetUserGetInfo(nullptr, username.c_str(), 1011, (LPBYTE*)&pUserInfo);
    
    if (status == NERR_Success) {
        if (pUserInfo->usri1011_full_name) {
            fullName = pUserInfo->usri1011_full_name;
        } else {
            fullName = username;
        }
        
        // 获取用户状态
        USER_INFO_2* pUserInfo2 = nullptr;
        status = NetUserGetInfo(nullptr, username.c_str(), 2, (LPBYTE*)&pUserInfo2);
        if (status == NERR_Success) {
            isActive = !(pUserInfo2->usri2_flags & UF_ACCOUNTDISABLE);
            NetApiBufferFree(pUserInfo2);
        } else {
            isActive = true;
        }
        
        NetApiBufferFree(pUserInfo);
        return S_OK;
    }
    
    return HRESULT_FROM_WIN32(status);
}

HRESULT AccountManager::SetUserActive(const std::wstring& username, bool active) {
    USER_INFO_1008 ui1008 = {0};
    ui1008.usri1008_flags = active ? UF_NORMAL_ACCOUNT : UF_ACCOUNTDISABLE;
    
    DWORD dwError = 0;
    NET_API_STATUS status = NetUserSetInfo(nullptr, username.c_str(), 1008, (LPBYTE)&ui1008, &dwError);
    
    return (status == NERR_Success) ? S_OK : HRESULT_FROM_WIN32(status);
}

HRESULT AccountManager::GetAllBoundNFCCards(std::map<std::string, std::wstring>& cardMappings) {
    cardMappings.clear();
    
    std::wifstream file(m_dbPath);
    if (file.is_open()) {
        std::wstring line;
        while (std::getline(file, line)) {
            if (line.find(L"UID:") == 0) {
                size_t uidStart = line.find(L":") + 1;
                size_t uidEnd = line.find(L" ", uidStart);
                std::wstring storedUID = line.substr(uidStart, uidEnd - uidStart);
                
                size_t userStart = line.find(L"USER:") + 5;
                size_t userEnd = line.find(L" ", userStart);
                std::wstring username = line.substr(userStart, userEnd - userStart);
                
                cardMappings[WStringToString(storedUID)] = username;
            }
        }
        file.close();
        return S_OK;
    }
    
    return E_FAIL;
}

// 辅助函数实现
std::wstring StringToWString(const std::string& str) {
    if (str.empty()) return std::wstring();
    
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), nullptr, 0);
    std::wstring wstrTo(size_needed, 0);
    MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &wstrTo[0], size_needed);
    return wstrTo;
}

std::string WStringToString(const std::wstring& wstr) {
    if (wstr.empty()) return std::string();
    
    int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), nullptr, 0, nullptr, nullptr);
    std::string strTo(size_needed, 0);
    WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), &strTo[0], size_needed, nullptr, nullptr);
    return strTo;
}

std::wstring GetLocalAppDataPath() {
    wchar_t path[MAX_PATH];
    if (SUCCEEDED(SHGetFolderPathW(nullptr, CSIDL_LOCAL_APPDATA, nullptr, 0, path))) {
        return std::wstring(path);
    }
    return L"";
}

std::wstring GetUserFullName(const std::wstring& username) {
    USER_INFO_1011* pUserInfo = nullptr;
    NET_API_STATUS status = NetUserGetInfo(nullptr, username.c_str(), 1011, (LPBYTE*)&pUserInfo);
    
    std::wstring fullName = username;
    if (status == NERR_Success) {
        if (pUserInfo->usri1011_full_name) {
            fullName = pUserInfo->usri1011_full_name;
        }
        NetApiBufferFree(pUserInfo);
    }
    
    return fullName;
}

bool ValidateUsername(const std::wstring& username) {
    if (username.empty() || username.length() > 20) {
        return false;
    }
    
    // 检查是否包含非法字符
    for (wchar_t c : username) {
        if (!iswalnum(c) && c != L'_' && c != L'-') {
            return false;
        }
    }
    
    return true;
}

bool ValidatePassword(const std::wstring& password) {
    return !password.empty() && password.length() >= 6;
}

bool ValidateNFCCardUID(const std::string& uid) {
    if (uid.empty() || uid.length() != 8) {
        return false;
    }
    
    // 检查是否为有效的十六进制UID
    for (char c : uid) {
        if (!isxdigit(c)) {
            return false;
        }
    }
    
    return true;
}

std::wstring GetCurrentTimestamp() {
    SYSTEMTIME st;
    GetLocalTime(&st);
    
    std::wstringstream ss;
    ss << std::setfill(L'0') 
       << std::setw(4) << st.wYear << L"-"
       << std::setw(2) << st.wMonth << L"-"
       << std::setw(2) << st.wDay << L" "
       << std::setw(2) << st.wHour << L":"
       << std::setw(2) << st.wMinute << L":"
       << std::setw(2) << st.wSecond;
    
    return ss.str();
}