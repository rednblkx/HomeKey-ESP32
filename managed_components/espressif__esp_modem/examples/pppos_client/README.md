# PPPoS simple client example

(See the README.md file in the upper level 'examples' directory for more information about examples.)

## Overview
This example shows how to act as a MQTT client after the PPPoS channel created by using [ESP-MQTT](https://docs.espressif.com/projects/esp-idf/en/latest/api-reference/protocols/mqtt.html) APIs.

## How to use this example

See the README.md file in the upper level `pppos` directory for more information about the PPPoS examples.

### USB DTE support

For USB enabled targets (ESP32-S2, ESP32-S3, or ESP32-P4), it is possible to connect to the modem device via USB.
1. In menuconfig, navigate to `Example Configuration->Type of serial connection to the modem` and choose `USB`.
2. Connect the modem USB signals to your ESP chip (pin 19 (DATA-) and 20 (DATA+) for ESP32-S2/S3).

USB example uses Quactel BG96 modem device. BG96 needs a positive pulse on its PWK pin to boot-up.

This example supports USB modem hot-plugging and reconnection.
