#include "ConfigManager.hpp"
#include "MbedtlsHelpers.hpp"
#include "cJSON.h"
#include "config.hpp"
#include <ranges>
#include <string>
#include <vector>
#include <limits>
#include "esp_log.h"
#include "fmt/ranges.h"
#include "mbedtls/sha256.h"
#include "mbedtls/x509.h"
#include "msgpack.h"
#include <LittleFS.h>
#include "fmt/format.h"
#include "nvs.h"
#include <mbedtls/entropy.h>
#include <mbedtls/ctr_drbg.h>
#include <mbedtls/x509_crt.h>
#include <mbedtls/pk.h>
#include <mbedtls/error.h>


const char* ConfigManager::TAG = "ConfigManager";

using crypto::ScopedEntropy;
using crypto::ScopedCtrDrbg;
using crypto::ScopedPk;
using crypto::ScopedX509Crt;

/**
 * @brief Initialize ConfigManager and build the configuration key-to-field map.
 *
 * Constructs a ConfigManager instance, marks it uninitialized, and populates
 * the internal m_configMap with mappings from configuration keys (for the
 * "mqtt" and "misc" sections) to pointers of their corresponding in-memory
 * fields. The map enables unified serialization/deserialization and JSON
 * handling for both MQTT and miscellaneous settings.
 */
ConfigManager::ConfigManager() : m_isInitialized(false) {
  m_configMap = { 
    {"mqtt",{ 
      // MQTT Config
      {"mqttBroker", &m_mqttConfig.mqttBroker},
      {"mqttPort", &m_mqttConfig.mqttPort},
      {"mqttClientId", &m_mqttConfig.mqttClientId},
      {"mqttUsername", &m_mqttConfig.mqttUsername},
      {"mqttPassword", &m_mqttConfig.mqttPassword},
      {"hassMqttDiscoveryEnabled",
        &m_mqttConfig.hassMqttDiscoveryEnabled},
      {"lwtTopic", &m_mqttConfig.lwtTopic},
      {"hkTopic", &m_mqttConfig.hkTopic},
      {"lockStateTopic", &m_mqttConfig.lockStateTopic},
      {"lockStateCmd", &m_mqttConfig.lockStateCmd},
      {"lockCStateCmd", &m_mqttConfig.lockCStateCmd},
      {"lockTStateCmd", &m_mqttConfig.lockTStateCmd},
      {"btrLvlCmdTopic", &m_mqttConfig.btrLvlCmdTopic},
      {"hkAltActionTopic", &m_mqttConfig.hkAltActionTopic},
      {"lockCustomStateTopic", &m_mqttConfig.lockCustomStateTopic},
      {"lockCustomStateCmd", &m_mqttConfig.lockCustomStateCmd},
      {"lockEnableCustomState", &m_mqttConfig.lockEnableCustomState},
      {"nfcTagNoPublish", &m_mqttConfig.nfcTagNoPublish},
      {"useSSL", &m_mqttConfig.useSSL},
      {"allowInsecure", &m_mqttConfig.allowInsecure},
      {"customLockStates", &m_mqttConfig.customLockStates},
      {"customLockActions", &m_mqttConfig.customLockActions}
      }
    },
{
      "ssl", {
        {"caCert", &m_mqttSslConfig.caCert},
        {"clientCert", &m_mqttSslConfig.clientCert},
        {"clientKey", &m_mqttSslConfig.clientKey},
      }
    },
    {
      "https", {
        {"serverCert", &m_httpsCertsConfig.serverCert},
        {"privateKey", &m_httpsCertsConfig.privateKey},
        {"caCert", &m_httpsCertsConfig.caCert},
      }
    },
    {"misc",{
      // Miscellaneous Config
      {"deviceName", &m_miscConfig.deviceName},
      {"otaPasswd", &m_miscConfig.otaPasswd},
      {"hk_key_color", &m_miscConfig.hk_key_color},
      {"setupCode", &m_miscConfig.setupCode},
      {"lockAlwaysUnlock", &m_miscConfig.lockAlwaysUnlock},
      {"lockAlwaysLock", &m_miscConfig.lockAlwaysLock},
      {"hkAuthPrecomputeEnabled", &m_miscConfig.hkAuthPrecomputeEnabled},
      {"nfcFastPollingEnabled", &m_miscConfig.nfcFastPollingEnabled},
      {"controlPin", &m_miscConfig.controlPin},
      {"hsStatusPin", &m_miscConfig.hsStatusPin},
      {"webAuthEnabled", &m_miscConfig.webAuthEnabled},
      {"webUsername", &m_miscConfig.webUsername},
      {"webPassword", &m_miscConfig.webPassword},
      {"webHttpsEnabled", &m_miscConfig.webHttpsEnabled},
      {"nfcGpioPins", &m_miscConfig.nfcGpioPins},
      {"nfcPinsPreset", &m_miscConfig.nfcPinsPreset},
      {"btrLowStatusThreshold", &m_miscConfig.btrLowStatusThreshold},
      {"proxBatEnabled", &m_miscConfig.proxBatEnabled},
      {"ethernetEnabled", &m_miscConfig.ethernetEnabled},
      {"ethActivePreset", &m_miscConfig.ethActivePreset},
      {"ethPhyType", &m_miscConfig.ethPhyType},
      {"ethSpiBus", &m_miscConfig.ethSpiBus},
      {"ethRmiiConfig", &m_miscConfig.ethRmiiConfig},
      {"ethSpiConfig", &m_miscConfig.ethSpiConfig},
      {"logLevel", &m_miscConfig.logLevel}
    }
    },
    {
      "actions", {
        {"nfcNeopixelPin", &m_actionsConfig.nfcNeopixelPin},
        {"neoPixelType", &m_actionsConfig.neoPixelType},
        {"neopixelSuccessColor", &m_actionsConfig.neopixelSuccessColor},
        {"neopixelFailureColor", &m_actionsConfig.neopixelFailureColor},
        {"neopixelSuccessTime", &m_actionsConfig.neopixelSuccessTime},
        {"neopixelFailTime", &m_actionsConfig.neopixelFailTime},
        {"neopixelTagEventTime", &m_actionsConfig.neopixelTagEventTime},
        {"neopixelTagEventColor", &m_actionsConfig.neopixelTagEventColor},
        {"nfcSuccessPin", &m_actionsConfig.nfcSuccessPin},
        {"nfcSuccessTime", &m_actionsConfig.nfcSuccessTime},
        {"nfcSuccessHL", &m_actionsConfig.nfcSuccessHL},
        {"nfcFailPin", &m_actionsConfig.nfcFailPin},
        {"nfcFailTime", &m_actionsConfig.nfcFailTime},
        {"nfcFailHL", &m_actionsConfig.nfcFailHL},
        {"tagEventPin", &m_actionsConfig.tagEventPin},
        {"tagEventTimeout", &m_actionsConfig.tagEventTimeout},
        {"tagEventHL", &m_actionsConfig.tagEventHL},
        {"gpioActionPin", &m_actionsConfig.gpioActionPin},
        {"gpioActionLockState", &m_actionsConfig.gpioActionLockState},
        {"gpioActionUnlockState", &m_actionsConfig.gpioActionUnlockState},
        {"gpioActionMomentaryEnabled", &m_actionsConfig.gpioActionMomentaryEnabled},
        {"hkGpioControlledState", &m_actionsConfig.hkGpioControlledState},
        {"gpioActionMomentaryTimeout", &m_actionsConfig.gpioActionMomentaryTimeout},
        {"hkDumbSwitchMode", &m_actionsConfig.hkDumbSwitchMode},
        {"hkAltActionPin", &m_actionsConfig.hkAltActionPin},
        {"hkAltActionTimeout", &m_actionsConfig.hkAltActionTimeout},
        {"hkAltActionGpioState", &m_actionsConfig.hkAltActionGpioState},
        {"hkAltActionInitPin", &m_actionsConfig.hkAltActionInitPin},
        {"hkAltActionInitLedPin", &m_actionsConfig.hkAltActionInitLedPin},
        {"hkAltActionInitTimeout", &m_actionsConfig.hkAltActionInitTimeout},
      }
    }
  };
}

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

  esp_err_t err = nvs_open("SAVED_DATA", NVS_READWRITE, &m_nvsHandle);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "Error (%s) opening NVS handle!", esp_err_to_name(err));
    return false;
  }

  nvs_stats_t nvs_stats;
  nvs_get_stats(NULL, &nvs_stats);
  ESP_LOGI(TAG,"Count: UsedEntries = (%lu), FreeEntries = (%lu), AvailableEntries = (%lu), AllEntries = (%lu)\n",
       nvs_stats.used_entries, nvs_stats.free_entries, nvs_stats.available_entries, nvs_stats.total_entries);

  m_isInitialized = true;

  ESP_LOGI(TAG, "Loading configurations from NVS...");
  loadConfigFromNvs("MQTTDATA");
  loadConfigFromNvs("MQTTSSLDATA");
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
    
    esp_err_t err_mqtt = nvs_erase_key(m_nvsHandle, "MQTTDATA");
    esp_err_t err_ssl = nvs_erase_key(m_nvsHandle, "MQTTSSLDATA");

    bool mqtt_ok = (err_mqtt == ESP_OK || err_mqtt == ESP_ERR_NVS_NOT_FOUND);
    bool ssl_ok = (err_ssl == ESP_OK || err_ssl == ESP_ERR_NVS_NOT_FOUND);

    if (!mqtt_ok) {
      ESP_LOGE(TAG, "Failed to erase MQTTDATA: %s", esp_err_to_name(err_mqtt));
    }
    if (!ssl_ok) {
      ESP_LOGE(TAG, "Failed to erase MQTTSSLDATA: %s", esp_err_to_name(err_ssl));
    }

    if (mqtt_ok && ssl_ok) {
      esp_err_t commit_err = nvs_commit(m_nvsHandle);
      if (commit_err == ESP_OK) {
        return true;
      }
      ESP_LOGE(TAG, "Failed to commit deleteConfig: %s", esp_err_to_name(commit_err));
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
    ESP_LOGE(TAG, "Failed to erase HTTPSDATA: %s", esp_err_to_name(err));
    return false;
  } else {
    static_assert(std::is_void_v<ConfigType> && false, "Unsupported ConfigType for deleteConfig");
  }
  return false;
}
template bool ConfigManager::deleteConfig<espConfig::mqttConfig_t>();
template bool ConfigManager::deleteConfig<espConfig::misc_config_t>();

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
    ESP_LOGE(TAG, "Error (%s) getting blob size for key '%s'",
             esp_err_to_name(err), key);
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
    ESP_LOGE(TAG, "Error (%s) reading blob for key '%s'", esp_err_to_name(err),
             key);
    return;
  }

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
    } else if(!strcmp(key, "MISCDATA")){
      deserialize(obj, "misc");
      deserialize(obj, "actions");
    } else if(!strcmp(key, "HTTPSDATA")){
      deserialize(obj, "https");
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

  std::vector<uint8_t> buf;
  if (!strcmp(key, "MISCDATA")){
    buf = serialize<espConfig::misc_config_t>();
  } else if (!strcmp(key, "MQTTSSLDATA")){
    buf = serialize<espConfig::mqtt_ssl_t>();
  } else if (!strcmp(key, "MQTTDATA")){
    buf = serialize<espConfig::mqttConfig_t>();
  } else if (!strcmp(key, "HTTPSDATA")){
    buf = serialize<espConfig::https_certs_t>();
  }

  esp_err_t set_err = nvs_set_blob(m_nvsHandle, key, buf.data(), buf.size());

  if (set_err != ESP_OK) {
    ESP_LOGE(TAG, "Failed to set blob in NVS for key '%s': %s", key,
             esp_err_to_name(set_err));
    return false;
  }

  esp_err_t commit_err = nvs_commit(m_nvsHandle);
  if (commit_err != ESP_OK) {
    ESP_LOGE(TAG, "Failed to commit NVS changes for key '%s': %s", key,
             esp_err_to_name(commit_err));
    return false;
  }
  return true;
}

/**
 * @brief Populates in-memory configuration fields from a MessagePack map.
 *
 * Deserializes a MessagePack map object into the ConfigManager's in-memory configuration
 * entries identified by the provided section name (`type`). For each string key found
 * in the MessagePack map that exists in the internal `m_configMap[type]`, the matching
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
 *             corresponding key → field mapping in `m_configMap`.
 */
void ConfigManager::deserialize(msgpack_object obj, std::string type) {
  if (obj.type == MSGPACK_OBJECT_MAP) {
    msgpack_object_kv *map = obj.via.map.ptr;
    msgpack_object_kv *const end = obj.via.map.ptr + obj.via.map.size;
    std::span range(map, end);
    for (auto v : range) {
      if (v.key.type == MSGPACK_OBJECT_STR) {
        std::string key(v.key.via.str.ptr, v.key.via.str.size);
        if (!m_configMap.contains(type) || !m_configMap[type].contains(key))
          continue;

        std::visit([&](auto&& arg) {
          using T = std::decay_t<decltype(arg)>;
          if constexpr (std::is_pointer_v<T>) {
            using PointeeType = std::remove_pointer_t<T>;

            switch (v.val.type) {
            case MSGPACK_OBJECT_STR: {
              if constexpr (std::is_same_v<PointeeType, std::string>) {
                arg->assign(std::string(v.val.via.str.ptr, v.val.via.str.size));
              }
              break;
            }
            case MSGPACK_OBJECT_BOOLEAN: {
              if constexpr (std::is_same_v<PointeeType, bool>) {
                *arg = v.val.via.boolean;
              }
              break;
            }
            case MSGPACK_OBJECT_POSITIVE_INTEGER: {
              if constexpr (std::is_same_v<PointeeType, uint8_t> || std::is_same_v<PointeeType, uint16_t>) {
                if (v.val.via.u64 > std::numeric_limits<PointeeType>::max()) {
                  ESP_LOGW(TAG, "Value overflow for '%s': %llu exceeds max %u", 
                           key.c_str(), v.val.via.u64, std::numeric_limits<PointeeType>::max());
                  break;
                }
                *arg = static_cast<PointeeType>(v.val.via.u64);
              }
              break;
            }
            case MSGPACK_OBJECT_ARRAY: {
              auto msgpack_elements = std::ranges::subrange(v.val.via.array.ptr, v.val.via.array.ptr + v.val.via.array.size);
              auto integer_view = msgpack_elements | std::ranges::views::transform([](const msgpack_object& o){return o.via.u64;});

              if constexpr (std::is_same_v<PointeeType, std::array<uint8_t, 4>>) {
                if (msgpack_elements.size() != 4) {
                  ESP_LOGW(TAG, "Validation failed for '%s': array size is not 4.", key.c_str());
                  break;
                }
                std::ranges::copy(integer_view, arg->begin());
              } else if constexpr (std::is_same_v<PointeeType, std::array<uint8_t, 5>>) {
                if (msgpack_elements.size() != 5) {
                  ESP_LOGW(TAG, "Validation failed for '%s': array size is not 5.", key.c_str());
                  break;
                }
                std::ranges::copy(integer_view, arg->begin());
              } else if constexpr (std::is_same_v<PointeeType, std::array<uint8_t, 7>>) {
                if (msgpack_elements.size() != 7) {
                  ESP_LOGW(TAG, "Validation failed for '%s': array size is not 7.", key.c_str());
                  break;
                }
                std::ranges::copy(integer_view, arg->begin());
              } else if constexpr (std::is_same_v<PointeeType, std::map<espConfig::actions_config_t::colorMap, uint8_t>>) {
                std::ranges::for_each(msgpack_elements, [&](const msgpack_object& o) {
                    if (o.type == MSGPACK_OBJECT_ARRAY && o.via.array.size == 2) {
                        const msgpack_object* inner_array_ptr = o.via.array.ptr;
                        uint8_t key_val = inner_array_ptr[0].via.u64;
                        uint8_t value_val = inner_array_ptr[1].via.u64;
                        (*arg)[static_cast<espConfig::actions_config_t::colorMap>(key_val)] = static_cast<uint8_t>(value_val);
                    }
                });
              } else if constexpr (std::is_same_v<PointeeType, std::map<std::string, uint8_t>>) {
                std::ranges::for_each(msgpack_elements, [&](const msgpack_object& o) {
                    if (o.type == MSGPACK_OBJECT_ARRAY && o.via.array.size >= 2) {
                        const msgpack_object* inner_array_ptr = o.via.array.ptr;
                        std::string key_str(inner_array_ptr[0].via.str.ptr, inner_array_ptr[0].via.str.size);
                        uint64_t value_val = inner_array_ptr[1].via.u64;
                        (*arg)[key_str] = static_cast<uint8_t>(value_val);
                    }
                });
              }
              break;
            }
            case MSGPACK_OBJECT_MAP: {
              if constexpr (std::is_same_v<PointeeType, std::map<std::string, uint8_t>>) {
                arg->clear();
                msgpack_object_kv *sub_map = v.val.via.map.ptr;
                msgpack_object_kv *const sub_end = v.val.via.map.ptr + v.val.via.map.size;
                for (; sub_map != sub_end; ++sub_map) {
                  if (sub_map->key.type == MSGPACK_OBJECT_STR && sub_map->val.type == MSGPACK_OBJECT_POSITIVE_INTEGER) {
                    std::string sub_key(sub_map->key.via.str.ptr, sub_map->key.via.str.size);
                    uint8_t sub_value = static_cast<uint8_t>(sub_map->val.via.u64);
                    arg->emplace(sub_key, sub_value);
                  }
                }
              }
              break;
            }
            default:
              ESP_LOGW(TAG, "DON'T KNOW THIS ONE! - %s (%d) = %d", key.c_str(), v.val.type, v.val.via.u64);
            }
          }
        }, m_configMap[type][key]);
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
 * @return std::vector<uint8_t> Byte vector containing the MessagePack-encoded configuration. 
 */
std::vector<uint8_t> ConfigManager::serialize() {
  msgpack_sbuffer sbuf;
  msgpack_sbuffer_init(&sbuf);
  msgpack_packer pk;
  msgpack_packer_init(&pk, &sbuf, msgpack_sbuffer_write);
  ConfigMapType configMap;
  if constexpr (std::is_same_v<espConfig::misc_config_t, ConfigType>){
    configMap = m_configMap["misc"];
    configMap.insert(m_configMap["actions"].begin(), m_configMap["actions"].end());
  } else if constexpr (std::is_same_v<espConfig::actions_config_t, ConfigType>){
    configMap = m_configMap["actions"];
    configMap.insert(m_configMap["misc"].begin(), m_configMap["misc"].end());
  } else if constexpr (std::is_same_v<espConfig::mqtt_ssl_t, ConfigType>){
    configMap = m_configMap["ssl"];
  } else if constexpr (std::is_same_v<espConfig::mqttConfig_t, ConfigType>){
    configMap = m_configMap["mqtt"];
  } else if constexpr (std::is_same_v<espConfig::https_certs_t, ConfigType>){
    configMap = m_configMap["https"];
  }
  msgpack_pack_map(&pk, configMap.size()); // Pack the map size

  for (const auto &pair : configMap) {
    msgpack_pack_str(&pk, pair.first.size()); // Pack the key (string) size
    msgpack_pack_str_body(&pk, pair.first.data(), pair.first.size()); // Pack the key (string) body

    std::visit([&](auto&& arg) {
      using T = std::decay_t<decltype(arg)>;
      if constexpr (std::is_pointer_v<T>) {
        using PointeeType = std::remove_pointer_t<T>;

        if constexpr (std::is_same_v<PointeeType, std::string>) {
          msgpack_pack_str(&pk, arg->size());
          msgpack_pack_str_body(&pk, arg->data(), arg->size());
        } else if constexpr (std::is_same_v<PointeeType, bool>) {
          if (*arg) {
            msgpack_pack_true(&pk);
          } else {
            msgpack_pack_false(&pk);
          }
        } else if constexpr (std::is_same_v<PointeeType, uint8_t>) {
          msgpack_pack_unsigned_char(&pk, *arg);
        } else if constexpr (std::is_same_v<PointeeType, uint16_t>) {
          msgpack_pack_unsigned_short(&pk, *arg);
        } else if constexpr (std::is_same_v<PointeeType, std::array<uint8_t, 4>>) {
          msgpack_pack_array(&pk, arg->size());
          for (const auto& val : *arg) {
            msgpack_pack_unsigned_char(&pk, val);
          }
        } else if constexpr (std::is_same_v<PointeeType, std::array<uint8_t, 5>>) {
          msgpack_pack_array(&pk, arg->size());
          for (const auto& val : *arg) {
            msgpack_pack_unsigned_char(&pk, val);
          }
        } else if constexpr (std::is_same_v<PointeeType, std::array<uint8_t, 7>>) {
          msgpack_pack_array(&pk, arg->size());
          for (const auto& val : *arg) {
            msgpack_pack_unsigned_char(&pk, val);
          }
        } else if constexpr (std::is_same_v<PointeeType, std::map<espConfig::actions_config_t::colorMap, uint8_t>>) {
          msgpack_pack_array(&pk, arg->size());
          for (const auto& map_pair : *arg) {
            msgpack_pack_array(&pk, 2);
            msgpack_pack_unsigned_char(&pk, static_cast<uint8_t>(map_pair.first));
            msgpack_pack_unsigned_char(&pk, map_pair.second);
          }
        } else if constexpr (std::is_same_v<PointeeType, std::map<std::string, uint8_t>>) {
          msgpack_pack_map(&pk, arg->size());
          for (const auto& map_pair : *arg) {
            msgpack_pack_str(&pk, map_pair.first.size());
            msgpack_pack_str_body(&pk, map_pair.first.data(), map_pair.first.size());
            msgpack_pack_unsigned_char(&pk, map_pair.second);
          }     }
      }
    }, pair.second);
  }

  std::vector<uint8_t> serialized_data(reinterpret_cast<uint8_t*>(sbuf.data), reinterpret_cast<uint8_t*>(sbuf.data) + sbuf.size);
  msgpack_sbuffer_destroy(&sbuf);
  return serialized_data;
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

  ConfigMapType* configMapPtr = nullptr;
  if constexpr (std::is_same_v<ConfigType, espConfig::misc_config_t>) {
    configMapPtr = &m_configMap["misc"];
  } else if constexpr (std::is_same_v<ConfigType, espConfig::actions_config_t>) {
    configMapPtr = &m_configMap["actions"];
  } else if constexpr (std::is_same_v<ConfigType, espConfig::mqttConfig_t>) {
    configMapPtr = &m_configMap["mqtt"];
  } else {
    ESP_LOGE(TAG, "Invalid configuration type specified.");
    return "";
  }
  
  for (cJSON *it = root.get()->child; it != NULL; it = it->next) {
    std::string keyStr = it->string;
    auto config_entry = configMapPtr->find(keyStr);

    if (config_entry != configMapPtr->end()) {
      std::visit([&](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_pointer_v<T>) {
          using PointeeType = std::remove_pointer_t<T>;

          if constexpr (std::is_same_v<PointeeType, std::string>) {
            if (cJSON_IsString(it)) {
              arg->assign(it->valuestring);
            } else {
              ESP_LOGW(TAG, "Validation failed for '%s': type mismatch, expected string.", keyStr.c_str());
            }
          } else if constexpr (std::is_same_v<PointeeType, bool>) {
            if (cJSON_IsBool(it)) {
              *arg = cJSON_IsTrue(it);
            } else if (cJSON_IsNumber(it)) {
              *arg = (it->valueint != 0);
            } else {
              ESP_LOGW(TAG, "Validation failed for '%s': type mismatch, expected boolean.", keyStr.c_str());
            }
          } else if constexpr (std::is_integral_v<PointeeType>) {
            if (cJSON_IsNumber(it)) {
              if (it->valueint < 0 || it->valueint > std::numeric_limits<PointeeType>::max()) {
                ESP_LOGW(TAG, "Value out of range for '%s': %d", keyStr.c_str(), it->valueint);
              } else {
                *arg = static_cast<PointeeType>(it->valueint);
              }
            } else {
              ESP_LOGW(TAG, "Validation failed for '%s': type mismatch, expected number.", keyStr.c_str());
            }
          } else if constexpr (std::is_same_v<PointeeType, std::array<uint8_t, 4>> ||
                               std::is_same_v<PointeeType, std::array<uint8_t, 5>> ||
                               std::is_same_v<PointeeType, std::array<uint8_t, 7>>) {
            if (cJSON_IsArray(it)) {
              int array_size = cJSON_GetArraySize(it);
              if (array_size == arg->size()) {
                bool array_success = true;
                for (int i = 0; i < array_size; ++i) {
                  cJSON *sub_item = cJSON_GetArrayItem(it, i);
                  if (cJSON_IsNumber(sub_item)) {
                    (*arg)[i] = static_cast<uint8_t>(sub_item->valueint);
                  } else {
                    array_success = false;
                    break;
                  }
                }
                if (!array_success) {
                  ESP_LOGW(TAG, "Validation failed for '%s': array contains non-numeric elements.", keyStr.c_str());
                }
              } else {
                ESP_LOGW(TAG, "Validation failed for '%s': incorrect array size. Expected %zu, got %d.", keyStr.c_str(), arg->size(), array_size);
              }
            } else {
              ESP_LOGW(TAG, "Validation failed for '%s': type mismatch, expected array.", keyStr.c_str());
            }
          } else if constexpr (std::is_same_v<PointeeType, std::map<espConfig::actions_config_t::colorMap, uint8_t>>) {
            if (cJSON_IsArray(it)) {
              arg->clear();
              cJSON *inner_array;
              cJSON_ArrayForEach(inner_array, it) {
                if (cJSON_IsArray(inner_array) && cJSON_GetArraySize(inner_array) == 2) {
                  cJSON *key_json = cJSON_GetArrayItem(inner_array, 0);
                  cJSON *value_json = cJSON_GetArrayItem(inner_array, 1);
                  if (cJSON_IsNumber(key_json) && cJSON_IsNumber(value_json)) {
                    arg->emplace(
                        static_cast<espConfig::actions_config_t::colorMap>(key_json->valueint),
                        static_cast<uint8_t>(value_json->valueint)
                    );
                  }
                }
              }
            } else {
              ESP_LOGW(TAG, "Validation failed for '%s': type mismatch, expected an array of [key, value] pairs.", keyStr.c_str());
            }
          } else if constexpr (std::is_same_v<PointeeType, std::map<std::string, uint8_t>>) {
            if (cJSON_IsObject(it)) {
              arg->clear();
              cJSON* sub_obj_item;
              cJSON_ArrayForEach(sub_obj_item, it) {
                if (cJSON_IsNumber(sub_obj_item)) {
                  (*arg)[sub_obj_item->string] = static_cast<uint8_t>(sub_obj_item->valueint);
                }
              }
            } else {
              ESP_LOGW(TAG, "Validation failed for '%s': type mismatch, expected an object.", keyStr.c_str());
            }
          }
        }
      }, config_entry->second);
    } else {
      ESP_LOGW(TAG, "'%s' is not a valid configuration key and will be ignored.", keyStr.c_str());
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

  ConfigMapType configMap;
  if constexpr (std::is_same_v<espConfig::misc_config_t, ConfigType>){
    configMap = m_configMap["misc"];
  } else if constexpr (std::is_same_v<espConfig::actions_config_t, ConfigType>){
    configMap = m_configMap["actions"];
  } else if constexpr (std::is_same_v<espConfig::mqttConfig_t, ConfigType>){
    configMap = m_configMap["mqtt"];
  }
    for (const auto &pair : configMap) {
        const std::string& key = pair.first;
        std::visit([&](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_pointer_v<T>) {
                using PointeeType = std::remove_pointer_t<T>;

                if constexpr (std::is_same_v<PointeeType, std::string>) {
                    if(key.contains("Password") || key.contains("Passwd")){
                        cJSON_AddStringToObject(root.get(), key.c_str(), "********");
                    } else {
                        cJSON_AddStringToObject(root.get(), key.c_str(), arg->c_str());
                    }
                } else if constexpr (std::is_same_v<PointeeType, bool>) {
                    cJSON_AddBoolToObject(root.get(), key.c_str(), *arg);
                } else if constexpr (std::is_same_v<PointeeType, uint8_t> || std::is_same_v<PointeeType, uint16_t>) {
                    cJSON_AddNumberToObject(root.get(), key.c_str(), static_cast<double>(*arg));
                } else if constexpr (std::is_same_v<PointeeType, std::array<uint8_t, 4>> ||
                                     std::is_same_v<PointeeType, std::array<uint8_t, 5>> ||
                                     std::is_same_v<PointeeType, std::array<uint8_t, 7>>) {
                    cJSON *array = cJSON_CreateArray();
                    if (array) {
                        for (const auto& val : *arg) {
                            cJSON_AddItemToArray(array, cJSON_CreateNumber(static_cast<double>(val)));
                        }
                        cJSON_AddItemToObject(root.get(), key.c_str(), array);
                    }
                } else if constexpr (std::is_same_v<PointeeType, std::map<espConfig::actions_config_t::colorMap, uint8_t>>) {
                    cJSON *array_of_arrays = cJSON_CreateArray();
                    if (array_of_arrays) {
                        for (const auto& map_pair : *arg) {
                            cJSON *inner_array = cJSON_CreateArray();
                            cJSON_AddItemToArray(inner_array, cJSON_CreateNumber(static_cast<double>(map_pair.first))); // Enum key as integer
                            cJSON_AddItemToArray(inner_array, cJSON_CreateNumber(static_cast<double>(map_pair.second)));
                            cJSON_AddItemToArray(array_of_arrays, inner_array);
                        }
                        cJSON_AddItemToObject(root.get(), key.c_str(), array_of_arrays);
                    }
                } else if constexpr (std::is_same_v<PointeeType, std::map<std::string, uint8_t>>) {
                    cJSON *map_obj = cJSON_CreateObject();
                    if (map_obj) {
                        for (const auto& map_pair : *arg) {
                            cJSON_AddNumberToObject(map_obj, map_pair.first.c_str(), static_cast<double>(map_pair.second));
                        }
                        cJSON_AddItemToObject(root.get(), key.c_str(), map_obj);
                    }
                }
            }
        }, pair.second);
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

    ConfigMapType configMap;
    if constexpr (std::is_same_v<ConfigType, espConfig::misc_config_t>){
      configMap = m_configMap["misc"];
    } else if constexpr (std::is_same_v<ConfigType, espConfig::actions_config_t>){
      configMap = m_configMap["actions"];
    } else if constexpr (std::is_same_v<ConfigType, espConfig::mqttConfig_t>){
      configMap = m_configMap["mqtt"];
    } else {
      static_assert(std::is_void_v<ConfigType> && false, "Unsupported ConfigType for deserializeFromJson");
    }
    bool success = true;
    cJSON *item = root.get()->child;
    while (item) {
        std::string key = item->string;
        if (configMap.contains(key)) {
            std::visit([&](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_pointer_v<T>) {
                    using PointeeType = std::remove_pointer_t<T>;

                    if constexpr (std::is_same_v<PointeeType, std::string>) {
                        if (cJSON_IsString(item)) {
                            arg->assign(item->valuestring);
                        } else {
                            ESP_LOGW(TAG, "Validation failed for '%s': type mismatch, expected string.", key.c_str());
                            success = false;
                        }
                    } else if constexpr (std::is_same_v<PointeeType, bool>) {
                        if (cJSON_IsBool(item)) {
                            *arg = cJSON_IsTrue(item);
                        } else if(cJSON_IsNumber(item)) {
                            *arg = static_cast<PointeeType>(item->valueint);
                        } else {
                            ESP_LOGW(TAG, "Validation failed for '%s': type mismatch, expected boolean.", key.c_str());
                            success = false;
                        }
                    } else if constexpr (std::is_same_v<PointeeType, uint8_t> || std::is_same_v<PointeeType, uint16_t>) {
                        if (cJSON_IsNumber(item)) {
                            if (item->valuedouble < 0 || item->valuedouble > std::numeric_limits<PointeeType>::max()) {
                                ESP_LOGW(TAG, "Value out of range for '%s': %f", key.c_str(), item->valuedouble);
                                success = false;
                            } else {
                                *arg = static_cast<PointeeType>(item->valuedouble);
                            }
                        } else {
                            ESP_LOGW(TAG, "Validation failed for '%s': type mismatch, expected number.", key.c_str());
                            success = false;
                        }
                    } else if constexpr (std::is_same_v<PointeeType, std::array<uint8_t, 4>> ||
                                      std::is_same_v<PointeeType, std::array<uint8_t, 5>> ||
                                      std::is_same_v<PointeeType, std::array<uint8_t, 7>>) {
                        if (cJSON_IsArray(item)) {
                            int array_size = cJSON_GetArraySize(item);
                            if (array_size == arg->size()) {
                                bool array_success = true;
                                for (int i = 0; i < array_size; ++i) {
                                    cJSON *sub_item = cJSON_GetArrayItem(item, i);
                                    if (cJSON_IsNumber(sub_item)) {
                                        (*arg)[i] = static_cast<uint8_t>(sub_item->valuedouble);
                                    } else {
                                        array_success = false;
                                        break;
                                    }
                                }
                                if (!array_success) {
                                    ESP_LOGW(TAG, "Validation failed for '%s': array contains non-numeric elements.", key.c_str());
                                    success = false;
                                }
                            } else {
                                ESP_LOGW(TAG, "Validation failed for '%s': incorrect array size. Expected %zu, got %d.", key.c_str(), arg->size(), array_size);
                                success = false;
                            }
                        } else {
                            ESP_LOGW(TAG, "Validation failed for '%s': type mismatch, expected array.", key.c_str());
                            success = false;
                        }
                    } else if constexpr (std::is_same_v<PointeeType, std::map<espConfig::actions_config_t::colorMap, uint8_t>>) {
                        if (cJSON_IsArray(item)) {
                            arg->clear();
                            int array_size = cJSON_GetArraySize(item);
                            bool map_success = true;
                            for (int i = 0; i < array_size; ++i) {
                                cJSON *inner_array = cJSON_GetArrayItem(item, i);
                                if (cJSON_IsArray(inner_array) && cJSON_GetArraySize(inner_array) == 2) {
                                    cJSON *key_json = cJSON_GetArrayItem(inner_array, 0);
                                    cJSON *value_json = cJSON_GetArrayItem(inner_array, 1);
                                    if (cJSON_IsNumber(key_json) && cJSON_IsNumber(value_json)) {
                                        uint64_t key_val = static_cast<uint64_t>(key_json->valuedouble);
                                        uint64_t value_val = static_cast<uint64_t>(value_json->valuedouble);
                                        arg->try_emplace(
                                            static_cast<espConfig::actions_config_t::colorMap>(key_val),
                                            static_cast<uint8_t>(value_val)
                                        );
                                    } else {
                                        map_success = false; break;
                                    }
                                } else {
                                    map_success = false; break;
                                }
                            }
                            if (!map_success) {
                                ESP_LOGW(TAG, "Validation failed for '%s': invalid map format.", key.c_str());
                                success = false;
                            }
                        } else {
                            ESP_LOGW(TAG, "Validation failed for '%s': type mismatch, expected array for map.", key.c_str());
                            success = false;
                        }
                    } else if constexpr (std::is_same_v<PointeeType, std::map<std::string, uint8_t>>) {
                        if (cJSON_IsObject(item)) {
                            arg->clear();
                            cJSON *sub_obj_item = item->child;
                            bool map_success = true;
                            while(sub_obj_item) {
                                if (cJSON_IsNumber(sub_obj_item)) {
                                    arg->operator[](sub_obj_item->string) = static_cast<uint8_t>(sub_obj_item->valuedouble);
                                } else {
                                    map_success = false;
                                    break;
                                }
                                sub_obj_item = sub_obj_item->next;
                            }
                            if (!map_success) {
                                ESP_LOGW(TAG, "Validation failed for '%s': map contains non-numeric values.", key.c_str());
                                success = false;
                            }
                        } else {
                            ESP_LOGW(TAG, "Validation failed for '%s': type mismatch, expected object for map.", key.c_str());
                            success = false;
                        }
                    }
                }
            }, configMap.at(key));
        } else ESP_LOGW(TAG, "Key '%s' could not be found!", key.c_str());
        item = item->next;
    }

    return success;
}
template bool ConfigManager::deserializeFromJson<espConfig::misc_config_t>(const std::string& json_string);
template bool ConfigManager::deserializeFromJson<espConfig::actions_config_t>(const std::string& json_string);
template bool ConfigManager::deserializeFromJson<espConfig::mqttConfig_t>(const std::string& json_string);

// Certificate storage implementation
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

    const char* typeStr = "";
    bool isHttps = false;

    switch(certType) {
        case espConfig::CertType::MQTT_CA:
            m_mqttSslConfig.caCert = certContent;
            typeStr = "CA certificate";
            break;
        case espConfig::CertType::MQTT_CLIENT:
            m_mqttSslConfig.clientCert = certContent;
            typeStr = "Client certificate";
            break;
        case espConfig::CertType::MQTT_PRIVATE_KEY:
            m_mqttSslConfig.clientKey = certContent;
            typeStr = "Private key";
            break;
        case espConfig::CertType::HTTPS_SERVER_CERT:
            m_httpsCertsConfig.serverCert = certContent;
            typeStr = "HTTPS server certificate";
            isHttps = true;
            break;
        case espConfig::CertType::HTTPS_PRIVATE_KEY:
            m_httpsCertsConfig.privateKey = certContent;
            typeStr = "HTTPS private key";
            isHttps = true;
            break;
        case espConfig::CertType::HTTPS_CA_CERT:
            m_httpsCertsConfig.caCert = certContent;
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

std::string ConfigManager::loadCertificate(espConfig::CertType certType) {
    if (!m_isInitialized) {
        ESP_LOGE(TAG, "Cannot load certificate, ConfigManager not initialized.");
        return "";
    }

    std::string content;
    const char* typeStr = "";

    switch(certType) {
        case espConfig::CertType::MQTT_CA:
            content = m_mqttSslConfig.caCert;
            typeStr = "CA";
            break;
        case espConfig::CertType::MQTT_CLIENT:
            content = m_mqttSslConfig.clientCert;
            typeStr = "Client";
            break;
        case espConfig::CertType::MQTT_PRIVATE_KEY:
            content = m_mqttSslConfig.clientKey;
            typeStr = "Private Key";
            break;
        case espConfig::CertType::HTTPS_SERVER_CERT:
            content = m_httpsCertsConfig.serverCert;
            typeStr = "HTTPS Server";
            break;
        case espConfig::CertType::HTTPS_PRIVATE_KEY:
            content = m_httpsCertsConfig.privateKey;
            typeStr = "HTTPS Private Key";
            break;
        case espConfig::CertType::HTTPS_CA_CERT:
            content = m_httpsCertsConfig.caCert;
            typeStr = "HTTPS CA";
            break;
        default:
    		break;
    }

    ESP_LOGD(TAG, "%s loaded successfully (size: %zu bytes)", typeStr, content.length());
    return content;
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
            m_mqttSslConfig.caCert.clear();
            typeStr = "CA";
            break;
        case espConfig::CertType::MQTT_CLIENT:
            m_mqttSslConfig.clientCert.clear();
            typeStr = "Client";
            break;
        case espConfig::CertType::MQTT_PRIVATE_KEY:
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

    int ret = mbedtls_x509_crt_parse(cert.get(), reinterpret_cast<const unsigned char*>(certContent.c_str()), certContent.length() + 1);

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

    int ret = mbedtls_pk_parse_key(pk.get(), reinterpret_cast<const unsigned char*>(keyContent.c_str()), keyContent.length() + 1, nullptr, 0, nullptr, nullptr);

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

    ScopedEntropy entropy;
    ScopedCtrDrbg ctr_drbg;

    int ret = mbedtls_ctr_drbg_seed(ctr_drbg.get(), mbedtls_entropy_func, entropy.get(),
                                  nullptr, 0);
    if (ret != 0) {
        ESP_LOGE("ConfigManager", "mbedtls_ctr_drbg_seed: %d", ret);
        return false;
    }

    ScopedPk pk;
    ret = mbedtls_pk_parse_key(pk.get(), reinterpret_cast<const unsigned char*>(privateKey.c_str()), privateKey.length() + 1, nullptr, 0, mbedtls_ctr_drbg_random, ctr_drbg.get());
    if (ret != 0) {
        char error_buf[100];
        mbedtls_strerror(ret, error_buf, sizeof(error_buf));
        ESP_LOGE("ConfigManager", "Failed to parse %s private key: %s (error code: %d)", context, error_buf, ret);
        return false;
    }

    ScopedX509Crt cert;
    ret = mbedtls_x509_crt_parse(cert.get(), reinterpret_cast<const unsigned char*>(certificate.c_str()), certificate.length() + 1);
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

    ESP_LOGI("ConfigManager", "%s private key and certificate are cryptographically compatible", context);
    return true;
}

std::vector<CertificateStatus> ConfigManager::getCertificatesStatus(){
  std::vector<CertificateStatus> certificates;
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
    certStr = loadCertificate(certType);
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

      int ret = mbedtls_x509_crt_parse(cert.get(), reinterpret_cast<const unsigned char*>(certStr.c_str()), certStr.length() + 1);
      if(ret){
        ESP_LOGE(TAG, "Unable to parse '%s' certificate: %d", typeStr, ret);
        continue;
      }
      char subject[256], issuer[256], serial[256];
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
      unsigned char sha256_hash[32];
      mbedtls_sha256(cert.get()->raw.p, cert.get()->raw.len, sha256_hash, 0);
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
          fmt::format("{:02X}", fmt::join(sha256_hash, ":")),
          {.from = fmt::format("{:04}-{:02}-{:02}T{:02}:{:02}:{:02}Z", valid_from.year, valid_from.mon,
                               valid_from.day, valid_from.hour, valid_from.min, valid_from.sec),
           .to = fmt::format("{:04}-{:02}-{:02}T{:02}:{:02}:{:02}Z", valid_to.year, valid_to.mon,
                             valid_to.day, valid_to.hour, valid_to.min, valid_to.sec)},
          keyMatches});
    } else if(not certStr.empty() && isPrivateKey){
      ScopedPk pk;

      int ret = mbedtls_pk_parse_key(pk.get(), reinterpret_cast<const unsigned char*>(certStr.c_str()), certStr.length() + 1, nullptr, 0, nullptr, nullptr);

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
