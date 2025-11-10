---
title: "Troubleshooting"
weight: 7
---

## `espota.py` - "No response from Device" or "No response from the ESP"

`espota.py` starts by listening on a random port between 10000-60000(definable through option `-P`) and then sends an invitation for connection to the ESP32 on the port 3232.
This means that not only your PC has to be able to reach the ESP32 but it also has to work the other way around.

Make sure your network configuration allows for a connection between the ESP32 and the PC to be established. Host IP and Port can be defined using the `-I` and `-P` options respectively.

## HomeKey not working on Apple Watch

Make sure HomeKey is present in the Watch's wallet app. If it is not, wait until you get a notification that HomeKey is available.

If you don't get the notification, try to restart the Watch.

If the accessory was previously unpaired in Home and the Card did not get removed after unpairing, it will not work, you have to wait until the Watch syncs with iCloud and updates the Wallet. If that doesn't happen, try to restart the Watch.

## Sharing HomeKey

HomeKit accessories are synced with iCloud which enables your other devices tied to your account to pair with the accessory and also configure HomeKey if compatible.

To share HomeKey with someone else, you need to invite them to your Apple Home, see https://support.apple.com/en-us/102386 for more information.
