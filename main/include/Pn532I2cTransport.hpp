#pragma once

#include "pn532_cxx/transport.hpp"

#include <array>
#include <cstddef>
#include <cstdint>
#include <driver/i2c_master.h>
#include <soc/gpio_num.h>

/**
 * @brief PN532 transport over the esp-idf i2c_master driver.
 *
 * pn532_hal's own I2cTransport can't be used here: it sits on the legacy
 * driver/i2c.h, and esp-idf aborts at boot when that is linked alongside the
 * i2c_master driver St25r3916Reader uses.
 *
 * Every PN532 I2C read starts with a status byte and has to fetch the frame in
 * one go, while pn532::Frontend parses a frame through several small reads.
 * The frame is therefore read once into m_rx and readChunk() serves from there.
 */
class Pn532I2cTransport : public pn532::Transport {
public:
    Pn532I2cTransport(gpio_num_t sda, gpio_num_t scl);
    ~Pn532I2cTransport() override;
    Pn532I2cTransport(const Pn532I2cTransport&) = delete;
    Pn532I2cTransport& operator=(const Pn532I2cTransport&) = delete;

    void swReset() override;
    void abort() override;
    pn532::Transaction begin() override;

protected:
    pn532::Status writeChunk(pn532::span<const uint8_t> data) override;
    bool waitReady(uint32_t timeout_ms) override;
    pn532::Status prepareRead() override;
    pn532::Status readChunk(pn532::span<uint8_t> buffer) override;
    void endTransaction() override;

private:
    // Status byte + the largest (extended) information frame.
    static constexpr size_t RX_BUFFER_SIZE = 276;

    i2c_master_bus_handle_t m_bus = nullptr;
    i2c_master_dev_handle_t m_dev = nullptr;

    std::array<uint8_t, RX_BUFFER_SIZE> m_rx{};
    size_t m_rxLen = 0;
    size_t m_rxPos = 0;

    static constexpr const char* TAG = "Pn532I2c";
};
