---
title: "LockManager"
---

**Description:** Manages the state and core logic of the lock mechanism. This class is the single source of truth for the lock's state (Locked, Unlocked, etc.). It receives commands from various sources (HomeKit, MQTT, NFC) and orchestrates the necessary actions, including controlling the physical hardware and managing timers.

### Public Methods:

*   **`LockManager(HardwareManager& hardwareManager, const espConfig::misc_config_t& miscConfig)`**
    *   **Description:** Constructs the LockManager.
    *   **Parameters:**
        *   `hardwareManager` (`HardwareManager&`): Reference to the manager that controls physical GPIOs.
        *   `miscConfig` (`const espConfig::misc_config_t&`): Reference to the application configuration manager.
*   **`void begin()`**
    *   **Description:** Initializes the lock state to its default.
*   **`int getCurrentState() const`**
    *   **Description:** Gets the current physical state of the lock.
    *   **Returns:** `int` - The current state (e.g., `lockStates::LOCKED`).
*   **`int getTargetState() const`**
    *   **Description:** Gets the intended target state of the lock.
    *   **Returns:** `int` - The target state (e.g., `lockStates::LOCKED`).
*   **`void setTargetState(uint8_t state, Source source)`**
    *   **Description:** Sets the intended target state of the lock, triggering a physical action.
    *   **Parameters:**
        *   `state` (`uint8_t`): The desired state (e.g., `lockStates::LOCKED`).
        *   `source` (`Source`): The origin of the command.
*   **`void overrideState(uint8_t cstate, uint8_t tstate)`**
    *   **Description:** Forces the manager's internal state to match an external report. This method updates the state WITHOUT triggering a physical action. Useful for syncing with external state changes reported via MQTT.
    *   **Parameters:**
        *   `cstate` (`uint8_t`): The new current state.
        *   `tstate` (`uint8_t`): The new target state.

---
