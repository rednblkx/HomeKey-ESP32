#ifndef NFC_MANAGER_HPP
#define NFC_MANAGER_HPP

#include "PN532.h"
#include "PN532_SPI.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <array>
#include <vector>

class LockManager;
class HardwareManager;
class ReaderDataManager;
class MqttManager;
namespace espConfig { struct misc_config_t; }

class NfcManager {
public:
    NfcManager(ReaderDataManager& readerDataManager, void(*)(const std::vector<uint8_t>& issuerId, const std::vector<uint8_t>& endpointId, const std::vector<uint8_t>& readerId),
               void(*)(const uint8_t* uid, uint8_t uidLen, const uint8_t* atqa, const uint8_t* sak),
               void(*fail_cb)(),
               const std::array<uint8_t, 4> &nfcGpioPins);
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

    void(*hk_cb)(const std::vector<uint8_t>& issuerId, const std::vector<uint8_t>& endpointId, const std::vector<uint8_t>& readerId) = {};
    void(*tag_cb)(const uint8_t* uid, uint8_t uidLen, const uint8_t* atqa, const uint8_t* sak) = {};
    void(*fail_cb)() = {};

    std::array<uint8_t, 18> m_ecpData;

    static const char* TAG;
};

#endif // NFC_MANAGER_HPP
