#include "NfcManager.hpp"
#include "ReaderDataManager.hpp"
#include "ddk/homekey/ProfileFactory.h"
#include "ddk/session/AuthOutcome.h"
#include "ddk/session/Flow.h"
#include "ddk/session/Session.h"
#include "ddk/store/CredentialStore.h"
#include "ddk/transport/NfcChannel.h"
#include "esp32-hal.h"
#include "esp_log_buffer.h"
#include "esp_log_level.h"
#include "eventStructs.hpp"
#include "fmt/ranges.h"
#include "freertos/idf_additions.h"
#include "Pn532Reader.hpp"
#include "Pn7160Reader.hpp"
#include "St25r3916Reader.hpp"
#include "hal/gpio_types.h"
#include "utils.hpp"

#include <array>
#include <cstdint>
#include <esp_log.h>
#include <chrono>
#include <functional>
#include <memory>
// See eventStructs.hpp: alpaca crc32 platform-detection workaround.
#ifndef __gnu_linux__
#define __gnu_linux__
#endif
#include "alpaca/alpaca.h"

const char* NfcManager::TAG = "NfcManager";

static const uint8_t ECP_HEAD[] = { 0x6A, 0x2, 0xCB, 0x2, 0x6, 0x2, 0x11, 0x00 };

void NfcManager::invalidateAuthCache() {
  if (!m_hkAuthPrecomputeEnabled) {
    return;
  }
  m_readerDataGeneration.fetch_add(1, std::memory_order_relaxed);
}

std::unique_ptr<ddk::Session> NfcManager::buildAuthSession() {
  std::function<bool(std::vector<uint8_t>&, std::vector<uint8_t>&)> nfcFn =
      [this](std::vector<uint8_t>& send, std::vector<uint8_t>& recv) -> bool {
        if (!m_reader || send.size() > 255) {
          return false;
        }
        return m_reader->exchangeApdu(send, recv, 1000);
      };
  ddk::SessionConfig config;
  config.target_flow = authFlow;
  // Constructing the Session runs mbedTLS P-256 ephemeral key generation
  // (~50 ms on ESP32) -- the cost precompute moves off the tap path.
  return std::make_unique<ddk::Session>(
      std::make_shared<ddk::NfcChannel>(nfcFn), m_readerDataManager, config);
}

/**
 * @brief Construct and initialize an NfcManager, set up ECP data and event wiring.
 *
 * Initializes internal state, registers the NFC bus topic, and subscribes to HomeKit/internal
 * events so that ACCESSDATA_CHANGED updates ECP data and invalidates the auth cache, and
 * DEBUG_AUTH_FLOW updates the debug authentication flow when received.
 *
 * @param readerDataManager Reference to the ReaderDataManager used to read and persist reader data.
 * @param nfcGpioPins Four GPIO pin numbers used for SPI communication (SS/CS, SCK, MISO, MOSI).
 * @param nfcReaderType 0 = PN532 (SPI), 1 = PN7160, 2 = ST25R3916 (I2C).
 * @param nfcIrqPin IRQ pin for PN7160 (255 = unset).
 * @param nfcVenPin VEN pin for PN7160 (255 = unset).
 * @param hkAuthPrecomputeEnabled If true, enables HomeKit authentication precompute behavior.
 * @param nfcFastPollingEnabled If true, shortens the delay between polling iterations.
 */
NfcManager::NfcManager(NvsCredentialStore& readerDataManager,
                       const std::array<uint8_t, 4> &nfcGpioPins,
                       uint8_t nfcReaderType,
                       uint8_t nfcIrqPin,
                       uint8_t nfcVenPin,
                       bool hkAuthPrecomputeEnabled,
                       bool nfcFastPollingEnabled)
    : nfcGpioPins(nfcGpioPins),
      m_nfcReaderType(nfcReaderType),
      m_nfcIrqPin(nfcIrqPin),
      m_nfcVenPin(nfcVenPin),
      m_readerDataManager(readerDataManager),
      m_hkAuthPrecomputeEnabled(hkAuthPrecomputeEnabled),
      m_nfcFastPollingEnabled(nfcFastPollingEnabled),
      m_pollingTaskHandle(nullptr),
      m_retryTaskHandle(nullptr)
{
  std::copy(ECP_HEAD, ECP_HEAD + 8, m_ecpData.begin());
  if (nfcReaderType == ST25R3916) {
    pinAllocations.emplace(PinFunctions::SDA, GPIOAllocator::instance().acquire(gpio_num_t(nfcGpioPins[0]), GPIO_MODE_DISABLE, GPIOAllocator::PinRole::I2cSda, GPIOAllocator::PinConsumer::Nfc, "I2C_SDA"));
    pinAllocations.emplace(PinFunctions::SCL, GPIOAllocator::instance().acquire(gpio_num_t(nfcGpioPins[1]), GPIO_MODE_DISABLE, GPIOAllocator::PinRole::I2cScl, GPIOAllocator::PinConsumer::Nfc, "I2C_SCL"));
  } else {
    pinAllocations.emplace(PinFunctions::SS, GPIOAllocator::instance().acquire(gpio_num_t(nfcGpioPins[0]), GPIO_MODE_DISABLE, GPIOAllocator::PinRole::SpiCs, GPIOAllocator::PinConsumer::Nfc, "SPI2_SS"));
    pinAllocations.emplace(PinFunctions::SCK, GPIOAllocator::instance().acquire(gpio_num_t(nfcGpioPins[1]), GPIO_MODE_DISABLE, GPIOAllocator::PinRole::SpiSck, GPIOAllocator::PinConsumer::Nfc, "SPI2_SCK"));
    pinAllocations.emplace(PinFunctions::MISO, GPIOAllocator::instance().acquire(gpio_num_t(nfcGpioPins[2]), GPIO_MODE_DISABLE, GPIOAllocator::PinRole::SpiMiso, GPIOAllocator::PinConsumer::Nfc, "SPI2_MISO"));
    pinAllocations.emplace(PinFunctions::MOSI, GPIOAllocator::instance().acquire(gpio_num_t(nfcGpioPins[3]), GPIO_MODE_DISABLE, GPIOAllocator::PinRole::SpiMosi, GPIOAllocator::PinConsumer::Nfc, "SPI2_MOSI"));
  }
  if (nfcReaderType == PN7160){
    if(nfcIrqPin != 255)
      pinAllocations.emplace(PinFunctions::IRQ, GPIOAllocator::instance().acquire(gpio_num_t(nfcIrqPin), GPIO_MODE_DISABLE, GPIOAllocator::PinRole::NfcIrq, GPIOAllocator::PinConsumer::Nfc, "NFC_IRQ"));
    if(nfcVenPin != 255)
      pinAllocations.emplace(PinFunctions::VEN, GPIOAllocator::instance().acquire(gpio_num_t(nfcVenPin), GPIO_MODE_DISABLE, GPIOAllocator::PinRole::NfcVen, GPIOAllocator::PinConsumer::Nfc, "NFC_VEN"));
  }
  for(auto &p : pinAllocations){
    if(!p.second.has_value()){
      ESP_LOGW(TAG, "Could not acquire GPIO Pin for '%s' with error '%s'", pin_function_str(p.first), GPIOAllocator::error_str(p.second.error()));
    }
  }
  m_hk_event = AppEventLoop::subscribe(HK_EVENT, HK_INTERNAL_EVENT, [&](const uint8_t* data, size_t size){
    if(size == 0 || data == nullptr) return;
    std::span<const uint8_t> payload(data, size);
    std::error_code ec;
    HomekitEvent hk_event = alpaca::deserialize<HomekitEvent>(payload, ec);
    if(ec) { ESP_LOGE(TAG, "Failed to deserialize HomeKit event: %s", ec.message().c_str()); return; }
    switch(hk_event.type) {
      case ACCESSDATA_CHANGED: {
        const auto& readerGid = readerDataManager.reader_identity().group_identifier;
        if (readerGid.size() == 8) {
            std::copy(ECP_HEAD, ECP_HEAD + 8, m_ecpData.begin());
            memcpy(m_ecpData.data() + 8, readerGid.data(), 8);
            Utils::crc16a(m_ecpData.data(), 16, m_ecpData.data() + 16);
        } else {
            std::fill(m_ecpData.begin(), m_ecpData.end(), 0);
        }
        
        m_reconfigRequested.store(true, std::memory_order_release);
        invalidateAuthCache();
      }
      break;
      case DEBUG_AUTH_FLOW: {
        EventValueChanged s = alpaca::deserialize<EventValueChanged>(hk_event.data, ec);
        if(!ec){
          authFlow = ddk::Flow(s.newValue);
          // Cached sessions bake target_flow into their SessionConfig at
          // construction, so drop any built for the previous flow.
          invalidateAuthCache();
        } else {
          ESP_LOGE(TAG, "Failed to deserialize debug auth flow event: %s", ec.message().c_str());
          return;
        }
      }
      break;
      default:
        break;
    }
  });
}

NfcManager::~NfcManager() = default;

/**
 * @brief Initialize the selected NFC reader and start the NFC polling task.
 *
 * Instantiates the correct reader implementation (PN532 or PN7160) and launches
 * the background task that drives NFC polling.
 *
 * @return `true` if the NFC polling task was started, `false` otherwise.
 */
bool NfcManager::begin() {
    const auto& readerGid = m_readerDataManager.reader_identity().group_identifier;
    if (readerGid.size() == 8) {
        memcpy(m_ecpData.data() + 8, readerGid.data(), 8);
        Utils::crc16a(m_ecpData.data(), 16, m_ecpData.data() + 16);
    } else if(readerGid.size() == 0) {
        std::fill(m_ecpData.begin(), m_ecpData.end(), 0);
    }
    if (m_nfcReaderType == 0) {
        m_reader = std::make_unique<Pn532Reader>(nfcGpioPins, m_ecpData);
        ESP_LOGI(TAG, "Using PN532 reader");
    } else if (m_nfcReaderType == 1) {
    	 if (m_nfcIrqPin == 255 || m_nfcVenPin == 255) {
				 ESP_LOGE(TAG, "PN7160 selected but IRQ/VEN pins are unset");
				 return false;
			 }
			m_reader = std::make_unique<Pn7160Reader>(nfcGpioPins, m_nfcIrqPin, m_nfcVenPin, m_ecpData);
			ESP_LOGI(TAG, "Using PN7160 reader");
    } else if (m_nfcReaderType == 2) {
        // I2C: nfcGpioPins[0] = SDA, [1] = SCL. Entries [2]/[3] are unused.
        // Checked here for the same reason the PN7160 branch checks IRQ/VEN:
        // otherwise an unconfigured device reaches i2c_new_master_bus() with
        // sda_io_num = 255 and reports only a generic driver argument error.
        if (nfcGpioPins[0] == 255 || nfcGpioPins[1] == 255) {
            ESP_LOGE(TAG, "ST25R3916 selected but SDA/SCL pins are unset");
            return false;
        }
        m_reader = std::make_unique<St25r3916Reader>(nfcGpioPins, m_ecpData);
        ESP_LOGI(TAG, "Using ST25R3916 reader (I2C)");
    } else {
    	ESP_LOGE(TAG, "Unsupported NFC reader type: %u", m_nfcReaderType);
    	return false;
    }
    ESP_LOGI(TAG, "Auth precompute %s.", m_hkAuthPrecomputeEnabled ? "enabled" : "disabled");
    ESP_LOGI(TAG, "NFC fast polling: %s", m_nfcFastPollingEnabled ? "enabled" : "disabled");
    ESP_LOGI(TAG, "Starting NFC polling task...");
		BaseType_t ok = xTaskCreateUniversal(
				pollingTaskEntry, "nfc_poll_task", 8192, this, 4, &m_pollingTaskHandle, 1);
		if (ok != pdPASS || !m_pollingTaskHandle) {
			ESP_LOGE(TAG, "Failed to create NFC polling task.");
			return false;
		}
		return true;
}

/**
 * @brief Initialize the selected NFC reader and refresh ECP data.
 *
 * Delegates to the active reader's init() method. On success, updates the manager's
 * ECP data buffer.
 *
 * @return `true` if the reader was successfully initialized and ECP data updated, `false` otherwise.
 */
bool NfcManager::initializeReader() {
    if (!m_reader) {
        ESP_LOGE(TAG, "No reader instance available.");
        return false;
    }
    if (!m_reader->init()) {
    		err:
        ESP_LOGE(TAG, "Reader initialization failed.");
        return false;
    }
		if (!m_reader->beginDiscovery()){goto err;}
    ESP_LOGI(TAG, "Reader initialized. Waiting for tags...");
    return true;
}

/**
 * @brief FreeRTOS task entry that dispatches to an instance's polling loop.
 *
 * This static function is used as a task entry point and calls the associated
 * NfcManager instance's pollingTask method.
 *
 * @param instance Pointer to the NfcManager instance whose pollingTask will be executed.
 */
void NfcManager::pollingTaskEntry(void* instance) {
    static_cast<NfcManager*>(instance)->pollingTask();
}

/**
 * @brief Main NFC polling loop that monitors the reader and dispatches tag handling.
 *
 * @details Initializes the NFC reader and then runs indefinitely, repeatedly
 * checking the reader's responsiveness and polling for passive ISO14443A tags.
 * If the reader becomes unresponsive, this task schedules a reconnection retry
 * and suspends itself. When a tag is detected, it invokes the tag handling path
 * and waits for the tag to be removed before continuing normal polling.
 */
void NfcManager::pollingTask() {
    if (!initializeReader()) {
    	while (true) {
    		if (initializeReader()) {
    			ESP_LOGI(TAG, "Reader reconnected successfully.");
    			break;
    		}
    		ESP_LOGW(TAG, "Reconnect attempt failed. Retrying in 5 seconds...");
    		vTaskDelay(pdMS_TO_TICKS(5000));
    	}
    }

    const uint16_t passiveTargetTimeoutMs = 500;
    const TickType_t pollDelayTicks =
        pdMS_TO_TICKS(m_nfcFastPollingEnabled ? 5 : 100);

    ESP_LOGI(TAG,
             "NFC poll tuning active: delay=%lu ms, passiveTimeout=%u ms",
             static_cast<unsigned long>(pollDelayTicks * portTICK_PERIOD_MS),
             static_cast<unsigned int>(passiveTargetTimeoutMs));

    while (true) {
        if (m_reconfigRequested.exchange(false, std::memory_order_acq_rel)) {
          ESP_LOGI(TAG, "ECP Frame update requested...");
          if (m_reader) {
              if (!m_reader->updateECP()) {
                  ESP_LOGE(TAG, "ECP update failed; performing full reader re-initialization.");
                  m_reader->stop();
                  if (!initializeReader()) {
                      vTaskDelay(pdMS_TO_TICKS(1000));
                      continue;
                  }
              }
          }
        }

        if (m_hkAuthPrecomputeEnabled) {
            const auto& identity = m_readerDataManager.reader_identity();
            const bool provisioned =
                identity.group_identifier.size() == 8 &&
                !identity.sub_identifier.empty() &&
                !identity.private_key.empty() &&
                !identity.public_key.empty();
            if (provisioned &&
                (!m_cachedSession ||
                 m_cachedSessionGeneration != m_readerDataGeneration.load(std::memory_order_relaxed))) {
                auto startTime = std::chrono::high_resolution_clock::now();
                m_cachedSession = buildAuthSession();
                m_cachedSessionGeneration = m_readerDataGeneration.load(std::memory_order_relaxed);
                ESP_LOGI(TAG, "Auth session precomputed in %lli ms.",
                         std::chrono::duration_cast<std::chrono::milliseconds>(
                             std::chrono::high_resolution_clock::now() - startTime)
                                 .count());
            }
        }
        if (!m_reader->healthCheck()) {
					ESP_LOGE(TAG, "NFC reader is unresponsive. Attempting to reconnect...");
					while (true) {
						if (initializeReader()) {
							ESP_LOGI(TAG, "Reader reconnected successfully.");
							break;
						}
						ESP_LOGW(TAG, "Reconnect attempt failed. Retrying in 5 seconds...");
						vTaskDelay(pdMS_TO_TICKS(5000));
					}
					continue;
        }

        std::vector<uint8_t> uid;
        std::array<uint8_t,2> atqa;
        uint8_t sak;
        if (m_reader->pollForTag(uid, atqa, sak, passiveTargetTimeoutMs)) {
            ESP_LOGI(TAG, "NFC tag detected!");
            handleTagPresence(uid, atqa, sak);
            waitForTagRemoval();
        }

        vTaskDelay(pollDelayTicks);
        taskYIELD();
    }
}

/**
 * @brief Handles a detected NFC tag by attempting to select the HomeKey applet and processing the tag accordingly.
 *
 * Attempts to select the HomeKey applet on the tag via an APDU select command; if selection succeeds, proceeds with
 * HomeKey authentication handling. If selection fails, reads the tag's UID/ATQA/SAK and processes it as a generic ISO14443A tag.
 * Logs the tag processing duration and releases the reader device state before returning.
 */
void NfcManager::handleTagPresence(const std::vector<uint8_t>& uid, const std::array<uint8_t,2>& atqa, const uint8_t& sak) {
    auto startTime = std::chrono::high_resolution_clock::now();
    uint8_t selectAppletCmd[] = { 0x00, 0xA4, 0x04, 0x00, 0x07, 0xA0, 0x00, 0x00, 0x08, 0x58, 0x01, 0x01, 0x00 };
    std::vector<uint8_t> response;
    bool ok = m_reader->exchangeApdu(std::vector<uint8_t>(selectAppletCmd, selectAppletCmd + sizeof(selectAppletCmd)), response, 500);

    // Check for success SW1=0x90, SW2=0x00
    if (ok && response.size() >= 2 && response[response.size() - 2] == 0x90 && response[response.size() - 1] == 0x00) {
        ESP_LOG_BUFFER_HEX_LEVEL(TAG, response.data(), response.size(), ESP_LOG_DEBUG);
        ESP_LOGI(TAG, "HomeKey applet selected successfully.");
        handleHomeKeyAuth(response);
    } else {
        ESP_LOGI(TAG, "Not a HomeKey tag, or failed to select applet.");
        ESP_LOGD(TAG, "Passive target UID: %s (%zu)", fmt::format("{:02X}", fmt::join(uid, "")).c_str(), uid.size());
        handleGenericTag(uid, atqa, sak);
    }

    auto stopTime = std::chrono::high_resolution_clock::now();
    ESP_LOGI(TAG, "Total processing time: %lli ms", std::chrono::duration_cast<std::chrono::milliseconds>(stopTime - startTime).count());
    // Headroom check. This task runs mbedTLS P-256 operations (ECDH, ECDSA) on
    // top of the reader's frame buffers, and an overflow here would look
    // exactly like the observed symptom: a reboot part-way through a
    // transaction. Reported every time so a downward trend is visible.
    const UBaseType_t stackFreeWords = uxTaskGetStackHighWaterMark(nullptr);
    if (stackFreeWords < 512) {
      ESP_LOGW(TAG, "nfc task stack headroom LOW: %u bytes free",
               (unsigned)(stackFreeWords * sizeof(StackType_t)));
    } else {
      ESP_LOGD(TAG, "nfc task stack headroom: %u bytes free",
               (unsigned)(stackFreeWords * sizeof(StackType_t)));
    }
    ESP_LOGD(TAG, "heap: %u free, %u min-ever", (unsigned)esp_get_free_heap_size(),
             (unsigned)esp_get_minimum_free_heap_size());

}

/**
 * @brief Block until the active tag leaves the reader's RF field.
 *
 * Spins on `isTagStillPresent()` with a 60 ms delay between checks.  If the
 * tag is not removed within 10 seconds, the reader is force-released and the
 * function returns to avoid an infinite loop.
 */
void NfcManager::waitForTagRemoval() {
    if (!m_reader) return;
    TickType_t start = xTaskGetTickCount();
    while (m_reader->isTagStillPresent()) {
        if ((xTaskGetTickCount() - start) * portTICK_PERIOD_MS > 10000) {
            ESP_LOGW(TAG, "Tag removal wait timed out, forcing release.");
            m_reader->releaseTag();
            break;
        }
        vTaskDelay(pdMS_TO_TICKS(60));
    }
    m_reader->releaseTag();
}

/**
 * @brief Attempt HomeKey authentication for the currently-present NFC tag.
 *
 * Performs the configured HomeKey authentication flow for the active tag and publishes a HOMEKEY_TAP
 * event describing the outcome. On successful authentication, stored reader data may be updated.
 *
 * If HomeKey precomputation is enabled, a precomputed authentication session may be consumed (when
 * generation matches current reader data); otherwise a fresh ("cold") authentication session is used.
 *
 * Side effects: may update ReaderDataManager, publish a HOMEKEY_TAP event on the NFC bus, and
 * reset the consumed precomputed session (rebuilt by the polling task).
 */
void NfcManager::handleHomeKeyAuth(const std::vector<uint8_t>& select_response) {
    auto publishAuthResult = [](
        const ddk::AuthOutcome& authResult,
        const std::vector<uint8_t>& readerId
    ) {
        if (authResult.state != ddk::FlowState::Failed) {
            ESP_LOGI(TAG, "HomeKey authentication successful!");
            EventHKTap s{.status = true, .issuerId = authResult.issuer->id, .endpointId = authResult.endpoint->id, .readerId = readerId };
            std::vector<uint8_t> d;
            alpaca::serialize(s, d);
            NfcEvent event{.type=HOMEKEY_TAP, .data=d};
            std::vector<uint8_t> event_data;
            alpaca::serialize(event, event_data);
            AppEventLoop::publish(NFC_EVENT, NFC_TAP_EVENT, event_data.data(), event_data.size());
        } else {
            ESP_LOGW(TAG, "HomeKey authentication failed.");
            EventHKTap s{.status = false, .issuerId = {}, .endpointId = {}, .readerId = {} };
            std::vector<uint8_t> d;
            alpaca::serialize(s, d);
            NfcEvent event{.type=HOMEKEY_TAP, .data=d};
            std::vector<uint8_t> event_data;
            alpaca::serialize(event, event_data);
            AppEventLoop::publish(NFC_EVENT, NFC_TAP_EVENT, event_data.data(), event_data.size());
        }
    };

    auto runAuth = [&](ddk::Profile& profile, ddk::Session& session) {
        auto startTime = std::chrono::high_resolution_clock::now();
        auto err = profile.validate_select(session, select_response);
        if (err != ddk::FailureReason::None) {
          ESP_LOGE(TAG, "Profile Rejected SELECT Response");
          return;
        }
        auto state = ddk::FlowState::Selected;
        while (state != ddk::FlowState::Done && state != ddk::FlowState::Failed) {
            state = profile.step(session, state);
        }

        auto outcome = profile.finalize(session);
        if (outcome) {
            ESP_LOGI(TAG, "Endpoint authenticated in %lli ms", std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - startTime).count());
            publishAuthResult(outcome, m_readerDataManager.reader_identity().sub_identifier);
            profile.control_flow(session, 0x01, 0x00);
        } else {
            profile.control_flow(session, 0x00, 0x00);
        }
    };

    auto authenticateCold = [&]() {
        auto session = buildAuthSession();
        auto profile = ddk::homekey::make_profile();
        runAuth(*profile, *session);
    };

    if (m_hkAuthPrecomputeEnabled) {
        const uint32_t genNow = m_readerDataGeneration.load(std::memory_order_relaxed);
        if (m_cachedSession && m_cachedSessionGeneration == genNow) {
            ESP_LOGI(TAG, "Auth cache hit (gen=%u).", genNow);
            auto profile = ddk::homekey::make_profile();
            runAuth(*profile, *m_cachedSession);
            m_cachedSession.reset();
            return;
        }
        ESP_LOGI(TAG, "Auth cache miss (gen=%u) -> cold init.", genNow);
    }
    authenticateCold();
}

/**
 * @brief Publish a TAG_TAP NFC event for a detected non-HomeKey (generic) tag.
 *
 * Constructs an EventTagTap containing the tag UID, ATQA, and SAK, serializes it
 * into an NfcEvent payload, and publishes the serialized event to the NFC event
 * topic.
 */
void NfcManager::handleGenericTag(const std::vector<uint8_t>& uid, const std::array<uint8_t,2>& atqa, const uint8_t& sak) {
    EventTagTap s{.uid = uid, .atqa = atqa, .sak = sak};
    std::vector<uint8_t> d;
    alpaca::serialize(s, d);
    NfcEvent event{.type=TAG_TAP, .data=d};
    std::vector<uint8_t> event_data;
    alpaca::serialize(event, event_data);
    AppEventLoop::publish(NFC_EVENT, NFC_TAP_EVENT, event_data.data(), event_data.size());
}
