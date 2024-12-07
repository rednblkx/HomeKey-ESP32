// Copyright 2021-2022 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
#pragma once

#include "generate/esp_modem_command_declare_helper.inc"

#define DECLARE_SOCK_COMMANDS(...) \
/**
 * @brief Opens network in AT command mode
 * @return OK, FAIL or TIMEOUT
 */ \
ESP_MODEM_DECLARE_DCE_COMMAND(net_open, command_result, 0) \
    \
/**
 * @brief Closes network in AT command mode
 * @return OK, FAIL or TIMEOUT
 */ \
ESP_MODEM_DECLARE_DCE_COMMAND(net_close, command_result, 0) \
    \
/**
 * @brief Opens a TCP connection
 * @param[in] host Host name or IP address to connect to
 * @param[in] port Port number
 * @param[in] timeout Connection timeout
 * @return OK, FAIL or TIMEOUT
 */ \
ESP_MODEM_DECLARE_DCE_COMMAND(tcp_open, command_result, 3, STRING_IN(p1, host), INT_IN(p2, port), INT_IN(p3, timeout)) \
    \
/**
 * @brief Closes opened TCP socket
 * @return OK, FAIL or TIMEOUT
 */ \
ESP_MODEM_DECLARE_DCE_COMMAND(tcp_close, command_result, 0) \
    \
/**
 * @brief Gets modem IP address
 * @param[out] addr String representation of modem's IP
 * @return OK, FAIL or TIMEOUT
 */ \
ESP_MODEM_DECLARE_DCE_COMMAND(get_ip, command_result, 1, STRING_OUT(p1, addr)) \
    \
/**
 * @brief Sets Rx mode
 * @param[in] mode 0=auto, 1=manual
 * @return OK, FAIL or TIMEOUT
 */ \
ESP_MODEM_DECLARE_DCE_COMMAND(set_rx_mode, command_result, 1, INT_IN(p1, mode))
