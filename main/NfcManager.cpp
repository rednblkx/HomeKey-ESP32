#include "NfcManager.hpp"
#include "PN532.h"
#include "PN532_SPI.h"
#include "ReaderDataManager.hpp"
#include "esp32-hal.h"
#include "eventStructs.hpp"
#include "hkAuthContext.h"
#include "utils.hpp"

#include <array>
#include <esp_log.h>
#include <chrono>
#include <functional>
#include <serialization.hpp>

const char* NfcManager::TAG = "NfcManager";

static EventBus::Bus& event_bus = EventBus::Bus::instance();

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
      uint8_t sendLength = static_cast<uint8_t>(send.size());
      uint8_t responseBuffer[256];
      uint16_t responseLength = sizeof(responseBuffer);
      bool ok = m_nfc->inDataExchange(send.data(), sendLength, responseBuffer, &responseLength, isLong);
      if (ok) {
        recv.assign(responseBuffer, responseBuffer + responseLength);
      }
      return ok;
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
    item->ctx = new (std::nothrow) HKAuthenticationContext(item->nfcFn, item->readerData, item->saveFn);
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
 * @brief Initializes the NFC manager, configures internal state, and registers NFC-related event handlers.
 *
 * Sets initial ECP data and task handles, registers an "nfc/event" publisher, and subscribes to
 * "homekit/internal" events to react to ACCESSDATA_CHANGED (updates ECP data) and DEBUG_AUTH_FLOW
 * (updates the authentication flow).
 *
 * @param readerDataManager Reference to the ReaderDataManager used to read and persist reader data.
 * @param nfcGpioPins Four GPIO pin numbers used to construct the PN532 SPI interface.
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
    m_pn532spi = new PN532_SPI(nfcGpioPins[0], nfcGpioPins[1], nfcGpioPins[2], nfcGpioPins[3]); 
    m_nfc = new PN532 (*m_pn532spi);
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
    uint32_t versiondata = m_nfc->getFirmwareVersion();
    if (!versiondata) {
        ESP_LOGE(TAG, "Error establishing PN532 connection.");
        m_nfc->stop();
        return false;
    }
    ESP_LOGI(TAG, "Found chip PN5%x, Firmware ver. %d.%d", (versiondata >> 24) & 0xFF, (versiondata >> 16) & 0xFF, (versiondata >> 8) & 0xFF);
    m_nfc->SAMConfig();
    m_nfc->setRFField(0x02, 0x01);
    m_nfc->setPassiveActivationRetries(0);
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
        if (!m_nfc->writeRegister(0x633d, 0, true)) {
            ESP_LOGE(TAG, "PN532 is unresponsive. Attempting to reconnect...");
            m_nfc->stop();
            startRetryTask();
            vTaskSuspend(NULL);
            continue;
        }

        uint8_t res[4];
        uint16_t resLen = 4;
        m_nfc->inCommunicateThru(m_ecpData.data(), m_ecpData.size(), res, &resLen, 0, true);

        uint8_t *uid = new uint8_t[16];
        uint8_t *uidLen = new uint8_t[1];
        uint8_t *atqa = new uint8_t[2];
        uint8_t *sak = new uint8_t[1];
        bool passiveTargetFound = m_nfc->readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, uidLen, atqa, sak, 200, true, true);
        
        if (passiveTargetFound) {
            ESP_LOGI(TAG, "NFC tag detected!");
            m_nfc->setPassiveActivationRetries(5);
            handleTagPresence();
            waitForTagRemoval();
            m_nfc->setPassiveActivationRetries(0);
        }
        delete[] uid;
        delete[] uidLen;
        delete[] atqa;
        delete[] sak;

        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

/**
 * @brief Handles a detected NFC tag by attempting to select the HomeKey applet and processing the tag accordingly.
 *
 * Attempts to select the HomeKey applet on the tag via an APDU select command; if selection succeeds, proceeds with
 * HomeKey authentication handling. If selection fails, reads the tag's UID/ATQA/SAK and processes it as a generic ISO14443A tag.
 * Logs the tag processing duration and releases the PN532 device state before returning.
 */
void NfcManager::handleTagPresence() {
    auto startTime = std::chrono::high_resolution_clock::now();
    uint8_t selectAppletCmd[] = { 0x00, 0xA4, 0x04, 0x00, 0x07, 0xA0, 0x00, 0x00, 0x08, 0x58, 0x01, 0x01, 0x00 };
    uint8_t response[64];
    uint16_t responseLength = sizeof(response);

    bool status = m_nfc->inDataExchange(selectAppletCmd, sizeof(selectAppletCmd), response, &responseLength);

    // Check for success SW1=0x90, SW2=0x00
    if (status && responseLength >= 2 && response[responseLength - 2] == 0x90 && response[responseLength - 1] == 0x00) {
        ESP_LOGI(TAG, "HomeKey applet selected successfully.");
        handleHomeKeyAuth();
    } else {
        ESP_LOGI(TAG, "Not a HomeKey tag, or failed to select applet.");
        uint8_t uid[16], uidLen = 0, atqa[2], sak[1];
        m_nfc->readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLen, atqa, sak);
        handleGenericTag(uid, uidLen, atqa, sak);
    }
    
    auto stopTime = std::chrono::high_resolution_clock::now();
    ESP_LOGI(TAG, "Total processing time: %lli ms", std::chrono::duration_cast<std::chrono::milliseconds>(stopTime - startTime).count());
    
    m_nfc->inRelease();
}

/**
 * @brief Attempts HomeKey authentication for the currently-present NFC tag.
 *
 * Performs authentication using the manager's configured authentication flow and the current reader data.
 * On successful authentication, updates stored reader data and publishes a HOMEKEY_TAP event containing
 * success status, issuer ID, endpoint ID, and reader ID. On failure, publishes a HOMEKEY_TAP event indicating
 * authentication failure.
 *
 * @note This method has side effects: it may update ReaderDataManager and will publish an event via EventManager.
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
                uint8_t sendLength = static_cast<uint8_t>(send.size());
                uint8_t responseBuffer[256];
                uint16_t responseLength = sizeof(responseBuffer);
                bool ok = m_nfc->inDataExchange(send.data(), sendLength, responseBuffer, &responseLength, isLong);
                if (ok) {
                    recv.assign(responseBuffer, responseBuffer + responseLength);
                }
                return ok;
            };
        std::function<void(const readerData_t&)> saveFn = [this](const readerData_t& data) {
            m_readerDataManager.updateReaderData(data);
            invalidateAuthCache();
        };

        HKAuthenticationContext authCtx(nfcFn, readerData, saveFn);
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
void NfcManager::handleGenericTag(const uint8_t* uid, uint8_t uidLen, const uint8_t* atqa, const uint8_t* sak) {
    EventTagTap s{.uid = std::vector<uint8_t>(uid, uid+uidLen), .atqa = std::vector<uint8_t>(atqa, atqa + 2), .sak = std::vector<uint8_t>(sak, sak + 1)};
    std::vector<uint8_t> d;
    alpaca::serialize(s, d);
    NfcEvent event{.type=TAG_TAP, .data=d};
    std::vector<uint8_t> event_data;
    alpaca::serialize(event, event_data);
    event_bus.publish({event_bus.get_topic(NFC_BUS_TOPIC).value_or(EventBus::INVALID_TOPIC), 0, event_data.data(), event_data.size()});
}

/**
 * @brief Waits for an NFC tag to be removed from the reader's RF field and recovers if it remains.
 *
 * Polls the reader for tag presence up to 50 times with 50 ms intervals, calling inRelease between checks.
 * If the tag remains after the timeout, forces an RF field reset (toggle off/on) to clear the tag state.
 * Always issues a final inRelease before returning.
 */
void NfcManager::waitForTagRemoval() {
    ESP_LOGD(TAG, "Waiting for tag to be removed from field...");
    int counter = 50;
    bool deviceStillInField = m_nfc->inListPassiveTarget();

    while (deviceStillInField && counter > 0) {
      vTaskDelay(pdMS_TO_TICKS(50));
      m_nfc->inRelease();
      deviceStillInField = m_nfc->inListPassiveTarget();
      counter--;
    }
    
    if (deviceStillInField) {
        ESP_LOGW(TAG, "Tag was not removed from field, forcing RF field reset.");
        m_nfc->setRFField(0x02, 0x00);
        vTaskDelay(pdMS_TO_TICKS(100));
        m_nfc->setRFField(0x02, 0x01);
    } else {
        ESP_LOGD(TAG, "Tag removed.");
    }
    m_nfc->inRelease();
}
