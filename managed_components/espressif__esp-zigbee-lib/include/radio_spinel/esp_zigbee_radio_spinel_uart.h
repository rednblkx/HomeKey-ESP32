/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "esp_err.h"
#include "zb_config_platform.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Configure the radio spinel uart
 * 
 * @param[in] radio_config Radio spinel UART interface configuration
 * @return
 *      - ESP_OK: On success, otherwise, failure.
 */
esp_err_t esp_zb_radio_spinel_config_uart(const esp_zb_uart_config_t *uart_config);

#ifdef __cplusplus
} // extern "C"
#endif
