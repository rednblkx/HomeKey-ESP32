#pragma once
#include "DDKReaderData.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include <array>
#include <atomic>
#include <expected>
#include <functional>
#include <map>
#include <memory>
#include "app_event_loop.hpp"
#include "NfcReader.hpp"
#include "GPIOAllocator.hpp"

class LockManager;
class HardwareManager;
class ReaderDataManager;
class MqttManager;
class DDKAuthenticationContext;
namespace espConfig { struct misc_config_t; }

class NfcManager {
public:
    NfcManager(ReaderDataManager& readerDataManager,
               const std::array<uint8_t, 4> &nfcGpioPins,
               uint8_t nfcReaderType,
               uint8_t nfcIrqPin,
               uint8_t nfcVenPin,
               bool hkAuthPrecomputeEnabled,
               bool nfcFastPollingEnabled);
    /**
     * `@brief` Destructor.
     *
     * The HomeKey event subscription (m_hk_event) is automatically unregistered
     * when NfcManager is destroyed, via SubscriptionHandle's RAII cleanup.
     */
    ~NfcManager() = default;
    bool begin();

private:
    std::atomic<bool> m_reconfigRequested{false};
    // --- Task Management ---
    static void pollingTaskEntry(void* instance);
    void pollingTask();

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
    uint8_t m_nfcReaderType;
    uint8_t m_nfcIrqPin;
    uint8_t m_nfcVenPin;
    std::unique_ptr<INfcReader> m_reader;

    ReaderDataManager& m_readerDataManager;
    const bool m_hkAuthPrecomputeEnabled;
    const bool m_nfcFastPollingEnabled;

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
    AppEventLoop::SubscriptionHandle m_hk_event;
    // Stack for the hk_auth_precompute task. mbedTLS P-256 key generation was
    // measured using 4056-4288 bytes, so 4096 was not survivable.
    static constexpr uint32_t kAuthPrecomputeStackBytes = 6144;

    enum PinFunctions {
      SCK,
      MISO,
      MOSI,
      SS,
      IRQ,
      VEN,
      // I2C readers (ST25R3916) use only two of the four nfcGpioPins entries.
      SDA,
      SCL
    };

    std::map<PinFunctions, std::expected<GPIOAllocator::GPIOLease, GPIOAllocator::GPIOAllocatorError>> pinAllocations;

public:
    /**
     * @brief Check if NFC reader is connected.
     * @return true if connected, false otherwise.
     */
    bool isConnected() const { return m_reader ? m_reader->isConnected() : false; }

    /**
     * @brief Get firmware version major.
     * @return Major version number.
     */
    uint8_t getFirmwareVersionMajor() const { return m_reader ? m_reader->getFwMajor() : 0; }

    /**
     * @brief Get firmware version minor.
     * @return Minor version number.
     */
    uint8_t getFirmwareVersionMinor() const { return m_reader ? m_reader->getFwMinor() : 0; }
};
