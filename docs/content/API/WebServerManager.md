---
title: "WebServerManager"
---

## Overview

The `WebServerManager` class implements a full-featured web server running on the ESP32. It is designed to provide a comprehensive web-based user interface for device configuration, status monitoring, and management. The server handles RESTful API endpoints for configuration changes, provides a real-time status dashboard via WebSockets, and includes robust implementations for Over-the-Air (OTA) updates and SSL/TLS certificate management.

The class serves all static front-end assets (HTML, CSS, JavaScript) from an onboard LittleFS filesystem and can be protected with Basic HTTP Authentication.

## Key Responsibilities

*   **HTTP Server:** Manages the lifecycle of the ESP32's HTTP server.
*   **Static File Serving:** Serves the web UI assets, with support for gzip-compressed files.
*   **Configuration API:** Provides REST endpoints to get, save, and clear device configurations (e.g., MQTT, miscellaneous settings).
*   **WebSocket Communication:** Manages WebSocket connections for pushing real-time device metrics and status updates to connected clients.
*   **Over-the-Air (OTA) Updates:** Implements a resilient, asynchronous OTA process for both firmware and LittleFS filesystem images.
*   **Certificate Management:** Offers API endpoints for uploading, deleting, and checking the status of SSL/TLS certificates for secure MQTT.
*   **System Actions:** Exposes endpoints to trigger system-level actions like rebooting, resetting HomeKit pairings, or resetting Wi-Fi credentials.
*   **Authentication:** Enforces Basic HTTP Authentication for all endpoints if enabled in the configuration.

## Public API

### WebServerManager()

Constructs a new `WebServerManager` instance. The constructor initializes references to core managers and sets up event publishers.

**Signature:**
```cpp
WebServerManager(ConfigManager& configManager, NvsCredentialStore& readerDataManager);
```

**Parameters:**
*   `configManager`: A reference to the `ConfigManager` for accessing and modifying device configurations.
*   `readerDataManager`: A reference to the `NvsCredentialStore` for retrieving HomeKey-related information.

### ~WebServerManager()

Destructor for the `WebServerManager`. It ensures a clean shutdown by stopping the HTTP server, deleting the status timer, and cleaning up any resources allocated for asynchronous OTA operations.

**Signature:**
```cpp
~WebServerManager();
```

### begin()

Initializes and starts the web server. This method must be called after the constructor. It detects whether the device is in Access Point mode and configures routes accordingly (captive portal routes in AP mode, full web interface routes in normal mode). It performs the following actions:
1.  Mounts the LittleFS filesystem.
2.  Configures and starts the underlying `esp_http_server` (with HTTPS if enabled).
3.  Sets up the WebSocket communication channel and its associated background task.
4.  Registers HTTP and WebSocket routes (captive portal routes in AP mode, full routes otherwise).
5.  Creates a periodic timer to push status updates to WebSocket clients.

**Signature:**
```cpp
void begin();
```

### end()

Stops the HTTP server. This is useful for temporarily disabling the web interface, for instance, before entering a different operational mode like a configuration AP. It stops the SSL HTTP server, deletes the WebSocket task and queue, and stops/deletes the status timer.

**Signature:**
```cpp
void end();
```

## API Endpoints (Routes)

The server exposes the following categories of endpoints. All are subject to Basic HTTP Authentication if enabled — except the static/asset routes and the captive-portal endpoints, which are intentionally unauthenticated (the WebSocket uses session-based authentication instead).

### Static Content

*   `GET /static/*`, `GET /assets/*`, `GET /*`: Serves static files for the web UI from the LittleFS filesystem. It automatically handles content types and serves pre-compressed `.gz` files to capable browsers.

### Configuration Management

*   `GET /config?type=<type>`: Retrieves the current configuration for the specified `type` (`mqtt`, `misc`, `actions`, or `hkinfo`).
*   `POST /config/save?type=<type>`: Saves a new configuration from the JSON request body for the specified `type`. The server validates the request against the existing schema and triggers necessary application events or a reboot.
*   `POST /config/clear?type=<type>`: Clears the configuration for the specified `type` and reboots the device.
*   `GET /eth_get_config`: Retrieves supported Ethernet configurations and presets.
*   `GET /nfc_get_presets`: Retrieves available NFC GPIO pin presets for different board configurations.

### Captive Portal (AP Mode)

These endpoints are available when the device is in Access Point configuration mode:

*   `GET /captive_portal`: Redirects to the captive portal page.
*   `GET /captive_portal_config`: Retrieves initial configuration options for the captive portal (NFC presets, Ethernet config, current settings).
*   `POST /captive_portal_config`: Saves configuration from the captive portal (WiFi credentials, HomeKit setup code, NFC pins, etc.) and reboots the device.
*   `GET /wifi_scan`: Scans for available WiFi networks and returns a list of SSIDs with signal strength.

### System Actions

*   `POST /reboot_device`: Reboots the device.
*   `GET /reset_hk_pair`: Erases all HomeKit pairings and reader data, then reboots.
*   `GET /reset_wifi_cred`: Erases saved Wi-Fi credentials and reboots.
*   `GET /start_config_ap`: Stops the web server and puts the device into Wi-Fi Access Point mode for configuration.

### Over-the-Air (OTA) Updates

Both OTA uploads share a single `POST /ota/*` route; the last URI segment selects the upload type:

*   `POST /ota/upload?skipReboot=<bool>`: Initiates an asynchronous firmware update. The binary firmware file should be the request body. The optional `skipReboot=true` query parameter prevents the automatic reboot after a successful update.
*   `POST /ota/littlefs?skipReboot=<bool>`: Initiates an asynchronous update of the LittleFS filesystem. The filesystem image should be the request body.

### Certificate Management

All certificate operations share a single `/certificates` path; the HTTP method distinguishes the action and the numeric `type` query parameter selects the certificate (matching the `espConfig::CertType` enum: `0` = MQTT CA, `1` = MQTT client, `2` = MQTT private key, `3` = HTTPS server cert, `4` = HTTPS private key, `5` = HTTPS CA):

*   `POST /certificates?type=<type>`: Uploads a new SSL/TLS certificate. The PEM content is the request body (maximum 8 KB).
*   `GET /certificates`: Returns the status of all stored certificates, including issuer, subject, expiration, SHA1 fingerprint, serial number, and key-match status. Includes both MQTT and HTTPS certificates.
*   `DELETE /certificates?type=<type>`: Deletes the specified certificate `type`.

## WebSocket Interface

The server provides a WebSocket endpoint at `/ws` for real-time, bidirectional communication.

### Server-to-Client Messages

The server pushes the following JSON messages to all connected clients:

*   **System Information (`sysinfo`)**: Sent on initial connection.
    ```json
    {"type":"sysinfo","deviceName":"MyLock","version":"1.2.3",...}
    ```
*   **Metrics (`metrics`)**: Pushed periodically.
    ```json
    {"type":"metrics","uptime":123456,"free_heap":85000,"wifi_rssi":-55,"nfc_connected":true,"mqtt_connected":true,"mqtt_error_code":0,"mqtt_error_message":""}
    ```
    *   `uptime`: System uptime in milliseconds
    *   `free_heap`: Available heap memory in bytes
    *   `wifi_rssi`: WiFi signal strength in dBm
    *   `nfc_connected`: NFC module connection status (true if NFC Module is connected and responsive)
    *   `mqtt_connected`: MQTT broker connection status (true if connected to the MQTT broker)
    *   `mqtt_error_code`: MQTT error code when connection fails (0 = no error, 1 = connection refused, 2 = auth failed, 3 = network error, 4 = SSL error, 5 = timeout, 255 = unknown)
    *   `mqtt_error_message`: Human-readable error message when MQTT connection fails
*   **OTA Status (`ota_status`)**: Pushed during an OTA update.
    ```json
    {"type":"ota_status","in_progress":true,"progress_percent":50.5,...}
    ```

### Client-to-Server Messages

Clients can send JSON messages to request information or adjust runtime settings:

*   **Ping**: The server will respond with a `pong` message carrying a `timestamp` (milliseconds).
    ```json
    {"type":"ping"}
    ```
*   **Request Metrics**: The server will respond with the latest `metrics`.
    ```json
    {"type":"metrics"}
    ```
*   **Request System Info**: The server will respond with the `sysinfo` message.
    ```json
    {"type":"sysinfo"}
    ```
*   **Request OTA Info**: The server will respond with the current `ota_status`.
    ```json
    {"type":"ota_info"}
    ```
*   **Set Log Level**: Sets the global log level (0–5; `ERROR`=1, `WARN`=2, `INFO`=3, `DEBUG`=4, `VERBOSE`=5) at runtime and persists it to NVS. The server responds with the updated `sysinfo`.
    ```json
    {"type":"set_log_level","data":3}
    ```
*   **Set Log Backlog Size**: Sets the maximum number of log entries buffered for WebSocket broadcast (0–65535) and persists it to NVS. The server responds with the updated `sysinfo`.
    ```json
    {"type":"set_backlog_max_size","data":100}
    ```

Unknown message types receive an `error` response echoing the received type:
```json
{"type":"error","message":"Unknown message type","received_type":"..."}
```
