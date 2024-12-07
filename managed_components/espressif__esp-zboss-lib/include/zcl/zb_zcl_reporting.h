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
/* PURPOSE: Zigbee cluster library definitions for reporting functionality
*/

#ifndef ZB_ZCL_REPORTING_H
#define ZB_ZCL_REPORTING_H 1

#if !(defined ZB_ZCL_DISABLE_REPORTING) || defined(DOXYGEN)

/** @cond DOXYGEN_ZCL_SECTION */

/**
 *  @addtogroup ZB_ZCL_REPORTING
 *  @{
 *    @details
 *    ZCL attribute reporting being described in ZCL spec, subclauses 2.4.7 through 2.4.11, and
 *    subclauses covering clusters with reportable attributes.
 *
 *    API for attribute reporting configuring and configuration details reading documented in: @ref
 *    cfg_reporting_cmd, and @ref report_attr_cmd respectively.
 *
 *    Since all events related to attribute reporting are being handled internally, application can
 *    be provided with information on attribute report receipt event, and attribute report not
 *    received within predefined interval event. To handle these events, application may set
 *    "report attribute" and "no reporting" callbacks with @ref ZB_ZCL_SET_REPORT_ATTR_CB() and
 *    @ref ZB_ZCL_SET_NO_REPORTING_CB() macros respectively.
 *
 *    @par Example
 *    @snippet light_sample/dimmable_light/bulb.c zb_zcl_set_report_attr_cb
 *    @snippet light_sample/dimmable_light/bulb.c zb_zcl_set_report_attr_cb_example
 *    @par
 */

/* Boundary and default values are defined by application profile */

/*! Default value for minimum reporting interval */
#define ZB_ZCL_MIN_REPORTING_INTERVAL_DEFAULT 0x0005

/*! Default value for maximum reporting interval */
#define ZB_ZCL_MAX_REPORTING_INTERVAL_DEFAULT 0x0000

union zb_zcl_attr_var_u
{
  zb_uint8_t u8;
  zb_int8_t s8;
  zb_uint16_t u16;
  zb_int16_t s16;
  zb_uint24_t u24;
  zb_int24_t s24;
  zb_uint32_t u32;
  zb_int32_t s32;
  zb_uint48_t u48;
  zb_uint8_t data_buf[4];
  zb_uint32_t data_buf_crc32;
};

/** @cond internals_doc */

/** @brief Structure for storing Reporting information */
typedef struct zb_zcl_reporting_info_s
{
  zb_uint8_t direction;   /*!< Direction: report is send or received */
  zb_uint8_t ep;          /*!< Endpoint number */
  zb_uint16_t cluster_id; /*!< Cluster ID for reporting */
  zb_uint8_t cluster_role;
  zb_uint16_t attr_id;    /*!< Attribute ID for reporting */
  zb_uint8_t flags;       /*!< Flags to inform status of reporting */
  zb_time_t run_time;     /*!< Time to run next reporting activity */
  union
  {
    struct
    {
      zb_uint16_t min_interval; /*!< Actual minimum reporting interval */
      zb_uint16_t max_interval; /*!< Actual maximum reporting interval */
      union zb_zcl_attr_var_u delta; /*!< Actual reportable change */
      union zb_zcl_attr_var_u reported_value; /*!< The last reported value */
      zb_uint16_t def_min_interval; /*!< Default minimum reporting interval */
      zb_uint16_t def_max_interval; /*!< Default maximum reporting interval */
    }
    send_info;  /*!< send_info is stored on cluster server side (as usual) and describes how
                  attribute should be reported */
    struct
    {
      zb_uint16_t timeout; /*!< Timeout period */
    }
    recv_info; /*!< recv_info is stored on cluster client side (as usual) and describes how
                 attribute report is received */
  }
  u;

  struct
  {
    zb_uint16_t short_addr; /*!< Destination short address */
    zb_uint8_t  endpoint;   /*!< Destination endpoint */
    zb_uint16_t profile_id; /*!< Profile id */
  }
  dst;

  zb_uint16_t manuf_code; /*!< Manufacturer specific code */
}
zb_zcl_reporting_info_t;

#if defined ZB_USE_NVRAM

/* FIXME: Dataset is not aligned (21 bytes)! Fix align and use u48 instead of u32 for delta! */

/** @brief Structure for storing Reporting information to NVRAM */
typedef ZB_PACKED_PRE struct zb_zcl_reporting_info_nvram_s
{
  zb_uint8_t direction;   /*!< Direction: report is send or received */
  zb_uint8_t ep;          /*!< Endpoint number */
  zb_uint16_t cluster_id; /*!< Cluster ID for reporting */
  zb_uint8_t cluster_role; /*!< Cluster role */
  zb_uint16_t attr_id;    /*!< Attribute ID for reporting */
  zb_uint16_t manuf_code;      /*!< Manufacturer specific code */
  zb_uint8_t flags;       /*!< Flags to inform status of reporting */

  zb_uint16_t min_interval; /*!< Minimum reporting interval or timeout for client*/
  zb_uint16_t max_interval; /*!< Maximum reporting interval */
  /* FIXME: Should be U48 here! Leave it as is to do not perform migration! U32 delta is enough for
   * the most cases... */
  zb_uint32_t delta; /*!< Reportable change */

  zb_uint16_t short_addr_dest; /*!< Destination short address */
  zb_uint8_t  endpoint_dest;   /*!< Destination endpoint */
  zb_uint16_t profile_id_dest; /*!< Profile id */
  zb_uint8_t align[1];
} ZB_PACKED_STRUCT zb_zcl_reporting_info_nvram_t;

#endif /* defined ZB_USE_NVRAM */

typedef struct zb_zcl_reporting_ctx_s
{
  zb_uint16_t min_interval;
  zb_uint16_t max_interval;
  zb_uint16_t timeout;

  zb_uint8_t buf_ref;
}
zb_zcl_reporting_ctx_t;

/** @endcond */ /* internals_doc */

/** @brief Informs application that some attribute value was not reported during defined time
    interval
    @param ep - endpoint number
    @param cluster_id - cluster ID
    @param attr_id - attribute ID
*/
typedef void (*zb_zcl_no_reporting_cb_t)(
    zb_uint8_t ep, zb_uint16_t cluster_id, zb_uint16_t attr_id);

/** @brief Informs application about received attribute report
    @param addr - source address
    @param src_ep - source endpoint number
    @param dst_ep - destination endpoint number
    @param cluster_id - cluster ID
    @param attr_id - attribute ID
    @param attr_type - attribute type
    @param value - pointer to reported data value
*/

typedef void (*zb_zcl_report_attr_cb_t)(
    zb_zcl_addr_t *addr, zb_uint8_t src_ep, zb_uint8_t dst_ep, zb_uint16_t cluster_id, zb_uint16_t attr_id, zb_uint8_t attr_type, zb_uint8_t *value);
/** @cond internals_doc */

/** Enumeration of reporting info flag values */
typedef enum zb_zcl_reporting_flags_e
{
  ZB_ZCL_REPORTING_SLOT_BUSY = 0x01,      /*!< Reporting info slot is busy */
  ZB_ZCL_REPORT_ATTR         = 0x01 << 1, /*!< Current attribute should be reported */
  ZB_ZCL_REPORT_IS_ALLOWED   = 0x01 << 2, /*!< Reporting for attribute is allowed if
                                           * min_interval timeout condition is met */
  ZB_ZCL_REPORTING_STOP      = 0x01 << 3, /*!< Stop reporting this attribute */
  ZB_ZCL_REPORT_TIMER_STARTED = 0x01 << 4,
  ZB_ZCL_REPORT_IS_SENT      = 0x01 << 5,
  ZB_ZCL_REPORT_IS_FIRST     = 0x01 << 6, /*!< First report for this attribute */
}
zb_zcl_reporting_flags_t;

#define ZB_ZCL_SET_REPORTING_FLAG(rep_info, f) ((rep_info)->flags |= (zb_uint8_t)(f))

#define ZB_ZCL_CLR_REPORTING_FLAG(rep_info, f) ((rep_info)->flags &= (zb_uint8_t)(f^0xFF))

#define ZB_ZCL_GET_REPORTING_FLAG(rep_info, f) ((rep_info)->flags & (zb_uint8_t)(f))

#define ZB_ZCL_CLR_ALL_REPORTING_FLAGS(rep_info) ((rep_info)->flags = 0)


/*! @internal min_interval zero value */
#define ZB_ZCL_MIN_INTERVAL_ZERO 0x0000

/*! @internal Check, if min_interval should be taken into account */
#define ZB_ZCL_MIN_INTERVAL_SPECIFIED                                \
  (ZCL_CTX().reporting_ctx.min_interval != ZB_ZCL_MIN_INTERVAL_ZERO)

/** @endcond */ /* internals_doc */

/** @brief If Maximum reporting interval is set to value 0xFFFF,reporting is not needed for current
    attribute
*/
#define ZB_ZCL_REPORTING_NOT_NEEDED 0xFFFF

/** @cond internals_doc */

#define ZB_ZCL_TIMEOUT_ZERO 0x0000

#define ZB_ZCL_INVALID_TIMEOUT 0xFFFF

#define ZB_ZCL_UNDEFINED_REPORTING_SLOT 0xFF
#define ZB_ZCL_MAX_REPORTING_SLOTS_BY_EP (ZB_ZCL_UNDEFINED_REPORTING_SLOT / ZB_ZCL_MAX_EP_NUMBER)

/************************** Reporting functions declarations **********************************/

#ifdef ZB_ZCL_ENABLE_DEFAULT_REPORT_ATTR_PROCESSING
void zb_zcl_report_attr_cmd_handler(zb_uint8_t param);
#endif

void zb_zcl_report_received(zb_uint8_t ep, zb_uint16_t cluster_id, zb_uint8_t cluster_role, zb_uint16_t attr_id);

void zb_zcl_report_received_manuf(zb_uint8_t ep, zb_uint16_t cluster_id, zb_uint8_t cluster_role, zb_uint16_t attr_id, zb_uint16_t manuf_code);

zb_ret_t zb_zcl_put_reporting_info(zb_zcl_reporting_info_t* rep_info_ptr, zb_bool_t override);

zb_ret_t zb_zcl_put_reporting_info_from_req(zb_zcl_configure_reporting_req_t *config_rep_req,
                                            zb_zcl_attr_addr_info_t* attr_addr_info);

void zb_zcl_init_reporting_info(void);

zb_zcl_reporting_info_t* zb_zcl_find_reporting_info(  zb_uint8_t ep,
                                                      zb_uint16_t cluster_id,
                                                      zb_uint8_t cluster_role,
                                                      zb_uint16_t attr_id);

zb_zcl_reporting_info_t* zb_zcl_find_reporting_info_manuf(  zb_uint8_t ep,
                                                            zb_uint16_t cluster_id,
                                                            zb_uint8_t cluster_role,
                                                            zb_uint16_t attr_id,
                                                            zb_uint16_t manuf_code);

zb_zcl_reporting_info_t* zb_zcl_get_next_reporting_info(
  zb_zcl_reporting_info_t *rep_info_init, zb_uint8_t is_manuf_spec);

void zb_zcl_reporting_cb(zb_uint8_t param);

void zb_zcl_save_reported_value(zb_zcl_reporting_info_t *rep_info, zb_zcl_attr_t* attr_desc);

void zb_zcl_mark_attr_for_reporting(zb_uint8_t ep, zb_uint16_t cluster_id, zb_uint8_t cluster_role, zb_uint16_t attr_id);
void zb_zcl_mark_attr_for_reporting_manuf(zb_uint8_t ep, zb_uint16_t cluster_id, zb_uint8_t cluster_role, zb_uint16_t attr_id, zb_uint16_t manuf_code);

/** @endcond */ /* internals_doc */

/** @brief Start attribute reporting

    @deprecated This function will be removed in the next Major release after February 2023
                Use @ref zb_zcl_start_attr_reporting_manuf() instead

    @param ep - endpoint number
    @param cluster_id - cluster ID
    @param cluster_role - cluster role
    @param attr_id - attribute ID
    @return RET_OK if reporting is successfully started \n
            RET_DOES_NOT_EXIST if reporting can't be configured with given parameters (i.e cluster not present)
*/
zb_ret_t zb_zcl_start_attr_reporting(zb_uint8_t ep, zb_uint16_t cluster_id, zb_uint8_t cluster_role, zb_uint16_t attr_id);

/** @brief Start attribute reporting
    @param ep - endpoint number
    @param cluster_id - cluster ID
    @param cluster_role - cluster role
    @param attr_id - attribute ID
    @param manuf_code - manufacturer specific code
    @return RET_OK if reporting is successfully started \n
            RET_DOES_NOT_EXIST if reporting can't be configured with given parameters (i.e cluster not present)
*/
zb_ret_t zb_zcl_start_attr_reporting_manuf(zb_uint8_t ep, zb_uint16_t cluster_id, zb_uint8_t cluster_role, zb_uint16_t attr_id, zb_uint16_t manuf_code);

/** @brief Stop attribute reporting

    @deprecated This function will be removed in the next Major release after February 2023
                Use @ref zb_zcl_stop_attr_reporting_manuf() instead

    @param ep - endpoint number
    @param cluster_id - cluster ID
    @param cluster_role - cluster role
    @param attr_id - attribute ID
    @return RET_OK if reporting is successfully stopped \n
            RET_DOES_NOT_EXIST if reporting was not set up with given parameters
*/
zb_ret_t zb_zcl_stop_attr_reporting(zb_uint8_t ep, zb_uint16_t cluster_id, zb_uint8_t cluster_role, zb_uint16_t attr_id);

/** @brief Stop attribute reporting
    @param ep - endpoint number
    @param cluster_id - cluster ID
    @param cluster_role - cluster role
    @param attr_id - attribute ID
    @param manuf_code - manufacturer specific code
    @return RET_OK if reporting is successfully stopped \n
            RET_DOES_NOT_EXIST if reporting was not set up with given parameters
*/
zb_ret_t zb_zcl_stop_attr_reporting_manuf(zb_uint8_t ep, zb_uint16_t cluster_id, zb_uint8_t cluster_role, zb_uint16_t attr_id, zb_uint16_t manuf_code);

/** @brief Check if an attribute is configured for reporting or not

    @deprecated This function will be removed in the next Major release after February 2023
                Use @ref zcl_is_attr_reported_manuf() instead

    @param ep - endpoint number
    @param cluster_id - cluster ID
    @param cluster_role - cluster role
    @param attr_id - attribute ID
    @return ZB_TRUE if attribute is reported
*/
zb_bool_t zcl_is_attr_reported(zb_uint8_t ep, zb_uint16_t cluster_id, zb_uint8_t cluster_role, zb_uint16_t attr_id);

/** @brief Check if an attribute is configured for reporting or not
    @param ep - endpoint number
    @param cluster_id - cluster ID
    @param cluster_role - cluster role
    @param attr_id - attribute ID
    @param manuf_code - manufacturer specific code
    @return ZB_TRUE if attribute is reported
*/
zb_bool_t zcl_is_attr_reported_manuf(zb_uint8_t ep, zb_uint16_t cluster_id, zb_uint8_t cluster_role, zb_uint16_t attr_id, zb_uint16_t manuf_code);

/* TODO: Document */
zb_ret_t zb_zcl_put_reporting_info(zb_zcl_reporting_info_t* rep_info_ptr, zb_bool_t override);

/** @} */

/** @endcond */ /* DOXYGEN_ZCL_SECTION */

#endif /* !(defined ZB_ZCL_DISABLE_REPORTING) || defined(DOXYGEN) */

#endif /* ZB_ZCL_REPORTING_H */
