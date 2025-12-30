#include <event_manager.hpp>
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
#include <system_error>
#include <serialization.hpp>

const char* NfcManager::TAG = "NfcManager";

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
NfcManager::NfcManager(ReaderDataManager& readerDataManager,const std::array<uint8_t, 4> &nfcGpioPins)
    : nfcGpioPins(nfcGpioPins),
      m_readerDataManager(readerDataManager),
      m_pollingTaskHandle(nullptr),
      m_retryTaskHandle(nullptr),
      m_ecpData({ 0x6A, 0x2, 0xCB, 0x2, 0x6, 0x2, 0x11, 0x0 })
{
  espp::EventManager::get().add_publisher("nfc/event", "NfcManager");
  espp::EventManager::get().add_subscriber("homekit/internal", "NfcManager", [&](const std::vector<uint8_t> &data){
    std::error_code ec;
    HomekitEvent event = alpaca::deserialize<HomekitEvent>(data, ec);
    if(ec) return;
    switch(event.type) {
      case ACCESSDATA_CHANGED:
        updateEcpData();
        break;
      case DEBUG_AUTH_FLOW: {
        EventValueChanged s = alpaca::deserialize<EventValueChanged>(event.data, ec);
        if(!ec){
          authFlow = KeyFlow(s.newValue);
        }
        break;
      }
      default:
        break;
    }
  }, 3072);
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
    const auto& readerGid = m_readerDataManager.getReaderGid();
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
    ESP_LOGI(TAG, "Found chip PN5%x, Firmware ver. %u.%u", (unsigned int)((versiondata >> 24) & 0xFF), (unsigned int)((versiondata >> 16) & 0xFF), (unsigned int)((versiondata >> 8) & 0xFF));
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
        bool passiveTargetFound = m_nfc->readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, uidLen, atqa, sak, 500, true, true);
        
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

        vTaskDelay(pdMS_TO_TICKS(50));
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
    auto transceiveLambda = [this](std::vector<uint8_t>& send, std::vector<uint8_t>& recv, bool isLong) -> bool {
        uint8_t responseBuffer[256];
        uint16_t responseLength = sizeof(responseBuffer);
        bool status = m_nfc->inDataExchange(send.data(), send.size(), responseBuffer, &responseLength, isLong);
        if (status) recv.assign(responseBuffer, responseBuffer + responseLength);
        return status;
    };

    auto save_cb = [this](const readerData_t& data){
      m_readerDataManager.updateReaderData(data);
    };

    auto readerData = m_readerDataManager.getReaderData();
    HKAuthenticationContext authCtx(transceiveLambda, readerData, save_cb);
    auto authResult = authCtx.authenticate(authFlow);

    if (std::get<2>(authResult) != kFlowFailed) {
        ESP_LOGI(TAG, "HomeKey authentication successful!");
        EventHKTap s{.status = true, .issuerId = std::get<0>(authResult), .endpointId = std::get<1>(authResult), .readerId = readerData.reader_id };
        std::vector<uint8_t> d;
        alpaca::serialize(s, d);
        NfcEvent event{.type=HOMEKEY_TAP, .data=d};
        std::vector<uint8_t> event_data;
        alpaca::serialize(event, event_data);
        espp::EventManager::get().publish("nfc/event", event_data);
    } else {
        ESP_LOGW(TAG, "HomeKey authentication failed.");
        EventHKTap s{.status = false, .issuerId = {}, .endpointId = {}, .readerId = {} };
        std::vector<uint8_t> d;
        alpaca::serialize(s, d);
        NfcEvent event{.type=HOMEKEY_TAP, .data=d};
        std::vector<uint8_t> event_data;
        alpaca::serialize(event, event_data);
        espp::EventManager::get().publish("nfc/event", event_data);
    }
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
    espp::EventManager::get().publish("nfc/event", event_data);
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
