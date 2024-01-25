## HomeKey-ESP32
### ESP32 HomeKit Lock with support for the HomeKey

Credit to [@kormax](https://github.com/kormax) for reverse-engineering the Homekey [NFC Protocol](https://github.com/kormax/apple-home-key) and publishing a [POC](https://github.com/kormax/apple-home-key-reader) and credit to [@kupa22](https://github.com/kupa22) for the [research](https://github.com/kupa22/apple-homekey) on the HAP side of things, this project was possible thanks to their work.
Additionaly, thanks to the Arduino library [HomeSpan](https://github.com/HomeSpan/HomeSpan) for being a great library that eased my work on HAP and other things and allowed me to just focus on the homekey functionality.

## Overview

- Only the FAST and STANDARD flows are implementated right now, the ATTESTATION flow will be implemented at a later time since STANDARD can be used in most cases
- Lock State can be received and controlled via MQTT through user-defined topics
- Any NFC Target that's not identified as homekey will skip the flow and at least at the moment it only just publishes the UID, ATQA and SAK to the same MQTT topic as homekey with the property `homekey` set to false
- It is not made for battery-powered applications (yet)

## Configuration

Currently the WiFi can only be configured from the terminal, though the library [HomeSpan](https://github.com/HomeSpan/HomeSpan) that this is based on also has the option of a hotspot but requires a button so haven't bothered with it.

### WIFI

To connect it to WiFi, open the serial terminal, press <kbd>W</kbd> + <kbd>Return</kbd>, and now it should start searching for networks and then proceed accordingly.

### HomeKit

The pairing code is the default `466-37-726` from the HomeSpan library that can be found in their documentation [here](https://github.com/HomeSpan/HomeSpan/blob/master/docs/UserGuide.md#pairing-to-homekit). This can be changed at runtime from the terminal. 

For any informations regarding the HomeKit implementation not related to the HomeKey feature, please see the repository for HomeSpan [here](https://github.com/HomeSpan/HomeSpan) where you will also found the documentation.

### MQTT

In order to connect to your MQTT Broker, first create a file called `mqtt.h` in the src folder with the following content and modify it accordingly with your details

```cpp
#define MQTT_HOST "0.0.0.0"
#define MQTT_PORT 1883
#define MQTT_CLIENTID "homekey_mqtt"
#define MQTT_USERNAME "username"
#define MQTT_PASSWORD "password"
#define MQTT_AUTH_TOPIC "topic/auth"
#define MQTT_SET_STATE_TOPIC "topic/set_state"
#define MQTT_STATE_TOPIC "topic/state"
```

 `MQTT_AUTH_TOPIC` is where authentication details will be published on success
 
 `MQTT_SET_STATE_TOPIC` is the control topic from which the state of the lock can be changed
 
 `MQTT_STATE_TOPIC` is where changes to the lock state will be published.
 
On the `MQTT_AUTH_TOPIC` topic, the data format is as follows, depending whether a homekey was authenticated or just an NFC Target was sensed.

- HomeKey
```json
{
  "endpointId": "000000000000",
  "homekey": true,
  "issuerId": "0000000000000000"
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
The code still needs some working so it's very much a work in progress, but the main implementation is roughly there.

Goal of the project is to make it easy to add the homekey functionality to locks that don't support it or to anything for that matter :) .
