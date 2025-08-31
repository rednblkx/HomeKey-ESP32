![homekey-logo-white](https://github.com/user-attachments/assets/fc93a70a-ef1e-4390-9067-6fafb255e5ac)

# HomeKey-ESP32 [![Discord](https://badgen.net/discord/members/VWpZ5YyUcm?icon=discord)](https://discord.com/invite/VWpZ5YyUcm) [![CI](https://github.com/rednblkx/HomeKey-ESP32/actions/workflows/esp32.yml/badge.svg?branch=main)](https://github.com/rednblkx/HomeKey-ESP32/actions/workflows/esp32.yml)

### HomeKey functionality for the rest of us.

[![ko-fi](https://ko-fi.com/img/githubbutton_sm.svg)](https://ko-fi.com/L3L2UCY8N)

## Overview

This project aims to provide Apple HomeKey functionality using an ESP32 and PN532 NFC Module. Its primary focus is HomeKey; other NFC functionalities like MIFARE® Authentication are out of scope.

*   The internal state is published and controlled via MQTT through user-defined topics.
*   Any NFC Target not identified as HomeKey will publish its UID, ATQA, and SAK to the same MQTT topic, with the `"homekey"` field set to `false`.
*   **Important:** This project is **not yet** made for battery-powered applications as it's currently unable to use sleep states effectively.
*   Designed for an ESP32 chip with 4MB Flash size.

The project's goal is to enable HomeKey functionality for locks or other devices that don't natively support it.

For more advanced functionality, consider [HAP-ESPHome](https://github.com/rednblkx/HAP-ESPHome), which integrates HomeKit (and HomeKey) into ESPHome for ultimate automations.

## ✨ Features

*   **Apple HomeKey Integration:** Unlock your door with a tap using your iPhone or Apple Watch.
*   **HomeKit Compatible:** Seamlessly integrates with your Apple Home ecosystem.
*   **NFC Power:** Utilizes the PN532 NFC module for reliable and fast communication.
*   **Home Assistant Integration:** Trigger automations and track lock state within Home Assistant using NFC tags and MQTT.
*   **MQTT Connectivity:** Publish lock status and receive commands over MQTT.
*   **Web Interface:** Easy configuration and management through a user-friendly web interface.
*   **Over-The-Air (OTA) Updates:** Keep your firmware fresh and secure wirelessly.

## 🛠️ Key Technologies Under the Hood

*   **ESP-IDF:** Robust Espressif IoT Development Framework.
*   **HomeSpan:** Framework for HomeKit integration.
*   **PN532 NFC Module:** Hardware for reading HomeKey passes.
*   **MQTT:** Lightweight messaging protocol for device communication.
*   **ESPAsyncWebServer:** Powers the web interface.
*   **LittleFS:** Stores web interface files and configuration data.

## 🚀 Getting Started

Ready to dive in? We've got you covered!

1.  **Prerequisites:** Ensure you have all necessary tools and software. Check our [Prerequisites Guide](docs/prerequisites.md).
2.  **Setup:** Follow our step-by-step [Setup Guide](docs/setup.md) to get your ESP32 and PN532 running.
3.  **Configuration:** Learn how to customize your HomeKey-ESP32 in the [Configuration Guide](docs/configuration.md).
4.  **MQTT Integration:** Discover how to connect your lock to your MQTT broker and Home Assistant in the [MQTT Guide](docs/mqtt.md).
5.  **Updates:** Keep your device up-to-date with our [Updates Guide](docs/updates.md).

## Disclaimer

Use this at your own risk; I'm a hobbyist, not a cryptographic expert. HomeKey was implemented through reverse-engineering, so it may lack elements from Apple's private specification.

This project is still a **work in progress**; will try to keep things stable but breaking changes should still be expected.

## Contributing & Support

Contributions are welcome. Feel free to fork the repository and submit pull requests.

For suggestions or assistance, open an issue or join the Discord server at https://discord.com/invite/VWpZ5YyUcm

If you like the project, please consider giving it a star ⭐ to show appreciation and help others discover it.

## Credits

-   [@kormax](https://github.com/kormax) for reverse-engineering the Homekey [NFC Protocol](https://github.com/kormax/apple-home-key) and publishing a [PoC](https://github.com/kormax/apple-home-key-reader)
-   [@kupa22](https://github.com/kupa22) for the [research](https://github.com/kupa22/apple-homekey) on the HAP side of things for HomeKey
-   [HomeSpan](https://github.com/HomeSpan/HomeSpan) which is being used as the framework implementing the HomeKit accessory

## License

This project is licensed under the [MIT License](LICENSE).
