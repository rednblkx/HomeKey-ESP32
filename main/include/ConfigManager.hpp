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
  espConfig::CertType type;
  std::string issuer = "";
  std::string subject = "";
  std::string serial = "";
  std::string fingerprint = "";
  struct {
    std::string from = "";
    std::string to = "";
  } expiration = {};
  bool keyMatchesCert = false;
  std::string keyType = "";
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

    bool saveCertificate(espConfig::CertType certType, const std::string& certContent);
    bool deleteCertificate(espConfig::CertType certType);
    std::string loadCertificate(espConfig::CertType certType);
    
    std::vector<CertificateStatus> getCertificatesStatus();
    const espConfig::mqtt_ssl_t& getMqttSslConfig() const {
      return m_mqttSslConfig;
    }
    const espConfig::https_certs_t& getHttpsCertsConfig() const {
      return m_httpsCertsConfig;
    }

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
    bool validateCertificateWithMbedTLS(const std::string& certContent, espConfig::CertType certType);
    bool validateCertificateContent(const std::string& certContent, espConfig::CertType certType);
    bool validatePrivateKeyContent(const std::string& keyContent);
    bool validateKeyCertPair(const std::string& privateKey, const std::string& certificate, const char* context);

    std::map<std::string, ConfigMapType> m_configMap;
    espConfig::mqttConfig_t m_mqttConfig;
    espConfig::mqtt_ssl_t m_mqttSslConfig;
    espConfig::https_certs_t m_httpsCertsConfig;
    espConfig::misc_config_t m_miscConfig;
    espConfig::actions_config_t m_actionsConfig;
    nvs_handle m_nvsHandle;
    bool m_isInitialized;
    static const char* TAG;
};

