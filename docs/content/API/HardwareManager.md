---
title: "HardwareManager"
---

## 1. Overview

The `HardwareManager` class serves as the primary interface between the application's logic and the physical hardware components of the device. It is responsible for managing GPIO pins, controlling visual feedback (like NeoPixels), handling physical lock outputs, and managing timed actions.

The class operates on an event-driven, asynchronous model. It subscribes to high-level application events (e.g., "lock the door," "NFC tag tapped") and translates them into low-level hardware actions (e.g., setting a GPIO pin high, flashing an LED). It utilizes FreeRTOS tasks and queues to handle these actions without blocking the main application flow.

#### Key System Features & Improvements

*   **GPIO Allocator & Pin Safety (`GpioAllocator`):** Centralized, thread-safe GPIO allocation tracking system. Pin allocations across all hardware modules (NFC readers, Ethernet PHYs, Relays, Status LEDs, Buzzers, and HomeSpan pins) are leased via RAII `GpioLease` instances.
    *   **Target-Specific Strapping Pin Protection:** Validates requested GPIO pins against chip-specific boot strapping pin lists (ESP32, ESP32-S3, ESP32-C3, ESP32-C6).
    *   **SPI Bus Intersection Checks:** Validates SPI bus sharing between NFC controllers and SPI Ethernet modules, preventing pin ownership conflicts.
    *   **Strapping Override Option:** Supports `overrideStrappingRestriction` for custom hardware designs.
*   **Memory Safety Improvements:** `HardwareManager` instances are managed using `std::unique_ptr` in `main.cpp`, ensuring clean object lifecycles.
*   **Timer Reliability:** Hardware timers in `HardwareManager` use non-static member contexts and initialization checks, ensuring reliability across device re-initialization.

### Key Responsibilities:

*   **Lock Control:** Manages the GPIO pin(s) that physically control a lock mechanism.
*   **User Feedback:** Provides visual feedback for success and failure events using NeoPixels and dedicated GPIO pins.
*   **Alternate Action:** Implements a special "alternate action" feature, which can be armed by a physical button/input and then triggered by another event (like a HomeKey tap).
*   **Dynamic Pin Configuration:** Listens for events that indicate GPIO pin assignments have changed and reconfigures the hardware accordingly.

### Architecture:

*   **Event-Driven:** Uses `AppEventLoop` system to subscribe to commands and publish state changes.
*   **Asynchronous Tasks:** Offloads hardware operations to dedicated FreeRTOS tasks to prevent blocking:
    *   A **lock control task** processes requests to change the lock's state.
    *   A **feedback task** manages timed visual indicators (LEDs, NeoPixels).
    *   An **initiator task** handles the arming mechanism for the alternate action.
*   **ESP-Timers:** Uses one-shot timers to control the duration of feedback signals and timeouts.

---

## 2. Public API & GpioAllocator

### `GpioAllocator` Subsystem

The `GpioAllocator` namespace provides thread-safe GPIO allocation tracking:

```cpp
// Example: Acquire a lease for a relay output pin
auto lease = GpioAllocator::acquire(gpio_num, Owner::LOCK_ACTION);
if (!lease.isValid()) {
    // Pin conflict or invalid pin assignment
}
```

*   `acquire(int gpio_num, Owner owner)`: Acquires a GPIO pin lease. Validates against target strapping pins and active leases. Returns a `GpioLease`.
*   `release(int gpio_num, Owner owner)`: Explicitly releases an acquired pin lease.
*   `checkStrappingPins(int gpio_num)`: Returns `true` if the requested pin is a boot strapping pin for the target ESP32 chip family.

### Constructor

#### `HardwareManager()`

Constructs a new `HardwareManager` instance. The constructor initializes its internal configuration and sets up all necessary event subscribers and publishers.

*   **Subscribes to (via AppEventLoop):**
    *   `HW_EVENT` (`HW_ACTION`): To receive commands to change the lock state.
    *   `NFC_EVENT` (`NFC_TAP_EVENT`): To trigger success/failure feedback or the alternate action based on NFC events.
    *   `HW_EVENT` (`HW_CONFIG_CHANGED`): To dynamically update GPIO configurations.
*   **Publishes (via AppEventLoop):**
    *   `LOCK_EVENT` (`LOCK_UPDATE_STATE`): To notify the system of a change in the physical lock state.
    *   `HW_EVENT` (`HW_ALT_ACTION`): To signal that the alternate action has been triggered.

**Signature:**
```cpp
HardwareManager(const espConfig::actions_config_t& miscConfig);
```

**Parameters:**
*   `miscConfig`: A constant reference to an `espConfig::actions_config_t` struct containing all necessary pin numbers, timings, and color configurations.

### Initialization

#### `begin()`

Initializes all hardware resources via `GpioAllocator` and starts background tasks. This method must be called after the constructor and before any other methods.

**It performs the following actions:**
*   Configures GPIO pins for feedback (success/fail LEDs), the lock action, and the alternate action mechanism.
*   Initializes the NeoPixel driver if a valid pin is configured.
*   Sets up ESP-IDF timers for all timed hardware events.
*   Creates FreeRTOS queues and tasks for lock control, feedback, and the alternate action initiator.
*   Installs an ISR (Interrupt Service Routine) if the alternate action initiator pin is configured.

**Signature:**
```cpp
void begin();
```

### Public Methods

#### `setLockOutput()`

Asynchronously sends a command to change the state of the physical lock. The command is placed on a queue and processed by the internal `lockControlTask`.

**Signature:**
```cpp
void setLockOutput(int state);
```

**Parameters:**
*   `state`: The desired lock state (`LockManager::LOCKED` or `LockManager::UNLOCKED`).

#### `showSuccessFeedback()`

Triggers the predefined hardware feedback sequence for a successful operation. Queues the request for `feedbackTask`.

**Signature:**
```cpp
void showSuccessFeedback();
```

#### `showFailureFeedback()`

Triggers the predefined hardware feedback sequence for a failed operation. Queues the request for `feedbackTask`.

**Signature:**
```cpp
void showFailureFeedback();
```

---

## 3. Internal Workings & Task Descriptions

#### Lock Control Task (`lockControlTask`)

This task runs in an infinite loop, waiting for state change commands on its queue (sent via `setLockOutput`).

*   When a state (e.g., `LOCKED`) is received, it drives the `gpioActionPin` to the corresponding configured level (`gpioActionLockState` or `gpioActionUnlockState`).
*   After changing the pin state, it publishes a `LOCK_UPDATE_STATE` event via `AppEventLoop` to inform the rest of the system that the physical state has been updated.

#### Feedback Task (`feedbackTask`)

This task waits for feedback requests on its queue (sent by `showSuccessFeedback` or `showFailureFeedback`).

*   **On `SUCCESS`:**
    *   Illuminates the NeoPixel with the configured `neopixelSuccessColor`.
    *   Activates the `nfcSuccessPin`.
    *   Starts one-shot timers to automatically turn off the NeoPixel and deactivate the success pin after configured durations (`neopixelSuccessTime`, `nfcSuccessTime`).
*   **On `FAILURE`:**
    *   Performs the same sequence using failure-specific colors, pins, and durations.

#### Alternate Action Mechanism

This feature allows a secondary action to be triggered under specific conditions, typically "arming" via a button press and "triggering" via an NFC HomeKey tap.

*   **`initiator_isr_handler` (ISR):** Attached to `hkAltActionInitPin`. Sends a message to `initiator_task`'s queue upon button press.
*   **`initiator_task`:** Arms the alternate action (`m_altActionArmed = true`), illuminates indicator LED (`hkAltActionInitLedPin`), and starts expiration timer `m_altActionInitTimer`.
*   **`triggerAltAction()` (Internal Method):** If armed when a HomeKey tap occurs, publishes `HW_ALT_ACTION` event and triggers physical output on `hkAltActionPin` for `hkAltActionTimeout`.

#### Timer Callback (`handleTimer`)

A single callback used by all ESP-timers created by the manager to deactivate GPIO pins or turn off NeoPixels upon timer expiration.

