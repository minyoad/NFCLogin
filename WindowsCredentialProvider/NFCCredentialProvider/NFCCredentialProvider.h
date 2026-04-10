#pragma once

#include <credentialprovider.h>
#include <windows.h>
#include <string>
#include <vector>

// CLSID定义
// {7A8A8F2E-4C3D-4F1B-9E2A-3C4D5F6A7B8C}
DEFINE_GUID(CLSID_NFCCredentialProvider, 
0x7a8a8f2e, 0x4c3d, 0x4f1b, 0x9e, 0x2a, 0x3c, 0x4d, 0x5f, 0x6a, 0x7b, 0x8c);

#define SZ_CLSID_NFCCredentialProvider L"{7A8A8F2E-4C3D-4F1B-9E2A-3C4D5F6A7B8C}"

// NFC凭证提供程序类
class NFCCredentialProvider : public ICredentialProvider {
public:
    // IUnknown
    STDMETHODIMP QueryInterface(REFIID riid, void **ppv);
    STDMETHODIMP_(ULONG) AddRef();
    STDMETHODIMP_(ULONG) Release();

    // ICredentialProvider
    STDMETHODIMP SetUsageScenario(CREDENTIAL_PROVIDER_USAGE_SCENARIO cpus, DWORD dwFlags);
    STDMETHODIMP SetSerialization(const CREDENTIAL_PROVIDER_CREDENTIAL_SERIALIZATION *pcpcs);
    STDMETHODIMP Advise(ICredentialProviderEvents *pcpe, UINT_PTR upAdviseContext);
    STDMETHODIMP UnAdvise();
    STDMETHODIMP GetFieldDescriptorCount(DWORD *pdwCount);
    STDMETHODIMP GetFieldDescriptorAt(DWORD dwIndex, CREDENTIAL_PROVIDER_FIELD_DESCRIPTOR **ppcpfd);
    STDMETHODIMP GetCredentialCount(DWORD *pdwCount, DWORD *pdwDefault, BOOL *pbAutoLogonWithDefault);
    STDMETHODIMP GetCredentialAt(DWORD dwIndex, ICredentialProviderCredential **ppcpc);

    // 构造函数和析构函数
    NFCCredentialProvider();
    ~NFCCredentialProvider();

private:
    LONG m_cRef;
    CREDENTIAL_PROVIDER_USAGE_SCENARIO m_cpus;
    ICredentialProviderEvents *m_pcpe;
    UINT_PTR m_upAdviseContext;
    std::vector<ICredentialProviderCredential*> m_rgCredentials;
    
    void _CleanupCredentials();
    HRESULT _CreateCredential();
};

// 辅助函数
void LogMessage(const std::string& message);
std::string ReadNFCCardUID();
std::string GetUsernameForNFCCard(const std::string& nfcUID);
bool ValidateNFCCard(const std::string& nfcUID);
void ReportResult(NTSTATUS ntsStatus, NTSTATUS ntsSubstatus, LPWSTR pszOptionalStatusText, 
                  CREDENTIAL_PROVIDER_STATUS_ICON cpsiOptionalStatusIcon);