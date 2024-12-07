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
/* PURPOSE: On/Off cluster definitions
*/

#ifndef ZB_ZCL_ON_OFF_H
#define ZB_ZCL_ON_OFF_H 1

#include "zcl/zb_zcl_common.h"
#include "zcl/zb_zcl_commands.h"

/** @cond DOXYGEN_ZCL_SECTION */

/** @addtogroup ZB_ZCL_ON_OFF
 *  @{
 *    @details
 *    All commands in the cluster have only request form, and carry no payload.
 *
 *    @par Example
 *    On command sending:
 *    @snippet HA_samples/on_off_output/sample_zed.c onoff_server_send_on_req_param
 *    @snippet HA_samples/on_off_output/sample_zed.c onoff_server_send_on_req_command
 *    @par
 *
 *    Other two commands can be sent in the same way using appropriate macros.
 *
 */

/* Cluster ZB_ZCL_CLUSTER_ID_ON_OFF */

/*! @name On/Off cluster attributes
    @{
*/

/*! @brief On/Off cluster attribute identifiers
    @see ZCL spec, subclause 3.8.2.2
*/
enum zb_zcl_on_off_attr_e
{
  /*! @brief OnOff attribute */
  ZB_ZCL_ATTR_ON_OFF_ON_OFF_ID            = 0,
  /*! Global Scene Control attribute identifier. */
  ZB_ZCL_ATTR_ON_OFF_GLOBAL_SCENE_CONTROL = 0x4000,
  /*! On Time attribute identifier. */
  ZB_ZCL_ATTR_ON_OFF_ON_TIME              = 0x4001,
  /*! Off Wait Time attribute identifier. */
  ZB_ZCL_ATTR_ON_OFF_OFF_WAIT_TIME        = 0x4002,
  /*! The StartUpOnOff attribute SHALL define the desired startup behavior of a lamp
   * device when it is supplied with power and this state SHALL be reflected in the OnOff attribute. */
  ZB_ZCL_ATTR_ON_OFF_START_UP_ON_OFF      = 0x4003,
};

/*! @brief Permissible values for OnOff attribute */
enum zb_zcl_on_off_on_off_e
{
  /*! "Off" value */
  ZB_ZCL_ON_OFF_IS_OFF = 0,
  /*! "On" value */
  ZB_ZCL_ON_OFF_IS_ON  = 1
};

enum zb_zcl_on_off_start_up_on_off_e
{
  /*! "Off" value */
  ZB_ZCL_ON_OFF_START_UP_ON_OFF_IS_OFF         = 0,
  /*! "ON" value */
  ZB_ZCL_ON_OFF_START_UP_ON_OFF_IS_ON          = 1,
  /*! "Toggle" value */
  ZB_ZCL_ON_OFF_START_UP_ON_OFF_IS_TOGGLE      = 2,
  /*! "Previous" value */
  ZB_ZCL_ON_OFF_START_UP_ON_OFF_IS_PREVIOUS = 0xFF
};

/** @brief Default value for OnOff cluster revision global attribute */
#define ZB_ZCL_ON_OFF_CLUSTER_REVISION_DEFAULT ((zb_uint16_t)0x0002u)

/** @brief Default value for OnOff attribute */
#define ZB_ZCL_ON_OFF_ON_OFF_DEFAULT_VALUE (ZB_ZCL_ON_OFF_IS_OFF)

/** @brief Default value for GlobalSceneControl attribute */
#define ZB_ZCL_ON_OFF_GLOBAL_SCENE_CONTROL_DEFAULT_VALUE ((zb_bool_t)0x01)

/** @brief Default value for OnTime attribute */
#define ZB_ZCL_ON_OFF_ON_TIME_DEFAULT_VALUE ((zb_uint16_t)0x0000)

/** @brief Default value for OffWaitTime attribute */
#define ZB_ZCL_ON_OFF_OFF_WAIT_TIME_DEFAULT_VALUE ((zb_uint16_t)0x0000)

/** @brief Declare attribute list for On/Off cluster
    @param attr_list - attribute list name
    @param on_off - pointer to variable to store On/Off attribute value
*/
#define ZB_ZCL_DECLARE_ON_OFF_ATTRIB_LIST(attr_list, on_off)            \
  ZB_ZCL_START_DECLARE_ATTRIB_LIST_CLUSTER_REVISION(attr_list, ZB_ZCL_ON_OFF) \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_ON_OFF_ON_OFF_ID, (on_off))          \
  ZB_ZCL_FINISH_DECLARE_ATTRIB_LIST

/**
 *  @brief Declare attribute list for On/Off cluster (extended attribute set).
 *  @param attr_list [IN] - attribute list name being declared by this macro.
 *  @param on_off [IN] - pointer to a boolean variable storing on/off attribute value.
 *  @param global_scene_ctrl [IN] - pointer to a boolean variable storing global scene control attribute value.
 *  @param on_time [IN] - pointer to a unsigned 16-bit integer variable storing on time attribute value.
 *  @param off_wait_time [IN] - pointer to a unsigned 16-bit integer variable storing off wait time attribute value.
 */
#define ZB_ZCL_DECLARE_ON_OFF_ATTRIB_LIST_EXT(                                                  \
    attr_list, on_off, global_scene_ctrl, on_time, off_wait_time                                \
    )                                                                                           \
    ZB_ZCL_START_DECLARE_ATTRIB_LIST_CLUSTER_REVISION(attr_list, ZB_ZCL_ON_OFF)                 \
    ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_ON_OFF_ON_OFF_ID, (on_off))                                \
    ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_ON_OFF_GLOBAL_SCENE_CONTROL, (global_scene_ctrl))          \
    ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_ON_OFF_ON_TIME, (on_time))                                 \
    ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_ON_OFF_OFF_WAIT_TIME, (off_wait_time))                     \
    ZB_ZCL_FINISH_DECLARE_ATTRIB_LIST

/*! @} */ /* On/Off cluster attributes */

/*! @name On/Off cluster commands
    @{
*/

/*! @brief On/Off cluster command identifiers
    @see ZCL spec, subclause 3.8.2.3
*/
enum zb_zcl_on_off_cmd_e
{
  ZB_ZCL_CMD_ON_OFF_OFF_ID                         = 0x00, /**< "Turn off" command. */
  ZB_ZCL_CMD_ON_OFF_ON_ID                          = 0x01, /**< "Turn on" command. */
  ZB_ZCL_CMD_ON_OFF_TOGGLE_ID                      = 0x02, /**< "Toggle state" command. */
  ZB_ZCL_CMD_ON_OFF_OFF_WITH_EFFECT_ID             = 0x40, /**< "Off with effect" command. */
  ZB_ZCL_CMD_ON_OFF_ON_WITH_RECALL_GLOBAL_SCENE_ID = 0x41, /**< "On with recall global scene" command. */
  ZB_ZCL_CMD_ON_OFF_ON_WITH_TIMED_OFF_ID           = 0x42, /**< "On with timed off" command. */
};

/** @cond internals_doc */
/* ON OFF cluster commands list : only for information - do not modify */
#define ZB_ZCL_CLUSTER_ID_ON_OFF_SERVER_ROLE_GENERATED_CMD_LIST

#define ZB_ZCL_CLUSTER_ID_ON_OFF_CLIENT_ROLE_RECEIVED_CMD_LIST ZB_ZCL_CLUSTER_ID_ON_OFF_SERVER_ROLE_GENERATED_CMD_LIST

#define ZB_ZCL_CLUSTER_ID_ON_OFF_CLIENT_ROLE_GENERATED_CMD_LIST                         \
                                      ZB_ZCL_CMD_ON_OFF_OFF_ID,                         \
                                      ZB_ZCL_CMD_ON_OFF_ON_ID,                          \
                                      ZB_ZCL_CMD_ON_OFF_TOGGLE_ID,                      \
                                      ZB_ZCL_CMD_ON_OFF_OFF_WITH_EFFECT_ID,             \
                                      ZB_ZCL_CMD_ON_OFF_ON_WITH_RECALL_GLOBAL_SCENE_ID, \
                                      ZB_ZCL_CMD_ON_OFF_ON_WITH_TIMED_OFF_ID

#define ZB_ZCL_CLUSTER_ID_ON_OFF_SERVER_ROLE_RECEIVED_CMD_LIST ZB_ZCL_CLUSTER_ID_ON_OFF_CLIENT_ROLE_GENERATED_CMD_LIST

/** @endcond */ /* internals_doc */

/*! @brief General macro for sending On/Off cluster command
    @param buffer to put data to
    @param addr - address of the device to send command to
    @param dst_addr_mode - addressing mode
    @param dst_ep destination endpoint
    @param ep - current endpoint
    @param prof_id - profile identifier
    @param dis_default_resp - "Disable default response" flag
    @param command_id - command identifier
    @param cb - callback to call to report send status
*/
#define ZB_ZCL_ON_OFF_SEND_REQ(                                                         \
    buffer, addr, dst_addr_mode, dst_ep, ep, prof_id, dis_default_resp, command_id, cb) \
{                                                                                       \
  zb_uint8_t* ptr = ZB_ZCL_START_PACKET_REQ(buffer)                                     \
  ZB_ZCL_CONSTRUCT_SPECIFIC_COMMAND_REQ_FRAME_CONTROL(ptr, dis_default_resp)            \
  ZB_ZCL_CONSTRUCT_COMMAND_HEADER_REQ(ptr, ZB_ZCL_GET_SEQ_NUM(), command_id);           \
  ZB_ZCL_FINISH_PACKET(buffer, ptr)                                                     \
  ZB_ZCL_SEND_COMMAND_SHORT(                                                            \
      buffer, addr, dst_addr_mode, dst_ep, ep, prof_id, ZB_ZCL_CLUSTER_ID_ON_OFF, cb);  \
}

/*! Specific macro for sending Off command */
#define ZB_ZCL_ON_OFF_SEND_OFF_REQ(                                         \
    buffer, addr, dst_addr_mode, dst_ep, ep, prfl_id, dis_default_resp, cb) \
  ZB_ZCL_ON_OFF_SEND_REQ(                                                   \
      buffer,                                                               \
      addr,                                                                 \
      dst_addr_mode,                                                        \
      dst_ep,                                                               \
      ep,                                                                   \
      prfl_id,                                                              \
      dis_default_resp,                                                     \
      ZB_ZCL_CMD_ON_OFF_OFF_ID,                                             \
      cb)

/*! Specific macro for sending On command */
#define ZB_ZCL_ON_OFF_SEND_ON_REQ(                                          \
    buffer, addr, dst_addr_mode, dst_ep, ep, prfl_id, dis_default_resp, cb) \
  ZB_ZCL_ON_OFF_SEND_REQ(                                                   \
      buffer,                                                               \
      addr,                                                                 \
      dst_addr_mode,                                                        \
      dst_ep,                                                               \
      ep,                                                                   \
      prfl_id,                                                              \
      dis_default_resp,                                                     \
      ZB_ZCL_CMD_ON_OFF_ON_ID,                                              \
      cb)

/*! Specific macro for sending Toggle command */
#define ZB_ZCL_ON_OFF_SEND_TOGGLE_REQ(                                      \
    buffer, addr, dst_addr_mode, dst_ep, ep, prfl_id, dis_default_resp, cb) \
  ZB_ZCL_ON_OFF_SEND_REQ(                                                   \
      buffer,                                                               \
      addr,                                                                 \
      dst_addr_mode,                                                        \
      dst_ep,                                                               \
      ep,                                                                   \
      prfl_id,                                                              \
      dis_default_resp,                                                     \
      ZB_ZCL_CMD_ON_OFF_TOGGLE_ID,                                          \
      cb)

/*! Specific macro for sending Off with effect command */
#define ZB_ZCL_ON_OFF_SEND_OFF_WITH_EFFECT_REQ(                                                 \
  buffer, addr, dst_addr_mode, dst_ep, ep, prof_id, dis_default_resp, effect_id, effect_var, cb)\
{                                                                                               \
  zb_uint8_t* ptr = ZB_ZCL_START_PACKET_REQ(buffer)                                             \
  ZB_ZCL_CONSTRUCT_SPECIFIC_COMMAND_REQ_FRAME_CONTROL(ptr, dis_default_resp)                    \
  ZB_ZCL_CONSTRUCT_COMMAND_HEADER_REQ(ptr, ZB_ZCL_GET_SEQ_NUM(), ZB_ZCL_CMD_ON_OFF_OFF_WITH_EFFECT_ID); \
  ZB_ZCL_PACKET_PUT_DATA8(ptr, effect_id);                                                      \
  ZB_ZCL_PACKET_PUT_DATA8(ptr, effect_var);                                                     \
  ZB_ZCL_FINISH_PACKET(buffer, ptr)                                                             \
  ZB_ZCL_SEND_COMMAND_SHORT(buffer, addr, dst_addr_mode, dst_ep, ep, prof_id, ZB_ZCL_CLUSTER_ID_ON_OFF, cb); \
}

/*! Specific macro for sending On with recall global scene command */
#define ZB_ZCL_ON_OFF_SEND_ON_WITH_RECALL_GLOBAL_SCENE_REQ(                                     \
  buffer, addr, dst_addr_mode, dst_ep, ep, prof_id, dis_default_resp, cb)                       \
  ZB_ZCL_ON_OFF_SEND_REQ(buffer, addr, dst_addr_mode, dst_ep, ep, prof_id, dis_default_resp,    \
  ZB_ZCL_CMD_ON_OFF_ON_WITH_RECALL_GLOBAL_SCENE_ID, cb)

/*! Specific macro for sending On with timed off command */
#define ZB_ZCL_ON_OFF_SEND_ON_WITH_TIMED_OFF_REQ(                                               \
  buffer, addr, dst_addr_mode, dst_ep, ep, prof_id, dis_default_resp, on_off, on_time, off_wait_time, cb)\
{                                                                                               \
  zb_uint8_t* ptr = ZB_ZCL_START_PACKET_REQ(buffer)                                             \
  ZB_ZCL_CONSTRUCT_SPECIFIC_COMMAND_REQ_FRAME_CONTROL(ptr, dis_default_resp)                    \
  ZB_ZCL_CONSTRUCT_COMMAND_HEADER_REQ(ptr, ZB_ZCL_GET_SEQ_NUM(), ZB_ZCL_CMD_ON_OFF_ON_WITH_TIMED_OFF_ID); \
  ZB_ZCL_PACKET_PUT_DATA8(ptr, on_off);                                                         \
  ZB_ZCL_PACKET_PUT_DATA16_VAL(ptr, on_time);                                                   \
  ZB_ZCL_PACKET_PUT_DATA16_VAL(ptr, off_wait_time);                                             \
  ZB_ZCL_FINISH_PACKET(buffer, ptr)                                                             \
  ZB_ZCL_SEND_COMMAND_SHORT(buffer, addr, dst_addr_mode, dst_ep, ep, prof_id, ZB_ZCL_CLUSTER_ID_ON_OFF, cb); \
}

/** Effect identifier enum
 * @see ZCL spec 3.8.2.3.4.1 */
enum zb_zcl_on_off_effect_e
{
/**< Effect identifier field value: Delayed all off */
  ZB_ZCL_ON_OFF_EFFECT_ID_DELAYED_ALL_OFF = 0x00,
/**< Effect identifier field value: Dying light */
  ZB_ZCL_ON_OFF_EFFECT_ID_DYING_LIGHT = 0x01
};

/** Effect variant field for delayed enum
* @see ZCL spec 3.8.2.3.4.2 */
enum zb_zcl_on_off_effect_variant_delayed_e
{
/**< Effect variant field value: Fade to off in 0.8 seconds */
  ZB_ZCL_ON_OFF_EFFECT_VARIANT_FADE = 0x00,
/**< Effect variant field value: No fade */
  ZB_ZCL_ON_OFF_EFFECT_VARIANT_NO_FADE = 0x01,
/**< Effect variant field value: 50% dim down in 0.8 seconds then fade to off in 12 seconds */
  ZB_ZCL_ON_OFF_EFFECT_VARIANT_50PART_FADE = 0x02
};

/** Effect variant field for delayed enum
* @see ZCL spec 3.8.2.3.4.2 */
enum zb_zcl_on_off_effect_variant_dying_e
{
/**< Effect variant field value: 20% dim up in 0.5s then fade to off in 1 second */
  ZB_ZCL_ON_OFF_EFFECT_VARIANT_20PART_FADE = 0x00
};

/** Accept only when on
* @see ZCL spec 3.8.2.3.6.1 */
#define ZB_ZCL_ON_OFF_ACCEPT_ONLY_WHEN_ON   1

/*! @brief Structured representation of Off with effect command payload */
typedef ZB_PACKED_PRE struct zb_zcl_on_off_off_with_effect_req_s
{
  /** Effect identify */
  zb_uint8_t effect_id;
  /** Effect variant */
  zb_uint8_t effect_variant;
} ZB_PACKED_STRUCT zb_zcl_on_off_off_with_effect_req_t;

/*! @brief Structured representation of On with timed off command payload */
typedef ZB_PACKED_PRE struct zb_zcl_on_off_on_with_timed_off_req_s
{
  /** On/off control */
  zb_uint8_t on_off;
  /** On time variable */
  zb_uint16_t on_time;
  /** Off wait time variable */
  zb_uint16_t off_wait_time;
} ZB_PACKED_STRUCT zb_zcl_on_off_on_with_timed_off_req_t;

/** @brief Parses Off with Effect command
  * @attention Assumes that ZCL header already cut.
  * @param data_ptr - pointer to a variable of type @ref
  * zb_zcl_on_off_off_with_effect_req_s.
  * @param buffer containing the packet (by pointer).
  * @param status - variable to put parse status to (see @ref zb_zcl_parse_status_t).
  */
#define ZB_ZCL_ON_OFF_GET_OFF_WITH_EFFECT_REQ(data_ptr, buffer, status)             \
{                                                                                   \
  if (zb_buf_len((buffer)) < sizeof(zb_zcl_on_off_off_with_effect_req_t))           \
  {                                                                                 \
   (status) = ZB_ZCL_PARSE_STATUS_FAILURE;                                          \
  }                                                                                 \
  else                                                                              \
  {                                                                                 \
    zb_zcl_on_off_off_with_effect_req_t *src_ptr =                                  \
                      (zb_zcl_on_off_off_with_effect_req_t*)zb_buf_begin((buffer)); \
    (status) = ZB_ZCL_PARSE_STATUS_SUCCESS;                                         \
    ZB_MEMCPY((data_ptr), src_ptr, sizeof(zb_zcl_on_off_off_with_effect_req_t));    \
  }                                                                                 \
}

/** @brief Parses On with timed off command
  * @attention Assumes that ZCL header already cut.
  * @param data_ptr - pointer to a variable of type @ref
  * zb_zcl_on_off_on_with_timed_off_req_s.
  * @param buffer containing the packet (by pointer).
  * @param status - variable to put parse status to (see @ref zb_zcl_parse_status_t).
  */
#define ZB_ZCL_ON_OFF_GET_ON_WITH_TIMED_OFF_REQ(data_ptr, buffer, status)           \
{                                                                                   \
  if (zb_buf_len((buffer)) < sizeof(zb_zcl_on_off_on_with_timed_off_req_t))         \
  {                                                                                 \
   (status) = ZB_ZCL_PARSE_STATUS_FAILURE;                                          \
  }                                                                                 \
  else                                                                              \
  {                                                                                 \
    zb_zcl_on_off_on_with_timed_off_req_t *src_ptr =                                \
              (zb_zcl_on_off_on_with_timed_off_req_t*)zb_buf_begin((buffer));       \
    (status) = ZB_ZCL_PARSE_STATUS_SUCCESS;                                         \
    (data_ptr)->on_off = src_ptr->on_off;                                           \
    ZB_LETOH16(&((data_ptr)->on_time), &(src_ptr->on_time));                        \
    ZB_LETOH16(&((data_ptr)->off_wait_time), &(src_ptr->off_wait_time));            \
  }                                                                                 \
}

/** @cond internals_doc */
/**
 *  @name Inform User App about On/Off cluster command and change attributes.
 *  Internal structures and define-procedure for inform User App about On/Off
 *  cluster command and change attributes.
 *  @internal
 *  @{
 */

/** @brief Declare run Set Effect command for User Application
*/
typedef struct zb_zcl_on_off_set_effect_value_param_s
{
  zb_uint8_t effect_id;         /*!< Effect identify */
  zb_uint8_t effect_variant;    /*!< Effect variant */
} zb_zcl_on_off_set_effect_value_param_t;

/*! Struct for invoke User App & continue after */
typedef struct zb_zcl_on_off_effect_user_app_schedule_e
{
  zb_zcl_parsed_hdr_t cmd_info;                 /**< Parameters for continue command,
                                                     see @ref zb_zcl_parsed_hdr_s */
  zb_zcl_on_off_set_effect_value_param_t param; /**< User App command parameters,
                                                     see @ref zb_zcl_on_off_set_effect_value_param_s */
} zb_zcl_on_off_effect_user_app_schedule_t;

#define ZB_ZCL_ON_OFF_EFFECT_SCHEDULE_USER_APP(buffer, pcmd_info, effectId, effectVar)      \
{                                                                                           \
  zb_zcl_on_off_effect_user_app_schedule_t* user_data =                                     \
          ZB_BUF_GET_PARAM((buffer), zb_zcl_on_off_effect_user_app_schedule_t);             \
  ZB_MEMMOVE(&(user_data->cmd_info), (pcmd_info), sizeof(zb_zcl_parsed_hdr_t));             \
  user_data->param.effect_id = (effectId);                                                  \
  user_data->param.effect_variant = (effectVar);                                            \
  ZB_SCHEDULE_CALLBACK(zb_zcl_on_off_effect_invoke_user_app, (buffer));                     \
}

/*! @}
 *  @endcond */ /* internals_doc */

/*! @} */ /* On/Off cluster commands */

/*! @cond internals_doc
    @internal @name On/Off cluster internals
    Internal structures for attribute representation in cluster definitions.
    @{
*/

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_ON_OFF_ON_OFF_ID(data_ptr)                        \
{                                                                                            \
  ZB_ZCL_ATTR_ON_OFF_ON_OFF_ID,                                                              \
  ZB_ZCL_ATTR_TYPE_BOOL,                                                                     \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY | ZB_ZCL_ATTR_ACCESS_REPORTING | ZB_ZCL_ATTR_ACCESS_SCENE,    \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                                        \
  (void*) data_ptr                                                                           \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_ON_OFF_GLOBAL_SCENE_CONTROL(data_ptr) \
{                                                                                \
  ZB_ZCL_ATTR_ON_OFF_GLOBAL_SCENE_CONTROL,                                       \
  ZB_ZCL_ATTR_TYPE_BOOL,                                                         \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                                  \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                            \
  (void*) data_ptr                                                               \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_ON_OFF_ON_TIME(data_ptr)  \
{                                                                    \
  ZB_ZCL_ATTR_ON_OFF_ON_TIME,                                        \
  ZB_ZCL_ATTR_TYPE_U16,                                              \
  ZB_ZCL_ATTR_ACCESS_READ_WRITE,                                     \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                \
  (void*) data_ptr                                                   \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_ON_OFF_OFF_WAIT_TIME(data_ptr)    \
{                                                                            \
  ZB_ZCL_ATTR_ON_OFF_OFF_WAIT_TIME,                                          \
  ZB_ZCL_ATTR_TYPE_U16,                                                      \
  ZB_ZCL_ATTR_ACCESS_READ_WRITE,                                             \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                        \
  (void*) data_ptr                                                           \
}

/** @struct zb_zcl_on_off_attrs_s
 *  @brief On/Off cluster attributes
 */
typedef struct zb_zcl_on_off_attrs_s
{
  zb_uint8_t on_off;
} zb_zcl_on_off_attrs_t;

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_ON_OFF_START_UP_ON_OFF(data_ptr)  \
{                                                                            \
  ZB_ZCL_ATTR_ON_OFF_START_UP_ON_OFF,                                        \
  ZB_ZCL_ATTR_TYPE_8BIT_ENUM,                                                \
  ZB_ZCL_ATTR_ACCESS_READ_WRITE,                                             \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                        \
  (void*) data_ptr                                                           \
}

#define ZB_ZCL_DECLARE_ON_OFF_ATTR_LIST(attr_list, attrs)                    \
  ZB_ZCL_DECLARE_ON_OFF_ATTRIB_LIST(attr_list, &attrs.on_off)

/*! @internal Number of attributes mandatory for reporting in On/Off cluster */
#define ZB_ZCL_ON_OFF_REPORT_ATTR_COUNT 1

/*! @internal Struct for invoke User App & continue after */
typedef struct zb_on_off_user_app_schedule_e
{
  zb_zcl_parsed_hdr_t cmd_info;
  zb_uint8_t new_value;
  zb_bool_t is_run_timer;
} zb_on_off_user_app_schedule_t;


/*! @}
    @endcond */ /* On/Off cluster internals */

/*! @} */ /* ZCL On/Off cluster definitions */

/** @endcond */ /* DOXYGEN_ZCL_SECTION */

void zb_zcl_on_off_init_server(void);
void zb_zcl_on_off_init_client(void);
#define ZB_ZCL_CLUSTER_ID_ON_OFF_SERVER_ROLE_INIT zb_zcl_on_off_init_server
#define ZB_ZCL_CLUSTER_ID_ON_OFF_CLIENT_ROLE_INIT zb_zcl_on_off_init_client

#endif /* ZB_ZCL_ON_OFF_H */
