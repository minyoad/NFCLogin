#pragma once

#include <windows.h>
#include <string>
#include <memory>

// NFC卡类型枚举
enum NFCCardType {
    NFC_CARD_UNKNOWN = 0,
    NFC_CARD_MIFARE_CLASSIC = 1,    // MIFARE Classic (M1)
    NFC_CARD_MIFARE_ULTRALIGHT = 2, // MIFARE Ultralight
    NFC_CARD_NTAG = 3,               // NTAG系列
    NFC_CARD_FELICA = 4              // FeliCa
};

// NFC卡信息结构
struct NFCCardInfo {
    std::string uid;          // 卡片UID
    NFCCardType type;         // 卡片类型
    std::string typeName;     // 类型名称
    DWORD size;               // 卡片容量
    bool isReadOnly;          // 是否只读
    SYSTEMTIME detectedTime;  // 检测时间
};

// NFC管理器类
class NFCManager {
public:
    NFCManager();
    ~NFCManager();

    // 初始化NFC管理器
    HRESULT Initialize();

    // 检测NFC读卡器
    HRESULT DetectReader(bool& readerPresent);

    // 读取NFC卡片UID
    HRESULT ReadCardUID(std::string& uid);

    // 获取卡片详细信息
    HRESULT GetCardInfo(NFCCardInfo& cardInfo);

    // 检查卡片是否存在
    HRESULT IsCardPresent(bool& cardPresent);

    // 等待卡片放置
    HRESULT WaitForCard(DWORD timeoutMs = 5000);

    // 获取支持的卡片类型列表
    HRESULT GetSupportedCardTypes(std::vector<std::string>& cardTypes);

    // 获取最后错误信息
    std::string GetLastErrorMessage();

    // 设置调试模式
    void SetDebugMode(bool enable);

    // 获取NFC读卡器信息
    HRESULT GetReaderInfo(std::wstring& readerName, std::wstring& readerVersion);

private:
    // 初始化串口通信
    HRESULT InitializeSerialPort();

    // 关闭串口通信
    void CloseSerialPort();

    // 发送命令到读卡器
    HRESULT SendCommand(const std::string& command, std::string& response);

    // 解析UID响应
    HRESULT ParseUIDResponse(const std::string& response, std::string& uid);

    // 获取卡片类型
    NFCCardType DetectCardType(const std::string& atqa);

    // 计算校验和
    BYTE CalculateChecksum(const BYTE* data, size_t length);

    // 私有成员变量
    HANDLE m_hSerialPort;          // 串口句柄
    std::wstring m_portName;       // 串口名称
    bool m_bInitialized;           // 是否已初始化
    bool m_bDebugMode;             // 调试模式
    std::string m_lastError;     // 最后错误信息
    NFCCardInfo m_lastCardInfo;    // 最后检测到的卡片信息
    
    // 常用命令定义
    static const std::string CMD_GET_VERSION;
    static const std::string CMD_READ_UID;
    static const std::string CMD_DETECT_CARD;
    static const std::string CMD_GET_CARD_TYPE;
    
    // 响应超时时间
    static const DWORD RESPONSE_TIMEOUT = 1000; // 1秒
};

// 辅助函数
std::wstring GetSerialPortList();
std::string ConvertToHex(const BYTE* data, size_t length);
std::vector<BYTE> ParseHexString(const std::string& hexString);
bool ValidateUIDFormat(const std::string& uid);
std::string GetCurrentTimestamp();