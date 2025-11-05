---
title: "Updates"
weight: 5
---

# Keeping Your HomeKey-ESP32 Fresh!

This document outlines different methods for updating the firmware on your HomeKey-ESP32 device. Keeping your device up-to-date ensures you have the latest features, bug fixes, and security enhancements.

**Required Files for Updates:**

*   `*-firmware.bin`: The main application firmware file.
*   `littlefs.bin`: Contains the web interface files (LittleFS filesystem).
*   `*-firmware-merged.bin`: A combined file containing the bootloader, application, and LittleFS, primarily used for initial flashing or full re-flashes via USB.

## 1. Over-The-Air (OTA) Updates

The primary method for Over-The-Air (OTA) updates is through the WebUI. This allows you to update your device wirelessly.

### 1.1. WebUI Updates

The easiest way to update your device is through the web interface. Simply navigate to the device's IP address in your web browser and nvaigate to the "OTA Update" page.

1.  **Access the Web Interface:** Navigate to the device's IP address in your web browser.
2.  **Navigate to OTA Update section:** Click the "OTA Update" button on the left-hand side of the page.
3.  **Select Firmware File:** Select the `*-firmware.bin` file you downloaded earlier.
4.  **Select LittleFS File:** Select the `littlefs.bin` file you downloaded earlier.
5.  **Flash Firmware and LittleFS:** Click the "Upload Both" button to initiate the update process.
6.  **Reboot:** The device will automatically reboot after the OTA process is complete.

If everything went smoothly, you should see the "Current Version" and "Running Partition" fields update (once it reconnected) to reflect the new firmware version and partition.

### 1.2. `espota` Updates

### 1.2.1. Requirements

*   Your HomeKey-ESP32 device connected to your Wi-Fi network.
*   `espota` tool (available as a Windows executable or a Python script for Linux/macOS, both available [here](https://github.com/espressif/arduino-esp32/tree/master/tools)).
*   The `*-firmware.bin` file for your ESP32 chip (e.g., `esp32-firmware.bin`, `esp32c3-firmware.bin`, or `esp32s3-firmware.bin`) from the [GitHub Releases page](https://github.com/rednblkx/HomeKey-ESP32/releases).
*   The `littlefs.bin` file from the [GitHub Releases page](https://github.com/rednblkx/HomeKey-ESP32/releases).
*   The IP address of your HomeKey-ESP32 device.
*   (Optional) The OTA password, if you've set one in the [Configuration Guide](../configuration#homespan).
    *   The default OTA password is `homespan-ota`

### 1.2.2. Update

1.  **Download `espota`:** Get the `espota` tool from the provided link.
2.  **Open Terminal/Command Prompt:** Navigate to the directory where you downloaded `espota` and your firmware files.
3.  **Flash Firmware:** Use the following command to flash the main firmware:
    *   **Windows:**
        ```bash
        espota.exe -r -i <address_of_device> -a <ota_password> -f <esp32xx-firmware.bin>
        ```
    *   **Linux/macOS:**
        ```bash
        python espota.py -r -i <address_of_device> -a <ota_password> -f <esp32xx-firmware.bin>
        ```
    *   Replace `<address_of_device>` with your device's IP address, `<ota_password>` with your OTA password (if set), and `<esp32xx-firmware.bin>` with the path to your `*-firmware.bin` file.
4.  **Flash LittleFS:** After the firmware is flashed, you must flash the `littlefs.bin` file using a similar command, but with the `-s` flag added:
    *   **Windows:**
        ```bash
        espota.exe -r -i <address_of_device> -a <ota_password> -f <littlefs.bin> -s
        ```
    *   **Linux/macOS:**
        ```bash
        python espota.py -r -i <address_of_device> -a <ota_password> -f <littlefs.bin> -s
        ```
5.  **Reboot:** The device will automatically reboot after the OTA process is complete.

## 2. Manual Update via USB (`esptool.py`)

If OTA updates aren't working, or if you prefer a wired connection, you can always update your device via USB using `esptool.py`. This method is similar to the initial flashing process.

### 2.1 Requirements

*   Your HomeKey-ESP32 device.
*   A USB cable to connect your ESP32 to your computer.
*   `esptool.py` installed on your computer (see [Prerequisites Guide](../prerequisites#option-1-command-line-flashing-with-esptoolpy-recommended-for-advanced-users)).
*   The `esp32XX-firmware.bin` and `littlefs.bin` files.

### 2.2. Update

1.  **Connect ESP32:** Connect your ESP32 development board to your computer using a USB cable.
2.  **Identify Serial Port:** Find the serial port your ESP32 is connected to (refer to [Setup Guide](../setup#option-a-flashing-with-esptoolpy-command-line) for details).
3.  **Open Terminal/Command Prompt:** Navigate to the directory where you downloaded the `esptool.py` script and your firmware files.

4.  **Flash Firmware and LittleFS Separately (Advanced):**
    If you need to flash the application and filesystem separately (e.g., for specific development or recovery scenarios), use the following command. **Note the different flash addresses.**

    ```bash
    esptool.py --port YOUR_PORT write_flash 0x20000 <firmware.bin> 0x200000 <firmware.bin> 0x3e0000 <littlefs.bin>
    ```
    *   Replace `<firmware.bin>` and `<littlefs.bin>` with the paths to your respective files.
    *   Replace `YOUR_PORT` with your serial port assigned to your ESP32.

5.  **Initiate Flash Mode:** If the flashing doesn't start automatically, you might need to manually put your ESP32 into flash mode (refer to [Setup Guide](../setup#option-a-flashing-with-esptoolpy-command-line) for details).
6.  **Wait for Completion:** The flashing process will take a few moments. Once complete, you'll see a "Hash of data verified" message.
7.  **Reboot:** Disconnect and reconnect your ESP32 from USB to reboot the device.

## 3. Important Notes on Updates

*   **Check Release Notes:** Always check the release notes on the [GitHub Releases page](https://github.com/rednblkx/HomeKey-ESP32/releases) before updating. These notes will inform you about new features, bug fixes, and any potential breaking changes or special migration steps required between versions.
*   **Power Stability:** Ensure a stable power supply during the update process. Interrupting power during a flash can corrupt the firmware and require a full re-flash via USB.

---

Keeping your HomeKey-ESP32 updated is key to a smooth and secure smart home experience. Happy updating!
