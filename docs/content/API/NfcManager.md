---
title: "NfcManager"
---

## Overview

The `NfcManager` is a high-level class responsible for managing all Near Field Communication (NFC) interactions. It abstracts the low-level details of the NFC reader chip, providing a clean, event-driven interface to the rest of the application.

Its primary function is to continuously poll for NFC tags. When a tag is detected, it determines whether it is an Apple HomeKey-compatible device or a generic NFC tag. It then orchestrates the appropriate authentication or identification process and publishes the results as events for other components to consume. The manager is also responsible for the stability of the NFC reader, automatically handling disconnections and attempting to reconnect in the background.

## Key Responsibilities

*   **NFC Reader Management:** Initializes, configures, and maintains the connection to the NFC reader.
*   **Polling and Detection:** Runs a continuous background task to poll for nearby NFC tags.
*   **Tag Type Differentiation:** Identifies whether a detected tag is a HomeKey device or a generic tag.
*   **HomeKey Authentication:** Manages the entire HomeKey authentication flow by coordinating with the DigitalDoorKey (DDK) library's `ddk::Session` and `ddk::Profile` types.
*   **Generic Tag Identification:** Reads the UID, ATQA, and SAK of non-HomeKey tags.
*   **Event Publishing:** Publishes detailed events about NFC interactions (`NFC_TAP_EVENT`) via `AppEventLoop` to the application's event system.
*   **Resilience:** Automatically detects if the NFC reader becomes unresponsive and starts a background task to re-establish the connection.

## Public API

### NfcManager()

Constructs a new `NfcManager` instance. The constructor initializes internal state, including the ECP (Enhanced Contactless Polling) data used for HomeKey discovery, and registers subscribers for internal events that can modify its behavior.

**Signature:**
```cpp
NfcManager(NvsCredentialStore& readerDataManager,
           const std::array<uint8_t, 4>& nfcGpioPins,
           uint8_t nfcReaderType,
           uint8_t nfcIrqPin,
           uint8_t nfcVenPin,
           bool hkAuthPrecomputeEnabled,
           bool nfcFastPollingEnabled);
```

**Parameters:**
*   `readerDataManager`: A reference to the `NvsCredentialStore`, which provides the necessary reader data (like the Reader GID) for HomeKey operations.
*   `nfcGpioPins`: An array of four GPIO pin numbers. For SPI readers (PN532, PN7161) these are SS/SCK/MISO/MOSI; for the I2C reader (ST25R3916) only entries 0 and 1 are used, as SDA and SCL.
*   `nfcReaderType`: The reader backend to instantiate: `0` = PN532 (SPI), `1` = PN7160/PN7161 (SPI), `2` = ST25R3916 (I2C).
*   `nfcIrqPin`: The IRQ pin for the PN7161 (255 = unset; required for reader type 1).
*   `nfcVenPin`: The VEN (enable/reset) pin for the PN7161 (255 = unset; required for reader type 1).
*   `hkAuthPrecomputeEnabled`: Whether to enable authentication precomputation for faster response times.
*   `nfcFastPollingEnabled`: Whether to use a reduced delay between poll cycles for faster tag detection.

### begin()

Initializes the NFC hardware driver and starts the main NFC polling task. This method must be called after the constructor to begin NFC operations.

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
1.  **Initialize Reader:** Attempts to initialize the NFC Module. If it fails, it retries every second until successful.
2.  **Precompute Session:** If authentication precompute is enabled and the reader data is provisioned, builds (or rebuilds, when the data generation changed) a cached authentication session for faster taps.
3.  **Health Check:** Periodically checks if the NFC Module is still responsive. If not, it reconnects inline — repeatedly calling `initializeReader()` with a 5-second delay between attempts — before resuming polling.
4.  **Poll for Tags:** Actively listens for a passive ISO14443A tag to enter the reader's field.
5.  **Handle Presence:** If a tag is found, it calls `handleTagPresence()` to process it.
6.  **Wait for Removal:** After processing, it calls `waitForTagRemoval()` to ensure the tag has left the field before polling again.

### Reader Reconnection

There is no separate retry task: reconnection happens inline within the polling loop. When the health check fails, the task logs the failure and loops on `initializeReader()` (5 seconds between attempts) until the reader responds, then continues polling. This makes the NFC functionality resilient to hardware glitches without the overhead of a dedicated task.

### Tag Handling

*   **`handleTagPresence`**: This is the entry point for processing a detected tag. It first tries to select the HomeKey applet on the tag.
    *   If successful, it proceeds to `handleHomeKeyAuth()`.
    *   If it fails, it treats the tag as a generic one and calls `handleGenericTag()`.

*   **`handleHomeKeyAuth`**: This method orchestrates the complex HomeKey authentication process using the DigitalDoorKey (DDK) library — it builds a `ddk::Session` (wrapping a `ddk::NfcChannel` backed by the reader's APDU exchange) and drives a `ddk::Profile` state machine through `validate_select` → `step` → `finalize`. Upon completion, it publishes a `NFC_TAP_EVENT` via `AppEventLoop` with the outcome (success or failure) and relevant identifiers in an `EventHKTap` structure (which carries the issuer, endpoint, and reader sub-identifier).

*   **`handleGenericTag`**: This method is called for non-HomeKey tags. It reads the tag's unique identifiers (UID, ATQA, SAK) and publishes them in a `NFC_TAP_EVENT` via `AppEventLoop` with an `EventTagTap` structure.

*   **`waitForTagRemoval`**: After a tag is handled, this method ensures that the tag is no longer in the reader's field before the manager resumes polling. This prevents the same tag from being processed multiple times. If the tag is not removed within a timeout, it will reset the reader's RF field to clear its state.
