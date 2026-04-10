#pragma once

#include <windows.h>
#include <credentialprovider.h>
#include <shlwapi.h>
#include <security.h>
#include <ntsecapi.h>
#include <vector>
#include <string>

// 前向声明
class AccountManager;

class NFCCredentialProviderCredential : public ICredentialProviderCredential {
public:
    // IUnknown
    IFACEMETHODIMP QueryInterface(REFIID riid, void **ppv);
    IFACEMETHODIMP_(ULONG) AddRef();
    IFACEMETHODIMP_(ULONG) Release();

    // ICredentialProviderCredential
    IFACEMETHODIMP Advise(ICredentialProviderCredentialEvents *pcpce);
    IFACEMETHODIMP UnAdvise();
    IFACEMETHODIMP SetSelected(BOOL *pbAutoLogon);
    IFACEMETHODIMP SetDeselected();
    IFACEMETHODIMP GetFieldState(DWORD dwFieldID, CREDENTIAL_PROVIDER_FIELD_STATE *pcpfs, CREDENTIAL_PROVIDER_FIELD_INTERACTIVE_STATE *pcpfis);
    IFACEMETHODIMP GetStringValue(DWORD dwFieldID, PWSTR *ppsz);
    IFACEMETHODIMP GetBitmapValue(DWORD dwFieldID, HBITMAP *phbmp);
    IFACEMETHODIMP GetCheckboxValue(DWORD dwFieldID, BOOL *pbChecked, PWSTR *ppszLabel);
    IFACEMETHODIMP GetSubmitButtonValue(DWORD dwFieldID, DWORD *pdwAdjacentTo);
    IFACEMETHODIMP GetComboBoxValueCount(DWORD dwFieldID, DWORD *pcItems, DWORD *pdwSelectedItem);
    IFACEMETHODIMP GetComboBoxValueAt(DWORD dwFieldID, DWORD dwItem, PWSTR *ppszItem);
    IFACEMETHODIMP SetStringValue(DWORD dwFieldID, PCWSTR psz);
    IFACEMETHODIMP SetCheckboxValue(DWORD dwFieldID, BOOL bChecked);
    IFACEMETHODIMP SetComboBoxSelectedValue(DWORD dwFieldID, DWORD dwSelectedItem);
    IFACEMETHODIMP CommandLinkClicked(DWORD dwFieldID);
    IFACEMETHODIMP GetSerialization(CREDENTIAL_PROVIDER_GET_SERIALIZATION_RESPONSE *pcpgsr,
                                   CREDENTIAL_PROVIDER_CREDENTIAL_SERIALIZATION *pcpcs, 
                                   PWSTR *ppszOptionalStatusText, CREDENTIAL_PROVIDER_STATUS_ICON *pcpsiOptionalStatusIcon);
    IFACEMETHODIMP ReportResult(NTSTATUS ntsStatus, NTSTATUS ntsSubstatus, 
                               PWSTR *ppszOptionalStatusText, CREDENTIAL_PROVIDER_STATUS_ICON *pcpsiOptionalStatusIcon);

    // 初始化函数
    HRESULT Initialize(CREDENTIAL_PROVIDER_USAGE_SCENARIO cpus, const CREDENTIAL_PROVIDER_FIELD_DESCRIPTOR *rgcpfd, DWORD dwFlags);

public:
    NFCCredentialProviderCredential();
    virtual ~NFCCredentialProviderCredential();

private:
    LONG m_cRef;                    // 引用计数
    CREDENTIAL_PROVIDER_USAGE_SCENARIO m_cpus;  // 使用场景
    CREDENTIAL_PROVIDER_FIELD_DESCRIPTOR *m_rgcpfd;  // 字段描述符
    DWORD m_dwFlags;                // 标志
    DWORD m_dwFieldCount;           // 字段数量
    
    ICredentialProviderCredentialEvents *m_pcpce;  // 事件接口
    AccountManager *m_pAccountManager;  // 账户管理器
    
    // 字段值
    std::wstring m_strUsername;     // 用户名
    std::wstring m_strPassword;     // 密码
    std::wstring m_strNFCCardID;    // NFC卡ID
    
    // 辅助函数
    HRESULT _GetStringValueInternal(DWORD dwFieldID, PWSTR *ppsz);
    HRESULT _TryNFCLogin();
    bool _ValidateCredentials();
};