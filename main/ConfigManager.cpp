#include "ConfigManager.hpp"
#include "MbedtlsHelpers.hpp"
#include "cJSON.h"
#include "config.hpp"
#include <cstddef>
#include <cstring>
#include <cstdint>
#include <string>
#include <vector>
#include <limits>
#include "esp_err.h"
#include "esp_log.h"
#include "esp_log_level.h"
#include "fmt/ranges.h"
#include "mbedtls/x509.h"
#define MSGPACK_SBUFFER_INIT_SIZE 2048
#include "msgpack.h"
#include <LittleFS.h>
#include "fmt/format.h"
#include "msgpack/sbuffer.h"
#include "nvs.h"
#include <mbedtls/entropy.h>
#include <mbedtls/ctr_drbg.h>
#include <mutex>
#include <mbedtls/x509_crt.h>
#include <mbedtls/pk.h>
#include <mbedtls/error.h>
#include <mbedtls/pem.h>


const char* ConfigManager::TAG = "ConfigManager";

using crypto::ScopedEntropy;
using crypto::ScopedCtrDrbg;
using crypto::ScopedPk;
using crypto::ScopedX509Crt;

/**
 * @brief Initialize ConfigManager.
 *
 * Field metadata now lives in the compile-time kConfigFields table (.rodata);
 * the constructor only marks the manager uninitialized.
 */
ConfigManager::ConfigManager() : m_isInitialized(false) {
}

// ---------------------------------------------------------------------------
// Static config field table
//
// Sections are laid out in this order; findField() and sectionRange() rely on
// the contiguity of each section's entries. Offsets are byte offsets of the
// field inside the owning section struct, resolved at first use via
// sectionBase() — this keeps the whole table in .rodata with no heap or BSS.
// Section names are the public API names ("mqtt", "ssl", "https", "misc",
// "actions") used by every caller; the F() macro takes the struct type
// separately. loadConfigFromNvs() dispatches with exactly these strings.
// ---------------------------------------------------------------------------

// GCC supports offsetof on these non-standard-layout structs (std::map
// members); suppress the conditional-support warning for the table only.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Winvalid-offsetof"
#define OFF(s, m) (offsetof(espConfig::s, m))
#define F(str, s, m, type) { str, #m, uint16_t(OFF(s, m)), FieldType::type }

const ConfigManager::ConfigField ConfigManager::kConfigFields[ConfigManager::kFieldCount] = {
  // --- mqtt (22) ---
  F("mqtt", mqttConfig_t, mqttBroker, Str),
  F("mqtt", mqttConfig_t, mqttPort, U16),
  F("mqtt", mqttConfig_t, mqttClientId, Str),
  F("mqtt", mqttConfig_t, mqttUsername, Str),
  F("mqtt", mqttConfig_t, mqttPassword, Str),
  F("mqtt", mqttConfig_t, hassMqttDiscoveryEnabled, Bool),
  F("mqtt", mqttConfig_t, lwtTopic, Str),
  F("mqtt", mqttConfig_t, hkTopic, Str),
  F("mqtt", mqttConfig_t, lockStateTopic, Str),
  F("mqtt", mqttConfig_t, lockStateCmd, Str),
  F("mqtt", mqttConfig_t, lockCStateCmd, Str),
  F("mqtt", mqttConfig_t, lockTStateCmd, Str),
  F("mqtt", mqttConfig_t, btrLvlCmdTopic, Str),
  F("mqtt", mqttConfig_t, hkAltActionTopic, Str),
  F("mqtt", mqttConfig_t, lockCustomStateTopic, Str),
  F("mqtt", mqttConfig_t, lockCustomStateCmd, Str),
  F("mqtt", mqttConfig_t, lockEnableCustomState, Bool),
  F("mqtt", mqttConfig_t, nfcTagNoPublish, Bool),
  F("mqtt", mqttConfig_t, useSSL, Bool),
  F("mqtt", mqttConfig_t, allowInsecure, Bool),
  F("mqtt", mqttConfig_t, customLockStates, U8Arr6),
  F("mqtt", mqttConfig_t, customLockActions, U8Arr2),
  // --- ssl (3) ---
  F("ssl", mqtt_ssl_t, caCert, Str),
  F("ssl", mqtt_ssl_t, clientCert, Str),
  F("ssl", mqtt_ssl_t, clientKey, Str),
  // --- https (3) ---
  F("https", https_certs_t, serverCert, Str),
  F("https", https_certs_t, privateKey, Str),
  F("https", https_certs_t, caCert, Str),
  // --- misc (39) ---
  F("misc", misc_config_t, deviceName, Str),
  F("misc", misc_config_t, otaPasswd, Str),
  F("misc", misc_config_t, hk_key_color, U8),
  F("misc", misc_config_t, setupCode, Str),
  F("misc", misc_config_t, lockAlwaysUnlock, Bool),
  F("misc", misc_config_t, lockAlwaysLock, Bool),
  F("misc", misc_config_t, hkAuthPrecomputeEnabled, Bool),
  F("misc", misc_config_t, nfcFastPollingEnabled, Bool),
  F("misc", misc_config_t, nfcReaderType, U8),
  F("misc", misc_config_t, nfcIrqPin, U8),
  F("misc", misc_config_t, nfcVenPin, U8),
  F("misc", misc_config_t, controlPin, U8),
  F("misc", misc_config_t, hsStatusPin, U8),
  F("misc", misc_config_t, webAuthEnabled, Bool),
  F("misc", misc_config_t, webUsername, Str),
  F("misc", misc_config_t, webPassword, Str),
  F("misc", misc_config_t, webHttpsEnabled, Bool),
  F("misc", misc_config_t, nfcGpioPins, Arr4),
  F("misc", misc_config_t, nfcPinsPreset, U8),
  F("misc", misc_config_t, btrLowStatusThreshold, U8),
  F("misc", misc_config_t, proxBatEnabled, Bool),
  F("misc", misc_config_t, ethernetEnabled, Bool),
  F("misc", misc_config_t, ethActivePreset, U8),
  F("misc", misc_config_t, ethPhyType, U8),
  F("misc", misc_config_t, ethSpiBus, U8),
  F("misc", misc_config_t, ethRmiiConfig, Arr5),
  F("misc", misc_config_t, ethSpiConfig, Arr7),
  F("misc", misc_config_t, overrideStrappingRestriction, Bool),
  F("misc", misc_config_t, accessPointPassword, Str),
  F("misc", misc_config_t, keypadEnabled, Bool),
  F("misc", misc_config_t, keypadLayout, U8),
  F("misc", misc_config_t, keypadRowPins, Arr5),
  F("misc", misc_config_t, keypadColumnPins, Arr4),
  F("misc", misc_config_t, keypadActiveLevel, U8),
  F("misc", misc_config_t, keypadDebounceTicks, U8),
  F("misc", misc_config_t, keypadDoorbellKey, U8),
  F("misc", misc_config_t, keypadMinCodeLength, U8),
  F("misc", misc_config_t, keypadMaxCodeLength, U8),
  F("misc", misc_config_t, keypadMaxCodes, U8),
  // --- actions (30) ---
  F("actions", actions_config_t, nfcNeopixelPin, U8),
  F("actions", actions_config_t, neoPixelType, U8),
  F("actions", actions_config_t, neopixelSuccessColor, ColorMap),
  F("actions", actions_config_t, neopixelFailureColor, ColorMap),
  F("actions", actions_config_t, neopixelSuccessTime, U16),
  F("actions", actions_config_t, neopixelFailTime, U16),
  F("actions", actions_config_t, neopixelTagEventTime, U16),
  F("actions", actions_config_t, neopixelTagEventColor, ColorMap),
  F("actions", actions_config_t, nfcSuccessPin, U8),
  F("actions", actions_config_t, nfcSuccessTime, U16),
  F("actions", actions_config_t, nfcSuccessHL, Bool),
  F("actions", actions_config_t, nfcFailPin, U8),
  F("actions", actions_config_t, nfcFailTime, U16),
  F("actions", actions_config_t, nfcFailHL, Bool),
  F("actions", actions_config_t, tagEventPin, U8),
  F("actions", actions_config_t, tagEventTimeout, U16),
  F("actions", actions_config_t, tagEventHL, Bool),
  F("actions", actions_config_t, gpioActionPin, U8),
  F("actions", actions_config_t, gpioActionLockState, Bool),
  F("actions", actions_config_t, gpioActionUnlockState, Bool),
  F("actions", actions_config_t, gpioActionMomentaryEnabled, U8),
  F("actions", actions_config_t, hkGpioControlledState, Bool),
  F("actions", actions_config_t, gpioActionMomentaryTimeout, U16),
  F("actions", actions_config_t, hkDumbSwitchMode, Bool),
  F("actions", actions_config_t, hkAltActionPin, U8),
  F("actions", actions_config_t, hkAltActionTimeout, U16),
  F("actions", actions_config_t, hkAltActionGpioState, U8),
  F("actions", actions_config_t, hkAltActionInitPin, U8),
  F("actions", actions_config_t, hkAltActionInitLedPin, U8),
  F("actions", actions_config_t, hkAltActionInitTimeout, U16),
};

void* ConfigManager::sectionBase(const char* section) {
  if (!strcmp(section, "mqtt")) return &m_mqttConfig;
  if (!strcmp(section, "ssl")) return &m_mqttSslConfig;
  if (!strcmp(section, "https")) return &m_httpsCertsConfig;
  if (!strcmp(section, "misc")) return &m_miscConfig;
  if (!strcmp(section, "actions")) return &m_actionsConfig;
  return nullptr;
}

ConfigManager::FieldRange ConfigManager::sectionRange(const char* section) {
  // Table is ordered by section; locate the [first, last) span.
  const ConfigField* first = nullptr;
  const ConfigField* last = kConfigFields + kFieldCount;
  for (size_t i = 0; i < kFieldCount; ++i) {
    if (!strcmp(kConfigFields[i].section, section)) {
      if (!first) first = &kConfigFields[i];
    } else if (first) {
      last = &kConfigFields[i];
      break;
    }
  }
  return {first, first ? last : first};
}

// Boot-time consistency guard: every entry's section must be resolvable by
// sectionBase(), and every sectionBase() name must occur in the table. A
// rename applied to one side only would otherwise surface as a null section
// base and crash on first config load — the failure mode the first version
// of this table shipped with.
void ConfigManager::verifyFieldTable() {
  static bool checked = false;
  if (checked) return;
  checked = true;
  const char* known[] = {"mqtt", "ssl", "https", "misc", "actions"};
  for (size_t i = 0; i < kFieldCount; ++i) {
    if (sectionBase(kConfigFields[i].section) == nullptr) {
      ESP_LOGE(TAG, "FATAL: field '%s' references unknown section '%s'",
               kConfigFields[i].name, kConfigFields[i].section);
      abort();
    }
  }
  for (const char* s : known) {
    bool found = false;
    for (size_t i = 0; i < kFieldCount && !found; ++i) {
      if (!strcmp(kConfigFields[i].section, s)) found = true;
    }
    if (!found) {
      ESP_LOGE(TAG, "FATAL: section '%s' has no table entries", s);
      abort();
    }
  }
}

const ConfigManager::ConfigField* ConfigManager::findField(
    const char* section, const char* name, size_t nameLen) const {
  for (const auto& f : kConfigFields) {
    if (strcmp(f.section, section) == 0 && strlen(f.name) == nameLen &&
        memcmp(f.name, name, nameLen) == 0) {
      return &f;
    }
  }
  return nullptr;
}
#undef F
#undef OFF
#pragma GCC diagnostic pop

/**
 * @brief Releases resources held by ConfigManager.
 *
 * Closes the open NVS handle if the manager was initialized.
 */
ConfigManager::~ConfigManager() {
  if (m_isInitialized) {
    nvs_close(m_nvsHandle);
    m_isInitialized = false;
    m_nvsHandle = 0; // Invalidate handle
  }
}

/**
 * @brief Initialize the NVS-backed configuration subsystem and load saved MQTT and miscellaneous settings.
 *
 * This opens the NVS namespace used for saved configuration, sets the internal initialized state,
 * and loads configuration blobs for "MQTTDATA" and "MISCDATA" into memory.
 *
 * @return true if initialization succeeded or was already performed, false if opening the NVS handle failed.
 */
bool ConfigManager::begin() {
  if (m_isInitialized) {
    ESP_LOGW(TAG, "ConfigManager already initialized.");
    return true;
  }

  ESP_LOGI(TAG, "Initializing...");

  verifyFieldTable();

  if(!m_nvsHandle){
    esp_err_t err = nvs_open("SAVED_DATA", NVS_READWRITE, &m_nvsHandle);
    if (err != ESP_OK) {
      ESP_LOGE(TAG, "Error (%04#x) opening NVS handle!", err);
      return false;
    }
  }

  nvs_stats_t nvs_stats;
  nvs_get_stats(NULL, &nvs_stats);
  ESP_LOGI(TAG,"Count: UsedEntries = (%lu), FreeEntries = (%lu), AvailableEntries = (%lu), AllEntries = (%lu)\n",
       nvs_stats.used_entries, nvs_stats.free_entries, nvs_stats.available_entries, nvs_stats.total_entries);

  m_isInitialized = true;

  ESP_LOGI(TAG, "Loading configurations from NVS...");
  loadConfigFromNvs("MQTTDATA");
  if (m_mqttConfig.useSSL) {
    loadConfigFromNvs("MQTTSSLDATA");
  }
  loadConfigFromNvs("MISCDATA");
  loadConfigFromNvs("HTTPSDATA");

  ESP_LOGI(TAG, "Initialization complete.");
  return true;
}

template <typename ConfigType>
/**
 * @brief Access the stored configuration corresponding to the requested ConfigType.
 *
 * Returns a reference to the internal configuration instance for the specified
 * template type.
 *
 * @tparam ConfigType The configuration struct type to retrieve. Supported values:
 *                    `espConfig::mqttConfig_t` or `espConfig::misc_config_t`.
 * @return const ConfigType& Reference to the matching in-memory configuration:
 *                           the MQTT configuration when `ConfigType` is
 *                           `espConfig::mqttConfig_t`, or the miscellaneous
 *                           configuration when `ConfigType` is
 *                           `espConfig::misc_config_t`.
 */
const ConfigType& ConfigManager::getConfig() const {
  using NonConstConfigType = std::remove_const_t<ConfigType>;

  if constexpr (std::is_same_v<NonConstConfigType, espConfig::mqttConfig_t>) {
    return m_mqttConfig;
  } else if constexpr (std::is_same_v<NonConstConfigType, espConfig::misc_config_t>) {
    return m_miscConfig;
  } else if constexpr (std::is_same_v<NonConstConfigType, espConfig::actions_config_t>) {
    return m_actionsConfig;
  }else {
    static_assert(std::is_void_v<ConfigType> && false, "Unsupported ConfigType for getConfig");
  }
}
template const espConfig::mqttConfig_t& ConfigManager::getConfig<espConfig::mqttConfig_t>() const;
template const espConfig::mqttConfig_t& ConfigManager::getConfig<espConfig::mqttConfig_t const>() const;
template const espConfig::misc_config_t& ConfigManager::getConfig<espConfig::misc_config_t>() const;
template const espConfig::misc_config_t& ConfigManager::getConfig<espConfig::misc_config_t const>() const;
template const espConfig::actions_config_t& ConfigManager::getConfig<espConfig::actions_config_t>() const;
template const espConfig::actions_config_t& ConfigManager::getConfig<espConfig::actions_config_t const>() const;

template <typename ConfigType>
/**
 * @brief Clears the in-memory configuration for the specified config type and removes its persisted NVS blob.
 *
 * The function resets the matching member configuration to its default/empty state and attempts to erase
 * the corresponding NVS entry ("MQTTDATA" for MQTT config, "MISCDATA" for misc config).
 *
 * @returns `true` if the NVS key was erased successfully, `false` otherwise.
 *
 * @note Instantiating this template with an unsupported `ConfigType` produces a compile-time error.
 */
bool ConfigManager::deleteConfig() {
  if (!m_isInitialized) {
    ESP_LOGE(TAG, "Cannot delete config, NVS not initialized.");
    return false;
  }

  if constexpr (std::is_same_v<ConfigType, espConfig::mqttConfig_t>){
    m_mqttConfig = {};
    ensureMqttSslLoaded();
    m_mqttSslConfig = {};

    esp_err_t err_mqtt = nvs_erase_key(m_nvsHandle, "MQTTDATA");
    esp_err_t err_ssl = nvs_erase_key(m_nvsHandle, "MQTTSSLDATA");

    bool mqtt_ok = (err_mqtt == ESP_OK || err_mqtt == ESP_ERR_NVS_NOT_FOUND);
    bool ssl_ok = (err_ssl == ESP_OK || err_ssl == ESP_ERR_NVS_NOT_FOUND);

    if (!mqtt_ok) {
      ESP_LOGE(TAG, "Failed to erase MQTTDATA: %04#x", err_mqtt);
    }
    if (!ssl_ok) {
      ESP_LOGE(TAG, "Failed to erase MQTTSSLDATA: %04#x", err_ssl);
    }

    if (mqtt_ok && ssl_ok) {
      esp_err_t commit_err = nvs_commit(m_nvsHandle);
      if (commit_err == ESP_OK) {
        return true;
      }
      ESP_LOGE(TAG, "Failed to commit deleteConfig: %04#x", commit_err);
      return false;
    }
    return false;

  } else if constexpr(std::is_same_v<ConfigType, espConfig::misc_config_t>){
    m_miscConfig = {};
    return saveConfig<espConfig::misc_config_t>();
  } else if constexpr(std::is_same_v<ConfigType, espConfig::actions_config_t>){
    m_actionsConfig = {};
    return saveConfig<espConfig::actions_config_t>();
  } else if constexpr(std::is_same_v<ConfigType, espConfig::https_certs_t>){
    m_httpsCertsConfig = {};
    esp_err_t err = nvs_erase_key(m_nvsHandle, "HTTPSDATA");
    if (err == ESP_OK || err == ESP_ERR_NVS_NOT_FOUND) {
      esp_err_t commit_err = nvs_commit(m_nvsHandle);
      return commit_err == ESP_OK;
    }
    ESP_LOGE(TAG, "Failed to erase HTTPSDATA: %04#x", err);
    return false;
  } else {
    static_assert(std::is_void_v<ConfigType> && false, "Unsupported ConfigType for deleteConfig");
  }
  return false;
}
template bool ConfigManager::deleteConfig<espConfig::mqttConfig_t>();
template bool ConfigManager::deleteConfig<espConfig::misc_config_t>();
template bool ConfigManager::deleteConfig<espConfig::actions_config_t>();

template <typename ConfigType>
/**
 * @brief Saves the selected configuration group to non-volatile storage (NVS).
 *
 * Chooses the NVS blob key based on the config type and attempts to write and commit the serialized data.
 *
 * @returns `true` if the configuration was written and committed to NVS, `false` otherwise.
 */
bool ConfigManager::saveConfig() {
  std::string key;
  if constexpr (std::is_same_v<ConfigType, espConfig::mqttConfig_t>){
    key = "MQTTDATA";
  } else if constexpr (std::is_same_v<ConfigType, espConfig::mqtt_ssl_t>){
    key = "MQTTSSLDATA";
  } else if constexpr(std::is_same_v<ConfigType, espConfig::misc_config_t> || std::is_same_v<ConfigType, espConfig::actions_config_t>){
    key = "MISCDATA";
  } else if constexpr(std::is_same_v<ConfigType, espConfig::https_certs_t>){
    key = "HTTPSDATA";
  } else {
    static_assert(std::is_void_v<ConfigType> && false, "Unsupported ConfigType for saveConfig");
  }
  ESP_LOGI(TAG, "Attempting to save '%s' configuration...", key.c_str());
  if (saveConfigToNvs(key.c_str())) {
    ESP_LOGI(TAG, "'%s' successfully saved and updated.", key.c_str());
    return true;
  }
  ESP_LOGE(TAG, "Failed to save '%s' configuration.", key.c_str());
  return false;
}
template bool ConfigManager::saveConfig<espConfig::mqttConfig_t>();
template bool ConfigManager::saveConfig<espConfig::misc_config_t>();
template bool ConfigManager::saveConfig<espConfig::actions_config_t>();

/**
 * @brief Loads and applies a configuration blob from NVS into the in-memory config.
 *
 * Attempts to read a MessagePack blob stored under the given NVS key and, if present
 * and valid, deserializes it into the corresponding in-memory configuration (either
 * the MQTT or misc config). If the key is absent, the blob is empty, or parsing fails,
 * the in-memory configuration is left unchanged (defaults remain).
 *
 * @param key NVS blob key to load; valid values are "MQTTDATA" (loads MQTT config)
 *            and "MISCDATA" (loads misc config).
 */
void ConfigManager::loadConfigFromNvs(const char *key) {
  if (!m_isInitialized) {
    ESP_LOGE(TAG, "Cannot load, NVS not initialized.");
    return;
  }

  size_t required_size = 0;
  esp_err_t err = nvs_get_blob(m_nvsHandle, key, NULL, &required_size);

  if (err == ESP_ERR_NVS_NOT_FOUND) {
    ESP_LOGI(TAG, "Config key '%s' not found in NVS. Using default values.",
             key);
    return;
  } else if (err != ESP_OK) {
    ESP_LOGE(TAG, "Error (%04#x) getting blob size for key '%s'",
             err, key);
    return;
  }

  if (required_size == 0) {
    ESP_LOGW(TAG, "Key '%s' found but size is 0. Using defaults.", key);
    return;
  }

  // Validate size to prevent heap exhaustion on ESP32
  const size_t MAX_CONFIG_SIZE = 8192; // 8KB max config size
  if (required_size > MAX_CONFIG_SIZE) {
    ESP_LOGE(TAG, "Config size too large for key '%s': %zu bytes (max: %zu)", 
             key, required_size, MAX_CONFIG_SIZE);
    return;
  }

  std::vector<uint8_t> buffer(required_size);
  err = nvs_get_blob(m_nvsHandle, key, buffer.data(), &required_size);

  if (err != ESP_OK) {
    ESP_LOGE(TAG, "Error (%04#x) reading blob for key '%s'", err,
             key);
    return;
  }

  ESP_LOGD(TAG, "Read %zu bytes for key '%s'", buffer.size(), key);

  msgpack_unpacked unpacked;
  msgpack_unpacked_init(&unpacked);
  
  // RAII guard to ensure cleanup on all code paths
  struct UnpackedGuard {
    msgpack_unpacked* ptr;
    ~UnpackedGuard() { if(ptr) msgpack_unpacked_destroy(ptr); }
  } guard{&unpacked};
  
  bool success = msgpack_unpack((const char*)buffer.data(), buffer.size(), NULL, unpacked.zone, &unpacked.data);
  if(success) {
    msgpack_object obj = unpacked.data;
    if(!strcmp(key, "MQTTDATA")){
      deserialize(obj, "mqtt");
    } else if(!strcmp(key, "MQTTSSLDATA")){
      deserialize(obj, "ssl");
      migratePemToDer();
    } else if(!strcmp(key, "MISCDATA")){
      deserialize(obj, "misc");
      deserialize(obj, "actions");
    } else if(!strcmp(key, "HTTPSDATA")){
      deserialize(obj, "https");
      migratePemToDer();
    } else {ESP_LOGE(TAG, "Key '%s' not valid", key);return;}
  } else {
    ESP_LOGE(TAG, "Failed to parse msgpack for key '%s'. Using defaults.", key);
  }
}

/**
 * @brief Persist the specified configuration group into NVS under the given key.
 *
 * Writes a MessagePack-serialized blob for the configuration identified by `key`
 * ("MISCDATA" or "MQTTDATA") into the opened NVS namespace and commits the change.
 *
 * @param key NVS blob key that identifies which configuration to save ("MISCDATA" or "MQTTDATA").
 * @return bool `true` if the blob was written and NVS commit succeeded, `false` otherwise (including when NVS is not initialized or on write/commit errors).
 */
bool ConfigManager::saveConfigToNvs(const char *key) {
  if (!m_isInitialized) {
    ESP_LOGE(TAG, "Cannot save, NVS not initialized.");
    return false;
  }

  const SerializedBuffer buf = [&] {
    if (!strcmp(key, "MISCDATA")) return serialize<espConfig::misc_config_t>();
    if (!strcmp(key, "MQTTSSLDATA")) return serialize<espConfig::mqtt_ssl_t>();
    if (!strcmp(key, "MQTTDATA")) return serialize<espConfig::mqttConfig_t>();
    if (!strcmp(key, "HTTPSDATA")) return serialize<espConfig::https_certs_t>();
    return SerializedBuffer{};
  }();

  ESP_LOGD(TAG, "Config '%s' serialized, size %zu", key, buf.size);
  esp_err_t set_err = nvs_set_blob(m_nvsHandle, key, buf.data.get(), buf.size);

  if (set_err != ESP_OK) {
    ESP_LOGE(TAG, "Failed to set blob in NVS for key '%s': %04#x", key, set_err);
    return false;
  }

  esp_err_t commit_err = nvs_commit(m_nvsHandle);
  if (commit_err != ESP_OK) {
    ESP_LOGE(TAG, "Failed to commit NVS changes for key '%s': %04#x", key, commit_err);
    return false;
  }
  return true;
}

/**
 * @brief Populates in-memory configuration fields from a MessagePack map.
 *
 * Deserializes a MessagePack map object into the ConfigManager's in-memory configuration
 * entries identified by the provided section name (`type`). For each string key found
 * in the MessagePack map that exists in the internal field table for `type`, the matching
 * in-memory field is updated. Supported value types:
 * - string -> std::string
 * - boolean -> bool
 * - positive integer -> uint8_t / uint16_t
 * - array of integers -> std::array<uint8_t, N> (N = 4, 5, 7)
 * - array of [enum, value] pairs -> std::map<espConfig::misc_config_t::colorMap, uint8_t>
 * - array of [string, value] pairs -> std::map<std::string, uint8_t>
 *
 * Unknown keys are ignored. If `obj` is not a MessagePack map, an error is logged and
 * no assignments are performed.
 *
 * @param obj MessagePack object expected to be a map of configuration keys to values.
 * @param type Configuration section name (e.g., "mqtt" or "misc") used to look up the
 *             corresponding key → field mapping in the static field table.
 */
void ConfigManager::deserialize(msgpack_object obj, std::string type) {
  if (obj.type == MSGPACK_OBJECT_MAP) {
    FieldRange range = sectionRange(type.c_str());
    if (!range.first) return;
    msgpack_object_kv *map = obj.via.map.ptr;
    msgpack_object_kv *const end = obj.via.map.ptr + obj.via.map.size;
    std::span range_kv(map, end);
    for (auto v : range_kv) {
      if (v.key.type == MSGPACK_OBJECT_STR) {
        const ConfigField* entry =
            findField(type.c_str(), v.key.via.str.ptr, v.key.via.str.size);
        if (!entry) continue;
        const char* key = entry->name;
        size_t keyLen = strlen(key);

        switch (entry->type) {
        case FieldType::Str: {
          if (v.val.type == MSGPACK_OBJECT_STR) {
            fieldPtr<std::string>(*entry)->assign(v.val.via.str.ptr, v.val.via.str.size);
          }
          break;
        }
        case FieldType::Bool: {
          if (v.val.type == MSGPACK_OBJECT_BOOLEAN) {
            *fieldPtr<bool>(*entry) = v.val.via.boolean;
          }
          break;
        }
        case FieldType::U8:
        case FieldType::U16: {
          if (v.val.type == MSGPACK_OBJECT_POSITIVE_INTEGER) {
            const size_t maxv = entry->type == FieldType::U8
                                    ? std::numeric_limits<uint8_t>::max()
                                    : std::numeric_limits<uint16_t>::max();
            if (v.val.via.u64 > maxv) {
              ESP_LOGW(TAG, "Value overflow for '%.*s': %llu exceeds max %u",
                       int(keyLen), key, (unsigned long long)v.val.via.u64, unsigned(maxv));
              break;
            }
            if (entry->type == FieldType::U8) {
              *fieldPtr<uint8_t>(*entry) = static_cast<uint8_t>(v.val.via.u64);
            } else {
              *fieldPtr<uint16_t>(*entry) = static_cast<uint16_t>(v.val.via.u64);
            }
          }
          break;
        }
        case FieldType::Arr3:
        case FieldType::Arr4:
        case FieldType::Arr5:
        case FieldType::Arr7: {
          if (v.val.type != MSGPACK_OBJECT_ARRAY) break;
          const size_t want = entry->type == FieldType::Arr3 ? 3
                              : entry->type == FieldType::Arr4 ? 4
                              : entry->type == FieldType::Arr5 ? 5 : 7;
          if (v.val.via.array.size != want) {
            ESP_LOGW(TAG, "Validation failed for '%.*s': array size is not %u.",
                     int(keyLen), key, unsigned(want));
            break;
          }
          uint8_t* dst = nullptr;
          if (entry->type == FieldType::Arr3) dst = fieldPtr<std::array<uint8_t,3>>(*entry)->data();
          else if (entry->type == FieldType::Arr4) dst = fieldPtr<std::array<uint8_t,4>>(*entry)->data();
          else if (entry->type == FieldType::Arr5) dst = fieldPtr<std::array<uint8_t,5>>(*entry)->data();
          else dst = fieldPtr<std::array<uint8_t,7>>(*entry)->data();
          for (size_t i = 0; i < want; ++i) {
            dst[i] = static_cast<uint8_t>(v.val.via.array.ptr[i].via.u64);
          }
          break;
        }
        case FieldType::ColorMap: {
          auto* m = fieldPtr<std::map<espConfig::actions_config_t::colorMap, uint8_t>>(*entry);
          if (v.val.type == MSGPACK_OBJECT_ARRAY) {
            for (size_t i = 0; i < v.val.via.array.size; ++i) {
              const msgpack_object& o = v.val.via.array.ptr[i];
              if (o.type == MSGPACK_OBJECT_ARRAY && o.via.array.size == 2) {
                (*m)[static_cast<espConfig::actions_config_t::colorMap>(o.via.array.ptr[0].via.u64)] =
                    static_cast<uint8_t>(o.via.array.ptr[1].via.u64);
              }
            }
          }
          break;
        }
        case FieldType::StrMap: {
          auto* m = fieldPtr<std::map<std::string, uint8_t>>(*entry);
          if (v.val.type == MSGPACK_OBJECT_MAP) {
            m->clear();
            msgpack_object_kv *sub = v.val.via.map.ptr;
            msgpack_object_kv *const subEnd = v.val.via.map.ptr + v.val.via.map.size;
            for (; sub != subEnd; ++sub) {
              if (sub->key.type == MSGPACK_OBJECT_STR && sub->val.type == MSGPACK_OBJECT_POSITIVE_INTEGER) {
                m->emplace(std::string(sub->key.via.str.ptr, sub->key.via.str.size),
                           static_cast<uint8_t>(sub->val.via.u64));
              }
            }
          } else if (v.val.type == MSGPACK_OBJECT_ARRAY) {
            for (size_t i = 0; i < v.val.via.array.size; ++i) {
              const msgpack_object& o = v.val.via.array.ptr[i];
              if (o.type == MSGPACK_OBJECT_ARRAY && o.via.array.size >= 2 &&
                  o.via.array.ptr[0].type == MSGPACK_OBJECT_STR &&
                  o.via.array.ptr[1].type == MSGPACK_OBJECT_POSITIVE_INTEGER) {
                (*m)[std::string(o.via.array.ptr[0].via.str.ptr, o.via.array.ptr[0].via.str.size)] =
                    static_cast<uint8_t>(o.via.array.ptr[1].via.u64);
              }
            }
          }
          break;
        }
        case FieldType::U8Arr6:
        case FieldType::U8Arr2: {
          // Custom lock states/actions: array of [name, value] pairs or a
          // name→value map. Values are positional; names are fixed.
          uint8_t* const arr = entry->type == FieldType::U8Arr6
                          ? fieldPtr<std::array<uint8_t,6>>(*entry)->data()
                          : fieldPtr<std::array<uint8_t,2>>(*entry)->data();
          const char* const* names = entry->type == FieldType::U8Arr6
                          ? espConfig::mqttConfig_t::customLockStateNames
                          : espConfig::mqttConfig_t::customLockActionNames;
          const size_t n = entry->type == FieldType::U8Arr6 ? 6 : 2;
          if (v.val.type == MSGPACK_OBJECT_MAP) {
            msgpack_object_kv *sub = v.val.via.map.ptr;
            msgpack_object_kv *const subEnd = v.val.via.map.ptr + v.val.via.map.size;
            for (; sub != subEnd; ++sub) {
              if (sub->key.type != MSGPACK_OBJECT_STR) continue;
              for (size_t i = 0; i < n; ++i) {
                size_t nl = strlen(names[i]);
                if (sub->key.via.str.size == nl &&
                    memcmp(names[i], sub->key.via.str.ptr, nl) == 0) {
                  arr[i] = static_cast<uint8_t>(sub->val.via.u64);
                  break;
                }
              }
            }
          } else if (v.val.type == MSGPACK_OBJECT_ARRAY) {
            for (size_t i = 0; i < v.val.via.array.size; ++i) {
              const msgpack_object& o = v.val.via.array.ptr[i];
              if (o.type == MSGPACK_OBJECT_ARRAY && o.via.array.size >= 2 &&
                  o.via.array.ptr[0].type == MSGPACK_OBJECT_STR) {
                for (size_t j = 0; j < n; ++j) {
                  size_t nl = strlen(names[j]);
                  if (o.via.array.ptr[0].via.str.size == nl &&
                      memcmp(names[j], o.via.array.ptr[0].via.str.ptr, nl) == 0) {
                    arr[j] = static_cast<uint8_t>(o.via.array.ptr[1].via.u64);
                    break;
                  }
                }
              }
            }
          }
          break;
        }
        default:
          ESP_LOGW(TAG, "DON'T KNOW THIS ONE! - %.*s (%d) = %d", int(keyLen), key, v.val.type, int(v.val.via.u64));
        }
      }
    }
  } else {
    ESP_LOGE(TAG, "Error: Expected a MessagePack map object for deserialization.");
  }
}

template <typename ConfigType>
/**
 * @brief Serializes the selected configuration type into a MessagePack binary blob.
 *
 * Produces a MessagePack representation of the in-memory configuration for either the misc or mqtt config type,
 * encoding strings, booleans, unsigned integers, fixed-size byte arrays, and map entries. Map entries are encoded
 * as arrays of key/value pairs; enum-keyed color maps are encoded as [enum, value] pairs and string-keyed maps as
 * [string, value] pairs.
 *
 * @return ConfigManager::SerializedBuffer handle to the MessagePack-encoded configuration.
 */
ConfigManager::SerializedBuffer ConfigManager::serialize() {
  msgpack_sbuffer sbuf;
  msgpack_packer pk;
  msgpack_sbuffer_init(&sbuf);
  msgpack_packer_init(&pk, &sbuf, msgpack_sbuffer_write);
  FieldRange primary{}, secondary{};
  if constexpr (std::is_same_v<espConfig::misc_config_t, ConfigType>){
    primary = sectionRange("misc");
    secondary = sectionRange("actions");
  } else if constexpr (std::is_same_v<espConfig::actions_config_t, ConfigType>){
    primary = sectionRange("actions");
    secondary = sectionRange("misc");
  } else if constexpr (std::is_same_v<espConfig::mqtt_ssl_t, ConfigType>){
    primary = sectionRange("ssl");
  } else if constexpr (std::is_same_v<espConfig::mqttConfig_t, ConfigType>){
    primary = sectionRange("mqtt");
  } else if constexpr (std::is_same_v<espConfig::https_certs_t, ConfigType>){
    primary = sectionRange("https");
  }
  const size_t mapSize = (primary.last - primary.first) + (secondary.last - secondary.first);
  msgpack_pack_map(&pk, mapSize); // Pack the map size

  for (auto section : {primary, secondary}) {
    for (const ConfigField* it = section.first; it != section.last; ++it) {
      const ConfigField& f = *it;
      const size_t keyLen = strlen(f.name);
      msgpack_pack_str(&pk, keyLen); // Pack the key (string) size
      msgpack_pack_str_body(&pk, f.name, keyLen); // Pack the key (string) body

      switch (f.type) {
      case FieldType::Str: {
        const auto* s = fieldPtr<std::string>(f);
        msgpack_pack_str(&pk, s->size());
        msgpack_pack_str_body(&pk, s->data(), s->size());
        break;
      }
      case FieldType::Bool:
        if (*fieldPtr<bool>(f)) {
          msgpack_pack_true(&pk);
        } else {
          msgpack_pack_false(&pk);
        }
        break;
      case FieldType::U8:
        msgpack_pack_unsigned_char(&pk, *fieldPtr<uint8_t>(f));
        break;
      case FieldType::U16:
        msgpack_pack_unsigned_short(&pk, *fieldPtr<uint16_t>(f));
        break;
      case FieldType::Arr3:
      case FieldType::Arr4:
      case FieldType::Arr5:
      case FieldType::Arr7: {
        const uint8_t* data = nullptr;
        size_t n = 0;
        if (f.type == FieldType::Arr3) { data = fieldPtr<std::array<uint8_t,3>>(f)->data(); n = 3; }
        else if (f.type == FieldType::Arr4) { data = fieldPtr<std::array<uint8_t,4>>(f)->data(); n = 4; }
        else if (f.type == FieldType::Arr5) { data = fieldPtr<std::array<uint8_t,5>>(f)->data(); n = 5; }
        else { data = fieldPtr<std::array<uint8_t,7>>(f)->data(); n = 7; }
        msgpack_pack_array(&pk, n);
        for (size_t i = 0; i < n; ++i) {
          msgpack_pack_unsigned_char(&pk, data[i]);
        }
        break;
      }
      case FieldType::ColorMap: {
        const auto* m = fieldPtr<std::map<espConfig::actions_config_t::colorMap, uint8_t>>(f);
        msgpack_pack_array(&pk, m->size());
        for (const auto& map_pair : *m) {
          msgpack_pack_array(&pk, 2);
          msgpack_pack_unsigned_char(&pk, static_cast<uint8_t>(map_pair.first));
          msgpack_pack_unsigned_char(&pk, map_pair.second);
        }
        break;
      }
      case FieldType::StrMap: {
        const auto* m = fieldPtr<std::map<std::string, uint8_t>>(f);
        msgpack_pack_map(&pk, m->size());
        for (const auto& map_pair : *m) {
          msgpack_pack_str(&pk, map_pair.first.size());
          msgpack_pack_str_body(&pk, map_pair.first.data(), map_pair.first.size());
          msgpack_pack_unsigned_char(&pk, map_pair.second);
        }
        break;
      }
      case FieldType::U8Arr6:
      case FieldType::U8Arr2: {
        // Encoded as an array of [name, value] pairs, matching the legacy
        // [string, value] map encoding so NVS blobs stay compatible.
        const uint8_t* vals = nullptr;
        const char* const* names = nullptr;
        size_t n = 0;
        if (f.type == FieldType::U8Arr6) {
          vals = fieldPtr<std::array<uint8_t,6>>(f)->data();
          names = espConfig::mqttConfig_t::customLockStateNames;
          n = 6;
        } else {
          vals = fieldPtr<std::array<uint8_t,2>>(f)->data();
          names = espConfig::mqttConfig_t::customLockActionNames;
          n = 2;
        }
        msgpack_pack_array(&pk, n);
        for (size_t i = 0; i < n; ++i) {
          msgpack_pack_array(&pk, 2);
          const size_t nl = strlen(names[i]);
          msgpack_pack_str(&pk, nl);
          msgpack_pack_str_body(&pk, names[i], nl);
          msgpack_pack_unsigned_char(&pk, vals[i]);
        }
        break;
      }
      }
    }
  }

  SerializedBuffer out;
  out.size = sbuf.size;
  out.data.reset(reinterpret_cast<uint8_t*>(msgpack_sbuffer_release(&sbuf)));
  return out;
}

template <typename ConfigType>
std::string ConfigManager::updateFromJson(const std::string& json_string) {
  // RAII guard for cJSON cleanup
  struct cJSONGuard {
    cJSON* ptr;
    cJSONGuard(cJSON* p) : ptr(p) {}
    ~cJSONGuard() { if(ptr) cJSON_Delete(ptr); }
    cJSON* get() { return ptr; }
    operator bool() const { return ptr != nullptr; }
  };
  
  cJSONGuard root(cJSON_Parse(json_string.c_str()));
  if (!root) {
    ESP_LOGE(TAG, "Failed to parse JSON: invalid format.");
    return "";
  }

  if (!cJSON_IsObject(root.get())) {
    ESP_LOGE(TAG, "JSON root is not an object.");
    return "";
  }

  const char* section = nullptr;
  if constexpr (std::is_same_v<ConfigType, espConfig::misc_config_t>) {
    section = "misc";
  } else if constexpr (std::is_same_v<ConfigType, espConfig::actions_config_t>) {
    section = "actions";
  } else if constexpr (std::is_same_v<ConfigType, espConfig::mqttConfig_t>) {
    section = "mqtt";
  } else {
    ESP_LOGE(TAG, "Invalid configuration type specified.");
    return "";
  }

  for (cJSON *it = root.get()->child; it != NULL; it = it->next) {
    const char* keyStr = it->string;
    const ConfigField* entry =
        findField(section, keyStr, strlen(keyStr));

    if (entry) {
      switch (entry->type) {
      case FieldType::Str:
        if (cJSON_IsString(it)) {
          fieldPtr<std::string>(*entry)->assign(it->valuestring);
        } else {
          ESP_LOGW(TAG, "Validation failed for '%s': type mismatch, expected string.", keyStr);
        }
        break;
      case FieldType::Bool:
        if (cJSON_IsBool(it)) {
          *fieldPtr<bool>(*entry) = cJSON_IsTrue(it);
        } else if (cJSON_IsNumber(it)) {
          *fieldPtr<bool>(*entry) = (it->valueint != 0);
        } else {
          ESP_LOGW(TAG, "Validation failed for '%s': type mismatch, expected boolean.", keyStr);
        }
        break;
      case FieldType::U8:
      case FieldType::U16:
        if (cJSON_IsNumber(it)) {
          const double maxv = entry->type == FieldType::U8
                                  ? std::numeric_limits<uint8_t>::max()
                                  : std::numeric_limits<uint16_t>::max();
          if (it->valueint < 0 || it->valuedouble > maxv) {
            ESP_LOGW(TAG, "Value out of range for '%s': %d", keyStr, it->valueint);
          } else if (entry->type == FieldType::U8) {
            *fieldPtr<uint8_t>(*entry) = static_cast<uint8_t>(it->valueint);
          } else {
            *fieldPtr<uint16_t>(*entry) = static_cast<uint16_t>(it->valueint);
          }
        } else {
          ESP_LOGW(TAG, "Validation failed for '%s': type mismatch, expected number.", keyStr);
        }
        break;
      case FieldType::Arr3:
      case FieldType::Arr4:
      case FieldType::Arr5:
      case FieldType::Arr7: {
        if (cJSON_IsArray(it)) {
          const size_t want = entry->type == FieldType::Arr3 ? 3
                              : entry->type == FieldType::Arr4 ? 4
                              : entry->type == FieldType::Arr5 ? 5 : 7;
          int array_size = cJSON_GetArraySize(it);
          if (array_size >= 0 && size_t(array_size) == want) {
            uint8_t* dst = nullptr;
            if (entry->type == FieldType::Arr3) dst = fieldPtr<std::array<uint8_t,3>>(*entry)->data();
            else if (entry->type == FieldType::Arr4) dst = fieldPtr<std::array<uint8_t,4>>(*entry)->data();
            else if (entry->type == FieldType::Arr5) dst = fieldPtr<std::array<uint8_t,5>>(*entry)->data();
            else dst = fieldPtr<std::array<uint8_t,7>>(*entry)->data();
            bool array_success = true;
            for (int i = 0; i < array_size; ++i) {
              cJSON *sub_item = cJSON_GetArrayItem(it, i);
              if (cJSON_IsNumber(sub_item)) {
                dst[i] = static_cast<uint8_t>(sub_item->valueint);
              } else {
                array_success = false;
                break;
              }
            }
            if (!array_success) {
              ESP_LOGW(TAG, "Validation failed for '%s': array contains non-numeric elements.", keyStr);
            }
          } else {
            ESP_LOGW(TAG, "Validation failed for '%s': incorrect array size. Expected %zu, got %d.", keyStr, want, array_size);
          }
        } else {
          ESP_LOGW(TAG, "Validation failed for '%s': type mismatch, expected array.", keyStr);
        }
        break;
      }
      case FieldType::ColorMap: {
        auto* m = fieldPtr<std::map<espConfig::actions_config_t::colorMap, uint8_t>>(*entry);
        if (cJSON_IsArray(it)) {
          m->clear();
          cJSON *inner_array;
          cJSON_ArrayForEach(inner_array, it) {
            if (cJSON_IsArray(inner_array) && cJSON_GetArraySize(inner_array) == 2) {
              cJSON *key_json = cJSON_GetArrayItem(inner_array, 0);
              cJSON *value_json = cJSON_GetArrayItem(inner_array, 1);
              if (cJSON_IsNumber(key_json) && cJSON_IsNumber(value_json)) {
                m->emplace(
                    static_cast<espConfig::actions_config_t::colorMap>(key_json->valueint),
                    static_cast<uint8_t>(value_json->valueint)
                );
              }
            }
          }
        } else {
          ESP_LOGW(TAG, "Validation failed for '%s': type mismatch, expected an array of [key, value] pairs.", keyStr);
        }
        break;
      }
      case FieldType::StrMap: {
        auto* m = fieldPtr<std::map<std::string, uint8_t>>(*entry);
        if (cJSON_IsObject(it)) {
          m->clear();
          cJSON* sub_obj_item;
          cJSON_ArrayForEach(sub_obj_item, it) {
            if (cJSON_IsNumber(sub_obj_item)) {
              (*m)[sub_obj_item->string] = static_cast<uint8_t>(sub_obj_item->valueint);
            }
          }
        } else {
          ESP_LOGW(TAG, "Validation failed for '%s': type mismatch, expected an object.", keyStr);
        }
        break;
      }
      case FieldType::U8Arr6:
      case FieldType::U8Arr2: {
        uint8_t* const arr = entry->type == FieldType::U8Arr6
                        ? fieldPtr<std::array<uint8_t,6>>(*entry)->data()
                        : fieldPtr<std::array<uint8_t,2>>(*entry)->data();
        const char* const* names = entry->type == FieldType::U8Arr6
                        ? espConfig::mqttConfig_t::customLockStateNames
                        : espConfig::mqttConfig_t::customLockActionNames;
        const size_t n = entry->type == FieldType::U8Arr6 ? 6 : 2;
        if (cJSON_IsObject(it)) {
          cJSON* sub_obj_item;
          cJSON_ArrayForEach(sub_obj_item, it) {
            if (cJSON_IsNumber(sub_obj_item)) {
              for (size_t i = 0; i < n; ++i) {
                if (strcmp(names[i], sub_obj_item->string) == 0) {
                  arr[i] = static_cast<uint8_t>(sub_obj_item->valueint);
                  break;
                }
              }
            }
          }
        } else {
          ESP_LOGW(TAG, "Validation failed for '%s': type mismatch, expected an object.", keyStr);
        }
        break;
      }
      }
    } else {
      ESP_LOGW(TAG, "'%s' is not a valid configuration key and will be ignored.", keyStr);
    }
  }

  return serializeToJson<ConfigType>();
}
template std::string ConfigManager::updateFromJson<espConfig::misc_config_t>(const std::string& json_string);
template std::string ConfigManager::updateFromJson<espConfig::actions_config_t>(const std::string& json_string);
template std::string ConfigManager::updateFromJson<espConfig::mqttConfig_t>(const std::string& json_string);

template <typename ConfigType>
/**
 * @brief Convert the selected configuration (MQTT or misc) into a JSON string.
 *
 * Serializes in-memory configuration fields into a JSON object. Supported field types:
 * - std::string as JSON strings
 * - bool as JSON booleans
 * - uint8_t/uint16_t as JSON numbers
 * - fixed-size byte arrays (std::array<uint8_t, 4|5|7>) as JSON arrays of numbers
 * - std::map<espConfig::misc_config_t::colorMap, uint8_t> as an array of two-element arrays [enumValue, value]
 * - std::map<std::string, uint8_t> as a JSON object mapping names to numeric values
 *
 * @return std::string JSON representation of the configuration; returns an empty string if serialization fails.
 */
std::string ConfigManager::serializeToJson() {
  // RAII guard for cJSON cleanup
  struct cJSONGuard {
    cJSON* ptr;
    cJSONGuard(cJSON* p) : ptr(p) {}
    ~cJSONGuard() { if(ptr) cJSON_Delete(ptr); }
    cJSON* get() { return ptr; }
    operator bool() const { return ptr != nullptr; }
  };
  
  cJSONGuard root(cJSON_CreateObject());
  if (!root) {
      return ""; // Error creating JSON object
  }

  const char* section = nullptr;
  if constexpr (std::is_same_v<espConfig::misc_config_t, ConfigType>){
    section = "misc";
  } else if constexpr (std::is_same_v<espConfig::actions_config_t, ConfigType>){
    section = "actions";
  } else if constexpr (std::is_same_v<espConfig::mqttConfig_t, ConfigType>){
    section = "mqtt";
  }
  if (!section) return "";
  const FieldRange range = sectionRange(section);

  for (const ConfigField* it = range.first; it != range.last; ++it) {
    const ConfigField& f = *it;
    const char* key = f.name;
    switch (f.type) {
    case FieldType::Str: {
      const auto* s = fieldPtr<std::string>(f);
      if (strstr(key, "Password") || strstr(key, "Passwd")) {
        cJSON_AddStringToObject(root.get(), key, "********");
      } else {
        cJSON_AddStringToObject(root.get(), key, s->c_str());
      }
      break;
    }
    case FieldType::Bool:
      cJSON_AddBoolToObject(root.get(), key, *fieldPtr<bool>(f));
      break;
    case FieldType::U8:
    case FieldType::U16:
      cJSON_AddNumberToObject(root.get(), key,
          f.type == FieldType::U8 ? double(*fieldPtr<uint8_t>(f)) : double(*fieldPtr<uint16_t>(f)));
      break;
    case FieldType::Arr3:
    case FieldType::Arr4:
    case FieldType::Arr5:
    case FieldType::Arr7: {
      const uint8_t* data = nullptr;
      size_t n = 0;
      if (f.type == FieldType::Arr3) { data = fieldPtr<std::array<uint8_t,3>>(f)->data(); n = 3; }
      else if (f.type == FieldType::Arr4) { data = fieldPtr<std::array<uint8_t,4>>(f)->data(); n = 4; }
      else if (f.type == FieldType::Arr5) { data = fieldPtr<std::array<uint8_t,5>>(f)->data(); n = 5; }
      else { data = fieldPtr<std::array<uint8_t,7>>(f)->data(); n = 7; }
      cJSON *array = cJSON_CreateArray();
      if (array) {
        for (size_t i = 0; i < n; ++i) {
          cJSON_AddItemToArray(array, cJSON_CreateNumber(static_cast<double>(data[i])));
        }
        cJSON_AddItemToObject(root.get(), key, array);
      }
      break;
    }
    case FieldType::ColorMap: {
      const auto* m = fieldPtr<std::map<espConfig::actions_config_t::colorMap, uint8_t>>(f);
      cJSON *array_of_arrays = cJSON_CreateArray();
      if (array_of_arrays) {
        for (const auto& map_pair : *m) {
          cJSON *inner_array = cJSON_CreateArray();
          cJSON_AddItemToArray(inner_array, cJSON_CreateNumber(static_cast<double>(map_pair.first))); // Enum key as integer
          cJSON_AddItemToArray(inner_array, cJSON_CreateNumber(static_cast<double>(map_pair.second)));
          cJSON_AddItemToArray(array_of_arrays, inner_array);
        }
        cJSON_AddItemToObject(root.get(), key, array_of_arrays);
      }
      break;
    }
    case FieldType::StrMap: {
      const auto* m = fieldPtr<std::map<std::string, uint8_t>>(f);
      cJSON *map_obj = cJSON_CreateObject();
      if (map_obj) {
        for (const auto& map_pair : *m) {
          cJSON_AddNumberToObject(map_obj, map_pair.first.c_str(), static_cast<double>(map_pair.second));
        }
        cJSON_AddItemToObject(root.get(), key, map_obj);
      }
      break;
    }
    case FieldType::U8Arr6:
    case FieldType::U8Arr2: {
      const uint8_t* vals = nullptr;
      const char* const* names = nullptr;
      size_t n = 0;
      if (f.type == FieldType::U8Arr6) {
        vals = fieldPtr<std::array<uint8_t,6>>(f)->data();
        names = espConfig::mqttConfig_t::customLockStateNames;
        n = 6;
      } else {
        vals = fieldPtr<std::array<uint8_t,2>>(f)->data();
        names = espConfig::mqttConfig_t::customLockActionNames;
        n = 2;
      }
      cJSON *map_obj = cJSON_CreateObject();
      if (map_obj) {
        for (size_t i = 0; i < n; ++i) {
          cJSON_AddNumberToObject(map_obj, names[i], static_cast<double>(vals[i]));
        }
        cJSON_AddItemToObject(root.get(), key, map_obj);
      }
      break;
    }
    }
  }

    char *json_string = cJSON_PrintUnformatted(root.get());
    std::string result(json_string ? json_string : "");
    if (json_string) {
        free(json_string);
    }
    return result;
}

template std::string ConfigManager::serializeToJson<espConfig::misc_config_t>();
template std::string ConfigManager::serializeToJson<espConfig::actions_config_t>();
template std::string ConfigManager::serializeToJson<espConfig::mqttConfig_t>();

template <typename ConfigType>
/**
 * @brief Parses a JSON string and applies its fields to the selected in-memory configuration.
 *
 * Parses `json_string`, validates types and shapes of present fields, and updates the corresponding
 * configuration members (either the "misc" or "mqtt" config selected by the template parameter).
 * Only keys that exist in the manager's config map are processed; keys with type/shape mismatches
 * are ignored and cause the function to report failure.
 *
 * @param json_string JSON object string containing configuration keys and values.
 * @return bool `true` if the JSON was parsed and all processed keys were valid and applied; `false` if parsing failed or one or more processed keys failed validation.
 */
bool ConfigManager::deserializeFromJson(const std::string& json_string) {
    // RAII guard for cJSON cleanup
    struct cJSONGuard {
      cJSON* ptr;
      cJSONGuard(cJSON* p) : ptr(p) {}
      ~cJSONGuard() { if(ptr) cJSON_Delete(ptr); }
      cJSON* get() { return ptr; }
      operator bool() const { return ptr != nullptr; }
    };
    
    cJSONGuard root(cJSON_Parse(json_string.c_str()));
    if (!root) {
        ESP_LOGE(TAG, "Failed to parse JSON: invalid format.");
        return false;
    }

    if (!cJSON_IsObject(root.get())) {
        ESP_LOGE(TAG, "JSON root is not an object.");
        return false;
    }

    const char* section = nullptr;
    if constexpr (std::is_same_v<ConfigType, espConfig::misc_config_t>){
      section = "misc";
    } else if constexpr (std::is_same_v<ConfigType, espConfig::actions_config_t>){
      section = "actions";
    } else if constexpr (std::is_same_v<ConfigType, espConfig::mqttConfig_t>){
      section = "mqtt";
    } else {
      static_assert(std::is_void_v<ConfigType> && false, "Unsupported ConfigType for deserializeFromJson");
    }
    bool success = true;
    cJSON *item = root.get()->child;
    while (item) {
        const char* key = item->string;
        const ConfigField* entry = key ? findField(section, key, strlen(key)) : nullptr;
        if (entry) {
            switch (entry->type) {
            case FieldType::Str:
                if (cJSON_IsString(item)) {
                    fieldPtr<std::string>(*entry)->assign(item->valuestring);
                } else {
                    ESP_LOGW(TAG, "Validation failed for '%s': type mismatch, expected string.", key);
                    success = false;
                }
                break;
            case FieldType::Bool:
                if (cJSON_IsBool(item)) {
                    *fieldPtr<bool>(*entry) = cJSON_IsTrue(item);
                } else if(cJSON_IsNumber(item)) {
                    *fieldPtr<bool>(*entry) = item->valueint != 0;
                } else {
                    ESP_LOGW(TAG, "Validation failed for '%s': type mismatch, expected boolean.", key);
                    success = false;
                }
                break;
            case FieldType::U8:
            case FieldType::U16:
                if (cJSON_IsNumber(item)) {
                    const double maxv = entry->type == FieldType::U8
                                            ? std::numeric_limits<uint8_t>::max()
                                            : std::numeric_limits<uint16_t>::max();
                    if (item->valuedouble < 0 || item->valuedouble > maxv) {
                        ESP_LOGW(TAG, "Value out of range for '%s': %f", key, item->valuedouble);
                        success = false;
                    } else if (entry->type == FieldType::U8) {
                        *fieldPtr<uint8_t>(*entry) = static_cast<uint8_t>(item->valuedouble);
                    } else {
                        *fieldPtr<uint16_t>(*entry) = static_cast<uint16_t>(item->valuedouble);
                    }
                } else {
                    ESP_LOGW(TAG, "Validation failed for '%s': type mismatch, expected number.", key);
                    success = false;
                }
                break;
            case FieldType::Arr3:
            case FieldType::Arr4:
            case FieldType::Arr5:
            case FieldType::Arr7: {
                if (cJSON_IsArray(item)) {
                    const size_t want = entry->type == FieldType::Arr3 ? 3
                                        : entry->type == FieldType::Arr4 ? 4
                                        : entry->type == FieldType::Arr5 ? 5 : 7;
                    int array_size = cJSON_GetArraySize(item);
                    if (array_size >= 0 && size_t(array_size) == want) {
                        uint8_t* dst = nullptr;
                        if (entry->type == FieldType::Arr3) dst = fieldPtr<std::array<uint8_t,3>>(*entry)->data();
                        else if (entry->type == FieldType::Arr4) dst = fieldPtr<std::array<uint8_t,4>>(*entry)->data();
                        else if (entry->type == FieldType::Arr5) dst = fieldPtr<std::array<uint8_t,5>>(*entry)->data();
                        else dst = fieldPtr<std::array<uint8_t,7>>(*entry)->data();
                        bool array_success = true;
                        for (int i = 0; i < array_size; ++i) {
                            cJSON *sub_item = cJSON_GetArrayItem(item, i);
                            if (cJSON_IsNumber(sub_item)) {
                                dst[i] = static_cast<uint8_t>(sub_item->valuedouble);
                            } else {
                                array_success = false;
                                break;
                            }
                        }
                        if (!array_success) {
                            ESP_LOGW(TAG, "Validation failed for '%s': array contains non-numeric elements.", key);
                            success = false;
                        }
                    } else {
                        ESP_LOGW(TAG, "Validation failed for '%s': incorrect array size. Expected %zu, got %d.", key, want, array_size);
                        success = false;
                    }
                } else {
                    ESP_LOGW(TAG, "Validation failed for '%s': type mismatch, expected array.", key);
                    success = false;
                }
                break;
            }
            case FieldType::ColorMap: {
                auto* m = fieldPtr<std::map<espConfig::actions_config_t::colorMap, uint8_t>>(*entry);
                if (cJSON_IsArray(item)) {
                    m->clear();
                    int array_size = cJSON_GetArraySize(item);
                    bool map_success = true;
                    for (int i = 0; i < array_size; ++i) {
                        cJSON *inner_array = cJSON_GetArrayItem(item, i);
                        if (cJSON_IsArray(inner_array) && cJSON_GetArraySize(inner_array) == 2) {
                            cJSON *key_json = cJSON_GetArrayItem(inner_array, 0);
                            cJSON *value_json = cJSON_GetArrayItem(inner_array, 1);
                            if (cJSON_IsNumber(key_json) && cJSON_IsNumber(value_json)) {
                                m->emplace(
                                    static_cast<espConfig::actions_config_t::colorMap>(key_json->valuedouble),
                                    static_cast<uint8_t>(value_json->valuedouble)
                                );
                            } else {
                                map_success = false; break;
                            }
                        } else {
                            map_success = false; break;
                        }
                    }
                    if (!map_success) {
                        ESP_LOGW(TAG, "Validation failed for '%s': invalid map format.", key);
                        success = false;
                    }
                } else {
                    ESP_LOGW(TAG, "Validation failed for '%s': type mismatch, expected array for map.", key);
                    success = false;
                }
                break;
            }
            case FieldType::StrMap: {
                auto* m = fieldPtr<std::map<std::string, uint8_t>>(*entry);
                if (cJSON_IsObject(item)) {
                    m->clear();
                    cJSON *sub_obj_item = item->child;
                    bool map_success = true;
                    while(sub_obj_item) {
                        if (cJSON_IsNumber(sub_obj_item)) {
                            (*m)[sub_obj_item->string] = static_cast<uint8_t>(sub_obj_item->valuedouble);
                        } else {
                            map_success = false;
                            break;
                        }
                        sub_obj_item = sub_obj_item->next;
                    }
                    if (!map_success) {
                        ESP_LOGW(TAG, "Validation failed for '%s': map contains non-numeric values.", key);
                        success = false;
                    }
                } else {
                    ESP_LOGW(TAG, "Validation failed for '%s': type mismatch, expected object for map.", key);
                    success = false;
                }
                break;
            }
            case FieldType::U8Arr6:
            case FieldType::U8Arr2: {
                uint8_t* const arr = entry->type == FieldType::U8Arr6
                                ? fieldPtr<std::array<uint8_t,6>>(*entry)->data()
                                : fieldPtr<std::array<uint8_t,2>>(*entry)->data();
                const char* const* names = entry->type == FieldType::U8Arr6
                                ? espConfig::mqttConfig_t::customLockStateNames
                                : espConfig::mqttConfig_t::customLockActionNames;
                const size_t n = entry->type == FieldType::U8Arr6 ? 6 : 2;
                if (cJSON_IsObject(item)) {
                    cJSON *sub_obj_item = item->child;
                    while(sub_obj_item) {
                        if (cJSON_IsNumber(sub_obj_item)) {
                            for (size_t i = 0; i < n; ++i) {
                                if (strcmp(names[i], sub_obj_item->string) == 0) {
                                    arr[i] = static_cast<uint8_t>(sub_obj_item->valuedouble);
                                    break;
                                }
                            }
                        }
                        sub_obj_item = sub_obj_item->next;
                    }
                } else {
                    ESP_LOGW(TAG, "Validation failed for '%s': type mismatch, expected object for map.", key);
                    success = false;
                }
                break;
            }
            }
        } else ESP_LOGW(TAG, "Key '%s' could not be found!", key ? key : "(null)");
        item = item->next;
    }

    return success;
}
template bool ConfigManager::deserializeFromJson<espConfig::misc_config_t>(const std::string& json_string);
template bool ConfigManager::deserializeFromJson<espConfig::actions_config_t>(const std::string& json_string);
template bool ConfigManager::deserializeFromJson<espConfig::mqttConfig_t>(const std::string& json_string);

// Certificate storage implementation

std::string ConfigManager::pemToDer(const std::string& pem) {
  constexpr const char* kBegin = "-----BEGIN ";
  if (pem.compare(0, strlen(kBegin), kBegin) != 0) {
    return pem;
  }

  size_t headerEnd = pem.find("-----", strlen(kBegin));
  size_t footerStart = pem.rfind("-----END ");
  size_t footerEnd = (footerStart == std::string::npos)
      ? std::string::npos : pem.find("-----", footerStart + strlen("-----END "));
  if (headerEnd == std::string::npos || footerStart == std::string::npos ||
      footerEnd == std::string::npos) {
    ESP_LOGW(TAG, "Malformed PEM envelope, storing content as-is");
    return pem;
  }

  const std::string header = pem.substr(0, headerEnd + strlen("-----"));
  const std::string footer = pem.substr(footerStart, footerEnd + strlen("-----") - footerStart);

  mbedtls_pem_context pemCtx;
  mbedtls_pem_init(&pemCtx);
  size_t useLen = 0;
  int ret = mbedtls_pem_read_buffer(&pemCtx, header.c_str(), footer.c_str(),
                                    reinterpret_cast<const unsigned char*>(pem.data()),
                                    nullptr, 0, &useLen);
  std::string der;
  size_t derLen = 0;
  const unsigned char* derBuf = mbedtls_pem_get_buffer(&pemCtx, &derLen);
  if (ret == 0 && derBuf && derLen > 0) {
    der.assign(reinterpret_cast<const char*>(derBuf), derLen);
  } else {
    ESP_LOGW(TAG, "PEM decode failed (-0x%04X), storing content as-is", -ret);
  }
  mbedtls_pem_free(&pemCtx);
  return der.empty() ? pem : der;
}

size_t ConfigManager::mbedtlsParseLen(const std::string& content) {
  if (!content.empty() && content.compare(0, 11, "-----BEGIN ") == 0) {
    return content.length() + 1;
  }
  return content.length();
}

void ConfigManager::ensureMqttSslLoaded() {
  static bool loaded = false;
  if (!loaded) {
    loadConfigFromNvs("MQTTSSLDATA");
    loaded = true;
  }
}

void ConfigManager::migratePemToDer() {
  auto isPemEncoded = [](const std::string& s) {
    return s.size() >= 11 && s.compare(0, 11, "-----BEGIN ") == 0;
  };
  struct CertFieldRef {
    std::string* field;
    const char* name;
  };
  const CertFieldRef mqttFields[] = {
      {&m_mqttSslConfig.caCert, "mqtt CA certificate"},
      {&m_mqttSslConfig.clientCert, "mqtt client certificate"},
      {&m_mqttSslConfig.clientKey, "mqtt client key"},
  };
  const CertFieldRef httpsFields[] = {
      {&m_httpsCertsConfig.caCert, "https CA certificate"},
      {&m_httpsCertsConfig.serverCert, "https server certificate"},
      {&m_httpsCertsConfig.privateKey, "https private key"},
  };
  auto anyPem = [&isPemEncoded](const CertFieldRef* fields, size_t n) {
    for (size_t i = 0; i < n; ++i) {
      if (isPemEncoded(*fields[i].field)) return true;
    }
    return false;
  };
  if (!anyPem(mqttFields, 3) && !anyPem(httpsFields, 3)) {
    return;
  }

  auto convertAll = [&isPemEncoded](const CertFieldRef* fields, size_t n) {
    bool changed = false;
    for (size_t i = 0; i < n; ++i) {
      std::string& field = *fields[i].field;
      if (!isPemEncoded(field)) continue;
      field = pemToDer(field);
      if (isPemEncoded(field)) {
        ESP_LOGE(TAG, "PEM->DER migration failed for %s, keeping PEM blob",
                 fields[i].name);
      } else {
        changed = true;
      }
    }
    return changed;
  };
  const bool mqttChanged = convertAll(mqttFields, 3);
  const bool httpsChanged = convertAll(httpsFields, 3);

  if (mqttChanged && saveConfigToNvs("MQTTSSLDATA")) {
    ESP_LOGI(TAG, "Migrated MQTT SSL certificates from PEM to DER");
  }
  if (httpsChanged && saveConfigToNvs("HTTPSDATA")) {
    ESP_LOGI(TAG, "Migrated HTTPS certificates from PEM to DER");
  }
}

bool ConfigManager::saveCertificate(espConfig::CertType certType, const std::string& certContent) {
    if (!m_isInitialized) {
        ESP_LOGE(TAG, "Cannot save certificate, ConfigManager not initialized.");
        return false;
    }

    const size_t MAX_CERT_SIZE = 16384;
    if (certContent.length() > MAX_CERT_SIZE) {
        ESP_LOGE(TAG, "Certificate too large: %zu bytes (max: %zu)", 
                 certContent.length(), MAX_CERT_SIZE);
        return false;
    }

    if(!validateCertificateContent(certContent, certType)) {
        ESP_LOGE(TAG, "Unable to validate certificate");
        return false;
    }

    const std::string& stored = pemToDer(certContent);
    if (stored.empty()) {
        ESP_LOGE(TAG, "PEM to DER conversion produced no data");
        return false;
    }

    const char* typeStr = "";
    bool isHttps = false;

    switch(certType) {
        case espConfig::CertType::MQTT_CA:
            ensureMqttSslLoaded();
            m_mqttSslConfig.caCert = stored;
            typeStr = "CA certificate";
            break;
        case espConfig::CertType::MQTT_CLIENT:
            ensureMqttSslLoaded();
            m_mqttSslConfig.clientCert = stored;
            typeStr = "Client certificate";
            break;
        case espConfig::CertType::MQTT_PRIVATE_KEY:
            ensureMqttSslLoaded();
            m_mqttSslConfig.clientKey = stored;
            typeStr = "Private key";
            break;
        case espConfig::CertType::HTTPS_SERVER_CERT:
            m_httpsCertsConfig.serverCert = stored;
            typeStr = "HTTPS server certificate";
            isHttps = true;
            break;
        case espConfig::CertType::HTTPS_PRIVATE_KEY:
            m_httpsCertsConfig.privateKey = stored;
            typeStr = "HTTPS private key";
            isHttps = true;
            break;
        case espConfig::CertType::HTTPS_CA_CERT:
            m_httpsCertsConfig.caCert = stored;
            typeStr = "HTTPS CA certificate";
            isHttps = true;
            break;
        default:
    		break;
    }

    ESP_LOGI(TAG, "%s saved successfully", typeStr);

    if (isHttps) {
        return saveConfig<espConfig::https_certs_t>();
    } else {
        return saveConfig<espConfig::mqtt_ssl_t>();
    }
}

void ConfigManager::loadCertificateInto(espConfig::CertType certType, std::string& out) {
    out.clear();
    if (!m_isInitialized) {
        ESP_LOGE(TAG, "Cannot load certificate, ConfigManager not initialized.");
        return;
    }

    const std::string* content = nullptr;
    const char* typeStr = "";

    switch(certType) {
        case espConfig::CertType::MQTT_CA:
            content = &m_mqttSslConfig.caCert;
            typeStr = "CA";
            break;
        case espConfig::CertType::MQTT_CLIENT:
            content = &m_mqttSslConfig.clientCert;
            typeStr = "Client";
            break;
        case espConfig::CertType::MQTT_PRIVATE_KEY:
            content = &m_mqttSslConfig.clientKey;
            typeStr = "Private Key";
            break;
        case espConfig::CertType::HTTPS_SERVER_CERT:
            content = &m_httpsCertsConfig.serverCert;
            typeStr = "HTTPS Server";
            break;
        case espConfig::CertType::HTTPS_PRIVATE_KEY:
            content = &m_httpsCertsConfig.privateKey;
            typeStr = "HTTPS Private Key";
            break;
        case espConfig::CertType::HTTPS_CA_CERT:
            content = &m_httpsCertsConfig.caCert;
            typeStr = "HTTPS CA";
            break;
        default:
            break;
    }

    if (content) {
        out = *content;
    }
    ESP_LOGD(TAG, "%s loaded successfully (size: %zu bytes)", typeStr, out.length());
}

bool ConfigManager::deleteCertificate(espConfig::CertType certType) {
    if (!m_isInitialized) {
        ESP_LOGE(TAG, "Cannot delete certificate, ConfigManager not initialized.");
        return false;
    }

    const char* typeStr = "";
    bool isHttps = false;

    switch(certType) {
        case espConfig::CertType::MQTT_CA:
            ensureMqttSslLoaded();
            m_mqttSslConfig.caCert.clear();
            typeStr = "CA";
            break;
        case espConfig::CertType::MQTT_CLIENT:
            ensureMqttSslLoaded();
            m_mqttSslConfig.clientCert.clear();
            typeStr = "Client";
            break;
        case espConfig::CertType::MQTT_PRIVATE_KEY:
            ensureMqttSslLoaded();
            m_mqttSslConfig.clientKey.clear();
            typeStr = "Private Key";
            break;
        case espConfig::CertType::HTTPS_SERVER_CERT:
            m_httpsCertsConfig.serverCert.clear();
            typeStr = "HTTPS server certificate";
            isHttps = true;
            break;
        case espConfig::CertType::HTTPS_PRIVATE_KEY:
            m_httpsCertsConfig.privateKey.clear();
            typeStr = "HTTPS private key";
            isHttps = true;
            break;
        case espConfig::CertType::HTTPS_CA_CERT:
            m_httpsCertsConfig.caCert.clear();
            typeStr = "HTTPS CA certificate";
            isHttps = true;
            break;
        default:
    		break;
    }

    ESP_LOGI(TAG, "%s deleted successfully", typeStr);

    if (isHttps) {
        return saveConfig<espConfig::https_certs_t>();
    } else {
        return saveConfig<espConfig::mqtt_ssl_t>();
    }
}

bool ConfigManager::validateCertificateContent(const std::string& certContent, espConfig::CertType certType) {
    switch(certType) {
        case espConfig::CertType::MQTT_PRIVATE_KEY:
        case espConfig::CertType::HTTPS_PRIVATE_KEY:
            return validatePrivateKeyContent(certContent);
        case espConfig::CertType::MQTT_CA:
        case espConfig::CertType::MQTT_CLIENT:
        case espConfig::CertType::HTTPS_SERVER_CERT:
        case espConfig::CertType::HTTPS_CA_CERT:
            return validateCertificateWithMbedTLS(certContent, certType);
        default:
    		break;
    }
    return false;
}

bool ConfigManager::validateCertificateWithMbedTLS(const std::string& certContent, espConfig::CertType certType) {
    ScopedX509Crt cert;

    int ret = mbedtls_x509_crt_parse(cert.get(), reinterpret_cast<const unsigned char*>(certContent.c_str()), mbedtlsParseLen(certContent));

    if (ret != 0) {
        char error_buf[100];
        mbedtls_strerror(ret, error_buf, sizeof(error_buf));
        ESP_LOGE(TAG, "Certificate parsing failed: %s (error code: %d)", error_buf, ret);
        return false;
    }

    bool isValid = true;

    if (cert.get()->version == 0) {
        ESP_LOGE(TAG, "Certificate has invalid version");
        isValid = false;
    }

    if (cert.get()->subject_raw.len == 0) {
        ESP_LOGE(TAG, "Certificate missing subject");
        isValid = false;
    }

    if (cert.get()->issuer_raw.len == 0) {
        ESP_LOGE(TAG, "Certificate missing issuer");
        isValid = false;
    }

    if (cert.get()->valid_from.year == 0 || cert.get()->valid_to.year == 0) {
        ESP_LOGE(TAG, "Certificate has invalid validity dates");
        isValid = false;
    }
    
    if (mbedtls_pk_get_type(&cert.get()->pk) == MBEDTLS_PK_NONE) {
        ESP_LOGE(TAG, "Certificate missing public key");
        isValid = false;
    }

    if (certType == espConfig::CertType::MQTT_CA || certType == espConfig::CertType::HTTPS_CA_CERT) {
        // CA certificates should have basic constraints extension
        if (!(cert.get()->MBEDTLS_PRIVATE(ca_istrue))) {
            ESP_LOGW(TAG, "CA certificate does not have CA:true in basic constraints");
        }
    }

    if (isValid) {
        char subject[256];
        char issuer[256];
        int ret_subject = mbedtls_x509_dn_gets(subject, sizeof(subject), &cert.get()->subject);
        int ret_issuer = mbedtls_x509_dn_gets(issuer, sizeof(issuer), &cert.get()->issuer);
        
        if (ret_subject < 0 || ret_issuer < 0) {
            ESP_LOGE(TAG, "Failed to get certificate DN strings (subject: %d, issuer: %d)", 
                     ret_subject, ret_issuer);
        } else {
            ESP_LOGI(TAG, "Certificate validated successfully - Subject: %s, Issuer: %s", subject, issuer);
        }
    }

    return isValid;
}

bool ConfigManager::validatePrivateKeyContent(const std::string& keyContent) {
    ScopedPk pk;

    int ret = mbedtls_pk_parse_key(pk.get(), reinterpret_cast<const unsigned char*>(keyContent.c_str()), mbedtlsParseLen(keyContent), nullptr, 0, nullptr, nullptr);

    if (ret != 0) {
        char error_buf[100];
        mbedtls_strerror(ret, error_buf, sizeof(error_buf));
        ESP_LOGE(TAG, "Private key parsing failed: %s (error code: %d)", error_buf, ret);
        return false;
    }

    mbedtls_pk_type_t key_type = mbedtls_pk_get_type(pk.get());
    if (key_type != MBEDTLS_PK_RSA && key_type != MBEDTLS_PK_ECKEY) {
        ESP_LOGE(TAG, "Unsupported private key type: %d", key_type);
        return false;
    }

    if (key_type == MBEDTLS_PK_RSA) {
        size_t key_bits = mbedtls_pk_get_bitlen(pk.get());
        if (key_bits < 2048) {
            ESP_LOGW(TAG, "RSA key size is only %zu bits, recommended minimum is 2048 bits", key_bits);
        }
    }

    if (key_type == MBEDTLS_PK_ECKEY) {
        const mbedtls_ecp_keypair* ec_key = mbedtls_pk_ec(*pk.get());
        if(!ec_key) {
            ESP_LOGE(TAG, "Failed to access EC key context");
            return false;
        }
    }

    ESP_LOGI(TAG, "Private key validated successfully - Type: %s, Size: %zu bits",
             key_type == MBEDTLS_PK_RSA ? "RSA" : "EC", mbedtls_pk_get_bitlen(pk.get()));

    return true;
}

bool ConfigManager::validateKeyCertPair(const std::string& privateKey, const std::string& certificate, const char* context) {
    if (privateKey.empty() || certificate.empty()) {
        ESP_LOGE("ConfigManager", "%s private key or certificate is empty", context);
        return false;
    }

    static ScopedEntropy entropy;
    static ScopedCtrDrbg ctr_drbg;
    static std::once_flag drbg_seeded;
    std::call_once(drbg_seeded, [] {
        if (mbedtls_ctr_drbg_seed(ctr_drbg.get(), mbedtls_entropy_func, entropy.get(),
                                  nullptr, 0) != 0) {
            ESP_LOGE("ConfigManager", "mbedtls_ctr_drbg_seed failed");
        }
    });

    ScopedPk pk;
    int ret = mbedtls_pk_parse_key(pk.get(), reinterpret_cast<const unsigned char*>(privateKey.c_str()), mbedtlsParseLen(privateKey), nullptr, 0, mbedtls_ctr_drbg_random, ctr_drbg.get());
    if (ret != 0) {
        char error_buf[100];
        mbedtls_strerror(ret, error_buf, sizeof(error_buf));
        ESP_LOGE("ConfigManager", "Failed to parse %s private key: %s (error code: %d)", context, error_buf, ret);
        return false;
    }

    ScopedX509Crt cert;
    ret = mbedtls_x509_crt_parse(cert.get(), reinterpret_cast<const unsigned char*>(certificate.c_str()), mbedtlsParseLen(certificate));
    if (ret != 0) {
        char error_buf[100];
        mbedtls_strerror(ret, error_buf, sizeof(error_buf));
        ESP_LOGE("ConfigManager", "Failed to parse %s certificate: %s (error code: %d)", context, error_buf, ret);
        return false;
    }

    ret = mbedtls_pk_check_pair(&cert.get()->pk, pk.get(), mbedtls_ctr_drbg_random, ctr_drbg.get());
    if (ret != 0) {
        char error_buf[100];
        mbedtls_strerror(ret, error_buf, sizeof(error_buf));
        ESP_LOGE("ConfigManager", "%s private key does not match certificate public key: %s (error code: %d)", context, error_buf, ret);
        return false;
    }

    mbedtls_pk_type_t cert_key_type = mbedtls_pk_get_type(&cert.get()->pk);
    mbedtls_pk_type_t key_type = mbedtls_pk_get_type(pk.get());

    if (cert_key_type != key_type) {
        ESP_LOGE("ConfigManager", "%s certificate and private key have different types (cert: %d, key: %d)", context, cert_key_type, key_type);
        return false;
    }

    if (key_type == MBEDTLS_PK_RSA) {
        size_t cert_key_bits = mbedtls_pk_get_bitlen(&cert.get()->pk);
        size_t key_bits = mbedtls_pk_get_bitlen(pk.get());
        if (cert_key_bits != key_bits) {
            ESP_LOGE("ConfigManager", "%s RSA key sizes don't match (cert: %zu bits, key: %zu bits)", context, cert_key_bits, key_bits);
            return false;
        }
    }

    if (key_type == MBEDTLS_PK_ECKEY || key_type == MBEDTLS_PK_ECKEY_DH || key_type == MBEDTLS_PK_ECDSA) {
        const mbedtls_ecp_keypair* cert_ec = mbedtls_pk_ec(cert.get()->pk);
        const mbedtls_ecp_keypair* key_ec = mbedtls_pk_ec(*pk.get());
        if (cert_ec && key_ec) {
            mbedtls_ecp_group_id cert_curve = cert_ec->MBEDTLS_PRIVATE(grp).id;
            mbedtls_ecp_group_id key_curve = key_ec->MBEDTLS_PRIVATE(grp).id;
            if (cert_curve != key_curve) {
                ESP_LOGE("ConfigManager", "%s EC curves don't match (cert: %d, key: %d)", context, cert_curve, key_curve);
                return false;
            }
        }
    }

    ESP_LOGD("ConfigManager", "%s private key and certificate are cryptographically compatible", context);
    return true;
}

std::vector<CertificateStatus> ConfigManager::getCertificatesStatus(){
  std::vector<CertificateStatus> certificates;
  ensureMqttSslLoaded();
  std::array<espConfig::CertType, 6> types{
    espConfig::CertType::MQTT_CA,
    espConfig::CertType::MQTT_CLIENT,
    espConfig::CertType::MQTT_PRIVATE_KEY,
    espConfig::CertType::HTTPS_SERVER_CERT,
    espConfig::CertType::HTTPS_PRIVATE_KEY,
    espConfig::CertType::HTTPS_CA_CERT
  };
  std::string certStr;
  for (auto certType : types) {
    ScopedX509Crt cert;
    loadCertificateInto(certType, certStr);
    const char* typeStr = "";
    switch(certType) {
        case espConfig::CertType::MQTT_CA: typeStr = "ca"; break;
        case espConfig::CertType::MQTT_CLIENT: typeStr = "client"; break;
        case espConfig::CertType::MQTT_PRIVATE_KEY: typeStr = "privateKey"; break;
        case espConfig::CertType::HTTPS_SERVER_CERT: typeStr = "https_server"; break;
        case espConfig::CertType::HTTPS_PRIVATE_KEY: typeStr = "https_private_key"; break;
        case espConfig::CertType::HTTPS_CA_CERT: typeStr = "https_ca"; break;
        default:
    		break;
    }

    bool isPrivateKey = (certType == espConfig::CertType::MQTT_PRIVATE_KEY || certType == espConfig::CertType::HTTPS_PRIVATE_KEY);

    if(!certStr.empty() && !isPrivateKey){

      int ret = mbedtls_x509_crt_parse(cert.get(), reinterpret_cast<const unsigned char*>(certStr.c_str()), mbedtlsParseLen(certStr));
      if(ret){
        ESP_LOGE(TAG, "Unable to parse '%s' certificate: %d", typeStr, ret);
        continue;
      }
      char subject[128], issuer[128], serial[128];
      ret = mbedtls_x509_dn_gets(subject, sizeof(subject), &cert.get()->subject);
      if(ret < 0){
        ESP_LOGE(TAG, "Unable to retrieve subject DN for '%s' certificate: %d", typeStr, ret);
        continue;
      }
      ret = mbedtls_x509_dn_gets(issuer, sizeof(issuer), &cert.get()->issuer);
      if(ret < 0){
        ESP_LOGE(TAG, "Unable to retrieve issuer DN for '%s' certificate: %d", typeStr, ret);
        continue;
      }
      ret = mbedtls_x509_serial_gets(serial, sizeof(serial), &cert.get()->serial);
      if(ret < 0){
        ESP_LOGE(TAG, "Unable to retrieve serial number for '%s' certificate: %d", typeStr, ret);
        continue;
      }
      unsigned char sha1_hash[20];
      mbedtls_sha1(cert.get()->raw.p, cert.get()->raw.len, sha1_hash);
      mbedtls_x509_time valid_from = cert.get()->valid_from;
      mbedtls_x509_time valid_to = cert.get()->valid_to;

      bool keyMatches = false;
      if (certType == espConfig::CertType::HTTPS_SERVER_CERT) {
        keyMatches = validateKeyCertPair(m_httpsCertsConfig.privateKey, m_httpsCertsConfig.serverCert, "HTTPS");
      } else if (certType == espConfig::CertType::MQTT_CLIENT) {
        keyMatches = validateKeyCertPair(m_mqttSslConfig.clientKey, m_mqttSslConfig.clientCert, "MQTT");
      }

      certificates.emplace_back(CertificateStatus{
          certType,
          issuer,
          subject,
          serial,
          fmt::format("{:02X}", fmt::join(sha1_hash, ":")),
          {.from = fmt::format("{:04}-{:02}-{:02}T{:02}:{:02}:{:02}Z", valid_from.year, valid_from.mon,
                               valid_from.day, valid_from.hour, valid_from.min, valid_from.sec),
           .to = fmt::format("{:04}-{:02}-{:02}T{:02}:{:02}:{:02}Z", valid_to.year, valid_to.mon,
                             valid_to.day, valid_to.hour, valid_to.min, valid_to.sec)},
          keyMatches});
    } else if(not certStr.empty() && isPrivateKey){
      ScopedPk pk;

      int ret = mbedtls_pk_parse_key(pk.get(), reinterpret_cast<const unsigned char*>(certStr.c_str()), mbedtlsParseLen(certStr), nullptr, 0, nullptr, nullptr);

      if (ret != 0) {
          char error_buf[100];
          mbedtls_strerror(ret, error_buf, sizeof(error_buf));
          ESP_LOGE(TAG, "Private key parsing failed: %s (error code: %d)", error_buf, ret);
          continue;
      }

      mbedtls_pk_type_t key_type = mbedtls_pk_get_type(pk.get());
      if (key_type != MBEDTLS_PK_RSA && key_type != MBEDTLS_PK_ECKEY && key_type != MBEDTLS_PK_ECKEY_DH && key_type != MBEDTLS_PK_ECDSA) {
          ESP_LOGE(TAG, "Unsupported private key type: %d", key_type);
          continue;
      }
      certificates.emplace_back(CertificateStatus{
          .type = certType,
          .keyType = std::string(mbedtls_pk_get_name(pk.get()))});
    }
  }
  return certificates;
}

bool ConfigManager::setNVSLogLevel(const uint8_t level) {
  if(!m_nvsHandle){
    esp_err_t err = nvs_open("SAVED_DATA", NVS_READWRITE, &m_nvsHandle);
    if (err != ESP_OK) {
      ESP_LOGE(TAG, "Error (%04#x) opening NVS handle!", err);
      return false;
    }
  }
  const char* key = "GlobalLogLevel";
  esp_err_t set_err = nvs_set_u8(m_nvsHandle, key, level);

  if (set_err != ESP_OK) {
    ESP_LOGE(TAG, "Failed to set blob in NVS for key '%s': %04#x", key, set_err);
    return false;
  }

  esp_err_t commit_err = nvs_commit(m_nvsHandle);
  if (commit_err != ESP_OK) {
    ESP_LOGE(TAG, "Failed to commit NVS changes for key '%s': %04#x", key, commit_err);
    return false;
  } else {
    ESP_LOGI(TAG, "Log level '%d' successfully commited to NVS.", level);
  }
  return true;
}

bool ConfigManager::getNVSLogLevel(uint8_t &level) {
  if(!m_nvsHandle){
    esp_err_t err = nvs_open("SAVED_DATA", NVS_READWRITE, &m_nvsHandle);
    if (err != ESP_OK) {
      ESP_LOGE(TAG, "Error (%04#x) opening NVS handle!", err);
      return false;
    }
  }
  const char *key = "GlobalLogLevel";
  esp_err_t get_err = nvs_get_u8(m_nvsHandle, key, &level);
  if (get_err == ESP_ERR_NVS_NOT_FOUND){
    ESP_LOGW(TAG, "%s not found in NVS. Returning default level.",
              key);
    level = esp_log_get_default_level();
    return true;
  }
  if (get_err != ESP_OK) {
    ESP_LOGE(TAG, "Failed to get blob in NVS for key '%s': %04#x", key,
              get_err);
    return false;
  }
  return true;
}

bool ConfigManager::setBacklogMaxSize(const uint16_t size){
  if(!m_nvsHandle){
    esp_err_t err = nvs_open("SAVED_DATA", NVS_READWRITE, &m_nvsHandle);
    if (err != ESP_OK) {
      ESP_LOGE(TAG, "Error (%04#x) opening NVS handle!", err);
      return false;
    }
  }
  const char* key = "BackLogMaxSize";
  esp_err_t set_err = nvs_set_u16(m_nvsHandle, key, size);

  if (set_err != ESP_OK) {
    ESP_LOGE(TAG, "Failed to set blob in NVS for key '%s': %04#x", key, set_err);
    return false;
  }
  esp_err_t commit_err = nvs_commit(m_nvsHandle);
  if (commit_err != ESP_OK) {
    ESP_LOGE(TAG, "Failed to commit NVS changes for key '%s': %04#x", key, commit_err);
    return false;
  } else {
    ESP_LOGI(TAG, "BackLogMaxSize set to '%d' and successfully commited to NVS.", size);
  }
  return true;
}

bool ConfigManager::getBacklogMaxSize(uint16_t &size) {
  if(!m_nvsHandle){
    esp_err_t err = nvs_open("SAVED_DATA", NVS_READWRITE, &m_nvsHandle);
    if (err != ESP_OK) {
      ESP_LOGE(TAG, "Error (%04#x) opening NVS handle!", err);
      return false;
    }
  }
  const char *key = "BackLogMaxSize";
  esp_err_t get_err = nvs_get_u16(m_nvsHandle, key, &size);
  if (get_err == ESP_ERR_NVS_NOT_FOUND){
    ESP_LOGW(TAG, "%s not found in NVS.", key);
    return true;
  }
  if (get_err != ESP_OK) {
    ESP_LOGE(TAG, "Failed to get blob in NVS for key '%s': %04#x", key,
              get_err);
    return false;
  }
  return true;
}
