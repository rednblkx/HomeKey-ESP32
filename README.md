## HomeKey-ESP32
### ESP32 HomeKit Lock with support for the HomeKey

Credit to @kormax for reverse-engineering the Homekey [NFC Protocol](https://github.com/kormax/apple-home-key) and publishing a [POC](https://github.com/kormax/apple-home-key-reader) and credit to @kupa22 for the [research](https://github.com/kupa22/apple-homekey) on the HAP side of things, this project was possible thanks to their work.

## Overview

- All stages of authentication are mostly implemented, only missing some sanity checks to prevent crashes and stuff and attestation is only like 95% done, it triggers the phone with a success but checks on the lock's side are missing and i was lazy to complete it
- At the moment it's only toggling the state of the lock, you need to bring your own implementation
- It is not battery-powered ready though i plan to make it

The code is currently very rough and needs a lot of polishing so it's still very much a work in progress, but the main implementation is roughly there.

Goal of the project is to make it easy to add the homekey functionality to locks that don't support it or to anything for that matter :) .