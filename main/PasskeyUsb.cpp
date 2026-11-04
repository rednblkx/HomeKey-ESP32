
#include "PasskeyUsb.hpp"

#include <stdint.h>
#include "esp_log.h"
#include "tusb.h"

static const char* TAG = "PasskeyUsb";

// FIDO Alliance HID usage page and authenticator usage.
// 64-byte input and output reports.
static const uint8_t fido_hid_report_descriptor[] = {
    0x06, 0xD0, 0xF1,
    0x09, 0x01,
    0xA1, 0x01,
    0x09, 0x20,
    0x15, 0x00,
    0x26, 0xFF, 0x00,
    0x75, 0x08,
    0x95, 0x40,
    0x81, 0x02,
    0x09, 0x21,
    0x15, 0x00,
    0x26, 0xFF, 0x00,
    0x75, 0x08,
    0x95, 0x40,
    0x91, 0x02,
    0xC0
};

bool PasskeyUsb::begin() {
    ESP_LOGI(TAG, "FIDO HID descriptor prepared");
    return true;
}

void PasskeyUsb::process() {
    // CTAPHID packet handling will be implemented here.
}

void PasskeyUsb::onPacket(
    const uint8_t* data,
    size_t length
) {
    if (!data || length == 0) {
        ESP_LOGW(TAG, "Empty USB packet");
        return;
    }

    ESP_LOGI(TAG, "USB packet received: %u bytes",
             (unsigned int)length);
}

bool PasskeyUsb::sendPacket(
    const uint8_t* data,
    size_t length
) {
    if (!data || length != 64) {
        return false;
    }

    // TinyUSB HID report transmission goes here.
    return false;
}
