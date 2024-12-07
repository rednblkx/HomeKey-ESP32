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
/* PURPOSE: ZCL "Identify cluster" definitions
*/

#ifndef ZB_ZCL_IDENTIFY_H
#define ZB_ZCL_IDENTIFY_H 1

#if defined(ZB_ZCL_SUPPORT_CLUSTER_IDENTIFY) || defined(DOXYGEN)

#include "zcl/zb_zcl_common.h"
#include "zcl/zb_zcl_commands.h"

/** @cond DOXYGEN_ZCL_SECTION */

/** @addtogroup ZB_ZCL_IDENTIFY
 *  @{
 *    @details
 *    Identify cluster implementation supports Identify command and Identify Query request-response
 *    command pair. All these commands have simple payload. Both Identify and Identify Query
 *    request are being processed, and Identify Query response is being generated in ZCL internals.
 *
 *    @par Example
 *    Identify sending:
 *    @snippet HA_samples/door_lock/sample_zed.c send_identify_req
 *
 *    Identify Query request sending:
 *    @snippet HA_samples/on_off_switch/sample_zed.c identify_send_identify_query_req
 */

/** @name Identify cluster attributes
  * @{
  */

/** @brief Identify cluster attribute identifier
    @see ZCL8 spec, subclause 3.5.2.2
*/
enum zb_zcl_identify_attr_e
{
  /*! Identify time attribute */
  ZB_ZCL_ATTR_IDENTIFY_IDENTIFY_TIME_ID = 0x0000
};

/** @brief Default value for Identify cluster revision global attribute */
#define ZB_ZCL_IDENTIFY_CLUSTER_REVISION_DEFAULT ((zb_uint16_t)0x0002u)

/** @brief Default value for Identify attribute */
#define ZB_ZCL_IDENTIFY_IDENTIFY_TIME_DEFAULT_VALUE 0x0000

/** @} */ /* Identify cluster attributes */

/*! @name Identify cluster command structures and definitions
    @{
*/

/** @brief Command identifiers for "Identify" cluster
    @see ZCL8 spec, subclauses 3.5.2.3, 3.5.2.4
*/
enum zb_zcl_identify_cmd_e
{
  ZB_ZCL_CMD_IDENTIFY_IDENTIFY_ID           = 0x00,  /**< Identify command */
  ZB_ZCL_CMD_IDENTIFY_IDENTIFY_QUERY_ID     = 0x01,  /**< Identify query command */
  ZB_ZCL_CMD_IDENTIFY_TRIGGER_EFFECT_ID     = 0x40,  /**< "Trigger effect" command identifier. */
  ZB_ZCL_CMD_IDENTIFY_IDENTIFY_QUERY_RSP_ID = 0x00   /**< Identify query response */
};

/** @cond internals_doc */
/* Identify cluster commands lists : only for information - do not modify */
#define ZB_ZCL_CLUSTER_ID_IDENTIFY_SERVER_ROLE_RECEIVED_CMD_LIST                         \
                                                  ZB_ZCL_CMD_IDENTIFY_IDENTIFY_ID,       \
                                                  ZB_ZCL_CMD_IDENTIFY_IDENTIFY_QUERY_ID, \
                                                  ZB_ZCL_CMD_IDENTIFY_TRIGGER_EFFECT_ID,

#define ZB_ZCL_CLUSTER_ID_IDENTIFY_CLIENT_ROLE_RECEIVED_CMD_LIST                             \
                                                  ZB_ZCL_CMD_IDENTIFY_IDENTIFY_QUERY_RSP_ID,

#define ZB_ZCL_CLUSTER_ID_IDENTIFY_SERVER_ROLE_GENERATED_CMD_LIST ZB_ZCL_CLUSTER_ID_IDENTIFY_CLIENT_ROLE_RECEIVED_CMD_LIST

#define ZB_ZCL_CLUSTER_ID_IDENTIFY_CLIENT_ROLE_GENERATED_CMD_LIST ZB_ZCL_CLUSTER_ID_IDENTIFY_SERVER_ROLE_RECEIVED_CMD_LIST
/*! @}
 *  @endcond */ /* internals_doc */

/** Effect identifier enum
 * @see ZCL8 spec 3.5.2.3.3.1 */
enum zb_zcl_identify_trigger_effect_e
{
  /**< Effect identifier field value: Light is turned on/off once */
  ZB_ZCL_IDENTIFY_EFFECT_ID_BLINK           = 0x00,
  /**< Effect identifier field value: Light turned on/off over 1 second and
   * repeated 15 times */
  ZB_ZCL_IDENTIFY_EFFECT_ID_BREATHE         = 0x01,
  /**< Effect identifier field value: Colored light turns green for 1 second;
   * non-colored light flashes twice */
  ZB_ZCL_IDENTIFY_EFFECT_ID_OKAY            = 0x02,
  /**< Effect identifier field value: Colored light turns orange for 8 seconds; non-colored
   * light switches to maximum brightness for 0.5s and then minimum brightness for 7.5s */
  ZB_ZCL_IDENTIFY_EFFECT_ID_CHANNEL_CHANGE  = 0x0b,
  /**< Effect identifier field value: Complete the current effect sequence before terminating.
   * E.g., if in the middle of a breathe effect (as above), first complete the current 1s
   * breathe effect and then terminate the effect*/
  ZB_ZCL_IDENTIFY_EFFECT_ID_FINISH_EFFECT   = 0xfe,
  /**< Effect identifier field value: Terminate the effect as soon as possible */
  ZB_ZCL_IDENTIFY_EFFECT_ID_STOP            = 0xff,
};


/** Effect variant enum
 * @see ZCL8 spec 3.5.2.3.3.2 */
enum zb_zcl_identify_trigger_variant_e
{
  /**< Effect variant field value: Default */
  ZB_ZCL_IDENTIFY_EFFECT_ID_VARIANT_DEFAULT       = 0x00,
};


/*! @brief Structured representation of Trigger effect command payload */
typedef ZB_PACKED_PRE struct zb_zcl_identify_effect_req_s
{
  zb_uint8_t effect_id;         /*!< Effect identify, see @ref zb_zcl_identify_trigger_effect_e */
  zb_uint8_t effect_variant;    /*!< Effect variant, see @ref zb_zcl_identify_trigger_variant_e  */
} ZB_PACKED_STRUCT zb_zcl_identify_effect_req_t;


/*! @brief Send Trigger effect command
    @param buffer to put packet to
    @param addr - address to send packet to
    @param dst_addr_mode - addressing mode
    @param dst_ep - destination endpoint
    @param ep - sending endpoint
    @param prof_id - profile identifier
    @param def_resp - flag "Default response required"
    @param cb - callback for getting command send status
    @param effect_id - effect identify, see @ref zb_zcl_identify_trigger_effect_e
    @param effect_var - effect variant, see @ref zb_zcl_identify_trigger_variant_e
*/
#define ZB_ZCL_IDENTIFY_SEND_TRIGGER_VARIANT_REQ(                                                  \
  buffer, addr, dst_addr_mode, dst_ep, ep, prof_id, def_resp, cb, effect_id, effect_var)  \
{                                                                                                  \
zb_uint8_t* ptr = ZB_ZCL_START_PACKET_REQ(buffer)                                                  \
  ZB_ZCL_CONSTRUCT_SPECIFIC_COMMAND_REQ_FRAME_CONTROL(ptr, def_resp)                               \
ZB_ZCL_CONSTRUCT_COMMAND_HEADER_REQ(ptr, ZB_ZCL_GET_SEQ_NUM(), ZB_ZCL_CMD_IDENTIFY_TRIGGER_EFFECT_ID); \
ZB_ZCL_PACKET_PUT_DATA8(ptr, (effect_id));                              \
ZB_ZCL_PACKET_PUT_DATA8(ptr, (effect_var));                             \
ZB_ZCL_FINISH_PACKET(buffer, ptr)                                                                 \
ZB_ZCL_SEND_COMMAND_SHORT(                                                                        \
    buffer, addr, dst_addr_mode, dst_ep, ep, prof_id, ZB_ZCL_CLUSTER_ID_IDENTIFY, cb);            \
}


/** @brief Parses Trigger effect command
  * @attention Assumes that ZCL header already cut.
  * @param data_ptr - pointer to a variable of type @ref
  * zb_zcl_identify_effect_req_s.
  * @param buffer containing the packet (by pointer).
  * @param status - variable to put parse status to (see @ref zb_zcl_parse_status_t).
  */
#define ZB_ZCL_IDENTIFY_GET_TRIGGER_VARIANT_REQ(data_ptr, buffer, status)    \
{                                                                            \
  if (zb_buf_len((buffer)) < sizeof(zb_zcl_identify_effect_req_t))           \
  {                                                                          \
   (status) = ZB_ZCL_PARSE_STATUS_FAILURE;                                   \
  }                                                                          \
  else                                                                       \
  {                                                                          \
    zb_zcl_identify_effect_req_t *src_ptr =                                  \
                      (zb_zcl_identify_effect_req_t*)zb_buf_begin((buffer)); \
    (status) = ZB_ZCL_PARSE_STATUS_SUCCESS;                                  \
    ZB_MEMCPY((data_ptr), src_ptr, sizeof(zb_zcl_identify_effect_req_t));    \
  }                                                                          \
}

/**
 *  @name Inform User App about ZCL Identify cluster command and change attributes.
 *  Internal structures and define-procedure for inform User App about ZCL Identify
 *  cluster command and change attributes.
 *  @internal
 *  @{
 */

/** @brief Declare change Identify attribute for User Application
*/
typedef struct zb_zcl_identify_effect_value_param_s
{
  zb_uint8_t effect_id;         /*!< Effect identify */
  zb_uint8_t effect_variant;    /*!< Effect variant */
} zb_zcl_identify_effect_value_param_t;


/*! Struct for invoke User App & continue after */
typedef struct zb_zcl_identify_effect_user_app_schedule_e
{
  zb_zcl_parsed_hdr_t cmd_info;         /**< Parameters for continue command,
                                              see @ref zb_zcl_parsed_hdr_s */
  zb_zcl_identify_effect_value_param_t param; /**< User App command parameters,
                                        see @ref zb_zcl_identify_effect_value_param_s */
} zb_zcl_identify_effect_user_app_schedule_t;

#define ZB_ZCL_IDENTIFY_EFFECT_SCHEDULE_USER_APP(buffer, pcmd_info, effectId, effectVar)    \
{                                                                                           \
  zb_zcl_identify_effect_user_app_schedule_t* data =                                        \
          ZB_BUF_GET_PARAM((buffer), zb_zcl_identify_effect_user_app_schedule_t);           \
  ZB_MEMMOVE(&(data->cmd_info), (pcmd_info), sizeof(zb_zcl_parsed_hdr_t));                  \
  data->param.effect_id = (effectId);                                                       \
  data->param.effect_variant = (effectVar);                                                 \
  ZB_SCHEDULE_CALLBACK(zb_zcl_identify_effect_invoke_user_app, (buffer)); \
}
/*! @} */ /* internal */

/*! @brief Send Identify command
    @param buffer to put packet to
    @param time the device will be identifying
    @param addr - address to send packet to
    @param dst_addr_mode - addressing mode
    @param dst_ep - destination endpoint
    @param ep - sending endpoint
    @param prfl_id - profile identifier
    @param def_resp - flag "Default response required"
    @param cb - callback for getting command send status
*/
#define ZB_ZCL_IDENTIFY_SEND_IDENTIFY_REQ(                                                     \
    buffer, time, addr, dst_addr_mode, dst_ep, ep, prfl_id, def_resp, cb)                      \
{                                                                                              \
  zb_uint8_t* ptr = ZB_ZCL_START_PACKET_REQ(buffer)                                            \
  ZB_ZCL_CONSTRUCT_SPECIFIC_COMMAND_REQ_FRAME_CONTROL(ptr, def_resp)                           \
  ZB_ZCL_CONSTRUCT_COMMAND_HEADER_REQ(ptr, ZB_ZCL_GET_SEQ_NUM(), ZB_ZCL_CMD_IDENTIFY_IDENTIFY_ID); \
  ZB_ZCL_PACKET_PUT_DATA16_VAL(ptr, time);                                                     \
  ZB_ZCL_FINISH_PACKET(buffer, ptr)                                                            \
  ZB_ZCL_SEND_COMMAND_SHORT(                                                                   \
      buffer, addr, dst_addr_mode, dst_ep, ep, prfl_id, ZB_ZCL_CLUSTER_ID_IDENTIFY, cb);       \
}

/*! @brief Send Identify Query command
    @param buffer to put packet to
    @param addr - address to send packet to
    @param dst_addr_mode - addressing mode
    @param dst_ep - destination endpoint
    @param ep - sending endpoint
    @param prfl_id - profile identifier
    @param def_resp - flag "Default response required"
    @param cb - callback for getting command send status

    @snippet on_off_switch/sample_zed.c identify_send_identify_query_req
*/
#define ZB_ZCL_IDENTIFY_SEND_IDENTIFY_QUERY_REQ(                                         \
    buffer, addr, dst_addr_mode, dst_ep, ep, prfl_id, def_resp, cb)                      \
{                                                                                        \
  zb_uint8_t* ptr = ZB_ZCL_START_PACKET_REQ(buffer)                                         \
  ZB_ZCL_CONSTRUCT_SPECIFIC_COMMAND_REQ_FRAME_CONTROL(ptr, def_resp)                    \
  ZB_ZCL_CONSTRUCT_COMMAND_HEADER_REQ(                                                       \
      ptr, ZB_ZCL_GET_SEQ_NUM(), ZB_ZCL_CMD_IDENTIFY_IDENTIFY_QUERY_ID);                 \
  ZB_ZCL_FINISH_PACKET(buffer, ptr)                                                     \
  ZB_ZCL_SEND_COMMAND_SHORT(                                                             \
      buffer, addr, dst_addr_mode, dst_ep, ep, prfl_id, ZB_ZCL_CLUSTER_ID_IDENTIFY, cb); \
}

/*! @brief Send Identify Query Response command
    @param buffer to put packet to
    @param time the device will be identifying
    @param addr - address to send packet to
    @param dst_addr_mode - addressing mode
    @param dst_ep - destination endpoint
    @param ep - sending endpoint
    @param prfl_id - profile identifier
    @param seq_num - sequence number
    @param aps_secured - APS security mode
*/
#define ZB_ZCL_IDENTIFY_SEND_IDENTIFY_QUERY_RES(                                              \
    buffer, time, addr, dst_addr_mode, dst_ep, ep, prfl_id, seq_num, aps_secured)             \
{                                                                                             \
  zb_uint8_t* ptr = ZB_ZCL_START_PACKET(buffer);                                              \
  ZB_ZCL_CONSTRUCT_SPECIFIC_COMMAND_RES_FRAME_CONTROL(ptr);                                   \
  ZB_ZCL_CONSTRUCT_COMMAND_HEADER(ptr, seq_num, ZB_ZCL_CMD_IDENTIFY_IDENTIFY_QUERY_RSP_ID);   \
  ZB_ZCL_PACKET_PUT_DATA16_VAL(ptr, time);                                                    \
  ZB_ZCL_FINISH_N_SEND_PACKET_NEW(buffer, ptr,                                                \
    addr, dst_addr_mode, dst_ep, ep, prfl_id, ZB_ZCL_CLUSTER_ID_IDENTIFY, NULL, aps_secured,  \
    ZB_FALSE, 0);                                                                             \
}

/**
 * @brief Start identify process on given endpoint
 *
 * @param endpoint - endpoint to start identifying on
 * @param timeout - time (in seconds) after which identifying will stop
 */
zb_uint8_t zb_zcl_start_identifying(zb_uint8_t endpoint, zb_uint16_t timeout);

/**
 * @brief Stop identify process on given endpoint
 * Complimentary function to @ref zb_zcl_start_identifying.
 * Used to force identify termination before timeout provided to
 * @ref zb_zcl_start_identifying is elapsed
 *
 * @param endpoint  Endpoint
 */
void zb_zcl_stop_identifying(zb_uint8_t endpoint);

/**
 *  @brief Indicates that endpoint supports Identify cluster and is identifying
 *  @param endpoint number to analyze
 *  @return ZB_TRUE if endpoint is in the "identifying" state, ZB_FALSE otherwise.
 */
zb_uint8_t zb_zcl_is_identifying(zb_uint8_t endpoint);

/*! @brief Structured representation of Identify command payload */
typedef ZB_PACKED_PRE struct zb_zcl_identify_req_s
{
  zb_uint16_t timeout;  /*!< Time to identify */
} ZB_PACKED_STRUCT zb_zcl_identify_req_t;

/*! @brief Structured representation of Identify Query Response command payload */
typedef ZB_PACKED_PRE struct zb_zcl_identify_query_res_s
{
  zb_uint16_t timeout;  /*!< Reported time to identify */
} ZB_PACKED_STRUCT zb_zcl_identify_query_res_t;

/*! @brief Identify cluster Identify command payload structured read
  * @param data_ptr - pointer to a variable of type @ref zb_zcl_identify_req_t
  * @param buffer containing the packet (by pointer).
  * @param status - variable to put parse status to (see @ref zb_zcl_parse_status_t).
*/
#define ZB_ZCL_IDENTIFY_GET_IDENTIFY_REQ(data_ptr, buffer, status)      \
{                                                                       \
  if (zb_buf_len((buffer)) < sizeof(zb_zcl_identify_req_t))             \
  {                                                                     \
    (status) = ZB_ZCL_PARSE_STATUS_FAILURE;                             \
  }                                                                     \
  else                                                                  \
  {                                                                     \
    zb_zcl_identify_req_t *src_ptr =                                    \
      (zb_zcl_identify_req_t*)zb_buf_begin((buffer));                   \
    (status) = ZB_ZCL_PARSE_STATUS_SUCCESS;                             \
    ZB_HTOLE16(&((data_ptr)->timeout), &(src_ptr->timeout));            \
  }                                                                     \
}

/*! @brief Identify cluster Identify Query Response command payload structured
    read
  * @param data_ptr - pointer to a variable of type @ref zb_zcl_identify_query_res_t
  * @param buffer containing the packet (by pointer).
  * @param status - variable to put parse status to (see @ref zb_zcl_parse_status_t).
  */
#define ZB_ZCL_IDENTIFY_GET_IDENTIFY_QUERY_RES(data_ptr, buffer, status) \
{                                                                        \
  if (zb_buf_len((buffer)) < sizeof(zb_zcl_identify_query_res_t))        \
  {                                                                      \
    (status) = ZB_ZCL_PARSE_STATUS_FAILURE;                              \
  }                                                                      \
  else                                                                   \
  {                                                                      \
    zb_zcl_identify_query_res_t *src_ptr =                               \
      (zb_zcl_identify_query_res_t*)zb_buf_begin((buffer));              \
    (status) = ZB_ZCL_PARSE_STATUS_SUCCESS;                              \
    ZB_HTOLE16(&((data_ptr)->timeout), &(src_ptr->timeout));             \
  }                                                                      \
}

/*! @} */ /* Identify cluster command structures and definitions */

/** @cond internals_doc */
/*!
    @name Identify cluster internals
    @internal
    Internal structures for attribute representation in cluster definitions.
    @{
*/
#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_IDENTIFY_IDENTIFY_TIME_ID(data_ptr)  \
{                                                                               \
  ZB_ZCL_ATTR_IDENTIFY_IDENTIFY_TIME_ID,                                        \
  ZB_ZCL_ATTR_TYPE_U16,                                                         \
  ZB_ZCL_ATTR_ACCESS_READ_WRITE,                                                \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                           \
  (void*) data_ptr                                                              \
}

#if defined ZB_ZCL_SUPPORT_CLUSTER_SCENES
/*! Scenes fieldset length for Identify cluster */
#define ZB_ZCL_CLUSTER_ID_IDENTIFY_SCENE_FIELD_SETS_LENGTH 0
#endif /* defined ZB_ZCL_SUPPORT_CLUSTER_SCENES */

/*! @} */ /* Identify cluster internals */
/*! @}
 *  @endcond */ /* internals_doc */

/** @brief Declare attribute list for Identify cluster
  @param attr_list - attribute list name
  @param identify_time - pointer to variable to store identify time attribute value
*/
#define ZB_ZCL_DECLARE_IDENTIFY_ATTRIB_LIST(attr_list, identify_time)   \
  ZB_ZCL_START_DECLARE_ATTRIB_LIST_CLUSTER_REVISION(attr_list, ZB_ZCL_IDENTIFY) \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_IDENTIFY_IDENTIFY_TIME_ID, (identify_time)) \
  ZB_ZCL_FINISH_DECLARE_ATTRIB_LIST

/**
 *  @brief Identify cluster attributes
 */
typedef struct zb_zcl_identify_attrs_s
{
  /** @copydoc ZB_ZCL_ATTR_IDENTIFY_IDENTIFY_TIME_ID
   * @see ZB_ZCL_ATTR_IDENTIFY_IDENTIFY_TIME_ID
   */
  zb_uint16_t identify_time;
} zb_zcl_identify_attrs_t;


/** @brief Declare attribute list for Identify cluster cluster
 *  @param[in]  attr_list - attribute list variable name
 *  @param[in]  attrs - variable of zb_zcl_identify_attrs_t type (containing Identify cluster attributes)
 */
#define ZB_ZCL_DECLARE_IDENTIFY_ATTR_LIST(attr_list, attrs)   \
  ZB_ZCL_DECLARE_IDENTIFY_ATTRIB_LIST(attr_list, &attrs.identify_time)



#if defined ZB_ENABLE_HA

zb_uint8_t zb_zcl_get_cmd_list_identify(zb_bool_t is_client_generated, zb_uint8_t **cmd_list);

#endif /* defined ZB_ENABLE_HA */

/** @endcond */ /* DOXYGEN_ZCL_SECTION */

void zb_zcl_identify_init_server(void);
void zb_zcl_identify_init_client(void);
#define ZB_ZCL_CLUSTER_ID_IDENTIFY_SERVER_ROLE_INIT zb_zcl_identify_init_server
#define ZB_ZCL_CLUSTER_ID_IDENTIFY_CLIENT_ROLE_INIT zb_zcl_identify_init_client

#endif /* defined(ZB_ZCL_SUPPORT_CLUSTER_IDENTIFY) */

#endif /* ZB_ZCL_IDENTIFY_H */
