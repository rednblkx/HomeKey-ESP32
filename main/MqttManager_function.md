# MqttManager Module

## Description
The `MqttManager` provides integration with MQTT brokers, primarily to support Home Assistant. It acts as a bridge, translating internal application events into MQTT messages and vice versa.

## Key Responsibilities
- **Connection Management**: Maintains the connection to the MQTT broker and handles reconnections.
- **Home Assistant Discovery**: Automatically publishes discovery payloads so Home Assistant can detect the lock without manual configuration.
- **State Reporting**: Publishes the current lock state, battery level, and other sensors to MQTT topics.
- **Command Handling**: Subscribes to command topics to allow remote control of the lock via MQTT.

## Dependencies
- **espp::EventManager**: Subscribes to state changes and publishes commands.
- **ConfigManager**: Retrieves broker URL, credentials, and topic prefixes.

## Key Files
- `main/MqttManager.cpp`
- `main/include/MqttManager.hpp`
