#pragma once
#include "config.hpp"
#include <nvs.h>
#include <variant>
#include <vector>
#include <string>
#include <map>
#include <LittleFS.h>
#include "msgpack/object.h"

class ConfigManager {
public:
    ConfigManager();
    ~ConfigManager();

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

    // Certificate storage methods
    bool saveCertificate(const std::string& certType, const std::string& certContent);
    bool saveCertificateBundle(const std::string& bundleContent);
    std::string loadCertificate(const std::string& certType);
    bool deleteCertificate(const std::string& certType);
    
    // Enhanced certificate validation and information
    bool validateCertificateContent(const std::string& certContent, const std::string& certType);
    bool validatePrivateKeyMatchesCertificate(const std::string& privateKey, const std::string& certificate);
    std::string getCertificateIssuer(const std::string& certContent);
    std::string getCertificateSubject(const std::string& certContent);
    std::string getCertificateExpiration(const std::string& certContent);

  private:
    // Helper functions for certificate validation
    bool validateCertificateWithMbedTLS(const std::string& certContent, const std::string& certType);
    bool validatePrivateKeyContent(const std::string& keyContent);

  private:
    using ConfigMapType = std::map<std::string,
                      std::variant<
                          std::string *, uint16_t *, uint8_t *, bool *,
                          std::map<std::string, uint8_t> *,
                          std::map<espConfig::misc_config_t::colorMap, uint8_t> *,
                          std::array<uint8_t, 4> *, std::array<uint8_t, 5> *,
                          std::array<uint8_t, 7> *>>;
    void deserialize(msgpack_object obj, std::string key);

    template <typename ConfigType>
    std::vector<uint8_t> serialize();

    void loadConfigFromNvs(const char* key);
    bool saveConfigToNvs(const char* key);

    // Certificate storage helpers
    bool validateCertificateFormat(const std::string& certContent);

    std::map<std::string, ConfigMapType> m_configMap;
    espConfig::mqttConfig_t m_mqttConfig;
    espConfig::mqtt_ssl_t m_mqttSslConfig;
    espConfig::misc_config_t m_miscConfig;
    nvs_handle m_nvsHandle;
    bool m_isInitialized;
    static const char* TAG;
};

