#include "ReaderDataManager.hpp"
#include "config.hpp"
#include <ArduinoJson.hpp>
#include <nvs_flash.h>
#include <esp_log.h>
#include <algorithm>

const char* ReaderDataManager::TAG = "ReaderDataManager";
const char* ReaderDataManager::NVS_KEY = "READERDATA";

ReaderDataManager::ReaderDataManager() : m_isInitialized(false) {
}

ReaderDataManager::~ReaderDataManager() {
    if (m_isInitialized) {
        nvs_close(m_nvsHandle);
    }
}

bool ReaderDataManager::begin() {
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

const readerData_t& ReaderDataManager::getReaderData() const {
    return m_readerData;
}

const std::vector<uint8_t>& ReaderDataManager::getReaderGid() const {
    return m_readerData.reader_gid;
}

const std::vector<uint8_t>& ReaderDataManager::getReaderId() const {
    return m_readerData.reader_id;
}

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
    
    ArduinoJson::JsonDocument doc;
    ArduinoJson::deserializeMsgPack(doc, buffer);
    m_readerData = doc.as<readerData_t>();
    if (doc.isNull()) {
        ESP_LOGE(TAG, "Failed to parse reader data msgpack. Data may be corrupt.");
        return;
    }
    
    ESP_LOGI(TAG, "Successfully loaded reader data from NVS.");
}

const readerData_t* ReaderDataManager::saveData() {
    if (!m_isInitialized) {
        ESP_LOGE(TAG, "Cannot save, not initialized.");
        return nullptr;
    }
    
    ByteWriter serialized_data;
    ArduinoJson::JsonDocument doc;
    doc = m_readerData;
    serializeMsgPack(doc, serialized_data);

    esp_err_t set_err = nvs_set_blob(m_nvsHandle, NVS_KEY, serialized_data, serialized_data);
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

const readerData_t* ReaderDataManager::updateReaderData(const readerData_t& newData) {
    m_readerData = newData;
    return saveData();
}

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
