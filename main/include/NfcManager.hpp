#pragma once
#include "DDKReaderData.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include <array>
#include <atomic>
#include <functional>
#include "event_bus.hpp"
#include "pn532_cxx/pn532.hpp"
#include "pn532_hal/spi.hpp"

class LockManager;
class HardwareManager;
class ReaderDataManager;
class MqttManager;
class DDKAuthenticationContext;
namespace espConfig { struct misc_config_t; }

class NfcManager {
public:
    NfcManager(ReaderDataManager& readerDataManager, const std::array<uint8_t, 4> &nfcGpioPins, bool hkAuthPrecomputeEnabled);
    /**
 * @brief Unsubscribes the manager's HomeKey event subscription from the global EventBus.
 *
 * Ensures the subscriber identified by m_hk_event is removed when the NfcManager is destroyed.
 */
~NfcManager() {EventBus::Bus::instance().unsubscribe(m_hk_event);}
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
        DDKAuthenticationContext* ctx = nullptr;
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
    void handleTagPresence(const std::vector<uint8_t>& uid, const std::array<uint8_t,2>& atqa, const uint8_t& sak);
    void handleHomeKeyAuth();
    void handleGenericTag(const std::vector<uint8_t>& uid, const std::array<uint8_t,2>& atqa, const uint8_t& sak);
    void waitForTagRemoval();
    
    // --- Member Variables ---
    const std::array<uint8_t, 4> &nfcGpioPins;
    pn532::SpiTransport *m_pn532spi;
    pn532::Frontend *m_nfc;

    ReaderDataManager& m_readerDataManager;
    const bool m_hkAuthPrecomputeEnabled;

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
    EventBus::SubscriberHandle m_hk_event;
    EventBus::TopicHandle m_nfc_topic;
};
