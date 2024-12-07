/*
 * Copyright (c) 2012-2022 DSR Corporation, Denver CO, USA
 * Copyright (c) 2021-2022 Espressif Systems (Shanghai) PTE LTD
 * All rights reserved.
 *
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form, except as embedded into a Espressif Systems
 *    integrated circuit in a product or a software update for such product,
 *    must reproduce the above copyright notice, this list of conditions and
 *    the following disclaimer in the documentation and/or other materials
 *    provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors
 *    may be used to endorse or promote products derived from this software without
 *    specific prior written permission.
 *
 * 4. Any software provided in binary form under this license must not be reverse
 *    engineered, decompiled, modified and/or disassembled.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */
/* PURPOSE: IAS Zone cluster definitions
*/

#ifndef ZB_ZCL_IAS_ZONE_H
#define ZB_ZCL_IAS_ZONE_H 1

#include "zcl/zb_zcl_common.h"
#include "zcl/zb_zcl_commands.h"

/** @cond DOXYGEN_ZCL_SECTION */

/** @addtogroup ZB_ZCL_IAS_ZONE
 *  @{
 *    @details
 *    IAS Zone cluster definitions
 */

/** @cond internals_doc */
/** @brief Hook on Write attribute
 * send Zone Status Change Notification Command if change ZoneStatus attribute */
void zb_zcl_ias_zone_write_attr_hook(zb_uint8_t endpoint, zb_uint16_t attr_id, zb_uint8_t *new_value);
/*! @}
 *  @endcond */ /* internals_doc */

/* Cluster ZB_ZCL_CLUSTER_ID_IAS_ZONE */

/*! @name IAS Zone cluster attributes
    @{
*/

/*! @brief IAS Zone cluster attribute identifiers
    @see ZCL spec, IAS Zone Cluster 8.2.2.2
*/
enum zb_zcl_ias_zone_attr_e
{
  /*! @brief ZoneState attribute, ZCL spec 8.2.2.2.1.1 */
  ZB_ZCL_ATTR_IAS_ZONE_ZONESTATE_ID = 0x0000,
  /*! @brief ZoneType attribute, ZCL spec 8.2.2.2.1.2 */
  ZB_ZCL_ATTR_IAS_ZONE_ZONETYPE_ID = 0x0001,
  /*! @brief ZoneStatus attribute, ZCL spec 8.2.2.2.1.3 */
  ZB_ZCL_ATTR_IAS_ZONE_ZONESTATUS_ID = 0x0002,
  /*! @brief IAS_CIE_Address  attribute, ZCL spec 8.2.2.2.2.1. */
  ZB_ZCL_ATTR_IAS_ZONE_IAS_CIE_ADDRESS_ID = 0x0010,
  /*! @brief ZoneID attribute, ZCL spec 8.2.2.1.2.2 */
  ZB_ZCL_ATTR_IAS_ZONE_ZONEID_ID = 0x0011,
  /*! @brief NumberOfZoneSensitivityLevelsSupported attribute, ZCL spec 8.2.2.1.2.3 */
  ZB_ZCL_ATTR_IAS_ZONE_NUMBER_OF_ZONE_SENSITIVITY_LEVELS_SUPPORTED_ID = 0x0012,
  /*! @brief CurrentZoneSensitivityLevel attribute, ZCL spec 8.2.2.1.2.4 */
  ZB_ZCL_ATTR_IAS_ZONE_CURRENT_ZONE_SENSITIVITY_LEVEL_ID = 0x0013,
  /* custom attribute */
  ZB_ZCL_ATTR_CUSTOM_ZGP_CALIBRATION = 0x8000,
  ZB_ZCL_ATTR_CUSTOM_ZGP_CLOUD_ACK = 0x8003,
  /* IAS_CIE_Address can be reassigned via Write attribute only when ZB_ZCL_ATTR_CUSTOM_CIE_ADDR_IS_SET is equal to ZB_FALSE */
  ZB_ZCL_ATTR_CUSTOM_CIE_ADDR_IS_SET = 0xE000,
  ZB_ZCL_ATTR_CUSTOM_CIE_EP = 0xE001,
  ZB_ZCL_ATTR_CUSTOM_CIE_SHORT_ADDR = 0xE002,
  /* TODO: move this attribute to IAS Zone ZCL implementation */
  /*! @brief Struct with pointers on User App callbacks */
  ZB_ZCL_ATTR_IAS_ZONE_INT_CTX_ID = 0xeffe,
};

/**
 * @brief IAS Zone ZoneState value
 */
enum zb_zcl_ias_zone_zonestate_e
{
  /** @brief ZoneState not enrolled value */
    ZB_ZCL_IAS_ZONE_ZONESTATE_NOT_ENROLLED  = 0,

  /** @brief ZoneState enrolled value */
    ZB_ZCL_IAS_ZONE_ZONESTATE_ENROLLED  = 1,
};

/** @brief IAS Zone ZoneState attribute default value */
#define ZB_ZCL_IAS_ZONE_ZONESTATE_DEF_VALUE            ZB_ZCL_IAS_ZONE_ZONESTATE_NOT_ENROLLED

/** @brief Default value for IAS Zone cluster revision global attribute */
#define ZB_ZCL_IAS_ZONE_CLUSTER_REVISION_DEFAULT ((zb_uint16_t)0x0002u)

/**
 * @brief IAS Zone ZoneType value
 */
enum zb_zcl_ias_zone_zonetype_e
{
  /** @brief ZoneType Standard CIE System Alarm value */
    ZB_ZCL_IAS_ZONE_ZONETYPE_STANDARD_CIE  = 0x0000,

  /** @brief ZoneType Motion value */
    ZB_ZCL_IAS_ZONE_ZONETYPE_MOTION  = 0x000d,

  /** @brief ZoneType Contact switch value */
    ZB_ZCL_IAS_ZONE_ZONETYPE_CONTACT_SWITCH  = 0x0015,

  /** @brief ZoneType Fire sensor value */
    ZB_ZCL_IAS_ZONE_ZONETYPE_FIRE_SENSOR  = 0x0028,

  /** @brief ZoneType Water sensor value */
    ZB_ZCL_IAS_ZONE_ZONETYPE_WATER_SENSOR  = 0x002a,

  /** @brief ZoneType Gas sensor value */
    ZB_ZCL_IAS_ZONE_ZONETYPE_GAS_SENSOR  = 0x002b,

  /** @brief ZoneType Personal emergency value */
    ZB_ZCL_IAS_ZONE_ZONETYPE_PERSONAL_EMERGENCY  = 0x002c,

  /** @brief ZoneType Vibration / Movement sensor value */
    ZB_ZCL_IAS_ZONE_ZONETYPE_VIBRATION_MOVEMENT  = 0x002d,

  /** @brief ZoneType Remote Control value */
    ZB_ZCL_IAS_ZONE_ZONETYPE_REMOTE_CONTROL  = 0x010f,

  /** @brief ZoneType Key fob value */
    ZB_ZCL_IAS_ZONE_ZONETYPE_KEY_FOB  = 0x0115,

  /** @brief ZoneType Keypad value */
    ZB_ZCL_IAS_ZONE_ZONETYPE_KEYPAD  = 0x021d,

  /** @brief ZoneType Standard Warning Device  value */
    ZB_ZCL_IAS_ZONE_ZONETYPE_STANDARD_WARNING  = 0x0225,

  /** @brief Manufacturer specific ZoneType value */
    ZB_ZCL_IAS_ZONE_ZONETYPE_MANUF_SPEC  = 0x8000,

  /** @brief ZoneType Invalid Zone Type value */
    ZB_ZCL_IAS_ZONE_ZONETYPE_INVALID  = 0xffff,

};

/*! @brief IAS Zone ZoneStatus attribute flags
    @see ZCL spec 8.2.2.2.1.3
*/
enum zb_zcl_ias_zone_zonestatus_e
{
  /** Alarm 1 */
  ZB_ZCL_IAS_ZONE_ZONE_STATUS_ALARM1      = 1 << 0,
  /** Alarm 2 */
  ZB_ZCL_IAS_ZONE_ZONE_STATUS_ALARM2      = 1 << 1,
  /** Tamper */
  ZB_ZCL_IAS_ZONE_ZONE_STATUS_TAMPER      = 1 << 2,
  /** Battery */
  ZB_ZCL_IAS_ZONE_ZONE_STATUS_BATTERY     = 1 << 3,
  /** Supervision reports */
  ZB_ZCL_IAS_ZONE_ZONE_STATUS_SUPERVISION = 1 << 4,
  /** Restore reports */
  ZB_ZCL_IAS_ZONE_ZONE_STATUS_RESTORE     = 1 << 5,
  /** Trouble */
  ZB_ZCL_IAS_ZONE_ZONE_STATUS_TROUBLE     = 1 << 6,
  /** AC (mains) */
  ZB_ZCL_IAS_ZONE_ZONE_STATUS_AC_MAINS    = 1 << 7,
  /** Test */
  ZB_ZCL_IAS_ZONE_ZONE_STATUS_TEST            = 1 << 8,
  /** Battery Defect */
  ZB_ZCL_IAS_ZONE_ZONE_STATUS_BATTERY_DEFECT  = 1 << 9,
};

/** @brief IAS Zone ZoneStatus attribute default value */
#define ZB_ZCL_IAS_ZONE_ZONE_STATUS_DEF_VALUE            0

/** @brief Min value for NumberOfZoneSensitivityLevelsSupported attribute */
#define ZB_ZCL_IAS_ZONE_NUMBER_OF_ZONE_SENSITIVITY_LEVELS_SUPPORTED_MIN_VALUE ((zb_uint8_t)0x02)

/** @brief Default value for NumberOfZoneSensitivityLevelsSupported attribute */
#define ZB_ZCL_IAS_ZONE_NUMBER_OF_ZONE_SENSITIVITY_LEVELS_SUPPORTED_DEFAULT_VALUE \
  ZB_ZCL_IAS_ZONE_NUMBER_OF_ZONE_SENSITIVITY_LEVELS_SUPPORTED_MIN_VALUE

/** @brief Default value for CurrentZoneSensitivityLevel attribute */
#define ZB_ZCL_IAS_ZONE_CURRENT_ZONE_SENSITIVITY_LEVEL_DEFAULT_VALUE ((zb_uint8_t)0x00)

/** @brief IAS Zone ZoneID attribute default value */
#define ZB_ZCL_IAS_ZONEID_ID_DEF_VALUE            0xff

/** @cond internals_doc */
/** This macros should not be used by the user application directly */
/*! @internal @name IAS Zone cluster internals
    Internal structures for attribute representation in cluster definitions.
    @{
*/

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_CUSTOM_CIE_ADDR_IS_SET(data_ptr) \
{                                                       \
  ZB_ZCL_ATTR_CUSTOM_CIE_ADDR_IS_SET,                   \
  ZB_ZCL_ATTR_TYPE_U8,                                  \
  ZB_ZCL_ATTR_ACCESS_INTERNAL,                          \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                   \
  (void*) data_ptr                                      \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_CUSTOM_CIE_EP(data_ptr) \
{                                                       \
  ZB_ZCL_ATTR_CUSTOM_CIE_EP,                            \
  ZB_ZCL_ATTR_TYPE_U8,                                  \
  ZB_ZCL_ATTR_ACCESS_INTERNAL,                          \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                   \
  (void*) data_ptr                                      \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_CUSTOM_CIE_SHORT_ADDR(data_ptr) \
{                                                       \
  ZB_ZCL_ATTR_CUSTOM_CIE_SHORT_ADDR,                    \
  ZB_ZCL_ATTR_TYPE_U16,                                 \
  ZB_ZCL_ATTR_ACCESS_INTERNAL,                          \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                   \
  (void*) data_ptr                                      \
}


#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_IAS_ZONE_ZONESTATE_ID(data_ptr) \
{                                                       \
  ZB_ZCL_ATTR_IAS_ZONE_ZONESTATE_ID,                    \
  ZB_ZCL_ATTR_TYPE_8BIT_ENUM,                           \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                         \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                   \
  (void*) data_ptr                                      \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_IAS_ZONE_ZONETYPE_ID(data_ptr) \
{                                                       \
  ZB_ZCL_ATTR_IAS_ZONE_ZONETYPE_ID,                     \
  ZB_ZCL_ATTR_TYPE_16BIT_ENUM,                          \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                         \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                   \
  (void*) data_ptr                                      \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_IAS_ZONE_ZONESTATUS_ID(data_ptr) \
{                                                               \
  ZB_ZCL_ATTR_IAS_ZONE_ZONESTATUS_ID,                           \
  ZB_ZCL_ATTR_TYPE_16BITMAP,                                    \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY | ZB_ZCL_ATTR_ACCESS_REPORTING,  \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                           \
  (void*) data_ptr                                              \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_IAS_ZONE_IAS_CIE_ADDRESS_ID(data_ptr) \
{                                                       \
  ZB_ZCL_ATTR_IAS_ZONE_IAS_CIE_ADDRESS_ID,              \
  ZB_ZCL_ATTR_TYPE_IEEE_ADDR,                           \
  ZB_ZCL_ATTR_ACCESS_READ_WRITE,                        \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                   \
  (void*) data_ptr                                      \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_IAS_ZONE_ZONEID_ID(data_ptr) \
{                                                       \
  ZB_ZCL_ATTR_IAS_ZONE_ZONEID_ID,                       \
  ZB_ZCL_ATTR_TYPE_U8,                                  \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                         \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                   \
  (void*) data_ptr                                      \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_IAS_ZONE_NUMBER_OF_ZONE_SENSITIVITY_LEVELS_SUPPORTED_ID(data_ptr) \
{                                                                                                   \
  ZB_ZCL_ATTR_IAS_ZONE_NUMBER_OF_ZONE_SENSITIVITY_LEVELS_SUPPORTED_ID,                              \
  ZB_ZCL_ATTR_TYPE_U8,                                                                              \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                                                     \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                                               \
  (void*) data_ptr                                                                                  \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_IAS_ZONE_CURRENT_ZONE_SENSITIVITY_LEVEL_ID(data_ptr) \
{                                                                                               \
  ZB_ZCL_ATTR_IAS_ZONE_CURRENT_ZONE_SENSITIVITY_LEVEL_ID,                                       \
  ZB_ZCL_ATTR_TYPE_U8,                                                                          \
  ZB_ZCL_ATTR_ACCESS_READ_WRITE,                                                                \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                                           \
  (void*) data_ptr                                                                              \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_IAS_ZONE_INT_CTX_ID(data_ptr) \
{                                                       \
  ZB_ZCL_ATTR_IAS_ZONE_INT_CTX_ID,                      \
  ZB_ZCL_ATTR_TYPE_NULL,                                \
  ZB_ZCL_ATTR_ACCESS_INTERNAL,                          \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                   \
  (void*) data_ptr                                      \
}

/*! @internal Number of attributes mandatory for reporting in IAS Zone cluster */
#define ZB_ZCL_IAS_ZONE_REPORT_ATTR_COUNT 1

/*! @}
 *  @endcond */ /* internals_doc */

/** @brief Declare attribute list for IAS Zone cluster - server side
    @param attr_list - attribute list name
    @param zone_state - pointer to variable to store ZoneState attribute
    @param zone_type - pointer to variable to store ZoneType attribute
    @param zone_status - pointer to variable to store ZoneStatus attribute
    @param ias_cie_address - pointer to variable to store IAS-CIE address attribute
    @param cie_short_addr - custom attribute to store CIE short address
    @param cie_ep - custom attribute to store CIE Endpoint number
*/
/* FIXME: declare custom attributes internally */
#define ZB_ZCL_DECLARE_IAS_ZONE_ATTRIB_LIST(                                                 \
  attr_list, zone_state, zone_type, zone_status,ias_cie_address,                             \
  cie_short_addr, cie_ep)                                                                    \
  zb_uint8_t cie_addr_is_set_##attr_list;                                                    \
  ZB_ZCL_START_DECLARE_ATTRIB_LIST(attr_list)                                                \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_IAS_ZONE_ZONESTATE_ID, (zone_state))                      \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_IAS_ZONE_ZONETYPE_ID, (zone_type))                        \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_IAS_ZONE_ZONESTATUS_ID, (zone_status))                    \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_IAS_ZONE_IAS_CIE_ADDRESS_ID, (ias_cie_address))           \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_CUSTOM_CIE_SHORT_ADDR, (cie_short_addr))                  \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_CUSTOM_CIE_EP, (cie_ep))                                  \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_CUSTOM_CIE_ADDR_IS_SET, &(cie_addr_is_set_##attr_list))   \
  ZB_ZCL_FINISH_DECLARE_ATTRIB_LIST

/** @brief Declare attribute list for IAS Zone cluster - server side (extended attribute set)
    @param attr_list - attribute list name
    @param zone_state - pointer to variable to store ZoneState attribute
    @param zone_type - pointer to variable to store ZoneType attribute
    @param zone_status - pointer to variable to store ZoneStatus attribute
    @param ias_cie_address - pointer to variable to store IAS-CIE address attribute
    @param zone_id - pointer to variable to store Zone ID attribute
    @param number_of_zone_sens_levels_supported - pointer to variable to store
    NumberOfZoneSensitivityLevelsSupported attribute
    @param current_zone_sens_level - pointer to variable to store CurrentZoneSensitivityLevel attribute
    @param cie_short_addr - custom attribute to store CIE short address
    @param cie_ep - custom attribute to store CIE Endpoint number
*/
/* FIXME: declare custom attributes internally */
#define ZB_ZCL_DECLARE_IAS_ZONE_ATTRIB_LIST_EXT(                                                                 \
  attr_list, zone_state, zone_type, zone_status, number_of_zone_sens_levels_supported, current_zone_sens_level,  \
  ias_cie_address, zone_id, cie_short_addr, cie_ep)                                                              \
  zb_uint8_t cie_addr_is_set_##attr_list;                                                                        \
  zb_uint16_t last_change_##attr_list;                                                                           \
  zb_zcl_ias_zone_int_ctx_t int_ctx_##attr_list;                                                                 \
  ZB_ZCL_START_DECLARE_ATTRIB_LIST_CLUSTER_REVISION(attr_list, ZB_ZCL_IAS_ZONE)                                  \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_IAS_ZONE_ZONESTATE_ID, (zone_state))                                          \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_IAS_ZONE_ZONETYPE_ID, (zone_type))                                            \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_IAS_ZONE_ZONESTATUS_ID, (zone_status))                                        \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_IAS_ZONE_IAS_CIE_ADDRESS_ID, (ias_cie_address))                               \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_IAS_ZONE_ZONEID_ID, (zone_id))                                                \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_IAS_ZONE_NUMBER_OF_ZONE_SENSITIVITY_LEVELS_SUPPORTED_ID,                      \
                       (number_of_zone_sens_levels_supported))                                                   \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_IAS_ZONE_CURRENT_ZONE_SENSITIVITY_LEVEL_ID,                                   \
                       (current_zone_sens_level))                                                                \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_IAS_ZONE_INT_CTX_ID, &(int_ctx_##attr_list))                                  \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_CUSTOM_CIE_SHORT_ADDR, (cie_short_addr))                                      \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_CUSTOM_CIE_EP, (cie_ep))                                                      \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_CUSTOM_CIE_ADDR_IS_SET, &(cie_addr_is_set_##attr_list))                       \
  ZB_ZCL_FINISH_DECLARE_ATTRIB_LIST

/*! @} */ /* IAS Zone cluster attributes */

/*! @name IAS Zone cluster commands
    @{
*/

/*! @brief IAS Zone cluster command identifiers
    @see ZCL spec, IAS Zone Cluster, 8.2.2.3
*/
enum zb_zcl_ias_zone_cmd_e
{
  ZB_ZCL_CMD_IAS_ZONE_ZONE_ENROLL_RESPONSE_ID        = 0x00, /**< "Zone Enroll Response" command,
                                                              * ZCL spec 8.2.2.3.1 */
  /**< "Initiate Normal Operation Mode" command, ZCL spec 8.2.2.2.1 */
  ZB_ZCL_CMD_IAS_ZONE_INITIATE_NORMAL_OPERATION_MODE_ID        = 0x01,
  /**< "Initiate Test Mode" command, ZCL spec 8.2.2.2.2.3 */
  ZB_ZCL_CMD_IAS_ZONE_INITIATE_TEST_MODE_ID        = 0x02,
};


/*! @brief IAS Zone cluster response command identifiers
    @see ZCL spec, IAS Zone Cluster, 8.2.2.4
*/
enum zb_zcl_ias_zone_resp_cmd_e
{
  ZB_ZCL_CMD_IAS_ZONE_ZONE_STATUS_CHANGE_NOT_ID = 0x00,  /**< "Zone Status Change Notification" command.
                                                          ZCL spec 8.2.2.4.1 */
  ZB_ZCL_CMD_IAS_ZONE_ZONE_ENROLL_REQUEST_ID    = 0x01   /**< "Zone Enroll Request" command.
                                                          ZCL spec 8.2.2.4.2 */
};

/** @cond internals_doc */
/* Ias zone cluster commands list : only for information - do not modify */
#define ZB_ZCL_CLUSTER_ID_IAS_ZONE_SERVER_ROLE_RECEIVED_CMD_LIST        \
  ZB_ZCL_CMD_IAS_ZONE_ZONE_ENROLL_RESPONSE_ID,                          \
  ZB_ZCL_CMD_IAS_ZONE_INITIATE_NORMAL_OPERATION_MODE_ID,                \
  ZB_ZCL_CMD_IAS_ZONE_INITIATE_TEST_MODE_ID

#define ZB_ZCL_CLUSTER_ID_IAS_ZONE_CLIENT_ROLE_RECEIVED_CMD_LIST                             \
                                             ZB_ZCL_CMD_IAS_ZONE_ZONE_STATUS_CHANGE_NOT_ID,  \
                                             ZB_ZCL_CMD_IAS_ZONE_ZONE_ENROLL_REQUEST_ID

#define ZB_ZCL_CLUSTER_ID_IAS_ZONE_SERVER_ROLE_GENERATED_CMD_LIST ZB_ZCL_CLUSTER_ID_IAS_ZONE_CLIENT_ROLE_RECEIVED_CMD_LIST

#define ZB_ZCL_CLUSTER_ID_IAS_ZONE_CLIENT_ROLE_GENERATED_CMD_LIST ZB_ZCL_CLUSTER_ID_IAS_ZONE_SERVER_ROLE_RECEIVED_CMD_LIST
/*! @}
 *  @endcond */ /* internals_doc */

/*! @brief Structure representsation of Initiate Test Mode - Errata to 05-3520-29 Spec, 1.1.23.2.2.1 */
typedef ZB_PACKED_PRE struct zb_zcl_ias_zone_init_test_mode_ha_s
{
  /** Test mode duration */
  zb_uint8_t test_mode_duration;
  /** Current Zone Sensitivity Level */
  zb_uint8_t current_zone_sens_level;
} ZB_PACKED_STRUCT zb_zcl_ias_zone_init_test_mode_t;

/** @brief Macro for getting "Initiate Test Mode" command
  * @attention Assumes that ZCL header already cut.
  * @param data_ptr - pointer to a variable of type @ref
  * zb_zcl_ias_zone_init_test_mode_t.
  * @param buffer containing the packet (by pointer).
  * @param status - variable to put parse status to (see @ref zb_zcl_parse_status_t).
  */
#define ZB_ZCL_IAS_ZONE_GET_INITIATE_TEST_MODE_REQ(data_ptr, buffer, status) \
{                                                                            \
  if (zb_buf_len((buffer)) < sizeof(zb_zcl_ias_zone_init_test_mode_t))       \
  {                                                                          \
   (status) = ZB_ZCL_PARSE_STATUS_FAILURE;                                   \
  }                                                                          \
  else                                                                       \
  {                                                                          \
    zb_zcl_ias_zone_init_test_mode_t *src_ptr =                              \
         (zb_zcl_ias_zone_init_test_mode_t*)zb_buf_begin((buffer));          \
    (status) = ZB_ZCL_PARSE_STATUS_SUCCESS;                                  \
    (data_ptr)->test_mode_duration = src_ptr->test_mode_duration;            \
    (data_ptr)->current_zone_sens_level = src_ptr->current_zone_sens_level;  \
  }                                                                          \
}

/*! @brief Initiate Test Mode command
    @param buffer - to put packet to
    @param addr - address to send packet to
    @param dst_addr_mode - addressing mode
    @param dst_ep - destination endpoint
    @param ep - sending endpoint
    @param prfl_id - profile identifier
    @param cb - callback for getting command send status
    @param test_mode_duration - Test Mode duration
    @param current_zone_sens_level - CurrentZoneSensitivityLevel
*/
#define ZB_ZCL_IAS_ZONE_SEND_INITIATE_TEST_MODE_REQ(                                            \
    buffer, addr, dst_addr_mode, dst_ep, ep, prfl_id, cb,                                       \
    test_mode_duration, current_zone_sens_level)                                                \
{                                                                                               \
  zb_uint8_t* ptr = ZB_ZCL_START_PACKET_REQ(buffer)                                             \
  ZB_ZCL_CONSTRUCT_SPECIFIC_COMMAND_REQ_FRAME_CONTROL(ptr, ZB_ZCL_ENABLE_DEFAULT_RESPONSE)      \
  ZB_ZCL_CONSTRUCT_COMMAND_HEADER_REQ(ptr, ZB_ZCL_GET_SEQ_NUM(),                                \
      ZB_ZCL_CMD_IAS_ZONE_INITIATE_TEST_MODE_ID);                                               \
  ZB_ZCL_PACKET_PUT_DATA8(ptr, (test_mode_duration));                                           \
  ZB_ZCL_PACKET_PUT_DATA8(ptr, (current_zone_sens_level));                                      \
  ZB_ZCL_FINISH_PACKET((buffer), ptr)                                                           \
  ZB_ZCL_SEND_COMMAND_SHORT(                                                                    \
      buffer, addr, dst_addr_mode, dst_ep, ep, prfl_id,                                         \
      ZB_ZCL_CLUSTER_ID_IAS_ZONE, cb);                                                          \
}

/*! @brief Initiate Normal Operation Mode command
    @param buffer - to put packet to
    @param addr - address to send packet to
    @param dst_addr_mode - addressing mode
    @param dst_ep - destination endpoint
    @param ep - sending endpoint
    @param prfl_id - profile identifier
    @param cb - callback for getting command send status
*/
#define ZB_ZCL_IAS_ZONE_SEND_INITIATE_NORMAL_OPERATION_MODE_REQ(                                \
    buffer, addr, dst_addr_mode, dst_ep, ep, prfl_id, cb)                                       \
{                                                                                               \
  zb_uint8_t* ptr = ZB_ZCL_START_PACKET_REQ(buffer)                                             \
  ZB_ZCL_CONSTRUCT_SPECIFIC_COMMAND_REQ_FRAME_CONTROL(ptr, ZB_ZCL_ENABLE_DEFAULT_RESPONSE)      \
  ZB_ZCL_CONSTRUCT_COMMAND_HEADER_REQ(ptr, ZB_ZCL_GET_SEQ_NUM(),                                \
      ZB_ZCL_CMD_IAS_ZONE_INITIATE_NORMAL_OPERATION_MODE_ID);                                   \
  ZB_ZCL_FINISH_PACKET((buffer), ptr)                                                           \
  ZB_ZCL_SEND_COMMAND_SHORT(                                                                    \
      buffer, addr, dst_addr_mode, dst_ep, ep, prfl_id,                                         \
      ZB_ZCL_CLUSTER_ID_IAS_ZONE, cb);                                                          \
}

/******************************* Zone Status Change Notification ******************************/

/*! @brief Structure representsation of Zone Status Change Notification
 * see ZCL spec 8.2.2.4.1.1 */
typedef ZB_PACKED_PRE struct zb_zcl_ias_zone_status_change_not_s
{
  /** Zone Status, see @ref zb_zcl_ias_zone_zonestatus_e*/
  zb_uint16_t zone_status;
  /** Extended Status */
  zb_uint8_t extended_status;
  /** Zone ID*/
  zb_uint8_t zone_id;
  /** Delay */
  zb_uint16_t delay;
} ZB_PACKED_STRUCT zb_zcl_ias_zone_status_change_not_t;


/*! @brief Zone Status Change Notification command
    @param buffer - to put packet to
    @param addr - address to send packet to
    @param dst_addr_mode - addressing mode
    @param dst_ep - destination endpoint
    @param ep - sending endpoint
    @param prfl_id - profile identifier
    @param cb - callback for getting command send status
    @param zone_status - Zone Status, see @ref zb_zcl_ias_zone_zonestatus_e
    @param extended_status - Extended Status
    @param zone_id - Zone ID
    @param delay - Delay
*/
#define ZB_ZCL_IAS_ZONE_SEND_STATUS_CHANGE_NOTIFICATION_REQ(                \
    buffer, addr, dst_addr_mode, dst_ep, ep, prfl_id, cb,                   \
    zone_status, extended_status, zone_id, delay )                          \
{                                                                           \
  zb_uint8_t* ptr = ZB_ZCL_START_PACKET(buffer);                            \
  ZB_ZCL_CONSTRUCT_SPECIFIC_COMMAND_RES_FRAME_CONTROL(ptr);                 \
  ZB_ZCL_CONSTRUCT_COMMAND_HEADER(ptr, ZB_ZCL_GET_SEQ_NUM(),                \
      ZB_ZCL_CMD_IAS_ZONE_ZONE_STATUS_CHANGE_NOT_ID);                       \
  ZB_ZCL_PACKET_PUT_DATA16_VAL(ptr, (zone_status));                         \
  ZB_ZCL_PACKET_PUT_DATA8(ptr, (extended_status));                          \
  ZB_ZCL_PACKET_PUT_DATA8(ptr, (zone_id));                                  \
  ZB_ZCL_PACKET_PUT_DATA16_VAL(ptr, (delay));                               \
  ZB_ZCL_FINISH_PACKET((buffer), ptr)                                       \
  ZB_ZCL_SEND_COMMAND_SHORT(                                                \
      buffer, addr, dst_addr_mode, dst_ep, ep, prfl_id,                     \
      ZB_ZCL_CLUSTER_ID_IAS_ZONE, cb);                                      \
}

/** @brief Macro for getting Zone Status Change Notification command
  * @attention Assumes that ZCL header already cut.
  * @param data_ptr - pointer to a variable of type @ref
  * zb_zcl_ias_zone_status_change_not_s.
  * @param buffer containing the packet (by pointer).
  * @param status - variable to put parse status to (see @ref zb_zcl_parse_status_t).
  */
#define ZB_ZCL_IAS_ZONE_GET_STATUS_CHANGE_NOTIFICATION_REQ(data_ptr, buffer, status)     \
{                                                                           \
  if (zb_buf_len((buffer)) < sizeof(zb_zcl_ias_zone_status_change_not_t))   \
  {                                                                         \
   (status) = ZB_ZCL_PARSE_STATUS_FAILURE;                                  \
  }                                                                         \
  else                                                                      \
  {                                                                         \
    zb_zcl_ias_zone_status_change_not_t *src_ptr =                          \
         (zb_zcl_ias_zone_status_change_not_t*)zb_buf_begin((buffer));      \
    (status) = ZB_ZCL_PARSE_STATUS_SUCCESS;                                 \
    ZB_HTOLE16(&((data_ptr)->zone_status), &(src_ptr->zone_status));        \
    (data_ptr)->extended_status = src_ptr->extended_status;                 \
    (data_ptr)->zone_id = src_ptr->zone_id;                                   \
    ZB_HTOLE16(&((data_ptr)->delay), &(src_ptr->delay));                      \
  }                                                                         \
}

/******************************* Zone Enroll Request command ******************************/

/*! @brief Structure representsation of "Zone Enroll Request" command
 * see ZCL spec 8.2.2.4.1.2 */
typedef ZB_PACKED_PRE struct zb_zcl_ias_zone_enroll_request_s
{
  /** Zone Type, see @ref zb_zcl_ias_zone_zonetype_e*/
  zb_uint16_t zone_type;
  /** Manufacturer code */
  zb_uint16_t manufacturer_code;
} ZB_PACKED_STRUCT zb_zcl_ias_zone_enroll_request_t;


/*! @brief Zone "Zone Enroll Request" command, see ZCL spec 8.2.2.4.2
    @param buffer - to put packet to
    @param addr - address to send packet to
    @param dst_addr_mode - addressing mode
    @param dst_ep - destination endpoint
    @param ep - sending endpoint
    @param prfl_id - profile identifier
    @param cb - callback for getting command send status
    @param zone_type - Zone Type, see @ref zb_zcl_ias_zone_zonetype_e
    @param manufacturer_code - Manufacture code
*/
#define ZB_ZCL_IAS_ZONE_SEND_ZONE_ENROLL_REQUEST_REQ(                       \
    buffer, addr, dst_addr_mode, dst_ep, ep, prfl_id, cb,                   \
    zone_type, manufacturer_code)                                           \
{                                                                           \
  zb_uint8_t* ptr = ZB_ZCL_START_PACKET(buffer);                            \
  ZB_ZCL_CONSTRUCT_SPECIFIC_COMMAND_RES_FRAME_CONTROL(ptr);                 \
  ZB_ZCL_CONSTRUCT_COMMAND_HEADER(ptr, ZB_ZCL_GET_SEQ_NUM(),                \
      ZB_ZCL_CMD_IAS_ZONE_ZONE_ENROLL_REQUEST_ID);                          \
  ZB_ZCL_PACKET_PUT_DATA16_VAL(ptr, (zone_type));                           \
  ZB_ZCL_PACKET_PUT_DATA16_VAL(ptr, (manufacturer_code));                   \
  ZB_ZCL_FINISH_PACKET((buffer), ptr)                                      \
  ZB_ZCL_SEND_COMMAND_SHORT(                                                \
      buffer, addr, dst_addr_mode, dst_ep, ep, prfl_id,                     \
      ZB_ZCL_CLUSTER_ID_IAS_ZONE, cb);                                      \
}

/** @brief Macro for getting Zone Status Change Notification command, see ZCL spec 8.2.2.4.1
  * @attention Assumes that ZCL header already cut.
  * @param data_ptr - pointer to a variable of type @ref
  * zb_zcl_ias_zone_enroll_request_s.
  * @param buffer containing the packet (by pointer).
  * @param status - variable to put parse status to (see @ref zb_zcl_parse_status_t).
  */
#define ZB_ZCL_IAS_ZONE_GET_ZONE_ENROLL_REQUEST_REQ(data_ptr, buffer, status)   \
{                                                                           \
  if (zb_buf_len((buffer)) < sizeof(zb_zcl_ias_zone_enroll_request_t))      \
  {                                                                         \
   (status) = ZB_ZCL_PARSE_STATUS_FAILURE;                                  \
  }                                                                         \
  else                                                                      \
  {                                                                         \
    zb_zcl_ias_zone_enroll_request_t *src_ptr =                             \
         (zb_zcl_ias_zone_enroll_request_t*)zb_buf_begin((buffer));         \
    (status) = ZB_ZCL_PARSE_STATUS_SUCCESS;                                 \
    ZB_HTOLE16(&((data_ptr)->zone_type), &(src_ptr->zone_type));            \
    ZB_HTOLE16(&((data_ptr)->manufacturer_code), &(src_ptr->manufacturer_code));  \
  }                                                                         \
}

/******************************* Zone Enroll response ******************************/

/** @brief Values of the Enroll Response Code
 * see ZCL spec 8.2.2.3.1.1 */
enum zb_zcl_ias_zone_enroll_responce_code_e
{
  /** Success */
  ZB_ZCL_IAS_ZONE_ENROLL_RESPONCE_CODE_SUCCESS          = 0x00,
  /** Not supported */
  ZB_ZCL_IAS_ZONE_ENROLL_RESPONCE_CODE_NOT_SUPPORTED    = 0x01,
  /** No enroll permit */
  ZB_ZCL_IAS_ZONE_ENROLL_RESPONCE_CODE_NO_ENROLL        = 0x02,
  /** Too many zones  */
  ZB_ZCL_IAS_ZONE_ENROLL_RESPONCE_CODE_TOO_MANY_ZONES   = 0x03,
};

/*! @brief Structure representsation of Enroll response command payload
 * see ZCL spec 8.2.2.3.1.1 */
typedef ZB_PACKED_PRE struct zb_zcl_ias_zone_enroll_res_s
{
  /** Enroll response code */
  zb_uint8_t code;
  /** Zone ID */
  zb_uint8_t zone_id;
} ZB_PACKED_STRUCT zb_zcl_ias_zone_enroll_res_t;

/*! @brief Send "Zone Enroll response" command, see ZCL spec 8.2.2.3.1
    @param buffer to put packet to
    @param addr - address to send packet to
    @param dst_addr_mode - addressing mode
    @param dst_ep - destination endpoint
    @param ep - sending endpoint
    @param prfl_id - profile identifier
    @param def_resp - enable/disable default response
    @param cb - callback
    @param code - Enroll response code, see @ref zb_zcl_ias_zone_enroll_responce_code_e
    @param zone_id - Zone ID
*/
#define ZB_ZCL_IAS_ZONE_SEND_ZONE_ENROLL_RES(                               \
    buffer, addr, dst_addr_mode, dst_ep, ep, prfl_id, def_resp, cb,         \
    code, zone_id)                                                          \
{                                                                           \
  zb_uint8_t* ptr = ZB_ZCL_START_PACKET_REQ(buffer)                            \
  ZB_ZCL_CONSTRUCT_SPECIFIC_COMMAND_REQ_FRAME_CONTROL(ptr, (def_resp))     \
  ZB_ZCL_CONSTRUCT_COMMAND_HEADER_REQ(ptr, ZB_ZCL_GET_SEQ_NUM(),                \
      ZB_ZCL_CMD_IAS_ZONE_ZONE_ENROLL_RESPONSE_ID);                         \
  ZB_ZCL_PACKET_PUT_DATA8(ptr, (code));                                     \
  ZB_ZCL_PACKET_PUT_DATA8(ptr, (zone_id));                                  \
  ZB_ZCL_FINISH_PACKET((buffer), ptr)                                      \
  ZB_ZCL_SEND_COMMAND_SHORT(                                                \
      buffer, addr, dst_addr_mode, dst_ep, ep, prfl_id,                     \
      ZB_ZCL_CLUSTER_ID_IAS_ZONE, cb);                                      \
}

/*! @brief Send "Zone Enroll response with add parameters - sequency" command, see ZCL spec 8.2.2.3.1
    @param buffer to put packet to
    @param addr - address to send packet to
    @param dst_addr_mode - addressing mode
    @param dst_ep - destination endpoint
    @param ep - sending endpoint
    @param prfl_id - profile identifier
    @param def_resp - enable/disable default response
    @param cb - callback
    @param code - Enroll response code, see @ref zb_zcl_ias_zone_enroll_responce_code_e
    @param zone_id - Zone ID
    @param seq - known sequency
*/
#define ZB_ZCL_IAS_ZONE_SEND_ZONE_ENROLL_RES_EXT(                           \
    buffer, addr, dst_addr_mode, dst_ep, ep, prfl_id, def_resp, cb,         \
    code, zone_id, seq)                                                     \
{                                                                           \
  zb_uint8_t* ptr = ZB_ZCL_START_PACKET_REQ(buffer)                            \
  ZB_ZCL_CONSTRUCT_SPECIFIC_COMMAND_REQ_FRAME_CONTROL(ptr, (def_resp))     \
  ZB_ZCL_CONSTRUCT_COMMAND_HEADER_REQ(ptr, (seq),                               \
      ZB_ZCL_CMD_IAS_ZONE_ZONE_ENROLL_RESPONSE_ID);                         \
  ZB_ZCL_PACKET_PUT_DATA8(ptr, (code));                                     \
  ZB_ZCL_PACKET_PUT_DATA8(ptr, (zone_id));                                  \
  ZB_ZCL_FINISH_PACKET((buffer), ptr)                                      \
  ZB_ZCL_SEND_COMMAND_SHORT(                                                \
      buffer, addr, dst_addr_mode, dst_ep, ep, prfl_id,                     \
      ZB_ZCL_CLUSTER_ID_IAS_ZONE, cb);                                      \
}

/** @brief Macro for getting "Zone Enroll response" command
  * @attention Assumes that ZCL header already cut.
  * @param data_ptr - pointer to a variable of type @ref
  * zb_zcl_ias_zone_enroll_res_s.
  * @param buffer containing the packet (by pointer).
  * @param status - variable to put parse status to (see @ref zb_zcl_parse_status_t).
  */
#define ZB_ZCL_IAS_ZONE_GET_ZONE_ENROLL_RES(data_ptr, buffer, status)     \
{                                                                         \
  if (zb_buf_len((buffer)) < sizeof(zb_zcl_ias_zone_enroll_res_t))        \
  {                                                                       \
   (status) = ZB_ZCL_PARSE_STATUS_FAILURE;                                \
  }                                                                       \
  else                                                                    \
  {                                                                       \
    zb_zcl_ias_zone_enroll_res_t *src_ptr =                               \
         (zb_zcl_ias_zone_enroll_res_t*)zb_buf_begin((buffer));           \
    (status) = ZB_ZCL_PARSE_STATUS_SUCCESS;                               \
    (data_ptr)->code = src_ptr->code;                                     \
    (data_ptr)->zone_id = src_ptr->zone_id;                               \
  }                                                                       \
}

/*! @} */ /* IAS Zone cluster commands */

/******************************* Set/Clear Zone Status bits ******************************/

/* Set or clear Zone Status bits
 */
void zb_zcl_ias_zone_change_status(zb_uint8_t param);

/** @brief Declare Set/Clear Zone Status bits
*/
typedef struct zb_zcl_ias_zone_status_param_s
{
  zb_uint16_t bits;           /*!< Bits map, see @ref zb_zcl_ias_zone_zonestatus_e */
  zb_bool_t is_set;           /*!< true - set, false - clean */
  zb_uint8_t endpoint;        /*!< endpoint */

} zb_zcl_ias_zone_status_param_t;

/*! @brief Set Zone Status bits custom server command
    @param buffer - to put packet to
    @param ep - endpoint
    @param set_bits - flag set of Zone Status, see @ref zb_zcl_ias_zone_zonestatus_e
*/
#define ZB_ZCL_IAS_ZONE_SET_BITS(buffer, ep, set_bits)                            \
{                                                                                 \
  zb_zcl_ias_zone_status_param_t* cmd_data =                                      \
          ZB_BUF_GET_PARAM((buffer), zb_zcl_ias_zone_status_param_t);             \
  cmd_data->endpoint = (ep);                                                      \
  cmd_data->bits = (set_bits);                                                    \
  cmd_data->is_set = ZB_TRUE;                                                     \
  ZB_SCHEDULE_CALLBACK(zb_zcl_ias_zone_change_status, ((buffer))); \
}

/*! @brief Clear Zone Status bits custom server command
    @param buffer - to put packet to
    @param ep - sending endpoint
    @param set_bits - flag set of Zone Status, see @ref zb_zcl_ias_zone_zonestatus_e
*/
#define ZB_ZCL_IAS_ZONE_CLEAR_BITS(buffer, ep, set_bits)                          \
{                                                                                 \
  zb_zcl_ias_zone_status_param_t* cmd_data =                                      \
          ZB_BUF_GET_PARAM((buffer), zb_zcl_ias_zone_status_param_t);             \
  cmd_data->endpoint = (ep);                                                      \
  cmd_data->bits = (set_bits);                                                    \
  cmd_data->is_set = ZB_FALSE;                                                    \
  ZB_SCHEDULE_CALLBACK(zb_zcl_ias_zone_change_status, ((buffer))); \
}

/**
   @brief Structure is used to pass parameters for ZoneStatus
   notification command
 */
typedef struct zb_zcl_ias_zone_notification_param_s
{
  zb_uint8_t  ep;             /*!< endpoint number */
  zb_uint16_t status_val;     /*!< new value of zone_zone_status, see @ref zb_zcl_ias_zone_zonestatus_e */
  zb_uint16_t delay;          /*!< Delay */
}
zb_zcl_ias_zone_notification_param_t;


/**
   @brief Use this function to change IAS Zone status. This function
   will set new value of ZoneStatus and will send Zone status change
   notification.
   @note If another API (ZB_ZCL_SET_ATTRIBUTE()) is used for modifying
   ZoneStatus, notification will NOT be send. This is limitation of
   the current implementation.
   @param ep - local endpoint number
   @param new_val - new value for ZoneStatus
   @param delay - delay in quarter seconds (actual only for HA ZCL
   version, is ignored for pure ZCL implementation)
   @param buf_param - reference to a buffer that will be used for
   sending notification. If buffer reference is not specified,
   notification will NOT be sent
   @return Returns ZB_TRUE if notification is scheduled for send,
   ZB_FALSE otherwise
   @note Notification send result is reported to user App using
   callback that is registered with zb_zcl_ias_zone_register_cb()
*/
zb_bool_t zb_zcl_ias_zone_set_status(
  zb_uint8_t ep,
  zb_uint16_t new_val,
  zb_uint16_t delay,
  zb_uint8_t buf_param);

enum zb_zcl_general_cb_params_e
{
  ZB_ZCL_VALID_CIE_ADDR_SET = 1,
  ZB_ZCL_CURR_ZONE_SENSITIVITY_LEVEL_SET = 2,
  ZB_ZCL_INIT_TEST_MODE = 3,
  ZB_ZCL_INIT_NORMAL_MODE = 4,
};

typedef zb_ret_t (ZB_CODE * zb_ias_zone_app_callback_t)(zb_uint8_t param, zb_uint16_t general_val);

typedef struct zb_zcl_ias_zone_int_ctx_s
{
  zb_callback_t process_result_cb;
  zb_ias_zone_app_callback_t general_cb;
  zb_uint8_t restore_current_zone_sens_level;
  zb_uint8_t new_current_zone_sens_level;
}
zb_zcl_ias_zone_int_ctx_t;

/** @brief Declare run Enroll Response command for User Application
*/
typedef struct zb_zcl_ias_zone_enroll_response_value_param_s
{
  zb_uint8_t enroll_response;  /*!< Enroll response code */
  zb_uint8_t zone_id;          /*!< Zone ID */
} zb_zcl_ias_zone_enroll_response_value_param_t;

/*! Struct for invoke User App & continue after */
typedef struct zb_zcl_ias_zone_enroll_response_user_app_schedule_e
{
  /**< Parameters for continue command, see @ref zb_zcl_parsed_hdr_s */
  zb_zcl_parsed_hdr_t cmd_info;
  /**< User App command parameters, see @ref zb_zcl_ias_zone_enroll_response_value_param_s */
  zb_zcl_ias_zone_enroll_response_value_param_t param;
} zb_zcl_ias_zone_enroll_response_user_app_schedule_t;

#define ZB_ZCL_IAS_ZONE_ENROLL_RESPONSE_SCHEDULE_USER_APP(buffer, pcmd_info, enroll_response_, zone_id_) \
{                                                                                               \
  zb_zcl_ias_zone_enroll_response_user_app_schedule_t* user_data =                              \
          ZB_BUF_GET_PARAM((buffer), zb_zcl_ias_zone_enroll_response_user_app_schedule_t);      \
  ZB_MEMMOVE(&(user_data->cmd_info), (pcmd_info), sizeof(zb_zcl_parsed_hdr_t));                 \
  user_data->param.enroll_response = (enroll_response_);                                        \
  user_data->param.zone_id = (zone_id_);                                                        \
  ZB_SCHEDULE_CALLBACK(zb_zcl_ias_zone_enroll_response_invoke_user_app, ((buffer))); \
}

/** @brief Registry callbacks to get status of Zone Status Change Notification command
    @param endpoint - local endpoint number
    @param process_result_cb - notification sent callback function
    @param general_cb - general callback function
*/
void zb_zcl_ias_zone_register_cb(
    zb_uint8_t endpoint,
    zb_callback_t process_result_cb,
    zb_ias_zone_app_callback_t general_cb);

/**
   @brief checks, if Zone Status change notification should be sent
   and schedules command if it is needed
   @return ZB_TRUE if command is sent
*/
zb_bool_t zb_zcl_ias_zone_check_attr_notify(
  zb_uint8_t buf_param);

/**
   @brief fills in notification command and sends it
*/
void zb_zcl_ias_zone_send_status_change_not(zb_uint8_t param);

/**
   @brief checks cie address on zcl initialization
*/
void zb_zcl_ias_zone_check_cie_addr_on_zcl_initialization(zb_uint8_t ep_id);

/**
   @brief puts cie command to binding whitelist table
*/
zb_ret_t zb_zcl_ias_zone_put_cie_address_to_binding_whitelist(zb_uint8_t endpoint);

/**
   @brief IAS Zone specific set attribute value postprocessing
*/
void zb_zcl_ias_set_attr_val_post_process(zb_zcl_parsed_hdr_t *cmd_info, zb_uint16_t attr_id, zb_uint8_t *value);

/*! @} */ /* ZCL IAS Zone cluster definitions */

/** @endcond */ /* DOXYGEN_ZCL_SECTION */

void zb_zcl_ias_zone_init_server(void);
void zb_zcl_ias_zone_init_client(void);
#define ZB_ZCL_CLUSTER_ID_IAS_ZONE_SERVER_ROLE_INIT zb_zcl_ias_zone_init_server
#define ZB_ZCL_CLUSTER_ID_IAS_ZONE_CLIENT_ROLE_INIT zb_zcl_ias_zone_init_client

zb_bool_t zb_zcl_process_ias_zone_specific_commands(zb_uint8_t param);

#endif /* ZB_ZCL_IAS_ZONE_H */
