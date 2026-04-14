#include "AccountManager.h"
#include <shlobj.h>
#include <lm.h>
#include <stdexcept>

// Forward declaration for logging function from NFCCredentialProvider.cpp
void LogMessage(const char* format, ...);

#pragma comment(lib, "netapi32.lib")

// Helper to convert std::wstring to std::string
std::string WStringToString(const std::wstring& wstr) {
    if (wstr.empty()) return std::string();
    int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
    std::string strTo(size_needed, 0);
    WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), &strTo[0], size_needed, NULL, NULL);
    return strTo;
}

// Helper to convert std::string to std::wstring
std::wstring StringToWString(const std::string& str) {
    if (str.empty()) return std::wstring();
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0);
    std::wstring wstrTo(size_needed, 0);
    MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &wstrTo[0], size_needed);
    return wstrTo;
}


AccountManager::AccountManager() : m_db(nullptr) {
    // Initialization is now handled by the public Initialize() method
}

AccountManager::~AccountManager() {
    if (m_db) {
        sqlite3_close(m_db);
        m_db = nullptr;
    }
}

HRESULT AccountManager::Initialize() {
    return InitializeDatabase();
}

HRESULT AccountManager::InitializeDatabase() {
    if (m_db) {
        return S_OK; // Already initialized
    }

    wchar_t appDataPath[MAX_PATH];
    if (FAILED(SHGetFolderPathW(nullptr, CSIDL_LOCAL_APPDATA, nullptr, 0, appDataPath))) {
        LogMessage("Failed to get Local AppData path.");
        return E_FAIL;
    }

    m_dbPath = std::wstring(appDataPath) + L"\\NFCLogin";
    
    // Create the directory if it doesn't exist
    if (!CreateDirectoryW(m_dbPath.c_str(), nullptr) && GetLastError() != ERROR_ALREADY_EXISTS) {
        LogMessage("Failed to create directory: %ls. Error: %d", m_dbPath.c_str(), GetLastError());
        return E_FAIL;
    }

    m_dbPath += L"\\users.db";
    LogMessage("Database path: %ls", m_dbPath.c_str());
    
    int rc = sqlite3_open16(m_dbPath.c_str(), &m_db);
    if (rc != SQLITE_OK) {
        LogMessage("Failed to open database: %s", sqlite3_errmsg(m_db));
        sqlite3_close(m_db);
        m_db = nullptr;
        return E_FAIL;
    }

    LogMessage("Database initialized successfully.");
    return S_OK;
}

HRESULT AccountManager::FindUserByNFCCardUID(const std::string& uid, std::wstring& username) {
    if (!m_db) {
        return E_FAIL;
    }

    sqlite3_stmt* stmt = nullptr;
    username = L"";
    HRESULT hr = E_FAIL;

    const char* sql = "SELECT Username FROM Users WHERE NFCCardId = ? AND IsActive = 1";
    
    int rc = sqlite3_prepare_v2(m_db, sql, -1, &stmt, nullptr);
    if (rc == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, uid.c_str(), -1, SQLITE_STATIC);

        if (sqlite3_step(stmt) == SQLITE_ROW) {
            const unsigned char* result = sqlite3_column_text(stmt, 0);
            if (result) {
                username = StringToWString(reinterpret_cast<const char*>(result));
                hr = S_OK;
            }
        }
        else {
            hr = S_FALSE; // Not found
        }
    }

    sqlite3_finalize(stmt);
    return hr;
}

HRESULT AccountManager::ValidateLocalUserCredentials(const std::wstring& username, const std::wstring& password)
{
    std::wstring domain = L".";
    std::wstring user = username;

    size_t slashPos = username.find(L'\\');
    if (slashPos != std::wstring::npos) {
        domain = username.substr(0, slashPos);
        user = username.substr(slashPos + 1);
    } else {
        size_t atPos = username.find(L'@');
        if (atPos != std::wstring::npos) {
            // For UPN format (user@domain.com), LogonUserW expects domain to be the DNS domain name
            // and username to be the full UPN.
            // However, for simplicity and broader compatibility, we can split it,
            // but it's often better to pass the full UPN as the username and NULL/L"." as the domain
            // if the UPN is for a federated domain. Let's stick to a simpler split for now.
            domain = username.substr(atPos + 1);
            user = username; // Keep the full UPN as username for LogonUserW
        }
    }

    HANDLE hToken;
    if (LogonUserW(user.c_str(), domain.c_str(), password.c_str(), LOGON32_LOGON_INTERACTIVE, LOGON32_PROVIDER_DEFAULT, &hToken))
    {
        CloseHandle(hToken);
        return S_OK;
    }
    return E_ACCESSDENIED;
}

HRESULT AccountManager::CreateLocalUser(const std::wstring& username, const std::wstring& password, const std::wstring& fullName, const std::string& nfcCardUID)
{
    // This functionality is primarily handled by the C# application.
    // This is a placeholder and would require significant implementation for a real-world scenario.
    return E_NOTIMPL;
}

HRESULT AccountManager::BindNFCCardToUser(const std::wstring& username, const std::string& nfcCardUID)
{
    // This functionality is primarily handled by the C# application.
    return E_NOTIMPL;
}

HRESULT AccountManager::UnbindNFCCardFromUser(const std::wstring& username)
{
    // This functionality is primarily handled by the C# application.
    return E_NOTIMPL;
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
    USER_INFO_2* pUserInfo = nullptr;
    NET_API_STATUS status = NetUserGetInfo(nullptr, username.c_str(), 2, (LPBYTE*)&pUserInfo);
    
    if (status == NERR_Success) {
        if (pUserInfo->usri2_full_name) {
            fullName = pUserInfo->usri2_full_name;
        } else {
            fullName = username;
        }
        
        isActive = !(pUserInfo->usri2_flags & UF_ACCOUNTDISABLE);
        
        NetApiBufferFree(pUserInfo);
        return S_OK;
    }
    
    return HRESULT_FROM_WIN32(status);
}

HRESULT AccountManager::SetUserActive(const std::wstring& username, bool active) {
    USER_INFO_1008 ui1008;
    DWORD dwError = 0;
    
    // First, get the current flags
    USER_INFO_2* pUserInfo2 = nullptr;
    NET_API_STATUS status = NetUserGetInfo(nullptr, username.c_str(), 2, (LPBYTE*)&pUserInfo2);
    if (status != NERR_Success) {
        return HRESULT_FROM_WIN32(status);
    }

    if (active) {
        pUserInfo2->usri2_flags &= ~UF_ACCOUNTDISABLE;
    } else {
        pUserInfo2->usri2_flags |= UF_ACCOUNTDISABLE;
    }
    
    status = NetUserSetInfo(nullptr, username.c_str(), 2, (LPBYTE)pUserInfo2, &dwError);
    NetApiBufferFree(pUserInfo2);
    
    return (status == NERR_Success) ? S_OK : HRESULT_FROM_WIN32(status);
}

HRESULT AccountManager::GetAllBoundNFCCards(std::map<std::string, std::wstring>& cardMappings) {
    cardMappings.clear();
    if (!m_db) {
        return E_FAIL;
    }

    sqlite3_stmt* stmt = nullptr;
    const char* sql = "SELECT NFCCardId, Username FROM Users WHERE NFCCardId IS NOT NULL AND NFCCardId != ''";

    int rc = sqlite3_prepare_v2(m_db, sql, -1, &stmt, nullptr);
    if (rc == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            const unsigned char* cardId_c = sqlite3_column_text(stmt, 0);
            const unsigned char* username_c = sqlite3_column_text(stmt, 1);

            if (cardId_c && username_c) {
                std::string cardId = reinterpret_cast<const char*>(cardId_c);
                std::wstring username = StringToWString(reinterpret_cast<const char*>(username_c));
                cardMappings[cardId] = username;
            }
        }
    }
    
    sqlite3_finalize(stmt);
    return S_OK;
}

HRESULT AccountManager::LoadAccountMappings()
{
    // This is now handled by direct DB queries.
    return S_OK;
}

HRESULT AccountManager::SaveAccountMappings()
{
    // This is now handled by direct DB queries.
    return S_OK;
}

// Auxiliary function implementations from the header
std::wstring GetLocalAppDataPath() {
    wchar_t path[MAX_PATH];
    if (SUCCEEDED(SHGetFolderPathW(nullptr, CSIDL_LOCAL_APPDATA, nullptr, 0, path))) {
        return std::wstring(path);
    }
    return L"";
}

std::wstring GetUserFullName(const std::wstring& username) {
    std::wstring fullName;
    bool isActive;
    AccountManager am;
    if (SUCCEEDED(am.Initialize()) && SUCCEEDED(am.GetUserInfo(username, fullName, isActive))) {
        return fullName;
    }
    return username;
}

bool ValidateUsername(const std::wstring& username) {
    if (username.empty() || username.length() > 20) {
        return false;
    }
    for (wchar_t c : username) {
        if (!iswalnum(c) && c != L'_' && c != L'-' && c != L'.' && c != L' ') {
            return false;
        }
    }
    return true;
}

bool ValidatePassword(const std::wstring& password) {
    return !password.empty();
}

bool ValidateNFCCardUID(const std::string& uid) {
    if (uid.empty()) {
        return false;
    }
    for (char c : uid) {
        if (!isxdigit(c)) {
            return false;
        }
    }
    return true;
}