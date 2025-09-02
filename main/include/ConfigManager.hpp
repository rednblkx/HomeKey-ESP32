#pragma once
#include <nvs.h>
#include <variant>
#include <vector>
#include "msgpack/object.h"
#include "config.hpp"

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

    std::map<std::string, ConfigMapType> m_configMap;
    espConfig::mqttConfig_t m_mqttConfig;
    espConfig::misc_config_t m_miscConfig;
    nvs_handle m_nvsHandle;
    bool m_isInitialized;
    static const char* TAG;
};

