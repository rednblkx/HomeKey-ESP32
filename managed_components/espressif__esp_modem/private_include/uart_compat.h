/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "esp_log.h"
#include "esp_idf_version.h"
#include "driver/uart.h"

/**
 * @brief This is a compatible header, which just takes care of different data ptr type
 * across different IDF version in driver/uart
 */
static inline int uart_write_bytes_compat(uart_port_t uart_num, const void *src, size_t size)
{
#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(4, 3, 0)
    const void *data = src;
#else
    auto *data = reinterpret_cast<const char *>(src);
#endif
    return uart_write_bytes(uart_num, data, size);
}
