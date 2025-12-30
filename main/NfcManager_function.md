# NfcManager Module

## Description
The `NfcManager` handles all interactions with the PN532 NFC controller. It is responsible for detecting NFC tags, executing the Apple HomeKey authentication protocol, and reporting successful authentications to the system.

## Key Responsibilities
- **Hardware Driver**: Manages the SPI/I2C communication with the PN532 chip.
- **Polling Loop**: Continuously scans for NFC cards or smartphones.
- **HomeKey Authentication**: When a compatible device is detected, it orchestrates the fast auth or standard auth flow using the `HK-HomeKit-Lib`.
- **Legacy Support**: Can optionally read standard Mifare tags for legacy access control.

## Dependencies
- **HK-HomeKit-Lib**: Provides the cryptographic logic for HomeKey.
- **ReaderDataManager**: Retrieves the cryptographic keys and issuer data required for authentication.
- **espp::EventManager**: Publishes `nfc/auth` events upon successful entry.

## Key Files
- `main/NfcManager.cpp`
- `main/include/NfcManager.hpp`
- `components/PN532/` (Driver library)
