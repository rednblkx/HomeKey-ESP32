/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#pragma once
#include "esp_err.h"
#include "esp_netif.h"

/**
 * @brief Initialize PPP network (with a generic PPP server)
 * @param netif PPP netif
 * @return ESP_OK on success
 */
esp_err_t modem_init_network(esp_netif_t *netif);

/**
 * @brief Start PPP network
 */
void modem_start_network();

/**
 * @brief Stop PPP network
 */
void modem_stop_network();
