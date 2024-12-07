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
/* PURPOSE: Zigbee cluster library commands common for all clusters
*/
#ifndef ZB_ZCL_COMMANDS_H
#define ZB_ZCL_COMMANDS_H 1

#include "zcl/zb_zcl_common.h"

/** @cond DOXYGEN_ZCL_SECTION */

struct zb_zcl_reporting_info_s; /* Forward declaration */

/* Debug functions */
#if TRACE_ENABLED(TRACE_ZCL1)
#define ZB_ZCL_DEBUG_DUMP_COMMAND(buf) zb_zcl_dump_cmd(buf)
void zb_zcl_dump_cmd(zb_bufid_t buf);
#endif /* TRACE_ENABLED(TRACE_ZCL1) */

#if TRACE_ENABLED(TRACE_ZCL3)
#define ZB_ZCL_DEBUG_DUMP_HEADER(header) dump_zcl_header(header)
void dump_zcl_header(zb_zcl_parsed_hdr_t *header);
#endif /* TRACE_ENABLED(TRACE_ZCL3) */

#ifndef ZB_ZCL_DEBUG_DUMP_COMMAND
#define ZB_ZCL_DEBUG_DUMP_COMMAND(buf) ((void)buf)
#endif /* ZB_ZCL_DEBUG_DUMP_COMMAND */

#ifndef ZB_ZCL_DEBUG_DUMP_HEADER
#define ZB_ZCL_DEBUG_DUMP_HEADER(header) ((void)header)
#endif /* ZB_ZCL_DEBUG_DUMP_HEADER */

/** @addtogroup ZB_ZCL_COMMANDS
 *  @{
 *    @details
 *    This section describes data structures representing command payloads shared by all clusters,
 *    and APIs for sending these commands and parsing their payloads.
 */

/**
 * @name ZCL common command IDs
 * @anchor zcl_cmd
 * @see ZCL spec, clause 2.4
 * @note These values do not contain commands not unused in ZB HA profile
 */
/** @{ */
#define ZB_ZCL_CMD_READ_ATTRIB                     0x00U /*!< Read attributes command */
#define ZB_ZCL_CMD_READ_ATTRIB_RESP                0x01U /*!< Read attributes response command */
#define ZB_ZCL_CMD_WRITE_ATTRIB                    0x02U /*!< Write attributes foundation command */
#define ZB_ZCL_CMD_WRITE_ATTRIB_UNDIV              0x03U /*!< Write attributes undivided command */
#define ZB_ZCL_CMD_WRITE_ATTRIB_RESP               0x04U /*!< Write attributes response command */
#define ZB_ZCL_CMD_WRITE_ATTRIB_NO_RESP            0x05U /*!< Write attributes no response command */
#define ZB_ZCL_CMD_CONFIG_REPORT                   0x06U /*!< Configure reporting command */
#define ZB_ZCL_CMD_CONFIG_REPORT_RESP              0x07U /*!< Configure reporting response command */
#define ZB_ZCL_CMD_READ_REPORT_CFG                 0x08U /*!< Read reporting config command */
#define ZB_ZCL_CMD_READ_REPORT_CFG_RESP            0x09U /*!< Read reporting config response command */
#define ZB_ZCL_CMD_REPORT_ATTRIB                   0x0aU /*!< Report attribute command */
#define ZB_ZCL_CMD_DEFAULT_RESP                    0x0bU /*!< Default response command */
#define ZB_ZCL_CMD_DISC_ATTRIB                     0x0cU /*!< Discover attributes command */
#define ZB_ZCL_CMD_DISC_ATTRIB_RESP                0x0dU /*!< Discover attributes response command */
/* Not release yet */
#define ZB_ZCL_CMD_READ_ATTRIB_STRUCT              0x0eU /*!< Read attributes structured */
#define ZB_ZCL_CMD_WRITE_ATTRIB_STRUCT             0x0fU /*!< Write attributes structured */
#define ZB_ZCL_CMD_WRITE_ATTRIB_STRUCT_RESP        0x10U /*!< Write attributes structured response */

#define ZB_ZCL_CMD_DISCOVER_COMMANDS_RECEIVED      0x11U /*!< Discover Commands Received command */
/** Discover Commands Received response command */
#define ZB_ZCL_CMD_DISCOVER_COMMANDS_RECEIVED_RES  0x12U
#define ZB_ZCL_CMD_DISCOVER_COMMANDS_GENERATED     0x13U /*!< Discover Commands Generated command */
/** Discover Commands Generated response command */
#define ZB_ZCL_CMD_DISCOVER_COMMANDS_GENERATED_RES 0x14U

/* Discover attr ext is HA1.2 specific, but as soon as this command handling is done together with
 * ZCL Discover attr cmd, declare it unconditionally */
#define ZB_ZCL_CMD_DISCOVER_ATTR_EXT               0x15U /*!< Discover attributes extended command */
/** Discover attributes extended response command */
#define ZB_ZCL_CMD_DISCOVER_ATTR_EXT_RES           0x16U
/** @} */

/**
 * @brief Type for ZCL common command IDs.
 *
 * @deprecated holds one of @ref zcl_cmd. Kept only for backward compatibility as
 * @ref zcl_cmd were declared previously as enum. Can be removed in future releases.
 */
typedef zb_uint8_t zb_zcl_cmd_t;

/** @brief ZCL broadcast endpoint */
#define ZB_ZCL_BROADCAST_ENDPOINT   0xFFU

/** @brief Minimum time delay between responses to ZCL command sent to broadcast endpoint */
#define ZB_ZCL_BROADCAST_ENDPOINT_CMD_RESP_JITTER (ZB_MILLISECONDS_TO_SYS_TIMER_INTERVAL(100))

/** @cond internals_doc */

/** @brief Start to declare cluster descriptors list */
#define ZB_ZCL_START_DECLARE_CLUSTER_LIST(_cluster_list_name) \
  zb_zcl_cluster_desc_t _cluster_list_name[] = {


/** @brief Declare general cluster descriptor */
#define ZB_ZCL_DECLARE_CLUSTER_DESC(_cluster_id, _attrib_list, _role) \
  {                                                                   \
    (_cluster_id),                                                    \
    ZB_ZCL_ARRAY_SIZE((_attrib_list), zb_zcl_attr_t),                 \
    (_attrib_list),                                                   \
    (_role),                                                          \
    ZB_ZCL_MANUF_CODE_INVALID,                                        \
  }


/** @brief Declare manufacturer specific cluster descriptor */
#define ZB_ZCL_DECLARE_MANUF_CLUSTER_DESC(_cluster_id, _attrib_list, _role, _code) \
  {                                                                                \
    (_cluster_id),                                                                 \
    ZB_ZCL_ARRAY_SIZE((_attrib_list), zb_zcl_attr_t),                              \
    (_attrib_list),                                                                \
    (_role),                                                                       \
    (_code)                                                                        \
  }


/** @brief Finish cluster descriptors list */
#define ZB_ZCL_FINISH_DECLARE_CLUSTER_LIST  }


/** @internal @brief Send ZCL request command

    Sends ZCL request command (with direction @ref ZB_ZCL_FRAME_DIRECTION_TO_SRV) to the device
    with 16-bit address.
    @param buffer - ID zb_bufid_t of a buffer with payload
    @param addr - short destination address
    @param dst_addr_mode - address mode, only @ref ZB_APS_ADDR_MODE_16_GROUP_ENDP_NOT_PRESENT and
    @ref ZB_APS_ADDR_MODE_16_ENDP_PRESENT are supported
    @param dst_ep - destination end point
    @param ep - our end point
    @param prof_id - profile identifier
    @param cluster_id - cluster identifier
    @param cmd_id - command identifier
*/
#define ZB_ZCL_SEND_GENERAL_COMMAND_REQ_SHORT(                                                  \
    buffer, addr, dst_addr_mode, dst_ep, ep, prof_id, cluster_id, cmd_id)                       \
{                                                                                               \
  zb_uint8_t * ptr = NULL;                                                                      \
  (void)zb_buf_alloc_left((buffer), sizeof(zb_zcl_frame_hdr_short_t), ptr);                     \
  ZB_ZCL_CONSTRUCT_GENERAL_COMMAND_REQ_FRAME_CONTROL(ptr, ZB_ZCL_ENABLE_DEFAULT_RESPONSE);      \
  ZB_ZCL_CONSTRUCT_COMMAND_HEADER(ptr, ZB_ZCL_GET_SEQ_NUM(), (cmd_id));                         \
  ZB_ZCL_SEND_COMMAND_SHORT(buffer, addr, dst_addr_mode, dst_ep, ep, prof_id, cluster_id, NULL) \
}

/** @internal @brief Send ZCL response command

    Sends ZCL response command (with direction @ref ZB_ZCL_FRAME_DIRECTION_TO_CLI) to the device
    with 16-bit address.
    @param buffer - ID zb_bufid_t of a buffer with payload
    @param addr - short destination address
    @param dst_addr_mode - address mode, only @ref ZB_APS_ADDR_MODE_16_GROUP_ENDP_NOT_PRESENT and
    @ref ZB_APS_ADDR_MODE_16_ENDP_PRESENT are supported
    @param dst_ep - destination end point
    @param ep - our end point
    @param prof_id - profile identifier
    @param cluster_id - cluster identifier
    @param seq_num - sequence number
    @param cmd_id - command identifier
*/
#define ZB_ZCL_SEND_GENERAL_COMMAND_RESP_SHORT(                                                 \
    buffer, addr, dst_addr_mode, dst_ep, ep, prof_id, cluster_id, seq_num, cmd_id)              \
{                                                                                               \
  zb_uint8_t * ptr = NULL;                                                                      \
  (void)zb_buf_alloc_left((buffer), sizeof(zb_zcl_frame_hdr_short_t), ptr);                     \
  ZB_ZCL_CONSTRUCT_GENERAL_COMMAND_RESP_FRAME_CONTROL(ptr);                                     \
  TRACE_MSG(                                                                                    \
      TRACE_APS1,                                                                               \
      "fc: 0x%x, seq num: %i, cmd: 0x%hx",                                                      \
      (FMT__D_D_D, *(zb_uint8_t*)zb_buf_begin(buffer), seq_num, cmd_id));                       \
  ZB_ZCL_CONSTRUCT_COMMAND_HEADER(ptr, (seq_num), (cmd_id));                                    \
  ZB_ZCL_SEND_COMMAND_SHORT(buffer, addr, dst_addr_mode, dst_ep, ep, prof_id, cluster_id, NULL) \
}

#define ZB_ZCL_SCHEDULE_STATUS_ABORT(buffer, addr, dst_addr_mode, dst_ep, ep, cb)                  \
  zb_zcl_schedule_status_abort(buffer, addr, dst_addr_mode, dst_ep, ep, cb)

#ifdef ZB_USEALIAS
void zb_zcl_send_command_short_alias(zb_bufid_t buffer,
                                          zb_addr_u *addr, zb_uint8_t dst_addr_mode,
                                          zb_uint8_t dst_ep, zb_uint8_t ep,
                                          zb_uint16_t prof_id, zb_uint16_t cluster_id,
                                          zb_uint8_t radius, zb_callback_t cb, zb_uint8_t use_alias,
                                          zb_uint16_t alias_addr, zb_uint8_t alias_seq);

/** @internal @brief Send ZCL request command + Aliases

    Sends ZCL request command (with direction @ref ZB_ZCL_FRAME_DIRECTION_TO_SRV) to the device
    with 16-bit address.
    All parameters are the same as in ZB_ZCL_SEND_COMMAND_SHORT but with Aliases support
    +
    @param alias_addr - alias address
    @param alias_seq - alias sequence number
    */
#define ZB_ZCL_SEND_COMMAND_SHORT_ALIAS(                                           \
    buffer, addr, dst_addr_mode, dst_ep, ep, prof_id, cluster_id, rd, cb, _use_alias, alias_addr, alias_seq) \
  zb_zcl_send_command_short_alias(buffer, addr, dst_addr_mode, dst_ep, ep, prof_id, cluster_id, rd, cb, _use_alias, alias_addr, alias_seq)
#endif /* ZB_USEALIAS */

/** @internal @brief Send ZCL request command without ACK TX

    Sends ZCL request command (with direction @ref ZB_ZCL_FRAME_DIRECTION_TO_SRV) to the device
    with 16-bit address.
    @param buffer - ID zb_bufid_t of a buffer with payload
    @param ptr - pointer to @ref zb_uint8_t start of ZCL command header
    @param addr - short destination address
    @param dst_addr_mode - address mode, only @ref ZB_APS_ADDR_MODE_16_GROUP_ENDP_NOT_PRESENT and
    @ref ZB_APS_ADDR_MODE_16_ENDP_PRESENT are supported
    @param dst_ep - destination end point
    @param ep - our end point
    @param prof_id - profile identifier
    @param cluster_id - cluster identifier
    @param cb - callback for make next sequence number
    @param random_delay - for a random delay before running the command
    */
#define ZB_ZCL_SEND_COMMAND_SHORT_WITHOUT_ACK(                                                \
    buffer, ptr, addr, dst_addr_mode, dst_ep, ep, prof_id, cluster_id, cb, random_delay)           \
  (void) zb_zcl_finish_and_send_packet_new((buffer), (ptr), (zb_addr_u *)(void *)(&(addr)), dst_addr_mode, dst_ep, ep, prof_id, cluster_id, cb, ZB_FALSE, ZB_TRUE, ZB_RANDOM_VALUE(random_delay))

void zb_zcl_send_command_short_schedule(zb_bufid_t buffer,
                                             zb_uint16_t addr, zb_uint8_t dst_addr_mode,
                                             zb_uint8_t dst_ep, zb_uint8_t ep,
                                             zb_uint16_t prof_id, zb_uint16_t cluster_id,
                                             zb_callback_t cb, zb_uint16_t delay);

/** @internal @brief Send ZCL request command with delay (ms)

    Sends ZCL request command (with direction @ref ZB_ZCL_FRAME_DIRECTION_TO_SRV) to the device
    with 16-bit address.
    @param buffer - ID zb_bufid_t of a buffer with payload
    @param addr - short destination address
    @param dst_addr_mode - address mode, only @ref ZB_APS_ADDR_MODE_16_GROUP_ENDP_NOT_PRESENT and
    @ref ZB_APS_ADDR_MODE_16_ENDP_PRESENT are supported
    @param dst_ep - destination end point
    @param ep - our end point
    @param prof_id - profile identifier
    @param cluster_id - cluster identifier
    @param cb - callback for make next sequence number
    @param delay - Delay current send
    */
#define ZB_ZCL_SEND_COMMAND_SHORT_SCHEDULE(                                        \
    buffer, addr, dst_addr_mode, dst_ep, ep, prof_id, cluster_id, cb, delay)       \
  zb_zcl_send_command_short_schedule(buffer, addr, dst_addr_mode, dst_ep, ep, prof_id, cluster_id, cb, delay)

/** @endcond */ /* internals_doc */

/**
 *  @defgroup zcl_def_resp Default response command sending and parsing.
 *  @{
 *    @details
 *    Default response command is defined in ZCL spec, subclause 2.4.12.
 *
 *    @par Example
 *    Command can be sent like in the following snippet:
 *    @snippet custom_cluster/custom_cluster_zr/custom_cluster_zr.c ZCL_SEND_DEFAULT_RESP
 *    Incoming default response can be parsed as following:
 *    @code
 *      zb_zcl_default_resp_payload_t* payload = ZB_ZCL_READ_DEFAULT_RESP(zcl_cmd_buf);
 *    @endcode
 *    @par
 */

/**
 *  @brief Send default response command with custom Direction
 *
 *  If you don't want to specify direction explicitly, use @ref ZB_ZCL_SEND_DEFAULT_RESP()
 *
 *  @param buffer - ID zb_bufid_t of a buffer with payload
 *  @param addr - short destination address
 *  @param addr_mode - address mode, only @ref ZB_APS_ADDR_MODE_16_GROUP_ENDP_NOT_PRESENT and @ref
 *                     ZB_APS_ADDR_MODE_16_ENDP_PRESENT are supported
 *  @param dst_ep - destination end point
 *  @param ep - our end point
 *  @param prof_id - profile identifier
 *  @param cluster_id - cluster identifier
 *  @param seq_num - sequence number
 *  @param cmd - identifier of the command the response is dedicated to
 *  @param status_code - status field for received command
 *  @param direction - direction of the command (see @ref zcl_frame_direction)
 */
#define ZB_ZCL_SEND_DEFAULT_RESP_DIRECTION(buffer, addr, addr_mode, dst_ep, ep, prof_id,           \
                                           cluster_id, seq_num, cmd, status_code, direction)       \
  {                                                                                                \
    zb_uint8_t *ptr;                                                                               \
    ptr = ZB_ZCL_START_PACKET(buffer);                                                             \
    ZB_ZCL_CONSTRUCT_GENERAL_COMMAND_RESP_FRAME_CONTROL_A(                                         \
        ptr, (direction), ZB_U2B(ZB_ZCL_NOT_MANUFACTURER_SPECIFIC));                               \
    ZB_ZCL_CONSTRUCT_COMMAND_HEADER(ptr, (seq_num), ZB_ZCL_CMD_DEFAULT_RESP);                      \
    *(ptr++) = (cmd);                                                                              \
    *(ptr++) = (status_code);                                                                      \
    ZB_ZCL_FINISH_PACKET((buffer), ptr)                                                            \
    ZB_ZCL_SEND_COMMAND_SHORT((buffer), (addr), (addr_mode), (dst_ep), (ep), (prof_id),            \
                              (cluster_id), NULL);                                                 \
  }

/**
 *  @brief Send default response command and execute callback when it is acknowledged or expired
 *
 *  If no callback is needed, use @ref ZB_ZCL_SEND_DEFAULT_RESP()
 *
 *  @param buffer - ID zb_bufid_t of a buffer with payload
 *  @param addr - short destination address
 *  @param addr_mode - address mode, only @ref ZB_APS_ADDR_MODE_16_GROUP_ENDP_NOT_PRESENT and @ref
 *                     ZB_APS_ADDR_MODE_16_ENDP_PRESENT are supported
 *  @param dst_ep - destination end point
 *  @param ep - our end point
 *  @param prof_id - profile identifier
 *  @param cluster_id - cluster identifier
 *  @param seq_num - sequence number
 *  @param cmd - identifier of the command the response is dedicated to
 *  @param status_code - status field for received command
 *  @param callback - callback to be executed when command is acknowledged or expired (of type @ref zb_callback_t)
 */
#define ZB_ZCL_SEND_DEFAULT_RESP_WITH_CB(buffer, addr, addr_mode, dst_ep, ep, prof_id, cluster_id, \
                                         seq_num, cmd, status_code, callback)                      \
  {                                                                                                \
    zb_uint8_t *ptr;                                                                               \
    ptr = ZB_ZCL_START_PACKET(buffer);                                                             \
    ZB_ZCL_CONSTRUCT_GENERAL_COMMAND_RESP_FRAME_CONTROL(ptr);                                      \
    ZB_ZCL_CONSTRUCT_COMMAND_HEADER(ptr, (seq_num), ZB_ZCL_CMD_DEFAULT_RESP);                      \
    *(ptr++) = (cmd);                                                                              \
    *(ptr++) = (status_code);                                                                      \
    ZB_ZCL_FINISH_PACKET((buffer), ptr)                                                            \
    ZB_ZCL_SEND_COMMAND_SHORT((buffer), (addr), (addr_mode), (dst_ep), (ep), (prof_id),            \
                              (cluster_id), (callback));                                           \
  }

#define ZB_ZCL_SEND_DEFAULT_RESP_WITH_CB_NEW(buffer, addr, addr_mode, dst_ep, ep, prof_id,         \
                                             cluster_id, seq_num, cmd, status_code, callback,      \
                                             aps_secured)                                          \
  {                                                                                                \
    zb_uint8_t *ptr;                                                                               \
    ptr = ZB_ZCL_START_PACKET(buffer);                                                             \
    ZB_ZCL_CONSTRUCT_GENERAL_COMMAND_RESP_FRAME_CONTROL(ptr);                                      \
    ZB_ZCL_CONSTRUCT_COMMAND_HEADER(ptr, (seq_num), ZB_ZCL_CMD_DEFAULT_RESP);                      \
    *(ptr++) = (cmd);                                                                              \
    *(ptr++) = (status_code);                                                                      \
    ZB_ZCL_FINISH_N_SEND_PACKET_NEW((buffer), ptr, (addr), (addr_mode), (dst_ep), (ep), (prof_id), \
                                    (cluster_id), (callback), (aps_secured), ZB_FALSE, 0);         \
  }

/**
 *  @brief Send default response command and execute callback when it's acknowledged or expired
 *
 *  If no callback is needed, use @ref ZB_ZCL_SEND_DEFAULT_RESP()
 *
 *  @param buffer - ID zb_bufid_t of a buffer with payload
 *  @param addr - short destination address
 *  @param addr_mode - address mode, only @ref ZB_APS_ADDR_MODE_16_GROUP_ENDP_NOT_PRESENT and @ref
 *                     ZB_APS_ADDR_MODE_16_ENDP_PRESENT are supported
 *  @param dst_ep - destination end point
 *  @param ep - our end point
 *  @param prof_id - profile identifier
 *  @param cluster_id - cluster identifier
 *  @param seq_num - sequence number
 *  @param cmd - identifier of the command the response is dedicated to
 *  @param status_code - status field for received command
 *  @param manuf_code - manufacturer code
 *  @param direction - direction of the command (see @ref zcl_frame_direction)
 *  @param callback - callback to be executed when command is acknowledged or expired (of type @ref zb_callback_t)
 */
#define ZB_ZCL_SEND_DEFAULT_RESP_MANUF_WITH_CB(buffer, addr, addr_mode, dst_ep, ep, prof_id,       \
  cluster_id, seq_num, cmd, status_code, manuf_code, direction, callback)                          \
{                                                                                                  \
  zb_uint8_t * ptr = NULL;                                                                         \
  ptr = ZB_ZCL_START_PACKET(buffer);                                                               \
  ZB_ZCL_CONSTRUCT_GENERAL_COMMAND_RESP_FRAME_CONTROL_EXT(ptr, ZB_TRUE, direction);                \
  ZB_ZCL_CONSTRUCT_COMMAND_HEADER_EXT(ptr, seq_num, ZB_TRUE, manuf_code, ZB_ZCL_CMD_DEFAULT_RESP); \
  *(ptr++) = cmd;                                                                                  \
  *(ptr++) = status_code;                                                                          \
  ZB_ZCL_FINISH_PACKET(buffer, ptr)                                                                \
  ZB_ZCL_SEND_COMMAND_SHORT(buffer, addr, addr_mode, dst_ep, ep, prof_id, cluster_id, (callback)); \
}

/**
 * @brief check whether command requires default response to be sent
 *
 * Default response is sent if:
 * - particular response is not sent yet
 * - original command is NOT broadcast
 * - disable_default_response is set to FALSE or command status is not Success
 * - command itself is NOT default response
 *
 * This is a helper method, use @ref ZB_ZCL_CHECK_IF_SEND_DEFAULT_RESP instead
 *
 * @param _is_broadcast - broadcast bit from NWK header
 * @param _delivery_mode - delivery mode from APS header
 * @param _disable_def_resp - Disable Default Response bit from ZCL header
 * @param _status - status of the handled command
 * @param _is_def_resp_frame - check for command type
 */
#define ZB_ZCL_CHECK_IF_SEND_DEFAULT_RESP_EXT(                                  \
  _is_broadcast, _delivery_mode, _disable_def_resp, _status, _is_def_resp_frame) \
  (!(_is_broadcast) && ((_delivery_mode) == ZB_APS_DELIVERY_UNICAST) &&          \
   (!(_disable_def_resp) || (_status) != ZB_ZCL_STATUS_SUCCESS)                 \
   && !(_is_def_resp_frame))


/**
   @brief API call that is used to check if it is needed to send
   Default response for the command
   @param _cmd_info - variable of zb_zcl_parsed_hdr_t type, containing
   received command header data
   @param _status - status of the handled command
 */
#define ZB_ZCL_CHECK_IF_SEND_DEFAULT_RESP(_cmd_info, _status)                         \
 (ZB_ZCL_CHECK_IF_SEND_DEFAULT_RESP_EXT(                                              \
    ZB_NWK_IS_ADDRESS_BROADCAST((_cmd_info).addr_data.common_data.dst_addr),          \
    ZB_APS_FC_GET_DELIVERY_MODE((_cmd_info).addr_data.common_data.fc),                \
    (_cmd_info).disable_default_response, _status,                                    \
    ((_cmd_info).is_common_command && (_cmd_info).cmd_id == ZB_ZCL_CMD_DEFAULT_RESP)) \
  && !ZB_ZCL_ADDR_TYPE_IS_GPD((_cmd_info).addr_data.common_data.source.addr_type))

/**
 * @brief General API for sending Default response command
 *
 * @param _buffer - zb_bufid_t buffer
 * @param _dst_addr - 16-bit destination address
 * @param _dst_addr_mode - destination address mode. Possible values
 * ZB_APS_ADDR_MODE_DST_ADDR_ENDP_NOT_PRESENT, ZB_APS_ADDR_MODE_16_GROUP_ENDP_NOT_PRESENT,
 * ZB_APS_ADDR_MODE_16_ENDP_PRESENT
 * @param _dst_ep - destination Endpoint number
 * @param _src_ep - source Endpoint number
 * @param _prof_id - profile ID
 * @param _cluster_id - cluster ID
 * @param _seq_num - transaction sequence number
 * @param _cmd - command ID
 * @param _status_code - command status (see @ref zcl_status)
 * @param _direction - direction of command (see @ref zcl_frame_direction)
 * @param _is_manuf_specific - flag, equal to 1 if command is
 * manufacturer specific
 * @param _manuf_code - manufacturer specific code, is taken unto
 * account only if _is_manuf_specific is equal to 1
 * @param _callback - pointer to the callback function that will be
 * called when the command is sent
 */
#define ZB_ZCL_SEND_DEFAULT_RESP_EXT(_buffer, _dst_addr, _dst_addr_mode, _dst_ep, _src_ep,         \
                                     _prof_id, _cluster_id, _seq_num, _cmd, _status_code,          \
                                     _direction, _is_manuf_specific, _manuf_code, _callback)       \
  {                                                                                                \
    zb_uint8_t *_ptr;                                                                              \
    _ptr = ZB_ZCL_START_PACKET(_buffer);                                                           \
    ZB_ZCL_CONSTRUCT_GENERAL_COMMAND_RESP_FRAME_CONTROL_A(_ptr, (_direction),                      \
                                                          (_is_manuf_specific));                   \
    ZB_ZCL_CONSTRUCT_COMMAND_HEADER_EXT(_ptr, (_seq_num), (_is_manuf_specific), (_manuf_code),     \
                                        ZB_ZCL_CMD_DEFAULT_RESP);                                  \
    *(_ptr++) = (_cmd);                                                                            \
    *(_ptr++) = (_status_code);                                                                    \
    ZB_ZCL_FINISH_PACKET((_buffer), _ptr)                                                          \
    ZB_ZCL_SEND_COMMAND_SHORT((_buffer), (_dst_addr), (_dst_addr_mode), (_dst_ep), (_src_ep),      \
                              (_prof_id), (_cluster_id), (_callback));                             \
  }

#define ZB_ZCL_SEND_DEFAULT_RESP_EXT_SECURED(_buffer, _dst_addr, _dst_addr_mode, _dst_ep, _src_ep,                    \
                                     _prof_id, _cluster_id, _seq_num, _cmd, _status_code,                             \
                                     _direction, _is_manuf_specific, _manuf_code, _callback, _aps_secured)            \
  {                                                                                                                   \
    zb_uint8_t *_ptr;                                                                                                 \
    _ptr = ZB_ZCL_START_PACKET(_buffer);                                                                              \
    ZB_ZCL_CONSTRUCT_GENERAL_COMMAND_RESP_FRAME_CONTROL_A(_ptr, (_direction),                                         \
                                                          (_is_manuf_specific));                                      \
    ZB_ZCL_CONSTRUCT_COMMAND_HEADER_EXT(_ptr, (_seq_num), (_is_manuf_specific), (_manuf_code),                        \
                                        ZB_ZCL_CMD_DEFAULT_RESP);                                                     \
    *(_ptr++) = (_cmd);                                                                                               \
    *(_ptr++) = (_status_code);                                                                                       \
    ZB_ZCL_FINISH_N_SEND_PACKET_NEW((_buffer), _ptr, (_dst_addr), (_dst_addr_mode), (_dst_ep), (_src_ep), (_prof_id), \
                                    (_cluster_id), (_callback), (_aps_secured), ZB_FALSE, 0);                         \
  }

/** @brief Send default response command.
 *  @param buffer - ID zb_bufid_t of a buffer with payload
 *  @param addr - short destination address
 *  @param addr_mode - address mode, only @ref ZB_APS_ADDR_MODE_16_GROUP_ENDP_NOT_PRESENT and @ref
 *  ZB_APS_ADDR_MODE_16_ENDP_PRESENT are supported
 *  @param dst_ep - destination end point
 *  @param ep - our end point
 *  @param prof_id - profile identifier
 *  @param cluster_id - cluster identifier
 *  @param seq_num - sequence number
 *  @param cmd - identifier of the command the response is dedicated to
 *  @param status_code - status field for received command
 */
#define ZB_ZCL_SEND_DEFAULT_RESP(buffer, addr, addr_mode, dst_ep, ep, prof_id, cluster_id,         \
                                 seq_num, cmd, status_code)                                        \
  ZB_ZCL_SEND_DEFAULT_RESP_WITH_CB(buffer, addr, addr_mode, dst_ep, ep, prof_id, cluster_id,       \
                                   seq_num, cmd, status_code, NULL)

#define ZB_ZCL_SEND_DEFAULT_RESP_NEW(buffer, addr, addr_mode, dst_ep, ep, prof_id, cluster_id,     \
                                     seq_num, cmd, status_code, aps_secured)                       \
  ZB_ZCL_SEND_DEFAULT_RESP_WITH_CB_NEW(buffer, addr, addr_mode, dst_ep, ep, prof_id, cluster_id,   \
                                       seq_num, cmd, status_code, NULL, aps_secured)

/**
 *  @brief Send default response command.
 *
 *  @param buffer - ID zb_bufid_t of a buffer with payload
 *  @param addr - short destination address
 *  @param addr_mode - address mode, only @ref ZB_APS_ADDR_MODE_16_GROUP_ENDP_NOT_PRESENT and @ref
 *                     ZB_APS_ADDR_MODE_16_ENDP_PRESENT are supported
 *  @param dst_ep - destination end point
 *  @param ep - our end point
 *  @param prof_id - profile identifier
 *  @param cluster_id - cluster identifier
 *  @param seq_num - sequence number
 *  @param cmd - identifier of the command the response is dedicated to
 *  @param status_code - status field for received command
 *  @param manuf_code - manufacturer code
 *  @param direction - direction of command (see @ref zcl_frame_direction)
 */
#define ZB_ZCL_SEND_DEFAULT_RESP_MANUF(                                                                         \
    buffer, addr, addr_mode, dst_ep, ep, prof_id, cluster_id, seq_num, cmd, status_code, manuf_code, direction) \
      ZB_ZCL_SEND_DEFAULT_RESP_MANUF_WITH_CB(buffer, addr, addr_mode, dst_ep, ep, prof_id, cluster_id,          \
      seq_num, cmd, status_code, manuf_code, direction, NULL)



/** @brief Default response payload structure */
typedef ZB_PACKED_PRE struct zb_zcl_default_resp_payload_s
{
  zb_uint8_t command_id;  /*!< Command identifier */
  zb_uint8_t status;      /*!< Command execution status */
} ZB_PACKED_STRUCT zb_zcl_default_resp_payload_t;

/** @brief Default response structured reading
    @param buffer - pointer to the message buffer (of type zb_bufid_t)
    containing payload
    @return pointer to @ref zb_zcl_default_resp_payload_s structure
    @attention returned pointer will point to the same data in the buffer thus
    being valid until buffer data will be overwritten.
*/
#define ZB_ZCL_READ_DEFAULT_RESP(buffer)                                                                                      \
  ( (zb_buf_len((buffer)) < sizeof(zb_zcl_default_resp_payload_t)) ?                                                          \
    NULL                                                            :                                                         \
    (zb_zcl_default_resp_payload_t*)zb_buf_begin((buffer)));                                                                  \
  if (zb_buf_len((buffer)) >= sizeof(zb_zcl_default_resp_payload_t))                                                          \
  {                                                                                                                           \
    zb_zcl_default_resp_payload_t *default_resp_payload = (zb_zcl_default_resp_payload_t*)zb_buf_begin((buffer));             \
    default_resp_payload->status = zb_zcl_zcl8_statuses_conversion(default_resp_payload->status);                             \
  }

/** @} */ /* Default response command sending and parsing. */

/*************** Read attribute command definitions ************************/

/**
 *  @defgroup read_attr_command Read attributes request and response sending and parsing.
 *  @{
 *    @details
 *    Read attributes command described in ZCL spec, subclauses 2.4.1 and 2.4.2.
 *
 *    Read attributes request command can be formed and sent as in following snippet:
 *    @code
 *    ZB_ZCL_GENERAL_INIT_READ_ATTR_REQ(zcl_cmd_buf, cmd_ptr, ZB_ZCL_ENABLE_DEFAULT_RESPONSE);
 *    ZB_ZCL_GENERAL_ADD_ID_READ_ATTR_REQ(cmd_ptr, ZB_ZCL_ATTR_BINARY_INPUT_PRESENT_VALUE_ID);
 *    ZB_ZCL_GENERAL_SEND_READ_ATTR_REQ(zcl_cmd_buf, cmd_ptr, DUT_ADDR, DUT_ADDR_MODE, DUT_ENDPOINT,
 *                                     TH_ENDPOINT, ZB_AF_HA_PROFILE_ID,
 *                                     ZB_ZCL_CLUSTER_ID_BINARY_INPUT, NULL);
 *    @endcode
 *    Read attributes response can be parsed as:
 *    @code
 *    ZB_ZCL_GENERAL_GET_NEXT_READ_ATTR_RES(buf, read_attr_resp);
 *    @endcode
 *    until allocated buffer space exceeds.
 *
 *    Read attributes request parsing and response filling and sending is implemented in ZCL
 *    library internal functions.
 */

/*! @brief ZCL Read Attribute Command frame
    @see ZCL spec, zb_zcl_read_attr 2.4.1 Read Attributes Command
    @note Command frame contains variable number of parameters
*/
typedef ZB_PACKED_PRE struct zb_zcl_read_attr_req_s
{
  zb_uint16_t attr_id[1]; /*!< Attribute ID list */
} ZB_PACKED_STRUCT
zb_zcl_read_attr_req_t;

/** @cond internals_doc */

/** @brief Minimal size of Read attribute response, it should contain attribute id and status */
#define ZB_ZCL_READ_ATTR_RESP_SIZE (sizeof(zb_uint16_t) + sizeof(zb_uint8_t))

/** @endcond */ /* internals_doc */

/*! @brief ZCL Read Attribute Response Command frame
    @see ZCL spec, zb_zcl_read_attr 2.4.2 Read Attributes Response Command
    @note Command frame contains variable number of parameters. Also, based on status value
    attr_type and attr_value maybe absent.
*/
typedef ZB_PACKED_PRE struct zb_zcl_read_attr_res_s
{
  zb_uint16_t attr_id;       /*!< Attribute ID */
  zb_uint8_t  status;        /*!< Attribute status  */
  zb_uint8_t  attr_type;     /*!< Attribute type */
  zb_uint8_t  attr_value[1]; /*!< Attribute value */
} ZB_PACKED_STRUCT zb_zcl_read_attr_res_t;

/** @brief Parses Read attribute response and returns next Read attribute status record or NULL if
    there is no more data.

   If response contains invalid data, NULL is returned.
   @param data_buf - ID zb_bufid_t of a buffer containing read attribute response data
   @param read_attr_resp - out pointer to zb_zcl_read_attr_res_t, containing Read attribute status
   record
   @note data_buf buffer should contain Read attribute response payload, without ZCL header.  Each
   parsed Read attribute status record is extracted from initial data_buf buffer
 */
#define ZB_ZCL_GENERAL_GET_NEXT_READ_ATTR_RES(data_buf, read_attr_resp)                        \
{                                                                                              \
  zb_uint16_t resp_size = 0xffff;                                                              \
  (read_attr_resp) = zb_buf_len(data_buf) >= ZB_ZCL_READ_ATTR_RESP_SIZE ?                      \
    (zb_zcl_read_attr_res_t*)zb_buf_begin(data_buf) : NULL;                                    \
                                                                                               \
  if (read_attr_resp != NULL)                                                                  \
  {                                                                                            \
    (read_attr_resp)->status = zb_zcl_zcl8_statuses_conversion((read_attr_resp)->status);      \
    resp_size = ZB_ZCL_READ_ATTR_RESP_SIZE;                                                    \
    ZB_ZCL_HTOLE16_INPLACE(&(read_attr_resp)->attr_id);                                        \
    if ((read_attr_resp)->status == ZB_ZCL_STATUS_SUCCESS)                                     \
    {                                                                                          \
      resp_size +=                                                                             \
        sizeof(zb_uint8_t) +                                                                   \
        zb_zcl_get_attribute_size((read_attr_resp)->attr_type, (read_attr_resp)->attr_value);  \
      if (resp_size <= zb_buf_len(data_buf))                                                   \
      {                                                                                        \
        ZB_ZCL_FIX_ENDIAN((read_attr_resp)->attr_value, (read_attr_resp)->attr_type);          \
      }                                                                                        \
    }                                                                                          \
                                                                                               \
    if (resp_size <= zb_buf_len(data_buf))                                                     \
    {                                                                                          \
      (void)zb_buf_cut_left((data_buf), resp_size);                                            \
    }                                                                                          \
    else                                                                                       \
    {                                                                                          \
      (read_attr_resp) = NULL;                                                                 \
    }                                                                                          \
  }                                                                                            \
}

/** @brief Initialize Read attribute command
    @param buffer to put packet to
    @param cmd_ptr - command buffer pointer
    @param def_resp - enable/disable default response
*/
#define ZB_ZCL_GENERAL_INIT_READ_ATTR_REQ(buffer, cmd_ptr, def_resp)                      \
{                                                                                         \
  cmd_ptr = ZB_ZCL_START_PACKET(buffer);                                                  \
  ZB_ZCL_CONSTRUCT_GENERAL_COMMAND_REQ_FRAME_CONTROL(cmd_ptr, def_resp);                  \
  ZB_ZCL_CONSTRUCT_COMMAND_HEADER(cmd_ptr, ZB_ZCL_GET_SEQ_NUM(), ZB_ZCL_CMD_READ_ATTRIB); \
}

/**
 * @brief Initialize Read Attribute Request command
 *
 * @param buffer - buffer to store command data
 * @param cmd_ptr - pointer to a command data memory
 * @param direction - direction of command (see @ref zcl_frame_direction)
 * @param def_resp - enable/disable default response
 */
#define ZB_ZCL_GENERAL_INIT_READ_ATTR_REQ_A(buffer, cmd_ptr, direction, def_resp)         \
{                                                                                         \
  cmd_ptr = ZB_ZCL_START_PACKET(buffer);                                                  \
  ZB_ZCL_CONSTRUCT_GENERAL_COMMAND_REQ_FRAME_CONTROL_A(cmd_ptr, direction,  ZB_ZCL_NOT_MANUFACTURER_SPECIFIC, def_resp);    \
  ZB_ZCL_CONSTRUCT_COMMAND_HEADER(cmd_ptr, ZB_ZCL_GET_SEQ_NUM(), ZB_ZCL_CMD_READ_ATTRIB); \
}

/**
 * @brief Initialize Read Attribute Request command with manufacturer code
 *
 * @param buffer - buffer to store command data
 * @param cmd_ptr - pointer to a command data memory
 * @param direction - direction of command (see @ref zcl_frame_direction)
 * @param def_resp - enable/disable default response
 * @param manuf_code - manufacturer specific code
 */
#define ZB_ZCL_GENERAL_INIT_READ_ATTR_REQ_MANUF(buffer, cmd_ptr, direction, def_resp, manuf_code)                    \
{                                                                                                                    \
  cmd_ptr = ZB_ZCL_START_PACKET(buffer);                                                                             \
  ZB_ZCL_CONSTRUCT_GENERAL_COMMAND_REQ_FRAME_CONTROL_A(cmd_ptr, direction,  ZB_ZCL_MANUFACTURER_SPECIFIC, def_resp); \
  ZB_ZCL_CONSTRUCT_COMMAND_HEADER_EXT(cmd_ptr, ZB_ZCL_GET_SEQ_NUM(), ZB_TRUE, manuf_code, ZB_ZCL_CMD_READ_ATTRIB);   \
}

/** @brief Add attribute id to command payload
    @param cmd_ptr - command buffer pointer
    @param attr_id - attribute ID
*/
#define ZB_ZCL_GENERAL_ADD_ID_READ_ATTR_REQ(cmd_ptr, attr_id) \
{                                                             \
  ZB_ZCL_PACKET_PUT_DATA16_VAL(cmd_ptr, (attr_id));           \
}

/** @brief Sends Read attribute command
    @param buffer to place data to
    @param cmd_ptr - pointer to the memory area after the command data end
    @param addr - address to send packet to
    @param dst_addr_mode - addressing mode
    @param dst_ep - destination endpoint
    @param ep - sending endpoint
    @param profile_id - profile identifier
    @param cluster_id - cluster identifier
    @param cb - callback for getting command send status
*/
#define ZB_ZCL_GENERAL_SEND_READ_ATTR_REQ(buffer, cmd_ptr, addr, dst_addr_mode, dst_ep, ep, profile_id, cluster_id, cb) \
  ZB_ZCL_FINISH_PACKET(buffer, cmd_ptr)                              \
  ZB_ZCL_SEND_COMMAND_SHORT(buffer, addr, dst_addr_mode, dst_ep, ep, profile_id, cluster_id, cb);

/** @brief Parses Read attribute request and returns next Read attribute record or NULL if there
  * is no more data.

   If request contains invalid data, NULL is returned.
   @param _data_buf - ID zb_bufid_t of a buffer containing write attribute request data
   @param _read_attr_req - out pointer to zb_zcl_read_attr_req_t, containing Read attribute record
   out value direct into data_buf. Do not change data_buf before finish work with read_attr_req
   @note data_buf buffer should contain Read attribute request payload, without ZCL header.  Each
   parsed Read attribute record is extracted from initial data_buf buffer

 */
#define ZB_ZCL_GENERAL_GET_READ_ATTR_REQ(_data_buf, _read_attr_req)                     \
{                                                                                       \
  (_read_attr_req) = zb_buf_len((_data_buf)) >= sizeof(zb_zcl_read_attr_req_t) ?        \
    (zb_zcl_read_attr_req_t*)zb_buf_begin((_data_buf)) : NULL;                          \
                                                                                        \
  if ((_read_attr_req))                                                                 \
  {                                                                                     \
    ZB_ZCL_HTOLE16_INPLACE(&(_read_attr_req)->attr_id);                                 \
    (void)zb_buf_cut_left((_data_buf), sizeof(zb_zcl_read_attr_req_t));                 \
  }                                                                                     \
}

/** @brief Initialize Read Attribute Response command
    @param _buffer - buffer to store command data
    @param _cmd_ptr - pointer to a command data memory
    @param _seq - command sequence
*/
#define ZB_ZCL_GENERAL_INIT_READ_ATTR_RESP(_buffer, _cmd_ptr, _seq)                     \
{                                                                                       \
  cmd_ptr = ZB_ZCL_START_PACKET((_buffer));                                             \
  ZB_ZCL_CONSTRUCT_GENERAL_COMMAND_RESP_FRAME_CONTROL((_cmd_ptr));                      \
  ZB_ZCL_CONSTRUCT_COMMAND_HEADER((_cmd_ptr), (_seq), ZB_ZCL_CMD_READ_ATTRIB_RESP);     \
}

/**
 * @brief Initialize Read Attribute Response command
 *
 * @param _buffer - buffer to store command data
 * @param _cmd_ptr - pointer to a command data memory
 * @param _direction - direction of command (see @ref zcl_frame_direction)
 * @param _seq - command sequence
 * @param _is_manuf - whether command is manufacturer specific
 * @param _manuf_id - manufacturer ID (needed if _is_manuf is set)
 */
#define ZB_ZCL_GENERAL_INIT_READ_ATTR_RESP_EXT(_buffer, _cmd_ptr, _direction, _seq, _is_manuf, _manuf_id)       \
{                                                                                                               \
  cmd_ptr = ZB_ZCL_START_PACKET((_buffer));                                                                     \
  ZB_ZCL_CONSTRUCT_GENERAL_COMMAND_RESP_FRAME_CONTROL_A((_cmd_ptr), (_direction), (_is_manuf));                 \
  ZB_ZCL_CONSTRUCT_COMMAND_HEADER_EXT((_cmd_ptr), (_seq), (_is_manuf), (_manuf_id), ZB_ZCL_CMD_READ_ATTRIB_RESP);       \
}


/** @brief Send Read attribute response command
    @param _buffer - buffer to store command data
    @param _cmd_ptr - pointer to a command data memory
    @param _addr - address to send packet to
    @param _dst_addr_mode - addressing mode
    @param _dst_ep - destination endpoint
    @param _ep - sending endpoint
    @param _profile_id - profile identifier
    @param _cluster_id - cluster identifier
    @param _cb - callback for getting command send status
*/
#define ZB_ZCL_GENERAL_SEND_READ_ATTR_RESP(                                                     \
    _buffer, _cmd_ptr, _addr, _dst_addr_mode, _dst_ep, _ep, _profile_id, _cluster_id, _cb)      \
{                                                                                               \
  ZB_ZCL_FINISH_PACKET((_buffer), (_cmd_ptr))                                                  \
  ZB_ZCL_SEND_COMMAND_SHORT((_buffer), (_addr), (_dst_addr_mode), (_dst_ep), (_ep),             \
                (_profile_id), (_cluster_id), (_cb));                                           \
}

/** @} */ /* Read attribute request and response sending and parsing. */

/********************** Write attribute command definitions ***************************/

/**
 *  @defgroup write_attr_cmd Write attributes command sending and parsing.
 *  @{
 *    @details
 *    Both write attributes request and response commands have variable-length payload.
 *
 *    Write attributes request can be filled as following:
 *    @code
 *    ZB_ZCL_GENERAL_INIT_WRITE_ATTR_REQ(data_buf, ptr, ZB_ZCL_ENABLE_DEFAULT_RESPONSE);
 *    ZB_ZCL_GENERAL_ADD_VALUE_WRITE_ATTR_REQ(ptr, attr_id, attr_type, attr_val);
 *    ZB_ZCL_GENERAL_SEND_WRITE_ATTR_REQ(
 *        data_buf,
 *        ptr,
 *        dst_addr,
 *        ZB_APS_ADDR_MODE_16_ENDP_PRESENT,
 *        dst_ep,
 *        src_ep,
 *        ZB_AF_HA_PROFILE_ID,
 *        ZB_ZCL_CLUSTER_ID_ON_OFF,
 *        NULL);
 *    @endcode
 *    On the server side, this packet could be parsed in the following manner:
 *    @code
 *    zb_zcl_write_attr_req_t *write_attr_req;
 *    ...
 *    do
 *    {
 *      ZB_ZCL_GENERAL_GET_NEXT_WRITE_ATTR_REQ(data_buf, write_attr_req);
 *      if (write_attr_req)
 *      {
 *        process write attribute request record
 *      }
 *    }
 *    while(write_attr_req);
 *    @endcode
 *
 *    Response sending and parsing could be done in the same manner.
 */

/*! @brief ZCL Write Attribute Command frame
    @see ZCL spec, 2.4.3 Write Attributes Command
*/
typedef ZB_PACKED_PRE struct zb_zcl_write_attr_req_s
{
  zb_uint16_t attr_id;       /*!< Attribute ID */
  zb_uint8_t  attr_type;     /*!< Attribute type */
  zb_uint8_t  attr_value[1]; /*!< Attribute value */
} ZB_PACKED_STRUCT
zb_zcl_write_attr_req_t;

/** @cond internals_doc */

/**  @brief Minimal size of Write attribute request, it will be more if attr_value size is more
  * than 1 byte
  */
#define ZB_ZCL_WRITE_ATTR_REQ_SIZE sizeof(zb_zcl_write_attr_req_t)

/** @endcond */ /* internals_doc */

/** @brief Parses Write attribute request and returns next Write attribute record or NULL if there
  * is no more data.

   If request contains invalid data, NULL is returned.
   @param data_ptr - pointer to the data of a zb_bufid_t buffer containing write attribute request data
   @param data_len - variable containing length of a zb_bufid_t buffer
   @param write_attr_req - out pointer to zb_zcl_write_attr_req_t, containing Write attribute record
   @note buffer data by data_ptr should contain Write attribute request payload, without ZCL header.
 */
#define ZB_ZCL_GENERAL_GET_NEXT_WRITE_ATTR_REQ(data_ptr, data_len, write_attr_req)                 \
  {                                                                                                \
    zb_uint16_t req_size = ZB_UINT16_MAX;                                                          \
    (write_attr_req) = (data_len) >= ZB_ZCL_WRITE_ATTR_REQ_SIZE                                    \
                           ? (zb_zcl_write_attr_req_t *)(void *)(data_ptr)                         \
                           : NULL;                                                                 \
                                                                                                   \
    if ((write_attr_req) != NULL)                                                                  \
    {                                                                                              \
      /* substruct sizeof(zb_uint8_t) because its size */                                          \
      /* is already included into ZB_ZCL_WRITE_ATTR_REQ_SIZE */                                    \
      ZB_ASSERT_COMPILE_TIME(ZB_ZCL_WRITE_ATTR_REQ_SIZE <= ZB_UINT8_MAX);                                 \
      req_size = (zb_uint8_t)ZB_ZCL_WRITE_ATTR_REQ_SIZE - (zb_uint8_t)sizeof(zb_uint8_t)           \
                 + zb_zcl_get_attribute_size((write_attr_req)->attr_type,                          \
                                             (write_attr_req)->attr_value);                        \
                                                                                                   \
      if (req_size <= (data_len))                                                                  \
      {                                                                                            \
        ZB_ZCL_HTOLE16_INPLACE(&(write_attr_req)->attr_id);                                        \
        ZB_ZCL_FIX_ENDIAN((write_attr_req)->attr_value, (write_attr_req)->attr_type);              \
      }                                                                                            \
    }                                                                                              \
                                                                                                   \
    if (req_size <= (data_len))                                                                    \
    {                                                                                              \
      (data_ptr) = (data_ptr) + req_size;                                                          \
      (data_len) = (data_len)-req_size;                                                            \
    }                                                                                              \
    else                                                                                           \
    {                                                                                              \
      (write_attr_req) = NULL;                                                                     \
    }                                                                                              \
  }

/*! @brief ZCL Write Attribute Command frame
    @see ZCL spec, 2.4.3 Write Attributes Command
*/
typedef ZB_PACKED_PRE struct zb_zcl_write_attr_res_s
{
  zb_uint8_t status;         /*!< Write attribute status */
  zb_uint16_t attr_id;       /*!< Attribute ID */
} ZB_PACKED_STRUCT
zb_zcl_write_attr_res_t;

/** @cond internals_doc */

/** Minimal size of write attribute response */
#define ZB_ZCL_WRITE_ATTR_RES_SIZE sizeof(zb_uint8_t)

/** @endcond */ /* internals_doc */

/** @brief Parses Write attribute response and returns next Write attribute status or NULL if there
    is no more data.

   If response contains invalid data, NULL is returned.
   @param data_buf - ID zb_bufid_t of a buffer containing write attribute response data
   @param write_attr_resp - out pointer to zb_zcl_write_attr_res_t, containing Write attribute status
   @note data_buf buffer should contain Write attribute response payload, without ZCL header.  Each
   parsed Write attribute response is extracted from initial data_buf buffer
 */
#define ZB_ZCL_GET_NEXT_WRITE_ATTR_RES(data_buf, write_attr_resp)          \
{                                                                          \
  zb_uint16_t res_size;                                                    \
  (write_attr_resp) = zb_buf_len(data_buf) >= ZB_ZCL_WRITE_ATTR_RES_SIZE ? \
    (zb_zcl_write_attr_res_t*)zb_buf_begin(data_buf) : NULL;               \
                                                                           \
  if (write_attr_resp)                                                     \
  {                                                                        \
    (write_attr_resp)->status =                                            \
      zb_zcl_zcl8_statuses_conversion((write_attr_resp)->status);          \
    if ((write_attr_resp)->status != ZB_ZCL_STATUS_SUCCESS)                \
    {                                                                      \
      /* In case of error, attribute id is reported */                     \
      res_size = sizeof(zb_zcl_write_attr_res_t);                          \
      if (res_size <= zb_buf_len(data_buf))                                \
      {                                                                    \
        ZB_ZCL_HTOLE16_INPLACE(&(write_attr_resp)->attr_id);               \
      }                                                                    \
    }                                                                      \
    else                                                                   \
    {                                                                      \
      res_size = ZB_ZCL_WRITE_ATTR_RES_SIZE;                               \
    }                                                                      \
                                                                           \
  if (res_size <= zb_buf_len(data_buf))                                    \
  {                                                                        \
    (void)zb_buf_cut_left((data_buf), res_size);                           \
  }                                                                        \
  else                                                                     \
  {                                                                        \
    (write_attr_resp) = NULL;                                              \
  }                                                                        \
  }                                                                        \
}

/** @brief Initialize Write attribute command
    @param buffer - buffer to store command data
    @param cmd_ptr - pointer to a command data memory
    @param def_resp - enable/disable default response
    @param write_attr_type - type of 'Write Attribute' command:
           default - @see ZB_ZCL_CMD_WRITE_ATTRIB;
           no response - @see ZB_ZCL_CMD_WRITE_ATTRIB_NO_RESP;
           undivided - @see ZB_ZCL_CMD_WRITE_ATTRIB_UNDIV;
*/
#define ZB_ZCL_GENERAL_INIT_WRITE_ATTR_REQ_BY_TYPE(buffer, cmd_ptr, def_resp, write_attr_type) \
{                                                                                          \
  cmd_ptr = ZB_ZCL_START_PACKET(buffer);                                                   \
  ZB_ZCL_CONSTRUCT_GENERAL_COMMAND_REQ_FRAME_CONTROL(cmd_ptr, def_resp);                   \
  ZB_ZCL_CONSTRUCT_COMMAND_HEADER(cmd_ptr, ZB_ZCL_GET_SEQ_NUM(), write_attr_type);             \
}

/** @brief Initialize Write attribute command
    @param buffer - buffer to store command data
    @param cmd_ptr - pointer to a command data memory
    @param def_resp - enable/disable default response
*/
#define ZB_ZCL_GENERAL_INIT_WRITE_ATTR_REQ(buffer, cmd_ptr, def_resp) \
  ZB_ZCL_GENERAL_INIT_WRITE_ATTR_REQ_BY_TYPE( \
    (buffer), (cmd_ptr), (def_resp), ZB_ZCL_CMD_WRITE_ATTRIB);

/** @brief Initialize Write attribute command
    @param buffer - buffer to store command data
    @param cmd_ptr - pointer to a command data memory
    @param direction - direction of command (see @ref zcl_frame_direction)
    @param def_resp - enable/disable default response
    @param manuf_code - manufacturer specific code
*/
#define ZB_ZCL_GENERAL_INIT_WRITE_ATTR_REQ_MANUF(buffer, cmd_ptr, direction, def_resp, manuf_code) \
{                                                                                                  \
  cmd_ptr = ZB_ZCL_START_PACKET(buffer);                                                           \
  ZB_ZCL_CONSTRUCT_GENERAL_COMMAND_REQ_FRAME_CONTROL_A(                                            \
    cmd_ptr, direction, ZB_ZCL_MANUFACTURER_SPECIFIC, def_resp);                                   \
  ZB_ZCL_CONSTRUCT_COMMAND_HEADER_EXT(                                                             \
    cmd_ptr, ZB_ZCL_GET_SEQ_NUM(), ZB_TRUE, manuf_code, ZB_ZCL_CMD_WRITE_ATTRIB);                  \
}

/** @brief Initialize Write Attribute No Response command
    @param buffer - buffer to store command data
    @param cmd_ptr - pointer to a command data memory
    @param def_resp - enable/disable default response
*/
#define ZB_ZCL_GENERAL_INIT_WRITE_ATTR_REQ_NO_RESP(buffer, cmd_ptr, def_resp) \
  ZB_ZCL_GENERAL_INIT_WRITE_ATTR_REQ_BY_TYPE( \
    (buffer), (cmd_ptr), (def_resp), ZB_ZCL_CMD_WRITE_ATTRIB_NO_RESP);

/** @brief Initialize Write Attribute Undivided command
    @param buffer - buffer to store command data
    @param cmd_ptr - pointer to a command data memory
    @param def_resp - enable/disable default response
*/
#define ZB_ZCL_GENERAL_INIT_WRITE_ATTR_REQ_UNDIV(buffer, cmd_ptr, def_resp) \
  ZB_ZCL_GENERAL_INIT_WRITE_ATTR_REQ_BY_TYPE( \
    (buffer), (cmd_ptr), (def_resp), ZB_ZCL_CMD_WRITE_ATTRIB_UNDIV);

/**
 * @brief Initialize Write attribute command
 *
 * @param buffer - buffer to store command data
 * @param cmd_ptr - pointer to a command data memory
 * @param direction - direction of command (see @ref zcl_frame_direction)
 * @param def_resp - enable/disable default response
 */
#define ZB_ZCL_GENERAL_INIT_WRITE_ATTR_REQ_A(buffer, cmd_ptr, direction, def_resp)         \
{                                                                                          \
  cmd_ptr = ZB_ZCL_START_PACKET(buffer);                                                   \
  ZB_ZCL_CONSTRUCT_GENERAL_COMMAND_REQ_FRAME_CONTROL_A(cmd_ptr, direction, ZB_ZCL_NOT_MANUFACTURER_SPECIFIC, def_resp);      \
  ZB_ZCL_CONSTRUCT_COMMAND_HEADER(cmd_ptr, ZB_ZCL_GET_SEQ_NUM(), ZB_ZCL_CMD_WRITE_ATTRIB); \
}

/** @brief Add attribute value to command payload
   @param cmd_ptr - pointer to a command data memory
   @param attr_id - attribute identifier
   @param attr_type - attribute type
   @param attr_val - pointer to attribute data value
*/
#define ZB_ZCL_GENERAL_ADD_VALUE_WRITE_ATTR_REQ(cmd_ptr, attr_id, attr_type, attr_val) \
{                                                                                      \
  ZB_ZCL_PACKET_PUT_DATA16_VAL(cmd_ptr, (attr_id));                                    \
  ZB_ZCL_PACKET_PUT_DATA8(cmd_ptr, (attr_type));                                       \
  (cmd_ptr) = zb_zcl_put_value_to_packet(cmd_ptr, attr_type, attr_val);                \
}

/** @brief Send Write attribute command
    @param buffer - buffer to store command data
    @param cmd_ptr - pointer to a command data memory
    @param addr - address to send packet to
    @param dst_addr_mode - addressing mode
    @param dst_ep - destination endpoint
    @param ep - sending endpoint
    @param profile_id - profile identifier
    @param cluster_id - cluster identifier
    @param cb - callback for getting command send status
*/
#define ZB_ZCL_GENERAL_SEND_WRITE_ATTR_REQ(                                                       \
    buffer, cmd_ptr, addr, dst_addr_mode, dst_ep, ep, profile_id, cluster_id, cb)                 \
{                                                                                                 \
  ZB_ZCL_FINISH_PACKET(buffer, cmd_ptr)                                                          \
  ZB_ZCL_SEND_COMMAND_SHORT(buffer, addr, dst_addr_mode, dst_ep, ep, profile_id, cluster_id, cb); \
}


/** @brief Send "write attributes" request.
 * deprecate */
#define ZB_ZCL_GENERAL_SEND_WRITE_ATTRS_REQ(                                             \
    buffer, addr, addr_mode, dst_ep, ep, prof_id, cluster_id)                            \
  ZB_ZCL_SEND_GENERAL_COMMAND_REQ_SHORT(                                                 \
      buffer, addr, addr_mode, dst_ep, ep, prof_id, cluster_id, ZB_ZCL_CMD_WRITE_ATTRIB)


/** @brief Initialize Write attribute response command
    @param _buffer - buffer to store command data
    @param _cmd_ptr - pointer to a command data memory
    @param _seq - command sequence
*/
#define ZB_ZCL_GENERAL_INIT_WRITE_ATTR_RESP(_buffer, _cmd_ptr, _seq)                    \
{                                                                                       \
  cmd_ptr = ZB_ZCL_START_PACKET((_buffer));                                             \
  ZB_ZCL_CONSTRUCT_GENERAL_COMMAND_RESP_FRAME_CONTROL((_cmd_ptr));                      \
  ZB_ZCL_CONSTRUCT_COMMAND_HEADER((_cmd_ptr), (_seq), ZB_ZCL_CMD_WRITE_ATTRIB_RESP);    \
}

/**
 * @brief Initialize Write attribute response command
 *
 * @param _buffer - buffer to store command data
 * @param _cmd_ptr - pointer to a command data memory
 * @param _direction - direction of command (see @ref zcl_frame_direction)
 * @param _seq - command sequence
 * @param _is_manuf - whether command is manufacturer specific
 * @param _manuf_id - manufacturer ID (needed if _is_manuf is set)
 */
#define ZB_ZCL_GENERAL_INIT_WRITE_ATTR_RESP_EXT(_buffer, _cmd_ptr, _direction, _seq, _is_manuf, _manuf_id)      \
{                                                                                                               \
  cmd_ptr = ZB_ZCL_START_PACKET((_buffer));                                                                     \
  ZB_ZCL_CONSTRUCT_GENERAL_COMMAND_RESP_FRAME_CONTROL_A((_cmd_ptr), (_direction), (_is_manuf));                 \
  ZB_ZCL_CONSTRUCT_COMMAND_HEADER_EXT((_cmd_ptr), (_seq), (_is_manuf), (_manuf_id), ZB_ZCL_CMD_WRITE_ATTRIB_RESP);      \
}

/** @brief Add Success status value to Write attribute response command payload
   @param _cmd_ptr - pointer to a command data memory
*/
#define ZB_ZCL_GENERAL_SUCCESS_WRITE_ATTR_RESP(_cmd_ptr)        \
{                                                               \
  ZB_ZCL_PACKET_PUT_DATA8((_cmd_ptr), ZB_ZCL_STATUS_SUCCESS);   \
}


/** @brief Send Write attribute response command
    @param _buffer - buffer to store command data
    @param _cmd_ptr - pointer to a command data memory
    @param _addr - address to send packet to
    @param _dst_addr_mode - addressing mode
    @param _dst_ep - destination endpoint
    @param _ep - sending endpoint
    @param _profile_id - profile identifier
    @param _cluster_id - cluster identifier
    @param _cb - callback for getting command send status
*/
#define ZB_ZCL_GENERAL_SEND_WRITE_ATTR_RESP(                                                     \
    _buffer, _cmd_ptr, _addr, _dst_addr_mode, _dst_ep, _ep, _profile_id, _cluster_id, _cb)      \
{                                                                                               \
  ZB_ZCL_FINISH_PACKET((_buffer), (_cmd_ptr))                                                  \
  ZB_ZCL_SEND_COMMAND_SHORT((_buffer), (_addr), (_dst_addr_mode), (_dst_ep), (_ep),             \
                (_profile_id), (_cluster_id), (_cb));                                           \
}


/** @} */ /* Write attributes command sending and parsing. */

/*************** Discover attribute command definitions ************************/

/**
 *  @defgroup disc_attr_command Discover attributes request and response sending and parsing.
 *  @{
 *    @details
 *    Discover attributes command described in ZCL spec, subclauses 2.4.13 and 2.4.14.
 *
 *    Read attributes request command can be formed and sent as in following snippet:
 *    @code
 *    ZB_ZCL_GENERAL_DISC_ATTR_REQ(zcl_cmd_buf, cmd_ptr, ZB_ZCL_ENABLE_DEFAULT_RESPONSE,
 *                      ZB_ZCL_ATTR_BINARY_INPUT_PRESENT_VALUE_ID, 5,
 *                       DUT_ADDR, DUT_ADDR_MODE, DUT_ENDPOINT,
 *                       TH_ENDPOINT, ZB_AF_HA_PROFILE_ID, ZB_ZCL_CLUSTER_ID_BINARY_INPUT, NULL);
 *    @endcode
 *    Discover attributes response can be parsed as:
 *    @code
 *    ZB_ZCL_GENERAL_GET_COMPLETE_DISC_RES(buf, complete);
 *    ZB_ZCL_GENERAL_GET_NEXT_DISC_ATTR_RES(buf, disc_attr_info);
 *    @endcode
 *    until allocated buffer space exceeds.
 *
 *    Discover attributes request parsing and response filling and sending is implemented in ZCL
 *    library internal functions.
 */

/*! @brief ZCL Discover Attribute Command frame
    @see ZCL spec, zb_zcl_disc_attr 2.4.13 Discover Attributes Command
*/
typedef ZB_PACKED_PRE struct zb_zcl_disc_attr_req_s
{
  zb_uint16_t start_attr_id; /*!< Start attribute identifier */
  zb_uint8_t maximum;        /*!< Maximum attribute identifiers */
} ZB_PACKED_STRUCT
zb_zcl_disc_attr_req_t;

/*! @brief ZCL Description for Discover Attribute Response frame
    @see ZCL spec, zb_zcl_read_attr 2.4.14 Discover Attribute Response
*/
typedef ZB_PACKED_PRE struct zb_zcl_disc_attr_info_s
{
  zb_uint16_t attr_id;  /*!< Attribute identifier */
  zb_uint8_t data_type; /*!< Attribute data type */
} ZB_PACKED_STRUCT
zb_zcl_disc_attr_info_t;


/*! @brief ZCL Discover Attribute Response frame
    @see ZCL spec, zb_zcl_read_attr 2.4.15 Discover Attribute Response
    @note Command frame contains variable number of parameters
*/
typedef ZB_PACKED_PRE struct zb_zcl_disc_attr_res_s
{
  zb_uint8_t complete;       /*!< Discovery complete */
  zb_zcl_disc_attr_info_t info[1]; /*!< Attribute desc list */
} ZB_PACKED_STRUCT
zb_zcl_disc_attr_res_t;

/**
 * @name ZCL Discover Attribute Response - complete field
 * @anchor zcl_disc_complete
 * @see ZCL spec, clause 2.4.14
 */
/** @{ */
#define  ZB_ZCL_DISC_NON_COMPLETE  0x00U /*!< more attributes to be discovered */
#define  ZB_ZCL_DISC_COMPLETE      0x01U /*!< no more attributes to be discovered */
/** @} */

/**
 * @brief Type for possible values of ZCL Discover Attribute Response.
 *
 * @deprecated holds one of @ref zcl_disc_complete. Kept only for backward
 * compatibility as @ref zcl_disc_complete were declared previously as enum. Can be
 * removed in future releases.
 */
typedef zb_uint8_t zb_zcl_disc_complete_t;

/** @cond internals_doc */

#define ZB_ZCL_DISC_ATTR_RESP_SIZE      sizeof(zb_zcl_disc_attr_info_t)

/** @endcond */ /* internals_doc */

/** @brief Parses Discovery attribute response and returns next Read attribute status record or NULL if
    there is no more data.

   If response contains invalid data, 0 is returned.
   @param data_buf -  ID zb_bufid_t of a buffer containing discover attribute response data
   @param complete - variable to store Complete field
   @note data_buf buffer should contain Discover attribute response payload, without ZCL header.
 */
#define ZB_ZCL_GENERAL_GET_COMPLETE_DISC_RES(data_buf, complete)        \
{                                                                       \
  if (zb_buf_len(data_buf) > 0)                                        \
  {                                                                     \
    complete = *(zb_uint8_t*)zb_buf_begin(data_buf);                    \
    (void)zb_buf_cut_left((data_buf), sizeof(zb_uint8_t));              \
  }                                                                     \
  else                                                                  \
  {                                                                     \
    (complete) = 0;                                                     \
  }                                                                     \
}

/** @brief Discovery attribute response and returns next Discovery attribute status record or NULL if
    there is no more data.

   If response contains invalid data, NULL is returned.
   @param data_buf - ID zb_bufid_t of a buffer containing part of Discover attribute response data
   @param disc_attr_info - out pointer to zb_zcl_disc_attr_info_t, containing Discover attribute status
   record
   @note data_buf buffer should contain Discover attribute response payload, without ZCL header.  Each
   parsed Discover attribute status record is extracted from initial data_buf buffer
 */
#define ZB_ZCL_GENERAL_GET_NEXT_DISC_ATTR_RES(data_buf, disc_attr_info)                 \
{                                                                                       \
  (disc_attr_info) = zb_buf_len(data_buf) >= ZB_ZCL_DISC_ATTR_RESP_SIZE ?               \
    (zb_zcl_disc_attr_info_t*)zb_buf_begin(data_buf) : (zb_zcl_disc_attr_info_t*)0;     \
                                                                                        \
  if (disc_attr_info)                                                                   \
  {                                                                                     \
    ZB_ZCL_HTOLE16_INPLACE(&(disc_attr_info)->attr_id);                                 \
                                                                                        \
    (void)zb_buf_cut_left((data_buf), ZB_ZCL_DISC_ATTR_RESP_SIZE);                      \
  }                                                                                     \
}

/** @brief Discover attribute command
    @param buffer - reference to buffer to put packet into
    @param def_resp - enable/disable default response
    @param start_attr_id - start attribute ID
    @param max_len - max count
    @param addr - address to send packet to
    @param dst_addr_mode - addressing mode
    @param dst_ep - destination endpoint
    @param ep - sending endpoint
    @param profile_id - profile identifier
    @param cluster_id - cluster identifier
    @param cb - callback for getting command send status
*/
#define ZB_ZCL_GENERAL_DISC_READ_ATTR_REQ(buffer, def_resp, start_attr_id, max_len,       \
                            addr, dst_addr_mode, dst_ep, ep, profile_id, cluster_id, cb)  \
{                                                                                         \
  zb_uint8_t *cmd_ptr = ZB_ZCL_START_PACKET(buffer);                                      \
  ZB_ZCL_CONSTRUCT_GENERAL_COMMAND_REQ_FRAME_CONTROL(cmd_ptr, def_resp);                  \
  ZB_ZCL_CONSTRUCT_COMMAND_HEADER(cmd_ptr, ZB_ZCL_GET_SEQ_NUM(), ZB_ZCL_CMD_DISC_ATTRIB); \
  ZB_ZCL_PACKET_PUT_DATA16_VAL(cmd_ptr, (start_attr_id));                                 \
  ZB_ZCL_PACKET_PUT_DATA8(cmd_ptr, (max_len));                                            \
  ZB_ZCL_FINISH_PACKET(buffer, cmd_ptr)                                                  \
  ZB_ZCL_SEND_COMMAND_SHORT(buffer, addr, dst_addr_mode, dst_ep, ep, profile_id, cluster_id, cb); \
}

/**
 * @brief Discover Attribute Request
 * @param buffer - reference to buffer to put packet into
 * @param cmd_ptr - pointer to command (not used)
 * @param direction - direction of command (see @ref zcl_frame_direction)
 * @param def_resp - enable/disable default response
 * @param start_attr_id - start attribute ID
 * @param max_len - max count
 * @param addr - address to send packet to
 * @param dst_addr_mode - addressing mode
 * @param dst_ep - destination endpoint
 * @param ep - sending endpoint
 * @param profile_id - profile identifier
 * @param cluster_id - cluster identifier
 * @param cb - callback for getting command send status
 */
#define ZB_ZCL_GENERAL_DISC_ATTR_REQ_A(buffer, cmd_ptr, direction, def_resp,              \
                            start_attr_id, max_len,                                       \
                            addr, dst_addr_mode, dst_ep, ep, profile_id, cluster_id, cb)  \
{                                                                                         \
  zb_uint8_t *cmd_ptr = ZB_ZCL_START_PACKET(buffer);                                      \
  ZB_ZCL_CONSTRUCT_GENERAL_COMMAND_REQ_FRAME_CONTROL_A(cmd_ptr, direction,                \
                                          ZB_ZCL_NOT_MANUFACTURER_SPECIFIC, def_resp);    \
  ZB_ZCL_CONSTRUCT_COMMAND_HEADER(cmd_ptr, ZB_ZCL_GET_SEQ_NUM(), ZB_ZCL_CMD_DISC_ATTRIB); \
  ZB_ZCL_PACKET_PUT_DATA16_VAL(cmd_ptr, (start_attr_id));                                 \
  ZB_ZCL_PACKET_PUT_DATA8(cmd_ptr, (max_len));                                            \
  ZB_ZCL_FINISH_PACKET(buffer, cmd_ptr)                                                  \
  ZB_ZCL_SEND_COMMAND_SHORT(buffer, addr, dst_addr_mode, dst_ep, ep, profile_id, cluster_id, cb); \
}

/**
 * @brief Discover Attribute Request
 * @param buffer - reference to buffer to put packet into
 * @param cmd_ptr - pointer to command (not used)
 * @param direction - direction of command (see @ref zcl_frame_direction)
 * @param def_resp - enable/disable default response
 * @param manuf_code - manufacturer specific code
 * @param start_attr_id - start attribute ID
 * @param max_len - max count
 * @param addr - address to send packet to
 * @param dst_addr_mode - addressing mode
 * @param dst_ep - destination endpoint
 * @param ep - sending endpoint
 * @param profile_id - profile identifier
 * @param cluster_id - cluster identifier
 * @param cb - callback for getting command send status
 */
#define ZB_ZCL_GENERAL_DISC_ATTR_REQ_MANUF(buffer, cmd_ptr, direction, def_resp,          \
                                           manuf_code, start_attr_id, max_len,            \
                                           addr, dst_addr_mode, dst_ep, ep,               \
                                           profile_id, cluster_id, cb)                    \
{                                                                                         \
  zb_uint8_t *cmd_ptr = ZB_ZCL_START_PACKET(buffer);                                      \
  ZB_ZCL_CONSTRUCT_GENERAL_COMMAND_REQ_FRAME_CONTROL_A(cmd_ptr, direction,                \
                                                       ZB_ZCL_MANUFACTURER_SPECIFIC, def_resp); \
  ZB_ZCL_CONSTRUCT_COMMAND_HEADER_EXT(                                                    \
    cmd_ptr, ZB_ZCL_GET_SEQ_NUM(),                                                        \
    ZB_ZCL_MANUFACTURER_SPECIFIC, (manuf_code), ZB_ZCL_CMD_DISC_ATTRIB);                  \
  ZB_ZCL_PACKET_PUT_DATA16_VAL(cmd_ptr, (start_attr_id));                                 \
  ZB_ZCL_PACKET_PUT_DATA8(cmd_ptr, (max_len));                                            \
  ZB_ZCL_FINISH_PACKET(buffer, cmd_ptr)                                                   \
  ZB_ZCL_SEND_COMMAND_SHORT(buffer, addr, dst_addr_mode, dst_ep, ep, profile_id, cluster_id, cb); \
}

/** @} */ /* Discover attribute request and response sending and parsing. */



/********************** Configure reporting command definitions ***************************/

#if !(defined ZB_ZCL_DISABLE_REPORTING) || defined(DOXYGEN)

/**
 *  @defgroup cfg_reporting_cmd Configure reporting command sending and parsing
 *  @{
 *    @details
 *    Most of actions related to the attribute reporting configuration are implemented in ZCL
 *    internals.
 *
 *    As described in ZCL spec, subclause 2.4.7, Configure Reporting command has two forms:
 *    @li client to server - instructs server to configure for attribute reporting.
 *    @li server to client - notifies client that server has configured for attribute reporting,
 *    and will report attribute values to the client.
 *
 *    Request to configure server for attribute reporting can be filled like in the snippet below:
 *    @snippet simple_gw/simple_gw.c zcl_general_fill_configure_report
 *    Other variant of the command can be filled in a similar way with @ref
 *    ZB_ZCL_GENERAL_INIT_CONFIGURE_REPORTING_CLI_REQ() and @ref
 *    ZB_ZCL_GENERAL_ADD_RECV_REPORT_CONFIGURE_REPORTING_REQ() macros, and scheduled for sending
 *    with ZB_ZCL_GENERAL_SEND_CONFIGURE_REPORTING_REQ() macro.
 *
 *    Configure reporting request record can be parsed as:
 *    @code
 *    zb_bufid_t  buffer = pointer_to_the_packet_buffer;
 *    zb_zcl_configure_reporting_req_t* req_record;
 *    ...
 *    do
 *    {
 *      ZB_ZCL_GENERAL_GET_NEXT_CONFIGURE_REPORTING_REQ(buffer, req_record);
 *      if (! req_record)
 *      {
 *        break;
 *      }
 *      process_request_record(req_record);
 *    } while (1);
 *    @endcode
 *
 *    Configure Reporting response command will be generated automatically by ZCL internals.
 *    Response record to the Configure Reporting command can be parsed as:
 *    @code
 *    ZB_ZCL_GENERAL_GET_NEXT_CONFIGURE_REPORTING_RES(buf, config_res);
 *    @endcode
 *    If there are several Configure Reporting response records, they could be processed cyclically
 *    in the same manner as Configure Reporting request ones.
 *
 */

    /** u.clnt: as usual, cluster with client role sends this request
    to a server to configure reporting: how attribute should be reported by a server */
typedef ZB_PACKED_PRE struct zb_zcl_configure_reporting_req_clnt_s
    {
      zb_uint8_t attr_type;     /*!< Attribute data type */
      zb_uint16_t min_interval; /*!< Minimum reporting interval */
      zb_uint16_t max_interval; /*!< Maximum reporting interval */
      zb_uint8_t delta[1];      /*!< Reportable change */
    } ZB_PACKED_STRUCT
zb_zcl_configure_reporting_req_clnt_t;

    /** u.srv: as usual, cluster with server role sends this request
    to a client, to inform him how an attribute will be reported by a server */
typedef ZB_PACKED_PRE struct zb_zcl_configure_reporting_req_srv_s
    {
      zb_uint16_t timeout; /*!< Timeout period */
    } ZB_PACKED_STRUCT
zb_zcl_configure_reporting_req_srv_t;

/**
 *  @brief General type for Configure Reporting Request command.

 *  see @ref zb_zcl_configure_reporting_req_srv_t
 *  see @ref zb_zcl_configure_reporting_req_clnt_t
 */
typedef ZB_PACKED_PRE union zb_zcl_configure_reporting_req_u_s
{
  zb_zcl_configure_reporting_req_clnt_t clnt; /*!< Parameters for client */
  zb_zcl_configure_reporting_req_srv_t srv;   /*!< Parameters for server */
} ZB_PACKED_STRUCT
zb_zcl_configure_reporting_req_u_t;

/** @brief One chunk of Configure reporting command request
  *
  * Attribute reporting configuration record
  */
/* WARNING: Do not put directly packed_struct declaration inside another packet_struct - some
 * compilers does not handle it correctly! */
typedef ZB_PACKED_PRE struct zb_zcl_configure_reporting_req_s
{
  zb_uint8_t direction;  /*!< Direction */
  zb_uint16_t attr_id;   /*!< Attribute ID */
  zb_zcl_configure_reporting_req_u_t u; /*!< Request fields */
} ZB_PACKED_STRUCT zb_zcl_configure_reporting_req_t;

/*! Configure reporting command, direction field values */
typedef enum zb_zcl_configure_reporting_direction_value_e
{
  ZB_ZCL_CONFIGURE_REPORTING_SEND_REPORT = 0x00, /**< Report should be send by a cluster. */
  ZB_ZCL_CONFIGURE_REPORTING_RECV_REPORT = 0x01  /**< Report should be received by a cluster. */
}
zb_zcl_configure_reporting_direction_value_t;

/** @cond internals_doc */

/* client configuration size is larger then srv version, can take
 * sizeof(struct); reduce by sizeof(zb_uint8_t) because delta maybe omitted */
#define ZB_ZCL_CONFIGURE_REPORTING_FOR_SEND_SIZE                    \
    (sizeof(zb_zcl_configure_reporting_req_t) - sizeof(zb_uint8_t))

/* calculate size for srv command version: direction, attr_id, timeout */
#define ZB_ZCL_CONFIGURE_REPORTING_FOR_RECV_SIZE sizeof(zb_uint8_t) + sizeof(zb_uint16_t)*2

/** @endcond */ /* internals_doc */

/** @brief Parses Configure reporting command request and returns next Attribute reporting
    configuration record or NULL if there is no more data.

   If request contains invalid data, NULL is returned.
   @param data_buf -  ID zb_bufid_t of a buffer containing Parses Configure reporting command data
   @param config_rep_req - out pointer to zb_zcl_configure_reporting_req_t, containing Attribute
   reporting configuration record
   @note data_buf buffer should contain Configure reporting command payload, without ZCL header.
   Each parsed Attribute reporting configuration record is extracted from initial data_buf buffer
 */
#define ZB_ZCL_GENERAL_GET_NEXT_CONFIGURE_REPORTING_REQ(data_buf, config_rep_req)                  \
{                                                                                                  \
  zb_uint16_t res_size = 0xffff;                                                                   \
  /* ZB_ZCL_CONFIGURE_REPORTING_FOR_SEND_SIZE - is minimum payload length */                       \
  (config_rep_req) = zb_buf_len(data_buf) >= ZB_ZCL_CONFIGURE_REPORTING_FOR_RECV_SIZE ?            \
    (zb_zcl_configure_reporting_req_t *)zb_buf_begin(data_buf) : NULL;                             \
                                                                                                   \
  if (config_rep_req)                                                                              \
  {                                                                                                \
    ZB_ZCL_HTOLE16_INPLACE(&(config_rep_req)->attr_id);                                            \
    if ((config_rep_req)->direction == ZB_ZCL_CONFIGURE_REPORTING_RECV_REPORT)                     \
    {                                                                                              \
      res_size = ZB_ZCL_CONFIGURE_REPORTING_FOR_RECV_SIZE;                                         \
      ZB_ZCL_HTOLE16_INPLACE(&(config_rep_req)->u.srv.timeout);                                    \
    }                                                                                              \
    else /* ZB_ZCL_CONFIGURE_REPORTING_SEND_REPORT */                                              \
    {                                                                                              \
      res_size = ZB_ZCL_CONFIGURE_REPORTING_FOR_SEND_SIZE;                                         \
      ZB_ZCL_HTOLE16_INPLACE(&(config_rep_req)->u.clnt.min_interval);                              \
      ZB_ZCL_HTOLE16_INPLACE(&(config_rep_req)->u.clnt.max_interval);                              \
      if (zb_zcl_is_analog_data_type((config_rep_req)->u.clnt.attr_type))                          \
      {                                                                                            \
        res_size += zb_zcl_get_analog_attribute_size((config_rep_req)->u.clnt.attr_type);          \
        if (res_size <= zb_buf_len(data_buf))                                                      \
        {                                                                                          \
          ZB_ZCL_FIX_ENDIAN((config_rep_req)->u.clnt.delta, (config_rep_req)->u.clnt.attr_type);   \
        }                                                                                          \
      }                                                                                            \
    } /* if ZB_ZCL_CONFIGURE_REPORTING_SEND_REPORT */                                              \
  } /* if config_rep_req */                                                                        \
                                                                                                   \
  if (res_size <= zb_buf_len(data_buf))                                                            \
  {                                                                                                \
    (void)zb_buf_cut_left((data_buf), res_size);                                                   \
  }                                                                                                \
  else                                                                                             \
  {                                                                                                \
    (config_rep_req) = NULL;                                                                       \
  }                                                                                                \
}

/** One chunk of Configure reporting response command
  *
  * Attribute status record
  */
typedef ZB_PACKED_PRE struct zb_zcl_configure_reporting_res_s
{
  zb_uint8_t status;    /*!< Configure reporting status */
  zb_uint8_t direction; /*!< Direction */
  zb_uint16_t attr_id;  /*!< Attribute ID */
} ZB_PACKED_STRUCT
zb_zcl_configure_reporting_res_t;

/** @cond internals_doc */

/*! Minimum size of zb_zcl_configure_reporting_res_t */
#define ZB_ZCL_CONFIGURE_REPORTING_RES_SIZE sizeof(zb_uint8_t)

/** @endcond */ /* internals_doc */

/** @brief Parses Configure reporting response and returns next
    configure attribute status record or NULL if there is no more data.

    If response contains invalid data, NULL is returned.
    @param data_buf -  ID zb_bufid_t of a buffer containing Configure reporting response data
    @param config_rep_res - out pointer to zb_zcl_configure_reporting_res_t, containing Configure
    attribute status record
    @note data_buf buffer should contain Configure reporting response payload, without ZCL header.
    Each parsed Configure attribute status record is extracted from initial data_buf buffer
 */
#define ZB_ZCL_GENERAL_GET_NEXT_CONFIGURE_REPORTING_RES(data_buf, config_rep_res)   \
{                                                                                   \
  zb_uint16_t res_size = 0;                                                         \
  (config_rep_res) = zb_buf_len(data_buf) >= ZB_ZCL_CONFIGURE_REPORTING_RES_SIZE ?  \
    (zb_zcl_configure_reporting_res_t*)zb_buf_begin(data_buf) : NULL;               \
                                                                                    \
  if (config_rep_res)                                                               \
  {                                                                                 \
    (config_rep_res)->status =                                                      \
      zb_zcl_zcl8_statuses_conversion((config_rep_res)->status);                    \
    if ((config_rep_res)->status != ZB_ZCL_STATUS_SUCCESS                           \
         && (config_rep_res)->status != ZB_ZCL_STATUS_MALFORMED_CMD)                \
    {                                                                               \
      /* In case of error, direction and attribute id is reported */                \
      res_size = sizeof(zb_zcl_configure_reporting_res_t);                          \
      if (res_size <= zb_buf_len(data_buf))                                         \
      {                                                                             \
        ZB_ZCL_HTOLE16_INPLACE(&(config_rep_res)->attr_id);                         \
      }                                                                             \
    }                                                                               \
    else                                                                            \
    {                                                                               \
      res_size = ZB_ZCL_CONFIGURE_REPORTING_RES_SIZE;                               \
    }                                                                               \
                                                                                    \
  if (res_size <= zb_buf_len(data_buf))                                             \
  {                                                                                 \
    (void)zb_buf_cut_left((data_buf), res_size);                                    \
  }                                                                                 \
  else                                                                              \
  {                                                                                 \
    (config_rep_res) = NULL;                                                        \
  }                                                                                 \
  }                                                                                 \
}

/*! @brief Initialize Configure reporting command (report send case)
    @param buffer to put packet to
    @param ptr - command buffer pointer
    @param def_resp - enable/disable default response
*/
#define ZB_ZCL_GENERAL_INIT_CONFIGURE_REPORTING_SRV_REQ(buffer, ptr, def_resp)          \
{                                                                                       \
  ptr = ZB_ZCL_START_PACKET(buffer);                                                    \
  ZB_ZCL_CONSTRUCT_GENERAL_COMMAND_REQ_FRAME_CONTROL_A(                                 \
      ptr, ZB_ZCL_FRAME_DIRECTION_TO_SRV, ZB_ZCL_NOT_MANUFACTURER_SPECIFIC, def_resp);  \
  ZB_ZCL_CONSTRUCT_COMMAND_HEADER(ptr, ZB_ZCL_GET_SEQ_NUM(), ZB_ZCL_CMD_CONFIG_REPORT); \
}

/*! @brief Initialize Configure reporting command (report send case)
    @param buffer to put packet to
    @param ptr - command buffer pointer
    @param def_resp - enable/disable default response
    @param manuf_code - manufacturer specific code
*/
#define ZB_ZCL_GENERAL_INIT_CONFIGURE_REPORTING_SRV_REQ_MANUF(buffer, ptr, def_resp, manuf_code) \
{                                                                                            \
  ptr = ZB_ZCL_START_PACKET(buffer);                                                         \
  ZB_ZCL_CONSTRUCT_GENERAL_COMMAND_REQ_FRAME_CONTROL_A(                                      \
      ptr, ZB_ZCL_FRAME_DIRECTION_TO_SRV, ZB_ZCL_MANUFACTURER_SPECIFIC, def_resp);           \
  ZB_ZCL_CONSTRUCT_COMMAND_HEADER_EXT(ptr, ZB_ZCL_GET_SEQ_NUM(), ZB_TRUE,                    \
                                      manuf_code, ZB_ZCL_CMD_CONFIG_REPORT);                 \
}

/*! @brief Initialize Configure reporting command (report receive case)
    @param buffer to put packet to
    @param ptr - command buffer pointer
    @param def_resp - enable/disable default response
*/
#define ZB_ZCL_GENERAL_INIT_CONFIGURE_REPORTING_CLI_REQ(buffer, ptr, def_resp)          \
{                                                                                       \
  ptr = ZB_ZCL_START_PACKET(buffer);                                                    \
  ZB_ZCL_CONSTRUCT_GENERAL_COMMAND_REQ_FRAME_CONTROL_A(                                 \
      ptr, ZB_ZCL_FRAME_DIRECTION_TO_CLI, ZB_ZCL_NOT_MANUFACTURER_SPECIFIC, def_resp);  \
  ZB_ZCL_CONSTRUCT_COMMAND_HEADER(ptr, ZB_ZCL_GET_SEQ_NUM(), ZB_ZCL_CMD_CONFIG_REPORT); \
}

/*! @brief Add attribute reporting configuration record to command payload (report send case)
    @param ptr - command buffer pointer
    @param attr_id - attribute identifier
    @param attr_type - type of the attribute
    @param min_interval - reporting minimum interval
    @param max_interval - reporting maximum interval
    @param report_change - reportable value change
*/
#define ZB_ZCL_GENERAL_ADD_SEND_REPORT_CONFIGURE_REPORTING_REQ(            \
    ptr, attr_id, attr_type, min_interval, max_interval, report_change)    \
{                                                                          \
  ZB_ZCL_PACKET_PUT_DATA8(ptr, ZB_ZCL_CONFIGURE_REPORTING_SEND_REPORT);    \
  ZB_ZCL_PACKET_PUT_DATA16_VAL(ptr, (attr_id));                            \
  ZB_ZCL_PACKET_PUT_DATA8(ptr, (attr_type));                               \
  ZB_ZCL_PACKET_PUT_DATA16_VAL(ptr, (min_interval));                       \
  ZB_ZCL_PACKET_PUT_DATA16_VAL(ptr, (max_interval));                       \
  if (zb_zcl_is_analog_data_type(attr_type))                               \
  {                                                                        \
    (ptr) = zb_zcl_put_value_to_packet((ptr), (attr_type), report_change); \
  }                                                                        \
}

/*! @brief Add attribute reporting configuration record to command payload (report receive case)
    @param ptr - command buffer pointer
    @param attr_id - attribute identifier
    @param timeout - reporting timeout
*/
#define ZB_ZCL_GENERAL_ADD_RECV_REPORT_CONFIGURE_REPORTING_REQ(ptr, attr_id, timeout) \
{                                                                                     \
  ZB_ZCL_PACKET_PUT_DATA8(ptr, ZB_ZCL_CONFIGURE_REPORTING_RECV_REPORT);               \
  ZB_ZCL_PACKET_PUT_DATA16_VAL(ptr, (attr_id));                                       \
  ZB_ZCL_PACKET_PUT_DATA16_VAL(ptr, (timeout));                                       \
}

/** @brief Sends Configure reporting command
    @param buffer to put data to
    @param ptr - pointer to the memory area to put data to
    @param addr - address to send packet to
    @param dst_addr_mode - addressing mode
    @param dst_ep - destination endpoint
    @param ep - sending endpoint
    @param prfl_id - profile identifier
    @param cluster_id - cluster identifier
    @param cb - callback for getting command send status
*/
#define ZB_ZCL_GENERAL_SEND_CONFIGURE_REPORTING_REQ(                                           \
    buffer, ptr, addr, dst_addr_mode, dst_ep, ep, prfl_id, cluster_id, cb)                     \
{                                                                                              \
  ZB_ZCL_FINISH_PACKET(buffer, ptr)                                                           \
  ZB_ZCL_SEND_COMMAND_SHORT(buffer, addr, dst_addr_mode, dst_ep, ep, prfl_id, cluster_id, cb); \
}

/** @} */ /* Configure reporting command sending and parsing. */

/************************** Report attribute command definitions **********************************/

/**
 *  @defgroup report_attr_cmd Report attribute command parsing
 *  @{
 *    @details
 *    Report attributes command originates from ZCL internals as a packet constructed according to
 *    ZCL spec, subclause 2.4.11. This command can be parsed by cyclical application of @ref
 *    ZB_ZCL_GENERAL_GET_NEXT_REPORT_ATTR_REQ() macro.
 *
 */

/** @cond internals_doc */

/** @brief Minimal size of Read attribute response, it should contain attribute id and status */
#define ZB_ZCL_REPORT_ATTR_CMD_SIZE sizeof(zb_zcl_report_attr_req_t)

/** @endcond */ /* internals_doc */

/*! @brief ZCL Report Attribute Command frame
    @see ZCL spec, 2.4.11.1 Report Attributes Command
    @note Report attribute command contains variable number of attribute reports,
    zb_zcl_report_attr_req_t defines one attribute report
*/
typedef ZB_PACKED_PRE struct zb_zcl_report_attr_req_s
{
  zb_uint16_t attr_id;       /*!< Attribute ID */
  zb_uint8_t  attr_type;     /*!< Attribute type */
  zb_uint8_t  attr_value[1]; /*!< Attribute value */
} ZB_PACKED_STRUCT
zb_zcl_report_attr_req_t;

/** @cond internals_doc */

/** @brief Minimum size of report attribute command */
#define ZB_ZCL_REPORT_ATTR_REQ_SIZE sizeof(zb_zcl_report_attr_req_t)

/** @endcond */ /* internals_doc */

/** @brief Parses Report attribute command and returns next Attribute
    report or NULL if there is no more data.

    If command contains invalid data, NULL is returned.
    @param data_buf -  ID zb_bufid_t of a buffer containing Report attribute command data
    @param rep_attr_req - out pointer to zb_zcl_report_attr_req_t, containing Attribute report
    @note data_buf buffer should contain Report attribute command payload, without ZCL header.
    Each parsed Attribute report attribute is extracted from initial data_buf buffer
 */
#define ZB_ZCL_GENERAL_GET_NEXT_REPORT_ATTR_REQ(data_buf, rep_attr_req)                     \
{                                                                                           \
  zb_uint16_t req_size = 0xffff;                                                            \
  (rep_attr_req) = zb_buf_len(data_buf) >= ZB_ZCL_REPORT_ATTR_REQ_SIZE ?                    \
    (zb_zcl_report_attr_req_t*)zb_buf_begin(data_buf) : NULL;                               \
                                                                                            \
  if (rep_attr_req)                                                                         \
  {                                                                                         \
    req_size = ZB_ZCL_REPORT_ATTR_REQ_SIZE;                                                 \
    ZB_ZCL_HTOLE16_INPLACE(&(rep_attr_req)->attr_id);                                       \
    /* Reduce req_size value by sizeof(zb_uint8_t) because it is                            \
     * already included into zb_zcl_report_attr_req_t */                                    \
    req_size +=                                                                             \
        zb_zcl_get_attribute_size((rep_attr_req)->attr_type, (rep_attr_req)->attr_value) -  \
        sizeof(zb_uint8_t);                                                                 \
    if (req_size <= zb_buf_len(data_buf))                                                   \
    {                                                                                       \
      ZB_ZCL_FIX_ENDIAN((rep_attr_req)->attr_value, (rep_attr_req)->attr_type);             \
    }                                                                                       \
                                                                                            \
    if (req_size <= zb_buf_len(data_buf))                                                   \
    {                                                                                       \
      (void)zb_buf_cut_left((data_buf), req_size);                                          \
    }                                                                                       \
    else                                                                                    \
    {                                                                                       \
      (rep_attr_req) = NULL;                                                                \
    }                                                                                       \
  }                                                                                         \
}

/** @} */ /* Report attribute command parsing. */

/************************** Read Reporting Configuration command definitions **********************************/

/**
 *  @defgroup read_reporting_cfg_cmd  Read reporting configuration command sending and parsing
 *  @{
 *    @details
 *    Most of actions related to the read attribute reporting configuration are implemented in ZCL
 *    internals.
 *    Read reporting configuration command is described in ZCL spec, subclause 2.4.9.
 */



/**Format of the Attribute Status Record Field
 *Figure 2.20 in ZCL spec.
 *NOTE: it can be various number of attribute status record fields in Read
 *reporting configuration request
 */
typedef ZB_PACKED_PRE struct zb_zcl_read_reporting_cfg_req_s
{
  zb_uint8_t direction;  /*!< The direction field specifies whether
                          *  values of the attribute are reported (0x00), or
                          *whether reports of the attribute are received (0x01).
                          */
  zb_uint16_t attr_id;   /*!< The attribute identifier field shall
                          *contain the identifier of the attribute whose
                          *reporting configuration details are to be read.
                          */
} ZB_PACKED_STRUCT
  zb_zcl_read_reporting_cfg_req_t;

/**  @brief Minimal size of Write attribute request, it will be more if attr_value size is more
  * than 1 byte
  */
#define ZB_ZCL_READ_REP_CFG_REQ_SIZE sizeof(zb_zcl_read_reporting_cfg_req_t)

/** @brief Parses read reporting configuration request and returns
  * next Read reporting configuration attribute record or NULL if there
  * is no more data.

   If request contains invalid data, NULL is returned.
   @param data_buf -  ID zb_bufid_t of a buffer containing read reporting configuration request data
   @param rep_cfg_req - out pointer to @ref zb_zcl_read_attr_req_t, containing read reporting configuration request
   @param rslt - returns TRUE if record exist and FALSE if not
   @note data_buf buffer should contain read reporting configuration request payload, without ZCL header.  Each
   parsed read reporting configuration request is extracted from initial data_buf buffer
 */
#define ZB_ZCL_GENERAL_GET_NEXT_READ_REP_CFG_REQ(data_buf, rep_cfg_req, rslt)              \
  {                                                                                        \
zb_zcl_read_reporting_cfg_req_t *cfg_req;                                                  \
  (cfg_req) = zb_buf_len(data_buf) >=  ZB_ZCL_READ_REP_CFG_REQ_SIZE ?                      \
   (zb_zcl_read_reporting_cfg_req_t *) zb_buf_begin(data_buf) : NULL;                      \
  if (cfg_req)                                                                             \
   {                                                                                       \
     rep_cfg_req.direction  = cfg_req -> direction;                                        \
     rep_cfg_req.attr_id = cfg_req -> attr_id;                                             \
     ZB_ZCL_HTOLE16_INPLACE(&(read_rep_cfg_req).attr_id);                                  \
     (void)zb_buf_cut_left((data_buf), ZB_ZCL_READ_REP_CFG_REQ_SIZE);                      \
     rslt = ZB_TRUE;                                                                       \
   }                                                                                       \
  else                                                                                     \
   {                                                                                       \
     rslt = ZB_FALSE;                                                                      \
   }                                                                                       \
}


/**Format of the Attribute Reporting Configuration Record Field
 *Figure 2.22 in ZCL spec.
 *NOTE: it can be various number of attribute recording configuration
 *record fields in Read reporting configuration response
 */
typedef ZB_PACKED_PRE struct zb_zcl_read_reporting_cfg_rsp_s
{
  zb_uint8_t status;       /*!<If the attribute is not implemented on
                            * the sender or receiver of the command,
                            *whichever is relevant (depending on
                            * direction), this field shall be set to
                            *UNSUPPORTED_ATTRIBUTE. If the attribute
                            * is supported, but is not capable of
                            *being reported, this field shall be set
                            * to UNREPORTABLE_ATTRIBUTE.
                            *Otherwise, this field shall be set to SUCCESS.
                            */

  zb_uint8_t direction;    /*!<The direction field specifies whether
                            * values of the attribute are reported (0x00), or
                            *whether reports of the attribute are received (0x01).
                            */
  zb_uint16_t attr_id;    /*!<The attribute identifier field is 16
                           * bits in length and shall contain the identifier of
                           *the attribute that the reporting configuration details apply to
                           */

  /** Response contents: different for client and server */
  ZB_PACKED_PRE union
  {
    /** clnt: if the direction field is set to 0, then the attribute
     * data type field, the minimum reporting interval field, the
     * maximum reporting interval field and the reportable
     *change field are included in the payload, and the timeoutperiod field is omitted.
     */
    ZB_PACKED_PRE struct
    {
      zb_uint8_t attr_type;     /*!< Attribute data type */
      zb_uint16_t min_interval; /*!< The minimum reporting interval field is 16 bits in length and shall contain the
                                 *minimum interval, in seconds, between issuing reports for the attribute specified
                                 *in the attribute identifier field. If the minimum reporting interval has not been
                                 *configured, this field shall contain the value 0xffff.
                                 */
      zb_uint16_t max_interval; /*!< The maximum reporting interval field is 16 bits in length and shall contain the
                                 *maximum interval, in seconds, between issuing reports for the attribute specified
                                 *in the attribute identifier field. If the maximum reporting interval has not been
                                 *configured, this field shall contain the value 0xffff.
                                 */
      zb_uint8_t delta[1];      /*!< The reportable change field shall contain the minimum change to the attribute that
                                 *will result in a report being issued. For attributes with 'analog' data type (see
                                 *Table 2.16) the field has the same data type as the attribute. If the reportable
                                 *change has not been configured, this field shall contain the invalid value for the
                                 *relevant data type. For attributes
                                 *of 'discrete' data type (see Table
                                 *2.16) this field is omitted.
                                 */
    } ZB_PACKED_STRUCT
    clnt;

    /** srv: if the direction field is set to 1, then the timeout period field is included in the payload,
     *and the attribute data type field, the minimum reporting interval field, the
     *maximum reporting interval field and the reportable change field are omitted.
     */
    ZB_PACKED_PRE struct
    {
      zb_uint16_t timeout;        /*!< The timeout period field is 16 bits in length and shall contain the maximum
                                   *expected time, in seconds, between received reports for the attribute specified in
                                   *the attribute identifier field. If the timeout period has not been configured, this
                                   *field shall contain the value 0xffff.
                                   */
    } ZB_PACKED_STRUCT
    srv;

  } u;
} ZB_PACKED_STRUCT zb_zcl_read_reporting_cfg_rsp_t;

/** @cond internals_doc */
/** calculate size for command request: direction, attr_id */
#define ZB_ZCL_READ_REPORTING_CFG_REQ_SIZE (sizeof(zb_uint8_t) + sizeof(zb_uint16_t))

/** calculate size for unsuccess response version: status, direction, attr_id */
#define ZB_ZCL_READ_REPORTING_CFG_ERROR_SIZE (sizeof(zb_uint8_t)*2 + sizeof(zb_uint16_t))

#define ZB_ZCL_READ_REPORTING_CFG_RES_SIZE (sizeof(zb_zcl_read_reporting_cfg_rsp_t))

/** @endcond */

/*! @brief Initialize Read reporting configuration command (report send case)
    @param buffer to put packet to
    @param ptr - command buffer pointer
    @param def_resp - enable/disable default response
*/
#define ZB_ZCL_GENERAL_INIT_READ_REPORTING_CONFIGURATION_SRV_REQ(buffer, ptr, def_resp)          \
{                                                                                       \
  ptr = ZB_ZCL_START_PACKET(buffer);                                                    \
  ZB_ZCL_CONSTRUCT_GENERAL_COMMAND_REQ_FRAME_CONTROL_A(                                 \
      ptr, ZB_ZCL_FRAME_DIRECTION_TO_SRV, ZB_ZCL_NOT_MANUFACTURER_SPECIFIC, def_resp);  \
  ZB_ZCL_CONSTRUCT_COMMAND_HEADER(ptr, ZB_ZCL_GET_SEQ_NUM(), ZB_ZCL_CMD_READ_REPORT_CFG); \
}


/*! @brief Initialize Read reporting configuration command (report send case)
    @param buffer to put packet to
    @param ptr - command buffer pointer
    @param def_resp - enable/disable default response
    @param manuf_code - manufacturer specific code
*/
#define ZB_ZCL_GENERAL_INIT_READ_REPORTING_CONFIGURATION_SRV_REQ_MANUF(buffer, ptr, def_resp, manuf_code) \
{                                                                                                   \
  ptr = ZB_ZCL_START_PACKET(buffer);                                                                \
  ZB_ZCL_CONSTRUCT_GENERAL_COMMAND_REQ_FRAME_CONTROL_A(                                             \
    ptr, ZB_ZCL_FRAME_DIRECTION_TO_SRV, ZB_ZCL_MANUFACTURER_SPECIFIC, def_resp);                    \
  ZB_ZCL_CONSTRUCT_COMMAND_HEADER_EXT(ptr, ZB_ZCL_GET_SEQ_NUM(), ZB_TRUE,                           \
                                      manuf_code, ZB_ZCL_CMD_READ_REPORT_CFG);                      \
}

/*! @brief Initialize Read reporting configuration command (report receive case)
    @param buffer to put packet to
    @param ptr - command buffer pointer
    @param def_resp - enable/disable default response
*/
#define ZB_ZCL_GENERAL_INIT_READ_REPORTING_CONFIGURATION_CLI_REQ(buffer, ptr, def_resp)          \
{                                                                                       \
  ptr = ZB_ZCL_START_PACKET(buffer);                                                    \
  ZB_ZCL_CONSTRUCT_GENERAL_COMMAND_REQ_FRAME_CONTROL_A(                                 \
      ptr, ZB_ZCL_FRAME_DIRECTION_TO_CLI, ZB_ZCL_NOT_MANUFACTURER_SPECIFIC, def_resp);  \
  ZB_ZCL_CONSTRUCT_COMMAND_HEADER(ptr, ZB_ZCL_GET_SEQ_NUM(), ZB_ZCL_CMD_READ_REPORT_CFG); \
}

/*! @brief Add Read reporting configuration record to command payload (report send case)
    @param ptr - command buffer pointer
    @param attr_id - attribute identifier
*/
#define ZB_ZCL_GENERAL_ADD_SEND_READ_REPORTING_CONFIGURATION_REQ(ptr, attr_id) \
{                                                                          \
  ZB_ZCL_PACKET_PUT_DATA8(ptr, ZB_ZCL_CONFIGURE_REPORTING_SEND_REPORT);    \
  ZB_ZCL_PACKET_PUT_DATA16_VAL(ptr, (attr_id));                            \
}

/*! @brief Add Read reporting configuration record to command payload (report receive case)
    @param ptr - command buffer pointer
    @param attr_id - attribute identifier
*/
#define ZB_ZCL_GENERAL_ADD_RECV_READ_REPORTING_CONFIGURATION_REQ(ptr, attr_id) \
{                                                                                     \
  ZB_ZCL_PACKET_PUT_DATA8(ptr, ZB_ZCL_CONFIGURE_REPORTING_RECV_REPORT);               \
  ZB_ZCL_PACKET_PUT_DATA16_VAL(ptr, (attr_id));                                       \
}

/** @brief Sends Read reporting configuration command
    @param buffer to put data to
    @param ptr - pointer to the memory area to put data to
    @param addr - address to send packet to
    @param dst_addr_mode - addressing mode
    @param dst_ep - destination endpoint
    @param ep - sending endpoint
    @param prfl_id - profile identifier
    @param cluster_id - cluster identifier
    @param cb - callback for getting command send status
*/
#define ZB_ZCL_GENERAL_SEND_READ_REPORTING_CONFIGURATION_REQ(                                           \
    buffer, ptr, addr, dst_addr_mode, dst_ep, ep, prfl_id, cluster_id, cb)                     \
{                                                                                              \
  ZB_ZCL_FINISH_PACKET(buffer, ptr)                                                           \
  ZB_ZCL_SEND_COMMAND_SHORT(buffer, addr, dst_addr_mode, dst_ep, ep, prfl_id, cluster_id, cb); \
}


/** @brief Parses Read reporting configuration response and returns next
    read reporting configuration record or NULL if there is no more data.

    If response contains invalid data, NULL is returned.
    @param data_buf -  ID zb_bufid_t of a buffer containing Configure reporting response data
    @param read_rep_conf_res - out pointer to zb_zcl_read_reporting_cfg_rsp_t, containing
    read reporting configuration  record
    @note data_buf buffer should contain Read reporting configuration, without ZCL header.
    Each parsed Read reporting configuration record is extracted from initial data_buf buffer
 */
#define ZB_ZCL_GENERAL_GET_READ_REPORTING_CONFIGURATION_RES(data_buf, read_rep_conf_res)   \
{                                                                                          \
  zb_uint16_t res_size = 0;                                                                \
  zb_uint16_t min_resp_size = sizeof(zb_uint16_t) + 2*sizeof(zb_uint8_t);                  \
  (read_rep_conf_res) = (min_resp_size <= zb_buf_len(data_buf))?                           \
    (zb_zcl_read_reporting_cfg_rsp_t *)zb_buf_begin(data_buf): NULL;                       \
                                                                                           \
  if ((read_rep_conf_res))                                                                 \
  {                                                                                        \
    if ((read_rep_conf_res)->direction == ZB_ZCL_CONFIGURE_REPORTING_SEND_REPORT)          \
    {                                                                                      \
      min_resp_size = zb_zcl_is_analog_data_type((read_rep_conf_res)->u.clnt.attr_type)?   \
                         ZB_ZCL_READ_REPORTING_CFG_RES_SIZE:                               \
                         ZB_ZCL_READ_REPORTING_CFG_RES_SIZE - sizeof(zb_uint8_t);          \
    }                                                                                      \
    else                                                                                   \
    {                                                                                      \
      min_resp_size += sizeof(zb_uint16_t); /* timeout value */                            \
    }                                                                                      \
                                                                                           \
    (read_rep_conf_res)->status =                                                          \
      zb_zcl_zcl8_statuses_conversion((read_rep_conf_res)->status);                        \
    if ((read_rep_conf_res)->status != ZB_ZCL_STATUS_SUCCESS                               \
         && (read_rep_conf_res)->status != ZB_ZCL_STATUS_MALFORMED_CMD)                    \
    {                                                                                      \
      /* In case of error, direction and attribute id is reported */                       \
      res_size = sizeof(zb_uint16_t) + 2*sizeof(zb_uint8_t);                               \
    }                                                                                      \
    else                                                                                   \
    {                                                                                      \
      res_size = min_resp_size;                                                            \
    }                                                                                      \
  }                                                                                        \
                                                                                           \
  if (res_size <= zb_buf_len(data_buf))                                                    \
  {                                                                                        \
    ZB_ZCL_HTOLE16_INPLACE(&(read_rep_conf_res)->attr_id);                                 \
    (void)zb_buf_cut_left((data_buf), res_size);                                           \
  }                                                                                        \
  else                                                                                     \
  {                                                                                        \
    (read_rep_conf_res) = NULL;                                                            \
  }                                                                                        \
}


/** @} */ /* Configure read reporting configuration command sending and parsing. */

/** @cond internals_doc */

#endif


/******************** HA extensions: discovery commands ***************************/
/********************* HA extension: discovery commands *****************************/

#if defined ZB_ENABLE_HA || defined DOXYGEN

/**
 *  @defgroup disc_cmd Discovery commands command sending and parsing.
 *  @{
 *    @details
 *    Discovery commands request has fixed length payload.
 *    Discovery commands response has variable-length payload.
 *
 */

/********* request commands ************/

/*! @brief ZCL Discovery Commands Command frame
    @see HA spec, 12.1.1 Discovery Commands Command
    @see HA spec, 12.1.3 Discovery Commands Command
*/
typedef ZB_PACKED_PRE struct zb_zcl_disc_cmd_req_s
{
  zb_uint8_t start_cmd_id;   /*!< Start command identifier */
  zb_uint8_t maximum;        /*!< Maximum command identifiers */
} ZB_PACKED_STRUCT
zb_zcl_disc_cmd_req_t;

/** @brief Discover commands Generated command
    @param _buffer to put packet to
    @param _direction - direction client-to-server or server-to-client
    @param _def_resp - enable/disable default response
    @param _addr - address to send packet to
    @param _dst_addr_mode - addressing mode
    @param _dst_ep - destination endpoint
    @param _ep - sending endpoint
    @param _profile_id - profile identifier
    @param _cluster_id - cluster identifier
    @param _cb - callback for getting command send status
    @param _is_manuf - is discovery manufacturer attributes
    @param _manuf_id - manufacturer ID
    @param _start_cmd_id - start command ID
    @param _max_len - max count
*/
#define ZB_ZCL_GENERAL_DISC_COMMAND_GENERATED_REQ(_buffer, _direction, _def_resp,       \
          _addr, _dst_addr_mode, _dst_ep, _ep, _profile_id, _cluster_id, _cb,           \
          _is_manuf, _manuf_id, _start_cmd_id, _max_len)                                \
{                                                                                       \
  zb_uint8_t *cmd_ptr = ZB_ZCL_START_PACKET((_buffer));                                 \
  ZB_ZCL_CONSTRUCT_GENERAL_COMMAND_REQ_FRAME_CONTROL_EXT(cmd_ptr,                       \
      ((_is_manuf) ? ZB_ZCL_MANUFACTURER_SPECIFIC : ZB_ZCL_NOT_MANUFACTURER_SPECIFIC),  \
      (_direction),  (_def_resp));                                                      \
  ZB_ZCL_CONSTRUCT_COMMAND_HEADER_EXT(cmd_ptr, ZB_ZCL_GET_SEQ_NUM(), (_is_manuf),       \
      (_manuf_id), ZB_ZCL_CMD_DISCOVER_COMMANDS_GENERATED);                             \
  ZB_ZCL_PACKET_PUT_DATA8(cmd_ptr, (_start_cmd_id));                                    \
  ZB_ZCL_PACKET_PUT_DATA8(cmd_ptr, (_max_len));                                         \
  ZB_ZCL_FINISH_PACKET((_buffer), cmd_ptr)                                             \
  ZB_ZCL_SEND_COMMAND_SHORT((_buffer), (_addr), (_dst_addr_mode), (_dst_ep), (_ep),     \
      (_profile_id), (_cluster_id), (_cb));                                             \
}

/** @brief Discover commands Received command
    @param _buffer to put packet to
    @param _direction - direction client-to-server or server-to-client
    @param _def_resp - enable/disable default response
    @param _addr - address to send packet to
    @param _dst_addr_mode - addressing mode
    @param _dst_ep - destination endpoint
    @param _ep - sending endpoint
    @param _profile_id - profile identifier
    @param _cluster_id - cluster identifier
    @param _cb - callback for getting command send status
    @param _is_manuf - is discovery manufacturer attributes
    @param _manuf_id - manufacturer ID
    @param _start_cmd_id - start command ID
    @param _max_len - max count
*/
#define ZB_ZCL_GENERAL_DISC_COMMAND_RECEIVED_REQ(_buffer, _direction, _def_resp,        \
          _addr, _dst_addr_mode, _dst_ep, _ep, _profile_id, _cluster_id, _cb,           \
          _is_manuf, _manuf_id, _start_cmd_id, _max_len)                                \
{                                                                                       \
  zb_uint8_t *cmd_ptr = ZB_ZCL_START_PACKET((_buffer));                                 \
  ZB_ZCL_CONSTRUCT_GENERAL_COMMAND_REQ_FRAME_CONTROL_EXT(cmd_ptr,                       \
      ((_is_manuf) ? ZB_ZCL_MANUFACTURER_SPECIFIC : ZB_ZCL_NOT_MANUFACTURER_SPECIFIC),  \
      (_direction),  (_def_resp));                                                      \
  ZB_ZCL_CONSTRUCT_COMMAND_HEADER_EXT(cmd_ptr, ZB_ZCL_GET_SEQ_NUM(), (_is_manuf),       \
      (_manuf_id), ZB_ZCL_CMD_DISCOVER_COMMANDS_RECEIVED);                              \
  ZB_ZCL_PACKET_PUT_DATA8(cmd_ptr, (_start_cmd_id));                                    \
  ZB_ZCL_PACKET_PUT_DATA8(cmd_ptr, (_max_len));                                         \
  ZB_ZCL_FINISH_PACKET((_buffer), cmd_ptr)                                             \
  ZB_ZCL_SEND_COMMAND_SHORT((_buffer), (_addr), (_dst_addr_mode), (_dst_ep), (_ep),     \
        (_profile_id), (_cluster_id), (_cb));                                           \
}

/** @brief Get parameter of Discovery command request.

   @param _buffer - buffer containing part of Discover command request data
   @param _data_ptr - out pointer to zb_zcl_disc_cmd_req_t, containing Discover command data
   @param _status - result parse, see @ref zb_zcl_parse_status_t
 */
#define ZB_ZCL_GENERAL_GET_DISC_COMMAND(_data_ptr, _buffer, _status) \
{                                                               \
  if (zb_buf_len((_buffer)) != sizeof(zb_zcl_disc_cmd_req_t))   \
  {                                                             \
    (_status) = ZB_ZCL_PARSE_STATUS_FAILURE;                    \
  }                                                             \
  else                                                          \
  {                                                             \
    zb_zcl_disc_cmd_req_t *src_ptr =                            \
            (zb_zcl_disc_cmd_req_t*)zb_buf_begin((_buffer));    \
    (_status) = ZB_ZCL_PARSE_STATUS_SUCCESS;                    \
    (_data_ptr)->start_cmd_id = src_ptr->start_cmd_id;          \
    (_data_ptr)->maximum = src_ptr->maximum;                    \
  }                                                             \
}

/********* response commands ************/

/*! @brief ZCL Discover command complete enum
    @see HA spec, subclause 12.1.2.1.2.
*/
typedef enum zb_zcl_disc_cmd_complete_e
{
  /*! Are more commands to be discovered */
  ZB_ZCL_DISC_CMD_NOT_COMPLETE = 0x00,
  /*! No more commands to be discovered */
  ZB_ZCL_DISC_CMD_COMPLETE = 0x01,
} zb_zcl_disc_cmd_complete_t;

/*! @brief ZCL Discovery Commands Command frame
    @see HA spec, 12.1.2 Discovery Commands Command
    @see HA spec, 12.1.4 Discovery Commands Command
*/
typedef ZB_PACKED_PRE struct zb_zcl_disc_cmd_resp_s
{
  zb_uint8_t complete;   /*!< Discovery complete */
} ZB_PACKED_STRUCT
zb_zcl_disc_cmd_resp_t;

/** @brief Init Discover commands response manufacture specific and not specific
    @param _cmd_ptr - pointer of current part command
    @param _buffer to put packet to
    @param _direction - direction client-to-server or server-to-client
    @param _seq - sequence of request
    @param _cmd - command Id
    @param _is_manuf - is manufacturer specific
    @param _manuf_id - manufacturer ID
    @param _complete - The discovery complete field is a boolean field
*/
#define ZB_ZCL_GENERAL_INIT_DISC_COMMAND_RESP(_cmd_ptr, _buffer,                \
        _direction, _seq, _cmd, _is_manuf, _manuf_id, _complete)                \
{                                                                               \
  (_cmd_ptr) = ZB_ZCL_START_PACKET((_buffer));                                  \
  ZB_ZCL_CONSTRUCT_GENERAL_COMMAND_RESP_FRAME_CONTROL_EXT((_cmd_ptr),           \
      ((_is_manuf)!=ZB_FALSE ?                                                  \
            ZB_ZCL_MANUFACTURER_SPECIFIC : ZB_ZCL_NOT_MANUFACTURER_SPECIFIC),   \
      (_direction));                                                            \
  ZB_ZCL_CONSTRUCT_COMMAND_HEADER_EXT((_cmd_ptr), (_seq), (_is_manuf),          \
          (_manuf_id), (_cmd));                                                 \
  ZB_ZCL_PACKET_PUT_DATA8((_cmd_ptr), (_complete));                             \
}

/** @brief Add command Id to Discover commands response
    @param _cmd_ptr - pointer of current part command
    @param _cmd_id - The discovery complete field is a boolean field
*/
#define ZB_ZCL_GENERAL_ADD_DISC_COMMAND_RESP(_cmd_ptr, _cmd_id)                 \
{                                                                               \
  ZB_ZCL_PACKET_PUT_DATA8((_cmd_ptr), (_cmd_id));                               \
}

/** @brief Finish command Id to Discover commands response
    @param _buffer to put packet to
    @param _cmd_ptr - pointer of current part command
    @param _addr - address to send packet to
    @param _dst_addr_mode - addressing mode
    @param _dst_ep - destination endpoint
    @param _ep - sending endpoint
    @param _profile_id - profile identifier
    @param _cluster_id - cluster identifier
*/
#define ZB_ZCL_GENERAL_FINISH_DISC_COMMAND_RESP(_buffer, _cmd_ptr,              \
        _addr, _dst_addr_mode, _dst_ep, _ep, _profile_id, _cluster_id)          \
{                                                                               \
  ZB_ZCL_FINISH_PACKET((_buffer), (_cmd_ptr))                                  \
  ZB_ZCL_SEND_COMMAND_SHORT((_buffer), (_addr), (_dst_addr_mode), (_dst_ep),    \
      (_ep), (_profile_id), (_cluster_id), NULL);                               \
}


/** @brief Get parameter of Discovery command response.

   @param _buffer -  ID zb_bufid_t of a buffer containing part of Discover command response data
   @param _data_ptr - out pointer to zb_zcl_disc_cmd_res_t, containing Discover command response
   @param _status - result parse, see @ref zb_zcl_parse_status_t
 */
#define ZB_ZCL_GENERAL_GET_DISC_COMMAND_RESP(_data_ptr, _buffer, _status)  \
{                                                                       \
  if (zb_buf_len((_buffer)) < sizeof(zb_zcl_disc_cmd_resp_t))           \
  {                                                                     \
    (_status) = ZB_ZCL_PARSE_STATUS_FAILURE;                            \
  }                                                                     \
  else                                                                  \
  {                                                                     \
    zb_zcl_disc_cmd_resp_t *src_ptr =                                   \
        (zb_zcl_disc_cmd_resp_t*)zb_buf_begin((_buffer));               \
    (_status) = ZB_ZCL_PARSE_STATUS_SUCCESS;                            \
    (_data_ptr)->complete = src_ptr->complete;                          \
    (void)zb_buf_cut_left(()buffer), sizeof(zb_zcl_disc_cmd_resp_t));   \
  }                                                                     \
}

/** @brief Get next command ID from Discovery command response.

   @param _buffer -  ID zb_bufid_t of a buffer containing part of Discover command response data
   @param _cmd_id - out next command ID
   @param _status - result parse, see @ref zb_zcl_parse_status_t
 */
/* sizeof(cmd_id) == sizeof(zb_uint8_t),
   command ID has not special struct */
#define ZB_ZCL_GENERAL_GET_NEXT_CMD_ID_DISC_COMMAND_RESP(_cmd_id, _buffer, _status)  \
{                                                                       \
  if (zb_buf_len((_buffer)) < sizeof(zb_uint8_t))                       \
  {                                                                     \
    (_status) = ZB_ZCL_PARSE_STATUS_FAILURE;                            \
  }                                                                     \
  else                                                                  \
  {                                                                     \
    zb_uint8_t *cmd_id_list = (zb_uint8_t*)zb_buf_begin((_buffer));     \
    (_status) = ZB_ZCL_PARSE_STATUS_SUCCESS;                            \
    (_cmd_id) = cmd_id_list[0];                                         \
    (void)zb_buf_cut_left((_buffer), sizeof(zb_uint8_t));               \
  }                                                                     \
}

/** @} */ /* Discovery commands command sending and parsing */

#endif /* defined ZB_ENABLE_HA || defined DOXYGEN */

/***** HA extension: discovery attribute extended ************************/

/*! @brief ZCL Discover Attribute Extended Command frame
    @see HA1.2 spec, zb_zcl_disc_attr_ext 12.1.5 Discover Attributes Extended Command
*/
/* command frames are identical for Discover attr and Discover attr ext */
typedef zb_zcl_disc_attr_req_t zb_zcl_disc_attr_ext_req_t;

/** @brief Discover Attributes Extended command
    @param _buffer to put packet to
    @param _direction - direction client-to-server or server-to-client
    @param _def_resp - enable/disable default response
    @param _addr - address to send packet to
    @param _dst_addr_mode - addressing mode
    @param _dst_ep - destination endpoint
    @param _ep - sending endpoint
    @param _profile_id - profile identifier
    @param _cluster_id - cluster identifier
    @param _cb - callback for getting command send status
    @param _is_manuf - is discovery manufacturer attributes
    @param _manuf_id - manufacturer ID
    @param _start_attr_id - start attribute ID
    @param _max_len - max count
*/
#define ZB_ZCL_GENERAL_DISC_ATTRIBUTE_EXT_REQ(_buffer, _direction, _def_resp,           \
        _addr, _dst_addr_mode, _dst_ep, _ep, _profile_id, _cluster_id, _cb,             \
        _is_manuf, _manuf_id, _start_attr_id, _max_len)                                 \
{                                                                                       \
  zb_uint8_t *cmd_ptr = ZB_ZCL_START_PACKET((_buffer));                                 \
  ZB_ZCL_CONSTRUCT_GENERAL_COMMAND_REQ_FRAME_CONTROL_EXT(cmd_ptr,                       \
      ((_is_manuf) ? ZB_ZCL_MANUFACTURER_SPECIFIC : ZB_ZCL_NOT_MANUFACTURER_SPECIFIC),  \
      (_direction),  (_def_resp));                                                      \
  ZB_ZCL_CONSTRUCT_COMMAND_HEADER_EXT(cmd_ptr, ZB_ZCL_GET_SEQ_NUM(), (_is_manuf),       \
      (_manuf_id), ZB_ZCL_CMD_DISCOVER_ATTR_EXT);                                       \
  ZB_ZCL_PACKET_PUT_DATA16_VAL(cmd_ptr, (_start_attr_id));                              \
  ZB_ZCL_PACKET_PUT_DATA8(cmd_ptr, (_max_len));                                         \
  ZB_ZCL_FINISH_PACKET((_buffer), cmd_ptr)                                             \
  ZB_ZCL_SEND_COMMAND_SHORT((_buffer), (_addr), (_dst_addr_mode),                       \
      (_dst_ep), (_ep), (_profile_id), (_cluster_id), (_cb));                           \
}

/*! @brief HA1.2 Description for Discover Attribute Response frame
    @see ZCL spec, zb_zcl_read_attr 2.4.14 Discover Attribute Response
*/
typedef ZB_PACKED_PRE struct zb_zcl_disc_attr_ext_info_s
{
  zb_uint16_t attr_id;  /*!< Attribute identifier */
  zb_uint8_t data_type; /*!< Attribute data type */
  zb_uint8_t attr_access; /*!< Attribute access control */
} ZB_PACKED_STRUCT
zb_zcl_disc_attr_ext_info_t;

#define ZB_ZCL_DISC_ATTR_EXT_RESP_SIZE      sizeof(zb_zcl_disc_attr_ext_info_t)

/*! @brief ZCL Discover Attribute Response frame
    @see ZCL spec, zb_zcl_read_attr 2.4.15 Discover Attribute Response
    @note Command frame contains variable number of parameters
*/
typedef ZB_PACKED_PRE struct zb_zcl_disc_attr_ext_res_s
{
  zb_uint8_t complete;       /*!< Discovery complete */
  zb_zcl_disc_attr_ext_info_t info[1]; /*!< Attribute desc list */
} ZB_PACKED_STRUCT
zb_zcl_disc_attr_ext_res_t;

/** @brief Get parameter of Discovery Attribute Extended response.

   @param _buffer -  ID zb_bufid_t of a buffer containing part of Discover Attribute Extended response data
   @param _complete - out variable to complete field Discover command response, see @ref zcl_disc_complete
   @param _status - result parse, see @ref zb_zcl_parse_status_t
 */
#define ZB_ZCL_GENERAL_GET_COMPLETE_DISC_ATTR_RESP(_complete, _buffer, _status)  \
{                                                                       \
  if (zb_buf_len((_buffer)) < sizeof(zb_uint8_t))           \
  {                                                                     \
    (_status) = ZB_ZCL_PARSE_STATUS_FAILURE;                            \
  }                                                                     \
  else                                                                  \
  {                                                                     \
    (_status) = ZB_ZCL_PARSE_STATUS_SUCCESS;                            \
    zb_uint8_t *complete_ptr = (zb_uint8_t*)zb_buf_begin((_buffer));    \
    (_complete) = *complete_ptr;                                        \
    (void)zb_buf_cut_left((_buffer), sizeof(zb_uint8_t));               \
  }                                                                     \
}

/** @brief Get next command ID from Discovery command response.

   @param _buffer -  ID zb_bufid_t of a buffer containing part of Discover command response data
   @param _data_ptr - out Description for Discover Attribute Response frame, see @ref zb_zcl_disc_attr_ext_info_t
   @param _status - result parse, see @ref zb_zcl_parse_status_t
 */
#define ZB_ZCL_GENERAL_GET_NEXT_ATTR_DISC_COMMAND_RESP(_data_ptr, _buffer, _status)  \
{                                                                       \
  if (zb_buf_len((_buffer)) < sizeof(zb_zcl_disc_attr_ext_info_t))      \
  {                                                                     \
    (_status) = ZB_ZCL_PARSE_STATUS_FAILURE;                            \
  }                                                                     \
  else                                                                  \
  {                                                                     \
    zb_zcl_disc_attr_ext_info_t *src_ptr =                              \
          (zb_zcl_disc_attr_ext_info_t*)zb_buf_begin((_buffer));        \
    (_status) = ZB_ZCL_PARSE_STATUS_SUCCESS;                            \
    ZB_HTOLE16(&((_data_ptr)->attr_id), &(src_ptr->attr_id));           \
    (_data_ptr)->data_type = src_ptr->data_type;                        \
    (_data_ptr)->data_access = src_ptr->data_access;                    \
    (void)zb_buf_cut_left((_buffer), sizeof(zb_zcl_disc_attr_ext_info_t)); \
  }                                                                     \
}

  /*! Convert internal attribute access bitmask into ZCL/HA1.2 bitmask
 *  value (actually, support 0 and 1 bits) */
#define ZB_ZCL_CONVERT_ATTR_ACCESS_BITMASK(_access) ((_access) & 0x7U)

/******************** Command handlers ***************************/

/* ZCL handlers */
void zb_zcl_read_attr_handler(zb_uint8_t param);
void zb_zcl_write_attr_handler(zb_uint8_t param);
#if defined ZB_ENABLE_HA
void zb_zcl_discover_commands_res(zb_uint8_t param, zb_bool_t recv_cmd_type);
#endif /* ZB_ENABLE_HA */

#if !(defined ZB_ZCL_DISABLE_REPORTING) || defined(DOXYGEN)

void zb_zcl_configure_reporting_handler(zb_uint8_t param);

/* ZCL commands */
void zb_zcl_send_report_attr_command(struct zb_zcl_reporting_info_s *rep_info, zb_uint8_t param);

#endif

zb_bool_t zb_zcl_handle_general_commands(zb_uint8_t param);

/** @endcond */ /* internals_doc */

/** @} */

/** @endcond */ /* DOXYGEN_ZCL_SECTION */

#endif /* ZB_ZCL_COMMANDS_H */
