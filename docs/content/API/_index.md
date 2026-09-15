---
title: "API Documentation"
weight: 100
---

This section provides an overview of the public APIs implemented in the HomeKey-ESP32 project. It details the classes responsible for managing various aspects of the system, their functionalities, and how to interact with them.

## Key Subsystems

*   **[AppEventLoop](../api/appeventloop/):** Decoupled event bus wrapper around ESP-IDF native `esp_event`.
*   **[ConfigManager](../api/configmanager/):** JSON-based NVS/SPIFFS configuration persistence and schema validation.
*   **[HardwareManager](../api/hardwaremanager/):** Hardware abstraction layer with `GPIOAllocator` thread-safe pin leasing and strapping pin protection.
*   **[HomeKitLock](../api/homekitlock/):** HomeSpan HomeKit accessory implementation.
*   **[LockManager](../api/lockmanager/):** Lock state machine managing target vs current states.
*   **[MqttManager](../api/mqttmanager/):** Async MQTT client, TLS management, and HASS Auto-Discovery.
*   **[NfcManager](../api/nfcmanager/):** Multi-reader NFC driver (PN532 SPI, PN7161 SPI and ST25R3916 I2C), ECP frame broadcasting, and DigitalDoorKey integration.
*   **[ReaderDataManager](../api/readerdatamanager/):** Storage for Apple HomeKey reader keys and issuer endpoint data.
*   **[WebServerManager](../api/webservermanager/):** Async HTTP/HTTPS web server, Svelte 5 WebUI with `sv-router`, WebSockets, and certificate management.
*   **[EthernetDriver](../api/ethernetdriver/):** Dedicated module for Ethernet initialization, GPIO pin leasing, and lifecycle event handling (start, link, IP assignment) over the `AppEventLoop`.

## Event System (AppEventLoop)
The project uses the `AppEventLoop` system for internal communication between components. This is a modern C++ wrapper around ESP-IDF's native event loop. See [AppEventLoop](../api/appeventloop/) for details.

---
