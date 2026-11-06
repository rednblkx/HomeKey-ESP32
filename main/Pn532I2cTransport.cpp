#include "Pn532I2cTransport.hpp"
#include "pn532_cxx/transaction.hpp"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include <cstring>

namespace {
constexpr uint16_t PN532_I2C_ADDR = 0x24;
constexpr uint32_t I2C_SPEED_HZ = 400000;
constexpr int XFER_TIMEOUT_MS = 100;
constexpr int STATUS_TIMEOUT_MS = 20;
constexpr uint8_t STATUS_READY = 0x01;
// The PN532 can NACK its address while it wakes from low-power mode.
constexpr int WRITE_ATTEMPTS = 3;
constexpr int WRITE_RETRY_DELAY_MS = 5;
constexpr size_t ACK_FRAME_LEN = 6;
constexpr uint8_t ACK_FRAME[ACK_FRAME_LEN] = {0x00, 0x00, 0xFF, 0x00, 0xFF, 0x00};
}

Pn532I2cTransport::Pn532I2cTransport(gpio_num_t sda, gpio_num_t scl) {
    i2c_master_bus_config_t buscfg = {};
    buscfg.i2c_port = -1;
    buscfg.sda_io_num = sda;
    buscfg.scl_io_num = scl;
    buscfg.clk_source = I2C_CLK_SRC_DEFAULT;
    buscfg.glitch_ignore_cnt = 7;
    buscfg.flags.enable_internal_pullup = true;

    esp_err_t err = i2c_new_master_bus(&buscfg, &m_bus);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "i2c_new_master_bus failed: %s", esp_err_to_name(err));
        return;
    }

    i2c_device_config_t devcfg = {};
    devcfg.dev_addr_length = I2C_ADDR_BIT_LEN_7;
    devcfg.device_address = PN532_I2C_ADDR;
    devcfg.scl_speed_hz = I2C_SPEED_HZ;

    err = i2c_master_bus_add_device(m_bus, &devcfg, &m_dev);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "i2c_master_bus_add_device failed: %s", esp_err_to_name(err));
        m_dev = nullptr;
    }
}

Pn532I2cTransport::~Pn532I2cTransport() {
    if (m_dev) {
        i2c_master_bus_rm_device(m_dev);
    }
    if (m_bus) {
        i2c_del_master_bus(m_bus);
    }
}

void Pn532I2cTransport::swReset() {
    abort();
}

void Pn532I2cTransport::abort() {
    m_rxLen = 0;
    m_rxPos = 0;
    // An ACK frame cancels whatever command the PN532 is still processing.
    (void)writeChunk({ACK_FRAME, ACK_FRAME_LEN});
    vTaskDelay(pdMS_TO_TICKS(2));
}

pn532::Transaction Pn532I2cTransport::begin() {
    return pn532::Transaction(*this, m_dev != nullptr);
}

pn532::Status Pn532I2cTransport::writeChunk(pn532::span<const uint8_t> data) {
    if (!m_dev) {
        return pn532::Status::TRANSPORT_ERROR;
    }
    esp_err_t err = ESP_FAIL;
    for (int attempt = 0; attempt < WRITE_ATTEMPTS; ++attempt) {
        if (attempt > 0) {
            vTaskDelay(pdMS_TO_TICKS(WRITE_RETRY_DELAY_MS));
        }
        err = i2c_master_transmit(m_dev, data.data(), data.size(), XFER_TIMEOUT_MS);
        if (err == ESP_OK) {
            return pn532::Status::SUCCESS;
        }
    }
    ESP_LOGE(TAG, "I2C write failed: %s", esp_err_to_name(err));
    return pn532::Status::TRANSPORT_ERROR;
}

bool Pn532I2cTransport::waitReady(uint32_t timeout_ms) {
    if (!m_dev) {
        return false;
    }
    const TickType_t start = xTaskGetTickCount();
    const TickType_t timeout = pdMS_TO_TICKS(timeout_ms);
    do {
        uint8_t status = 0;
        if (i2c_master_receive(m_dev, &status, 1, STATUS_TIMEOUT_MS) == ESP_OK &&
            (status & STATUS_READY)) {
            return true;
        }
        vTaskDelay(pdMS_TO_TICKS(1));
    } while (xTaskGetTickCount() - start < timeout);
    return false;
}

pn532::Status Pn532I2cTransport::prepareRead() {
    m_rxLen = 0;
    m_rxPos = 0;
    return pn532::Status::SUCCESS;
}

pn532::Status Pn532I2cTransport::readChunk(pn532::span<uint8_t> buffer) {
    if (m_rxLen == 0) {
        if (!m_dev) {
            return pn532::Status::TRANSPORT_ERROR;
        }
        // Only an ACK has a known length up front. A response frame's length
        // isn't known until it has been read, so fetch the maximum; the PN532
        // tolerates reads past the end of a frame.
        const size_t want = buffer.size() == ACK_FRAME_LEN ? ACK_FRAME_LEN + 1 : m_rx.size();
        esp_err_t err = i2c_master_receive(m_dev, m_rx.data(), want, XFER_TIMEOUT_MS);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "I2C read failed: %s", esp_err_to_name(err));
            return pn532::Status::TRANSPORT_ERROR;
        }
        if (!(m_rx[0] & STATUS_READY)) {
            ESP_LOGE(TAG, "Read frame while PN532 was not ready");
            return pn532::Status::TRANSPORT_ERROR;
        }
        m_rxLen = want;
        m_rxPos = 1; // skip the status byte
    }
    if (buffer.size() > m_rxLen - m_rxPos) {
        ESP_LOGE(TAG, "Read of %u bytes runs past the received frame", (unsigned)buffer.size());
        return pn532::Status::TRANSPORT_ERROR;
    }
    memcpy(buffer.data(), m_rx.data() + m_rxPos, buffer.size());
    m_rxPos += buffer.size();
    return pn532::Status::SUCCESS;
}

void Pn532I2cTransport::endTransaction() {
    // Each I2C transfer is self-contained, nothing to release.
}
