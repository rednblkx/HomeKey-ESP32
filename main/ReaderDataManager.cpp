#include "ReaderDataManager.hpp"
#include <algorithm>
#include <esp_log.h>
#include <map>
#include <nvs_flash.h>
#include "app_event_loop.hpp"
#include "eventStructs.hpp"
#include "msgpack.h"

const char* NvsCredentialStore::TAG = "NvsCredStore";
const char* NvsCredentialStore::NVS_KEY = "READERDATA";

namespace {

const char* CODEC_TAG = "NvsCredStore";

void pack_bytes(msgpack_packer* pk, const char* key, const std::vector<uint8_t>& value) {
    const size_t key_len = strlen(key);
    msgpack_pack_str(pk, key_len);
    msgpack_pack_str_body(pk, key, key_len);
    msgpack_pack_array(pk, value.size());
    for (uint8_t o : value) {
        msgpack_pack_unsigned_char(pk, o);
    }
}

void unpack_bytes(const std::map<std::string, msgpack_object>& obj_map,
                  const char* key, std::vector<uint8_t>& out) {
    auto it = obj_map.find(key);
    if (it == obj_map.end() || it->second.type != MSGPACK_OBJECT_ARRAY) {
        return;
    }
    const msgpack_object_array& arr = it->second.via.array;
    out.clear();
    out.reserve(arr.size);
    for (uint32_t i = 0; i < arr.size; ++i) {
        out.push_back(static_cast<uint8_t>(arr.ptr[i].via.u64));
    }
}

std::map<std::string, msgpack_object> to_map(msgpack_object obj) {
    std::map<std::string, msgpack_object> obj_map;
    if (obj.type != MSGPACK_OBJECT_MAP) {
        return obj_map;
    }
    for (uint32_t i = 0; i < obj.via.map.size; ++i) {
        msgpack_object_kv* kv = &obj.via.map.ptr[i];
        if (kv->key.type == MSGPACK_OBJECT_STR) {
            obj_map[std::string(kv->key.via.str.ptr, kv->key.via.str.size)] = kv->val;
        }
    }
    return obj_map;
}

void pack_endpoint(msgpack_packer* pk, const ddk::Endpoint& endpoint) {
    msgpack_pack_map(pk, 7);

    pack_bytes(pk, "endpointId", endpoint.id);

    msgpack_pack_str(pk, strlen("last_used_at"));
    msgpack_pack_str_body(pk, "last_used_at", strlen("last_used_at"));
    msgpack_pack_unsigned_int(pk, endpoint.used_at);

    msgpack_pack_str(pk, strlen("counter"));
    msgpack_pack_str_body(pk, "counter", strlen("counter"));
    msgpack_pack_int(pk, endpoint.counter);

    msgpack_pack_str(pk, strlen("key_type"));
    msgpack_pack_str_body(pk, "key_type", strlen("key_type"));
    msgpack_pack_int(pk, static_cast<int>(endpoint.key_type));

    pack_bytes(pk, "publicKey", endpoint.public_key);
    pack_bytes(pk, "endpoint_key_x", endpoint.public_key_x);
    pack_bytes(pk, "persistent_key", endpoint.persistent_key);
}

void unpack_endpoint(msgpack_object obj, ddk::Endpoint& endpoint) {
    if (obj.type != MSGPACK_OBJECT_MAP) {
        ESP_LOGE(CODEC_TAG, "Expected map for endpoint deserialization.");
        return;
    }
    auto obj_map = to_map(obj);

    unpack_bytes(obj_map, "endpointId", endpoint.id);
    if (obj_map.count("last_used_at") && obj_map["last_used_at"].type == MSGPACK_OBJECT_POSITIVE_INTEGER) {
        endpoint.used_at = static_cast<uint32_t>(obj_map["last_used_at"].via.u64);
    }
    // Verbatim quirk preserved: POSITIVE_INTEGER type check + .via.i64 read.
    if (obj_map.count("counter") && obj_map["counter"].type == MSGPACK_OBJECT_POSITIVE_INTEGER) {
        endpoint.counter = static_cast<uint8_t>(obj_map["counter"].via.i64);
    }
    if (obj_map.count("key_type") && obj_map["key_type"].type == MSGPACK_OBJECT_POSITIVE_INTEGER) {
        endpoint.key_type = static_cast<ddk::KeyType>(obj_map["key_type"].via.i64);
    }
    unpack_bytes(obj_map, "publicKey", endpoint.public_key);
    unpack_bytes(obj_map, "endpoint_key_x", endpoint.public_key_x);
    unpack_bytes(obj_map, "persistent_key", endpoint.persistent_key);
}

void pack_issuer(msgpack_packer* pk, const ddk::Issuer& issuer) {
    msgpack_pack_map(pk, 4);

    pack_bytes(pk, "issuerId", issuer.id);
    pack_bytes(pk, "publicKey", issuer.public_key);
    // Written for wire fidelity (rollback reads it); empty in practice today.
    pack_bytes(pk, "issuer_key_x", issuer.public_key_x);

    msgpack_pack_str(pk, strlen("endpoints"));
    msgpack_pack_str_body(pk, "endpoints", strlen("endpoints"));
    msgpack_pack_array(pk, issuer.endpoints.size());
    for (const auto& endpoint : issuer.endpoints) {
        pack_endpoint(pk, endpoint);
    }
}

void unpack_issuer(msgpack_object obj, ddk::Issuer& issuer) {
    if (obj.type != MSGPACK_OBJECT_MAP) {
        ESP_LOGE(CODEC_TAG, "Expected map for issuer deserialization.");
        return;
    }
    auto obj_map = to_map(obj);

    unpack_bytes(obj_map, "issuerId", issuer.id);
    unpack_bytes(obj_map, "publicKey", issuer.public_key);
    unpack_bytes(obj_map, "issuer_key_x", issuer.public_key_x);
    if (obj_map.count("endpoints") && obj_map["endpoints"].type == MSGPACK_OBJECT_ARRAY) {
        msgpack_object_array endpoints_array = obj_map["endpoints"].via.array;
        issuer.endpoints.resize(endpoints_array.size);
        for (uint32_t i = 0; i < endpoints_array.size; ++i) {
            unpack_endpoint(endpoints_array.ptr[i], issuer.endpoints[i]);
        }
    }
}

void pack_all(msgpack_packer* pk, const ddk::ReaderIdentity& identity,
              const std::vector<ddk::Issuer>& issuers) {
    msgpack_pack_map(pk, 6);

    pack_bytes(pk, "reader_private_key", identity.private_key);
    pack_bytes(pk, "reader_public_key", identity.public_key);
    pack_bytes(pk, "reader_key_x", identity.public_key_x);
    pack_bytes(pk, "group_identifier", identity.group_identifier);
    pack_bytes(pk, "unique_identifier", identity.sub_identifier);

    msgpack_pack_str(pk, strlen("issuers"));
    msgpack_pack_str_body(pk, "issuers", strlen("issuers"));
    msgpack_pack_array(pk, issuers.size());
    for (const auto& issuer : issuers) {
        pack_issuer(pk, issuer);
    }
}

void unpack_all(msgpack_object obj, ddk::ReaderIdentity& identity,
                std::vector<ddk::Issuer>& issuers) {
    if (obj.type != MSGPACK_OBJECT_MAP) {
        ESP_LOGE(CODEC_TAG, "Expected map for top-level deserialization.");
        return;
    }
    auto obj_map = to_map(obj);

    unpack_bytes(obj_map, "reader_private_key", identity.private_key);
    unpack_bytes(obj_map, "reader_public_key", identity.public_key);
    unpack_bytes(obj_map, "reader_key_x", identity.public_key_x);
    unpack_bytes(obj_map, "group_identifier", identity.group_identifier);
    unpack_bytes(obj_map, "unique_identifier", identity.sub_identifier);
    if (obj_map.count("issuers") && obj_map["issuers"].type == MSGPACK_OBJECT_ARRAY) {
        msgpack_object_array issuers_array = obj_map["issuers"].via.array;
        issuers.resize(issuers_array.size);
        for (uint32_t i = 0; i < issuers_array.size; ++i) {
            unpack_issuer(issuers_array.ptr[i], issuers[i]);
        }
    }
}

} // namespace

// ---------------------------------------------------------------------------
// Lifecycle
// ---------------------------------------------------------------------------

NvsCredentialStore::NvsCredentialStore() = default;

NvsCredentialStore::~NvsCredentialStore() {
    if (initialized_) {
        nvs_close(handle_);
    }
}

bool NvsCredentialStore::begin() {
    esp_log_level_set(TAG, ESP_LOG_INFO);
    if (initialized_) {
        ESP_LOGW(TAG, "Already initialized.");
        return true;
    }

    esp_err_t err = nvs_open("SAVED_DATA", NVS_READWRITE, &handle_);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Error (%s) opening NVS handle!", esp_err_to_name(err));
        return false;
    }

    initialized_ = true;
    load();
    return true;
}

// ---------------------------------------------------------------------------
// ddk::CredentialStore — non-locking accessors
// ---------------------------------------------------------------------------

const ddk::ReaderIdentity& NvsCredentialStore::reader_identity() const {
    return identity_;
}

void NvsCredentialStore::provision_identity(const ddk::ReaderIdentity& identity) {
  if(identity_.group_identifier.empty() && identity_.sub_identifier.empty() && identity_.private_key.empty()){
    identity_ = identity;
  } else ESP_LOGE(TAG, "Reader Identity already provisioned");
}

ddk::span<ddk::Issuer> NvsCredentialStore::issuers() {
    return issuers_;
}

void NvsCredentialStore::save() {
    if (!initialized_) {
        ESP_LOGE(TAG, "Cannot save, not initialized.");
        return;
    }

    msgpack_sbuffer sbuf;
    msgpack_packer pk;
    msgpack_sbuffer_init(&sbuf);
    msgpack_packer_init(&pk, &sbuf, msgpack_sbuffer_write);
    {
        std::lock_guard<std::mutex> lock(mutex_);
        pack_all(&pk, identity_, issuers_);
    }

    esp_err_t set_err = nvs_set_blob(handle_, NVS_KEY, sbuf.data, sbuf.size);
    msgpack_sbuffer_destroy(&sbuf);

    if (set_err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to set blob in NVS: %s", esp_err_to_name(set_err));
        return;
    }

    esp_err_t commit_err = nvs_commit(handle_);
    if (commit_err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to commit NVS changes: %s", esp_err_to_name(commit_err));
        return;
    }

    ESP_LOGI(TAG, "Reader data successfully saved to NVS.");
}

// ---------------------------------------------------------------------------
// Load / snapshot
// ---------------------------------------------------------------------------

void NvsCredentialStore::load() {
    if (!initialized_) {
        ESP_LOGE(TAG, "Cannot load, not initialized.");
        return;
    }

    size_t required_size = 0;
    esp_err_t err = nvs_get_blob(handle_, NVS_KEY, NULL, &required_size);

    if (err == ESP_ERR_NVS_NOT_FOUND) {
        ESP_LOGI(TAG, "Reader data not found in NVS. Starting with a clean slate.");
        std::lock_guard<std::mutex> lock(mutex_);
        identity_ = {};
        issuers_.clear();
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
    err = nvs_get_blob(handle_, NVS_KEY, buffer.data(), &required_size);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Error (%s) reading blob for key '%s'", esp_err_to_name(err), NVS_KEY);
        return;
    }

    msgpack_unpacked unpacked;
    msgpack_unpacked_init(&unpacked);
    bool success = msgpack_unpack_next(&unpacked, (const char*)buffer.data(), buffer.size(), NULL);
    if (success) {
        ddk::ReaderIdentity loadedIdentity{};
        std::vector<ddk::Issuer> loadedIssuers;
        unpack_all(unpacked.data, loadedIdentity, loadedIssuers);
        std::lock_guard<std::mutex> lock(mutex_);
        identity_ = std::move(loadedIdentity);
        issuers_ = std::move(loadedIssuers);
    } else {
        ESP_LOGE(TAG, "Failed to parse msgpack for reader data. Data may be corrupt.");
    }
    msgpack_unpacked_destroy(&unpacked);
}

NvsCredentialStore::Snapshot NvsCredentialStore::snapshot() const {
    std::lock_guard<std::mutex> lock(mutex_);
    Snapshot snap;
    snap.identity = identity_;
    snap.issuers = issuers_;
    return snap;
}

// ---------------------------------------------------------------------------
// Firmware lifecycle
// ---------------------------------------------------------------------------

bool NvsCredentialStore::eraseReaderKey() {
    if (!initialized_) {
        ESP_LOGE(TAG, "Cannot delete, not initialized.");
        return false;
    }

    {
        std::lock_guard<std::mutex> lock(mutex_);
        identity_ = {};
    }
    ESP_LOGI(TAG, "In-memory reader key cleared.");

    save();

    ESP_LOGI(TAG, "Reader key successfully erased from NVS.");
    return true;
}

bool NvsCredentialStore::deleteAllReaderData() {
    if (!initialized_) {
        ESP_LOGE(TAG, "Cannot delete, not initialized.");
        return false;
    }

    {
        std::lock_guard<std::mutex> lock(mutex_);
        identity_ = {};
        issuers_.clear();
    }
    ESP_LOGI(TAG, "In-memory reader data cleared.");

    esp_err_t erase_err = nvs_erase_key(handle_, NVS_KEY);
    if (erase_err != ESP_OK && erase_err != ESP_ERR_NVS_NOT_FOUND) {
        ESP_LOGE(TAG, "Failed to erase NVS key '%s': %s", NVS_KEY, esp_err_to_name(erase_err));
        return false;
    }

    esp_err_t commit_err = nvs_commit(handle_);
    if (commit_err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to commit NVS erase: %s", esp_err_to_name(commit_err));
        return false;
    }

    HomekitEvent event{.type=ACCESSDATA_CHANGED, .data={}};
    std::vector<uint8_t> event_data;
    alpaca::serialize(event, event_data);
    AppEventLoop::publish(HK_EVENT, HK_INTERNAL_EVENT, event_data.data(), event_data.size());
    ESP_LOGI(TAG, "Reader data successfully erased from NVS.");
    return true;
}

bool NvsCredentialStore::addIssuerIfNotExists(const std::vector<uint8_t>& issuerId,
                                              const uint8_t* publicKey) {
    std::lock_guard<std::mutex> lock(mutex_);
    for (const auto& issuer : issuers_) {
        if (issuer.id.size() == issuerId.size() &&
            std::equal(issuer.id.begin(), issuer.id.end(), issuerId.begin())) {
            ESP_LOGD(TAG, "Issuer already exists, skipping.");
            return false;
        }
    }

    ESP_LOGI(TAG, "Adding new issuer.");
    ddk::Issuer newIssuer;
    newIssuer.id = issuerId;
    newIssuer.public_key.assign(publicKey, publicKey + 32);

    issuers_.emplace_back(std::move(newIssuer));
    return true;
}

bool NvsCredentialStore::removeIssuerIfExists(const std::vector<uint8_t>& issuerId) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = std::find_if(issuers_.begin(), issuers_.end(),
        [&issuerId](const ddk::Issuer& issuer) {
            return issuer.id.size() == issuerId.size() &&
                   std::equal(issuer.id.begin(), issuer.id.end(), issuerId.begin());
        });

    if (it == issuers_.end()) {
        ESP_LOGD(TAG, "Issuer not found, nothing to remove.");
        return false;
    }

    ESP_LOGI(TAG, "Removing issuer.");
    issuers_.erase(it);
    return true;
}
