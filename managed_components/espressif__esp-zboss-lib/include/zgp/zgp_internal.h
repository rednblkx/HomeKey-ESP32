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
/* PURPOSE: Interface for ZGP endpoint (both sink and proxy)
*/

#ifndef ZGP_INTERNAL_H
#define ZGP_INTERNAL_H 1


#ifdef ZB_ENABLE_ZGP


/**
   @cond internals_doc
   @addtogroup zgp_internal
   @{
*/

/**
 * @brief Security key sub-field value of Extended NWK frame control field
 *        derived from ZGPD key type (see @ref zb_zgp_security_key_type_e)
 *
 * ZGP spec, A.1.4.1.3
 * The Security Key sub-field, if set to 0b1, indicates an individual key
 * (KeyType 0b100 or 0b111). If set to 0b0, it indicates a shared key
 * (KeyType 0b011, 0b01017 or 0b001) or no key.
 */
#define ZGP_KEY_TYPE_IS_INDIVIDUAL(kt)                                  \
  ((kt) == ZB_ZGP_SEC_KEY_TYPE_ZGPD_INDIVIDUAL                          \
   || (kt) == ZB_ZGP_SEC_KEY_TYPE_DERIVED_INDIVIDUAL)

#ifndef ZB_ZGPD_ROLE

#define ZB_ZGP_MAX_CONTACT_STATUS_BITS          8
#define ZB_ZGP_MAX_GPDF_CMD_PAYLOAD_APP_ID_0000 59
#define ZB_ZGP_MAX_GPDF_CMD_PAYLOAD_APP_ID_0010 54

#define ZB_ZGP_UNSEL_TEMP_MASTER_IDX   0xFF
#define ZB_ZGP_TEMP_MASTER_EMPTY_ENTRY 0xFFFF

#define ZB_APP_DESCR_TIMEOUT (20*ZB_TIME_ONE_SECOND)
#define ZB_ZGP_MULTI_SENSOR_COMMISSIONING_TIMEOUT (20*ZB_TIME_ONE_SECOND) /* A.3.6.2.4 */

#define ZB_ZGPS_CMD_PROCESSING_POSTPONED_MS 100U

typedef ZB_PACKED_PRE struct zb_zgp_gp_proxy_info_s
{
  zb_uint16_t short_addr;
  zb_uint8_t  link;
}ZB_PACKED_STRUCT zb_zgp_gp_proxy_info_t;

/** @brief possible types of GP device */
typedef enum zgp_gp_device_e {
  ZGP_DEVICE_PROXY       = 0x0060,
  ZGP_DEVICE_PROXY_BASIC = 0x0061,
  ZGP_DEVICE_TARGET_PLUS = 0x0062,
  ZGP_DEVICE_TARGET      = 0x0063,
  ZGP_DEVICE_COMMISSIONING_TOOL  = 0x0064,
  ZGP_DEVICE_COMBO       = 0x0065,
  ZGP_DEVICE_COMBO_BASIC = 0x0066
} zgp_gp_device_t;

/*! @} */

/* Structure stores GreenPower security and commissioning parameters */
typedef ZB_PACKED_PRE struct zb_zgp_cluster_s
{
  /* FIXME: Decrease memory consumed by: a) move this field after arrays; b) use bitfields where possible. */
  zb_uint8_t  gp_shared_security_key_type; /**< @see zb_zgp_shared_security_key_type_t */
  zb_uint8_t  gp_shared_security_key[ZB_CCM_KEY_SIZE];
  zb_uint8_t  gp_link_key[ZB_CCM_KEY_SIZE];
  zb_uint8_t  gps_communication_mode; /**< @see zgp_communication_mode_t  */
  zb_uint8_t  gps_commissioning_exit_mode; /**< @see zgp_commissioning_exit_mode_t  */
  zb_uint8_t  gps_security_level;
  zb_uint16_t gps_commissioning_window;
  /* let's align it to 16 bytes (38 + 10) */
  zb_uint16_t align_dummy[5];
} ZB_PACKED_STRUCT zb_zgp_cluster_t;

typedef struct zb_zgp_tbl_array_s
{
  zb_uint32_t back_sec_counter;
  zb_uint32_t security_counter;
  zb_uint32_t nvram_offset;
  zb_uint8_t  lqi;
  zb_int8_t   rssi;

  /* bit 0 - EntryValid flag
   * bit 1 - FirstToForward flag
   * bit 2 - HasAllUnicastRoutes flag
   * bits 3-7 reserved
 */
  zb_uint8_t runtime_options;
  zb_uint8_t search_counter;
} zb_zgp_tbl_array_t;

#if defined ZB_ENABLE_ZGP_SINK && defined ZB_ENABLE_ZGP_PROXY
#define ZB_ZGP_SEC_CNT_TIMEOUT_TBL_SIZE (((ZB_ZGP_PROXY_TBL_SIZE > ZB_ZGP_SINK_TBL_SIZE) ? ZB_ZGP_PROXY_TBL_SIZE : ZB_ZGP_SINK_TBL_SIZE + 7) / 8)
#else
#ifdef ZB_ENABLE_ZGP_SINK
#define ZB_ZGP_SEC_CNT_TIMEOUT_TBL_SIZE ((ZB_ZGP_SINK_TBL_SIZE + 7) / 8)
#endif  /* ZB_ENABLE_ZGP_SINK */
#ifdef ZB_ENABLE_ZGP_PROXY
#define ZB_ZGP_SEC_CNT_TIMEOUT_TBL_SIZE ((ZB_ZGP_PROXY_TBL_SIZE + 7) / 8)
#endif  /* ZB_ENABLE_ZGP_PROXY */
#endif  /* defined ZB_ENABLE_ZGP_SINK && defined ZB_ENABLE_ZGP_PROXY */

typedef struct zb_zgp_tbl_s
{
  zgp_tbl_ent_t cached;
  /* pack 8 4-bit entries to every array element */
  zb_uint32_t security_counter_timeouts[ZB_ZGP_SEC_CNT_TIMEOUT_TBL_SIZE];
  zb_uint_t cached_i;
  zb_uint_t entry_size;
  zb_uint_t tbl_size;
  zb_uint8_t nvram_dataset;     /*<! @see zb_nvram_dataset_types_t  */
  zb_uint8_t nvram_page;

  zb_zgp_tbl_array_t array[1];
} zb_zgp_tbl_t;

#ifdef ZB_ENABLE_ZGP_PROXY
typedef struct zb_zgp_proxy_table_s
{
  zb_zgp_tbl_t base;

  zb_zgp_tbl_array_t array[ZB_ZGP_PROXY_TBL_SIZE - 1];
} zb_zgp_proxy_table_t;
#endif  /* ZB_ENABLE_ZGP_PROXY */

#ifdef ZB_ENABLE_ZGP_SINK
typedef struct zb_zgp_sink_table_s
{
  zb_zgp_tbl_t base;

  zb_zgp_tbl_array_t array[ZB_ZGP_SINK_TBL_SIZE - 1];
} zb_zgp_sink_table_t;
#endif  /* ZB_ENABLE_ZGP_SINK */
#endif  /* !ZB_ZGPD_ROLE */
/**
 * @brief MAC addressing fields of GPDF as they are transmitted over the air.
 *
 * This structure is used during security processing for
 * security level @ref ZB_ZGP_SEC_LEVEL_REDUCED.
 *
 * From ZGP spec, A.1.5.4.2 "Initialization":
 * Header = MAC sequence number || MAC addressing fields || NWK Frame Control || Extended NWK
 * Frame Control || SrcID.
 *
 * When ZGP or ZGPD is encrypting message, MAC addressing fields of the packet are not ready.
 * They will be calculated only on the MAC layer. This is a problem and dirty hack is implemented:
 * MAC addressing fields are filled at ZGP level.
 *
 * For incoming frames MAC addressing fields are just copied from incoming frames.
 *
 * This union contains MAC addressing fields of GPDF as they are transmitted over the air.
 */
typedef ZB_PACKED_PRE union zb_gpdf_mac_addr_flds_u
{
  ZB_PACKED_PRE struct zgp_mac_addr_flds_short_s
  {
    zb_uint16_t  dst_addr;
    zb_uint16_t  dst_pan_id;
  }
  ZB_PACKED_STRUCT s;

  ZB_PACKED_PRE struct zgp_mac_addr_flds_long_s
  {
    zb_ieee_addr_t  addr;
  }
  ZB_PACKED_STRUCT l;

  ZB_PACKED_PRE struct zgp_mac_addr_flds_combined_s
  {
    zb_uint16_t     dst_addr;
    zb_uint16_t     dst_pan_id;
    zb_ieee_addr_t  src_addr;
  }
  ZB_PACKED_STRUCT comb;
#ifndef ZB_ZGPD_ROLE
  zb_zgp_gp_proxy_info_t proxy_info;
#endif  /* !ZB_ZGPD_ROLE */

  zb_uint8_t in[12];         /**< MAC addressing fields of incoming frame */
}
zb_gpdf_mac_addr_flds_t;

/**
 * @brief Parsed values of GPDF frame
 *
 * Structure contains GPDF information that is needed by ZGP stub and
 * upper layers.
 */
typedef ZB_PACKED_PRE struct zb_gpdf_info_s
{
  zb_time_t                 recv_timestamp;    /**< Packet reception time */
  zb_gpdf_mac_addr_flds_t   mac_addr_flds;     /**< MAC addressing fields */
  zb_uint8_t                mac_addr_flds_len; /**< Length of data in MAC addressing fields */
  zb_uint8_t                mac_seq_num;       /**< MAC sequence number */
  zb_uint8_t                nwk_frame_ctl;     /**< NWK frame control */
  zb_uint8_t                nwk_ext_frame_ctl; /**< Extended NWK frame control */
  zb_zgpd_id_t              zgpd_id;           /**< ZGPD ID */
  zb_uint32_t               sec_frame_counter; /**< Security frame counter */
  zb_uint8_t                zgpd_cmd_id;       /**< ZGPD command ID */
  zb_uint8_t                nwk_hdr_len;       /**< Length of the GPDF NWK header */
  zb_uint8_t                lqi;               /**< LQI value from alien MAC */
  zb_int8_t                 rssi;              /**< RSSI value from alien MAC */
  zb_uint8_t                status;            /**< 'status' for GP-DATA.indication @see  */
  zb_uint8_t                key[ZB_CCM_KEY_SIZE];/**< Key to decrypt the frame */
  zb_uint8_t                key_type;       /**< type of key to be used for
                                                 that frame @see zb_zgp_security_key_type_e */
  zb_uint8_t                mic[ZB_CCM_M];
  zb_bitfield_t             rx_directly:1;       /**< If 1, received directly by
                                                   * GP stub, else got from GPP */
  zb_bitfield_t             recv_as_unicast:1; /**< Unicast mode of
                                                   * packet received via GP Notification. */
  /* WARNING: Aligned to 62 bytes here - struct is not stored in NVRAM so it is ok.
     Aligning to 64 bytes (closest multiple of 4) brings strange errors and overlapping of data!
     TODO: Debug overlapping. Possibly it is better to divide this struct to some parts and do not
     play with such big structure in the packet tail. Here we have timestamps, lqi/rssi, mac, nwk
     and security payloads at the same structure!
     EE: maybe, can win 1 byte by using bitfield for key type so we can remove 1 byte of align?
     Also, recheck twice: do we need all that info in GPPB? Some can came from Target- and can be not used now.
     TC: If unaligned, the following allocation may lead to memory corruption:
        zgp_gp_notif_iterator_t *it = zb_buf_get_tail(param, sizeof(zgp_gp_notif_iterator_t) + sizeof(zb_gpdf_info_t));
        zb_gpdf_info_t *gpdf_info = ZB_BUF_GET_PARAM(param, zb_gpdf_info_t);
      The zb_buf_get_tail always returns aligned pointer, so if the zb_gpdf_info_t is unaligned, the gpdf_info pointer
      will point to some bytes inside the zgp_gp_notif_iterator_t.
      There are around 8 allocations of this type inside zgp_sink.c and zgp_cluster_gp.c files.
  */
  zb_bufid_t buf_iterator;
}
ZB_PACKED_STRUCT zb_gpdf_info_t;

typedef struct zgp_runtime_app_description_ctx_s
{
  zb_gpdf_info_t gpdf_info;
  zb_uint8_t     report_idx;
  zb_uint8_t     point_desc_offset;
}zgp_runtime_app_description_ctx_t;

#ifndef ZB_ZGPD_ROLE
#ifdef ZB_ENABLE_ZGP_DIRECT
typedef ZB_PACKED_PRE struct zb_cgp_data_req_s
{
  zb_uint8_t      tx_options;
  zb_uint8_t      src_addr_mode;
  zb_uint16_t     src_pan_id;
  zb_addr_u src_addr;
  zb_uint8_t      dst_addr_mode;
  zb_uint16_t     dst_pan_id;
  zb_addr_u dst_addr;
  zb_uint8_t      buf_ref;
  zb_uint8_t      handle;
  zb_time_t       recv_timestamp;
}
ZB_PACKED_STRUCT
zb_cgp_data_req_t;
#endif  /* ZB_ENABLE_ZGP_DIRECT */

typedef enum gp_sec_resp_e
{
  GP_SEC_RESPONSE_DROP_FRAME,
  GP_SEC_RESPONSE_PASS_UNPROCESSED,
  GP_SEC_RESPONSE_TX_THEN_DROP,
  GP_SEC_RESPONSE_MATCH,
  GP_SEC_RESPONSE_TX_THEN_PASS_COMMISSIONING_UNPROCESSED,
  GP_SEC_RESPONSE_ILLEGAL = 0xff
} gp_sec_resp_t;

typedef enum gp_data_ind_status_e
{
  GP_DATA_IND_STATUS_SECURITY_SUCCESS,
  GP_DATA_IND_STATUS_NO_SECURITY,
  GP_DATA_IND_STATUS_AUTH_FAILURE,
  GP_DATA_IND_STATUS_UNPROCESSED,
  GP_DATA_IND_STATUS_COMMISSIONING_UNPROCESSED
} gp_data_ind_status_t;

#define ZGP_PROXY_LWADDR_INVALID_IDX 0xff
#define ZGP_PROXY_GROUP_INVALID_IDX 0xffff
#define ZGP_PROXY_GROUP_DERIVED_ALIAS 0xffff
#define ZGP_PROXY_GP_NOTIF_ITERATOR_INVALID_IDX 0xff

/*
  entry with the exact GPD ID as in the GPDF and Endpoint = 0xff, or - if the
  Endpoint in the GP-DATA.indication is 0xff  or 0x00 - if it has an entry with
  the exact GPD ID
*/
#define GP_ENPOINT_MATCH(gpdf_ep, table_ep)     \
  ((gpdf_ep) == (table_ep)                      \
   || (table_ep) == 0xff                        \
    || (gpdf_ep) == 0                           \
    || (gpdf_ep) == 0xff)

/* ZGP spec, A.3.6.1.3.1:
 *
 * For the incremental sequence number (when SecurityLevel = 0b00), the
 * counter must be allowed to roll over, because of the limited
 * sequence number length of 1 octet, so care must be taken when
 * comparing for freshness.  It is recommended that this comparison be
 * accomplished as follows:
 * define
 * a = sequence number stored by the GPP/GPS;
 * b = sequence number from the GPDF;
 * if 1 ≤ ( (b – a)mod 256 ) < 128 accept GPDF;
 * else drop GPDF;
 */

/* NK: Skip freshness check for some special cases, for example, to do not drop packets after ZGP
 * reset. This behaviour is not clear in general, seems like it is temporary feature. */
#ifdef ZB_ZGP_SKIP_FRESHNESS_CHECK
#define ZB_ZGP_CHECK_MAC_SEQ_NUM_FRESHNESS(prev_sn, new_sn, ret)        \
{                                                                       \
  ret = ZB_TRUE;                                                        \
}
#else
#define ZB_ZGP_CHECK_MAC_SEQ_NUM_FRESHNESS(prev_sn, new_sn, ret)                                         \
{                                                                                                        \
  /* Modular arithmetic takes place here, result value is "wrap around" */                               \
  /* NK: Another overflow processing. */                                                                 \
  /*     For example, prev_sn is 125, new_sn is 9; in this case: */                                      \
  /*     (new_sn - prev_sn) = 9 - 125 (overflowed) = 255 - 116 = 139 > 128 - frame dropped  */           \
  /*     abs(new_sn - prev_sn) = 125 - 9 = 116 < 128 - frame passed  */                                  \
  /*     It is needed to avoid too many drop situations after ZGP reset.  */                             \
  /* NK:TODO: We do not store this counter in nvram, so when GW reset occurs, it will skip some pkts. */ \
  zb_uint8_t diff = (new_sn > prev_sn) ? (new_sn - prev_sn) : (prev_sn - new_sn);                        \
  /* Check that diff in seq nums falls into half of seq num value range */                               \
  ret = ((diff > 0) && (diff < 128)) ? ZB_TRUE : ZB_FALSE;                                               \
}
#endif

/********************************************************************/
/****************** ZGP Proxy table definitions *********************/
/********************************************************************/
#define ZGP_TBL_IS_SINK(ent) ((ent)->is_sink==ZB_TRUE)

#define ZGP_TBL_SINK_GET_SEC_PRESENT(ent) (((ent)->options & (1<<9)) ? 1 : 0)
#define ZGP_TBL_PROXY_GET_SEC_PRESENT(ent) (((ent)->options & (1<<14)) ? 1 : 0)

#define ZGP_TBL_GET_SEC_PRESENT(ent) ( ZGP_TBL_IS_SINK(ent) ? ZGP_TBL_SINK_GET_SEC_PRESENT(ent) : ZGP_TBL_PROXY_GET_SEC_PRESENT(ent))

#define ZGP_TBL_SINK_GET_SEC_KEY_TYPE(ent) (((ent)->options & (1<<9)) ? (((ent)->sec_options >> 2) & 7) : 0)
#define ZGP_TBL_PROXY_GET_SEC_KEY_TYPE(ent) (((ent)->options & (1<<14)) ? (((ent)->sec_options >> 2) & 7) : 0)

#define ZGP_TBL_GET_SEC_KEY_TYPE(ent) ( ZGP_TBL_IS_SINK(ent) ? ZGP_TBL_SINK_GET_SEC_KEY_TYPE(ent) : ZGP_TBL_PROXY_GET_SEC_KEY_TYPE(ent))

#define ZGP_SINK_GET_SEC_LEVEL(opt) ((opt) & 3)

#define ZGP_TBL_SINK_GET_SEC_LEVEL(ent)    (((ent)->options & (1<<9)) ? ZGP_SINK_GET_SEC_LEVEL((ent)->sec_options): 0)
#define ZGP_TBL_PROXY_GET_SEC_LEVEL(ent)    (((ent)->options & (1<<14)) ? ZGP_SINK_GET_SEC_LEVEL((ent)->sec_options): 0)

#define ZGP_TBL_GET_SEC_LEVEL(ent)    ( ZGP_TBL_IS_SINK(ent) ? ZGP_TBL_SINK_GET_SEC_LEVEL(ent) : ZGP_TBL_PROXY_GET_SEC_LEVEL(ent))

#define ZGP_TBL_GET_SEQ_NUM_CAP(ent) ((ent)->options & (1<<5))
#define ZGP_TBL_GET_APP_ID(ent) ((ent)->options & 7)
/**
   Get communication mode by Proxy/Sink table entry
   @see zgp_communication_mode_t
 */
#define ZGP_TBL_SINK_GET_COMMUNICATION_MODE(ent)    (((ent)->options >> 3) & 3)

#define ZGP_TBL_SINK_GET_FIXED_LOCATION(ent) (((ent)->options >> 7) & 1)
#define ZGP_TBL_PROXY_GET_FIXED_LOCATION(ent) (((ent)->options >> 11) & 1)

#define ZGP_TBL_GET_FIXED_LOCATION(ent)    ( ZGP_TBL_IS_SINK(ent) ? ZGP_TBL_SINK_GET_FIXED_LOCATION(ent) : ZGP_TBL_PROXY_GET_FIXED_LOCATION(ent))

#define ZGP_TBL_SINK_GET_ASSIGNED_ALIAS(ent) (((ent)->options >> 8) & 1)
#define ZGP_TBL_PROXY_GET_ASSIGNED_ALIAS(ent) (((ent)->options >> 13) & 1)

#define ZGP_TBL_SINK_SET_ASSIGNED_ALIAS(ent) (ent)->options |= (1<<8)
#define ZGP_TBL_PROXY_SET_ASSIGNED_ALIAS(ent) (ent)->options |= (1<<13)
#define ZGP_TBL_PROXY_CLR_ASSIGNED_ALIAS(ent) (ent)->options &= ~(1<<13)

#define ZGP_TBL_GET_ASSIGNED_ALIAS(ent)    ( ZGP_TBL_IS_SINK(ent) ? ZGP_TBL_SINK_GET_ASSIGNED_ALIAS(ent) : ZGP_TBL_PROXY_GET_ASSIGNED_ALIAS(ent))

#define ZGP_TBL_GET_INRANGE(ent)    (((ent)->options >> 10) & 1)
#define ZGP_TBL_SET_INRANGE(ent)    (ent)->options |= (1<<10)
#define ZGP_TBL_CLR_INRANGE(ent)    (ent)->options &= ~(1<<10)

#define ZGP_TBL_GET_ACTIVE(ent)     (((ent)->options >> 3) & 1)
#define ZGP_TBL_SET_ACTIVE(ent)     (ent)->options |= (1<<3)
#define ZGP_TBL_CLR_ACTIVE(ent)     (ent)->options &= ~(1<<3)

#define ZGP_TBL_GET_VALID(ent)      (((ent)->options >> 4) & 1)
#define ZGP_TBL_SET_VALID(ent)      (ent)->options |= (1<<4)
#define ZGP_TBL_CLR_VALID(ent)      (ent)->options &= ~(1<<4)

#define ZGP_TBL_GET_FIRST_TO_FORWARD(ent) (((ent)->options >> 9) & 1)
#define ZGP_TBL_SET_FIRST_TO_FORWARD(ent) (ent)->options |= (1<<9)
#define ZGP_TBL_CLR_FIRST_TO_FORWARD(ent) (ent)->options &= ~(1<<9)

#define ZGP_TBL_GET_HAS_ALL_UNICAST_ROUTES(ent) (((ent)->options >> 12) & 1)
#define ZGP_TBL_SET_HAS_ALL_UNICAST_ROUTES(ent) (ent)->options |= (1<<12)
#define ZGP_TBL_CLR_HAS_ALL_UNICAST_ROUTES(ent) (ent)->options &= ~(1<<12)

#define ZGP_TBL_SET_LWUC(ent, val)  (ent)->options = ((ent)->options & (~(1<<6))) | (val<<6)
#define ZGP_TBL_GET_LWUC(ent)       (((ent)->options >> 6) & 1)
#define ZGP_TBL_SET_DGGC(ent, val)  (ent)->options = ((ent)->options & (~(1<<7))) | (val<<7)
#define ZGP_TBL_GET_DGGC(ent)       (((ent)->options >> 7) & 1)
#define ZGP_TBL_SET_CGGC(ent, val)  (ent)->options = ((ent)->options & (~(1<<8))) | (val<<8)
#define ZGP_TBL_GET_CGGC(ent)       (((ent)->options >> 8) & 1)

#define ZGP_PROXY_TABLE_ENTRY_IS_EMPTY(ent)\
  (!(ZGP_TBL_GET_LWUC((ent)) || ZGP_TBL_GET_DGGC((ent)) || ZGP_TBL_GET_CGGC((ent))))

#define ZGP_TBL_GET_RXON_CAP0(ent)   (((ent)->options >> 6) & 1)

/*
Bits: 0..2      3..4                    5                               6               7               8               9
ApplicationID   Communication mode      Sequence number capabilities    RxOnCapability  FixedLocation   AssignedAlias   Security use
*/

#define ZGP_TBL_SINK_FILL_OPTIONS(app_id, comm_mode, sn_cap, rxon_cap, fix_loc, asn_alias, secur_use) \
  (((app_id) & 7) | (((comm_mode) & 3) << 3) | ((!!(sn_cap)) << 5) | ((!!(rxon_cap)) << 6) | ((!!(fix_loc)) << 7) | ((!!(asn_alias)) << 8) | ((!!(secur_use)) << 9))

/*
Bits Parameters
0..2 ApplicationID
   3 EntryActive
   4 EntryValid
   5 Sequence number capabilities
   6 Lightweight Unicast GPS
   7 Derived Group GPS
   8 Commissioned Group GPS
   9 FirstToForward
  10 InRange
  11 GPD Fixed
  12 HasAllUnicastRoutes
  13 AssignedAlias
  14 SecurityUse
  15 Options Extension
*/
#define ZGP_TBL_PROXY_FILL_OPTIONS(app_id, ea, ev, sn_cap, lw, dg, cg, ftf, ir, fix_loc, haur, asn_alias, secur_use, ext) \
  (((app_id) & 7) | ((!!(ea)) << 3) | ((!!(ev)) << 4) | ((!!(sn_cap)) << 5) | ((!!(lw)) << 6) | ((!!(dg)) << 7) |\
   ((!!(cg)) << 8) | ((!!(ftf)) << 9) | ((!!(ir)) << 10) | ((!!(fix_loc)) << 11) | ((!!(haur)) << 12) |\
    ((!!(asn_alias)) << 13) | ((!!(secur_use)) << 14) | ((!!(ext)) << 15) )

/*
Bits: 0-1       2-4             5-7
SecurityLevel   SecurityKeyType Reserved
*/
#define ZGP_TBL_FILL_SEC_OPTIONS(sec_lev, key_type) \
(((sec_lev) & 3) | (((key_type) & 7) << 2))

/********************************************************************/
/******************** ZGP TX Queue definitions **********************/
/********************************************************************/
#ifdef ZB_ENABLE_ZGP_DIRECT
#define USED_MASK(count) ((count)/8 + 1)

#ifndef ZB_ZGP_IMMED_TX
ZB_ASSERT_COMPILE_DECL(ZB_ZGP_TX_QUEUE_SIZE <= ZB_ZGP_TX_PACKET_INFO_COUNT);
#endif  /* ZB_ZGP_IMMED_TX */

#define TX_PACKET_INFO_QUEUE_USED_MASK USED_MASK(ZB_ZGP_TX_PACKET_INFO_COUNT)

#define ZB_ZGPD_CMD_ID_IS_PART_OF_COMMISSIONING_REPLIES(zgpd_cmd_id)      \
  (((zgpd_cmd_id) == ZB_GPDF_CMD_CHANNEL_CONFIGURATION) ||                \
   ((zgpd_cmd_id) == ZB_GPDF_CMD_COMMISSIONING_REPLY))

typedef struct zb_zgp_tx_pinfo_s
{
  zb_uint8_t                    handle;
  zb_zgpd_id_t                  zgpd_id;
  zb_uint8_t                    buf_ref;   /* Reference to buffer, which is used for GPDF */
  zb_uint8_t                    buf_ref_delayed; /* Reference to buffer, which is used for delayed gp_data_ind processing */
} zb_zgp_tx_pinfo_t;

typedef struct zb_zgp_tx_q_ent_s
{
  zb_uint8_t    tx_options;
  zb_uint8_t    cmd_id;
  zb_uint8_t    payload_len;
  zb_uint8_t    pld[ZB_ZGP_TX_CMD_PLD_MAX_SIZE]; /**< Payload */
  zb_bitfield_t is_expired:1;
  zb_bitfield_t sent:1;
  zb_bitfield_t reserved:6;
} zb_zgp_tx_q_ent_t;

typedef struct zb_zgp_tx_q_s
{
  zb_zgp_tx_q_ent_t queue[ZB_ZGP_TX_QUEUE_SIZE];
} zb_zgp_tx_q_t;

typedef struct zb_zgp_tx_packet_info_q_s
{
  zb_zgp_tx_pinfo_t queue[ZB_ZGP_TX_PACKET_INFO_COUNT];
  zb_uint8_t        used_mask[TX_PACKET_INFO_QUEUE_USED_MASK];
} zb_zgp_tx_packet_info_q_t;

enum zb_zgp_tx_packet_info_search_mode_e
{
  ZB_ZGP_TX_PACKET_INFO_ALL_PACKETS,
  ZB_ZGP_TX_PACKET_INFO_PENDING_PACKETS,
#ifdef ZB_ZGP_IMMED_TX
  ZB_ZGP_TX_PACKET_INFO_IMMED_PACKETS,
#endif  /* ZB_ZGP_IMMED_TX */
};

zb_uint8_t zb_zgp_tx_packet_info_q_find_pos(
    zb_zgp_tx_packet_info_q_t *tx_p_i_q,
    zb_zgpd_id_t *id,
    zb_uint8_t search_mode      /* zb_zgp_tx_packet_search_mode_e */
);

zb_uint8_t zb_zgp_tx_q_find_ent_pos_for_send(zb_zgp_tx_packet_info_q_t *tx_p_i_q,
    zb_zgp_tx_q_t *tx_q, zb_zgpd_id_t *id, zb_uint8_t type);

zb_uint8_t zb_zgp_tx_q_find_ent_pos_for_cfm(zb_zgp_tx_packet_info_q_t *tx_p_i_q,
                                            zb_uint8_t                 buf_ref);

zb_uint8_t zb_zgp_tx_q_find_expired_ent_pos(zb_zgp_tx_packet_info_q_t *tx_p_i_q,
    zb_zgp_tx_q_t *tx_q);

zb_uint8_t zb_zgp_tx_packet_info_q_grab_free_ent_pos(zb_zgp_tx_packet_info_q_t *tx_p_i_q,
    zb_uint8_t search_mode);

void zb_zgp_tx_packet_info_q_delete_ent(zb_zgp_tx_packet_info_q_t *tx_p_i_q, zb_uint8_t pos);

/**
 * @brief Delete any unsent commissioning-related entries from the gpTxQueue
 */
void zb_zgp_tx_q_delete_all_comm_ent(zb_zgp_tx_packet_info_q_t *tx_p_i_q, zb_zgp_tx_q_t *tx_q);

zb_bool_t zb_has_zgp_tx_packet_info_q_capacity_to_store(zb_zgp_tx_packet_info_q_t *tx_p_i_q,
    zb_uint8_t search_mode);

#define ZB_ZGP_TX_Q_FILLED_CNT(q, ret_cnt) \
  ret_cnt = zb_calc_non_zero_bits_in_bit_vector((q)->used_mask, USED_MASK_SIZE)

void zgp_clean_zgpd_info_from_queue(zb_uint8_t    buf_ref,
                                    zb_zgpd_id_t *zgpd_id,
                                    zb_uint8_t    handle);

#ifdef ZB_ENABLE_ZGP_TEST_HARNESS
#define ZGP_OUT_GPDF_INFO_GET_OPER_CHANNEL(ci) \
  (((ci) >> 4) + ZB_ZGPD_FIRST_CH)

#define ZGP_OUT_GPDF_INFO_SET_OPER_CHANNEL(ci, op) \
  (ci) = ((ci) & 0x0F) | (((op) - ZB_ZGPD_FIRST_CH) << 4)

#define ZGP_OUT_GPDF_INFO_GET_TEMP_CHANNEL(ci) \
  (((ci) & 0x0F) + ZB_ZGPD_FIRST_CH)

#define ZGP_OUT_GPDF_INFO_SET_TEMP_CHANNEL(ci, tp) \
  (ci) = ((ci) & 0xF0) | ((tp) - ZB_ZGPD_FIRST_CH)

enum zb_outgoing_gpdf_state_e
{
  ZB_OUTGOING_GPDF_STATE_GET_OPER_CHANNEL,
  ZB_OUTGOING_GPDF_STATE_SET_TEMP_CHANNEL,
  ZB_OUTGOING_GPDF_STATE_SET_OPER_CHANNEL
};

enum zb_outgoing_gpdf_status_e
{
  ZB_OUTGOING_GPDF_STATUS_SUCCESS,
  ZB_OUTGOING_GPDF_STATUS_PIB_ERROR,
  ZB_OUTGOING_GPDF_STATUS_ENC_ERROR,
};

typedef void (*zb_outgoing_gpdf_cb_t)(zb_uint8_t param, zb_uint8_t status);

typedef struct zb_outgoing_gpdf_info_s
{
//internal
  zb_uint8_t     state;
  zb_uint8_t     tx_options;
  zb_uint8_t     channel_info;
  zb_uint8_t     channel;
  zb_uint8_t     buf_ref;
//gpdf
  zb_uint8_t     nwk_frame_ctl;
  zb_uint8_t     nwk_ext_frame_ctl;
  zb_zgpd_addr_t addr;
  zb_uint8_t     endpoint;
  zb_uint32_t    sec_frame_counter;
  zb_uint8_t     payload_len;
  zb_uint8_t     payload[ZB_ZGP_MAX_GPDF_CMD_PAYLOAD_APP_ID_0000];
  zb_outgoing_gpdf_cb_t cb;
} zb_outgoing_gpdf_info_t;

#define ZGP_GPDF_NWK_PUT_FCTL(ptr_, fctl_) \
  *(ptr_) = (fctl_); \
  (ptr_)++;

#define ZGP_GPDF_NWK_PUT_EFCTL(ptr_, efctl_) \
  *(ptr_) = (efctl_); \
  (ptr_)++;

#define ZGP_GPDF_NWK_PUT_SRC_ADDR(ptr_, addr_) \
  ZB_HTOLE32((ptr_), (addr_)); \
  (ptr_) += 4;

#define ZGP_GPDF_NWK_PUT_ENDP(ptr_, endp_) \
  *(ptr_) = (endp_); \
  (ptr_)++;

#define ZGP_GPDF_NWK_PUT_SFCNT(ptr_, sfcnt_) \
  ZB_HTOLE32((ptr_), (sfcnt_)); \
  (ptr_) += 4;

zb_ret_t zb_zgp_protect_out_gpdf(zb_uint8_t               buf_ref,
                                 zb_outgoing_gpdf_info_t *gpdf_info,
                                 zb_uint8_t              *key,
                                 zb_uint8_t               nwk_hdr_len);
zb_ret_t zgp_send_gpdf(zb_uint8_t buf_ref, zb_outgoing_gpdf_info_t *gpdf_info);
#endif  /* ZB_ENABLE_ZGP_TEST_HARNESS */
#endif  /* ZB_ENABLE_ZGP_DIRECT */

#ifdef ZB_ENABLE_ZGP_SINK
#ifndef ZB_ZGP_SINK_POSTPONED_DATA_FRAME_ARRAY_SIZE
#define ZB_ZGP_SINK_POSTPONED_DATA_FRAME_ARRAY_SIZE 3
#endif
#endif

/**
 * @brief Information about commissioning process with a ZGPD
 */
typedef struct zb_zgps_dev_comm_data_s
{
  zb_uint8_t     state;         /**< Current commissioning state \ref zb_zgp_comm_state_t */
  zb_zgpd_id_t   zgpd_id;       /**< ZGPD ID */
  zb_uint8_t     oper_channel;  /**< ZGP physical operational channel */
  zb_uint8_t     temp_master_tx_chnl;  /**< Temp channel during commissioning */
#ifdef ZB_ENABLE_ZGP_DIRECT
  /* check if we are working on temp channel 
   * Indicates that we have physically switched from the operational channel */
  zb_bool_t      is_work_on_temp_channel;
#endif  /* ZB_ENABLE_ZGP_DIRECT */
  zb_uint8_t     channel_conf_payload;
  zb_bool_t      channel_conf_sent;
  zb_bool_t      comm_reply_sent;
  zb_uint8_t     result;        /**< Commissioning result @ref zb_zgp_comm_status_t */
  /** Used for duplicate filtering during early stages of commissioning, when device is not
   * added to the sink table yet. Two frame types are filtered this way:
   * - Channel request frames (filtered by MAC sequence number). Also it is impossible to filter
   *   duplicates using sink table if Channel request is sent as Maintenance frame without concrete
   *   ZGPD ID.
   * - Commissioning frames (both secured and unsecured).
   */
  zb_uint32_t    comm_dup_counter;
  /* indicate for dup_counter check function that comm_dup_counter field have legal and valid value */
  zb_uint8_t     any_packet_received;

  /* A.3.3.5.3 The Options as shown in Figure 53 without action field */
  /*
    0-2:  exit mode
      3:  channel present
      4:  unicast communication
  */
  zb_uint8_t     proxy_comm_mode_options;
  zb_uint16_t    sink_addr;

  zb_uint8_t     gpdf_options;
  zb_uint8_t     gpdf_ext_options;
  zb_uint32_t    gpdf_security_frame_counter;

#ifdef ZB_ENABLE_ZGP_SINK
  zb_uint8_t     selected_temp_master_idx;
  zb_zgp_gp_proxy_info_t temp_master_list[ZB_ZGP_MAX_TEMP_MASTER_COUNT];
  zb_uint8_t     need_send_dev_annce;
  /** Postponed data frames array. It is used to put here a buffer id
   *  that contains a data frame for the commissioned GPD device
   *  but the frame was received before we finished the commissioning procedure */
  zb_bufid_t     pdf_array[ZB_ZGP_SINK_POSTPONED_DATA_FRAME_ARRAY_SIZE];
  zb_uint8_t     comm_req_buf;  /**< Reference to buffer with Commissioning GPDF from ZGPD */
  /* Application information to be sent with GP Pairing Configuration */
  zb_gpdf_comm_app_info_t app_info;
#endif  /* ZB_ENABLE_ZGP_SINK */
} zb_zgps_dev_comm_data_t;

/** @brief global ZGP context */
typedef struct zb_zgp_ctx_s
{
  zb_zgp_cluster_t          cluster;
  zb_zgps_dev_comm_data_t   comm_data;  /**< Info about currently ongoing commissioning process */
  zb_uint8_t                mode_change_reason; /**< Reason for mode change @ref zb_zgp_mode_change_reason_t */

  /* b0 - if 1 - start comm/stop comm by gp_sink_commissioning_mode request
   * b1 - if 1 - unicast proxy commissioning mode, otherwise - broadcast mode
   * b7-b15 - gp_sink_commissioning_mode options */
  zb_uint16_t               sink_comm_mode_opt;

#ifdef ZB_ENABLE_ZGP_SINK
  zb_uint8_t                sink_mode; /**< Current mode of Sink side. One of the @ref zb_zgp_mode_t */
  const zb_zgps_mapping_entry_t **app_mapping_table; /**< Pointer to mapping table array */
  zb_uint16_t *app_mapping_table_size; /**< Pointer to mapping table array length */
#ifdef ZB_ZGP_SINK_SUPPORT_LEGACY_MATCH_INFO
  /** Functionality matching information. Should be set by user application.
    * It is used during commissioning to fill mapping table entries based
    * on ZGPD Device ID received in Commissioning command. */
  const zb_zgps_match_info_t *match_info;
#endif  /* ZB_ZGP_SINK_SUPPORT_LEGACY_MATCH_INFO */
  /* Since commissioning doesn't necessarily end after one pairing,
   * pairing endpoint needs to be placed outside of comm_data
   * because comm_data is cleared after each commissioned device.
   */
  zb_uint8_t     pairing_endpoint;            /**< Endpoint for current commissioniing process */
  zb_zgp_comm_req_cb_t      comm_req_cb;   /**< Commissioning callback when ZGPD sent commissioning frame */
  zb_zgp_app_cfm_cb_t       app_cfm_cb;    /**< Application callback
                                             * when ZGPD sent application data */
  zb_zgp_app_comm_ind_cb_t  app_comm_op_cb;    /**< Application callback when receive commissioning in operational mode */

  zgp_approve_comm_params_t app_comm_params;    /**< Commissioning (or Pairing Configuration) params for Application approval */
  zb_bufid_t                pairing_conf_buf;   /**< Reference to buffer with GP Pairing Configuration command */

  /* new Sink table, unified with Proxy table */
  zb_zgp_sink_table_t       sink_table;

  #define ZB_ZGP_APP_TBL_SIZE ZB_ZGP_SINK_TBL_SIZE
  zgp_runtime_app_tbl_ent_t app_table[ZB_ZGP_APP_TBL_SIZE];
#endif  /* ZB_ENABLE_ZGP_SINK */
#ifdef ZB_ENABLE_ZGP_PROXY
  zb_uint8_t                proxy_mode; /**< Current mode of Proxy side. One of the @ref
                                         * zb_zgp_mode_t */
  zb_zgp_proxy_table_t      proxy_table;
#endif  /* ZB_ENABLE_ZGP_PROXY */

#ifdef ZB_ENABLE_ZGP_DIRECT
  zb_zgp_tx_q_t             tx_queue;     /**< ZGP Tx queue */
  zb_zgp_tx_packet_info_q_t tx_packet_info_queue;
#ifdef ZB_ZGP_IMMED_TX
  zb_uint32_t               immed_tx_frame_counter;
#endif  /* ZB_ZGP_IMMED_TX */
#ifdef ZB_ENABLE_ZGP_TEST_HARNESS
  zb_outgoing_gpdf_info_t   out_gpdf_info;
#endif
#endif  /* ZB_ENABLE_ZGP_DIRECT */

  zgp_runtime_app_description_ctx_t app_descr_ctx;

  zb_bitfield_t skip_gpdf:1;
  zb_bitfield_t init_by_scheduler:1;
  zb_bitfield_t gp_disabled:1;
  /* ZGP InvolveTC (A.3.3.2.6 gpsSecurityLevel attribute)

    According to the current version of the specification, sinks joining a distributed Zigbee network
    or joining using the default Trust Centre Link Key SHALL set this bit to 0b0.
    Sinks joining the Zigbee network using IC-based unique link key SHALL set this bit to 0b1

    We use this bitfield just to remember the join type until the authorized_signal will be generated.

    The procedure is:

    1. Joiner receives Transport Key in zb_aps_in_transport_key()
       and we see what key type is for the provisioning key.
       It calls zb_zgp_notification_network_join_begins() with the flag indicating whether the key was IC based or not.
    2. Joiner finalize tclk updation by zdo_secur_update_tclk_done() and calls zb_zgp_notification_network_join_done()
  */
  zb_bitfield_t ic_based_join_type:1;
  zb_bitfield_t aligned:4;

} zb_zgp_ctx_t;

#ifdef ZB_CERTIFICATION_HACKS
typedef struct zgp_cert_hacks_s
{
#ifdef ZB_ENABLE_ZGP_PROXY
  zb_bitfield_t gp_proxy_ignore_duplicate_gp_frames:1; /*!< If set to 1, disable proxy drop duplicate gp frames */
  zb_bitfield_t gp_proxy_replace_gp_notif_sec_level:1; /*!< If set to 1, proxy replaces security level
                                                        *   ONLY in options of the GP Notification packet */
  zb_uint8_t    gp_proxy_replace_sec_level;            /*!< data to set in replace mode*/
  zb_bitfield_t gp_proxy_replace_gp_notif_sec_key_type:1; /*!< If set to 1, proxy replaces security key type
                                                        *   ONLY in options of the GP Notification packet */
  zb_uint8_t    gp_proxy_replace_sec_key_type;         /*!< data to set in replace mode*/
  zb_bitfield_t gp_proxy_replace_gp_notif_sec_frame_counter:1; /*!< If set to 1, proxy replaces security frame counter */
  zb_uint32_t   gp_proxy_replace_sec_frame_counter;    /*!< data to set in replace mode*/
  zb_callback_t gp_proxy_gp_notif_req_cb;              /*!< Called before proxy send gp notification */
  zb_callback_t gp_proxy_gp_comm_notif_req_cb;         /*!< Called before proxy send gp commissioning notification */
  zb_bitfield_t gp_proxy_replace_comm_app_id:1;        /*!< If set to 1, proxy replaces app_id in commissioning frame */
  zb_bitfield_t gp_proxy_replace_comm_app_id_format:1; /*!< If set to 1, proxy replaces app_id in commissioning frame and its format */
  zb_uint8_t    gp_proxy_replace_comm_app_id_value;    /*!< data to set in replace mode*/
  zb_bitfield_t gp_proxy_replace_comm_options:1;       /*!< If set to 1, proxy replaces app_id in commissioning frame */
  zb_uint16_t   gp_proxy_replace_comm_options_value;   /*!< data to set in replace mode*/
  zb_uint16_t   gp_proxy_replace_comm_options_mask;    /*!< mask to set in replace mode*/

  zb_bitfield_t gp_proxy_replace_comm_gpd_id:1;        /*!< If set to 1, proxy replaces gpd_id in commissioning frame */
  zb_uint32_t   gp_proxy_replace_comm_gpd_id_value;    /*!< data to set in replace mode*/
  zb_ieee_addr_t gp_proxy_replace_comm_gpd_ieee_value;    /*!< data to set in replace mode*/
  zb_uint8_t    gp_proxy_replace_comm_gpd_ep_value;    /*!< data to set in replace mode*/
#endif  /* ZB_ENABLE_ZGP_PROXY */
#if defined ZB_ENABLE_ZGP_SINK || defined ZGP_COMMISSIONING_TOOL
  zb_bitfield_t gp_sink_use_assigned_alias_for_dgroup_commissioning:1; /*!< If set to 1, Sink will use assignead alias
                                                                        *  insead derived alias for
                                                                        *  next commissioning process */
  zb_bitfield_t gp_sink_replace_sec_lvl_on_pairing:1;  //replace sec_lvl on pairing
  zb_bitfield_t gp_sink_sec_lvl_on_pairing:2;
  zb_uint16_t   gp_sink_assigned_alias;
  zb_uint16_t   gp_sink_pairing_dest;                  /*!< destination of GP pairing, by default is
                                                        * 0xFFFD */
#endif  /* defined ZB_ENABLE_ZGP_SINK || defined ZGP_COMMISSIONING_TOOL */
} zgp_cert_hacks_t;

zgp_cert_hacks_t* zb_zgp_cert_hacks_get(void);
extern zgp_cert_hacks_t zb_zgp_cert_hacks;
#define ZGP_CERT_HACKS() zb_zgp_cert_hacks

#endif  /* ZB_CERTIFICATION_HACKS */

/**
   Direct access to GP context.

   To be used internally only.
 */
zb_zgp_ctx_t *zb_zgp_ctx_get(void);
extern zb_zgp_ctx_t zb_zgp_ctx;
#define ZGP_CTXC() zb_zgp_ctx

#ifdef ZB_ENABLE_ZGP_DIRECT
/**
 * Start of buffer contains zb_gp_data_req_t.
 *
 * @pre (req->tx_options & ZB_GP_DATA_REQ_USE_GP_TX_QUEUE) ==
 *      (req->tx_q_ent_lifetime != ZB_GP_TX_QUEUE_ENTRY_LIFETIME_NONE)
 */
void zb_gp_data_req(zb_uint8_t param);

void zb_zgp_tx_q_entry_expired(zb_uint8_t param);
void schedule_gp_txdata_req(zb_uint8_t param);

void zb_gp_data_cfm(zb_uint8_t param);
#endif  /* ZB_ENABLE_ZGP_DIRECT */

/**
 * Return back operational channel
 *
 * Operational channel can be changed to send channel configuration
 * during commissioning. This function sets operational channel back.
 * If channel change is necessary, then request to MAC is initiated
 * and zgp_mlme_set_cfm_cb will be called after that. Otherwise,
 * zgp_mlme_set_cfm_cb is called immediately
 *
 * @param  param   reference to buffer
 */
void zgp_back_to_oper_channel(zb_uint8_t param);

#define ZB_ZGP_IS_COMM_STATE(checked_state) \
  (ZGP_CTXC().comm_data.state == (checked_state))

#define ZB_ZGP_SET_COMM_STATE(new_state) \
{ \
  ZGP_CTXC().comm_data.state = (new_state); \
}

/* Read commissioning options (in sink) */
#define ZB_ZGPS_COMM_GET_RX_AFTER_TX()         ZB_GPDF_EXT_NFC_GET_RX_AFTER_TX(ZGP_CTXC().comm_data.gpdf_nwk_ext_frame_ctl)
#define ZB_ZGPS_COMM_GET_SEC_KEY_REQ()         ZB_GPDF_COMM_OPT_SEC_KEY_REQ(ZGP_CTXC().comm_data.gpdf_options)
#define ZB_ZGPS_COMM_GET_PAN_ID_REQ()          ZB_GPDF_COMM_OPT_PAN_ID_REQ(ZGP_CTXC().comm_data.gpdf_options)
#define ZB_ZGPS_COMM_GET_ZGPD_KEY_PRESENT()    ZB_GPDF_COMM_OPT_ZGPD_KEY_PRESENT(ZGP_CTXC().comm_data.gpdf_ext_options)
#define ZB_ZGPS_COMM_GET_ZGPD_KEY_ENCRYPTED()  ZB_GPDF_COMM_OPT_ZGPD_KEY_ENCRYPTED(ZGP_CTXC().comm_data.gpdf_ext_options)
#define ZB_ZGPS_COMM_GET_SEC_KEY_TYPE()        ZB_GPDF_COMM_OPT_SEC_KEY_TYPE(ZGP_CTXC().comm_data.gpdf_ext_options)

/* Hash function for GP keys derivation A.3.7.1.2.1, A.3.7.1.2.2 */
void zb_zgp_key_hash(zb_uint8_t *key, zb_uint8_t *input, zb_uint8_t input_len, zb_uint8_t *hash_key);

/* GP security key generator with given key_type, see A.3.7.1.2. Table 48 values of gpSecurityKeyType */
zb_ret_t zb_zgp_key_gen(enum zb_zgp_security_key_type_e security_key_type, zb_zgpd_id_t *zgpd_id, zb_uint8_t *oob, zb_uint8_t *key);
#endif  /* !ZB_ZGPD_ROLE */

zb_ret_t zb_zgp_protect_frame(
    zb_gpdf_info_t *gpdf_info,
    zb_uint8_t *key,
    zb_bufid_t packet);

zb_ret_t zb_zgp_decrypt_and_auth(zb_uint8_t param);

void zb_zgp_protect_gpd_key(
    zb_bool_t from_gpd,
    zb_zgpd_id_t *zgpd_id,
    zb_uint8_t *key_to_encrypt,
    zb_uint8_t *key_encrypt_with,
    zb_uint8_t *crypted_key,
    zb_uint32_t security_frame_counter,
    zb_uint8_t *mic);

zb_ret_t zb_zgp_decrypt_n_auth_gpd_key(
    zb_bool_t from_gpd,
    zb_zgpd_id_t *zgpd_id,
    zb_uint8_t *key_decrypt_with,
    zb_uint8_t *crypted_key,
    zb_uint32_t security_frame_counter,
    zb_uint8_t *mic,
    zb_uint8_t *plain_key);

#ifndef ZB_ZGPD_ROLE
void zgp_channel_config_transceiver_channel_change(zb_uint8_t param,
                                                   zb_bool_t  back);
void zb_zgp_channel_config_get_current_channel(zb_uint8_t param);
/**
 * @brief Prepare channel configuration packet and add it into TX queue
 *
 * @param param      [in]  Buffer reference
 *
 */
void zgp_channel_config_add_to_queue(zb_uint8_t param, zb_uint8_t payload);

void zgp_tbl_init(void);
void zgp_tbl_clear(void);
#ifdef ZB_ENABLE_ZGP_PROXY
zb_ret_t zgp_proxy_table_write(zb_zgpd_id_t *zgpd_id, zgp_tbl_ent_t *ent);
zb_ret_t zgp_proxy_table_read(zb_zgpd_id_t *zgpd_id, zgp_tbl_ent_t *ent);
zb_ret_t zgp_proxy_table_read_by_idx(zb_uint_t idx, zgp_tbl_ent_t *ent);
zb_ret_t zgp_proxy_table_idx(zb_zgpd_id_t *zgpd_id, zb_uint_t *idx);
zb_ret_t zgp_proxy_table_del(zb_zgpd_id_t *zgpd_id);
zb_uint32_t zgp_proxy_table_get_security_counter(zb_zgpd_id_t *zgpd_id);
zb_uint32_t zgp_proxy_table_get_dup_counter(zb_zgpd_id_t *zgpd_id);
zb_ret_t zgp_proxy_table_restore_security_counter(zb_zgpd_id_t *zgpd_id);
zb_ret_t zgp_proxy_table_set_security_counter(zb_zgpd_id_t *zgpd_id, zb_uint32_t counter);
zb_uint8_t zgp_proxy_table_get_search_counter(zb_zgpd_id_t *zgpd_id);
zb_ret_t zgp_proxy_table_set_search_counter(zb_zgpd_id_t *zgpd_id, zb_uint8_t counter);
void zgp_proxy_table_get_lqi_rssi(zb_zgpd_id_t *zgpd_id, zb_uint8_t *lqi_p, zb_int8_t *rssi_p);
void zgp_proxy_table_set_lqi_rssi(zb_zgpd_id_t *zgpd_id, zb_uint8_t lqi, zb_int8_t rssi);
zb_bool_t zb_zgp_is_proxy_table_empty(void);
zb_uint8_t zb_zgp_proxy_table_non_empty_entries_count(void);

/**
 * @brief Search table entry by index in nonempty entries list
 *
 * @param index   [in]   Index of table entry which needed
 * @param ent     [out]  Pointer to allocated memory space for table entry
 *
 */
zb_bool_t zb_zgp_proxy_table_get_entry_by_non_empty_list_index(zb_uint8_t index, zgp_tbl_ent_t *ent);
zb_uint8_t zb_zgp_proxy_table_entry_get_search_counter(zgp_tbl_ent_t *ent);
zb_ret_t zb_zgp_proxy_table_entry_set_search_counter(zgp_tbl_ent_t *ent, zb_uint8_t counter);

/**
 * @brief Get runtime bit field from runtime options
 *
 * @param ent     [in]  Pointer to table entry
 * @param field   [in]  bit field index
 *
 * @return bit field current value
 */
zb_uint8_t zb_zgp_proxy_table_entry_get_runtime_field(zgp_tbl_ent_t *ent, zb_uint8_t field);

/**
 * @brief Set runtime bit field in runtime options
 *
 * @param ent     [in]  Pointer to table entry
 * @param field   [in]  bit field index
 *
 */
void zb_zgp_proxy_table_entry_set_runtime_field(zgp_tbl_ent_t *ent, zb_uint8_t field);

/**
 * @brief Reset runtime bit field current value in runtime options
 *
 * @param ent     [in]  Pointer to table entry
 * @param field   [in]  bit field index
 *
 */
void zb_zgp_proxy_table_entry_clr_runtime_field(zgp_tbl_ent_t *ent, zb_uint8_t field);

#define ZGP_TBL_RUNTIME_GET_VALID(ent) zb_zgp_proxy_table_entry_get_runtime_field(ent, 0)
#define ZGP_TBL_RUNTIME_SET_VALID(ent) zb_zgp_proxy_table_entry_set_runtime_field(ent, 0)
#define ZGP_TBL_RUNTIME_CLR_VALID(ent) zb_zgp_proxy_table_entry_clr_runtime_field(ent, 0)

#define ZGP_TBL_RUNTIME_GET_FIRST_TO_FORWARD(ent) zb_zgp_proxy_table_entry_get_runtime_field(ent, 1)
#define ZGP_TBL_RUNTIME_SET_FIRST_TO_FORWARD(ent) zb_zgp_proxy_table_entry_set_runtime_field(ent, 1)
#define ZGP_TBL_RUNTIME_CLR_FIRST_TO_FORWARD(ent) zb_zgp_proxy_table_entry_clr_runtime_field(ent, 1)

#define ZGP_TBL_RUNTIME_FIRST_TO_FORWARD_UPDATE(ent, value)\
  {\
    if ((value))\
      ZGP_TBL_RUNTIME_SET_FIRST_TO_FORWARD((ent));\
    else\
      ZGP_TBL_RUNTIME_CLR_FIRST_TO_FORWARD((ent));\
  }

#define ZGP_TBL_RUNTIME_GET_HAS_ALL_UNICAST_ROUTES(ent) zb_zgp_proxy_table_entry_get_runtime_field(ent, 2)
#define ZGP_TBL_RUNTIME_SET_HAS_ALL_UNICAST_ROUTES(ent) zb_zgp_proxy_table_entry_set_runtime_field(ent, 2)
#define ZGP_TBL_RUNTIME_CLR_HAS_ALL_UNICAST_ROUTES(ent) zb_zgp_proxy_table_entry_clr_runtime_field(ent, 2)

#define ZGP_TBL_GET_SEARCH_COUNTER(ent) zb_zgp_proxy_table_entry_get_search_counter(ent)
#define ZGP_TBL_SET_SEARCH_COUNTER(ent, counter) zb_zgp_proxy_table_entry_set_search_counter(ent, counter)
#endif  /* ZB_ENABLE_ZGP_PROXY */
#ifdef ZB_ENABLE_ZGP_SINK
zb_ret_t zgp_sink_table_write(zb_zgpd_id_t *zgpd_id, zgp_tbl_ent_t *ent);
zb_ret_t zgp_sink_table_read(zb_zgpd_id_t *zgpd_id, zgp_tbl_ent_t *ent);
zb_ret_t zgp_sink_table_read_by_idx(zb_uint_t idx, zgp_tbl_ent_t *ent);
zb_ret_t zgp_sink_table_idx(zb_zgpd_id_t *zgpd_id, zb_uint_t *idx);
zb_ret_t zgp_sink_table_del(zb_zgpd_id_t *zgpd_id);
zb_uint32_t zgp_sink_table_get_security_counter(zb_zgpd_id_t *zgpd_id);
zb_uint32_t zgp_sink_table_get_dup_counter(zb_zgpd_id_t *zgpd_id);
zb_ret_t zgp_sink_table_restore_security_counter(zb_zgpd_id_t *zgpd_id);
zb_ret_t zgp_sink_table_set_security_counter(zb_zgpd_id_t *zgpd_id, zb_uint32_t counter);
void zgp_sink_get_lqi_rssi(zb_zgpd_id_t *zgpd_id, zb_uint8_t *lqi_p, zb_int8_t *rssi_p);
void zgp_sink_set_lqi_rssi(zb_zgpd_id_t *zgpd_id, zb_uint8_t lqi, zb_int8_t rssi);
zb_bool_t zb_zgp_is_sink_table_empty(void);
zb_uint8_t zb_zgp_sink_table_non_empty_entries_count(void);
zb_bool_t zb_zgp_sink_table_get_entry_by_non_empty_list_index(zb_uint8_t index, zgp_tbl_ent_t *ent);
zb_bool_t zb_zgps_get_dev_matching_tbl_index(zb_uint8_t zgpd_dev_id, zb_uint16_t manuf_id,
                                             zb_uint16_t model_id, zb_zgpd_id_t *zgpd_id,
                                             zb_uint8_t *idx);
void zb_zgps_app_descr_timeout(zb_uint8_t param);

#define ZGP_CACHED_SINK_ENTRY() ZGP_CTXC().sink_table.base.cached

#define ZGP_INVALID_MATCH_DEV_TABLE_IDX 0xFF

zgp_runtime_app_tbl_ent_t *zb_zgp_alloc_app_tbl_ent_by_id(zb_zgpd_id_t *zgpd_id_p);
zgp_runtime_app_tbl_ent_t *zb_zgp_alloc_app_tbl_ent_with_switch_info_by_id(zb_zgpd_id_t *zgpd_id_p, zb_gpdf_comm_switch_info_t switch_info);
zgp_runtime_app_tbl_ent_t *zb_zgp_get_app_tbl_ent_by_alias(zb_uint16_t addr);
zgp_runtime_app_tbl_ent_t *zb_zgp_get_app_tbl_ent_by_id(zb_zgpd_id_t *zgpd_id_p);
void zb_zgp_erase_app_table_ent_by_id(zb_zgpd_id_t *zgpd_id_p);
void zb_zgp_erase_app_tbl_ent(zgp_runtime_app_tbl_ent_t *ent);
zgp_report_desc_t *zb_zgp_get_report_desc_from_app_tbl(zb_zgpd_id_t *zgpd_id_p, zb_uint8_t report_idx);
zb_uint32_t zb_zgp_app_desc_receive_reports_count(const zgp_runtime_app_tbl_ent_t *ent);

void zb_zgp_handle_app_descr_init_values(zb_zgpd_id_t *zgpd_id_p);

#endif  /* ZB_ENABLE_ZGP_SINK */

#define ZGP_IS_SINK_IN_OPERATIONAL_MODE() (ZGP_CTXC().sink_mode == ZB_ZGP_OPERATIONAL_MODE)
#define ZGP_IS_SINK_IN_COMMISSIONING_MODE() (ZGP_CTXC().sink_mode == ZB_ZGP_COMMISSIONING_MODE)
#define ZGP_IS_PROXY_IN_OPERATIONAL_MODE() (ZGP_CTXC().proxy_mode == ZB_ZGP_OPERATIONAL_MODE)
#define ZGP_IS_PROXY_IN_COMMISSIONING_MODE() (ZGP_CTXC().proxy_mode == ZB_ZGP_COMMISSIONING_MODE)

void zb_gp_data_indication(zb_uint8_t param);

/********************************************************************/
/******************* ZGP CLUSTER definitions ************************/
/********************************************************************/

#define ZB_GP_VER 0

enum zgp_server_commands_e {
  ZGP_SERVER_CMD_GP_NOTIFICATION                        = 0x00,
  ZGP_SERVER_CMD_GP_PAIRING_SEARCH                      = 0x01,
  ZGP_SERVER_CMD_GP_TUNNELING_STOP                      = 0x03,
  ZGP_SERVER_CMD_GP_COMMISSIONING_NOTIFICATION          = 0x04,
  ZGP_SERVER_CMD_GP_SINK_COMMISSIONING_MODE             = 0x05,
  ZGP_SERVER_CMD_GP_TRANSLATION_TABLE_UPDATE_COMMAND    = 0x07,
  ZGP_SERVER_CMD_GP_TRANSLATION_TABLE_REQUEST           = 0x08,
  ZGP_SERVER_CMD_GP_PAIRING_CONFIGURATION               = 0x09,
  ZGP_SERVER_CMD_GP_SINK_TABLE_REQUEST                  = 0x0a,
  ZGP_SERVER_CMD_GP_PROXY_TABLE_RESPONSE                = 0x0b
};


enum zgp_client_commands_e {
  ZGP_CLIENT_CMD_GP_NOTIFICATION_RESPONSE     = 0x00, /* not for basic */
  ZGP_CLIENT_CMD_GP_PAIRING                   = 0x01,
  ZGP_CLIENT_CMD_GP_PROXY_COMMISSIONING_MODE  = 0x02,
  ZGP_CLIENT_CMD_GP_RESPONSE                  = 0x06,
  ZGP_CLIENT_CMD_GP_SINK_TABLE_RESPONSE       = 0x0a,
  ZGP_CLIENT_CMD_GP_PROXY_TABLE_REQUEST       = 0x0b
};


/*
gppFunctionality

b0	GP feature	0b1
b1	Direct communication (reception of GPDF via GP stub) 	0b1
b2	Derived groupcast communication 	0b1
b3	Pre-commissioned groupcast communication 	0b1
b4	Full unicast communication	0b0
b5	Lightweight unicast communication	0b1
b6	Reserved	0b0
b7	Bidirectional operation	0b0
b8	Proxy Table maintenance (active and passive, for GPD mobility and GPP robustness)	0b0
b9	Reserved	0b0
b10	GP commissioning	0b1
b11	CT-based commissioning	0b1
b12	Maintenance of GPD (deliver channel/key during operation)	0b0
b13	gpdSecurityLevel = 0b00	0b1
b14	Deprecated: gpdSecurityLevel = 0b01 	0b0
b15	gpdSecurityLevel = 0b10	0b1
b16	gpdSecurityLevel = 0b11	0b1
b17	Reserved	0b0
b18	Reserved	0b0
b19	GPD IEEE address	0b1

So, for GPPB we have 10011010110000101111
 */

#define ZGP_GPPB_FUNCTIONALITY 0x9ac2f

/* ZGP spec, A.3.4.2.7 */

typedef enum zgp_gpp_functionality_e {
  ZGP_GPP_GP_FEATURE                               = (1 <<  0),
  ZGP_GPP_DIRECT_COMMUNICATION                     = (1 <<  1),
  ZGP_GPP_DERIVED_GROUPCAST_COMMUNICATION          = (1 <<  2),
  ZGP_GPP_PRECOMMISSIONED_GROUPCAST_COMMUNICATION  = (1 <<  3),
  ZGP_GPP_FULL_UNICAST_COMMUNICATION               = (1 <<  4),
  ZGP_GPP_LIGHTWEIGHT_UNICAST_COMMUNICATION        = (1 <<  5),
  ZGP_GPP_RESERVED_B6                              = (1 <<  6),
  ZGP_GPP_BIDIRECTIONAL_OPERATION                  = (1 <<  7),
  ZGP_GPP_PROXY_TABLE_MAINTENANCE                  = (1 <<  8),
  ZGP_GPP_RESERVED_B9                              = (1 <<  9),
  ZGP_GPP_GP_COMMISSIONING                         = (1 << 10),
  ZGP_GPP_CT_BASED_COMMISSIONING                   = (1 << 11),
  ZGP_GPP_MAINTENANCE_OF_GPD                       = (1 << 12),
  ZGP_GPP_SEC_LEVEL_NO_SECURITY                    = (1 << 13),
  ZGP_GPP_SEC_LEVEL_REDUCED                        = (1 << 14),
  ZGP_GPP_SEC_LEVEL_FULL_NO_ENC                    = (1 << 15),
  ZGP_GPP_SEC_LEVEL_FULL_WITH_ENC                  = (1 << 16),
  ZGP_GPP_RESERVED_B17                             = (1 << 17),
  ZGP_GPP_RESERVED_B18                             = (1 << 18),
  ZGP_GPP_GPD_IEEE_ADDRESS                         = (1 << 19)
} zgp_gpp_functionality_t;

/*
gpsFunctionality

b0	GP feature	0b1
b1	Direct communication (reception of GPDF via GP stub) 	device-specific
b2	Derived groupcast communication 	device-specific
b3	Pre-commissioned groupcast communication 	device-specific
b4	Full unicast communication	0b0
b5	Lightweight unicast communication	device-specific
b6	Proximity bidirectional operation	0b0
b7	Multi-hop bidirectional operation	0b0
b8	Proxy Table maintenance (active and passive, for GPD mobility and proxy robustness)	0b0
b9	Proximity commissioning (unidirectional and bidirectional)	device-specific
b10	Multi-hop commissioning (unidirectional and bidirectional)	0b1
b11	CT-based commissioning	0b1
b12	Maintenance of GPD (deliver channel/key during operation)	0b0
b13	gpdSecurityLevel = 0b00  in operation	device-specific
b14	Deprecated: gpdSecurityLevel = 0b01 	0b0
b15	gpdSecurityLevel = 0b10	0b1
b16	gpdSecurityLevel = 0b11	0b1
b17	Sink Table-based groupcast forwarding	0b0
b18	Translation Table	device-specific
b19	GPD IEEE address	0b1

For Basic Sink in our case:
- no Translation table (0)
- no gpdSecurityLevel = 0b00  in operation 0
- yes Proximity commissioning - 1
- yes Lightweight unicast communication - 1
- yes Pre-commissioned groupcast communication - 1
- yes Derived groupcast communication - 1
- yes Direct communication (reception of GPDF via GP stub) - 1

10011000111000101111

 */

#define ZGP_GPSB_FUNCTIONALITY 0x98e2f

/* ZGP spec, A.3.3.2.7 */

typedef enum zgp_gps_functionality_e {
  ZGP_GPS_GP_FEATURE                               = (1 <<  0),
  ZGP_GPS_DIRECT_COMMUNICATION                     = (1 <<  1),
  ZGP_GPS_DERIVED_GROUPCAST_COMMUNICATION          = (1 <<  2),
  ZGP_GPS_PRECOMMISSIONED_GROUPCAST_COMMUNICATION  = (1 <<  3),
  ZGP_GPS_FULL_UNICAST_COMMUNICATION               = (1 <<  4),
  ZGP_GPS_LIGHTWEIGHT_UNICAST_COMMUNICATION        = (1 <<  5),
  ZGP_GPS_PROXIMITY_BIDIRECTIONAL_OPERATION        = (1 <<  6),
  ZGP_GPS_MULTIHOP_BIDIRECTIONAL_OPERATION         = (1 <<  7),
  ZGP_GPS_PROXY_TABLE_MAINTENANCE                  = (1 <<  8),
  ZGP_GPS_PROXIMITY_COMMISSIONING                  = (1 <<  9),
  ZGP_GPS_MULTIHOP_COMMISSIONING                   = (1 << 10),
  ZGP_GPS_CT_BASED_COMMISSIONING                   = (1 << 11),
  ZGP_GPS_MAINTENANCE_OF_GPD                       = (1 << 12),
  ZGP_GPS_SEC_LEVEL_NO_SECURITY                    = (1 << 13),
  ZGP_GPS_SEC_LEVEL_REDUCED                        = (1 << 14),
  ZGP_GPS_SEC_LEVEL_FULL_NO_ENC                    = (1 << 15),
  ZGP_GPS_SEC_LEVEL_FULL_WITH_ENC                  = (1 << 16),
  ZGP_GPS_SINK_TABLE_BASED_GROUPCAST_FORWARDING    = (1 << 17),
  ZGP_GPS_TRANSLATION_TABLE                        = (1 << 18),
  ZGP_GPS_GPD_IEEE_ADDRESS                         = (1 << 19)
} zgp_gps_functionality_t;

enum zgp_gp_attr_e {
  ZB_ZCL_ATTR_GPS_MAX_SINK_TABLE_ENTRIES_ID     = 0x0000,
  ZB_ZCL_ATTR_GPS_SINK_TABLE_ID                 = 0x0001,
  ZB_ZCL_ATTR_GPS_COMMUNICATION_MODE_ID         = 0x0002,
  ZB_ZCL_ATTR_GPS_COMMISSIONING_EXIT_MODE_ID    = 0x0003,
  ZB_ZCL_ATTR_GPS_COMMISSIONING_WINDOW_ID       = 0x0004,
  ZB_ZCL_ATTR_GPS_SECURITY_LEVEL_ID             = 0x0005,
  ZB_ZCL_ATTR_GPS_FUNCTIONALITY_ID              = 0x0006,
  ZB_ZCL_ATTR_GPS_ACTIVE_FUNCTIONALITY_ID       = 0x0007,
  ZB_ZCL_ATTR_GPP_MAX_PROXY_TABLE_ENTRIES_ID    = 0x0010,
  ZB_ZCL_ATTR_GPP_PROXY_TABLE_ID                = 0x0011,
  ZB_ZCL_ATTR_GPP_NOTIFICATION_RETRY_NUMBER_ID  = 0x0012,
  ZB_ZCL_ATTR_GPP_NOTIFICATION_RETRY_TIMER_ID   = 0x0013,
  ZB_ZCL_ATTR_GPP_MAX_SEARCH_COUNTER_ID         = 0x0014,
  ZB_ZCL_ATTR_GPP_BLOCKED_GPDID_ID              = 0x0015,
  ZB_ZCL_ATTR_GPP_FUNCTIONALITY_ID              = 0x0016,
  ZB_ZCL_ATTR_GPP_ACTIVE_FUNCTIONALITY_ID       = 0x0017,
  ZB_ZCL_ATTR_GP_SHARED_SECURITY_KEY_TYPE_ID    = 0x0020,
  ZB_ZCL_ATTR_GP_SHARED_SECURITY_KEY_ID         = 0x0021,
  ZB_ZCL_ATTR_GP_LINK_KEY_ID                    = 0x0022
};

#define ZB_ZCL_GREEN_POWER_CLUSTER_REVISION_DEFAULT ((zb_uint16_t)0x0002u)

#define ZB_ZCL_GREEN_POWER_CLUSTER_REVISION_MAX ZB_ZCL_GREEN_POWER_CLUSTER_REVISION_DEFAULT

/* For some reason ZBOSS uses that ugly ZB_SET_ATTR_DESCR_WITH_xxx
 * definitions. Let's use simpler syntax! */

#define ZB_ZCL_SET_ATTR_DESC_M(id, data_ptr, type, attr)                 \
  {                                                                      \
    (id), (type), (attr), ZB_ZCL_NON_MANUFACTURER_SPECIFIC, (data_ptr)   \
  },

#define ZB_ZCL_DECLARE_GPT_ATTRIB_LIST_CLI(attr_list,                   \
                                gp_shared_security_key_type,            \
                                gp_shared_security_key,                 \
                                gp_link_key                             \
  )                                                                     \
  ZB_ZCL_START_DECLARE_ATTRIB_LIST_CLUSTER_REVISION(attr_list, ZB_ZCL_GREEN_POWER) \
  ZB_ZCL_SET_ATTR_DESC_M(ZB_ZCL_ATTR_GP_SHARED_SECURITY_KEY_TYPE_ID, (gp_shared_security_key_type), ZB_ZCL_ATTR_TYPE_8BITMAP, ZB_ZCL_ATTR_ACCESS_READ_WRITE) \
  ZB_ZCL_SET_ATTR_DESC_M(ZB_ZCL_ATTR_GP_SHARED_SECURITY_KEY_ID, (gp_shared_security_key), ZB_ZCL_ATTR_TYPE_128_BIT_KEY, ZB_ZCL_ATTR_ACCESS_READ_WRITE) \
  ZB_ZCL_SET_ATTR_DESC_M(ZB_ZCL_ATTR_GP_LINK_KEY_ID, (gp_link_key), ZB_ZCL_ATTR_TYPE_128_BIT_KEY, ZB_ZCL_ATTR_ACCESS_READ_WRITE)       \
  ZB_ZCL_FINISH_DECLARE_ATTRIB_LIST

#define ZB_ZCL_DECLARE_GPT_ATTRIB_LIST_SRV(attr_list,                   \
                                gp_shared_security_key_type,            \
                                gp_shared_security_key,                 \
                                gp_link_key,                            \
                                gps_max_sink_table_entries,             \
                                gps_communication_mode,                 \
                                gps_commissioning_exit_mode,            \
                                gps_security_level,                     \
                                gps_functionality,                      \
                                gps_active_functionality,               \
                                gps_commissioning_window,               \
                                gps_sink_table_ptr                      \
  )                                                                     \
  ZB_ZCL_START_DECLARE_ATTRIB_LIST_CLUSTER_REVISION(attr_list, ZB_ZCL_GREEN_POWER) \
  ZB_ZCL_SET_ATTR_DESC_M(ZB_ZCL_ATTR_GP_SHARED_SECURITY_KEY_TYPE_ID, (gp_shared_security_key_type), ZB_ZCL_ATTR_TYPE_8BITMAP, ZB_ZCL_ATTR_ACCESS_READ_WRITE) \
  ZB_ZCL_SET_ATTR_DESC_M(ZB_ZCL_ATTR_GP_SHARED_SECURITY_KEY_ID, (gp_shared_security_key), ZB_ZCL_ATTR_TYPE_128_BIT_KEY, ZB_ZCL_ATTR_ACCESS_READ_WRITE) \
  ZB_ZCL_SET_ATTR_DESC_M(ZB_ZCL_ATTR_GP_LINK_KEY_ID, (gp_link_key), ZB_ZCL_ATTR_TYPE_128_BIT_KEY, ZB_ZCL_ATTR_ACCESS_READ_WRITE)       \
  ZB_ZCL_SET_ATTR_DESC_M(ZB_ZCL_ATTR_GPS_MAX_SINK_TABLE_ENTRIES_ID, (gps_max_sink_table_entries), ZB_ZCL_ATTR_TYPE_U8, ZB_ZCL_ATTR_ACCESS_READ_ONLY) \
  ZB_ZCL_SET_ATTR_DESC_M(ZB_ZCL_ATTR_GPS_SINK_TABLE_ID, gps_sink_table_ptr, ZB_ZCL_ATTR_TYPE_LONG_OCTET_STRING, ZB_ZCL_ATTR_ACCESS_READ_ONLY) \
  ZB_ZCL_SET_ATTR_DESC_M(ZB_ZCL_ATTR_GPS_COMMUNICATION_MODE_ID, (gps_communication_mode), ZB_ZCL_ATTR_TYPE_8BITMAP, ZB_ZCL_ATTR_ACCESS_READ_WRITE) \
  ZB_ZCL_SET_ATTR_DESC_M(ZB_ZCL_ATTR_GPS_COMMISSIONING_EXIT_MODE_ID, (gps_commissioning_exit_mode), ZB_ZCL_ATTR_TYPE_8BITMAP, ZB_ZCL_ATTR_ACCESS_READ_WRITE) \
  ZB_ZCL_SET_ATTR_DESC_M(ZB_ZCL_ATTR_GPS_SECURITY_LEVEL_ID, (gps_security_level), ZB_ZCL_ATTR_TYPE_8BITMAP, ZB_ZCL_ATTR_ACCESS_READ_WRITE) \
  ZB_ZCL_SET_ATTR_DESC_M(ZB_ZCL_ATTR_GPS_FUNCTIONALITY_ID, (gps_functionality), ZB_ZCL_ATTR_TYPE_24BITMAP, ZB_ZCL_ATTR_ACCESS_READ_ONLY) \
  ZB_ZCL_SET_ATTR_DESC_M(ZB_ZCL_ATTR_GPS_ACTIVE_FUNCTIONALITY_ID, (gps_active_functionality), ZB_ZCL_ATTR_TYPE_24BITMAP, ZB_ZCL_ATTR_ACCESS_READ_ONLY) \
  ZB_ZCL_SET_ATTR_DESC_M(ZB_ZCL_ATTR_GPS_COMMISSIONING_WINDOW_ID, (gps_commissioning_window), ZB_ZCL_ATTR_TYPE_U16, ZB_ZCL_ATTR_ACCESS_READ_WRITE) \
  ZB_ZCL_FINISH_DECLARE_ATTRIB_LIST

#define ZB_ZCL_DECLARE_GPCB_ATTRIB_LIST_CLI(attr_list,                  \
                                        gp_shared_security_key_type,    \
                                        gp_shared_security_key,         \
                                        gp_link_key,                    \
                                        gpp_max_proxy_table_entries,    \
                                        gpp_functionality,              \
                                        gpp_active_functionality,       \
                                        gpp_proxy_table_ptr             \
  )                                                                     \
  ZB_ZCL_START_DECLARE_ATTRIB_LIST_CLUSTER_REVISION(attr_list, ZB_ZCL_GREEN_POWER) \
  ZB_ZCL_SET_ATTR_DESC_M(ZB_ZCL_ATTR_GP_SHARED_SECURITY_KEY_TYPE_ID, (gp_shared_security_key_type), ZB_ZCL_ATTR_TYPE_8BITMAP, ZB_ZCL_ATTR_ACCESS_READ_WRITE) \
  ZB_ZCL_SET_ATTR_DESC_M(ZB_ZCL_ATTR_GP_SHARED_SECURITY_KEY_ID, (gp_shared_security_key), ZB_ZCL_ATTR_TYPE_128_BIT_KEY, ZB_ZCL_ATTR_ACCESS_READ_WRITE) \
  ZB_ZCL_SET_ATTR_DESC_M(ZB_ZCL_ATTR_GP_LINK_KEY_ID, (gp_link_key), ZB_ZCL_ATTR_TYPE_128_BIT_KEY, ZB_ZCL_ATTR_ACCESS_READ_WRITE)       \
  ZB_ZCL_SET_ATTR_DESC_M(ZB_ZCL_ATTR_GPP_MAX_PROXY_TABLE_ENTRIES_ID, (gpp_max_proxy_table_entries), ZB_ZCL_ATTR_TYPE_U8, ZB_ZCL_ATTR_ACCESS_READ_ONLY) \
  ZB_ZCL_SET_ATTR_DESC_M(ZB_ZCL_ATTR_GPP_PROXY_TABLE_ID, gpp_proxy_table_ptr, ZB_ZCL_ATTR_TYPE_LONG_OCTET_STRING, ZB_ZCL_ATTR_ACCESS_READ_ONLY) \
  ZB_ZCL_SET_ATTR_DESC_M(ZB_ZCL_ATTR_GPP_FUNCTIONALITY_ID, (gpp_functionality), ZB_ZCL_ATTR_TYPE_24BITMAP, ZB_ZCL_ATTR_ACCESS_READ_ONLY) \
  ZB_ZCL_SET_ATTR_DESC_M(ZB_ZCL_ATTR_GPP_ACTIVE_FUNCTIONALITY_ID, (gpp_active_functionality), ZB_ZCL_ATTR_TYPE_24BITMAP, ZB_ZCL_ATTR_ACCESS_READ_ONLY) \
  ZB_ZCL_FINISH_DECLARE_ATTRIB_LIST

#define ZB_ZCL_DECLARE_GPCB_ATTRIB_LIST_SRV(attr_list,                  \
                                gp_shared_security_key_type,            \
                                gp_shared_security_key,                 \
                                gp_link_key,                            \
                                gps_max_sink_table_entries,             \
                                gps_communication_mode,                 \
                                gps_commissioning_exit_mode,            \
                                gps_security_level,                     \
                                gps_functionality,                      \
                                gps_active_functionality,               \
                                gps_commissioning_window,               \
                                gps_sink_table_ptr                      \
  )                                                                     \
  ZB_ZCL_START_DECLARE_ATTRIB_LIST_CLUSTER_REVISION(attr_list, ZB_ZCL_GREEN_POWER) \
  ZB_ZCL_SET_ATTR_DESC_M(ZB_ZCL_ATTR_GP_SHARED_SECURITY_KEY_TYPE_ID, (gp_shared_security_key_type), ZB_ZCL_ATTR_TYPE_8BITMAP, ZB_ZCL_ATTR_ACCESS_READ_WRITE) \
  ZB_ZCL_SET_ATTR_DESC_M(ZB_ZCL_ATTR_GP_SHARED_SECURITY_KEY_ID, (gp_shared_security_key), ZB_ZCL_ATTR_TYPE_128_BIT_KEY, ZB_ZCL_ATTR_ACCESS_READ_WRITE) \
  ZB_ZCL_SET_ATTR_DESC_M(ZB_ZCL_ATTR_GP_LINK_KEY_ID, (gp_link_key), ZB_ZCL_ATTR_TYPE_128_BIT_KEY, ZB_ZCL_ATTR_ACCESS_READ_WRITE)       \
  ZB_ZCL_SET_ATTR_DESC_M(ZB_ZCL_ATTR_GPS_MAX_SINK_TABLE_ENTRIES_ID, (gps_max_sink_table_entries), ZB_ZCL_ATTR_TYPE_U8, ZB_ZCL_ATTR_ACCESS_READ_ONLY) \
  ZB_ZCL_SET_ATTR_DESC_M(ZB_ZCL_ATTR_GPS_SINK_TABLE_ID, gps_sink_table_ptr, ZB_ZCL_ATTR_TYPE_LONG_OCTET_STRING, ZB_ZCL_ATTR_ACCESS_READ_ONLY) \
  ZB_ZCL_SET_ATTR_DESC_M(ZB_ZCL_ATTR_GPS_COMMUNICATION_MODE_ID, (gps_communication_mode), ZB_ZCL_ATTR_TYPE_8BITMAP, ZB_ZCL_ATTR_ACCESS_READ_WRITE) \
  ZB_ZCL_SET_ATTR_DESC_M(ZB_ZCL_ATTR_GPS_COMMISSIONING_EXIT_MODE_ID, (gps_commissioning_exit_mode), ZB_ZCL_ATTR_TYPE_8BITMAP, ZB_ZCL_ATTR_ACCESS_READ_WRITE) \
  ZB_ZCL_SET_ATTR_DESC_M(ZB_ZCL_ATTR_GPS_SECURITY_LEVEL_ID, (gps_security_level), ZB_ZCL_ATTR_TYPE_8BITMAP, ZB_ZCL_ATTR_ACCESS_READ_WRITE) \
  ZB_ZCL_SET_ATTR_DESC_M(ZB_ZCL_ATTR_GPS_FUNCTIONALITY_ID, (gps_functionality), ZB_ZCL_ATTR_TYPE_24BITMAP, ZB_ZCL_ATTR_ACCESS_READ_ONLY) \
  ZB_ZCL_SET_ATTR_DESC_M(ZB_ZCL_ATTR_GPS_ACTIVE_FUNCTIONALITY_ID, (gps_active_functionality), ZB_ZCL_ATTR_TYPE_24BITMAP, ZB_ZCL_ATTR_ACCESS_READ_ONLY) \
  ZB_ZCL_SET_ATTR_DESC_M(ZB_ZCL_ATTR_GPS_COMMISSIONING_WINDOW_ID, (gps_commissioning_window), ZB_ZCL_ATTR_TYPE_U16, ZB_ZCL_ATTR_ACCESS_READ_WRITE) \
  ZB_ZCL_FINISH_DECLARE_ATTRIB_LIST

#define ZB_ZCL_DECLARE_GPPB_ATTRIB_LIST_CLI(attr_list,                      \
                                            gp_shared_security_key_type,    \
                                            gp_shared_security_key,         \
                                            gp_link_key,                    \
                                            gpp_max_proxy_table_entries,    \
                                            gpp_functionality,              \
                                            gpp_active_functionality,       \
                                            gpp_proxy_table_ptr             \
        )                                                               \
  ZB_ZCL_START_DECLARE_ATTRIB_LIST_CLUSTER_REVISION(attr_list, ZB_ZCL_GREEN_POWER) \
  ZB_ZCL_SET_ATTR_DESC_M(ZB_ZCL_ATTR_GP_SHARED_SECURITY_KEY_TYPE_ID, (gp_shared_security_key_type), ZB_ZCL_ATTR_TYPE_8BITMAP, ZB_ZCL_ATTR_ACCESS_READ_WRITE) \
  ZB_ZCL_SET_ATTR_DESC_M(ZB_ZCL_ATTR_GP_SHARED_SECURITY_KEY_ID, (gp_shared_security_key), ZB_ZCL_ATTR_TYPE_128_BIT_KEY, ZB_ZCL_ATTR_ACCESS_READ_WRITE) \
  ZB_ZCL_SET_ATTR_DESC_M(ZB_ZCL_ATTR_GP_LINK_KEY_ID, (gp_link_key), ZB_ZCL_ATTR_TYPE_128_BIT_KEY, ZB_ZCL_ATTR_ACCESS_READ_WRITE)       \
  ZB_ZCL_SET_ATTR_DESC_M(ZB_ZCL_ATTR_GPP_MAX_PROXY_TABLE_ENTRIES_ID, (gpp_max_proxy_table_entries), ZB_ZCL_ATTR_TYPE_U8, ZB_ZCL_ATTR_ACCESS_READ_ONLY) \
  ZB_ZCL_SET_ATTR_DESC_M(ZB_ZCL_ATTR_GPP_PROXY_TABLE_ID, gpp_proxy_table_ptr, ZB_ZCL_ATTR_TYPE_LONG_OCTET_STRING, ZB_ZCL_ATTR_ACCESS_READ_ONLY) \
  ZB_ZCL_SET_ATTR_DESC_M(ZB_ZCL_ATTR_GPP_FUNCTIONALITY_ID, (gpp_functionality), ZB_ZCL_ATTR_TYPE_24BITMAP, ZB_ZCL_ATTR_ACCESS_READ_ONLY) \
  ZB_ZCL_SET_ATTR_DESC_M(ZB_ZCL_ATTR_GPP_ACTIVE_FUNCTIONALITY_ID, (gpp_active_functionality), ZB_ZCL_ATTR_TYPE_24BITMAP, ZB_ZCL_ATTR_ACCESS_READ_ONLY) \
  ZB_ZCL_FINISH_DECLARE_ATTRIB_LIST

#define ZB_ZCL_DECLARE_GPPB_ATTRIB_LIST_SRV(attr_list,                      \
                                            gp_shared_security_key_type,    \
                                            gp_shared_security_key,         \
                                            gp_link_key                     \
  )                                                                         \
  ZB_ZCL_START_DECLARE_ATTRIB_LIST_CLUSTER_REVISION(attr_list, ZB_ZCL_GREEN_POWER) \
  ZB_ZCL_SET_ATTR_DESC_M(ZB_ZCL_ATTR_GP_SHARED_SECURITY_KEY_TYPE_ID, (gp_shared_security_key_type), ZB_ZCL_ATTR_TYPE_8BITMAP, ZB_ZCL_ATTR_ACCESS_READ_WRITE) \
  ZB_ZCL_SET_ATTR_DESC_M(ZB_ZCL_ATTR_GP_SHARED_SECURITY_KEY_ID, (gp_shared_security_key), ZB_ZCL_ATTR_TYPE_128_BIT_KEY, ZB_ZCL_ATTR_ACCESS_READ_WRITE) \
  ZB_ZCL_SET_ATTR_DESC_M(ZB_ZCL_ATTR_GP_LINK_KEY_ID, (gp_link_key), ZB_ZCL_ATTR_TYPE_128_BIT_KEY, ZB_ZCL_ATTR_ACCESS_READ_WRITE)       \
  ZB_ZCL_FINISH_DECLARE_ATTRIB_LIST

#define ZB_ZCL_DECLARE_GP_CLUSTER_LIST(cluster_list_name, attr_list_srv, attr_list_cli) \
  zb_zcl_cluster_desc_t cluster_list_name[] =                           \
  {                                                                     \
    ZB_ZCL_CLUSTER_DESC(                                                \
      ZB_ZCL_CLUSTER_ID_GREEN_POWER,                                    \
      ZB_ZCL_ARRAY_SIZE(attr_list_srv, zb_zcl_attr_t),                  \
      (attr_list_srv),                                                  \
      ZB_ZCL_CLUSTER_SERVER_ROLE,                                       \
      ZB_ZCL_MANUF_CODE_INVALID                                         \
      ),                                                                \
    ZB_ZCL_CLUSTER_DESC(                                                \
      ZB_ZCL_CLUSTER_ID_GREEN_POWER,                                    \
      ZB_ZCL_ARRAY_SIZE(attr_list_cli, zb_zcl_attr_t),                  \
      (attr_list_cli),                                                  \
      ZB_ZCL_CLUSTER_CLIENT_ROLE,                                       \
      ZB_ZCL_MANUF_CODE_INVALID                                         \
      )                                                                 \
  }

#if defined ZB_APS_ENCRYPTION_PER_CLUSTER

#define ZB_ZCL_DECLARE_GPT_SIMPLE_DESC(                                 \
    ep_name, ep_id, in_clust_num, out_clust_num, dev_id)                \
   ZB_DECLARE_SIMPLE_DESC(in_clust_num, out_clust_num);                 \
   ZB_AF_SIMPLE_DESC_TYPE(in_clust_num, out_clust_num)  simple_desc_##ep_name = \
   {                                                                    \
     ep_id,                                                             \
     ZB_AF_GP_PROFILE_ID,                                               \
     dev_id,                                                            \
     ZB_GP_VER,                                                         \
     0,                                                                 \
     in_clust_num,                                                      \
     out_clust_num,                                                     \
     {                                                                  \
       ZB_ZCL_CLUSTER_ID_GREEN_POWER,                                   \
     },                                                                 \
     { 0 }                                                              \
   }

#define ZB_ZCL_DECLARE_GPPB_SIMPLE_DESC(                                \
    ep_name, ep_id, in_clust_num, out_clust_num, dev_id)                \
  ZB_DECLARE_SIMPLE_DESC(in_clust_num, out_clust_num);                  \
   ZB_AF_SIMPLE_DESC_TYPE(in_clust_num, out_clust_num)  simple_desc_##ep_name = \
   {                                                                    \
     ep_id,                                                             \
     ZB_AF_GP_PROFILE_ID,                                               \
     dev_id,                                                            \
     ZB_GP_VER,                                                         \
     0,                                                                 \
     in_clust_num,                                                      \
     out_clust_num,                                                     \
     {                                                                  \
       ZB_ZCL_CLUSTER_ID_GREEN_POWER,                                   \
     },                                                                 \
     { 0 }                                                              \
   }

#define ZB_ZCL_DECLARE_GPCB_SIMPLE_DESC(                                \
    ep_name, ep_id, in_clust_num, out_clust_num, dev_id)                \
  /* WARNING: Do not redefine _1_1_ simple desc - it already exists. Need to add \
     ZB_DECLARE_SIMPLE_DESC() if in_clust_num or out_clust_num is not 1 */ \
   ZB_AF_SIMPLE_DESC_TYPE(in_clust_num, out_clust_num)  simple_desc_##ep_name = \
   {                                                                    \
     ep_id,                                                             \
     ZB_AF_GP_PROFILE_ID,                                               \
     dev_id,                                                            \
     ZB_GP_VER,                                                         \
     0,                                                                 \
     in_clust_num,                                                      \
     out_clust_num,                                                     \
     {                                                                  \
       ZB_ZCL_CLUSTER_ID_GREEN_POWER,                                   \
       ZB_ZCL_CLUSTER_ID_GREEN_POWER                                    \
     },                                                                 \
     { 0 }                                                              \
   }

#else /* ZB_APS_ENCRYPTION_PER_CLUSTER */

#define ZB_ZCL_DECLARE_GPT_SIMPLE_DESC(                                 \
    ep_name, ep_id, in_clust_num, out_clust_num, dev_id)                \
   ZB_DECLARE_SIMPLE_DESC(in_clust_num, out_clust_num);                  \
   ZB_AF_SIMPLE_DESC_TYPE(in_clust_num, out_clust_num)  simple_desc_##ep_name = \
   {                                                                    \
     ep_id,                                                             \
     ZB_AF_GP_PROFILE_ID,                                               \
     dev_id,                                                            \
     ZB_GP_VER,                                                         \
     0,                                                                 \
     in_clust_num,                                                      \
     out_clust_num,                                                     \
     {                                                                  \
       ZB_ZCL_CLUSTER_ID_GREEN_POWER,                                   \
     }                                                                  \
   }

#define ZB_ZCL_DECLARE_GPPB_SIMPLE_DESC(                                \
    ep_name, ep_id, in_clust_num, out_clust_num, dev_id)                \
  ZB_DECLARE_SIMPLE_DESC(in_clust_num, out_clust_num);                  \
   ZB_AF_SIMPLE_DESC_TYPE(in_clust_num, out_clust_num)  simple_desc_##ep_name = \
   {                                                                    \
     ep_id,                                                             \
     ZB_AF_GP_PROFILE_ID,                                               \
     dev_id,                                                            \
     ZB_GP_VER,                                                         \
     0,                                                                 \
     in_clust_num,                                                      \
     out_clust_num,                                                     \
     {                                                                  \
       ZB_ZCL_CLUSTER_ID_GREEN_POWER,                                   \
     }                                                                  \
   }

#define ZB_ZCL_DECLARE_GPCB_SIMPLE_DESC(                                \
    ep_name, ep_id, in_clust_num, out_clust_num, dev_id)                \
  /* WARNING: Do not redefine _1_1_ simple desc - it already exists. Need to add \
     ZB_DECLARE_SIMPLE_DESC() if in_clust_num or out_clust_num is not 1 */ \
   ZB_AF_SIMPLE_DESC_TYPE(in_clust_num, out_clust_num)  simple_desc_##ep_name = \
   {                                                                    \
     ep_id,                                                             \
     ZB_AF_GP_PROFILE_ID,                                               \
     dev_id,                                                            \
     ZB_GP_VER,                                                         \
     0,                                                                 \
     in_clust_num,                                                      \
     out_clust_num,                                                     \
     {                                                                  \
       ZB_ZCL_CLUSTER_ID_GREEN_POWER,                                   \
       ZB_ZCL_CLUSTER_ID_GREEN_POWER                                    \
     }                                                                  \
   }

#endif /* ZB_APS_ENCRYPTION_PER_CLUSTER */

#define ZB_ZCL_DECLARE_GPT_EP(ep_name, ep_id, cluster_list)             \
  ZB_ZCL_DECLARE_GPT_SIMPLE_DESC(                                       \
     ep_name,                                                           \
     ep_id,                                                             \
     1,                                                                 \
     0,                                                                 \
     ZGP_DEVICE_TARGET);                                                \
  ZB_AF_DECLARE_ENDPOINT_DESC(                                          \
     ep_name,                                                           \
     ep_id,                                                             \
     ZB_AF_GP_PROFILE_ID,                                               \
     0,                                                                 \
     NULL,                                                              \
     ZB_ZCL_ARRAY_SIZE(                                                 \
       cluster_list,                                                    \
       zb_zcl_cluster_desc_t),                                          \
     cluster_list,                                                      \
     (zb_af_simple_desc_1_1_t*)&simple_desc_##ep_name,                  \
     0, NULL, /* No reporting ctx */                                    \
     0, NULL)

#define ZB_ZCL_DECLARE_GPTP_EP(ep_name, ep_id, cluster_list)            \
  ZB_ZCL_DECLARE_GPT_SIMPLE_DESC(                                       \
     ep_name,                                                           \
     ep_id,                                                             \
     1,                                                                 \
     0,                                                                 \
     ZGP_DEVICE_TARGET_PLUS);                                           \
  ZB_AF_DECLARE_ENDPOINT_DESC(                                          \
     ep_name,                                                           \
     ep_id,                                                             \
     ZB_AF_GP_PROFILE_ID,                                               \
     0,                                                                 \
     NULL,                                                              \
     ZB_ZCL_ARRAY_SIZE(                                                 \
       cluster_list,                                                    \
       zb_zcl_cluster_desc_t),                                          \
     cluster_list,                                                      \
     (zb_af_simple_desc_1_1_t*)&simple_desc_##ep_name,                  \
     0, NULL, /* No reporting ctx */                                    \
     0, NULL)

#define ZB_ZCL_DECLARE_GPPB_EP(ep_name, ep_id, cluster_list)            \
  ZB_ZCL_DECLARE_GPPB_SIMPLE_DESC(                                      \
     ep_name,                                                           \
     ep_id,                                                             \
     0,                                                                 \
     1,                                                                 \
     ZGP_DEVICE_PROXY_BASIC);                                           \
  ZB_AF_DECLARE_ENDPOINT_DESC(                                          \
     ep_name,                                                           \
     ep_id,                                                             \
     ZB_AF_GP_PROFILE_ID,                                               \
     0,                                                                 \
     NULL,                                                              \
     ZB_ZCL_ARRAY_SIZE(                                                 \
       cluster_list,                                                    \
       zb_zcl_cluster_desc_t),                                          \
     cluster_list,                                                      \
     (zb_af_simple_desc_1_1_t*)&simple_desc_##ep_name,                  \
     0, NULL, /* No reporting ctx */                                    \
     0, NULL)


#define ZB_ZCL_DECLARE_GPCB_EP(ep_name, ep_id, cluster_list)            \
  ZB_ZCL_DECLARE_GPCB_SIMPLE_DESC(                                      \
          ep_name,                                                      \
          ep_id,                                                        \
          1,                                                            \
          1,                                                            \
          ZGP_DEVICE_COMBO_BASIC);                                      \
   ZB_AF_DECLARE_ENDPOINT_DESC(                                         \
          ep_name,                                                      \
          ep_id,                                                        \
          ZB_AF_GP_PROFILE_ID,                                          \
          0,                                                            \
          NULL,                                                         \
          ZB_ZCL_ARRAY_SIZE(                                            \
            cluster_list,                                               \
            zb_zcl_cluster_desc_t),                                     \
          cluster_list,                                                 \
          (zb_af_simple_desc_1_1_t*)&simple_desc_##ep_name,             \
          0, NULL, /* No reporting ctx */                               \
          0, NULL)


#define ZB_ZCL_DECLARE_GP_CTX(device_ctx, ep_name)                      \
  ZBOSS_DECLARE_DEVICE_CTX_1_EP(device_ctx, ep_name)                    \

void zb_zcl_green_power_init_server(void);
void zb_zcl_green_power_init_client(void);
bool zb_zcl_green_power_cluster_handler(zb_uint8_t param);
#define ZB_ZCL_CLUSTER_ID_GREEN_POWER_SERVER_ROLE_INIT zb_zcl_green_power_init_server
#define ZB_ZCL_CLUSTER_ID_GREEN_POWER_CLIENT_ROLE_INIT zb_zcl_green_power_init_client

#define ZGP_GPS_COMMUNICATION_MODE      ZGP_CTXC().cluster.gps_communication_mode
#define ZGP_GPS_COMMISSIONING_EXIT_MODE ZGP_CTXC().cluster.gps_commissioning_exit_mode
#define ZGP_GPS_COMMISSIONING_WINDOW    ZGP_CTXC().cluster.gps_commissioning_window

#define ZGP_GP_LINK_KEY                 ZGP_CTXC().cluster.gp_link_key
#define ZGP_GPS_SECURITY_LEVEL          ZGP_CTXC().cluster.gps_security_level
#define ZGP_GP_SHARED_SECURITY_KEY_TYPE ZGP_CTXC().cluster.gp_shared_security_key_type
#define ZGP_GP_SHARED_SECURITY_KEY      ZGP_CTXC().cluster.gp_shared_security_key

#define ZGP_GPS_GET_SECURITY_LEVEL()\
  (ZGP_GPS_SECURITY_LEVEL & 3)

#define ZGP_GPS_GET_PROTECT_WITH_GP_LINK_KEY()\
  ((ZGP_GPS_SECURITY_LEVEL >> 2) & 1)

#define ZGP_GPS_GET_INVOLVE_TC()\
  ((ZGP_GPS_SECURITY_LEVEL >> 3) & 1)

#define ZB_ZGP_COMM_MODE_OPT_GET_ACTION(opt)\
  ((opt) & 1)

#define ZB_ZGP_COMM_MODE_OPT_GET_UNICAST(opt)\
  (((opt) >> 5) & 1)

#define ZB_ZGP_PROXY_COMM_MODE_INT_OPT_GET_EXIT_MODE(opt)\
  ((opt) & 7)

#define ZB_ZGP_PROXY_COMM_MODE_INT_OPT_GET_CHNL_PRESENT(opt)\
  (((opt) >> 3) & 1)

#define ZB_ZGP_PROXY_COMM_MODE_INT_OPT_GET_UNICAST_COMMUNICATION(opt)\
  (((opt) >> 4) & 1)

#define ZB_ZGP_PROXY_COMM_MODE_IS_UNICAST()\
  ZB_ZGP_PROXY_COMM_MODE_INT_OPT_GET_UNICAST_COMMUNICATION(\
    ZGP_CTXC().comm_data.proxy_comm_mode_options)

#define ZB_ZGP_PROXY_COMM_MODE_IS_EXIT_AFTER_FIRST_PAIRING_SUCCESS()\
  (ZB_ZGP_PROXY_COMM_MODE_INT_OPT_GET_EXIT_MODE(\
    ZGP_CTXC().comm_data.proxy_comm_mode_options) & \
    ZGP_COMMISSIONING_EXIT_MODE_ON_PAIRING_SUCCESS)

#define ZB_ZGP_COMM_MODE_OPT_GET_ON_COMM_WIND_EXP(opt)  \
  (((opt) >> 1) & 0x01)

#define ZB_ZGP_COMM_MODE_OPT_GET_CHNL_PRESENT(opt)\
  (((opt) >> 4) & 0x01)

#define ZGP_GPS_GET_COMMISSIONING_WINDOW()\
  ((ZGP_GPS_COMMISSIONING_EXIT_MODE & ZGP_COMMISSIONING_EXIT_MODE_ON_COMMISSIONING_WINDOW_EXPIRATION) ?\
      ZGP_GPS_COMMISSIONING_WINDOW : 0)

#define ZB_ZGP_COMM_NOTIF_OPT_GET_APP_ID(opt)\
  ((opt) & 0x07)

#define ZB_ZGP_GP_COMM_NOTIF_OPT_GET_RX_AFTER_TX(opt)\
  (((opt) >> 3) & 0x01)

#define ZB_ZGP_GP_COMM_NOTIF_OPT_GET_SEC_LVL(opt)\
  (((opt) >> 4) & 0x03)

#define ZB_ZGP_GP_COMM_NOTIF_OPT_GET_KEY_TYPE(opt)\
  (((opt) >> 6) & 0x07)

#define ZB_ZGP_GP_COMM_NOTIF_OPT_GET_SECUR_FAILED(opt)\
  (((opt) >> 9) & 0x01)

/* The MIC field SHALL only be present if the Security processing failed
 * sub-field is set to 0b1 */
#define ZB_ZGP_COMM_NOTIF_OPT_GET_MIC_PRESENT(opt)\
  (((opt) >> 9) & 0x01)

#define ZB_ZGP_GP_COMM_NOTIF_OPT_GET_BIDIR_CAP(opt)\
  (((opt) >> 10) & 0x01)

#define ZB_ZGP_COMM_NOTIF_OPT_GET_PROXY_INFO_PRESENT(opt)\
  (((opt) >> 11) & 0x01)

/*
Bits: 0..2      3               4..5            6..8            9                               10                             11
ApplicationID   RxAfterTx       SecurityLevel   SecurityKeyType Security processing failed      Bidirectional Capability       Proxy info present

 */
#define ZB_ZGP_FILL_COMM_NOTIFICATION_OPTIONS(app_id, rx_after_tx, secur_level, key_type, secur_failed, bidir_cap, proxy_info) \
  (((app_id) & 7) | ((!!(rx_after_tx)) << 3) | (((secur_level) & 3) << 4) | (((key_type) & 7) << 6) | ((!!(secur_failed)) << 9) | ((!!(bidir_cap)) << 10) | ((!!(proxy_info)) << 11))

#define ZB_ZGP_PAIRING_OPT_GET_APP_ID(opt)\
  ((opt) & 0x07)

#define ZB_ZGP_PAIRING_OPT_GET_REMOVE_GPD(opt)\
  (((opt) >> 4) & 0x01)

#define ZB_ZGP_PAIRING_OPT_GET_COMMUNICATION_MODE(opt)\
  (((opt) >> 5) & 0x03)

#define ZB_ZGP_PAIRING_OPT_GET_ADD_SINK(opt)\
  (((opt) >> 3) & 0x01)

#define ZB_ZGP_PAIRING_OPT_GET_SEC_LEVEL(opt)\
  (((opt) >> 9) & 0x03)

#define ZB_ZGP_PAIRING_OPT_GET_KEY_TYPE(opt)\
  (((opt) >> 11) & 0x07)

#define ZB_ZGP_PAIRING_OPT_GET_FRAME_CNT_PRESENT(opt)\
  (((opt) >> 14) & 0x01)

#define ZB_ZGP_PAIRING_OPT_GET_SEQ_NUM_CAP(opt)\
  (((opt) >> 8) & 0x01)

#define ZB_ZGP_PAIRING_OPT_GET_FIX_LOC(opt)\
  (((opt) >> 7) & 0x01)

#define ZB_ZGP_PAIRING_OPT_GET_SEC_KEY_PRESENT(opt)\
  (((opt) >> 15) & 0x01)

#define ZB_ZGP_PAIRING_OPT_GET_ASSIGNED_ALIAS_PRESENT(opt)\
  (((opt) >> 16) & 0x01)

#define ZB_ZGP_PAIRING_OPT_GET_FRWD_RADIUS(opt)\
  (((opt) >> 17) & 0x01)

#define ZB_ZGP_GP_NOTIF_OPT_GET_APP_ID(opt)\
  ((opt) & 0x07)

#define ZB_ZGP_GP_NOTIF_OPT_GET_SEC_LVL(opt)\
  (((opt) >> 6) & 0x03)

#define ZB_ZGP_GP_NOTIF_OPT_GET_KEY_TYPE(opt)\
  (((opt) >> 8) & 0x07)

#define ZB_ZGP_GP_NOTIF_OPT_GET_RX_AFTER_TX(opt)\
  (((opt) >> 11) & 0x01)

#define ZB_ZGP_GP_NOTIF_OPT_GET_BIDIR_CAP(opt)\
  (((opt) >> 13) & 0x01)

#define ZB_ZGP_GP_NOTIF_OPT_GET_PROXY_INFO_PRESENT(opt)\
  (((opt) >> 14) & 0x01)

#define ZB_ZGP_GP_NOTIF_OPT_GET_RECV_AS_UNICAST(opt)\
  (((opt) >> 15) & 0x01)

#define ZB_ZGP_GP_NOTIF_OPT_SET_RECV_AS_UNICAST(opt)\
  ((opt) = (opt | (1 << 15)))

#define ZB_ZGP_GP_RESPONSE_OPT_GET_APP_ID(opt) \
  ((opt) & 0x07)

#define ZB_ZGP_FILL_GP_RESPONSE_OPTIONS(app_id, ep_match) \
  (((app_id) & 7) | ((!!(ep_match)) << 3))

#define ZB_ZGP_PROXY_ENTRY_OPT_GET_LW_GPS(opt)\
  (((opt) >> 6) & 0x01)

#define ZB_ZGP_PROXY_ENTRY_OPT_GET_DGROUP_GPS(opt)\
  (((opt) >> 7) & 0x01)

#define ZB_ZGP_PROXY_ENTRY_OPT_GET_PRECOMMISSIONED_GROUP_GPS(opt)\
  (((opt) >> 8) & 0x01)

#define ZB_ZGP_PROXY_ENTRY_OPT_GET_OPT_EXT(opt)\
  (((opt) >> 15) & 0x01)

#define ZB_ZGP_GP_PAIRING_CONF_OPT_GET_APP_ID(opt) \
  ((opt) & 0x07)

#define ZB_ZGP_GP_PAIRING_CONF_GET_COMMUNICATION_MODE(opt)\
  (((opt) >> 3) & 0x03)

#define ZB_ZGP_GP_PAIRING_CONF_GET_SEQ_NUM_CAPS(opt)\
  (((opt) >> 5) & 0x01)

#define ZB_ZGP_GP_PAIRING_CONF_GET_RX_ON_CAPS(opt)\
  (((opt) >> 6) & 0x01)

#define ZB_ZGP_GP_PAIRING_CONF_GET_FIXED_LOC(opt)\
  (((opt) >> 7) & 0x01)

#define ZB_ZGP_GP_PAIRING_CONF_GET_ASSIGNED_ALIAS_PRESENT(opt)\
  (((opt) >> 8) & 0x01)

#define ZB_ZGP_GP_PAIRING_CONF_GET_SEC_USE(opt)\
  (((opt) >> 9) & 0x01)

#define ZB_ZGP_GP_PAIRING_CONF_GET_APP_INFO_PRESENT(opt)\
  (((opt) >> 10) & 0x01)

#define ZB_ZGP_GP_PAIRING_CONF_APP_INFO_GET_MANUF_ID_PRESENT(app_info)\
  ((app_info) & 0x01)

#define ZB_ZGP_GP_PAIRING_CONF_APP_INFO_GET_MODEL_ID_PRESENT(app_info)\
  (((app_info) >> 1) & 0x01)

#define ZB_ZGP_GP_PAIRING_CONF_APP_INFO_GET_CMDS_PRESENT(app_info)\
  (((app_info) >> 2) & 0x01)

#define ZB_ZGP_GP_PAIRING_CONF_APP_INFO_GET_CLUSTERS_PRESENT(app_info)\
  (((app_info) >> 3) & 0x01)

#define ZB_ZGP_GP_PAIRING_CONF_APP_INFO_GET_SWITCH_INFO_PRESENT(app_info)\
  (((app_info) >> 4) & 0x01)

#define ZB_ZGP_GP_PAIRING_CONF_APP_INFO_GET_APP_DESCRIPTION_FOLLOWS(app_info)\
  (((app_info) >> 5) & 0x01)

#define ZB_ZGP_GP_PAIRING_CONF_APP_INFO_SET_MANUF_ID_PRESENT(app_info)\
  ((app_info) |= 0x01)

#define ZB_ZGP_GP_PAIRING_CONF_APP_INFO_SET_MODEL_ID_PRESENT(app_info)\
  ((app_info) |= 0x02)

#define ZB_ZGP_GP_PAIRING_CONF_APP_INFO_SET_CMDS_PRESENT(app_info)\
  ((app_info) |= 0x04)

#define ZB_ZGP_GP_PAIRING_CONF_APP_INFO_SET_CLUSTERS_PRESENT(app_info)\
  ((app_info) |= 0x08)

#define ZB_ZGP_GP_PAIRING_CONF_APP_INFO_SET_SWITCH_INFO_PRESENT(app_info)\
  ((app_info) |= 0x10)

#define ZB_ZGP_GP_PAIRING_CONF_APP_INFO_SET_APP_DESCRIPTION_FOLLOWS(app_info)\
  ((app_info) |= 0x20)

#define ZB_ZGP_GP_PAIRING_CONF_GET_ACTIONS(actions)\
  ((actions) & 0x07)

#define ZB_ZGP_GP_PAIRING_CONF_GET_SEND_PAIRING(actions)\
  (((actions) >> 3) & 0x01)

#define ZB_ZGP_FILL_GP_PAIRING_CONF_OPTIONS(app_id, comm_mode, seq_num_cap, rx_on_cap, fix_loc, asgnd_alias, sec_use, app_info)\
  ((app_id) | (((comm_mode) & 3) << 3) | ((!!(seq_num_cap)) << 5) | ((!!(rx_on_cap)) << 6) | ((!!(fix_loc)) << 7) | ((!!(asgnd_alias)) << 8) | ((!!(sec_use)) << 9) | ((!!(app_info)) << 10))

#define ZB_ZGP_FILL_GP_PAIRING_CONF_ACTIONS(action, send_pairing)\
  ((action) | ((!!(send_pairing)) << 3))

#define ZB_ZGP_GP_PAIRING_CONF_APP_INFO_MANUF_ID_NO_PRESENT 0
#define ZB_ZGP_GP_PAIRING_CONF_APP_INFO_MANUF_ID_PRESENT 1

#define ZB_ZGP_GP_PAIRING_CONF_APP_INFO_MODEL_ID_NO_PRESENT 0
#define ZB_ZGP_GP_PAIRING_CONF_APP_INFO_MODEL_ID_PRESENT 1

#define ZB_ZGP_GP_PAIRING_CONF_APP_INFO_GPD_CMDS_NO_PRESENT 0
#define ZB_ZGP_GP_PAIRING_CONF_APP_INFO_GPD_CMDS_PRESENT 1

#define ZB_ZGP_GP_PAIRING_CONF_APP_INFO_CLSTS_NO_PRESENT 0
#define ZB_ZGP_GP_PAIRING_CONF_APP_INFO_CLSTS_PRESENT 1

#define ZB_ZGP_FILL_GP_PAIRING_CONF_APP_INFO(manuf_id, model_id, gpd_cmds, clsts)\
  ((!!(manuf_id)) | ((!!(model_id)) << 1) | ((!!(gpd_cmds)) << 2) | ((!!(clsts)) << 3))

typedef enum zgp_table_request_entries_type_e
{
  ZGP_REQUEST_TABLE_ENTRIES_BY_GPD_ID,
  ZGP_REQUEST_TABLE_ENTRIES_BY_INDEX
} zgp_table_request_entries_type_t;

#define ZB_ZGP_GP_PROXY_TBL_REQ_GET_APP_ID(opt)\
  ((opt) & 0x07)

#define ZB_ZGP_GP_PROXY_TBL_REQ_GET_REQ_TYPE(opt)\
  (((opt) >> 3) & 3)

#define ZB_ZGP_GP_PROXY_TBL_REQ_FILL_OPT(app_id, req_type)\
  ((app_id) | (((req_type) & 3) << 3))

#define ZB_ZGP_GP_SINK_COMM_MODE_FILL_OPT(action, inv_gpm_seq, inv_gpm_pair, inv_prx) \
  ((!!(action)) | ((!!(inv_gpm_seq)) << 1) | ((!!(inv_gpm_pair)) << 2) | ((!!(inv_prx)) << 3))

#define ZB_ZGP_GP_SINK_COMM_MODE_GET_ACTION(opt)\
  ((opt) & 0x01)

#define ZB_ZGP_GP_SINK_COMM_MODE_GET_INVOLVE_GPM_IN_SECURITY(opt)\
  (((opt) >> 1) & 0x01)

#define ZB_ZGP_GP_SINK_COMM_MODE_GET_INVOLVE_GPM_IN_PAIRING(opt)\
  (((opt) >> 2) & 0x01)

#define ZB_ZGP_GP_SINK_COMM_MODE_GET_INV_PROXIES(opt)\
  (((opt) >> 3) & 0x01)

#define ZB_ZGP_SET_SINK_COMM_MODE(opt) \
  ZGP_CTXC().sink_comm_mode_opt = (1 | (ZGP_CTXC().sink_comm_mode_opt & 2) | ((opt) << 8))

#define ZB_ZGP_CLR_SINK_COMM_MODE()\
  ZGP_CTXC().sink_comm_mode_opt = (ZGP_CTXC().sink_comm_mode_opt & 2)

#define ZB_ZGP_SET_PROXY_COMM_MODE_COMMUNICATION(cm)\
  ZGP_CTXC().sink_comm_mode_opt = ((ZGP_CTXC().sink_comm_mode_opt & ~2) | ((!!(cm)) << 1))

#define ZB_ZGP_SINK_IS_SEND_ENTER_OR_LEAVE_FOR_PROXIES()\
  (((ZGP_CTXC().sink_comm_mode_opt & 1) == 0) || ((ZGP_CTXC().sink_comm_mode_opt & 1) == 1 &&\
                                             (ZB_ZGP_GP_SINK_COMM_MODE_GET_INV_PROXIES((ZGP_CTXC().sink_comm_mode_opt & 0xFF00) >> 8))))

#define ZB_ZGP_GET_SINK_COMM_MODE_START_STOP_CAUSE() \
  ((ZGP_CTXC().sink_comm_mode_opt) & 1)

#define ZGP_PROXY_COMM_MODE_IS_UNICAST()\
  ((ZGP_CTXC().sink_comm_mode_opt >> 1) & 1)

#ifdef ZB_ENABLE_ZGP_DIRECT
/**
 * @brief Converting LQI incoming from MAC into ZGP cluster specific format
 *
 * @param lqi  [in]  Incoming LQI value
 *
 * @return Encoded lqi specific value
 *
 * @see ZGP spec, A.3.3.4.1
 */
zb_uint8_t zb_zgp_cluster_encode_lqi(zb_uint8_t lqi);
/**
 * @brief Converting RSSI and LQI incoming from MAC into ZGP cluster specific format
 *
 * @param rssi [in]  Incoming RSSI value
 * @param lqi  [in]  Incoming LQI value
 *
 * @return Encoded rssi+lqi specific value
 *
 * @see ZGP spec, A.3.3.4.1
 */
zb_uint8_t zb_zgp_cluster_encode_link_quality(zb_int8_t rssi, zb_uint8_t lqi);
#endif  /* ZB_ENABLE_ZGP_DIRECT */

/********************************************************************/
/***************** ZGP ZCL helpers definitions **********************/
/********************************************************************/

/**
 * @brief Perform send general zcl read attributes command for ZGP cluster
 *
 * @param buf_ref        [in]  Buffer reference
 * @param dst_addr       [in]  Destination address
 * @param dst_addr_mode  [in]  Destination address mode
 * @param attr_ids       [in]  Attribute IDs list
 * @param attr_cnt       [in]  Attribute IDs list size
 * @param def_resp       [in]  Enable ZCL default response if TRUE
 * @param cb             [in]  Call callback if needed after sending request
 *
 */
void zgp_cluster_read_attrs(zb_uint8_t     buf_ref,
                                 zb_uint16_t    dst_addr,
                                 zb_uint8_t     dst_addr_mode,
                                 zb_uint16_t   *attr_ids,
                                 zb_uint8_t     attr_cnt,
                                 zb_uint8_t     dir,
                                 zb_uint8_t     def_resp,
                                 zb_callback_t  cb);

/**
 * @brief Perform send general zcl read attribute command for ZGP cluster
 *
 * @param buf_ref        [in]  Buffer reference
 * @param dst_addr       [in]  Destination address
 * @param dst_addr_mode  [in]  Destination address mode
 * @param attr_id        [in]  Attribute ID
 * @param def_resp       [in]  Enable ZCL default response if TRUE
 * @param cb             [in]  Call callback if needed after sending request
 *
 */
void zgp_cluster_read_attr(zb_uint8_t     buf_ref,
                                zb_uint16_t    dst_addr,
                                zb_uint8_t     dst_addr_mode,
                                zb_uint16_t    attr_id,
                                zb_uint8_t     dir,
                                zb_uint8_t     def_resp,
                                zb_callback_t  cb);

/**
 * @brief Perform send general zcl write attribute command for ZGP cluster
 *
 * @param buf_ref        [in]  Buffer reference
 * @param dst_addr       [in]  Destination address
 * @param dst_addr_mode  [in]  Destination address mode
 * @param attr_id        [in]  Attribute ID
 * @param attr_type      [in]  Attribute type
 * @param attr_val       [in]  Pointer to attribute value
 * @param def_resp       [in]  Enable ZCL default response if TRUE
 * @param cb             [in]  Call callback if needed after sending request
 *
 */
void zgp_cluster_write_attr(zb_uint8_t     buf_ref,
                                 zb_uint16_t    dst_addr,
                                 zb_uint8_t     dst_addr_mode,
                                 zb_uint16_t    attr_id,
                                 zb_uint8_t     attr_type,
                                 zb_uint8_t    *attr_val,
                                 zb_uint8_t     dir,
                                 zb_uint8_t     def_resp,
                                 zb_callback_t  cb);

// ******************************************************************

#ifdef ZB_ENABLE_ZGP_DIRECT
/**
 * @brief ZGP message status
 *
 * Possible values of Status field in zgp data confirmations,
 * responses and indications.
 *
 * Status codes are not specified in ZGP specification, only their names.
 * For ZGP status codes range 0x80-0xda is used since it was reserved and not
 * used in MAC status enumeration.
 */
typedef enum zb_zgp_status_e
{
  ZB_ZGP_STATUS_ENTRY_REPLACED = 0x80,
  ZB_ZGP_STATUS_ENTRY_ADDED    = 0x81,
  ZB_ZGP_STATUS_ENTRY_EXPIRED  = 0x82,
  ZB_ZGP_STATUS_ENTRY_REMOVED  = 0x83,
  ZB_ZGP_STATUS_TX_QUEUE_FULL  = 0x84,
}
zb_zgp_status_t;
#endif  /* ZB_ENABLE_ZGP_DIRECT */

/**
 * @brief ZGP commissioning states enumeration
 */
typedef enum zb_zgp_comm_state_e
{
  ZGP_COMM_STATE_IDLE,  /* 0 */
  ZGP_COMM_STATE_CHANNEL_REQ_RECEIVED,
  ZGP_COMM_STATE_CHANNEL_CONFIG_GET_CUR_CHANNEL,
#ifdef ZB_MAC_COEX_CONTROL
  ZGP_COMM_STATE_SET_COEX_SHUTDOWN,
#endif /* ZB_MAC_COEX_CONTROL */
#ifdef ZB_ENABLE_ZGP_DIRECT
  ZGP_COMM_STATE_CHANNEL_CONFIG_SET_TEMP_CHANNEL,
  ZGP_COMM_STATE_CHANNEL_CONFIG_ADDED_TO_Q,
  ZGP_COMM_STATE_CHAN_CFG_SENT_RET_CHANNEL,
  ZGP_COMM_STATE_CHAN_CFG_FAILED_RET_CHANNEL,
#endif  /* ZB_ENABLE_ZGP_DIRECT */
  ZGP_COMM_STATE_CHANNEL_CONFIG_SENT,
  ZGP_COMM_STATE_COMM_REQ_RECEIVED_WAIT_FOR_APP,
  ZGP_COMM_STATE_COMM_REQ_RECEIVED_AND_APPROVED,
#ifdef ZB_ENABLE_ZGP_DIRECT
  ZGP_COMM_STATE_COMMISSIONING_REPLY_ADDED_TO_Q,
#endif  /* ZB_ENABLE_ZGP_DIRECT */
  ZGP_COMM_STATE_COMMISSIONING_REPLY_SENT,
  ZGP_COMM_STATE_CHANNEL_REQ_COLLECT,
  ZGP_COMM_STATE_COMMISSIONING_REQ_COLLECT,
  ZGP_COMM_STATE_COMMISSIONING_FINALIZING,
  ZGP_COMM_STATE_COMMISSIONING_TIMED_OUT,
  ZGP_COMM_STATE_COMMISSIONING_CANCELLED,
  ZGP_COMM_STATE_COMMISSIONING_WAIT_APP_DESCR
}
zb_zgp_comm_state_t;

typedef ZB_PACKED_PRE struct zb_gp_data_req_s
{
  zb_uint8_t      handle;
  zb_uint8_t      action;
  zb_uint8_t      tx_options;
  zb_zgpd_id_t    zgpd_id;
  zb_ieee_addr_t  ieee_addr;
  zb_uint8_t      cmd_id;
  zb_uint8_t      payload_len;
  zb_time_t       tx_q_ent_lifetime;
  zb_uint8_t      pld[ZB_ZGP_TX_CMD_PLD_MAX_SIZE]; /**< Payload */
}
ZB_PACKED_STRUCT
zb_gp_data_req_t;

#ifdef ZB_ENABLE_ZGP_DIRECT
typedef struct zb_gp_data_cfm_s
{
  zb_uint8_t        handle;
  /* Extension of the spec */
  zb_zgpd_id_t      zgpd_id;
  zb_uint8_t        cmd_id;
}
zb_gp_data_cfm_t;
#endif  /* ZB_ENABLE_ZGP_DIRECT */

typedef ZB_PACKED_PRE struct zb_zgp_gp_comm_notification_req_s
{
  /* optimization: use reserved 15 bit of options as indicate that command received in unicast mode */
  zb_uint16_t            options;
  zb_zgpd_addr_t         zgpd_addr;
  zb_uint8_t             endpoint;
  zb_uint32_t            gpd_sec_frame_counter;
  zb_uint8_t             gpd_cmd_id;
  /* +1 bytes for payload size placed at beginning buffer */
  zb_uint8_t             payload[MAX_ZGP_CLUSTER_GPDF_PAYLOAD_SIZE+1];
  zb_zgp_gp_proxy_info_t proxy_info;
  zb_uint32_t            mic;
}
ZB_PACKED_STRUCT zb_zgp_gp_comm_notification_req_t;

typedef ZB_PACKED_PRE struct zb_zgp_gp_pairing_req_s
{
  zb_uint32_t    options;
  zb_zgpd_addr_t zgpd_addr;
  zb_uint8_t     endpoint;
  zb_ieee_addr_t sink_ieee_addr;
  zb_uint16_t    sink_nwk_addr;
  zb_uint16_t    sink_group_id;
  zb_uint8_t     dev_id;
  zb_uint32_t    sec_frame_counter;
  zb_uint8_t     key[ZB_CCM_KEY_SIZE];
  zb_uint16_t    assigned_alias;
  zb_uint8_t     frwd_radius;
}
ZB_PACKED_STRUCT zb_zgp_gp_pairing_req_t;

typedef ZB_PACKED_PRE struct zb_zgp_gp_pairing_send_req_s
{
  /* 0 bit - add_sink
   * 1 bit - remove gpd
   * 2 bit - send dev annce
   * 3 bit - indicate that dup for groupcast precommissioned is complete
   * 4 bit - need send incorrect LW unicast remove pairing, see A.3.5.2.4
   */
  zb_uint8_t       send_options;
  zb_zgpd_addr_t   zgpd_id;
  zb_uint8_t       endpoint;
  zb_uint16_t      options;
  zb_uint8_t       sec_options;
  zb_uint32_t      security_counter;
  zb_uint8_t       zgpd_key[ZB_CCM_KEY_SIZE];
  zb_uint16_t      zgpd_assigned_alias;
  zb_uint8_t       groupcast_radius;
  zb_uint8_t       device_id;
  zgp_pair_group_list_t sgrp[ZB_ZGP_MAX_SINK_GROUP_PER_GPD];
  zb_callback_t    callback;
}
ZB_PACKED_STRUCT zb_zgp_gp_pairing_send_req_t;

#define ZB_ZGP_GP_PAIRING_MAKE_SEND_OPTIONS(req, add_sink, remove_gpd, send_da)\
  (req)->send_options = ((!!(add_sink)) | ((!!(remove_gpd)) << 1) | ((!!(send_da)) << 2))

#define ZB_ZGP_GP_PAIRING_OPTIONS_SET_DUP_COMPLETE(req)\
  (req)->send_options |= (1 << 3)

#define ZB_ZGP_GP_PAIRING_OPTIONS_SET_SEND_INCORRECT_LW_PAIR_REMOVE(req)\
  (req)->send_options |= (1 << 4)

#define ZB_ZGP_GP_PAIRING_OPTIONS_UPDATE_SEND_INCORRECT_LW_PAIR_REMOVE(req)\
  ZB_ZGP_GP_PAIRING_MAKE_SEND_OPTIONS(req, 0, 0, 0);\
  ZB_ZGP_GP_PAIRING_OPTIONS_SET_SEND_INCORRECT_LW_PAIR_REMOVE(req)

#define ZB_ZGP_GP_PAIRING_SEND_REQ_CREATE(buf, req, ent, cb) \
  zb_buf_reuse((buf));\
  (req) = ZB_BUF_GET_PARAM((buf), zb_zgp_gp_pairing_send_req_t);\
  (req)->zgpd_id = (ent)->zgpd_id;\
  (req)->endpoint = (ent)->endpoint;\
  (req)->options = (ent)->options;\
  (req)->sec_options = (ent)->sec_options;\
  (req)->security_counter = (ent)->security_counter;\
  ZB_MEMCPY((req)->zgpd_key, (ent)->zgpd_key, sizeof((ent)->zgpd_key));\
  (req)->zgpd_assigned_alias = (ent)->zgpd_assigned_alias;\
  (req)->groupcast_radius = (ent)->groupcast_radius;\
  (req)->device_id = (ent)->u.sink.device_id;\
  ZB_MEMCPY((req)->sgrp, (ent)->u.sink.sgrp, sizeof((ent)->u.sink.sgrp)); \
  (req)->callback = (cb)

#define ZB_ZGP_GP_PCONF_PAIRING_SEND_REQ_CREATE(buf, req, conf, cb)\
  zb_buf_reuse((buf));                                             \
  (req) = ZB_BUF_GET_PARAM((buf), zb_zgp_gp_pairing_send_req_t);   \
  (req)->zgpd_id = (conf)->zgpd_addr;                              \
  (req)->endpoint = (conf)->endpoint;                              \
  (req)->options = (conf)->options;                                \
  (req)->sec_options = (conf)->u.action_flds.sec_options;                        \
  (req)->security_counter = (conf)->u.action_flds.sec_frame_counter;             \
  ZB_MEMCPY((req)->zgpd_key, (conf)->u.action_flds.key, sizeof((conf)->u.action_flds.key));    \
  (req)->zgpd_assigned_alias = (conf)->u.action_flds.assigned_alias;             \
  (req)->groupcast_radius = (conf)->frwd_radius;                   \
  (req)->device_id = (conf)->device_id;                            \
  ZB_MEMCPY(&(req)->sgrp[0], &(conf)->u.action_flds.sgrp[0], sizeof((conf)->u.action_flds.sgrp)); \
  (req)->callback = (cb)

#define ZGP_PAIRING_SEND_GET_ADD_SINK(req) ((req)->send_options & 1)
#define ZGP_PAIRING_SEND_GET_REMOVE_GPD(req) (((req)->send_options >> 1) & 1)
#define ZGP_PAIRING_SEND_GET_SEND_DEV_ANNCE(req) (((req)->send_options >> 2) & 1)
#define ZGP_PAIRING_SEND_GET_DUP_COMPLETE(req) (((req)->send_options >> 3) & 1)
#define ZGP_PAIRING_SEND_GET_SEND_INCORRECT_LW_PAIR_REMOVE(req) (((req)->send_options >> 4) & 1)
#define ZGP_PAIRING_SEND_CLR_SEND_INCORRECT_LW_PAIR_REMOVE(req) ((req)->send_options &= ~(1 << 4))
#define ZGP_PAIRING_SEND_GET_ASSIGNED_ALIAS(req) (((req)->options >> 8) & 1)
#define ZGP_PAIRING_SEND_GET_COMMUNICATION_MODE(req) (ZGP_PAIRING_SEND_GET_SEND_INCORRECT_LW_PAIR_REMOVE(req) ?\
                                                      ZGP_COMMUNICATION_MODE_LIGHTWEIGHT_UNICAST : (((req)->options >> 3) & 3))
#define ZGP_PAIRING_SEND_GET_APP_ID(req) ((req)->options & 7)
#define ZGP_PAIRING_SEND_GET_SEC_LEVEL(req) (((req)->options & (1<<9)) ? ZGP_SINK_GET_SEC_LEVEL((req)->sec_options): 0)
#define ZGP_PAIRING_SEND_GET_FIXED_LOCATION(req) (((req)->options >> 7) & 1)
#define ZGP_PAIRING_SEND_GET_SEQ_NUM_CAP(req) (((req)->options >> 5) & 1)
#define ZGP_PAIRING_SEND_GET_SEC_KEY_TYPE(req) (((req)->options & (1<<9)) ? (((req)->sec_options >> 2) & 7) : 0)

typedef ZB_PACKED_PRE struct zb_zgp_gp_notification_req_s
{
  zb_uint16_t            options;
  zb_zgpd_addr_t         zgpd_addr;
  zb_uint8_t             endpoint;
  zb_uint32_t            gpd_sec_frame_counter;
  zb_uint8_t             gpd_cmd_id;
  /* +1 bytes for payload size placed at beginning buffer */
  zb_uint8_t             payload[MAX_ZGP_CLUSTER_GPDF_PAYLOAD_SIZE+1];
  zb_zgp_gp_proxy_info_t proxy_info;
}
ZB_PACKED_STRUCT zb_zgp_gp_notification_req_t;

typedef ZB_PACKED_PRE struct zb_zgp_gp_response_s
{
  zb_uint8_t     options;
  zb_uint16_t    temp_master_addr;
  zb_uint8_t     temp_master_tx_chnl;
  zb_zgpd_addr_t zgpd_addr;
  zb_uint8_t     endpoint;
  zb_uint8_t     gpd_cmd_id;
  /* +1 bytes for payload size placed at beginning buffer */
  zb_uint8_t     payload[MAX_ZGP_CLUSTER_GPDF_PAYLOAD_SIZE+1];
}
ZB_PACKED_STRUCT zb_zgp_gp_response_t;

enum zb_zgp_pairing_conf_actions_e
{
  ZGP_PAIRING_CONF_NO_ACTION        = 0,
  ZGP_PAIRING_CONF_EXTEND           = 1,
  ZGP_PAIRING_CONF_REPLACE          = 2,
  ZGP_PAIRING_CONF_REMOVE_PAIRING   = 3,
  ZGP_PAIRING_CONF_REMOVE_GPD       = 4,
  ZGP_PAIRING_CONF_APP_DESCRIPTION  = 5,
  ZGP_PAIRING_CONF_REMOVE_RESERVED6 = 6,
  ZGP_PAIRING_CONF_REMOVE_RESERVED7 = 7
};

enum zb_zgp_pairing_conf_actions_send_pairing_e
{
  ZGP_PAIRING_CONF_NO_SEND_PAIRING,
  ZGP_PAIRING_CONF_SEND_PAIRING
};


typedef ZB_PACKED_PRE struct zb_zgp_gp_pairing_conf_s
{
  zb_uint8_t     actions;
  zb_uint16_t    options;
  zb_zgpd_addr_t zgpd_addr;
  zb_uint8_t     endpoint;
  zb_uint8_t     device_id;
  zb_uint8_t     frwd_radius;
  zb_uint8_t     num_paired_endpoints;
  zb_uint8_t     paired_endpoints[ZB_ZGP_MAX_PAIRED_ENDPOINTS];
  zb_uint8_t     app_info;

  union
  {
    /* A.3.3.4.6.7, Table 35
     * Fields for GP Pairing Configuration with action = 0b101.
     * They need to be put separately since most fields common for
     * other actions are forbidden for this one.
     */
    struct app_descr_flds_s
    {
      zb_uint8_t total_num_of_reports;
      zb_uint8_t num_of_reports;
      zgp_report_desc_pairing_config_t reports[ZB_ZGP_APP_DESCR_REPORTS_NUM];
    } app_descr;

    /* A.3.3.4.6.7, Table 35
     * Fields for GP Pairing Configuration with actions 0b000-0b100
     */
    struct actions_flds_s
    {
      zgp_pair_group_list_t sgrp[ZB_ZGP_MAX_SINK_GROUP_PER_GPD];
      zb_uint16_t    assigned_alias;
      zb_uint8_t     sec_options;
      zb_uint32_t    sec_frame_counter;
      zb_uint8_t     key[ZB_CCM_KEY_SIZE];
      zb_uint16_t    manuf_id;
      zb_uint16_t    model_id;
      zb_zgp_gpd_cmds_list_t gpd_cmds_list;
      zb_zgp_cluster_list_t cl;
      zb_gpdf_comm_switch_info_t switch_info;
    } action_flds;

  } u;
}
ZB_PACKED_STRUCT zb_zgp_gp_pairing_conf_t;

typedef ZB_PACKED_PRE struct zb_zgp_gp_sink_comm_mode_s
{
  zb_uint8_t     options;
  zb_uint16_t    gpm_addr_for_sec;
  zb_uint16_t    gpm_addr_for_pair;
  zb_uint8_t     sink_endpoint;
}
ZB_PACKED_STRUCT zb_zgp_gp_sink_comm_mode_t;
#endif  /* !ZB_ZGPD_ROLE */

typedef ZB_PACKED_PRE struct zb_zgp_aes_nonce_s
{
  ZB_PACKED_PRE union zb_zgp_nonce_source_addr_u
  {
    zb_64bit_addr_t  ieee_addr;
    zb_uint32_t      splitted_addr[2];
  } src_addr;

  zb_uint32_t       frame_counter;
  zb_uint8_t        security_control;
}
ZB_PACKED_STRUCT zb_zgp_aes_nonce_t;

#ifdef ZB_TRACE_LEVEL
/**
 * @brief dump @ref zb_gpdf_info_t structure into trace log
 */
void zb_zgp_dump_gpdf_info(zb_gpdf_info_t *gpdf);
#define ZB_DUMP_GPDF_INFO(gpdf) zb_zgp_dump_gpdf_info(gpdf)
/**
 * @brief dump @ref zb_zgpd_id_t structure into trace log
 */
void zb_zgp_dump_zgpd_id(zb_zgpd_id_t *id);
#define ZB_DUMP_ZGPD_ID(id) zb_zgp_dump_zgpd_id(&(id))
#else
#define ZB_DUMP_GPDF_INFO(gpdf)
#define ZB_DUMP_ZGPD_ID(id)
#endif  /* ZB_TRACE_LEVEL */

/**
 * @brief Get duplicate filtering counter from GPDF
 *
 * ZGP spec, A.3.6.1.2:
 * If the ZGPD command used SecurityLevel 0b00, the filtering of duplicate
 * ZGPD messages is based on the MAC sequence number of a particular ZGPD,
 * identified by ZGPD SrcID. If the ZGPD command used SecurityLevel 0b01,
 * 0b10 or 0b11, then the filtering of duplicate messages is performed
 * based on the ZGPD security frame counter.
 */
#define ZB_GPDF_INFO_GET_DUP_COUNTER(gpdf_info) \
  ((ZB_GPDF_EXT_NFC_GET_SEC_LEVEL((gpdf_info)->nwk_ext_frame_ctl) == 0) ? \
                 (gpdf_info)->mac_seq_num : (gpdf_info)->sec_frame_counter)

#define ZB_ZGP_CLUSTER_SET_DUP_COUNTER(counter, gpdf_info) \
  (gpdf_info)->mac_seq_num = ((counter) & 0xFF);\
  (gpdf_info)->sec_frame_counter = (counter);

#define ZB_ZGP_UPDATE_DUP_COUNTER(gpdf_info)\
  {\
    if (ZGP_CTXC().sink_mode == ZB_ZGP_COMMISSIONING_MODE)\
    {\
      ZGP_CTXC().comm_data.comm_dup_counter = ZB_GPDF_INFO_GET_DUP_COUNTER((gpdf_info));\
    }\
    if (ZGP_CTXC().proxy_mode == ZB_ZGP_COMMISSIONING_MODE)\
    {\
      ZGP_CTXC().comm_data.comm_dup_counter = ZB_GPDF_INFO_GET_DUP_COUNTER((gpdf_info));\
    }\
    ZGP_CTXC().comm_data.any_packet_received = 1;\
  }

/**
 * @brief NWK frame type used with @ref ZB_ZGP_PROTOCOL_VERSION
 * @see ZGP spec, A.1.4.1.2
 */
enum zb_gpdf_frame_type_e
{
  ZGP_FRAME_TYPE_DATA         = 0x00, /**< Data frame */
  ZGP_FRAME_TYPE_MAINTENANCE  = 0x01, /**< Maintenance frame */
  ZGP_FRAME_TYPE_RESERVED1    = 0x02, /**< Reserved */
  ZGP_FRAME_TYPE_RESERVED2    = 0x03  /**< Reserved */
};


/**
 * @brief Possible values of direction sub-field in
 *        GPDF Extended NWK frame control field
 * @see ZGP spec, A.1.4.1.3
 */
enum zb_gpdf_frame_dir_e
{
  ZGP_FRAME_DIR_FROM_ZGPD  = 0x00,  /**< GPDF is transmitted by ZGPD */
  ZGP_FRAME_DIR_TO_ZGPD    = 0x01   /**< GPDF is transmitted to ZGPD */
};

/********************************************************************/
/************ Sizes of different GPDF variable fields ***************/
/********************************************************************/
/**
 * @brief Extended NWK frame control size based on ZGPD context
 *
 * ZGP spec, A.1.4.1.3 (about Extended NWK frame control presence):
 * It shall be present if the ApplicationID different than 0b000...
 * For ApplicationID 0b000 (ZGP), the Extended NWK Frame Control field
 * shall be present if the GPDF is protected, if RxAfterTx is set, or
 * if the GPDF is sent to the ZGPD.
 */
#define ZGPD_EXTENDED_NWK_FRAME_CTL_SIZE(frame_type, _sec_level) \
  (((frame_type == ZGP_FRAME_TYPE_DATA) && \
   ((ZGPD->id.app_id != ZB_ZGP_APP_ID_0000) || \
   (ZGPD->commissioning_method == ZB_ZGPD_COMMISSIONING_BIDIR) || \
   ((_sec_level) > 0) || ZGPD->ext_nwk_present)) \
   ? 1 : 0)

/**
 * @brief GPDF SrcID field size
 *
 * ZGP spec, A.1.4.1.4:
 * The ZGPDSrcID field is present if the FrameType sub-field is set to 0b00
 * and the ApplicationID sub-field of the Extended NWK Frame Control field
 * is set to 0b000 (or not present)
 */
#define ZGPD_SRC_ID_SIZE(app_id, frame_type) \
  (((app_id == ZB_ZGP_APP_ID_0000) && (frame_type == ZGP_FRAME_TYPE_DATA)) ? 4 : 0)

/* ZGP spec, 1.4.1.3:
 *
 * If the SecurityLevel is set to 0b00, the fields Security frame counter
 * and MIC are not present. ...
 * If the SecurityLevel is set to 0b01, the Security Frame counter field is
 * not present and the MIC field is present, has the length of 2B ...
 * If the SecurityLevel is set to 0b10 or 0b11, the Security Frame counter field
 * is present, has the length of 4B, and carries the full 4B security frame counter,
 * the MIC field is present, has the length of 4B.
 */

/**
 * @brief Size of Security frame counter field
 */
#define GPDF_SECURITY_FRAME_COUNTER_SIZE(sec_level) \
  ((sec_level > ZB_ZGP_SEC_LEVEL_REDUCED) ? 4 : 0)

/**
 * @brief Size of MIC field
 */
#define ZB_GPDF_MIC_SIZE(sec_level) \
  ((sec_level > 0) ? ((sec_level == ZB_ZGP_SEC_LEVEL_REDUCED) ? 2 : 4) : 0)

/********************************************************************/
/************ Macros for filling GPDF fields with values ************/
/********************************************************************/

/**
 * @brief Construct GPDF NWK Frame control from given values
 */
#define ZB_GPDF_NWK_FRAME_CONTROL(frame_ctl, frame_type, auto_comm, frame_ext) \
  (frame_ctl) = (  (frame_type) \
               | (ZB_ZGP_PROTOCOL_VERSION << 2) \
               | ((auto_comm) << 6) \
               | ((frame_ext) << 7))

/**
 * @brief Construct GPDF Extended NWK Frame control for ZGPD outgoing frame
 */
#define ZB_GPDF_NWK_FRAME_CTL_EXT(ext_frame_ctl, app_id, sec_level, sec_key, rx_after_tx, dir) \
  ext_frame_ctl = (  (app_id) \
                   | ((sec_level) << 3) \
                   | (!!(sec_key) << 5) \
                   | (!!(rx_after_tx) << 6) \
                   | (!!(dir) << 7))

/********************************************************************/
/********** Get/set macros for individual bit sub-fields ************/
/********************************************************************/

#define ZB_GPDF_NFC_GET_NFC_EXT(frame_ctl) (((frame_ctl) & 0x80) >> 7)

#define ZB_GPDF_NFC_GET_FRAME_TYPE(frame_ctl) ((frame_ctl) & 0x03)

#define ZB_GPDF_NFC_SET_FRAME_TYPE(frame_ctl, frame_type) ((frame_ctl) |= ((frame_type) & 0x03))

#define ZB_GPDF_NFC_GET_AUTO_COMMISSIONING(frame_ctl) (((frame_ctl) >> 6) & 0x01)

#define ZB_GPDF_NFC_SET_AUTO_COMMISSIONING(frame_ctl, auto_comm) ((frame_ctl) |= ((!!(auto_comm)) << 6))

#define ZB_GPDF_EXT_NFC_GET_APP_ID(ext_fc) ((ext_fc) & 0x03)

#define ZB_GPDF_EXT_NFC_GET_SEC_LEVEL(ext_fc) (((ext_fc) >> 3) & 0x03)

#define ZB_GPDF_EXT_NFC_SET_SEC_LEVEL(ext_fc, lvl) ((ext_fc) |= (((lvl) & 0x03) << 3))

#define ZB_GPDF_EXT_NFC_GET_SEC_KEY(ext_fc) (((ext_fc) >> 5) & 0x01)

#define ZB_GPDF_EXT_NFC_SET_SEC_KEY(ext_fc, sec_key) ((ext_fc) |= ((!!(sec_key)) << 5))

#define ZB_GPDF_EXT_NFC_GET_RX_AFTER_TX(ext_fc) (((ext_fc) >> 6) & 0x01)

#define ZB_GPDF_EXT_NFC_SET_RX_AFTER_TX(ext_fc, rx_after_tx) ((ext_fc) |= ((!!(rx_after_tx))<<6))

#define ZB_GPDF_EXT_NFC_CLR_RX_AFTER_TX(ext_fc) (ext_fc) &= ~(1<<6)

#define ZB_GPDF_EXT_NFC_GET_DIRECTION(ext_fc) (((ext_fc) & 0x80) >> 7)

#define ZB_GPDF_EXT_NFC_SET_DIRECTION(ext_fc, dir) ((ext_fc) |= ((!!(dir))<<7))

#ifndef ZB_ZGPD_ROLE
/**
 * @brief Handle commissioning mode request
 *
 * @param param      [in]  Buffer reference
 * @param options    [in]  Request options
 * @param comm_wind  [in]  Commissioning window
 * @param channel    [in]  Channel for perform commissioning
 *
 * @see ZGP spec, A.3.3.5.3
 */
void zgp_proxy_handle_commissioning_mode(zb_uint8_t  param,
                                         zb_uint8_t  options,
                                         zb_uint16_t comm_wind,
                                         zb_uint8_t  channel);

void zb_dgp_data_ind(zb_uint8_t param);

#ifdef ZB_ENABLE_ZGP_SINK
/**
 * @brief Handle commissioning notification request
 *
 * @param param      [in]  Buffer reference
 *
 * @see ZGP spec, A.3.3.4.3
 */
void zgp_sink_handle_gp_comm_notification_req(zb_uint8_t param);
/**
 * @brief Handle notification request
 *
 * @param param      [in]  Buffer reference
 *
 * @see ZGP spec, A.3.3.4.1
 */
void zgp_sink_handle_gp_notification_req(zb_uint8_t param);
/**
 * @brief Handle gp pairing configuration request
 *
 * @param param      [in]  Buffer reference
 *
 * @see ZGP spec, A.3.3.4.6
 */
void zgp_sink_handle_gp_pairing_configuration(zb_uint8_t param);
/**
 * @brief Accept/reject gp pairing configuration
 *
 * @param accept     [in]  ZB_TRUE  - accepted
 *                         ZB_FALSE - rejected
 */
void zgp_sink_accept_gp_pairing_configuration(zb_bool_t accept);
/**
 * @brief Handle gp sink commissioning mode request
 *
 * @param param      [in]  Buffer reference
 *
 * @see ZGP spec, A.3.3.4.8
 */
void zgp_sink_handle_gp_sink_commissioning_mode(zb_uint8_t param);
#endif  /* ZB_ENABLE_ZGP_SINK */

#ifdef ZB_ENABLE_ZGP_PROXY
/**
 * @brief Perform paring request
 *
 * @param param  [in]  Buffer reference
 * @param req    [in]  Pointer to the GP Paring request information
 *
 * @see ZGP spec, A.3.3.5.2
 */
void zgp_proxy_handle_gp_pairing_req(zb_uint8_t param, zb_zgp_gp_pairing_req_t *req);

/**
 * @brief Handle gp response
 *
 * @param param      [in]  Buffer reference
 *
 * @see ZGP spec, A.3.3.5.4
 *
 */
void zgp_proxy_handle_gp_response(zb_uint8_t param);

/**
 * @brief Perform GP Commissioning Notification request
 *
 * @param param          [in]  Buffer reference
 * @param use_alias      [in]  Using aliases in the broadcast forwarding
 * @param alias_addr     [in]  Alias for the broadcast forwarding
 * @param alias_seq      [in]  Alias sequence number for the broadcast forwarding
 * @param dst_addr       [in]  Destination address
 * @param dst_addr_mode  [in]  Destination address mode
 * @param options        [in]  Request options
 * @param cb             [in]  Callback call if needed after send request
 *
 * @see ZGP spec, A.3.3.4.3
 */
void zb_zgp_cluster_gp_comm_notification_req(zb_uint8_t    param,
                                             zb_uint8_t    use_alias,
                                             zb_uint16_t   alias_addr,
                                             zb_uint8_t    alias_seq,
                                             zb_uint16_t   dst_addr,
                                             zb_uint8_t    dst_addr_mode,
                                             zb_uint16_t   options,
                                             zb_callback_t cb);

/**
 * @brief Perform GP Notification request
 *
 * @param param          [in]  Buffer reference
 * @param use_alias      [in]  Using aliases in the groupcast forwarding
 * @param alias_addr     [in]  Alias for the groupcast forwarding
 * @param alias_seq      [in]  Alias sequence number for the groupcast forwarding
 * @param dst_addr       [in]  Destination address for the unicast forwarding
 * @param dst_addr_mode  [in]  Destination address mode for the unicast forwarding
 * @param options        [in]  Request options
 * @param cb             [in]  Callback call if needed after send request
 *
 * @see ZGP spec, A.3.3.4.1
 */
void zb_zgp_cluster_gp_notification_req(zb_uint8_t    param,
                                        zb_uint8_t    use_alias,
                                        zb_uint16_t   alias_src_addr,
                                        zb_uint8_t    alias_seq,
                                        zb_uint16_t   dst_addr,
                                        zb_uint8_t    dst_addr_mode,
                                        zb_uint16_t   options,
                                        zb_uint8_t    groupcast_radius,
                                        zb_callback_t cb);

zb_bool_t zgp_proxy_is_support_functionality(zgp_gpp_functionality_t gpp_f);
#define ZB_ZGP_PROXY_IS_SUPPORT_FUNCTIONALITY(f) zgp_proxy_is_support_functionality(f)

/**
 * @brief Calculate count of nonempty lightweight addresses in lightweight address list
 *
 * @param ent [in]  Pointer to proxy table entry
 *
 * @return Count of nonempty lightweight addresses
 */
zb_uint8_t zgp_proxy_get_lwsink_addr_list_size(zgp_tbl_ent_t *ent);
#endif  /* ZB_ENABLE_ZGP_PROXY */

/**
 * @brief Perform GP Response request
 *
 * @param param          [in]  Buffer reference
 * @param dst_addr       [in]  Destination address
 * @param dst_addr_mode  [in]  Destination address mode
 * @param cb             [in]  Callback call if needed after send request
 *
 * @see ZGP spec, A.3.3.5.4
 */
void zb_zgp_cluster_gp_response_send(zb_uint8_t    param,
                                            zb_uint16_t            dst_addr,
                                            zb_uint8_t             dst_addr_mode,
                                            zb_callback_t          cb);

#ifdef ZB_ENABLE_ZGP_SINK
zb_bool_t zgp_sink_is_support_functionality(zgp_gps_functionality_t gps_f);
zb_bool_t zgp_sink_is_support_communication_mode(zb_uint8_t cm);
#define ZB_ZGP_SINK_IS_SUPPORT_FUNCTIONALITY(f) zgp_sink_is_support_functionality(f)
#define ZB_ZGP_SINK_IS_SUPPORT_COMMUNICATION_MODE(cm) zgp_sink_is_support_communication_mode(cm)
void zb_zgps_clear_temp_master_list_ctx(void);
void zb_zgps_unbind_aps_group_for_aliasing(zb_zgp_sink_tbl_ent_t *ent);
#endif  /* ZB_ENABLE_ZGP_SINK */

#ifdef ZB_ENABLE_ZGP_PROXY
void zb_gp_proxy_data_indication(zb_uint8_t param);
void zb_gp_proxy_mlme_get_cfm_cb(zb_uint8_t param);
void zb_gp_proxy_mlme_set_cfm_cb(zb_uint8_t param);
#endif  /* ZB_ENABLE_ZGP_PROXY */
#ifdef ZB_ENABLE_ZGP_SINK
void zb_gp_sink_data_indication(zb_uint8_t param);
void zb_gp_sink_mlme_get_cfm_cb(zb_uint8_t param);
void zb_gp_sink_mlme_set_cfm_cb(zb_uint8_t param);
#endif  /* ZB_ENABLE_ZGP_SINK */

/**
    A.3.6.3.3.1   Derivation of alias source address
*/
zb_uint16_t zgp_calc_alias_source_address(zb_zgpd_id_t *zgpd_id);

/**
 * @brief Calculate count of nonempty groupcast addresses in group address list
 *
 * @param ent [in]  Pointer to proxy table entry
 *
 * @return Count of nonempty groupcast addresses
 */
zb_uint8_t zgp_get_group_list_size(zgp_pair_group_list_t *group_list);

#if defined ZB_ENABLE_ZGP_SINK || defined ZGP_COMMISSIONING_TOOL
void zgp_cluster_send_gp_pairing(zb_uint8_t param);
#endif  /* defined ZB_ENABLE_ZGP_SINK || defined ZGP_COMMISSIONING_TOOL */

#ifdef ZB_ALIEN_ZGP_STUB

void zgp_alien_stub_table_entry_add(zgp_tbl_ent_t *ent);
#define ALIEN_STUB_TBL_ENTRY_ADD(ent) zgp_alien_stub_table_entry_add(ent)

void zgp_alien_stub_table_entry_remove(zgp_tbl_ent_t *ent);
#define ALIEN_STUB_TBL_ENTRY_REMOVE(ent) zgp_alien_stub_table_entry_remove(ent)

void zgp_alien_stub_table_remove_all_entries(zb_uint8_t unused);
#define ALIEN_STUB_TBL_REMOVE_ALL_ENTRIES() zgp_alien_stub_table_remove_all_entries(0)

#else  /* ZB_ALIEN_ZGP_STUB */

#define ALIEN_STUB_TBL_ENTRY_ADD(ent)
#define ALIEN_STUB_TBL_ENTRY_REMOVE(ent)
#define ALIEN_STUB_TBL_REMOVE_ALL_ENTRIES(tbl)

#endif  /* ZB_ALIEN_ZGP_STUB */

zb_uint8_t zb_zgp_try_bidir_tx(zb_uint8_t param);
zb_ret_t zgp_key_recovery(zgp_tbl_ent_t *ent, zb_bool_t individual, zb_uint8_t *key, zb_uint8_t *key_type);

void zb_zgp_write_dataset(zb_uint8_t param);

#define ZB_ZGP_ENT_ENUMERATE_CTX_START_IDX   0xff

typedef struct zb_zgp_ent_enumerate_ctx_s
{
  zb_uint8_t idx;
  zb_uint8_t entries_count;
} zb_zgp_ent_enumerate_ctx_t;

zb_ret_t zgp_any_table_read(zb_zgpd_id_t *zgpd_id, zgp_tbl_ent_t *ent);
zb_ret_t zgp_proxy_table_enumerate(zb_zgp_ent_enumerate_ctx_t *ctx, zgp_tbl_ent_t *ent);
zb_ret_t zgp_sink_table_enumerate(zb_zgp_ent_enumerate_ctx_t *ctx, zgp_tbl_ent_t *ent);

#ifdef ZB_ENABLE_ZGP_DIRECT
/**
   Check that ZBOSS skips all inclosing GPDFS

   @return ZB_TRUE if incoming GPDFS are skipped.
*/
#endif /* ZB_ENABLE_ZGP_DIRECT */
#endif  /* !ZB_ZGPD_ROLE */

zb_uint8_t zgp_parse_gpdf_nwk_hdr(zb_uint8_t *gpdf, zb_uint8_t gpdf_len, zb_gpdf_info_t *gpdf_info);

void zgp_init_by_scheduler(zb_uint8_t param);

void zgp_set_link_key(zb_uint8_t *key);

/**
 * Disable GP Functionality. Should be called before zgp_init_by_scheduler().
 *
 * @return RET_OK, if GP was disabled successfully, RET_ERROR otherwise
 *         (the only case - zgp_disabled() is called after zgp_init_by_scheduler())
 */
zb_ret_t zgp_disable(void);

/**
 * @brief Convert GPDF packet to ZCL packet (packets)
 *
 * @param buf_ref [in] Reference to buffer with GPDF packet.
 *                   Buffer parameter contains filled @ref zb_gpdf_info_t struct
 */
void zb_zgp_gpdf_to_zcl(zb_bufid_t buf_ref);

zb_ret_t zb_zgp_get_next_point_descr(zb_uint8_t **rpos, zb_uint8_t *max_pos, zgp_data_point_desc_t *point_desc);

/**
 * @brief ZGP Command type related to mapping table iterating
 *
 */
typedef enum zb_zgp_command_type_e
{
  ZGP_COMMAND_TYPE_NEW = 0,       /**< Command type is unknown at the moment, used for first iteration */
  ZGP_COMMAND_TYPE_UNDEFINED,     /**< GPD Command ID wasn't found, using 0xFF for further translations */
  ZGP_COMMAND_TYPE_REGULAR        /**< GPD Command ID was found, using actual code for further translations */
} zb_zgp_command_type_t;


/**
 * @brief Mapping table iteration auxiliary struct
 *
 */
typedef ZB_PACKED_PRE struct zb_zgp_mapping_table_iterator_s
{
  zb_uint16_t   index;  /**< Mapping table index for next iteration*/
  zb_uint8_t    command_type; /** Command type @ref zb_zgp_command_type_t */
} ZB_PACKED_STRUCT zb_zgp_mapping_table_iterator_t;

/**
 * @brief Set new value of mapping table iterator
 *
 */
#define ZGP_MAPPING_TABLE_SET_ITERATOR(iterator, index_, type) \
  (iterator)->index = (index_); \
  (iterator)->command_type = (type);

#endif  /* #ifdef ZB_ENABLE_ZGP */

/* ZGP InvolveTC (A.3.3.2.6 gpsSecurityLevel attribute) */
void zb_zgp_notification_network_join_begins(zb_bool_t ic_based_join_type);
void zb_zgp_notification_network_join_done(void);

/*ZGP functionality setting */
void zb_zgp_set_proxy_functionality(zb_uint32_t functionality, zb_uint32_t active_functionality);
void zb_zgp_get_proxy_functionality(zb_uint32_t *functionality, zb_uint32_t *active_functionality);
void zb_zgp_set_sink_functionality(zb_uint32_t functionality, zb_uint32_t active_functionality);
void zb_zgp_get_sink_functionality(zb_uint32_t *functionality, zb_uint32_t *active_functionality);
#endif /* ZGP_INTERNAL_H */
