---
title: "NfcManager"
---

**Description:** Manages NFC communication with the PN532 module.

### Public Methods:

*   **`NfcManager(ReaderDataManager& readerDataManager, const std::array<uint8_t, 4> &nfcGpioPins)`**
    *   **Description:** Constructs the NfcManager.
    *   **Parameters:**
        *   `readerDataManager` (`ReaderDataManager&`): Reference to the ReaderDataManager.
        *   `nfcGpioPins` (`const std::array<uint8_t, 4> &`): Array of GPIO pins used for NFC.
*   **`bool begin()`**
    *   **Description:** Initializes the NFC module and starts polling for tags.
    *   **Returns:** `bool` - `true` on success, `false` otherwise.
*   **`void updateEcpData()`**
    *   **Description:** Updates the ECP data.

---
