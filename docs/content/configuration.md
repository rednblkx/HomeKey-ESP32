---
title: "Configuration"
weight: 3
---

# Making HomeKey-ESP32 Yours (via the Web Interface)

Welcome to the control center of your HomeKey-ESP32! This guide will walk you through all the settings you can tweak to make your device work exactly how you want it to. All these configurations are easily managed through the device's intuitive web interface.

## 1. Accessing the Web Interface

After you've successfully flashed the firmware and completed the initial setup (connecting to the `HomeSpan-Setup` Wi-Fi and configuring your home network), you can access the web interface by navigating to your device's IP address in your web browser. If you're unsure of the IP address, check your router's connected devices list, or try accessing using the following hostname format `<Serial Number>.local` (replace `<Serial Number>` with the Serial Number seen in the Home App, e.g. `HK-A1B2C3D4.local`)

The web interface is organized into several sections, accessible via the main navigation buttons: **HK Info**, **MQTT**, **Actions**, and **Misc**.

---

## 2. Info

### 2.1. HomeKey

This section provides read-only information about your HomeKey reader, including its Group Identifier (GID), Unique Identifier (ID), and details about any configured issuers and their endpoints. This is useful for debugging and verifying HomeKey functionality.

*   **Reader GID:** The Group Identifier of your HomeKey reader.
*   **Reader ID:** The Unique Identifier of your HomeKey reader.
*   **Issuers List:** A list of HomeKey issuers configured on your device, along with their Issuer IDs and their enrolled Endpoint IDs.

### 2.2. System

This section provides read-only information about your HomeKey-ESP32 device, including its firmware version, device name, and Wi-Fi signal strength.

*   **Version:** The firmware version of HomeKey-ESP32.
*   **UI Version:** This is the version of the web interface. 
*   **Device Name:** The name of your device.
*   **Uptime:** The amount of time in milliseconds that your device has been running since boot.
*   **Free Heap:** The amount of free memory available on your device.
*   **Wi-Fi Signal:** The Wi-Fi signal strength as RSSI (Received Signal Strength Indicator) in dBm.
*   **Ethernet enabled:** Whether Ethernet is enabled or disabled.

---

## 3. MQTT

This section allows you to configure how your HomeKey-ESP32 device communicates with your MQTT broker. MQTT is a lightweight messaging protocol that enables seamless integration with home automation platforms like Home Assistant. Changes in this section will reboot the device.

### 3.1. Broker

These settings define how your device connects to your MQTT broker.

*   **Address:** The hostname or IP address of your MQTT broker (e.g., `192.168.1.100` or `mqtt.myhome.com`).
*   **Port:** The port number your MQTT broker is listening on (default is usually `1883`).
*   **Client ID:** A unique client ID for your device on the MQTT broker. 
    * By default, a unique ID will be generated.
*   **LWT Topic:** The Last Will and Testament (LWT) topic. 
    * The broker will publish an `offline` message to this topic when the device disconnects. 
    * When connected, the device will publish `online` to this topic.
*   **Username:** The username for authenticating with your MQTT broker (if required).
*   **Password:** The password for authenticating with your MQTT broker (if required).
    *   **Note:** The password is stored in plain-text and is not encrypted.
    *   **Note:** The current password is not shown as it is obscured from the backend.
*   **HASS MQTT Discovery:** Enable or disable Home Assistant MQTT Discovery. 
    * If enabled, Home Assistant can automatically discover and configure your device's MQTT entities (the lock and NFC tags entities).
*   Enable SSL/TLS: Enable or disable SSL/TLS for MQTT communication.
    * When enabled, a new section called "SSL/TLS Settings" will appear below it to configure the certificates.

### 3.2. Topics

This section is organized into two tabs: **Core Topics** and **Custom Topics**.

#### 3.2.1. Core

These are the essential MQTT topics used by the HomeKey-ESP32.

*   **NFC/HK Topic:** The MQTT topic where HomeKey authentication data or RFID UID will be published.
*   **Ignore NFC Tags:** If enabled, the device will read NFC tags but will *not* publish their data to MQTT.
*   **Secondary action Topic:** The MQTT topic for publishing the Alternate Action status.
*   **Lock State Topic:** The MQTT topic for publishing the internal state.
*   **Lock State Cmd Topic:** The MQTT control topic for the HomeKit lock state (current and target).
*   **Lock Current State Cmd Topic:** The MQTT control topic for the HomeKey-ESP32 lock current state.
*   **Lock Target State Cmd Topic:** The MQTT control topic for the HomeKey-ESP32 lock target state.
*   **SmartLock battery level Cmd Topic:** The MQTT control topic for setting the battery level to be shown in HomeKit.

#### 3.2.2. Custom

These settings allow for more advanced customization of MQTT topics and states.

*   **Status (for Custom States):** Toggle to enable or disable the use of custom lock states via MQTT.
*   **Custom State Topic:** The MQTT topic where the device will publish its custom lock state.
*   **Custom State Cmd Topic:** The MQTT topic where you can send commands to change the device's custom lock state.
*   **Custom Lock Actions:** Define custom MQTT actions for "Unlock" and "Lock" commands published by HomeKey-ESP32 on the Custom State Topic.
*   **Custom Lock States:** Define custom MQTT states for various lock conditions (e.g., "Unlocking", "Locking", "Unlocked", "Locked", "Jammed", "Unknown") received on the Custom State Command topic.

---

## 4. Actions (Hardware Actions)

This section allows you to configure how your HomeKey-ESP32 interacts with physical components like relays, LEDs, and NeoPixels, providing visual and physical feedback for HomeKey events.

### 4.1. NFC Triggers

These actions are executed on successful/failed HomeKey authentications and on NFC Tag detection. Both Neopixel and Simple GPIO have a momentary state.

This section is organized into two tabs: **Neopixel** and **Simple GPIO**.

#### 4.1.1. Pixel

Configure a Pixel LED for visual feedback.

*   **GPIO Pin:** GPIO Pin used for Pixel.
*   **Pixel Type:** Type of Pixel (e.g., `GRB` for WS2812B).
*   **Timeout (ms) Auth Success:** Duration in milliseconds for the success event.
*   **Timeout (ms) Auth Failed:** Duration in milliseconds for the fail event.
*   **Auth Success Color (R, G, B):** RGB color for Pixel on successful HomeKey authentication.
*   **Auth Failure Color (R, G, B):** RGB color for Pixel on failed HomeKey authentication.
*   **Tag Event Color (R, G, B):** RGB color for Pixel on scanned NFC tag.

#### 4.1.2. Simple GPIO

Configure a simple GPIO pin for visual or physical feedback.

*   **HomeKey Authentication:**
  *   **Auth Success:**
      *   **GPIO Pin:** GPIO Pin pulled HIGH or LOW on successful HomeKey authentication.
      *   **Timeout (ms):** How long (in milliseconds) the success pin should hold its state.
      *   **GPIO State:** Whether the success pin should go `LOW` or `HIGH`.
  *   **Auth Failure:**
      *   **GPIO Pin:** GPIO Pin pulled HIGH or LOW on failed HomeKey authentication.
      *   **Timeout (ms):** How long (in milliseconds) the fail pin should hold its state.
      *   **GPIO State:** Whether the fail pin should go `LOW` or `HIGH`.
  *   **Seconds action on success:** Configure a secondary action on successful HomeKey authentication.
      *   **Initiator GPIO Pin:** GPIO Pin to initialize an alternate action after successful HomeKey authentication.
      *   **Initiator Timeout (ms):** Timeout for alternate action initialization.
      *   **Feedback LED Pin:** GPIO Pin for an LED indicating alternate action initialization.
      *   **GPIO Pin:** GPIO Pin to trigger the alternate action.
      *   **Timeout (ms):** Timeout for the alternate action.
      *   **GPIO State:** GPIO state for the alternate action.
*   **NFC Tag Event:**
    *   **GPIO Pin:** GPIO Pin pulled HIGH or LOW on scanned NFC tag.
    *   **Timeout (ms):** How long (in milliseconds) the tag event pin should hold its state.
    *   **GPIO State:** Whether the tag event pin should go `LOW` or `HIGH`.

### 4.2. State Triggers

These actions are executed upon interaction in the Home app and optionally on successful HomeKey Authentication (enabled by default). 

This section is organized into two tabs: **Simple GPIO** and **Dummy**.

#### 4.2.1. Simple GPIO

Configure a simple GPIO pin to trigger for lock/unlock actions. This follows the "Always Lock/Unlock on HomeKey" option. Momentary state applies only if the initial state is "LOCKED".

*   **GPIO Pin:** The primary GPIO pin to trigger for lock/unlock actions (e.g., connected to a relay for a door strike).
*   **GPIO State - Locked:** The electrical level (`LOW`/`HIGH`) for the action pin when the lock is in the "locked" state.
*   **GPIO State - Unlocked:** The electrical level (`LOW`/`HIGH`) for the action pin when the lock is in the "unlocked" state.
*   **Actionable by HomeKey:** If enabled, HomeKit will directly control the GPIO state.
*   **Momentary state:** Enables momentary state for the action pin. Options: `Disabled`, `Home App Only`, `Home Key Only`, `Home App + Home Key`.
*   **Momentary timeout (ms):** Duration (in milliseconds) for the momentary state.

#### 4.2.2. Dummy

This option follows the "Always Lock/Unlock on HomeKey" option. Momentary state applies only if the initial state is "LOCKED".

*   **Status:** Enable or disable "Dumb Switch" mode for HomeKit.
*   **Momentary state:** Enables momentary state. Options: `Disabled`, `Home App Only`, `Home Key Only`, `Home App + Home Key`.
*   **Momentary timeout (ms):** Duration (in milliseconds) for the momentary state.

---

## 5. Misc (Miscellaneous)

This section covers general device behavior, HomeKit parameters, GPIO pin assignments, and web interface authentication. Changes in this section will reboot the device.

### 5.1. General settings

This section is organized into several subsections: **HomeKit**, **HomeKey**, **PN532**, **HomeSpan**, and **Ethernet**.

#### 5.1.1. HomeKit

*   **Device Name:** The name your HomeKey-ESP32 will display in your Home app.
*   **Setup Code:** The 8-digit code used to pair your device with the Apple Home app. 
    *   **Note:** If you updated the HomeKit Setup Code using the AP configuration page, it will not be updated here due to internal constraints.
*   **Always Lock on HomeKey:** If enabled, a successful HomeKey authentication will always trigger a "locked" state, regardless of the current state.
*   **Always Unlock on HomeKey:** If enabled, a successful HomeKey authentication will always trigger an "unlocked" state, regardless of the current state.
*   **SmartLock battery reporting:** Enable this if you want HomeKey-ESP32 to report the battery level of an *external* smart lock or similar device to HomeKit. This requires your external device to send battery level updates to HomeKey-ESP32 via MQTT.
*   **Battery low status Threshold:** The percentage below which HomeKit will report a "low battery" status for the device.

> [!IMPORTANT]
> If using `Always Lock/Unlock on HomeKey`, HomeKit might not react if the new state is the same as the current
> and if you are using HomeKit Automations those will not get triggered

#### 5.1.2. HomeKey

*   **HomeKey Card Finish:** Choose the color of the HomeKey icon that appears in Apple Wallet when you add a HomeKey pass. Options: `Tan`, `Gold`, `Silver`, `Black`.

#### 5.1.3. PN532

This section allows you to manually assign the GPIO pins for the PN532 NFC module's SPI communication. You typically only need to adjust these if you're using a custom wiring setup or a non-standard ESP32 board.

*   **SS Pin:** Slave Select Pin for PN532 SPI communication.
*   **SCK Pin:** Serial Clock Pin for PN532 SPI communication.
*   **MISO Pin:** Master In, Slave Out Pin for PN532 SPI communication.
*   **MOSI Pin:** Master Out, Slave In Pin for PN532 SPI communication.

#### 5.1.4. HomeSpan

*   **OTA Password:** The password required for Over-The-Air (OTA) firmware updates.
    *   **Note:** The password is stored in plain-text and is not encrypted.
    *   **Note:** The current password is not shown as it is obscured from the backend.
*   **Control GPIO Pin:** GPIO Pin for a Configuration Mode button. Refer to HomeSpan documentation for more details.
*   **Status LED GPIO Pin:** GPIO Pin for an LED that indicates the HomeSpan status.

#### 5.1.5. Ethernet

*   **Status:** Enable or disable Ethernet connectivity.
*   **Board Preset:** Choose from predefined Ethernet configurations for various boards, or select "Custom" if you have specific pin assignments.
*   **Ethernet chip:** Select the Ethernet PHY chip used.

Rest of the fields are shown depending on the selected PHY chip as there are two protocols available for this:
  - SPI:
    *   **SPI Frequency:** SPI Frequency for Ethernet communication.
    *   **SS Pin:** Slave Select Pin for SPI Ethernet.
    *   **IRQ Pin:** Interrupt Request Pin for SPI Ethernet.
    *   **RST Pin:** Reset Pin for SPI Ethernet.
    *   **SCK Pin:** Serial Clock Pin for SPI Ethernet.
    *   **MISO Pin:** Master In, Slave Out Pin for SPI Ethernet.
    *   **MOSI Pin:** Master Out, Slave In Pin for SPI Ethernet.
  - RMII(only available on ESP32, newer variants don't have EMAC):
    *   **PHY Address:** PHY Address for RMII Ethernet.
    *   **MCD Pin:** Management Data Clock Pin for RMII Ethernet.
    *   **MDIO Pin:** Management Data Input/Output Pin for RMII Ethernet.
    *   **Power Pin:** Power Pin for RMII Ethernet.
    *   **RMII Clock Mode:** RMII Clock Mode for Ethernet.

### 5.2. WebUI

Protect your device's configuration by setting up authentication for the web interface. This section is organized into one tab: **Authentication**.

#### 5.2.1. Authentication

*   **Status:** Toggle to enable or disable a username and password for accessing the web interface.
*   **Username:** The username for accessing the web interface.
*   **Password:** The password for accessing the web interface. 
    *   **Note:** The password is stored in plain-text and is not encrypted.
    *   **Note:** The current password is not shown as it is obscured from the backend.

---

Remember to save your changes after modifying any settings in the web interface. Your device will typically reboot after saving to apply the new configurations unless stated otherwise.
