![homekey-logo-white](https://github.com/user-attachments/assets/fc93a70a-ef1e-4390-9067-6fafb255e5ac)

# HomeKey-ESP32 [![Discord](https://badgen.net/discord/members/VWpZ5YyUcm?icon=discord)](https://discord.com/invite/VWpZ5YyUcm) [![CI](https://github.com/rednblkx/HomeKey-ESP32/actions/workflows/esp32.yml/badge.svg?branch=main)](https://github.com/rednblkx/HomeKey-ESP32/actions/workflows/esp32.yml)

### HomeKey functionality for the rest of us.

[![ko-fi](https://ko-fi.com/img/githubbutton_sm.svg)](https://ko-fi.com/L3L2UCY8N)

## Overview

This project aims to provide the Apple HomeKey functionality with just an ESP32 and PN532 NFC Module. Sole purpose of the project is to provide the HomeKey functionality and other NFC functionalities such as MIfare Authentication or others are out of scope.

- It integrates with HomeAssistant's Tags which makes it easier to create automations based on a person(issuer) or device(endpoint).
- The internal state is published and controlled via MQTT through user-defined topics
- Any NFC Target that's not identified as homekey will skip the flow and publish the UID, ATQA and SAK on the same MQTT topic as HomeKey with the `"homekey"` field set to `false` 
- Code is not ready for battery-powered applications
- Designed for a board with an ESP32 chip and 4MB Flash size

Goal of the project is to make it possible to add the homekey functionality to locks that don't support it or to anything for that matter :)

For more advanced functionality, you might also be interested in [HAP-ESPHome](https://github.com/rednblkx/HAP-ESPHome) which attempts to integrate HomeKit (and HomeKey) into ESPHome for ultimate automations.

## Usage

Visit the [wiki](https://github.com/rednblkx/HomeKey-ESP32/wiki) for documentation on the project

## Disclaimer

Use this at your own risk, i'm not a cryptographic expert, just a hobbyist. Keep in mind that the HomeKey was implemented through reverse-engineering as indicated above so it might be lacking stuff from Apple's specification to which us private individuals do not have access.

While functional as it is now, the project should still be considered as a **work in progress** so expect breaking changes.

## Contributing & Support

All contributions to the repository are welcomed, if you think you can bring an improvement into the project, feel free to fork the repository and submit your pull requests.

If you have a suggestion or are in need of assistance, you can open an issue. Additionally, you can join the Discord server at https://discord.com/invite/VWpZ5YyUcm

If you like the project, please consider giving it a star ⭐ to show the appreciation for it and for others to know this repository is worth something.

## Credits

- [@kormax](https://github.com/kormax) for reverse-engineering the Homekey [NFC Protocol](https://github.com/kormax/apple-home-key) and publishing a [PoC](https://github.com/kormax/apple-home-key-reader)
- [@kupa22](https://github.com/kupa22) for the [research](https://github.com/kupa22/apple-homekey) on the HAP side of things for Homekey
- [HomeSpan](https://github.com/HomeSpan/HomeSpan) which is being used as the framework implementing the HomeKit accessory
