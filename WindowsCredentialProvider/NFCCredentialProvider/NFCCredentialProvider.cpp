#include "NFCCredentialProvider.h"
#include "NFCCredentialProviderCredential.h"
#include "NFCManager.h"
#include "AccountManager.h"
#include <initguid.h>
#include <credentialprovider.h>
#include <shlwapi.h>
#include <strsafe.h>
#include <iostream>
#include <stdarg.h>
#include <combaseapi.h>

#define SZ_CLSID_NFCCredentialProvider L"{7A8A8F2E-4C3D-4F1B-9E2A-3C4D5F6A7B8C}"

// Forward declarations
void LogMessage(const char* format, ...);
const char* GuidToString(const GUID& guid);

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
    m_cRef(1), 
    m_pUserArray(nullptr), 
    m_pcpe(nullptr), 
    m_upAdviseContext(0), 
    m_cpus(CPUS_INVALID),
    m_hMonitorThread(nullptr),
    m_bStopMonitor(FALSE),
    m_bPendingUINotify(FALSE)
{
    InterlockedIncrement(&g_cRefModule);
    LogMessage("Provider created, constructor started");
    InitializeCriticalSection(&m_critsecCardState);
    InitializeCriticalSection(&m_critsecUINotify);
    m_nfcManager.Initialize();
    m_accountManager.Initialize();
    LogMessage("Provider created, constructor finished");
}

NFCCredentialProvider::~NFCCredentialProvider() {
    LogMessage("Provider destroying");
    if (m_pcpe) {
        m_pcpe->Release();
        m_pcpe = nullptr;
    }

    if (m_pUserArray) {
        m_pUserArray->Release();
        m_pUserArray = nullptr;
    }

    _StopMonitorThread(); // Ensure thread is stopped
    
    DeleteCriticalSection(&m_critsecCardState);
    DeleteCriticalSection(&m_critsecUINotify);
    InterlockedDecrement(&g_cRefModule);
    LogMessage("Provider destroyed");
}

// IUnknown实现
IFACEMETHODIMP NFCCredentialProvider::QueryInterface(REFIID riid, void **ppv) {
    LogMessage("NFCCredentialProvider::QueryInterface asking for %s", GuidToString(riid));
    static const QITAB qit[] = {
        QITABENT(NFCCredentialProvider, ICredentialProvider),
        QITABENT(NFCCredentialProvider, ICredentialProviderSetUserArray),
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

// Starts the NFC monitoring thread.
void NFCCredentialProvider::_StartMonitorThread()
{
    if (m_hMonitorThread == nullptr)
    {
        m_bStopMonitor = FALSE;
        m_hMonitorThread = CreateThread(nullptr, 0, _MonitorThreadProc, this, 0, nullptr);
        if (m_hMonitorThread == nullptr)
        {
            LogMessage("Error: Failed to create monitor thread.");
        }
        else
        {
            LogMessage("Monitor thread started successfully.");
        }
    }
}

// Stops the NFC monitoring thread.
void NFCCredentialProvider::_StopMonitorThread()
{
    if (m_hMonitorThread != nullptr)
    {
        LogMessage("Stopping monitor thread...");
        m_bStopMonitor = TRUE;
        if (WaitForSingleObject(m_hMonitorThread, 5000) == WAIT_TIMEOUT)
        {
            LogMessage("Warning: Monitor thread did not exit gracefully. Terminating.");
            TerminateThread(m_hMonitorThread, -1);
        }
        CloseHandle(m_hMonitorThread);
        m_hMonitorThread = nullptr;
        LogMessage("Monitor thread stopped.");
    }
}

// The background thread procedure that waits for an NFC card and triggers auto-logon.
DWORD WINAPI NFCCredentialProvider::_MonitorThreadProc(LPVOID lpParam)
{
    NFCCredentialProvider* pThis = static_cast<NFCCredentialProvider*>(lpParam);
    LogMessage("NFCCredentialProvider::_MonitorThreadProc started.");

    SCARD_READERSTATE readerState = { 0 };
    const std::wstring readerName = pThis->m_nfcManager.GetReaderName();
    readerState.szReader = readerName.c_str();
    readerState.dwCurrentState = SCARD_STATE_UNAWARE;

    std::string lastSeenUID = "";
    bool cardWasPresent = false;

    while (!pThis->m_bStopMonitor)
    {
        // This call will now block until the card state *changes* from the last known state.
        HRESULT hr = pThis->m_nfcManager.WaitForCard(&readerState, INFINITE);

        // After the call, update the current state to the event state for the next iteration.
        readerState.dwCurrentState = readerState.dwEventState;

        bool cardIsPresent = (readerState.dwEventState & SCARD_STATE_PRESENT) != 0;

        if (cardIsPresent && !cardWasPresent)
        {
            // Card was just inserted.
            LogMessage("_MonitorThreadProc: Card inserted.");
            std::string currentUID;
            if (SUCCEEDED(pThis->m_nfcManager.ReadCardUID(currentUID)) && !currentUID.empty())
            {
                LogMessage("_MonitorThreadProc: Successfully read UID: %s", currentUID.c_str());
                lastSeenUID = currentUID;

                std::wstring currentUsername;
                HRESULT hr = pThis->m_accountManager.FindUserByNFCCardUID(currentUID, currentUsername);
                if (SUCCEEDED(hr) && !currentUsername.empty())
                {
                    LogMessage("_MonitorThreadProc: Found user '%ls' for UID %s", currentUsername.c_str(), currentUID.c_str());
                }
                else
                {
                    LogMessage("_MonitorThreadProc: No user found for UID %s. HRESULT: 0x%X", currentUID.c_str(), hr);
                }

                EnterCriticalSection(&pThis->m_critsecCardState);
                pThis->m_sCardUID = currentUID;
                pThis->m_sCardUserName = currentUsername;
                LeaveCriticalSection(&pThis->m_critsecCardState);

                LogMessage("_MonitorThreadProc: Setting UI notification for new card.");
                EnterCriticalSection(&pThis->m_critsecUINotify);
                pThis->m_bPendingUINotify = TRUE;
                LeaveCriticalSection(&pThis->m_critsecUINotify);
            }
            else
            {
                LogMessage("_MonitorThreadProc: Failed to read UID after card insertion.");
            }
        }
        else if (!cardIsPresent && cardWasPresent)
        {
            // Card was just removed.
            LogMessage("_MonitorThreadProc: Card removed.");
            lastSeenUID = "";

            EnterCriticalSection(&pThis->m_critsecCardState);
            pThis->m_sCardUID = "";
            pThis->m_sCardUserName = L"";
            LeaveCriticalSection(&pThis->m_critsecCardState);

            LogMessage("_MonitorThreadProc: Setting UI notification for card removal.");
            EnterCriticalSection(&pThis->m_critsecUINotify);
            pThis->m_bPendingUINotify = TRUE;
            LeaveCriticalSection(&pThis->m_critsecUINotify);
        }

        cardWasPresent = cardIsPresent;

        // If the thread is stopping, break the loop.
        if (pThis->m_bStopMonitor)
        {
            break;
        }
    }

    LogMessage("NFCCredentialProvider::_MonitorThreadProc exiting.");
    return 0;
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
    _StartMonitorThread(); // Start monitoring for NFC cards
    return S_OK;
}

IFACEMETHODIMP NFCCredentialProvider::UnAdvise() {
    LogMessage("UnAdvise called");
    _StopMonitorThread(); // Stop monitoring
    if (m_pcpe) {
        m_pcpe->Release();
        m_pcpe = nullptr;
    }
    
    m_upAdviseContext = 0;
    return S_OK;
}

IFACEMETHODIMP NFCCredentialProvider::GetCredentialCount(DWORD* pdwCount, DWORD* pdwDefault, BOOL* pbAutoLogon)
{
    LogMessage("NFCCredentialProvider::GetCredentialCount");

    // Clear existing credentials
    for (auto& cred : m_rgpCredentials)
    {
        cred->Release();
    }
    m_rgpCredentials.clear();

    *pdwDefault = (DWORD)-1; // CREDENTIAL_PROVIDER_INVALID_DEFAULT_CREDENTIAL
    *pbAutoLogon = FALSE;

    const auto& userMap = m_accountManager.GetUserToNFCMap();
    
    EnterCriticalSection(&m_critsecCardState);
    std::wstring activeUsername = m_sCardUserName;
    LeaveCriticalSection(&m_critsecCardState);

    if (userMap.empty())
    {
        // No users in the database, create a default tile.
        NFCCredentialProviderCredential* pCredential = nullptr;
        if (SUCCEEDED(NFCCredentialProviderCredential::NFCCredentialProviderCredential_CreateInstance(IID_PPV_ARGS(&pCredential))))
        {
            pCredential->Initialize(this, L"", "", m_cpus);
            m_rgpCredentials.push_back(pCredential);
        }
    }
    else
    {
        // Create a tile for each user in the database.
        DWORD credIndex = 0;
        for (const auto& pair : userMap)
        {
            const std::wstring& username = pair.first;
            const std::string& uid = pair.second;

            NFCCredentialProviderCredential* pCredential = nullptr;
            if (SUCCEEDED(NFCCredentialProviderCredential::NFCCredentialProviderCredential_CreateInstance(IID_PPV_ARGS(&pCredential))))
            {
                pCredential->Initialize(this, username, uid, m_cpus);
                m_rgpCredentials.push_back(pCredential);

                // If this user is the one who just swiped the card, set them as the default for auto-logon.
                if (!activeUsername.empty() && username == activeUsername)
                {
                    *pdwDefault = credIndex;
                    *pbAutoLogon = TRUE;
                }
            }
            credIndex++;
        }
    }

    *pdwCount = (DWORD)m_rgpCredentials.size();
    LogMessage("GetCredentialCount: Returning %d credentials. Default index: %d. Auto-logon is %s.", *pdwCount, *pdwDefault, *pbAutoLogon ? "ENABLED" : "DISABLED");
    return S_OK;
}

HRESULT NFCCredentialProvider::_CreateCredential() {
    // 定义字段描述符
    static const CREDENTIAL_PROVIDER_FIELD_DESCRIPTOR rgcpfd[] = {
        {0, CPFT_TILE_IMAGE, const_cast<wchar_t*>(L"Icon")},
        {1, CPFT_EDIT_TEXT, const_cast<wchar_t*>(L"Username")},
        {2, CPFT_PASSWORD_TEXT, const_cast<wchar_t*>(L"Password")},
        {3, CPFT_SMALL_TEXT, const_cast<wchar_t*>(L"NFC Status")},
        {4, CPFT_SUBMIT_BUTTON, const_cast<wchar_t*>(L"Submit")}
    };

    m_dwFieldCount = ARRAYSIZE(rgcpfd);
    m_rgcpfd = (CREDENTIAL_PROVIDER_FIELD_DESCRIPTOR*)CoTaskMemAlloc(sizeof(rgcpfd));
    if (!m_rgcpfd) return E_OUTOFMEMORY;
    memcpy(m_rgcpfd, rgcpfd, sizeof(rgcpfd));

    // 创建凭据对象
    m_pCredential = new NFCCredentialProviderCredential();
    if (!m_pCredential) {
        CoTaskMemFree(m_rgcpfd);
        m_rgcpfd = nullptr;
        return E_OUTOFMEMORY;
    }
    m_pCredential->AddRef();
    
    HRESULT hr = m_pCredential->Initialize(m_cpus, m_rgcpfd, 0);
    if (FAILED(hr)) {
        LogMessage("Failed to initialize credential, hr=0x%X", hr);
        m_pCredential->Release();
        m_pCredential = nullptr;
        CoTaskMemFree(m_rgcpfd);
        m_rgcpfd = nullptr;
        return hr;
    }
    
    LogMessage("Credential and field descriptors created successfully");
    return S_OK;
}

IFACEMETHODIMP NFCCredentialProvider::GetCredentialAt(DWORD dwIndex, ICredentialProviderCredential **ppcpc) {
    LogMessage("GetCredentialAt called with index %d", dwIndex);
    if (dwIndex >= m_rgpCredentials.size() || !ppcpc) {
        return E_INVALIDARG;
    }
    
    HRESULT hr = m_rgpCredentials[dwIndex]->QueryInterface(IID_PPV_ARGS(ppcpc));
    return hr;
}

IFACEMETHODIMP NFCCredentialProvider::GetFieldDescriptorCount(DWORD *pdwCount) {
    *pdwCount = m_dwFieldCount;
    return S_OK;
}

IFACEMETHODIMP NFCCredentialProvider::GetFieldDescriptorAt(DWORD dwIndex, CREDENTIAL_PROVIDER_FIELD_DESCRIPTOR **ppcpfd) {
    if (dwIndex >= m_dwFieldCount || !ppcpfd) {
        return E_INVALIDARG;
    }
    
    *ppcpfd = (CREDENTIAL_PROVIDER_FIELD_DESCRIPTOR*)CoTaskMemAlloc(sizeof(CREDENTIAL_PROVIDER_FIELD_DESCRIPTOR));
    if (!*ppcpfd) {
        return E_OUTOFMEMORY;
    }

    (*ppcpfd)->dwFieldID = m_rgcpfd[dwIndex].dwFieldID;
    (*ppcpfd)->cpft = m_rgcpfd[dwIndex].cpft;
    (*ppcpfd)->guidFieldType = m_rgcpfd[dwIndex].guidFieldType;

    return SHStrDupW(m_rgcpfd[dwIndex].pszLabel, &(*ppcpfd)->pszLabel);
}

// ICredentialProviderSetUserArray实现
IFACEMETHODIMP NFCCredentialProvider::SetUserArray(ICredentialProviderUserArray *pUserArray) {
    LogMessage("SetUserArray called");
    if (m_pUserArray) {
        m_pUserArray->Release();
    }
    m_pUserArray = pUserArray;
    if (m_pUserArray) {
        m_pUserArray->AddRef();
    }
    return S_OK;
}

#include <stdio.h>

#define PROVIDER_VERSION "1.0.3" // 版本号提升

// 将日志消息写入 C:\temp\nfc_provider.log

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
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}