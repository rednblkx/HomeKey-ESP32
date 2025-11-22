#pragma once
#include "config.hpp"
#include <nvs.h>
#include <variant>
#include <vector>
#include <string>
#include <map>
#include <LittleFS.h>
#include "msgpack/object.h"

struct CertificateStatus {
  std::string type;
  std::string issuer = "";
  std::string subject = "";
  struct {
    std::string from = "";
    std::string to = "";
  } expiration = {};
  bool keyMatchesCert = false;
};

class ConfigManager {
public:
    ConfigManager();
    ~ConfigManager();
    
    // Prevent copying (NVS handle cannot be safely copied)
    ConfigManager(const ConfigManager&) = delete;
    ConfigManager& operator=(const ConfigManager&) = delete;
    
    // Prevent moving (mutex cannot be moved)
    ConfigManager(ConfigManager&&) = delete;
    ConfigManager& operator=(ConfigManager&&) = delete;

    bool begin();

    template <typename ConfigType>
    const ConfigType& getConfig() const;

    template <typename ConfigType>
    bool deleteConfig();

    template <typename ConfigType>
    bool saveConfig();

    template <typename ConfigType>
    bool deserializeFromJson(const std::string& json_string);

    template <typename ConfigType>
    std::string serializeToJson();

    template <typename ConfigType>
    std::string updateFromJson(const std::string& json_string);

    bool saveCertificate(const std::string& certType, const std::string& certContent);
    bool deleteCertificate(const std::string& certType);
    
    bool validateCertificateContent(const std::string& certContent, const std::string& certType);
    bool validatePrivateKeyMatchesCertificate();
    std::vector<CertificateStatus> getCertificatesStatus();
    const espConfig::mqtt_ssl_t* getMqttSslConfig() const {static bool configured = false; if(not configured) {return &m_mqttSslConfig;configured=true;} else ESP_LOGE(TAG, "Ptr already assigned");return nullptr;};

  private:
    using ConfigMapType = std::map<std::string,
                      std::variant<
                          std::string *, uint16_t *, uint8_t *, bool *,
                          std::map<std::string, uint8_t> *,
                          std::map<espConfig::actions_config_t::colorMap, uint8_t> *,
                          std::array<uint8_t, 4> *, std::array<uint8_t, 5> *,
                          std::array<uint8_t, 7> *>>;
    void deserialize(msgpack_object obj, std::string key);

    template <typename ConfigType>
    std::vector<uint8_t> serialize();

    void loadConfigFromNvs(const char* key);
    bool saveConfigToNvs(const char* key);

    std::string loadCertificate(const std::string& certType);
    bool validateCertificateFormat(const std::string& certContent);
    bool validateCertificateWithMbedTLS(const std::string& certContent, const std::string& certType);
    bool validatePrivateKeyContent(const std::string& keyContent);

    std::map<std::string, ConfigMapType> m_configMap;
    espConfig::mqttConfig_t m_mqttConfig;
    espConfig::mqtt_ssl_t m_mqttSslConfig;
    espConfig::misc_config_t m_miscConfig;
    espConfig::actions_config_t m_actionsConfig;
    nvs_handle m_nvsHandle;
    bool m_isInitialized;
    static const char* TAG;
};

