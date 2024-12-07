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
 * @brief Level control attribute list
 */
typedef enum {
    ESP_ZB_ZCL_ATTR_LEVEL_CONTROL_CURRENT_LEVEL_ID          = 0x0000,  /*!< Current Level attribute */
    ESP_ZB_ZCL_ATTR_LEVEL_CONTROL_REMAINING_TIME_ID         = 0x0001,  /*!< Remaining Time attribute */
    ESP_ZB_ZCL_ATTR_LEVEL_CONTROL_MIN_LEVEL_ID              = 0x0002,  /*!< The MinLevel attribute indicates the minimum value of CurrentLevel that is capable of being assigned. */
    ESP_ZB_ZCL_ATTR_LEVEL_CONTROL_MAX_LEVEL_ID              = 0x0003,  /*!< The MaxLevel attribute indicates the maximum value of CurrentLevel that is capable of being assigned. */
    ESP_ZB_ZCL_ATTR_LEVEL_CONTROL_CURRENT_FREQUENCY_ID      = 0x0004,  /*!< The CurrentFrequency attribute represents the frequency that the devices is at CurrentLevel. */
    ESP_ZB_ZCL_ATTR_LEVEL_CONTROL_MIN_FREQUENCY_ID          = 0x0005,  /*!< The MinFrequency attribute indicates the minimum value of CurrentFrequency that is capable of being assigned. */
    ESP_ZB_ZCL_ATTR_LEVEL_CONTROL_MAX_FREQUENCY_ID          = 0x0006,  /*!< The MaxFrequency attribute indicates the maximum value of CurrentFrequency that is capable of being assigned. */
    ESP_ZB_ZCL_ATTR_LEVEL_CONTROL_ON_OFF_TRANSITION_TIME_ID = 0x0010,  /*!< On off transition time attribute */
    ESP_ZB_ZCL_ATTR_LEVEL_CONTROL_ON_LEVEL_ID               = 0x0011,  /*!< On Level attribute */
    ESP_ZB_ZCL_ATTR_LEVEL_CONTROL_ON_TRANSITION_TIME_ID     = 0x0012,  /*!< The OnTransitionTime attribute represents the time taken to move the current level */
    ESP_ZB_ZCL_ATTR_LEVEL_CONTROL_OFF_TRANSITION_TIME_ID    = 0x0013,  /*!< The OffTransitionTime attribute represents the time taken to move the current level */
    ESP_ZB_ZCL_ATTR_LEVEL_CONTROL_DEFAULT_MOVE_RATE_ID      = 0x0014,  /*!< The DefaultMoveRate attribute determines the movement rate, in units per second */
    ESP_ZB_ZCL_ATTR_LEVEL_CONTROL_OPTIONS_ID                = 0x000F,  /*!< The Options attribute is a bitmap that determines the default behavior of some cluster commands. */
    ESP_ZB_ZCL_ATTR_LEVEL_CONTROL_START_UP_CURRENT_LEVEL_ID = 0x4000,  /*!< The StartUpCurrentLevel attribute SHALL define the desired startup level */
    ESP_ZB_ZCL_ATTR_LEVEL_CONTROL_MOVE_STATUS_ID            = 0xefff   /*!< Special Move Variables attribute Internal usage */
} esp_zb_zcl_level_control_attr_t;

/** @brief Current Level attribute default value */
#define ESP_ZB_ZCL_LEVEL_CONTROL_CURRENT_LEVEL_DEFAULT_VALUE ((uint8_t)0xFF)

/** @brief Default value for RemainingTime attribute */
#define ESP_ZB_ZCL_LEVEL_CONTROL_REMAINING_TIME_DEFAULT_VALUE ((uint16_t)0x0000)

/** @brief Default value for MinLevel attribute */
#define ESP_ZB_ZCL_LEVEL_CONTROL_MIN_LEVEL_DEFAULT_VALUE ((uint8_t)0x00)

/** @brief Default value for MaxLevel attribute */
#define ESP_ZB_ZCL_LEVEL_CONTROL_MAX_LEVEL_DEFAULT_VALUE ((uint8_t)0xFF)

/** @brief Default value for CurrentFrequency attribute */
#define ESP_ZB_ZCL_LEVEL_CONTROL_CURRENT_FREQUENCY_DEFAULT_VALUE ((uint16_t)0x0000)

/** @brief Default value for MinFrequency attribute */
#define ESP_ZB_ZCL_LEVEL_CONTROL_MIN_FREQUENCY_DEFAULT_VALUE ((uint16_t)0x0000)

/** @brief Default value for MaxFrequency attribute */
#define ESP_ZB_ZCL_LEVEL_CONTROL_MAX_FREQUENCY_DEFAULT_VALUE ((uint16_t)0x0000)

/** @brief Default value for OnOffTransitionTime attribute */
#define ESP_ZB_ZCL_LEVEL_CONTROL_ON_OFF_TRANSITION_TIME_DEFAULT_VALUE ((uint16_t)0x0000)

/** @brief Default value for OnLevel attribute */
#define ESP_ZB_ZCL_LEVEL_CONTROL_ON_LEVEL_DEFAULT_VALUE ((uint8_t)0xFF)

/** @brief Default value for OnTransitionTime attribute */
#define ESP_ZB_ZCL_LEVEL_CONTROL_ON_TRANSITION_TIME_DEFAULT_VALUE ((uint16_t)0xFFFF)

/** @brief Default value for OffTransitionTime attribute */
#define ESP_ZB_ZCL_LEVEL_CONTROL_OFF_TRANSITION_TIME_DEFAULT_VALUE ((uint16_t)0xFFFF)

/** @brief Default move rate */
#define ESP_ZB_ZCL_LEVEL_CONTROL_RATE_DEFAULT_VALUE ((uint8_t)0xff)

/** @brief Default value for Options attribute */
#define ESP_ZB_ZCL_LEVEL_CONTROL_OPTIONS_DEFAULT_VALUE ((uint8_t)0x00)

/** @brief Default value for OnLevel attribute */
#define ESP_ZB_ZCL_LEVEL_CONTROL_START_UP_CURRENT_LEVEL_USE_PREVIOUS_VALUE ((uint8_t)0xFF)

/*! @brief Level control cluster command identifiers
*/
typedef enum {
    ESP_ZB_ZCL_CMD_LEVEL_CONTROL_MOVE_TO_LEVEL             = 0x00,  /*!< Move To Level command */
    ESP_ZB_ZCL_CMD_LEVEL_CONTROL_MOVE                      = 0x01,  /*!< Move command */
    ESP_ZB_ZCL_CMD_LEVEL_CONTROL_STEP                      = 0x02,  /*!< Step command */
    ESP_ZB_ZCL_CMD_LEVEL_CONTROL_STOP                      = 0x03,  /*!< Stop command */
    ESP_ZB_ZCL_CMD_LEVEL_CONTROL_MOVE_TO_LEVEL_WITH_ON_OFF = 0x04,  /*!< Move To Level with On/Off command */
    ESP_ZB_ZCL_CMD_LEVEL_CONTROL_MOVE_WITH_ON_OFF          = 0x05,  /*!< Move with On/Off command */
    ESP_ZB_ZCL_CMD_LEVEL_CONTROL_STEP_WITH_ON_OFF          = 0x06,  /*!< Step with On/Off command */
    ESP_ZB_ZCL_CMD_LEVEL_CONTROL_STOP_WITH_ON_OFF          = 0x07,  /*!< Step with On/Off command */
    ESP_ZB_ZCL_CMD_LEVEL_CONTROL_MOVE_TO_CLOSEST_FREQUENCY = 0x08,  /*!< Upon receipt of "Move to Closest Frequency" command */
} esp_zb_zcl_level_control_cmd_id_t;

#ifdef __cplusplus
}
#endif
