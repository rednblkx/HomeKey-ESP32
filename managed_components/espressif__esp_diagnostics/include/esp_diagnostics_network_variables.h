/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

#if CONFIG_DIAG_ENABLE_NETWORK_VARIABLES
/**
 * @brief Initialize the network variables
 *
 * Below listed Wi-Fi and IP parameters are collected and reported to cloud on change.
 * Wi-Fi connection status, BSSID, SSID, channel, authentication mode,
 * Wi-Fi disconnection reason, IP address, netmask, and gateway.
 *
 * @return ESP_OK if successful, appropriate error code otherwise.
 */
esp_err_t esp_diag_network_variables_init(void);

/**
 * @brief Deinitialize the network variables
 */
esp_err_t esp_diag_network_variables_deinit(void);
#endif /* CONFIG_DIAG_ENABLE_NETWORK_VARIABLES */

#ifdef __cplusplus
}
#endif
