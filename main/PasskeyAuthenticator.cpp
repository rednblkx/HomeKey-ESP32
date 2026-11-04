
#include "PasskeyAuthenticator.hpp"
#include "esp_log.h"

static const char* TAG = "PasskeyAuthenticator";

bool PasskeyAuthenticator::begin() {
    ESP_LOGI(TAG, "Passkey authenticator initialized");
    return true;
}

void PasskeyAuthenticator::onRequest(
    const uint8_t* data,
    size_t length
) {
    if (data == nullptr || length == 0) {
        ESP_LOGW(TAG, "Invalid request");
        return;
    }

    ESP_LOGI(TAG, "Request received: %u bytes",
             (unsigned int)length);
}

void PasskeyAuthenticator::process() {
    // USB CTAP2 processing will be added here.
}

void PasskeyAuthenticator::cancel() {
    ESP_LOGI(TAG, "Request cancelled");
}
