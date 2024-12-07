/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <memory>
#include <functional>
#include <exception>
#include <cstddef>
#include <cstdint>
#include <utility>
#include "esp_err.h"
#include "esp_modem_primitives.hpp"

namespace esp_modem {

/**
 * @defgroup ESP_MODEM_TERMINAL
 * @brief Definition of an abstract terminal to be attached to DTE class
 */

/** @addtogroup ESP_MODEM_TERMINAL
* @{
*/

/**
 * @brief Terminal errors
 */
enum class terminal_error {
    BUFFER_OVERFLOW,
    CHECKSUM_ERROR,
    UNEXPECTED_CONTROL_FLOW,
    DEVICE_GONE,
};

/**
 * @brief Terminal interface. All communication interfaces must comply to this interface in order to be used as a DTE
 */
class Terminal {
public:
    virtual ~Terminal() = default;

    void set_error_cb(std::function<void(terminal_error)> f)
    {
        on_error = std::move(f);
    }

    virtual void set_read_cb(std::function<bool(uint8_t *data, size_t len)> f)
    {
        on_read = std::move(f);
    }

    /**
     * @brief Writes data to the terminal
     * @param data Data pointer
     * @param len Data len
     * @return length of data written
     */
    virtual int write(uint8_t *data, size_t len) = 0;

    /**
     * @brief Read from the terminal. This function doesn't block, but return all available data.
     * @param data Data pointer to store the read payload
     * @param len Maximum data len to read
     * @return length of data actually read
     */
    virtual int read(uint8_t *data, size_t len) = 0;

    virtual void start() = 0;

    virtual void stop() = 0;

protected:
    std::function<bool(uint8_t *data, size_t len)> on_read;
    std::function<void(terminal_error)> on_error;
};

/**
 * @}
 */

} // namespace esp_modem
