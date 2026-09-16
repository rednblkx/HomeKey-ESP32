---
title: "ReaderDataManager"
---

## Overview

The reader data storage component is implemented by the `NvsCredentialStore` class (header: `main/include/ReaderDataManager.hpp`), which implements the `ddk::CredentialStore` interface from the DigitalDoorKey (DDK) component. It is the persistent store for the reader's cryptographic identity and the list of trusted HomeKey issuers and their endpoints.

All data is serialized with MessagePack and persisted to a single NVS blob under the `"SAVED_DATA"` namespace. A mutex protects the in-memory state so UI/telemetry readers cannot race with authentication-path mutations.

## Key Responsibilities

*   **Identity Storage:** Holds the reader's `ddk::ReaderIdentity` (private key, public key, public key X, group identifier, sub/unique identifier, and an optional certificate reserved for future Aliro support).
*   **Issuer Management:** Maintains the list of `ddk::Issuer` entries, each with its ID and enrolled endpoints.
*   **Persistence:** Loads on `begin()` and writes back on `save()`, using MessagePack with the DDK map keys (`reader_private_key`, `reader_public_key`, `reader_key_x`, `group_identifier`, `unique_identifier`, `issuers`).
*   **Credential Store Contract:** Provides the identity, issuer list, and provisioning hooks that the DDK authentication flow consumes.

## Public API

### NvsCredentialStore()

Constructs the store. No NVS access happens until `begin()` is called.

**Signature:**
```cpp
NvsCredentialStore();
```

### ~NvsCredentialStore()

Closes the NVS handle if one was opened.

**Signature:**
```cpp
~NvsCredentialStore() override;
```

### begin()

Opens the `"SAVED_DATA"` NVS namespace and calls `load()` to populate the in-memory identity and issuer list from NVS. Must be called before any other method.

**Signature:**
```cpp
bool begin();
```

**Returns:**
*   `bool`: `true` if the NVS handle was opened successfully, `false` otherwise.

### reader_identity()

Returns a constant reference to the reader's identity (implements `ddk::CredentialStore`).

**Signature:**
```cpp
const ddk::ReaderIdentity& reader_identity() const override;
```

**Returns:**
*   `const ddk::ReaderIdentity&`: The reader identity. Notable members:
    *   `private_key` (`"reader_private_key"`)
    *   `public_key` (`"reader_public_key"`, derived at provisioning)
    *   `public_key_x` (`"reader_key_x"`, derived at provisioning)
    *   `group_identifier` — the Reader GID (8 bytes when provisioned)
    *   `sub_identifier` — the reader's unique identifier ("Reader ID" in the WebUI)

### provision_identity()

Replaces the stored reader identity (implements `ddk::CredentialStore`). Does not save by itself; call `save()` to persist.

**Signature:**
```cpp
void provision_identity(const ddk::ReaderIdentity& identity) override;
```

### issuers()

Returns a mutable span over the stored issuer list (implements `ddk::CredentialStore`).

**Signature:**
```cpp
ddk::span<ddk::Issuer> issuers() override;
```

### save()

Serializes the current in-memory state (identity + issuers) to MessagePack and writes it to the NVS blob.

**Signature:**
```cpp
void save() override;
```

### snapshot()

Returns a consistent, mutex-protected copy of everything, for UI/telemetry tasks that must not race with authentication-path mutations.

**Signature:**
```cpp
Snapshot snapshot() const;
```

**Returns:**
*   `Snapshot`: A struct containing a `ddk::ReaderIdentity identity` and a `std::vector<ddk::Issuer> issuers` copy.

### eraseReaderKey()

Clears the reader's own key material. Does not touch the issuer list.

**Signature:**
```cpp
bool eraseReaderKey();
```

**Returns:**
*   `bool`: `true` on success, `false` if the store is not initialized.

### deleteAllReaderData()

Erases everything (identity and issuers) and publishes an `HK_EVENT` / `HK_INTERNAL_EVENT` carrying `HomekitEventType::ACCESSDATA_CHANGED` so subscribers re-synchronize.

**Signature:**
```cpp
bool deleteAllReaderData();
```

**Returns:**
*   `bool`: `true` on success, `false` if the store is not initialized.

### addIssuerIfNotExists()

Adds a new issuer if absent (matched by issuer ID). **Does not save** — the caller must call `save()` to persist.

**Signature:**
```cpp
bool addIssuerIfNotExists(const std::vector<uint8_t>& issuerId,
                          const uint8_t* publicKey);
```

**Parameters:**
*   `issuerId`: The unique identifier for the new issuer.
*   `publicKey`: A pointer to the issuer's long-term public key (LTPK).

**Returns:**
*   `bool`: `true` if a new issuer was added, `false` if an issuer with that ID already existed.

### removeIssuerIfExists()

Removes an issuer if present (matched by issuer ID). **Does not save** — the caller must call `save()` to persist. This is invoked when an Apple Home administrator unpairs a controller.

**Signature:**
```cpp
bool removeIssuerIfExists(const std::vector<uint8_t>& issuerId);
```

**Returns:**
*   `bool`: `true` if an issuer was removed, `false` if none matched.

## Internal Methods

### load()

Private method called by `begin()`. It reads the MessagePack blob from NVS, deserializes it via the `unpack_bytes`-based helpers into the in-memory `identity_` and `issuers_`, and initializes with an empty state if no data is found.
