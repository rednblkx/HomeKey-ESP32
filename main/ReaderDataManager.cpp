#include "ReaderDataManager.hpp"
#include <map>
#include <nvs_flash.h>
#include <esp_log.h>
#include <algorithm>
#include <ranges>
#include "msgpack.h"

const char* ReaderDataManager::TAG = "ReaderDataManager";
const char* ReaderDataManager::NVS_KEY = "READERDATA";

/**
 * @brief Constructs a ReaderDataManager and initializes internal state.
 *
 * Initializes the manager with no open NVS handle and marks it as not initialized.
 */
ReaderDataManager::ReaderDataManager() : m_isInitialized(false) {
}

/**
 * @brief Releases resources held by the manager and closes the NVS handle if initialized.
 *
 * If the manager opened the NVS namespace during its lifetime, this destructor closes
 * the associated NVS handle to ensure proper cleanup of system resources.
 */
ReaderDataManager::~ReaderDataManager() {
    if (m_isInitialized) {
        nvs_close(m_nvsHandle);
    }
}

/**
 * @brief Initialize NVS storage for reader data and load any existing data.
 *
 * Opens the "SAVED_DATA" NVS namespace, sets the logger level for this component,
 * marks the manager as initialized, and invokes load() to populate in-memory data.
 *
 * @return true if initialization succeeded and load was triggered, false if opening NVS failed.
 */
bool ReaderDataManager::begin() {
  esp_log_level_set(TAG, ESP_LOG_INFO);
    if (m_isInitialized) {
        ESP_LOGW(TAG, "Already initialized.");
        return true;
    }

    esp_err_t err = nvs_open("SAVED_DATA", NVS_READWRITE, &m_nvsHandle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Error (%s) opening NVS handle!", esp_err_to_name(err));
        return false;
    }

    m_isInitialized = true;
    load();
    return true;
}

/**
 * @brief Accesses the current in-memory reader data.
 *
 * @return const readerData_t& Reference to the stored reader data.
 */
const readerData_t& ReaderDataManager::getReaderData() const {
    return m_readerData;
}

/**
 * @brief Accesses the reader's group identifier.
 *
 * @return const std::vector<uint8_t>& Reference to the stored group identifier bytes.
 */
const std::vector<uint8_t>& ReaderDataManager::getReaderGid() const {
    return m_readerData.reader_gid;
}

/**
 * @brief Provides access to the stored reader unique identifier.
 *
 * @return const std::vector<uint8_t>& Reference to the reader unique identifier as a byte vector.
 */
const std::vector<uint8_t>& ReaderDataManager::getReaderId() const {
    return m_readerData.reader_id;
}

/**
 * @brief Loads reader data from NVS into the in-memory reader data structure.
 *
 * Reads the MessagePack-serialized blob stored under the NVS key and deserializes it into
 * m_readerData. If the key is not found, the in-memory reader data is reset to defaults.
 * If the stored blob is empty or cannot be read or parsed, the in-memory state is left
 * unchanged (or reset only when key is not found) and an error is logged.
 *
 * Preconditions:
 * - The NVS namespace must be opened (m_isInitialized == true).
 */
void ReaderDataManager::load() {
    if (!m_isInitialized) {
        ESP_LOGE(TAG, "Cannot load, not initialized.");
        return;
    }

    size_t required_size = 0;
    esp_err_t err = nvs_get_blob(m_nvsHandle, NVS_KEY, NULL, &required_size);

    if (err == ESP_ERR_NVS_NOT_FOUND) {
        ESP_LOGI(TAG, "Reader data not found in NVS. Starting with a clean slate.");
        m_readerData = {}; // Reset to default
        return;
    }
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Error (%s) getting blob size for key '%s'", esp_err_to_name(err), NVS_KEY);
        return;
    }
    if (required_size == 0) {
        ESP_LOGW(TAG, "Key '%s' found but size is 0. Using defaults.", NVS_KEY);
        return;
    }

    std::vector<uint8_t> buffer(required_size);
    err = nvs_get_blob(m_nvsHandle, NVS_KEY, buffer.data(), &required_size);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Error (%s) reading blob for key '%s'", esp_err_to_name(err), NVS_KEY);
        return;
    }
    
    msgpack_unpacked unpacked;
    msgpack_unpacked_init(&unpacked);
    bool success = msgpack_unpack_next(&unpacked, (const char*)buffer.data(), buffer.size(), NULL);
    if(success) {
        msgpack_object obj = unpacked.data;
        unpack_readerData_t(obj, m_readerData);
    } else {
        ESP_LOGE(TAG, "Failed to parse msgpack for reader data. Data may be corrupt.");
    }
    msgpack_unpacked_destroy(&unpacked);
}

/**
 * @brief Persist the in-memory reader data to non-volatile storage (NVS).
 *
 * Serializes the current in-memory readerData_t and writes it to NVS under the class's configured key, then commits the change.
 *
 * @return const readerData_t* Pointer to the in-memory reader data after a successful save; `nullptr` on failure (for example, if the manager is not initialized or an NVS error occurs).
 */
const readerData_t* ReaderDataManager::saveData() {
    if (!m_isInitialized) {
        ESP_LOGE(TAG, "Cannot save, not initialized.");
        return nullptr;
    }
    msgpack_sbuffer sbuf;
    msgpack_packer pk;
    msgpack_sbuffer_init(&sbuf);
    msgpack_packer_init(&pk, &sbuf, msgpack_sbuffer_write);
    pack_readerData_t(&pk, m_readerData);

    esp_err_t set_err = nvs_set_blob(m_nvsHandle, NVS_KEY, sbuf.data, sbuf.size);
    msgpack_sbuffer_destroy(&sbuf);

    if (set_err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to set blob in NVS: %s", esp_err_to_name(set_err));
        return nullptr;
    }

    esp_err_t commit_err = nvs_commit(m_nvsHandle);
    if (commit_err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to commit NVS changes: %s", esp_err_to_name(commit_err));
        return nullptr;
    }

    ESP_LOGI(TAG, "Reader data successfully saved to NVS.");
    return &getReaderData();
}

/**
 * @brief Replace the in-memory reader data with the supplied data and persist it to NVS.
 *
 * @param newData The reader data to store (replaces the current in-memory state).
 * @return const readerData_t* Pointer to the stored reader data after a successful save, or `nullptr` on error.
 */
const readerData_t* ReaderDataManager::updateReaderData(const readerData_t& newData) {
    m_readerData = newData;
    return saveData();
}

/**
 * @brief Clears the in-memory reader key material and persists the cleared state to NVS.
 *
 * If the manager is not initialized, no changes are made and the function returns immediately.
 *
 * @return `true` if the in-memory key was cleared (and a save attempt was issued), `false` if the manager was not initialized.
 */
bool ReaderDataManager::eraseReaderKey() {
    if (!m_isInitialized) {
        ESP_LOGE(TAG, "Cannot delete, not initialized.");
        return false;
    }
    
    m_readerData.reader_gid = {};
    m_readerData.reader_id = {};
    m_readerData.reader_pk = {};
    m_readerData.reader_pk_x = {};
    m_readerData.reader_sk = {};
    ESP_LOGI(TAG, "In-memory reader key cleared.");

    saveData();

    ESP_LOGI(TAG, "Reader key successfully erased from NVS.");
    return true;
}

/**
 * @brief Erase all reader data from both in-memory state and NVS persistent storage.
 *
 * @details Clears the in-memory readerData_t, removes the stored blob under the manager's NVS key,
 * and commits the erase to NVS. Fails if the manager is not initialized or if erase/commit operations fail.
 *
 * @return true if the in-memory data was cleared and the NVS erase + commit succeeded, false otherwise.
 */
bool ReaderDataManager::deleteAllReaderData() {
    if (!m_isInitialized) {
        ESP_LOGE(TAG, "Cannot delete, not initialized.");
        return false;
    }
    
    m_readerData = {};
    ESP_LOGI(TAG, "In-memory reader data cleared.");

    esp_err_t erase_err = nvs_erase_key(m_nvsHandle, NVS_KEY);
    if (erase_err != ESP_OK && erase_err != ESP_ERR_NVS_NOT_FOUND) {
        ESP_LOGE(TAG, "Failed to erase NVS key '%s': %s", NVS_KEY, esp_err_to_name(erase_err));
        return false;
    }

    esp_err_t commit_err = nvs_commit(m_nvsHandle);
    if (commit_err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to commit NVS erase: %s", esp_err_to_name(commit_err));
        return false;
    }
    
    ESP_LOGI(TAG, "Reader data successfully erased from NVS.");
    return true;
}

/**
 * @brief Adds a new issuer to the in-memory issuer list if an issuer with the same identifier is not already present.
 *
 * If no existing issuer has an identifier equal to `issuerId`, appends a new issuer to the manager's in-memory
 * issuers vector using `issuerId` and the first 32 bytes of `publicKey`.
 *
 * @param issuerId Byte sequence identifying the issuer.
 * @param publicKey Pointer to the issuer's public key bytes; the first 32 bytes are copied.
 * @return true if a new issuer was appended, false if an issuer with the same identifier already existed.
 */
bool ReaderDataManager::addIssuerIfNotExists(const std::vector<uint8_t>& issuerId, const uint8_t* publicKey) {
    for (const auto& issuer : m_readerData.issuers) {
        if (issuer.issuer_id.size() == issuerId.size() && 
            std::equal(issuer.issuer_id.begin(), issuer.issuer_id.end(), issuerId.begin())) {
            ESP_LOGD(TAG, "Issuer already exists, skipping.");
            return false;
        }
    }

    ESP_LOGI(TAG, "Adding new issuer.");
    hkIssuer_t newIssuer;
    newIssuer.issuer_id = issuerId;
    newIssuer.issuer_pk.assign(publicKey, publicKey + 32);

    m_readerData.issuers.emplace_back(newIssuer);
    return true;
}

/**
 * @brief Packs an hkEndpoint_t into MessagePack format using the provided packer.
 *
 * Serializes the endpoint's seven fields: "endpointId", "last_used_at", "counter",
 * "key_type", "publicKey", "endpoint_key_x", and "persistent_key".
 *
 * @param pk MessagePack packer used to write the serialized data.
 * @param endpoint Endpoint structure whose contents will be serialized.
 */

void ReaderDataManager::pack_hkEndpoint_t(msgpack_packer* pk, const hkEndpoint_t& endpoint) {
    msgpack_pack_map(pk, 7); // 7 members in hkEndpoint_t

    msgpack_pack_str(pk, strlen("endpointId"));
    msgpack_pack_str_body(pk, "endpointId", strlen("endpointId"));
    msgpack_pack_array(pk, endpoint.endpoint_id.size());
    std::ranges::for_each(endpoint.endpoint_id, [&pk](const auto&o){
      msgpack_pack_unsigned_char(pk, o);
    });

    msgpack_pack_str(pk, strlen("last_used_at"));
    msgpack_pack_str_body(pk, "last_used_at", strlen("last_used_at"));
    msgpack_pack_unsigned_int(pk, endpoint.last_used_at);

    msgpack_pack_str(pk, strlen("counter"));
    msgpack_pack_str_body(pk, "counter", strlen("counter"));
    msgpack_pack_int(pk, endpoint.counter);

    msgpack_pack_str(pk, strlen("key_type"));
    msgpack_pack_str_body(pk, "key_type", strlen("key_type"));
    msgpack_pack_int(pk, endpoint.key_type);

    msgpack_pack_str(pk, strlen("publicKey"));
    msgpack_pack_str_body(pk, "publicKey", strlen("publicKey"));
    msgpack_pack_array(pk, endpoint.endpoint_pk.size());
    std::ranges::for_each(endpoint.endpoint_pk, [&pk](const auto&o){
      msgpack_pack_unsigned_char(pk, o);
    });

    msgpack_pack_str(pk, strlen("endpoint_key_x"));
    msgpack_pack_str_body(pk, "endpoint_key_x", strlen("endpoint_key_x"));
    msgpack_pack_array(pk, endpoint.endpoint_pk_x.size());
    std::ranges::for_each(endpoint.endpoint_pk_x, [&pk](const auto&o){
      msgpack_pack_unsigned_char(pk, o);
    });

    msgpack_pack_str(pk, strlen("persistent_key"));
    msgpack_pack_str_body(pk, "persistent_key", strlen("persistent_key"));
    msgpack_pack_array(pk, endpoint.endpoint_prst_k.size());
    std::ranges::for_each(endpoint.endpoint_prst_k, [&pk](const auto&o){
      msgpack_pack_unsigned_char(pk, o);
    });
}

/**
 * @brief Deserialize a MessagePack object into an hkEndpoint_t structure.
 *
 * Parses a MessagePack map containing endpoint fields and populates the provided
 * hkEndpoint_t with any present entries. Recognized keys (and their target
 * members) are: "endpointId" -> endpoint_id, "last_used_at" -> last_used_at,
 * "counter" -> counter, "key_type" -> key_type, "publicKey" -> endpoint_pk,
 * "endpoint_key_x" -> endpoint_pk_x, and "persistent_key" -> endpoint_prst_k.
 *
 * If the incoming object is not a map, the function returns immediately and
 * does not modify the output parameter.
 *
 * @param obj MessagePack object expected to be a map representing an endpoint.
 * @param[out] endpoint Destination hkEndpoint_t to populate with parsed values.
 */
void ReaderDataManager::unpack_hkEndpoint_t(msgpack_object obj, hkEndpoint_t& endpoint) {
    if (obj.type != MSGPACK_OBJECT_MAP) {
        ESP_LOGE(TAG, "Error: Expected map for hkEndpoint_t deserialization.");
        return;
    }

    std::map<std::string, msgpack_object> obj_map;
    for (uint32_t i = 0; i < obj.via.map.size; ++i) {
        msgpack_object_kv* kv = &obj.via.map.ptr[i];
        if (kv->key.type == MSGPACK_OBJECT_STR) {
            obj_map[std::string(kv->key.via.str.ptr, kv->key.via.str.size)] = kv->val;
        }
    }

    if (obj_map.count("endpointId") && obj_map["endpointId"].type == MSGPACK_OBJECT_ARRAY) {
      auto msgpack_elements = std::ranges::subrange(obj_map["endpointId"].via.array.ptr, obj_map["endpointId"].via.array.ptr + obj_map["endpointId"].via.array.size);
      auto integer_view = msgpack_elements | std::ranges::views::transform([](const msgpack_object& o){return o.via.u64;});
      endpoint.endpoint_id.assign(integer_view.begin(), integer_view.end());
    }
    if (obj_map.count("last_used_at") && obj_map["last_used_at"].type == MSGPACK_OBJECT_POSITIVE_INTEGER) {
      endpoint.last_used_at = obj_map["last_used_at"].via.u64;
    }
    if (obj_map.count("counter") && obj_map["counter"].type == MSGPACK_OBJECT_POSITIVE_INTEGER) { // Can be negative, but for simplicity, assuming positive
      endpoint.counter = obj_map["counter"].via.i64;
    }
    if (obj_map.count("key_type") && obj_map["key_type"].type == MSGPACK_OBJECT_POSITIVE_INTEGER) { // Can be negative, but for simplicity, assuming positive
      endpoint.key_type = obj_map["key_type"].via.i64;
    }
    if (obj_map.count("publicKey") && obj_map["publicKey"].type == MSGPACK_OBJECT_ARRAY) {
      auto msgpack_elements = std::ranges::subrange(obj_map["publicKey"].via.array.ptr, obj_map["publicKey"].via.array.ptr + obj_map["publicKey"].via.array.size);
      auto integer_view = msgpack_elements | std::ranges::views::transform([](const msgpack_object& o){return o.via.u64;});
      endpoint.endpoint_pk.assign(integer_view.begin(), integer_view.end());
    }
    if (obj_map.count("endpoint_key_x") && obj_map["endpoint_key_x"].type == MSGPACK_OBJECT_ARRAY) {
      auto msgpack_elements = std::ranges::subrange(obj_map["endpoint_key_x"].via.array.ptr, obj_map["endpoint_key_x"].via.array.ptr + obj_map["endpoint_key_x"].via.array.size);
      auto integer_view = msgpack_elements | std::ranges::views::transform([](const msgpack_object& o){return o.via.u64;});
      endpoint.endpoint_pk_x.assign(integer_view.begin(), integer_view.end());
    }
    if (obj_map.count("persistent_key") && obj_map["persistent_key"].type == MSGPACK_OBJECT_ARRAY) {
      auto msgpack_elements = std::ranges::subrange(obj_map["persistent_key"].via.array.ptr, obj_map["persistent_key"].via.array.ptr + obj_map["persistent_key"].via.array.size);
      auto integer_view = msgpack_elements | std::ranges::views::transform([](const msgpack_object& o){return o.via.u64;});
      endpoint.endpoint_prst_k.assign(integer_view.begin(), integer_view.end());
    }
}

/**
 * @brief Serializes an hkIssuer_t into MessagePack format and writes it to the given packer.
 *
 * The issuer is encoded as a map with four keys: "issuerId", "publicKey", "issuer_key_x", and "endpoints".
 * Byte-vector fields are written as arrays of unsigned bytes. The "endpoints" key is written as an array
 * of endpoint objects, each serialized as a nested MessagePack structure.
 *
 * @param pk Pointer to an active msgpack_packer used for writing the serialized data.
 * @param issuer The issuer object whose fields will be serialized into the packer.
 */
void ReaderDataManager::pack_hkIssuer_t(msgpack_packer* pk, const hkIssuer_t& issuer) {
    msgpack_pack_map(pk, 4); // 4 members in hkIssuer_t

    msgpack_pack_str(pk, strlen("issuerId"));
    msgpack_pack_str_body(pk, "issuerId", strlen("issuerId"));
    msgpack_pack_array(pk, issuer.issuer_id.size());
    std::ranges::for_each(issuer.issuer_id, [&pk](const auto&o){
      msgpack_pack_unsigned_char(pk, o);
    });  

    msgpack_pack_str(pk, strlen("publicKey"));
    msgpack_pack_str_body(pk, "publicKey", strlen("publicKey"));
    msgpack_pack_array(pk, issuer.issuer_pk.size());
    std::ranges::for_each(issuer.issuer_pk, [&pk](const auto&o){
      msgpack_pack_unsigned_char(pk, o);
    });  

    msgpack_pack_str(pk, strlen("issuer_key_x"));
    msgpack_pack_str_body(pk, "issuer_key_x", strlen("issuer_key_x"));
    msgpack_pack_array(pk, issuer.issuer_pk_x.size());
    std::ranges::for_each(issuer.issuer_pk_x, [&pk](const auto&o){
      msgpack_pack_unsigned_char(pk, o);
    });  

    msgpack_pack_str(pk, strlen("endpoints"));
    msgpack_pack_str_body(pk, "endpoints", strlen("endpoints"));
    msgpack_pack_array(pk, issuer.endpoints.size());
    for (const auto& endpoint : issuer.endpoints) {
        pack_hkEndpoint_t(pk, endpoint);
    }
}

/**
 * @brief Deserialize an hkIssuer_t from a MessagePack map.
 *
 * Parses the provided MessagePack object and populates the given `issuer`
 * with any present fields: `issuerId`, `publicKey`, `issuer_key_x`, and
 * `endpoints`. Array fields are converted to their corresponding byte
 * vectors; `endpoints` is deserialized into `issuer.endpoints` using
 * `unpack_hkEndpoint_t`. Fields that are not present are left unchanged.
 *
 * If `obj` is not a map, the function logs an error and returns without
 * modifying `issuer`.
 *
 * @param obj MessagePack object expected to be a map representing an hkIssuer_t.
 * @param[out] issuer Reference to the hkIssuer_t to populate.
 */
void ReaderDataManager::unpack_hkIssuer_t(msgpack_object obj, hkIssuer_t& issuer) {
    if (obj.type != MSGPACK_OBJECT_MAP) {
        ESP_LOGE(TAG, "Error: Expected map for hkIssuer_t deserialization.");
        return;
    }

    std::map<std::string, msgpack_object> obj_map;
    for (uint32_t i = 0; i < obj.via.map.size; ++i) {
        msgpack_object_kv* kv = &obj.via.map.ptr[i];
        if (kv->key.type == MSGPACK_OBJECT_STR) {
            obj_map[std::string(kv->key.via.str.ptr, kv->key.via.str.size)] = kv->val;
        }
    }

    if (obj_map.count("issuerId") && obj_map["issuerId"].type == MSGPACK_OBJECT_ARRAY) {
      auto msgpack_elements = std::ranges::subrange(obj_map["issuerId"].via.array.ptr, obj_map["issuerId"].via.array.ptr + obj_map["issuerId"].via.array.size);
      auto integer_view = msgpack_elements | std::ranges::views::transform([](const msgpack_object& o){return o.via.u64;});
      issuer.issuer_id.assign(integer_view.begin(), integer_view.end());
    }
    if (obj_map.count("publicKey") && obj_map["publicKey"].type == MSGPACK_OBJECT_ARRAY) {
      auto msgpack_elements = std::ranges::subrange(obj_map["publicKey"].via.array.ptr, obj_map["publicKey"].via.array.ptr + obj_map["publicKey"].via.array.size);
      auto integer_view = msgpack_elements | std::ranges::views::transform([](const msgpack_object& o){return o.via.u64;});
      issuer.issuer_pk.assign(integer_view.begin(), integer_view.end());
    }
    if (obj_map.count("issuer_key_x") && obj_map["issuer_key_x"].type == MSGPACK_OBJECT_ARRAY) {
      auto msgpack_elements = std::ranges::subrange(obj_map["issuer_key_x"].via.array.ptr, obj_map["issuer_key_x"].via.array.ptr + obj_map["issuer_key_x"].via.array.size);
      auto integer_view = msgpack_elements | std::ranges::views::transform([](const msgpack_object& o){return o.via.u64;});
      issuer.issuer_pk_x.assign(integer_view.begin(), integer_view.end());
    }
    if (obj_map.count("endpoints") && obj_map["endpoints"].type == MSGPACK_OBJECT_ARRAY) {
        msgpack_object_array endpoints_array = obj_map["endpoints"].via.array;
        issuer.endpoints.resize(endpoints_array.size);
        for (uint32_t i = 0; i < endpoints_array.size; ++i) {
            unpack_hkEndpoint_t(endpoints_array.ptr[i], issuer.endpoints[i]);
        }
    }
}

/**
 * @brief Serializes a readerData_t instance into MessagePack and appends it to the packer.
 *
 * Packs a map with the keys `reader_private_key`, `reader_public_key`, `reader_key_x`,
 * `group_identifier`, `unique_identifier`, and `issuers`. Byte-array fields are written
 * as arrays of unsigned bytes; `issuers` is written as an array with each entry serialized
 * via pack_hkIssuer_t.
 *
 * @param pk Pointer to an initialized msgpack_packer that will receive the serialized data.
 * @param reader_data The readerData_t object to serialize.
 */
void ReaderDataManager::pack_readerData_t(msgpack_packer* pk, const readerData_t& reader_data) {
    msgpack_pack_map(pk, 6); // 6 members in readerData_t

    msgpack_pack_str(pk, strlen("reader_private_key"));
    msgpack_pack_str_body(pk, "reader_private_key", strlen("reader_private_key"));
    msgpack_pack_array(pk, reader_data.reader_sk.size());
    std::ranges::for_each(reader_data.reader_sk, [&pk](const auto&o){
      msgpack_pack_unsigned_char(pk, o);
    });  

    msgpack_pack_str(pk, strlen("reader_public_key"));
    msgpack_pack_str_body(pk, "reader_public_key", strlen("reader_public_key"));
    msgpack_pack_array(pk, reader_data.reader_pk.size());
    std::ranges::for_each(reader_data.reader_pk, [&pk](const auto&o){
      msgpack_pack_unsigned_char(pk, o);
    });  

    msgpack_pack_str(pk, strlen("reader_key_x"));
    msgpack_pack_str_body(pk, "reader_key_x", strlen("reader_key_x"));
    msgpack_pack_array(pk, reader_data.reader_pk_x.size());
    std::ranges::for_each(reader_data.reader_pk_x, [&pk](const auto&o){
      msgpack_pack_unsigned_char(pk, o);
    });  

    msgpack_pack_str(pk, strlen("group_identifier"));
    msgpack_pack_str_body(pk, "group_identifier", strlen("group_identifier"));
    msgpack_pack_array(pk, reader_data.reader_gid.size());
    std::ranges::for_each(reader_data.reader_gid, [&pk](const auto&o){
      msgpack_pack_unsigned_char(pk, o);
    });  

    msgpack_pack_str(pk, strlen("unique_identifier"));
    msgpack_pack_str_body(pk, "unique_identifier", strlen("unique_identifier"));
    msgpack_pack_array(pk, reader_data.reader_id.size());
    std::ranges::for_each(reader_data.reader_id, [&pk](const auto&o){
      msgpack_pack_unsigned_char(pk, o);
    });  

    msgpack_pack_str(pk, strlen("issuers"));
    msgpack_pack_str_body(pk, "issuers", strlen("issuers"));
    msgpack_pack_array(pk, reader_data.issuers.size());
    for (const auto& issuer : reader_data.issuers) {
        pack_hkIssuer_t(pk, issuer);
    }
}

/**
 * @brief Populate a readerData_t structure from a MessagePack object.
 *
 * Deserializes expected fields from a MessagePack map into the provided reader_data structure; missing or type-mismatched fields are left unchanged and the function returns immediately if the top-level object is not a map.
 *
 * @param obj MessagePack object expected to be a map containing keys: "reader_private_key", "reader_public_key", "reader_key_x", "group_identifier", "unique_identifier", and "issuers".
 * @param[out] reader_data Destination structure that will be updated with any fields present and correctly typed in the MessagePack map.
 */
void ReaderDataManager::unpack_readerData_t(msgpack_object obj, readerData_t& reader_data) {
    if (obj.type != MSGPACK_OBJECT_MAP) {
        ESP_LOGE(TAG, "Error: Expected map for readerData_t deserialization.");
        return;
    }

    std::map<std::string, msgpack_object> obj_map;
    for (uint32_t i = 0; i < obj.via.map.size; ++i) {
        msgpack_object_kv* kv = &obj.via.map.ptr[i];
        if (kv->key.type == MSGPACK_OBJECT_STR) {
            obj_map[std::string(kv->key.via.str.ptr, kv->key.via.str.size)] = kv->val;
        }
    } 

    if (obj_map.count("reader_private_key") && obj_map["reader_private_key"].type == MSGPACK_OBJECT_ARRAY) {
      auto msgpack_elements = std::ranges::subrange(obj_map["reader_private_key"].via.array.ptr, obj_map["reader_private_key"].via.array.ptr + obj_map["reader_private_key"].via.array.size);
      auto integer_view = msgpack_elements | std::ranges::views::transform([](const msgpack_object& o){return o.via.u64;});
      reader_data.reader_sk.assign(integer_view.begin(), integer_view.end());
    }
    if (obj_map.count("reader_public_key") && obj_map["reader_public_key"].type == MSGPACK_OBJECT_ARRAY) {
      auto msgpack_elements = std::ranges::subrange(obj_map["reader_public_key"].via.array.ptr, obj_map["reader_public_key"].via.array.ptr + obj_map["reader_public_key"].via.array.size);
      auto integer_view = msgpack_elements | std::ranges::views::transform([](const msgpack_object& o){return o.via.u64;});
      reader_data.reader_pk.assign(integer_view.begin(), integer_view.end());
    }
    if (obj_map.count("reader_key_x") && obj_map["reader_key_x"].type == MSGPACK_OBJECT_ARRAY) {
      auto msgpack_elements = std::ranges::subrange(obj_map["reader_key_x"].via.array.ptr, obj_map["reader_key_x"].via.array.ptr + obj_map["reader_key_x"].via.array.size);
    auto integer_view = msgpack_elements | std::ranges::views::transform([](const msgpack_object& o){return o.via.u64;});
      reader_data.reader_pk_x.assign(integer_view.begin(), integer_view.end());
    }
    if (obj_map.count("group_identifier") && obj_map["group_identifier"].type == MSGPACK_OBJECT_ARRAY) {
      auto msgpack_elements = std::ranges::subrange(obj_map["group_identifier"].via.array.ptr, obj_map["group_identifier"].via.array.ptr + obj_map["group_identifier"].via.array.size);
      auto integer_view = msgpack_elements | std::ranges::views::transform([](const msgpack_object& o){return o.via.u64;});
      reader_data.reader_gid.assign(integer_view.begin(), integer_view.end());
    }
    if (obj_map.count("unique_identifier") && obj_map["unique_identifier"].type == MSGPACK_OBJECT_ARRAY) {
      auto msgpack_elements = std::ranges::subrange(obj_map["unique_identifier"].via.array.ptr, obj_map["unique_identifier"].via.array.ptr + obj_map["unique_identifier"].via.array.size);
      auto integer_view = msgpack_elements | std::ranges::views::transform([](const msgpack_object& o){return o.via.u64;});
      reader_data.reader_id.assign(integer_view.begin(), integer_view.end());
    }
    if (obj_map.count("issuers") && obj_map["issuers"].type == MSGPACK_OBJECT_ARRAY) {
      msgpack_object_array issuers_array = obj_map["issuers"].via.array;
      reader_data.issuers.resize(issuers_array.size);
      for (uint32_t i = 0; i < issuers_array.size; ++i) {
        unpack_hkIssuer_t(issuers_array.ptr[i], reader_data.issuers[i]);
      }
    }
}
