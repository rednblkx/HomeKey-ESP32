#pragma once
#include "HomeKey.h"
#include "PN532.h"
#include "PN532_SPI.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include <array>
#include <atomic>
#include <functional>

class LockManager;
class HardwareManager;
class ReaderDataManager;
class MqttManager;
class HKAuthenticationContext;
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

    // --- HomeKey Auth Cache (precompute) ---
    struct AuthCtxCacheItem {
        readerData_t readerData;
        std::function<bool(std::vector<uint8_t>&, std::vector<uint8_t>&, bool)> nfcFn;
        std::function<void(const readerData_t&)> saveFn;
        HKAuthenticationContext* ctx = nullptr;
        uint32_t generation = 0;
    };

    // Keep exactly one precomputed context ready for the next tap, and one extra slot to
    // allow generating the next context while a tap is being processed.
    static constexpr size_t kAuthCtxCacheSize = 1;
    static constexpr size_t kAuthCtxPoolSize = 2;
    static void authPrecomputeTaskEntry(void* instance);
    void authPrecomputeTask();
    void initAuthPrecompute();
    void invalidateAuthCache();

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
    TaskHandle_t m_authPrecomputeTaskHandle = nullptr;

    QueueHandle_t m_authCtxFreeQueue = nullptr;
    QueueHandle_t m_authCtxReadyQueue = nullptr;
    AuthCtxCacheItem m_authPool[kAuthCtxPoolSize];
    std::atomic<uint32_t> m_readerDataGeneration{0};

    std::array<uint8_t, 18> m_ecpData;

    KeyFlow authFlow = KeyFlow::kFlowFAST;

    static const char* TAG;
};
