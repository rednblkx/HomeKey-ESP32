---
title: "HomeKitLock"
---

**Description:** Implements the HomeKit lock service and manages HomeKit-related functionalities.

### Public Methods:

*   **`HomeKitLock(std::function<void(int)> &conn_cb, LockManager& lockManager, ConfigManager& configManager, ReaderDataManager& readerDataManager)`**
    *   **Description:** Constructs the HomeKitLock manager.
    *   **Parameters:**
        *   `conn_cb` (`std::function<void(int)> &`): Callback for connection status.
        *   `lockManager` (`LockManager&`): Reference to the LockManager.
        *   `configManager` (`ConfigManager&`): Reference to the ConfigManager.
        *   `readerDataManager` (`ReaderDataManager&`): Reference to the ReaderDataManager.
*   **`void begin()`**
    *   **Description:** Initializes the HomeKit lock service.
*   **`void updateLockState(int currentState, int targetState)`**
    *   **Description:** Updates the HomeKit lock state.
    *   **Parameters:**
        *   `currentState` (`int`): The current physical state of the lock.
        *   `targetState` (`int`): The intended state of the lock.
*   **`void updateBatteryStatus(uint8_t batteryLevel, bool isLow)`**
    *   **Description:** Updates the HomeKit battery status.
    *   **Parameters:**
        *   `batteryLevel` (`uint8_t`): The current battery level (0-100).
        *   `isLow` (`bool`): `true` if the battery is low, `false` otherwise.

---
