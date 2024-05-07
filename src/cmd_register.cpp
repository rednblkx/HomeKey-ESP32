#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "esp_log.h"
#include "esp_console.h"
#include "esp_system.h"
#include "argtable3/argtable3.h"

static void register_nfc_debug();

void register_system()
{
    register_nfc_debug();
}

int debug_logs(int argc, char** argv) {
  const char* TAG = "";
  if (argc > 1) {
    ESP_LOGI("CMD", "%s", argv[1]);
    switch (*argv[1])
    {
    case 'n':
      if (!strcmp(argv[1], "nd")) {
        ESP_LOGI("CMD", "PN532");
        TAG = "PN532";
      }
      else if (!strcmp(argv[1], "nsd")) {
        ESP_LOGI("CMD", "PN532_SPI");
        TAG = "PN532_SPI";
      }
      break;
    case 'h':
      if (!strcmp(argv[1], "hk")) {
        ESP_LOGI("CMD", "HK-HomeKit");
        TAG = "HK_HomeKit";
      }
      else if (!strcmp(argv[1], "hkc")) {
        ESP_LOGI("CMD", "HK-CTX");
        TAG = "HKAuthCtx";
      }
      else if (!strcmp(argv[1], "hkf")) {
        ESP_LOGI("CMD", "HK-FA");
        TAG = "HKFastAuth";
      }
      else if (!strcmp(argv[1], "hks")) {
        ESP_LOGI("CMD", "HK-STD");
        TAG = "HKStdAuth";
      }
      else if (!strcmp(argv[1], "hka")) {
        ESP_LOGI("CMD", "HK-Attest");
        TAG = "HKAttestAuth";
      }
      break;
    case 'i':
      if (!strcmp(argv[1], "iso")) {
        ESP_LOGI("CMD", "ISO18013 CTX");
        TAG = "ISO18013_SC";
      }
      break;
    default:
      break;
    }
  }
  esp_log_level_t set_level = esp_log_level_get(TAG);
  esp_log_level_t target_level = ESP_LOG_DEBUG;
  if (argc == 3) {
    ESP_LOGI("CMD", "%s", argv[2]);
    switch (*argv[2])
    {
    case '0':
      ESP_LOGI("CMD", "INFO");
      target_level = ESP_LOG_INFO;
      break;
    case '1':
      target_level = ESP_LOG_DEBUG;
      ESP_LOGI("CMD", "DEBUG");
      break;
    case '2':
      target_level = ESP_LOG_VERBOSE;
      ESP_LOGI("CMD", "VERBOSE");
      break;
    default:
      break;
    }
  }
  esp_log_level_set(TAG, target_level);
  return ESP_OK;
}

void register_nfc_debug() {
  const esp_console_cmd_t cmd = {
    .command = "dbg",
    .help = "Enable debug logs for various HK and PN532 components",
    .hint = "\n nd - PN532 HAL\n nsd - PN532 SPI\n hk - HomeKey to HAP communications\n hkc - HomeKey Auth Context\n hkf - HomeKey Fast Auth\n hks - HomeKey STD Auth\n hka - HomeKey Attestation Auth\n iso - ISO18013 Secure Context",
    .func = &debug_logs,
  };
  ESP_ERROR_CHECK( esp_console_cmd_register(&cmd) );
}