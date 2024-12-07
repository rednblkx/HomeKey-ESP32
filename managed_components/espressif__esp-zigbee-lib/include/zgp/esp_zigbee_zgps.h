/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "esp_err.h"
#include "esp_zigbee_type.h"
#include "zgp/esp_zigbee_zgp.h"
#include "zb_config.h"
#include "zb_config_common.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Set the security level for ZGP sink
 *
 * @param[in] level Security level
 */
void esp_zb_zgps_set_security_level(uint8_t level);

/**
 * @brief Set the mapping table for ZGP sink
 *
 * @param[in] table       The pointer of mapping entry of table, refer to esp_zb_zgps_mapping_entry_t
 * @param[in] table_size  The pointer of table size
 */
void esp_zb_zgps_set_mapping_table(const esp_zb_zgps_mapping_entry_t **table, uint16_t *table_size);

/**
 * @brief Set the communication mode for ZGP sink
 *
 * @param[in] mode The commission mode, refer to esp_zb_zgp_communication_mode_t
 */
void esp_zb_zgps_set_communication_mode(esp_zb_zgp_communication_mode_t mode);

/**
 * @brief Set the ZGP sink commissioning exit mode
 *
 * @param[in] mode The exit mode, refer to esp_zgp_commissioning_exit_mode_t
 */
void esp_zb_zgps_set_commissioning_exit_mode(esp_zgp_commissioning_exit_mode_t mode);

/**
 * @brief Set the ZGP sink commissioning window timeout
 *
 * @param[in] timeout The timeout of commissioning window
 */
void esp_zb_zgps_set_commissioning_window(uint16_t timeout);

/**
 * @brief Set the ZGP sink functionality
 *
 * @param[in] functionality         Functionality, refer to esp_zgp_gps_functionality_t
 * @param[in] active_functionality  Active functionality indicates which bits of functionality are active.
 */
void esp_zb_zgps_set_functionality(uint32_t functionality, uint32_t active_functionality);

/**
 * @brief Start the commissioning for ZGP sink on endpoint
 *
 * @param[in] endpoint The endpoint identifier for ZGP sink commissioning
 * @param[in] timeout  The timeout for commissioning
 */
void esp_zb_zgps_start_commissioning_on_endpoint(uint8_t endpoint, uint32_t timeout);

/**
 * @brief Stop commissioning for ZGP sink
 *
 */
void esp_zb_zgps_stop_commissioning();

/**
 * @brief Accept commissioning setting
 *
 * @param[in] accepted Whether accept the commissioning
 */
void esp_zb_zgps_accept_commissioning(bool accepted);

#ifdef __cplusplus
}
#endif
