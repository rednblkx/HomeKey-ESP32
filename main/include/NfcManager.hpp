#pragma once
#include "HomeKey.h"
#include "PN532.h"
#include "PN532_SPI.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <array>

class LockManager;
class HardwareManager;
class ReaderDataManager;
class MqttManager;
namespace espConfig { struct misc_config_t; }

class NfcManager {
public:
    NfcManager(ReaderDataManager& readerDataManager, const std::array<uint8_t, 4> &nfcGpioPins);
    bool begin();
    void updateEcpData();

private:
    // --- Task Management ---
    static void pollingTaskEntry(void* instance);
    void pollingTask();
    static void retryTaskEntry(void* instance);
    void retryTask();
    void startRetryTask();

    // --- Core NFC Logic ---
    bool initializeReader();
    void handleTagPresence();
    void handleHomeKeyAuth();
    void handleGenericTag(const uint8_t* uid, uint8_t uidLen, const uint8_t* atqa, const uint8_t* sak);
    void waitForTagRemoval();
    
    // --- Member Variables ---
    const std::array<uint8_t, 4> &nfcGpioPins;
    PN532_SPI *m_pn532spi;
    PN532 *m_nfc;
    
    ReaderDataManager& m_readerDataManager;

    TaskHandle_t m_pollingTaskHandle;
    TaskHandle_t m_retryTaskHandle;

    std::array<uint8_t, 18> m_ecpData;

    KeyFlow authFlow = KeyFlow::kFlowFAST;

    static const char* TAG;
};

