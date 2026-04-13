#pragma once

#include <windows.h>
#include <string>
#include <memory>
#include <vector>
#include <sstream>
#include <iomanip>
#include <winscard.h>

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

    // 测试PCSC功能
    bool TestPCSC();

    // 连接到卡片
    HRESULT ConnectToCard();

    // 断开卡片连接
    HRESULT DisconnectFromCard();

private:
    // 初始化PCSC资源
    HRESULT InitializePCSC();

    // 释放PCSC资源
    void CleanupPCSC();

    // 发送APDU命令到卡片
    HRESULT SendAPDUCommand(const BYTE* cmd, DWORD cmdLen, BYTE* response, DWORD* responseLen);

    // 解析UID响应
    HRESULT ParseUIDResponse(const BYTE* response, DWORD responseLen, std::string& uid);

    // 获取卡片类型
    NFCCardType DetectCardType(const BYTE* atr, DWORD atrLen);

    // 获取PCSC错误信息
    std::string GetPCSCErrorString(LONG errorCode);

    // 私有成员变量
    SCARDCONTEXT m_hContext;        // PCSC上下文
    SCARDHANDLE m_hCard;           // 卡片连接句柄
    std::wstring m_readerName;     // 读卡器名称
    DWORD m_dwActiveProtocol;       // 活动协议
    bool m_bInitialized;           // 是否已初始化
    bool m_bDebugMode;             // 调试模式
    std::string m_lastError;       // 最后错误信息
    NFCCardInfo m_lastCardInfo;    // 最后检测到的卡片信息
    std::vector<std::wstring> m_readers; // 可用读卡器列表
    
    // 响应超时时间
    static const DWORD RESPONSE_TIMEOUT = 5000; // 5秒
};

// 辅助函数
std::string ConvertToHex(const BYTE* data, size_t length);
std::vector<BYTE> ParseHexString(const std::string& hexString);
bool ValidateUIDFormat(const std::string& uid);
std::string GetCurrentTimestamp();
std::string GetPCSCErrorString(LONG errorCode);
std::string WStringToString(const std::wstring& wstr);
SYSTEMTIME GetCurrentSystemTime();