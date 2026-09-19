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

    // Bulk responses separate consecutive fragments with 2 zero bytes
    constexpr uint8_t PADDING[2] = {0x00, 0x00};
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

std::vector<uint8_t> AccessCodeManager::handle_write(TLV8& request_tlv) {
    TLV8_itc op_it = request_tlv.find(AcTlv::TAG_OPERATION);
    if (op_it == request_tlv.end()) {
        ESP_LOGE(TAG, "Access Code write is missing operation tag.");
        return {};
    }

    uint8_t operation = op_it->getVal<uint8_t>();
    ESP_LOGI(TAG, "Handling Access Code operation: %d", operation);

    TLV8 response_tlv;
    response_tlv.add(AcTlv::TAG_OPERATION, operation); // Response must include the operation

    switch (operation) {
        case AcTlv::OP_LIST:
            return list_codes(response_tlv);
        case AcTlv::OP_READ:
            return read_codes(request_tlv, response_tlv);
        case AcTlv::OP_ADD:
            return add_code(request_tlv, response_tlv);
        case AcTlv::OP_UPDATE:
            return update_code(request_tlv, response_tlv);
        case AcTlv::OP_REMOVE:
            return remove_code(request_tlv, response_tlv);
        default:
            ESP_LOGW(TAG, "Unhandled Access Code operation: %d", operation);
            return {};
    }
}

/**
 * @brief Packs the response TLV, inserting 2 zero bytes between consecutive
 *        control-response fragments (required by bulk List/Read operations).
 */
static std::vector<uint8_t> pack_response(TLV8& response) {
    TLV8 padded;
    size_t fragments = 0;
    for (auto it = response.begin(); it != response.end(); ++it) {
        if (it->getTag() == AcTlv::TAG_CONTROL_RESPONSE) {
            if (fragments > 0) {
                padded.add(0x00, sizeof(AcTlv::PADDING), AcTlv::PADDING);
            }
            fragments++;
        }
        TLV8 sub;
        sub.unpack(it->get(), it->getLen());
        padded.add(it->getTag(), sub);
    }
    padded.pack_init();
    std::vector<uint8_t> out(padded.pack_size());
    padded.pack(out.data());
    return out;
}

// --- Operation Handlers ---

std::vector<uint8_t> AccessCodeManager::list_codes(TLV8& response) {
    for (const auto& code_entry : access_codes) {
        TLV8 fragment = build_code_response_fragment(code_entry);
        response.add(AcTlv::TAG_CONTROL_RESPONSE, fragment);
    }
    return pack_response(response);
}

std::vector<uint8_t> AccessCodeManager::read_codes(TLV8& request, TLV8& response) {
    for (auto it = request.begin(); it != request.end(); ++it) {
        if (it->getTag() == AcTlv::TAG_CONTROL_REQUEST) {
            TLV8 req_fragment;
            req_fragment.unpack(it->get(), it->getLen());
            uint16_t id = 0;
            if (extract_id(req_fragment, id)) {
                AccessCode* code = find_code_by_id(id);
                if (code) {
                    TLV8 res_fragment = build_code_response_fragment(*code);
                    response.add(AcTlv::TAG_CONTROL_RESPONSE, res_fragment);
                } else {
                    AccessCode err_code{};
                    err_code.id = id;
                    err_code.status = AcTlv::STATUS_DOES_NOT_EXIST;
                    TLV8 res_fragment = build_code_response_fragment(err_code);
                    response.add(AcTlv::TAG_CONTROL_RESPONSE, res_fragment);
                }
            }
        }
    }
    return pack_response(response);
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

std::vector<uint8_t> AccessCodeManager::add_code(TLV8& request, TLV8& response) {
    TLV8_itc req_it = request.find(AcTlv::TAG_CONTROL_REQUEST);
    if (req_it == request.end()) return {};

    TLV8 data_tlv;
    data_tlv.unpack(req_it->get(), req_it->getLen());

    TLV8_itc code_it = data_tlv.find(AcTlv::TAG_ACCESS_CODE);
    if (code_it == data_tlv.end()) return {};

    std::string new_code_str(reinterpret_cast<const char*>(code_it->get()), code_it->getLen());

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

    TLV8 fragment = build_code_response_fragment(result);
    response.add(AcTlv::TAG_CONTROL_RESPONSE, fragment);
    return pack_response(response);
}

std::vector<uint8_t> AccessCodeManager::remove_code(TLV8& request, TLV8& response) {
    TLV8_itc req_it = request.find(AcTlv::TAG_CONTROL_REQUEST);
    if (req_it == request.end()) return {};

    TLV8 req_fragment;
    req_fragment.unpack(req_it->get(), req_it->getLen());

    uint16_t id_to_remove = 0;
    if (!extract_id(req_fragment, id_to_remove)) return {};

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

    TLV8 fragment = build_code_response_fragment(response_code);
    response.add(AcTlv::TAG_CONTROL_RESPONSE, fragment);
    return pack_response(response);
}

std::vector<uint8_t> AccessCodeManager::update_code(TLV8& request, TLV8& response) {
    TLV8_itc req_it = request.find(AcTlv::TAG_CONTROL_REQUEST);
    if (req_it == request.end()) return {};

    TLV8 req_fragment;
    req_fragment.unpack(req_it->get(), req_it->getLen());

    uint16_t id_to_update = 0;
    TLV8_itc code_it = req_fragment.find(AcTlv::TAG_ACCESS_CODE);
    if (!extract_id(req_fragment, id_to_update) || code_it == req_fragment.end()) {
        // Invalid request, missing identifier or new code
        return {};
    }

    AccessCode* code_to_update = find_code_by_id(id_to_update);

    if (code_to_update) {
        code_to_update->code.assign(reinterpret_cast<const char*>(code_it->get()), code_it->getLen());
        code_to_update->status = AcTlv::STATUS_SUCCESS;
        save_codes();
        TLV8 fragment = build_code_response_fragment(*code_to_update);
        response.add(AcTlv::TAG_CONTROL_RESPONSE, fragment);
        ESP_LOGI(TAG, "Updated access code for ID: %d", id_to_update);
    } else {
        AccessCode error_response;
        error_response.id = id_to_update;
        error_response.status = AcTlv::STATUS_DOES_NOT_EXIST;
        TLV8 fragment = build_code_response_fragment(error_response);
        response.add(AcTlv::TAG_CONTROL_RESPONSE, fragment);
        ESP_LOGW(TAG, "Attempted to update non-existent access code with ID: %d", id_to_update);
    }

    return pack_response(response);
}


// --- Helper Function Implementations ---

TLV8 AccessCodeManager::build_code_response_fragment(const AccessCode& code) {
    TLV8 fragment_tlv;
    // Identifier is multi-byte little-endian so IDs above 255 survive round-trips
    fragment_tlv.add(AcTlv::TAG_IDENTIFIER, sizeof(code.id), reinterpret_cast<const uint8_t*>(&code.id));
    // Only add code string if status is success, otherwise it might not be available
    if (code.status == AcTlv::STATUS_SUCCESS && !code.code.empty()) {
        fragment_tlv.add(AcTlv::TAG_ACCESS_CODE, code.code.size(), reinterpret_cast<const uint8_t*>(code.code.data()));
    }
    fragment_tlv.add(AcTlv::TAG_FLAGS, code.flags);
    fragment_tlv.add(AcTlv::TAG_STATUS_CODE, code.status);
    return fragment_tlv;
}

AccessCode* AccessCodeManager::find_code_by_id(uint16_t id) {
    auto it = std::find_if(access_codes.begin(), access_codes.end(),
        [id](const AccessCode& ac){ return ac.id == id; });

    if (it != access_codes.end()) {
        return &(*it);
    }
    return nullptr;
}

bool AccessCodeManager::extract_id(TLV8& fragment, uint16_t& id) {
    TLV8_itc id_it = fragment.find(AcTlv::TAG_IDENTIFIER);
    if (id_it == fragment.end()) return false;
    id = id_it->getVal<uint16_t>();
    return true;
}
