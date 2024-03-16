<img src="https://github.com/rednblkx/HomeKey-ESP32/assets/29458132/6c2e1ea8-f74e-41ac-a690-461e339525acb" width="250" height="250">

# HomeKey-ESP32
[![CI](https://github.com/rednblkx/HomeKey-ESP32/actions/workflows/main.yml/badge.svg?branch=main)](https://github.com/rednblkx/HomeKey-ESP32/actions/workflows/main.yml)
### ESP32 HomeKit Lock with support for the HomeKey

Credit to [@kormax](https://github.com/kormax) for reverse-engineering the Homekey [NFC Protocol](https://github.com/kormax/apple-home-key) and publishing a [POC](https://github.com/kormax/apple-home-key-reader) and credit to [@kupa22](https://github.com/kupa22) for the [research](https://github.com/kupa22/apple-homekey) on the HAP side of things, this project was possible thanks to their work.
Additionaly, thanks to the Arduino library [HomeSpan](https://github.com/HomeSpan/HomeSpan) for being a great library that eased my work on HAP and other things and allowed me to just focus on the homekey functionality.

## Disclaimer

Use this at your own risk, i'm not a cryptographic expert, just a hobbyist. Keep in mind that the HomeKey was implemented through reverse-engineering as indicated above so it might be lacking stuff from Apple's specification to which us private individuals do not have access.

While functional, **the project is still a work in progress so expect breaking changes** as i'm figuring out how everything should fit together and the overrall structure of the code.

## Overview

Right now only the PN532 is supported as the NFC module, however, beware of cheap clones espcially clones of Elechouse's as it will cause issues, i recommend the Elechouse PN532 NFC Module V3 just make sure to buy from a trusted vendor.

- It integrates with HomeAssistant's Tags and can be used to create automations based on a person(issuer) or device(endpoint).
- The lock's state is published and controlled via MQTT through user-defined topics
- Any NFC Target that's not identified as homekey will skip the flow and publishes the UID, ATQA and SAK on the same Authentication MQTT topic 
- It is not made for battery-powered applications due to the power hungry WiFi and the code not being optimized for this
- Designed for a board with an ESP32 chip and 4MB Flash size

Goal of the project is to make it easy to add the homekey functionality to locks that don't support it or to anything for that matter :) .

## Usage

Please visit the [wiki](https://github.com/rednblkx/HomeKey-ESP32/wiki) for documentation on the project

## OTA

Authentication is enabled and the default password is `homespan-ota`.

To upload via OTA, define the address of the ESP on the environment variable `PLATFORMIO_UPLOAD_PORT` and then use the PlatformIO environment `ota`, 

If the default password is not used, you probably know what you are doing, however, a custom password can be used by setting the environment variable `PLATFORMIO_UPLOAD_FLAGS` to `--auth=<ota-password>`
