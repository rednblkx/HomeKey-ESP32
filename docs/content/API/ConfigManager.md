---
title: "ConfigManager"
---

**Description:** Manages the device's configuration, providing templated methods for loading, saving, and serializing different configuration types.

### Public Methods:

*   **`ConfigManager()`**
    *   **Description:** Constructor for ConfigManager.
*   **`~ConfigManager()`**
    *   **Description:** Destructor for ConfigManager.
*   **`bool begin()`**
    *   **Description:** Initializes the configuration manager.
    *   **Returns:** `bool` - `true` on success, `false` otherwise.
*   **`template <typename ConfigType> const ConfigType& getConfig() const`**
    *   **Description:** Retrieves a reference to the specified configuration type.
    *   **Returns:** `const ConfigType&` - A constant reference to the configuration object.
*   **`template <typename ConfigType> bool deleteConfig()`**
    *   **Description:** Deletes the specified configuration type.
    *   **Returns:** `bool` - `true` on success, `false` otherwise.
*   **`template <typename ConfigType> bool saveConfig()`**
    *   **Description:** Saves the specified configuration type.
    *   **Returns:** `bool` - `true` on success, `false` otherwise.
*   **`template <typename ConfigType> bool deserializeFromJson(const std::string& json_string)`**
    *   **Description:** Deserializes a JSON string into the specified configuration type.
    *   **Parameters:**
        *   `json_string` (`const std::string&`): The JSON string to deserialize.
    *   **Returns:** `bool` - `true` on success, `false` otherwise.
*   **`template <typename ConfigType> std::string serializeToJson()`**
    *   **Description:** Serializes the specified configuration type to a JSON string.
    *   **Returns:** `std::string` - The JSON string representation of the configuration.
