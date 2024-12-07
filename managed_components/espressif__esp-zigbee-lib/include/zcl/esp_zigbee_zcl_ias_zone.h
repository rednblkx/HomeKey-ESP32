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

/** @brief IAS Zone cluster attribute identifiers
*/
typedef enum {
    ESP_ZB_ZCL_ATTR_IAS_ZONE_ZONESTATE_ID                                       = 0x0000,    /*!< ZoneState attribute */
    ESP_ZB_ZCL_ATTR_IAS_ZONE_ZONETYPE_ID                                        = 0x0001,    /*!< ZoneType attribute */
    ESP_ZB_ZCL_ATTR_IAS_ZONE_ZONESTATUS_ID                                      = 0x0002,    /*!< ZoneStatus attribute */
    ESP_ZB_ZCL_ATTR_IAS_ZONE_IAS_CIE_ADDRESS_ID                                 = 0x0010,    /*!< IAS_CIE_Address attribute */
    ESP_ZB_ZCL_ATTR_IAS_ZONE_ZONEID_ID                                          = 0x0011,    /*!< ZoneID attribute */
    ESP_ZB_ZCL_ATTR_IAS_ZONE_NUMBER_OF_ZONE_SENSITIVITY_LEVELS_SUPPORTED_ID     = 0x0012,    /*!< NumberOfZoneSensitivityLevelsSupported attribute */
    ESP_ZB_ZCL_ATTR_IAS_ZONE_CURRENT_ZONE_SENSITIVITY_LEVEL_ID                  = 0x0013,    /*!< CurrentZoneSensitivityLevel attribute */
    ESP_ZB_ZCL_ATTR_CUSTOM_CIE_ADDR_IS_SET                                      = 0xE000,    /*!< Custom CIE address for checking establishment and authorization internally*/
    ESP_ZB_ZCL_ATTR_CUSTOM_CIE_EP                                               = 0xE001,    /*!< Custom CIE endpoint for checking establishment and authorization internally*/
    ESP_ZB_ZCL_ATTR_CUSTOM_CIE_SHORT_ADDR                                       = 0xE002,    /*!< Custom CIE short address for checking establishment and authorization internally*/
    ESP_ZB_ZCL_ATTR_IAS_ZONE_INT_CTX_ID                                         = 0xeffe,    /*!< Application context */
} esp_zb_zcl_ias_zone_attr_t;

/** @brief IAS Zone ZoneState value
 */
typedef enum {
    ESP_ZB_ZCL_IAS_ZONE_ZONESTATE_NOT_ENROLLED  = 0,        /*!< ZoneState not enrolled value */
    ESP_ZB_ZCL_IAS_ZONE_ZONESTATE_ENROLLED      = 1,        /*!< ZoneState enrolled value */
} esp_zb_zcl_ias_zone_zonestate_t;

/** @brief IAS Zone ZoneType value
 */
typedef enum {
    ESP_ZB_ZCL_IAS_ZONE_ZONETYPE_STANDARD_CIE           = 0x0000,  /*!< ZoneType Standard CIE System Alarm value */
    ESP_ZB_ZCL_IAS_ZONE_ZONETYPE_MOTION                 = 0x000d,  /*!< ZoneType Motion value */
    ESP_ZB_ZCL_IAS_ZONE_ZONETYPE_CONTACT_SWITCH         = 0x0015,  /*!< ZoneType Contact switch value */
    ESP_ZB_ZCL_IAS_ZONE_ZONETYPE_FIRE_SENSOR            = 0x0028,  /*!< ZoneType Fire sensor value */
    ESP_ZB_ZCL_IAS_ZONE_ZONETYPE_WATER_SENSOR           = 0x002a,  /*!< ZoneType Water sensor value */
    ESP_ZB_ZCL_IAS_ZONE_ZONETYPE_GAS_SENSOR             = 0x002b,  /*!< ZoneType Gas sensor value */
    ESP_ZB_ZCL_IAS_ZONE_ZONETYPE_PERSONAL_EMERGENCY     = 0x002c,  /*!< ZoneType Personal emergency value */
    ESP_ZB_ZCL_IAS_ZONE_ZONETYPE_VIBRATION_MOVEMENT     = 0x002d,  /*!< ZoneType Vibration / Movement sensor value */
    ESP_ZB_ZCL_IAS_ZONE_ZONETYPE_REMOTE_CONTROL         = 0x010f,  /*!< ZoneType Remote Control value */
    ESP_ZB_ZCL_IAS_ZONE_ZONETYPE_KEY_FOB                = 0x0115,  /*!< ZoneType Key fob value */
    ESP_ZB_ZCL_IAS_ZONE_ZONETYPE_KEYPAD                 = 0x021d,  /*!< ZoneType Keypad value */
    ESP_ZB_ZCL_IAS_ZONE_ZONETYPE_STANDARD_WARNING       = 0x0225,  /*!< ZoneType Standard Warning Device value */
    ESP_ZB_ZCL_IAS_ZONE_ZONETYPE_GLASS_BREAK            = 0x0225,  /*!< ZoneType Standard glass break sensor */
    ESP_ZB_ZCL_IAS_ZONE_ZONETYPE_INVALID                = 0xffff,  /*!< ZoneType Invalid Zone Type value */
} esp_zb_zcl_ias_zone_zonetype_t;

/** @brief IAS Zone ZoneStatus attribute flags
*/
typedef enum {
    ESP_ZB_ZCL_IAS_ZONE_ZONE_STATUS_ALARM1          = 1 << 0,    /*!< Alarm 1 */
    ESP_ZB_ZCL_IAS_ZONE_ZONE_STATUS_ALARM2          = 1 << 1,    /*!< Alarm 2 */
    ESP_ZB_ZCL_IAS_ZONE_ZONE_STATUS_TAMPER          = 1 << 2,    /*!< Tamper */
    ESP_ZB_ZCL_IAS_ZONE_ZONE_STATUS_BATTERY         = 1 << 3,    /*!< Battery */
    ESP_ZB_ZCL_IAS_ZONE_ZONE_STATUS_SUPERVISION     = 1 << 4,    /*!< Supervision reports */
    ESP_ZB_ZCL_IAS_ZONE_ZONE_STATUS_RESTORE         = 1 << 5,    /*!< Restore reports */
    ESP_ZB_ZCL_IAS_ZONE_ZONE_STATUS_TROUBLE         = 1 << 6,    /*!< Trouble */
    ESP_ZB_ZCL_IAS_ZONE_ZONE_STATUS_AC_MAINS        = 1 << 7,    /*!< AC (mains) */
    ESP_ZB_ZCL_IAS_ZONE_ZONE_STATUS_TEST            = 1 << 8,    /*!< Test */
    ESP_ZB_ZCL_IAS_ZONE_ZONE_STATUS_BATTERY_DEFECT  = 1 << 9,    /*!< Battery Defect */
} esp_zb_zcl_ias_zone_zonestatus_t;

#define ESP_ZB_ZCL_ZONE_IAS_CIE_ADDR_DEFAULT    {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff}

/** @brief IAS Zone cluster command identifiers for client
*/
typedef enum {
    ESP_ZB_ZCL_CMD_IAS_ZONE_ZONE_ENROLL_RESPONSE_ID                 = 0x00,    /*!< "Zone Enroll Response" command */
    ESP_ZB_ZCL_CMD_IAS_ZONE_INITIATE_NORMAL_OPERATION_MODE_ID       = 0x01,    /*!< "Initiate Normal Operation Mode" command */
    ESP_ZB_ZCL_CMD_IAS_ZONE_INITIATE_TEST_MODE_ID                   = 0x02,    /*!< "Initiate Test Mode" command */
} esp_zb_zcl_ias_zone_cmd_t;

/** @brief IAS Zone cluster command identifiers for server
*/
typedef enum {
    ESP_ZB_ZCL_CMD_IAS_ZONE_ZONE_STATUS_CHANGE_NOT_ID = 0x00,  /*!< "Zone Status Change Notification" command */
    ESP_ZB_ZCL_CMD_IAS_ZONE_ZONE_ENROLL_REQUEST_ID    = 0x01   /*!< "Zone Enroll Request" command. */
} esp_zb_zcl_ias_zone_resp_cmd_t;

/** @brief Values of the Enroll Response Code defintion
*/
typedef enum {
    ESP_ZB_ZCL_IAS_ZONE_ENROLL_RESPONSE_CODE_SUCCESS          = 0x00,  /*!< Success */
    ESP_ZB_ZCL_IAS_ZONE_ENROLL_RESPONSE_CODE_NOT_SUPPORTED    = 0x01,  /*!< Not supported */
    ESP_ZB_ZCL_IAS_ZONE_ENROLL_RESPONSE_CODE_NO_ENROLL        = 0x02,  /*!< No enroll permit */
    ESP_ZB_ZCL_IAS_ZONE_ENROLL_RESPONSE_CODE_TOO_MANY_ZONES   = 0x03,  /*!< Too many zones  */
} esp_zb_zcl_ias_zone_enroll_response_code_t;

#ifdef __cplusplus
}
#endif
