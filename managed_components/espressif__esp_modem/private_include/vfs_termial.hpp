/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "cxx_include/esp_modem_dte.hpp"

struct esp_modem_dte_config;

namespace esp_modem {

std::unique_ptr<Terminal> create_vfs_terminal(const esp_modem_dte_config *config);

}  // namespace esp_modem
