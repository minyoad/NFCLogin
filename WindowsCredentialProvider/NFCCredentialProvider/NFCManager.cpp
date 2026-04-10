#include "NFCManager.h"
#include <windows.h>
#include <setupapi.h>
#include <devguid.h>
#include <tchar.h>
#include <sstream>
#include <iomanip>
#include <thread>
#include <chrono>

#pragma comment(lib, "setupapi.lib")

// 静态成员初始化
const std::string NFCManager::CMD_GET_VERSION = "GET_VERSION";
const std::string NFCManager::CMD_READ_UID = "READ_UID";
const std::string NFCManager::CMD_DETECT_CARD = "DETECT_CARD";
const std::string NFCManager::CMD_GET_CARD_TYPE = "GET_CARD_TYPE";

NFCManager::NFCManager() : 
    m_hSerialPort(INVALID_HANDLE_VALUE),
    m_bInitialized(false),
    m_bDebugMode(false) {
    ZeroMemory(&m_lastCardInfo, sizeof(m_lastCardInfo));
}

NFCManager::~NFCManager() {
    CloseSerialPort();
}

HRESULT NFCManager::Initialize() {
    if (m_bInitialized) {
        return S_OK;
    }

    // 检测可用的串口
    std::wstring portList = GetSerialPortList();
    if (portList.empty()) {
        m_lastError = "未检测到串口设备";
        return E_FAIL;
    }

    // 尝试连接到NFC读卡器
    // 这里假设读卡器连接到第一个可用串口
    std::wstring firstPort = portList.substr(0, portList.find(L','));
    if (!firstPort.empty()) {
        m_portName = firstPort;
        HRESULT hr = InitializeSerialPort();
        if (SUCCEEDED(hr)) {
            m_bInitialized = true;
            return S_OK;
        }
    }

    m_lastError = "无法连接到NFC读卡器";
    return E_FAIL;
}

HRESULT NFCManager::InitializeSerialPort() {
    // 打开串口
    m_hSerialPort = CreateFileW(
        m_portName.c_str(),
        GENERIC_READ | GENERIC_WRITE,
        0,
        nullptr,
        OPEN_EXISTING,
        0,
        nullptr
    );

    if (m_hSerialPort == INVALID_HANDLE_VALUE) {
        m_lastError = "无法打开串口: " + WStringToString(m_portName);
        return E_FAIL;
    }

    // 配置串口参数
    DCB dcb = {0};
    dcb.DCBlength = sizeof(dcb);

    if (!GetCommState(m_hSerialPort, &dcb)) {
        CloseHandle(m_hSerialPort);
        m_hSerialPort = INVALID_HANDLE_VALUE;
        m_lastError = "无法获取串口状态";
        return E_FAIL;
    }

    // 设置常见NFC读卡器参数：9600波特率，8数据位，无校验，1停止位
    dcb.BaudRate = CBR_9600;
    dcb.ByteSize = 8;
    dcb.Parity = NOPARITY;
    dcb.StopBits = ONESTOPBIT;

    if (!SetCommState(m_hSerialPort, &dcb)) {
        CloseHandle(m_hSerialPort);
        m_hSerialPort = INVALID_HANDLE_VALUE;
        m_lastError = "无法设置串口参数";
        return E_FAIL;
    }

    // 设置超时
    COMMTIMEOUTS timeouts = {0};
    timeouts.ReadIntervalTimeout = 50;
    timeouts.ReadTotalTimeoutConstant = 50;
    timeouts.ReadTotalTimeoutMultiplier = 10;
    timeouts.WriteTotalTimeoutConstant = 50;
    timeouts.WriteTotalTimeoutMultiplier = 10;

    if (!SetCommTimeouts(m_hSerialPort, &timeouts)) {
        CloseHandle(m_hSerialPort);
        m_hSerialPort = INVALID_HANDLE_VALUE;
        m_lastError = "无法设置串口超时";
        return E_FAIL;
    }

    return S_OK;
}

void NFCManager::CloseSerialPort() {
    if (m_hSerialPort != INVALID_HANDLE_VALUE) {
        CloseHandle(m_hSerialPort);
        m_hSerialPort = INVALID_HANDLE_VALUE;
    }
    m_bInitialized = false;
}

HRESULT NFCManager::DetectReader(bool& readerPresent) {
    readerPresent = false;

    if (!m_bInitialized) {
        HRESULT hr = Initialize();
        if (FAILED(hr)) {
            return hr;
        }
    }

    // 发送检测命令
    std::string response;
    HRESULT hr = SendCommand(CMD_GET_VERSION, response);
    
    if (SUCCEEDED(hr) && !response.empty()) {
        readerPresent = true;
        return S_OK;
    }

    return S_FALSE;
}

HRESULT NFCManager::ReadCardUID(std::string& uid) {
    uid.clear();

    if (!m_bInitialized) {
        return E_FAIL;
    }

    // 首先检测卡片是否存在
    bool cardPresent = false;
    HRESULT hr = IsCardPresent(cardPresent);
    if (FAILED(hr) || !cardPresent) {
        m_lastError = "未检测到NFC卡片";
        return E_FAIL;
    }

    // 读取UID
    std::string response;
    hr = SendCommand(CMD_READ_UID, response);
    
    if (FAILED(hr)) {
        m_lastError = "无法读取卡片UID";
        return hr;
    }

    // 解析UID响应
    return ParseUIDResponse(response, uid);
}

HRESULT NFCManager::IsCardPresent(bool& cardPresent) {
    cardPresent = false;

    if (!m_bInitialized) {
        return E_FAIL;
    }

    std::string response;
    HRESULT hr = SendCommand(CMD_DETECT_CARD, response);
    
    if (SUCCEEDED(hr) && response.find("CARD_PRESENT") != std::string::npos) {
        cardPresent = true;
        return S_OK;
    }

    return S_FALSE;
}

HRESULT NFCManager::SendCommand(const std::string& command, std::string& response) {
    response.clear();

    if (m_hSerialPort == INVALID_HANDLE_VALUE) {
        m_lastError = "串口未打开";
        return E_FAIL;
    }

    // 发送命令
    DWORD bytesWritten = 0;
    std::string cmd = command + "\r\n"; // 添加换行符
    
    if (!WriteFile(m_hSerialPort, cmd.c_str(), cmd.length(), &bytesWritten, nullptr)) {
        m_lastError = "发送命令失败";
        return E_FAIL;
    }

    // 等待响应
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    // 读取响应
    char buffer[256] = {0};
    DWORD bytesRead = 0;
    
    if (ReadFile(m_hSerialPort, buffer, sizeof(buffer) - 1, &bytesRead, nullptr)) {
        if (bytesRead > 0) {
            buffer[bytesRead] = '\0';
            response = buffer;
            
            // 移除换行符和回车符
            size_t pos = response.find_last_not_of("\r\n");
            if (pos != std::string::npos) {
                response = response.substr(0, pos + 1);
            }
            
            return S_OK;
        }
    }

    m_lastError = "读取响应失败";
    return E_FAIL;
}

HRESULT NFCManager::ParseUIDResponse(const std::string& response, std::string& uid) {
    uid.clear();

    // 查找UID标记
    size_t uidPos = response.find("UID:");
    if (uidPos == std::string::npos) {
        m_lastError = "响应格式错误：未找到UID";
        return E_FAIL;
    }

    // 提取UID值
    size_t uidStart = uidPos + 4; // 跳过"UID:"
    size_t uidEnd = response.find(" ", uidStart);
    if (uidEnd == std::string::npos) {
        uidEnd = response.length();
    }

    uid = response.substr(uidStart, uidEnd - uidStart);
    
    // 验证UID格式
    if (!ValidateUIDFormat(uid)) {
        m_lastError = "UID格式错误";
        uid.clear();
        return E_FAIL;
    }

    // 更新最后检测到的卡片信息
    m_lastCardInfo.uid = uid;
    m_lastCardInfo.detectedTime = GetCurrentSystemTime();
    
    return S_OK;
}

HRESULT NFCManager::GetCardInfo(NFCCardInfo& cardInfo) {
    if (!m_bInitialized) {
        return E_FAIL;
    }

    // 首先读取UID
    std::string uid;
    HRESULT hr = ReadCardUID(uid);
    if (FAILED(hr)) {
        return hr;
    }

    // 获取卡片类型
    std::string response;
    hr = SendCommand(CMD_GET_CARD_TYPE, response);
    
    if (SUCCEEDED(hr)) {
        // 解析卡片类型
        if (response.find("MIFARE_CLASSIC") != std::string::npos) {
            m_lastCardInfo.type = NFC_CARD_MIFARE_CLASSIC;
            m_lastCardInfo.typeName = "MIFARE Classic";
            m_lastCardInfo.size = 1024; // 1KB
        } else if (response.find("MIFARE_ULTRALIGHT") != std::string::npos) {
            m_lastCardInfo.type = NFC_CARD_MIFARE_ULTRALIGHT;
            m_lastCardInfo.typeName = "MIFARE Ultralight";
            m_lastCardInfo.size = 64; // 64字节
        } else if (response.find("NTAG") != std::string::npos) {
            m_lastCardInfo.type = NFC_CARD_NTAG;
            m_lastCardInfo.typeName = "NTAG";
            m_lastCardInfo.size = 924; // NTAG213
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
    DWORD startTime = GetTickCount();
    
    while (GetTickCount() - startTime < timeoutMs) {
        bool cardPresent = false;
        HRESULT hr = IsCardPresent(cardPresent);
        
        if (SUCCEEDED(hr) && cardPresent) {
            return S_OK;
        }
        
        // 等待100ms后重试
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    
    return HRESULT_FROM_WIN32(ERROR_TIMEOUT);
}

std::string NFCManager::GetLastErrorMessage() {
    return m_lastError;
}

void NFCManager::SetDebugMode(bool enable) {
    m_bDebugMode = enable;
}

// 辅助函数实现
std::wstring GetSerialPortList() {
    std::wstring portList;
    
    // 使用SetupAPI枚举串口设备
    HDEVINFO hDevInfo = SetupDiGetClassDevs(&GUID_DEVCLASS_PORTS, nullptr, nullptr, DIGCF_PRESENT);
    if (hDevInfo == INVALID_HANDLE_VALUE) {
        return portList;
    }

    SP_DEVINFO_DATA devInfoData = {0};
    devInfoData.cbSize = sizeof(SP_DEVINFO_DATA);

    for (DWORD i = 0; SetupDiEnumDeviceInfo(hDevInfo, i, &devInfoData); i++) {
        // 获取设备友好名称
        WCHAR friendlyName[256] = {0};
        DWORD requiredSize = 0;
        
        if (SetupDiGetDeviceRegistryPropertyW(hDevInfo, &devInfoData, SPDRP_FRIENDLYNAME, 
                                              nullptr, (PBYTE)friendlyName, sizeof(friendlyName), &requiredSize)) {
            std::wstring deviceName = friendlyName;
            
            // 查找COM端口
            size_t comPos = deviceName.find(L"(COM");
            if (comPos != std::wstring::npos) {
                size_t comEnd = deviceName.find(L")", comPos);
                if (comEnd != std::wstring::npos) {
                    std::wstring comPort = deviceName.substr(comPos + 1, comEnd - comPos - 1);
                    if (!portList.empty()) {
                        portList += L",";
                    }
                    portList += comPort;
                }
            }
        }
    }

    SetupDiDestroyDeviceInfoList(hDevInfo);
    return portList;
}

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