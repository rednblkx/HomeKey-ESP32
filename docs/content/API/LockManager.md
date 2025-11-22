---
title: "LockManager"
---

## Overview

The `LockManager` class is the central authority for managing the logical state of the lock. It acts as the "brain" of the locking mechanism, maintaining the definitive `currentState` and `targetState`. It receives state change requests from various sources (such as HomeKit, NFC, or MQTT), applies business logic (like momentary unlocks or NFC tap behavior), and then publishes the resulting state changes to the rest of the application. This decouples the high-level state logic from the low-level hardware implementation.

## Key Responsibilities

*   **State Management:** Maintains the official current and target states of the lock.
*   **Business Logic:** Implements rules such as "always unlock on NFC tap," "dumb switch" mode, and timed momentary unlocks.
*   **Event Aggregation:** Listens for events from multiple sources that intend to change the lock's state.
*   **State Publication:** Notifies other system components (like `HardwareManager` and `HomeKitLock`) about state changes by publishing events.

## Public API

### LockManager()

Constructs a new `LockManager` instance. The constructor initializes the lock's state and registers all necessary event publishers and subscribers with the `espp::EventManager`. This setup allows it to listen for requests and broadcast state changes.

**Signature:**
```cpp
LockManager(const espConfig::misc_config_t& miscConfig, const espConfig::actions_config_t& actionsConfig);
```

**Parameters:**
*   `miscConfig`: A reference to the miscellaneous configuration struct, which contains settings like `lockAlwaysUnlock` and `lockAlwaysLock`.
*   `actionsConfig`: A reference to the actions configuration struct, which contains settings for momentary timeouts and GPIO behavior.

### begin()

Performs post-construction initialization. Its primary role is to publish the lock's initial state to the `lock/action` event channel. This ensures that all other components, particularly the `HardwareManager`, can synchronize with the lock's state as soon as the system starts.

**Signature:**
```cpp
void begin();
```

### getCurrentState()

Returns the current logical state of the lock.

**Signature:**
```cpp
int getCurrentState() const;
```

**Returns:**
*   `int`: The current state of the lock (e.g., `LockManager::LOCKED`, `LockManager::UNLOCKED`).

### getTargetState()

Returns the target state that the lock is attempting to achieve.

**Signature:**
```cpp
int getTargetState() const;
```

**Returns:**
*   `int`: The target state of the lock (e.g., `LockManager::LOCKED`, `LockManager::UNLOCKED`).

### setTargetState()

This is the primary method for requesting a change in the lock's state. It updates the internal target state and orchestrates the necessary events to drive the physical hardware and update listeners.

The method's logic includes:
*   Updating the internal target state.
*   Publishing a `lock/action` event to signal the `HardwareManager` to perform a physical action.
*   Publishing a `lock/stateChanged` event to notify all listeners (like `HomeKitLock`) of the new state.
*   If the request is to `UNLOCK` and the source is configured for momentary action, it starts a one-shot timer to automatically re-lock after a configured timeout.

**Signature:**
```cpp
void setTargetState(uint8_t state, Source source);
```

**Parameters:**
*   `state`: The desired target state (e.g., `LockManager::LOCKED`).
*   `source`: An enum value of type `Source` indicating the origin of the request (e.g., `Source::NFC`, `Source::HOMEKIT`). The source can affect behavior, such as whether a momentary unlock is triggered.

### overrideState()

Forcibly sets the lock's internal current and target states. This method is used when an external system (like an MQTT topic) reports the lock's state directly, bypassing the standard request flow. It immediately synchronizes the entire application to this new, authoritative state.

**Signature:**
```cpp
void overrideState(uint8_t c_state, uint8_t t_state);
```

**Parameters:**
*   `c_state`: The new current state to apply. Use `255` to leave the current state unchanged.
*   `t_state`: The new target state to apply. Use `255` to leave the target state unchanged.

## Internal Methods

### handleTimer()

A static callback function triggered when the momentary unlock timer expires. Its sole purpose is to call `setTargetState(LOCKED, INTERNAL)`, automatically re-locking the door.

**Signature:**
```cpp
void handleTimer(void* instance);
```
