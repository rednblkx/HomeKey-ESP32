---
title: "HardwareManager"
---

## 1. Overview

The `HardwareManager` class serves as the primary interface between the application's logic and the physical hardware components of the device. It is responsible for managing GPIO pins, controlling visual feedback (like NeoPixels), handling physical lock outputs, and managing timed actions.

The class operates on an event-driven, asynchronous model. It subscribes to high-level application events (e.g., "lock the door," "NFC tag tapped") and translates them into low-level hardware actions (e.g., setting a GPIO pin high, flashing an LED). It utilizes FreeRTOS tasks and queues to handle these actions without blocking the main application flow.

### Key Responsibilities:

*   **Lock Control:** Manages the GPIO pin(s) that physically control a lock mechanism.
*   **User Feedback:** Provides visual feedback for success and failure events using NeoPixels and dedicated GPIO pins.
*   **Alternate Action:** Implements a special "alternate action" feature, which can be armed by a physical button/input and then triggered by another event (like a HomeKey tap).
*   **Dynamic Pin Configuration:** Listens for events that indicate GPIO pin assignments have changed and reconfigures the hardware accordingly.

### Architecture:

*   **Event-Driven:** Uses `espp::EventManager` to subscribe to commands and publish state changes.
*   **Asynchronous Tasks:** Offloads hardware operations to dedicated FreeRTOS tasks to prevent blocking:
    *   A **lock control task** processes requests to change the lock's state.
    *   A **feedback task** manages timed visual indicators (LEDs, NeoPixels).
    *   An **initiator task** handles the arming mechanism for the alternate action.
*   **ESP-Timers:** Uses one-shot timers to control the duration of feedback signals and timeouts.

## 2. Public API

### Constructor

#### `HardwareManager()`

Constructs a new `HardwareManager` instance. The constructor initializes its internal configuration and sets up all necessary event subscribers and publishers.

*   **Subscribes to:**
    *   `lock/action`: To receive commands to change the lock state.
    *   `nfc/event`: To trigger success/failure feedback or the alternate action based on NFC events.
    *   `hardware/gpioPinChanged`: To dynamically update GPIO configurations.
*   **Publishes:**
    *   `lock/updateState`: To notify the system of a change in the physical lock state.
    *   `lock/altAction`: To signal that the alternate action has been triggered.

**Signature:**
```cpp
HardwareManager(const espConfig::actions_config_t& miscConfig);
```

**Parameters:**
*   `miscConfig`: A constant reference to an `espConfig::actions_config_t` struct containing all necessary pin numbers, timings, and color configurations.

### Initialization

#### `begin()`

Initializes all hardware resources and starts the background tasks. This method must be called after the constructor and before any other methods.

**It performs the following actions:**
*   Configures GPIO pins for feedback (success/fail LEDs), the lock action, and the alternate action mechanism.
*   Initializes the NeoPixel driver if a valid pin is configured.
*   Sets up ESP-IDF timers for all timed hardware events.
*   Creates the FreeRTOS queues and tasks for lock control, feedback, and the alternate action initiator.
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
*   `state`: The desired lock state. This should correspond to values defined in `LockManager`, such as `LockManager::LOCKED` or `LockManager::UNLOCKED`.

#### `showSuccessFeedback()`

Triggers the predefined hardware feedback sequence for a successful operation. This is an asynchronous call that queues the request for the `feedbackTask`.

**Signature:**
```cpp
void showSuccessFeedback();
```

#### `showFailureFeedback()`

Triggers the predefined hardware feedback sequence for a failed operation. This is an asynchronous call that queues the request for the `feedbackTask`.

**Signature:**
```cpp
void showFailureFeedback();
```

## 3. Internal Workings & Task Descriptions

The following components describe the internal, asynchronous behavior of the `HardwareManager`. They are not called directly but are essential to understanding its operation.

#### Lock Control Task (`lockControlTask`)

This task runs in an infinite loop, waiting for state change commands on its queue (sent via `setLockOutput`).

*   When a state (e.g., `LOCKED`) is received, it drives the `gpioActionPin` to the corresponding configured level (`gpioActionLockState` or `gpioActionUnlockState`).
*   After changing the pin state, it publishes a `lock/updateState` event to inform the rest of the system that the physical state has been updated.

#### Feedback Task (`feedbackTask`)

This task waits for feedback requests on its queue (sent by `showSuccessFeedback` or `showFailureFeedback`).

*   **On `SUCCESS`:**
    *   It illuminates the NeoPixel with the configured `neopixelSuccessColor`.
    *   It activates the `nfcSuccessPin`.
    *   It starts one-shot timers to automatically turn off the NeoPixel and deactivate the success pin after the configured durations (`neopixelSuccessTime`, `nfcSuccessTime`).
*   **On `FAILURE`:**
    *   It performs the same sequence using the failure-specific colors, pins, and durations.

#### Alternate Action Mechanism

This feature allows a secondary action to be triggered under specific conditions, typically "arming" via a button press and "triggering" via an NFC HomeKey tap.

*   **`initiator_isr_handler` (ISR):**
    *   Attached to the `hkAltActionInitPin`. When this pin is triggered (e.g., by a button press), this lightweight ISR immediately sends a message to the `initiator_task`'s queue and returns.

*   **`initiator_task`:**
    *   This task waits for a message from the ISR.
    *   Upon receiving a message, it "arms" the alternate action by setting a flag (`m_altActionArmed = true`).
    *   It may light up an indicator LED (`hkAltActionInitLedPin`).
    *   It starts a timer (`m_altActionInitTimer`). When this timer expires, the armed state is automatically cleared.

*   **`triggerAltAction()` (Internal Method):**
    *   This method is called internally, for example, when a successful HomeKey tap event is received.
    *   It checks if the alternate action is currently armed.
    *   If armed, it publishes the `lock/altAction` event and triggers the physical output on `hkAltActionPin` for the duration specified by `hkAltActionTimeout`.

#### Timer Callback (`handleTimer`)

This is a single, static callback function used by all ESP-timers created by the manager.

*   It receives a context argument that identifies which timer fired (e.g., `GPIO_S` for the success GPIO timer, `PIXEL_F` for the failure pixel timer).
*   Based on the source, it performs the corresponding "off" action, such as deactivating a GPIO pin or turning off the NeoPixel.
