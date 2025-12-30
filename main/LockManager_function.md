# LockManager Module

## Description
The `LockManager` is the central brain of the application. It implements an event-driven state machine that orchestrates the behavior of the smart lock. It decouples the logical state of the lock from the various control interfaces (HomeKit, MQTT, Web, NFC).

## Key Responsibilities
- **State Management**: Maintains the current state of the lock (Locked, Unlocked, Jammed, etc.) and the target state.
- **Event Orchestration**: Subscribes to input events from protocol adapters (HomeKit, MQTT, NFC) and publishes action events to the HardwareManager.
- **Auto-Lock**: Manages the auto-lock timer to automatically secure the door after a set duration.
- **Synchronization**: Ensures that state changes triggered by one source (e.g., NFC) are reflected across all other interfaces (e.g., HomeKit, MQTT).

## Dependencies
- **espp::EventManager**: Used to publish state changes and subscribe to command events.
- **HardwareManager**: Receives action commands (open/close) from the LockManager.
- **ConfigManager**: Provides settings for features like auto-lock duration.

## Key Files
- `main/LockManager.cpp`
- `main/include/LockManager.hpp`
- `main/HomeKitLock.cpp` (Acts as the HomeKit protocol adapter)
- `main/include/HomeKitLock.hpp`
