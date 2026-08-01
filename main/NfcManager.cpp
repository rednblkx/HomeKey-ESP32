#include "NfcManager.hpp"
#include "DDKReaderData.h"
#include "ReaderDataManager.hpp"
#include "esp32-hal.h"
#include "eventStructs.hpp"
#include "fmt/ranges.h"
#include "freertos/idf_additions.h"
#include "DDKAuthContext.h"
#include "Pn532Reader.hpp"
#include "Pn7160Reader.hpp"
#include "hal/gpio_types.h"
#include "utils.hpp"

#include <array>
#include <esp_log.h>
#include <chrono>
#include <functional>
#include <serialization.hpp>

const char* NfcManager::TAG = "NfcManager";


/**
 * @brief Task entry wrapper that invokes an instance's auth precompute task.
 *
 * For use as a C-style task entry point; casts the provided task parameter to
 * an NfcManager pointer and calls its authPrecomputeTask method.
 *
 * @param instance Pointer to the NfcManager instance passed as the task parameter.
 */
void NfcManager::authPrecomputeTaskEntry(void* instance) {
  static_cast<NfcManager*>(instance)->authPrecomputeTask();
}

void NfcManager::initAuthPrecompute() {
  if (m_authCtxFreeQueue || m_authCtxReadyQueue || m_authPrecomputeTaskHandle) {
    return;
  }

  m_authCtxFreeQueue = xQueueCreate(kAuthCtxPoolSize, sizeof(AuthCtxCacheItem*));
  m_authCtxReadyQueue = xQueueCreate(kAuthCtxCacheSize, sizeof(AuthCtxCacheItem*));
  if (!m_authCtxFreeQueue || !m_authCtxReadyQueue) {
    ESP_LOGE(TAG, "Failed to create auth precompute queues.");
    if (m_authCtxFreeQueue) {
      vQueueDelete(m_authCtxFreeQueue);
      m_authCtxFreeQueue = nullptr;
    }
    if (m_authCtxReadyQueue) {
      vQueueDelete(m_authCtxReadyQueue);
      m_authCtxReadyQueue = nullptr;
    }
    return;
  }

  for (size_t i = 0; i < kAuthCtxPoolSize; i++) {
    m_authPool[i].nfcFn = [this](std::vector<uint8_t>& send, std::vector<uint8_t>& recv, bool isLong) -> bool {
      if (!m_reader || send.size() > 255) {
        return false;
      }
      return m_reader->exchangeApdu(send, recv, isLong ? 1000 : 500);
    };
    m_authPool[i].saveFn = [this](const readerData_t& data) {
      m_readerDataManager.updateReaderData(data);
      // Reader data changed (e.g., new persistent key / endpoint). Drop any cached contexts.
      invalidateAuthCache();
    };
    AuthCtxCacheItem* item = &m_authPool[i];
    xQueueSend(m_authCtxFreeQueue, &item, 0);
  }

  BaseType_t ok = xTaskCreateUniversal(authPrecomputeTaskEntry, "hk_auth_precompute", 4096, this, 3, &m_authPrecomputeTaskHandle, 0);
  if (ok != pdPASS || !m_authPrecomputeTaskHandle) {
    ESP_LOGE(TAG, "Failed to start auth precompute task.");
    m_authPrecomputeTaskHandle = nullptr;
    vQueueDelete(m_authCtxFreeQueue);
    m_authCtxFreeQueue = nullptr;
    vQueueDelete(m_authCtxReadyQueue);
    m_authCtxReadyQueue = nullptr;
    return;
  }

  ESP_LOGI(TAG, "Auth precompute enabled (cache=%u, pool=%u).", kAuthCtxCacheSize, kAuthCtxPoolSize);
}

void NfcManager::invalidateAuthCache() {
  if (!m_hkAuthPrecomputeEnabled) {
    return;
  }
  m_readerDataGeneration.fetch_add(1, std::memory_order_relaxed);

  if (!m_authCtxReadyQueue || !m_authCtxFreeQueue) {
    return;
  }

  AuthCtxCacheItem* item = nullptr;
  uint32_t invalidatedCount = 0;
  while (xQueueReceive(m_authCtxReadyQueue, &item, 0) == pdTRUE) {
    if (!item) continue;
    delete item->ctx;
    item->ctx = nullptr;
    xQueueSend(m_authCtxFreeQueue, &item, 0);
    invalidatedCount++;
  }

  if (invalidatedCount > 0) {
    ESP_LOGI(TAG, "Auth cache invalidated (%u items).", invalidatedCount);
  }
  if (m_authPrecomputeTaskHandle) {
    xTaskNotifyGive(m_authPrecomputeTaskHandle);
  }
}

void NfcManager::authPrecomputeTask() {
  ESP_LOGI(TAG, "Auth precompute task started.");
  while (true) {
    if (!m_authCtxFreeQueue || !m_authCtxReadyQueue) {
      vTaskDelay(pdMS_TO_TICKS(1000));
      continue;
    }

    const UBaseType_t readyCount = uxQueueMessagesWaiting(m_authCtxReadyQueue);
    if (readyCount >= kAuthCtxCacheSize) {
      ulTaskNotifyTake(pdTRUE, pdMS_TO_TICKS(1000));
      continue;
    }

    AuthCtxCacheItem* item = nullptr;
    if (xQueueReceive(m_authCtxFreeQueue, &item, pdMS_TO_TICKS(1000)) != pdTRUE || !item) {
      ulTaskNotifyTake(pdTRUE, pdMS_TO_TICKS(1000));
      continue;
    }

    readerData_t snapshot = m_readerDataManager.getReaderDataCopy();
    const bool provisioned =
      snapshot.reader_gid.size() == 8 &&
      !snapshot.reader_id.empty() &&
      !snapshot.reader_sk.empty() &&
      !snapshot.reader_pk.empty();

    if (!provisioned) {
      ESP_LOGD(TAG, "Auth precompute: reader not provisioned yet, retrying...");
      xQueueSend(m_authCtxFreeQueue, &item, 0);
      ulTaskNotifyTake(pdTRUE, pdMS_TO_TICKS(1000));
      continue;
    }

    delete item->ctx;
    item->ctx = nullptr;
    item->readerData = std::move(snapshot);
    item->generation = m_readerDataGeneration.load(std::memory_order_relaxed);

    ESP_LOGI(TAG, "Auth precompute: generating (gen=%u, free=%u, ready=%u)...",
             item->generation,
             uxQueueMessagesWaiting(m_authCtxFreeQueue),
             uxQueueMessagesWaiting(m_authCtxReadyQueue));

    auto startTime = std::chrono::high_resolution_clock::now();
    item->ctx = new (std::nothrow) DDKAuthenticationContext(kHomeKey, item->nfcFn, item->readerData, item->saveFn);
    auto stopTime = std::chrono::high_resolution_clock::now();
    const auto durationMs =
        std::chrono::duration_cast<std::chrono::milliseconds>(stopTime - startTime).count();


    // Constructing the context runs mbedTLS P-256 key generation. Report the
    // headroom straight after: an overflow here reboots the device mid-
    // transaction, which is indistinguishable from a hang in the logs unless
    // the reset reason is checked at boot.
    {
      const UBaseType_t freeWords = uxTaskGetStackHighWaterMark(nullptr);
      const unsigned freeBytes = (unsigned)(freeWords * sizeof(StackType_t));
      if (freeBytes < 768) {
        ESP_LOGE(TAG, "precompute task stack CRITICALLY LOW: %u bytes free of %u",
                 freeBytes, 4096u);
      } else {
        ESP_LOGD(TAG, "precompute task stack headroom: %u bytes free of %u", freeBytes,
                 4096u);
      }
    }

    if (!item->ctx) {
      ESP_LOGE(TAG, "Auth precompute: allocation failed.");
      xQueueSend(m_authCtxFreeQueue, &item, 0);
      vTaskDelay(pdMS_TO_TICKS(250));
      continue;
    }

    const uint32_t genAfter = m_readerDataGeneration.load(std::memory_order_relaxed);
    if (item->generation != genAfter) {
      ESP_LOGI(TAG, "Auth precompute: stale during generation (itemGen=%u, genNow=%u), retrying...",
               item->generation,
               genAfter);
      delete item->ctx;
      item->ctx = nullptr;
      xQueueSend(m_authCtxFreeQueue, &item, 0);
      continue;
    }

    if (xQueueSend(m_authCtxReadyQueue, &item, 0) != pdTRUE) {
      ESP_LOGW(TAG, "Auth precompute: cache is full unexpectedly, dropping context.");
      delete item->ctx;
      item->ctx = nullptr;
      xQueueSend(m_authCtxFreeQueue, &item, 0);
      continue;
    }

    ESP_LOGI(TAG, "Auth precompute: ready in %lli ms (gen=%u, free=%u, ready=%u)",
             durationMs,
             item->generation,
             uxQueueMessagesWaiting(m_authCtxFreeQueue),
             uxQueueMessagesWaiting(m_authCtxReadyQueue));
  }
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
 * @param nfcReaderType 0 = PN532, 1 = PN7160.
 * @param nfcIrqPin IRQ pin for PN7160 (255 = unset).
 * @param nfcVenPin VEN pin for PN7160 (255 = unset).
 * @param hkAuthPrecomputeEnabled If true, enables HomeKit authentication precompute behavior.
 * @param nfcFastPollingEnabled If true, shortens the delay between polling iterations.
 */
NfcManager::NfcManager(ReaderDataManager& readerDataManager,
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
      m_retryTaskHandle(nullptr),
      m_ecpData({ 0x6A, 0x2, 0xCB, 0x2, 0x6, 0x2, 0x11, 0x0 })
{
  pinAllocations.emplace(PinFunctions::SS, GPIOAllocator::instance().acquire(gpio_num_t(nfcGpioPins[0]), GPIO_MODE_DISABLE, "SPI2_SS"));
  pinAllocations.emplace(PinFunctions::SCK, GPIOAllocator::instance().acquire(gpio_num_t(nfcGpioPins[1]), GPIO_MODE_DISABLE, "SPI2_SCK"));
  pinAllocations.emplace(PinFunctions::MISO, GPIOAllocator::instance().acquire(gpio_num_t(nfcGpioPins[2]), GPIO_MODE_DISABLE, "SPI2_MISO"));
  pinAllocations.emplace(PinFunctions::MOSI, GPIOAllocator::instance().acquire(gpio_num_t(nfcGpioPins[3]), GPIO_MODE_DISABLE, "SPI2_MOSI"));
  if(nfcIrqPin != 255)
    pinAllocations.emplace(PinFunctions::IRQ, GPIOAllocator::instance().acquire(gpio_num_t(nfcIrqPin), GPIO_MODE_DISABLE, "NFC_IRQ"));
  if(nfcVenPin != 255)
    pinAllocations.emplace(PinFunctions::VEN, GPIOAllocator::instance().acquire(gpio_num_t(nfcVenPin), GPIO_MODE_DISABLE, "NFC_VEN"));
  m_hk_event = AppEventLoop::subscribe(HK_EVENT, HK_INTERNAL_EVENT, [&](const uint8_t* data, size_t size){
    if(size == 0 || data == nullptr) return;
    std::span<const uint8_t> payload(data, size);
    std::error_code ec;
    HomekitEvent hk_event = alpaca::deserialize<HomekitEvent>(payload, ec);
    if(ec) { ESP_LOGE(TAG, "Failed to deserialize HomeKit event: %s", ec.message().c_str()); return; }
    switch(hk_event.type) {
      case ACCESSDATA_CHANGED: {
        updateEcpData();
        invalidateAuthCache();
      }
      break;
      case DEBUG_AUTH_FLOW: {
        EventValueChanged s = alpaca::deserialize<EventValueChanged>(hk_event.data, ec);
        if(!ec){
          authFlow = KeyFlow(s.newValue);
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

/**
 * @brief Initialize the selected NFC reader and start the NFC polling task.
 *
 * Instantiates the correct reader implementation (PN532 or PN7160) and launches
 * the background task that drives NFC polling.
 *
 * @return `true` if the NFC polling task was started, `false` otherwise.
 */
bool NfcManager::begin() {
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
    } else {
    	ESP_LOGE(TAG, "Unsupported NFC reader type: %u", m_nfcReaderType);
    	return false;
    }
    if (m_hkAuthPrecomputeEnabled) {
        initAuthPrecompute();
    } else {
        ESP_LOGI(TAG, "Auth precompute disabled.");
    }
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
 * @brief Update the internal ECP data buffer with the reader GID and its CRC16.
 *
 * If the Reader GID is 8 bytes long, copies it into bytes 8–15 of the internal
 * ECP buffer and computes a CRC16 over the first 16 bytes, storing the 2-byte
 * CRC at bytes 16–17 of the buffer. If the Reader GID is not provisioned,
 * logs a warning and leaves the ECP buffer unchanged.
 */
void NfcManager::updateEcpData() {
    const auto readerData = m_readerDataManager.getReaderDataCopy();
    const auto& readerGid = readerData.reader_gid;
    if (readerGid.size() == 8) {
        memcpy(m_ecpData.data() + 8, readerGid.data(), 8);
        Utils::crc16a(m_ecpData.data(), 16, m_ecpData.data() + 16);
        if(m_nfcReaderType == 1 && m_reader){
          m_reader->updateECP();
        }
    } else {
        ESP_LOGW(TAG, "Reader GID is not provisioned. ECP data may be invalid.");
    }
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
    if(m_nfcReaderType != 1) {
	    updateEcpData();
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
        ESP_LOGI(TAG, "HomeKey applet selected successfully.");
        handleHomeKeyAuth();
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
 * If HomeKey precomputation is enabled, a precomputed authentication context may be consumed (when
 * generation matches current reader data); otherwise a fresh ("cold") authentication context is used.
 *
 * Side effects: may update ReaderDataManager, publish a HOMEKEY_TAP event on the NFC bus, notify the
 * auth precompute task, and modify internal auth-cache queues.
 */
void NfcManager::handleHomeKeyAuth() {
    auto publishAuthResult = [](
        const AuthContextResult& authResult,
        const std::vector<uint8_t>& readerId
    ) {
        if (authResult.flow != kFlowFailed) {
            ESP_LOGI(TAG, "HomeKey authentication successful!");
            EventHKTap s{.status = true, .issuerId = authResult.issuer_id, .endpointId = authResult.endpoint_id, .readerId = readerId };
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

    auto authenticateCold = [this, &publishAuthResult]() {
        readerData_t readerData = m_readerDataManager.getReaderDataCopy();

        // IMPORTANT: HKAuthenticationContext stores references to std::function objects.
        // Do NOT pass lambdas directly (would bind to temporaries and dangle).
        std::function<bool(std::vector<uint8_t>&, std::vector<uint8_t>&, bool)> nfcFn =
            [this](std::vector<uint8_t>& send, std::vector<uint8_t>& recv, bool isLong) -> bool {
                if (!m_reader || send.size() > 255) {
                    return false;
                }
                return m_reader->exchangeApdu(send, recv, isLong ? 1000 : 500);
            };
        std::function<void(const readerData_t&)> saveFn = [this](const readerData_t& data) {
            m_readerDataManager.updateReaderData(data);
            invalidateAuthCache();
        };
        DDKAuthenticationContext authCtx(kHomeKey, nfcFn, readerData, saveFn);
        auto authResult = authCtx.authenticate(authFlow);
        publishAuthResult(authResult, readerData.reader_id);
    };

    if (!m_hkAuthPrecomputeEnabled) {
        authenticateCold();
        return;
    }

    const UBaseType_t readyBefore = m_authCtxReadyQueue ? uxQueueMessagesWaiting(m_authCtxReadyQueue) : 0;
    const UBaseType_t freeBefore = m_authCtxFreeQueue ? uxQueueMessagesWaiting(m_authCtxFreeQueue) : 0;
    const uint32_t genNow = m_readerDataGeneration.load(std::memory_order_relaxed);

    AuthCtxCacheItem* item = nullptr;
    bool gotCached = false;
    if (m_authCtxReadyQueue && xQueueReceive(m_authCtxReadyQueue, &item, 0) == pdTRUE) {
      if (item && item->ctx) {
        gotCached = true;
      } else if (item) {
        ESP_LOGW(TAG, "Auth cache item dequeued without context, returning to free queue.");
        if (m_authCtxFreeQueue) {
          xQueueSend(m_authCtxFreeQueue, &item, 0);
        }
        item = nullptr;
      }
    }

    if (gotCached) {
      const bool genMatch = (item->generation == genNow);
      if (!genMatch) {
        ESP_LOGW(TAG, "Auth cache stale (itemGen=%u, genNow=%u) -> cold init.", item->generation, genNow);
        delete item->ctx;
        item->ctx = nullptr;
        xQueueSend(m_authCtxFreeQueue, &item, 0);
        item = nullptr;
      } else {
        const UBaseType_t readyAfter = uxQueueMessagesWaiting(m_authCtxReadyQueue);
        const UBaseType_t freeAfter = m_authCtxFreeQueue ? uxQueueMessagesWaiting(m_authCtxFreeQueue) : 0;
        ESP_LOGI(TAG, "Auth cache hit (gen=%u, free=%u->%u, ready=%u->%u).",
                 genNow, freeBefore, freeAfter, readyBefore, readyAfter);
        if (m_authPrecomputeTaskHandle) {
          xTaskNotifyGive(m_authPrecomputeTaskHandle);
        }

        auto authResult = item->ctx->authenticate(authFlow);
        const auto readerId = item->readerData.reader_id;
        delete item->ctx;
        item->ctx = nullptr;
        xQueueSend(m_authCtxFreeQueue, &item, 0);
        publishAuthResult(authResult, readerId);
        return;
      }
    }

    ESP_LOGI(TAG, "Auth cache miss (gen=%u, free=%u, ready=%u) -> cold init.", genNow, freeBefore, readyBefore);
    if (m_authPrecomputeTaskHandle) {
      xTaskNotifyGive(m_authPrecomputeTaskHandle);
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
