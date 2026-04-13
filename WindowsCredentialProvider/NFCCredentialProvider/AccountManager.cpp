#include "AccountManager.h"
#include <shlobj.h>
#include <stdexcept>

// Helper to convert std::wstring to std::string
std::string wstring_to_string(const std::wstring& wstr) {
    if (wstr.empty()) return std::string();
    int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
    std::string strTo(size_needed, 0);
    WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), &strTo[0], size_needed, NULL, NULL);
    return strTo;
}

// Helper to convert std::string to std::wstring
std::wstring string_to_wstring(const std::string& str) {
    if (str.empty()) return std::wstring();
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0);
    std::wstring wstrTo(size_needed, 0);
    MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &wstrTo[0], size_needed);
    return wstrTo;
}

AccountManager::AccountManager() : m_db(nullptr) {
    InitializeDatabase();
}

AccountManager::~AccountManager() {
    if (m_db) {
        sqlite3_close(m_db);
    }
}

void AccountManager::InitializeDatabase() {
    wchar_t appDataPath[MAX_PATH];
    if (SUCCEEDED(SHGetFolderPathW(nullptr, CSIDL_LOCAL_APPDATA, nullptr, 0, appDataPath))) {
        m_dbPath = std::wstring(appDataPath) + L"\\NFCLogin\\users.db";
        
        // The C# application is responsible for creating the directory and the database file.
        // Here, we just try to open it.
        int rc = sqlite3_open16(m_dbPath.c_str(), &m_db);
        if (rc != SQLITE_OK) {
            // Log the error, but don't throw, as the DB might not exist yet,
            // which is not a critical failure for the provider itself.
            // In a real-world scenario, you'd have more robust error logging.
            sqlite3_close(m_db);
            m_db = nullptr;
        }
    }
}

std::wstring AccountManager::FindUserByNFCCardUID(const std::wstring& uid) {
    if (!m_db) {
        return L"";
    }

    sqlite3_stmt* stmt = nullptr;
    std::wstring username = L"";

    const char* sql = "SELECT Username FROM Users WHERE NFCCardId = ? AND IsActive = 1";
    
    int rc = sqlite3_prepare_v2(m_db, sql, -1, &stmt, nullptr);
    if (rc == SQLITE_OK) {
        std::string uid_utf8 = wstring_to_string(uid);
        sqlite3_bind_text(stmt, 1, uid_utf8.c_str(), -1, SQLITE_STATIC);

        if (sqlite3_step(stmt) == SQLITE_ROW) {
            const unsigned char* result = sqlite3_column_text(stmt, 0);
            if (result) {
                username = string_to_wstring(reinterpret_cast<const char*>(result));
            }
        }
    }

    sqlite3_finalize(stmt);
    return username;
}

// The C# application handles user validation and binding.
// These methods are no longer needed here.
bool AccountManager::ValidateUser(const std::wstring& username, const std::wstring& password) {
    // This logic is now handled by the C# application and Windows itself.
    return false; 
}

bool AccountManager::BindNFCCardToUser(const std::wstring& username, const std::wstring& uid) {
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
             << L" TIMESTAMP:" << StringToWString(GetCurrentTimestamp()).c_str() 
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