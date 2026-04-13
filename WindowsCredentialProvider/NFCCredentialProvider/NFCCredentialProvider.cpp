#include "NFCCredentialProvider.h"
#include "NFCCredentialProviderCredential.h"
#include <credentialprovider.h>
#include <shlwapi.h>
#include <strsafe.h>
#include <iostream>
#include <stdarg.h>
#include <combaseapi.h>

#define SZ_CLSID_NFCCredentialProvider L"{7A8A8F2E-4C3D-4F1B-9E2A-3C4D5F6A7B8C}"

// {7A8A8F2E-4C3D-4F1B-9E2A-3C4D5F6A7B8C}
const GUID CLSID_NFCCredentialProvider =
{ 0x7a8a8f2e, 0x4c3d, 0x4f1b, { 0x9e, 0x2a, 0x3c, 0x4d, 0x5f, 0x6a, 0x7b, 0x8c } };


#pragma comment(lib, "shlwapi.lib")
#pragma comment(lib, "credui.lib")
#pragma comment(lib, "advapi32.lib")

// 全局实例计数
static LONG g_cRefModule = 0;
static HINSTANCE g_hinst = nullptr;

// 类工厂
class CClassFactory : public IClassFactory {
public:
    // IUnknown
    IFACEMETHODIMP QueryInterface(REFIID riid, void **ppv) {
        LogMessage("CClassFactory::QueryInterface asking for %s", GuidToString(riid));
        static const QITAB qit[] = {
            QITABENT(CClassFactory, IClassFactory),
            {0}
        };
        HRESULT hr = QISearch(this, qit, riid, ppv);
        if (SUCCEEDED(hr)) {
            LogMessage("CClassFactory::QueryInterface succeeded");
        } else {
            LogMessage("CClassFactory::QueryInterface failed with hr=0x%X", hr);
        }
        return hr;
    }

    IFACEMETHODIMP_(ULONG) AddRef() {
        ULONG cRef = InterlockedIncrement(&m_cRef);
        LogMessage("CClassFactory::AddRef, m_cRef=%d", cRef);
        return cRef;
    }

    IFACEMETHODIMP_(ULONG) Release() {
        ULONG cRef = InterlockedDecrement(&m_cRef);
        LogMessage("CClassFactory::Release, m_cRef=%d", cRef);
        if (!cRef) {
            delete this;
            LogMessage("CClassFactory destroyed");
        }
        return cRef;
    }

    // IClassFactory
    IFACEMETHODIMP CreateInstance(IUnknown *punkOuter, REFIID riid, void **ppv) {
        HRESULT hr;
        if (punkOuter) {
            hr = CLASS_E_NOAGGREGATION;
        } else {
            NFCCredentialProvider *pProvider = new NFCCredentialProvider();
            if (pProvider) {
                hr = pProvider->QueryInterface(riid, ppv);
                pProvider->Release();
            } else {
                hr = E_OUTOFMEMORY;
            }
        }
        return hr;
    }

    IFACEMETHODIMP LockServer(BOOL fLock) {
        if (fLock) {
            InterlockedIncrement(&g_cRefModule);
        } else {
            InterlockedDecrement(&g_cRefModule);
        }
        return S_OK;
    }

    CClassFactory() : m_cRef(1) {
        InterlockedIncrement(&g_cRefModule);
    }

private:
    LONG m_cRef;
};

// NFCCredentialProvider实现

NFCCredentialProvider::NFCCredentialProvider() : 
    m_cRef(1), m_cpus(CPUS_LOGON), m_rgcpfd(nullptr), m_dwFieldCount(0), 
    m_rgpcpc(nullptr), m_dwCredentialCount(0), m_pcpe(nullptr), m_upAdviseContext(0) {
    InterlockedIncrement(&g_cRefModule);
    LogMessage("Provider created, constructor successful");
}

NFCCredentialProvider::~NFCCredentialProvider() {
    if (m_rgpcpc) {
        for (DWORD i = 0; i < m_dwCredentialCount; i++) {
            if (m_rgpcpc[i]) {
                m_rgpcpc[i]->Release();
            }
        }
        CoTaskMemFree(m_rgpcpc);
    }
    
    if (m_pcpe) {
        m_pcpe->Release();
        m_pcpe = nullptr;
    }
    
    if (m_rgcpfd) {
        for (DWORD i = 0; i < m_dwFieldCount; i++) {
            if (m_rgcpfd[i].pszLabel) {
                CoTaskMemFree(m_rgcpfd[i].pszLabel);
            }
        }
        CoTaskMemFree(m_rgcpfd);
    }
    
    InterlockedDecrement(&g_cRefModule);
    LogMessage("Provider destroyed");
}

// IUnknown实现
IFACEMETHODIMP NFCCredentialProvider::QueryInterface(REFIID riid, void **ppv) {
    LogMessage("NFCCredentialProvider::QueryInterface asking for %s", GuidToString(riid));
    static const QITAB qit[] = {
        QITABENT(NFCCredentialProvider, ICredentialProvider),
        {0}
    };
    HRESULT hr = QISearch(this, qit, riid, ppv);
    if (SUCCEEDED(hr)) {
        LogMessage("NFCCredentialProvider::QueryInterface succeeded");
    } else {
        LogMessage("NFCCredentialProvider::QueryInterface failed with hr=0x%X", hr);
    }
    return hr;
}

IFACEMETHODIMP_(ULONG) NFCCredentialProvider::AddRef() {
    ULONG cRef = InterlockedIncrement(&m_cRef);
    LogMessage("NFCCredentialProvider::AddRef, m_cRef=%d", cRef);
    return cRef;
}

IFACEMETHODIMP_(ULONG) NFCCredentialProvider::Release() {
    ULONG cRef = InterlockedDecrement(&m_cRef);
    LogMessage("NFCCredentialProvider::Release, m_cRef=%d", cRef);
    if (cRef == 0) {
        delete this;
    }
    return cRef;
}

// ICredentialProvider实现
IFACEMETHODIMP NFCCredentialProvider::SetUsageScenario(CREDENTIAL_PROVIDER_USAGE_SCENARIO cpus, DWORD dwFlags) {
    m_cpus = cpus;
    LogMessage("SetUsageScenario called, scenario set");
    return S_OK;
}

IFACEMETHODIMP NFCCredentialProvider::SetSerialization(const CREDENTIAL_PROVIDER_CREDENTIAL_SERIALIZATION *pcpcs) {
    LogMessage("SetSerialization called");
    return E_NOTIMPL;
}

IFACEMETHODIMP NFCCredentialProvider::Advise(ICredentialProviderEvents *pcpe, UINT_PTR upAdviseContext) {
    LogMessage("Advise called");
    if (m_pcpe) {
        m_pcpe->Release();
    }
    
    m_pcpe = pcpe;
    if (m_pcpe) {
        m_pcpe->AddRef();
    }
    
    m_upAdviseContext = upAdviseContext;
    return S_OK;
}

IFACEMETHODIMP NFCCredentialProvider::UnAdvise() {
    LogMessage("UnAdvise called");
    if (m_pcpe) {
        m_pcpe->Release();
        m_pcpe = nullptr;
    }
    
    m_upAdviseContext = 0;
    return S_OK;
}

IFACEMETHODIMP NFCCredentialProvider::GetCredentialCount(DWORD *pdwCount, DWORD *pdwDefault, BOOL *pbAutoLogonWithDefault) {
    LogMessage("GetCredentialCount called");
    if (pdwCount) {
        *pdwCount = 1; // 提供一个凭据
    }
    
    if (pdwDefault) {
        *pdwDefault = 0; // 默认凭据索引
    }
    
    if (pbAutoLogonWithDefault) {
        *pbAutoLogonWithDefault = FALSE; // 不自动登录
    }
    
    return S_OK;
}

IFACEMETHODIMP NFCCredentialProvider::GetCredentialAt(DWORD dwIndex, ICredentialProviderCredential **ppcpc) {
    LogMessage("GetCredentialAt called with index %d", dwIndex);
    if (dwIndex != 0 || !ppcpc) {
        return E_INVALIDARG;
    }
    
    NFCCredentialProviderCredential *pCredential = new NFCCredentialProviderCredential();
    if (!pCredential) {
        return E_OUTOFMEMORY;
    }
    
    HRESULT hr = pCredential->QueryInterface(IID_PPV_ARGS(ppcpc));
    pCredential->Release();
    
    return hr;
}

IFACEMETHODIMP NFCCredentialProvider::GetFieldDescriptorCount(DWORD *pdwCount) {
    LogMessage("GetFieldDescriptorCount called");
    if (pdwCount) {
        *pdwCount = 6; // 6个字段
    }
    return S_OK;
}

IFACEMETHODIMP NFCCredentialProvider::GetFieldDescriptorAt(DWORD dwIndex, CREDENTIAL_PROVIDER_FIELD_DESCRIPTOR **ppcpfd) {
    LogMessage("GetFieldDescriptorAt called with index %d", dwIndex);
    if (!ppcpfd || dwIndex >= 6) {
        return E_INVALIDARG;
    }
    
    *ppcpfd = (CREDENTIAL_PROVIDER_FIELD_DESCRIPTOR*)CoTaskMemAlloc(sizeof(CREDENTIAL_PROVIDER_FIELD_DESCRIPTOR));
    if (!*ppcpfd) {
        return E_OUTOFMEMORY;
    }
    
    ZeroMemory(*ppcpfd, sizeof(CREDENTIAL_PROVIDER_FIELD_DESCRIPTOR));
    
    switch (dwIndex) {
    case 0: // 图标
        (*ppcpfd)->dwFieldID = 0;
        (*ppcpfd)->cpft = CPFT_TILE_IMAGE;
        (*ppcpfd)->pszLabel = (PWSTR)CoTaskMemAlloc(256 * sizeof(WCHAR));
        StringCchCopyW((*ppcpfd)->pszLabel, 256, L"TileImage");
        break;
    case 1: // 用户名
        (*ppcpfd)->dwFieldID = 1;
        (*ppcpfd)->cpft = CPFT_EDIT_TEXT;
        (*ppcpfd)->pszLabel = (PWSTR)CoTaskMemAlloc(256 * sizeof(WCHAR));
        StringCchCopyW((*ppcpfd)->pszLabel, 256, L"Username");
        break;
    case 2: // 密码
        (*ppcpfd)->dwFieldID = 2;
        (*ppcpfd)->cpft = CPFT_PASSWORD_TEXT;
        (*ppcpfd)->pszLabel = (PWSTR)CoTaskMemAlloc(256 * sizeof(WCHAR));
        StringCchCopyW((*ppcpfd)->pszLabel, 256, L"Password");
        break;
    case 3: // NFC标签
        (*ppcpfd)->dwFieldID = 3;
        (*ppcpfd)->cpft = CPFT_SMALL_TEXT;
        (*ppcpfd)->pszLabel = (PWSTR)CoTaskMemAlloc(256 * sizeof(WCHAR));
        StringCchCopyW((*ppcpfd)->pszLabel, 256, L"NFC Card");
        break;
    case 4: // NFC状态
        (*ppcpfd)->dwFieldID = 4;
        (*ppcpfd)->cpft = CPFT_LARGE_TEXT;
        (*ppcpfd)->pszLabel = (PWSTR)CoTaskMemAlloc(256 * sizeof(WCHAR));
        StringCchCopyW((*ppcpfd)->pszLabel, 256, L"Waiting for NFC card...");
        break;
    case 5: // 提交按钮
        (*ppcpfd)->dwFieldID = 5;
        (*ppcpfd)->cpft = CPFT_SUBMIT_BUTTON;
        (*ppcpfd)->pszLabel = (PWSTR)CoTaskMemAlloc(256 * sizeof(WCHAR));
        StringCchCopyW((*ppcpfd)->pszLabel, 256, L"Log On");
        break;
    }
    
    (*ppcpfd)->guidFieldType = GUID_NULL;
    (*ppcpfd)->dwFieldID = dwIndex;
    
    return S_OK;
}

#include <stdio.h>

#define PROVIDER_VERSION "1.0.2" // 版本号提升

// 将日志消息写入 C:\temp\nfc_provider.log
void LogMessage(const char* message);

// DLL导出函数
STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, void **ppv) {
    LogMessage("DllGetClassObject called, attempting to create class factory");
    *ppv = nullptr;
    
    HRESULT hr = E_FAIL;
    if (IsEqualCLSID(rclsid, CLSID_NFCCredentialProvider)) {
        CClassFactory *pClassFactory = new CClassFactory();
        if (pClassFactory) {
            hr = pClassFactory->QueryInterface(riid, ppv);
            pClassFactory->Release();
            LogMessage("Class factory created successfully");
        } else {
            hr = E_OUTOFMEMORY;
            LogMessage("Failed to create class factory, out of memory");
        }
    } else {
        LogMessage("CLSID does not match NFCCredentialProvider");
    }
    
    return hr;
}

STDAPI DllCanUnloadNow() {
    return g_cRefModule > 0 ? S_FALSE : S_OK;
}

STDAPI DllRegisterServer() {
    HRESULT hr = S_OK;
    
    // 创建注册表项
    HKEY hKey = NULL;
    DWORD dwData;
    WCHAR szModule[MAX_PATH];
    
    if (SUCCEEDED(hr)) {
        hr = StringCchPrintfW(szModule, ARRAYSIZE(szModule), L"Software\\Classes\\CLSID\\%s", 
                              SZ_CLSID_NFCCredentialProvider);
    }
    
    if (SUCCEEDED(hr)) {
        hr = HRESULT_FROM_WIN32(RegCreateKeyExW(HKEY_LOCAL_MACHINE, szModule, 0, NULL, 
                                             REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hKey, &dwData));
    }
    
    if (SUCCEEDED(hr)) {
        hr = HRESULT_FROM_WIN32(RegSetValueExW(hKey, nullptr, 0, REG_SZ, 
                                             (LPBYTE)L"NFC Credential Provider", sizeof(L"NFC Credential Provider")));
        RegCloseKey(hKey);
    }
    
    // 注册为凭证提供程序
    if (SUCCEEDED(hr)) {
        hr = HRESULT_FROM_WIN32(RegCreateKeyExW(HKEY_LOCAL_MACHINE, 
                                             L"Software\\Microsoft\\Windows\\CurrentVersion\\Authentication\\Credential Providers\\" SZ_CLSID_NFCCredentialProvider, 
                                             0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hKey, &dwData));
    }
    
    if (SUCCEEDED(hr)) {
        hr = HRESULT_FROM_WIN32(RegSetValueExW(hKey, nullptr, 0, REG_SZ, 
                                             (LPBYTE)L"NFC Credential Provider", sizeof(L"NFC Credential Provider")));
        RegCloseKey(hKey);
    }
    
    // 注册DLL路径
    if (SUCCEEDED(hr)) {
        hr = HRESULT_FROM_WIN32(RegCreateKeyExW(HKEY_LOCAL_MACHINE, 
                                             L"Software\\Classes\\CLSID\\" SZ_CLSID_NFCCredentialProvider L"\\InprocServer32", 
                                             0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hKey, &dwData));
    }
    
    if (SUCCEEDED(hr)) {
        GetModuleFileNameW(g_hinst, szModule, ARRAYSIZE(szModule));
        hr = HRESULT_FROM_WIN32(RegSetValueExW(hKey, nullptr, 0, REG_SZ, 
                                             (LPBYTE)szModule, (lstrlenW(szModule) + 1) * sizeof(WCHAR)));
        RegCloseKey(hKey);
    }
    
    // 设置线程模型
    if (SUCCEEDED(hr)) {
        hr = HRESULT_FROM_WIN32(RegCreateKeyExW(HKEY_LOCAL_MACHINE, 
                                             L"Software\\Classes\\CLSID\\" SZ_CLSID_NFCCredentialProvider L"\\InprocServer32", 
                                             0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hKey, &dwData));
    }
    
    if (SUCCEEDED(hr)) {
        hr = HRESULT_FROM_WIN32(RegSetValueExW(hKey, L"ThreadingModel", 0, REG_SZ, 
                                             (LPBYTE)L"Apartment", sizeof(L"Apartment")));
        RegCloseKey(hKey);
    }
    
    LogMessage("Credential Provider registered");
    return hr;
}

const char* GuidToString(const GUID& guid) {
    static WCHAR wszGuid[40] = { 0 };
    static char szGuid[40] = { 0 };
    if (StringFromGUID2(guid, wszGuid, ARRAYSIZE(wszGuid))) {
        WideCharToMultiByte(CP_ACP, 0, wszGuid, -1, szGuid, sizeof(szGuid), NULL, NULL);
        return szGuid;
    }
    return "Invalid GUID";
}

void LogMessage(const char* format, ...) {
    FILE* log_file = nullptr;
    if (fopen_s(&log_file, "C:\\temp\\nfc_provider.log", "a") == 0 && log_file) {
        SYSTEMTIME st;
        GetLocalTime(&st);
        fprintf(log_file, "[%04d-%02d-%02d %02d:%02d:%02d.%03d] [v%s] ",
            st.wYear, st.wMonth, st.wDay,
            st.wHour, st.wMinute, st.wSecond, st.wMilliseconds,
            PROVIDER_VERSION);

        char buffer[1024];
        va_list args;
        va_start(args, format);
        vsprintf_s(buffer, sizeof(buffer), format, args);
        va_end(args);

        fprintf(log_file, "%s\n", buffer);
        fclose(log_file);
    }
}

STDAPI DllUnregisterServer() {
    HRESULT hr = S_OK;
    
    // 删除注册表项
    hr = HRESULT_FROM_WIN32(RegDeleteTreeW(HKEY_LOCAL_MACHINE, 
                                         L"Software\\Classes\\CLSID\\" SZ_CLSID_NFCCredentialProvider));
    
    hr = HRESULT_FROM_WIN32(RegDeleteTreeW(HKEY_LOCAL_MACHINE, 
                                         L"Software\\Microsoft\\Windows\\CurrentVersion\\Authentication\\Credential Providers\\" SZ_CLSID_NFCCredentialProvider));
    
    LogMessage("Credential Provider unregistered");
    return hr;
}

// DLL入口点
BOOL APIENTRY DllMain(HMODULE hModule, DWORD dwReason, LPVOID lpReserved) {
    switch (dwReason) {
    case DLL_PROCESS_ATTACH:
        g_hinst = hModule;
        DisableThreadLibraryCalls(hModule);
        LogMessage("DLL_PROCESS_ATTACH");
        break;
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
        break;
    case DLL_PROCESS_DETACH:
        LogMessage("DLL_PROCESS_DETACH");
        break;
    }
    return TRUE;
}