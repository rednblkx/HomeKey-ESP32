#include "ConfigManager.hpp"
#include "cJSON.h"
#include "config.hpp"
#include <iterator>
#include <ranges>
#include <string>
#include <vector>
#include "esp_log.h"
#include "esp_log_buffer.h"
#include "esp_log_level.h"
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
      {"hassMqttDiscoveryEnabled",
        &m_mqttConfig.hassMqttDiscoveryEnabled},
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
    {"misc",{
      // Miscellaneous Config
      {"deviceName", &m_miscConfig.deviceName},
      {"otaPasswd", &m_miscConfig.otaPasswd},
      {"hk_key_color", &m_miscConfig.hk_key_color},
      {"setupCode", &m_miscConfig.setupCode},
      {"lockAlwaysUnlock", &m_miscConfig.lockAlwaysUnlock},
      {"lockAlwaysLock", &m_miscConfig.lockAlwaysLock},
      {"controlPin", &m_miscConfig.controlPin},
      {"hsStatusPin", &m_miscConfig.hsStatusPin},
      {"webAuthEnabled", &m_miscConfig.webAuthEnabled},
      {"webUsername", &m_miscConfig.webUsername},
      {"webPassword", &m_miscConfig.webPassword},
      {"nfcGpioPins", &m_miscConfig.nfcGpioPins},
      {"btrLowStatusThreshold", &m_miscConfig.btrLowStatusThreshold},
      {"proxBatEnabled", &m_miscConfig.proxBatEnabled},
      {"ethernetEnabled", &m_miscConfig.ethernetEnabled},
      {"ethActivePreset", &m_miscConfig.ethActivePreset},
      {"ethPhyType", &m_miscConfig.ethPhyType},
      {"ethRmiiConfig", &m_miscConfig.ethRmiiConfig},
      {"ethSpiConfig", &m_miscConfig.ethSpiConfig}}
    },
    {
      "actions", {
        {"nfcNeopixelPin", &m_actionsConfig.nfcNeopixelPin},
        {"neoPixelType", &m_actionsConfig.neoPixelType},
        {"neopixelSuccessColor", &m_actionsConfig.neopixelSuccessColor},
        {"neopixelFailureColor", &m_actionsConfig.neopixelFailureColor},
        {"neopixelSuccessTime", &m_actionsConfig.neopixelSuccessTime},
        {"neopixelFailTime", &m_actionsConfig.neopixelFailTime},
        {"nfcSuccessPin", &m_actionsConfig.nfcSuccessPin},
        {"nfcSuccessTime", &m_actionsConfig.nfcSuccessTime},
        {"nfcSuccessHL", &m_actionsConfig.nfcSuccessHL},
        {"nfcFailPin", &m_actionsConfig.nfcFailPin},
        {"nfcFailTime", &m_actionsConfig.nfcFailTime},
        {"nfcFailHL", &m_actionsConfig.nfcFailHL},
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
  if constexpr (std::is_same_v<ConfigType, espConfig::mqttConfig_t>){
    m_mqttConfig = {}; 
    return !nvs_erase_key(m_nvsHandle, "MQTTDATA") && !nvs_erase_key(m_nvsHandle, "MQTTSSLDATA");
  } else if constexpr(std::is_same_v<ConfigType, espConfig::misc_config_t>){
    m_miscConfig = {};
    return saveConfig<espConfig::misc_config_t>();
  } else if constexpr(std::is_same_v<ConfigType, espConfig::actions_config_t>){
    m_actionsConfig = {};
    return saveConfig<espConfig::actions_config_t>();
  }else {
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

  std::vector<uint8_t> buffer(required_size);
  err = nvs_get_blob(m_nvsHandle, key, buffer.data(), &required_size);

  if (err != ESP_OK) {
    ESP_LOGE(TAG, "Error (%s) reading blob for key '%s'", esp_err_to_name(err),
             key);
    return;
  }

  msgpack_unpacked unpacked;
  msgpack_unpacked_init(&unpacked);
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
    } else {ESP_LOGE(TAG, "Key '%s' not valid", key);return;}
  } else {
    ESP_LOGE(TAG, "Failed to parse msgpack for key '%s'. Using defaults.", key);
  }
  msgpack_unpacked_destroy(&unpacked);
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
                *arg = v.val.via.u64;
              }
              break;
            }
            case MSGPACK_OBJECT_ARRAY: {
              auto msgpack_elements = std::ranges::subrange(v.val.via.array.ptr, v.val.via.array.ptr + v.val.via.array.size);
              auto integer_view = msgpack_elements | std::ranges::views::transform([](const msgpack_object& o){return o.via.u64;});

              if constexpr (std::is_same_v<PointeeType, std::array<uint8_t, 4>>) {
                std::ranges::copy(integer_view, arg->begin());
              } else if constexpr (std::is_same_v<PointeeType, std::array<uint8_t, 5>>) {
                std::ranges::copy(integer_view, arg->begin());
              } else if constexpr (std::is_same_v<PointeeType, std::array<uint8_t, 7>>) {
                std::ranges::copy(integer_view, arg->begin());
              } else if constexpr (std::is_same_v<PointeeType, std::map<espConfig::actions_config_t::colorMap, uint8_t>>) {
                std::ranges::for_each(msgpack_elements, [&](const msgpack_object& o) {
                    if (o.type == MSGPACK_OBJECT_ARRAY && o.via.array.size >= 2) {
                        const msgpack_object* inner_array_ptr = o.via.array.ptr;
                        uint64_t key_val = inner_array_ptr[0].via.u64;
                        uint64_t value_val = inner_array_ptr[1].via.u64;
                        arg->try_emplace(
                            static_cast<espConfig::actions_config_t::colorMap>(key_val),
                            static_cast<uint8_t>(value_val)
                        );
                    }
                });
              } else if constexpr (std::is_same_v<PointeeType, std::map<std::string, uint8_t>>) { // For mqttConfigMap's customLockStates/Actions
                std::ranges::for_each(msgpack_elements, [&](const msgpack_object& o) {
                    if (o.type == MSGPACK_OBJECT_ARRAY && o.via.array.size >= 2) {
                        const msgpack_object* inner_array_ptr = o.via.array.ptr;
                        std::string key_str(inner_array_ptr[0].via.str.ptr, inner_array_ptr[0].via.str.size);
                        uint64_t value_val = inner_array_ptr[1].via.u64;
                        arg->try_emplace(key_str, static_cast<uint8_t>(value_val));
                    }
                });
              }
              break;
            }
            default:
              ESP_LOGW(TAG, "DON'T KNOW THIS ONE!");
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
          msgpack_pack_array(&pk, arg->size()); // Pack as array of arrays for map
          for (const auto& map_pair : *arg) {
            msgpack_pack_array(&pk, 2); // Key-value pair
            msgpack_pack_unsigned_char(&pk, static_cast<uint8_t>(map_pair.first)); // Enum key as integer
            msgpack_pack_unsigned_char(&pk, map_pair.second);
          }
        } else if constexpr (std::is_same_v<PointeeType, std::map<std::string, uint8_t>>) {
          msgpack_pack_array(&pk, arg->size()); // Pack as array of arrays for map
          for (const auto& map_pair : *arg) {
            msgpack_pack_array(&pk, 2); // Key-value pair
            msgpack_pack_str(&pk, map_pair.first.size()); // String key size
            msgpack_pack_str_body(&pk, map_pair.first.data(), map_pair.first.size()); // String key body
            msgpack_pack_unsigned_char(&pk, map_pair.second);
          }
        }
      }
    }, pair.second);
  }

  std::vector<uint8_t> serialized_data(reinterpret_cast<uint8_t*>(sbuf.data), reinterpret_cast<uint8_t*>(sbuf.data) + sbuf.size);
  msgpack_sbuffer_destroy(&sbuf);
  return serialized_data;
}

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
  cJSON *root = cJSON_CreateObject();
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
                    cJSON_AddStringToObject(root, key.c_str(), arg->c_str());
                } else if constexpr (std::is_same_v<PointeeType, bool>) {
                    cJSON_AddBoolToObject(root, key.c_str(), *arg);
                } else if constexpr (std::is_same_v<PointeeType, uint8_t> || std::is_same_v<PointeeType, uint16_t>) {
                    cJSON_AddNumberToObject(root, key.c_str(), static_cast<double>(*arg));
                } else if constexpr (std::is_same_v<PointeeType, std::array<uint8_t, 4>> ||
                                     std::is_same_v<PointeeType, std::array<uint8_t, 5>> ||
                                     std::is_same_v<PointeeType, std::array<uint8_t, 7>>) {
                    cJSON *array = cJSON_CreateArray();
                    if (array) {
                        for (const auto& val : *arg) {
                            cJSON_AddItemToArray(array, cJSON_CreateNumber(static_cast<double>(val)));
                        }
                        cJSON_AddItemToObject(root, key.c_str(), array);
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
                        cJSON_AddItemToObject(root, key.c_str(), array_of_arrays);
                    }
                } else if constexpr (std::is_same_v<PointeeType, std::map<std::string, uint8_t>>) {
                    cJSON *map_obj = cJSON_CreateObject();
                    if (map_obj) {
                        for (const auto& map_pair : *arg) {
                            cJSON_AddNumberToObject(map_obj, map_pair.first.c_str(), static_cast<double>(map_pair.second));
                        }
                        cJSON_AddItemToObject(root, key.c_str(), map_obj);
                    }
                }
            }
        }, pair.second);
    }

    char *json_string = cJSON_Print(root);
    std::string result(json_string ? json_string : "");
    cJSON_Delete(root);
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
    cJSON *root = cJSON_Parse(json_string.c_str());
    if (!root) {
        ESP_LOGE(TAG, "Failed to parse JSON: invalid format.");
        return false; // Error parsing JSON
    }

    if (!cJSON_IsObject(root)) {
        ESP_LOGE(TAG, "JSON root is not an object.");
        cJSON_Delete(root);
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
    cJSON *item = root->child;
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
                            *arg = static_cast<PointeeType>(item->valuedouble);
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
        } else ESP_LOGW(TAG, "Key '%s' could not be found!");
        item = item->next;
    }

    cJSON_Delete(root);
    return success;
}
template bool ConfigManager::deserializeFromJson<espConfig::misc_config_t>(const std::string& json_string);
template bool ConfigManager::deserializeFromJson<espConfig::actions_config_t>(const std::string& json_string);
template bool ConfigManager::deserializeFromJson<espConfig::mqttConfig_t>(const std::string& json_string);

// Certificate storage implementation
bool ConfigManager::saveCertificate(const std::string& certType, const std::string& certContent) {
    if (!m_isInitialized) {
        ESP_LOGE(TAG, "Cannot save certificate, ConfigManager not initialized.");
        return false;
    }

    // Validate certificate type
    if (certType.compare("ca") && certType.compare("client") && certType.compare("privateKey")) {
        ESP_LOGE(TAG, "Invalid certificate type: '%s' - Must be 'ca', 'client', or 'privateKey'", certType.c_str());
        return false;
    }
 
    // Validate the actual content
    if(!validateCertificateContent(certContent, certType)) { 
        ESP_LOGE(TAG, "Unable to validate certificate type: %s", certType.c_str());
        return false;
    }

    if(certType == "ca"){
      m_mqttSslConfig.caCert = certContent;
    } else if(certType == "client"){
      m_mqttSslConfig.clientCert = certContent;
    } else if(certType == "privateKey"){
      m_mqttSslConfig.clientKey = certContent;
    }

    saveConfig<espConfig::mqtt_ssl_t>();

    ESP_LOGI(TAG, "Certificate saved successfully: %s", certType.c_str());
    return true;
}

std::string ConfigManager::loadCertificate(const std::string& certType) {
    if (!m_isInitialized) {
        ESP_LOGE(TAG, "Cannot load certificate, ConfigManager not initialized.");
        return "";
    }

    // Validate certificate type
    if (certType != "ca" && certType != "client" && certType != "privateKey") {
        ESP_LOGE(TAG, "Invalid certificate type: %s. Must be 'ca', 'client', or 'key'.", certType.c_str());
        return "";
    }

    std::string content;

    if(certType == "ca"){
      content = m_mqttSslConfig.caCert;
    } else if(certType == "client"){
      content = m_mqttSslConfig.clientCert;
    } else if(certType == "privateKey"){
      content = m_mqttSslConfig.clientKey;
    }

    ESP_LOGI(TAG, "Certificate loaded successfully: %s (size: %zu bytes)", certType.c_str(), content.length());
    return content;
}

bool ConfigManager::deleteCertificate(const std::string& certType) {
    if (!m_isInitialized) {
        ESP_LOGE(TAG, "Cannot delete certificate, ConfigManager not initialized.");
        return false;
    }

    // Validate certificate type
    if (certType != "ca" && certType != "client" && certType != "privateKey") {
        ESP_LOGE(TAG, "Invalid certificate type: %s. Must be 'ca', 'client', or 'key'.", certType.c_str());
        return false;
    }
  
    if(certType == "ca"){
      m_mqttSslConfig.caCert.clear();
    } else if(certType == "client"){
      m_mqttSslConfig.clientCert.clear();
    } else if(certType == "privateKey"){
      m_mqttSslConfig.clientKey.clear();
    }
 
    saveConfig<espConfig::mqtt_ssl_t>();

    ESP_LOGI(TAG, "Certificate deleted successfully: %s", certType.c_str());
    return true;
}

bool ConfigManager::validateCertificateFormat(const std::string& certContent) {
    // Enhanced PEM format validation
    if (certContent.empty()) {
        ESP_LOGE(TAG, "Certificate content is empty");
        return false;
    }

    // Check for BEGIN and END markers
    size_t beginPos = certContent.find("-----BEGIN");
    size_t endPos = certContent.find("-----END");

    if (beginPos == std::string::npos || endPos == std::string::npos) {
        ESP_LOGE(TAG, "Certificate missing required PEM BEGIN/END markers");
        return false;
    }

    if (beginPos >= endPos) {
        ESP_LOGE(TAG, "Invalid PEM format: END marker appears before BEGIN marker");
        return false;
    }

    // Extract the type from BEGIN marker (e.g., "CERTIFICATE", "PRIVATE KEY", etc.)
    size_t beginEnd = certContent.find("-----", beginPos + 11);
    if (beginEnd == std::string::npos) {
        ESP_LOGE(TAG, "Invalid PEM BEGIN marker format");
        return false;
    }
    std::string beginType = certContent.substr(beginPos + 11, beginEnd - (beginPos + 11));

    // Extract the type from END marker
    size_t endEnd = certContent.find("-----", endPos + 9);
    if (endEnd == std::string::npos) {
        ESP_LOGE(TAG, "Invalid PEM END marker format");
        return false;
    }
    std::string endType = certContent.substr(endPos + 9, endEnd - (endPos + 9));

    // Ensure BEGIN and END types match
    if (beginType != endType) {
        ESP_LOGE(TAG, "PEM BEGIN and END marker types don't match: '%s' vs '%s'", beginType.c_str(), endType.c_str());
        return false;
    }

    // Find the actual content start (after BEGIN marker line)
    size_t contentStart = certContent.find('\n', beginEnd + 5);
    if (contentStart == std::string::npos) {
        ESP_LOGE(TAG, "Invalid PEM format: no newline after BEGIN marker");
        return false;
    }
    contentStart++; // Move past the newline

    // Find content end (before END marker line)
    size_t contentEnd = certContent.rfind('\n', endPos);
    if (contentEnd == std::string::npos || contentEnd <= contentStart) {
        ESP_LOGE(TAG, "Invalid PEM format: no content between markers");
        return false;
    }

    std::string base64Content = certContent.substr(contentStart, contentEnd - contentStart);

    // Remove all whitespace
    base64Content.erase(std::remove_if(base64Content.begin(), base64Content.end(), ::isspace), base64Content.end());

    if (base64Content.empty()) {
        ESP_LOGE(TAG, "Certificate base64 content is empty");
        return false;
    }

    // Validate base64 characters and padding
    bool validBase64 = true;
    int paddingCount = 0;
    for (size_t i = 0; i < base64Content.length(); ++i) {
        char c = base64Content[i];
        if (c == '=') {
            paddingCount++;
            // Padding can only appear at the end
            if (i < base64Content.length() - 1 && base64Content[i + 1] != '=') {
                validBase64 = false;
                break;
            }
        } else if (!isalnum(c) && c != '+' && c != '/') {
            validBase64 = false;
            break;
        }
    }

    if (!validBase64) {
        ESP_LOGE(TAG, "Invalid base64 characters in certificate content");
        return false;
    }

    // Validate padding (must be 0, 1, or 2 padding characters)
    if (paddingCount > 2) {
        ESP_LOGE(TAG, "Invalid base64 padding: too many '=' characters");
        return false;
    }

    // Note: Removed strict base64 length validation as mbedTLS will validate the actual decoding

    ESP_LOGD(TAG, "PEM format validation passed for type: %s", beginType.c_str());
    return true;
}

bool ConfigManager::validateCertificateContent(const std::string& certContent, const std::string& certType) {
    // First, validate basic PEM format
    if (!validateCertificateFormat(certContent)) {
        return false;
    }

    if (certType == "privateKey") {
        // Validate private key using mbedTLS
        return validatePrivateKeyContent(certContent);
    } else if (certType == "ca" || certType == "client") {
        // Validate certificate using mbedTLS
        return validateCertificateWithMbedTLS(certContent, certType);
    }

    ESP_LOGE(TAG, "Unknown certificate type: %s", certType.c_str());
    return false;
}

bool ConfigManager::validateCertificateWithMbedTLS(const std::string& certContent, const std::string& certType) {
    mbedtls_x509_crt cert;
    mbedtls_x509_crt_init(&cert);

    int ret = mbedtls_x509_crt_parse(&cert, reinterpret_cast<const unsigned char*>(certContent.c_str()), certContent.length() + 1);

    if (ret != 0) {
        char error_buf[100];
        mbedtls_strerror(ret, error_buf, sizeof(error_buf));
        ESP_LOGE(TAG, "Certificate parsing failed: %s (error code: %d)", error_buf, ret);
        mbedtls_x509_crt_free(&cert);
        return false;
    }

    // Validate certificate structure and basic fields
    bool isValid = true;

    // Check if certificate has valid version
    if (cert.version == 0) {
        ESP_LOGE(TAG, "Certificate has invalid version");
        isValid = false;
    }

    // Check if certificate has subject
    if (cert.subject_raw.len == 0) {
        ESP_LOGE(TAG, "Certificate missing subject");
        isValid = false;
    }

    // Check if certificate has issuer
    if (cert.issuer_raw.len == 0) {
        ESP_LOGE(TAG, "Certificate missing issuer");
        isValid = false;
    }

    // Check if certificate has valid dates
    if (cert.valid_from.year == 0 || cert.valid_to.year == 0) {
        ESP_LOGE(TAG, "Certificate has invalid validity dates");
        isValid = false;
    }

    // Check expiration
    time_t now = time(nullptr);
    struct tm* tm_now = gmtime(&now);

    if (tm_now) {
        struct tm cert_not_after = {
            .tm_sec = cert.valid_to.sec,
            .tm_min = cert.valid_to.min,
            .tm_hour = cert.valid_to.hour,
            .tm_mday = cert.valid_to.day,
            .tm_mon = cert.valid_to.mon - 1,  // mbedTLS months are 1-based
            .tm_year = cert.valid_to.year - 1900,
            .tm_isdst = -1
        };

        time_t cert_expiry = mktime(&cert_not_after);
        if (cert_expiry == -1) {
            ESP_LOGE(TAG, "Failed to parse certificate expiration date");
            isValid = false;
        } else if (now > cert_expiry) {
            ESP_LOGE(TAG, "Certificate has expired");
            isValid = false;
        } else {
            // Check if certificate expires soon (within 30 days)
            time_t thirty_days = 30 * 24 * 60 * 60;
            if (cert_expiry - now < thirty_days) {
                ESP_LOGW(TAG, "Certificate expires soon (within 30 days)");
            }
        }
    } else {
        ESP_LOGE(TAG, "Failed to get current time for expiration check");
        isValid = false;
    }

    // Check if certificate has public key
    if (mbedtls_pk_get_type(&cert.pk) == MBEDTLS_PK_NONE) {
        ESP_LOGE(TAG, "Certificate missing public key");
        isValid = false;
    }

    // Additional checks for CA certificates
    if (certType == "ca") {
        // CA certificates should have basic constraints extension
        if (!(cert.MBEDTLS_PRIVATE(ca_istrue))) {
            ESP_LOGW(TAG, "CA certificate does not have CA:true in basic constraints");
        }
    }

    // Log certificate information
    if (isValid) {
        char subject[256];
        char issuer[256];
        mbedtls_x509_dn_gets(subject, sizeof(subject), &cert.subject);
        mbedtls_x509_dn_gets(issuer, sizeof(issuer), &cert.issuer);

        ESP_LOGI(TAG, "Certificate validated successfully - Subject: %s, Issuer: %s", subject, issuer);
    }

    mbedtls_x509_crt_free(&cert);
    return isValid;
}

bool ConfigManager::validatePrivateKeyContent(const std::string& keyContent) {
    mbedtls_pk_context pk;
    mbedtls_pk_init(&pk);

    int ret = mbedtls_pk_parse_key(&pk, reinterpret_cast<const unsigned char*>(keyContent.c_str()), keyContent.length() + 1, nullptr, 0, nullptr, nullptr);

    if (ret != 0) {
        char error_buf[100];
        mbedtls_strerror(ret, error_buf, sizeof(error_buf));
        ESP_LOGE(TAG, "Private key parsing failed: %s (error code: %d)", error_buf, ret);
        mbedtls_pk_free(&pk);
        return false;
    }

    // Check key type
    mbedtls_pk_type_t key_type = mbedtls_pk_get_type(&pk);
    if (key_type != MBEDTLS_PK_RSA && key_type != MBEDTLS_PK_ECKEY && key_type != MBEDTLS_PK_ECKEY_DH && key_type != MBEDTLS_PK_ECDSA) {
        ESP_LOGE(TAG, "Unsupported private key type: %d", key_type);
        mbedtls_pk_free(&pk);
        return false;
    }

    // For RSA keys, check minimum key size
    if (key_type == MBEDTLS_PK_RSA) {
        size_t key_bits = mbedtls_pk_get_bitlen(&pk);
        if (key_bits < 2048) {
            ESP_LOGW(TAG, "RSA key size is only %zu bits, recommended minimum is 2048 bits", key_bits);
        }
    }

    // For EC keys, check curve
    if (key_type == MBEDTLS_PK_ECKEY || key_type == MBEDTLS_PK_ECKEY_DH || key_type == MBEDTLS_PK_ECDSA) {
        const mbedtls_ecp_keypair* ec_key = mbedtls_pk_ec(pk);
        if (ec_key) {
            mbedtls_ecp_group_id curve_id = ec_key->MBEDTLS_PRIVATE(grp).id;
            if (curve_id != MBEDTLS_ECP_DP_SECP256R1 && curve_id != MBEDTLS_ECP_DP_SECP384R1 && curve_id != MBEDTLS_ECP_DP_SECP521R1) {
                ESP_LOGW(TAG, "EC key uses non-standard curve: %d", curve_id);
            }
        } else {
            ESP_LOGE(TAG, "Failed to access EC key context");
            mbedtls_pk_free(&pk);
            return false;
        }
    }

    ESP_LOGI(TAG, "Private key validated successfully - Type: %s, Size: %zu bits",
             key_type == MBEDTLS_PK_RSA ? "RSA" : "EC", mbedtls_pk_get_bitlen(&pk));

    mbedtls_pk_free(&pk);
    return true;
}

bool ConfigManager::validatePrivateKeyMatchesCertificate() {
    if (m_mqttSslConfig.clientKey.empty() || m_mqttSslConfig.clientCert.empty()) {
        ESP_LOGE(TAG, "Private key or certificate is empty");
        return false;
    }

    mbedtls_entropy_context entropy;
    mbedtls_ctr_drbg_context ctr_drbg;
  
    mbedtls_entropy_init(&entropy);
    mbedtls_ctr_drbg_init(&ctr_drbg);

    int ret = mbedtls_ctr_drbg_seed(&ctr_drbg, mbedtls_entropy_func, &entropy,
                                  (const unsigned char *)"polaris1", strlen("polaris1"));
    if (ret != 0) {
        ESP_LOGE(TAG, "mbedtls_ctr_drbg_seed: %d", ret);
        mbedtls_ctr_drbg_free(&ctr_drbg);
        mbedtls_entropy_free(&entropy);
        return false;
    }

    // Parse the private key
    mbedtls_pk_context pk;
    mbedtls_pk_init(&pk);
    ret = mbedtls_pk_parse_key(&pk, reinterpret_cast<const unsigned char*>(m_mqttSslConfig.clientKey.c_str()), m_mqttSslConfig.clientKey.length() + 1, nullptr, 0, mbedtls_ctr_drbg_random, &ctr_drbg);
    if (ret != 0) {
        char error_buf[100];
        mbedtls_strerror(ret, error_buf, sizeof(error_buf));
        ESP_LOGE(TAG, "Failed to parse private key: %s (error code: %d)", error_buf, ret);
        mbedtls_pk_free(&pk);
        return false;
    }

    // Parse the certificate
    mbedtls_x509_crt cert;
    mbedtls_x509_crt_init(&cert);
    ret = mbedtls_x509_crt_parse(&cert, reinterpret_cast<const unsigned char*>(m_mqttSslConfig.clientCert.c_str()), m_mqttSslConfig.clientCert.length() + 1);
    if (ret != 0) {
        char error_buf[100];
        mbedtls_strerror(ret, error_buf, sizeof(error_buf));
        ESP_LOGE(TAG, "Failed to parse certificate: %s (error code: %d)", error_buf, ret);
        mbedtls_pk_free(&pk);
        mbedtls_x509_crt_free(&cert);
        return false;
    }

    // Check if the private key and certificate public key match
    ret = mbedtls_pk_check_pair(&cert.pk, &pk, mbedtls_ctr_drbg_random, &ctr_drbg);
    if (ret != 0) {
        char error_buf[100];
        mbedtls_strerror(ret, error_buf, sizeof(error_buf));
        ESP_LOGE(TAG, "Private key does not match certificate public key: %s (error code: %d)", error_buf, ret);
        mbedtls_pk_free(&pk);
        mbedtls_x509_crt_free(&cert);
        return false;
    }

    mbedtls_ctr_drbg_free(&ctr_drbg);
    mbedtls_entropy_free(&entropy);

    // Additional checks for key type compatibility
    mbedtls_pk_type_t cert_key_type = mbedtls_pk_get_type(&cert.pk);
    mbedtls_pk_type_t key_type = mbedtls_pk_get_type(&pk);

    if (cert_key_type != key_type) {
        ESP_LOGE(TAG, "Certificate and private key have different types (cert: %d, key: %d)", cert_key_type, key_type);
        mbedtls_pk_free(&pk);
        mbedtls_x509_crt_free(&cert);
        return false;
    }

    // For RSA keys, check key sizes match
    if (key_type == MBEDTLS_PK_RSA) {
        size_t cert_key_bits = mbedtls_pk_get_bitlen(&cert.pk);
        size_t key_bits = mbedtls_pk_get_bitlen(&pk);
        if (cert_key_bits != key_bits) {
            ESP_LOGE(TAG, "RSA key sizes don't match (cert: %zu bits, key: %zu bits)", cert_key_bits, key_bits);
            mbedtls_pk_free(&pk);
            mbedtls_x509_crt_free(&cert);
            return false;
        }
    }

    // For EC keys, check curve matches
    if (key_type == MBEDTLS_PK_ECKEY || key_type == MBEDTLS_PK_ECKEY_DH || key_type == MBEDTLS_PK_ECDSA) {
        const mbedtls_ecp_keypair* cert_ec = mbedtls_pk_ec(cert.pk);
        const mbedtls_ecp_keypair* key_ec = mbedtls_pk_ec(pk);
        if (cert_ec && key_ec) {
            mbedtls_ecp_group_id cert_curve = cert_ec->MBEDTLS_PRIVATE(grp).id;
            mbedtls_ecp_group_id key_curve = key_ec->MBEDTLS_PRIVATE(grp).id;
            if (cert_curve != key_curve) {
                ESP_LOGE(TAG, "EC curves don't match (cert: %d, key: %d)", cert_curve, key_curve);
                mbedtls_pk_free(&pk);
                mbedtls_x509_crt_free(&cert);
                return false;
            }
        }
    }

    ESP_LOGI(TAG, "Private key and certificate are cryptographically compatible");
    mbedtls_pk_free(&pk);
    mbedtls_x509_crt_free(&cert);
    return true;
}

std::vector<CertificateStatus> ConfigManager::getCertificatesStatus(){
  std::vector<CertificateStatus> certificates;
  std::array<std::string, 3> types{"ca", "client", "privateKey"};
  std::string certStr;
  for (auto c : types) {
    mbedtls_x509_crt cert;
    mbedtls_x509_crt_init(&cert);
    certStr = loadCertificate(c);
    if(!certStr.empty() && c != "privateKey"){

      int ret = mbedtls_x509_crt_parse(&cert, reinterpret_cast<const unsigned char*>(certStr.c_str()), certStr.length() + 1);
      if(ret){
        ESP_LOGE(TAG, "Unable to parse '%s' certificate: %d", c.c_str(), ret);
        continue;
      }
      char subject[256], issuer[256];
      ret = mbedtls_x509_dn_gets(subject, sizeof(subject), &cert.subject);
      if(!ret){
        ESP_LOGE(TAG, "Unable to retrieve DN for '%s' certificate: %d", c.c_str(), ret);
        continue;
      }
      ret = mbedtls_x509_dn_gets(issuer, sizeof(issuer), &cert.issuer);
      if(!ret){
        ESP_LOGE(TAG, "Unable to retrieve DN for '%s' certificate: %d", c.c_str(), ret);
        continue;
      }
      mbedtls_x509_time valid_from = cert.valid_from;
      mbedtls_x509_time valid_to = cert.valid_to;
      certificates.emplace_back(CertificateStatus{
          c,
          issuer,
          subject,
          {.from = fmt::format("{}/{}/{}", valid_from.day, valid_from.mon,
                               valid_from.year),
           .to = fmt::format("{}/{}/{}", valid_to.day, valid_to.mon,
                             valid_to.year)},
          c == "client" ? validatePrivateKeyMatchesCertificate() : false});
    } else if(not certStr.empty() && c == "privateKey"){ 
      certificates.emplace_back(CertificateStatus{c});
    }
    mbedtls_x509_crt_free(&cert);
  }
  return certificates;
}
