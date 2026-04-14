#pragma once

#include <windows.h>
#include <credentialprovider.h>
#include <unknwn.h>
#include <strsafe.h>
#include <shlwapi.h>
#include <shlobj.h>
#include <ntsecapi.h>
#include <security.h>
#include <vector>
#include <fstream>
#include <string>

// 前向声明
class NFCCredentialProviderCredential;

// CLSID定义
// {7A8A8F2E-4C3D-4F1B-9E2A-3C4D5F6A7B8C}
DEFINE_GUID(CLSID_NFCCredentialProvider, 
0x7a8a8f2e, 0x4c3d, 0x4f1b, 0x9e, 0x2a, 0x3c, 0x4d, 0x5f, 0x6a, 0x7b, 0x8c);

// 字符串化CLSID
#define SZ_CLSID_NFCCredentialProvider L"{7A8A8F2E-4C3D-4F1B-9E2A-3C4D5F6A7B8C}"

// 日志记录函数
inline void LogMessage(const std::string& message) {
    std::ofstream log("C:\\NFCLogin\\logs\\provider.log", std::ios::app);
    if (log.is_open()) {
        SYSTEMTIME st;
        GetLocalTime(&st);
        log << "[" << st.wYear << "-" << st.wMonth << "-" << st.wDay << " " 
            << st.wHour << ":" << st.wMinute << ":" << st.wSecond << "] " 
            << message << std::endl;
        log.close();
    }
}

// 读取NFC卡UID
inline std::string ReadNFCCardUID() {
    LogMessage("Attempting to read NFC card...");
    
    // 这里应该实现实际的NFC读取逻辑
    // 现在返回模拟的UID用于测试
    std::ifstream nfcFile("C:\\NFCLogin\\nfc_current_uid.txt");
    if (nfcFile.is_open()) {
        std::string uid;
        std::getline(nfcFile, uid);
        nfcFile.close();
        
        if (!uid.empty()) {
            LogMessage("NFC card detected: " + uid);
            return uid;
        }
    }
    
    LogMessage("No NFC card detected");
    return "";
}

// 验证NFC卡
inline bool ValidateNFCCard(const std::string& nfcUID) {
    // 这里应该查询数据库验证NFC卡
    LogMessage("Validating NFC card: " + nfcUID);
    
    // 模拟验证
    return !nfcUID.empty() && nfcUID.length() >= 8;
}

// NFC凭证提供程序类
class NFCCredentialProvider : public ICredentialProvider, public ICredentialProviderSetUserArray {
public:
    // IUnknown
    IFACEMETHODIMP QueryInterface(REFIID riid, void **ppv) override;
    IFACEMETHODIMP_(ULONG) AddRef() override;
    IFACEMETHODIMP_(ULONG) Release() override;

    // ICredentialProvider
    IFACEMETHODIMP SetUsageScenario(CREDENTIAL_PROVIDER_USAGE_SCENARIO cpus, DWORD dwFlags) override;
    IFACEMETHODIMP SetSerialization(const CREDENTIAL_PROVIDER_CREDENTIAL_SERIALIZATION *pcpcs) override;
    IFACEMETHODIMP Advise(ICredentialProviderEvents *pcpe, UINT_PTR upAdviseContext) override;
    IFACEMETHODIMP UnAdvise() override;
    IFACEMETHODIMP GetCredentialCount(DWORD *pdwCount, DWORD *pdwDefault, BOOL *pbAutoLogonWithDefault) override;
    IFACEMETHODIMP GetCredentialAt(DWORD dwIndex, ICredentialProviderCredential **ppcpc) override;
    IFACEMETHODIMP GetFieldDescriptorCount(DWORD *pdwCount) override;
    IFACEMETHODIMP GetFieldDescriptorAt(DWORD dwIndex, CREDENTIAL_PROVIDER_FIELD_DESCRIPTOR **ppcpfd) override;

    // ICredentialProviderSetUserArray
    IFACEMETHODIMP SetUserArray(ICredentialProviderUserArray *pUserArray) override;

public:
    NFCCredentialProvider();
    virtual ~NFCCredentialProvider();

private:
    HRESULT _CreateCredential();

private:
    LONG m_cRef;
    ICredentialProviderUserArray *m_pUserArray;
    ICredentialProviderEvents *m_pcpe;
    UINT_PTR m_upAdviseContext;
    CREDENTIAL_PROVIDER_USAGE_SCENARIO m_cpus;
    
    // 凭据和字段描述符
    NFCCredentialProviderCredential* m_pCredential;
    CREDENTIAL_PROVIDER_FIELD_DESCRIPTOR* m_rgcpfd;
    DWORD m_dwFieldCount;
};