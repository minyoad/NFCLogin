#include "NFCCredentialProviderCredential.h"
#include "NFCCredentialProvider.h"
#include <initguid.h>
#include <windows.h>
#include <strsafe.h>
#include <shlwapi.h>
#include <security.h>
#include <ntsecapi.h>
#include <shlobj.h>
#include <sddl.h>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <vector>

// Forward declarations for logging functions from the other file
void LogMessage(const char* format, ...);
const char* GuidToString(const GUID& guid);

#pragma comment(lib, "Secur32.lib")

// 字段ID定义
enum FIELDS {
    FIELD_TILE_IMAGE = 0,
    FIELD_USERNAME = 1,
    FIELD_PASSWORD = 2,
    FIELD_NFC_STATUS = 3,
    FIELD_SUBMIT_BUTTON = 4,
    FIELD_COUNT = 5
};

NFCCredentialProviderCredential::NFCCredentialProviderCredential() : 
    m_cRef(1), 
    m_cpus(CPUS_LOGON),
    m_rgcpfd(nullptr),
    m_dwFlags(0),
    m_dwFieldCount(FIELD_COUNT),
    m_pcpce(nullptr),
    m_fIsAutoLogon(false) {
}

NFCCredentialProviderCredential::~NFCCredentialProviderCredential() {
    if (m_rgcpfd) {
        for (DWORD i = 0; i < m_dwFieldCount; i++) {
            CoTaskMemFree(m_rgcpfd[i].pszLabel);
        }
        CoTaskMemFree(m_rgcpfd);
    }
}

// IUnknown实现
IFACEMETHODIMP NFCCredentialProviderCredential::QueryInterface(REFIID riid, void **ppv) {
    LogMessage("NFCCredentialProviderCredential::QueryInterface asking for %s", GuidToString(riid));
    static const QITAB qit[] = {
        QITABENT(NFCCredentialProviderCredential, ICredentialProviderCredential),
        QITABENT(NFCCredentialProviderCredential, ICredentialProviderCredential2),
        QITABENT(NFCCredentialProviderCredential, NFCCredentialProviderCredential),
        {0}
    };
    HRESULT hr = QISearch(this, qit, riid, ppv);
    if (SUCCEEDED(hr)) {
        LogMessage("NFCCredentialProviderCredential::QueryInterface succeeded");
    } else {
        LogMessage("NFCCredentialProviderCredential::QueryInterface failed with hr=0x%X", hr);
    }
    return hr;
}

IFACEMETHODIMP_(ULONG) NFCCredentialProviderCredential::AddRef() {
    ULONG cRef = InterlockedIncrement(&m_cRef);
    LogMessage("NFCCredentialProviderCredential::AddRef, m_cRef=%d", cRef);
    return cRef;
}

IFACEMETHODIMP_(ULONG) NFCCredentialProviderCredential::Release() {
    ULONG cRef = InterlockedDecrement(&m_cRef);
    LogMessage("NFCCredentialProviderCredential::Release, m_cRef=%d", cRef);
    if (cRef == 0) {
        delete this;
    }
    return cRef;
}

// 初始化函数
HRESULT NFCCredentialProviderCredential::Initialize(CREDENTIAL_PROVIDER_USAGE_SCENARIO cpus, 
                                                  const CREDENTIAL_PROVIDER_FIELD_DESCRIPTOR *rgcpfd, 
                                                  DWORD dwFlags) {
    HRESULT hr = S_OK;
    
    m_cpus = cpus;
    m_dwFlags = dwFlags;
    
    // 复制字段描述符
    m_rgcpfd = (CREDENTIAL_PROVIDER_FIELD_DESCRIPTOR*)CoTaskMemAlloc(sizeof(CREDENTIAL_PROVIDER_FIELD_DESCRIPTOR) * m_dwFieldCount);
    if (m_rgcpfd) {
        ZeroMemory(m_rgcpfd, sizeof(CREDENTIAL_PROVIDER_FIELD_DESCRIPTOR) * m_dwFieldCount);
        
        for (DWORD i = 0; i < m_dwFieldCount; i++) {
            m_rgcpfd[i] = rgcpfd[i];
            if (rgcpfd[i].pszLabel) {
                hr = SHStrDupW(rgcpfd[i].pszLabel, &m_rgcpfd[i].pszLabel);
                if (FAILED(hr)) {
                    break;
                }
            }
        }
    } else {
        hr = E_OUTOFMEMORY;
    }
    
    return hr;
}

HRESULT NFCCredentialProviderCredential::Initialize(
    NFCCredentialProvider* pProvider, 
    const std::wstring& username, 
    const std::string& uid, 
    CREDENTIAL_PROVIDER_USAGE_SCENARIO cpus)
{
    m_cpus = cpus;
    m_strUsername = username;
    m_fIsAutoLogon = !username.empty();
    // We don't need to store the provider or UID in this class for now,
    // as the auto-logon process is driven by the provider itself.
    return S_OK;
}

// ICredentialProviderCredential实现
IFACEMETHODIMP NFCCredentialProviderCredential::Advise(ICredentialProviderCredentialEvents *pcpce) {
    if (m_pcpce) {
        m_pcpce->Release();
    }
    m_pcpce = pcpce;
    if (m_pcpce) {
        m_pcpce->AddRef();
    }
    return S_OK;
}

IFACEMETHODIMP NFCCredentialProviderCredential::UnAdvise() {
    if (m_pcpce) {
        m_pcpce->Release();
        m_pcpce = nullptr;
    }
    return S_OK;
}

IFACEMETHODIMP NFCCredentialProviderCredential::SetSelected(BOOL *pbAutoLogon) {
    if (pbAutoLogon) {
        *pbAutoLogon = m_fIsAutoLogon;
    }
    
    return S_OK;
}

IFACEMETHODIMP NFCCredentialProviderCredential::SetDeselected() {
    return S_OK;
}

IFACEMETHODIMP NFCCredentialProviderCredential::GetFieldState(DWORD dwFieldID, 
                                                               CREDENTIAL_PROVIDER_FIELD_STATE *pcpfs, 
                                                               CREDENTIAL_PROVIDER_FIELD_INTERACTIVE_STATE *pcpfis) {
    HRESULT hr;
    
    if (dwFieldID < m_dwFieldCount && pcpfs && pcpfis) {
        *pcpfs = CPFS_DISPLAY_IN_BOTH;
        *pcpfis = CPFIS_NONE;
        
        switch (dwFieldID) {
            case FIELD_TILE_IMAGE:
                *pcpfs = CPFS_DISPLAY_IN_BOTH;
                break;
            case FIELD_USERNAME:
                *pcpfs = CPFS_DISPLAY_IN_SELECTED_TILE;
                *pcpfis = CPFIS_FOCUSED;
                break;
            case FIELD_PASSWORD:
                *pcpfs = CPFS_DISPLAY_IN_SELECTED_TILE;
                *pcpfis = CPFIS_NONE;
                break;
            case FIELD_NFC_STATUS:
                *pcpfs = CPFS_DISPLAY_IN_SELECTED_TILE;
                break;
            case FIELD_SUBMIT_BUTTON:
                *pcpfs = CPFS_DISPLAY_IN_SELECTED_TILE;
                break;
            default:
                hr = E_INVALIDARG;
                break;
        }
        
        if (dwFieldID < FIELD_COUNT) {
            hr = S_OK;
        }
    } else {
        hr = E_INVALIDARG;
    }
    
    return hr;
}

IFACEMETHODIMP NFCCredentialProviderCredential::GetStringValue(DWORD dwFieldID, PWSTR *ppsz) {
    HRESULT hr = E_UNEXPECTED;
    *ppsz = nullptr;

    switch (dwFieldID)
    {
    case FIELD_USERNAME:
        // Return the username. If it's empty, return a single space
        // to prevent the field from collapsing, which can cause UI glitches.
        hr = SHStrDupW(m_strUsername.empty() ? L" " : m_strUsername.c_str(), ppsz);
        break;
    case FIELD_NFC_STATUS:
        // This text is shown on the tile. It's updated by the background thread.
        // We provide a default value here.
        if (m_fIsAutoLogon) {
            hr = SHStrDupW(L"卡已识别，准备自动登录...", ppsz);
        } else {
            hr = SHStrDupW(L"请刷NFC卡登录...", ppsz);
        }
        break;
    default:
        // For other fields like the submit button, delegate to the internal helper.
        hr = _GetStringValueInternal(dwFieldID, ppsz);
        break;
    }

    if (FAILED(hr))
    {
        LogMessage("GetStringValue failed for field %d. HRESULT: 0x%X", dwFieldID, hr);
    }
    
    return hr;
}

IFACEMETHODIMP NFCCredentialProviderCredential::GetBitmapValue(DWORD dwFieldID, HBITMAP *phbmp) {
    HRESULT hr = E_INVALIDARG;
    
    if (dwFieldID == FIELD_TILE_IMAGE && phbmp) {
        HBITMAP hbmp = (HBITMAP)LoadImageW(GetModuleHandleW(L"NFCCredentialProvider.dll"), 
                                          MAKEINTRESOURCEW(101), IMAGE_BITMAP, 0, 0, 0);
        if (hbmp) {
            *phbmp = hbmp;
            hr = S_OK;
        } else {
            hr = E_FAIL;
        }
    }
    
    return hr;
}

IFACEMETHODIMP NFCCredentialProviderCredential::GetSubmitButtonValue(DWORD dwFieldID, DWORD *pdwAdjacentTo) {
    HRESULT hr = E_INVALIDARG;
    
    if (dwFieldID == FIELD_SUBMIT_BUTTON && pdwAdjacentTo) {
        *pdwAdjacentTo = FIELD_PASSWORD;
        hr = S_OK;
    }
    
    return hr;
}

IFACEMETHODIMP NFCCredentialProviderCredential::SetStringValue(DWORD dwFieldID, PCWSTR psz) {
    HRESULT hr = E_INVALIDARG;
    
    if (dwFieldID < m_dwFieldCount && psz) {
        switch (dwFieldID) {
            case FIELD_USERNAME:
                m_strUsername = psz;
                hr = S_OK;
                break;
            case FIELD_PASSWORD:
                m_strPassword = psz;
                hr = S_OK;
                break;
            default:
                break;
        }
    }
    
    return hr;
}

IFACEMETHODIMP NFCCredentialProviderCredential::GetSerialization(CREDENTIAL_PROVIDER_GET_SERIALIZATION_RESPONSE *pcpgsr,
                                                                CREDENTIAL_PROVIDER_CREDENTIAL_SERIALIZATION *pcpcs,
                                                                PWSTR *ppszOptionalStatusText, CREDENTIAL_PROVIDER_STATUS_ICON *pcpsiOptionalStatusIcon) {
    if (!pcpgsr || !pcpcs || !ppszOptionalStatusText || !pcpsiOptionalStatusIcon) {
        return E_INVALIDARG;
    }

    *pcpgsr = CPGSR_NO_CREDENTIAL_FINISHED;
    ZeroMemory(pcpcs, sizeof(*pcpcs));
    *ppszOptionalStatusText = nullptr;
    *pcpsiOptionalStatusIcon = CPSI_NONE;

    LogMessage("GetSerialization started.");

    // This function is called when the user clicks the login arrow.
    // At this point, the background thread should have already detected the card
    // and populated the user information. We just need to check if it was successful.
    if (!m_fIsAutoLogon || m_strUsername.empty()) {
        // If we don't have a valid user, we cannot proceed with login.
        // This might happen if the user clicks before a card is read.
        LogMessage("GetSerialization failed: Not in auto-logon state or username is empty.");
        SHStrDupW(L"无法自动登录。请重试。", ppszOptionalStatusText);
        *pcpsiOptionalStatusIcon = CPSI_ERROR;
        *pcpgsr = CPGSR_NO_CREDENTIAL_FINISHED;
        return S_OK; // Return S_OK to display the status message
    }

    // We have a valid user from the NFC card. Now, we proceed to package the credentials
    // for the Local Security Authority (LSA).
    // For this passwordless scenario, we use KerbInteractiveLogon with an empty password.
    m_strPassword = L"";

    KERB_INTERACTIVE_LOGON kil = {0};
    kil.MessageType = KerbInteractiveLogon;

    // Note: The string buffers for UserName, Domain, and Password must be part of the
    // serialized buffer, not just pointers to our local strings.
    kil.UserName.Length = (USHORT)(m_strUsername.length() * sizeof(WCHAR));
    kil.UserName.MaximumLength = kil.UserName.Length;
    kil.Password.Length = 0;
    kil.Password.MaximumLength = 0;
    kil.LogonDomainName.Length = 0;
    kil.LogonDomainName.MaximumLength = 0;

    ULONG ulAuthPackage;
    LSA_STRING lsaszAuthPackage;
    char authPackageName[] = "Kerberos";
    lsaszAuthPackage.Buffer = authPackageName;
    lsaszAuthPackage.Length = (USHORT)strlen(lsaszAuthPackage.Buffer);
    lsaszAuthPackage.MaximumLength = (USHORT)(lsaszAuthPackage.Length + 1);

    HANDLE hLsa;
    NTSTATUS status = LsaConnectUntrusted(&hLsa);
    if (SUCCEEDED(status)) {
        status = LsaLookupAuthenticationPackage(hLsa, &lsaszAuthPackage, &ulAuthPackage);
        LsaDeregisterLogonProcess(hLsa);
    }

    if (FAILED(status)) {
        LogMessage("GetSerialization: LsaLookupAuthenticationPackage failed with status 0x%X", status);
        return E_UNEXPECTED;
    }

    pcpcs->ulAuthenticationPackage = ulAuthPackage;
    pcpcs->clsidCredentialProvider = CLSID_NFCCredentialProvider;

    // Calculate the total size of the serialization buffer
    ULONG ulSize = sizeof(KERB_INTERACTIVE_LOGON) + kil.UserName.Length;
    pcpcs->rgbSerialization = (BYTE*)CoTaskMemAlloc(ulSize);
    if (pcpcs->rgbSerialization) {
        pcpcs->cbSerialization = ulSize;
        BYTE* pbCurrent = pcpcs->rgbSerialization;

        // Copy the KERB_INTERACTIVE_LOGON structure
        CopyMemory(pbCurrent, &kil, sizeof(KERB_INTERACTIVE_LOGON));
        pbCurrent += sizeof(KERB_INTERACTIVE_LOGON);

        // Copy the username string
        CopyMemory(pbCurrent, m_strUsername.c_str(), kil.UserName.Length);

        *pcpgsr = CPGSR_RETURN_CREDENTIAL_FINISHED;
        LogMessage("GetSerialization successful for user: %S", m_strUsername.c_str());
    } else {
        return E_OUTOFMEMORY;
    }

    return S_OK;
}

IFACEMETHODIMP NFCCredentialProviderCredential::GetComboBoxValueCount(DWORD dwFieldID, DWORD *pcItems, DWORD *pdwSelectedItem) {
    return E_NOTIMPL;
}

IFACEMETHODIMP NFCCredentialProviderCredential::GetComboBoxValueAt(DWORD dwFieldID, DWORD dwItem, PWSTR *ppszItem) {
    return E_NOTIMPL;
}

IFACEMETHODIMP NFCCredentialProviderCredential::GetCheckboxValue(DWORD dwFieldID, BOOL* pbChecked, PWSTR* ppszLabel)
{
    return E_NOTIMPL;
}

IFACEMETHODIMP NFCCredentialProviderCredential::SetCheckboxValue(DWORD dwFieldID, BOOL bChecked) {
    return E_NOTIMPL;
}

IFACEMETHODIMP NFCCredentialProviderCredential::SetComboBoxSelectedValue(DWORD dwFieldID, DWORD dwSelectedItem) {
    return E_NOTIMPL;
}

IFACEMETHODIMP NFCCredentialProviderCredential::CommandLinkClicked(DWORD dwFieldID) {
    return E_NOTIMPL;
}

IFACEMETHODIMP NFCCredentialProviderCredential::ReportResult(NTSTATUS ntsStatus, NTSTATUS ntsSubstatus, 
                                                            PWSTR *ppszOptionalStatusText, CREDENTIAL_PROVIDER_STATUS_ICON *pcpsiOptionalStatusIcon) {
    return E_NOTIMPL;
}

// ICredentialProviderCredential2实现
IFACEMETHODIMP NFCCredentialProviderCredential::GetUserSid(PWSTR *ppszSid) {
    if (m_strUsername.empty()) {
        return E_UNEXPECTED;
    }

    HRESULT hr = E_FAIL;
    PSID pSid = nullptr;
    DWORD cbSid = 0;
    DWORD cchDomain = 0;
    SID_NAME_USE eUse;

    // 第一次调用LookupAccountName获取SID和domain name的大小
    LookupAccountNameW(nullptr, m_strUsername.c_str(), nullptr, &cbSid, nullptr, &cchDomain, &eUse);

    if (cbSid > 0) {
        pSid = (PSID)CoTaskMemAlloc(cbSid);
        if (pSid) {
            PWSTR pszDomain = (PWSTR)CoTaskMemAlloc(cchDomain * sizeof(WCHAR));
            if (pszDomain) {
                if (LookupAccountNameW(nullptr, m_strUsername.c_str(), pSid, &cbSid, pszDomain, &cchDomain, &eUse)) {
                    if (ConvertSidToStringSidW(pSid, ppszSid)) {
                        hr = S_OK;
                    }
                }
                CoTaskMemFree(pszDomain);
            }
            CoTaskMemFree(pSid);
        } else {
            hr = E_OUTOFMEMORY;
        }
    }

    return hr;
}

HRESULT NFCCredentialProviderCredential::_GetStringValueInternal(DWORD dwFieldID, PWSTR* ppsz)
{
    // This is a helper for GetStringValue. For now, it does nothing.
    // We can expand it later if other fields need to return strings.
    return E_NOTIMPL;
}

// New functions to manage auto-logon state
void NFCCredentialProviderCredential::SetAutoLogonInfo(const std::wstring& username) {
    m_strUsername = username;
    m_fIsAutoLogon = true;
    LogMessage("SetAutoLogonInfo for user: %S", username.c_str());
}

void NFCCredentialProviderCredential::ClearAutoLogonInfo() {
    m_strUsername.clear();
    m_fIsAutoLogon = false;
    LogMessage("ClearAutoLogonInfo called.");
}

#pragma comment(lib, "credui.lib")
#pragma comment(lib, "advapi32.lib")
#pragma comment(lib, "shlwapi.lib")

HRESULT NFCCredentialProviderCredential::NFCCredentialProviderCredential_CreateInstance(REFIID riid, void** ppv)
{
    HRESULT hr;
    NFCCredentialProviderCredential* pCredential = new (std::nothrow) NFCCredentialProviderCredential();
    if (pCredential)
    {
        hr = pCredential->QueryInterface(riid, ppv);
        pCredential->Release();
    }
    else
    {
        hr = E_OUTOFMEMORY;
    }
    return hr;
}