---
title: "ReaderDataManager"
---

**Description:** Manages the lifecycle of HomeKey reader data (`readerData_t`). This class handles loading, saving, modifying, and deleting the dynamic HomeKey provisioning data from NVS. It ensures that operations are performed safely and provides a single point of access to this critical data.

### Public Methods:

*   **`ReaderDataManager()`**
    *   **Description:** Constructor.
*   **`~ReaderDataManager()`**
    *   **Description:** Destructor, ensures the NVS handle is closed.
*   **`bool begin()`**
    *   **Description:** Initializes the manager by opening NVS and loading existing reader data.
    *   **Returns:** `bool` - `true` on success, `false` on failure.
*   **`const readerData_t& getReaderData() const`**
    *   **Description:** Provides read-only access to the complete reader data structure.
    *   **Returns:** `const readerData_t&` - A constant reference to the `readerData_t` object.
*   **`const std::vector<uint8_t>& getReaderGid() const`**
    *   **Description:** Provides convenient read-only access to the reader group identifier.
    *   **Returns:** `const std::vector<uint8_t>&` - A constant reference to the reader GID vector.
*   **`const std::vector<uint8_t>& getReaderId() const`**
    *   **Description:** Provides convenient read-only access to the reader unique identifier.
    *   **Returns:** `const std::vector<uint8_t>&` - A constant reference to the reader ID vector.
*   **`const readerData_t* updateReaderData(const readerData_t& newData)`**
    *   **Description:** Replaces the current reader data with a new version and saves it to NVS. This is used by the NFCAccess service during provisioning.
    *   **Parameters:**
        *   `newData` (`const readerData_t&`): The complete new `readerData_t` structure to save.
    *   **Returns:** `const readerData_t*` - A constant pointer to the `readerData_t` object if successful, otherwise a `nullptr`.
*   **`bool eraseReaderKey()`**
    *   **Description:** Erases reader's key and IDs from memory and NVS. Used when such request is received from the controller.
    *   **Returns:** `bool` - `true` if the NVS key was successfully erased, `false` otherwise.
*   **`bool deleteAllReaderData()`**
    *   **Description:** Erases all reader data from memory and NVS. Used when the last HomeKit controller is unpaired.
    *   **Returns:** `bool` - `true` if the NVS key was successfully erased, `false` otherwise.
*   **`bool addIssuerIfNotExists(const std::vector<uint8_t>& issuerId, const uint8_t* publicKey)`**
    *   **Description:** Adds a new issuer to the in-memory list if it doesn't already exist. NOTE: This does NOT automatically save to NVS. Call `saveData()` afterwards.
    *   **Parameters:**
        *   `issuerId` (`const std::vector<uint8_t>&`): The 8-byte unique identifier for the issuer.
        *   `publicKey` (`const uint8_t*`): The 32-byte long-term public key of the controller.
    *   **Returns:** `bool` - `true` if a new issuer was added, `false` if it already existed.
*   **`const readerData_t* saveData()`**
    *   **Description:** Persists the current in-memory reader data to NVS.
    *   **Returns:** `const readerData_t*` - A constant pointer to the `readerData_t` object if successful, otherwise a `nullptr`.

---
