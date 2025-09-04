---
title: "MqttManager"
---

**Description:** Manages all MQTT client operations, including connection, discovery, and communication. This class handles the full lifecycle of the MQTT connection. It publishes device state, reports events like NFC taps, and listens for commands from the MQTT broker to control the lock.

### Public Methods:

*   **`MqttManager(ConfigManager& configManager)`**
    *   **Description:** Constructs the MqttManager.
    *   **Parameters:**
        *   `configManager` (`ConfigManager&`): Reference to the application configuration manager.
*   **`bool begin(std::string deviceID)`**
    *   **Description:** Initializes and starts the MQTT client.
    *   **Parameters:**
        *   `deviceID` (`std::string`): The unique ID of the device.
    *   **Returns:** `bool` - `true` if the client was started, `false` if the configuration was invalid.
*   **`void publishLockState(int currentState, int targetState)`**
    *   **Description:** Publishes the current state of the lock. Called by the main application loop when a state change is detected.
    *   **Parameters:**
        *   `currentState` (`int`): The current physical state of the lock.
        *   `targetState` (`int`): The intended state of the lock.
*   **`void publishHomeKeyTap(const std::vector<uint8_t>& issuerId, const std::vector<uint8_t>& endpointId, const std::vector<uint8_t>& readerId)`**
    *   **Description:** Publishes a successful HomeKey authentication event.
    *   **Parameters:**
        *   `issuerId` (`const std::vector<uint8_t>&`): The ID of the authenticated issuer.
        *   `endpointId` (`const std::vector<uint8_t>&`): The ID of the authenticated endpoint.
        *   `readerId` (`const std::vector<uint8_t>&`): The ID of this reader device.
*   **`void publishUidTap(const std::vector<uint8_t>& uid, const std::vector<uint8_t> &atqa, const std::vector<uint8_t> &sak)`**
    *   **Description:** Publishes a generic (non-HomeKey) NFC tag scan event.
    *   **Parameters:**
        *   `uid` (`const std::vector<uint8_t>&`): Pointer to the UID byte array.
        *   `uidLen` (`uint8_t`): Length of the UID.
        *   `atqa` (`const std::vector<uint8_t>&`): Pointer to the ATQA byte array (can be null).
        *   `sak` (`const std::vector<uint8_t>&`): Pointer to the SAK byte array (can be null).

---
