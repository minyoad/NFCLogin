#include "NFCCredentialProviderCredential.h"
#include "NFCCredentialProvider.h"
#include "AccountManager.h"
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
    m_pAccountManager(nullptr),
    m_pNFCManager(nullptr) {
    m_pNFCManager = new NFCManager();
    if (m_pNFCManager) {
        m_pNFCManager->Initialize();
    }
}

NFCCredentialProviderCredential::~NFCCredentialProviderCredential() {
    if (m_rgcpfd) {
        for (DWORD i = 0; i < m_dwFieldCount; i++) {
            CoTaskMemFree(m_rgcpfd[i].pszLabel);
        }
        CoTaskMemFree(m_rgcpfd);
    }
    
    if (m_pAccountManager) {
        delete m_pAccountManager;
        m_pAccountManager = nullptr;
    }

    if (m_pNFCManager) {
        delete m_pNFCManager;
        m_pNFCManager = nullptr;
    }
}

// IUnknown实现
IFACEMETHODIMP NFCCredentialProviderCredential::QueryInterface(REFIID riid, void **ppv) {
    LogMessage("NFCCredentialProviderCredential::QueryInterface asking for %s", GuidToString(riid));
    static const QITAB qit[] = {
        QITABENT(NFCCredentialProviderCredential, ICredentialProviderCredential),
        QITABENT(NFCCredentialProviderCredential, ICredentialProviderCredential2),
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
    
    // 创建账户管理器
    if (!m_pAccountManager) {
        m_pAccountManager = new AccountManager();
        if (m_pAccountManager) {
            hr = m_pAccountManager->Initialize();
            if (FAILED(hr)) {
                delete m_pAccountManager;
                m_pAccountManager = nullptr;
                return hr;
            }
        } else {
            return E_OUTOFMEMORY;
        }
    }
    
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
        *pbAutoLogon = FALSE;
    }
    
    // 尝试NFC登录
    _TryNFCLogin();
    
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
    HRESULT hr = E_INVALIDARG;
    LogMessage("GetStringValue called for field %d", dwFieldID);
    
    if (dwFieldID < m_dwFieldCount && ppsz) {
        switch (dwFieldID) {
            case FIELD_USERNAME:
                hr = SHStrDupW(m_strUsername.c_str(), ppsz);
                break;
            case FIELD_NFC_STATUS:
                {
                    std::wstring nfcStatus = L"NFC状态: ";
                    std::string uid = ReadNFCCardUID();
                    LogMessage("ReadNFCCardUID returned: %s", uid.c_str());
                    if (!uid.empty()) {
                        std::wstring uidW(uid.begin(), uid.end());
                        nfcStatus += L"检测到卡片 - " + uidW;
                    } else {
                        nfcStatus += L"等待刷卡...";
                    }
                    hr = SHStrDupW(nfcStatus.c_str(), ppsz);
                }
                break;
            default:
                hr = _GetStringValueInternal(dwFieldID, ppsz);
                break;
        }
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

    if (_ValidateCredentials()) {
        KERB_INTERACTIVE_LOGON kil = {0};
        kil.MessageType = KerbInteractiveLogon;

        kil.UserName.Buffer = (PWSTR)CoTaskMemAlloc((m_strUsername.length() + 1) * sizeof(WCHAR));
        if (kil.UserName.Buffer) {
            StringCchCopyW(kil.UserName.Buffer, m_strUsername.length() + 1, m_strUsername.c_str());
            kil.UserName.Length = (USHORT)(m_strUsername.length() * sizeof(WCHAR));
            kil.UserName.MaximumLength = (USHORT)((m_strUsername.length() + 1) * sizeof(WCHAR));
        }

        kil.Password.Buffer = (PWSTR)CoTaskMemAlloc((m_strPassword.length() + 1) * sizeof(WCHAR));
        if (kil.Password.Buffer) {
            StringCchCopyW(kil.Password.Buffer, m_strPassword.length() + 1, m_strPassword.c_str());
            kil.Password.Length = (USHORT)(m_strPassword.length() * sizeof(WCHAR));
            kil.Password.MaximumLength = (USHORT)((m_strPassword.length() + 1) * sizeof(WCHAR));
        }

        kil.LogonDomainName.Buffer = nullptr;
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
        if (status == 0) {
            status = LsaLookupAuthenticationPackage(hLsa, &lsaszAuthPackage, &ulAuthPackage);
            LsaDeregisterLogonProcess(hLsa);
        }

        if (status == 0) {
            pcpcs->ulAuthenticationPackage = ulAuthPackage;
            pcpcs->clsidCredentialProvider = CLSID_NFCCredentialProvider;

            ULONG ulSize = sizeof(KERB_INTERACTIVE_LOGON) +
                          kil.UserName.Length + sizeof(WCHAR) +
                          kil.Password.Length + sizeof(WCHAR);

            pcpcs->rgbSerialization = (BYTE*)CoTaskMemAlloc(ulSize);
            if (pcpcs->rgbSerialization) {
                BYTE* pbBuffer = pcpcs->rgbSerialization;

                CopyMemory(pbBuffer, &kil, sizeof(KERB_INTERACTIVE_LOGON));
                pbBuffer += sizeof(KERB_INTERACTIVE_LOGON);

                if (kil.UserName.Buffer) {
                    CopyMemory(pbBuffer, kil.UserName.Buffer, kil.UserName.Length + sizeof(WCHAR));
                    CoTaskMemFree(kil.UserName.Buffer);
                }

                if (kil.Password.Buffer) {
                    CopyMemory(pbBuffer, kil.Password.Buffer, kil.Password.Length + sizeof(WCHAR));
                    CoTaskMemFree(kil.Password.Buffer);
                }

                pcpcs->cbSerialization = ulSize;
                *pcpgsr = CPGSR_RETURN_CREDENTIAL_FINISHED;
            } else {
                *pcpgsr = CPGSR_NO_CREDENTIAL_FINISHED;
            }
        } else {
            if (kil.UserName.Buffer) CoTaskMemFree(kil.UserName.Buffer);
            if (kil.Password.Buffer) CoTaskMemFree(kil.Password.Buffer);
            *pcpgsr = CPGSR_NO_CREDENTIAL_FINISHED;
        }
    } else {
        SHStrDupW(L"Invalid username or password", ppszOptionalStatusText);
        *pcpsiOptionalStatusIcon = CPSI_ERROR;
        *pcpgsr = CPGSR_NO_CREDENTIAL_FINISHED;
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

// Helper function implementation
HRESULT NFCCredentialProviderCredential::_GetStringValueInternal(DWORD dwFieldID, PWSTR *ppsz) {
    HRESULT hr = E_INVALIDARG;

    if (dwFieldID < m_dwFieldCount && ppsz) {
        switch (dwFieldID) {
            case FIELD_USERNAME:
                hr = SHStrDupW(L"Username", ppsz);
                break;
            case FIELD_PASSWORD:
                hr = SHStrDupW(L"Password", ppsz);
                break;
            case FIELD_SUBMIT_BUTTON:
                hr = SHStrDupW(L"Login", ppsz);
                break;
            default:
                hr = SHStrDupW(L"", ppsz);
                break;
        }
    }

    return hr;
}

HRESULT NFCCredentialProviderCredential::_TryNFCLogin() {
    LogMessage("_TryNFCLogin called");
    std::string uid = ReadNFCCardUID();
    if (!uid.empty()) {
        // 查找UID对应的用户
        std::wstring username;
        HRESULT hr = m_pAccountManager->FindUserByNFCCardUID(uid, username);
        if (SUCCEEDED(hr) && !username.empty()) {
            m_strUsername = username;
            
            // 更新UI
            if (m_pcpce) {
                m_pcpce->SetFieldString(this, FIELD_USERNAME, m_strUsername.c_str());
            }
            
            LogMessage("NFC login successful for user: " + std::string(username.begin(), username.end()));
        } else {
            LogMessage("No user found for NFC card: " + uid);
        }
    }
    
    return S_OK;
}

std::string NFCCredentialProviderCredential::ReadNFCCardUID() {
    LogMessage("ReadNFCCardUID called");
    std::string uid;
    if (m_pNFCManager) {
        HRESULT hr = m_pNFCManager->ReadCardUID(uid);
        if (FAILED(hr)) {
            LogMessage("Failed to read NFC card UID, hr=0x%X", hr);
        }
    }
    return uid;
}

bool NFCCredentialProviderCredential::_ValidateCredentials() {
    if (m_strUsername.empty() || m_strPassword.empty()) {
        return false;
    }
    
    // 验证用户凭据
    HRESULT hr = m_pAccountManager->ValidateLocalUserCredentials(m_strUsername, m_strPassword);
    return SUCCEEDED(hr);
}

#pragma comment(lib, "credui.lib")
#pragma comment(lib, "advapi32.lib")
#pragma comment(lib, "shlwapi.lib")

// 全局辅助函数 - 已移动到AccountManager类中