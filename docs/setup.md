# Setup Guide: Bringing Your HomeKey-ESP32 to Life! 🚀

<!--toc:start-->
- [Setup Guide: Bringing Your HomeKey-ESP32 to Life! 🚀](#setup-guide-bringing-your-homekey-esp32-to-life-🚀)
  - [1. Download the Firmware](#1-download-the-firmware)
  - [2. Connect Your Hardware](#2-connect-your-hardware)
    - [2.1. NFC Module Wiring](#21-nfc-module-wiring)
      - [2.1.1. Integrated PCB BOARDS](#211-integrated-pcb-boards)
      - [2.1.2. Default GPIO Pinouts](#212-default-gpio-pinouts)
  - [3. Flash the Firmware](#3-flash-the-firmware)
    - [Option A: Flashing with esptool.py (Command-Line)](#option-a-flashing-with-esptoolpy-command-line)
    - [Option B: Flashing with esptool-js (Browser-Based)](#option-b-flashing-with-esptool-js-browser-based)
  - [4. Wi-Fi Configuration & Initial Setup](#4-wi-fi-configuration-initial-setup)
  - [5. HomeKit Pairing](#5-homekit-pairing)
  - [6. Troubleshooting Common Setup Issues](#6-troubleshooting-common-setup-issues)
<!--toc:end-->

Welcome to the exciting part! This guide will walk you through the process of getting your HomeKey-ESP32 device up and running. We'll cover everything from downloading the firmware to flashing it onto your ESP32 and getting it connected to your network.

## 1. Download the Firmware

First things first, let's get the brains of your HomeKey-ESP32 onto your computer. We provide pre-compiled firmware binaries, so you don't need to worry about compiling anything yourself!

1.  **Visit the GitHub Releases Page:** Head over to the official HomeKey-ESP32 GitHub releases page:
    [https://github.com/rednblkx/HomeKey-ESP32/releases](https://github.com/rednblkx/HomeKey-ESP32/releases)
2.  **Download the Latest Release:** Look for the "Latest release" tag. Under the "Assets" section, you'll find several firmware files:

    *   **`*-firmware-merged.bin`**: This is the recommended file for flashing a new device for the first time. It contains the bootloader, application, and LittleFS filesystem all merged into one, ready to be flashed at address `0x0`.
    *   **`*-firmware.bin`**: This file contains only the application firmware and is primarily used for Over-The-Air (OTA) updates.
    *   **`littlefs.bin`**: This file contains the web interface files and is also used for OTA updates of the filesystem.

    More details on OTA updates can be found in the [Updates Guide](updates.md).

## 2. Connect Your Hardware

Before flashing, you need to connect your PN532 NFC module to your ESP32 development board.

*   **Using Jumper Wires:** If you're connecting the modules manually, refer to the [NFC Module Wiring](#21-nfc-module-wiring) section below. Ensure you connect the correct SPI pins (SCK, MISO, MOSI, SS) and power (VCC/3V3, GND). Remember the tip about soldering for the best connectivity!
*   **Using an Integrated PCB Board:** If you have one of the awesome [Integrated PCB Boards](prerequisites.md#23-integrated-pcb-boards-optional-but-simpler) then everything should ready but make sure to check for any additional instructions for the board you've chosen.

### 2.1. NFC Module Wiring

> [!NOTE]
>  Both the ESP32 and the PN532 must use the same power supply to communicate.

#### 2.1.1. Integrated PCB BOARDS

When using an Integrated PCB, all the physical connections between the PN532 and the ESP32 have already been made, however, you might still have to update the pins used within the firmware from the [WebUI](configuration.md#pn532).

1.  **@lollokara's board (ESP32-C3)**
    *   SS: GPIO6
    *   SCK: GPIO5
    *   MISO: GPIO4
    *   MOSI: GPIO7

#### 2.1.2. Default GPIO Pinouts

The default pinout differs depending on the chip variant and they are defined inside [Arduino-ESP32](https://github.com/espressif/arduino-esp32), see the links below.

> [!NOTE]
> These can be changed from the [WebUI](configuration.md#pn532)

**Default pinout for ESP32:** [pins_arduino.h](https://github.com/espressif/arduino-esp32/blob/master/variants/esp32/pins_arduino.h)
**Default pinout for ESP32-C3:** [pins_arduino.h](https://github.com/espressif/arduino-esp32/blob/master/variants/esp32c3/pins_arduino.h)
**Default pinout for ESP32-C6:** [pins_arduino.h](https://github.com/espressif/arduino-esp32/blob/master/variants/esp32c6/pins_arduino.h)
**Default pinout for ESP32-S3:** [pins_arduino.h](https://github.com/espressif/arduino-esp32/blob/master/variants/esp32s3/pins_arduino.h)

See the table below as wiring example using the ESP32:

| ESP32 Pin | PN532 Pin |
| :-------- | :-------- |
| VCC/3V3   | VCC       |
| GND       | GND       |
| GPIO18    | SCK       |
| GPIO19    | MISO      |
| GPIO23    | MOSI      |
| GPIO5     | SS        |

## 3. Flash the Firmware

Now for the magic moment! You have two main options for flashing the firmware: command-line with `esptool.py` or browser-based with `esptool-js`. Choose the method you're most comfortable with.

### Option A: Flashing with esptool.py (Command-Line)

1.  **Install esptool.py:** If you haven't already, install `esptool.py` using pip:
    ```bash
    pip install esptool
    ```
2.  **Connect ESP32:** Connect your ESP32 development board to your computer using a USB cable.
3.  **Identify Serial Port:** You'll need to find the serial port your ESP32 is connected to.
    *   **Linux:** Typically `/dev/ttyUSB0` or `/dev/ttyACM0`. You can check with `ls /dev/tty*`.
    *   **macOS:** Typically `/dev/cu.usbserial-XXXX` or `/dev/cu.SLAB_USBtoUART`. You can check with `ls /dev/cu.*`.
    *   **Windows:** Check Device Manager for "USB Serial Device" or "CP210x USB to UART Bridge" under "Ports (COM & LPT)". Note the COM port number (e.g., `COM3`).
4.  **Open Terminal/Command Prompt:** Navigate to the directory where you downloaded the `esptool.py` script (if you downloaded it manually) and the `*-firmware-merged.bin` file.
5.  **Run the Flash Command:** Execute the following command, replacing `YOUR_PORT` with your serial port and `YOUR_MERGED_FIRMWARE_FILE.bin` with the name of your downloaded `*-firmware-merged.bin` file:

    ```bash
    esptool.py --chip esp32 --port YOUR_PORT --baud 460800 write_flash 0x0 YOUR_MERGED_FIRMWARE_FILE.bin
    ```
    *   **`--chip esp32`**: Specifies the chip type. Adjust if you are using ESP32-C3 or ESP32-S3.
    *   **`--port YOUR_PORT`**: Your ESP32's serial port.
    *   **`--baud 460800`**: The baud rate for flashing. This is a good balance between speed and reliability.
    *   **`write_flash 0x0`**: The command to write the flash, starting at address `0x0`. **Crucially, use `0x0` for the merged firmware files.**
    *   **`YOUR_MERGED_FIRMWARE_FILE.bin`**: The path to your downloaded `*-firmware-merged.bin` binary.

6.  **Erase Flash (Optional, for troubleshooting):** If you encounter any issues, such as a reset loop, you can try erasing the flash before flashing the firmware:
    ```bash
    esptool.py --chip esp32 --port YOUR_PORT erase_flash
    ```
    Then, re-run the flash command from step 5.

7.  **Initiate Flash Mode:** If the flashing doesn't start automatically, you might need to manually put your ESP32 into flash mode. This usually involves:
    *   Holding down the "BOOT" or "GPIO0" button on your ESP32 board.
    *   Pressing and releasing the "EN" or "RST" button.
    *   Releasing the "BOOT" button.
    *   Try the flash command again.

8.  **Wait for Completion:** The flashing process will take a few moments. Once complete, you'll see a "Hash of data verified" message.

### Option B: Flashing with esptool-js (Browser-Based)

This is the easiest way to flash if you're using a Chromium-based browser!

1.  **Connect ESP32:** Connect your ESP32 development board to your computer using a USB cable.
2.  **Open esptool-js Page:** Navigate to the page below in your Chromium-based browser (Chrome, Edge, Brave, etc.):
    [https://espressif.github.io/esptool-js/](https://espressif.github.io/esptool-js/)
3.  **Select Firmware File:** On the web page, you'll find an option to select your firmware file. Click "Choose File" and select the appropriate `*-firmware-merged.bin` file you downloaded earlier.
4.  **Select Serial Port:** Click the "Connect" button. A pop-up will appear asking you to select the serial port for your ESP32. Choose the correct port and click "Connect".
5.  **Change Flash Address (if needed):** Ensure the Flash Address is set to `0x0`. The `esptool-js` interface should typically handle this automatically for merged binaries.
6.  **Flash Device:** Once connected, click the "Program" button.
7.  **Initiate Flash Mode (if needed):** Similar to the command-line method, you might need to manually put your ESP32 into flash mode if the process doesn't start. Follow the steps in Option A, step 7.
8.  **Wait for Completion:** The browser interface will show the progress. Once complete, you'll get a success message.

## 4. Wi-Fi Configuration & Initial Setup

After successfully flashing the firmware, your HomeKey-ESP32 is ready for its first boot!

1.  **Disconnect and Reconnect:** Disconnect your ESP32 from USB and then reconnect it. This will restart the device.
2.  **Monitor Serial Output (Optional but Recommended):** If you have a serial monitor (like the one built into the Arduino IDE, or using `idf.py monitor` if you have ESP-IDF installed), you can connect to your ESP32's serial port (usually at 115200 baud) to see boot messages and debug information.
3.  **Connect to Wi-Fi Access Point:** On its first boot, the device will start a Wi-Fi access point (AP) with the following details:
    *   **SSID:** `HomeSpan-Setup`
    *   **Password:** `homespan`
    Connect your phone or computer to this Wi-Fi network.
4.  **Access the Captive Portal:** A captive portal should open automatically. If it doesn't, you can open it manually by navigating to `http://192.168.4.1/hotspot-detect.html` in your web browser.
5.  **Configure Wi-Fi and HomeKit Setup Code:** On the captive portal page, you can configure the device to connect to your home Wi-Fi network. You can also set the HomeKit setup code on this page.
6.  **Access the Web Interface:** After the device is connected to your Wi-Fi network, you can access the full web interface by navigating to the device's IP address in your web browser. The web interface allows you to configure all aspects of the device, including MQTT, actions, and other miscellaneous settings.

## 5. HomeKit Pairing

The default HomeKit pairing code is `466-37-726`. You can change this code in the web UI or through the captive portal during Wi-Fi setup.

Once the device is connected to your Wi-Fi network, you can pair it with HomeKit by entering the setup code in the Home app.

## 6. Troubleshooting Common Setup Issues

*   **"Failed to connect to ESP32: Timed out waiting for packet header" or similar errors during flashing:**
    *   Ensure your ESP32 is properly in flash mode (hold BOOT, press/release EN, release BOOT).
    *   Double-check that you've selected the correct serial port.
    *   Try a different USB cable or USB port on your computer.
    *   Ensure your power supply is stable.
*   **ESP32 not creating Wi-Fi AP:**
    *   Verify the firmware flashed successfully.
    *   Try a hard reset (disconnect/reconnect USB power).
    *   Check serial output for any error messages.
*   **Cannot access web interface (192.168.4.1):**
    *   Make sure your device (phone/computer) is actually connected to the HomeKey-ESP32 Wi-Fi AP.
    *   Try clearing your browser's cache or using an incognito/private browsing window.
    *   Ensure no other network settings on your computer are interfering.

---

Congratulations! Your HomeKey-ESP32 is now flashed and should be connected to your network. The next step is to dive into the [Configuration Guide](configuration.md) to fine-tune its settings and integrate it with HomeKit and Home Assistant!
