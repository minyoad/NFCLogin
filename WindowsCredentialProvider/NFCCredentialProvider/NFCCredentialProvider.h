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

#include "NFCManager.h"
#include "AccountManager.h"

// 前向声明
class NFCCredentialProviderCredential;

// CLSID定义
// {7A8A8F2E-4C3D-4F1B-9E2A-3C4D5F6A7B8C}
DEFINE_GUID(CLSID_NFCCredentialProvider, 
0x7a8a8f2e, 0x4c3d, 0x4f1b, 0x9e, 0x2a, 0x3c, 0x4d, 0x5f, 0x6a, 0x7b, 0x8c);

// 字符串化CLSID
#define SZ_CLSID_NFCCredentialProvider L"{7A8A8F2E-4C3D-4F1B-9E2A-3C4D5F6A7B8C}"



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

    // Thread management functions
    void _StartMonitorThread();
    void _StopMonitorThread();
    static DWORD WINAPI _MonitorThreadProc(LPVOID lpParam);

private:
    LONG m_cRef;
    ICredentialProviderUserArray *m_pUserArray;
    ICredentialProviderEvents *m_pcpe;
    UINT_PTR m_upAdviseContext;
    CREDENTIAL_PROVIDER_USAGE_SCENARIO m_cpus;
    
    // 凭据和字段描述符
    NFCCredentialProviderCredential* m_pCredential;
    std::vector<NFCCredentialProviderCredential*> m_rgpCredentials; // Vector to hold credential objects
    CREDENTIAL_PROVIDER_FIELD_DESCRIPTOR* m_rgcpfd;
    DWORD m_dwFieldCount;

    // NFC, Account, and Threading
    NFCManager m_nfcManager;
    AccountManager m_accountManager;
    HANDLE m_hMonitorThread;
    BOOL m_bStopMonitor;

    // Thread-safe state for sharing card info between threads
    CRITICAL_SECTION m_critsecCardState;
    std::string m_sCardUID;
    std::wstring m_sCardUserName;
    
    // Thread-safe UI notification flag
    CRITICAL_SECTION m_critsecUINotify;
    BOOL m_bPendingUINotify;
};