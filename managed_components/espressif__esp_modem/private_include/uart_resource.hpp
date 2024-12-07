/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "cxx_include/esp_modem_dte.hpp"
#include "esp_modem_config.h"

struct esp_modem_uart_term_config;

namespace esp_modem {

/**
 * @brief Uart Resource is a platform specific struct which is implemented separately for ESP_PLATFORM and linux target
 */
struct uart_resource {
    explicit uart_resource(const esp_modem_uart_term_config *config, QueueHandle_t *event_queue, int fd);

    ~uart_resource();

    uart_port_t port{};
};


}  // namespace esp_modem
