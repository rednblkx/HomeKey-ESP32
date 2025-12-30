# ConfigManager Module

## Description
The `ConfigManager` is responsible for persistent storage of general device settings. It uses the ESP32's Non-Volatile Storage (NVS) to save configuration across reboots.

## Key Responsibilities
- **Settings Storage**: Saves and loads WiFi credentials, hostname, MQTT settings, and hardware pin configurations.
- **Serialization**: Converts configuration structures to/from JSON or binary formats for storage and API exchange.
- **Defaults**: Provides default values for all settings to ensure the device can boot in a known state.

## Dependencies
- **NVS Flash**: The underlying storage mechanism.
- **cJSON**: Used for serializing config data for the Web API.

## Key Files
- `main/ConfigManager.cpp`
- `main/include/ConfigManager.hpp`
