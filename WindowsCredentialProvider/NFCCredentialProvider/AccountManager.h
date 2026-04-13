#pragma once

#include <windows.h>
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <fstream>
#include <sstream>

// 账户管理器类
class AccountManager {
public:
    AccountManager();
    ~AccountManager();

    // 初始化账户管理器
    HRESULT Initialize();

    // 通过NFC卡UID查找用户
    HRESULT FindUserByNFCCardUID(const std::string& uid, std::wstring& username);

    // 验证本地用户凭据
    HRESULT ValidateLocalUserCredentials(const std::wstring& username, const std::wstring& password);

    // 创建新的本地用户
    HRESULT CreateLocalUser(const std::wstring& username, const std::wstring& password, 
                           const std::wstring& fullName, const std::string& nfcCardUID);

    // 绑定NFC卡到现有用户
    HRESULT BindNFCCardToUser(const std::wstring& username, const std::string& nfcCardUID);

    // 解绑用户的NFC卡
    HRESULT UnbindNFCCardFromUser(const std::wstring& username);

    // 检查用户是否存在
    HRESULT UserExists(const std::wstring& username, bool& exists);

    // 获取用户信息
    HRESULT GetUserInfo(const std::wstring& username, std::wstring& fullName, bool& isActive);

    // 启用/禁用用户
    HRESULT SetUserActive(const std::wstring& username, bool active);

    // 获取所有绑定的NFC卡
    HRESULT GetAllBoundNFCCards(std::map<std::string, std::wstring>& cardMappings);

private:
    // 数据库文件路径
    std::wstring m_dbPath;
    
    // 数据库连接句柄（简化实现，实际应该使用SQLite）
    void* m_dbHandle;
    
    // 初始化数据库
    HRESULT InitializeDatabase();
    
    // 加载账户映射
    HRESULT LoadAccountMappings();
    
    // 保存账户映射
    HRESULT SaveAccountMappings();
    
    // 内存中的账户映射（简化实现）
    std::map<std::string, std::wstring> m_nfcToUserMap;
    std::map<std::wstring, std::string> m_userToNFCMap;
    
    // 用户缓存
    std::map<std::wstring, std::wstring> m_userInfoCache;
};

// 辅助函数
std::wstring GetLocalAppDataPath();
std::wstring GetUserFullName(const std::wstring& username);
bool ValidateUsername(const std::wstring& username);
bool ValidatePassword(const std::wstring& password);
bool ValidateNFCCardUID(const std::string& uid);
std::wstring GetCurrentTimestamp();