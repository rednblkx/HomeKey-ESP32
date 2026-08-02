---
title: "API Documentation"
weight: 100
---

This section provides an overview of the public APIs implemented in the HomeKey-ESP32 project. It details the classes responsible for managing various aspects of the system, their functionalities, and how to interact with them.

## Key Subsystems

*   **[AppEventLoop](AppEventLoop):** Decoupled event bus wrapper around ESP-IDF native `esp_event`.
*   **[ConfigManager](ConfigManager):** JSON-based NVS/SPIFFS configuration persistence and schema validation.
*   **[HardwareManager](HardwareManager):** Hardware abstraction layer with `GpioAllocator` thread-safe pin leasing and strapping pin protection.
*   **[HomeKitLock](HomeKitLock):** HomeSpan HomeKit accessory implementation.
*   **[LockManager](LockManager):** Lock state machine managing target vs current states.
*   **[MqttManager](MqttManager):** Async MQTT client, TLS management, and HASS Auto-Discovery.
*   **[NfcManager](NfcManager):** Multi-reader NFC driver (PN532 SPI & NXP PN7160/PN7161 SPI), ECP frame broadcasting, and DigitalDoorKey integration.
*   **[ReaderDataManager](ReaderDataManager):** Storage for Apple HomeKey reader keys and issuer endpoint data.
*   **[WebServerManager](WebServerManager):** Async HTTP/HTTPS web server, Svelte 5 WebUI with `sv-router`, WebSockets, and certificate management.

## Event System (AppEventLoop)
The project uses the `AppEventLoop` system for internal communication between components. This is a modern C++ wrapper around ESP-IDF's native event loop. See [AppEventLoop](AppEventLoop) for details.

---
