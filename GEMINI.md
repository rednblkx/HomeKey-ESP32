# GEMINI.md

## Project Overview

This project, "HomeKey-ESP32," is an embedded C++ application for the ESP32 microcontroller that implements Apple HomeKey functionality. It allows an ESP32 and a PN532 NFC module to act as a HomeKit-compatible lock, enabling users to unlock it with their Apple devices (iPhone, Apple Watch).

The project integrates with HomeAssistant's tag functionality and uses MQTT to publish its state and receive commands. It also features a web-based interface for configuration and management.

**Key Technologies:**

*   **Framework:** ESP-IDF with HomeSpan for HomeKit integration.
*   **NFC:** PN532 module for NFC communication.
*   **Communication:** MQTT for IoT messaging.
*   **Web Interface:** Asynchronous web server (`ESPAsyncWebServer`) serving files from a LittleFS partition.
*   **Build System:** CMake.

**Architecture:**

The application is structured into several manager components, each responsible for a specific aspect of the system:

*   `NfcManager`: Handles NFC communication with the PN532 module.
*   `LockManager`: Manages the lock's state (locked/unlocked).
*   `MqttManager`: Handles MQTT communication.
*   `WebServerManager`: Manages the web interface.
*   `ConfigManager`: Manages the device's configuration.
*   `HardwareManager`: Manages hardware-related tasks.
*   `ReaderDataManager`: Manages data related to the NFC reader.
*   `HomeKitLock`: Implements the HomeKit lock service.

## Building and Running

This project is built using the ESP-IDF build system.

**Prerequisites:**

*   ESP-IDF (Espressif IoT Development Framework) installed and configured.
*   An ESP32 development board.
*   A PN532 NFC module.

**Build Commands:**

1.  **Configure the project:**
    ```bash
    idf.py menuconfig
    ```
    (Configure Wi-Fi credentials, MQTT settings, and other options.)

2.  **Build the project:**
    ```bash
    idf.py build
    ```

3.  **Flash the firmware to the ESP32:**
    ```bash
    idf.py flash
    ```

4.  **Monitor the serial output:**
    ```bash
    idf.py monitor
    ```

## Development Conventions

*   **Language:** C++
*   **Code Style:** The code is organized into classes with clear responsibilities. Header files (`.hpp`, `.h`) are used to define class interfaces, and implementation files (`.cpp`) contain the logic.
*   **Dependencies:** Project dependencies are managed through the `components` directory and specified in the `main/CMakeLists.txt` file.
*   **Configuration:** Configuration is handled through a combination of Kconfig (`menuconfig`) and a JSON-based configuration file.
*   **Web Interface:** The web interface files are located in the `data` directory and are flashed to a LittleFS partition on the ESP32.
