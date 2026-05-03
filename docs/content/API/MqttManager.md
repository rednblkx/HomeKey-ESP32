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
*   **JsonGuard Integration:** The manager now uses `JsonBuilder` (part of the `JsonGuard` utility) for constructing JSON payloads. This provides RAII-based memory safety for cJSON objects and a cleaner, more readable fluent API for building JSON strings.
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

### end()

Stops the MQTT client and unsubscribes from all event listeners. Performs a clean shutdown of the MQTT client by stopping and destroying it, then removes all event subscriptions registered by this instance.

**Signature:**
```cpp
void end();
```

### getLastErrorCode()

Gets the last MQTT error code that occurred during connection or operation.

**Signature:**
```cpp
MqttErrorCode getLastErrorCode() const;
```

**Returns:**
*   `MqttErrorCode`: The last error code. Possible values:
    *   `NONE` (0): No error
    *   `CONNECTION_REFUSED` (1): Connection refused by broker
    *   `AUTH_FAILED` (2): Authentication failed
    *   `NETWORK_ERROR` (3): Network connectivity issue
    *   `SSL_ERROR` (4): TLS/SSL handshake or certificate error
    *   `TIMEOUT` (5): Connection or operation timed out
    *   `UNKNOWN` (255): Unknown error

### getLastErrorMessage()

Gets the last MQTT error message as a human-readable string.

**Signature:**
```cpp
const std::string& getLastErrorMessage() const;
```

**Returns:**
*   `const std::string&`: Reference to the last error message string.

## Internal Workings

### Event Handling

The `MqttManager` is primarily event-driven. The `mqttEventHandler` is a static callback registered with the ESP-MQTT library, which forwards all events to the instance's `onMqttEvent` method.

*   **`onMqttEvent`**: This method acts as a dispatcher. It handles connection/disconnection logic, logs errors, updates the internal status (error code and message), and passes incoming message data to the `onData` method.
*   **`onConnected`**: Called upon a successful connection, this method subscribes to all necessary command topics, publishes an `MQTT_STATUS_CHANGED` event to the `AppEventLoop`, and triggers the Home Assistant discovery process.
*   **`onData`**: This is the core of the command handling logic. It parses the topic and payload of an incoming message and translates it into an appropriate internal event using the `AppEventLoop` system. For example, a message on the `lockTStateCmd` topic will be converted into a `LOCK_TARGET_STATE_CHANGED` event.

### Event System Integration

The `MqttManager` uses the `AppEventLoop` system (ESP-IDF's native event loop) for internal communication:

*   **Subscribes to:**
    *   `LOCK_EVENT` (`LOCK_STATE_CHANGED`): Publishes lock state changes to MQTT
    *   `HW_EVENT` (`HW_ALT_ACTION`): Publishes alternate action events
    *   `NFC_EVENT` (`NFC_TAP_EVENT`): Publishes NFC/HomeKey tap data
*   **Publishes:**
    *   `MQTT_EVENT` (`MQTT_STATUS_CHANGED`): Notifies other components of MQTT connection status changes

### Data Publishing

The manager subscribes to internal events via the `AppEventLoop` to publish data *out* to the MQTT broker.

*   **`publishLockState`**: Listens for `LOCK_STATE_CHANGED` events and publishes the lock's status to the configured state topic. It correctly represents transitional states like "locking" or "unlocking."
*   **`publishHomeKeyTap` / `publishUidTap`**: Listen for `NFC_TAP_EVENT` notifications and publish detailed, JSON-formatted information about the NFC tap to the `hkTopic`.
*   **`publishMqttStatus`**: Updates internal MQTT connection status (error code and message) and publishes an `MQTT_STATUS_CHANGED` event to the `AppEventLoop` for internal components (like the WebUI) to consume. Does **not** publish to an MQTT topic.
*   **Home Assistant Discovery**: The `publishHassDiscovery` method constructs detailed JSON configuration payloads that describe the lock and NFC tag sensor entities to Home Assistant, allowing for zero-config integration.

### SSL/TLS Configuration

*   **`configureSSL`**: This method populates the `esp_mqtt_client_config_t` struct with pointers to the certificate strings (CA, client cert, and private key) obtained from the `ConfigManager`. It also handles the `allowInsecure` flag.
*   **`logSSLError`**: A utility function that provides detailed, human-readable logging for SSL/TLS errors to aid in troubleshooting connection issues.
