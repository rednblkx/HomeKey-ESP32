# WebServerManager Module

## Description
The `WebServerManager` hosts the local web interface and API. It serves the Svelte-based frontend and provides REST/WebSocket endpoints for configuration and real-time control.

## Key Responsibilities
- **Static File Serving**: Serves the compiled HTML/JS/CSS assets of the frontend from the filesystem.
- **REST API**: Provides endpoints for updating settings (`/api/config`), managing HomeKey data, and performing OTA updates.
- **Real-time Communication**: Uses WebSockets to stream log data and lock state updates to the browser.
- **Authentication**: (If enabled) Enforces basic authentication to protect the interface.

## Dependencies
- **esp_http_server**: The underlying HTTP server library.
- **ConfigManager**: Read/Write configuration via API.
- **ReaderDataManager**: Read/Write HomeKey data via API.
- **MqttManager**: To allow configuration of MQTT settings.

## Key Files
- `main/WebServerManager.cpp`
- `main/include/WebServerManager.hpp`
