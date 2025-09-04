---
title: "Troubleshooting"
weight: 7
---

## `espota.py` - "No response from Device" or "No response from the ESP"

`espota.py` starts by listening on a random port between 10000-60000(definable through option `-P`) and then sends an invitation for connection to the ESP32 on the port 3232.
This means that not only your PC has to be able to reach the ESP32 but it also has to work the other way around.

Make sure your network configuration allows for a connection between the ESP32 and the PC to be established. Host IP and Port can be defined using the `-I` and `-P` options respectively.
