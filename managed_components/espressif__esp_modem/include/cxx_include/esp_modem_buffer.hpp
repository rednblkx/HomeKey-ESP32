/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

namespace esp_modem {

/**
 * Common unique buffer, which is transferable between DTE and CMUX
 *
 */
struct unique_buffer {
    explicit unique_buffer(size_t size);
    unique_buffer (unique_buffer const &) = delete;
    unique_buffer &operator=(unique_buffer const &) = delete;
    unique_buffer(unique_buffer &&other) noexcept
    {
        data = std::move(other.data);
        size = other.size;
        consumed = 0;
    }
    unique_buffer &operator=(unique_buffer &&other) noexcept
    {
        if (&other == this) {
            return *this;
        }
        data = std::move(other.data);
        size = other.size;
        consumed = 0;
        return *this;
    }
    [[nodiscard]] uint8_t *get() const
    {
        return data.get();
    }

    std::unique_ptr<uint8_t[]> data;
    size_t size{};
    size_t consumed{};
};

}
