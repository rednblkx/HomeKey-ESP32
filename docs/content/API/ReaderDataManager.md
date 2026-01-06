---
title: "ReaderDataManager"
---

## Overview

The `ReaderDataManager` class is a specialized data management component responsible for the persistence of all data related to the NFC reader's identity and its trusted HomeKey issuers. It provides a crucial service by storing, retrieving, and managing the complex data structures required for HomeKey authentication, ensuring this data survives device reboots.

The class uses the ESP-IDF's Non-volatile Storage (NVS) system as its backend and employs the MessagePack binary serialization format for efficient storage of the nested data structures, which include cryptographic keys, identifiers, and lists of trusted issuers and their associated endpoints.

## Key Responsibilities

*   **Data Persistence:** Manages the loading and saving of the `readerData_t` structure to and from NVS.
*   **In-Memory Cache:** Holds the authoritative `readerData_t` object in memory for fast access by other components like the `NfcManager`.
*   **Data Integrity:** Provides methods to safely update, add to, or delete the stored data.
*   **Serialization:** Handles the serialization (packing) and deserialization (unpacking) of the complex, nested `readerData_t` struct into the MessagePack format.
*   **Issuer Management:** Includes logic to add new HomeKey issuers to the trusted list, avoiding duplicates.

## Public API

### ReaderDataManager()

Constructs a new `ReaderDataManager` instance. The constructor initializes the manager in an uninitialized state.

**Signature:**
```cpp
ReaderDataManager();
```

### ~ReaderDataManager()

Destructor for the `ReaderDataManager`. It ensures that if an NVS handle was opened, it is properly closed to release system resources.

**Signature:**
```cpp
~ReaderDataManager();
```

### begin()

Initializes the manager by opening a handle to the NVS namespace. It must be called before any other methods. Upon successful initialization, it automatically calls `load()` to populate the in-memory data from NVS.

**Signature:**
```cpp
bool begin();
```

**Returns:**
*   `bool`: `true` if the NVS handle was opened successfully, `false` otherwise.

### getReaderData()

Provides read-only access to the entire in-memory `readerData_t` structure.

**Signature:**
```cpp
const readerData_t& getReaderData() const;
```

**Returns:**
*   `const readerData_t&`: A constant reference to the cached reader data.

### getReaderGid()

A convenience method to get the reader's Group Identifier (GID).

**Signature:**
```cpp
const std::vector<uint8_t>& getReaderGid() const;
```

**Returns:**
*   `const std::vector<uint8_t>&`: A constant reference to the reader GID byte vector.

### getReaderId()

A convenience method to get the reader's Unique Identifier.

**Signature:**
```cpp
const std::vector<uint8_t>& getReaderId() const;
```

**Returns:**
*   `const std::vector<uint8_t>&`: A constant reference to the reader ID byte vector.

### saveData()

Serializes the current in-memory `readerData_t` structure to MessagePack and writes the resulting binary blob to NVS. This is the primary method for persisting any changes made to the reader data.

**Signature:**
```cpp
const readerData_t* saveData();
```

**Returns:**
*   `const readerData_t*`: A pointer to the current in-memory data on success, or `nullptr` if the save operation fails.

**Note:**
This function persists a snapshot of the in-memory data to NVS. The returned pointer refers to the manager's current
in-memory state, which may change concurrently. If you need a consistent snapshot, call `getReaderDataCopy()`.

### updateReaderData()

Replaces the entire in-memory `readerData_t` object with a new one and then calls `saveData()` to persist the change.

**Signature:**
```cpp
const readerData_t* updateReaderData(const readerData_t& newData);
```

**Parameters:**
*   `newData`: The new `readerData_t` object to store.

**Returns:**
*   `const readerData_t*`: A pointer to the updated in-memory data on success, or `nullptr` on failure.

### eraseReaderKey()

Clears the reader's own cryptographic key material (private key, public key, GID, etc.) from the in-memory data and then saves this cleared state to NVS. The list of issuers is preserved.

**Signature:**
```cpp
bool eraseReaderKey();
```

**Returns:**
*   `bool`: `true` if the key was cleared and the save was attempted, `false` if the manager was not initialized.

### deleteAllReaderData()

Completely wipes all reader data. It clears the in-memory `readerData_t` object and erases the corresponding key from NVS, effectively performing a factory reset of the reader's data.

**Signature:**
```cpp
bool deleteAllReaderData();
```

**Returns:**
*   `bool`: `true` if the data was cleared from memory and NVS successfully, `false` otherwise.

### addIssuerIfNotExists()

Adds a new issuer to the in-memory list of trusted issuers. It checks for duplicates based on the `issuerId` to prevent adding the same issuer multiple times. Note that this method only modifies the in-memory data; `saveData()` must be called separately to persist the new issuer.

**Signature:**
```cpp
bool addIssuerIfNotExists(const std::vector<uint8_t>& issuerId, const uint8_t* publicKey);
```

**Parameters:**
*   `issuerId`: The unique identifier for the new issuer.
*   `publicKey`: A pointer to the issuer's public key.

**Returns:**
*   `bool`: `true` if a new issuer was added, `false` if an issuer with that ID already existed.

## Internal Methods

### load()

This private method is called by `begin()`. It reads the MessagePack blob from NVS, deserializes it using the `unpack_readerData_t` helper, and populates the in-memory `m_readerData` object. If no data is found in NVS, it initializes with a default, empty state.

### Serialization and Deserialization

The class contains a set of `pack_*` and `unpack_*` static helper functions. These functions are responsible for the detailed work of converting the nested `readerData_t`, `hkIssuer_t`, and `hkEndpoint_t` structs to and from the MessagePack format. They handle the mapping of struct members to map keys and correctly serialize different data types (integers, byte vectors, and nested objects).
