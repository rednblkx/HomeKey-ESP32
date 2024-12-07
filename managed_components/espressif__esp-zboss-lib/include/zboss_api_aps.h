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
/*  PURPOSE: Public APS layer API
*/
#ifndef ZB_ZBOSS_API_APS_H
#define ZB_ZBOSS_API_APS_H 1

/*! \addtogroup aps_api */
/*! @{ */

/** @addtogroup aps_common_constants APS common constants
  * @{
  */

#include "zb_config.h"
#include "zb_address.h"
#include "zboss_api_aps_interpan.h"

/**
 * @name APS status constants
 * @anchor aps_status
 */
/** @{ */

/** A request has been executed successfully. */
#define ZB_APS_STATUS_SUCCESS               0x00U
/** A transmit request failed since the ASDU is too large and fragmentation is not supported.*/
#define ZB_APS_ASDU_TOO_LONG                0xa0U
/** A received fragmented frame could not be defragmented at the current time. */
#define ZB_APS_DEFRAG_DEFERRED              0xa1U
/** A received fragmented frame could not be defragmented since the device does not support
 * fragmentation. */
#define ZB_APS_DEFRAG_UNSUPPORTED           0xa2U
/** A parameter value was out of range. */
#define ZB_APS_STATUS_ILLEGAL_REQUEST       0xa3U
/** An APSME-UNBIND.request failed due to the requested binding link not existing in the  binding
 * table. */
#define ZB_APS_STATUS_INVALID_BINDING       0xa4U
/** An APSME-REMOVE-GROUP.request has been issued with a group identifier that does not appear in
 * the group table. */
#define ZB_APS_STATUS_INVALID_GROUP         0xa5U
/** A parameter value was invalid or out of range. */
#define ZB_APS_STATUS_INVALID_PARAMETER     0xa6U
/** An APSDE-DATA.request requesting acknowledged transmission failed due to no acknowledgement
 * being received. */
#define ZB_APS_STATUS_NO_ACK                0xa7U
/** An APSDE-DATA.request with a destination addressing mode set to 0x00 failed due to  there being
 * no devices bound to this device. */
#define ZB_APS_STATUS_NO_BOUND_DEVICE       0xa8U
/** An APSDE-DATA.request with a destination addressing mode set to 0x03 failed due to no
 * corresponding short address found in the address map table. */
#define ZB_APS_STATUS_NO_SHORT_ADDRESS      0xa9U
/** An APSDE-DATA.request with a destination addressing mode set to 0x00 failed due to a binding
 * table not being supported on the device. */
#define ZB_APS_STATUS_NOT_SUPPORTED         0xaaU
/** An ASDU was received that was secured using a link key. */
#define ZB_APS_STATUS_SECURED_LINK_KEY      0xabU
/** An ASDU was received that was secured using a network key. */
#define ZB_APS_STATUS_SECURED_NWK_KEY       0xacU
/** An APSDE-DATA.request requesting security has resulted in an error during the corresponding
 * security processing. */
#define ZB_APS_STATUS_SECURITY_FAIL         0xadU
/** An APSME-BIND.request or APSME.ADD-GROUP.request issued when the binding or group tables,
 * respectively, were full. */
#define ZB_APS_STATUS_TABLE_FULL            0xaeU
/** An ASDU was received without any security. */
#define ZB_APS_STATUS_UNSECURED             0xafU
/** An APSME-GET.request or APSME-SET.request has been issued with an unknown attribute identifier.
 */
#define ZB_APS_STATUS_UNSUPPORTED_ATTRIBUTE 0xb0U
/** @} */

/**
 * @brief Type for APS status constants
 *
 * @deprecated holds one of @ref aps_status. Kept only for backward compatibility as
 * @ref aps_status were declared previously as enum. Can be removed in future releases.
 */
typedef zb_uint8_t zb_aps_status_t;

/**
 * @name APS addressing mode constants
 * @anchor aps_addr_mode
 */
/** @{ */
#define ZB_APS_ADDR_MODE_DST_ADDR_ENDP_NOT_PRESENT  0x00U /*!< DstAddress and DstEndpoint not present  */
#define ZB_APS_ADDR_MODE_16_GROUP_ENDP_NOT_PRESENT  0x01U /*!< 16-bit group address for DstAddress; DstEndpoint not present */
#define ZB_APS_ADDR_MODE_16_ENDP_PRESENT            0x02U /*!< 16-bit address for DstAddress and DstEndpoint present */
#define ZB_APS_ADDR_MODE_64_ENDP_PRESENT            0x03U /*!< 64-bit extended address for DstAddress and DstEndpoint present  */
#define ZB_APS_ADDR_MODE_BIND_TBL_ID                0x04U /*!< "destination endpoint" is interpreted as an index in the binding table,
                                                              all other destination address information is ignored */
/** @} */

/**
 * @brief Type for APS addressing mode constants.
 *
 * Holds one of @ref aps_addr_mode. Kept only for backward compatibility as
 * @ref aps_addr_mode were declared previously as enum.
 */
typedef zb_uint8_t zb_aps_addr_mode_t;

/** @brief Minimum valid endpoint number. */
#define ZB_MIN_ENDPOINT_NUMBER 1U

/** @brief Maximum valid endpoint number. */
#define ZB_MAX_ENDPOINT_NUMBER 240U

/** @brief Broadcast endpoint number. */
#define ZB_APS_BROADCAST_ENDPOINT_NUMBER 255U

/** @} */

/** @} */ /* aps_api */

/** @cond internals_doc */
/*! \addtogroup aps_ib */
/*! @{ */

/**
 * @name APS Information Base constants
 * @anchor aps_aib_attr_id
 */
/** @{ */
/** The current set of binding table entries in the device (@see subclause 2.2.8.2.1). */
#define ZB_APS_AIB_BINDING 0xc1U
/** TRUE if the device should become the Zigbee Coordinator on startupU FALSE if otherwise. */
#define ZB_APS_AIB_DESIGNATED_COORD 0xc2U
/** The list of masks of allowable channels for this device to use for network operations. */
#define ZB_APS_AIB_CHANNEL_MASK_LIST 0xc3U
/** The 64-bit address of a network to form or to join. */
#define ZB_APS_AIB_USE_EXT_PANID 0xc4U
/** The current set of group table entries (@see Table 2.25). */
#define ZB_APS_AIB_GROUP_TABLE 0xc5U
/** The value to be used for the NonmemberRadius parameter when using NWK layer multicast. */
#define ZB_APS_AIB_NONMEMBER_RADIUS 0xc6U
/** The current set of permission configuration items. */
#define ZB_APS_AIB_PERMISSION_CONFIG 0xc7U
/** A flag controlling the use of insecure join at startup. */
#define ZB_APS_AIB_USE_INSECURE_JOIN 0xc8U
/** Fragmentation parameter - the standard delayU in millisecondsU between sending two blocks of a
 * fragmented transmission (@see subclause 2.2.8.4.5). */
#define ZB_APS_AIB_INTERFRAME_DELAY 0xc9U
/** The energy measurement for the channel energy scan performed on the previous channel just before
 * a channel change (in accordance with [B1]). */
#define ZB_APS_AIB_LAST_CHANNEL_ENERGY 0xcaU
/** The latest percentage of transmission network transmission failures for the previous channel
 * just before a channel change (in percentage of failed transmissions to the total number of
 * transmissions attempted) */
#define ZB_APS_AIB_LAST_CHANNEL_FAILURE_RATE 0xcbU
/** A countdown timer (in hours) indicating the time to the next permitted frequency agility channel
 * change. A value of NULL indicates the channel has not been changed previously. */
#define ZB_APS_AIB_CHANNEL_TIMER 0xccU
/** A table with the active endpoints and their respective apsMaxWin-dowSize where frag-mentation is
 * used (active endpoints not supporting fragmentations shall be omitted from the list). */
#define ZB_APS_AIB_MAX_WINDOW_SIZE 0xcdU
/** The value of the current countdown timer before the next Parent_annce is sent. */
#define ZB_APS_AIB_PARENT_ANNOUNCE_TIMER 0xceU
/** @} */

/**
 * @brief Type for APS Information Base constants.
 *
 * Holds one of @ref aps_aib_attr_id. Kept for backward compatibility as
 * @ref aps_aib_attr_id were declared previously as enum.
 */
typedef zb_uint8_t zb_aps_aib_attr_id_t;

/*! @} */ /* aps_ib */
/** @endcond */ /* internals_doc */

/** @addtogroup aps_api
 * @{
 */

/** @addtogroup aps_data_service APS data service
  * @{
  */

/** @brief Parsed APS header.
 */
typedef ZB_PACKED_PRE struct zb_aps_hdr_s
{
  /* TODO: rearrange fields to make that structure packet and exclude "packed" attribute. Don't forget to update NCP side. */
  zb_uint8_t  fc;              /*!< Frame control. */
  zb_uint16_t src_addr;        /*!< Source address of packet originator. */
  zb_uint16_t dst_addr;        /*!< The destination address of the packet. */
  zb_uint16_t group_addr;      /*!< If send to group then group address destination else 0. */
  zb_uint8_t  dst_endpoint;    /*!< The destination endpoint of the packet receiver. */
  zb_uint8_t  src_endpoint;    /*!< The source endpoint from which that packet was send. */
  zb_uint16_t clusterid;       /*!< The identifier of the cluster on the source device. */
  zb_uint16_t profileid;        /*!< Profile ID */
  zb_uint8_t  aps_counter;      /*!< APS Counter for check APS dup command. */
  zb_uint16_t mac_src_addr;     /*!< Source address of device that transmit that packet. */
  zb_uint16_t mac_dst_addr;     /*!< Next hop address used for frame transmission. */
  zb_uint8_t lqi;
  zb_int8_t rssi;
  /* attributes of the key used to unsecure this frame */
  zb_bitfield_t aps_key_source:1;    /*!< @if ZB_SECUR
                                   *     @ref secur_key_sources
                                   *   @endif
                                   */
  zb_bitfield_t aps_key_attrs:2;     /*!< @if ZB_SECUR
                                   *      @ref secur_key_attributes
                                   *   @endif
                                   */
  zb_bitfield_t aps_key_from_tc:1; /* Denotes that packet received from TC and properly
                                    * encrypted with TCLK by any appropriate method:
                                    * 1) BDB Request key (with Key Type: TCLK) ->
                                    *    Transport Key -> Verify Key
                                    * 2) BDB+SE Mixed CBKE
                                    * 3) SE CBKE */
  zb_bitfield_t extended_fc:2;
  zb_bitfield_t reserved:2;

  #define FIRST_INTERNAL_APSIND_FIELD tsn
  zb_uint8_t tsn;               /*!< Transaction sequence number for ZDO/ZCL command. */
  zb_uint8_t block_num;         /*!< Fragmentation: block number. */
  zb_uint8_t block_ack;         /*!< Fragmentation: block ack. */
  zb_uint8_t radius;            /*!< radius from nwk header */
  zb_uint8_t align[7];
} ZB_PACKED_STRUCT zb_aps_hdr_t;


/**
    Parameters of the APSDE-DATA.indication primitive.

    Also used as a parameter in different APS calls.
 */
typedef zb_aps_hdr_t zb_apsde_data_indication_t;
ZB_ASSERT_IF_NOT_ALIGNED_TO_16(zb_apsde_data_indication_t);

/** @} */ /* APS data service */
/** @} */

/** @addtogroup aps_api
 *  @{
 */

/** @addtogroup aps_management_service APS management service
  * @{
  */

/**
 * @brief APSME binding structure.
 *
 * This data structure passed to @ref zb_apsme_bind_request()
 * and to @ref zb_apsme_unbind_request().
 *
 * @deprecated Corresponding function @ref zb_apsme_unbind_request() is deprecated and it will be
 * moved to the private header in November 2022. Do not use this structure and corresponding function
 * in the applications.
 */
typedef struct zb_apsme_binding_req_s
{
  zb_ieee_addr_t  src_addr;       /*!< The source IEEE address for the binding entry. */
  zb_uint8_t      src_endpoint;   /*!< The source endpoint for the binding entry. */
  zb_uint16_t     clusterid;      /*!< The identifier of the cluster on the source
                                        device that is to be bound to the destination device.*/
  zb_uint8_t      addr_mode;      /*!< The type of destination address supplied by
                                       the DstAddr parameter - see @ref aps_addr_mode  */
  zb_addr_u       dst_addr;       /*!< The destination address for the binding entry. */
  zb_uint8_t      dst_endpoint;   /*!< This parameter will be present only if
                                       the DstAddrMode parameter has a value of
                                       0x03 and, if present, will be the
                                       destination endpoint for the binding entry.*/
#ifdef SNCP_MODE
  zb_uint8_t       remote_bind;   /*!< Indication if the bind req is local or remote */
  zb_uint8_t       id;             /*!< unique identifier of the entry for NCP, updated only in
                                    zb_apsme_bind_request and zb_apsme_unbind_request and used to
                                    notify NCP */
  /* confirm_cb is not sent in payload by NCP, keep it in the end of this structure */
#endif
  zb_callback_t   confirm_cb;     /*!< The callback to be called when the operation is completed. */
} zb_apsme_binding_req_t;


/**
  * This data structure passed to @ref zb_zdo_check_binding_request().
  */
typedef struct zb_aps_check_binding_req_s
{
  zb_uint8_t src_endpoint; /* a source endpoint, if ZB_ZCL_BROADCAST_ENDPOINT - any endpoint */
  zb_uint16_t cluster_id; /* a source cluster ID */
  zb_callback_t response_cb;
} zb_aps_check_binding_req_t;


/**
  * This data structure passed to callback passed to @ref zb_zdo_check_binding_request().
  */
typedef struct zb_aps_check_binding_resp_s
{
  zb_uint8_t src_endpoint; /* a source endpoint from the corresponding request */
  zb_uint16_t cluster_id; /* a source cluster ID from the corresponding request */
  zb_bool_t exists; /* whether the binding with specified parameters exists */
} zb_aps_check_binding_resp_t;

/** @brief APSME-ADD-GROUP.request primitive parameters, should be passed to @ref zb_zdo_add_group_req() */
typedef struct zb_apsme_add_group_req_s
{
  zb_uint16_t     group_address; /*!< The 16-bit address of the group being added.  */
  zb_uint8_t      endpoint;      /*!< The endpoint to which the given group is being added.  */
  zb_callback_t   confirm_cb;    /*!< The callback to be called when the operation is completed. */
} zb_apsme_add_group_req_t;

/** @brief APSME-ADD-GROUP.confirm primitive parameters. */
typedef struct zb_apsme_add_group_conf_s
{
  zb_uint16_t group_address;    /*!< The 16-bit address of the group being added.  */
  zb_uint8_t  endpoint;         /*!< The endpoint to which the given group is being added.  */
  zb_ret_t  status;           /*!< Request send status. */
} zb_apsme_add_group_conf_t;

/** @brief APSME-REMOVE-GROUP.request primitive parameters, should be passed to @ref zb_zdo_add_group_req()  */
typedef struct zb_apsme_add_group_req_s zb_apsme_remove_group_req_t;

/** @brief APSME-REMOVE-GROUP.confirm primitive parameters.  */
typedef struct zb_apsme_add_group_conf_s zb_apsme_remove_group_conf_t;

/** @brief APSME-REMOVE-ALL-GROUPS.request primitive parameters, should be passed to @ref zb_zdo_remove_all_groups_req().  */
typedef struct zb_apsme_remove_all_groups_req_s
{
  zb_uint8_t    endpoint;      /*!< The endpoint to which the given group is being removed. */
  zb_callback_t confirm_cb;    /*!< The callback to be called when the operation is completed. */
} zb_apsme_remove_all_groups_req_t;

/** @brief APSME-REMOVE-ALL-GROUPS.confirm primitive parameters.  */
typedef struct zb_apsme_remove_all_groups_conf_s
{
  zb_uint8_t  endpoint;         /*!< The endpoint which is to be removed from all groups. */
  zb_ret_t  status;           /*!< The status of the request to remove all groups. */
} zb_apsme_remove_all_groups_conf_t;

zb_bool_t zb_aps_is_endpoint_in_group(
    zb_uint16_t group_id,
    zb_uint8_t endpoint);

/** @cond DOXYGEN_INTERNAL_DOC */
/** @brief APSME GET request structure. */
typedef struct zb_apsme_get_request_s
{
  zb_aps_aib_attr_id_t aib_attr;       /*!< The identifier of the AIB attribute to read. */
  zb_callback_t        confirm_cb;     /*!< User's callback to be called as APSME-GET.confirm  */
} zb_apsme_get_request_t;

/** @brief APSME GET confirm structure. */
typedef ZB_PACKED_PRE struct zb_apsme_get_confirm_s
{
  zb_ret_t status;         /*!< The results of the request to read an AIB attribute value. */
  zb_aps_aib_attr_id_t aib_attr;  /*!< The identifier of the AIB attribute that was read.*/
  zb_uint8_t aib_length;          /*!< The length, in octets, of the attribute value being returned.*/
  /* Various */                   /* The value of the AIB attribute that was read.*/
} ZB_PACKED_STRUCT zb_apsme_get_confirm_t;

/** @brief APSME SET request structure. */
typedef ZB_PACKED_PRE struct zb_apsme_set_request_s
{
  zb_aps_aib_attr_id_t aib_attr; /*!< The identifier of the AIB attribute to be written. */
  zb_uint8_t           aib_length; /*!< The length, in octets, of the attribute value being set. */
  zb_callback_t        confirm_cb; /*!< User's callback to be called as APSME-SET.confirm  */
  /* Various */                   /* The value of the AIB attribute that should be written. */
} ZB_PACKED_STRUCT zb_apsme_set_request_t;

/** @brief APSME SET confirm structure. */
typedef ZB_PACKED_PRE struct zb_apsme_set_confirm_s
{
  zb_ret_t   status;       /*!< The result of the request to write the AIB Attribute. */
  zb_aps_aib_attr_id_t aib_attr;  /*!< The identifier of the AIB attribute that was written. */
} ZB_PACKED_STRUCT zb_apsme_set_confirm_t;

/** @endcond */ /* DOXYGEN_INTERNAL_DOC */

/**
 * @brief APSME-BIND.request primitive.
 * @param param - index of buffer containing request data (see @ref
 * zb_apsme_binding_req_t).
 *
 * @deprecated This function will be moved to the private header in November 2022. Use @ref
 * zb_zdo_bind_req() instead.
 */
void zb_apsme_bind_request(zb_uint8_t param);

/**
 * @brief APSME-UNBIND.request primitive.
 * @param param - index of buffer containing request data (see @ref
 * zb_apsme_binding_req_t).
 *
 * @deprecated This function will be moved to the private header in November 2022. Use @ref
 * zb_zdo_unbind_req() instead.
 */
void zb_apsme_unbind_request(zb_uint8_t param);

/**
 *@brief Perform unbind all entries. This custom function and it is not described
 * in Zigbee specification.
 * @param param - not used.
 *
 * @deprecated This function will be moved to the private header in November 2022. Use @ref
 * zb_zdo_unbind_all_local() instead.
 */
void zb_apsme_unbind_all(zb_uint8_t param);

/**
 * @brief Checks if the binding with specified parameters exists
 *
 * @deprecated This function will be moved to the private header in November 2022. Use @ref
 * zb_zdo_check_binding_request() instead.
 */
void zb_aps_check_binding_request(zb_bufid_t param);

/**
 * @brief APSME-ADD-GROUP.request primitive.
 *
 * @param param - index of buffer with parameter. See @ref zb_apsme_add_group_req_t.
 *
 * @deprecated This function will be moved to the private header in November 2022. Use @ref
 * zb_zdo_add_group_req() instead.
 */
void zb_apsme_add_group_request(zb_uint8_t param);

/**
 * @brief APSME-REMOVE-GROUP.request primitive.
 *
 * @internal
 * Use macro @ref ZDO_REGISTER_CALLBACK to register APSME-REMOVE-GROUP.confirm callback.
 * @endinternal
 * @param param - index of buffer with parameter. See @ref zb_apsme_remove_group_req_t.
 *
 * @deprecated This function will be moved to the private header in November 2022. Use @ref
 * zb_zdo_remove_group_req() instead.
 *
 */
void zb_apsme_remove_group_request(zb_uint8_t param);

/**
 * @brief APSME-REMOVE-ALL-GROUPS.request primitive.
 *
 * @internal
 * Use macro @ref ZDO_REGISTER_CALLBACK to register APSME-REMOVE-ALL-GROUPS.confirm callback.
 * @endinternal
 * @param param - index of buffer with parameter. See @ref zb_apsme_remove_all_groups_req_t.
 *
 * @deprecated This function will be moved to the private header in November 2022. Use @ref
 * zb_zdo_remove_all_groups_req() instead.
 *
 */
void zb_apsme_remove_all_groups_request(zb_uint8_t param);

/** @} */ /* APS management service data structures and API. */

/*! @} */ /* aps_api */


#ifdef APS_FRAGMENTATION


void zb_aps_add_max_trans_size(zb_uint16_t short_addr, zb_uint16_t max_trans_size, zb_uint8_t max_buffer_size);
zb_uint16_t zb_aps_get_max_trans_size(zb_uint16_t short_addr);
zb_uint8_t zb_aps_get_max_buffer_size(zb_uint16_t short_addr);

#endif

#ifdef ZB_APS_USER_PAYLOAD
/** @addtogroup aps_api
 * @{
 */

/** @addtogroup aps_user_payload APS user payload
 * @{
 */

/*!
 * @brief
 * Callback to notify an application of sending an APS user payload status
 *
 * @param param - index of buffer with an APS user payload
 */
typedef void (*zb_aps_user_payload_callback_t)(zb_uint8_t param);

/**
 * @brief NLDE-DATA.indication data callback
 *
 * @param[in] param index of buffer with an APS indication
 *
 * @return
 *    - true: Processed
 *    - false: Not processed
*/
typedef zb_bool_t (*zb_aps_indication_data_callback_t)(zb_uint8_t param);

/*!
 * @brief
 * APS user payload transmission status
 *
 * Stored in the buf->u.hdr.status field
 */
typedef enum zb_aps_user_payload_cb_status_e
{
  /*! APS user payload transmission is successful*/
  ZB_APS_USER_PAYLOAD_CB_STATUS_SUCCESS    = (zb_uint8_t)0x00,
  /* Failed to transmit APS user payload - No confirmation from APS*/
  ZB_APS_USER_PAYLOAD_CB_STATUS_NO_APS_ACK = (zb_uint8_t)0xbe
} zb_aps_user_payload_cb_status_t;

/*!
 * @brief Send user payload using APS
 *
 * @param param - reference to a buffer
 * @param dst_addr - destination address
 * @param profile_id - profile ID for which this frame is intended for
 * @param cluster_id - object ID for which this frame is intended for
 * @param dst_endpoint - either the endpoint ID
 *                       of the entity to which the ASDU is being
 *                       transferred or the broadcast endpoint (0xff)
 * @param src_endpoint - the individual endpoint of the entity from
 *                       which the ASDU is being transferred
 * @param addr_mode - the type of destination address supplied by
 *                    the dst_addr parameter - see @ref aps_addr_mode
 * @param aps_ack_is_enabled - enable (ZB_TRUE) or disable (ZB_FALSE) APS acknowledging
 * @param *payload_ptr - pointer to user data
 * @param payload_size - size of user data; not more than 82 bytes
 *
 * @return RET_OK - if transmission was successful scheduled;
 * @return RET_INVALID_PARAMETER_1 - if the buffer is invalid
 * @return RET_INVALID_PARAMETER_2 - if the payload_ptr parameter is invalid
 * @return RET_INVALID_PARAMETER_3 - if the payload_size parameter is too large
 */
zb_ret_t zb_aps_send_user_payload(
  zb_uint8_t param,
  zb_addr_u dst_addr,
  zb_uint16_t profile_id,
  zb_uint16_t cluster_id,
  zb_uint8_t dst_endpoint,
  zb_uint8_t src_endpoint,
  zb_uint8_t addr_mode,
  zb_bool_t aps_ack_is_enabled,
  zb_uint8_t *payload_ptr,
  zb_uint8_t payload_size);

/*!
 * @brief Get pointer to an APS payload
 *
 * @param param - reference to a buffer
 * @param aps_payload_size - [out] pointer to a variable to store
 *                           an APS payload size
 *
 * @return Pointer to an APS payload;
 * @return NULL if a buffer is invalid or an aps_payload_size pointer is invalid.
 */
zb_uint8_t *zb_aps_get_aps_payload(zb_uint8_t param, zb_uint8_t *aps_payload_size);

/*!
 * @brief Set callback to notify results of transmitting
 *        an APS frame with user payload
 *
 * @param cb - pointer to a callback
 */
void zb_aps_set_user_data_tx_cb(zb_aps_user_payload_callback_t cb);

/**
 * @brief Set callback to notify information of APS frame
 *
 * @param cb - pointer to a callback
 */
void zb_aps_set_indication_data_rx_cb(zb_aps_indication_data_callback_t cb);

/*!
 * @brief Set callback to be called when ZDO command packet is sent.
 *
 * That callback is to be used when application wants to know the fact that ZDO
 * command send is completed.
 *
 * Callback provided to ZBOSS ZDO request function (second parameter of
 * zb_zdo_node_desc_req and similar function) not always can be used for that
 * purpose. ZBOSS calls ZDO callback immediately when a) unicast transmit of APS
 * message failed (no ACK) and b) this is ZDO message that does not suppose to
 * have an answer, like broadcast NWK Update req.  In other cases if callback is
 * set using zb_af_set_zdo_data_conf_cb, it is called when outgoing ZDO command
 * is sent; callback passed to zb_zdo_node_desc_req and friends is called when
 * ZBOSS got response/responses to ZDO command, or ZDO command is timed out.
 *
 * @param cb - callback. The buffer passed to the callback has zb_apsde_data_confirm_t in parameters section.
 */
void zb_af_set_zdo_data_conf_cb(zb_callback_t cb);

/** @} */ /* aps_user_payload */

/** @} */

#endif /* #ifdef ZB_APS_USER_PAYLOAD */

#endif /* #ifndef ZB_ZBOSS_API_APS_H */
