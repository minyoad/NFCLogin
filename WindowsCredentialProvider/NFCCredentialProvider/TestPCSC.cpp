#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include "NFCManager.h"

int main() {
    std::cout << "NFC PCSC功能测试程序" << std::endl;
    std::cout << "=====================" << std::endl;

    // 创建NFC管理器实例
    NFCManager nfcManager;
    
    // 初始化
    std::cout << "正在初始化PCSC..." << std::endl;
    HRESULT hr = nfcManager.Initialize();
    if (FAILED(hr)) {
        std::cout << "初始化失败: " << nfcManager.GetLastErrorMessage() << std::endl;
        return 1;
    }
    std::cout << "PCSC初始化成功!" << std::endl;

    // 检测读卡器
    bool readerPresent = false;
    hr = nfcManager.DetectReader(readerPresent);
    if (FAILED(hr) || !readerPresent) {
        std::cout << "未检测到NFC读卡器: " << nfcManager.GetLastErrorMessage() << std::endl;
        return 1;
    }
    std::cout << "检测到NFC读卡器!" << std::endl;

    // 检测卡片
    std::cout << "正在检测NFC卡片..." << std::endl;
    bool cardPresent = false;
    hr = nfcManager.IsCardPresent(cardPresent);
    if (FAILED(hr)) {
        std::cout << "卡片检测失败: " << nfcManager.GetLastErrorMessage() << std::endl;
        return 1;
    }

    if (!cardPresent) {
        std::cout << "未检测到NFC卡片" << std::endl;
        std::cout << "请将NFC卡片靠近读卡器后重新运行测试" << std::endl;
        return 0;
    }
    std::cout << "检测到NFC卡片!" << std::endl;

    // 读取UID
    std::cout << "正在读取卡片UID..." << std::endl;
    std::string uid;
    hr = nfcManager.ReadCardUID(uid);
    if (FAILED(hr)) {
        std::cout << "读取UID失败: " << nfcManager.GetLastErrorMessage() << std::endl;
        return 1;
    }
    std::cout << "卡片UID: " << uid << std::endl;

    // 获取卡片信息
    std::cout << "正在获取卡片信息..." << std::endl;
    NFCCardInfo cardInfo;
    hr = nfcManager.GetCardInfo(cardInfo);
    if (FAILED(hr)) {
        std::cout << "获取卡片信息失败: " << nfcManager.GetLastErrorMessage() << std::endl;
    } else {
        std::cout << "卡片类型: " << cardInfo.typeName << std::endl;
        std::cout << "卡片大小: " << cardInfo.size << " 字节" << std::endl;
        std::cout << "检测时间: " << cardInfo.detectedTime.wYear << "-" 
                  << std::setfill('0') << std::setw(2) << cardInfo.detectedTime.wMonth << "-"
                  << std::setfill('0') << std::setw(2) << cardInfo.detectedTime.wDay << " "
                  << std::setfill('0') << std::setw(2) << cardInfo.detectedTime.wHour << ":"
                  << std::setfill('0') << std::setw(2) << cardInfo.detectedTime.wMinute << ":"
                  << std::setfill('0') << std::setw(2) << cardInfo.detectedTime.wSecond << std::endl;
    }

    std::cout << std::endl;
    std::cout << "测试完成!" << std::endl;
    std::cout << "========" << std::endl;
    std::cout << "PCSC功能测试通过" << std::endl;
    std::cout << "可以提交到GitHub进行进一步测试" << std::endl;

    return 0;
}