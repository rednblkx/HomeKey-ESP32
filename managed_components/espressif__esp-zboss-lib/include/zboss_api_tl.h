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
/* PURPOSE: ZBOSS Zigbee cluster library API header
*/
#ifndef ZBOSS_API_TL_H
#define ZBOSS_API_TL_H 1

#include "zb_types.h"

#include "zcl/zb_zcl_config.h"
#include "zcl/zb_zcl_common.h"
#include "zcl/zb_zcl_commands.h"

#include "zcl/zb_zcl_touchlink_commissioning.h"

/** @cond (DOXYGEN_ZCL_SECTION && DOXYGEN_TOUCHLINK_FEATURE) */

/**
 *  @addtogroup ZB_ZCL_TOUCHLINK_COMMON
 *  @{
 *    @details
 *    This section describes data structures and common definitions used by Touchlink cluster.
 */

/** Controls number of device info record stored in Touchlink transaction context. */
#define ZB_ZLL_TRANS_CTX_DEV_INFO_MAX_NUMBER 8U

/** Controls number of endpoint descriptions stored in Touchlink transaction context. */
#define ZB_ZLL_TRANS_CTX_EP_INFO_MAX_NUMBER 6U

/** @cond internals_doc */

/** @brief Touchlink Address range */
typedef struct zb_zll_addr_range_s
{
  zb_uint16_t addr_begin;
  zb_uint16_t addr_end;
}
zb_zll_addr_range_t;

/** @brief Touchlink Group ID range */
typedef struct zb_zll_group_id_range_s
{
  zb_uint16_t group_id_begin;
  zb_uint16_t group_id_end;
}
zb_zll_group_id_range_t;

/** Touchlink Sub-device information */
typedef struct zb_zll_sub_device_info_s
{
  zb_uint8_t ep_id;
  zb_uint16_t profile_id;
  zb_uint16_t device_id;
  zb_uint8_t version;
  zb_uint8_t group_id_count;
  zb_uint8_t dev_idx; /**< Index (in the device info table) of the device endpoint belongs to. */
} zb_zll_sub_device_info_t;

/** Touchlink Discovered device information */
typedef struct zb_zll_ext_device_info_s
{
  /** @brief Our custom field to be able to address device information request. */
  zb_ieee_addr_t device_addr;
  zb_uint8_t rssi_correction;
  zb_uint8_t zb_info;
  zb_uint8_t zll_info;
  zb_uint16_t key_info;
  zb_uint32_t resp_id;
  zb_ext_pan_id_t ext_pan_id;
  zb_uint8_t nwk_update_id;
  zb_uint8_t channel_number;
  zb_uint16_t pan_id;
  zb_uint16_t nwk_addr;
  zb_uint8_t sub_device_count;
  zb_uint8_t total_group_id_count;
  zb_uint8_t ep_info_idx;
  zb_int8_t rssi;
} zb_zll_ext_device_info_t;

/**
 *  @brief Structure to store data for Touchlink Start new network command.
 *  Stored in the context on the target side.
 */
typedef struct zb_zll_start_new_network_target_data_s
{
  zb_uint16_t seq_number;           /**< ZCL sequence number. */
  zb_uint16_t pan_id;               /**< Identifier of the PAN to be created. */
  zb_ext_pan_id_t ext_pan_id;       /**< Extended identifier of the PAN to be created. */
  zb_uint8_t channel;               /**< Channel to be used for PAN creation. */
  zb_uint16_t short_addr;           /**< New short addr */
  zb_ieee_addr_t target_ieee_addr;  /**< Target IEEE addr */
} zb_zll_start_new_network_target_data_t;

/** @brief Touchlink Start router struct */
typedef struct zb_zll_start_router_param_s
{
  zb_ext_pan_id_t ext_pan_id;   /**< Extension Pan ID */
  zb_uint16_t short_pan_id;     /**< Short address Pan ID */
  zb_uint8_t channel;           /**< Channel number */
  zb_uint16_t short_addr;       /**< New short address of router */
}
zb_zll_start_router_param_t;

/** @brief Touchlink Rejoin struct */
typedef struct zb_zll_rejon_nwk_ed_param_s
{
  zb_ext_pan_id_t ext_pan_id;   /**< Extension Pan ID */
  zb_uint16_t short_pan_id;     /**< Short address Pan ID */
  zb_uint8_t channel;           /**< Channel number */
}
zb_zll_rejon_nwk_ed_param_t;

/** @brief Touchlink Join router struct */
typedef struct zb_zll_network_join_router_data_s
{
  zb_ieee_addr_t target_ieee_addr;        /**< Target IEEE addr */
} zb_zll_network_join_router_data_t;

/**
 *  @brief Type of the Touchlink command data storage.
 *  Stored in the context on the target side for to be used in asynchronous chains.
 */
typedef union zb_zll_command_target_data_u
{
  zb_zll_start_new_network_target_data_t start_new_nwk; /**< "Start new network" command data. */
  zb_zll_start_router_param_t start_router_param;       /**< Parameters for start router command */
  zb_zll_rejon_nwk_ed_param_t rejoin_nwk_param;         /**< Parameters for rejoin nwk end device router
                                                         * command */
  zb_zll_network_join_router_data_t nwk_join_router;    /**< Parameters for rejoin nwk end device router command */
} zb_zll_command_target_data_t;

/**
 *  @brief Touchlink callback type for checking requested action is allowed.
 *  @param action [IN] - action to check.
 *  @returns ZB_TRUE if action allowed, ZB_FALSE otherwise.
 */
typedef zb_bool_t (*zb_zll_is_action_allowed_cb_t)(zb_uint8_t action);

/** Touchlink device info data */
typedef struct zb_zll_device_info_s
{
  zb_uint8_t rssi_correction;
  zb_uint8_t zll_info;
  zb_uint16_t key_info;
  zb_uint8_t key_index;
  zb_zll_addr_range_t addr_range;
  zb_zll_group_id_range_t group_id_range;
  zb_uint8_t total_group_id_count; /* TODO: Not sure it is needed maybe calculate it using group ID
                                      count for EP? */
  /** @brief User-provided "report task result" callback. */
  zb_callback_t report_task_result;

  zb_uint8_t master_key[16];
  zb_uint8_t certification_key[16];
  zb_uint32_t transaction_id;
  zb_uint32_t response_id;
  zb_uint8_t encr_nwk_key[16];  /* FIXME: do we really need to store it? */

  zb_uint16_t identify_duration;  /**< Identify duration period for ZLL commissioning server. */
  zb_callback_t identify_handler; /**< User-provided identify start/stop handler. */

  zb_uint8_t freqagility_channel;
  zb_uint8_t nwk_channel;       /* Logical channel number for start/join the network (as initiator
                                 * role) */
  zb_int8_t rssi_threshold;     /* RSSI threshold */
  zb_uint32_t target_timeout;   /* Timeout for target to wait initiator */
}
zb_zll_device_info_t;

/** @brief Touchlink transaction data context */
typedef struct zb_zll_transaction_ctx_s
{
  /** @brief Inter-PAN transaction identifier for the transaction under processing. */
  zb_uint32_t transaction_id;

  /** Specifies if identify request was sent during transaction */
  zb_bitfield_t identify_sent:1;
  /** "Send confirmed" flag. */
  zb_bitfield_t send_confirmed:1;
  /** "Scan timed out" flag. */
  zb_bitfield_t scan_timed_out:1;
  /** "Extended scan" flag. */
  zb_bitfield_t ext_scan:1;
  /** "Out of memory" flag. */
  zb_bitfield_t out_of_memory:1;

  /** Source device address received with the packet in the current transaction stage. */
  zb_ieee_addr_t src_addr;

  /**
   *  @brief Describe current transaction task. Possible tasks are defined in enumeration @ref
   *  zb_zll_transaction_task_e
   */
  zb_ushort_t transaction_task;

  zb_uint8_t n_device_infos;  /**< Number of device infos currently stored in the table. */
  zb_uint8_t current_dev_info_idx;  /**< Index for the device info record iterator. */
  /** Table of devices reported during the active transaction. */
  zb_zll_ext_device_info_t device_infos[ZB_ZLL_TRANS_CTX_DEV_INFO_MAX_NUMBER];

  /** Number of filled endpoint info records. */
  zb_uint8_t n_ep_infos;
  /** Sub-device (endpoint) info table. */
  zb_zll_sub_device_info_t ep_infos[ZB_ZLL_TRANS_CTX_DEV_INFO_MAX_NUMBER * ZB_ZLL_TRANS_CTX_EP_INFO_MAX_NUMBER];

  /** @brief User-provided "check action allowed" callback. */
  zb_zll_is_action_allowed_cb_t check_action_allowed;
  /** @internal @brief Packet send counter.
    *
    * Counts packets sent during particular transaction procedure.
    * @attention Should be neither referenced nor changed from the userspace.
    */
  zb_uint8_t pckt_cnt;
  /** Old address range for transaction rollback needs. */
  zb_zll_addr_range_t addr_range;
  /** Old group identifiers range for transaction rollback needs. */
  zb_zll_group_id_range_t group_id_range;
  /** Callback-chain data for target-side command handling. */
  zb_zll_command_target_data_t command_data;

} zb_zll_transaction_ctx_t;

/** @brief Touchlink context data */
typedef struct zb_zll_ctx_s
{
  zb_zll_device_info_t zll_device_info;
  zb_zll_transaction_ctx_t zll_tran_ctx;
} zb_zll_ctx_t;

/**
   Returns Touchlink context.
   @return pointer Touchlink context
  */
zb_zll_ctx_t *zb_zll_get_ctx(void);

/**
   Returns Touchlink device info.
   @return pointer Touchlink device info
  */
zb_zll_device_info_t *zb_zll_get_device_info(void);

/**
   Returns Touchlink transaction context.
   @return pointer Touchlink transaction context
  */
zb_zll_transaction_ctx_t *zb_zll_get_transaction_ctx(void);

/** @endcond */ /* internals_doc */

/** Returns pointer to Touchlink context */
#define ZLL_CTX() (*zb_zll_get_ctx())

/** Returns pointer to Touchlink device info */
#define ZLL_DEVICE_INFO() (*zb_zll_get_device_info())

/** Returns pointer to Touchlink transaction context */
#define ZLL_TRAN_CTX() (*zb_zll_get_transaction_ctx())

/** @} */ /* ZB_ZCL_TOUCHLINK_COMMON */

/** @endcond */ /* DOXYGEN_ZCL_SECTION && DOXYGEN_TOUCHLINK_FEATURE */

#endif /* ZBOSS_API_TL_H */
