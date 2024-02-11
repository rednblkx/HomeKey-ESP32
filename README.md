# HomeKey-ESP32
[![CI](https://github.com/rednblkx/HomeKey-ESP32/actions/workflows/main.yml/badge.svg?branch=main)](https://github.com/rednblkx/HomeKey-ESP32/actions/workflows/main.yml)
### ESP32 HomeKit Lock with support for the HomeKey

Credit to [@kormax](https://github.com/kormax) for reverse-engineering the Homekey [NFC Protocol](https://github.com/kormax/apple-home-key) and publishing a [POC](https://github.com/kormax/apple-home-key-reader) and credit to [@kupa22](https://github.com/kupa22) for the [research](https://github.com/kupa22/apple-homekey) on the HAP side of things, this project was possible thanks to their work.
Additionaly, thanks to the Arduino library [HomeSpan](https://github.com/HomeSpan/HomeSpan) for being a great library that eased my work on HAP and other things and allowed me to just focus on the homekey functionality.

## Overview

Only the PN532 is supported as an NFC module.

- Only the FAST and STANDARD flows are implementated right now, the ATTESTATION flow will be implemented at a later time since STANDARD can be used in most cases
- Lock State can be received and controlled via MQTT through user-defined topics
- Any NFC Target that's not identified as homekey will skip the flow and at least at the moment it only just publishes the UID, ATQA and SAK to the same MQTT topic as homekey with the property `homekey` set to false
- It is not made for battery-powered applications (yet)
- Designed for a board with 4MB Flash size as OTA is enabled and it needs a second partition of the same size

The code still needs some working so it's very much a work in progress, but the main implementation is roughly there.

Goal of the project is to make it easy to add the homekey functionality to locks that don't support it or to anything for that matter :) .

## Wiring

The current implementation is using SPI for communication.

Pins are the default Arduino pins for SPI which should be as follows:

GPIO18 - SCK

GPIO19 - MISO

GPIO23 - MOSI

GPIO5 - SS

## Configuration

Currently the WiFi can only be configured from the terminal, though the library [HomeSpan](https://github.com/HomeSpan/HomeSpan) that this is based on also has the option of a hotspot but requires a button so haven't bothered with it.

### WIFI

To connect it to WiFi there are two options
- Add your WiFi details in the `config.h` file
- Open the serial terminal, press <kbd>W</kbd> + <kbd>Return</kbd>, and now it should start searching for networks from which to select.
- Open the serial terminal, press <kbd>A</kbd> to start a temporary Access Point then connect to the Wifi network "HomeSpan-Setup" with the password `homespan` and if you are on a phone it should automatically open up the page where you can configure the Wifi credentials, alternatively you can access the page manually on `http://192.168.4.1/hotspot-detect.html`

### HomeKit

The pairing code is the default `466-37-726` from the HomeSpan library that can be found in their documentation [here](https://github.com/HomeSpan/HomeSpan/blob/master/docs/UserGuide.md#pairing-to-homekit). This can be changed at runtime from the terminal. 

For any informations regarding the HomeKit implementation not related to the HomeKey feature, please see the repository for HomeSpan [here](https://github.com/HomeSpan/HomeSpan) where you will also found the documentation.

### Configuration file

In order to connect to your MQTT Broker and WiFi, first modify the file called `config.h` in the src folder with the following content and modify it accordingly with your details

```cpp
#define MQTT_HOST "0.0.0.0"
#define MQTT_PORT 1883
#define MQTT_CLIENTID "homekey_mqtt"
#define MQTT_USERNAME "username"
#define MQTT_PASSWORD "password"
#define MQTT_AUTH_TOPIC "topic/auth"
#define MQTT_SET_STATE_TOPIC "topic/set_state"
#define MQTT_SET_TARGET_STATE_TOPIC "topic/set_target_state"
#define MQTT_SET_CURRENT_STATE_TOPIC "topic/set_current_state"
#define MQTT_STATE_TOPIC "topic/state"
#define HK_CODE "46637726"
#define LED_PIN 2
#define OTA_PWD "homespan-ota"
#define SSID "ssid"
#define WIFI_PWD "password"
#define NAME "HK Lock"
#define DISCOVERY "1"
```

 `MQTT_AUTH_TOPIC` is where authentication details will be published on success
 
 `MQTT_SET_STATE_TOPIC` is the control topic that sets the current and target state of the lock

 `MQTT_SET_TARGET_STATE_TOPIC` controls the target state of the lock

 `MQTT_SET_CURRENT_STATE_TOPIC` controls the current state of the lock
 
 `MQTT_STATE_TOPIC` is where changes to the lock state will be published.
 
 `HK_CODE` is the paring code for HomeKit
 
 `LED_PIN` is the pin for the status led

 `OTA_PWD` is the password for OTA

 `SSID` is the name of your network
 
 `WIFI_PWD` is the Wi-Fi password for your network

 `NAME` is the name for the lock in homekit
 
 `DISCOVERY` controls if [discovery](https://www.home-assistant.io/integrations/tag.mqtt/) messages for the [Home Assistant tag functionality](https://www.home-assistant.io/integrations/tag/) are send at startup, set to 1 to enable, 0 to disable.

On the `MQTT_AUTH_TOPIC` topic, the data format is as follows, depending whether a homekey was authenticated or just an NFC Target was sensed.

- HomeKey
```yaml
{
  "endpointId": "000000000000", # This is unique per device
  "homekey": true,
  "issuerId": "0000000000000000" # This is unique per Apple ID
}
```
- ISO14443A card
```json
{
  "atqa": "0004",
  "homekey": false,
  "sak": "08",
  "uid": "00000000"
}
```

### OTA

Authentication is enabled and the default password is `homespan-ota`.

To upload via OTA, define the address of the ESP on the environment variable `PLATFORMIO_UPLOAD_PORT` and then use the PlatformIO environment `ota`, 

If the default password is not used, you probably know what you are doing, however, a custom password can be used by setting the environment variable `PLATFORMIO_UPLOAD_FLAGS` to `--auth=<ota-password>`
