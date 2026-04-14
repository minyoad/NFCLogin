#include "NFCManager.h"
#include <windows.h>
#include <winscard.h>
#include <sstream>
#include <iomanip>
#include <thread>
#include <chrono>
#include <iostream>

#pragma comment(lib, "winscard.lib")

// Forward declarations for logging functions
void LogMessage(const char* format, ...);

// 辅助函数实现 - 移到文件前面
std::string ConvertToHex(const BYTE* data, size_t length) {
    std::stringstream ss;
    ss << std::hex << std::uppercase << std::setfill('0');
    
    for (size_t i = 0; i < length; i++) {
        ss << std::setw(2) << (int)data[i];
    }
    
    return ss.str();
}

std::vector<BYTE> ParseHexString(const std::string& hexString) {
    std::vector<BYTE> result;
    
    for (size_t i = 0; i < hexString.length(); i += 2) {
        std::string byteString = hexString.substr(i, 2);
        BYTE byte = (BYTE)strtol(byteString.c_str(), nullptr, 16);
        result.push_back(byte);
    }
    
    return result;
}

bool ValidateUIDFormat(const std::string& uid) {
    if (uid.empty() || uid.length() > 16) {
        return false;
    }
    
    // 检查是否为有效的十六进制字符串
    for (char c : uid) {
        if (!isxdigit(c)) {
            return false;
        }
    }
    
    return true;
}

std::wstring StringToWString(const std::string& str) {
    if (str.empty()) return std::wstring();
    
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), nullptr, 0);
    std::wstring wstrTo(size_needed, 0);
    MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &wstrTo[0], size_needed);
    return wstrTo;
}

std::string WStringToString(const std::wstring& wstr) {
    if (wstr.empty()) return std::string();
    
    int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), nullptr, 0, nullptr, nullptr);
    std::string strTo(size_needed, 0);
    WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), &strTo[0], size_needed, nullptr, nullptr);
    return strTo;
}

SYSTEMTIME GetCurrentSystemTime() {
    SYSTEMTIME st;
    GetLocalTime(&st);
    return st;
}

std::string GetCurrentTimestamp() {
    SYSTEMTIME st;
    GetLocalTime(&st);
    
    std::stringstream ss;
    ss << std::setfill('0')
       << std::setw(4) << st.wYear << "-"
       << std::setw(2) << st.wMonth << "-"
       << std::setw(2) << st.wDay << " "
       << std::setw(2) << st.wHour << ":"
       << std::setw(2) << st.wMinute << ":"
       << std::setw(2) << st.wSecond << "."
       << std::setw(3) << st.wMilliseconds;
    
    return ss.str();
}

std::string NFCManager::GetPCSCErrorString(LONG errorCode) {
    if (errorCode == SCARD_S_SUCCESS) return "操作成功";
    if (errorCode == SCARD_E_CANCELLED) return "操作被取消";
    if (errorCode == SCARD_E_CANT_DISPOSE) return "无法释放资源";
    if (errorCode == SCARD_E_INSUFFICIENT_BUFFER) return "缓冲区不足";
    if (errorCode == SCARD_E_INVALID_ATR) return "无效的ATR";
    if (errorCode == SCARD_E_INVALID_HANDLE) return "无效句柄";
    if (errorCode == SCARD_E_INVALID_PARAMETER) return "无效参数";
    if (errorCode == SCARD_E_INVALID_TARGET) return "无效目标";
    if (errorCode == SCARD_E_INVALID_VALUE) return "无效值";
    if (errorCode == SCARD_E_NO_MEMORY) return "内存不足";
    if (errorCode == SCARD_E_UNKNOWN_CARD) return "未知卡片";
    if (errorCode == SCARD_E_UNKNOWN_READER) return "未知读卡器";
    if (errorCode == SCARD_E_NO_SERVICE) return "PCSC服务未运行";
    if (errorCode == SCARD_E_NO_SMARTCARD) return "未检测到智能卡";
    if (errorCode == SCARD_E_NOT_READY) return "读卡器未就绪";
    if (errorCode == SCARD_E_NOT_TRANSACTED) return "交易未完成";
    if (errorCode == SCARD_E_PROTO_MISMATCH) return "协议不匹配";
    if (errorCode == SCARD_E_READER_UNAVAILABLE) return "读卡器不可用";
    if (errorCode == SCARD_E_SHARING_VIOLATION) return "共享冲突";
    if (errorCode == SCARD_E_TIMEOUT) return "操作超时";

    return "未知错误: " + std::to_string(errorCode);
}

// NFCManager类实现
NFCManager::NFCManager() : 
    m_hContext(NULL),
    m_hCard(NULL),
    m_dwActiveProtocol(SCARD_PROTOCOL_UNDEFINED),
    m_bInitialized(false),
    m_bDebugMode(false) {
    ZeroMemory(&m_lastCardInfo, sizeof(m_lastCardInfo));
}

NFCManager::~NFCManager() {
    CleanupPCSC();
}

HRESULT NFCManager::Initialize() {
    if (m_bInitialized) {
        return S_OK;
    }

    HRESULT hr = InitializePCSC();
    if (SUCCEEDED(hr)) {
        m_bInitialized = true;
        return S_OK;
    }

    return E_FAIL;
}

HRESULT NFCManager::InitializePCSC() {
    LogMessage("NFCManager: Initializing PCSC...");
    // 建立PCSC上下文
    LONG lReturn = SCardEstablishContext(SCARD_SCOPE_SYSTEM, NULL, NULL, &m_hContext);
    if (lReturn != SCARD_S_SUCCESS) {
        m_lastError = "无法建立PCSC上下文: " + GetPCSCErrorString(lReturn);
        LogMessage("NFCManager: SCardEstablishContext failed with error 0x%X: %s", lReturn, m_lastError.c_str());
        return E_FAIL;
    }
    LogMessage("NFCManager: SCardEstablishContext successful.");

    // 获取读卡器列表
    DWORD dwReaders = SCARD_AUTOALLOCATE;
    LPWSTR mszReaders = NULL;
    
    lReturn = SCardListReaders(m_hContext, NULL, (LPWSTR)&mszReaders, &dwReaders);
    if (lReturn != SCARD_S_SUCCESS) {
        m_lastError = "无法获取读卡器列表: " + GetPCSCErrorString(lReturn);
        LogMessage("NFCManager: SCardListReaders failed with error 0x%X: %s", lReturn, m_lastError.c_str());
        SCardReleaseContext(m_hContext);
        m_hContext = NULL;
        return E_FAIL;
    }
    LogMessage("NFCManager: SCardListReaders successful.");

    // 解析读卡器名称
    m_readers.clear();
    LPWSTR pReader = mszReaders;
    while (*pReader) {
        m_readers.push_back(pReader);
        pReader += wcslen(pReader) + 1;
    }

    if (m_readers.empty()) {
        m_lastError = "未找到可用的NFC读卡器";
        LogMessage("NFCManager: No NFC readers found.");
        SCardFreeMemory(m_hContext, mszReaders);
        SCardReleaseContext(m_hContext);
        m_hContext = NULL;
        return E_FAIL;
    }
    LogMessage("NFCManager: Found %d readers.", m_readers.size());

    // 使用第一个读卡器
    m_readerName = m_readers[0];
    LogMessage("NFCManager: Using reader: %S", m_readerName.c_str());
    SCardFreeMemory(m_hContext, mszReaders);

    return S_OK;
}

void NFCManager::CleanupPCSC() {
    if (m_hCard != NULL) {
        SCardDisconnect(m_hCard, SCARD_LEAVE_CARD);
        m_hCard = NULL;
    }

    if (m_hContext != NULL) {
        SCardReleaseContext(m_hContext);
        m_hContext = NULL;
    }

    m_bInitialized = false;
}

HRESULT NFCManager::ConnectToCard() {
    LogMessage("NFCManager: Attempting to connect to card...");
    if (!m_bInitialized) {
        LogMessage("NFCManager: Not initialized, calling Initialize().");
        HRESULT hr = Initialize();
        if (FAILED(hr)) {
            LogMessage("NFCManager: Initialize() failed with hr=0x%X", hr);
            return hr;
        }
    }

    if (m_readerName.empty()) {
        m_lastError = "未选择读卡器";
        LogMessage("NFCManager: No reader name specified.");
        return E_FAIL;
    }

    // 断开之前的连接
    if (m_hCard != NULL) {
        LogMessage("NFCManager: Disconnecting previous card handle.");
        SCardDisconnect(m_hCard, SCARD_LEAVE_CARD);
        m_hCard = NULL;
    }

    // 连接到卡片
    LogMessage("NFCManager: Calling SCardConnect for reader '%S'...", m_readerName.c_str());
    LONG lReturn = SCardConnect(
        m_hContext,
        m_readerName.c_str(),
        SCARD_SHARE_DIRECT, // Changed from SCARD_SHARE_SHARED
        SCARD_PROTOCOL_UNDEFINED, // Changed from SCARD_PROTOCOL_T0 | SCARD_PROTOCOL_T1
        &m_hCard,
        &m_dwActiveProtocol
    );

    if (lReturn != SCARD_S_SUCCESS) {
        m_lastError = "无法连接卡片: " + GetPCSCErrorString(lReturn);
        LogMessage("NFCManager: SCardConnect failed with error 0x%X: %s", lReturn, m_lastError.c_str());
        return E_FAIL;
    }

    LogMessage("NFCManager: SCardConnect successful. Active protocol: %d", m_dwActiveProtocol);
    return S_OK;
}

HRESULT NFCManager::DisconnectFromCard() {
    if (m_hCard != NULL) {
        LONG lReturn = SCardDisconnect(m_hCard, SCARD_LEAVE_CARD);
        if (lReturn != SCARD_S_SUCCESS) {
            m_lastError = "断开卡片连接失败: " + GetPCSCErrorString(lReturn);
            return E_FAIL;
        }
        m_hCard = NULL;
    }

    return S_OK;
}

HRESULT NFCManager::IsCardPresent(bool& cardPresent) {
    if (!m_bInitialized) {
        HRESULT hr = Initialize();
        if (FAILED(hr)) {
            return hr;
        }
    }

    if (m_readerName.empty()) {
        m_lastError = "未选择读卡器";
        return E_FAIL;
    }

    // 获取读卡器状态
    SCARD_READERSTATE rgReaderStates[1];
    rgReaderStates[0].szReader = m_readerName.c_str();
    rgReaderStates[0].dwCurrentState = SCARD_STATE_UNAWARE;

    LONG lReturn = SCardGetStatusChange(m_hContext, 0, rgReaderStates, 1);
    if (lReturn != SCARD_S_SUCCESS) {
        m_lastError = "无法获取读卡器状态: " + GetPCSCErrorString(lReturn);
        return E_FAIL;
    }

    // 检查是否有卡片
    cardPresent = (rgReaderStates[0].dwEventState & SCARD_STATE_PRESENT) != 0;
    
    return S_OK;
}

HRESULT NFCManager::ReadCardUID(std::string& uid) {
    LogMessage("NFCManager: Reading card UID...");
    if (m_hCard == NULL) {
        LogMessage("NFCManager: No card handle, attempting to connect...");
        HRESULT hr = ConnectToCard();
        if (FAILED(hr)) {
            LogMessage("NFCManager: ConnectToCard failed with hr=0x%X", hr);
            return hr;
        }
    }

    // 获取卡片状态
    SCARD_READERSTATE rgReaderStates[1];
    rgReaderStates[0].szReader = m_readerName.c_str();
    rgReaderStates[0].dwCurrentState = SCARD_STATE_UNAWARE;

    LogMessage("NFCManager: Calling SCardGetStatusChange...");
    LONG lReturn = SCardGetStatusChange(m_hContext, 0, rgReaderStates, 1);
    if (lReturn != SCARD_S_SUCCESS) {
        m_lastError = "无法获取读卡器状态: " + GetPCSCErrorString(lReturn);
        LogMessage("NFCManager: SCardGetStatusChange failed with error 0x%X: %s", lReturn, m_lastError.c_str());
        return E_FAIL;
    }
    LogMessage("NFCManager: SCardGetStatusChange successful.");

    // 获取ATR
    DWORD dwAtrLen = 32;
    BYTE pbAtr[32];
    DWORD dwState, dwProtocol;

    LogMessage("NFCManager: Calling SCardStatus...");
    lReturn = SCardStatus(m_hCard, NULL, NULL, &dwState, &dwProtocol, pbAtr, &dwAtrLen);
    if (lReturn != SCARD_S_SUCCESS) {
        m_lastError = "无法获取卡片状态: " + GetPCSCErrorString(lReturn);
        LogMessage("NFCManager: SCardStatus failed with error 0x%X: %s", lReturn, m_lastError.c_str());
        return E_FAIL;
    }
    LogMessage("NFCManager: SCardStatus successful. State: %d, Protocol: %d", dwState, dwProtocol);

    // 根据卡片类型发送相应的APDU命令
    // 这里实现MIFARE卡片的UID读取
    BYTE uidCommand[] = {0xFF, 0xCA, 0x00, 0x00, 0x00}; // Get Data Command
    BYTE response[256];
    DWORD responseLength = sizeof(response);

    const SCARD_IO_REQUEST* pci;
    if (dwProtocol == SCARD_PROTOCOL_T0) {
        pci = SCARD_PCI_T0;
    } else {
        pci = SCARD_PCI_T1;
    }

    LogMessage("NFCManager: Calling SCardTransmit to get UID...");
    lReturn = SCardTransmit(
        m_hCard,
        pci,
        uidCommand,
        sizeof(uidCommand),
        NULL,
        response,
        &responseLength
    );

    if (lReturn != SCARD_S_SUCCESS) {
        m_lastError = "APDU命令发送失败: " + GetPCSCErrorString(lReturn);
        LogMessage("NFCManager: SCardTransmit failed with error 0x%X: %s", lReturn, m_lastError.c_str());
        return E_FAIL;
    }
    LogMessage("NFCManager: SCardTransmit successful. Response length: %d", responseLength);

    // 检查响应
    if (responseLength < 2) {
        m_lastError = "无效的UID响应";
        LogMessage("NFCManager: Invalid UID response, length < 2.");
        return E_FAIL;
    }

    // 提取UID（排除最后2个字节的状态字）
    DWORD uidLength = responseLength - 2;
    if (response[uidLength] == 0x90 && response[uidLength + 1] == 0x00) {
        // 成功状态字
        uid = ConvertToHex(response, uidLength);
        LogMessage("NFCManager: Successfully read UID: %s", uid.c_str());
        return S_OK;
    } else {
        m_lastError = "读取UID失败，状态字: " + ConvertToHex(&response[uidLength], 2);
        LogMessage("NFCManager: Failed to read UID. Status word: %s", ConvertToHex(&response[uidLength], 2).c_str());
        return E_FAIL;
    }
}

HRESULT NFCManager::GetCardInfo(NFCCardInfo& cardInfo) {
    if (m_hCard == NULL) {
        HRESULT hr = ConnectToCard();
        if (FAILED(hr)) {
            return hr;
        }
    }

    // 首先读取UID
    std::string uid;
    HRESULT hr = ReadCardUID(uid);
    if (FAILED(hr)) {
        return hr;
    }

    // 获取卡片状态信息
    DWORD dwAtrLen = 32;
    BYTE pbAtr[32];
    DWORD dwState, dwProtocol;

    LONG lReturn = SCardStatus(m_hCard, NULL, NULL, &dwState, &dwProtocol, pbAtr, &dwAtrLen);
    if (lReturn != SCARD_S_SUCCESS) {
        m_lastError = "无法获取卡片状态: " + GetPCSCErrorString(lReturn);
        return E_FAIL;
    }

    // 填充卡片信息
    ZeroMemory(&m_lastCardInfo, sizeof(m_lastCardInfo));
    
    // 设置UID
    m_lastCardInfo.uid = uid;

    // 设置时间戳
    SYSTEMTIME st = GetCurrentSystemTime();
    m_lastCardInfo.detectedTime = st;

    // 根据ATR判断卡片类型
    if (dwAtrLen > 0) {
        std::string atr = ConvertToHex(pbAtr, dwAtrLen);
        
        // 这里可以根据ATR来判断卡片类型
        // 简化处理，假设为MIFARE卡片
        if (dwAtrLen >= 4 && pbAtr[0] == 0x3B) {
            m_lastCardInfo.type = NFC_CARD_MIFARE_CLASSIC;
            m_lastCardInfo.typeName = "MIFARE";
            m_lastCardInfo.size = 1024; // 1KB
        } else {
            m_lastCardInfo.type = NFC_CARD_UNKNOWN;
            m_lastCardInfo.typeName = "Unknown";
            m_lastCardInfo.size = 0;
        }
    }

    cardInfo = m_lastCardInfo;
    return S_OK;
}

HRESULT NFCManager::WaitForCard(DWORD timeoutMs) {
    if (!m_bInitialized || m_readerName.empty()) {
        m_lastError = "NFC管理器未初始化或未找到读卡器";
        LogMessage("NFCManager::WaitForCard - Not initialized or no reader.");
        return E_FAIL;
    }

    SCARD_READERSTATE rgReaderStates[1];
    rgReaderStates[0].szReader = m_readerName.c_str();
    rgReaderStates[0].dwCurrentState = SCARD_STATE_UNAWARE;

    // 调用 SCardGetStatusChange，它会阻塞直到状态改变或超时
    LogMessage("NFCManager::WaitForCard - Calling SCardGetStatusChange with timeout %dms", timeoutMs);
    LONG lReturn = SCardGetStatusChange(m_hContext, timeoutMs, rgReaderStates, 1);

    if (lReturn == SCARD_E_TIMEOUT) {
        // LogMessage("NFCManager::WaitForCard - Timed out.");
        return HRESULT_FROM_WIN32(ERROR_TIMEOUT);
    }

    if (lReturn != SCARD_S_SUCCESS) {
        m_lastError = "无法获取读卡器状态: " + GetPCSCErrorString(lReturn);
        LogMessage("NFCManager::WaitForCard - SCardGetStatusChange failed with error 0x%X: %s", lReturn, m_lastError.c_str());
        return E_FAIL;
    }

    // 检查状态是否变为有卡
    if ((rgReaderStates[0].dwEventState & SCARD_STATE_PRESENT)) {
        LogMessage("NFCManager::WaitForCard - Card is present.");
        return S_OK;
    }

    LogMessage("NFCManager::WaitForCard - Card is not present. Event state: 0x%X", rgReaderStates[0].dwEventState);
    return E_FAIL; // 卡片不存在
}

std::string NFCManager::GetLastErrorMessage() {
    return m_lastError;
}

void NFCManager::SetDebugMode(bool enable) {
    m_bDebugMode = enable;
}

// 测试PCSC功能
bool NFCManager::TestPCSC() {
    if (!m_bInitialized) {
        HRESULT hr = Initialize();
        if (FAILED(hr)) {
            return false;
        }
    }

    // 检查读卡器
    if (m_readers.empty()) {
        m_lastError = "未找到可用的NFC读卡器";
        return false;
    }

    // 测试卡片检测
    bool cardPresent = false;
    HRESULT hr = IsCardPresent(cardPresent);
    if (FAILED(hr)) {
        return false;
    }

    if (cardPresent) {
        // 尝试读取卡片信息
        NFCCardInfo cardInfo;
        hr = GetCardInfo(cardInfo);
        if (SUCCEEDED(hr)) {
            if (m_bDebugMode) {
                std::cout << "卡片检测成功: " << cardInfo.uid << std::endl;
                std::cout << "卡片类型: " << cardInfo.typeName << std::endl;
            }
            return true;
        }
    }

    return true;
}