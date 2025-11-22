---
title: "ConfigManager"
---

## 1. Overview

The `ConfigManager` class provides a comprehensive solution for managing application configurations on ESP32 devices. It is designed to handle the loading, saving, and modification of various configuration settings, persisting them in Non-Volatile Storage (NVS).

The manager organizes configuration into logical groups (e.g., `mqtt`, `misc`, `actions`, `ssl`) and supports serialization and deserialization to both MessagePack (for storage) and JSON (for external communication, like a web UI). It also includes a robust subsystem for managing, storing, and validating SSL/TLS certificates for secure communication.

### Key Features:

*   **Persistent Storage:** Uses ESP-IDF's Non-Volatile Storage (NVS) to save configurations across device reboots.
*   **Structured Configuration:** Manages multiple configuration structs (`mqttConfig_t`, `misc_config_t`, etc.) in a unified way.
*   **Serialization:** Supports binary serialization to MessagePack for efficient storage and text-based serialization to JSON for APIs.
*   **Certificate Management:** Provides functions to save, load, delete, and validate PEM-formatted SSL/TLS certificates and private keys.
*   **Type-Safe Access:** Uses C++ templates to provide type-safe access and manipulation of different configuration sections.

### Dependencies:

*   **ESP-IDF:** Core framework, specifically `esp_log` and `nvs`.
*   **cJSON:** For parsing and generating JSON strings.
*   **msgpack-c:** For binary serialization and deserialization.
*   **mbedTLS:** For validating certificates and private keys.
*   **fmtlib:** For string formatting (used in `getCertificatesStatus`).

## 2. Public Methods

### Constructor & Destructor

#### `ConfigManager::ConfigManager()`

Initializes a new instance of the `ConfigManager`. It sets up an internal map that links configuration key strings (e.g., "mqttBroker") to their corresponding in-memory struct members. This map is crucial for a unified approach to serialization and deserialization.

**Signature:**
```cpp
ConfigManager();
```

#### `ConfigManager::~ConfigManager()`

Destructor for the `ConfigManager`. It ensures that the Non-Volatile Storage (NVS) handle is closed, releasing any acquired resources.

**Signature:**
```cpp
~ConfigManager();
```

### Initialization

#### `begin()`

Initializes the `ConfigManager` subsystem. This method must be called before any other operations. It opens the NVS namespace "SAVED\_DATA" and loads all previously saved configurations (`MQTTDATA`, `MQTTSSLDATA`, `MISCDATA`) from NVS into memory.

**Signature:**
```cpp
bool begin();
```

**Returns:**
*   `bool`: `true` if initialization was successful or if it was already initialized. `false` if opening the NVS handle failed.

### Configuration Access

#### `getConfig<ConfigType>()`

Provides read-only access to one of the internal configuration data structures. This is the primary method for retrieving configuration values throughout the application.

**Signature:**
```cpp
template <typename ConfigType>
const ConfigType& getConfig() const;
```

**Template Parameters:**
*   `ConfigType`: The type of the configuration struct to retrieve. Supported types are:
    *   `espConfig::mqttConfig_t`
    *   `espConfig::misc_config_t`
    *   `espConfig::actions_config_t`

**Returns:**
*   `const ConfigType&`: A constant reference to the requested in-memory configuration struct.

**Example:**
```cpp
const auto& mqtt_config = configManager.getConfig<espConfig::mqttConfig_t>();
std::string broker = mqtt_config.mqttBroker;
```

### Configuration Management

#### `saveConfig<ConfigType>()`

Serializes the specified configuration struct to MessagePack and persists it to Non-Volatile Storage (NVS).

**Signature:**
```cpp
template <typename ConfigType>
bool saveConfig();
```

**Template Parameters:**
*   `ConfigType`: The configuration to save. Supported types are:
    *   `espConfig::mqttConfig_t`: Saves to the "MQTTDATA" key.
    *   `espConfig::mqtt_ssl_t`: Saves to the "MQTTSSLDATA" key.
    *   `espConfig::misc_config_t`: Saves both "misc" and "actions" sections to the "MISCDATA" key.
    *   `espConfig::actions_config_t`: Saves both "actions" and "misc" sections to the "MISCDATA" key.

**Returns:**
*   `bool`: `true` if the configuration was successfully written and committed to NVS, `false` otherwise.

#### `deleteConfig<ConfigType>()`

Resets the specified in-memory configuration struct to its default state and erases its corresponding entry from NVS.

**Signature:**
```cpp
template <typename ConfigType>
bool deleteConfig();
```

**Template Parameters:**
*   `ConfigType`: The configuration to delete. Supported types are:
    *   `espConfig::mqttConfig_t`: Clears MQTT and SSL data.
    *   `espConfig::misc_config_t`: Clears miscellaneous data.
    *   `espConfig::actions_config_t`: Clears actions data.


**Returns:**
*   `bool`: `true` if the NVS entry was erased successfully, `false` otherwise.

### JSON Serialization & Deserialization

#### `serializeToJson<ConfigType>()`

Serializes the specified in-memory configuration struct into a JSON string. Any fields containing passwords will be masked.

**Signature:**
```cpp
template <typename ConfigType>
std::string serializeToJson();
```

**Template Parameters:**
*   `ConfigType`: The configuration to serialize. Supported types are:
    *   `espConfig::mqttConfig_t`
    *   `espConfig::misc_config_t`
    *   `espConfig::actions_config_t`

**Returns:**
*   `std::string`: A JSON representation of the configuration. Returns an empty string on failure.

#### `updateFromJson<ConfigType>()`

Parses a JSON string and updates the corresponding in-memory configuration struct with the values provided. Only keys present in the JSON and valid for the specified `ConfigType` will be updated. After updating, it returns the complete, serialized JSON of the updated configuration.

**Signature:**
```cpp
template <typename ConfigType>
std::string updateFromJson(const std::string& json_string);
```

**Template Parameters:**
*   `ConfigType`: The configuration to update. Supported types are:
    *   `espConfig::mqttConfig_t`
    *   `espConfig::misc_config_t`
    *   `espConfig::actions_config_t`

**Parameters:**
*   `json_string`: A `std::string` containing the JSON object with the fields to update.

**Returns:**
*   `std::string`: A full JSON representation of the configuration after the update. Returns an empty string if JSON parsing fails.

#### `deserializeFromJson<ConfigType>()`

Parses a JSON string and applies its fields to the selected in-memory configuration. This method performs strict validation of field types and shapes.

**Signature:**
```cpp
template <typename ConfigType>
bool deserializeFromJson(const std::string& json_string);
```

**Template Parameters:**
*   `ConfigType`: The configuration to populate. Supported types are:
    *   `espConfig::mqttConfig_t`
    *   `espConfig::misc_config_t`
    *   `espConfig::actions_config_t`

**Parameters:**
*   `json_string`: A `std::string` containing the full JSON object for the configuration.

**Returns:**
*   `bool`: `true` if the JSON was parsed and all keys were successfully applied, `false` if parsing failed or any key failed validation.

### Certificate Management

#### `saveCertificate()`

Validates and saves a PEM-formatted certificate or private key to NVS.

**Signature:**
```cpp
bool saveCertificate(const std::string& certType, const std::string& certContent);
```

**Parameters:**
*   `certType`: The type of certificate to save. Must be one of:
    *   `"ca"`: CA Certificate
    *   `"client"`: Client Certificate
    *   `"privateKey"`: Client Private Key
*   `certContent`: A `std::string` containing the full PEM-formatted content.

**Returns:**
*   `bool`: `true` on successful validation and storage, `false` otherwise.

#### `loadCertificate()`

Loads a previously saved certificate or private key from NVS.

**Signature:**
```cpp
std::string loadCertificate(const std::string& certType);
```

**Parameters:**
*   `certType`: The type of certificate to load (`"ca"`, `"client"`, or `"privateKey"`).

**Returns:**
*   `std::string`: The PEM-formatted certificate content, or an empty string if not found or if the manager is uninitialized.

#### `deleteCertificate()`

Deletes a certificate or private key from NVS.

**Signature:**
```cpp
bool deleteCertificate(const std::string& certType);
```

**Parameters:**
*   `certType`: The type of certificate to delete (`"ca"`, `"client"`, or `"privateKey"`).

**Returns:**
*   `bool`: `true` on successful deletion, `false` otherwise.

### Certificate Validation & Status

#### `validatePrivateKeyMatchesCertificate()`

Performs a cryptographic check to ensure that the currently stored client certificate and private key are a valid pair.

**Signature:**
```cpp
bool validatePrivateKeyMatchesCertificate();
```

**Returns:**
*   `bool`: `true` if the private key and certificate match, `false` if they do not, if one is missing, or if parsing fails.

#### `getCertificatesStatus()`

Retrieves the status of all stored certificates (CA and client) and the private key. For certificates, it provides details like issuer, subject, and validity period.

**Signature:**
```cpp
std::vector<CertificateStatus> getCertificatesStatus();
```

**Returns:**
*   `std::vector<CertificateStatus>`: A vector of `CertificateStatus` structs. Each struct contains:
    *   `type` (string): "ca", "client", or "privateKey".
    *   `issuer` (string): The certificate issuer's distinguished name (DN).
    *   `subject` (string): The certificate subject's DN.
    *   `validity` (struct): Contains `from` and `to` date strings.
    *   `keyMatch` (bool): For the client certificate, indicates if it matches the private key.
