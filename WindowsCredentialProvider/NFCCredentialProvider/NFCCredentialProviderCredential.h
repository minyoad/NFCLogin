#pragma once

#include <windows.h>
#include <credentialprovider.h>
#include <shlwapi.h>
#include <security.h>
#include <ntsecapi.h>
#include <string>

class NFCCredentialProvider; // Forward declaration

// {8A4A6B6C-3B9A-4C8D-A5B5-5A6F2E7D8C9D}
DEFINE_GUID(CLSID_NFCCredentialProviderCredential, 
0x8a4a6b6c, 0x3b9a, 0x4c8d, 0xa5, 0xb5, 0x5a, 0x6f, 0x2e, 0x7d, 0x8c, 0x9d);

// Forward declarations
struct NFC_CREDENTIAL_DATA;

class __declspec(uuid("8A4A6B6C-3B9A-4C8D-A5B5-5A6F2E7D8C9D")) NFCCredentialProviderCredential : public ICredentialProviderCredential2 {
public:
    // IUnknown
    IFACEMETHODIMP QueryInterface(REFIID riid, void **ppv) override;
    IFACEMETHODIMP_(ULONG) AddRef() override;
    IFACEMETHODIMP_(ULONG) Release() override;

    // ICredentialProviderCredential
    IFACEMETHODIMP Advise(ICredentialProviderCredentialEvents *pcpce) override;
    IFACEMETHODIMP UnAdvise() override;
    IFACEMETHODIMP SetSelected(BOOL *pbAutoLogon) override;
    IFACEMETHODIMP SetDeselected() override;
    IFACEMETHODIMP GetFieldState(DWORD dwFieldID, CREDENTIAL_PROVIDER_FIELD_STATE *pcpfs, CREDENTIAL_PROVIDER_FIELD_INTERACTIVE_STATE *pcpfis) override;
    IFACEMETHODIMP GetStringValue(DWORD dwFieldID, PWSTR *ppsz) override;
    IFACEMETHODIMP GetBitmapValue(DWORD dwFieldID, HBITMAP *phbmp) override;
    IFACEMETHODIMP GetCheckboxValue(DWORD dwFieldID, BOOL *pbChecked, PWSTR *ppszLabel) override;
    IFACEMETHODIMP GetSubmitButtonValue(DWORD dwFieldID, DWORD *pdwAdjacentTo) override;
    IFACEMETHODIMP GetComboBoxValueCount(DWORD dwFieldID, DWORD *pcItems, DWORD *pdwSelectedItem) override;
    IFACEMETHODIMP GetComboBoxValueAt(DWORD dwFieldID, DWORD dwItem, PWSTR *ppszItem) override;
    IFACEMETHODIMP SetStringValue(DWORD dwFieldID, PCWSTR psz) override;
    IFACEMETHODIMP SetCheckboxValue(DWORD dwFieldID, BOOL bChecked) override;
    IFACEMETHODIMP SetComboBoxSelectedValue(DWORD dwFieldID, DWORD dwSelectedItem) override;
    IFACEMETHODIMP CommandLinkClicked(DWORD dwFieldID) override;
    IFACEMETHODIMP GetSerialization(CREDENTIAL_PROVIDER_GET_SERIALIZATION_RESPONSE *pcpgsr,
                                   CREDENTIAL_PROVIDER_CREDENTIAL_SERIALIZATION *pcpcs, 
                                   PWSTR *ppszOptionalStatusText, CREDENTIAL_PROVIDER_STATUS_ICON *pcpsiOptionalStatusIcon) override;
    IFACEMETHODIMP ReportResult(NTSTATUS ntsStatus, NTSTATUS ntsSubstatus, 
                               PWSTR *ppszOptionalStatusText, CREDENTIAL_PROVIDER_STATUS_ICON *pcpsiOptionalStatusIcon) override;

    // ICredentialProviderCredential2
    IFACEMETHODIMP GetUserSid(PWSTR *ppszSid) override;

    // 初始化函数
    HRESULT Initialize(CREDENTIAL_PROVIDER_USAGE_SCENARIO cpus, const CREDENTIAL_PROVIDER_FIELD_DESCRIPTOR *rgcpfd, DWORD dwFlags);
    HRESULT Initialize(NFCCredentialProvider* pProvider, const std::wstring& username, const std::string& uid, CREDENTIAL_PROVIDER_USAGE_SCENARIO cpus);


    // Functions to manage auto-logon state, called by the provider
    void SetAutoLogonInfo(const std::wstring& username);
    void ClearAutoLogonInfo();

    static HRESULT NFCCredentialProviderCredential_CreateInstance(REFIID riid, void** ppv);

public:
    NFCCredentialProviderCredential();
    virtual ~NFCCredentialProviderCredential();

private:
    HRESULT _GetStringValueInternal(DWORD dwFieldID, PWSTR* ppsz);
    LONG m_cRef;                    // 引用计数
    CREDENTIAL_PROVIDER_USAGE_SCENARIO m_cpus;  // 使用场景
    CREDENTIAL_PROVIDER_FIELD_DESCRIPTOR *m_rgcpfd;  // 字段描述符
    DWORD m_dwFlags;                // 标志
    DWORD m_dwFieldCount;           // 字段数量
    
    ICredentialProviderCredentialEvents *m_pcpce;  // 事件接口
    
    // Credential data
    std::wstring m_strUsername;     // 用户名
    std::wstring m_strPassword;     // 密码 (no longer used for input, but needed for serialization)
    bool m_fIsAutoLogon;            // 是否为自动登录
};