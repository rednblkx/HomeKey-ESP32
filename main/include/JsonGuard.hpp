#pragma once

#include <cJSON.h>
#include <string>

/**
 * @brief RAII wrapper for cJSON objects to ensure proper cleanup.
 * 
 * This class manages the lifecycle of a cJSON pointer, automatically
 * calling cJSON_Delete when the guard goes out of scope.
 */
class JsonGuard {
    cJSON* ptr;

public:
    explicit JsonGuard(cJSON* p = nullptr) : ptr(p) {}
    
    ~JsonGuard() { 
        if(ptr) cJSON_Delete(ptr); 
    }
    
    // Disable copy operations
    JsonGuard(const JsonGuard&) = delete;
    JsonGuard& operator=(const JsonGuard&) = delete;
    
    // Enable move operations
    JsonGuard(JsonGuard&& other) noexcept : ptr(other.ptr) { 
        other.ptr = nullptr; 
    }
    
    JsonGuard& operator=(JsonGuard&& other) noexcept {
        if (this != &other) {
            if(ptr) cJSON_Delete(ptr);
            ptr = other.ptr;
            other.ptr = nullptr;
        }
        return *this;
    }
    
    cJSON* get() const { return ptr; }
    cJSON* operator->() const { return ptr; }
    explicit operator bool() const { return ptr != nullptr; }
    
    cJSON* release() {
        cJSON* tmp = ptr;
        ptr = nullptr;
        return tmp;
    }
    
    void reset(cJSON* p = nullptr) {
        if(ptr) cJSON_Delete(ptr);
        ptr = p;
    }
    
    /**
     * @brief Convert to unformatted JSON string and cleanup.
     * @return JSON string (empty if ptr is null)
     */
    std::string toStringUnformatted() const {
        if (!ptr) return "";
        char* raw = cJSON_PrintUnformatted(ptr);
        std::string result = raw ? std::string(raw) : std::string{};
        free(raw);
        return result;
    }
    
    /**
     * @brief Convert to formatted JSON string and cleanup.
     * @return JSON string (empty if ptr is null)
     */
    std::string toStringFormatted() const {
        if (!ptr) return "";
        char* raw = cJSON_Print(ptr);
        std::string result = raw ? std::string(raw) : std::string{};
        free(raw);
        return result;
    }
};

/**
 * @brief Builder pattern for constructing JSON objects.
 * 
 * Provides a fluent interface for building JSON objects with RAII cleanup.
 */
class JsonBuilder {
    JsonGuard guard;

public:
    JsonBuilder() : guard(cJSON_CreateObject()) {}
    explicit JsonBuilder(cJSON* array) : guard(array) {}
    
    static JsonBuilder object() {
        return JsonBuilder();
    }
    
    static JsonBuilder array() {
        return JsonBuilder(cJSON_CreateArray());
    }
    
    JsonBuilder& addString(const char* key, const std::string& value) {
        if (guard) cJSON_AddStringToObject(guard.get(), key, value.c_str());
        return *this;
    }
    
    JsonBuilder& addString(const char* key, const char* value) {
        if (guard) cJSON_AddStringToObject(guard.get(), key, value);
        return *this;
    }
    
    JsonBuilder& addBool(const char* key, bool value) {
        if (guard) cJSON_AddBoolToObject(guard.get(), key, value);
        return *this;
    }
    
    JsonBuilder& addNumber(const char* key, double value) {
        if (guard) cJSON_AddNumberToObject(guard.get(), key, value);
        return *this;
    }
    
    JsonBuilder& addItem(const char* key, cJSON* item) {
        if (guard && item) cJSON_AddItemToObject(guard.get(), key, item);
        return *this;
    }
    
    JsonBuilder& addItemToArray(cJSON* item) {
        if (guard && item) cJSON_AddItemToArray(guard.get(), item);
        return *this;
    }
    
    JsonBuilder& addArray(const char* key) {
        if (guard) cJSON_AddArrayToObject(guard.get(), key);
        return *this;
    }
    
    cJSON* release() {
        return guard.release();
    }
    
    std::string toStringUnformatted() const {
        return guard.toStringUnformatted();
    }
    
    std::string toStringFormatted() const {
        return guard.toStringFormatted();
    }
};

/**
 * @brief Helper function to convert cJSON to string and delete the object.
 * @param obj cJSON object to stringify and delete
 * @return JSON string (empty if obj is null)
 */
inline std::string cjson_to_string_and_free(cJSON* obj) {
    JsonGuard guard(obj);
    return guard.toStringUnformatted();
}
