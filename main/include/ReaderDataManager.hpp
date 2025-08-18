#ifndef READER_DATA_MANAGER_H
#define READER_DATA_MANAGER_H

#include <vector>
#include <nvs.h>
#include "HomeKey.h"

/**
 * @class ReaderDataManager
 * @brief Manages the lifecycle of HomeKey reader data (readerData_t).
 *
 * This class handles loading, saving, modifying, and deleting the dynamic
 * HomeKey provisioning data from NVS. It ensures that operations are
 * performed safely and provides a single point of access to this critical data.
 */
class ReaderDataManager {
public:
    /**
     * @brief Constructor.
     */
    ReaderDataManager();

    /**
     * @brief Destructor, ensures the NVS handle is closed.
     */
    ~ReaderDataManager();

    /**
     * @brief Initializes the manager by opening NVS and loading existing reader data.
     * @return True on success, false on failure.
     */
    bool begin();

    /**
     * @brief Provides read-only access to the complete reader data structure.
     * @return A constant reference to the readerData_t object.
     */
    const readerData_t& getReaderData() const;

    /**
     * @brief Provides convenient read-only access to the reader group identifier.
     * @return A constant reference to the reader GID vector.
     */
    const std::vector<uint8_t>& getReaderGid() const;

    const std::vector<uint8_t>& getReaderId() const;
    /**
     * @brief Replaces the current reader data with a new version and saves it to NVS.
     * This is used by the NFCAccess service during provisioning.
     * @param newData The complete new readerData_t structure to save.
     * @return True if the data was successfully saved to NVS, false otherwise.
     */
    bool saveReaderData(const readerData_t& newData);

    bool deleteReaderKey();

    /**
     * @brief Erases all reader data from memory and NVS.
     * Used when the last HomeKit controller is unpaired.
     * @return True if the NVS key was successfully erased, false otherwise.
     */
    bool deleteAllReaderData();

    /**
     * @brief Adds a new issuer to the in-memory list if it doesn't already exist.
     * NOTE: This does NOT automatically save to NVS. Call save() afterwards.
     * @param issuerId The 8-byte unique identifier for the issuer.
     * @param publicKey The 32-byte long-term public key of the controller.
     * @return True if a new issuer was added, false if it already existed.
     */
    bool addIssuerIfNotExists(const std::vector<uint8_t>& issuerId, const uint8_t* publicKey);
    
    /**
     * @brief Persists the current in-memory reader data to NVS.
     * @return True on successful save, false on failure.
     */
    bool save();

private:
    /**
     * @brief Internal helper to load data from NVS into the member variable.
     */
    void load();

    readerData_t m_readerData;
    nvs_handle m_nvsHandle;
    bool m_isInitialized;

    static const char* TAG;
    static const char* NVS_KEY;
};

#endif // READER_DATA_MANAGER_H
