/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "esp_transport.h"
#include "sock_dce.hpp"

/**
 * @brief Initializes TCP transport based on AT commands
 *
 * @param DCE
 * @return Transport handle on success
 */
esp_transport_handle_t esp_transport_at_init(sock_dce::DCE *dce);
