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
/* PURPOSE: Alarms cluster definitions
*/

#ifndef ZB_ZCL_ALARMS_H
#define ZB_ZCL_ALARMS_H 1

#include "zcl/zb_zcl_common.h"
#include "zcl/zb_zcl_commands.h"

/** @cond DOXYGEN_ZCL_SECTION */

/** @addtogroup ZB_ZCL_ALARMS
 * @{
 */


/* Cluster ZB_ZCL_CLUSTER_ID_ALARMS */

/*! @name Alarms cluster attributes
    @{
*/

/*! @brief Alarms cluster attribute identifiers
    @see ZCL spec, subclause 3.11.2.2
*/
enum zb_zcl_alarms_attr_e
{
  /*! @brief AlarmCount attribute */
  ZB_ZCL_ATTR_ALARMS_ALARM_COUNT_ID = 0x0000
};

/*! @} */ /* Alarms cluster attributes */

/** @brief Default value for Alarms cluster revision global attribute */
#define ZB_ZCL_ALARMS_CLUSTER_REVISION_DEFAULT ((zb_uint16_t)0x0001u)

/*!
  @brief Declare attribute list for Alarms cluster (only cluster revision attribute)
  @param attr_list - attribute list name
*/
#define ZB_ZCL_DECLARE_ALARMS_ATTR_LIST(attr_list)                            \
  ZB_ZCL_START_DECLARE_ATTRIB_LIST_CLUSTER_REVISION(attr_list, ZB_ZCL_ALARMS) \
  ZB_ZCL_FINISH_DECLARE_ATTRIB_LIST

/*! @name Alarms cluster commands
    @{
*/

/*! @brief Alarms cluster command identifiers
    @see ZCL spec, subclause 3.11.2.4
*/
enum zb_zcl_alarms_cmd_e
{
  ZB_ZCL_CMD_ALARMS_RESET_ALARM_ID         = 0x00, /**< "Reset alarm" command. */
  ZB_ZCL_CMD_ALARMS_RESET_ALL_ALARMS_ID    = 0x01, /**< "Reset all alarms" command. */
  ZB_ZCL_CMD_ALARMS_GET_ALARM_ID           = 0x02, /**< "Get alarms" command. */
  ZB_ZCL_CMD_ALARMS_RESET_ALARM_LOG_ID     = 0x03  /**< "Reset alarm log" command. */
};

/*! @brief Structured representation of Reset alarm command payload */
typedef ZB_PACKED_PRE struct zb_zcl_alarms_reset_alarm_req_s
{
  /** Alarm Code field */
  zb_uint8_t alarm_code;
  /** Cluster ID field */
  zb_uint16_t cluster_id;
} ZB_PACKED_STRUCT zb_zcl_alarms_reset_alarm_req_t;

/** @brief Reset alarm payload length macro */
#define ZB_ZCL_ALARMS_RESET_ALARM_REQ_PAYLOAD_LEN \
  sizeof(zb_zcl_alarms_reset_alarm_req_t)

/*! @brief Send Reset alarm command
    @param buffer to put data to
    @param addr - address of the device to send command to
    @param dst_addr_mode - addressing mode
    @param dst_ep destination endpoint
    @param ep - current endpoint
    @param prof_id - profile identifier
    @param dis_default_resp - "Disable default response" flag
    @param cb - callback to call to report send status
    @param alarm_code - Alarm Code field
    @param cluster_id - Cluster ID field
*/
#define ZB_ZCL_ALARMS_SEND_RESET_ALARM_REQ(                                                       \
  buffer, addr, dst_addr_mode, dst_ep, ep, prof_id, dis_default_resp, cb, alarm_code, cluster_id) \
{                                                                                                 \
  zb_uint8_t* ptr = ZB_ZCL_START_PACKET_REQ(buffer)                                                  \
  ZB_ZCL_CONSTRUCT_SPECIFIC_COMMAND_REQ_FRAME_CONTROL(ptr, dis_default_resp)                     \
  ZB_ZCL_CONSTRUCT_COMMAND_HEADER_REQ(ptr, ZB_ZCL_GET_SEQ_NUM(), ZB_ZCL_CMD_ALARMS_RESET_ALARM_ID);   \
  ZB_ZCL_PACKET_PUT_DATA8(ptr, (alarm_code));                                                     \
  ZB_ZCL_PACKET_PUT_DATA16_VAL(ptr, (cluster_id));                                                \
  ZB_ZCL_FINISH_PACKET(buffer, ptr)                                                              \
  ZB_ZCL_SEND_COMMAND_SHORT(                                                                      \
      buffer, addr, dst_addr_mode, dst_ep, ep, prof_id, ZB_ZCL_CLUSTER_ID_ALARMS, cb);            \
}

/** @brief Parses Reset alarm command and fills in data request
    structure. If request contains invalid data, status will set to ZB_FALSE.
    @param data_buf - pointer to zb_buf_t buffer containing command request data
    @param reset_alarm_req - variable to save command request
    @param status - variable to put parse status to (see @ref zb_zcl_parse_status_t).
    @note data_buf buffer should contain command request payload without ZCL header.
 */
#define ZB_ZCL_ALARMS_GET_RESET_ALARM_REQ(data_buf, reset_alarm_req, status)       \
{                                                                                  \
  zb_zcl_alarms_reset_alarm_req_t *reset_alarm_req_ptr;                            \
  (reset_alarm_req_ptr) = zb_buf_len(data_buf) >=                                  \
    ZB_ZCL_ALARMS_RESET_ALARM_REQ_PAYLOAD_LEN ?                                    \
    (zb_zcl_alarms_reset_alarm_req_t*)zb_buf_begin(data_buf) : NULL;               \
  if (reset_alarm_req_ptr != NULL)                                                 \
  {                                                                                \
    reset_alarm_req.alarm_code = reset_alarm_req_ptr->alarm_code;                  \
    ZB_HTOLE16(&(reset_alarm_req).cluster_id, &(reset_alarm_req_ptr->cluster_id)); \
    (status) = ZB_ZCL_PARSE_STATUS_SUCCESS;                                        \
  }                                                                                \
  else                                                                             \
  {                                                                                \
    (status) = ZB_ZCL_PARSE_STATUS_FAILURE;                                        \
  }                                                                                \
}

/*! @brief Send Reset all alarms command
    @param buffer to put data to
    @param addr - address of the device to send command to
    @param dst_addr_mode - addressing mode
    @param dst_ep destination endpoint
    @param ep - current endpoint
    @param prof_id - profile identifier
    @param dis_default_resp - "Disable default response" flag
    @param cb - callback to call to report send status
*/
#define ZB_ZCL_ALARMS_SEND_RESET_ALL_ALARMS_REQ(                                                     \
  buffer, addr, dst_addr_mode, dst_ep, ep, prof_id, dis_default_resp, cb)                            \
{                                                                                                    \
  zb_uint8_t* ptr = ZB_ZCL_START_PACKET_REQ(buffer)                                                     \
  ZB_ZCL_CONSTRUCT_SPECIFIC_COMMAND_REQ_FRAME_CONTROL(ptr, dis_default_resp)                        \
  ZB_ZCL_CONSTRUCT_COMMAND_HEADER_REQ(ptr, ZB_ZCL_GET_SEQ_NUM(), ZB_ZCL_CMD_ALARMS_RESET_ALL_ALARMS_ID); \
  ZB_ZCL_FINISH_PACKET(buffer, ptr)                                                                 \
  ZB_ZCL_SEND_COMMAND_SHORT(                                                                         \
      buffer, addr, dst_addr_mode, dst_ep, ep, prof_id, ZB_ZCL_CLUSTER_ID_ALARMS, cb);               \
}

/*! @brief Send Get alarm command
    @param buffer to put data to
    @param addr - address of the device to send command to
    @param dst_addr_mode - addressing mode
    @param dst_ep destination endpoint
    @param ep - current endpoint
    @param prof_id - profile identifier
    @param dis_default_resp - "Disable default response" flag
    @param cb - callback to call to report send status
*/
#define ZB_ZCL_ALARMS_SEND_GET_ALARM_REQ(                                                     \
  buffer, addr, dst_addr_mode, dst_ep, ep, prof_id, dis_default_resp, cb)                     \
{                                                                                             \
  zb_uint8_t* ptr = ZB_ZCL_START_PACKET_REQ(buffer)                                              \
  ZB_ZCL_CONSTRUCT_SPECIFIC_COMMAND_REQ_FRAME_CONTROL(ptr, dis_default_resp)                 \
  ZB_ZCL_CONSTRUCT_COMMAND_HEADER_REQ(ptr, ZB_ZCL_GET_SEQ_NUM(), ZB_ZCL_CMD_ALARMS_GET_ALARM_ID); \
  ZB_ZCL_FINISH_PACKET(buffer, ptr)                                                          \
  ZB_ZCL_SEND_COMMAND_SHORT(                                                                  \
      buffer, addr, dst_addr_mode, dst_ep, ep, prof_id, ZB_ZCL_CLUSTER_ID_ALARMS, cb);        \
}

/*! @brief Send Reset alarm log command
    @param buffer to put data to
    @param addr - address of the device to send command to
    @param dst_addr_mode - addressing mode
    @param dst_ep destination endpoint
    @param ep - current endpoint
    @param prof_id - profile identifier
    @param dis_default_resp - "Disable default response" flag
    @param cb - callback to call to report send status
*/
#define ZB_ZCL_ALARMS_SEND_RESET_ALARM_LOG_REQ(                                                     \
  buffer, addr, dst_addr_mode, dst_ep, ep, prof_id, dis_default_resp, cb)                           \
{                                                                                                   \
  zb_uint8_t* ptr = ZB_ZCL_START_PACKET_REQ(buffer)                                                    \
  ZB_ZCL_CONSTRUCT_SPECIFIC_COMMAND_REQ_FRAME_CONTROL(ptr, dis_default_resp)                       \
  ZB_ZCL_CONSTRUCT_COMMAND_HEADER_REQ(ptr, ZB_ZCL_GET_SEQ_NUM(), ZB_ZCL_CMD_ALARMS_RESET_ALARM_LOG_ID); \
  ZB_ZCL_FINISH_PACKET(buffer, ptr)                                                                \
  ZB_ZCL_SEND_COMMAND_SHORT(                                                                        \
      buffer, addr, dst_addr_mode, dst_ep, ep, prof_id, ZB_ZCL_CLUSTER_ID_ALARMS, cb);              \
}

/********************************** Responses *******************/

/** @brief Alarms cluster response command identifiers
    @see ZCL spec, subclause 3.6.2.3
*/
enum zb_zcl_alarms_cmd_resp_e
{
  ZB_ZCL_CMD_ALARMS_ALARM_ID               = 0x00,  /**< Alarm command identifier. */
  ZB_ZCL_CMD_ALARMS_GET_ALARM_RES_ID       = 0x01   /**< Get alarm response command identifier. */
};

/** @cond internals_doc */
/* Alarms cluster commands list : only for information - do not modify */
#define ZB_ZCL_CLUSTER_ID_ALARMS_SERVER_ROLE_GENERATED_CMD_LIST ZB_ZCL_CMD_ALARMS_ALARM_ID

#define ZB_ZCL_CLUSTER_ID_ALARMS_CLIENT_ROLE_RECEIVED_CMD_LIST ZB_ZCL_CLUSTER_ID_ALARMS_SERVER_ROLE_GENERATED_CMD_LIST

#define ZB_ZCL_CLUSTER_ID_ALARMS_CLIENT_ROLE_GENERATED_CMD_LIST                        \
                                               ZB_ZCL_CMD_ALARMS_RESET_ALARM_ID,       \
                                               ZB_ZCL_CMD_ALARMS_RESET_ALL_ALARMS_ID

#define ZB_ZCL_CLUSTER_ID_ALARMS_SERVER_ROLE_RECEIVED_CMD_LIST ZB_ZCL_CLUSTER_ID_ALARMS_CLIENT_ROLE_GENERATED_CMD_LIST
/*! @}
 *  @endcond */ /* internals_doc */

/*! @brief Structured representation of Alarm command payload */
typedef zb_zcl_alarms_reset_alarm_req_t zb_zcl_alarms_alarm_res_t;

typedef ZB_PACKED_PRE struct zb_zcl_alarm_get_alarm_hdr_res_s
{
  zb_uint8_t status;         /**< Status field */
} ZB_PACKED_STRUCT zb_zcl_alarm_get_alarm_hdr_res_t;

/** @brief Get alarm response command structure */
typedef ZB_PACKED_PRE struct zb_zcl_alarm_get_alarm_res_s
{
  zb_uint8_t status;         /**< Status field */
  zb_uint8_t alarm_code;     /**< Alarm code field */
  zb_uint16_t cluster_id;    /**< Cluster ID field */
  zb_uint32_t timestamp;     /**< TimeStamp field */
} ZB_PACKED_STRUCT zb_zcl_alarm_get_alarm_res_t;

/** @internal Get alarm response size */
#define ZB_ZCL_ALARM_GET_ALARM_RES_SIZE \
  sizeof(zb_zcl_alarm_get_alarm_res_t)

/** @internal Get alarm response header size */
#define ZB_ZCL_ALARM_GET_ALARM_HDR_RES_SIZE \
  sizeof(zb_zcl_alarm_get_alarm_hdr_res_t)

/*! @brief Send Alarm command (to client)
    @param buffer to put data to
    @param addr - address of the device to send command to
    @param dst_addr_mode - addressing mode
    @param dst_ep destination endpoint
    @param ep - current endpoint
    @param prof_id - profile identifier
    @param cb - callback to call to report send status
    @param alarm_code - Alarm Code
    @param cluster_id - Cluster Id
*/
#define ZB_ZCL_ALARMS_SEND_ALARM_RES(                                                               \
  buffer, addr, dst_addr_mode, dst_ep, ep, prof_id, cb, alarm_code, cluster_id)                     \
{                                                                                                   \
  zb_uint8_t* ptr = ZB_ZCL_START_PACKET(buffer);                                                    \
  ZB_ZCL_CONSTRUCT_SPECIFIC_COMMAND_RES_FRAME_CONTROL(ptr);                                         \
  ZB_ZCL_CONSTRUCT_COMMAND_HEADER(ptr, ZB_ZCL_GET_SEQ_NUM(), ZB_ZCL_CMD_ALARMS_ALARM_ID);           \
  ZB_ZCL_PACKET_PUT_DATA8(ptr, (alarm_code));                                                       \
  ZB_ZCL_PACKET_PUT_DATA16_VAL(ptr, (cluster_id));                                                  \
  ZB_ZCL_FINISH_PACKET(buffer, ptr)                                                                \
  ZB_ZCL_SEND_COMMAND_SHORT(                                                                        \
      buffer, addr, dst_addr_mode, dst_ep, ep, prof_id, ZB_ZCL_CLUSTER_ID_ALARMS, cb);              \
}

/** @brief Parses Reset alarm command and fills in data request
    structure. If request contains invalid data, status will set to ZB_FALSE.
    @param data_ptr - pointer to a variable of type @ref zb_zcl_alarms_alarm_res_t.
    @param buffer containing the packet (by pointer).
    @param status - variable to put parse status to (see @ref zb_zcl_parse_status_t).
 */
#define ZB_ZCL_ALARMS_GET_ALARM_RES(data_ptr, buffer, status)           \
{                                                                       \
  if (zb_buf_len((buffer)) < sizeof(zb_zcl_alarms_alarm_res_t))         \
  {                                                                     \
    (status) = ZB_ZCL_PARSE_STATUS_FAILURE;                             \
  }                                                                     \
  else                                                                  \
  {                                                                     \
    zb_zcl_alarms_alarm_res_t *src_ptr =                                \
             (zb_zcl_alarms_alarm_res_t*)zb_buf_begin((buffer));        \
    (status) = ZB_ZCL_PARSE_STATUS_SUCCESS;                             \
    (data_ptr)->alarm_code = src_ptr->alarm_code;                       \
    ZB_HTOLE16(&((data_ptr)->cluster_id), &(src_ptr->cluster_id));      \
  }                                                                     \
}


/** @brief Parses Get alarm response command and returns response data
    structure or status = ZB_FALSE if request contains invalid data.
    @param data_buf - pointer to zb_buf_t buffer containing command response data
    @param get_alarm_res - command response record
    @param status - variable to put parse status to (see @ref zb_zcl_parse_status_t)
    @note data_buf buffer should contain response command payload without ZCL header
 */
#define ZB_ZCL_ALARMS_GET_GET_ALARM_RES(data_buf, get_alarm_res, status)         \
{                                                                                \
  zb_zcl_alarm_get_alarm_res_t *get_alarm_res_ptr;                               \
  if (zb_buf_len(data_buf) >= ZB_ZCL_ALARM_GET_ALARM_HDR_RES_SIZE)               \
  {                                                                              \
    (get_alarm_res_ptr) = zb_buf_begin(data_buf);                                \
    get_alarm_res.status = get_alarm_res_ptr->status;                            \
    if (zb_buf_len(data_buf) >= ZB_ZCL_ALARM_GET_ALARM_RES_SIZE)                 \
    {                                                                            \
      get_alarm_res.alarms_code = get_alarm_res_ptr->alarm_code;                 \
      ZB_HTOLE16(&(get_alarm_res).cluster_id, &(get_alarm_res_ptr->cluster_id)); \
      ZB_HTOLE32(&(get_alarm_res).timestamp, &(get_alarm_res_ptr->timestamp));   \
    }                                                                            \
    status = ZB_TRUE;                                                            \
  }                                                                              \
  else                                                                           \
  {                                                                              \
    status = ZB_FALSE;                                                           \
  }                                                                              \
}

/*! @} */ /* Alarms cluster commands */

/*! @internal @name Alarms cluster internals
    Internal structures for attribute representation in cluster definitions.
    @{
*/

/*! @internal Number of attributes mandatory for reporting in Alarms cluster */
#define ZB_ZCL_ALARMS_REPORT_ATTR_COUNT 0


/*! @} */ /* Alarms cluster internals */

/** @endcond */ /* DOXYGEN_ZCL_SECTION */

void zb_zcl_alarms_init_server(void);
void zb_zcl_alarms_init_client(void);

#define ZB_ZCL_CLUSTER_ID_ALARMS_SERVER_ROLE_INIT zb_zcl_alarms_init_server
#define ZB_ZCL_CLUSTER_ID_ALARMS_CLIENT_ROLE_INIT zb_zcl_alarms_init_client

#endif /* ZB_ZCL_ALARMS_H */
