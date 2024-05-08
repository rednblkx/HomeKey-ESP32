#include <hkAuthContext.h>
#include <HomeKey.h>
#include "pb_encode.h"
#include "pb_decode.h"
#include "HomeKeyData.pb.h"
#include <utils.h>
#include <chrono>
#include <HK_HomeKit.h>
#include <config.h>
#include <mqtt_client.h>
#include <esp_ota_ops.h>
#include "nvs_flash.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "sdkconfig.h"
#include <PN532_SPI.h>
#include <PN532.h>
#include <hap.h>
#include <hap_apple_servs.h>
#include <hap_apple_chars.h>
#include <hap_fw_upgrade.h>
#include <app_wifi.h>
#include <app_hap_setup_payload.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <pb_common.h>
#include "console.h"

const char* TAG = "MAIN";

enum lockStates
{
  UNLOCKED,
  LOCKED,
  JAMMED,
  UNKNOWN,
  UNLOCKING,
  LOCKING
};

#define BLINK_GPIO CONFIG_BLINK_GPIO

#define PN532_SCK CONFIG_PN532_SCK
#define PN532_MOSI CONFIG_PN532_MOSI
#define PN532_SS CONFIG_PN532_SS
#define PN532_MISO CONFIG_PN532_MISO

#define LOCK_TASK_PRIORITY  1
#define LOCK_TASK_STACKSIZE 4 * 1024
#define LOCK_TASK_NAME      "hap_lock"

PN532_SPI pn532spi;
PN532 nfc(pn532spi);

nvs_handle savedData;
HomeKeyData_ReaderData readerData = HomeKeyData_ReaderData_init_zero;
KeyFlow hkFlow = KeyFlow::kFlowFAST;
esp_mqtt_client_handle_t client = nullptr;

bool save_to_nvs() {
  uint8_t *buffer = (uint8_t *)malloc(HomeKeyData_ReaderData_size);
  pb_ostream_t ostream = pb_ostream_from_buffer(buffer, HomeKeyData_ReaderData_size);
  bool encodeStatus = pb_encode(&ostream, &HomeKeyData_ReaderData_msg, &readerData);
  LOG(I, "PB ENCODE STATUS: %d", encodeStatus);
  LOG(I, "PB BYTES WRITTEN: %d", ostream.bytes_written);
  esp_err_t set_nvs = nvs_set_blob(savedData, "READERDATA", buffer, ostream.bytes_written);
  esp_err_t commit_nvs = nvs_commit(savedData);
  LOG(D, "NVS SET STATUS: %s", esp_err_to_name(set_nvs));
  LOG(D, "NVS COMMIT STATUS: %s", esp_err_to_name(commit_nvs));
  free(buffer);
  return !set_nvs && !commit_nvs;
}

/* Mandatory identify routine for the accessory.
 * In a real accessory, something like LED blink should be implemented
 * got visual identification
 */
static int lock_identify(hap_acc_t *ha)
{
    ESP_LOGI(TAG, "Accessory identified");
    return HAP_SUCCESS;
}

// Function to calculate CRC16
void crc16a(unsigned char* data, unsigned int size, unsigned char* result) {
  unsigned short w_crc = 0x6363;

  for (unsigned int i = 0; i < size; ++i) {
    unsigned char byte = data[i];
    byte = (byte ^ (w_crc & 0x00FF));
    byte = ((byte ^ (byte << 4)) & 0xFF);
    w_crc = ((w_crc >> 8) ^ (byte << 8) ^ (byte << 3) ^ (byte >> 4)) & 0xFFFF;
  }

  result[0] = static_cast<unsigned char>(w_crc & 0xFF);
  result[1] = static_cast<unsigned char>((w_crc >> 8) & 0xFF);
}

// Function to append CRC16 to data
void with_crc16(unsigned char* data, unsigned int size, unsigned char* result) {
  crc16a(data, size, result);
}

void deleteReaderData() {
  memset(&readerData, 0, sizeof(readerData));
  esp_err_t erase_nvs = nvs_erase_key(savedData, "READERDATA");
  esp_err_t commit_nvs = nvs_commit(savedData);
  LOG(D,"*** NVS W STATUS");
  LOG(D,"ERASE: %s", esp_err_to_name(erase_nvs));
  LOG(D,"COMMIT: %s", esp_err_to_name(commit_nvs));
  LOG(D,"*** NVS W STATUS");
}

void hap_event_handler(hap_event_t event, void* data) {
  if (event == HAP_EVENT_CTRL_PAIRED) {
    char* ctrl_id = (char*)data;
    hap_ctrl_data_t* ctrl = hap_get_controller_data(ctrl_id);
    if (ctrl->valid) {
      std::vector<uint8_t> id = utils::getHashIdentifier(ctrl->info.ltpk, 32, true);
      ESP_LOG_BUFFER_HEX(TAG, ctrl->info.ltpk, 32);
      LOG(D, "Found allocated controller - Hash: %s", utils::bufToHexString(id.data(), 8).c_str());
      HomeKeyData_KeyIssuer* foundIssuer = nullptr;
      for (auto& issuer : readerData.issuers) {
        if (!memcmp(issuer.issuer_id, id.data(), 8)) {
          LOG(D, "Issuer %s already added, skipping", utils::bufToHexString(issuer.issuer_id, 8).c_str());
          foundIssuer = &issuer;
          break;
        }
      }
      if (foundIssuer == nullptr) {
        LOG(D, "Adding new issuer - ID: %s", utils::bufToHexString(id.data(), 8).c_str());
        memcpy(readerData.issuers[readerData.issuers_count].issuer_id, id.data(), 8);
        memcpy(readerData.issuers[readerData.issuers_count].issuer_pk, ctrl->info.ltpk, 32);
        readerData.issuers_count++;
        save_to_nvs();
      }
    }
  }
  else if (event == HAP_EVENT_CTRL_UNPAIRED) {
    int ctrl_count = hap_get_paired_controller_count();
    if (ctrl_count == 0) {
      deleteReaderData();
    }
    else {
      char* ctrl_id = (char*)data;
      hap_ctrl_data_t* ctrl = hap_get_controller_data(ctrl_id);
      if (ctrl->valid) {
      // readerData.issuers.erase(std::remove_if(readerData.issuers.begin(), readerData.issuers.end(),
      //   [ctrl](HomeKeyData_KeyIssuer x) {
      //     std::vector<uint8_t> id = utils::getHashIdentifier(ctrl->info.ltpk, 32, true);
      //     LOG(D, "Found allocated controller - Hash: %s", utils::bufToHexString(id.data(), 8).c_str());
      //     if (!memcmp(x.publicKey, id.data(), 8)) {
      //       return false;
      //     }
      //     LOG(D, "Issuer ID: %s - Associated controller was removed from Home, erasing from reader data.", utils::bufToHexString(x.issuerId, 8).c_str());
      //     return true;
      //   }),
      //   readerData.issuers.end());
      }
    }
  }
}

/* Callback for handling writes on the LockMechanism Service
 */
int lockMechanism_write(hap_write_data_t write_data[], int count,
        void *serv_priv, void *write_priv)
{
    int i, ret = HAP_SUCCESS;
    hap_write_data_t *write;
    for (i = 0; i < count; i++) {
        write = &write_data[i];
        /* Setting a default error value */
        *(write->status) = HAP_STATUS_VAL_INVALID;
        if (!strcmp(hap_char_get_type_uuid(write->hc), HAP_CHAR_UUID_LOCK_CURRENT_STATE)) {
            ESP_LOGI(TAG, "Received Write for Lock Current State %s", write->val.b ? "Unlocked" : "Locked");
                *(write->status) = HAP_STATUS_SUCCESS;
        } else if (!strcmp(hap_char_get_type_uuid(write->hc), HAP_CHAR_UUID_LOCK_TARGET_STATE)) {
            ESP_LOGI(TAG, "Received Write for Lock Target State %s", write->val.b ? "Unlocked" : "Locked");
                *(write->status) = HAP_STATUS_SUCCESS;
        } else {
            *(write->status) = HAP_STATUS_RES_ABSENT;
        }
        /* If the characteristic write was successful, update it in hap core
         */
        if (*(write->status) == HAP_STATUS_SUCCESS) {
            hap_char_update_val(write->hc, &(write->val));
        } else {
            /* Else, set the return value appropriately to report error */
            ret = HAP_FAIL;
        }
    }
    return ret;
}

uint8_t tlv8_data[128];

/* Callback for handling writes on the NFC Access Service
 */
int nfcAccess_write(hap_write_data_t write_data[], int count,
        void *serv_priv, void *write_priv)
{
  LOG(D, "PROVISIONED READER KEY: %s", utils::bufToHexString(readerData.reader_sk, sizeof(readerData.reader_sk)).c_str());
  LOG(D, "READER PUBLIC KEY: %s", utils::bufToHexString(readerData.reader_pk, sizeof(readerData.reader_pk)).c_str());
  LOG(D, "READER GROUP IDENTIFIER: %s", utils::bufToHexString(readerData.reader_group_id, sizeof(readerData.reader_group_id)).c_str());
  LOG(D, "READER UNIQUE IDENTIFIER: %s", utils::bufToHexString(readerData.reader_id, sizeof(readerData.reader_id)).c_str());
  int i, ret = HAP_SUCCESS;
    hap_write_data_t *write;
    for (i = 0; i < count; i++) {
        write = &write_data[i];
        /* Setting a default error value */
        *(write->status) = HAP_STATUS_VAL_INVALID;
        if (!strcmp(hap_char_get_type_uuid(write->hc), HAP_CHAR_UUID_NFC_ACCESS_CONTROL_POINT)) {
          hap_tlv8_val_t buf = write->val.t;
          esp_log_buffer_hex_internal(TAG, buf.buf, buf.buflen, ESP_LOG_INFO);
          HK_HomeKit ctx(readerData, savedData, "READERDATA");
          auto result = ctx.processResult(std::vector<uint8_t>(buf.buf, buf.buf + buf.buflen));
          memcpy(tlv8_data, result.data(), result.size());
          hap_val_t new_val;
          new_val.t.buf = tlv8_data;
          new_val.t.buflen = result.size();
          hap_char_update_val(write->hc, &new_val);
          *(write->status) = HAP_STATUS_SUCCESS;
        } else {
            *(write->status) = HAP_STATUS_RES_ABSENT;
        }
    }
    return ret;
}

void nfc_thread_entry(void* arg) {
  esp_log_level_set("PN532", ESP_LOG_VERBOSE);
  esp_log_level_set("PN532_SPI", ESP_LOG_VERBOSE);

  nfc.begin();

  uint32_t versiondata = nfc.getFirmwareVersion();
  if (!versiondata) {
    ESP_LOGE("NFC_SETUP", "Didn't find PN53x board");
  }
  else {
    unsigned int model = (versiondata >> 24) & 0xFF;
    ESP_LOGI("NFC_SETUP", "Found chip PN5%x", model);
    int maj = (versiondata >> 16) & 0xFF;
    int min = (versiondata >> 8) & 0xFF;
    ESP_LOGI("NFC_SETUP", "Firmware ver. %d.%d", maj, min);
    nfc.SAMConfig();
    nfc.setPassiveActivationRetries(0);
    ESP_LOGI("NFC_SETUP", "Waiting for an ISO14443A card");
  }
  uint8_t ecpData[18] = { 0x6A, 0x2, 0xCB, 0x2, 0x6, 0x2, 0x11, 0x0 };
  memcpy(ecpData + 8, readerData.reader_group_id, sizeof(readerData.reader_group_id));
  with_crc16(ecpData, 16, ecpData + 16);
  nfc.writeRegister(0x633d, 0);
  while (1) {
    uint8_t uid[16];
    uint8_t uidLen = 0;
    uint16_t atqa[1];
    uint8_t sak[1];
    bool passiveTarget = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLen, atqa, sak, 1000, true, true);
    if (passiveTarget) {
      LOG(D, "ATQA: %02x", atqa[0]);
      LOG(D, "SAK: %02x", sak[0]);
      ESP_LOG_BUFFER_HEX(TAG, uid, (size_t)uidLen);
      LOG(I, "*** PASSIVE TARGET DETECTED ***");
      uint8_t data[13] = { 0x00, 0xA4, 0x04, 0x00, 0x07, 0xA0, 0x00, 0x00, 0x08, 0x58, 0x01, 0x01, 0x0 };
      uint8_t selectCmdRes[9];
      uint16_t selectCmdResLength = 9;
      LOG(D, "SELECT HomeKey Applet, APDU: ");
      esp_log_buffer_hex_internal(TAG, data, sizeof(data), ESP_LOG_INFO);
      bool status = nfc.inDataExchange(data, sizeof(data), selectCmdRes, &selectCmdResLength);
      LOG(D, "SELECT HomeKey Applet, Response");
      esp_log_buffer_hex_internal(TAG, selectCmdRes, selectCmdResLength, ESP_LOG_INFO);
      if (status && selectCmdRes[selectCmdResLength - 2] == 0x90 && selectCmdRes[selectCmdResLength - 1] == 0x00) {
        LOG(D, "*** SELECT HOMEKEY APPLET SUCCESSFUL ***");
        LOG(D, "Reader Private Key: %s", utils::bufToHexString((const uint8_t*)readerData.reader_pk, sizeof(readerData.reader_pk)).c_str());
        HKAuthenticationContext authCtx(nfc, readerData, savedData);
        auto authResult = authCtx.authenticate(hkFlow);
        if (std::get<2>(authResult) != kFlowFailed) {
          LOG(D, "!!!!!!!!!!!!AUTHENTICATED!!!!!!!!!!!!!");
        }
      }
      vTaskDelay(100 / portTICK_PERIOD_MS);
      nfc.inRelease();
      nfc.setPassiveActivationRetries(10);
      bool deviceStillInField = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLen);
      LOG(I, "Target still present: %d", deviceStillInField);
      while (deviceStillInField) {
        vTaskDelay(100 / portTICK_PERIOD_MS);
        nfc.inRelease();
        deviceStillInField = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLen);
        LOG(I, "Target still present: %d", deviceStillInField);
      }
      nfc.inRelease();
      nfc.setPassiveActivationRetries(0);
    }
    else {
      nfc.ecpBroadcast(ecpData, sizeof(ecpData));
      vTaskDelay(50 / portTICK_PERIOD_MS);
    }
  }
}

uint8_t buf1[6] = { 0x01, 0x01, 0x10, 0x02, 0x01, 0x10 };
hap_tlv8_val_t nfcSupportedConf = {
  .buf = buf1,
  .buflen = 6
};

/*The main thread for handling the Lock Accessory */
void lock_thread_entry(void *arg)
{
    hap_acc_t *accessory;
    hap_serv_t *lockManagementService;
    hap_serv_t *lockMechanismService;
    hap_serv_t *nfcAccessService;

    /* Initialize the HAP core */
    hap_init(HAP_TRANSPORT_WIFI);

    /* Initialise the mandatory parameters for Accessory which will be added as
     * the mandatory services internally
     */
    uint8_t buf[6] = { 0x01, 0x04, 0x0, 0x0, 0x0, 0x0 };
    hap_tlv8_val_t hw_finish = {
      .buf = buf,
      .buflen = 6
    };
    hap_acc_cfg_t cfg = {
        .name = "Lock",
        .model = "HomeKey-ESP32",
        .manufacturer = "rednblkx",
        .serial_num = "abcdefg",
        .fw_rev = "0.3.0",
        .hw_rev = "1.0",
        .hw_finish = &hw_finish,
        .pv = "1.1.0",
        .cid = HAP_CID_LOCK,
        .identify_routine = lock_identify,
    };

    /* Create accessory object */
    accessory = hap_acc_create(&cfg);
    if (!accessory) {
        ESP_LOGE(TAG, "Failed to create accessory");
        hap_acc_delete(accessory);
        vTaskDelete(NULL);
    }

    /* Add a dummy Product Data */
    uint8_t product_data[] = {'E','S','P','3','2','H','A','P'};
    hap_acc_add_product_data(accessory, product_data, sizeof(product_data));

    /* Add Wi-Fi Transport service required for HAP Spec R16 */
    hap_acc_add_wifi_transport_service(accessory, 0);

    hap_tlv8_val_t management;
    management.buf = (uint8_t*)"";
    management.buflen = 1;
    /* Create the Lock Management Service. */
    lockManagementService = hap_serv_lock_management_create(&management, "1.0.0");
    if (!lockManagementService) {
        ESP_LOGE(TAG, "Failed to create Lock Management Service");
        hap_acc_delete(accessory);
        vTaskDelete(NULL);
    }

    lockMechanismService = hap_serv_lock_mechanism_create(1, 1);
    if (!lockMechanismService) {
        ESP_LOGE(TAG, "Failed to create Lock Mechanism Service");
        hap_acc_delete(accessory);
        vTaskDelete(NULL);
    }
    /* Create the NFC Access Service. */
    nfcAccessService = hap_serv_nfc_access_create(0, NULL, &nfcSupportedConf);
    if (!nfcAccessService) {
        ESP_LOGE(TAG, "Failed to create NFC Access Service");
        hap_acc_delete(accessory);
        vTaskDelete(NULL);
    }
    /* Set the write callback for the service */
    hap_serv_set_write_cb(lockMechanismService, lockMechanism_write);
    hap_serv_set_write_cb(nfcAccessService, nfcAccess_write);
    
    /* Add the services to the Accessory Object */
    hap_acc_add_serv(accessory, lockManagementService);
    hap_acc_add_serv(accessory, lockMechanismService);
    hap_acc_add_serv(accessory, nfcAccessService);

    /* Add the Accessory to the HomeKit Database */
    hap_add_accessory(accessory);
    /* Unique Setup code of the format xxx-xx-xxx. Default: 111-22-333 */
    hap_set_setup_code("111-22-333");
    /* Unique four character Setup Id. Default: ES32 */
    hap_set_setup_id("ES32");

    /* Initialize Wi-Fi */
    app_wifi_init();

    hap_register_event_handler(hap_event_handler);

    /* After all the initializations are done, start the HAP core */
    /* Start Wi-Fi */
    app_wifi_start(portMAX_DELAY);
    // hap_http_debug_enable();
    hap_start();

    console_init();
    /* The task ends here. The read/write callbacks will be invoked by the HAP Framework */
    vTaskDelete(NULL);
}

#ifdef __cplusplus
extern "C"
#endif
void app_main(void){
  ESP_LOGI(TAG, "[APP] Startup..");
  ESP_LOGI(TAG, "[APP] Free memory: %" PRIu32 " bytes", esp_get_free_heap_size());
  ESP_LOGI(TAG, "[APP] IDF version: %s", esp_get_idf_version());
  ESP_LOGI(TAG, "%s", esp_err_to_name(nvs_flash_init()));
  esp_log_level_set(TAG, ESP_LOG_DEBUG);
  xTaskCreate(lock_thread_entry, LOCK_TASK_NAME, LOCK_TASK_STACKSIZE,
    NULL, LOCK_TASK_PRIORITY, NULL);
  const esp_app_desc_t* app_desc = esp_app_get_description();
  std::string app_version = app_desc->version;
  size_t len;
  const char* TAG = "SETUP";
  esp_log_level_set(TAG, ESP_LOG_DEBUG);
  nvs_open("SAVED_DATA", NVS_READWRITE, &savedData);
  if (!nvs_get_blob(savedData, "READERDATA", NULL, &len)) {
    uint8_t savedBuf[len];
    pb_istream_t istream;
    nvs_get_blob(savedData, "READERDATA", savedBuf, &len);
    LOG(I, "NVS DATA LENGTH: %d", len);
    ESP_LOG_BUFFER_HEX_LEVEL(TAG, savedBuf, len, ESP_LOG_DEBUG);
    istream = pb_istream_from_buffer(savedBuf, len);
    bool decodeStatus = pb_decode(&istream, &HomeKeyData_ReaderData_msg, &readerData);
    LOG(I, "PB DECODE STATUS: %d", decodeStatus);
    if (!decodeStatus) {
      LOG(I, "PB ERROR: %s", PB_GET_ERROR(&istream));
    }
  }

  LOG(D, "READER GROUP ID (%d): %s", strlen((const char*)readerData.reader_group_id), utils::bufToHexString(readerData.reader_group_id, sizeof(readerData.reader_group_id)).c_str());
  LOG(D, "READER UNIQUE ID (%d): %s", strlen((const char*)readerData.reader_id), utils::bufToHexString(readerData.reader_id, sizeof(readerData.reader_id)).c_str());

  LOG(I, "HOMEKEY ISSUERS: %d", readerData.issuers_count);
  for (auto* issuer = readerData.issuers; issuer != (readerData.issuers + readerData.issuers_count); ++issuer) {
    LOG(D, "Issuer ID: %s, Public Key: %s", utils::bufToHexString(issuer->issuer_id, sizeof(issuer->issuer_id)).c_str(), utils::bufToHexString(issuer->issuer_pk, sizeof(issuer->issuer_pk)).c_str());
  }

  xTaskCreate(nfc_thread_entry, "nfc_task", 8192, NULL, 2, NULL);
 }