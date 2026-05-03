---
title: "AppEventLoop"
---

## Overview

The `AppEventLoop` namespace provides a modern, type-safe C++ wrapper around ESP-IDF's native event loop system. It replaces the previous custom `event_bus` implementation with a more robust and efficient solution that leverages ESP-IDF's built-in event loop capabilities.

This system enables decoupled, asynchronous communication between different components of the application through a publish-subscribe pattern.

## Key Responsibilities

*   **Event Subscription:** Allows components to subscribe to specific event bases and event IDs with type-safe callbacks
*   **Event Publishing:** Provides a mechanism to broadcast events to all interested subscribers
*   **Resource Management:** Automatic cleanup of event handlers through RAII-style `SubscriptionHandle`
*   **Thread Safety:** Built on ESP-IDF's thread-safe event loop implementation

## Architecture

The event system defines several event bases, each with its own set of event IDs:

### Event Bases

| Event Base | Description | Event IDs |
|------------|-------------|-----------|
| `LOCK_EVENT` | Lock state changes | `LOCK_STATE_CHANGED`, `LOCK_UPDATE_STATE`, `LOCK_TARGET_STATE_CHANGED`, `LOCK_OVERRIDE_STATE` |
| `NFC_EVENT` | NFC/RFID events | `NFC_TAP_EVENT`, `NFC_STATUS_CHANGED` |
| `HK_EVENT` | HomeKit internal events | `HK_INTERNAL_EVENT` |
| `HW_EVENT` | Hardware actions | `HW_ACTION`, `HW_ALT_ACTION`, `HW_CONFIG_CHANGED` |
| `MQTT_EVENT` | MQTT connection status | `MQTT_STATUS_CHANGED` |

## Public API

### subscribe()

Subscribes to events matching the specified event base and event ID. Returns a `SubscriptionHandle` that automatically unregisters the handler when destroyed.

**Signature:**
```cpp
SubscriptionHandle subscribe(esp_event_base_t base, int32_t id,
                              std::function<void(const uint8_t*, size_t)> callback);
```

**Parameters:**
*   `base`: The event base to subscribe to (e.g., `LOCK_EVENT`, `NFC_EVENT`)
*   `id`: The specific event ID, or `ESP_EVENT_ANY_ID` to receive all events for the base
*   `callback`: A function to be called when matching events are published. Receives the event data as a byte pointer and size

**Returns:**
*   `SubscriptionHandle`: An RAII handle that manages the subscription lifetime

**Example:**
```cpp
#include "app_event_loop.hpp"
#include "app_events.hpp"
#include "eventStructs.hpp"

// Subscribe to lock state changes
auto subscription = AppEventLoop::subscribe(
    LOCK_EVENT,
    LOCK_STATE_CHANGED,
    [](const uint8_t* data, size_t size) {
        EventLockState event;
        // Deserialize data into event struct
        // Handle the lock state change
    }
);
```

### publish()

Publishes an event to the event loop, notifying all matching subscribers.

**Signature:**
```cpp
esp_err_t publish(esp_event_base_t base, int32_t id, const void* data, size_t size);
```

**Parameters:**
*   `base`: The event base for the event
*   `id`: The specific event ID
*   `data`: Pointer to the event data to publish
*   `size`: Size of the event data in bytes

**Returns:**
*   `ESP_OK` on success, or an error code on failure

**Example:**
```cpp
EventLockState lockState;
lockState.currentState = LOCKED;
lockState.targetState = LOCKED;

esp_err_t err = AppEventLoop::publish(
    LOCK_EVENT,
    LOCK_STATE_CHANGED,
    &lockState,
    sizeof(lockState)
);
```

## SubscriptionHandle

A RAII wrapper around an ESP-IDF event handler instance. Automatically unregisters the handler when the handle is destroyed or reassigned.

### Public Methods

#### is_valid()

Checks if the handle holds a valid subscription.

```cpp
bool is_valid() const;
```

#### reset()

Manually unregisters the event handler and invalidates the handle.

```cpp
void reset();
```

## Data Structures

### EventLockState

Published on `LOCK_EVENT` (`LOCK_STATE_CHANGED`).

```cpp
struct EventLockState {
    uint8_t currentState = 255;
    uint8_t targetState = 255;
    uint8_t source;
};
```

### EventLockTargetState

Published on `LOCK_EVENT` (`LOCK_TARGET_STATE_CHANGED`).

```cpp
struct EventLockTargetState {
    uint8_t state;
    uint8_t source;
};
```

### EventLockOverrideState

Published on `LOCK_EVENT` (`LOCK_OVERRIDE_STATE`).

```cpp
struct EventLockOverrideState {
    uint8_t currentState;
    uint8_t targetState;
};
```

### EventHomeKeyTap

Published on `NFC_EVENT` (`NFC_TAP_EVENT`) for HomeKey authentications.

```cpp
struct EventHomeKeyTap {
    std::string issuerId;
    std::string endpointId;
};
```

### EventUidTap

Published on `NFC_EVENT` (`NFC_TAP_EVENT`) for generic NFC tag scans.

```cpp
struct EventUidTap {
    std::string uid;
    std::string atqa;
    std::string sak;
};
```

### EventMqttStatus

Published on `MQTT_EVENT` (`MQTT_STATUS_CHANGED`).

```cpp
enum class MqttErrorCode : uint8_t {
    NONE = 0,
    CONNECTION_REFUSED = 1,
    AUTH_FAILED = 2,
    NETWORK_ERROR = 3,
    SSL_ERROR = 4,
    TIMEOUT = 5,
    UNKNOWN = 255
};

struct EventMqttStatus {
    bool connected;
    MqttErrorCode errorCode;
    std::string errorMessage;
};
```

### EventNfcStatus

Published on `NFC_EVENT` (`NFC_STATUS_CHANGED`).

```cpp
struct EventNfcStatus {
    bool connected;
    uint8_t firmwareVersionMajor;
    uint8_t firmwareVersionMinor;
};
```

## Migration from event_bus

The `AppEventLoop` replaces the previous custom `event_bus` implementation. Key differences:

| Aspect | Old (event_bus) | New (AppEventLoop) |
|--------|-----------------|-------------------|
| Topic strings | String-based topics (`"lock/stateChanged"`) | Typed event bases and IDs (`LOCK_EVENT`, `LOCK_STATE_CHANGED`) |
| Subscription | `EventBus::Bus::instance().subscribe()` | `AppEventLoop::subscribe()` |
| Publishing | `EventBus::Bus::instance().publish()` | `AppEventLoop::publish()` |
| Event data | `std::vector<uint8_t>` serialization | Raw pointers with size |
| Thread safety | Custom implementation | ESP-IDF native |

## Internal Workings

### Event Loop Initialization

The event loop is initialized during application startup by calling `esp_event_loop_create_default()`. All event bases are registered using `ESP_EVENT_DECLARE_BASE` and `ESP_EVENT_DEFINE_BASE` macros.

### Handler Registration

When `subscribe()` is called:
1. The callback is wrapped in an ESP-IDF compatible handler
2. `esp_event_handler_instance_register()` registers the handler with the event loop
3. A `SubscriptionHandle` is returned containing the handler instance

### Event Dispatching

When `publish()` is called:
1. `esp_event_post()` adds the event to the event loop's queue
2. The event loop task dispatches the event to matching handlers
3. Handler callbacks are invoked with the event data

### Cleanup

When a `SubscriptionHandle` is destroyed or `reset()` is called:
1. `esp_event_handler_instance_unregister()` removes the handler
2. The handler instance is invalidated

## Best Practices

1. **Store SubscriptionHandles:** Keep the `SubscriptionHandle` returned by `subscribe()` as long as you want to receive events. When the handle is destroyed, the subscription is automatically cancelled.

2. **Use Structured Event Data:** Define C++ structs for event data to ensure type safety and consistent serialization/deserialization.

3. **Minimize Work in Callbacks:** Event callbacks run on the event loop task. Keep processing minimal to avoid blocking other events.

4. **Check is_valid():** Before manually calling `reset()` on a handle, check `is_valid()` to avoid unnecessary operations.

5. **Event Serialization:** Use consistent serialization methods (e.g., MessagePack, Protocol Buffers) when passing complex data structures through events.
