#include "ConfigManager.hpp"
#include "config.hpp"
#include "esp_log.h"
#include "nvs.h"
#include <nvs_flash.h>
#include <vector>

const char* ConfigManager::TAG = "ConfigManager";

ConfigManager::ConfigManager() : m_isInitialized(false) {}

ConfigManager::~ConfigManager() {
  if (m_isInitialized) {
    nvs_close(m_nvsHandle);
  }
}

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
  loadConfigFromNvs("MQTTDATA", m_mqttConfig);
  loadConfigFromNvs("MISCDATA", m_miscConfig);

  ESP_LOGI(TAG, "Initialization complete.");
  return true;
}

const espConfig::mqttConfig_t &ConfigManager::getMqttConfig() const {
  return m_mqttConfig;
}

const espConfig::misc_config_t &ConfigManager::getMiscConfig() const {
  return m_miscConfig;
}

bool ConfigManager::deleteMqttConfig() {
  m_mqttConfig = {};
  return !nvs_erase_key(m_nvsHandle, "MQTTDATA");
}

bool ConfigManager::deleteMiscConfig() {
  m_miscConfig = {};
  return !nvs_erase_key(m_nvsHandle, "MISCDATA");
}

bool ConfigManager::saveMqttConfig(const espConfig::mqttConfig_t &newConfig) {
  ESP_LOGI(TAG, "Attempting to save MQTT configuration...");
  if (saveConfigToNvs("MQTTDATA", newConfig)) {
    m_mqttConfig = newConfig;
    ESP_LOGI(TAG, "MQTT configuration successfully saved and updated.");
    return true;
  }
  ESP_LOGE(TAG, "Failed to save MQTT configuration.");
  return false;
}

bool ConfigManager::saveMiscConfig(const espConfig::misc_config_t &newConfig) {
  ESP_LOGI(TAG, "Attempting to save Miscellaneous configuration...");
  if (saveConfigToNvs("MISCDATA", newConfig)) {
    m_miscConfig = newConfig;
    ESP_LOGI(TAG,
             "Miscellaneous configuration successfully saved and updated.");
    return true;
  }
  ESP_LOGE(TAG, "Failed to save Miscellaneous configuration.");
  return false;
}

// --- Private Template Implementations ---

template <typename T>
void ConfigManager::loadConfigFromNvs(const char *key, T &configStruct) {
  if (!m_isInitialized) {
    ESP_LOGE(TAG, "Cannot load, NVS not initialized.");
    return;
  }

  size_t required_size = 0;
  esp_err_t err = nvs_get_blob(m_nvsHandle, key, NULL, &required_size);

  if (err == ESP_ERR_NVS_NOT_FOUND) {
    ESP_LOGI(TAG, "Config key '%s' not found in NVS. Using default values.",
             key);
    // The default values are already set by the struct's constructor
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

  JsonDocument doc;
  deserializeMsgPack(doc, buffer);
  if (!doc.isNull()) {
    configStruct = doc.as<T>();
    ESP_LOGI(TAG, "Successfully loaded configuration for key '%s'.", key);
  } else {
    ESP_LOGE(TAG, "Failed to parse msgpack for key '%s'. Using defaults.", key);
  }
}

template <typename T>
bool ConfigManager::saveConfigToNvs(const char *key, const T &configStruct) {
  if (!m_isInitialized) {
    ESP_LOGE(TAG, "Cannot save, NVS not initialized.");
    return false;
  }

  ByteWriter serialized_data;
  ArduinoJson::JsonDocument doc;
  doc.set(configStruct);
  serializeMsgPack(doc, serialized_data);
  esp_err_t set_err = nvs_set_blob(m_nvsHandle, key, serialized_data,
                                   serialized_data);
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

  ESP_LOGI(TAG, "Successfully saved configuration for key '%s' to NVS.", key);
  return true;
}
