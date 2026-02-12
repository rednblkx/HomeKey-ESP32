#include "NfcManager.hpp"
#include "DDKReaderData.h"
#include "ReaderDataManager.hpp"
#include "esp32-hal.h"
#include "eventStructs.hpp"
#include "fmt/ranges.h"
#include "freertos/idf_additions.h"
#include "DDKAuthContext.h"
#include "pn532_hal/spi.hpp"
#include "soc/gpio_num.h"
#include "utils.hpp"

#include <array>
#include <esp_log.h>
#include <chrono>
#include <functional>
#include <serialization.hpp>

const char* NfcManager::TAG = "NfcManager";

static EventBus::Bus& event_bus = EventBus::Bus::instance();

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
      if (send.size() > 255) {
        return false;
      }
      pn532::Status ok = m_nfc->InDataExchange(send, recv);
      if(recv.size() >= 2){
          recv.erase(recv.begin(), recv.begin() + 2);
      }
      return ok == pn532::Status::SUCCESS;
    };
    m_authPool[i].saveFn = [this](const readerData_t& data) {
      m_readerDataManager.updateReaderData(data);
      // Reader data changed (e.g., new persistent key / endpoint). Drop any cached contexts.
      invalidateAuthCache();
    };
    AuthCtxCacheItem* item = &m_authPool[i];
    xQueueSend(m_authCtxFreeQueue, &item, 0);
  }

  BaseType_t ok = xTaskCreateUniversal(authPrecomputeTaskEntry, "hk_auth_precompute", 8192, this, 3, &m_authPrecomputeTaskHandle, 0);
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
 * @param nfcGpioPins Four GPIO pin numbers used to construct the PN532 SPI interface.
 * @param hkAuthPrecomputeEnabled If true, enables HomeKit authentication precompute behavior.
 */
NfcManager::NfcManager(ReaderDataManager& readerDataManager, const std::array<uint8_t, 4> &nfcGpioPins, bool hkAuthPrecomputeEnabled)
    : nfcGpioPins(nfcGpioPins),
      m_readerDataManager(readerDataManager),
      m_hkAuthPrecomputeEnabled(hkAuthPrecomputeEnabled),
      m_pollingTaskHandle(nullptr),
      m_retryTaskHandle(nullptr),
      m_ecpData({ 0x6A, 0x2, 0xCB, 0x2, 0x6, 0x2, 0x11, 0x0 }),
      m_nfc_topic(event_bus.register_topic(NFC_BUS_TOPIC))
{
  m_hk_event = event_bus.subscribe(event_bus.get_topic(HK_BUS_TOPIC).value_or(EventBus::INVALID_TOPIC), [&](const EventBus::Event& event, void* context){
    if(event.payload_size == 0 || event.payload == nullptr) return;
    std::span<const uint8_t> payload(static_cast<const uint8_t*>(event.payload), event.payload_size);
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
 * @brief Initialize the PN532 SPI interface, create the PN532 instance, and start the NFC polling task.
 *
 * Initializes internal PN532_SPI and PN532 objects and launches the background task that drives NFC polling.
 *
 * @return `true` if the NFC polling task was started, `false` otherwise.
 */
bool NfcManager::begin() {
    m_pn532spi = new pn532::SpiTransport((gpio_num_t)GPIO_NUM_NC, (gpio_num_t)nfcGpioPins[2], (gpio_num_t)nfcGpioPins[3], (gpio_num_t)nfcGpioPins[1], (gpio_num_t)nfcGpioPins[0]); 
    m_nfc = new pn532::Frontend(*m_pn532spi);
    if (m_hkAuthPrecomputeEnabled) {
        initAuthPrecompute();
    } else {
        ESP_LOGI(TAG, "Auth precompute disabled.");
    }
    ESP_LOGI(TAG, "Starting NFC polling task...");
    xTaskCreateUniversal(pollingTaskEntry, "nfc_poll_task", 8192, this, 2, &m_pollingTaskHandle, 1);
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
    } else {
        ESP_LOGW(TAG, "Reader GID is not provisioned. ECP data may be invalid.");
    }
}

/**
 * @brief Initialize and configure the PN532 NFC reader and refresh ECP data.
 *
 * Attempts to start the PN532, verify firmware presence, configure the SAM,
 * RF field, and passive activation retry policy, then updates the manager's
 * ECP data buffer.
 *
 * @return `true` if the reader was successfully initialized and ECP data updated, `false` if firmware/version check failed and the reader was stopped.
 */
bool NfcManager::initializeReader() {
    m_nfc->begin();
    uint32_t versiondata = m_nfc->GetFirmwareVersion();
    if (!versiondata) {
        ESP_LOGE(TAG, "Error establishing PN532 connection.");
        return false;
    }
    ESP_LOGI(TAG, "Found chip PN532, Firmware ver. %d.%d", (versiondata >> 24) & 0xFF, (versiondata >> 16) & 0xFF);
    m_nfc->RFConfiguration(0x01, {0x03});
    m_nfc->setPassiveActivationRetries(0);
    m_nfc->RFConfiguration(0x02, {0x00, 0x0B, 0x10});
    m_nfc->RFConfiguration(0x04, {0xFF});
    ESP_LOGI(TAG, "Reader initialized. Waiting for tags...");
    updateEcpData();
    return true;
}

/**
 * @brief Starts the PN532 reconnection retry task if it is not already running.
 *
 * Creates and stores a task handle for the background retry loop; if a retry task
 * is already active, this function returns without side effects.
 */
void NfcManager::startRetryTask() {
    if (m_retryTaskHandle == nullptr) {
        xTaskCreateUniversal(retryTaskEntry, "nfc_retry_task", 4096, this, 5, &m_retryTaskHandle, 1);
    }
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
 * @brief RTOS task entry point for the NFC retry task.
 *
 * This function is the C-style entry invoked by the RTOS; it expects a pointer
 * to an NfcManager instance (passed as void*) and transfers control to that
 * instance's retry task implementation.
 *
 * @param instance Pointer to an NfcManager instance, provided by the RTOS task creation call.
 */
void NfcManager::retryTaskEntry(void* instance) {
    static_cast<NfcManager*>(instance)->retryTask();
}

/**
 * @brief Continuously attempts to reconnect to the PN532 and restores normal polling on success.
 *
 * Repeatedly calls reader initialization until it succeeds. When the PN532 is reinitialized,
 * the method resumes the polling task if present, clears the retry task handle, and terminates
 * the current retry task. On failure, it waits 5 seconds before trying again.
 */
void NfcManager::retryTask() {
    ESP_LOGI(TAG, "Starting PN532 reconnection task...");
    while (true) {
        if (initializeReader()) {
            ESP_LOGI(TAG, "PN532 reconnected successfully.");
            if (m_pollingTaskHandle) vTaskResume(m_pollingTaskHandle);
            m_retryTaskHandle = nullptr;
            vTaskDelete(NULL);
        }
        ESP_LOGW(TAG, "Reconnect attempt failed. Retrying in 5 seconds...");
        vTaskDelay(pdMS_TO_TICKS(5000));
    }
}

/**
 * @brief Main NFC polling loop that monitors the PN532 reader and dispatches tag handling.
 *
 * @details Initializes the NFC reader and then runs indefinitely, repeatedly
 * checking the reader's responsiveness and polling for passive ISO14443A tags.
 * If the reader becomes unresponsive, this task schedules a reconnection retry
 * and suspends itself. When a tag is detected, it invokes the tag handling path
 * and waits for the tag to be removed before continuing normal polling.
 */
void NfcManager::pollingTask() {
    if (!initializeReader()) {
      startRetryTask();
      vTaskSuspend(NULL);
    }

    while (true) {
        if (m_nfc->WriteRegister({0x63,0x3d,0x0}) != pn532::SUCCESS) {
            ESP_LOGE(TAG, "PN532 is unresponsive. Attempting to reconnect...");
            startRetryTask();
            vTaskSuspend(NULL);
            continue;
        }

        std::vector<uint8_t> res;
        m_nfc->InCommunicateThru(std::vector<uint8_t>(m_ecpData.begin(), m_ecpData.end()), res, 50);

        std::vector<uint8_t> uid;
        std::array<uint8_t,2> sens_res;
        uint8_t sel_res;
        pn532::Status passiveTargetFound = m_nfc->InListPassiveTarget(PN532_MIFARE_ISO14443A, uid, sens_res, sel_res, 500);
        
        if (passiveTargetFound == pn532::SUCCESS) {
            ESP_LOGI(TAG, "NFC tag detected!");
            handleTagPresence(uid, sens_res, sel_res);
            pn532::Status status;
            while ((status = m_nfc->InListPassiveTarget(0x00, uid, sens_res, sel_res)) == pn532::SUCCESS) {
              m_nfc->InRelease(1);
              vTaskDelay(pdMS_TO_TICKS(60));
            }
            ESP_LOGI(TAG, "NFC tag removed!");
            m_nfc->setPassiveActivationRetries(0);
        }

        vTaskDelay(pdMS_TO_TICKS(100));
        taskYIELD();
    }
}

/**
 * @brief Handles a detected NFC tag by attempting to select the HomeKey applet and processing the tag accordingly.
 *
 * Attempts to select the HomeKey applet on the tag via an APDU select command; if selection succeeds, proceeds with
 * HomeKey authentication handling. If selection fails, reads the tag's UID/ATQA/SAK and processes it as a generic ISO14443A tag.
 * Logs the tag processing duration and releases the PN532 device state before returning.
 */
void NfcManager::handleTagPresence(const std::vector<uint8_t>& uid, const std::array<uint8_t,2>& atqa, const uint8_t& sak) {
    auto startTime = std::chrono::high_resolution_clock::now();
    uint8_t selectAppletCmd[] = { 0x00, 0xA4, 0x04, 0x00, 0x07, 0xA0, 0x00, 0x00, 0x08, 0x58, 0x01, 0x01, 0x00 };
    std::vector<uint8_t> response;
    pn532::Status status = m_nfc->InDataExchange(std::vector<uint8_t>(selectAppletCmd, selectAppletCmd + sizeof(selectAppletCmd)), response);

    // Check for success SW1=0x90, SW2=0x00
    if (status == pn532::SUCCESS && response.size() >= 2 && response[response.size() - 2] == 0x90 && response[response.size() - 1] == 0x00) {
        ESP_LOGI(TAG, "HomeKey applet selected successfully.");
        handleHomeKeyAuth();
    } else {
        ESP_LOGI(TAG, "Not a HomeKey tag, or failed to select applet.");
        ESP_LOGD(TAG, "Passive target UID: %s (%d)", fmt::format("{:02X}", fmt::join(uid, "")).c_str(), uid.size());
        handleGenericTag(uid, atqa, sak);
    }
    
    auto stopTime = std::chrono::high_resolution_clock::now();
    ESP_LOGI(TAG, "Total processing time: %lli ms", std::chrono::duration_cast<std::chrono::milliseconds>(stopTime - startTime).count());
    
    m_nfc->InRelease(1);
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
        const std::tuple<std::vector<uint8_t>, std::vector<uint8_t>, KeyFlow>& authResult,
        const std::vector<uint8_t>& readerId
    ) {
        if (std::get<2>(authResult) != kFlowFailed) {
            ESP_LOGI(TAG, "HomeKey authentication successful!");
            EventHKTap s{.status = true, .issuerId = std::get<0>(authResult), .endpointId = std::get<1>(authResult), .readerId = readerId };
            std::vector<uint8_t> d;
            alpaca::serialize(s, d);
            NfcEvent event{.type=HOMEKEY_TAP, .data=d};
            std::vector<uint8_t> event_data;
            alpaca::serialize(event, event_data);
            event_bus.publish({event_bus.get_topic(NFC_BUS_TOPIC).value_or(EventBus::INVALID_TOPIC), 0, event_data.data(), event_data.size()});
        } else {
            ESP_LOGW(TAG, "HomeKey authentication failed.");
            EventHKTap s{.status = false, .issuerId = {}, .endpointId = {}, .readerId = {} };
            std::vector<uint8_t> d;
            alpaca::serialize(s, d);
            NfcEvent event{.type=HOMEKEY_TAP, .data=d};
            std::vector<uint8_t> event_data;
            alpaca::serialize(event, event_data);
            event_bus.publish({event_bus.get_topic(NFC_BUS_TOPIC).value_or(EventBus::INVALID_TOPIC), 0, event_data.data(), event_data.size()});
        }
    };

    auto authenticateCold = [this, &publishAuthResult]() {
        readerData_t readerData = m_readerDataManager.getReaderDataCopy();

        // IMPORTANT: HKAuthenticationContext stores references to std::function objects.
        // Do NOT pass lambdas directly (would bind to temporaries and dangle).
        std::function<bool(std::vector<uint8_t>&, std::vector<uint8_t>&, bool)> nfcFn =
            [this](std::vector<uint8_t>& send, std::vector<uint8_t>& recv, bool isLong) -> bool {
                if (send.size() > 255) {
                    return false;
                }
                pn532::Status ok = m_nfc->InDataExchange(send, recv);
                if(recv.size() >= 2){
                    recv.erase(recv.begin(), recv.begin() + 2);
                }
                return ok == pn532::SUCCESS;
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
 *
 * @param uid Pointer to the tag UID bytes.
 * @param uidLen Number of bytes in `uid`.
 * @param atqa Pointer to the 2-byte ATQA value.
 * @param sak Pointer to the 1-byte SAK value.
 */
void NfcManager::handleGenericTag(const std::vector<uint8_t>& uid, const std::array<uint8_t,2>& atqa, const uint8_t& sak) {
    EventTagTap s{.uid = uid, .atqa = atqa, .sak = sak};
    std::vector<uint8_t> d;
    alpaca::serialize(s, d);
    NfcEvent event{.type=TAG_TAP, .data=d};
    std::vector<uint8_t> event_data;
    alpaca::serialize(event, event_data);
    event_bus.publish({event_bus.get_topic(NFC_BUS_TOPIC).value_or(EventBus::INVALID_TOPIC), 0, event_data.data(), event_data.size()});
}
