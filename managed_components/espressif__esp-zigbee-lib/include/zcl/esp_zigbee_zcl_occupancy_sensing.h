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

/** @brief Occupancy Sensing cluster attribute identifiers
*/
typedef enum {
  ESP_ZB_ZCL_ATTR_OCCUPANCY_SENSING_OCCUPANCY_ID                                                = 0x0000,     /*!< Occupancy attribute identifier */
  ESP_ZB_ZCL_ATTR_OCCUPANCY_SENSING_OCCUPANCY_SENSOR_TYPE_ID                                    = 0x0001,     /*!< Occupancy Sensor Type attribute identifier */
  ESP_ZB_ZCL_ATTR_OCCUPANCY_SENSING_OCCUPANCY_SENSOR_TYPE_BITMAP_ID                             = 0x0002,     /*!< The OccupancySensorTypeBitmap attribute specifies the types of the occupancy sensor. */
  ESP_ZB_ZCL_ATTR_OCCUPANCY_SENSING_PIR_OCC_TO_UNOCC_DELAY_ID                                   = 0x0010,     /*!< PIROccupiedToUnoccupiedDelay identifier */
  ESP_ZB_ZCL_ATTR_OCCUPANCY_SENSING_PIR_UNOCC_TO_OCC_DELAY_ID                                   = 0x0011,     /*!< PIRUnoccupiedToOccupiedDelay identifier */
  ESP_ZB_ZCL_ATTR_OCCUPANCY_SENSING_PIR_UNOCC_TO_OCC_THRESHOLD_ID                               = 0x0012,     /*!< PIRUnoccupiedToOccupiedThreshold identifier */
  ESP_ZB_ZCL_ATTR_OCCUPANCY_SENSING_ULTRASONIC_OCCUPIED_TO_UNOCCUPIED_DELAY_ID                  = 0x0020,
  ESP_ZB_ZCL_ATTR_OCCUPANCY_SENSING_ULTRASONIC_UNOCCUPIED_TO_OCCUPIED_DELAY_ID                  = 0x0021,
  ESP_ZB_ZCL_ATTR_OCCUPANCY_SENSING_ULTRASONIC_UNOCCUPIED_TO_OCCUPIED_THRESHOLD_ID              = 0x0022,
  ESP_ZB_ZCL_ATTR_OCCUPANCY_SENSING_PHYSICAL_CONTACT_OCCUPIED_TO_UNOCCUPIED_DELAY_ID            = 0x0030,
  ESP_ZB_ZCL_ATTR_OCCUPANCY_SENSING_PHYSICAL_CONTACT_UNOCCUPIED_TO_OCCUPIED_DELAY_ID            = 0x0031,
  ESP_ZB_ZCL_ATTR_OCCUPANCY_SENSING_PHYSICAL_CONTACT_UNOCCUPIED_TO_OCCUPIED_THRESHOLD_ID        = 0x0032,
}esp_zb_zcl_occupancy_sensing_attr_t;

/** @brief Default value for Occupancy Sensing cluster revision global attribute */
#define ESP_ZB_ZCL_OCCUPANCY_SENSING_CLUSTER_REVISION_DEFAULT ((uint16_t)0x0002u)

/** @brief Minimal value for PIROccToUnoccDelay attribute */
#define ESP_ZB_ZCL_OCCUPANCY_SENSING_PIR_OCC_TO_UNOCC_DELAY_MIN_VALUE ((uint16_t)0x0000)

/** @brief Maximal value for PIROccToUnoccDelay attribute */
#define ESP_ZB_ZCL_OCCUPANCY_SENSING_PIR_OCC_TO_UNOCC_DELAY_MAX_VALUE ((uint16_t)0xfffe)

/** @brief Default value for PIROccToUnoccDelay attribute */
#define ESP_ZB_ZCL_OCCUPANCY_SENSING_PIR_OCC_TO_UNOCC_DELAY_DEFAULT_VALUE ESP_ZB_ZCL_OCCUPANCY_SENSING_PIR_OCC_TO_UNOCC_DELAY_MIN_VALUE

/** @brief Minimal value for PIRUnoccToOccDelay attribute */
#define ESP_ZB_ZCL_OCCUPANCY_SENSING_PIR_UNOCC_TO_OCC_DELAY_MIN_VALUE ((uint16_t)0x0000)

/** @brief Maximal value for PIRUnoccToOccDelay attribute */
#define ESP_ZB_ZCL_OCCUPANCY_SENSING_PIR_UNOCC_TO_OCC_DELAY_MAX_VALUE ((uint16_t)0xfffe)

/** @brief Default value for PIRUnoccToOccDelay attribute */
#define ESP_ZB_ZCL_OCCUPANCY_SENSING_PIR_UNOCC_TO_OCC_DELAY_DEFAULT_VALUE ESP_ZB_ZCL_OCCUPANCY_SENSING_PIR_UNOCC_TO_OCC_DELAY_MIN_VALUE

/** @brief Minimal value for PIRUnoccToOccThreshold attribute */
#define ESP_ZB_ZCL_OCCUPANCY_SENSING_PIR_UNOCC_TO_OCC_THRESHOLD_MIN_VALUE ((uint8_t)0x01)

/** @brief Maximal value for PIRUnoccToOccThreshold attribute */
#define ESP_ZB_ZCL_OCCUPANCY_SENSING_PIR_UNOCC_TO_OCC_THRESHOLD_MAX_VALUE ((uint8_t)0xfe)

/** @brief Default value for PIRUnoccToOccThreshold attribute */
#define ESP_ZB_ZCL_OCCUPANCY_SENSING_PIR_UNOCC_TO_OCC_THRESHOLD_DEFAULT_VALUE ESP_ZB_ZCL_OCCUPANCY_SENSING_PIR_UNOCC_TO_OCC_THRESHOLD_MIN_VALUE

/** @brief Minimal value for UltrasonicOccupiedToUnoccupiedDelay attribute */
#define ESP_ZB_ZCL_OCCUPANCY_SENSING_ULTRASONIC_OCCUPIED_TO_UNOCCUPIED_DELAY_MIN_VALUE ((uint16_t)0x0000)

/** @brief Maximal value for UltrasonicOccupiedToUnoccupiedDelay attribute */
#define ESP_ZB_ZCL_OCCUPANCY_SENSING_ULTRASONIC_OCCUPIED_TO_UNOCCUPIED_DELAY_MAX_VALUE ((uint16_t)0xfffe)

/** @brief Default value for UltrasonicOccupiedToUnoccupiedDelay attribute */
#define ESP_ZB_ZCL_OCCUPANCY_SENSING_ULTRASONIC_OCCUPIED_TO_UNOCCUPIED_DELAY_DEFAULT_VALUE ESP_ZB_ZCL_OCCUPANCY_SENSING_ULTRASONIC_OCCUPIED_TO_UNOCCUPIED_DELAY_MIN_VALUE

/** @brief Minimal value for UltrasonicUnoccupiedToOccupiedDelay attribute */
#define ESP_ZB_ZCL_OCCUPANCY_SENSING_ULTRASONIC_UNOCCUPIED_TO_OCCUPIED_DELAY_MIN_VALUE ((uint16_t)0x0000)

/** @brief Maximal value for UltrasonicUnoccupiedToOccupiedDelay attribute */
#define ESP_ZB_ZCL_OCCUPANCY_SENSING_ULTRASONIC_UNOCCUPIED_TO_OCCUPIED_DELAY_MAX_VALUE ((uint16_t)0xfffe)

/** @brief Default value for UltrasonicUnoccupiedToOccupiedDelay attribute */
#define ESP_ZB_ZCL_OCCUPANCY_SENSING_ULTRASONIC_UNOCCUPIED_TO_OCCUPIED_DELAY_DEFAULT_VALUE ESP_ZB_ZCL_OCCUPANCY_SENSING_ULTRASONIC_UNOCCUPIED_TO_OCCUPIED_DELAY_MIN_VALUE

/** @brief Minimal value for UltrasonicUnoccupiedToOccupiedThreshold attribute */
#define ESP_ZB_ZCL_OCCUPANCY_SENSING_ULTRASONIC_UNOCCUPIED_TO_OCCUPIED_THRESHOLD_MIN_VALUE ((uint8_t)0x01)

/** @brief Maximal value for UltrasonicUnoccupiedToOccupiedThreshold attribute */
#define ESP_ZB_ZCL_OCCUPANCY_SENSING_ULTRASONIC_UNOCCUPIED_TO_OCCUPIED_THRESHOLD_MAX_VALUE ((uint8_t)0xfe)

/** @brief Default value for UltrasonicUnoccupiedToOccupiedThreshold attribute */
#define ESP_ZB_ZCL_OCCUPANCY_SENSING_ULTRASONIC_UNOCCUPIED_TO_OCCUPIED_THRESHOLD_DEFAULT_VALUE ESP_ZB_ZCL_OCCUPANCY_SENSING_ULTRASONIC_UNOCCUPIED_TO_OCCUPIED_THRESHOLD_MIN_VALUE

/** @brief Default value for PhysicalContactOccupiedToUnoccupiedDelay attribute */
#define ESP_ZB_ZCL_OCCUPANCY_SENSING_PHYSICAL_CONTACT_OCCUPIED_TO_UNOCCUPIED_DELAY_DEFAULT_VALUE ((uint16_t)0x0000)

/** @brief No-reporting value for PhysicalContactOccupiedToUnoccupiedDelay attribute */
#define ESP_ZB_ZCL_OCCUPANCY_SENSING_PHYSICAL_CONTACT_OCCUPIED_TO_UNOCCUPIED_DELAY_NO_REPORTING_VALUE ((uint16_t)0xffff)

/** @brief Default value for PhysicalContactUnoccupiedToOccupiedDelay attribute */
#define ESP_ZB_ZCL_OCCUPANCY_SENSING_PHYSICAL_CONTACT_UNOCCUPIED_TO_OCCUPIED_DELAY_DEFAULT_VALUE ((uint16_t)0x0000)

/** @brief No-reporting value for PhysicalContactUnoccupiedToOccupiedDelay attribute */
#define ESP_ZB_ZCL_OCCUPANCY_SENSING_PHYSICAL_CONTACT_UNOCCUPIED_TO_OCCUPIED_DELAY_NO_REPORTING_VALUE ((uint16_t)0xffff)

/** @brief Minimal value for PhysicalContactUnoccupiedToOccupiedThreshold attribute */
#define ESP_ZB_ZCL_OCCUPANCY_SENSING_PHYSICAL_CONTACT_UNOCCUPIED_TO_OCCUPIED_THRESHOLD_MIN_VALUE ((uint8_t)0x01)

/** @brief Maximal value for PhysicalContactUnoccupiedToOccupiedThreshold attribute */
#define ESP_ZB_ZCL_OCCUPANCY_SENSING_PHYSICAL_CONTACT_UNOCCUPIED_TO_OCCUPIED_THRESHOLD_MAX_VALUE ((uint8_t)0xfe)

typedef enum {
  ESP_ZB_ZCL_OCCUPANCY_SENSING_OCCUPANCY_UNOCCUPIED = 0,            /*!< Unoccupied value */
  ESP_ZB_ZCL_OCCUPANCY_SENSING_OCCUPANCY_OCCUPIED   = 1             /*!< Occupied value */
}esp_zb_zcl_occupancy_sensing_occupancy_t;

typedef enum {
  ESP_ZB_ZCL_OCCUPANCY_SENSING_OCCUPANCY_SENSOR_TYPE_PIR                = 0,    /*!< PIR value */
  ESP_ZB_ZCL_OCCUPANCY_SENSING_OCCUPANCY_SENSOR_TYPE_ULTRASONIC         = 1,    /*!< Ultrasonic value */
  ESP_ZB_ZCL_OCCUPANCY_SENSING_OCCUPANCY_SENSOR_TYPE_PIR_AND_ULTRASONIC = 2,    /*!< PIR and Ultrasonic value */
  ESP_ZB_ZCL_OCCUPANCY_SENSING_OCCUPANCY_SENSOR_TYPE_RESERVED           = 3     /*!< Reserved value */
}esp_zb_zcl_occupancy_sensing_occupancy_sensor_type_t;

typedef enum {
  ESP_ZB_ZCL_CUSTOM_OCCUPANCY_SENSING_REPORING_MODE_TYPE_REGULAR_REPORTING    = 0,
  ESP_ZB_ZCL_CUSTOM_OCCUPANCY_SENSING_REPORING_MODE_TYPE_LOW_POWER_REPORTING  = 1
}esp_zb_zcl_custom_reporting_mode_type_t;

typedef enum {
  ESP_ZB_ZCL_OCCUPANCY_SENSING_CUSTOM_DEBUG_MODE_TYPE_SHORT         = 0,
  ESP_ZB_ZCL_OCCUPANCY_SENSING_CUSTOM_DEBUG_MODE_TYPE_ENHANCED      = 1,
  ESP_ZB_ZCL_OCCUPANCY_SENSING_CUSTOM_DEBUG_MODE_TYPE_DEBUG         = 2
}esp_zb_zcl_custom_debug_mode_type_t;

typedef enum {
  ESP_ZB_ZCL_OCCUPANCY_SENSING_CUSTOM_LEARN_NOT_YET_PERFORMED         = 0,
  ESP_ZB_ZCL_OCCUPANCY_SENSING_CUSTOM_LEARN_IN_PROGRESS               = 1,
  ESP_ZB_ZCL_OCCUPANCY_SENSING_CUSTOM_LEARN_FINISHED_WITH_A_FAILURE   = 2,
  ESP_ZB_ZCL_OCCUPANCY_SENSING_CUSTOM_LEARN_FINISHED_SUCCSESSFULLY    = 3
}esp_zb_zcl_custom_learn_type_t;

#ifdef __cplusplus
}
#endif
