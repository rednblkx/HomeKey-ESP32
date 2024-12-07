/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

/*
 * softAP to PPPoS Example (network_dce)
*/

#pragma once

#ifdef __cplusplus
extern "C" {
#endif


/**
 * @brief Initialize a singleton covering the PPP network provided by the connected modem device
 *
 * @param netif Already created network interface in PPP mode
 *
 * @return ESP_OK on success
 */
esp_err_t modem_init_network(esp_netif_t *netif);

/**
 * @brief Destroys the single network DCE
 */
void modem_deinit_network();

/**
 * @brief Starts the PPP network
 */
bool modem_start_network();

/**
 * @brief Stops the PPP network
 */
bool modem_stop_network();

bool modem_check_sync();

void modem_reset();

bool modem_check_signal();

#ifdef __cplusplus
}
#endif
