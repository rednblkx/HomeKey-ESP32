# Target test running OTA update

## Overview

The aim of this test is to exercise the most commonly failing scenario, running OTA over PPPoS with https.

This project opens a data session, runs basic mqtt operations and initiates OTA update.
It supports the following test configurations:
* Using a real modem device (default config)
* Using VFS device (only to exercise VFS DTE)
* Using network-only DCE (connecting directly to PPP server) -- needs some configuration

### Configuring the PPP server

You need to run these applications on your host machine:
* PPP server
```bash
sudo pppd /dev/ttyUSB1 115200 192.168.11.1:192.168.11.2 ms-dns 8.8.8.8 modem local noauth debug nocrtscts nodetach +ipv6
```
* MQTT broker: Running mosquitto in the default config is enough, configuring the broker's URL to the local PPP address: `config.broker.address.uri = "mqtt://192.168.11.1";`
* HTTP server: Need to support HTTP/1.1 (to support ranges). You can use the script `http_server.py` and configure the OTA endpoint as `"https://192.168.11.1:1234/esp32.bin"`

## Potential issues

When running this test it is expected to experience some buffer overflows or connection interruption.
The modem library should recover from these failure modes and continue and complete OTA update.
These issues are expected, since UART ISR is deliberately not placed into IRAM in the test configuration to exhibit some minor communication glitches.
