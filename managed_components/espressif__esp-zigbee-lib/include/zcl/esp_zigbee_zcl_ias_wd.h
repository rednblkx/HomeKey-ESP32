/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once
#ifdef __cplusplus
extern "C" {
#endif

#include "esp_zigbee_type.h"

/** @brief IAS WD cluster attribute identifiers. */
typedef enum {
    ESP_ZB_ZCL_ATTR_IAS_WD_MAX_DURATION_ID = 0x0000,   /**< Max Duration attribute */
} esp_zb_zcl_ias_wd_attr_t;

/** @brief Default value for IAS WD cluster revision global attribute */
#define ESP_ZB_ZCL_IAS_WD_CLUSTER_REVISION_DEFAULT ((uint16_t)0x0002u)

/** @brief Max Duration attribute default value */
#define ESP_ZB_ZCL_ATTR_IAS_WD_MAX_DURATION_DEF_VALUE   240

/** @brief Max Duration attribute minimum value */
#define ESP_ZB_ZCL_ATTR_IAS_WD_MAX_DURATION_MIN_VALUE   0

/** @brief Max Duration attribute maximum value */
#define ESP_ZB_ZCL_ATTR_IAS_WD_MAX_DURATION_MAX_VALUE   0xfffe

/** @brief IAS WD cluster command identifiers */
typedef enum {
    ESP_ZB_ZCL_CMD_IAS_WD_START_WARNING_ID = 0x00,   /**< Start warning command. */
    ESP_ZB_ZCL_CMD_IAS_WD_SQUAWK_ID        = 0x01,   /**< Squawk command. */
} esp_zb_zcl_ias_wd_cmd_t;

/******************************* Start warning command ******************************/

/** @brief Values of Warning Mode Field. */
typedef enum {
    ESP_ZB_ZCL_IAS_WD_WARNING_MODE_STOP            = 0x00,   /**< Stop */
    ESP_ZB_ZCL_IAS_WD_WARNING_MODE_BURGLAR         = 0x01,   /**< Burglar */
    ESP_ZB_ZCL_IAS_WD_WARNING_MODE_FIRE            = 0x02,   /**< Fire */
    ESP_ZB_ZCL_IAS_WD_WARNING_MODE_EMERGENCY       = 0x03,   /**< Emergency */
    ESP_ZB_ZCL_IAS_WD_WARNING_MODE_POLICE_PANIC    = 0x04,   /**< Police panic */
    ESP_ZB_ZCL_IAS_WD_WARNING_MODE_FIRE_PANIC      = 0x05,   /**< Fire panic */
    ESP_ZB_ZCL_IAS_WD_WARNING_MODE_EMERGENCY_PANIC = 0x06,   /**< Emergency panic (i.e., medical issue) */
} esp_zb_zcl_ias_wd_warning_mode_t;

/** @brief Values of Strobe Field. */
typedef enum {
    ESP_ZB_ZCL_IAS_WD_STROBE_NO_STROBE  = 0x00,   /**< No strobe */
    ESP_ZB_ZCL_IAS_WD_STROBE_USE_STROBE = 0x01,   /**< Use strobe in parallel to warning */
} esp_zb_zcl_ias_wd_strobe_t;

/** @brief Values of Siren Level Field. */
typedef enum {
    ESP_ZB_ZCL_IAS_WD_SIREN_LEVEL_LOW       = 0x00,   /**< Low level sound */
    ESP_ZB_ZCL_IAS_WD_SIREN_LEVEL_MEDIUM    = 0x01,   /**< Medium level sound */
    ESP_ZB_ZCL_IAS_WD_SIREN_LEVEL_HIGH      = 0x02,   /**< High level sound */
    ESP_ZB_ZCL_IAS_WD_SIREN_LEVEL_VERY_HIGH = 0x03,   /**< Very high level sound */
} esp_zb_zcl_ias_wd_siren_level_t;

/** @brief Values of Strobe Level Field. */
typedef enum {
    ESP_ZB_ZCL_IAS_WD_STROBE_LEVEL_LOW       = 0x00,   /**< Low level strobe */
    ESP_ZB_ZCL_IAS_WD_STROBE_LEVEL_MEDIUM    = 0x01,   /**< Medium level strobe */
    ESP_ZB_ZCL_IAS_WD_STROBE_LEVEL_HIGH      = 0x02,   /**< High level strobe */
    ESP_ZB_ZCL_IAS_WD_STROBE_LEVEL_VERY_HIGH = 0x03,   /**< Very high level strobe */
} esp_zb_zcl_ias_wd_strobe_level_t;

/** @brief Data in IAS_WD Start Warning command */
typedef struct esp_zb_zcl_ias_wd_start_warning_s {
    uint8_t  warning_mode;       /**< Warning Mode */
    uint8_t  strobe;             /**< Strobe */
    uint8_t  siren_level;        /**< Siren level */
    uint16_t duration;           /**< Duration */
    uint8_t  strobe_duty_cycle;  /**< Strobe duty cycle */
    uint8_t  strobe_level;       /**< Strobe level */
} esp_zb_zcl_ias_wd_start_warning_t;

/******************************* Squawk command ******************************/

/** @brief Values of Squawk Mode Field. */
typedef enum {
    ESP_ZB_ZCL_IAS_WD_SQUAWK_MODE_ARMED    = 0x00,   /**< Notification sound for "System is armed" */
    ESP_ZB_ZCL_IAS_WD_SQUAWK_MODE_DISARMED = 0x01,   /**< Notification sound for "System is disarmed" */
} esp_zb_zcl_ias_wd_squawk_mode_t;

/** @brief Values of Strobe Field. */
typedef enum {
    ESP_ZB_ZCL_IAS_WD_SQUAWK_STROBE_NO_STROBE  = 0x00,   /**< No strobe*/
    ESP_ZB_ZCL_IAS_WD_SQUAWK_STROBE_USE_STROBE = 0x01,   /**< Use strobe blink in parallel to squawk */
} esp_zb_zcl_ias_wd_squawk_strobe_t;

/** @brief Values of Squawk level Field. */
typedef enum {
    ESP_ZB_ZCL_IAS_WD_SQUAWK_LEVEL_LOW       = 0x00,   /**< Low level sound */
    ESP_ZB_ZCL_IAS_WD_SQUAWK_LEVEL_MEDIUM    = 0x01,   /**< Medium level sound */
    ESP_ZB_ZCL_IAS_WD_SQUAWK_LEVEL_HIGH      = 0x02,   /**< High level sound */
    ESP_ZB_ZCL_IAS_WD_SQUAWK_LEVEL_VERY_HIGH = 0x03,   /**< Very High level sound */
} esp_zb_zcl_ias_wd_squawk_level_t;

/** @brief Data in IAS_WD Squawk command */
typedef struct esp_zb_zcl_ias_wd_squawk_s {
    uint8_t squawk_mode;         /**< Squawk Mode */
    uint8_t strobe;              /**< Strobe */
    uint8_t squawk_level;        /**< Squawk level */
} esp_zb_zcl_ias_wd_squawk_t;

#ifdef __cplusplus
}
#endif
