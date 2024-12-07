/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once
#ifdef __cplusplus
extern "C" {
#endif

#include "esp_zigbee_type.h"

/** @brief Groups cluster attribute identifiers
*/
typedef enum {
    ESP_ZB_ZCL_ATTR_GROUPS_NAME_SUPPORT_ID = 0              /*!< NameSupport attribute */
} esp_zb_zcl_groups_attr_t;

/** @brief Default value for groups attribute
*/
#define ESP_ZB_ZCL_GROUPS_NAME_SUPPORT_DEFAULT_VALUE 0x00

/**
 *  @brief Groups cluster command identifiers.
 */
typedef enum {
    ESP_ZB_ZCL_CMD_GROUPS_ADD_GROUP                = 0x00,  /*!< Add group command identifier. */
    ESP_ZB_ZCL_CMD_GROUPS_VIEW_GROUP               = 0x01,  /*!< View group command identifier. */
    ESP_ZB_ZCL_CMD_GROUPS_GET_GROUP_MEMBERSHIP     = 0x02,  /*!< Get group membership command identifier.*/
    ESP_ZB_ZCL_CMD_GROUPS_REMOVE_GROUP             = 0x03,  /*!< Remove group command identifier. */
    ESP_ZB_ZCL_CMD_GROUPS_REMOVE_ALL_GROUPS        = 0x04,  /*!< Remove all groups command identifier. */
    ESP_ZB_ZCL_CMD_GROUPS_ADD_GROUP_IF_IDENTIFYING = 0x05   /*!< Add group if identifying command identifier. */
} esp_zb_zcl_groups_cmd_id_t;

#ifdef __cplusplus
}
#endif
