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
/* PURPOSE: IAS Ace cluster definitions
*/

#ifndef ZB_ZCL_IAS_ACE_H
#define ZB_ZCL_IAS_ACE_H 1

#include "zcl/zb_zcl_common.h"
#include "zcl/zb_zcl_commands.h"

/** @cond DOXYGEN_ZCL_SECTION */

/** @addtogroup ZB_ZCL_IAS_ACE
 *  @{
 */


/* Cluster ZB_ZCL_CLUSTER_ID_IAS_ACE */

/*! @name IAS Ace cluster attributes
    @{
*/

/*! @brief Structure representsation of IAS ACE Zone Table
 * see ZCL spec 8.3.2.3, table 8.11 */
typedef ZB_PACKED_PRE struct zb_zcl_ias_ace_zone_table_s
{
  /** Zone ID */
  zb_uint8_t zone_id;
  /** Zone Type, see @ref zb_zcl_ias_zone_zonetype_e */
  zb_uint16_t zone_type;
  /** Zone Address */
  zb_ieee_addr_t zone_address;
  /** The pointer to a string with Zone Label. If the pointer is zero, the Zone Label
   *  is not programmed */
  zb_char_t *zone_label;
} ZB_PACKED_STRUCT zb_zcl_ias_ace_zone_table_t;

/** @brief Default value for IAS ACE cluster revision global attribute */
#define ZB_ZCL_IAS_ACE_CLUSTER_REVISION_DEFAULT ((zb_uint16_t)0x0001u)

/** @brief IAS ACE Zone Table maximum length */
#define ZB_ZCL_IAS_ACE_ZONE_TABLE_LENGTH      255

/** @brief IAS ACE ZoneType attribute maximum value */
#define ZB_ZCL_IAS_ACE_ZONE_TYPE_MAX_VALUE    0xfffe

/** @brief IAS ACE ZoneID attribute maximum value */
#define ZB_ZCL_IAS_ACE_ZONE_ID_MAX_VALUE      0xfe

/** @brief IAS ACE ZoneID attribute default value */
#define ZB_ZCL_IAS_ACE_ZONE_ID_DEFAULT_VALUE  0

/** @brief IAS ACE Set Zone Address default value */
#define ZB_ZCL_IAS_ACE_SET_ZONE_ADDRESS_DEFAULT_VALUE(ptr)  (ZB_IEEE_ADDR_COPY(ptr, &g_unknown_ieee_addr))

/*! @brief IAS Ace cluster custom attribute identifiers
*/
enum zb_zcl_ias_ace_attr_e
{
  /*! @brief Ace table length, ZCL spec 8.3.2.3 */
  ZB_ZCL_ATTR_IAS_ACE_ZONE_TABLE_LENGTH_ID = 0xeffe,
  /*! @brief Ace table ZCL spec 8.3.2.3 */
  ZB_ZCL_ATTR_IAS_ACE_ZONE_TABLE_ID = 0xefff,

};

/*! @cond internals_doc
    @name IAS Ace cluster internals
    Internal structures for attribute representation in cluster definitions.
    @{
*/

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_IAS_ACE_ZONE_TABLE_LENGTH_ID(data_ptr) \
{                                                       \
  ZB_ZCL_ATTR_IAS_ACE_ZONE_TABLE_LENGTH_ID,             \
  ZB_ZCL_ATTR_TYPE_U8,                                  \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                         \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                   \
  (void*) data_ptr                                      \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_IAS_ACE_ZONE_TABLE_ID(data_ptr) \
{                                                       \
  ZB_ZCL_ATTR_IAS_ACE_ZONE_TABLE_ID,                    \
  ZB_ZCL_ATTR_TYPE_NULL,                                \
  ZB_ZCL_ATTR_ACCESS_INTERNAL,                          \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                   \
  (void*) data_ptr                                      \
}

/*! @internal Number of attributes mandatory for reporting in IAS Ace cluster */
#define ZB_ZCL_IAS_ACE_REPORT_ATTR_COUNT 0

/*! @}
 *  @endcond*/ /* IAS Ace cluster internals */

/** @brief Declare attribute list for IAS Ace cluster - server side
    @param attr_list - attribute list name
    @param length - (zb_uint16_t*) pointer to variable to store Length of IAS ACE Zone Table attribute,
        see ZCL spec 8.3.2.3, table 8.11
    @param table - pointer to variable to store IAS ACE Zone Table attribute,
    see ZCL spec 8.3.2.3, table 8.11
*/
#define ZB_ZCL_DECLARE_IAS_ACE_ATTRIB_LIST(attr_list, length, table)        \
  ZB_ZCL_START_DECLARE_ATTRIB_LIST_CLUSTER_REVISION(attr_list, ZB_ZCL_IAS_ACE) \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_IAS_ACE_ZONE_TABLE_LENGTH_ID, (length))  \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_IAS_ACE_ZONE_TABLE_ID, (table))          \
  ZB_ZCL_FINISH_DECLARE_ATTRIB_LIST

/*! @} */ /* IAS Ace cluster attributes */

/*! @name IAS Ace cluster commands
    @{
*/

/*! @brief IAS Ace cluster command identifiers
    @see ZCL spec, IAS Ace Cluster, 8.3.2.4
*/
enum zb_zcl_ias_ace_cmd_e
{
  /** Arm command, ZCL spec 8.3.2.4.1 */
  ZB_ZCL_CMD_IAS_ACE_ARM_ID                       = 0x00,
  /** Bypass command, ZCL spec 8.3.2.4.2 */
  ZB_ZCL_CMD_IAS_ACE_BYPASS_ID                    = 0x01,
  /** Emergency command, ZCL spec 8.3.2.4.3 */
  ZB_ZCL_CMD_IAS_ACE_EMERGENCY_ID                 = 0x02,
  /** Fire command, ZCL spec 8.3.2.4.3 */
  ZB_ZCL_CMD_IAS_ACE_FIRE_ID                      = 0x03,
  /** Panic command, ZCL spec 8.3.2.4.3 */
  ZB_ZCL_CMD_IAS_ACE_PANIC_ID                     = 0x04,
  /** Get Zone ID Map command, ZCL spec 8.3.2.4.4 */
  ZB_ZCL_CMD_IAS_ACE_GET_ZONE_ID_MAP_ID           = 0x05,
  /** Get Zone Information command, ZCL spec 8.3.2.4.5 */
  ZB_ZCL_CMD_IAS_ACE_GET_ZONE_INFO_ID             = 0x06,
  /** This command is used by ACE clients to request an update to the status
   *  (e.g. security system arm state) of the ACE server (i.e. the IAS CIE) */
  ZB_ZCL_CMD_IAS_ACE_GET_PANEL_STATUS_ID          = 0x07,
  /** Provides IAS ACE clients with a way to retrieve the list of zones to be bypassed. */
  ZB_ZCL_CMD_IAS_ACE_GET_BYPASSED_ZONE_LIST_ID    = 0x08,
  /** This command is used by ACE clients to request an update of the status
   *  of the IAS Zone devices managed by the ACE server (i.e. the IAS CIE). */
  ZB_ZCL_CMD_IAS_ACE_GET_ZONE_STATUS_ID           = 0x09,
};

/*! @brief IAS Ace cluster response command identifiers
    @see ZCL spec, IAS Ace Cluster, 8.3.2.5
*/
enum zb_zcl_ias_ace_resp_cmd_e
{
  /** Arm Response command, ZCL spec 8.3.2.5.1 */
  ZB_ZCL_CMD_IAS_ACE_ARM_RESP_ID                  = 0x00,
  /** Get Zone ID Map Response command, ZCL spec 8.3.2.5.2 */
  ZB_ZCL_CMD_IAS_ACE_GET_ZONE_ID_MAP_RESP_ID      = 0x01,
  /** Get Zone Information Response command, ZCL spec 8.3.2.5.3 */
  ZB_ZCL_CMD_IAS_ACE_GET_ZONE_INFO_RESP_ID        = 0x02,
  /** This command updates ACE clients in the system of changes to zone status
   *  recorded by the ACE server (e.g., IAS CIE device). */
  ZB_ZCL_CMD_IAS_ACE_ZONE_STATUS_CHANGED_ID       = 0x03,
  /** This command updates ACE clients in the system of changes to zone status
   *  recorded by the ACE server (e.g., IAS CIE device). */
  ZB_ZCL_CMD_IAS_ACE_PANEL_STATUS_CHANGED_ID      = 0x04,
  /** This command updates ACE clients in the system of changes to panel status
   *   recorded by the ACE server (e.g., IAS CIE device). */
  ZB_ZCL_CMD_IAS_ACE_GET_PANEL_STATUS_RESPONSE_ID = 0x05,
  /** Sets the list of bypassed zones on the IAS ACE client. */
  ZB_ZCL_CMD_IAS_ACE_SET_BYPASSED_ZONE_LIST_ID    = 0x06,
  /** Provides the response of the security panel to the request from the IAS ACE client
   *  to bypass zones via a Bypass command. */
  ZB_ZCL_CMD_IAS_ACE_BYPASS_RESPONSE_ID           = 0x07,
  /** Get Zone Status Response Command. */
  ZB_ZCL_CMD_IAS_ACE_GET_ZONE_STATUS_RESPONSE_ID  = 0x08,
};

/** @cond internals_doc */
/* IAS ACE cluster commands list : only for information - do not modify */
#define ZB_ZCL_CLUSTER_ID_IAS_ACE_SERVER_ROLE_GENERATED_CMD_LIST                       \
                                      ZB_ZCL_CMD_IAS_ACE_ARM_RESP_ID,                  \
                                      ZB_ZCL_CMD_IAS_ACE_GET_ZONE_ID_MAP_RESP_ID,      \
                                      ZB_ZCL_CMD_IAS_ACE_GET_ZONE_INFO_RESP_ID,        \
                                      ZB_ZCL_CMD_IAS_ACE_ZONE_STATUS_CHANGED_ID,       \
                                      ZB_ZCL_CMD_IAS_ACE_PANEL_STATUS_CHANGED_ID,      \
                                      ZB_ZCL_CMD_IAS_ACE_GET_PANEL_STATUS_RESPONSE_ID, \
                                      ZB_ZCL_CMD_IAS_ACE_SET_BYPASSED_ZONE_LIST_ID,    \
                                      ZB_ZCL_CMD_IAS_ACE_BYPASS_RESPONSE_ID,           \
                                      ZB_ZCL_CMD_IAS_ACE_GET_ZONE_STATUS_RESPONSE_ID

#define ZB_ZCL_CLUSTER_ID_IAS_ACE_CLIENT_ROLE_RECEIVED_CMD_LIST ZB_ZCL_CLUSTER_ID_IAS_ACE_SERVER_ROLE_GENERATED_CMD_LIST

#define ZB_ZCL_CLUSTER_ID_IAS_ACE_CLIENT_ROLE_GENERATED_CMD_LIST                    \
                                      ZB_ZCL_CMD_IAS_ACE_ARM_ID,                    \
                                      ZB_ZCL_CMD_IAS_ACE_BYPASS_ID,                 \
                                      ZB_ZCL_CMD_IAS_ACE_EMERGENCY_ID,              \
                                      ZB_ZCL_CMD_IAS_ACE_FIRE_ID,                   \
                                      ZB_ZCL_CMD_IAS_ACE_PANIC_ID,                  \
                                      ZB_ZCL_CMD_IAS_ACE_GET_ZONE_ID_MAP_ID,        \
                                      ZB_ZCL_CMD_IAS_ACE_GET_ZONE_INFO_ID,          \
                                      ZB_ZCL_CMD_IAS_ACE_GET_PANEL_STATUS_ID,       \
                                      ZB_ZCL_CMD_IAS_ACE_GET_BYPASSED_ZONE_LIST_ID, \
                                      ZB_ZCL_CMD_IAS_ACE_GET_ZONE_STATUS_ID

#define ZB_ZCL_CLUSTER_ID_IAS_ACE_SERVER_ROLE_RECEIVED_CMD_LIST ZB_ZCL_CLUSTER_ID_IAS_ACE_CLIENT_ROLE_GENERATED_CMD_LIST
/*! @}
 *  @endcond */ /* internals_doc */


/******************************* Arm Command ******************************/

/** @brief Values of the Arm Mode
 * see ZCL spec 8.3.2.4.1.2
*/
enum zb_zcl_ias_ace_arm_mode_e
{
  /** Disarm */
  ZB_ZCL_IAS_ACE_ARM_MODE_DISARM  = 0x00,
  /** Arm Day/Home Zones Only */
  ZB_ZCL_IAS_ACE_ARM_MODE_DAY     = 0x01,
  /** Arm Night/Sleep Zones Only */
  ZB_ZCL_IAS_ACE_ARM_MODE_NIGHT   = 0x02,
  /** Arm All Zones */
  ZB_ZCL_IAS_ACE_ARM_MODE_ALL     = 0x03,
};

/*! @brief Structure representsation of Arm command, ZCL spec 8.3.2.4.1.1 */
typedef ZB_PACKED_PRE struct zb_zcl_ias_ace_arm_s
{
  /** Arm Mode, see @ref zb_zcl_ias_ace_arm_mode_e*/
  zb_uint8_t arm_mode;
  /** Arm/Disarm Code */
  zb_uchar_t  arm_disarm_code[ZB_ZCL_MAX_STRING_SIZE];
  /** Zone ID */
  zb_uint8_t zone_id;

} ZB_PACKED_STRUCT zb_zcl_ias_ace_arm_t;

/*! Arm request command payload size */
#define ZB_ZCL_IAS_ACE_ARM_REQ_PAYLOAD_SIZE(ptr)    (sizeof(zb_zcl_ias_ace_arm_t)-sizeof(((zb_zcl_ias_ace_arm_t *)(ptr))->arm_disarm_code)+ \
                                                     1+ZB_ZCL_GET_STRING_LENGTH(((zb_zcl_ias_ace_arm_t *)(ptr))->arm_disarm_code))

/*! @brief Arm command, ZCL spec 8.3.2.4.1
    @param buffer - to put packet to
    @param addr - address to send packet to
    @param dst_addr_mode - addressing mode
    @param dst_ep - destination endpoint
    @param ep - sending endpoint
    @param prfl_id - profile identifier
    @param def_resp - enable/disable default response
    @param cb - callback for getting command send status
    @param arm_mode - Arm Mode, see @ref zb_zcl_ias_ace_arm_mode_e
    @param arm_disarm_code - Arm/Disarm Code string
    @param zone_id - Zone ID
*/
#define ZB_ZCL_IAS_ACE_SEND_ARM_REQ(                              \
  buffer, addr, dst_addr_mode, dst_ep, ep, prfl_id, def_resp, cb, \
  arm_mode, arm_disarm_code, zone_id)                             \
{                                                                 \
  zb_uint8_t* ptr = ZB_ZCL_START_PACKET_REQ(buffer)               \
  ZB_ZCL_CONSTRUCT_SPECIFIC_COMMAND_REQ_FRAME_CONTROL(ptr, (def_resp)) \
  ZB_ZCL_CONSTRUCT_COMMAND_HEADER_REQ(ptr, ZB_ZCL_GET_SEQ_NUM(),  \
      ZB_ZCL_CMD_IAS_ACE_ARM_ID);                                 \
  ZB_ZCL_PACKET_PUT_DATA8(ptr, (arm_mode));                       \
  ZB_ZCL_PACKET_PUT_STRING(ptr, (arm_disarm_code));               \
  ZB_ZCL_PACKET_PUT_DATA8(ptr, (zone_id));                        \
  ZB_ZCL_FINISH_PACKET((buffer), ptr)                             \
  ZB_ZCL_SEND_COMMAND_SHORT(                                      \
      buffer, addr, dst_addr_mode, dst_ep, ep, prfl_id,           \
      ZB_ZCL_CLUSTER_ID_IAS_ACE, cb);                             \
}

/** @brief Macro for getting Arm command, see ZCL spec 8.3.2.4.1
  * @attention Assumes that ZCL header already cut.
  * @param data_ptr - pointer to a variable of type @ref
  * zb_zcl_ias_ace_arm_s.
  * @param buffer containing the packet (by pointer).
  * @param status - variable to put parse status to (see @ref zb_zcl_parse_status_t).
  */
#define ZB_ZCL_IAS_ACE_GET_ARM_REQ(data_ptr, buffer, status)  \
{                                                             \
  zb_uint8_t *data = zb_buf_begin(buffer);                    \
  if (zb_buf_len((buffer)) <                                  \
      ZB_ZCL_IAS_ACE_ARM_REQ_PAYLOAD_SIZE(data))              \
  {                                                           \
    (status) = ZB_ZCL_PARSE_STATUS_FAILURE;                   \
  }                                                           \
  else                                                        \
  {                                                           \
    ZB_ZCL_PACKET_GET_DATA8(&(data_ptr)->arm_mode, data);     \
    ZB_ZCL_PACKET_GET_STATIC_STRING((data_ptr)->arm_disarm_code, data);  \
    if (!data)                                                \
    {                                                         \
      (status) = ZB_ZCL_PARSE_STATUS_FAILURE;                 \
    }                                                         \
    else                                                      \
    {                                                         \
      ZB_ZCL_PACKET_GET_DATA8(&(data_ptr)->zone_id, data);    \
      (status) = ZB_ZCL_PARSE_STATUS_SUCCESS;                 \
    }                                                         \
  }                                                           \
}

/******************************* Bypass Command ******************************/

/*! @brief Structure representsation of Bypass command, ZCL spec 8.3.2.4.2.1 */
typedef ZB_PACKED_PRE struct zb_zcl_ias_ace_bypass_s
{
  /** Number of Zones */
  zb_uint8_t length;
  /** Zone ID array, see @ref zb_zcl_ias_ace_zone_table_s */
  zb_uint8_t zone_id[ZB_ZCL_IAS_ACE_ZONE_TABLE_LENGTH];
  /** Arm/Disarm Code */
  zb_uchar_t  arm_disarm_code[ZB_ZCL_MAX_STRING_SIZE];

} ZB_PACKED_STRUCT zb_zcl_ias_ace_bypass_t;

/*! Bypass command payload size */
#define ZB_ZCL_IAS_ACE_BYPASS_PAYLOAD_SIZE(ptr)   (zb_uint_t)(1+(ptr)[0]+1+ZB_ZCL_GET_STRING_LENGTH((ptr)+1+(ptr)[0]))

/*! @brief Start Bypass command, ZCL spec 8.3.2.4.1
    @param buffer - to put packet to
    @param def_resp - enable/disable default response
    @param length - Number of Zones
    @param ptr - [out] (zb_uint8_t*) current position for @ref ZB_ZCL_IAS_ACE_SEND_BYPASS_REQ_ADD
    and @ref ZB_ZCL_IAS_ACE_SEND_BYPASS_REQ_END
*/
#define ZB_ZCL_IAS_ACE_SEND_BYPASS_REQ_START(buffer, def_resp, length, ptr) \
{                                                                           \
  (ptr) = ZB_ZCL_START_PACKET_REQ(buffer)                                   \
  ZB_ZCL_CONSTRUCT_SPECIFIC_COMMAND_REQ_FRAME_CONTROL(ptr, (def_resp))      \
  ZB_ZCL_CONSTRUCT_COMMAND_HEADER_REQ(ptr, ZB_ZCL_GET_SEQ_NUM(),            \
      ZB_ZCL_CMD_IAS_ACE_BYPASS_ID);                                        \
  ZB_ZCL_PACKET_PUT_DATA8((ptr), (length));                                 \
}

/*! @brief Add Zone Id to Bypass command, ZCL spec 8.3.2.4.1
    @param ptr - [in/out] (zb_uint8_t*) current position
    @param zone_id - Zone ID, see @ref zb_zcl_ias_ace_zone_table_s
*/
#define ZB_ZCL_IAS_ACE_SEND_BYPASS_REQ_ADD(ptr, zone_id)          \
{                                                                 \
  ZB_ZCL_PACKET_PUT_DATA8(ptr, (zone_id));                        \
}

/*! @brief End form Bypass command and send it, ZCL spec 8.3.2.4.1
    @param ptr - (zb_uint8_t*) current position
    @param buffer - to put packet to
    @param arm_disarm_code - Arm/Disarm Code string
    @param addr - address to send packet to
    @param dst_addr_mode - addressing mode
    @param dst_ep - destination endpoint
    @param ep - sending endpoint
    @param prfl_id - profile identifier
    @param cb - callback for getting command send status
*/
#define ZB_ZCL_IAS_ACE_SEND_BYPASS_REQ_END(                       \
  ptr, buffer, arm_disarm_code, addr, dst_addr_mode, dst_ep, ep, prfl_id, cb) \
{                                                                 \
  ZB_ZCL_PACKET_PUT_STRING(ptr, (arm_disarm_code));               \
  ZB_ZCL_FINISH_PACKET((buffer), ptr)                             \
  ZB_ZCL_SEND_COMMAND_SHORT(                                      \
      buffer, addr, dst_addr_mode, dst_ep, ep, prfl_id,           \
      ZB_ZCL_CLUSTER_ID_IAS_ACE, cb);                             \
}

/** @brief Macro for getting Bypass command, see ZCL spec 8.3.2.4.1
  * @attention Assumes that ZCL header already cut.
  * @param data_ptr - pointer to a variable of type @ref
  * zb_zcl_ias_ace_bypass_s.
  * @param buffer containing the packet (by pointer).
  * @param status - variable to put parse status to (see @ref zb_zcl_parse_status_t).
  */
#define ZB_ZCL_IAS_ACE_GET_BYPASS_REQ(data_ptr, buffer, status)       \
{                                                                     \
  zb_uint8_t *data = zb_buf_begin(buffer);                            \
  if (zb_buf_len((buffer)) < ZB_ZCL_IAS_ACE_BYPASS_PAYLOAD_SIZE(data))       \
  {                                                                   \
    (status) = ZB_ZCL_PARSE_STATUS_FAILURE;                           \
  }                                                                   \
  else                                                                \
  {                                                                   \
    ZB_ZCL_PACKET_GET_DATA8(&(data_ptr)->length, data);               \
    ZB_ZCL_PACKET_GET_DATA_N((data_ptr)->zone_id, data, (data_ptr)->length); \
    ZB_ZCL_PACKET_GET_STATIC_STRING((data_ptr)->arm_disarm_code, data);      \
    if (!data)                                                        \
    {                                                                 \
      (status) = ZB_ZCL_PARSE_STATUS_FAILURE;                         \
    }                                                                 \
    else                                                              \
    {                                                                 \
      (status) = ZB_ZCL_PARSE_STATUS_SUCCESS;                         \
    }                                                                 \
  }                                                                   \
}

/******************************* Emergency Command ******************************/

/*! @brief Emergency command, ZCL spec 8.3.2.4.3
    @param buffer - to put packet to
    @param addr - address to send packet to
    @param dst_addr_mode - addressing mode
    @param dst_ep - destination endpoint
    @param ep - sending endpoint
    @param prfl_id - profile identifier
    @param def_resp - enable/disable default response
    @param cb - callback for getting command send status
*/
#define ZB_ZCL_IAS_ACE_SEND_EMERGENCY_REQ(                             \
  buffer, addr, dst_addr_mode, dst_ep, ep, prfl_id, def_resp, cb)      \
{                                                                      \
  zb_uint8_t* ptr = ZB_ZCL_START_PACKET_REQ(buffer)                    \
  ZB_ZCL_CONSTRUCT_SPECIFIC_COMMAND_REQ_FRAME_CONTROL(ptr, (def_resp)) \
  ZB_ZCL_CONSTRUCT_COMMAND_HEADER_REQ(ptr, ZB_ZCL_GET_SEQ_NUM(),       \
      ZB_ZCL_CMD_IAS_ACE_EMERGENCY_ID);                                \
  ZB_ZCL_FINISH_PACKET((buffer), ptr)                                  \
  ZB_ZCL_SEND_COMMAND_SHORT(                                           \
      buffer, addr, dst_addr_mode, dst_ep, ep, prfl_id,                \
      ZB_ZCL_CLUSTER_ID_IAS_ACE, cb);                                  \
}

/******************************* Fire Command ******************************/

/*! @brief Fire command, ZCL spec 8.3.2.4.3
    @param buffer - to put packet to
    @param addr - address to send packet to
    @param dst_addr_mode - addressing mode
    @param dst_ep - destination endpoint
    @param ep - sending endpoint
    @param prfl_id - profile identifier
    @param def_resp - enable/disable default response
    @param cb - callback for getting command send status
*/
#define ZB_ZCL_IAS_ACE_SEND_FIRE_REQ(                                  \
  buffer, addr, dst_addr_mode, dst_ep, ep, prfl_id, def_resp, cb)      \
{                                                                      \
  zb_uint8_t* ptr = ZB_ZCL_START_PACKET_REQ(buffer)                    \
  ZB_ZCL_CONSTRUCT_SPECIFIC_COMMAND_REQ_FRAME_CONTROL(ptr, (def_resp)) \
  ZB_ZCL_CONSTRUCT_COMMAND_HEADER_REQ(ptr, ZB_ZCL_GET_SEQ_NUM(),       \
      ZB_ZCL_CMD_IAS_ACE_FIRE_ID);                                     \
  ZB_ZCL_FINISH_PACKET((buffer), ptr)                                  \
  ZB_ZCL_SEND_COMMAND_SHORT(                                           \
      buffer, addr, dst_addr_mode, dst_ep, ep, prfl_id,                \
      ZB_ZCL_CLUSTER_ID_IAS_ACE, cb);                                  \
}

/******************************* Panic Command ******************************/

/*! @brief Panic command, ZCL spec 8.3.2.4.3
    @param buffer - to put packet to
    @param addr - address to send packet to
    @param dst_addr_mode - addressing mode
    @param dst_ep - destination endpoint
    @param ep - sending endpoint
    @param prfl_id - profile identifier
    @param def_resp - enable/disable default response
    @param cb - callback for getting command send status
*/
#define ZB_ZCL_IAS_ACE_SEND_PANIC_REQ(                                 \
  buffer, addr, dst_addr_mode, dst_ep, ep, prfl_id, def_resp, cb)      \
{                                                                      \
  zb_uint8_t* ptr = ZB_ZCL_START_PACKET_REQ(buffer)                    \
  ZB_ZCL_CONSTRUCT_SPECIFIC_COMMAND_REQ_FRAME_CONTROL(ptr, (def_resp)) \
  ZB_ZCL_CONSTRUCT_COMMAND_HEADER_REQ(ptr, ZB_ZCL_GET_SEQ_NUM(),       \
      ZB_ZCL_CMD_IAS_ACE_PANIC_ID);                                    \
  ZB_ZCL_FINISH_PACKET((buffer), ptr)                                  \
  ZB_ZCL_SEND_COMMAND_SHORT(                                           \
      buffer, addr, dst_addr_mode, dst_ep, ep, prfl_id,                \
      ZB_ZCL_CLUSTER_ID_IAS_ACE, cb);                                  \
}

/******************************* Get Zone ID Map command ******************************/

/*! @brief Get Zone ID Map command, ZCL spec 8.3.2.4.4
    @param buffer - to put packet to
    @param addr - address to send packet to
    @param dst_addr_mode - addressing mode
    @param dst_ep - destination endpoint
    @param ep - sending endpoint
    @param prfl_id - profile identifier
    @param def_resp - enable/disable default response
    @param cb - callback for getting command send status
*/
#define ZB_ZCL_IAS_ACE_SEND_GET_ZONE_ID_MAP_REQ(                       \
  buffer, addr, dst_addr_mode, dst_ep, ep, prfl_id, def_resp, cb)      \
{                                                                      \
  zb_uint8_t* ptr = ZB_ZCL_START_PACKET_REQ(buffer)                    \
  ZB_ZCL_CONSTRUCT_SPECIFIC_COMMAND_REQ_FRAME_CONTROL(ptr, (def_resp)) \
  ZB_ZCL_CONSTRUCT_COMMAND_HEADER_REQ(ptr, ZB_ZCL_GET_SEQ_NUM(),       \
      ZB_ZCL_CMD_IAS_ACE_GET_ZONE_ID_MAP_ID);                          \
  ZB_ZCL_FINISH_PACKET((buffer), ptr)                                  \
  ZB_ZCL_SEND_COMMAND_SHORT(                                           \
      buffer, addr, dst_addr_mode, dst_ep, ep, prfl_id,                \
      ZB_ZCL_CLUSTER_ID_IAS_ACE, cb);                                  \
}

/******************************* Get Zone Information command ******************************/

/*! @brief Structure representsation of Get Zone Information command, ZCL spec 8.3.2.4.5 */
typedef ZB_PACKED_PRE struct zb_zcl_ias_ace_get_zone_info_s
{
  /** Zone ID, see @ref zb_zcl_ias_ace_zone_table_s */
  zb_uint8_t zone_id;

} ZB_PACKED_STRUCT zb_zcl_ias_ace_get_zone_info_t;

/*! @brief Get Zone Information command, ZCL spec 8.3.2.4.5
    @param buffer - to put packet to
    @param addr - address to send packet to
    @param dst_addr_mode - addressing mode
    @param dst_ep - destination endpoint
    @param ep - sending endpoint
    @param prfl_id - profile identifier
    @param def_resp - enable/disable default response
    @param cb - callback for getting command send status
    @param zone_id - Zone ID, see @ref zb_zcl_ias_ace_zone_table_s
*/
#define ZB_ZCL_IAS_ACE_SEND_GET_ZONE_INFO_REQ(                         \
  buffer, addr, dst_addr_mode, dst_ep, ep, prfl_id, def_resp, cb,      \
    zone_id)                                                           \
{                                                                      \
  zb_uint8_t* ptr = ZB_ZCL_START_PACKET_REQ(buffer)                    \
  ZB_ZCL_CONSTRUCT_SPECIFIC_COMMAND_REQ_FRAME_CONTROL(ptr, (def_resp)) \
  ZB_ZCL_CONSTRUCT_COMMAND_HEADER_REQ(ptr, ZB_ZCL_GET_SEQ_NUM(),       \
      ZB_ZCL_CMD_IAS_ACE_GET_ZONE_INFO_ID);                            \
  ZB_ZCL_PACKET_PUT_DATA8(ptr, (zone_id));                             \
  ZB_ZCL_FINISH_PACKET((buffer), ptr)                                  \
  ZB_ZCL_SEND_COMMAND_SHORT(                                           \
      buffer, addr, dst_addr_mode, dst_ep, ep, prfl_id,                \
      ZB_ZCL_CLUSTER_ID_IAS_ACE, cb);                                  \
}

/** @brief Macro for getting Get Zone Information command, ZCL spec 8.3.2.4.5
  * @attention Assumes that ZCL header already cut.
  * @param data_ptr - pointer to a variable of type @ref
  * zb_zcl_ias_ace_get_zone_info_s.
  * @param buffer containing the packet (by pointer).
  * @param status - variable to put parse status to (see @ref zb_zcl_parse_status_t).
  */
#define ZB_ZCL_IAS_ACE_GET_GET_ZONE_INFO_REQ(data_ptr, buffer, status)  \
{                                                                       \
  if (zb_buf_len((buffer)) < sizeof(zb_zcl_ias_ace_get_zone_info_t))    \
  {                                                                     \
    (status) = ZB_ZCL_PARSE_STATUS_FAILURE;                             \
  }                                                                     \
  else                                                                  \
  {                                                                     \
    zb_zcl_ias_ace_get_zone_info_t *src_ptr =                           \
         (zb_zcl_ias_ace_get_zone_info_t*)zb_buf_begin((buffer));       \
    (status) = ZB_ZCL_PARSE_STATUS_SUCCESS;                             \
    (data_ptr)->zone_id = src_ptr->zone_id;                             \
  }                                                                     \
}

/******************************* Get Panel Status command ******************************/

/*! @brief Get Panel Status command, ZCL spec 8.3.2.3.6
    @param buffer - to put packet to
    @param addr - address to send packet to
    @param dst_addr_mode - addressing mode
    @param dst_ep - destination endpoint
    @param ep - sending endpoint
    @param prfl_id - profile identifier
    @param def_resp - enable/disable default response
    @param cb - callback for getting command send status
*/
#define ZB_ZCL_IAS_ACE_SEND_GET_PANEL_STATUS_REQ(                      \
  buffer, addr, dst_addr_mode, dst_ep, ep, prfl_id, def_resp, cb)      \
{                                                                      \
  zb_uint8_t* ptr = ZB_ZCL_START_PACKET_REQ(buffer)                    \
  ZB_ZCL_CONSTRUCT_SPECIFIC_COMMAND_REQ_FRAME_CONTROL(ptr, (def_resp)) \
  ZB_ZCL_CONSTRUCT_COMMAND_HEADER_REQ(ptr, ZB_ZCL_GET_SEQ_NUM(),       \
      ZB_ZCL_CMD_IAS_ACE_GET_PANEL_STATUS_ID);                         \
  ZB_ZCL_FINISH_PACKET((buffer), ptr)                                  \
  ZB_ZCL_SEND_COMMAND_SHORT(                                           \
      buffer, addr, dst_addr_mode, dst_ep, ep, prfl_id,                \
      ZB_ZCL_CLUSTER_ID_IAS_ACE, cb);                                  \
}

/******************************* Get Bypassed Zone List command ******************************/

/*! @brief Get Bypassed Zone List command, ZCL spec 8.3.2.3.7
    @param buffer - to put packet to
    @param addr - address to send packet to
    @param dst_addr_mode - addressing mode
    @param dst_ep - destination endpoint
    @param ep - sending endpoint
    @param prfl_id - profile identifier
    @param def_resp - enable/disable default response
    @param cb - callback for getting command send status
*/
#define ZB_ZCL_IAS_ACE_SEND_GET_BYPASSED_ZONE_LIST_REQ(                \
  buffer, addr, dst_addr_mode, dst_ep, ep, prfl_id, def_resp, cb)      \
{                                                                      \
  zb_uint8_t* ptr = ZB_ZCL_START_PACKET_REQ(buffer)                    \
  ZB_ZCL_CONSTRUCT_SPECIFIC_COMMAND_REQ_FRAME_CONTROL(ptr, (def_resp)) \
  ZB_ZCL_CONSTRUCT_COMMAND_HEADER_REQ(ptr, ZB_ZCL_GET_SEQ_NUM(),       \
      ZB_ZCL_CMD_IAS_ACE_GET_BYPASSED_ZONE_LIST_ID);                   \
  ZB_ZCL_FINISH_PACKET((buffer), ptr)                                  \
  ZB_ZCL_SEND_COMMAND_SHORT(                                           \
      buffer, addr, dst_addr_mode, dst_ep, ep, prfl_id,                \
      ZB_ZCL_CLUSTER_ID_IAS_ACE, cb);                                  \
}

/******************************* Get Zone Status command ******************************/

/*! @brief Structure representsation of Get Zone Status command, ZCL spec 8.3.2.3.8.1 */
typedef ZB_PACKED_PRE struct zb_zcl_ias_ace_get_zone_status_s
{
  /** Starting Zone ID, see @ref zb_zcl_ias_ace_zone_table_s */
  zb_uint8_t  starting_zone_id;
  /** Max Number of Zone IDs Requested Field */
  zb_uint8_t  max_num_zone_ids;
  /** Zone Status Mask Flag Field */
  zb_uint8_t  zone_status_mask_flag;
  /** Zone Status Mask Field, see @ref zb_zcl_ias_zone_zonestatus_e */
  zb_uint16_t zone_status_mask;

} ZB_PACKED_STRUCT zb_zcl_ias_ace_get_zone_status_t;

/*! @brief Get Zone Status command, ZCL spec 8.3.2.3.8
    @param buffer - to put packet to
    @param addr - address to send packet to
    @param dst_addr_mode - addressing mode
    @param dst_ep - destination endpoint
    @param ep - sending endpoint
    @param prfl_id - profile identifier
    @param def_resp - enable/disable default response
    @param cb - callback for getting command send status
    @param starting_zone_id - Starting Zone ID, see @ref zb_zcl_ias_ace_zone_table_s
    @param max_num_zone_ids - Max Number of Zone IDs Requested Field
    @param zone_status_mask_flag - Zone Status Mask Flag Field
    @param zone_status_mask - Zone Status Mask Field
*/
#define ZB_ZCL_IAS_ACE_SEND_GET_ZONE_STATUS_REQ(                       \
  buffer, addr, dst_addr_mode, dst_ep, ep, prfl_id, def_resp, cb,      \
  starting_zone_id, max_num_zone_ids, zone_status_mask_flag,           \
  zone_status_mask)                                                    \
{                                                                      \
  zb_uint8_t* ptr = ZB_ZCL_START_PACKET_REQ(buffer)                    \
  ZB_ZCL_CONSTRUCT_SPECIFIC_COMMAND_REQ_FRAME_CONTROL(ptr, (def_resp)) \
  ZB_ZCL_CONSTRUCT_COMMAND_HEADER_REQ(ptr, ZB_ZCL_GET_SEQ_NUM(),       \
      ZB_ZCL_CMD_IAS_ACE_GET_ZONE_STATUS_ID);                          \
  ZB_ZCL_PACKET_PUT_DATA8(ptr, (starting_zone_id));                    \
  ZB_ZCL_PACKET_PUT_DATA8(ptr, (max_num_zone_ids));                    \
  ZB_ZCL_PACKET_PUT_DATA8(ptr, (zone_status_mask_flag));               \
  ZB_ZCL_PACKET_PUT_DATA16_VAL(ptr, (zone_status_mask));               \
  ZB_ZCL_FINISH_PACKET((buffer), ptr)                                  \
  ZB_ZCL_SEND_COMMAND_SHORT(                                           \
      buffer, addr, dst_addr_mode, dst_ep, ep, prfl_id,                \
      ZB_ZCL_CLUSTER_ID_IAS_ACE, cb);                                  \
}

/** @brief Macro for getting Get Zone Status command, ZCL spec 8.3.2.3.8
  * @attention Assumes that ZCL header already cut.
  * @param data_ptr - pointer to a variable of type @ref
  * zb_zcl_ias_ace_get_zone_status_s.
  * @param buffer containing the packet (by pointer).
  * @param status - variable to put parse status to (see @ref zb_zcl_parse_status_t).
  */
#define ZB_ZCL_IAS_ACE_GET_GET_ZONE_STATUS_REQ(data_ptr, buffer, status) \
{                                                                       \
  if (zb_buf_len((buffer)) < sizeof(zb_zcl_ias_ace_get_zone_status_t))  \
  {                                                                     \
    (status) = ZB_ZCL_PARSE_STATUS_FAILURE;                             \
  }                                                                     \
  else                                                                  \
  {                                                                     \
    zb_zcl_ias_ace_get_zone_status_t *src_ptr =                         \
         (zb_zcl_ias_ace_get_zone_status_t*)zb_buf_begin((buffer));     \
    (status) = ZB_ZCL_PARSE_STATUS_SUCCESS;                             \
    (data_ptr)->starting_zone_id = src_ptr->starting_zone_id;           \
    (data_ptr)->max_num_zone_ids = src_ptr->max_num_zone_ids;           \
    (data_ptr)->zone_status_mask_flag = src_ptr->zone_status_mask_flag; \
    ZB_HTOLE16(&((data_ptr)->zone_status_mask), &(src_ptr->zone_status_mask)); \
  }                                                                     \
}


/******************************* Arm Response command ******************************/

/*! @brief Values of the Arm Notification
 * see ZCL8 spec 8.3.2.4.1.2
*/
enum zb_zcl_ias_ace_arm_not_e
{
  /*! @brief All Zones Disarmed
      @deprecated Enumerator is deprecated. It will be removed after February 2023. Use ZB_ZCL_IAS_ACE_ARM_NOTIF_ALL_ZONES_DISARMED instead.*/
  ZB_ZCL_IAS_ACE_ARM_NOT_DISARM  = 0x00,
  /*! @brief All Zones Disarmed */
  ZB_ZCL_IAS_ACE_ARM_NOTIF_ALL_ZONES_DISARMED = 0x00,
  /*! @brief Only Day/Home Zones Armed
      @deprecated Enumerator is deprecated. It will be removed after February 2023. Use ZB_ZCL_IAS_ACE_ARM_NOTIF_ONLY_DAY_HOME_ZONES_ARMED instead.*/
  ZB_ZCL_IAS_ACE_ARM_NOT_DAY     = 0x01,
  /*! @brief Only Day/Home Zones Armed */
  ZB_ZCL_IAS_ACE_ARM_NOTIF_ONLY_DAY_HOME_ZONES_ARMED = 0x01,
  /*! @brief Only Night/Sleep Zones Armed
      @deprecated Enumerator is deprecated. It will be removed after February 2023. Use ZB_ZCL_IAS_ACE_ARM_NOTIF_ONLY_NIGHT_SLEEP_ZONES_ARMED instead.*/
  ZB_ZCL_IAS_ACE_ARM_NOT_NIGHT   = 0x02,
  /*! @brief Only Night/Sleep Zones Armed */
  ZB_ZCL_IAS_ACE_ARM_NOTIF_ONLY_NIGHT_SLEEP_ZONES_ARMED = 0x02,
  /*! @brief All Zones Armed
      @deprecated Enumerator is deprecated. It will be removed after February 2023. Use ZB_ZCL_IAS_ACE_ARM_NOTIF_ALL_ZONES_ARMED instead.*/
  ZB_ZCL_IAS_ACE_ARM_NOT_ALL     = 0x03,
  /*! @brief All Zones Armed */
  ZB_ZCL_IAS_ACE_ARM_NOTIF_ALL_ZONES_ARMED = 0x03,
  /*! @brief Invalid Arm/Disarm Code */
  ZB_ZCL_IAS_ACE_ARM_NOTIF_INVALID_ARM_DISARM_CODE = 0x04,
  /*! @brief Not ready to arm */
  ZB_ZCL_IAS_ACE_ARM_NOTIF_NOT_READY_TO_ARM = 0x05,
  /*! @brief Already disarmed */
  ZB_ZCL_IAS_ACE_ARM_NOTIF_ALREADY_DISARMED = 0x06,
};

/*! @brief Structure representation of Arm Response command, ZCL spec 8.3.2.5.1  */
typedef ZB_PACKED_PRE struct zb_zcl_ias_ace_arm_resp_s
{
  /** Arm Notification, see @ref zb_zcl_ias_ace_arm_not_e */
  zb_uint8_t arm_notification;

} ZB_PACKED_STRUCT zb_zcl_ias_ace_arm_resp_t;


/*! @brief Arm Response command, ZCL spec 8.3.2.5.1
    @param buffer - to put packet to
    @param addr - address to send packet to
    @param dst_addr_mode - addressing mode
    @param dst_ep - destination endpoint
    @param ep - sending endpoint
    @param prfl_id - profile identifier
    @param seq - sequence
    @param arm_notification - Arm Notification, see @ref zb_zcl_ias_ace_arm_not_e
*/
#define ZB_ZCL_IAS_ACE_SEND_ARM_RESP(                           \
    buffer, addr, dst_addr_mode, dst_ep, ep, prfl_id, seq,      \
    arm_notification)                                           \
{                                                               \
  zb_uint8_t* ptr = ZB_ZCL_START_PACKET(buffer);                \
  ZB_ZCL_CONSTRUCT_SPECIFIC_COMMAND_RES_FRAME_CONTROL(ptr);     \
  ZB_ZCL_CONSTRUCT_COMMAND_HEADER(ptr, seq,                     \
      ZB_ZCL_CMD_IAS_ACE_ARM_RESP_ID);                          \
  ZB_ZCL_PACKET_PUT_DATA8(ptr, (arm_notification));             \
  ZB_ZCL_FINISH_PACKET((buffer), ptr)                           \
  ZB_ZCL_SEND_COMMAND_SHORT(                                    \
      buffer, addr, dst_addr_mode, dst_ep, ep, prfl_id,         \
      ZB_ZCL_CLUSTER_ID_IAS_ACE, NULL);                         \
}

/** @brief Macro for getting Arm Response command, ZCL spec 8.3.2.5.1
  * @attention Assumes that ZCL header already cut.
  * @param data_ptr - pointer to a variable of type @ref
  * zb_zcl_ias_ace_arm_resp_s.
  * @param buffer containing the packet (by pointer).
  * @param status - variable to put parse status to (see @ref zb_zcl_parse_status_t).
  */
#define ZB_ZCL_IAS_ACE_GET_ARM_RESP(data_ptr, buffer, status)       \
{                                                                   \
  if (zb_buf_len((buffer)) < sizeof(zb_zcl_ias_ace_arm_resp_t))     \
  {                                                                 \
    (status) = ZB_ZCL_PARSE_STATUS_FAILURE;                         \
  }                                                                 \
  else                                                              \
  {                                                                 \
    zb_zcl_ias_ace_arm_resp_t *src_ptr =                            \
         (zb_zcl_ias_ace_arm_resp_t*)zb_buf_begin((buffer));        \
    (status) = ZB_ZCL_PARSE_STATUS_SUCCESS;                         \
    (data_ptr)->arm_notification = src_ptr->arm_notification;       \
  }                                                                 \
}

/******************************* Get Zone ID Map Response command ******************************/

/*! @brief Length of array of Bitmap of Zone ID Map, ZCL spec 8.3.2.5.2.1  */
#define ZB_ZCL_IAS_ACE_GET_ZONE_ID_MAP_LENGTH   16

/*! @brief Structure representsation of Get Zone ID Map Response command, ZCL spec 8.3.2.5.2.1  */
typedef ZB_PACKED_PRE struct zb_zcl_ias_ace_get_zone_id_map_resp_s
{
  /** Zone ID Map */
  zb_uint16_t zone_id_map[ZB_ZCL_IAS_ACE_GET_ZONE_ID_MAP_LENGTH];

} ZB_PACKED_STRUCT zb_zcl_ias_ace_get_zone_id_map_resp_t;

/*! @brief Get Zone ID Map Response command, ZCL spec 8.3.2.5.2
    @param buffer - to put packet to
    @param addr - address to send packet to
    @param dst_addr_mode - addressing mode
    @param dst_ep - destination endpoint
    @param ep - sending endpoint
    @param prfl_id - profile identifier
    @param seq - sequence
    @param zone_id_map - Zone ID Map, array[16] of zb_uint16_t
*/
#define ZB_ZCL_IAS_ACE_SEND_GET_ZONE_ID_MAP_RESP(               \
    buffer, addr, dst_addr_mode, dst_ep, ep, prfl_id, seq,      \
    zone_id_map)                                                \
{                                                               \
  zb_uint8_t i;                                                 \
  zb_uint8_t* ptr = ZB_ZCL_START_PACKET(buffer);                \
  ZB_ZCL_CONSTRUCT_SPECIFIC_COMMAND_RES_FRAME_CONTROL(ptr);     \
  ZB_ZCL_CONSTRUCT_COMMAND_HEADER(ptr, seq,                     \
      ZB_ZCL_CMD_IAS_ACE_GET_ZONE_ID_MAP_RESP_ID);              \
  for(i=0; i<ZB_ZCL_IAS_ACE_GET_ZONE_ID_MAP_LENGTH; i++)        \
  {                                                             \
    ZB_ZCL_PACKET_PUT_DATA16_VAL(ptr, (zone_id_map)[i]);        \
  }                                                             \
  ZB_ZCL_FINISH_PACKET((buffer), ptr)                           \
  ZB_ZCL_SEND_COMMAND_SHORT(                                    \
      buffer, addr, dst_addr_mode, dst_ep, ep, prfl_id,         \
      ZB_ZCL_CLUSTER_ID_IAS_ACE, NULL);                         \
}

/** @brief Macro for getting Get Zone ID Map Response command, ZCL spec 8.3.2.5.2
  * @attention Assumes that ZCL header already cut.
  * @param data_ptr - pointer to a variable of type @ref
  * zb_zcl_ias_ace_get_zone_id_map_resp_s.
  * @param buffer containing the packet (by pointer).
  * @param status - variable to put parse status to (see @ref zb_zcl_parse_status_t).
  */
#define ZB_ZCL_IAS_ACE_GET_GET_ZONE_ID_MAP_RESP(data_ptr, buffer, status)       \
{                                                                               \
  if (zb_buf_len((buffer)) < sizeof(zb_zcl_ias_ace_get_zone_id_map_resp_t))     \
  {                                                                             \
    (status) = ZB_ZCL_PARSE_STATUS_FAILURE;                                     \
  }                                                                             \
  else                                                                          \
  {                                                                             \
    zb_uint8_t i;                                                               \
    zb_zcl_ias_ace_get_zone_id_map_resp_t *src_ptr =                            \
         (zb_zcl_ias_ace_get_zone_id_map_resp_t*)zb_buf_begin((buffer));        \
    (status) = ZB_ZCL_PARSE_STATUS_SUCCESS;                                     \
    for(i=0; i<ZB_ZCL_IAS_ACE_GET_ZONE_ID_MAP_LENGTH; i++)                      \
    {                                                                           \
      ZB_HTOLE16(&((data_ptr)->zone_id_map[i]), &(src_ptr->zone_id_map[i]));    \
    }                                                                           \
  }                                                                             \
}

/******************* Get Zone Information Response command ******************************/

/*! @brief Zone Type is empty, ZCL spec 8.3.2.5.3.1  */
#define ZB_ZCL_IAS_ACE_GET_ZONE_INFO_TYPE_NONE   0xffff

/*! @brief Structure representsation of Get Zone Information Response command, ZCL spec 8.3.2.5.3.1
 * see @ ref zb_zcl_ias_ace_zone_table_s*/
typedef ZB_PACKED_PRE struct zb_zcl_ias_ace_get_zone_info_resp_s
{
  /** Zone ID */
  zb_uint8_t zone_id;
  /** Zone Type, see @ref zb_zcl_ias_zone_zonetype_e */
  zb_uint16_t zone_type;
  /** Zone Address */
  zb_ieee_addr_t address;
  /** Zone Label Field */
  zb_uchar_t zone_label[ZB_ZCL_MAX_STRING_SIZE];

} ZB_PACKED_STRUCT zb_zcl_ias_ace_get_zone_info_resp_t;

/*! Get Zone Information Response command payload size */
#define ZB_ZCL_IAS_ACE_GET_ZONE_INFO_RESP_PAYLOAD_SIZE(ptr)   (sizeof(zb_zcl_ias_ace_get_zone_info_resp_t)-sizeof(((zb_zcl_ias_ace_get_zone_info_resp_t *)(ptr))->zone_label)+ \
                                                               1+ZB_ZCL_GET_STRING_LENGTH(((zb_zcl_ias_ace_get_zone_info_resp_t *)(ptr))->zone_label))

/*! @brief Get Zone Information Response command, ZCL spec 8.3.2.5.3
    @param buffer - to put packet to
    @param addr - address to send packet to
    @param dst_addr_mode - addressing mode
    @param dst_ep - destination endpoint
    @param ep - sending endpoint
    @param prfl_id - profile identifier
    @param seq - sequence
    @param zone_id - Zone ID
    @param zone_type - Zone Type, see @ref zb_zcl_ias_zone_zonetype_e
    @param address - Zone Address
    @param zone_label - Zone Label Field
*/
#define ZB_ZCL_IAS_ACE_SEND_GET_ZONE_INFO_RESP(                 \
    buffer, addr, dst_addr_mode, dst_ep, ep, prfl_id, seq,      \
    zone_id, zone_type, address, zone_label)                    \
{                                                               \
  zb_uint8_t* ptr = ZB_ZCL_START_PACKET(buffer);                \
  ZB_ZCL_CONSTRUCT_SPECIFIC_COMMAND_RES_FRAME_CONTROL(ptr);     \
  ZB_ZCL_CONSTRUCT_COMMAND_HEADER(ptr, seq,                     \
      ZB_ZCL_CMD_IAS_ACE_GET_ZONE_INFO_RESP_ID);                \
  ZB_ZCL_PACKET_PUT_DATA8(ptr, (zone_id));                      \
  ZB_ZCL_PACKET_PUT_DATA16_VAL(ptr, (zone_type));               \
  ZB_ZCL_PACKET_PUT_DATA_IEEE(ptr, (address));                  \
  ZB_ZCL_PACKET_PUT_STRING(ptr, (zone_label));                  \
  ZB_ZCL_FINISH_PACKET((buffer), ptr)                           \
  ZB_ZCL_SEND_COMMAND_SHORT(                                    \
      buffer, addr, dst_addr_mode, dst_ep, ep, prfl_id,         \
      ZB_ZCL_CLUSTER_ID_IAS_ACE, NULL);                         \
}

/** @brief Macro for getting Get Zone Information Response command, ZCL spec 8.3.2.5.3
  * @attention Assumes that ZCL header already cut.
  * @param data_ptr - pointer to a variable of type @ref
  * zb_zcl_ias_ace_get_zone_info_resp_s.
  * @param buffer containing the packet (by pointer).
  * @param status - variable to put parse status to (see @ref zb_zcl_parse_status_t).
  */
#define ZB_ZCL_IAS_ACE_GET_GET_ZONE_INFO_RESP(data_ptr, buffer, status)         \
{                                                                               \
  zb_uint8_t *data = zb_buf_begin(buffer);                                      \
  if (zb_buf_len((buffer)) < ZB_ZCL_IAS_ACE_GET_ZONE_INFO_RESP_PAYLOAD_SIZE(data))  \
  {                                                                             \
    (status) = ZB_ZCL_PARSE_STATUS_FAILURE;                                     \
  }                                                                             \
  else                                                                          \
  {                                                                             \
    ZB_ZCL_PACKET_GET_DATA8(&(data_ptr)->zone_id, data);                        \
    ZB_ZCL_PACKET_GET_DATA16(&(data_ptr)->zone_type, data);                     \
    ZB_ZCL_PACKET_GET_DATA_IEEE(&(data_ptr)->address, data);                    \
    ZB_ZCL_PACKET_GET_STATIC_STRING((data_ptr)->zone_label, data);              \
    if (!data)                                                                  \
    {                                                                           \
      (status) = ZB_ZCL_PARSE_STATUS_FAILURE;                                   \
    }                                                                           \
    else                                                                        \
    {                                                                           \
      (status) = ZB_ZCL_PARSE_STATUS_SUCCESS;                                   \
    }                                                                           \
  }                                                                             \
}

/******************* Zone Status Changed command ******************************/

/*! @brief IAS ACE Audible Notification Field
    @see ZCL spec 8.3.2.4.4.4
*/
enum zb_zcl_ias_ace_aud_notification_e
{
  /** Mute (i.e., no audible notification) */
  ZB_ZCL_IAS_ACE_AUD_NOTIFICATION_MUTE           = 0x00,
  /** Default sound */
  ZB_ZCL_IAS_ACE_AUD_NOTIFICATION_DEF_SOUND      = 0x01,
  /** Manufacturer specific */
  ZB_ZCL_IAS_ACE_AUD_NOTIFICATION_MANUF_SPECIFIC = 0x80,
};

/*! @brief Structure representsation of Zone Status Changed command, ZCL spec 8.3.2.4.4 */
typedef ZB_PACKED_PRE struct zb_zcl_ias_ace_zone_status_changed_s
{
  /** Zone ID */
  zb_uint8_t  zone_id;
  /** Zone Status, see @ref zb_zcl_ias_zone_zonestatus_e */
  zb_uint16_t zone_status;
  /** Audible Notification, see @ref zb_zcl_ias_ace_aud_notification_e */
  zb_uint8_t  aud_notification;
  /** Zone Label Field */
  zb_uchar_t   zone_label[ZB_ZCL_MAX_STRING_SIZE];

} ZB_PACKED_STRUCT zb_zcl_ias_ace_zone_status_changed_t;

/*! Zone Status Changed command payload size */
#define ZB_ZCL_IAS_ACE_ZONE_STATUS_CHANGED_PAYLOAD_SIZE(ptr)   (sizeof(zb_zcl_ias_ace_zone_status_changed_t)-sizeof(((zb_zcl_ias_ace_zone_status_changed_t *)(ptr))->zone_label)+ \
                                                                1+ZB_ZCL_GET_STRING_LENGTH(((zb_zcl_ias_ace_zone_status_changed_t *)(ptr))->zone_label))

/*! @brief Zone Status Changed command, ZCL spec 8.3.2.4.4
    @param buffer - to put packet to
    @param addr - address to send packet to
    @param dst_addr_mode - addressing mode
    @param dst_ep - destination endpoint
    @param ep - sending endpoint
    @param prfl_id - profile identifier
    @param def_resp - enable/disable default response
    @param cb - callback for getting command send status
    @param zone_id - Zone ID
    @param zone_status - Zone Status, see @ref zb_zcl_ias_zone_zonestatus_e
    @param aud_notification - Audible Notification, see @ref zb_zcl_ias_ace_aud_notification_e
    @param zone_label - Zone Label Field
*/
#define ZB_ZCL_IAS_ACE_SEND_ZONE_STATUS_CHANGED_REQ(            \
    buffer, addr, dst_addr_mode, dst_ep, ep, prfl_id, def_resp, \
    cb, zone_id, zone_status, aud_notification, zone_label)     \
{                                                               \
  zb_uint8_t* ptr = zb_zcl_start_command_header(buffer,         \
  ZB_ZCL_CONSTRUCT_FRAME_CONTROL(                               \
      ZB_ZCL_FRAME_TYPE_CLUSTER_SPECIFIC,                       \
      ZB_ZCL_NOT_MANUFACTURER_SPECIFIC,                         \
      ZB_ZCL_FRAME_DIRECTION_TO_CLI,                            \
      (def_resp)),                                              \
      0, /* No manuf_code */                                    \
      ZB_ZCL_CMD_IAS_ACE_ZONE_STATUS_CHANGED_ID,                \
      NULL);                                                    \
  ZB_ZCL_PACKET_PUT_DATA8(ptr, (zone_id));                      \
  ZB_ZCL_PACKET_PUT_DATA16_VAL(ptr, (zone_status));             \
  ZB_ZCL_PACKET_PUT_DATA8(ptr, (aud_notification));             \
  ZB_ZCL_PACKET_PUT_STRING(ptr, (zone_label));                  \
  ZB_ZCL_FINISH_PACKET((buffer), ptr)                           \
  ZB_ZCL_SEND_COMMAND_SHORT(                                    \
      buffer, addr, dst_addr_mode, dst_ep, ep, prfl_id,         \
      ZB_ZCL_CLUSTER_ID_IAS_ACE, cb);                           \
}

/** @brief Macro for getting Zone Status Changed command, ZCL spec 8.3.2.4.4
  * @attention Assumes that ZCL header already cut.
  * @param data_ptr - pointer to a variable of type @ref
  * zb_zcl_ias_ace_zone_status_changed_t.
  * @param buffer containing the packet (by pointer).
  * @param status - variable to put parse status to (see @ref zb_zcl_parse_status_t).
  */
#define ZB_ZCL_IAS_ACE_GET_ZONE_STATUS_CHANGED_REQ(data_ptr, buffer, status)    \
{                                                                               \
  zb_uint8_t *data = zb_buf_begin(buffer);                                      \
  if (zb_buf_len((buffer)) < ZB_ZCL_IAS_ACE_ZONE_STATUS_CHANGED_PAYLOAD_SIZE(data))  \
  {                                                                             \
    (status) = ZB_ZCL_PARSE_STATUS_FAILURE;                                     \
  }                                                                             \
  else                                                                          \
  {                                                                             \
    ZB_ZCL_PACKET_GET_DATA8(&(data_ptr)->zone_id, data);                        \
    ZB_ZCL_PACKET_GET_DATA16(&(data_ptr)->zone_status, data);                   \
    ZB_ZCL_PACKET_GET_DATA8(&(data_ptr)->aud_notification, data);               \
    ZB_ZCL_PACKET_GET_STATIC_STRING((data_ptr)->zone_label, data);              \
    if (!data)                                                                  \
    {                                                                           \
      (status) = ZB_ZCL_PARSE_STATUS_FAILURE;                                   \
    }                                                                           \
    else                                                                        \
    {                                                                           \
      (status) = ZB_ZCL_PARSE_STATUS_SUCCESS;                                   \
    }                                                                           \
  }                                                                             \
}

/******************* Panel Status Changed command ******************************/

/*! @brief IAS ACE PanelStatus Parameter
    @see ZCL spec 8.3.2.4.5.2
*/
enum zb_zcl_ias_ace_panel_status_e
{
  /** Panel disarmed (all zones disarmed) and ready to arm */
  ZB_ZCL_IAS_ACE_PANEL_STATUS_DISARMED      = 0x00,
  /** Armed stay */
  ZB_ZCL_IAS_ACE_PANEL_STATUS_ARMED_STAY    = 0x01,
    /** Armed night */
  ZB_ZCL_IAS_ACE_PANEL_STATUS_ARMED_NIGHT   = 0x02,
  /** Armed away */
  ZB_ZCL_IAS_ACE_PANEL_STATUS_ARMED_AWAY    = 0x03,
  /** Exit delay */
  ZB_ZCL_IAS_ACE_PANEL_STATUS_EXIT_DELAY    = 0x04,
  /** Entry delay */
  ZB_ZCL_IAS_ACE_PANEL_STATUS_ENTRY_DELAY   = 0x05,
  /** Not ready to arm */
  ZB_ZCL_IAS_ACE_PANEL_STATUS_NOT_READY     = 0x06,
  /** In alarm */
  ZB_ZCL_IAS_ACE_PANEL_STATUS_IN_ALARM      = 0x07,
  /** Arming Stay */
  ZB_ZCL_IAS_ACE_PANEL_STATUS_ARMING_STAY   = 0x08,
  /** Arming Night */
  ZB_ZCL_IAS_ACE_PANEL_STATUS_ARMING_NIGHT  = 0x09,
  /** Arming Away */
  ZB_ZCL_IAS_ACE_PANEL_STATUS_ARMING_AWAY   = 0x0a,
};

/*! @brief IAS ACE Alarm Status Field
    @see ZCL spec 8.3.2.4.5.4
*/
enum zb_zcl_ias_ace_alarm_status_e
{
  /** No alarm */
  ZB_ZCL_IAS_ACE_ALARM_STATUS_NO_ALARM        = 0x00,
  /** Burglar */
  ZB_ZCL_IAS_ACE_ALARM_STATUS_BURGLAR         = 0x01,
  /** Fire */
  ZB_ZCL_IAS_ACE_ALARM_STATUS_FIRE            = 0x02,
  /** Emergency */
  ZB_ZCL_IAS_ACE_ALARM_STATUS_EMERGENCY       = 0x03,
  /** Police Panic */
  ZB_ZCL_IAS_ACE_ALARM_STATUS_POLICE_PANIC    = 0x04,
  /** Fire Panic */
  ZB_ZCL_IAS_ACE_ALARM_STATUS_FIRE_PANIC      = 0x05,
  /** Emergency Panic */
  ZB_ZCL_IAS_ACE_ALARM_STATUS_EMERGENCY_PANIC = 0x06,
};

/*! @brief Structure representsation of Panel Status Changed command, ZCL spec 8.3.2.4.5 */
typedef ZB_PACKED_PRE struct zb_zcl_ias_ace_panel_status_changed_s
{
  /** Panel Status, see @ref zb_zcl_ias_ace_panel_status_e */
  zb_uint8_t panel_status;
  /** Seconds Remaining Parameter */
  zb_uint8_t seconds_remaining;
  /** Audible Notification, see @ref zb_zcl_ias_ace_aud_notification_e */
  zb_uint8_t aud_notification;
  /** Alarm Status Field, see @ref zb_zcl_ias_ace_alarm_status_e */
  zb_uint8_t alarm_status;

} ZB_PACKED_STRUCT zb_zcl_ias_ace_panel_status_changed_t;

/*! @brief Panel Status Changed command, ZCL spec 8.3.2.4.5
    @param buffer - to put packet to
    @param addr - address to send packet to
    @param dst_addr_mode - addressing mode
    @param dst_ep - destination endpoint
    @param ep - sending endpoint
    @param prfl_id - profile identifier
    @param def_resp - enable/disable default response
    @param cb - callback for getting command send status
    @param panel_status - Panel Status, see @ref zb_zcl_ias_ace_panel_status_e
    @param seconds_remaining - Seconds Remaining Parameter
    @param aud_notification - Audible Notification, see @ref zb_zcl_ias_ace_aud_notification_e
    @param alarm_status - Alarm Status Field, see @ref zb_zcl_ias_ace_alarm_status_e
*/
#define ZB_ZCL_IAS_ACE_SEND_PANEL_STATUS_CHANGED_REQ(           \
  buffer, addr, dst_addr_mode, dst_ep, ep, prfl_id, def_resp,   \
  cb, panel_status, seconds_remaining, aud_notification,        \
  alarm_status)                                                 \
{                                                               \
  zb_uint8_t* ptr = zb_zcl_start_command_header(buffer,         \
  ZB_ZCL_CONSTRUCT_FRAME_CONTROL(                               \
      ZB_ZCL_FRAME_TYPE_CLUSTER_SPECIFIC,                       \
      ZB_ZCL_NOT_MANUFACTURER_SPECIFIC,                         \
      ZB_ZCL_FRAME_DIRECTION_TO_CLI,                            \
      (def_resp)),                                              \
      0, /* No manuf_code */                                    \
      ZB_ZCL_CMD_IAS_ACE_PANEL_STATUS_CHANGED_ID,               \
      NULL);                                                    \
  ZB_ZCL_PACKET_PUT_DATA8(ptr, (panel_status));                 \
  ZB_ZCL_PACKET_PUT_DATA8(ptr, (seconds_remaining));            \
  ZB_ZCL_PACKET_PUT_DATA8(ptr, (aud_notification));             \
  ZB_ZCL_PACKET_PUT_DATA8(ptr, (alarm_status));                 \
  ZB_ZCL_FINISH_PACKET((buffer), ptr)                           \
  ZB_ZCL_SEND_COMMAND_SHORT(                                    \
      buffer, addr, dst_addr_mode, dst_ep, ep, prfl_id,         \
      ZB_ZCL_CLUSTER_ID_IAS_ACE, cb);                           \
}

/** @brief Macro for getting Panel Status Changed command, ZCL spec 8.3.2.4.5
  * @attention Assumes that ZCL header already cut.
  * @param data_ptr - pointer to a variable of type @ref
  * zb_zcl_ias_ace_panel_status_changed_t.
  * @param buffer containing the packet (by pointer).
  * @param status - variable to put parse status to (see @ref zb_zcl_parse_status_t).
  */
#define ZB_ZCL_IAS_ACE_GET_PANEL_STATUS_CHANGED_REQ(data_ptr, buffer, status)   \
{                                                                               \
  zb_uint8_t *data = zb_buf_begin(buffer);                                      \
  if (zb_buf_len((buffer)) < sizeof(zb_zcl_ias_ace_panel_status_changed_t))     \
  {                                                                             \
    (status) = ZB_ZCL_PARSE_STATUS_FAILURE;                                     \
  }                                                                             \
  else                                                                          \
  {                                                                             \
    ZB_ZCL_PACKET_GET_DATA8(&(data_ptr)->panel_status, data);                   \
    ZB_ZCL_PACKET_GET_DATA8(&(data_ptr)->seconds_remaining, data);              \
    ZB_ZCL_PACKET_GET_DATA8(&(data_ptr)->aud_notification, data);               \
    ZB_ZCL_PACKET_GET_DATA8(&(data_ptr)->alarm_status, data);                   \
    (status) = ZB_ZCL_PARSE_STATUS_SUCCESS;                                     \
  }                                                                             \
}

/******************* Get Panel Status Response command ******************************/

/*! @brief Structure representsation of Get Panel Status Response command, ZCL spec 8.3.2.4.6 */
typedef ZB_PACKED_PRE struct zb_zcl_ias_ace_get_panel_status_resp_s
{
  /** Panel Status, see @ref zb_zcl_ias_ace_panel_status_e */
  zb_uint8_t panel_status;
  /** Seconds Remaining Parameter */
  zb_uint8_t seconds_remaining;
  /** Audible Notification, see @ref zb_zcl_ias_ace_aud_notification_e */
  zb_uint8_t aud_notification;
  /** Alarm Status Field, see @ref zb_zcl_ias_ace_alarm_status_e */
  zb_uint8_t alarm_status;

} ZB_PACKED_STRUCT zb_zcl_ias_ace_get_panel_status_resp_t;

/*! @brief Get Panel Status Response command, ZCL spec 8.3.2.4.6
    @param buffer - to put packet to
    @param addr - address to send packet to
    @param dst_addr_mode - addressing mode
    @param dst_ep - destination endpoint
    @param ep - sending endpoint
    @param prfl_id - profile identifier
    @param seq - sequence
    @param panel_status - Panel Status, see @ref zb_zcl_ias_ace_panel_status_e
    @param seconds_remaining - Seconds Remaining Parameter
    @param aud_notification - Audible Notification, see @ref zb_zcl_ias_ace_aud_notification_e
    @param alarm_status - Alarm Status Field, see @ref zb_zcl_ias_ace_alarm_status_e
*/
#define ZB_ZCL_IAS_ACE_SEND_GET_PANEL_STATUS_RESP(              \
    buffer, addr, dst_addr_mode, dst_ep, ep, prfl_id, seq,      \
    panel_status, seconds_remaining, aud_notification,          \
    alarm_status)                                               \
{                                                               \
  zb_uint8_t* ptr = ZB_ZCL_START_PACKET(buffer);                \
  ZB_ZCL_CONSTRUCT_SPECIFIC_COMMAND_RES_FRAME_CONTROL(ptr);     \
  ZB_ZCL_CONSTRUCT_COMMAND_HEADER(ptr, seq,                     \
    ZB_ZCL_CMD_IAS_ACE_GET_PANEL_STATUS_RESPONSE_ID);           \
  ZB_ZCL_PACKET_PUT_DATA8(ptr, (panel_status));                 \
  ZB_ZCL_PACKET_PUT_DATA8(ptr, (seconds_remaining));            \
  ZB_ZCL_PACKET_PUT_DATA8(ptr, (aud_notification));             \
  ZB_ZCL_PACKET_PUT_DATA8(ptr, (alarm_status));                 \
  ZB_ZCL_FINISH_PACKET((buffer), ptr)                           \
  ZB_ZCL_SEND_COMMAND_SHORT(                                    \
      buffer, addr, dst_addr_mode, dst_ep, ep, prfl_id,         \
      ZB_ZCL_CLUSTER_ID_IAS_ACE, NULL);                         \
}

/** @brief Macro for getting Get Panel Status Response command, ZCL spec 8.3.2.4.6
  * @attention Assumes that ZCL header already cut.
  * @param data_ptr - pointer to a variable of type @ref
  * zb_zcl_ias_ace_get_panel_status_resp_t.
  * @param buffer containing the packet (by pointer).
  * @param status - variable to put parse status to (see @ref zb_zcl_parse_status_t).
  */
#define ZB_ZCL_IAS_ACE_GET_GET_PANEL_STATUS_RESP(data_ptr, buffer, status)      \
{                                                                               \
  zb_uint8_t *data = zb_buf_begin(buffer);                                      \
  if (zb_buf_len((buffer)) < sizeof(zb_zcl_ias_ace_panel_status_changed_t))     \
  {                                                                             \
    (status) = ZB_ZCL_PARSE_STATUS_FAILURE;                                     \
  }                                                                             \
  else                                                                          \
  {                                                                             \
    ZB_ZCL_PACKET_GET_DATA8(&(data_ptr)->panel_status, data);                   \
    ZB_ZCL_PACKET_GET_DATA8(&(data_ptr)->seconds_remaining, data);              \
    ZB_ZCL_PACKET_GET_DATA8(&(data_ptr)->aud_notification, data);               \
    ZB_ZCL_PACKET_GET_DATA8(&(data_ptr)->alarm_status, data);                   \
    (status) = ZB_ZCL_PARSE_STATUS_SUCCESS;                                     \
  }                                                                             \
}

/******************* Set Bypassed Zone List command ******************************/

/*! @brief Structure representsation of Set Bypassed Zone List command, ZCL spec 8.3.2.4.7 */
typedef ZB_PACKED_PRE struct zb_zcl_ias_ace_set_bypassed_zone_list_s
{
  /** Number of Zones */
  zb_uint8_t length;
  /** Zone ID array, see @ref zb_zcl_ias_ace_zone_table_s */
  zb_uint8_t zone_id[ZB_ZCL_IAS_ACE_ZONE_TABLE_LENGTH];

} ZB_PACKED_STRUCT zb_zcl_ias_ace_set_bypassed_zone_list_t;

/*! Set Bypassed Zone List command payload size */
#define ZB_ZCL_IAS_ACE_SET_BYPASSED_ZONE_LIST_PAYLOAD_SIZE(ptr)   (sizeof(zb_zcl_ias_ace_set_bypassed_zone_list_t)-sizeof(((zb_zcl_ias_ace_set_bypassed_zone_list_t *)(ptr))->zone_id)+ \
                                                                   ((zb_zcl_ias_ace_set_bypassed_zone_list_t *)(ptr))->length)

/*! @brief Start Set Bypassed Zone List command, ZCL spec 8.3.2.4.7
    @param buffer - to put packet to
    @param length - Number of Zones
    @param ptr - [out] (zb_uint8_t*) current position for @ref ZB_ZCL_IAS_ACE_SEND_SET_BYPASSED_ZONE_LIST_ADD
    and @ref ZB_ZCL_IAS_ACE_SEND_SET_BYPASSED_ZONE_LIST_END
*/
#define ZB_ZCL_IAS_ACE_SEND_SET_BYPASSED_ZONE_LIST_START(       \
    buffer, length, ptr)                                        \
{                                                               \
  (ptr) = ZB_ZCL_START_PACKET(buffer);                          \
  ZB_ZCL_CONSTRUCT_SPECIFIC_COMMAND_RES_FRAME_CONTROL(ptr);     \
  ZB_ZCL_CONSTRUCT_COMMAND_HEADER(ptr, ZB_ZCL_GET_SEQ_NUM(),    \
      ZB_ZCL_CMD_IAS_ACE_SET_BYPASSED_ZONE_LIST_ID);            \
  ZB_ZCL_PACKET_PUT_DATA8((ptr), (length));                     \
}

/*! @brief Start Set Bypassed Zone List command, ZCL spec 8.3.2.4.7
    (response to the Get Bypassed Zone List command)
    @param buffer - to put packet to
    @param seq - sequence
    @param length - Number of Zones
    @param ptr - [out] (zb_uint8_t*) current position for @ref ZB_ZCL_IAS_ACE_SEND_SET_BYPASSED_ZONE_LIST_ADD
    and @ref ZB_ZCL_IAS_ACE_SEND_SET_BYPASSED_ZONE_LIST_END
*/
#define ZB_ZCL_IAS_ACE_SEND_SET_BYPASSED_ZONE_LIST_START_RESP(  \
    buffer, seq, length, ptr)                                   \
{                                                               \
  (ptr) = ZB_ZCL_START_PACKET(buffer);                          \
  ZB_ZCL_CONSTRUCT_SPECIFIC_COMMAND_RES_FRAME_CONTROL(ptr);     \
  ZB_ZCL_CONSTRUCT_COMMAND_HEADER(ptr, seq,                     \
      ZB_ZCL_CMD_IAS_ACE_SET_BYPASSED_ZONE_LIST_ID);            \
  ZB_ZCL_PACKET_PUT_DATA8((ptr), (length));                     \
}

/*! @brief Start Set Bypassed Zone List command, ZCL spec 8.3.2.4.7
    (unsolicited request)
    @param buffer - to put packet to
    @param def_resp - enable/disable default response
    @param length - Number of Zones
    @param ptr - [out] (zb_uint8_t*) current position for @ref ZB_ZCL_IAS_ACE_SEND_SET_BYPASSED_ZONE_LIST_ADD
    and @ref ZB_ZCL_IAS_ACE_SEND_SET_BYPASSED_ZONE_LIST_END
*/
#define ZB_ZCL_IAS_ACE_SEND_SET_BYPASSED_ZONE_LIST_START_REQ(   \
    buffer, def_resp, length, ptr)                              \
{                                                               \
  (ptr) = zb_zcl_start_command_header(buffer,                   \
  ZB_ZCL_CONSTRUCT_FRAME_CONTROL(                               \
      ZB_ZCL_FRAME_TYPE_CLUSTER_SPECIFIC,                       \
      ZB_ZCL_NOT_MANUFACTURER_SPECIFIC,                         \
      ZB_ZCL_FRAME_DIRECTION_TO_CLI,                            \
      (def_resp)),                                              \
      0, /* No manuf_code */                                    \
      ZB_ZCL_CMD_IAS_ACE_SET_BYPASSED_ZONE_LIST_ID,             \
      NULL);                                                    \
  ZB_ZCL_PACKET_PUT_DATA8((ptr), (length));                     \
}

/*! @brief Add Zone Id to Set Bypassed Zone List command, ZCL spec 8.3.2.4.7
    @param ptr - [in/out] (zb_uint8_t*) current position
    @param zone_id - Zone ID, see @ref zb_zcl_ias_ace_zone_table_s
*/
#define ZB_ZCL_IAS_ACE_SEND_SET_BYPASSED_ZONE_LIST_ADD(ptr, zone_id) \
{                                                                    \
  ZB_ZCL_PACKET_PUT_DATA8(ptr, (zone_id));                           \
}

/*! @brief End form Set Bypassed Zone List command and send it, ZCL spec 8.3.2.4.7
    @param ptr - (zb_uint8_t*) current position
    @param buffer - to put packet to
    @param addr - address to send packet to
    @param dst_addr_mode - addressing mode
    @param dst_ep - destination endpoint
    @param ep - sending endpoint
    @param prfl_id - profile identifier
    @param cb - callback for getting command send status
*/
#define ZB_ZCL_IAS_ACE_SEND_SET_BYPASSED_ZONE_LIST_END(           \
    ptr, buffer, addr, dst_addr_mode, dst_ep, ep, prfl_id, cb)    \
{                                                                 \
  ZB_ZCL_FINISH_PACKET((buffer), ptr)                             \
  ZB_ZCL_SEND_COMMAND_SHORT(                                      \
      buffer, addr, dst_addr_mode, dst_ep, ep, prfl_id,           \
      ZB_ZCL_CLUSTER_ID_IAS_ACE, cb);                             \
}

/** @brief Macro for getting Set Bypassed Zone List command, ZCL spec 8.3.2.4.7
  * @attention Assumes that ZCL header already cut.
  * @param data_ptr - pointer to a variable of type @ref
  * zb_zcl_ias_ace_set_bypassed_zone_list_t.
  * @param buffer containing the packet (by pointer).
  * @param status - variable to put parse status to (see @ref zb_zcl_parse_status_t).
  */
#define ZB_ZCL_IAS_ACE_GET_SET_BYPASSED_ZONE_LIST(data_ptr, buffer, status)     \
{                                                                               \
  zb_uint8_t *data = zb_buf_begin(buffer);                                      \
  if (zb_buf_len((buffer)) < ZB_ZCL_IAS_ACE_SET_BYPASSED_ZONE_LIST_PAYLOAD_SIZE(data))  \
  {                                                                             \
    (status) = ZB_ZCL_PARSE_STATUS_FAILURE;                                     \
  }                                                                             \
  else                                                                          \
  {                                                                             \
    ZB_ZCL_PACKET_GET_DATA8(&(data_ptr)->length, data);                         \
    ZB_ZCL_PACKET_GET_DATA_N((data_ptr)->zone_id, data, (data_ptr)->length);    \
    (status) = ZB_ZCL_PARSE_STATUS_SUCCESS;                                     \
  }                                                                             \
}

/******************* Bypass Response command ******************************/

/*! @brief IAS ACE Bypass Result
    @see ZCL spec 8.3.2.4.8.3
*/
enum zb_zcl_ias_ace_bypass_result_e
{
  /** The Zone ID requested to be bypassed is successful. Zone is bypassed. */
  ZB_ZCL_IAS_ACE_BYPASS_RESULT_BYPASSED         = 0x00,
  /** The Zone ID requested to be bypassed is unsuccessful. Zone is not bypassed. */
  ZB_ZCL_IAS_ACE_BYPASS_RESULT_NOT_BYPASSED     = 0x01,
  /** The Zone ID requested to be bypassed is not eligible to be bypassed per the
   *  policy or user configurations on the IAS ACE server. Zone is not bypassed. */
  ZB_ZCL_IAS_ACE_BYPASS_RESULT_NOT_ALLOWED      = 0x02,
  /** The Zone ID requested to be bypassed is not in the valid range of Zone IDs. */
  ZB_ZCL_IAS_ACE_BYPASS_RESULT_INVALID_ZONE_ID  = 0x03,
  /** The Zone ID requested to be bypassed is in the valid range of Zone IDs, but
   *  the IAS ACE server does not have a record of the Zone ID requested. */
  ZB_ZCL_IAS_ACE_BYPASS_RESULT_UNKNOWN_ZONE_ID  = 0x04,
  /** A value returned indicating that the Arm/Disarm Code was entered incorrectly. */
  ZB_ZCL_IAS_ACE_BYPASS_RESULT_INVALID_ARM_CODE = 0x05,
};

/*! @brief Structure representsation of Bypass Response command, ZCL spec 8.3.2.4.8 */
typedef ZB_PACKED_PRE struct zb_zcl_ias_ace_bypass_resp_s
{
  /** Number of Zones */
  zb_uint8_t length;
  /** Bypass Result for Zone IDs, see @ref zb_zcl_ias_ace_bypass_result_e */
  zb_uint8_t bypass_result[ZB_ZCL_IAS_ACE_ZONE_TABLE_LENGTH];

} ZB_PACKED_STRUCT zb_zcl_ias_ace_bypass_resp_t;

/*! Bypass Response command payload size */
#define ZB_ZCL_IAS_ACE_BYPASS_RESP_PAYLOAD_SIZE(ptr)   (sizeof(zb_zcl_ias_ace_bypass_resp_t)-sizeof(((zb_zcl_ias_ace_bypass_resp_t *)(ptr))->bypass_result)+ \
                                                        ((zb_zcl_ias_ace_bypass_resp_t *)(ptr))->length)

/*! @brief Start Bypass Response command, ZCL spec 8.3.2.4.8
    @param buffer - to put packet to
    @param seq - sequence
    @param length - Number of Zones
    @param ptr - [out] (zb_uint8_t*) current position for @ref ZB_ZCL_IAS_ACE_SEND_BYPASS_RESP_ADD
    and @ref ZB_ZCL_IAS_ACE_SEND_BYPASS_RESP_END
*/
#define ZB_ZCL_IAS_ACE_SEND_BYPASS_RESP_START(                  \
    buffer, seq, length, ptr)                                   \
{                                                               \
  (ptr) = ZB_ZCL_START_PACKET(buffer);                          \
  ZB_ZCL_CONSTRUCT_SPECIFIC_COMMAND_RES_FRAME_CONTROL(ptr);     \
  ZB_ZCL_CONSTRUCT_COMMAND_HEADER(ptr, seq,                     \
      ZB_ZCL_CMD_IAS_ACE_BYPASS_RESPONSE_ID);                   \
  ZB_ZCL_PACKET_PUT_DATA8((ptr), (length));                     \
}

/*! @brief Add Bypass results to Bypass Response command, ZCL spec 8.3.2.4.8
    @param ptr - [in/out] (zb_uint8_t*) current position
    @param bypass_result - Bypass Result for Zone IDs, see @ref zb_zcl_ias_ace_bypass_result_e
*/
#define ZB_ZCL_IAS_ACE_SEND_BYPASS_RESP_ADD(ptr, bypass_result) \
{                                                               \
  ZB_ZCL_PACKET_PUT_DATA8(ptr, (bypass_result));                \
}

/*! @brief End form Bypass Response command and send it, ZCL spec 8.3.2.4.8
    @param ptr - (zb_uint8_t*) current position
    @param buffer - to put packet to
    @param addr - address to send packet to
    @param dst_addr_mode - addressing mode
    @param dst_ep - destination endpoint
    @param ep - sending endpoint
    @param prfl_id - profile identifier
*/
#define ZB_ZCL_IAS_ACE_SEND_BYPASS_RESP_END(                      \
    ptr, buffer, addr, dst_addr_mode, dst_ep, ep, prfl_id)        \
{                                                                 \
  ZB_ZCL_FINISH_PACKET((buffer), ptr)                             \
  ZB_ZCL_SEND_COMMAND_SHORT(                                      \
      buffer, addr, dst_addr_mode, dst_ep, ep, prfl_id,           \
      ZB_ZCL_CLUSTER_ID_IAS_ACE, NULL);                           \
}

/** @brief Macro for getting Bypass Response command, ZCL spec 8.3.2.4.8
  * @attention Assumes that ZCL header already cut.
  * @param data_ptr - pointer to a variable of type @ref
  * zb_zcl_ias_ace_bypass_resp_t.
  * @param buffer containing the packet (by pointer).
  * @param status - variable to put parse status to (see @ref zb_zcl_parse_status_t).
  */
#define ZB_ZCL_IAS_ACE_GET_BYPASS_RESP(data_ptr, buffer, status)                \
{                                                                               \
  zb_uint8_t *data = zb_buf_begin(buffer);                                      \
  if (zb_buf_len((buffer)) < ZB_ZCL_IAS_ACE_BYPASS_RESP_PAYLOAD_SIZE(data))     \
  {                                                                             \
    (status) = ZB_ZCL_PARSE_STATUS_FAILURE;                                     \
  }                                                                             \
  else                                                                          \
  {                                                                             \
    ZB_ZCL_PACKET_GET_DATA8(&(data_ptr)->length, data);                         \
    ZB_ZCL_PACKET_GET_DATA_N((data_ptr)->bypass_result, data, (data_ptr)->length); \
    (status) = ZB_ZCL_PARSE_STATUS_SUCCESS;                                     \
  }                                                                             \
}

/******************* Get Zone Status Response command ******************************/

/*! @brief IAS ACE Zone Status structure */
typedef ZB_PACKED_PRE struct zb_zcl_ias_ace_zone_status_s
{
  /** Zone ID, see @ref zb_zcl_ias_ace_zone_table_s */
  zb_uint8_t  zone_id;
  /** Zone Status, see @ref zb_zcl_ias_zone_zonestatus_e */
  zb_uint16_t zone_status;

} ZB_PACKED_STRUCT zb_zcl_ias_ace_zone_status_t;

/*! @brief Structure representsation of Get Zone Status Response command, ZCL spec 8.3.2.4.9 */
typedef ZB_PACKED_PRE struct zb_zcl_ias_ace_get_zone_status_resp_s
{
  /** Zone Status Complete Field */
  zb_uint8_t zone_status_complete;
  /** Number of Zones */
  zb_uint8_t length;
  /** Status of Zone IDs, see @ref zb_zcl_ias_ace_zone_status_t */
  zb_zcl_ias_ace_zone_status_t zone_id_status[ZB_ZCL_IAS_ACE_ZONE_TABLE_LENGTH];

} ZB_PACKED_STRUCT zb_zcl_ias_ace_get_zone_status_resp_t;

/*! Get Zone Status Response command payload size */
#define ZB_ZCL_IAS_ACE_GET_ZONE_STATUS_RESP_PAYLOAD_SIZE(ptr)   ((sizeof(zb_zcl_ias_ace_get_zone_status_resp_t)-sizeof(((zb_zcl_ias_ace_get_zone_status_resp_t *)(ptr))->zone_id_status))+ \
                                                                 ((zb_zcl_ias_ace_get_zone_status_resp_t *)(ptr))->length * sizeof(zb_zcl_ias_ace_zone_status_t))

/*! @brief Get Zone Status Response command, ZCL spec 8.3.2.4.9
    @param buffer - to put packet to
    @param seq - sequence
    @param zone_status_complete - Zone Status Complete
    @param length - Number of Zones
    @param ptr - [out] (zb_uint8_t*) current position for @ref ZB_ZCL_IAS_ACE_SEND_GET_ZONE_STATUS_RESP_ADD
    and @ref ZB_ZCL_IAS_ACE_SEND_GET_ZONE_STATUS_RESP_END
*/
#define ZB_ZCL_IAS_ACE_SEND_GET_ZONE_STATUS_RESP_START(         \
    buffer, seq, zone_status_complete, length, ptr)             \
{                                                               \
  (ptr) = ZB_ZCL_START_PACKET(buffer);                          \
  ZB_ZCL_CONSTRUCT_SPECIFIC_COMMAND_RES_FRAME_CONTROL(ptr);     \
  ZB_ZCL_CONSTRUCT_COMMAND_HEADER(ptr, seq,                     \
      ZB_ZCL_CMD_IAS_ACE_GET_ZONE_STATUS_RESPONSE_ID);          \
  ZB_ZCL_PACKET_PUT_DATA8((ptr), (zone_status_complete));       \
  ZB_ZCL_PACKET_PUT_DATA8((ptr), (length));                     \
}

/*! @brief Add Zone Id and Zone Status to Get Zone Status Response command, ZCL spec 8.3.2.4.9
    @param ptr - [in/out] (zb_uint8_t*) current position
    @param zone_id - Zone ID, see @ref zb_zcl_ias_ace_zone_table_s
    @param zone_status - Zone Status, see @ref zb_zcl_ias_zone_zonestatus_e
*/
#define ZB_ZCL_IAS_ACE_SEND_GET_ZONE_STATUS_RESP_ADD(                 \
    ptr, zone_id, zone_status)                                        \
{                                                                     \
  ZB_ZCL_PACKET_PUT_DATA8(ptr, (zone_id));                            \
  ZB_ZCL_PACKET_PUT_DATA8(ptr, (zone_status));                        \
}

/*! @brief End form Get Zone Status Response command and send it, ZCL spec 8.3.2.4.9
    @param ptr - (zb_uint8_t*) current position
    @param buffer - to put packet to
    @param addr - address to send packet to
    @param dst_addr_mode - addressing mode
    @param dst_ep - destination endpoint
    @param ep - sending endpoint
    @param prfl_id - profile identifier
*/
#define ZB_ZCL_IAS_ACE_SEND_GET_ZONE_STATUS_RESP_END(             \
    ptr, buffer, addr, dst_addr_mode, dst_ep, ep, prfl_id)        \
{                                                                 \
  ZB_ZCL_FINISH_PACKET((buffer), ptr)                             \
  ZB_ZCL_SEND_COMMAND_SHORT(                                      \
      buffer, addr, dst_addr_mode, dst_ep, ep, prfl_id,           \
      ZB_ZCL_CLUSTER_ID_IAS_ACE, NULL);                           \
}

/** @brief Macro for getting Get Zone Status Response command, ZCL spec 8.3.2.4.9
  * @attention Assumes that ZCL header already cut.
  * @param data_ptr - pointer to a variable of type @ref
  * zb_zcl_ias_ace_get_zone_status_resp_t.
  * @param buffer containing the packet (by pointer).
  * @param status - variable to put parse status to (see @ref zb_zcl_parse_status_t).
  */
#define ZB_ZCL_IAS_ACE_GET_GET_ZONE_STATUS_RESP(data_ptr, buffer, status)       \
{                                                                               \
  zb_uint8_t *data = zb_buf_begin(buffer);                                      \
  if (zb_buf_len((buffer)) < ZB_ZCL_IAS_ACE_GET_ZONE_STATUS_RESP_PAYLOAD_SIZE(data))  \
  {                                                                             \
    (status) = ZB_ZCL_PARSE_STATUS_FAILURE;                                     \
  }                                                                             \
  else                                                                          \
  {                                                                             \
    ZB_ZCL_PACKET_GET_DATA8(&(data_ptr)->zone_status_complete, data);           \
    ZB_ZCL_PACKET_GET_DATA8(&(data_ptr)->length, data);                         \
    ZB_ZCL_PACKET_GET_DATA_N((data_ptr)->zone_id_status, data,                  \
                             (data_ptr)->length * sizeof(zb_zcl_ias_ace_zone_status_t)); \
    (status) = ZB_ZCL_PARSE_STATUS_SUCCESS;                                     \
  }                                                                             \
}

/*! @} */ /* IAS Ace cluster commands */

/*! @} */ /* ZCL IAS Ace cluster definitions */

/** @endcond */ /* DOXYGEN_ZCL_SECTION */

void zb_zcl_ias_ace_init_server(void);
void zb_zcl_ias_ace_init_client(void);
#define ZB_ZCL_CLUSTER_ID_IAS_ACE_SERVER_ROLE_INIT zb_zcl_ias_ace_init_server
#define ZB_ZCL_CLUSTER_ID_IAS_ACE_CLIENT_ROLE_INIT zb_zcl_ias_ace_init_client

#endif /* ZB_ZCL_IAS_ACE_H */
