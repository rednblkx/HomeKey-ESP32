#include "ConfigManager.hpp"
#include "cJSON.h"
#include "config.hpp"
#include <algorithm>
#include <ranges>
#include <string>
#include <vector>
#include "esp_log.h"
#include "msgpack.h"

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
      {"customLockStates", &m_mqttConfig.customLockStates},
      {"customLockActions", &m_mqttConfig.customLockActions}
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
      {"nfcNeopixelPin", &m_miscConfig.nfcNeopixelPin},
      {"neoPixelType", &m_miscConfig.neoPixelType},
      {"neopixelSuccessColor", &m_miscConfig.neopixelSuccessColor},
      {"neopixelFailureColor", &m_miscConfig.neopixelFailureColor},
      {"neopixelSuccessTime", &m_miscConfig.neopixelSuccessTime},
      {"neopixelFailTime", &m_miscConfig.neopixelFailTime},
      {"nfcSuccessPin", &m_miscConfig.nfcSuccessPin},
      {"nfcSuccessTime", &m_miscConfig.nfcSuccessTime},
      {"nfcSuccessHL", &m_miscConfig.nfcSuccessHL},
      {"nfcFailPin", &m_miscConfig.nfcFailPin},
      {"nfcFailTime", &m_miscConfig.nfcFailTime},
      {"nfcFailHL", &m_miscConfig.nfcFailHL},
      {"gpioActionPin", &m_miscConfig.gpioActionPin},
      {"gpioActionLockState", &m_miscConfig.gpioActionLockState},
      {"gpioActionUnlockState", &m_miscConfig.gpioActionUnlockState},
      {"gpioActionMomentaryEnabled", &m_miscConfig.gpioActionMomentaryEnabled},
      {"hkGpioControlledState", &m_miscConfig.hkGpioControlledState},
      {"gpioActionMomentaryTimeout", &m_miscConfig.gpioActionMomentaryTimeout},
      {"webAuthEnabled", &m_miscConfig.webAuthEnabled},
      {"webUsername", &m_miscConfig.webUsername},
      {"webPassword", &m_miscConfig.webPassword},
      {"nfcGpioPins", &m_miscConfig.nfcGpioPins},
      {"btrLowStatusThreshold", &m_miscConfig.btrLowStatusThreshold},
      {"proxBatEnabled", &m_miscConfig.proxBatEnabled},
      {"hkDumbSwitchMode", &m_miscConfig.hkDumbSwitchMode},
      {"hkAltActionInitPin", &m_miscConfig.hkAltActionInitPin},
      {"hkAltActionInitLedPin", &m_miscConfig.hkAltActionInitLedPin},
      {"hkAltActionInitTimeout", &m_miscConfig.hkAltActionInitTimeout},
      {"hkAltActionPin", &m_miscConfig.hkAltActionPin},
      {"hkAltActionTimeout", &m_miscConfig.hkAltActionTimeout},
      {"hkAltActionGpioState", &m_miscConfig.hkAltActionGpioState},
      {"ethernetEnabled", &m_miscConfig.ethernetEnabled},
      {"ethActivePreset", &m_miscConfig.ethActivePreset},
      {"ethPhyType", &m_miscConfig.ethPhyType},
      {"ethRmiiConfig", &m_miscConfig.ethRmiiConfig},
      {"ethSpiConfig", &m_miscConfig.ethSpiConfig}}
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

  m_isInitialized = true;

  ESP_LOGI(TAG, "Loading configurations from NVS...");
  loadConfigFromNvs("MQTTDATA");
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
  } else {
    static_assert(std::is_void_v<ConfigType> && false, "Unsupported ConfigType for getConfig");
  }
}
template const espConfig::mqttConfig_t& ConfigManager::getConfig<espConfig::mqttConfig_t>() const;
template const espConfig::mqttConfig_t& ConfigManager::getConfig<espConfig::mqttConfig_t const>() const;
template const espConfig::misc_config_t& ConfigManager::getConfig<espConfig::misc_config_t>() const;
template const espConfig::misc_config_t& ConfigManager::getConfig<espConfig::misc_config_t const>() const;

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
    return !nvs_erase_key(m_nvsHandle, "MQTTDATA");
  } else if constexpr(std::is_same_v<ConfigType, espConfig::misc_config_t>){
    m_miscConfig = {};
    return !nvs_erase_key(m_nvsHandle, "MISCDATA");
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
  } else if constexpr(std::is_same_v<ConfigType, espConfig::misc_config_t>){
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
    } else if(!strcmp(key, "MISCDATA")){
      deserialize(obj, "misc");
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
              } else if constexpr (std::is_same_v<PointeeType, std::map<espConfig::misc_config_t::colorMap, uint8_t>>) {
                std::ranges::for_each(msgpack_elements, [&](const msgpack_object& o) {
                    if (o.type == MSGPACK_OBJECT_ARRAY && o.via.array.size >= 2) {
                        const msgpack_object* inner_array_ptr = o.via.array.ptr;
                        uint64_t key_val = inner_array_ptr[0].via.u64;
                        uint64_t value_val = inner_array_ptr[1].via.u64;
                        arg->try_emplace(
                            static_cast<espConfig::misc_config_t::colorMap>(key_val),
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
        } else if constexpr (std::is_same_v<PointeeType, std::map<espConfig::misc_config_t::colorMap, uint8_t>>) {
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
                } else if constexpr (std::is_same_v<PointeeType, std::map<espConfig::misc_config_t::colorMap, uint8_t>>) {
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
                    } else if constexpr (std::is_same_v<PointeeType, std::map<espConfig::misc_config_t::colorMap, uint8_t>>) {
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
                                            static_cast<espConfig::misc_config_t::colorMap>(key_val),
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
template bool ConfigManager::deserializeFromJson<espConfig::mqttConfig_t>(const std::string& json_string);
