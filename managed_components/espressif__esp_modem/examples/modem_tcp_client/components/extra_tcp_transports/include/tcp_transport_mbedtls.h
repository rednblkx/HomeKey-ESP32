/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "esp_transport.h"

/**
 * @brief Initializes TLS transport based on mbetls wrapper
 *
 * @param parent Transport on top of which we run TLS layer
 * @return Transport handle on success
 */
esp_transport_handle_t esp_transport_tls_init(esp_transport_handle_t parent);
