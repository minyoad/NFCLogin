#pragma once

#include <windows.h>
#include <credentialprovider.h>
#include <shlwapi.h>
#include <security.h>
#include <ntsecapi.h>
#include <vector>
#include "AccountManager.h"
#include "NFCManager.h"

// 前向声明
class AccountManager;

class NFCCredentialProviderCredential : public ICredentialProviderCredential2 {
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
    NFCManager *m_pNFCManager;          // NFC管理器
    
    // 字段值
    std::wstring m_strUsername;     // 用户名
    std::wstring m_strPassword;     // 密码
    std::wstring m_strNFCCardID;    // NFC卡ID
    
    // 辅助函数
    HRESULT _GetStringValueInternal(DWORD dwFieldID, PWSTR *ppsz);
    HRESULT _TryNFCLogin();
    bool _ValidateCredentials();
    std::string ReadNFCCardUID();
};