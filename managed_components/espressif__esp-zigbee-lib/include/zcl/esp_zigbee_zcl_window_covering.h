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

/** @brief Window Covering cluster attribute identifiers
*/
typedef enum{
  ESP_ZB_ZCL_WINDOW_COVERING_INFORMATION = 0x000,           /*!< Window Covering Information attribute set */
  ESP_ZB_ZCL_WINDOW_COVERING_SETTINGS    = 0x001            /*!< Window Covering Settings attribute set */
}esp_zb_zcl_window_covering_attribute_sets_t;

typedef enum{
  ESP_ZB_ZCL_ATTR_WINDOW_COVERING_WINDOW_COVERING_TYPE_ID                = 0x0000,      /*!< Window Covering Type attribute */
  ESP_ZB_ZCL_ATTR_WINDOW_COVERING_PHYSICAL_CLOSED_LIMIT_LIFT_ID          = 0x0001,      /*!< PhysicalClosedLimit Lift attribute */
  ESP_ZB_ZCL_ATTR_WINDOW_COVERING_PHY_CLOSED_LIMIT_TILT_ID               = 0x0002,      /*!< PhysicalClosedLimit Tilt attribute */
  ESP_ZB_ZCL_ATTR_WINDOW_COVERING_CURRENT_POSITION_LIFT_ID               = 0x0003,      /*!< CurrentPosition Lift attribute */
  ESP_ZB_ZCL_ATTR_WINDOW_COVERING_CURRENT_POSITION_TILT_ID               = 0x0004,      /*!< CurrentPosition Tilt attribute */
  ESP_ZB_ZCL_ATTR_WINDOW_COVERING_NUMBER_OF_ACTUATIONS_LIFT_ID           = 0x0005,      /*!< Number of Actuations Lift attribute */
  ESP_ZB_ZCL_ATTR_WINDOW_COVERING_NUMBER_OF_ACTUATIONS_TILT_ID           = 0x0006,      /*!< Number of Actuations Tilt attribute */
  ESP_ZB_ZCL_ATTR_WINDOW_COVERING_CONFIG_STATUS_ID                       = 0x0007,      /*!< Config/Status attribute */
  ESP_ZB_ZCL_ATTR_WINDOW_COVERING_CURRENT_POSITION_LIFT_PERCENTAGE_ID    = 0x0008,      /*!< Current Position Lift Percentage attribute */
  ESP_ZB_ZCL_ATTR_WINDOW_COVERING_CURRENT_POSITION_TILT_PERCENTAGE_ID    = 0x0009       /*!< Current Position Tilt Percentage attribute */
}esp_zb_zcl_window_covering_info_attr_t;

typedef enum {
  ESP_ZB_ZCL_ATTR_WINDOW_COVERING_TYPE_ROLLERSHADE                       = 0x00,        /*!< Rollershade value */
  ESP_ZB_ZCL_ATTR_WINDOW_COVERING_TYPE_ROLLERSHADE_2_MOTOR               = 0x01,        /*!< Rollershade - 2 Motor value */
  ESP_ZB_ZCL_ATTR_WINDOW_COVERING_TYPE_ROLLERSHADE_EXTERIOR              = 0x02,        /*!< Rollershade - Exterior value */
  ESP_ZB_ZCL_ATTR_WINDOW_COVERING_TYPE_ROLLERSHADE_EXTERIOR_2_MOTOR      = 0x03,        /*!< Rollershade - Exterior - 2 Motor value */
  ESP_ZB_ZCL_ATTR_WINDOW_COVERING_TYPE_DRAPERY                           = 0x04,        /*!< Drapery value */
  ESP_ZB_ZCL_ATTR_WINDOW_COVERING_TYPE_AWNING                            = 0x05,        /*!< Awning value */
  ESP_ZB_ZCL_ATTR_WINDOW_COVERING_TYPE_SHUTTER                           = 0x06,        /*!< Shutter value */
  ESP_ZB_ZCL_ATTR_WINDOW_COVERING_TYPE_TILT_BLIND_TILT_ONLY              = 0x07,        /*!< Tilt Blind - Tilt Only value */
  ESP_ZB_ZCL_ATTR_WINDOW_COVERING_TYPE_TILT_BLIND_LIFT_AND_TILT          = 0x08,        /*!< Tilt Blind - Lift and Tilt value */
  ESP_ZB_ZCL_ATTR_WINDOW_COVERING_TYPE_PROJECTOR_SCREEN                  = 0x09         /*!< Projector screen value */
}esp_zb_zcl_window_covering_window_covering_type_t;

typedef enum {
  ESP_ZB_ZCL_ATTR_WINDOW_COVERING_CONFIG_OPERATIONAL                    = 0x01,      /*!< Operational value */
  ESP_ZB_ZCL_ATTR_WINDOW_COVERING_CONFIG_ONLINE                         = 0x02,      /*!< Online value */
  ESP_ZB_ZCL_ATTR_WINDOW_COVERING_CONFIG_REVERSE_COMMANDS               = 0x04,      /*!< Open/Up Commands have been reversed value */
  ESP_ZB_ZCL_ATTR_WINDOW_COVERING_CONFIG_LIFT_CONTROL_IS_CLOSED_LOOP    = 0x08,      /*!< Lift control is Closed Loop value */
  ESP_ZB_ZCL_ATTR_WINDOW_COVERING_CONFIG_TILT_CONTROL_IS_CLOSED_LOOP    = 0x10,      /*!< Tilt control is Closed Loop value */
  ESP_ZB_ZCL_ATTR_WINDOW_COVERING_CONFIG_LIFT_ENCODER_CONTROLLED        = 0x20,      /*!< Lift Encoder Controlled value */
  ESP_ZB_ZCL_ATTR_WINDOW_COVERING_CONFIG_TILT_ENCODER_CONTROLLED        = 0x40       /*!< Tilt Encoder Controlled value */
}esp_zb_zcl_window_covering_config_status_t;

typedef enum {
  ESP_ZB_ZCL_ATTR_WINDOW_COVERING_INSTALLED_OPEN_LIMIT_LIFT_ID            = 0x0010,     /*!< InstalledOpenLimit -  Lift attribute */
  ESP_ZB_ZCL_ATTR_WINDOW_COVERING_INSTALLED_CLOSED_LIMIT_LIFT_ID          = 0x0011,     /*!< InstalledClosedLimit - Lift attribute */
  ESP_ZB_ZCL_ATTR_WINDOW_COVERING_INSTALLED_OPEN_LIMIT_TILT_ID            = 0x0012,     /*!< InstalledOpenLimit - Tilt attribute */
  ESP_ZB_ZCL_ATTR_WINDOW_COVERING_INSTALLED_CLOSED_LIMIT_TILT_ID          = 0x0013,     /*!< InstalledClosedLimit - Tilt attribute */
  ESP_ZB_ZCL_ATTR_WINDOW_COVERING_VELOCITY_ID                             = 0x0014,     /*!< Velocity - Lift attribute */
  ESP_ZB_ZCL_ATTR_WINDOW_COVERING_ACCELERATION_TIME_ID                    = 0x0015,     /*!< Acceleration Time - Lift attribute */
  ESP_ZB_ZCL_ATTR_WINDOW_COVERING_DECELERATION_TIME_ID                    = 0x0016,     /*!< Deceleration Time - Lift attribute */
  ESP_ZB_ZCL_ATTR_WINDOW_COVERING_MODE_ID                                 = 0x0017,     /*!< Mode attribute */
  ESP_ZB_ZCL_ATTR_WINDOW_COVERING_INTERMEDIATE_SETPOINTS_LIFT_ID          = 0x0018,     /*!< Intermediate Setpoints - Lift attribute */
  ESP_ZB_ZCL_ATTR_WINDOW_COVERING_INTERMEDIATE_SETPOINTS_TILT_ID          = 0x0019      /*!< Intermediate Setpoints - Tilt attribute */
}esp_zb_zcl_window_covering_settings_attr_t;

typedef enum {
  ESP_ZB_ZCL_ATTR_WINDOW_COVERING_TYPE_REVERSED_MOTOR_DIRECTION               = 0x01,   /*!< Reversed motor direction value */
  ESP_ZB_ZCL_ATTR_WINDOW_COVERING_TYPE_RUN_IN_CALIBRATION_MODE                = 0x02,   /*!< Run in calibration mode value */
  ESP_ZB_ZCL_ATTR_WINDOW_COVERING_TYPE_MOTOR_IS_RUNNING_IN_MAINTENANCE_MODE   = 0x04,   /*!< Motor is running in maintenance mode value */
  ESP_ZB_ZCL_ATTR_WINDOW_COVERING_TYPE_LEDS_WILL_DISPLAY_FEEDBACK             = 0x08    /*!< LEDs will display feedback value */
}esp_zb_zcl_window_covering_mode_t;

typedef enum{
  ESP_ZB_ZCL_CMD_WINDOW_COVERING_UP_OPEN               = 0x00,    /*!< Up/Open command */
  ESP_ZB_ZCL_CMD_WINDOW_COVERING_DOWN_CLOSE            = 0x01,    /*!< Down/Close command */
  ESP_ZB_ZCL_CMD_WINDOW_COVERING_STOP                  = 0x02,    /*!< Stop command */
  ESP_ZB_ZCL_CMD_WINDOW_COVERING_GO_TO_LIFT_VALUE      = 0x04,    /*!< Go to Lift Value command */
  ESP_ZB_ZCL_CMD_WINDOW_COVERING_GO_TO_LIFT_PERCENTAGE = 0x05,    /*!< Go to Lift Percentage command */
  ESP_ZB_ZCL_CMD_WINDOW_COVERING_GO_TO_TILT_VALUE      = 0x07,    /*!< Go to Tilt Value command */
  ESP_ZB_ZCL_CMD_WINDOW_COVERING_GO_TO_TILT_PERCENTAGE = 0x08     /*!< Go to Tilt Percentage command */
}esp_zb_zcl_window_covering_cmd_t;

/** @brief Default value for Window Covering cluster revision global attribute */
#define ESP_ZB_ZCL_WINDOW_COVERING_CLUSTER_REVISION_DEFAULT ((uint16_t)0x0003u)

/** @brief Default value for PhysicalClosedLimitLift attribute */
#define ESP_ZB_ZCL_WINDOW_COVERING_PHYSICAL_CLOSED_LIMIT_LIFT_DEFAULT_VALUE ((uint16_t)0x0000)

/** @brief Default value for PhyClosedLimitTilt attribute */
#define ESP_ZB_ZCL_WINDOW_COVERING_PHY_CLOSED_LIMIT_TILT_DEFAULT_VALUE ((uint16_t)0x0000)

/** @brief Default value for CurrentPositionLift attribute */
#define ESP_ZB_ZCL_WINDOW_COVERING_CURRENT_POSITION_LIFT_DEFAULT_VALUE ((uint16_t)0x0000)

/** @brief Default value for CurrentPositionTilt attribute */
#define ESP_ZB_ZCL_WINDOW_COVERING_CURRENT_POSITION_TILT_DEFAULT_VALUE ((uint16_t)0x0000)

/** @brief Default value for NumberOfActuationsLift attribute */
#define ESP_ZB_ZCL_WINDOW_COVERING_NUMBER_OF_ACTUATIONS_LIFT_DEFAULT_VALUE ((uint16_t)0x0000)

/** @brief Default value for NumberOfActuationsTilt attribute */
#define ESP_ZB_ZCL_WINDOW_COVERING_NUMBER_OF_ACTUATIONS_TILT_DEFAULT_VALUE ((uint16_t)0x0000)

/** @brief Window covering type attribute default value */
#define ESP_ZB_ZCL_WINDOW_COVERING_WINDOW_COVERING_TYPE_DEFAULT_VALUE 0x00


/** @brief Config/status attribute default value */
#define ESP_ZB_ZCL_WINDOW_COVERING_CONFIG_STATUS_DEFAULT_VALUE                                        \
  ESP_ZB_ZCL_ATTR_WINDOW_COVERING_CONFIG_OPERATIONAL                                                  \
  | ESP_ZB_ZCL_ATTR_WINDOW_COVERING_CONFIG_ONLINE

/** @brief Current position lift percentage attribute default value */
#define ESP_ZB_ZCL_WINDOW_COVERING_CURRENT_POSITION_LIFT_PERCENTAGE_DEFAULT_VALUE 0xff

/** @brief Current position lift percentage attribute max value */
#define ESP_ZB_ZCL_WINDOW_COVERING_CURRENT_POSITION_LIFT_PERCENTAGE_MAX_VALUE 0x64

/** @brief Current position tilt percentage attribute default value */
#define ESP_ZB_ZCL_WINDOW_COVERING_CURRENT_POSITION_TILT_PERCENTAGE_DEFAULT_VALUE 0xff

/** @brief Current position tilt percentage attribute max value */
#define ESP_ZB_ZCL_WINDOW_COVERING_CURRENT_POSITION_TILT_PERCENTAGE_MAX_VALUE 0x64

/** @brief Installed open limit lift attribute default value */
#define ESP_ZB_ZCL_WINDOW_COVERING_INSTALLED_OPEN_LIMIT_LIFT_DEFAULT_VALUE 0x0000

/** @brief Installed closed limit lift attribute default value */
#define ESP_ZB_ZCL_WINDOW_COVERING_INSTALLED_CLOSED_LIMIT_LIFT_DEFAULT_VALUE 0xffff

/** @brief Installed open limit tilt attribute default value */
#define ESP_ZB_ZCL_WINDOW_COVERING_INSTALLED_OPEN_LIMIT_TILT_DEFAULT_VALUE 0x0000

/** @brief Installed closed limit tilt attribute default value */
#define ESP_ZB_ZCL_WINDOW_COVERING_INSTALLED_CLOSED_LIMIT_TILT_DEFAULT_VALUE 0xffff

/** @brief Default value for Velocity attribute */
#define ESP_ZB_ZCL_WINDOW_COVERING_VELOCITY_DEFAULT_VALUE ((uint16_t)0x0000)

/** @brief Default value for AccelerationTime attribute */
#define ESP_ZB_ZCL_WINDOW_COVERING_ACCELERATION_TIME_DEFAULT_VALUE ((uint16_t)0x0000)

/** @brief Default value for DecelerationTime attribute */
#define ESP_ZB_ZCL_WINDOW_COVERING_DECELERATION_TIME_DEFAULT_VALUE ((uint16_t)0x0000)


/** @brief Mode attribute default value */
#define ESP_ZB_ZCL_WINDOW_COVERING_MODE_DEFAULT_VALUE ESP_ZB_ZCL_ATTR_WINDOW_COVERING_TYPE_MOTOR_IS_RUNNING_IN_MAINTENANCE_MODE

/** @brief Default value for IntermediateSetpointsLift attribute */
#define ESP_ZB_ZCL_WINDOW_COVERING_INTERMEDIATE_SETPOINTS_LIFT_DEFAULT_VALUE {0x31, 0x2C, 0x30, 0x78, 0x30, 0x30, 0x30, 0x30, 0x00}

/** @brief Default value for IntermediateSetpointsTilt attribute */
#define ESP_ZB_ZCL_WINDOW_COVERING_INTERMEDIATE_SETPOINTS_TILT_DEFAULT_VALUE {0x31, 0x2C, 0x30, 0x78, 0x30, 0x30, 0x30, 0x30, 0x00}

#ifdef __cplusplus
}
#endif
