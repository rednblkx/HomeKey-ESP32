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
/* PURPOSE: Time cluster definitions
*/

#ifndef ZB_ZCL_CUSTOM_ATTR_H
#define ZB_ZCL_CUSTOM_ATTR_H 1

#include "zcl/zb_zcl_common.h"
#include "zcl/zb_zcl_commands.h"


/** @cond (DOXYGEN_HA_SECTION && internals_doc) */

/** @addtogroup zcl_api
 * @{
 */

#define ZB_ZCL_CLUSTER_ID_CUSTOM 0x1A0A
#define ZB_CUSTOM_DEVICE_ID 0x29A
#define ZB_CUSTOM_DEVICE_VERSION 6

/*! @name Custom Attributes cluster attributes
    @{
*/

/*! @brief Custom Attributes attribute identifiers

*/
enum zb_zcl_custom_cluster_attr_e
{
  ZB_ZCL_CUSTOM_CLUSTER_ATTR_U8_ID                = 0x0001,
  ZB_ZCL_CUSTOM_CLUSTER_ATTR_S16_ID               = 0x0002,
  ZB_ZCL_CUSTOM_CLUSTER_ATTR_24BIT_ID             = 0x0003,
  ZB_ZCL_CUSTOM_CLUSTER_ATTR_32BITMAP_ID          = 0x0004,
  ZB_ZCL_CUSTOM_CLUSTER_ATTR_IEEE_ID              = 0x0005,
  ZB_ZCL_CUSTOM_CLUSTER_ATTR_CHAR_STRING_ID       = 0x0006,
  ZB_ZCL_CUSTOM_CLUSTER_ATTR_UTC_TIME_ID          = 0x0007,
  ZB_ZCL_CUSTOM_CLUSTER_ATTR_OCTET_STRING_ID      = 0x0008,
  ZB_ZCL_CUSTOM_CLUSTER_ATTR_BOOL_ID              = 0x0009,
  ZB_ZCL_CUSTOM_CLUSTER_ATTR_128_BIT_KEY_ID       = 0x000a,
  ZB_ZCL_CUSTOM_CLUSTER_ATTR_U16_ID               = 0x000b,
  ZB_ZCL_CUSTOM_CLUSTER_ATTR_U32_ID               = 0x000c,
  ZB_ZCL_CUSTOM_CLUSTER_ATTR_LONG_OCTET_STRING_ID = 0x000d
};

#define ZB_ZCL_CUSTOM_CLUSTER_ATTR_U8_DEFAULT_VALUE 0
#define ZB_ZCL_CUSTOM_CLUSTER_ATTR_S16_DEFAULT_VALUE 0
#define ZB_ZCL_CUSTOM_CLUSTER_ATTR_24BIT_DEFAULT_VALUE { 0 }
#define ZB_ZCL_CUSTOM_CLUSTER_ATTR_32BITMAP_DEFAULT_VALUE 0
#define ZB_ZCL_CUSTOM_CLUSTER_ATTR_IEEE_DEFAULT_VALUE { 0 }
#define ZB_ZCL_CUSTOM_CLUSTER_ATTR_CHAR_STRING_DEFAULT_VALUE { 0 }
#define ZB_ZCL_CUSTOM_CLUSTER_ATTR_UTC_TIME_DEFAULT_VALUE 0
#define ZB_ZCL_CUSTOM_CLUSTER_ATTR_BYTE_ARRAY_DEFAULT_VALUE { 0 }
#define ZB_ZCL_CUSTOM_CLUSTER_ATTR_BOOL_DEFAULT_VALUE ZB_FALSE
#define ZB_ZCL_CUSTOM_CLUSTER_ATTR_128_BIT_KEY_DEFAULT_VALUE { 0 }

/** @brief Default value for Custom cluster revision global attribute */
#define ZB_ZCL_CUSTOM_CLUSTER_REVISION_DEFAULT ((zb_uint16_t)0x0001u)

/*! @brief Max size of char string attribute (with length byte) */
#define ZB_ZCL_CUSTOM_CLUSTER_ATTR_CHAR_STRING_MAX_SIZE 11

/*! @brief Max size of byte array attribute (with length bytes) */
#define ZB_ZCL_CUSTOM_CLUSTER_ATTR_BYTE_ARRAY_MAX_SIZE 66

/*! @} */ /* Custom Attributes cluster attributes */

/*! @name Custom Attributes commands
    @{
*/

/*! @brief Custom cluster commands identifiers */
enum zb_zcl_custom_cluster_cmd_e
{
  ZB_ZCL_CUSTOM_CLUSTER_CMD1_ID = 0x21,
  ZB_ZCL_CUSTOM_CLUSTER_CMD2_ID = 0x22,
  ZB_ZCL_CUSTOM_CLUSTER_CMD3_ID = 0x23
};

/*! @brief Custom cluster responses identifiers */
enum zb_zcl_custom_cluster_resp_e
{
  ZB_ZCL_CUSTOM_CLUSTER_CMD1_RESP_ID = 0xF1,
  ZB_ZCL_CUSTOM_CLUSTER_CMD2_RESP_ID = 0xF2
};

/******** Custom cluster command 1 ********/

enum zb_zcl_custom_cluster_cmd1_mode_e
{
  ZB_ZCL_CUSTOM_CLUSTER_CMD1_MODE1 = 0x01,
  ZB_ZCL_CUSTOM_CLUSTER_CMD1_MODE2 = 0x02
};

typedef ZB_PACKED_PRE struct zb_zcl_custom_cluster_cmd1_req_s
{
  zb_uint8_t mode;
  zb_uint8_t value;
}
ZB_PACKED_STRUCT zb_zcl_custom_cluster_cmd1_req_t;

#define ZB_ZCL_CUSTOM_CLUSTER_GET_CMD1_REQ(buf, req, parse_status)         \
{                                                                          \
  zb_zcl_custom_cluster_cmd1_req_t *req_ptr =                              \
    zb_buf_len(buf) >= sizeof(zb_zcl_custom_cluster_cmd1_req_t) ?          \
    (zb_zcl_custom_cluster_cmd1_req_t*)zb_buf_begin(buf) : NULL;           \
  parse_status = ZB_ZCL_PARSE_STATUS_FAILURE;                              \
  if (req_ptr)                                                             \
  {                                                                        \
    ZB_MEMCPY(&(req), req_ptr, sizeof(zb_zcl_custom_cluster_cmd1_req_t));  \
    parse_status = ZB_ZCL_PARSE_STATUS_SUCCESS;                            \
  }                                                                        \
}

/*! @brief Send Custom cluster command 1
    @param buffer - to put packet to
    @param addr - address to send packet to
    @param dst_addr_mode - addressing mode
    @param dst_ep - destination endpoint
    @param ep - sending endpoint
    @param prfl_id - profile identifier
    @param def_resp - enable/disable default response
    @param cb - callback for getting command send status
    @param mode - mode value (@see zb_zcl_custom_cluster_cmd1_mode_t)
    @param value - some value
*/
#define ZB_ZCL_CUSTOM_CLUSTER_SEND_CMD1_REQ(                            \
  buffer, dst_addr, dst_addr_mode, dst_ep,                              \
  src_ep, def_resp, cb, mode, value)                                    \
{                                                                       \
  zb_uint8_t *ptr = ZB_ZCL_START_PACKET_REQ(buffer)                     \
  ZB_ZCL_CONSTRUCT_SPECIFIC_COMMAND_REQ_FRAME_CONTROL(ptr, (def_resp))  \
  ZB_ZCL_CONSTRUCT_COMMAND_HEADER_REQ(                                  \
    ptr, ZB_ZCL_GET_SEQ_NUM(), ZB_ZCL_CUSTOM_CLUSTER_CMD1_ID);          \
  ZB_ZCL_PACKET_PUT_DATA8(ptr, (mode));                                 \
  ZB_ZCL_PACKET_PUT_DATA8(ptr, (value));                                \
  ZB_ZCL_FINISH_PACKET((buffer), ptr)                                   \
  ZB_ZCL_SEND_COMMAND_SHORT(                                            \
    (buffer), (dst_addr), (dst_addr_mode), (dst_ep), (src_ep),          \
    ZB_AF_HA_PROFILE_ID, ZB_ZCL_CLUSTER_ID_CUSTOM, (cb));               \
}

/******** Custom cluster Response to command 1 ********/

typedef ZB_PACKED_PRE struct zb_zcl_custom_cluster_cmd1_resp_s
{
  zb_uint8_t status;
}
ZB_PACKED_STRUCT zb_zcl_custom_cluster_cmd1_resp_t;

#define ZB_ZCL_CUSTOM_CLUSTER_GET_CMD1_RESP(buf, resp, parse_status)          \
{                                                                             \
  zb_zcl_custom_cluster_cmd1_resp_t *resp_ptr =                               \
    zb_buf_len(buf) >= sizeof(zb_zcl_custom_cluster_cmd1_resp_t) ?            \
    (zb_zcl_custom_cluster_cmd1_resp_t*)zb_buf_begin(buf) : NULL;             \
  parse_status = ZB_ZCL_PARSE_STATUS_FAILURE;                                 \
  if (resp_ptr)                                                               \
  {                                                                           \
    ZB_MEMCPY(&(resp), resp_ptr, sizeof(zb_zcl_custom_cluster_cmd1_resp_t));  \
    parse_status = ZB_ZCL_PARSE_STATUS_SUCCESS;                               \
  }                                                                           \
}

/*! @brief Send Custom cluster response to command 1
    @param buffer - to put packet to
    @param addr - address to send packet to
    @param dst_addr_mode - addressing mode
    @param dst_ep - destination endpoint
    @param ep - sending endpoint
    @param prfl_id - profile identifier
    @param def_resp - enable/disable default response
    @param cb - callback for getting command send status
    @param status - status value
 */
#define ZB_ZCL_CUSTOM_CLUSTER_SEND_CMD1_RESP(                        \
  buffer, seq, dst_addr, dst_addr_mode,                              \
  dst_ep, src_ep, cb, status)                                        \
{                                                                    \
  zb_uint8_t *cmd_ptr = ZB_ZCL_START_PACKET(buffer);                 \
  ZB_ZCL_CONSTRUCT_SPECIFIC_COMMAND_RES_FRAME_CONTROL(cmd_ptr);      \
  ZB_ZCL_CONSTRUCT_COMMAND_HEADER(                                   \
    cmd_ptr, (seq), ZB_ZCL_CUSTOM_CLUSTER_CMD1_RESP_ID);             \
  ZB_ZCL_PACKET_PUT_DATA8(cmd_ptr, (status));                        \
  ZB_ZCL_FINISH_PACKET((buffer), cmd_ptr)                            \
  ZB_ZCL_SEND_COMMAND_SHORT(                                         \
    (buffer), (dst_addr), (dst_addr_mode), (dst_ep), (src_ep),       \
    ZB_AF_HA_PROFILE_ID, ZB_ZCL_CLUSTER_ID_CUSTOM, (cb));            \
}

/******** Custom cluster command 2 ********/

enum zb_zcl_custom_cluster_cmd2_param_e
{
  ZB_ZCL_CUSTOM_CLUSTER_CMD2_PARAM1 = 0x01,
  ZB_ZCL_CUSTOM_CLUSTER_CMD2_PARAM2 = 0x02,
  ZB_ZCL_CUSTOM_CLUSTER_CMD2_PARAM3 = 0x03,
  ZB_ZCL_CUSTOM_CLUSTER_CMD2_PARAM4 = 0x04
};

typedef ZB_PACKED_PRE struct zb_zcl_custom_cluster_cmd2_req_s
{
  zb_uint8_t param;
  zb_uint16_t value;
}
ZB_PACKED_STRUCT zb_zcl_custom_cluster_cmd2_req_t;

#define ZB_ZCL_CUSTOM_CLUSTER_GET_CMD2_REQ(buf, req, parse_status)         \
{                                                                          \
  zb_zcl_custom_cluster_cmd2_req_t *req_ptr =                              \
    zb_buf_len(buf) >= sizeof(zb_zcl_custom_cluster_cmd2_req_t) ?          \
    (zb_zcl_custom_cluster_cmd2_req_t*)zb_buf_begin(buf) : NULL;           \
  parse_status = ZB_ZCL_PARSE_STATUS_FAILURE;                              \
  if (req_ptr)                                                             \
  {                                                                        \
    ZB_MEMCPY(&(req), req_ptr, sizeof(zb_zcl_custom_cluster_cmd2_req_t));  \
    if (req_ptr->value < 0xFFFF)                                           \
    {                                                                      \
      parse_status = ZB_ZCL_PARSE_STATUS_SUCCESS;                          \
    }                                                                      \
  }                                                                        \
}

/*! @brief Send Custom cluster command 2
    @param buffer - to put packet to
    @param addr - address to send packet to
    @param dst_addr_mode - addressing mode
    @param dst_ep - destination endpoint
    @param ep - sending endpoint
    @param prfl_id - profile identifier
    @param def_resp - enable/disable default response
    @param cb - callback for getting command send status
    @param param - some parameter
    @param value - some value
 */
#define ZB_ZCL_CUSTOM_CLUSTER_SEND_CMD2_REQ(                            \
  buffer, dst_addr, dst_addr_mode, dst_ep,                              \
  src_ep, def_resp, cb, param, value)                                   \
{                                                                       \
  zb_uint8_t *ptr = ZB_ZCL_START_PACKET_REQ(buffer)                     \
  ZB_ZCL_CONSTRUCT_SPECIFIC_COMMAND_REQ_FRAME_CONTROL(ptr, (def_resp))  \
  ZB_ZCL_CONSTRUCT_COMMAND_HEADER_REQ(                                  \
    ptr, ZB_ZCL_GET_SEQ_NUM(), ZB_ZCL_CUSTOM_CLUSTER_CMD2_ID);          \
  ZB_ZCL_PACKET_PUT_DATA8(ptr, (param));                                \
  ZB_ZCL_PACKET_PUT_DATA16_VAL(ptr, (value));                           \
  ZB_ZCL_FINISH_PACKET((buffer), ptr)                                   \
  ZB_ZCL_SEND_COMMAND_SHORT(                                            \
    (buffer), (dst_addr), (dst_addr_mode), (dst_ep), (src_ep),          \
    ZB_AF_HA_PROFILE_ID, ZB_ZCL_CLUSTER_ID_CUSTOM, (cb));               \
}

/******** Custom cluster Response to command 2 ********/

typedef ZB_PACKED_PRE struct zb_zcl_custom_cluster_cmd2_resp_s
{
  zb_uint8_t status;
}
ZB_PACKED_STRUCT zb_zcl_custom_cluster_cmd2_resp_t;

#define ZB_ZCL_CUSTOM_CLUSTER_GET_CMD2_RESP(buf, resp, status)                \
{                                                                             \
  zb_zcl_custom_cluster_cmd2_resp_t *resp_ptr =                               \
    zb_buf_len(buf) >= sizeof(zb_zcl_custom_cluster_cmd2_resp_t) ?            \
    (zb_zcl_custom_cluster_cmd2_resp_t*)zb_buf_begin(buf) : NULL;             \
  status = ZB_ZCL_PARSE_STATUS_FAILURE;                                       \
  if (resp_ptr)                                                               \
  {                                                                           \
    ZB_MEMCPY(&(resp), resp_ptr, sizeof(zb_zcl_custom_cluster_cmd2_resp_t));  \
    status = ZB_ZCL_PARSE_STATUS_SUCCESS;                                     \
  }                                                                           \
}

/*! @brief Send Custom cluster response to command 2
    @param buffer - to put packet to
    @param addr - address to send packet to
    @param dst_addr_mode - addressing mode
    @param dst_ep - destination endpoint
    @param ep - sending endpoint
    @param prfl_id - profile identifier
    @param def_resp - enable/disable default response
    @param cb - callback for getting command send status
    @param status - status value
 */
#define ZB_ZCL_CUSTOM_CLUSTER_SEND_CMD2_RESP(                        \
  buffer, seq, dst_addr, dst_addr_mode,                              \
  dst_ep, src_ep, cb, status)                                        \
{                                                                    \
  zb_uint8_t *cmd_ptr = ZB_ZCL_START_PACKET(buffer);                 \
  ZB_ZCL_CONSTRUCT_SPECIFIC_COMMAND_RES_FRAME_CONTROL(cmd_ptr);      \
  ZB_ZCL_CONSTRUCT_COMMAND_HEADER(                                   \
    cmd_ptr, seq, ZB_ZCL_CUSTOM_CLUSTER_CMD2_RESP_ID);               \
  ZB_ZCL_PACKET_PUT_DATA8(cmd_ptr, (status));                        \
  ZB_ZCL_FINISH_PACKET((buffer), cmd_ptr)                            \
  ZB_ZCL_SEND_COMMAND_SHORT(                                         \
    (buffer), (dst_addr), (dst_addr_mode), (dst_ep), (src_ep),       \
    ZB_AF_HA_PROFILE_ID, ZB_ZCL_CLUSTER_ID_CUSTOM, (cb));            \
}

/******** Custom cluster command 3 ********/

typedef ZB_PACKED_PRE struct zb_zcl_custom_cluster_cmd3_req_s
{
  zb_char_t zcl_str[ZB_ZCL_CUSTOM_CLUSTER_ATTR_CHAR_STRING_MAX_SIZE];
}
ZB_PACKED_STRUCT zb_zcl_custom_cluster_cmd3_req_t;

#define ZB_ZCL_CUSTOM_CLUSTER_GET_CMD3_REQ(buf, req, parse_status)  \
{                                                                   \
  zb_uint8_t zcl_str_len = *((zb_uint8_t*)zb_buf_begin(buf));       \
  parse_status = ZB_ZCL_PARSE_STATUS_FAILURE;                       \
  if (((zb_uint_t)(zcl_str_len + 1)) ==  zb_buf_len(buf))           \
  {                                                                 \
    ZB_MEMCPY(&(req), zb_buf_begin(buf), zcl_str_len + 1);          \
    parse_status = ZB_ZCL_PARSE_STATUS_SUCCESS;                     \
  }                                                                 \
}

/*! @brief Send Custom cluster command 3
    @param buffer - to put packet to
    @param addr - address to send packet to
    @param dst_addr_mode - addressing mode
    @param dst_ep - destination endpoint
    @param ep - sending endpoint
    @param prfl_id - profile identifier
    @param def_resp - enable/disable default response
    @param cb - callback for getting command send status
    @param zcl_str_ptr - pointer to ZCL string
*/
#define ZB_ZCL_CUSTOM_CLUSTER_SEND_CMD3_REQ(                            \
  buffer, dst_addr, dst_addr_mode, dst_ep,                              \
  src_ep, def_resp, cb, zcl_str_ptr)                                    \
{                                                                       \
  zb_uint8_t *ptr = ZB_ZCL_START_PACKET_REQ(buffer)                     \
  ZB_ZCL_CONSTRUCT_SPECIFIC_COMMAND_REQ_FRAME_CONTROL(ptr, (def_resp))  \
  ZB_ZCL_CONSTRUCT_COMMAND_HEADER_REQ(                                  \
    ptr, ZB_ZCL_GET_SEQ_NUM(), ZB_ZCL_CUSTOM_CLUSTER_CMD3_ID);          \
  ZB_ZCL_PACKET_PUT_STRING(ptr, (zcl_str_ptr));                         \
  ZB_ZCL_FINISH_PACKET((buffer), ptr)                                   \
  ZB_ZCL_SEND_COMMAND_SHORT(                                            \
    (buffer), (dst_addr), (dst_addr_mode), (dst_ep), (src_ep),          \
    ZB_AF_HA_PROFILE_ID, ZB_ZCL_CLUSTER_ID_CUSTOM, (cb));               \
}

/*! @} */ /* Custom Attributes commands */

/*! @internal @name Custom Attributes internals
    Internal structures for attribute representation in cluster definitions.
    @{
*/

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_CUSTOM_CLUSTER_ATTR_U8_ID(data_ptr)           \
{                                                                                   \
  ZB_ZCL_CUSTOM_CLUSTER_ATTR_U8_ID,                                                 \
  ZB_ZCL_ATTR_TYPE_U8,                                                              \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                                     \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                               \
  (void*) data_ptr                                                                  \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_CUSTOM_CLUSTER_ATTR_S16_ID(data_ptr)          \
{                                                                                   \
  ZB_ZCL_CUSTOM_CLUSTER_ATTR_S16_ID,                                                \
  ZB_ZCL_ATTR_TYPE_S16,                                                             \
  ZB_ZCL_ATTR_ACCESS_WRITE_ONLY,                                                    \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                               \
  (void*) data_ptr                                                                  \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_CUSTOM_CLUSTER_ATTR_24BIT_ID(data_ptr)        \
{                                                                                   \
  ZB_ZCL_CUSTOM_CLUSTER_ATTR_24BIT_ID,                                              \
  ZB_ZCL_ATTR_TYPE_24BIT,                                                           \
  ZB_ZCL_ATTR_ACCESS_READ_WRITE,                                                    \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                               \
  (void*) data_ptr                                                                  \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_CUSTOM_CLUSTER_ATTR_32BITMAP_ID(data_ptr)     \
{                                                                                   \
  ZB_ZCL_CUSTOM_CLUSTER_ATTR_32BITMAP_ID,                                           \
  ZB_ZCL_ATTR_TYPE_32BITMAP,                                                        \
  ZB_ZCL_ATTR_ACCESS_WRITE_OPTIONAL,                                                \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                               \
  (void*) data_ptr                                                                  \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_CUSTOM_CLUSTER_ATTR_IEEE_ID(data_ptr)         \
{                                                                                   \
  ZB_ZCL_CUSTOM_CLUSTER_ATTR_IEEE_ID,                                               \
  ZB_ZCL_ATTR_TYPE_IEEE_ADDR,                                                       \
  ZB_ZCL_ATTR_ACCESS_WRITE_OPTIONAL | ZB_ZCL_ATTR_ACCESS_WRITE_ONLY,                \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                               \
  (void*) data_ptr                                                                  \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_CUSTOM_CLUSTER_ATTR_CHAR_STRING_ID(data_ptr)  \
{                                                                                   \
  ZB_ZCL_CUSTOM_CLUSTER_ATTR_CHAR_STRING_ID,                                        \
  ZB_ZCL_ATTR_TYPE_CHAR_STRING,                                                     \
  ZB_ZCL_ATTR_ACCESS_INTERNAL,                                                      \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                               \
  (void*) data_ptr                                                                  \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_CUSTOM_CLUSTER_ATTR_UTC_TIME_ID(data_ptr)     \
{                                                                                   \
  ZB_ZCL_CUSTOM_CLUSTER_ATTR_UTC_TIME_ID,                                           \
  ZB_ZCL_ATTR_TYPE_UTC_TIME,                                                        \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY | ZB_ZCL_ATTR_ACCESS_REPORTING,                      \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                               \
  (void*) data_ptr                                                                  \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_CUSTOM_CLUSTER_ATTR_OCTET_STRING_ID(data_ptr) \
{                                                                                   \
  ZB_ZCL_CUSTOM_CLUSTER_ATTR_OCTET_STRING_ID,                                       \
  ZB_ZCL_ATTR_TYPE_OCTET_STRING,                                                    \
  ZB_ZCL_ATTR_ACCESS_WRITE_ONLY | ZB_ZCL_ATTR_ACCESS_REPORTING,                     \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                               \
  (void*) data_ptr                                                                  \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_CUSTOM_CLUSTER_ATTR_BOOL_ID(data_ptr)         \
{                                                                                   \
  ZB_ZCL_CUSTOM_CLUSTER_ATTR_BOOL_ID,                                               \
  ZB_ZCL_ATTR_TYPE_BOOL,                                                            \
  ZB_ZCL_ATTR_ACCESS_READ_WRITE | ZB_ZCL_ATTR_ACCESS_SCENE,                         \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                               \
  (void*) data_ptr                                                                  \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_CUSTOM_CLUSTER_ATTR_128_BIT_KEY_ID(data_ptr)  \
{                                                                                   \
  ZB_ZCL_CUSTOM_CLUSTER_ATTR_128_BIT_KEY_ID,                                        \
  ZB_ZCL_ATTR_TYPE_128_BIT_KEY,                                                     \
  ZB_ZCL_ATTR_MANUF_SPEC,                                                           \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                               \
  (void*) data_ptr                                                                  \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_CUSTOM_CLUSTER_ATTR_LONG_OCTET_STRING_ID(data_ptr)  \
{                                                                                         \
  ZB_ZCL_CUSTOM_CLUSTER_ATTR_LONG_OCTET_STRING_ID,                                        \
  ZB_ZCL_ATTR_TYPE_LONG_OCTET_STRING,                                                     \
  ZB_ZCL_ATTR_ACCESS_READ_WRITE,                                                          \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                                     \
  (void*) data_ptr                                                                        \
}

/** @internal @brief Declare attribute list for Custom Attributes cluster
*/

#define ZB_ZCL_DECLARE_CUSTOM_ATTR_CLUSTER_ATTRIB_LIST(                                                     \
  attr_list, u8_attr, s16_attr, _24bit_attr, _32bitmap_attr, ieee_attr,                                     \
  char_string_attr, utc_time_attr, octet_string_attr, bool_attr, _128_bit_key_attr, long_octet_string_attr) \
    ZB_ZCL_START_DECLARE_ATTRIB_LIST_CLUSTER_REVISION(attr_list, ZB_ZCL_CUSTOM)                             \
    ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_CUSTOM_CLUSTER_ATTR_U8_ID, (u8_attr))                                       \
    ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_CUSTOM_CLUSTER_ATTR_S16_ID, (s16_attr))                                     \
    ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_CUSTOM_CLUSTER_ATTR_24BIT_ID, (_24bit_attr))                                \
    ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_CUSTOM_CLUSTER_ATTR_32BITMAP_ID, (_32bitmap_attr))                          \
    ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_CUSTOM_CLUSTER_ATTR_IEEE_ID, (ieee_attr))                                   \
    ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_CUSTOM_CLUSTER_ATTR_CHAR_STRING_ID, (char_string_attr))                     \
    ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_CUSTOM_CLUSTER_ATTR_UTC_TIME_ID, (utc_time_attr))                           \
    ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_CUSTOM_CLUSTER_ATTR_OCTET_STRING_ID, (octet_string_attr))                   \
    ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_CUSTOM_CLUSTER_ATTR_BOOL_ID, (bool_attr))                                   \
    ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_CUSTOM_CLUSTER_ATTR_128_BIT_KEY_ID, (_128_bit_key_attr))                    \
    ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_CUSTOM_CLUSTER_ATTR_LONG_OCTET_STRING_ID, (long_octet_string_attr))         \
    ZB_ZCL_FINISH_DECLARE_ATTRIB_LIST

/*! @internal Number of attributes mandatory for reporting in Custom Attributes cluster */
#define ZB_ZCL_CUSTOM_ATTR_REPORT_ATTR_COUNT 8

/*! @} */ /* Custom Attributes cluster internals */

/*! @} */ /* ZCL Custom Attributes cluster definitions */

/** @endcond */ /* (DOXYGEN_ZCL_SECTION && internals_doc) */

void zb_zcl_custom_attr_init_server(void);
void zb_zcl_custom_attr_init_client(void);
#define ZB_ZCL_CLUSTER_ID_CUSTOM_SERVER_ROLE_INIT zb_zcl_custom_attr_init_server
#define ZB_ZCL_CLUSTER_ID_CUSTOM_CLIENT_ROLE_INIT zb_zcl_custom_attr_init_client

#endif /* ZB_ZCL_CUSTOM_ATTR_H */
