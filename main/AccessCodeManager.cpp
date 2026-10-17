#include "AccessCodeManager.hpp"
#include "esp_log.h"
#include "nvs.h"
#include "msgpack.h"
#include <algorithm>

namespace AcTlv {
    // Root Tags
    constexpr uint8_t TAG_OPERATION = 0x01;
    constexpr uint8_t TAG_CONTROL_REQUEST = 0x02;
    constexpr uint8_t TAG_CONTROL_RESPONSE = 0x03;

    // Sub-Tags (Request/Response)
    constexpr uint8_t TAG_IDENTIFIER = 0x01;
    constexpr uint8_t TAG_ACCESS_CODE = 0x02;
    constexpr uint8_t TAG_FLAGS = 0x03;
    constexpr uint8_t TAG_STATUS_CODE = 0x04;

    // Operations
    constexpr uint8_t OP_LIST = 0x01;
    constexpr uint8_t OP_READ = 0x02;
    constexpr uint8_t OP_ADD = 0x03;
    constexpr uint8_t OP_UPDATE = 0x04;
    constexpr uint8_t OP_REMOVE = 0x05;

    // Status Codes
    constexpr uint8_t STATUS_SUCCESS = 0x00;
    constexpr uint8_t STATUS_EXCEEDED_MAX = 0x02;
    constexpr uint8_t STATUS_DUPLICATE = 0x04;
    constexpr uint8_t STATUS_TOO_SHORT = 0x05;
    constexpr uint8_t STATUS_TOO_LONG = 0x06;
    constexpr uint8_t STATUS_INVALID_CHAR = 0x07;
    constexpr uint8_t STATUS_INVALID_REQUEST = 0x08;
    constexpr uint8_t STATUS_DOES_NOT_EXIST = 0x09;
}

AccessCodeManager::AccessCodeManager(uint8_t min_length, uint8_t max_length, uint8_t max_codes)
    : min_code_length(min_length), max_code_length(max_length), max_access_codes(max_codes) {}

void AccessCodeManager::init() {
    nvs_handle handle;
    if (nvs_open(nvs_namespace, NVS_READONLY, &handle) != ESP_OK) {
        ESP_LOGI(TAG, "No access codes found in NVS. Starting fresh.");
        return;
    }
    size_t blob_len = 0;
    esp_err_t err = nvs_get_blob(handle, nvs_key, NULL, &blob_len);
    if (err != ESP_OK || blob_len == 0) {
        nvs_close(handle);
        ESP_LOGI(TAG, "No access codes found in NVS. Starting fresh.");
        return;
    }
    std::vector<uint8_t> blob(blob_len);
    err = nvs_get_blob(handle, nvs_key, blob.data(), &blob_len);
    nvs_close(handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to read access codes blob: %d", err);
        return;
    }

    msgpack_zone zone;
    msgpack_zone_init(&zone, 4096);
    msgpack_object obj;
    if (msgpack_unpack(reinterpret_cast<const char*>(blob.data()), blob.size(), NULL, &zone, &obj) != MSGPACK_UNPACK_SUCCESS) {
        ESP_LOGE(TAG, "Failed to deserialize access codes. Starting fresh.");
        msgpack_zone_destroy(&zone);
        return;
    }
    // copy into local scope before destroying the zone
    std::vector<AccessCode> loaded;
    uint16_t max_id = 0;
    if (obj.type == MSGPACK_OBJECT_ARRAY) {
        for (uint32_t i = 0; i < obj.via.array.size; i++) {
            const msgpack_object& entry = obj.via.array.ptr[i];
            if (entry.type != MSGPACK_OBJECT_ARRAY || entry.via.array.size != 4) continue;
            const msgpack_object* f = entry.via.array.ptr;
            AccessCode code;
            code.id = static_cast<uint16_t>(f[0].via.u64);
            code.code.assign(f[1].via.str.ptr, f[1].via.str.size);
            code.flags = static_cast<uint8_t>(f[2].via.u64);
            code.status = static_cast<uint8_t>(f[3].via.u64);
            loaded.push_back(code);
            if (code.id > max_id) max_id = code.id;
        }
    }
    msgpack_zone_destroy(&zone);
    access_codes = std::move(loaded);
    next_id = max_id + 1;
    ESP_LOGI(TAG, "Successfully loaded %d access codes from NVS.", access_codes.size());
}

bool AccessCodeManager::save_codes() {
    msgpack_sbuffer sbuf;
    msgpack_sbuffer_init(&sbuf);
    msgpack_packer pk;
    msgpack_packer_init(&pk, &sbuf, msgpack_sbuffer_write);
    msgpack_pack_array(&pk, access_codes.size());
    for (const auto& code : access_codes) {
        msgpack_pack_array(&pk, 4);
        msgpack_pack_unsigned_short(&pk, code.id);
        msgpack_pack_str(&pk, code.code.size());
        msgpack_pack_str_body(&pk, code.code.data(), code.code.size());
        msgpack_pack_unsigned_char(&pk, code.flags);
        msgpack_pack_unsigned_char(&pk, code.status);
    }

    nvs_handle handle;
    esp_err_t err = nvs_open(nvs_namespace, NVS_READWRITE, &handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to open NVS namespace '%s': %d", nvs_namespace, err);
        msgpack_sbuffer_destroy(&sbuf);
        return false;
    }
    err = nvs_set_blob(handle, nvs_key, sbuf.data, sbuf.size);
    if (err == ESP_OK) {
        err = nvs_commit(handle);
    }
    nvs_close(handle);
    msgpack_sbuffer_destroy(&sbuf);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to save access codes: %d", err);
        return false;
    }
    return true;
}

void AccessCodeManager::purge_codes() {
    access_codes.clear();
    next_id = 1;
    nvs_handle handle;
    esp_err_t err = nvs_open(nvs_namespace, NVS_READWRITE, &handle);
    if (err != ESP_OK) {
        ESP_LOGW(TAG, "Failed to open NVS namespace '%s' for purge: %d", nvs_namespace, err);
        return;
    }
    err = nvs_erase_key(handle, nvs_key);
    if (err == ESP_OK) {
        err = nvs_commit(handle);
    }
    nvs_close(handle);
    if (err != ESP_OK && err != ESP_ERR_NVS_NOT_FOUND) {
        ESP_LOGE(TAG, "Failed to erase access codes from NVS: %d", err);
        return;
    }
    ESP_LOGI(TAG, "All access codes purged.");
}

bool AccessCodeManager::validateCode(const std::string& code) const {
    auto it = std::find_if(access_codes.begin(), access_codes.end(),
        [&code](const AccessCode& ac){ return ac.code == code; });

    if (it != access_codes.end()) {
        ESP_LOGI(TAG, "Code is valid!");
        return true;
    }

    ESP_LOGW(TAG, "Code is invalid.");
    return false;
}

std::vector<uint8_t> AccessCodeManager::handle_write(const std::vector<uint8_t>& request) {
    std::vector<hap::core::TLV> request_tlv = hap::core::TLV8::parse(request);
    auto op = hap::core::TLV8::find_uint8(request_tlv, AcTlv::TAG_OPERATION);
    if (!op) {
        ESP_LOGE(TAG, "Access Code write is missing operation tag.");
        return {};
    }

    uint8_t operation = *op;
    ESP_LOGI(TAG, "Handling Access Code operation: %d", operation);

    // Response fragments: [operation TLV], then one encoded control-response
    // fragment per entry, separated by 2 zero bytes (required by bulk
    // List/Read operations so parsers don't defragment them together).
    std::vector<hap::core::TLV> response;
    response.emplace_back(AcTlv::TAG_OPERATION, operation);
    auto append_fragment = [&response](const std::vector<hap::core::TLV>& fragment) {
        if (std::any_of(response.begin(), response.end(),
                        [](const hap::core::TLV& t) { return t.type == AcTlv::TAG_CONTROL_RESPONSE; })) {
            response.emplace_back(0x00, std::vector<uint8_t>{0x00, 0x00});
        }
        response.emplace_back(AcTlv::TAG_CONTROL_RESPONSE, hap::core::TLV8::encode(fragment));
    };

    switch (operation) {
        case AcTlv::OP_LIST:
            for (const auto& code_entry : access_codes) {
                append_fragment(build_code_response_fragment(code_entry));
            }
            break;
        case AcTlv::OP_READ:
            read_codes(request_tlv, append_fragment);
            break;
        case AcTlv::OP_ADD:
            add_code(request_tlv, append_fragment);
            break;
        case AcTlv::OP_UPDATE:
            update_code(request_tlv, append_fragment);
            break;
        case AcTlv::OP_REMOVE:
            remove_code(request_tlv, append_fragment);
            break;
        default:
            ESP_LOGW(TAG, "Unhandled Access Code operation: %d", operation);
            return {};
    }
    return hap::core::TLV8::encode(response);
}

// --- Operation Handlers ---

void AccessCodeManager::read_codes(const std::vector<hap::core::TLV>& request,
                                   const FragmentSink& append_fragment) {
    for (const auto& item : request) {
        if (item.type != AcTlv::TAG_CONTROL_REQUEST) continue;
        auto req_fragment = hap::core::TLV8::parse(item.value);
        uint16_t id = 0;
        if (extract_id(req_fragment, id)) {
            AccessCode* code = find_code_by_id(id);
            if (code) {
                append_fragment(build_code_response_fragment(*code));
            } else {
                AccessCode err_code{};
                err_code.id = id;
                err_code.status = AcTlv::STATUS_DOES_NOT_EXIST;
                append_fragment(build_code_response_fragment(err_code));
            }
        }
    }
}

uint8_t AccessCodeManager::validate_constraints(const std::string& code) {
    if (code.size() < min_code_length) return AcTlv::STATUS_TOO_SHORT;
    if (code.size() > max_code_length) return AcTlv::STATUS_TOO_LONG;
    if (!std::all_of(code.begin(), code.end(), [](char c){ return c >= '0' && c <= '9'; })) {
        return AcTlv::STATUS_INVALID_CHAR;
    }
    for (const auto& existing : access_codes) {
        if (existing.code == code) return AcTlv::STATUS_DUPLICATE;
    }
    return AcTlv::STATUS_SUCCESS;
}

void AccessCodeManager::add_code(const std::vector<hap::core::TLV>& request,
                                 const FragmentSink& append_fragment) {
    auto req_it = hap::core::TLV8::find(request, AcTlv::TAG_CONTROL_REQUEST);
    if (!req_it) return;

    auto data_tlv = hap::core::TLV8::parse(*req_it);
    auto code_val = hap::core::TLV8::find(data_tlv, AcTlv::TAG_ACCESS_CODE);
    if (!code_val) return;

    std::string new_code_str(code_val->begin(), code_val->end());

    uint8_t validation_status = validate_constraints(new_code_str);
    uint8_t status = validation_status;
    if (status == AcTlv::STATUS_SUCCESS && access_codes.size() >= max_access_codes) {
        ESP_LOGW(TAG, "Cannot add new code, max limit of %d reached.", max_access_codes);
        status = AcTlv::STATUS_EXCEEDED_MAX;
    }

    AccessCode result{};
    if (status == AcTlv::STATUS_SUCCESS) {
        AccessCode new_code;
        new_code.id = next_id++;
        new_code.code = new_code_str;
        new_code.flags = 0;
        new_code.status = AcTlv::STATUS_SUCCESS;
        access_codes.push_back(new_code);
        save_codes();
        ESP_LOGI(TAG, "Added new access code with ID: %d", new_code.id);
        result = new_code;
    } else {
        // Error responses carry the attempted code so the controller can show it
        result.code = new_code_str;
        result.status = status;
        ESP_LOGW(TAG, "Add rejected with status %d", status);
    }

    append_fragment(build_code_response_fragment(result));
}

void AccessCodeManager::remove_code(const std::vector<hap::core::TLV>& request,
                                    const FragmentSink& append_fragment) {
    auto req_it = hap::core::TLV8::find(request, AcTlv::TAG_CONTROL_REQUEST);
    if (!req_it) return;

    auto req_fragment = hap::core::TLV8::parse(*req_it);

    uint16_t id_to_remove = 0;
    if (!extract_id(req_fragment, id_to_remove)) return;

    auto it = std::find_if(access_codes.begin(), access_codes.end(),
        [id_to_remove](const AccessCode& ac){ return ac.id == id_to_remove; });

    AccessCode response_code;
    response_code.id = id_to_remove;

    if (it != access_codes.end()) {
        response_code = *it; // Copy data before erasing
        response_code.status = AcTlv::STATUS_SUCCESS;
        access_codes.erase(it);
        save_codes();
        ESP_LOGI(TAG, "Removed access code with ID: %d", id_to_remove);
    } else {
        response_code.status = AcTlv::STATUS_DOES_NOT_EXIST;
        ESP_LOGW(TAG, "Attempted to remove non-existent access code with ID: %d", id_to_remove);
    }

    append_fragment(build_code_response_fragment(response_code));
}

void AccessCodeManager::update_code(const std::vector<hap::core::TLV>& request,
                                    const FragmentSink& append_fragment) {
    auto req_it = hap::core::TLV8::find(request, AcTlv::TAG_CONTROL_REQUEST);
    if (!req_it) return;

    auto req_fragment = hap::core::TLV8::parse(*req_it);

    uint16_t id_to_update = 0;
    auto code_val = hap::core::TLV8::find(req_fragment, AcTlv::TAG_ACCESS_CODE);
    if (!extract_id(req_fragment, id_to_update) || !code_val) {
        // Invalid request, missing identifier or new code
        return;
    }

    AccessCode* code_to_update = find_code_by_id(id_to_update);

    if (code_to_update) {
        code_to_update->code.assign(code_val->begin(), code_val->end());
        code_to_update->status = AcTlv::STATUS_SUCCESS;
        save_codes();
        append_fragment(build_code_response_fragment(*code_to_update));
        ESP_LOGI(TAG, "Updated access code for ID: %d", id_to_update);
    } else {
        AccessCode error_response;
        error_response.id = id_to_update;
        error_response.status = AcTlv::STATUS_DOES_NOT_EXIST;
        append_fragment(build_code_response_fragment(error_response));
        ESP_LOGW(TAG, "Attempted to update non-existent access code with ID: %d", id_to_update);
    }
}


// --- Helper Function Implementations ---

std::vector<hap::core::TLV> AccessCodeManager::build_code_response_fragment(const AccessCode& code) {
    std::vector<hap::core::TLV> fragment;
    // Identifier is multi-byte little-endian so IDs above 255 survive round-trips
    const uint8_t* id_bytes = reinterpret_cast<const uint8_t*>(&code.id);
    fragment.emplace_back(AcTlv::TAG_IDENTIFIER, std::vector<uint8_t>(id_bytes, id_bytes + sizeof(code.id)));
    // Only add code string if status is success, otherwise it might not be available
    if (code.status == AcTlv::STATUS_SUCCESS && !code.code.empty()) {
        fragment.emplace_back(AcTlv::TAG_ACCESS_CODE, code.code);
    }
    fragment.emplace_back(AcTlv::TAG_FLAGS, code.flags);
    fragment.emplace_back(AcTlv::TAG_STATUS_CODE, code.status);
    return fragment;
}

AccessCode* AccessCodeManager::find_code_by_id(uint16_t id) {
    auto it = std::find_if(access_codes.begin(), access_codes.end(),
        [id](const AccessCode& ac){ return ac.id == id; });

    if (it != access_codes.end()) {
        return &(*it);
    }
    return nullptr;
}

bool AccessCodeManager::extract_id(const std::vector<hap::core::TLV>& fragment, uint16_t& id) {
    auto id_val = hap::core::TLV8::find(fragment, AcTlv::TAG_IDENTIFIER);
    if (!id_val || id_val->empty()) return false;
    // Accept 1- or 2-byte little-endian encodings
    id = static_cast<uint16_t>((*id_val)[0]);
    if (id_val->size() > 1) {
        id |= static_cast<uint16_t>((*id_val)[1]) << 8;
    }
    return true;
}
