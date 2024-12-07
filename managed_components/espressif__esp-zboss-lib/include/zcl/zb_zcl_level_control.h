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
/* PURPOSE: Level control cluster definitions
*/

#ifndef ZB_ZCL_LEVEL_CONTROL_H
#define ZB_ZCL_LEVEL_CONTROL_H 1

#include "zcl/zb_zcl_common.h"
#include "zcl/zb_zcl_commands.h"

/** @cond DOXYGEN_ZCL_SECTION */

/** @addtogroup ZB_ZCL_LEVEL_CONTROL
    @{
*/

/* ZB_ZCL_CLUSTER_ID_LEVEL_CONTROL = 0x0008 defined in zb_zcl_common.h */

/*! @name Level Control cluster attributes
    @{
*/

/** @brief Level Control cluster information attribute identifiers
    @see ZCL spec, subclause 3.10.2.2
*/

/**
 * @brief Level control attribute list
 */
enum zb_zcl_level_control_attr_e
{
  /** @brief Current Level attribute */
  ZB_ZCL_ATTR_LEVEL_CONTROL_CURRENT_LEVEL_ID          = 0x0000,
  /** @brief Remaining Time attribute */
  ZB_ZCL_ATTR_LEVEL_CONTROL_REMAINING_TIME_ID         = 0x0001,
  /** @brief On Off Transition Time attribute */
  /** The MinLevel attribute indicates the minimum value of CurrentLevel that is capable
   *  of being assigned. */
  ZB_ZCL_ATTR_LEVEL_CONTROL_MIN_LEVEL_ID              = 0x0002,
  /** The MaxLevel attribute indicates the maximum value of CurrentLevel that is capable
   *  of being assigned. */
  ZB_ZCL_ATTR_LEVEL_CONTROL_MAX_LEVEL_ID              = 0x0003,
  /** The CurrentFrequency attribute represents the frequency that the devices is
   *  at CurrentLevel. */
  ZB_ZCL_ATTR_LEVEL_CONTROL_CURRENT_FREQUENCY_ID      = 0x0004,
  /** The MinFrequency attribute indicates the minimum value of CurrentFrequency that
   *  is capable of being assigned. */
  ZB_ZCL_ATTR_LEVEL_CONTROL_MIN_FREQUENCY_ID          = 0x0005,
  /** The MaxFrequency attribute indicates the maximum value of CurrentFrequency that
   *  is capable of being assigned. */
  ZB_ZCL_ATTR_LEVEL_CONTROL_MAX_FREQUENCY_ID          = 0x0006,

  ZB_ZCL_ATTR_LEVEL_CONTROL_ON_OFF_TRANSITION_TIME_ID = 0x0010,
  /** @brief On Level attribute */

  /* TODO: A close relationship between this cluster and the On/Off
   * cluster is needed. Refer to ZCL spec, subclause 3.10.2.1 */
  ZB_ZCL_ATTR_LEVEL_CONTROL_ON_LEVEL_ID               = 0x0011,

  /** The OnTransitionTime attribute represents the time taken to move the current level
   *  from the minimum level to the maximum level when an On command is received by
   *  an On/Off cluster on the same endpoint. */
  ZB_ZCL_ATTR_LEVEL_CONTROL_ON_TRANSITION_TIME_ID     = 0x0012,
  /** The OffTransitionTime attribute represents the time taken to move the current level
   *  from the maximum level to the minimum level when an Off command is received by
   *  an On/Off cluster on the same endpoint. */
  ZB_ZCL_ATTR_LEVEL_CONTROL_OFF_TRANSITION_TIME_ID    = 0x0013,
  /** The DefaultMoveRate attribute determines the movement rate, in units per second,
   *  when a Move command is received with a Rate parameter of 0xFF. */
  ZB_ZCL_ATTR_LEVEL_CONTROL_DEFAULT_MOVE_RATE_ID      = 0x0014,
  /** The Options attribute is a bitmap that determines the default behavior of some
   *  cluster commands. */
  ZB_ZCL_ATTR_LEVEL_CONTROL_OPTIONS_ID                = 0x000F,
  /** The StartUpCurrentLevel attribute SHALL define the desired startup level for a
   *  device when it is supplied with power and this level SHALL be reflected in
   *  the CurrentLevel attribute. */
  ZB_ZCL_ATTR_LEVEL_CONTROL_START_UP_CURRENT_LEVEL_ID = 0x4000,

  /** @internal Special Move Variables attribute */
  ZB_ZCL_ATTR_LEVEL_CONTROL_MOVE_STATUS_ID            = 0xefff
};

/**
 * @brief Level control Options attribute, ZCL spec 3.10.2.2.8
 */
enum zb_zcl_level_control_options_e
{
  /** @brief ExecuteIfOff bit */
  ZB_ZCL_LEVEL_CONTROL_OPTIONS_EXECUTE_IF_OFF    = 0,
  ZB_ZCL_LEVEL_CONTROL_OPTIONS_RESERVED          = 1,
};

/** @brief Default value for Level control cluster revision global attribute */
#define ZB_ZCL_LEVEL_CONTROL_CLUSTER_REVISION_DEFAULT ((zb_uint16_t)0x0003u)

/** @brief Maximal value for implemented Level control cluster revision global attribute */
#define ZB_ZCL_LEVEL_CONTROL_CLUSTER_REVISION_MAX ZB_ZCL_LEVEL_CONTROL_CLUSTER_REVISION_DEFAULT

/** @brief Current Level attribute minimum value */
#define ZB_ZCL_LEVEL_CONTROL_LEVEL_MIN_VALUE 0x00

/** @brief Current Level attribute maximum value */
#define ZB_ZCL_LEVEL_CONTROL_LEVEL_MAX_VALUE 0xff
/** @brief Current Level attribute maximum value */
#define ZB_ZCL_LEVEL_CONTROL_LEVEL_MAX_VALUE_SHADE 0xff

/** @brief Current Level attribute default value */
#define ZB_ZCL_LEVEL_CONTROL_CURRENT_LEVEL_DEFAULT_VALUE ((zb_uint8_t)0xFF)
/** @brief Default value for RemainingTime attribute */
#define ZB_ZCL_LEVEL_CONTROL_REMAINING_TIME_DEFAULT_VALUE ((zb_uint16_t)0x0000)

/** @brief Default value for MinLevel attribute */
#define ZB_ZCL_LEVEL_CONTROL_MIN_LEVEL_DEFAULT_VALUE ((zb_uint8_t)0x00)

/** @brief Default value for MaxLevel attribute */
#define ZB_ZCL_LEVEL_CONTROL_MAX_LEVEL_DEFAULT_VALUE ((zb_uint8_t)0xFF)

/** @brief Default value for CurrentFrequency attribute */
#define ZB_ZCL_LEVEL_CONTROL_CURRENT_FREQUENCY_DEFAULT_VALUE ((zb_uint16_t)0x0000)

/** @brief Default value for MinFrequency attribute */
#define ZB_ZCL_LEVEL_CONTROL_MIN_FREQUENCY_DEFAULT_VALUE ((zb_uint16_t)0x0000)

/** @brief Default value for MaxFrequency attribute */
#define ZB_ZCL_LEVEL_CONTROL_MAX_FREQUENCY_DEFAULT_VALUE ((zb_uint16_t)0x0000)

/** @brief Default value for OnOffTransitionTime attribute */
#define ZB_ZCL_LEVEL_CONTROL_ON_OFF_TRANSITION_TIME_DEFAULT_VALUE ((zb_uint16_t)0x0000)

/** @brief Default value for OnLevel attribute */
#define ZB_ZCL_LEVEL_CONTROL_ON_LEVEL_DEFAULT_VALUE ((zb_uint8_t)0xFF)

/** @brief Default value for OnTransitionTime attribute */
#define ZB_ZCL_LEVEL_CONTROL_ON_TRANSITION_TIME_DEFAULT_VALUE ((zb_uint16_t)0xFFFF)

/** @brief Default value for OffTransitionTime attribute */
#define ZB_ZCL_LEVEL_CONTROL_OFF_TRANSITION_TIME_DEFAULT_VALUE ((zb_uint16_t)0xFFFF)

/** @brief Default value for Options attribute */
#define ZB_ZCL_LEVEL_CONTROL_OPTIONS_DEFAULT_VALUE ((zb_uint8_t)0x00)

/** @brief Default value for OnLevel attribute */
#define ZB_ZCL_LEVEL_CONTROL_START_UP_CURRENT_LEVEL_USE_PREVIOUS_VALUE ((zb_uint8_t)0xFF)

/** @brief Transition Time as fast as able value
 see ZCL spec 3.10.2.3.1.2 */
#define ZB_ZCL_LEVEL_CONTROL_TRANSITION_TIME_AS_FAST_AS_ABLE 0xffff

//#define ZB_ZCL_LEVEL_CONTROL_TRANSITION_TIME_ERROR 20

/** @cond internals_doc */
#define ZB_ZCL_LEVEL_CONTROL_RATE_AS_FAST_AS_ABLE 0xff
/*! @}
 *  @endcond */ /* internals_doc */

/*!
  @brief Declare attribute list for Level Control cluster
  @param attr_list - attribute list name
  @param current_level - pointer to variable to store current_level attribute value
  @param remaining_time - pointer to variable to store remaining_time attribute value
*/
#define ZB_ZCL_DECLARE_LEVEL_CONTROL_ATTRIB_LIST(attr_list, current_level, remaining_time)  \
  zb_zcl_level_control_move_status_t move_status_data_ctx_## attr_list;                     \
  ZB_ZCL_START_DECLARE_ATTRIB_LIST_CLUSTER_REVISION(attr_list, ZB_ZCL_LEVEL_CONTROL)        \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_LEVEL_CONTROL_CURRENT_LEVEL_ID, (current_level))         \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_LEVEL_CONTROL_REMAINING_TIME_ID, (remaining_time))       \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_LEVEL_CONTROL_MOVE_STATUS_ID,                            \
                       (&(move_status_data_ctx_## attr_list)))                              \
  ZB_ZCL_FINISH_DECLARE_ATTRIB_LIST


/*!
  @brief Declare attribute list for Level Control cluster
  @param attr_list - attribute list name
  @param current_level - pointer to variable to store current_level attribute value
  @param remaining_time - pointer to variable to store remaining_time attribute value
  @param options - pointer to variable to store options attribute value
  @param start_up_current_level - pointer to variable to store start_up_current_level attribute value
*/
#define ZB_ZCL_DECLARE_LEVEL_CONTROL_ATTRIB_LIST_EXT(attr_list, current_level, remaining_time,        \
                                                 start_up_current_level, options)                     \
  zb_zcl_level_control_move_status_t move_status_data_ctx## _attr_list;                               \
  ZB_ZCL_START_DECLARE_ATTRIB_LIST_CLUSTER_REVISION(attr_list, ZB_ZCL_LEVEL_CONTROL)                  \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_LEVEL_CONTROL_CURRENT_LEVEL_ID, (current_level))                   \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_LEVEL_CONTROL_REMAINING_TIME_ID, (remaining_time))                 \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_LEVEL_CONTROL_OPTIONS_ID, (options))                               \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_LEVEL_CONTROL_START_UP_CURRENT_LEVEL_ID, (start_up_current_level)) \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_LEVEL_CONTROL_MOVE_STATUS_ID,                                      \
                       (&(move_status_data_ctx## _attr_list)))                                        \
  ZB_ZCL_FINISH_DECLARE_ATTRIB_LIST

/*! @} */ /* Level Control cluster attributes */

/** @cond internals_doc */
/*! @name Level Control cluster internals
    Internal structures for Level Control cluster
    @internal
    @{
*/

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_LEVEL_CONTROL_CURRENT_LEVEL_ID(data_ptr)       \
{                                                                                         \
  ZB_ZCL_ATTR_LEVEL_CONTROL_CURRENT_LEVEL_ID,                                             \
  ZB_ZCL_ATTR_TYPE_U8,                                                                    \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY | ZB_ZCL_ATTR_ACCESS_REPORTING | ZB_ZCL_ATTR_ACCESS_SCENE, \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                                     \
  (void*) data_ptr                                                                        \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_LEVEL_CONTROL_REMAINING_TIME_ID(data_ptr)\
{                                                                                   \
  ZB_ZCL_ATTR_LEVEL_CONTROL_REMAINING_TIME_ID,                                      \
  ZB_ZCL_ATTR_TYPE_U16,                                                             \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                                     \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                               \
  (void*) data_ptr                                                                  \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_LEVEL_CONTROL_OPTIONS_ID(data_ptr)      \
{                                                                                  \
  ZB_ZCL_ATTR_LEVEL_CONTROL_OPTIONS_ID,                                            \
  ZB_ZCL_ATTR_TYPE_8BITMAP,                                                        \
  ZB_ZCL_ATTR_ACCESS_READ_WRITE,                                                   \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                              \
  (void*) data_ptr                                                                 \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_LEVEL_CONTROL_MOVE_STATUS_ID(data_ptr) \
{                                                                                 \
  ZB_ZCL_ATTR_LEVEL_CONTROL_MOVE_STATUS_ID,                                       \
  ZB_ZCL_ATTR_TYPE_NULL,                                                          \
  ZB_ZCL_ATTR_ACCESS_INTERNAL,                                                    \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                             \
  (void*) data_ptr                                                                \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_LEVEL_CONTROL_START_UP_CURRENT_LEVEL_ID(data_ptr) \
{                                                                                            \
  ZB_ZCL_ATTR_LEVEL_CONTROL_START_UP_CURRENT_LEVEL_ID,                                       \
  ZB_ZCL_ATTR_TYPE_U8,                                                                       \
  ZB_ZCL_ATTR_ACCESS_READ_WRITE,                                                             \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                                        \
  (void*) data_ptr                                                                           \
}
/** @internal Structure of addr variables for Move commands
 */
typedef struct zb_zcl_level_control_move_addr_s
{
  zb_uint16_t src_addr;
  zb_uint8_t  src_endpoint;
  zb_uint8_t  dst_endpoint;
  zb_uint8_t  cmd_id;
  zb_uint8_t  seq_number;
  zb_bool_t disable_default_response;
  zb_uint16_t profile_id;
} zb_zcl_level_control_move_addr_t;

/** @internal Structure of Move variables for Move alarm
 */
typedef struct zb_zcl_level_control_move_variables_s
{
  /** @brief Level to set */
  zb_uint8_t curr_level;
  /** @brief End level */
  zb_uint8_t end_level;
  /** @brief On/Off flag: ZB_TRUE - need to setting On/Off attribute, ZB_FALSE - do not need */
  zb_bool_t is_onoff;
  /** @brief Address for response sending */
  zb_zcl_level_control_move_addr_t addr;
  /** @brief Start time for set Remaining Time attribute */
  zb_uint16_t start_time;
} zb_zcl_level_control_move_variables_t;

/** @internal Structure of Move Status attribute for Move alarm
 */
typedef struct zb_zcl_level_control_move_status_s
{
  zb_zcl_level_control_move_variables_t move_var;
  zb_uint8_t buf_id;
} zb_zcl_level_control_move_status_t;

/*! Number of attributes mandatory for reporting in Level Control cluster */
#define ZB_ZCL_LEVEL_CONTROL_REPORT_ATTR_COUNT 1

/*! @} */ /* Level Control cluster internals */
/*! @}
 *  @endcond */ /* internals_doc */

/*! @name Level Control cluster commands
    @{
*/

/*! @brief Level control cluster command identifiers
    @see ZCL spec, subclause 3.10.2.3
*/
enum zb_zcl_level_control_cmd_e
{
  /** Move To Level command */
  ZB_ZCL_CMD_LEVEL_CONTROL_MOVE_TO_LEVEL             = 0x00,
  /** Move command */
  ZB_ZCL_CMD_LEVEL_CONTROL_MOVE                      = 0x01,
  /** Step command */
  ZB_ZCL_CMD_LEVEL_CONTROL_STEP                      = 0x02,
  /** Stop command */
  ZB_ZCL_CMD_LEVEL_CONTROL_STOP                      = 0x03,
  /** Move To Level with On/Off command */
  ZB_ZCL_CMD_LEVEL_CONTROL_MOVE_TO_LEVEL_WITH_ON_OFF = 0x04,
  /** Move with On/Off command */
  ZB_ZCL_CMD_LEVEL_CONTROL_MOVE_WITH_ON_OFF          = 0x05,
  /** Step with On/Off command */
  ZB_ZCL_CMD_LEVEL_CONTROL_STEP_WITH_ON_OFF          = 0x06,
  /** Step with On/Off command */
  ZB_ZCL_CMD_LEVEL_CONTROL_STOP_WITH_ON_OFF          = 0x07,
  /** Upon receipt of "Move to Closest Frequency" command, the device shall change
   *   its current frequency to the requested frequency, or to the closest frequency
   *  that it can generate */
  ZB_ZCL_CMD_LEVEL_CONTROL_MOVE_TO_CLOSEST_FREQUENCY = 0x08,
};

/** @cond internals_doc */
/* LEVEL control cluster commands list : only for information - do not modify */
#define ZB_ZCL_CLUSTER_ID_LEVEL_SERVER_ROLE_GENERATED_CMD_LIST

#define ZB_ZCL_CLUSTER_ID_LEVEL_CLIENT_ROLE_RECEIVED_CMD_LIST ZB_ZCL_CLUSTER_ID_LEVEL_SERVER_ROLE_GENERATED_CMD_LIST

#define ZB_ZCL_CLUSTER_ID_LEVEL_CLIENT_ROLE_GENERATED_CMD_LIST                            \
                                      ZB_ZCL_CMD_LEVEL_CONTROL_MOVE_TO_LEVEL,             \
                                      ZB_ZCL_CMD_LEVEL_CONTROL_MOVE,                      \
                                      ZB_ZCL_CMD_LEVEL_CONTROL_STEP,                      \
                                      ZB_ZCL_CMD_LEVEL_CONTROL_STOP,                      \
                                      ZB_ZCL_CMD_LEVEL_CONTROL_MOVE_TO_LEVEL_WITH_ON_OFF, \
                                      ZB_ZCL_CMD_LEVEL_CONTROL_MOVE_WITH_ON_OFF,          \
                                      ZB_ZCL_CMD_LEVEL_CONTROL_STEP_WITH_ON_OFF,          \
                                      ZB_ZCL_CMD_LEVEL_CONTROL_STOP_WITH_ON_OFF

#define ZB_ZCL_CLUSTER_ID_LEVEL_SERVER_ROLE_RECEIVED_CMD_LIST ZB_ZCL_CLUSTER_ID_LEVEL_CLIENT_ROLE_GENERATED_CMD_LIST
/*! @}
 *  @endcond */ /* internals_doc */

/*! Default OptionsMask value */
#define ZB_ZCL_LEVEL_CONTROL_OPTIONS_MASK_DEFAULT_FIELD_VALUE ((zb_uint8_t)0x00)

/*! Default OptionsOverride value */
#define ZB_ZCL_LEVEL_CONTROL_OPTIONS_OVERRIDE_DEFAULT_FIELD_VALUE ((zb_uint8_t)0x00)

/************************** Level Control cluster command definitions ****************************/

/* command request structure */

/*! @brief Structured representation of Level Control command payload (optional part)
    @see ZCL spec, subclause 3.10.2.3.1.1*/
typedef ZB_PACKED_PRE struct zb_zcl_level_control_req_options_s
{
  /** OptionsMask field */
  zb_uint8_t options_mask;
  /** OptionsOverride field */
  zb_uint8_t options_override;
} ZB_PACKED_STRUCT zb_zcl_level_control_req_options_t;

/** @brief Move to Level payload length macro */
#define ZB_ZCL_LEVEL_CONTROL_REQ_OPTIONS_PAYLOAD_LEN \
  sizeof(zb_zcl_level_control_req_options_t)

/** @internal Macro for getting Move to Level command */
#define ZB_ZCL_LEVEL_CONTROL_GET_CMD_OPTIONS(data_buf, req_options)     \
{                                                                       \
  zb_zcl_level_control_req_options_t *req_options_ptr;                  \
  (req_options_ptr) = zb_buf_len(data_buf) >=                           \
    ZB_ZCL_LEVEL_CONTROL_REQ_OPTIONS_PAYLOAD_LEN ?                      \
    (zb_zcl_level_control_req_options_t*)zb_buf_begin(data_buf) : NULL; \
  if (req_options_ptr != NULL)                                          \
  {                                                                     \
    req_options.options_mask = req_options_ptr->options_mask;           \
    req_options.options_override = req_options_ptr->options_override;   \
    (void)zb_buf_cut_left(data_buf, sizeof(zb_zcl_level_control_req_options_t)); \
  }                                                                     \
  else                                                                  \
  {                                                                     \
    req_options.options_mask = (zb_uint8_t)0x00;                        \
    req_options.options_override = (zb_uint8_t)0x00;                    \
  }                                                                     \
}

/*! @brief Structured representation of MOVE_TO_LEVEL command payload
    @see ZCL spec, subclause 3.10.2.3.1.1*/
typedef ZB_PACKED_PRE struct zb_zcl_level_control_move_to_level_req_s
{
  /** Level field */
  zb_uint8_t level;
  /** Transition time field */
  zb_uint16_t transition_time;
} ZB_PACKED_STRUCT zb_zcl_level_control_move_to_level_req_t;

/** @brief Move to Level payload length macro */
#define ZB_ZCL_LEVEL_CONTROL_MOVE_TO_LEVEL_REQ_PAYLOAD_LEN \
  sizeof(zb_zcl_level_control_move_to_level_req_t)

/*! @brief Send Move to Level command
    @param buffer - to put packet to
    @param dst_addr - address to send packet to
    @param dst_addr_mode - addressing mode
    @param dst_ep - destination endpoint
    @param ep - sending endpoint
    @param prof_id - profile identifier
    @param def_resp - enable/disable default response
    @param cb - callback for getting command send status
    @param level - Level value
    @param transition_time - Transition Time value
    @param options_mask - Options Mask value
    @param options_override - Options Override value
    @param cmd_id - command id
*/
void zb_zcl_level_control_send_move_to_level_cmd_zcl8(zb_bufid_t buffer, const zb_addr_u *dst_addr,
                                                      zb_uint8_t dst_addr_mode, zb_uint8_t dst_ep,
                                                      zb_uint8_t ep, zb_uint16_t prof_id,
                                                      zb_uint8_t def_resp, zb_callback_t cb,
                                                      zb_uint8_t level, zb_uint16_t transition_time,
                                                      zb_uint8_t options_mask, zb_uint8_t options_override,
                                                      zb_uint8_t cmd_id);

/*! @brief Send Move to Level command (pre-ZCL8)
    Use @ref zb_zcl_level_control_send_move_to_level_cmd_zcl8 for ZCL8 revision call.
    @param buffer - to put packet to
    @param dst_addr - address to send packet to
    @param dst_addr_mode - addressing mode
    @param dst_ep - destination endpoint
    @param ep - sending endpoint
    @param prof_id - profile identifier
    @param def_resp - enable/disable default response
    @param cb - callback for getting command send status
    @param level - Level value
    @param transition_time - Transition Time value
    @param cmd_id - command id
*/
void zb_zcl_level_control_send_move_to_level_cmd(zb_bufid_t buffer, const zb_addr_u *dst_addr,
                                                 zb_uint8_t dst_addr_mode, zb_uint8_t dst_ep,
                                                 zb_uint8_t ep, zb_uint16_t prof_id,
                                                 zb_uint8_t def_resp, zb_callback_t cb,
                                                 zb_uint8_t level, zb_uint16_t transition_time,
                                                 zb_uint8_t cmd_id);

/** @internal Macro for sending Move to Level command */
#define ZB_ZCL_LEVEL_CONTROL_SEND_MOVE_TO_LEVEL_CMD_ZCL8(buffer, addr, dst_addr_mode,                   \
  dst_ep, ep, prfl_id, def_resp, cb, level, transition_time, options_mask,                              \
  options_override, cmd_id)                                                                             \
{                                                                                                       \
  zb_zcl_level_control_send_move_to_level_cmd_zcl8(buffer, ZB_ADDR_U_CAST(addr), dst_addr_mode, dst_ep, \
                                                   ep, prfl_id, def_resp, cb, level, transition_time,   \
                                                   options_mask, options_override, cmd_id);             \
}

/** Macro for calling @ref zb_zcl_level_control_send_move_to_level_cmd function
 */
#define ZB_ZCL_LEVEL_CONTROL_SEND_MOVE_TO_LEVEL_CMD(buffer, addr, dst_addr_mode,                          \
  dst_ep, ep, prfl_id, def_resp, cb, level, transition_time, cmd_id)                                      \
{                                                                                                         \
  zb_zcl_level_control_send_move_to_level_cmd(buffer, ZB_ADDR_U_CAST(addr), dst_addr_mode, dst_ep,        \
                                              ep, prfl_id, def_resp, cb, level, transition_time, cmd_id); \
}

/** @internal Macro for getting Move to Level command */
#define ZB_ZCL_LEVEL_CONTROL_GET_MOVE_TO_LEVEL_CMD(data_buf, move_to_level_req, status) \
{                                                                                                \
  zb_zcl_level_control_move_to_level_req_t *move_to_level_req_ptr;                               \
  (move_to_level_req_ptr) = zb_buf_len(data_buf) >=                                              \
    ZB_ZCL_LEVEL_CONTROL_MOVE_TO_LEVEL_REQ_PAYLOAD_LEN ?                                         \
    (zb_zcl_level_control_move_to_level_req_t*)zb_buf_begin(data_buf) : NULL;                    \
  if (move_to_level_req_ptr != NULL)                                                             \
  {                                                                                              \
    ZB_HTOLE16(&(move_to_level_req).transition_time, &(move_to_level_req_ptr->transition_time)); \
    move_to_level_req.level = move_to_level_req_ptr->level;                                      \
    status = ZB_TRUE;                                                                            \
    (void)zb_buf_cut_left(data_buf, sizeof(zb_zcl_level_control_move_to_level_req_t));           \
  }                                                                                              \
  else                                                                                           \
  {                                                                                              \
    status = ZB_FALSE;                                                                           \
  }                                                                                              \
}

/*! @brief Send Move to Level command
    @param buffer - to put packet to
    @param addr - address to send packet to
    @param dst_addr_mode - addressing mode
    @param dst_ep - destination endpoint
    @param ep - sending endpoint
    @param prfl_id - profile identifier
    @param def_resp - enable/disable default response
    @param cb - callback for getting command send status
    @param level - Level value
    @param transition_time - Transition Time value
    @param options_mask - Options Mask value
    @param options_override - Options Override value
*/

#define ZB_ZCL_LEVEL_CONTROL_SEND_MOVE_TO_LEVEL_REQ_ZCL8(buffer, addr, dst_addr_mode, \
  dst_ep, ep, prfl_id, def_resp, cb, level, transition_time, options_mask,            \
  options_override)                                                                   \
{                                                                                     \
  ZB_ZCL_LEVEL_CONTROL_SEND_MOVE_TO_LEVEL_CMD_ZCL8(                                   \
                                              buffer,                                 \
                                              addr,                                   \
                                              dst_addr_mode,                          \
                                              dst_ep,                                 \
                                              ep,                                     \
                                              prfl_id,                                \
                                              def_resp,                               \
                                              cb,                                     \
                                              level,                                  \
                                              transition_time,                        \
                                              options_mask,                           \
                                              options_override,                       \
                                              ZB_ZCL_CMD_LEVEL_CONTROL_MOVE_TO_LEVEL) \
}

/*! @brief Send Move to Level command (pre-ZCL8)
    Use #ZB_ZCL_LEVEL_CONTROL_SEND_MOVE_TO_LEVEL_REQ_ZCL8 for ZCL8 revision call.
    @param buffer - to put packet to
    @param addr - address to send packet to
    @param dst_addr_mode - addressing mode
    @param dst_ep - destination endpoint
    @param ep - sending endpoint
    @param prfl_id - profile identifier
    @param def_resp - enable/disable default response
    @param cb - callback for getting command send status
    @param level - Level value
    @param transition_time - Transition Time value
*/

#define ZB_ZCL_LEVEL_CONTROL_SEND_MOVE_TO_LEVEL_REQ(buffer, addr, dst_addr_mode,      \
  dst_ep, ep, prfl_id, def_resp, cb, level, transition_time)                          \
{                                                                                     \
  ZB_ZCL_LEVEL_CONTROL_SEND_MOVE_TO_LEVEL_CMD(buffer,                                 \
                                              addr,                                   \
                                              dst_addr_mode,                          \
                                              dst_ep,                                 \
                                              ep,                                     \
                                              prfl_id,                                \
                                              def_resp,                               \
                                              cb,                                     \
                                              level,                                  \
                                              transition_time,                        \
                                              ZB_ZCL_CMD_LEVEL_CONTROL_MOVE_TO_LEVEL) \
}

/*!
  @brief Parses Get Move to Level command and fills data request structure.
  @param data_buf - pointer to zb_buf_t buffer containing command request data
  @param move_to_level_req - variable to save command request
  @param status - success or not read parameters - ZB_TRUE or ZB_FALSE
  @note data_buf buffer should contain command request payload without ZCL header.
*/

#define ZB_ZCL_LEVEL_CONTROL_GET_MOVE_TO_LEVEL_REQ(data_buf, move_to_level_req, status) \
{                                                                                       \
  ZB_ZCL_LEVEL_CONTROL_GET_MOVE_TO_LEVEL_CMD(data_buf, move_to_level_req, status);      \
}

/*! @brief Send Move to Level with On/Off command
    @param buffer - to put packet to
    @param addr - address to send packet to
    @param dst_addr_mode - addressing mode
    @param dst_ep - destination endpoint
    @param ep - sending endpoint
    @param prfl_id - profile identifier
    @param def_resp - enable/disable default response
    @param cb - callback for getting command send status
    @param level - Level value
    @param transition_time - Transition Time value
    @param options_mask - Options Mask value
    @param options_override - Options Override value
*/

#define ZB_ZCL_LEVEL_CONTROL_SEND_MOVE_TO_LEVEL_WITH_ON_OFF_REQ_ZCL8(buffer, addr, dst_addr_mode, \
  dst_ep, ep, prfl_id, def_resp, cb, level, transition_time, options_mask, options_override)      \
{                                                                                                 \
  ZB_ZCL_LEVEL_CONTROL_SEND_MOVE_TO_LEVEL_CMD_ZCL8(                                               \
                                              buffer,                                             \
                                              addr,                                               \
                                              dst_addr_mode,                                      \
                                              dst_ep,                                             \
                                              ep,                                                 \
                                              prfl_id,                                            \
                                              def_resp,                                           \
                                              cb,                                                 \
                                              level,                                              \
                                              transition_time,                                    \
                                              options_mask,                                       \
                                              options_override,                                   \
                                              ZB_ZCL_CMD_LEVEL_CONTROL_MOVE_TO_LEVEL_WITH_ON_OFF) \
}

/*! @brief Send Move to Level with On/Off command (pre-ZCL8)
    Use #ZB_ZCL_LEVEL_CONTROL_SEND_MOVE_TO_LEVEL_WITH_ON_OFF_REQ_ZCL8 for ZCL8 revision call.
    @param buffer - to put packet to
    @param addr - address to send packet to
    @param dst_addr_mode - addressing mode
    @param dst_ep - destination endpoint
    @param ep - sending endpoint
    @param prfl_id - profile identifier
    @param def_resp - enable/disable default response
    @param cb - callback for getting command send status
    @param level - Level value
    @param transition_time - Transition Time value
*/

#define ZB_ZCL_LEVEL_CONTROL_SEND_MOVE_TO_LEVEL_WITH_ON_OFF_REQ(buffer, addr, dst_addr_mode,      \
  dst_ep, ep, prfl_id, def_resp, cb, level, transition_time)                                      \
{                                                                                                 \
  ZB_ZCL_LEVEL_CONTROL_SEND_MOVE_TO_LEVEL_CMD(buffer,                                             \
                                              addr,                                               \
                                              dst_addr_mode,                                      \
                                              dst_ep,                                             \
                                              ep,                                                 \
                                              prfl_id,                                            \
                                              def_resp,                                           \
                                              cb,                                                 \
                                              level,                                              \
                                              transition_time,                                    \
                                              ZB_ZCL_CMD_LEVEL_CONTROL_MOVE_TO_LEVEL_WITH_ON_OFF) \
}

/*!
  @brief Parses Get Move to Level with On/Off command and fills data request
  structure.
  @param data_buf - pointer to zb_buf_t buffer containing command request data
  @param move_to_level_req - variable to save command request.
  @param status - success or not read parameters - ZB_TRUE or ZB_FALSE
  @note data_buf buffer should contain command request payload without ZCL header.
*/

#define ZB_ZCL_LEVEL_CONTROL_GET_MOVE_TO_LEVEL_WITH_ON_OFF_REQ(                   \
  data_buf, move_to_level_req, status)                                            \
{                                                                                 \
  ZB_ZCL_LEVEL_CONTROL_GET_MOVE_TO_LEVEL_CMD(data_buf, move_to_level_req, status); \
}


enum zb_zcl_level_control_move_mode_e
{
  ZB_ZCL_LEVEL_CONTROL_MOVE_MODE_UP   = 0x00,
  ZB_ZCL_LEVEL_CONTROL_MOVE_MODE_DOWN = 0x01
};

/*! @brief Structured representation of MOVE command payload */
typedef ZB_PACKED_PRE struct zb_zcl_level_control_move_req_s
{
  /** Move Mode field */
  zb_uint8_t move_mode;
  /** Rate field */
  zb_uint8_t rate;
} ZB_PACKED_STRUCT zb_zcl_level_control_move_req_t;

/** @brief Move payload length macro */
#define ZB_ZCL_LEVEL_CONTROL_MOVE_REQ_PAYLOAD_LEN \
  sizeof(zb_zcl_level_control_move_req_t)

/*! @brief Send Move command
    @param buffer - to put packet to
    @param dst_addr - address to send packet to
    @param dst_addr_mode - addressing mode
    @param dst_ep - destination endpoint
    @param ep - sending endpoint
    @param prof_id - profile identifier
    @param def_resp - enable/disable default response
    @param cb - callback for getting command send status
    @param move_mode - Move Mode value
    @param rate - Rate value
    @param options_mask - Options Mask value
    @param options_override - Options Override value
    @param cmd_id - command id
*/
void zb_zcl_level_control_send_move_cmd_zcl8(zb_bufid_t buffer, const zb_addr_u *dst_addr,
                                             zb_uint8_t dst_addr_mode, zb_uint8_t dst_ep,
                                             zb_uint8_t ep, zb_uint16_t prof_id,
                                             zb_uint8_t def_resp, zb_callback_t cb,
                                             zb_uint8_t move_mode, zb_uint8_t rate,
                                             zb_uint8_t options_mask, zb_uint8_t options_override,
                                             zb_uint8_t cmd_id);

/*! @brief Send Move command (pre-ZCL8)
    Use @ref zb_zcl_level_control_send_move_cmd_zcl8 for ZCL8 revision call.
    @param buffer - to put packet to
    @param dst_addr - address to send packet to
    @param dst_addr_mode - addressing mode
    @param dst_ep - destination endpoint
    @param ep - sending endpoint
    @param prof_id - profile identifier
    @param def_resp - enable/disable default response
    @param cb - callback for getting command send status
    @param move_mode - Move Mode value
    @param rate - Rate value
    @param cmd_id - command id
*/
void zb_zcl_level_control_send_move_cmd(zb_bufid_t buffer, const zb_addr_u *dst_addr,
                                        zb_uint8_t dst_addr_mode, zb_uint8_t dst_ep,
                                        zb_uint8_t ep, zb_uint16_t prof_id,
                                        zb_uint8_t def_resp, zb_callback_t cb,
                                        zb_uint8_t move_mode, zb_uint8_t rate,
                                        zb_uint8_t cmd_id);

/** @internal Macro for sending Move command */
#define ZB_ZCL_LEVEL_CONTROL_SEND_MOVE_CMD_ZCL8(                                               \
  buffer, addr, dst_addr_mode, dst_ep, ep, prfl_id, def_resp, cb, move_mode, rate,             \
  options_mask, options_override, cmd_id)                                                      \
{                                                                                              \
  zb_zcl_level_control_send_move_cmd_zcl8(buffer, ZB_ADDR_U_CAST(addr), dst_addr_mode, dst_ep, \
                                          ep, prfl_id, def_resp, cb, move_mode, rate,          \
                                          options_mask, options_override, cmd_id);             \
}

/** Macro for calling @ref zb_zcl_level_control_send_move_cmd function
 */
#define ZB_ZCL_LEVEL_CONTROL_SEND_MOVE_CMD(                                                  \
  buffer, addr, dst_addr_mode, dst_ep, ep, prfl_id, def_resp, cb, move_mode, rate, cmd_id)   \
{                                                                                            \
  zb_zcl_level_control_send_move_cmd(buffer, ZB_ADDR_U_CAST(addr), dst_addr_mode, dst_ep,    \
                                     ep, prfl_id, def_resp, cb, move_mode, rate, cmd_id);    \
}

/** @internal Macro for getting Move command */
#define ZB_ZCL_LEVEL_CONTROL_GET_MOVE_CMD(data_buf, move_req, status)                  \
{                                                                                      \
  zb_zcl_level_control_move_req_t *move_req_ptr;                                       \
  (move_req_ptr) = zb_buf_len(data_buf) >= ZB_ZCL_LEVEL_CONTROL_MOVE_REQ_PAYLOAD_LEN ? \
    (zb_zcl_level_control_move_req_t*)zb_buf_begin(data_buf) : NULL;                   \
  if (move_req_ptr != NULL)                                                            \
  {                                                                                    \
    ZB_MEMCPY(&(move_req), move_req_ptr, sizeof(zb_zcl_level_control_move_req_t));     \
    (void)zb_buf_cut_left(data_buf, sizeof(zb_zcl_level_control_move_req_t));          \
    status = ZB_TRUE;                                                                  \
  }                                                                                    \
  else                                                                                 \
  {                                                                                    \
    status = ZB_FALSE;                                                                 \
  }                                                                                    \
}

/*! @brief Send Move command
    @param buffer - to put packet to
    @param addr - address to send packet to
    @param dst_addr_mode - addressing mode
    @param dst_ep - destination endpoint
    @param ep - sending endpoint
    @param prfl_id - profile identifier
    @param def_resp - enable/disable default response
    @param cb - callback for getting command send status
    @param move_mode - Move Mode value
    @param rate - Rate value
    @param options_mask - Options Mask value
    @param options_override - Options Override value
*/
#define ZB_ZCL_LEVEL_CONTROL_SEND_MOVE_REQ_ZCL8(buffer, addr, dst_addr_mode,    \
  dst_ep, ep, prfl_id, def_resp, cb, move_mode, rate, options_mask,             \
  options_override)                                                             \
{                                                                               \
  ZB_ZCL_LEVEL_CONTROL_SEND_MOVE_CMD_ZCL8(buffer,                               \
                                          addr,                                 \
                                          dst_addr_mode,                        \
                                          dst_ep,                               \
                                          ep,                                   \
                                          prfl_id,                              \
                                          def_resp,                             \
                                          cb,                                   \
                                          move_mode,                            \
                                          rate,                                 \
                                          options_mask,                         \
                                          options_override,                     \
                                          ZB_ZCL_CMD_LEVEL_CONTROL_MOVE);       \
}

/*! @brief Send Move command (pre-ZCL8)
    Use #ZB_ZCL_LEVEL_CONTROL_SEND_MOVE_REQ_ZCL8 for ZCL8 revision call.
    @param buffer - to put packet to
    @param addr - address to send packet to
    @param dst_addr_mode - addressing mode
    @param dst_ep - destination endpoint
    @param ep - sending endpoint
    @param prfl_id - profile identifier
    @param def_resp - enable/disable default response
    @param cb - callback for getting command send status
    @param move_mode - Move Mode value
    @param rate - Rate value
*/
#define ZB_ZCL_LEVEL_CONTROL_SEND_MOVE_REQ(buffer, addr, dst_addr_mode,    \
  dst_ep, ep, prfl_id, def_resp, cb, move_mode, rate)                      \
{                                                                          \
  ZB_ZCL_LEVEL_CONTROL_SEND_MOVE_CMD(buffer,                               \
                                     addr,                                 \
                                     dst_addr_mode,                        \
                                     dst_ep,                               \
                                     ep,                                   \
                                     prfl_id,                              \
                                     def_resp,                             \
                                     cb,                                   \
                                     move_mode,                            \
                                     rate,                                 \
                                     ZB_ZCL_CMD_LEVEL_CONTROL_MOVE);       \
}

/*!
  @brief Parses Get Move command and fills data request
  structure.
  @param data_buf - pointer to zb_buf_t buffer containing command request data
  @param move_req - variable to save command request
  @param status - success or not read parameters - ZB_TRUE or ZB_FALSE
  @note data_buf buffer should contain command request payload without ZCL header.
*/

#define ZB_ZCL_LEVEL_CONTROL_GET_MOVE_REQ(data_buf, move_req, status)   \
{                                                                       \
  ZB_ZCL_LEVEL_CONTROL_GET_MOVE_CMD(data_buf, move_req, status);        \
}

/*! @brief Send Move with On/Off command
    @param buffer - to put packet to
    @param addr - address to send packet to
    @param dst_addr_mode - addressing mode
    @param dst_ep - destination endpoint
    @param ep - sending endpoint
    @param prfl_id - profile identifier
    @param def_resp - enable/disable default response
    @param cb - callback for getting command send status
    @param move_mode - Move Mode value
    @param rate - Rate value
    @param options_mask - Options Mask value
    @param options_override - Options Override value
*/

#define ZB_ZCL_LEVEL_CONTROL_SEND_MOVE_WITH_ON_OFF_REQ_ZCL8(                            \
  buffer, addr, dst_addr_mode, dst_ep, ep, prfl_id, def_resp, cb, move_mode, rate,      \
  options_mask, options_override)                                                       \
{                                                                                       \
  ZB_ZCL_LEVEL_CONTROL_SEND_MOVE_CMD_ZCL8(buffer,                                       \
                                          addr,                                         \
                                          dst_addr_mode,                                \
                                          dst_ep,                                       \
                                          ep,                                           \
                                          prfl_id,                                      \
                                          def_resp,                                     \
                                          cb,                                           \
                                          move_mode,                                    \
                                          rate,                                         \
                                          options_mask,                                 \
                                          options_override,                             \
                                          ZB_ZCL_CMD_LEVEL_CONTROL_MOVE_WITH_ON_OFF);   \
}

/*! @brief Send Move with On/Off command (pre-ZCL8)
    Use #ZB_ZCL_LEVEL_CONTROL_SEND_MOVE_WITH_ON_OFF_REQ_ZCL8 for ZCL8 revision call.
    @param buffer - to put packet to
    @param addr - address to send packet to
    @param dst_addr_mode - addressing mode
    @param dst_ep - destination endpoint
    @param ep - sending endpoint
    @param prfl_id - profile identifier
    @param def_resp - enable/disable default response
    @param cb - callback for getting command send status
    @param move_mode - Move Mode value
    @param rate - Rate value
*/

#define ZB_ZCL_LEVEL_CONTROL_SEND_MOVE_WITH_ON_OFF_REQ(                            \
  buffer, addr, dst_addr_mode, dst_ep, ep, prfl_id, def_resp, cb, move_mode, rate) \
{                                                                                  \
  ZB_ZCL_LEVEL_CONTROL_SEND_MOVE_CMD(buffer,                                       \
                                     addr,                                         \
                                     dst_addr_mode,                                \
                                     dst_ep,                                       \
                                     ep,                                           \
                                     prfl_id,                                      \
                                     def_resp,                                     \
                                     cb,                                           \
                                     move_mode,                                    \
                                     rate,                                         \
                                     ZB_ZCL_CMD_LEVEL_CONTROL_MOVE_WITH_ON_OFF);   \
}

/*!
  @brief Parses Get Move with On/Off command and fills data request
  structure.
  @param data_buf - pointer to zb_buf_t buffer containing command request data
  @param move_req - variable to save command request
  @param status - success or not read parameters - ZB_TRUE or ZB_FALSE
  @note data_buf buffer should contain command request payload without ZCL header.
*/

#define ZB_ZCL_LEVEL_CONTROL_GET_MOVE_WITH_ON_OFF_REQ(data_buf, move_req, status) \
{                                                                                 \
  ZB_ZCL_LEVEL_CONTROL_GET_MOVE_CMD(data_buf, move_req, status);                  \
}

enum zb_zcl_level_control_step_mode_e
{
  ZB_ZCL_LEVEL_CONTROL_STEP_MODE_UP   = 0x00,
  ZB_ZCL_LEVEL_CONTROL_STEP_MODE_DOWN = 0x01
};

/*! @brief Structured representation of STEP command payload */
typedef ZB_PACKED_PRE struct zb_zcl_level_control_step_req_s
{
  /** Step Mode field */
  zb_uint8_t step_mode;
  /** Step Size field */
  zb_uint8_t step_size;
  /** Transition Time field */
  zb_uint16_t transition_time;
} ZB_PACKED_STRUCT zb_zcl_level_control_step_req_t;

/** @brief Step payload length macro */
#define ZB_ZCL_LEVEL_CONTROL_STEP_REQ_PAYLOAD_LEN \
  sizeof(zb_zcl_level_control_step_req_t)

/*! @brief Send Step command
    @param buffer - to put packet to
    @param dst_addr - address to send packet to
    @param dst_addr_mode - addressing mode
    @param dst_ep - destination endpoint
    @param ep - sending endpoint
    @param prof_id - profile identifier
    @param def_resp - enable/disable default response
    @param cb - callback for getting command send status
    @param step_mode - Step Mode value
    @param step_size - Step Size value
    @param transition_time - Transition Time value
    @param options_mask - Options Mask value
    @param options_override - Options Override value
    @param cmd_id - command id
*/
void zb_zcl_level_control_send_step_cmd_zcl8(zb_bufid_t buffer, const zb_addr_u *dst_addr,
                                             zb_uint8_t dst_addr_mode, zb_uint8_t dst_ep,
                                             zb_uint8_t ep, zb_uint16_t prof_id,
                                             zb_uint8_t def_resp, zb_callback_t cb,
                                             zb_uint8_t step_mode, zb_uint8_t step_size,
                                             zb_uint16_t transition_time,
                                             zb_uint8_t options_mask, zb_uint8_t options_override,
                                             zb_uint8_t cmd_id);

/*! @brief Send Step command (pre-ZCL8)
    Use @ref zb_zcl_level_control_send_step_cmd_zcl8 for ZCL8 revision call.
    @param buffer - to put packet to
    @param dst_addr - address to send packet to
    @param dst_addr_mode - addressing mode
    @param dst_ep - destination endpoint
    @param ep - sending endpoint
    @param prof_id - profile identifier
    @param def_resp - enable/disable default response
    @param cb - callback for getting command send status
    @param step_mode - Step Mode value
    @param step_size - Step Size value
    @param transition_time - Transition Time value
    @param cmd_id - command id
*/
void zb_zcl_level_control_send_step_cmd(zb_bufid_t buffer, const zb_addr_u *dst_addr,
                                        zb_uint8_t dst_addr_mode, zb_uint8_t dst_ep,
                                        zb_uint8_t ep, zb_uint16_t prof_id,
                                        zb_uint8_t def_resp, zb_callback_t cb,
                                        zb_uint8_t step_mode, zb_uint8_t step_size,
                                        zb_uint16_t transition_time,
                                        zb_uint8_t cmd_id);

/** @brief Macro for sending Step command */
#define ZB_ZCL_LEVEL_CONTROL_SEND_STEP_CMD_ZCL8(buffer,                 \
                                                addr,                   \
                                                dst_addr_mode,          \
                                                dst_ep,                 \
                                                ep,                     \
                                                prfl_id,                \
                                                def_resp,               \
                                                cb,                     \
                                                step_mode,              \
                                                step_size,              \
                                                transition_time,        \
                                                options_mask,           \
                                                options_override,       \
                                                cmd_id)                 \
{                                                                       \
  zb_zcl_level_control_send_step_cmd_zcl8(buffer,                       \
                                          ZB_ADDR_U_CAST(addr),         \
                                          dst_addr_mode,                \
                                          dst_ep,                       \
                                          ep,                           \
                                          prfl_id,                      \
                                          def_resp,                     \
                                          cb,                           \
                                          step_mode,                    \
                                          step_size,                    \
                                          transition_time,              \
                                          options_mask,                 \
                                          options_override,             \
                                          cmd_id);                      \
}

/** Macro for calling @ref zb_zcl_level_control_send_step_cmd function
 */
#define ZB_ZCL_LEVEL_CONTROL_SEND_STEP_CMD(buffer,                      \
                                           addr,                        \
                                           dst_addr_mode,               \
                                           dst_ep,                      \
                                           ep,                          \
                                           prfl_id,                     \
                                           def_resp,                    \
                                           cb,                          \
                                           step_mode,                   \
                                           step_size,                   \
                                           transition_time,             \
                                           cmd_id)                      \
{                                                                       \
  zb_zcl_level_control_send_step_cmd(buffer,                            \
                                     ZB_ADDR_U_CAST(addr),              \
                                     dst_addr_mode,                     \
                                     dst_ep,                            \
                                     ep,                                \
                                     prfl_id,                           \
                                     def_resp,                          \
                                     cb,                                \
                                     step_mode,                         \
                                     step_size,                         \
                                     transition_time,                   \
                                     cmd_id);                           \
}


/** @internal Macro for getting Step command */
#define ZB_ZCL_LEVEL_CONTROL_GET_STEP_CMD(data_buf, step_req, status)                  \
{                                                                                      \
  zb_zcl_level_control_step_req_t *step_req_ptr;                                       \
  (step_req_ptr) = zb_buf_len(data_buf) >= ZB_ZCL_LEVEL_CONTROL_STEP_REQ_PAYLOAD_LEN ? \
    (zb_zcl_level_control_step_req_t*)zb_buf_begin(data_buf) : NULL;                   \
  if (step_req_ptr != NULL)                                                            \
  {                                                                                    \
    step_req.step_mode = step_req_ptr->step_mode;                                      \
    step_req.step_size = step_req_ptr->step_size;                                      \
    ZB_HTOLE16(&(step_req).transition_time, &step_req_ptr->transition_time);           \
    (void)zb_buf_cut_left(data_buf, sizeof(zb_zcl_level_control_step_req_t));          \
    status = ZB_TRUE;                                                                  \
  }                                                                                    \
  else                                                                                 \
  {                                                                                    \
    status = ZB_FALSE;                                                                 \
  }                                                                                    \
}

/*! @brief Send Step command
    @param buffer - to put packet to
    @param addr - address to send packet to
    @param dst_addr_mode - addressing mode
    @param dst_ep - destination endpoint
    @param ep - sending endpoint
    @param prfl_id - profile identifier
    @param def_resp - enable/disable default response
    @param cb - callback for getting command send status
    @param step_mode - Step Mode value
    @param step_size - Step Size value
    @param transition_time - Transition Time value
    @param options_mask - Options Mask value
    @param options_override - Options Override value
*/

#define ZB_ZCL_LEVEL_CONTROL_SEND_STEP_REQ_ZCL8(buffer,                    \
                                                addr,                      \
                                                dst_addr_mode,             \
                                                dst_ep,                    \
                                                ep,                        \
                                                prfl_id,                   \
                                                def_resp,                  \
                                                cb,                        \
                                                step_mode,                 \
                                                step_size,                 \
                                                transition_time,           \
                                                options_mask,              \
                                                options_override)          \
{                                                                          \
  ZB_ZCL_LEVEL_CONTROL_SEND_STEP_CMD_ZCL8(buffer,                          \
                                          addr,                            \
                                          dst_addr_mode,                   \
                                          dst_ep,                          \
                                          ep,                              \
                                          prfl_id,                         \
                                          def_resp,                        \
                                          cb,                              \
                                          step_mode,                       \
                                          step_size,                       \
                                          transition_time,                 \
                                          options_mask,                    \
                                          options_override,                \
                                          ZB_ZCL_CMD_LEVEL_CONTROL_STEP);  \
}

/*! @brief Send Step command (pre-ZCL8)
    Use #ZB_ZCL_LEVEL_CONTROL_SEND_STEP_REQ_ZCL8 for ZCL8 revision call.
    @param buffer - to put packet to
    @param addr - address to send packet to
    @param dst_addr_mode - addressing mode
    @param dst_ep - destination endpoint
    @param ep - sending endpoint
    @param prfl_id - profile identifier
    @param def_resp - enable/disable default response
    @param cb - callback for getting command send status
    @param step_mode - Step Mode value
    @param step_size - Step Size value
    @param transition_time - Transition Time value
*/

#define ZB_ZCL_LEVEL_CONTROL_SEND_STEP_REQ(buffer,                    \
                                           addr,                      \
                                           dst_addr_mode,             \
                                           dst_ep,                    \
                                           ep,                        \
                                           prfl_id,                   \
                                           def_resp,                  \
                                           cb,                        \
                                           step_mode,                 \
                                           step_size,                 \
                                           transition_time)           \
{                                                                     \
  ZB_ZCL_LEVEL_CONTROL_SEND_STEP_CMD(buffer,                          \
                                     addr,                            \
                                     dst_addr_mode,                   \
                                     dst_ep,                          \
                                     ep,                              \
                                     prfl_id,                         \
                                     def_resp,                        \
                                     cb,                              \
                                     step_mode,                       \
                                     step_size,                       \
                                     transition_time,                 \
                                     ZB_ZCL_CMD_LEVEL_CONTROL_STEP);  \
}

/*!
  @brief Parses Get Step command and fills data request
  structure.
  @param data_buf - pointer to zb_buf_t buffer containing command request data
  @param step_req - variable to save command request
  @param status - success or not read parameters - ZB_TRUE or ZB_FALSE
  @note data_buf buffer should contain command request payload without ZCL header.
*/

#define ZB_ZCL_LEVEL_CONTROL_GET_STEP_REQ(data_buf, step_req, status)   \
{                                                                       \
  ZB_ZCL_LEVEL_CONTROL_GET_STEP_CMD(data_buf, step_req, status);        \
}

/*! @brief Send Step with On/Off command
    @param buffer - to put packet to
    @param addr - address to send packet to
    @param dst_addr_mode - addressing mode
    @param dst_ep - destination endpoint
    @param ep - sending endpoint
    @param prfl_id - profile identifier
    @param def_resp - enable/disable default response
    @param cb - callback for getting command send status
    @param step_mode - Step Mode value
    @param step_size - Step Size value
    @param transition_time - Transition Time value
    @param options_mask - Options Mask value
    @param options_override - Options Override value
*/

#define ZB_ZCL_LEVEL_CONTROL_SEND_STEP_WITH_ON_OFF_REQ_ZCL8(buffer,                   \
                                                            addr,                     \
                                                            dst_addr_mode,            \
                                                            dst_ep,                   \
                                                            ep,                       \
                                                            prfl_id,                  \
                                                            def_resp,                 \
                                                            cb,                       \
                                                            step_mode,                \
                                                            step_size,                \
                                                            transition_time,          \
                                                            options_mask,             \
                                                            options_override)         \
{                                                                                     \
  ZB_ZCL_LEVEL_CONTROL_SEND_STEP_CMD_ZCL8(buffer,                                     \
                                          addr,                                       \
                                          dst_addr_mode,                              \
                                          dst_ep,                                     \
                                          ep,                                         \
                                          prfl_id,                                    \
                                          def_resp,                                   \
                                          cb,                                         \
                                          step_mode,                                  \
                                          step_size,                                  \
                                          transition_time,                            \
                                          options_mask,                               \
                                          options_override,                           \
                                          ZB_ZCL_CMD_LEVEL_CONTROL_STEP_WITH_ON_OFF); \
}

/*! @brief Send Step with On/Off command (pre-ZCL8)
    Use #ZB_ZCL_LEVEL_CONTROL_SEND_STEP_WITH_ON_OFF_REQ_ZCL8 for ZCL8 revision call.
    @param buffer - to put packet to
    @param addr - address to send packet to
    @param dst_addr_mode - addressing mode
    @param dst_ep - destination endpoint
    @param ep - sending endpoint
    @param prfl_id - profile identifier
    @param def_resp - enable/disable default response
    @param cb - callback for getting command send status
    @param step_mode - Step Mode value
    @param step_size - Step Size value
    @param transition_time - Transition Time value
*/

#define ZB_ZCL_LEVEL_CONTROL_SEND_STEP_WITH_ON_OFF_REQ(buffer,                   \
                                                       addr,                     \
                                                       dst_addr_mode,            \
                                                       dst_ep,                   \
                                                       ep,                       \
                                                       prfl_id,                  \
                                                       def_resp,                 \
                                                       cb,                       \
                                                       step_mode,                \
                                                       step_size,                \
                                                       transition_time)          \
{                                                                                \
  ZB_ZCL_LEVEL_CONTROL_SEND_STEP_CMD(buffer,                                     \
                                     addr,                                       \
                                     dst_addr_mode,                              \
                                     dst_ep,                                     \
                                     ep,                                         \
                                     prfl_id,                                    \
                                     def_resp,                                   \
                                     cb,                                         \
                                     step_mode,                                  \
                                     step_size,                                  \
                                     transition_time,                            \
                                     ZB_ZCL_CMD_LEVEL_CONTROL_STEP_WITH_ON_OFF); \
}

/*!
  @brief Parses Get Step with On/Off command and fills data request
  structure.
  @param data_buf - pointer to zb_buf_t buffer containing command request data
  @param step_req - variable to save command request
  @param status - success or not read parameters - ZB_TRUE or ZB_FALSE
  @note data_buf buffer should contain command request payload without ZCL header.
*/

#define ZB_ZCL_LEVEL_CONTROL_GET_STEP_WITH_ON_OFF_REQ(data_buf, step_req, status) \
  {                                                                               \
  ZB_ZCL_LEVEL_CONTROL_GET_STEP_CMD(data_buf, step_req, status);                  \
}

/*! @brief Send Stop command
    @param buffer - to put packet to
    @param dst_addr - address to send packet to
    @param dst_addr_mode - addressing mode
    @param dst_ep - destination endpoint
    @param ep - sending endpoint
    @param prof_id - profile identifier
    @param def_resp - enable/disable default response
    @param cb - callback for getting command send status
    @param options_mask - Options Mask value
    @param options_override - Options Override value
*/
void zb_zcl_level_control_send_stop_req_zcl8(zb_bufid_t buffer, const zb_addr_u *dst_addr,
                                             zb_uint8_t dst_addr_mode, zb_uint8_t dst_ep,
                                             zb_uint8_t ep, zb_uint16_t prof_id,
                                             zb_uint8_t def_resp, zb_callback_t cb,
                                             zb_uint8_t options_mask, zb_uint8_t options_override);

/*! @brief Send Stop command (pre-ZCL8)
    Use @ref zb_zcl_level_control_send_stop_req_zcl8 for ZCL8 revision call.
    @param buffer - to put packet to
    @param dst_addr - address to send packet to
    @param dst_addr_mode - addressing mode
    @param dst_ep - destination endpoint
    @param ep - sending endpoint
    @param prof_id - profile identifier
    @param def_resp - enable/disable default response
    @param cb - callback for getting command send status
*/
void zb_zcl_level_control_send_stop_req(zb_bufid_t buffer, const zb_addr_u *dst_addr,
                                        zb_uint8_t dst_addr_mode, zb_uint8_t dst_ep,
                                        zb_uint8_t ep, zb_uint16_t prof_id,
                                        zb_uint8_t def_resp, zb_callback_t cb);

/** Macro for calling @ref zb_zcl_level_control_send_stop_req_zcl8 function
 */
#define ZB_ZCL_LEVEL_CONTROL_SEND_STOP_REQ_ZCL8(                                                                          \
  buffer, addr, dst_addr_mode, dst_ep, ep, prfl_id, def_resp, cb, options_mask,                                           \
  options_override)                                                                                                       \
{                                                                                                                         \
  zb_zcl_level_control_send_stop_req_zcl8(buffer, ZB_ADDR_U_CAST(addr), dst_addr_mode, dst_ep, ep, prfl_id, def_resp, cb, \
                                          options_mask, options_override);                                                \
}

/** Macro for calling @ref zb_zcl_level_control_send_stop_req function
 */
#define ZB_ZCL_LEVEL_CONTROL_SEND_STOP_REQ(                                                                           \
  buffer, addr, dst_addr_mode, dst_ep, ep, prfl_id, def_resp, cb)                                                     \
{                                                                                                                     \
  zb_zcl_level_control_send_stop_req(buffer, ZB_ADDR_U_CAST(addr), dst_addr_mode, dst_ep, ep, prfl_id, def_resp, cb); \
}

/*! @brief Send Stop with On/off command
    @param buffer - to put packet to
    @param addr - address to send packet to
    @param dst_addr_mode - addressing mode
    @param dst_ep - destination endpoint
    @param ep - sending endpoint
    @param prfl_id - profile identifier
    @param def_resp - enable/disable default response
    @param cb - callback for getting command send status
    @param options_mask - Options Mask value
    @param options_override - Options Override value
*/

#define ZB_ZCL_LEVEL_CONTROL_SEND_STOP_WITH_ON_OFF_REQ_ZCL8(                                 \
  buffer, addr, dst_addr_mode, dst_ep, ep, prfl_id, def_resp, cb, options_mask,              \
  options_override)                                                                          \
{                                                                                            \
  ZB_ZCL_LEVEL_CONTROL_SEND_STOP_REQ_ZCL8(buffer, addr, dst_addr_mode, dst_ep, ep, prfl_id,  \
    def_resp, cb, options_mask, options_override)                                            \
}

/*! @brief Send Stop with On/off command (pre-ZCL8)
    Use #ZB_ZCL_LEVEL_CONTROL_SEND_STOP_WITH_ON_OFF_REQ_ZCL8 for ZCL8 revision call.
    @param buffer - to put packet to
    @param addr - address to send packet to
    @param dst_addr_mode - addressing mode
    @param dst_ep - destination endpoint
    @param ep - sending endpoint
    @param prfl_id - profile identifier
    @param def_resp - enable/disable default response
    @param cb - callback for getting command send status
*/

#define ZB_ZCL_LEVEL_CONTROL_SEND_STOP_WITH_ON_OFF_REQ(                                   \
  buffer, addr, dst_addr_mode, dst_ep, ep, prfl_id, def_resp, cb)                         \
{                                                                                         \
    ZB_ZCL_LEVEL_CONTROL_SEND_STOP_REQ(buffer, addr, dst_addr_mode, dst_ep, ep, prfl_id,  \
    def_resp, cb)                                                                         \
}
/** @cond internals_doc */
typedef struct zb_zcl_level_control_set_value_param_s
{
  zb_uint8_t new_value;
} zb_zcl_level_control_set_value_param_t;
/*! @}
 *  @endcond */ /* internals_doc */

  /* definition to parse request */

  /* command response structure */

  /* definition to parse response */

  /* definition to send command request */

/*! @} */ /* Level Control cluster commands */

/*! @} */ /* ZCL Level Control cluster definitions */

/** @endcond */

void zb_zcl_level_control_init_server(void);
void zb_zcl_level_control_init_client(void);

/**
 * @brief Process the effect of On/Off command on Level Control attribute
 *
 * @note spec 3.10.2.1.1: Effect of On/Off Commands on the CurrentLevel Attribute
 *
 * @param[in] param   The buffer id identifies the parameter of On/Off command
 * @param[in] on_off  The status of On/Off attribute
 * @return
 *        - ZB_TRUE: The command has been processed without invoking the on/off device ID callback;
 *                   otherwise, it has not been processed.
 */
zb_bool_t zb_zcl_process_level_associate_with_on_off(zb_uint8_t param, zb_bool_t on_off);

#define ZB_ZCL_CLUSTER_ID_LEVEL_CONTROL_SERVER_ROLE_INIT zb_zcl_level_control_init_server
#define ZB_ZCL_CLUSTER_ID_LEVEL_CONTROL_CLIENT_ROLE_INIT zb_zcl_level_control_init_client

#endif /* ZB_ZCL_LEVEL_CONTROL_H */
