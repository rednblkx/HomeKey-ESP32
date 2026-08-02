---
title: "Configuration"
weight: 3
---

# Making HomeKey-ESP32 Yours (via the Web Interface)

Welcome to the control center of your HomeKey-ESP32! This guide will walk you through all the settings you can tweak to make your device work exactly how you want it to. All these configurations are easily managed through the device's intuitive web interface.

> [!WARNING]
> Flash memory on the device is currently **not encrypted**. This project started as a personal pet project and has grown significantly; however, implementing flash encryption now would require a painful migration process and force everyone to reconfigure their devices.
>
> If you require full flash encryption, a new project implementing the **Aliro** standard is in development which will include encryption by default. Join the Discord server for updates on when that becomes available.

## 1. Accessing the Web Interface

After you've successfully flashed the firmware and completed the initial setup (connecting to the setup Wi-Fi AP and configuring your home network), you can access the web interface by navigating to your device's IP address in your web browser. If you're unsure of the IP address, check your router's connected devices list, or try accessing using the following hostname format `<Serial Number>.local` (replace `<Serial Number>` with the Serial Number seen in the Home App, e.g. `HK-A1B2C3D4.local`).

The web interface is built using a lightweight Svelte 5 single-page architecture (utilizing `sv-router` / `svelte-router` for client-side navigation instead of a full framework overhead).

The web interface is organized into several main sections: **Info**, **MQTT**, **Actions**, **System**, **OTA Update**, and **Logs**.

---

## 2. Info

### 2.1. HomeKey

This section provides read-only information about your HomeKey reader, including its Group Identifier (GID), Unique Identifier (ID), and details about any configured issuers and their endpoints. This is useful for debugging and verifying HomeKey functionality.

*   **Reader GID:** The Group Identifier of your HomeKey reader.
*   **Reader ID:** The Unique Identifier of your HomeKey reader.
*   **Issuers List:** A list of HomeKey issuers configured on your device, along with their Issuer IDs and their enrolled Endpoint IDs.

### 2.2. System Metrics & Module Status

This section provides real-time information about your HomeKey-ESP32 device's operational status and system metrics:

*   **Version:** The firmware version of HomeKey-ESP32.
*   **UI Version:** The version of the web interface.
*   **Device Name:** The name of your device.
*   **Uptime:** System uptime in milliseconds since boot.
*   **Free Heap:** Available free heap memory in bytes.
*   **Wi-Fi Signal:** Wi-Fi RSSI (Received Signal Strength Indicator) in dBm.
*   **Ethernet Status:** Indicates whether Ethernet connectivity is active.
*   **NFC Module Status:** Shows the connection status of the configured NFC module (PN532 or PN7160/PN7161). Displays active status label and diagnostic messages.
*   **MQTT Connection Status:** Real-time label indicating connection state (Connected, Error, Disconnected) along with specific error codes/messages if connection fails.

---

## 3. MQTT

This section allows you to configure how your HomeKey-ESP32 device communicates with your MQTT broker for home automation integrations. Changes in this section will reboot the device.

### 3.1. Broker

*   **Address:** Hostname or IP address of your MQTT broker (e.g., `192.168.1.100`).
*   **Port:** Broker port number (default: `1883`, or `8883` for TLS).
*   **Client ID:** Unique MQTT client identifier (automatically generated if left blank).
*   **LWT Topic:** Last Will and Testament topic (`tele/LWT`). Publishes `online` / `offline`.
*   **Username / Password:** Credentials for authenticating with your MQTT broker.
*   **HASS MQTT Discovery:** Enables Home Assistant MQTT auto-discovery for locks and NFC tags.
*   **Enable SSL/TLS:** Enables TLS encryption for MQTT connections.
    *   **Disable CN Check (`mqttTlsCnCheckDisable`):** Option to bypass Common Name verification when using custom or self-signed broker certificates.

#### Connection Status

The MQTT interface displays real-time connection status labels:
*   **Connected** (green): Successfully authenticated and connected.
*   **Error** (red): Connection failed with details (authentication error, connection refused, network timeout, SSL error).
*   **Disconnected** (yellow): Not connected.

### 3.2. Topics

#### 3.2.1. Core Topics

*   **NFC/HK Topic:** Topic where HomeKey authentication data or generic NFC tag UIDs are published.
*   **Ignore NFC Tags:** Option to suppress publishing non-HomeKey generic NFC tags.
*   **Lock Control Topics:** `Lock State Topic`, `Lock State Cmd Topic`, `Lock Current State Cmd Topic`, `Lock Target State Cmd Topic`, `SmartLock battery level Cmd Topic`.

#### 3.2.2. Custom Lock States & Actions

*   Grouped custom lock state fields matching relevant topics.
*   Configurable numeric action payloads for custom Unlock/Lock state commands and conditions (`Unlocking`, `Locking`, `Unlocked`, `Locked`, `Jammed`, `Unknown`).

---

## 4. Actions (Hardware Actions)

Configures hardware feedback (Relays, LEDs, NeoPixels) triggered by HomeKey events and lock state updates.

### 4.1. NFC Triggers

*   **Neopixel:** GPIO pin, pixel type (WS2812B GRB, etc.), timeout settings, RGB colors for Auth Success, Auth Failure, and Generic Tag Scans.
*   **Simple GPIO:** Configurable GPIO pins, pulse timeouts, and electrical states (`HIGH`/`LOW`) for Auth Success, Auth Failure, and Alternate Action Initiator.

### 4.2. State Triggers

*   **Simple GPIO:** Primary relay action pin for lock/unlock state transitions, initial state behavior, and momentary pulse timeouts.
*   **Dummy Mode:** "Dumb Switch" HomeKit mode with configurable momentary timeouts.

---

## 5. System Settings

The **System** page (`/system`) provides device management tools, HomeKit settings, hardware pin configurations, and security settings.

At the top of the page, the **Quick Actions** toolbar provides immediate device management buttons:
*   **Reboot:** Reboots the device immediately.
*   **Start AP:** Manually starts the Wi-Fi setup Access Point (`HomeKey-ESP32`).
*   **Reset HomeKit:** Clears HomeKit pairing database (requires re-pairing with the Home app).
*   **Reset WiFi:** Erases saved Wi-Fi network credentials and restarts in AP mode.

Below Quick Actions, settings are structured into three intuitive tabs: **HomeKit**, **Hardware**, and **Security**.

---

### 5.1. HomeKit Tab

Configure HomeKit device identity and authentication behavior.

*   **Device Name:** Name of your HomeKit accessory as seen in the Apple Home app (e.g., `HK`).
*   **Setup Code:** 8-digit HomeKit pairing setup code (e.g., `15935728`).
*   **Always Lock on HomeKey:** Forces the device into a Locked state whenever a valid HomeKey is tapped, regardless of its current lock state.
*   **Always Unlock on HomeKey:** Forces the device into an Unlocked state whenever a valid HomeKey is tapped, regardless of its current lock state.
*   **SmartLock Battery Reporting:** Enables battery percentage reporting to HomeKit (configurable via MQTT).
*   **Auth Precompute Cache:** Precomputes authentication credentials for faster HomeKey tap responses (uses slightly higher CPU/RAM).
*   **HomeKey Finish Color:** Choose your preferred digital pass finish displayed in Apple Wallet (`Tan`, `Gold`, `Silver`, or `Black`).

---

### 5.2. Hardware Tab

Configure GPIO pin allocations for the NFC reader, Ethernet, and HomeSpan status controls.

#### 5.2.1. GPIO Allocation & Safety
*   **Override strapping pins restriction:** Toggles off safety warnings for boot strapping pins (e.g., GPIO 0, 2, 12, 15).
    > [!CAUTION]
    > Using strapping pins can disrupt normal ESP32 boot behavior if not handled carefully with external pull-up/pull-down resistors.

#### 5.2.2. NFC Reader Configuration
*   **Reader Type:** Select your NFC reader hardware (**PN532** over SPI, or **PN7160 / PN7161** over SPI).
*   **Preset:** Select predefined hardware board presets (@lollokara, CASmo-NFC, PN7161 presets, or `Custom`).
*   **Pin Assignments:**
    *   **PN532 (SPI):** Assign `SS Pin`, `SCK Pin`, `MISO Pin`, and `MOSI Pin`.
    *   **PN7160 / PN7161 (SPI):** Assign SPI bus pins alongside dedicated `IRQ Pin` and `VEN Pin` (Hardware Enable/Reset).
*   **Fast NFC Polling:** Enables reduced delay between poll cycles for quicker tag detection.

#### 5.2.3. Ethernet Configuration
*   **Enable Ethernet:** Enables wired Ethernet network connectivity instead of Wi-Fi.
*   **Board Preset & PHY Type:** Select board preset or PHY chip type (e.g., `W5500`, `LAN8720`, `IP101`).
*   **SPI Configuration:** When using SPI Ethernet modules (like W5500), configure `SPI Bus`, `Freq (MHz)`, `CS Pin`, `IRQ Pin`, `RST Pin`, `SCK Pin`, `MISO Pin`, and `MOSI Pin`.
    > [!NOTE]
    > When sharing an SPI bus between PN532/PN7160/PN7161 and SPI Ethernet, ensure the SCK, MISO, and MOSI pins match.

#### 5.2.4. HomeSpan Settings
*   **OTA Password:** Password for HomeSpan command-line OTA updates.
*   **Control GPIO Pin:** Optional physical pairing/reset button pin (set to `255` if unused).
*   **Status LED GPIO Pin:** Optional HomeSpan status indicator LED pin (set to `255` if unused).

---

### 5.3. Security Tab

Configure WebUI authentication, HTTPS SSL/TLS encryption, and certificates.

#### 5.3.1. Web Authentication
*   **Enable Web Authentication:** Protects the web interface with a username and password login prompt.
*   **Username / Password:** Credentials required to log into the web interface.

#### 5.3.2. HTTPS Encryption
*   **Enable HTTPS:** Enables SSL/TLS encryption for web browser access over Port 443.
    > [!NOTE]
    > Enabling or disabling HTTPS requires a device reboot to take effect.

#### 5.3.3. Certificate Management
*   **Certificate Requirements:** Upload PEM-formatted certificates (RSA 2048+ or SECP256R1 P-256). Server certificate and private key are required for HTTPS; CA certificate is optional for mTLS client verification.
*   **Upload Certificate:**
    *   `Certificate File (.pem, .crt, .cer)`
    *   `Private Key (.pem, .key)`
    *   `CA Certificate File (Optional) (.pem, .crt, .cer)`
*   Displays SHA1 fingerprints and expiration details for active server and client certificates.

---

## 6. OTA Update

Upload pre-compiled application binaries (`*.firmware.bin`) or web filesystem images (`littlefs.bin`) for seamless Over-The-Air upgrades.

---

## 7. Logs & Diagnostics

*   **Live Log Streaming:** Real-time log streaming using `@tanstack/svelte-virtual`.
*   **Log Level Filtering:** Toggle between `ERROR`, `WARN`, `INFO`, `DEBUG`, and `VERBOSE` log levels.
*   **Collapsible Mobile Toolbar:** Compact log toolbar designed for mobile viewports with expandable search and filter controls.
*   **Formatted JSON Log Export:** Export diagnostic logs to timestamped JSON files (`logs-YYYY-MM-DD-HH-MM-SS.json`) for easy troubleshooting and support sharing.

