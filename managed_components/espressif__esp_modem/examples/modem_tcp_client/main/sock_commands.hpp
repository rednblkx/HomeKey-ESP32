/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "cxx_include/esp_modem_dte.hpp"
#include "cxx_include/esp_modem_dce_module.hpp"
#include "cxx_include/esp_modem_types.hpp"
#include "socket_commands.inc"

namespace sock_commands {

#define ESP_MODEM_DECLARE_DCE_COMMAND(name, return_type, num, ...) \
        esp_modem::return_type name(esp_modem::CommandableIf *t, ## __VA_ARGS__);

DECLARE_SOCK_COMMANDS(declare name(Commandable *p, ...);)

#undef ESP_MODEM_DECLARE_DCE_COMMAND

}
