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

/**
 * @brief Color control attribute list
 */
typedef enum {
    ESP_ZB_ZCL_ATTR_COLOR_CONTROL_CURRENT_HUE_ID                           = 0x0000,  /*!< Current_HUE attribute */
    ESP_ZB_ZCL_ATTR_COLOR_CONTROL_CURRENT_SATURATION_ID                    = 0x0001,  /*!< Current Saturation attribute */
    ESP_ZB_ZCL_ATTR_COLOR_CONTROL_REMAINING_TIME_ID                        = 0x0002,  /*!< Remaining Time attribute */
    ESP_ZB_ZCL_ATTR_COLOR_CONTROL_CURRENT_X_ID                             = 0x0003,  /*!< CurrentX attribute */
    ESP_ZB_ZCL_ATTR_COLOR_CONTROL_CURRENT_Y_ID                             = 0x0004,  /*!< CurrentY attribute */
    ESP_ZB_ZCL_ATTR_COLOR_CONTROL_DRIFT_COMPENSATION_ID                    = 0x0005,  /*!< The DriftCompensation attribute indicates what mechanism */
    ESP_ZB_ZCL_ATTR_COLOR_CONTROL_COMPENSATION_TEXT_ID                     = 0x0006,  /*!< The CompensationText attribute holds a textual indication of what mechanism */
    ESP_ZB_ZCL_ATTR_COLOR_CONTROL_COLOR_TEMPERATURE_ID                     = 0x0007,  /*!< Color Temperature Mireds attribute */
    ESP_ZB_ZCL_ATTR_COLOR_CONTROL_COLOR_MODE_ID                            = 0x0008,  /*!< Color Mode attribute */
    ESP_ZB_ZCL_ATTR_COLOR_CONTROL_OPTIONS_ID                               = 0x000f,  /*!< The Options attribute is a bitmap that determines the default behavior of some cluster commands. */
    ESP_ZB_ZCL_ATTR_COLOR_CONTROL_ENHANCED_CURRENT_HUE_ID                  = 0x4000,  /*!< The EnhancedCurrentHue attribute represents non-equidistant steps along the CIE 1931 color triangle. */
    ESP_ZB_ZCL_ATTR_COLOR_CONTROL_ENHANCED_COLOR_MODE_ID                   = 0x4001,  /*!< The EnhancedColorMode attribute specifies which attributes are currently determining the color of the device. */
    ESP_ZB_ZCL_ATTR_COLOR_CONTROL_COLOR_LOOP_ACTIVE_ID                     = 0x4002,  /*!< The ColorLoopActive attribute specifies the current active status of the color loop. */
    ESP_ZB_ZCL_ATTR_COLOR_CONTROL_COLOR_LOOP_DIRECTION_ID                  = 0x4003,  /*!< The ColorLoopDirection attribute specifies the current direction of the color loop. */
    ESP_ZB_ZCL_ATTR_COLOR_CONTROL_COLOR_LOOP_TIME_ID                       = 0x4004,  /*!< The ColorLoopTime attribute specifies the number of seconds it SHALL take to perform a full color loop. */
    ESP_ZB_ZCL_ATTR_COLOR_CONTROL_COLOR_LOOP_START_ENHANCED_HUE_ID         = 0x4005,  /*!< The ColorLoopStartEnhancedHue attribute specifies the value of the EnhancedCurrentHue attribute from which the color loop SHALL be started. */
    ESP_ZB_ZCL_ATTR_COLOR_CONTROL_COLOR_LOOP_STORED_ENHANCED_HUE_ID        = 0x4006,  /*!< The ColorLoopStoredEnhancedHue attribute specifies the value of the EnhancedCurrentHue attribute before the color loop was stored. */
    ESP_ZB_ZCL_ATTR_COLOR_CONTROL_COLOR_CAPABILITIES_ID                    = 0x400a,  /*!< The ColorCapabilities attribute specifies the color capabilities of the device */
    ESP_ZB_ZCL_ATTR_COLOR_CONTROL_COLOR_TEMP_PHYSICAL_MIN_MIREDS_ID        = 0x400b,  /*!< The ColorTempPhysicalMinMireds attribute indicates the minimum mired value supported by the hardware. */
    ESP_ZB_ZCL_ATTR_COLOR_CONTROL_COLOR_TEMP_PHYSICAL_MAX_MIREDS_ID        = 0x400c,  /*!< The ColorTempPhysicalMaxMireds attribute indicates the maximum mired value supported by the hardware. */
    ESP_ZB_ZCL_ATTR_COLOR_CONTROL_COUPLE_COLOR_TEMP_TO_LEVEL_MIN_MIREDS_ID = 0x400d,  /*!< The CoupleColorTempToLevelMinMireds attribute specifies a lower bound on the value of the ColorTemperatureMireds attribute */
    ESP_ZB_ZCL_ATTR_COLOR_CONTROL_START_UP_COLOR_TEMPERATURE_MIREDS_ID     = 0x4010,  /*!< The StartUpColorTemperatureMireds attribute SHALL define the desired startup color temperature value a lamp SHALL use when it is supplied with power. */
} esp_zb_zcl_color_control_attr_t;


/** @brief Default value for CurrentHue attribute */
#define ESP_ZB_ZCL_COLOR_CONTROL_CURRENT_HUE_DEFAULT_VALUE ((uint8_t)0x00)

/** @brief Default value for CurrentSaturation attribute */
#define ESP_ZB_ZCL_COLOR_CONTROL_CURRENT_SATURATION_DEFAULT_VALUE ((uint8_t)0x00)

/** @brief Default value for RemainingTime attribute */
#define ESP_ZB_ZCL_COLOR_CONTROL_REMAINING_TIME_DEFAULT_VALUE ((uint16_t)0x00)

/** @brief Current X attribute default value */
#define ESP_ZB_ZCL_COLOR_CONTROL_CURRENT_X_DEF_VALUE            0x616b

/** @brief Current Y attribute default value */
#define ESP_ZB_ZCL_COLOR_CONTROL_CURRENT_Y_DEF_VALUE            0x607d

/** @brief Color Temperature default value (4000K) */
#define ESP_ZB_ZCL_COLOR_CONTROL_COLOR_TEMPERATURE_DEF_VALUE    0x00fa

/** @brief Default value for ColorMode attribute */
#define ESP_ZB_ZCL_COLOR_CONTROL_COLOR_MODE_DEFAULT_VALUE ((uint8_t)0x01)

/** @brief Default value for Options attribute */
#define ESP_ZB_ZCL_COLOR_CONTROL_OPTIONS_DEFAULT_VALUE ((uint8_t)0x00)

/** @brief Default value for EnhancedCurrentHue attribute */
#define ESP_ZB_ZCL_COLOR_CONTROL_ENHANCED_CURRENT_HUE_DEFAULT_VALUE ((uint16_t)0x0000)

/** @brief Default value for EnhancedColorMode attribute */
#define ESP_ZB_ZCL_COLOR_CONTROL_ENHANCED_COLOR_MODE_DEFAULT_VALUE ((uint8_t)0x01)

/** @brief Default value for ColorLoopActive attribute */
#define ESP_ZB_ZCL_COLOR_CONTROL_COLOR_LOOP_ACTIVE_DEFAULT_VALUE ((uint8_t)0x00)

/** @brief Default value for ColorLoopDirection attribute */
#define ESP_ZB_ZCL_COLOR_CONTROL_COLOR_LOOP_DIRECTION_DEFAULT_VALUE ((uint8_t)0x00)

/** @brief Color Loop Time attribute default value */
#define ESP_ZB_ZCL_COLOR_CONTROL_COLOR_LOOP_TIME_DEF_VALUE          0x0019

/** @brief Color Loop Start Enhanced Hue attribute default value */
#define ESP_ZB_ZCL_COLOR_CONTROL_COLOR_LOOP_START_DEF_VALUE         0x2300

/** @brief Default value for ColorLoopStoredEnhancedHue attribute */
#define ESP_ZB_ZCL_COLOR_CONTROL_COLOR_LOOP_STORED_ENHANCED_HUE_DEFAULT_VALUE ((uint16_t)0x0000)
/** @brief Default value for ColorCapabilities attribute */
#define ESP_ZB_ZCL_COLOR_CONTROL_COLOR_CAPABILITIES_DEFAULT_VALUE ((uint16_t)0x0000)

/** @brief Default value for ColorTempPhysicalMinMireds attribute */
#define ESP_ZB_ZCL_COLOR_CONTROL_COLOR_TEMP_PHYSICAL_MIN_MIREDS_DEFAULT_VALUE ((uint16_t)0x0000)

/** @brief Default value for ColorTempPhysicalMaxMireds attribute */
#define ESP_ZB_ZCL_COLOR_CONTROL_COLOR_TEMP_PHYSICAL_MAX_MIREDS_DEFAULT_VALUE ((uint16_t)0xFEFF)

/** @brief Color control cluster command identifiers
*/
typedef enum {
    ESP_ZB_ZCL_CMD_COLOR_CONTROL_MOVE_TO_HUE                      = 0x00,    /*!< Move To Hue command */
    ESP_ZB_ZCL_CMD_COLOR_CONTROL_MOVE_HUE                         = 0x01,    /*!< Move Hue command */
    ESP_ZB_ZCL_CMD_COLOR_CONTROL_STEP_HUE                         = 0x02,    /*!< Step Hue command */
    ESP_ZB_ZCL_CMD_COLOR_CONTROL_MOVE_TO_SATURATION               = 0x03,    /*!< Move To Saturation command */
    ESP_ZB_ZCL_CMD_COLOR_CONTROL_MOVE_SATURATION                  = 0x04,    /*!< Move Saturation command */
    ESP_ZB_ZCL_CMD_COLOR_CONTROL_STEP_SATURATION                  = 0x05,    /*!< Step Saturation command */
    ESP_ZB_ZCL_CMD_COLOR_CONTROL_MOVE_TO_HUE_SATURATION           = 0x06,    /*!< Move To Hue and Saturation command */
    ESP_ZB_ZCL_CMD_COLOR_CONTROL_MOVE_TO_COLOR                    = 0x07,    /*!< Move To Color command */
    ESP_ZB_ZCL_CMD_COLOR_CONTROL_MOVE_COLOR                       = 0x08,    /*!< Move Color command */
    ESP_ZB_ZCL_CMD_COLOR_CONTROL_STEP_COLOR                       = 0x09,    /*!< Step Color command */
    ESP_ZB_ZCL_CMD_COLOR_CONTROL_MOVE_TO_COLOR_TEMPERATURE        = 0x0a,    /*!< Move to color temperature command */
    ESP_ZB_ZCL_CMD_COLOR_CONTROL_ENHANCED_MOVE_TO_HUE             = 0x40,    /*!< Enhanced move to hue command */
    ESP_ZB_ZCL_CMD_COLOR_CONTROL_ENHANCED_MOVE_HUE                = 0x41,    /*!< Enhanced move hue command */
    ESP_ZB_ZCL_CMD_COLOR_CONTROL_ENHANCED_STEP_HUE                = 0x42,    /*!< Enhanced step hue command */
    ESP_ZB_ZCL_CMD_COLOR_CONTROL_ENHANCED_MOVE_TO_HUE_SATURATION  = 0x43,    /*!< Enhanced move to hue and saturation command */
    ESP_ZB_ZCL_CMD_COLOR_CONTROL_COLOR_LOOP_SET                   = 0x44,    /*!< Color loop set command */
    ESP_ZB_ZCL_CMD_COLOR_CONTROL_STOP_MOVE_STEP                   = 0x47,    /*!< Stop move step command */
    ESP_ZB_ZCL_CMD_COLOR_CONTROL_MOVE_COLOR_TEMPERATURE           = 0x4b,    /*!< Move color temperature command */
    ESP_ZB_ZCL_CMD_COLOR_CONTROL_STEP_COLOR_TEMPERATURE           = 0x4c,    /*!< Step color temperature command */
} esp_zb_zcl_color_control_cmd_id_t;

/** @brief Direction of Move to Hue defines
 */
typedef enum {
    ESP_ZB_ZCL_CMD_COLOR_CONTROL_MOVE_TO_HUE_SHORTEST = 0x00,   /*!< Shortest distance */
    ESP_ZB_ZCL_CMD_COLOR_CONTROL_MOVE_TO_HUE_LONGEST  = 0x01,   /*!< Longest distance */
    ESP_ZB_ZCL_CMD_COLOR_CONTROL_MOVE_TO_HUE_UP       = 0x02,   /*!< Up */
    ESP_ZB_ZCL_CMD_COLOR_CONTROL_MOVE_TO_HUE_DOWN     = 0x03,   /*!< Down */
} esp_zb_zcl_color_control_move_to_hue_direction_t;

/** @brief Direction of Move mode defines.
 */
typedef enum {
    ESP_ZB_ZCL_CMD_COLOR_CONTROL_MOVE_STOP              = 0x00, /*!< Stop */
    ESP_ZB_ZCL_CMD_COLOR_CONTROL_MOVE_UP                = 0x01, /*!< Up */
    ESP_ZB_ZCL_CMD_COLOR_CONTROL_MOVE_DOWN              = 0x03, /*!< Down */
} esp_zb_zcl_color_control_move_mode_t;

/** @brief Direction of Step mode defines.
 */
typedef enum {
    ESP_ZB_ZCL_CMD_COLOR_CONTROL_STEP_UP                = 0x01, /*!< Up */
    ESP_ZB_ZCL_CMD_COLOR_CONTROL_STEP_DOWN              = 0x03, /*!< Down */
} esp_zb_zcl_color_control_step_mode_t;

#ifdef __cplusplus
}
#endif
