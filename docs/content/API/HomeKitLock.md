---
title: "HomeKitLock"
---

## 1. Overview

The `HomeKitLock` class serves as the central bridge between the application's core logic and the Apple HomeKit ecosystem, facilitated by the HomeSpan library. It is designed as a singleton and is responsible for initializing the HomeKit accessory, defining its services and characteristics, managing the network connection (Wi-Fi or Ethernet), and synchronizing the lock's state with HomeKit.

A critical function of this class is managing the lifecycle of HomeKey issuers. It listens for changes in paired HomeKit controllers and automatically updates the `ReaderDataManager` with the necessary cryptographic keys (LTPK), ensuring that newly paired devices can use HomeKey.

### Key Responsibilities:

*   **HomeSpan Initialization:** Configures and starts the HomeSpan framework, setting up the device as a HomeKit lock accessory.
*   **Accessory Definition:** Creates all necessary HomeKit services, including the Lock Mechanism, Lock Management, NFC Access, and an optional Battery Service.
*   **State Synchronization:** Subscribes to internal application events to update the lock's current state, target state, and battery status in HomeKit in real-time.
*   **Network Management:** Handles the initialization of an Ethernet connection if configured, with a fallback to Wi-Fi managed by HomeSpan.
*   **HomeKey Issuer Management:** Automatically synchronizes the list of trusted HomeKey issuers with the list of paired HomeKit admin controllers.
*   **Debug Interface:** Provides a set of serial commands for runtime debugging and diagnostics.

## 2. Public API

### Constructor

#### `HomeKitLock()`

Constructs the `HomeKitLock` singleton instance. The constructor enforces that only one instance of the class can exist, restarting the device if a second instance is created. It initializes its internal references to the core application managers and subscribes to essential events for state synchronization.

**Signature:**
```cpp
HomeKitLock(
    std::function<void(int)> &conn_cb,
    LockManager& lockManager,
    ConfigManager& configManager,
    ReaderDataManager& readerDataManager
);
```

**Parameters:**
*   `conn_cb`: A callback function (`std::function<void(int)>`) that will be invoked with a status code whenever the HomeKit connection state changes.
*   `lockManager`: A reference to the `LockManager` instance, used to interact with the lock's logic.
*   `configManager`: A reference to the `ConfigManager` instance, used to retrieve device settings.
*   `readerDataManager`: A reference to the `ReaderDataManager`, used to manage HomeKey issuer data.

### Initialization

#### `begin()`

Initializes and starts all HomeKit-related services. This is the main entry point for the class and must be called after the constructor.

**It performs the following key actions:**
1.  Configures HomeSpan settings (e.g., control pin, status pin, OTA password, device name) from `ConfigManager`.
2.  Calls `initializeETH()` to set up the Ethernet connection if enabled.
3.  Starts the HomeSpan stack, advertising the device as a HomeKit lock.
4.  Builds the accessory hierarchy, adding all required and optional services (Lock Mechanism, NFC Access, Battery Service, etc.).
5.  Registers callbacks for HomeSpan events, such as connection changes and controller list modifications.
6.  Sets up a series of developer debug commands accessible via the serial console.

**Signature:**
```cpp
void begin();
```

### State Update Methods

#### `updateLockState()`

Updates the lock's `Current State` and `Target State` characteristics in HomeKit. To prevent unnecessary network traffic, this method only sends an update if the new state value is different from the currently stored value in the characteristic.

**Signature:**
```cpp
void updateLockState(int currentState, int targetState);
```

**Parameters:**
*   `currentState`: The new current state of the lock (e.g., `HAP_CURRENT_LOCK_STATE_UNSECURED`).
*   `targetState`: The new target state of the lock (e.g., `HAP_TARGET_LOCK_STATE_UNSECURED`).

#### `updateBatteryStatus()`

Updates the `Battery Level` and `Status Low Battery` characteristics in HomeKit. This method should only be called if the physical battery service is enabled in the configuration. It checks for value changes before sending notifications.

**Signature:**
```cpp
void updateBatteryStatus(uint8_t batteryLevel, bool isLow);
```

**Parameters:**
*   `batteryLevel`: The battery charge percentage (0-100).
*   `isLow`: The low battery status (`true` if the battery is low, `false` otherwise).

## 3. Network Management

#### `initializeETH()`

Initializes the Ethernet hardware based on settings stored in `ConfigManager`. If Ethernet is disabled, this method does nothing, allowing HomeSpan to manage the Wi-Fi connection instead. It supports both predefined board presets and custom pin configurations for various Ethernet PHYs.

**Signature:**
```cpp
void initializeETH();
```

#### `ethEventHandler()`

A static event handler that processes network events from the underlying framework (e.g., `ARDUINO_EVENT_ETH_START`, `ARDUINO_EVENT_ETH_GOT_IP`). It is responsible for setting the device hostname based on its MAC address and logging network status changes.

**Signature:**
```cpp
void ethEventHandler(arduino_event_id_t event, arduino_event_info_t info);
```

## 4. Core Logic & Callbacks

These methods are central to the class's operation but are typically invoked by HomeSpan or internal events rather than being called directly by the user.

#### `controllerCallback()`

This callback is triggered by HomeSpan whenever a HomeKit controller is paired or unpaired. It contains the critical logic for managing HomeKey issuers:
*   When a new admin controller is paired, its Long-Term Public Key (LTPK) is used to generate a unique issuer ID. This issuer is then added to the `ReaderDataManager`.
*   When the last admin controller is unpaired, all HomeKey issuer and reader data is wiped to ensure security.
*   Any changes to the issuer list are automatically saved to NVS.

#### `setupDebugCommands()`

This method registers a series of single-character commands with HomeSpan's diagnostic interface, allowing developers to perform actions via a serial terminal.

*   **`D`**: Deletes all stored HomeKey reader data.
*   **`L`**: Sets the log level for various system components.
*   **`F`**: Switches the HomeKey authentication flow (Fast, Standard, Attestation).
*   **`M`**: Erases the stored MQTT configuration.
*   **`N`**: Manually toggles the low-battery status characteristic.
*   **`B`**: Manually sets the battery level characteristic.
*   **`P`**: Prints a list of all currently registered HomeKey issuers.
