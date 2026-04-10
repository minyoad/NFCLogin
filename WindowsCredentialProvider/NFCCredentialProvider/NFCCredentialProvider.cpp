#include "NFCCredentialProvider.h"
#include "NFCCredentialProviderCredential.h"
#include <windows.h>
#include <credentialprovider.h>
#include <shlguid.h>
#include <strsafe.h>
#include <wincred.h>
#include <iostream>
#include <fstream>
#include <sstream>

// 全局变量
long g_cRef = 0;
HINSTANCE g_hinst = NULL;

// 日志记录函数
void LogMessage(const std::string& message) {
    std::ofstream log("C:\\NFCLogin\\logs\\credential_provider.log", std::ios::app);
    if (log.is_open()) {
        SYSTEMTIME st;
        GetLocalTime(&st);
        log << "[" << st.wYear << "-" << st.wMonth << "-" << st.wDay << " " 
            << st.wHour << ":" << st.wMinute << ":" << st.wSecond << "] " 
            << message << std::endl;
        log.close();
    }
}

// DLL入口点
BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpReserved) {
    switch (fdwReason) {
    case DLL_PROCESS_ATTACH:
        g_hinst = hinstDLL;
        DisableThreadLibraryCalls(hinstDLL);
        LogMessage("NFC Credential Provider DLL loaded");
        break;
    case DLL_PROCESS_DETACH:
        LogMessage("NFC Credential Provider DLL unloaded");
        break;
    }
    return TRUE;
}

// 类工厂
class CProviderFactory : public IClassFactory {
public:
    // IUnknown
    STDMETHODIMP QueryInterface(REFIID riid, void **ppv) {
        static const QITAB qit[] = {
            QITABENT(CProviderFactory, IClassFactory),
            {0}
        };
        return QISearch(this, qit, riid, ppv);
    }

    STDMETHODIMP_(ULONG) AddRef() {
        return InterlockedIncrement(&g_cRef);
    }

    STDMETHODIMP_(ULONG) Release() {
        ULONG cRef = InterlockedDecrement(&g_cRef);
        if (!cRef) {
            delete this;
        }
        return cRef;
    }

    // IClassFactory
    STDMETHODIMP CreateInstance(IUnknown *pUnkOuter, REFIID riid, void **ppv) {
        HRESULT hr;
        if (pUnkOuter != NULL) {
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

    STDMETHODIMP LockServer(BOOL fLock) {
        if (fLock) {
            InterlockedIncrement(&g_cRef);
        } else {
            InterlockedDecrement(&g_cRef);
        }
        return S_OK;
    }
};

// DLL导出函数
STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, void **ppv) {
    HRESULT hr = CLASS_E_CLASSNOTAVAILABLE;
    if (IsEqualCLSID(rclsid, CLSID_NFCCredentialProvider)) {
        CProviderFactory *pFactory = new CProviderFactory();
        if (pFactory) {
            hr = pFactory->QueryInterface(riid, ppv);
            pFactory->Release();
        } else {
            hr = E_OUTOFMEMORY;
        }
    }
    return hr;
}

STDAPI DllCanUnloadNow() {
    return g_cRef ? S_FALSE : S_OK;
}

// 注册函数
STDAPI DllRegisterServer() {
    HRESULT hr = S_OK;
    
    // 创建注册表项
    HKEY hKey;
    DWORD dwData;
    WCHAR szModule[MAX_PATH];
    
    if (SUCCEEDED(hr)) {
        hr = StringCchPrintfW(szModule, ARRAYSIZE(szModule), L"%s\%s", L"Software\\Classes\\CLSID", 
                              SZ_CLSID_NFCCredentialProvider);
    }
    
    if (SUCCEEDED(hr)) {
        hr = HRESULT_FROM_WIN32(RegCreateKeyExW(HKEY_LOCAL_MACHINE, szModule, 0, NULL, 
                                                 REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hKey, &dwData));
    }
    
    if (SUCCEEDED(hr)) {
        hr = HRESULT_FROM_WIN32(RegSetValueExW(hKey, NULL, 0, REG_SZ, 
                                                (LPBYTE)L"NFC Credential Provider", 
                                                sizeof(L"NFC Credential Provider")));
        RegCloseKey(hKey);
    }
    
    // 注册为凭证提供程序
    if (SUCCEEDED(hr)) {
        hr = HRESULT_FROM_WIN32(RegCreateKeyExW(HKEY_LOCAL_MACHINE, 
                                                 L"Software\\Microsoft\\Windows\\CurrentVersion\\Authentication\\Credential Providers\\{7A8A8F2E-4C3D-4F1B-9E2A-3C4D5F6A7B8C}", 
                                                 0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hKey, &dwData));
    }
    
    if (SUCCEEDED(hr)) {
        dwData = 1;
        hr = HRESULT_FROM_WIN32(RegSetValueExW(hKey, L"Disabled", 0, REG_DWORD, 
                                                (LPBYTE)&dwData, sizeof(dwData)));
        RegCloseKey(hKey);
    }
    
    LogMessage("Credential Provider registered");
    return hr;
}

// 卸载函数
STDAPI DllUnregisterServer() {
    HRESULT hr = S_OK;
    
    // 删除注册表项
    WCHAR szModule[MAX_PATH];
    StringCchPrintfW(szModule, ARRAYSIZE(szModule), L"%s\\%s", L"Software\\Classes\\CLSID", 
                      SZ_CLSID_NFCCredentialProvider);
    
    RegDeleteTreeW(HKEY_LOCAL_MACHINE, szModule);
    RegDeleteTreeW(HKEY_LOCAL_MACHINE, 
                    L"Software\\Microsoft\\Windows\\CurrentVersion\\Authentication\\Credential Providers\\{7A8A8F2E-4C3D-4F1B-9E2A-3C4D5F6A7B8C}");
    
    LogMessage("Credential Provider unregistered");
    return hr;
}