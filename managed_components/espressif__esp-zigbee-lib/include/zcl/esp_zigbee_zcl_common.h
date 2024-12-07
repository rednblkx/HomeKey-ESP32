/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once
#ifdef __cplusplus
extern "C" {
#endif

#include "esp_err.h"
#include "esp_zigbee_type.h"
#include "esp_zigbee_zcl_basic.h"
#include "esp_zigbee_zcl_identify.h"
#include "esp_zigbee_zcl_groups.h"
#include "esp_zigbee_zcl_scenes.h"
#include "esp_zigbee_zcl_on_off.h"
#include "esp_zigbee_zcl_on_off_switch_config.h"
#include "esp_zigbee_zcl_level.h"
#include "esp_zigbee_zcl_color_control.h"
#include "esp_zigbee_zcl_time.h"
#include "esp_zigbee_zcl_binary_input.h"
#include "esp_zigbee_zcl_commissioning.h"
#include "esp_zigbee_zcl_ias_zone.h"
#include "esp_zigbee_zcl_ias_ace.h"
#include "esp_zigbee_zcl_ias_wd.h"
#include "esp_zigbee_zcl_shade_config.h"
#include "esp_zigbee_zcl_door_lock.h"
#include "esp_zigbee_zcl_humidity_meas.h"
#include "esp_zigbee_zcl_temperature_meas.h"
#include "esp_zigbee_zcl_ota.h"
#include "esp_zigbee_zcl_electrical_meas.h"
#include "esp_zigbee_zcl_illuminance_meas.h"
#include "esp_zigbee_zcl_pressure_meas.h"
#include "esp_zigbee_zcl_flow_meas.h"
#include "esp_zigbee_zcl_occupancy_sensing.h"
#include "esp_zigbee_zcl_window_covering.h"
#include "esp_zigbee_zcl_thermostat.h"
#include "esp_zigbee_zcl_fan_control.h"
#include "esp_zigbee_zcl_thermostat_ui_config.h"
#include "esp_zigbee_zcl_analog_input.h"
#include "esp_zigbee_zcl_analog_output.h"
#include "esp_zigbee_zcl_analog_value.h"
#include "esp_zigbee_zcl_carbon_dioxide_measurement.h"
#include "esp_zigbee_zcl_pm2_5_measurement.h"
#include "esp_zigbee_zcl_multistate_value.h"
#include "esp_zigbee_zcl_metering.h"
#include "esp_zigbee_zcl_diagnostics.h"
#include "esp_zigbee_zcl_meter_identification.h"
#include "esp_zigbee_zcl_price.h"
#include "esp_zigbee_zcl_ec_measurement.h"
#include "esp_zigbee_zcl_ph_measurement.h"
#include "esp_zigbee_zcl_wind_speed_measurement.h"
#include "esp_zigbee_zcl_drlc.h"

#ifdef ZB_ENABLE_ZGP
#include "esp_zigbee_zcl_green_power.h"
#endif

/**
 * @brief Application Framework Profile identifiers.
 *
 */
typedef enum {
    ESP_ZB_AF_HA_PROFILE_ID     = 0x0104U,  /** HA profile ID */
    ESP_ZB_AF_SE_PROFILE_ID     = 0x0109U,  /** SE profile ID */
    ESP_ZB_AF_ZLL_PROFILE_ID    = 0xC05EU,  /** ZLL profile ID */
    ESP_ZB_AF_GP_PROFILE_ID     = 0xA1E0U,  /** GreenPower profile ID */
} esp_zb_af_profile_id_t;

/** Green power special endpoint */
#define ESP_ZGP_ENDPOINT 242
/** Non manufacturer specific code for certain attribute */
#define ESP_ZB_ZCL_ATTR_NON_MANUFACTURER_SPECIFIC 0xFFFFU
/** Non manufacturer specific code for certain cluster */
#define EZP_ZB_ZCL_CLUSTER_NON_MANUFACTURER_SPECIFIC 0x0000

/** @brief HA Device identifiers.
 */
typedef enum {
    ESP_ZB_HA_ON_OFF_SWITCH_DEVICE_ID                 = 0x0000,  /*!< General On/Off switch */
    ESP_ZB_HA_LEVEL_CONTROL_SWITCH_DEVICE_ID          = 0x0001,  /*!< Level Control Switch  */
    ESP_ZB_HA_ON_OFF_OUTPUT_DEVICE_ID                 = 0x0002,  /*!< General On/Off output */
    ESP_ZB_HA_LEVEL_CONTROLLABLE_OUTPUT_DEVICE_ID     = 0x0003,  /*!< Level Controllable Output */
    ESP_ZB_HA_SCENE_SELECTOR_DEVICE_ID                = 0x0004,  /*!< Scene Selector */
    ESP_ZB_HA_CONFIGURATION_TOOL_DEVICE_ID            = 0x0005,  /*!< Configuration Tool */
    ESP_ZB_HA_REMOTE_CONTROL_DEVICE_ID                = 0x0006,  /*!< Remote Control */
    ESP_ZB_HA_COMBINED_INTERFACE_DEVICE_ID            = 0x0007,  /*!< Combined Interface */
    ESP_ZB_HA_RANGE_EXTENDER_DEVICE_ID                = 0x0008,  /*!< Range Extender */
    ESP_ZB_HA_MAINS_POWER_OUTLET_DEVICE_ID            = 0x0009,  /*!< Mains Power Outlet */
    ESP_ZB_HA_DOOR_LOCK_DEVICE_ID                     = 0x000A,  /*!< Door lock client */
    ESP_ZB_HA_DOOR_LOCK_CONTROLLER_DEVICE_ID          = 0x000B,  /*!< Door lock controller */
    ESP_ZB_HA_SIMPLE_SENSOR_DEVICE_ID                 = 0x000C,  /*!< Simple Sensor device */
    ESP_ZB_HA_CONSUMPTION_AWARENESS_DEVICE_ID         = 0x000D,  /*!< Consumption Awareness Device */
    ESP_ZB_HA_HOME_GATEWAY_DEVICE_ID                  = 0x0050,  /*!< Home Gateway */
    ESP_ZB_HA_SMART_PLUG_DEVICE_ID                    = 0x0051,  /*!< Smart plug */
    ESP_ZB_HA_WHITE_GOODS_DEVICE_ID                   = 0x0052,  /*!< White Goods */
    ESP_ZB_HA_METER_INTERFACE_DEVICE_ID               = 0x0053,  /*!< Meter Interface */
    ESP_ZB_HA_ON_OFF_LIGHT_DEVICE_ID                  = 0x0100,  /*!< On/Off Light Device */
    ESP_ZB_HA_DIMMABLE_LIGHT_DEVICE_ID                = 0x0101,  /*!< Dimmable Light Device */
    ESP_ZB_HA_COLOR_DIMMABLE_LIGHT_DEVICE_ID          = 0x0102,  /*!< Color Dimmable Light Device */
    ESP_ZB_HA_DIMMER_SWITCH_DEVICE_ID                 = 0x0104,  /*!< Dimmer Switch Device */
    ESP_ZB_HA_COLOR_DIMMER_SWITCH_DEVICE_ID           = 0x0105,  /*!< Color Dimmer Switch Device */
    ESP_ZB_HA_LIGHT_SENSOR_DEVICE_ID                  = 0x0106,  /*!< Light Sensor Device */
    ESP_ZB_HA_SHADE_DEVICE_ID                         = 0x0200,  /*!< Shade */
    ESP_ZB_HA_SHADE_CONTROLLER_DEVICE_ID              = 0x0201,  /*!< Shade controller */
    ESP_ZB_HA_WINDOW_COVERING_DEVICE_ID               = 0x0202,  /*!< Window Covering client*/
    ESP_ZB_HA_WINDOW_COVERING_CONTROLLER_DEVICE_ID    = 0x0203,  /*!< Window Covering controller */
    ESP_ZB_HA_HEATING_COOLING_UNIT_DEVICE_ID          = 0x0300,  /*!< Heating/Cooling Unit device */
    ESP_ZB_HA_THERMOSTAT_DEVICE_ID                    = 0x0301,  /*!< Thermostat Device */
    ESP_ZB_HA_TEMPERATURE_SENSOR_DEVICE_ID            = 0x0302,  /*!< Temperature Sensor */
    ESP_ZB_HA_IAS_CONTROL_INDICATING_EQUIPMENT_ID     = 0x0400,  /*!< IAS Control and Indicating Equipment */
    ESP_ZB_HA_IAS_ANCILLARY_CONTROL_EQUIPMENT_ID      = 0x0401,  /*!< IAS Ancillary Control Equipment */
    ESP_ZB_HA_IAS_ZONE_ID                             = 0x0402,  /*!< IAS Zone */
    ESP_ZB_HA_IAS_WARNING_DEVICE_ID                   = 0x0403,  /*!< IAS Warning Device */
    ESP_ZB_HA_TEST_DEVICE_ID                          = 0xfff0,  /*!< Custom HA device for test */
    ESP_ZB_HA_CUSTOM_TUNNEL_DEVICE_ID                 = 0xfff1,  /*!< Custom Tunnel device (should declared in private profile) */
    ESP_ZB_HA_CUSTOM_ATTR_DEVICE_ID                   = 0xfff2   /*!< Custom Attributes Device */
} esp_zb_ha_standard_devices_t;

/**
 * @brief ZCL cluster role enum
 * @anchor esp_zb_zcl_cluster_role
 */
typedef enum {
    ESP_ZB_ZCL_CLUSTER_SERVER_ROLE = 0x01U,         /*!< Server cluster role */
    ESP_ZB_ZCL_CLUSTER_CLIENT_ROLE = 0x02U,         /*!< Client cluster role */
} esp_zb_zcl_cluster_role_t;

/**
 * @brief ZCL cluster identifiers
 * @anchor esp_zb_zcl_cluster_id
 */
typedef enum {
    ESP_ZB_ZCL_CLUSTER_ID_BASIC                 = 0x0000U,          /*!< Basic cluster identifier. */
    ESP_ZB_ZCL_CLUSTER_ID_POWER_CONFIG          = 0x0001U,          /*!< Power configuration cluster identifier. */
    ESP_ZB_ZCL_CLUSTER_ID_DEVICE_TEMP_CONFIG    = 0x0002U,          /*!< Device temperature configuration cluster identifier. */
    ESP_ZB_ZCL_CLUSTER_ID_IDENTIFY              = 0x0003U,          /*!< Identify cluster identifier. */
    ESP_ZB_ZCL_CLUSTER_ID_GROUPS                = 0x0004U,          /*!< Groups cluster identifier. */
    ESP_ZB_ZCL_CLUSTER_ID_SCENES                = 0x0005U,          /*!< Scenes cluster identifier. */
    ESP_ZB_ZCL_CLUSTER_ID_ON_OFF                = 0x0006U,          /*!< On/Off cluster identifier. */
    ESP_ZB_ZCL_CLUSTER_ID_ON_OFF_SWITCH_CONFIG  = 0x0007U,          /*!< On/Off switch configuration cluster identifier. */
    ESP_ZB_ZCL_CLUSTER_ID_LEVEL_CONTROL         = 0x0008U,          /*!< Level control cluster identifier. */
    ESP_ZB_ZCL_CLUSTER_ID_ALARMS                = 0x0009U,          /*!< Alarms cluster identifier. */
    ESP_ZB_ZCL_CLUSTER_ID_TIME                  = 0x000aU,          /*!< Time cluster identifier. */
    ESP_ZB_ZCL_CLUSTER_ID_RSSI_LOCATION         = 0x000bU,          /*!< RSSI location cluster identifier. */
    ESP_ZB_ZCL_CLUSTER_ID_ANALOG_INPUT          = 0x000cU,          /*!< Analog input (basic) cluster identifier. */
    ESP_ZB_ZCL_CLUSTER_ID_ANALOG_OUTPUT         = 0x000dU,          /*!< Analog output (basic) cluster identifier. */
    ESP_ZB_ZCL_CLUSTER_ID_ANALOG_VALUE          = 0x000eU,          /*!< Analog value (basic) cluster identifier. */
    ESP_ZB_ZCL_CLUSTER_ID_BINARY_INPUT          = 0x000fU,          /*!< Binary input (basic) cluster identifier. */
    ESP_ZB_ZCL_CLUSTER_ID_BINARY_OUTPUT         = 0x0010U,          /*!< Binary output (basic) cluster identifier. */
    ESP_ZB_ZCL_CLUSTER_ID_BINARY_VALUE          = 0x0011U,          /*!< Binary value (basic) cluster identifier. */
    ESP_ZB_ZCL_CLUSTER_ID_MULTI_INPUT           = 0x0012U,          /*!< Multistate input (basic) cluster identifier. */
    ESP_ZB_ZCL_CLUSTER_ID_MULTI_OUTPUT          = 0x0013U,          /*!< Multistate output (basic) cluster identifier. */
    ESP_ZB_ZCL_CLUSTER_ID_MULTI_VALUE           = 0x0014U,          /*!< Multistate value (basic) cluster identifier. */
    ESP_ZB_ZCL_CLUSTER_ID_COMMISSIONING         = 0x0015U,          /*!< Commissioning cluster identifier. */
    ESP_ZB_ZCL_CLUSTER_ID_OTA_UPGRADE           = 0x0019U,          /*!< Over The Air cluster identifier. */
    ESP_ZB_ZCL_CLUSTER_ID_POLL_CONTROL          = 0x0020U,          /*!< Poll control cluster identifier. */
    ESP_ZB_ZCL_CLUSTER_ID_GREEN_POWER           = 0x0021U,          /*!< Green Power cluster identifier. */
    ESP_ZB_ZCL_CLUSTER_ID_KEEP_ALIVE            = 0x0025U,          /*!< Keep Alive cluster identifier. */
    ESP_ZB_ZCL_CLUSTER_ID_SHADE_CONFIG          = 0x0100U,          /*!< Shade configuration cluster identifier. */
    ESP_ZB_ZCL_CLUSTER_ID_DOOR_LOCK             = 0x0101U,          /*!< Door lock cluster identifier. */
    ESP_ZB_ZCL_CLUSTER_ID_WINDOW_COVERING       = 0x0102U,          /*!< Window covering cluster identifier. */
    ESP_ZB_ZCL_CLUSTER_ID_PUMP_CONFIG_CONTROL   = 0x0200U,          /*!< Pump configuration and control cluster identifier. */
    ESP_ZB_ZCL_CLUSTER_ID_THERMOSTAT            = 0x0201U,          /*!< Thermostat cluster identifier. */
    ESP_ZB_ZCL_CLUSTER_ID_FAN_CONTROL           = 0x0202U,          /*!< Fan control cluster identifier. */
    ESP_ZB_ZCL_CLUSTER_ID_DEHUMID_CONTROL       = 0x0203U,          /*!< Dehumidification control cluster identifier. */
    ESP_ZB_ZCL_CLUSTER_ID_THERMOSTAT_UI_CONFIG  = 0x0204U,          /*!< Thermostat user interface configuration cluster identifier. */
    ESP_ZB_ZCL_CLUSTER_ID_COLOR_CONTROL         = 0x0300U,          /*!< Color control cluster identifier. */
    ESP_ZB_ZCL_CLUSTER_ID_BALLAST_CONFIG        = 0x0301U,          /*!< Ballast configuration cluster identifier. */
    ESP_ZB_ZCL_CLUSTER_ID_ILLUMINANCE_MEASUREMENT    = 0x0400U,     /*!< Illuminance measurement */
    ESP_ZB_ZCL_CLUSTER_ID_TEMP_MEASUREMENT           = 0x0402U,     /*!< Temperature measurement */
    ESP_ZB_ZCL_CLUSTER_ID_PRESSURE_MEASUREMENT       = 0x0403U,     /*!< Pressure measurement */
    ESP_ZB_ZCL_CLUSTER_ID_FLOW_MEASUREMENT           = 0x0404U,     /*!< Flow measurement */
    ESP_ZB_ZCL_CLUSTER_ID_REL_HUMIDITY_MEASUREMENT   = 0x0405U,     /*!< Relative humidity measurement */
    ESP_ZB_ZCL_CLUSTER_ID_OCCUPANCY_SENSING          = 0x0406U,     /*!< Occupancy sensing */
    ESP_ZB_ZCL_CLUSTER_ID_PH_MEASUREMENT             = 0x0409U,     /*!< pH measurement */
    ESP_ZB_ZCL_CLUSTER_ID_EC_MEASUREMENT             = 0x040aU,     /*!< Electrical conductivity measurement */
    ESP_ZB_ZCL_CLUSTER_ID_WIND_SPEED_MEASUREMENT     = 0x040bU,     /*!< Wind speed measurement */
    ESP_ZB_ZCL_CLUSTER_ID_CARBON_DIOXIDE_MEASUREMENT = 0x040dU,     /*!< Carbon dioxide measurement */
    ESP_ZB_ZCL_CLUSTER_ID_PM2_5_MEASUREMENT          = 0x042aU,     /*!< PM2.5 measurement */
    ESP_ZB_ZCL_CLUSTER_ID_IAS_ZONE                   = 0x0500U,     /*!< IAS zone */
    ESP_ZB_ZCL_CLUSTER_ID_IAS_ACE                    = 0x0501U,     /*!< IAS ACE */
    ESP_ZB_ZCL_CLUSTER_ID_IAS_WD                     = 0x0502U,     /*!< IAS WD */
    ESP_ZB_ZCL_CLUSTER_ID_PRICE                      = 0x0700U,     /*!< Price cluster identifier. */
    ESP_ZB_ZCL_CLUSTER_ID_DRLC                       = 0x0701U,     /*!< Demand Response and Load Control cluster identifier */
    ESP_ZB_ZCL_CLUSTER_ID_METERING                   = 0x0702U,     /*!< Metering */
    ESP_ZB_ZCL_CLUSTER_ID_METER_IDENTIFICATION       = 0x0b01U,     /*!< Meter Identification cluster identifier */
    ESP_ZB_ZCL_CLUSTER_ID_ELECTRICAL_MEASUREMENT     = 0x0b04U,     /*!< Electrical measurement */
    ESP_ZB_ZCL_CLUSTER_ID_DIAGNOSTICS                = 0x0b05U,     /*!< Home Automation Diagnostics */
} esp_zb_zcl_cluster_id_t;

/**
 * @brief ZCL status values
 * @anchor esp_zb_zcl_status
 */
typedef enum {
    ESP_ZB_ZCL_STATUS_SUCCESS               = 0x00U,     /*!< ZCL Success */
    ESP_ZB_ZCL_STATUS_FAIL                  = 0x01U,     /*!< ZCL Fail */
    ESP_ZB_ZCL_STATUS_NOT_AUTHORIZED        = 0x7EU,     /*!< Server is not authorized to upgrade the client */
    ESP_ZB_ZCL_STATUS_MALFORMED_CMD         = 0x80U,     /*!< Malformed command */
    ESP_ZB_ZCL_STATUS_UNSUP_CLUST_CMD       = 0x81U,     /*!< Unsupported cluster command */
    ESP_ZB_ZCL_STATUS_UNSUP_GEN_CMD         = 0x82U,     /*!< Unsupported general command */
    ESP_ZB_ZCL_STATUS_UNSUP_MANUF_CLUST_CMD = 0x83U,     /*!< Unsupported manuf-specific clust command */
    ESP_ZB_ZCL_STATUS_UNSUP_MANUF_GEN_CMD   = 0x84U,     /*!< Unsupported manuf-specific general command */
    ESP_ZB_ZCL_STATUS_INVALID_FIELD         = 0x85U,     /*!< Invalid field */
    ESP_ZB_ZCL_STATUS_UNSUP_ATTRIB          = 0x86U,     /*!< Unsupported attribute */
    ESP_ZB_ZCL_STATUS_INVALID_VALUE         = 0x87U,     /*!< Invalid value */
    ESP_ZB_ZCL_STATUS_READ_ONLY             = 0x88U,     /*!< Read only */
    ESP_ZB_ZCL_STATUS_INSUFF_SPACE          = 0x89U,     /*!< Insufficient space */
    ESP_ZB_ZCL_STATUS_DUPE_EXISTS           = 0x8aU,     /*!< Duplicate exists */
    ESP_ZB_ZCL_STATUS_NOT_FOUND             = 0x8bU,     /*!< Not found */
    ESP_ZB_ZCL_STATUS_UNREPORTABLE_ATTRIB   = 0x8cU,     /*!< Unreportable attribute */
    ESP_ZB_ZCL_STATUS_INVALID_TYPE          = 0x8dU,     /*!< Invalid type */
    ESP_ZB_ZCL_STATUS_WRITE_ONLY            = 0x8fU,     /*!< Write only */
    ESP_ZB_ZCL_STATUS_INCONSISTENT          = 0x92U,     /*!< Supplied values are inconsistent */
    ESP_ZB_ZCL_STATUS_ACTION_DENIED         = 0x93U,
    ESP_ZB_ZCL_STATUS_TIMEOUT               = 0x94U,     /*!< Timeout */
    ESP_ZB_ZCL_STATUS_ABORT                 = 0x95U,     /*!< Abort */
    ESP_ZB_ZCL_STATUS_INVALID_IMAGE         = 0x96U,     /*!< Invalid OTA upgrade image */
    ESP_ZB_ZCL_STATUS_WAIT_FOR_DATA         = 0x97U,     /*!< Server does not have data block available yet */
    ESP_ZB_ZCL_STATUS_NO_IMAGE_AVAILABLE    = 0x98U,
    ESP_ZB_ZCL_STATUS_REQUIRE_MORE_IMAGE    = 0x99U,
    ESP_ZB_ZCL_STATUS_NOTIFICATION_PENDING  = 0x9AU,
    ESP_ZB_ZCL_STATUS_HW_FAIL               = 0xc0U,     /*!< Hardware failure */
    ESP_ZB_ZCL_STATUS_SW_FAIL               = 0xc1U,     /*!< Software failure */
    ESP_ZB_ZCL_STATUS_CALIB_ERR             = 0xc2U,     /*!< Calibration error */
    ESP_ZB_ZCL_STATUS_UNSUP_CLUST           = 0xc3U,     /*!< Cluster is not found on the target endpoint */
    ESP_ZB_ZCL_STATUS_LIMIT_REACHED         = 0xc4U,     /*!< Cluster is not found on the target endpoint */
} esp_zb_zcl_status_t;

/**
 * @brief ZCL attribute data type values
 * @anchor esp_zb_zcl_attr_type
 */
typedef enum {
    ESP_ZB_ZCL_ATTR_TYPE_NULL               = 0x00U,        /*!< Null data type */
    ESP_ZB_ZCL_ATTR_TYPE_8BIT               = 0x08U,        /*!< 8-bit value data type */
    ESP_ZB_ZCL_ATTR_TYPE_16BIT              = 0x09U,        /*!< 16-bit value data type */
    ESP_ZB_ZCL_ATTR_TYPE_24BIT              = 0x0aU,        /*!< 24-bit value data type */
    ESP_ZB_ZCL_ATTR_TYPE_32BIT              = 0x0bU,        /*!< 32-bit value data type */
    ESP_ZB_ZCL_ATTR_TYPE_40BIT              = 0x0cU,        /*!< 40-bit value data type */
    ESP_ZB_ZCL_ATTR_TYPE_48BIT              = 0x0dU,        /*!< 48-bit value data type */
    ESP_ZB_ZCL_ATTR_TYPE_56BIT              = 0x0eU,        /*!< 56-bit value data type */
    ESP_ZB_ZCL_ATTR_TYPE_64BIT              = 0x0fU,        /*!< 64-bit value data type */
    ESP_ZB_ZCL_ATTR_TYPE_BOOL               = 0x10U,        /*!< Boolean data type */
    ESP_ZB_ZCL_ATTR_TYPE_8BITMAP            = 0x18U,        /*!< 8-bit bitmap data type */
    ESP_ZB_ZCL_ATTR_TYPE_16BITMAP           = 0x19U,        /*!< 16-bit bitmap data type */
    ESP_ZB_ZCL_ATTR_TYPE_24BITMAP           = 0x1aU,        /*!< 24-bit bitmap data type */
    ESP_ZB_ZCL_ATTR_TYPE_32BITMAP           = 0x1bU,        /*!< 32-bit bitmap data type */
    ESP_ZB_ZCL_ATTR_TYPE_40BITMAP           = 0x1cU,        /*!< 40-bit bitmap data type */
    ESP_ZB_ZCL_ATTR_TYPE_48BITMAP           = 0x1dU,        /*!< 48-bit bitmap data type */
    ESP_ZB_ZCL_ATTR_TYPE_56BITMAP           = 0x1eU,        /*!< 56-bit bitmap data type */
    ESP_ZB_ZCL_ATTR_TYPE_64BITMAP           = 0x1fU,        /*!< 64-bit bitmap data type */
    ESP_ZB_ZCL_ATTR_TYPE_U8                 = 0x20U,        /*!< Unsigned 8-bit value data type */
    ESP_ZB_ZCL_ATTR_TYPE_U16                = 0x21U,        /*!< Unsigned 16-bit value data type */
    ESP_ZB_ZCL_ATTR_TYPE_U24                = 0x22U,        /*!< Unsigned 24-bit value data type */
    ESP_ZB_ZCL_ATTR_TYPE_U32                = 0x23U,        /*!< Unsigned 32-bit value data type */
    ESP_ZB_ZCL_ATTR_TYPE_U40                = 0x24U,        /*!< Unsigned 40-bit value data type */
    ESP_ZB_ZCL_ATTR_TYPE_U48                = 0x25U,        /*!< Unsigned 48-bit value data type */
    ESP_ZB_ZCL_ATTR_TYPE_U56                = 0x26U,        /*!< Unsigned 56-bit value data type */
    ESP_ZB_ZCL_ATTR_TYPE_U64                = 0x27U,        /*!< Unsigned 64-bit value data type */
    ESP_ZB_ZCL_ATTR_TYPE_S8                 = 0x28U,        /*!< Signed 8-bit value data type */
    ESP_ZB_ZCL_ATTR_TYPE_S16                = 0x29U,        /*!< Signed 16-bit value data type */
    ESP_ZB_ZCL_ATTR_TYPE_S24                = 0x2aU,        /*!< Signed 24-bit value data type */
    ESP_ZB_ZCL_ATTR_TYPE_S32                = 0x2bU,        /*!< Signed 32-bit value data type */
    ESP_ZB_ZCL_ATTR_TYPE_S40                = 0x2cU,        /*!< Signed 40-bit value data type */
    ESP_ZB_ZCL_ATTR_TYPE_S48                = 0x2dU,        /*!< Signed 48-bit value data type */
    ESP_ZB_ZCL_ATTR_TYPE_S56                = 0x2eU,        /*!< Signed 56-bit value data type */
    ESP_ZB_ZCL_ATTR_TYPE_S64                = 0x2fU,        /*!< Signed 64-bit value data type */
    ESP_ZB_ZCL_ATTR_TYPE_8BIT_ENUM          = 0x30U,        /*!< 8-bit enumeration (U8 discrete) data type */
    ESP_ZB_ZCL_ATTR_TYPE_16BIT_ENUM         = 0x31U,        /*!< 16-bit enumeration (U16 discrete) data type */
    ESP_ZB_ZCL_ATTR_TYPE_SEMI               = 0x38U,        /*!< 2 byte floating point */
    ESP_ZB_ZCL_ATTR_TYPE_SINGLE             = 0x39U,        /*!< 4 byte floating point */
    ESP_ZB_ZCL_ATTR_TYPE_DOUBLE             = 0x3aU,        /*!< 8 byte floating point */
    ESP_ZB_ZCL_ATTR_TYPE_OCTET_STRING       = 0x41U,        /*!< Octet string data type */
    ESP_ZB_ZCL_ATTR_TYPE_CHAR_STRING        = 0x42U,        /*!< Character string (array) data type */
    ESP_ZB_ZCL_ATTR_TYPE_LONG_OCTET_STRING  = 0x43U,        /*!< Long octet string */
    ESP_ZB_ZCL_ATTR_TYPE_LONG_CHAR_STRING   = 0x44U,        /*!< Long character string */
    ESP_ZB_ZCL_ATTR_TYPE_ARRAY              = 0x48U,        /*!< Array data with 8bit type, size = 2 + sum of content len */
    ESP_ZB_ZCL_ATTR_TYPE_16BIT_ARRAY        = 0x49U,        /*!< Array data with 16bit type, size = 2 + sum of content len */
    ESP_ZB_ZCL_ATTR_TYPE_32BIT_ARRAY        = 0x4aU,        /*!< Array data with 32bit type, size = 2 + sum of content len */
    ESP_ZB_ZCL_ATTR_TYPE_STRUCTURE          = 0x4cU,        /*!< Structure data type 2 + sum of content len */
    ESP_ZB_ZCL_ATTR_TYPE_SET                = 0x50U,        /*!< Collection:set, size = sum of len of content */
    ESP_ZB_ZCL_ATTR_TYPE_BAG                = 0x51U,        /*!< Collection:bag, size = sum of len of content */
    ESP_ZB_ZCL_ATTR_TYPE_TIME_OF_DAY        = 0xe0U,        /*!< Time of day, 4 bytes */
    ESP_ZB_ZCL_ATTR_TYPE_DATE               = 0xe1U,        /*!< Date, 4 bytes */
    ESP_ZB_ZCL_ATTR_TYPE_UTC_TIME           = 0xe2U,        /*!< UTC Time, 4 bytes */
    ESP_ZB_ZCL_ATTR_TYPE_CLUSTER_ID         = 0xe8U,        /*!< Cluster ID, 2 bytes */
    ESP_ZB_ZCL_ATTR_TYPE_ATTRIBUTE_ID       = 0xe9U,        /*!< Attribute ID, 2 bytes */
    ESP_ZB_ZCL_ATTR_TYPE_BACNET_OID         = 0xeaU,        /*!< BACnet OID, 4 bytes */
    ESP_ZB_ZCL_ATTR_TYPE_IEEE_ADDR          = 0xf0U,        /*!< IEEE address (U64) type */
    ESP_ZB_ZCL_ATTR_TYPE_128_BIT_KEY        = 0xf1U,        /*!< 128-bit security key */
    ESP_ZB_ZCL_ATTR_TYPE_INVALID            = 0xffU,        /*!< Invalid data type */
} esp_zb_zcl_attr_type_t;

/**
 * @brief ZCL attribute access values
 * @anchor esp_zb_zcl_attr_access
 */
typedef enum {
    ESP_ZB_ZCL_ATTR_ACCESS_READ_ONLY  = 0x01U,   /*!< Attribute is read only */
    ESP_ZB_ZCL_ATTR_ACCESS_WRITE_ONLY = 0x02U,   /*!< Attribute is write only */
    ESP_ZB_ZCL_ATTR_ACCESS_READ_WRITE = 0x03U,   /*!< Attribute is read/write */
    ESP_ZB_ZCL_ATTR_ACCESS_REPORTING  = 0x04U,   /*!< Attribute is allowed for reporting */
    ESP_ZB_ZCL_ATTR_ACCESS_SINGLETON  = 0x08U,   /*!< Attribute is singleton */
    ESP_ZB_ZCL_ATTR_ACCESS_SCENE      = 0x10U,   /*!< Attribute is accessed through scene */
    ESP_ZB_ZCL_ATTR_MANUF_SPEC        = 0x20U,   /*!< Attribute is manufacturer specific */
    ESP_ZB_ZCL_ATTR_ACCESS_INTERNAL   = 0x40U,   /*!< Internal access only Attribute */
} esp_zb_zcl_attr_access_t;

/**
 * @brief The ZCL attribute location information struct
 *
 */
typedef struct esp_zb_zcl_attr_location_info_s {
    uint8_t endpoint_id;                    /*!< The endpoint identifier on which the cluster id is resident. */
    uint16_t cluster_id;                    /*!< The cluster identifier on which the attribute is resident, refer to esp_zb_zcl_cluster_id_t */
    uint8_t cluster_role;                   /*!< The role of cluster, refer to esp_zb_zcl_cluster_role_t */
    uint16_t manuf_code;                    /*!< The manufacturer code of attribute */
    uint16_t attr_id;                       /*!< The attribute identifier */
} esp_zb_zcl_attr_location_info_t;

/**
 * @brief ZCL Cluster Check Attribute Value Handler, which should be called before attribute change and checks if new value is in correct range
 *        and can be applied.
 *
 * @param[in] attr_id  ZCL Attribute ID
 * @param[in] endpoint Device endpoint
 * @param[in] value    Pointer to the new Attribute Value
 *
 * @return The result of check value whose value refer to esp_err_t
 */
typedef signed int (*esp_zb_zcl_cluster_check_value_callback_t)(uint16_t attr_id, uint8_t endpoint, uint8_t *value);

/**
 * @brief ZCL Cluster Write Attribute Handler, which should be called before attribute change (if any cluster-specific action needs to
 *        be bound to attribute change, it can be placed in this handler).
 *
 * @param[in] endpoint   Device endpoint
 * @param[in] attr_id    ZCL Attribute ID
 * @param[in] new_value  Pointer to the new Attribute Value
 * @param[in] manuf_code Manufacturer specific code
 */
typedef void (*esp_zb_zcl_cluster_write_attr_callback_t)(uint8_t endpoint, uint16_t attr_id, uint8_t *new_value, uint16_t manuf_code);

/**
 * @brief Get the size of ZCL attribute value
 *
 * @param[in] attr_type  The data type of attribute value
 * @param[in] attr_value The value of attribute
 * @return
 *      - 0x00 - 0xFFFE: The size of attribute value
 *      - 0xFFFF: Invalid size
 */
uint16_t esp_zb_zcl_get_attribute_size(uint8_t attr_type, uint8_t *attr_value);

/**
 * @brief Put the ZCL attribute value to specific memory
 *
 * @param data_ptr      A pointer of specific memory
 * @param type          The data type of attribute value
 * @param value         The value of attribute
 * @param value_size    The size of attribute value
 * @return              A pointer indicates the end location in specific memory after a value has been stored
 */
uint8_t *esp_zb_zcl_put_attribute_value(uint8_t *data_ptr, uint8_t type, uint8_t *value, uint16_t value_size);

#ifdef __cplusplus
}
#endif
