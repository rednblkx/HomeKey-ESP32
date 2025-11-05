---
title: "NfcManager"
---

## Overview

The `NfcManager` is a high-level class responsible for managing all Near Field Communication (NFC) interactions. It abstracts the low-level details of the PN532 NFC reader chip, providing a clean, event-driven interface to the rest of the application.

Its primary function is to continuously poll for NFC tags. When a tag is detected, it determines whether it is an Apple HomeKey-compatible device or a generic NFC tag. It then orchestrates the appropriate authentication or identification process and publishes the results as events for other components to consume. The manager is also responsible for the stability of the NFC reader, automatically handling disconnections and attempting to reconnect in the background.

## Key Responsibilities

*   **NFC Reader Management:** Initializes, configures, and maintains the connection to the PN532 NFC reader.
*   **Polling and Detection:** Runs a continuous background task to poll for nearby NFC tags.
*   **Tag Type Differentiation:** Identifies whether a detected tag is a HomeKey device or a generic tag.
*   **HomeKey Authentication:** Manages the entire HomeKey authentication flow by coordinating with the `HKAuthenticationContext`.
*   **Generic Tag Identification:** Reads the UID, ATQA, and SAK of non-HomeKey tags.
*   **Event Publishing:** Publishes detailed events about NFC interactions (`HOMEKEY_TAP` or `TAG_TAP`) to the application's event bus.
*   **Resilience:** Automatically detects if the PN532 reader becomes unresponsive and starts a background task to re-establish the connection.

## Public API

### NfcManager()

Constructs a new `NfcManager` instance. The constructor initializes internal state, including the ECP (Express Card Profile) data used for HomeKey discovery, and registers subscribers for internal events that can modify its behavior, such as changing the authentication flow for debugging purposes.

**Signature:**
```cpp
NfcManager(ReaderDataManager& readerDataManager, const std::array<uint8_t, 4>& nfcGpioPins);
```

**Parameters:**
*   `readerDataManager`: A reference to the `ReaderDataManager`, which provides the necessary reader data (like the Reader GID) for HomeKey operations.
*   `nfcGpioPins`: An array of four GPIO pin numbers required for the SPI communication with the PN532 chip.

### begin()

Initializes the PN532 hardware driver and starts the main NFC polling task. This method must be called after the constructor to begin NFC operations.

**Signature:**
```cpp
bool begin();
```

**Returns:**
*   `bool`: `true` if the polling task was successfully created and started, `false` otherwise.

## Internal Workings

The `NfcManager` operates primarily through a set of FreeRTOS tasks that run in the background.

### Polling Task (`pollingTask`)

This is the main task of the `NfcManager`. It runs in an infinite loop with the following logic:
1.  **Initialize Reader:** Attempts to initialize the PN532. If it fails, it starts the `retryTask` and suspends itself.
2.  **Health Check:** Periodically checks if the PN532 is still responsive. If not, it starts the `retryTask` and suspends itself.
3.  **Poll for Tags:** Actively listens for a passive ISO14443A tag to enter the reader's field.
4.  **Handle Presence:** If a tag is found, it calls `handleTagPresence()` to process it.
5.  **Wait for Removal:** After processing, it calls `waitForTagRemoval()` to ensure the tag has left the field before polling again.

### Retry Task (`retryTask`)

This task is created when the `pollingTask` detects that the PN532 is unresponsive. It runs in a loop, repeatedly trying to re-initialize the reader. Once successful, it resumes the main `pollingTask` and deletes itself. This makes the NFC functionality resilient to hardware glitches.

### Tag Handling

*   **`handleTagPresence`**: This is the entry point for processing a detected tag. It first tries to select the HomeKey applet on the tag.
    *   If successful, it proceeds to `handleHomeKeyAuth()`.
    *   If it fails, it treats the tag as a generic one and calls `handleGenericTag()`.

*   **`handleHomeKeyAuth`**: This method orchestrates the complex HomeKey authentication process using the `HKAuthenticationContext`. It provides a lambda function for the context to use for sending and receiving data (APDUs) with the tag. Upon completion, it publishes a `HOMEKEY_TAP` event with the outcome (success or failure) and relevant identifiers.

*   **`handleGenericTag`**: This method is called for non-HomeKey tags. It reads the tag's unique identifiers (UID, ATQA, SAK) and publishes them in a `TAG_TAP` event.

*   **`waitForTagRemoval`**: After a tag is handled, this method ensures that the tag is no longer in the reader's field before the manager resumes polling. This prevents the same tag from being processed multiple times. If the tag is not removed within a timeout, it will reset the reader's RF field to clear its state.
