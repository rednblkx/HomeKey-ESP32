---
title: "HardwareManager"
---

**Description:** Manages all low-level hardware interactions (GPIO, NeoPixels, etc.). This class provides a high-level API for controlling physical hardware. It abstracts away pin numbers, active high/low logic, and the implementation details of providing user feedback (e.g., blinking an LED or lighting a NeoPixel).

### Public Methods:

*   **`HardwareManager(const espConfig::misc_config_t &)`**
    *   **Description:** Constructs the HardwareManager.
    *   **Parameters:**
        *   `miscConfig` (`const espConfig::misc_config_t &`): Reference to the application configuration manager.
*   **`void begin()`**
    *   **Description:** Initializes all hardware pins and starts background tasks for feedback.
*   **`void setLockOutput(int state)`**
    *   **Description:** Sets the physical output for the lock mechanism (e.g., relay or motor driver).
    *   **Parameters:**
        *   `state` (`int`): The desired lock state (e.g., lockStates::LOCKED).
*   **`void showSuccessFeedback()`**
    *   **Description:** Triggers a non-blocking visual/audible signal for a successful action.
*   **`void showFailureFeedback()`**
    *   **Description:** Triggers a non-blocking visual/audible signal for a failed action.
*   **`void triggerAltAction()`**
    *   **Description:** Triggers the alternative GPIO pin action.
