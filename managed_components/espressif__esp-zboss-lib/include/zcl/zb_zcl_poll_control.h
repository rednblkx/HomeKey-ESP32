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
/* PURPOSE: Poll Control cluster definitions
*/

#ifndef ZB_ZCL_POLL_CONTROL_H
#define ZB_ZCL_POLL_CONTROL_H 1

#include "zcl/zb_zcl_common.h"
#include "zcl/zb_zcl_commands.h"

/** @cond DOXYGEN_ZCL_SECTION */

/** @addtogroup ZB_ZCL_POLL_CONTROL
 *  @{
 *    @details
 *    All commands in the cluster have only request form, and carry no payload.
 *
 */

#if defined ZB_HA_ENABLE_POLL_CONTROL_SERVER || defined DOXYGEN

/*! @brief Schedule poll control process on given endpoint
 * First check-in will occur after one check-in interval
 * Poll Control process also starts automatically during ZCL periodic activities
 * initialization if Poll Control server cluster is declared
 * @param param - buffer for check-in command
 * @param endpoint - endpoint to start poll control process on */
void zb_zcl_poll_control_start(zb_uint8_t param, zb_uint8_t endpoint);


/**
 * @brief Stop Poll control processing
 *
 * Checking already scheduled will be canceled
 * @return reference to buffer which was previously reserved for check-in.
 *         It must be reused or explicitly freed.
 */
zb_uint8_t zb_zcl_poll_control_stop(void);


/*! @brief Start Poll Control process on Server side
 * @param param - buffer for Poll Process
*/
void zb_zcl_poll_control_start_check_in(zb_uint8_t param);

/**
   @brief Register callback for Poll control cluster.

   This callback will be called when check-in command is sent.

   @param cb - pointer to callback
 */
void zb_zcl_poll_controll_register_cb(zb_callback_t cb);


#endif /* defined ZB_HA_ENABLE_POLL_CONTROL_SERVER || defined DOXYGEN */

/* Cluster ZB_ZCL_CLUSTER_ID_POLL_CONTROL */

/**
 * @name Poll Control cluster attribute identifiers
 * @anchor zcl_poll_control_attr
 * @see HA spec, Poll Control Cluster 9.5.4.1
 *
 * Note: These values were members of `enum zb_zcl_poll_control_attr_e` type but were converted to a
 * set of macros due to MISRA violations.
 */
/** @{ */
/** Check-in Interval attribute, HA spec 9.5.4.1.1 */
#define ZB_ZCL_ATTR_POLL_CONTROL_CHECKIN_INTERVAL_ID       0x0000U
/** Long Poll Interval attribute, HA spec 9.5.4.1.2 */
#define ZB_ZCL_ATTR_POLL_CONTROL_LONG_POLL_INTERVAL_ID     0x0001U
/** Short Poll Interval attribute, HA spec 9.5.4.1.3 */
#define ZB_ZCL_ATTR_POLL_CONTROL_SHORT_POLL_INTERVAL_ID    0x0002U
/** Fast Poll Timeout attribute, HA spec 9.5.4.1. */
#define ZB_ZCL_ATTR_POLL_CONTROL_FAST_POLL_TIMEOUT_ID      0x0003U
/** Check-in Interval Min attribute, HA spec 9.5.4.1.4 */
#define ZB_ZCL_ATTR_POLL_CONTROL_MIN_CHECKIN_INTERVAL_ID   0x0004U
/** Long Poll Interval Min attribute, HA spec 9.5.4.1.5 */
#define ZB_ZCL_ATTR_POLL_CONTROL_LONG_POLL_MIN_INTERVAL_ID 0x0005U
/** Fast Poll Timeout Max attribute, HA spec 9.5.4.1.6 */
#define ZB_ZCL_ATTR_POLL_CONTROL_FAST_POLL_MAX_TIMEOUT_ID  0x0006U

/** Status Data - custom non-spec parameters  */
#define ZB_ZCL_ATTR_POLL_CONTROL_STATUS_DATA_ID 0xefffU
/** Status Data - custom non-spec parameters for server side */
#define ZB_ZCL_ATTR_POLL_CONTROL_ADDR_DATA_ID   0xeffeU
/** @} */

/** @name Poll Control cluster attributes */
/** @{ */

/** @brief Default value for Poll Control cluster revision global attribute */
#define ZB_ZCL_POLL_CONTROL_CLUSTER_REVISION_DEFAULT ((zb_uint16_t)0x0003u)

/** @brief Value for stop Check-in process for Check-in Interval attribute */
#define ZB_ZCL_POLL_CONTROL_CHECKIN_INTERVAL_NO_CHECK_IN_VALUE    0x0000

/** @brief Default value for Check-in Interval attribute */
#define ZB_ZCL_POLL_CONTROL_CHECKIN_INTERVAL_DEFAULT_VALUE    0x3840

/** @brief Maximum value for Check-in Interval attribute */
#define ZB_ZCL_POLL_CONTROL_CHECKIN_INTERVAL_MAX_VALUE        0x6e0000

/** @brief Minimum value for Long Poll Interval attribute */
#define ZB_ZCL_POLL_CONTROL_LONG_POLL_INTERVAL_MIN_VALUE      ZB_PIM_MINIMUM_LONG_POLL_INTERVAL_IN_QS

/** @brief Default value for Long Poll Interval attribute */
#define ZB_ZCL_POLL_CONTROL_LONG_POLL_INTERVAL_DEFAULT_VALUE  ZB_PIM_DEFAULT_LONG_POLL_INTERVAL_IN_QS

/** @brief Maximum value for Long Poll Interval attribute */
#define ZB_ZCL_POLL_CONTROL_LONG_POLL_INTERVAL_MAX_VALUE      ZB_PIM_MAXIMUM_LONG_POLL_INTERVAL_IN_QS

/** @brief Minimum value for Short Poll Interval attribute */
#define ZB_ZCL_POLL_CONTROL_SHORT_POLL_INTERVAL_MIN_VALUE     ZB_PIM_MINIMUM_SHORT_POLL_INTERVAL_IN_QS

/** @brief Default value for Short Poll Interval attribute */
#define ZB_ZCL_POLL_CONTROL_SHORT_POLL_INTERVAL_DEFAULT_VALUE ZB_PIM_DEFAULT_SHORT_POLL_INTERVAL_IN_QS

/** @brief Maximum value for Short Poll Interval attribute */
#define ZB_ZCL_POLL_CONTROL_SHORT_POLL_INTERVAL_MAX_VALUE     ZB_PIM_MAXIMUM_SHORT_POLL_INTERVAL_IN_QS

/** @brief Minimum value for Fast Poll Timeout attribute */
#define ZB_ZCL_POLL_CONTROL_FAST_POLL_TIMEOUT_MIN_VALUE       0x01

/** @brief Default value for Fast Poll Timeout attribute */
#define ZB_ZCL_POLL_CONTROL_FAST_POLL_TIMEOUT_DEFAULT_VALUE   0x0028

/** @brief Maximum value for Fast Poll Timeout attribute */
#define ZB_ZCL_POLL_CONTROL_FAST_POLL_TIMEOUT_MAX_VALUE       0xffff

/** @brief Default value for CheckinIntervalMin attribute */
#define ZB_ZCL_POLL_CONTROL_CHECKIN_MIN_INTERVAL_DEFAULT_VALUE ((zb_uint32_t)0)

/** @brief Default value for LongPollIntervalMin attribute */
#define ZB_ZCL_POLL_CONTROL_LONG_POLL_MIN_INTERVAL_DEFAULT_VALUE ((zb_uint32_t)0)

/** @brief Default value for FastPollTimeoutMax attribute */
#define ZB_ZCL_POLL_CONTROL_FAST_POLL_MAX_TIMEOUT_DEFAULT_VALUE ((zb_uint16_t)0)
/** @} */

/** @cond DOXYGEN_INTERNAL_DOC */
/** @name Poll Control cluster internals
 *  Internal structures for attribute representation in cluster definitions. */
/** @{ */

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_POLL_CONTROL_CHECKIN_INTERVAL_ID(data_ptr) \
{                                                       \
  ZB_ZCL_ATTR_POLL_CONTROL_CHECKIN_INTERVAL_ID,         \
  ZB_ZCL_ATTR_TYPE_U32,                                 \
  ZB_ZCL_ATTR_ACCESS_READ_WRITE,                        \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                   \
  (void*) data_ptr                                      \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_POLL_CONTROL_LONG_POLL_INTERVAL_ID(data_ptr) \
{                                                       \
  ZB_ZCL_ATTR_POLL_CONTROL_LONG_POLL_INTERVAL_ID,       \
  ZB_ZCL_ATTR_TYPE_U32,                                 \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                         \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                   \
  (void*) data_ptr                                      \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_POLL_CONTROL_SHORT_POLL_INTERVAL_ID(data_ptr) \
{                                                       \
  ZB_ZCL_ATTR_POLL_CONTROL_SHORT_POLL_INTERVAL_ID,      \
  ZB_ZCL_ATTR_TYPE_U16,                                 \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                         \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                   \
  (void*) data_ptr                                      \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_POLL_CONTROL_FAST_POLL_TIMEOUT_ID(data_ptr) \
{                                                       \
  ZB_ZCL_ATTR_POLL_CONTROL_FAST_POLL_TIMEOUT_ID,        \
  ZB_ZCL_ATTR_TYPE_U16,                                 \
  ZB_ZCL_ATTR_ACCESS_READ_WRITE,                        \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                   \
  (void*) data_ptr                                      \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_POLL_CONTROL_MIN_CHECKIN_INTERVAL_ID(data_ptr) \
{                                                       \
  ZB_ZCL_ATTR_POLL_CONTROL_MIN_CHECKIN_INTERVAL_ID,     \
  ZB_ZCL_ATTR_TYPE_U32,                                 \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                         \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                   \
  (void*) data_ptr                                      \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_POLL_CONTROL_LONG_POLL_MIN_INTERVAL_ID(data_ptr) \
{                                                       \
  ZB_ZCL_ATTR_POLL_CONTROL_LONG_POLL_MIN_INTERVAL_ID,   \
  ZB_ZCL_ATTR_TYPE_U32,                                 \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                         \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                   \
  (void*) data_ptr                                      \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_POLL_CONTROL_FAST_POLL_MAX_TIMEOUT_ID(data_ptr) \
{                                                       \
  ZB_ZCL_ATTR_POLL_CONTROL_FAST_POLL_MAX_TIMEOUT_ID,    \
  ZB_ZCL_ATTR_TYPE_U16,                                 \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                         \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                   \
  (void*) data_ptr                                      \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_POLL_CONTROL_STATUS_DATA_ID(data_ptr) \
{                                                       \
  ZB_ZCL_ATTR_POLL_CONTROL_STATUS_DATA_ID,              \
  ZB_ZCL_ATTR_TYPE_NULL,                                \
  ZB_ZCL_ATTR_ACCESS_INTERNAL,                          \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                   \
  (void*) data_ptr                                      \
}


#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_POLL_CONTROL_ADDR_DATA_ID(data_ptr) \
{                                                       \
  ZB_ZCL_ATTR_POLL_CONTROL_ADDR_DATA_ID,                \
  ZB_ZCL_ATTR_TYPE_NULL,                                \
  ZB_ZCL_ATTR_ACCESS_INTERNAL,                          \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                   \
  (void*) data_ptr                                      \
}


/*! @brief Structure representation of Client Status attributes */
typedef ZB_PACKED_PRE struct zb_zcl_poll_control_client_status_s
{
  /** Client is In Poll Mode */
  zb_bool_t is_poll_mode;
  /** Client Fast Poll timeout*/
  zb_uint16_t fast_poll_timeout;

} ZB_PACKED_STRUCT zb_zcl_poll_control_client_status_t;


#define ZB_ZCL_POLL_CTRL_INVALID_ADDR 0xFFFF
#define ZB_ZCL_POLL_INVALID_EP        0xFF
/*! @brief Address information that is used for sending check-in command */
/* Currently not only address info is stored - also status information */
typedef struct zb_zcl_poll_control_srv_cfg_data_s
{
  /** Short address of the Poll control client */
  zb_uint16_t poll_addr;
  /** Endpoint number of the Poll control client */
  zb_uint8_t poll_ep;
  /** Flag to check if check-in command is being sent currently */
  zb_bitfield_t sending_cmd:4;
  /** Flag to determine fast poll mode / long poll mode */
  zb_bitfield_t fast_poll_mode:1;
}
zb_zcl_poll_control_srv_cfg_data_t;

/** @brief Default value for Fast Poll Timeout for Client command */
#define ZB_ZCL_POLL_CONTROL_FAST_POLL_TIMEOUT_CLIENT_DEFAULT_VALUE   0x00f0

/*! @internal Number of attributes mandatory for reporting in Poll Control cluster */
#define ZB_ZCL_POLL_CONTROL_REPORT_ATTR_COUNT 0

/** @} */
/** @endcond */ /* DOXYGEN_INTERNAL_DOC */ /* Poll Control cluster internals */

/** @name Poll Control cluster attributes */
/** @{ */

/** @brief Declare attribute list for Poll Control cluster - server side
    @param attr_list - attribute list name
    @param checkin_interval - pointer to variable to store Check-in Interval attribute
    @param long_poll_interval - pointer to variable to store Long Poll Interval attribute
    @param short_poll_interval - pointer to variable to store Short Poll Interval attribute
    @param fast_poll_timeout - pointer to variable to store Fast Poll Timeout attribute
    @param checkin_interval_min - pointer to variable to store Check-in Interval Min attribute
    @param long_poll_interval_min- pointer to variable to store Long Poll Interval Min attribute
    @param fast_poll_timeout_max - pointer to variable to store Fast Poll Timeout Max attribute
*/
#define ZB_ZCL_DECLARE_POLL_CONTROL_ATTRIB_LIST(attr_list,                                            \
        checkin_interval, long_poll_interval, short_poll_interval, fast_poll_timeout,                 \
        checkin_interval_min, long_poll_interval_min, fast_poll_timeout_max)                          \
  zb_zcl_poll_control_srv_cfg_data_t srv_cfg_data_ctx_##attr_list =                                         \
  { ZB_ZCL_POLL_CTRL_INVALID_ADDR, ZB_ZCL_POLL_INVALID_EP, 0, 0 };        \
  ZB_ZCL_START_DECLARE_ATTRIB_LIST_CLUSTER_REVISION(attr_list, ZB_ZCL_POLL_CONTROL)                   \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_POLL_CONTROL_CHECKIN_INTERVAL_ID, (checkin_interval))              \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_POLL_CONTROL_LONG_POLL_INTERVAL_ID, (long_poll_interval))          \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_POLL_CONTROL_SHORT_POLL_INTERVAL_ID, (short_poll_interval))        \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_POLL_CONTROL_FAST_POLL_TIMEOUT_ID, (fast_poll_timeout))            \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_POLL_CONTROL_MIN_CHECKIN_INTERVAL_ID, (checkin_interval_min))      \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_POLL_CONTROL_LONG_POLL_MIN_INTERVAL_ID, (long_poll_interval_min))  \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_POLL_CONTROL_FAST_POLL_MAX_TIMEOUT_ID, (fast_poll_timeout_max))    \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_POLL_CONTROL_ADDR_DATA_ID, &(srv_cfg_data_ctx_##attr_list))        \
  ZB_ZCL_FINISH_DECLARE_ATTRIB_LIST

/** @brief Declare attribute list for Poll Control cluster - client side
    @param attr_list - attribute list name
*/
#define ZB_ZCL_DECLARE_POLL_CONTROL_ATTRIB_LIST_CLIENT(attr_list)                             \
    zb_zcl_poll_control_client_status_t client_status_data_ctx_##attr_list =                  \
        { ZB_FALSE, ZB_ZCL_POLL_CONTROL_FAST_POLL_TIMEOUT_CLIENT_DEFAULT_VALUE };             \
    ZB_ZCL_START_DECLARE_ATTRIB_LIST_CLUSTER_REVISION(attr_list, ZB_ZCL_POLL_CONTROL)         \
    ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_POLL_CONTROL_STATUS_DATA_ID,                             \
                         (&(client_status_data_ctx_##attr_list)))                             \
  ZB_ZCL_FINISH_DECLARE_ATTRIB_LIST

/** @} */ /* Poll Control cluster attributes */

/** @name Poll Control cluster commands */
/** @{ */

/*! @brief Poll Control cluster command identifiers
    @see HA spec, Poll Control Cluster, 9.5.4.3
*/
enum zb_zcl_poll_control_cmd_e
{
  ZB_ZCL_CMD_POLL_CONTROL_CHECK_IN_ID    = 0x00   /**< "Check-in" command. HA spec 9.5.4.4 */
};

/*! @brief Poll Control cluster response command identifiers
    @see HA spec, Poll Control Cluster, 9.5.5.2
*/
enum zb_zcl_poll_control_resp_cmd_e
{
  ZB_ZCL_CMD_POLL_CONTROL_CHECK_IN_RESPONSE_ID        = 0x00, /**< "Check-in response" command, HA spec 9.5.5.3 */
  ZB_ZCL_CMD_POLL_CONTROL_FAST_POLL_STOP_ID           = 0x01, /**< "Fast Poll Stop" command, HA spec 9.5.5.4 */
  ZB_ZCL_CMD_POLL_CONTROL_SET_LONG_POLL_INTERVAL_ID   = 0x02, /**< "Set Long Poll Interval" command, HA spec 9.5.5.5 */
  ZB_ZCL_CMD_POLL_CONTROL_SET_SHORT_POLL_INTERVAL_ID  = 0x03, /**< "Set Short Poll Interval" command, HA spec 9.5.5.6 */
};

/** @cond DOXYGEN_INTERNAL_DOC */
/* Poll control cluster commands list : only for information - do not modify */
#define ZB_ZCL_CLUSTER_ID_POLL_CONTROL_SERVER_ROLE_GENERATED_CMD_LIST ZB_ZCL_CMD_POLL_CONTROL_CHECK_IN_ID

#define ZB_ZCL_CLUSTER_ID_POLL_CONTROL_CLIENT_ROLE_RECEIVED_CMD_LIST ZB_ZCL_CLUSTER_ID_POLL_CONTROL_SERVER_ROLE_GENERATED_CMD_LIST

#define ZB_ZCL_CLUSTER_ID_POLL_CONTROL_CLIENT_ROLE_GENERATED_CMD_LIST                    \
                                      ZB_ZCL_CMD_POLL_CONTROL_CHECK_IN_RESPONSE_ID,      \
                                      ZB_ZCL_CMD_POLL_CONTROL_FAST_POLL_STOP_ID,         \
                                      ZB_ZCL_CMD_POLL_CONTROL_SET_LONG_POLL_INTERVAL_ID, \
                                      ZB_ZCL_CMD_POLL_CONTROL_SET_SHORT_POLL_INTERVAL_ID

#define ZB_ZCL_CLUSTER_ID_POLL_CONTROL_SERVER_ROLE_RECEIVED_CMD_LIST ZB_ZCL_CLUSTER_ID_POLL_CONTROL_CLIENT_ROLE_GENERATED_CMD_LIST
/** @endcond */ /* DOXYGEN_INTERNAL_DOC */

/******************************* Check-in ******************************/

/*! @brief Send Check-in command, see HA spec 9.5.4.4
    @param _buffer - to put packet to
    @param _dst_addr - destination address
    @param _dst_addr_mode - addressing mode
    @param _dst_ep - destination endpoint
    @param _local_ep - source endpoint
    @param _prfl_id - profile identifier
    @param _cb - callback for getting command send status
*/
#define ZB_ZCL_POLL_CONTROL_SEND_CHECK_IN_REQ(                              \
     _buffer, _dst_addr, _dst_addr_mode, _dst_ep, _local_ep, _prfl_id, _cb) \
{                                                                           \
  zb_uint8_t* _ptr = ZB_ZCL_START_PACKET(_buffer);                          \
  ZB_ZCL_CONSTRUCT_SPECIFIC_COMMAND_RES_FRAME_CONTROL(_ptr);                \
  ZB_ZCL_CONSTRUCT_COMMAND_HEADER(_ptr, ZB_ZCL_GET_SEQ_NUM(),               \
      ZB_ZCL_CMD_POLL_CONTROL_CHECK_IN_ID);                                 \
  ZB_ZCL_FINISH_PACKET((_buffer), _ptr)                                    \
  ZB_ZCL_SEND_COMMAND_SHORT(                                                \
      _buffer, _dst_addr, _dst_addr_mode, _dst_ep, _local_ep, _prfl_id,     \
      ZB_ZCL_CLUSTER_ID_POLL_CONTROL, _cb);                                 \
}

/*! @brief Parameters to pass to device callback for incoming check in */
typedef struct zb_zcl_poll_control_check_in_cli_param_s
{
  zb_uint16_t fast_poll_timeout;
  zb_uint16_t short_addr;
  zb_uint8_t  ep;
}
zb_zcl_poll_control_check_in_cli_param_t;

/** @cond DOXYGEN_INTERNAL_DOC */
#define ZB_ZCL_POLL_CONTROL_CLI_CALL_USER_APP(                              \
  _buffer, _short_addr, _ep, _fast_poll_timeout, _result)                   \
{                                                                           \
  TRACE_MSG(TRACE_ZCL1, "ZB_ZCL_POLL_CONTROL_CLI_CALL_USER_APP", (FMT__0)); \
  if (ZCL_CTX().device_cb)                                                  \
  {                                                                         \
    zb_zcl_device_callback_param_t *user_app_data =                         \
      ZB_BUF_GET_PARAM((_buffer), zb_zcl_device_callback_param_t);          \
    zb_zcl_poll_control_check_in_cli_param_t *value =                       \
        &(user_app_data->cb_param.checkin_cli_param);                       \
    value->fast_poll_timeout = (_fast_poll_timeout);                        \
    value->short_addr = (_short_addr);                                      \
    value->ep = (_ep);                                                      \
    user_app_data->device_cb_id = ZB_ZCL_POLL_CONTROL_CHECK_IN_CLI_CB_ID;   \
    user_app_data->endpoint = (_ep);                                        \
    user_app_data->status = RET_OK;                                         \
    (ZCL_CTX().device_cb)(param);                                           \
    _result = user_app_data->status;                                        \
  }                                                                         \
  else                                                                      \
  {                                                                         \
    _result = RET_OK;                                                       \
  }                                                                         \
}
/** @endcond */ /* DOXYGEN_INTERNAL_DOC */

/******************************* Check-in response ******************************/

/*! @brief Structure representation of Check-in response command payload */
typedef ZB_PACKED_PRE struct zb_zcl_poll_control_check_in_res_s
{
  /** Start Fast Polling */
  zb_uint8_t is_start;
  /** Fast Poll Timeout */
  zb_uint16_t timeout;
} ZB_PACKED_STRUCT zb_zcl_poll_control_check_in_res_t;

/*! @brief Send "Check-in response" command, see HA spec 9.5.5.3
    @param buffer to put packet to
    @param addr - address to send packet to
    @param dst_addr_mode - addressing mode
    @param dst_ep - destination endpoint
    @param ep - sending endpoint
    @param prfl_id - profile identifier
    @param def_resp - enable/disable default response
    @param cb - callback
    @param is_start - Start Fast Polling
    @param timeout - Fast Poll Timeout
*/
#define ZB_ZCL_POLL_CONTROL_SEND_CHECK_IN_RES(                              \
    buffer, addr, dst_addr_mode, dst_ep, ep, prfl_id, def_resp, cb,         \
    is_start, timeout)                                                      \
{                                                                           \
  zb_uint8_t* ptr = ZB_ZCL_START_PACKET_REQ(buffer)                            \
  ZB_ZCL_CONSTRUCT_SPECIFIC_COMMAND_REQ_FRAME_CONTROL(ptr, (def_resp))     \
  ZB_ZCL_CONSTRUCT_COMMAND_HEADER_REQ(ptr, ZB_ZCL_GET_SEQ_NUM(),                \
      ZB_ZCL_CMD_POLL_CONTROL_CHECK_IN_RESPONSE_ID);                        \
  ZB_ZCL_PACKET_PUT_DATA8(ptr, (is_start));                                 \
  ZB_ZCL_PACKET_PUT_DATA16_VAL(ptr, (timeout));                             \
  ZB_ZCL_FINISH_PACKET((buffer), ptr)                                      \
  ZB_ZCL_SEND_COMMAND_SHORT(                                                \
      buffer, addr, dst_addr_mode, dst_ep, ep, prfl_id,                     \
      ZB_ZCL_CLUSTER_ID_POLL_CONTROL, cb);                                  \
}

/** @brief Macro for getting Check-in response command
  * @attention Assumes that ZCL header already cut.
  * @param data_ptr - pointer to a variable of type @ref
  * zb_zcl_poll_control_check_in_res_s.
  * @param buffer containing the packet (by pointer).
  * @param status - variable to put parse status to (see @ref zb_zcl_parse_status_t).
  */
#define ZB_ZCL_POLL_CONTROL_GET_CHECK_IN_RES(data_ptr, buffer, status)    \
{                                                                         \
  if (zb_buf_len((buffer)) < sizeof(zb_zcl_poll_control_check_in_res_t))  \
  {                                                                       \
   (status) = ZB_ZCL_PARSE_STATUS_FAILURE;                                \
  }                                                                       \
  else                                                                    \
  {                                                                       \
    zb_zcl_poll_control_check_in_res_t *src_ptr =                         \
         (zb_zcl_poll_control_check_in_res_t*)zb_buf_begin((buffer));     \
    (status) = ZB_ZCL_PARSE_STATUS_SUCCESS;                               \
    (data_ptr)->is_start = src_ptr->is_start;                             \
    ZB_HTOLE16(&((data_ptr)->timeout), &(src_ptr->timeout));              \
  }                                                                       \
}

/******************************* Fast Poll Stop ******************************/

/*! @brief Send "Fast Poll Stop" command, see HA spec 9.5.5.4
    @param buffer to put packet to
    @param addr - address to send packet to
    @param dst_addr_mode - addressing mode
    @param dst_ep - destination endpoint
    @param ep - sending endpoint
    @param prfl_id - profile identifier
    @param def_resp - enable/disable default response
    @param cb - callback
*/
#define ZB_ZCL_POLL_CONTROL_SEND_FAST_POLL_STOP_REQ(                        \
    buffer, addr, dst_addr_mode, dst_ep, ep, prfl_id, def_resp, cb)         \
{                                                                           \
  zb_uint8_t* ptr = ZB_ZCL_START_PACKET_REQ(buffer)                            \
  ZB_ZCL_CONSTRUCT_SPECIFIC_COMMAND_REQ_FRAME_CONTROL(ptr, (def_resp))     \
  ZB_ZCL_CONSTRUCT_COMMAND_HEADER_REQ(ptr, ZB_ZCL_GET_SEQ_NUM(),                \
      ZB_ZCL_CMD_POLL_CONTROL_FAST_POLL_STOP_ID);                           \
  ZB_ZCL_FINISH_PACKET((buffer), ptr)                                      \
  ZB_ZCL_SEND_COMMAND_SHORT(                                                \
      buffer, addr, dst_addr_mode, dst_ep, ep, prfl_id,                     \
      ZB_ZCL_CLUSTER_ID_POLL_CONTROL, cb);                                  \
}

/******************************* Set Long Poll Interval ******************************/

/*! @brief Structure representation of Set Long Poll Interval command payload */
typedef ZB_PACKED_PRE struct zb_zcl_poll_control_set_long_poll_interval_req_s
{
  /** New Long Poll Interval */
  zb_uint32_t interval;
} ZB_PACKED_STRUCT zb_zcl_poll_control_set_long_poll_interval_t;


/*! @brief Send "Set Long Poll Interval" command, see HA spec 9.5.5.5
    @param buffer to put packet to
    @param addr - address to send packet to
    @param dst_addr_mode - addressing mode
    @param dst_ep - destination endpoint
    @param ep - sending endpoint
    @param prfl_id - profile identifier
    @param def_resp - enable/disable default response
    @param cb - callback
    @param interval - New Long Poll Interval
*/
#define ZB_ZCL_POLL_CONTROL_SEND_SET_LONG_POLL_INTERVAL_REQ(                \
    buffer, addr, dst_addr_mode, dst_ep, ep, prfl_id, def_resp, cb, interval) \
{                                                                           \
  zb_uint8_t* ptr = ZB_ZCL_START_PACKET_REQ(buffer)                            \
  ZB_ZCL_CONSTRUCT_SPECIFIC_COMMAND_REQ_FRAME_CONTROL(ptr, (def_resp))     \
  ZB_ZCL_CONSTRUCT_COMMAND_HEADER_REQ(ptr, ZB_ZCL_GET_SEQ_NUM(),                \
      ZB_ZCL_CMD_POLL_CONTROL_SET_LONG_POLL_INTERVAL_ID);                   \
  ZB_ZCL_PACKET_PUT_DATA32_VAL(ptr, (interval));                            \
  ZB_ZCL_FINISH_PACKET((buffer), ptr)                                      \
  ZB_ZCL_SEND_COMMAND_SHORT(                                                \
      buffer, addr, dst_addr_mode, dst_ep, ep, prfl_id,                     \
      ZB_ZCL_CLUSTER_ID_POLL_CONTROL, cb);                                  \
}

/** @brief Macro for getting "Set Long Poll Interval" command
  * @attention Assumes that ZCL header already cut.
  * @param data_ptr - pointer to a variable of type @ref
  * zb_zcl_poll_control_set_long_poll_interval_t.
  * @param buffer containing the packet (by pointer).
  * @param status - variable to put parse status to (see @ref zb_zcl_parse_status_t).
  */
#define ZB_ZCL_POLL_CONTROL_GET_SET_LONG_POLL_INTERVAL_REQ(data_ptr, buffer, status)  \
{                                                                                   \
  if (zb_buf_len((buffer)) < sizeof(zb_zcl_poll_control_set_long_poll_interval_t))  \
  {                                                                                 \
   (status) = ZB_ZCL_PARSE_STATUS_FAILURE;                                          \
  }                                                                                 \
  else                                                                              \
  {                                                                                 \
    zb_zcl_poll_control_set_long_poll_interval_t *src_ptr =                         \
         (zb_zcl_poll_control_set_long_poll_interval_t*)zb_buf_begin((buffer));     \
    (status) = ZB_ZCL_PARSE_STATUS_SUCCESS;                                         \
    ZB_HTOLE32(&((data_ptr)->interval), &(src_ptr->interval));                      \
  }                                                                                 \
}

/******************************* Set Short Poll Interval ******************************/

/*! @brief Structure representation of Set Short Poll Interval command payload */
typedef ZB_PACKED_PRE struct zb_zcl_poll_control_set_short_poll_interval_req_s
{
  /** New Short Poll Interval */
  zb_uint16_t interval;
} ZB_PACKED_STRUCT zb_zcl_poll_control_set_short_poll_interval_t;


/*! @brief Send "Set Short Poll Interval" command, see HA spec 9.5.5.6
    @param buffer to put packet to
    @param addr - address to send packet to
    @param dst_addr_mode - addressing mode
    @param dst_ep - destination endpoint
    @param ep - sending endpoint
    @param prfl_id - profile identifier
    @param def_resp - enable/disable default response
    @param cb - callback
    @param interval - New Short Poll Interval
*/
#define ZB_ZCL_POLL_CONTROL_SEND_SET_SHORT_POLL_INTERVAL_REQ(               \
    buffer, addr, dst_addr_mode, dst_ep, ep, prfl_id, def_resp, cb, interval) \
{                                                                           \
  zb_uint8_t* ptr = ZB_ZCL_START_PACKET_REQ(buffer)                            \
  ZB_ZCL_CONSTRUCT_SPECIFIC_COMMAND_REQ_FRAME_CONTROL(ptr, (def_resp))     \
  ZB_ZCL_CONSTRUCT_COMMAND_HEADER_REQ(ptr, ZB_ZCL_GET_SEQ_NUM(),                \
      ZB_ZCL_CMD_POLL_CONTROL_SET_SHORT_POLL_INTERVAL_ID);                  \
  ZB_ZCL_PACKET_PUT_DATA16_VAL(ptr, (interval));                            \
  ZB_ZCL_FINISH_PACKET((buffer), ptr)                                      \
  ZB_ZCL_SEND_COMMAND_SHORT(                                                \
      buffer, addr, dst_addr_mode, dst_ep, ep, prfl_id,                     \
      ZB_ZCL_CLUSTER_ID_POLL_CONTROL, cb);                                  \
}

/** @brief Macro for getting "Set Short Poll Interval" command
  * @attention Assumes that ZCL header already cut.
  * @param data_ptr - pointer to a variable of type @ref
  * zb_zcl_poll_control_set_short_poll_interval_t.
  * @param buffer containing the packet (by pointer).
  * @param status - variable to put parse status to (see @ref zb_zcl_parse_status_t).
  */
#define ZB_ZCL_POLL_CONTROL_GET_SET_SHORT_POLL_INTERVAL_REQ(data_ptr, buffer, status)  \
{                                                                                   \
  if (zb_buf_len((buffer)) < sizeof(zb_zcl_poll_control_set_short_poll_interval_t)) \
  {                                                                                 \
   (status) = ZB_ZCL_PARSE_STATUS_FAILURE;                                          \
  }                                                                                 \
  else                                                                              \
  {                                                                                 \
    zb_zcl_poll_control_set_short_poll_interval_t *src_ptr =                        \
         (zb_zcl_poll_control_set_short_poll_interval_t*)zb_buf_begin((buffer));    \
    (status) = ZB_ZCL_PARSE_STATUS_SUCCESS;                                         \
    ZB_HTOLE16(&((data_ptr)->interval), &(src_ptr->interval));                      \
  }                                                                                 \
}

#if defined ZB_HA_ENABLE_POLL_CONTROL_SERVER || defined DOXYGEN

/**
 * @brief Set client address information. This address is used to
 * send Check-in command. If address information is NOT sent,
 * check-in command will be sent assuming binding with client is
 * created
 * @param local_ep - local endpoint number, is used to find particular attribute
 * @param addr - poll control client short address
 * @param ep - poll control client end point
 * @return RET_ERROR if addr is not a TC (WWAH requirement).
 *         RET_OK otherwise.
 */
zb_ret_t zb_zcl_poll_control_set_client_addr(zb_uint8_t local_ep, zb_uint16_t addr, zb_uint8_t ep);
#endif

#if defined ZB_ZCL_SUPPORT_CLUSTER_POLL_CONTROL || defined DOXYGEN
/**
  @brief Function is used by Poll control client to set Fast poll
  timeout value. This value is included into Check-in response
  command
  @param ep - local endpoint number, running Poll control client
  @param fast_poll_timeout - Fast poll timeout value (in quarterseconds)
*/
void zb_zcl_set_fast_poll_timeout(zb_uint8_t ep, zb_uint16_t fast_poll_timeout);

#if defined ZB_USE_NVRAM || defined DOXYGEN
/**
   @brief Save to NVRAM Poll Control dataset
   @param param
   @note ignore param, set it to 0
*/
void zb_zcl_poll_control_save_nvram(zb_uint8_t param);
#endif /* ZB_USE_NVRAM */
#endif /* ZB_ZCL_SUPPORT_CLUSTER_POLL_CONTROL */

/*! @} */ /* Poll Control cluster commands */

/*! @} */ /* ZCL Poll Control cluster definitions */

#if defined ZB_ENABLE_HA

zb_uint8_t zb_zcl_get_cmd_list_poll_control(zb_bool_t is_client_generated, zb_uint8_t **cmd_list);

#endif /* defined ZB_ENABLE_HA */

/** @endcond */ /* DOXYGEN_ZCL_SECTION */

#if defined ZB_ZCL_SUPPORT_CLUSTER_POLL_CONTROL
void zb_zcl_poll_control_init_server(void);
void zb_zcl_poll_control_init_client(void);
#define ZB_ZCL_CLUSTER_ID_POLL_CONTROL_SERVER_ROLE_INIT zb_zcl_poll_control_init_server
#define ZB_ZCL_CLUSTER_ID_POLL_CONTROL_CLIENT_ROLE_INIT zb_zcl_poll_control_init_client
#endif /* ZB_ZCL_SUPPORT_CLUSTER_POLL_CONTROL */

#endif /* ZB_ZCL_POLL_CONTROL_H */
