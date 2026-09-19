#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include "TLV8.h"

/**
 * @brief Represents a stored access code.
 */
struct AccessCode {
    uint16_t id;
    std::string code;
    uint8_t flags;
    uint8_t status;
};

/**
 * @class AccessCodeManager
 * @brief Manages HomeKit Access Code entries behind the Access Code service.
 *
 * Implements the Access Code control-point TLV8 protocol (LIST/READ/ADD/
 * UPDATE/REMOVE operations), persists the code list to NVS, and validates
 * codes entered on the keypad.
 */
class AccessCodeManager {
public:
    static constexpr uint8_t MIN_CODE_LENGTH = 4;
    static constexpr uint8_t MAX_CODE_LENGTH = 16;
    static constexpr uint8_t MAX_ACCESS_CODES = 8;

    /**
     * @brief Construct a new Access Code Manager object.
     * @param min_length Minimum accepted access code length.
     * @param max_length Maximum accepted access code length.
     * @param max_codes The maximum number of codes that can be stored.
     */
    AccessCodeManager(uint8_t min_length = MIN_CODE_LENGTH,
                      uint8_t max_length = MAX_CODE_LENGTH,
                      uint8_t max_codes = MAX_ACCESS_CODES);

    /**
     * @brief Loads access codes from NVS. Must be called after construction.
     */
    void init();

    /**
     * @brief Processes a control-point write from the HomeKit controller.
     * @param request TLV8 containing the request payload.
     * @return A vector<uint8_t> containing the full TLV8 response.
     */
    std::vector<uint8_t> handle_write(TLV8& request);

    /**
     * @brief Validates if a given code string exists.
     * @param code The code to check.
     * @return true if the code is valid, false otherwise.
     */
    bool validateCode(const std::string& code) const;

    /**
     * @brief Deletes all stored access codes and erases them from NVS.
     */
    void purge_codes();

    // Configured constraints (exposed for the supported-configuration characteristic)
    uint8_t minCodeLength() const { return min_code_length; }
    uint8_t maxCodeLength() const { return max_code_length; }
    uint8_t maxCodes() const { return max_access_codes; }

private:
    const char* TAG = "AccessCodeManager";
    uint8_t min_code_length;
    uint8_t max_code_length;
    uint8_t max_access_codes;
    std::vector<AccessCode> access_codes;
    uint16_t next_id = 1; // Keep track of the next available ID
    const char* nvs_namespace = "acc_codes";
    const char* nvs_key = "access_codes";

    // --- Operation handlers ---
    std::vector<uint8_t> list_codes(TLV8& response);
    std::vector<uint8_t> read_codes(TLV8& request, TLV8& response);
    std::vector<uint8_t> add_code(TLV8& request, TLV8& response);
    std::vector<uint8_t> remove_code(TLV8& request, TLV8& response);
    std::vector<uint8_t> update_code(TLV8& request, TLV8& response);

    /**
     * @brief Validates a code against the supported configuration constraints.
     * @return STATUS_SUCCESS, or the first matching validation failure status.
     */
    uint8_t validate_constraints(const std::string& code);

    // --- Helper functions ---
    /**
     * @brief Builds a single "Access Code Control Response" TLV fragment.
     * @param code The access code to serialize.
     * @return A TLV8 object representing the fragment.
     */
    TLV8 build_code_response_fragment(const AccessCode& code);

    /**
     * @brief Finds an access code by its 1-based identifier.
     * @param id The identifier to search for.
     * @return A pointer to the AccessCode if found, otherwise nullptr.
     */
    AccessCode* find_code_by_id(uint16_t id);

    /**
     * @brief Saves the current list of access codes to NVS.
     * @return true on success, false on failure.
     */
    bool save_codes();

    /**
     * @brief Extracts an identifier from a request fragment, accepting 1- or 2-byte encodings.
     * @param fragment TLV8 fragment containing the identifier tag.
     * @param id Output identifier.
     * @return true if the identifier tag was present.
     */
    static bool extract_id(TLV8& fragment, uint16_t& id);
};
