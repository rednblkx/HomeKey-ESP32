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
WebServerManager(ConfigManager& configManager, ReaderDataManager& readerDataManager);
```

**Parameters:**
*   `configManager`: A reference to the `ConfigManager` for accessing and modifying device configurations.
*   `readerDataManager`: A reference to the `ReaderDataManager` for retrieving HomeKey-related information.

### ~WebServerManager()

Destructor for the `WebServerManager`. It ensures a clean shutdown by stopping the HTTP server, deleting the status timer, and cleaning up any resources allocated for asynchronous OTA operations.

**Signature:**
```cpp
~WebServerManager();
```

### begin()

Initializes and starts the web server. This method must be called after the constructor. It performs the following actions:
1.  Mounts the LittleFS filesystem.
2.  Configures and starts the underlying `esp_http_server`.
3.  Sets up the WebSocket communication channel and its associated background task.
4.  Registers all HTTP and WebSocket routes.
5.  Creates a periodic timer to push status updates to WebSocket clients.

**Signature:**
```cpp
void begin();
```

### stop()

Stops the HTTP server. This is useful for temporarily disabling the web interface, for instance, before entering a different operational mode like a configuration AP.

**Signature:**
```cpp
void stop();
```

## API Endpoints (Routes)

The server exposes the following categories of endpoints. All are subject to Basic HTTP Authentication if enabled.

### Static Content

*   `GET /static/*`, `GET /_app/*`, `GET /*`: Serves static files for the web UI from the LittleFS filesystem. It automatically handles content types and serves pre-compressed `.gz` files to capable browsers.

### Configuration Management

*   `GET /config?type=<type>`: Retrieves the current configuration for the specified `type` (`mqtt`, `misc`, `actions`, or `hkinfo`).
*   `POST /config/save?type=<type>`: Saves a new configuration from the JSON request body for the specified `type`. The server validates the request against the existing schema and triggers necessary application events or a reboot.
*   `POST /config/clear?type=<type>`: Clears the configuration for the specified `type` and reboots the device.
*   `GET /eth_get_config`: Retrieves supported Ethernet configurations and presets.

### System Actions

*   `POST /reboot_device`: Reboots the device.
*   `GET /reset_hk_pair`: Erases all HomeKit pairings and reader data, then reboots.
*   `GET /reset_wifi_cred`: Erases saved Wi-Fi credentials and reboots.
*   `GET /start_config_ap`: Stops the web server and puts the device into Wi-Fi Access Point mode for configuration.

### Over-the-Air (OTA) Updates

*   `POST /ota/firmware`: Initiates an asynchronous firmware update. The binary firmware file should be the request body. An optional `?skipReboot=true` query parameter can be used to prevent an automatic reboot after a successful update.
*   `POST /ota/littlefs`: Initiates an asynchronous update of the LittleFS filesystem. The filesystem image should be the request body.

### Certificate Management

*   `POST /certificates/upload?type=<type>`: Uploads a new SSL/TLS certificate. The certificate content is the request body, and the `type` must be `ca`, `client`, or `privateKey`.
*   `GET /certificates/status`: Returns the status of all stored certificates, including issuer, subject, expiration, and whether the client certificate and private key match.
*   `DELETE /certificates/<type>`: Deletes the specified certificate `type`.

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
    {"type":"metrics","uptime":123456,"free_heap":85000,"wifi_rssi":-55}
    ```
*   **OTA Status (`ota_status`)**: Pushed during an OTA update.
    ```json
    {"type":"ota_status","in_progress":true,"progress_percent":50.5,...}
    ```

### Client-to-Server Messages

Clients can send JSON messages to request information:

*   **Ping**: The server will respond with a `pong` message.
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
