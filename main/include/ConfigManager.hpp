#pragma once
#include "config.hpp"
#include <cstddef>
#include <cstdint>
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

    std::vector<CertificateStatus> getCertificatesStatus();
    const espConfig::mqtt_ssl_t& getMqttSslConfig() const {
      return m_mqttSslConfig;
    }
    const espConfig::https_certs_t& getHttpsCertsConfig() const {
      return m_httpsCertsConfig;
    }

    bool setNVSLogLevel(const uint8_t level);
    bool getNVSLogLevel(uint8_t &level);
    bool setBacklogMaxSize(const uint16_t size);
    bool getBacklogMaxSize(uint16_t &size);

  private:
    enum class FieldType : uint8_t {
      Str, Bool, U8, U16, Arr3, Arr4, Arr5, Arr7, ColorMap, StrMap, U8Arr6, U8Arr2
    };
    struct ConfigField {
      const char* section;
      const char* name;
      uint16_t offset;
      FieldType type;
    };
    static constexpr size_t kFieldCount = 97;
    static const ConfigField kConfigFields[kFieldCount];

    // Guards that table section names and sectionBase() agree; aborts at
    // boot otherwise. Called from begin().
    void verifyFieldTable();

    // Field access: resolves a table entry to a typed pointer into the
    // section struct owned by this instance.
    void* sectionBase(const char* section);
    template <typename T>
    T* fieldPtr(const ConfigField& f) {
      return reinterpret_cast<T*>(reinterpret_cast<char*>(sectionBase(f.section)) + f.offset);
    }
    const ConfigField* findField(const char* section, const char* name, size_t nameLen) const;
    // Bounded [begin, end) range of the table entries belonging to one section.
    struct FieldRange { const ConfigField* first; const ConfigField* last; };
    static FieldRange sectionRange(const char* section);

    void deserialize(msgpack_object obj, std::string key);

    template <typename ConfigType>
    std::vector<uint8_t> serialize();

    void loadConfigFromNvs(const char* key);
    bool saveConfigToNvs(const char* key);
    static std::string pemToDer(const std::string& pem);
    static size_t mbedtlsParseLen(const std::string& content);
    void ensureMqttSslLoaded();
    void migratePemToDer();
    void loadCertificateInto(espConfig::CertType certType, std::string& out);
    bool validateCertificateWithMbedTLS(const std::string& certContent, espConfig::CertType certType);
    bool validateCertificateContent(const std::string& certContent, espConfig::CertType certType);
    bool validatePrivateKeyContent(const std::string& keyContent);
    bool validateKeyCertPair(const std::string& privateKey, const std::string& certificate, const char* context);

    espConfig::mqttConfig_t m_mqttConfig;
    espConfig::mqtt_ssl_t m_mqttSslConfig;
    espConfig::https_certs_t m_httpsCertsConfig;
    espConfig::misc_config_t m_miscConfig;
    espConfig::actions_config_t m_actionsConfig;
    nvs_handle m_nvsHandle = 0;
    bool m_isInitialized;
    static const char* TAG;
};

