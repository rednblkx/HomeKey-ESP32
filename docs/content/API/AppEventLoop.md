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
| `ETH_APP_EVENT` | Ethernet lifecycle | `ETH_STARTED`, `ETH_GOT_IP`, `ETH_LOST_IP`, `ETH_DISCONNECTED`, `ETH_STOPPED` |

> [!NOTE]
> The `HK_EVENT` base carries a single event ID (`HK_INTERNAL_EVENT`), but the payload is a `HomekitEvent` struct whose `type` field further distinguishes the sub-events: `SETUP_CODE_CHANGED`, `BTR_PROP_CHANGED`, `ACCESSDATA_CHANGED`, and `DEBUG_AUTH_FLOW`. Subscribers should deserialize the payload and switch on this type.

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
        std::span<const uint8_t> payload(data, size);
        std::error_code ec;
        EventLockState s = alpaca::deserialize<EventLockState>(payload, ec);
        if (ec) return; // handle deserialization failure
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

// Serialize the payload before publishing — all subscribers deserialize with alpaca
std::vector<uint8_t> d;
size_t d_len = alpaca::serialize(lockState, d);

esp_err_t err = AppEventLoop::publish(
    LOCK_EVENT,
    LOCK_STATE_CHANGED,
    d.data(),
    d_len
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

All event payloads are defined in `main/include/eventStructs.hpp` and serialized with [alpaca](https://github.com/p-ranav/alpaca) before publishing. The structs below reflect the current definitions.

### EventLockState

The payload for **all** lock events: `LOCK_EVENT` (`LOCK_STATE_CHANGED`, `LOCK_UPDATE_STATE`, `LOCK_TARGET_STATE_CHANGED`, and `LOCK_OVERRIDE_STATE`).

```cpp
struct EventLockState {
  uint8_t currentState = 255;
  uint8_t targetState = 255;
  uint8_t source = 0;
};
```

The `source` field carries a `LockManager::Source` value (`INTERNAL`, `HOMEKIT`, `NFC`, `MQTT`) describing the origin of the state change.

### EventHKTap

Published on `NFC_EVENT` (`NFC_TAP_EVENT`) for HomeKey authentications.

```cpp
struct EventHKTap {
  bool status;                        // true = successful authentication
  std::vector<uint8_t> issuerId;      // empty on failure
  std::vector<uint8_t> endpointId;    // empty on failure
  std::vector<uint8_t> readerId;      // the reader's sub-identifier
};
```

### EventTagTap

Published on `NFC_EVENT` (`NFC_TAP_EVENT`) for generic NFC tag scans.

```cpp
struct EventTagTap {
  std::vector<uint8_t> uid;
  std::array<uint8_t,2> atqa;
  uint8_t sak;
};
```

### EventValueChanged

Published on `HK_EVENT` (`HK_INTERNAL_EVENT`, wrapped in a `HomekitEvent`) and `HW_EVENT` (`HW_CONFIG_CHANGED`) when a configuration value changes.

```cpp
struct EventValueChanged {
  std::string name = "";
  uint8_t oldValue = 255;
  uint8_t newValue = 255;
  std::string str = "";
};
```

### HomekitEvent

The envelope published on `HK_EVENT` (`HK_INTERNAL_EVENT`). Its `type` field discriminates the sub-events; the `data` member holds the serialized payload (commonly an `EventValueChanged`).

```cpp
enum HomekitEventType : uint8_t {
    SETUP_CODE_CHANGED,
    BTR_PROP_CHANGED,
    ACCESSDATA_CHANGED,
    DEBUG_AUTH_FLOW
};

struct HomekitEvent {
    HomekitEventType type;
    std::vector<uint8_t> data;
};
```

### EventMqttStatus, EventNfcStatus, EventBinaryStatus

Declared in `eventStructs.hpp` (with the `MqttErrorCode` enum) but currently **unused** — MQTT status is polled via `MqttManager` getters and NFC status via `NfcManager::isConnected()` rather than delivered as events.

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
```

## Migration from event_bus

The `AppEventLoop` replaced the previous custom `event_bus` implementation (removed in this release). The key difference: string-based topics (`"lock/stateChanged"`) were replaced by typed event bases and IDs (`LOCK_EVENT`, `LOCK_STATE_CHANGED`), and event payloads are now alpaca-serialized structs posted to ESP-IDF's native, thread-safe event loop.

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

5. **Event Serialization:** Serialize complex payloads with alpaca (`alpaca::serialize` / `alpaca::deserialize`) before publishing and inside callbacks — this is the wire format every subscriber in the codebase expects.
