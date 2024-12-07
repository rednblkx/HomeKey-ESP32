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

/** @brief Scene cluster attribute identifiers
*/
typedef enum {
    ESP_ZB_ZCL_ATTR_SCENES_SCENE_COUNT_ID   = 0x0000,     /*!< Number of scenes currently in the device's scene table */
    ESP_ZB_ZCL_ATTR_SCENES_CURRENT_SCENE_ID = 0x0001,     /*!< Scene ID of the scene last invoked */
    ESP_ZB_ZCL_ATTR_SCENES_CURRENT_GROUP_ID = 0x0002,     /*!< Group ID of the scene last invoked */
    ESP_ZB_ZCL_ATTR_SCENES_SCENE_VALID_ID   = 0x0003,     /*!< Indicates whether the state of the device corresponds to CurrentScene and CurrentGroup attributes */
    ESP_ZB_ZCL_ATTR_SCENES_NAME_SUPPORT_ID  = 0x0004,     /*!< The most significant bit of the NameSupport attribute indicates whether or not scene names are supported */
    ESP_ZB_ZCL_ATTR_SCENES_LAST_CONFIGURED_BY_ID = 0x0005 /*!< specifies the IEEE address of the device that last configured the scene table */
} esp_zb_zcl_scene_attr_t;

/** @brief Default value for Scene count attribute */
#define ESP_ZB_ZCL_SCENES_SCENE_COUNT_DEFAULT_VALUE 0

/** @brief Default value for Current scene attribute */
#define ESP_ZB_ZCL_SCENES_CURRENT_SCENE_DEFAULT_VALUE 0

/** @brief Default value for Current group attribute */
#define ESP_ZB_ZCL_SCENES_CURRENT_GROUP_DEFAULT_VALUE 0

/** @brief Default value for Scene valid attribute */
#define ESP_ZB_ZCL_SCENES_SCENE_VALID_DEFAULT_VALUE 0

/** @brief Default value for Name support attribute */
#define ESP_ZB_ZCL_SCENES_NAME_SUPPORT_DEFAULT_VALUE 0

/** @brief Command identifiers for Scenes Cluster
*/
typedef enum {
    ESP_ZB_ZCL_CMD_SCENES_ADD_SCENE             = 0x00,         /*!< Add scene command */
    ESP_ZB_ZCL_CMD_SCENES_VIEW_SCENE            = 0x01,         /*!< View scene command */
    ESP_ZB_ZCL_CMD_SCENES_REMOVE_SCENE          = 0x02,         /*!< Remove scene command */
    ESP_ZB_ZCL_CMD_SCENES_REMOVE_ALL_SCENES     = 0x03,         /*!< Remove all scenes command */
    ESP_ZB_ZCL_CMD_SCENES_STORE_SCENE           = 0x04,         /*!< Store scene command */
    ESP_ZB_ZCL_CMD_SCENES_RECALL_SCENE          = 0x05,         /*!< Recall scene command */
    ESP_ZB_ZCL_CMD_SCENES_GET_SCENE_MEMBERSHIP  = 0x06,         /*!< Get scene membership command */
    ESP_ZB_ZCL_CMD_SCENES_ENHANCED_ADD_SCENE    = 0x40,         /*!< The Enhanced Add Scene command allows a scene to be added using a finer scene transition time than the Add Scene command. */
    ESP_ZB_ZCL_CMD_SCENES_ENHANCED_VIEW_SCENE   = 0x41,         /*!< The Enhanced View Scene command allows a scene to be retrieved using a finer scene transition time than the View Scene command. */
    ESP_ZB_ZCL_CMD_SCENES_COPY_SCENE            = 0x42,         /*!< The Copy Scene command allows a device to efficiently copy scenes from one group/scene identifier pair to another group/scene identifier pair. */
} esp_zb_zcl_scenes_cmd_id_t;

/** @brief Response command identifiers for Scenes Cluster
 */
typedef enum {
    ESP_ZB_ZCL_CMD_SCENES_ADD_SCENE_RESPONSE            = 0x00,   /*!< Add scene response */
    ESP_ZB_ZCL_CMD_SCENES_VIEW_SCENE_RESPONSE           = 0x01,   /*!< View scene response */
    ESP_ZB_ZCL_CMD_SCENES_REMOVE_SCENE_RESPONSE         = 0x02,   /*!< Remove scene response */
    ESP_ZB_ZCL_CMD_SCENES_REMOVE_ALL_SCENES_RESPONSE    = 0x03,   /*!< Remove all scenes response */
    ESP_ZB_ZCL_CMD_SCENES_STORE_SCENE_RESPONSE          = 0x04,   /*!< Store scene response */
    ESP_ZB_ZCL_CMD_SCENES_GET_SCENE_MEMBERSHIP_RESPONSE = 0x06,   /*!< Get scene membership response */
    ESP_ZB_ZCL_CMD_SCENES_ENHANCED_ADD_SCENE_RESPONSE   = 0x40,   /*!< The Enhanced Add Scene Response command allows a device to respond to an Enhanced Add Scene command.*/
    ESP_ZB_ZCL_CMD_SCENES_ENHANCED_VIEW_SCENE_RESPONSE  = 0x41,   /*!< The Enhanced View Scene Response command allows a device to respond to an Enhanced View Scene command 
                                                                        using a finer scene transition time that was available in the ZCL. */
    ESP_ZB_ZCL_CMD_SCENES_COPY_SCENE_RESPONSE           = 0x42,   /*!< The Copy Scene Response command allows a device to respond to a Copy Scene command.*/
} esp_zb_zcl_scenes_cmd_resp_id_t;

#ifdef __cplusplus
}
#endif
