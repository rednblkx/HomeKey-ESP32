#ifndef CONFIG_MANAGER_H
#define CONFIG_MANAGER_H

#include <string>
#include <vector>
#include <nvs.h>
#include "config.hpp"

/**
 * @class ConfigManager
 * @brief Manages loading, saving, and accessing application configuration from NVS.
 *
 * This class is responsible for all interactions with Non-Volatile Storage (NVS)
 * for configuration data. It loads settings on startup, provides read-only access
 * to the current configuration, and handles saving updated settings back to flash.
 */
class ConfigManager {
public:
    /**
     * @brief Constructor for the ConfigManager.
     */
    ConfigManager();

    /**
     * @brief Destructor. Ensures NVS handle is closed properly.
     */
    ~ConfigManager();

    /**
     * @brief Initializes the manager by opening NVS and loading all configurations.
     * This must be called before any other methods.
     * @return True if initialization was successful, false otherwise.
     */
    bool begin();

    /**
     * @brief Provides read-only access to the MQTT configuration.
     * @return A constant reference to the current MQTT configuration.
     */
    const espConfig::mqttConfig_t& getMqttConfig() const;

    void deleteMqttConfig();

    /**
     * @brief Provides read-only access to the miscellaneous configuration.
     * @return A constant reference to the current miscellaneous configuration.
     */
    const espConfig::misc_config_t& getMiscConfig() const;

    void deleteMiscConfig();

    /**
     * @brief Saves a new MQTT configuration to NVS and updates the internal state.
     * @param newConfig The new MQTT configuration to save.
     * @return True on success, false on failure.
     */
    bool saveMqttConfig(const espConfig::mqttConfig_t& newConfig);

    /**
     * @brief Saves a new miscellaneous configuration to NVS and updates the internal state.
     * @param newConfig The new miscellaneous configuration to save.
     * @return True on success, false on failure.
     */
    bool saveMiscConfig(const espConfig::misc_config_t& newConfig);

private:
    /**
     * @brief Generic templated function to load a configuration structure from NVS.
     * @tparam T The type of the configuration structure (e.g., mqttConfig_t).
     * @param key The NVS key where the data is stored.
     * @param configStruct The configuration structure to populate.
     */
    template <typename T>
    void loadConfigFromNvs(const char* key, T& configStruct);

    /**
     * @brief Generic templated function to save a configuration structure to NVS.
     * @tparam T The type of the configuration structure (e.g., mqttConfig_t).
     * @param key The NVS key to save the data under.
     * @param configStruct The configuration structure to save.
     * @return True on success, false on failure.
     */
    template <typename T>
    bool saveConfigToNvs(const char* key, const T& configStruct);

    // Member variables
    espConfig::mqttConfig_t m_mqttConfig;
    espConfig::misc_config_t m_miscConfig;
    nvs_handle m_nvsHandle;
    bool m_isInitialized;
    static const char* TAG;
};

#endif // CONFIG_MANAGER_H
