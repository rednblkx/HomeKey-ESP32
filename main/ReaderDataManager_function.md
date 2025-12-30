# ReaderDataManager Module

## Description
The `ReaderDataManager` is a specialized storage module for Apple HomeKey data. Unlike the general `ConfigManager`, it handles sensitive cryptographic material and dynamic provisioning data required for the HomeKey protocol.

## Key Responsibilities
- **Secure Storage**: Manages the storage of the Reader Private Key, Reader ID, and Group ID.
- **Issuer Management**: Stores the public keys and permissions of trusted HomeKit controllers (Issuers).
- **Endpoint Management**: Manages the endpoint configurations that define how the lock appears to iOS devices (e.g., color, name).
- **Lifecycle Management**: Handles the creation, update, and deletion of HomeKey credentials during the pairing and unpairing process.

## Dependencies
- **NVS Flash**: Stores the data persistently.
- **msgpack**: Uses MessagePack for efficient binary serialization of the complex HomeKey data structures.
- **HomeKey.h**: Defines the `readerData_t` structure.

## Key Files
- `main/ReaderDataManager.cpp` (implied, or implemented in `main.cpp` or similar if not present as standalone cpp, but header exists)
- `main/include/ReaderDataManager.hpp`
