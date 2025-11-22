---
title: "MqttManager"
---

## Overview

The `MqttManager` class provides a robust bridge between the device's internal event-driven system and an external MQTT broker. It handles the complexities of establishing and maintaining an MQTT connection (including SSL/TLS), subscribing to command topics, and publishing state and event data from the device.

This class acts as a translator, converting internal application events (like lock state changes or NFC taps) into MQTT messages, and converting incoming MQTT messages into internal events that other managers can act upon. It also supports Home Assistant MQTT Discovery, allowing the lock to be automatically recognized and configured in Home Assistant.

## Key Responsibilities

*   **MQTT Client Management:** Initializes, starts, and manages the lifecycle of the ESP-MQTT client.
*   **Connection Handling:** Manages connection and disconnection events, including publishing a Last Will and Testament (LWT) for presence detection.
*   **SSL/TLS Configuration:** Configures secure MQTT connections using certificates managed by the `ConfigManager`.
*   **Event Bridging:** Subscribes to internal events (`lock/stateChanged`, `nfc/event`, etc.) and publishes corresponding data to MQTT topics.
*   **Command Handling:** Subscribes to MQTT command topics and publishes internal events (`lock/targetStateChanged`, `lock/overrideState`, etc.) to control the device remotely.
*   **Home Assistant Discovery:** Publishes configuration payloads to Home Assistant's discovery topics, enabling seamless integration.
*   **Data Formatting:** Formats event data (e.g., NFC tap details) into structured JSON payloads for easy consumption by external services.

## Public API

### MqttManager()

Constructs a new `MqttManager` instance. The constructor retrieves the necessary MQTT and SSL configurations from the provided `ConfigManager` and sets up subscribers and publishers with the `espp::EventManager`. This wiring connects the manager to the application's event bus.

**Signature:**
```cpp
MqttManager(const ConfigManager& configManager);
```

**Parameters:**
*   `configManager`: A reference to the `ConfigManager`, which provides all necessary MQTT broker details, topics, and SSL certificates.

### ~MqttManager()

Destructor for the `MqttManager`. It ensures that the MQTT client is properly stopped and its resources are released to prevent memory leaks.

**Signature:**
```cpp
~MqttManager();
```

### begin()

Initializes and starts the MQTT client. This method configures the client with the broker address, credentials, and SSL settings, then attempts to establish a connection. If the MQTT broker is not configured in the settings, this method will do nothing and return `false`.

**Signature:**
```cpp
bool begin(std::string deviceID);
```

**Parameters:**
*   `deviceID`: A unique string identifier for this device, used in Home Assistant discovery payloads.

**Returns:**
*   `bool`: `true` if the MQTT client was started successfully, or `false` if the broker was not configured or if initialization failed.

### publish()

Publishes a message to a specified MQTT topic. This is a general-purpose method for sending data to the MQTT broker.

**Signature:**
```cpp
void publish(const std::string& topic, const std::string& payload, int qos = 0, bool retain = false);
```

**Parameters:**
*   `topic`: The MQTT topic to publish the message to.
*   `payload`: The message content to send.
*   `qos`: The Quality of Service level for the message (0, 1, or 2).
*   `retain`: A boolean flag indicating if the message should be retained by the broker.

### isConnected()

Returns the current connection status of the MQTT client.

**Signature:**
```cpp
bool isConnected() const;
```

**Returns:**
*   `bool`: `true` if the client is currently connected to the broker, `false` otherwise.

## Internal Workings

### Event Handling

The `MqttManager` is primarily event-driven. The `mqttEventHandler` is a static callback registered with the ESP-MQTT library, which forwards all events to the instance's `onMqttEvent` method.

*   **`onMqttEvent`**: This method acts as a dispatcher. It handles connection/disconnection logic, logs errors, and passes incoming message data to the `onData` method.
*   **`onConnected`**: Called upon a successful connection, this method subscribes to all necessary command topics and triggers the Home Assistant discovery process.
*   **`onData`**: This is the core of the command handling logic. It parses the topic and payload of an incoming message and translates it into an appropriate internal event. For example, a message on the `lockTStateCmd` topic will be converted into a `lock/targetStateChanged` event.

### Data Publishing

The manager also subscribes to internal events to publish data *out* to the MQTT broker.

*   **`publishLockState`**: Listens for `lock/stateChanged` events and publishes the lock's status to the configured state topic. It correctly represents transitional states like "locking" or "unlocking."
*   **`publishHomeKeyTap` / `publishUidTap`**: Listen for `nfc/event` notifications and publish detailed, JSON-formatted information about the NFC tap to the `hkTopic`.
*   **Home Assistant Discovery**: The `publishHassDiscovery` method constructs detailed JSON configuration payloads that describe the lock and NFC tag sensor entities to Home Assistant, allowing for zero-config integration.

### SSL/TLS Configuration

*   **`configureSSL`**: This method populates the `esp_mqtt_client_config_t` struct with pointers to the certificate strings (CA, client cert, and private key) obtained from the `ConfigManager`. It also handles the `allowInsecure` flag.
*   **`logSSLError`**: A utility function that provides detailed, human-readable logging for SSL/TLS errors to aid in troubleshooting connection issues.
