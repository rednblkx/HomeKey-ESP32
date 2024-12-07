/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "esp_err.h"
#include "generate/esp_modem_command_declare.inc"
#include "esp_modem_c_api_types.h"

#ifdef __cplusplus
extern "C" {
#endif


#define ESP_MODEM_DECLARE_DCE_COMMAND(name, return_type, num, ...) \
        esp_err_t esp_modem_ ## name(esp_modem_dce_t *dce, ##__VA_ARGS__);

DECLARE_ALL_COMMAND_APIS(declares esp_modem_<API>(esp_modem_t *dce, ...);)

#undef ESP_MODEM_DECLARE_DCE_COMMAND


#ifdef __cplusplus
}
#endif
