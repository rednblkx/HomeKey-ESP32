#pragma once
#include "ddk/session/Flow.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <array>
#include <atomic>
#include <cstdint>
#include <expected>
#include <map>
#include <memory>
#include "app_event_loop.hpp"
#include "NfcReader.hpp"
#include "GPIOAllocator.hpp"

class LockManager;
class HardwareManager;
class NvsCredentialStore;
class MqttManager;
namespace espConfig { struct misc_config_t; }
namespace ddk { class Session; }

enum ReaderType : uint8_t {
  PN532,
  PN7160,
  ST25R3916
};

class NfcManager {
public:
    NfcManager(NvsCredentialStore& readerDataManager,
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
    ~NfcManager();
    bool begin();

private:
    std::atomic<bool> m_reconfigRequested{false};
    // --- Task Management ---
    static void pollingTaskEntry(void* instance);
    void pollingTask();

    // --- HomeKey Auth Session Precompute ---
    // The polling task is the sole owner of the cached session; event tasks only
    // bump m_readerDataGeneration so the next loop iteration rebuilds it with
    // fresh store data / auth flow.
    void invalidateAuthCache();
    std::unique_ptr<ddk::Session> buildAuthSession();

    // --- Core NFC Logic ---
    bool initializeReader();
    void handleTagPresence(const std::vector<uint8_t>& uid, const std::array<uint8_t,2>& atqa, const uint8_t& sak);
    void handleHomeKeyAuth(const std::vector<uint8_t>& select_response);
    void handleGenericTag(const std::vector<uint8_t>& uid, const std::array<uint8_t,2>& atqa, const uint8_t& sak);
    void waitForTagRemoval();

    // --- Member Variables ---
    const std::array<uint8_t, 4> &nfcGpioPins;
    uint8_t m_nfcReaderType;
    uint8_t m_nfcIrqPin;
    uint8_t m_nfcVenPin;
    std::unique_ptr<INfcReader> m_reader;

    NvsCredentialStore& m_readerDataManager;
    bool m_hkAuthPrecomputeEnabled = false;
    const bool m_nfcFastPollingEnabled;

    TaskHandle_t m_pollingTaskHandle;
    TaskHandle_t m_retryTaskHandle;

    std::unique_ptr<ddk::Session> m_cachedSession;
    uint32_t m_cachedSessionGeneration = 0;
    std::atomic<uint32_t> m_readerDataGeneration{0};

    std::array<uint8_t, 18> m_ecpData;

    ddk::Flow authFlow = ddk::Flow::Fast;

    static const char* TAG;
    AppEventLoop::SubscriptionHandle m_hk_event;

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
    static constexpr const char* pin_function_str(PinFunctions fn) {
      switch (fn) {
        case SCK:  return "SCK";
        case MISO: return "MISO";
        case MOSI: return "MOSI";
        case SS:   return "SS";
        case IRQ:  return "IRQ";
        case VEN:  return "VEN";
        case SDA:  return "SDA";
        case SCL:  return "SCL";
      }
      return "Unknown";
    }

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
