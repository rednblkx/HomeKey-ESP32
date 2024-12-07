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

#ifndef ZB_ZCL_TIME_H
#define ZB_ZCL_TIME_H 1

#include "zcl/zb_zcl_common.h"
#include "zcl/zb_zcl_commands.h"


/** @cond DOXYGEN_ZCL_SECTION */

/** @addtogroup ZB_ZCL_TIME ZCL Time cluster
 *  @{
 *    @details
 */

/* Cluster ZB_ZCL_CLUSTER_ID_TIME */

/**
 * @brief Set real time clock callback
 * @param[in] time - UTC time at which real time clock will be set
 * @return ZB_TRUE if real time clock was set to new value and ZB_FALSE otherwise
 *
 */
typedef zb_bool_t (*zb_zcl_time_set_real_time_clock_t)(zb_uint32_t time);

/** @cond internals_doc  */

/**
 * @brief Set user's callback that will be initialize real time clock on device by
 * specified value.
 * @param[in] cb - Callback function
 * @see zb_zcl_time_set_real_time_clock_t
 *
 */
void zb_zcl_set_real_time_clock_callback(zb_zcl_time_set_real_time_clock_t cb);

/* typedef struct */

/** This enum defines possible authoritative levels of time server */
enum time_server_authoritative_level_e
{
  ZB_ZCL_TIME_SERVER_NOT_CHOSEN = 0,
  ZB_ZCL_TIME_HAS_SYNCHRONIZED_BIT,
  ZB_ZCL_TIME_HAS_MASTER_BIT,
  ZB_ZCL_TIME_HAS_MASTER_AND_SUPERSEDING_BITS,
  ZB_ZCL_TIME_COORDINATOR_WITH_MASTER_AND_SUPERSEDING_BITS
};

/** Time synchronization payload  */
typedef struct zb_zcl_time_sync_payload_s
{
  /** Network time received from the most authoritative Time server */
  zb_uint32_t time;
  /** Source address of the most authoritative Time server */
  zb_uint16_t addr;
  /** Source endpoint of the most authoritative Time server */
  zb_uint8_t endpoint;
  /** Authoritative level of Time source server.
   * @see @ref time_server_authoritative_level_e
   */
  zb_uint8_t level;
} zb_zcl_time_sync_payload_t;

/** @endcond */ /* internals_doc */

/**
 * @brief Set user's callback that will be initialize real time clock on device by
 * specified value.
 * "If the Master bit of the @ref ZB_ZCL_ATTR_TIME_TIME_STATUS_ID "TimeStatus" attribute has a value of 0,
 * writing to @ref ZB_ZCL_ATTR_TIME_TIME_ID "Time" attribute SHALL set the real time clock to the written value,
 * otherwise it cannot be written."
 * @see ZCL spec, subclause 3.12.2.2.1
 *
 */
#define ZB_ZCL_TIME_SET_REAL_TIME_CLOCK_CB(func_ptr) (zb_zcl_set_real_time_clock_callback((func_ptr)))


#define ZB_TIME_COMPARE_AUTH_LEVEL(new_level, new_short_addr, old_level, old_short_addr)  \
        (((new_level) > (old_level)) ||                                                   \
         (((old_level) > ZB_ZCL_TIME_SERVER_NOT_CHOSEN) &&                                \
          ((new_level) == (old_level)) &&                                                 \
          ((new_short_addr) < (old_short_addr))))


/**
 * @brief Callback to call when new time server found during synchronization.
 */
typedef void (*zb_zcl_time_sync_time_server_found_cb_t)(zb_ret_t status, zb_uint32_t auth_level, zb_uint16_t short_addr,
                                                        zb_uint8_t ep, zb_uint32_t nw_time);


/**
 * @brief Start time synchronization.
 * @param endpoint endpoint for each time server synchronization shall be started.
 * @param cb callback that will be called on each successful time server discovery.
 * @details Start time synchronization process. If device doesn't have master bit set in Time Status attribute of Time Cluster
 *          then starts to search available time server in Zigbee network and tries to read status and time attributes.
 *          After time server successfully found and gathered attributes their values will be passed to application
 *          to take further actions.
 */
void zb_zcl_time_server_synchronize(zb_uint8_t endpoint, zb_zcl_time_sync_time_server_found_cb_t cb);


/**
 * @brief Handle read attribute response for time cluster.
 * @details Handles read attribute response while time synchronization process running.
 *          If time synchronization process is not started or finished there will be no processing.
 */
zb_bool_t zb_zcl_time_server_read_attr_handle(zb_uint8_t param);


/** @defgroup ZB_ZCL_TIME_ATTRS Time cluster attributes
    @{
*/

/*! @brief Time cluster attribute identifiers
    @see ZCL spec, subclause 3.12.2.2
*/
enum zb_zcl_time_attr_e
{
  /*! @brief Time attribute */
  ZB_ZCL_ATTR_TIME_TIME_ID             = 0x0000,
  /*! @brief Time Status attribute */
  ZB_ZCL_ATTR_TIME_TIME_STATUS_ID      = 0x0001,
  /*! @brief Time Zone attribute */
  ZB_ZCL_ATTR_TIME_TIME_ZONE_ID        = 0x0002,
  /*! @brief Dst Start attribute */
  ZB_ZCL_ATTR_TIME_DST_START_ID        = 0x0003,
  /*! @brief Dst End attribute */
  ZB_ZCL_ATTR_TIME_DST_END_ID          = 0x0004,
  /*! @brief Dst Shift attribute */
  ZB_ZCL_ATTR_TIME_DST_SHIFT_ID        = 0x0005,
  /*! @brief Standard Time attribute */
  ZB_ZCL_ATTR_TIME_STANDARD_TIME_ID    = 0x0006,
  /*! @brief Local Time attribute */
  ZB_ZCL_ATTR_TIME_LOCAL_TIME_ID       = 0x0007,
  /*! @brief Last Set Time attribute */
  ZB_ZCL_ATTR_TIME_LAST_SET_TIME_ID    = 0x0008,
  /*! @brief Valid Until Time attribute */
  ZB_ZCL_ATTR_TIME_VALID_UNTIL_TIME_ID = 0x0009
};

/*! @brief Permissible values for Time Status attribute,
    @see ZCL spec, subclause 3.12.2.2.2 */
enum zb_zcl_time_time_status_e
{
  /*! Master value */
  ZB_ZCL_TIME_MASTER          = 0,
  /*! Synchronized value */
  ZB_ZCL_TIME_SYNCHRONIZED    = 1,
  /*! Master Zone Dst value */
  ZB_ZCL_TIME_MASTER_ZONE_DST = 2,
  /*! Superseding value */
  ZB_ZCL_TIME_SUPERSEDING     = 3
};


/** Check if @e Master bit of @ref zb_zcl_time_attr_e::ZB_ZCL_ATTR_TIME_TIME_STATUS_ID "TimeStatus" attribute is set.
 * @param val - 8-bit that contains value of @e TimeStatus attribute
 * @return zb_uint8_t value greater then 0 if @e Master bit is set, and return 0 otherwise.
 * @see @ref zb_zcl_time_time_status_e
*/
#define ZB_ZCL_TIME_TIME_STATUS_MASTER_BIT_IS_SET(val) ((val) & (1 << ZB_ZCL_TIME_MASTER))
/** Check if @e Synchronized bit of @ref zb_zcl_time_attr_e::ZB_ZCL_ATTR_TIME_TIME_STATUS_ID "TimeStatus" attribute is set.
 * @param val - 8-bit that contains value of @e TimeStatus attribute
 * @return zb_uint8_t value greater then 0 if @e Synchronized bit is set, and return 0 otherwise.
 * @see @ref zb_zcl_time_time_status_e
*/
#define ZB_ZCL_TIME_TIME_STATUS_SYNCHRONIZED_BIT_IS_SET(val) ((val) & (1 << ZB_ZCL_TIME_SYNCHRONIZED))
/** Check if MasterZoneDST bit of @ref zb_zcl_time_attr_e::ZB_ZCL_ATTR_TIME_TIME_STATUS_ID "TimeStatus" attribute is set.
 * @param val - 8-bit that contains value of @e TimeStatus attribute
 * @return zb_uint8_t value greater then 0 if @e MasterZoneDST bit is set, and return 0 otherwise.
 * @see @ref zb_zcl_time_time_status_e
*/
#define ZB_ZCL_TIME_TIME_STATUS_MASTER_ZONE_DST_BIT_IS_SET(val) ((val) & (1 << ZB_ZCL_TIME_MASTER_ZONE_DST))
/** Check if Superseding bit of @ref zb_zcl_time_attr_e::ZB_ZCL_ATTR_TIME_TIME_STATUS_ID "TimeStatus" attribute is set.
 * @param val - 8-bit that contains value of @e TimeStatus attribute
 * @return zb_uint8_t value greater then 0 if @e Superseding bit is set, and return 0 otherwise.
 * @see @ref zb_zcl_time_time_status_e
*/
#define ZB_ZCL_TIME_TIME_STATUS_SUPERSEDING_BIT_IS_SET(val) ((val) & (1 << ZB_ZCL_TIME_SUPERSEDING))

/** @brief Default value for Time cluster revision global attribute */
#define ZB_ZCL_TIME_CLUSTER_REVISION_DEFAULT ((zb_uint16_t)0x0002u)

/** @brief Invalid value of Time attribute */
#define ZB_ZCL_TIME_TIME_INVALID_VALUE ((zb_time_t)0xFFFFFFFF)

/** @brief Default value for Time attribute */
#define ZB_ZCL_TIME_TIME_DEFAULT_VALUE ZB_ZCL_TIME_TIME_INVALID_VALUE

/** @brief Minimum value for Time attribute */
#define ZB_ZCL_TIME_TIME_MIN_VALUE ((zb_time_t)0x0)

/** @brief Maximum value for Time attribute */
#define ZB_ZCL_TIME_TIME_MAX_VALUE ((zb_time_t)0xFFFFFFFE)

/** @brief Default value for Time Status attribute */
#define ZB_ZCL_TIME_TIME_STATUS_DEFAULT_VALUE 0x00

/** @brief Default value for Time Zone attribute */
#define ZB_ZCL_TIME_TIME_ZONE_DEFAULT_VALUE 0x00000000

/** @brief Default value for DstStart attribute */
#define ZB_ZCL_TIME_DST_START_DEFAULT_VALUE ((zb_uint32_t)0xFFFFFFFF)

/** @brief Default value for DstEnd attribute */
#define ZB_ZCL_TIME_DST_END_DEFAULT_VALUE ((zb_uint32_t)0xFFFFFFFF)

/** @brief Default value for Dst Shift attribute */
#define ZB_ZCL_TIME_DST_SHIFT_DEFAULT_VALUE 0x00000000

/** @brief Default value for StandardTime attribute */
#define ZB_ZCL_TIME_STANDARD_TIME_DEFAULT_VALUE ((zb_uint32_t)0xFFFFFFFF)

/** @brief Default value for LocalTime attribute */
#define ZB_ZCL_TIME_LOCAL_TIME_DEFAULT_VALUE ((zb_uint32_t)0xFFFFFFFF)

/** @brief Default value for Last Set Time attribute */
#define ZB_ZCL_TIME_LAST_SET_TIME_DEFAULT_VALUE 0xffffffff

/** @brief Default value for Valid Until Time attribute */
#define ZB_ZCL_TIME_VALID_UNTIL_TIME_DEFAULT_VALUE 0xffffffff

/** @brief Declare attribute list for Time cluster
    @param attr_list - attribute list name
    @param time - pointer to variable to store Time attribute value; write-optional acc.to ZCL8, be careful when redefining its handling
    @param time_status - pointer to variable to store Time Status attribute value; write-optional acc.to ZCL8, be careful when redefining its handling
    @param time_zone - pointer to variable to store Time Zone attribute value
    @param dst_start - pointer to variable to store Dst Start attribute value
    @param dst_end - pointer to variable to store Dst End attribute value
    @param dst_shift - pointer to variable to store Dst Shift attribute value
    @param standard_time - pointer to variable to store Standard Time attribute value
    @param local_time - pointer to variable to store Local Time attribute value
    @param last_set_time - pointer to variable to store Last Set Time attribute value
    @param valid_until_time - pointer to variable to store Valid Until Time attribute value

    Time and TimeStatus attributes are Read & Write-Optional acc. to ZCL8 spec.
    Due to internal implementation specifics Read-Write access mode is used for
    declaring the attributes, while optional writability is blocked by the stack
    in runtime automatically according to conditions from ZCL8 spec sections 3.12.2.2.1-3.12.2.2.2.
*/
#define ZB_ZCL_DECLARE_TIME_ATTRIB_LIST(attr_list, time, time_status, time_zone,             \
  dst_start, dst_end, dst_shift, standard_time, local_time, last_set_time, valid_until_time) \
  ZB_ZCL_START_DECLARE_ATTRIB_LIST_CLUSTER_REVISION(attr_list, ZB_ZCL_TIME)                  \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_TIME_TIME_ID, (time))                                     \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_TIME_TIME_STATUS_ID, (time_status))                       \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_TIME_TIME_ZONE_ID, (time_zone))                           \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_TIME_DST_START_ID, (dst_start))                           \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_TIME_DST_END_ID, (dst_end))                               \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_TIME_DST_SHIFT_ID, (dst_shift))                           \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_TIME_STANDARD_TIME_ID, (standard_time))                   \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_TIME_LOCAL_TIME_ID, (local_time))                         \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_TIME_LAST_SET_TIME_ID, (last_set_time))                   \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_TIME_VALID_UNTIL_TIME_ID, (valid_until_time))             \
  ZB_ZCL_FINISH_DECLARE_ATTRIB_LIST

/** @} */ /* ZB_ZCL_TIME_ATTRS */

/** @defgroup ZB_ZCL_TIME_COMMANDS Time cluster commands
 *  @{
 */

/** @} */ /* ZB_ZCL_TIME_COMMANDS */


/** @cond internals_doc */
/** Acc. to ZCL8 Table 3-69 and section 3.12.2.2.1 "Time Attribute" Time attribute is write-optional.
 *  The corresponding conditions are checked in the stack, but be careful when redefining
 *  processing of the attribute in applications.
 */
#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_TIME_TIME_ID(data_ptr)             \
{                                                                             \
  ZB_ZCL_ATTR_TIME_TIME_ID,                                                   \
  ZB_ZCL_ATTR_TYPE_UTC_TIME,                                                  \
  ZB_ZCL_ATTR_ACCESS_READ_WRITE,                                              \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                         \
  (void*) data_ptr                                                            \
}

/** Acc. to ZCL8 Table 3-69 and section 3.12.2.2.2 "TimeStatus Attribute" TimeStatus attribute is write-optional.
 *  The corresponding conditions are checked in the stack, but be careful when redefining
 *  processing of the attribute in applications.
 */
#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_TIME_TIME_STATUS_ID(data_ptr)      \
{                                                                             \
  ZB_ZCL_ATTR_TIME_TIME_STATUS_ID,                                            \
  ZB_ZCL_ATTR_TYPE_8BITMAP,                                                   \
  ZB_ZCL_ATTR_ACCESS_READ_WRITE,                                              \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                         \
  (void*) data_ptr                                                            \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_TIME_TIME_ZONE_ID(data_ptr)        \
{                                                                             \
  ZB_ZCL_ATTR_TIME_TIME_ZONE_ID,                                              \
  ZB_ZCL_ATTR_TYPE_S32,                                                       \
  ZB_ZCL_ATTR_ACCESS_READ_WRITE,                                              \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                         \
  (void*) data_ptr                                                            \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_TIME_DST_START_ID(data_ptr)        \
{                                                                             \
  ZB_ZCL_ATTR_TIME_DST_START_ID,                                              \
  ZB_ZCL_ATTR_TYPE_U32,                                                       \
  ZB_ZCL_ATTR_ACCESS_READ_WRITE,                                              \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                         \
  (void*) data_ptr                                                            \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_TIME_DST_END_ID(data_ptr)          \
{                                                                             \
  ZB_ZCL_ATTR_TIME_DST_END_ID,                                                \
  ZB_ZCL_ATTR_TYPE_U32,                                                       \
  ZB_ZCL_ATTR_ACCESS_READ_WRITE,                                              \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                         \
  (void*) data_ptr                                                            \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_TIME_DST_SHIFT_ID(data_ptr)        \
{                                                                             \
  ZB_ZCL_ATTR_TIME_DST_SHIFT_ID,                                              \
  ZB_ZCL_ATTR_TYPE_S32,                                                       \
  ZB_ZCL_ATTR_ACCESS_READ_WRITE,                                              \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                         \
  (void*) data_ptr                                                            \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_TIME_STANDARD_TIME_ID(data_ptr)    \
{                                                                             \
  ZB_ZCL_ATTR_TIME_STANDARD_TIME_ID,                                          \
  ZB_ZCL_ATTR_TYPE_U32,                                                       \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                               \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                         \
  (void*) data_ptr                                                            \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_TIME_LOCAL_TIME_ID(data_ptr)       \
{                                                                             \
  ZB_ZCL_ATTR_TIME_LOCAL_TIME_ID,                                             \
  ZB_ZCL_ATTR_TYPE_U32,                                                       \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                               \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                         \
  (void*) data_ptr                                                            \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_TIME_LAST_SET_TIME_ID(data_ptr)    \
{                                                                             \
  ZB_ZCL_ATTR_TIME_LAST_SET_TIME_ID,                                          \
  ZB_ZCL_ATTR_TYPE_UTC_TIME,                                                  \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                               \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                         \
  (void*) data_ptr                                                            \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_TIME_VALID_UNTIL_TIME_ID(data_ptr) \
{                                                                             \
  ZB_ZCL_ATTR_TIME_VALID_UNTIL_TIME_ID,                                       \
  ZB_ZCL_ATTR_TYPE_UTC_TIME,                                                  \
  ZB_ZCL_ATTR_ACCESS_READ_WRITE,                                              \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                         \
  (void*) data_ptr                                                            \
}

/*! @internal Number of attributes mandatory for reporting in Time cluster */
#define ZB_ZCL_TIME_REPORT_ATTR_COUNT 0

/** @endcond */ /* Time cluster internals */


/** @struct zb_zcl_time_attrs_s
 *  @brief Time cluster attributes
 */
typedef struct zb_zcl_time_attrs_s
{

  /** @copydoc ZB_ZCL_ATTR_TIME_TIME_ID
   * @see ZB_ZCL_ATTR_TIME_TIME_ID
   */
  zb_uint32_t time;

  /** @copydoc ZB_ZCL_ATTR_TIME_TIME_STATUS_ID
   * @see ZB_ZCL_ATTR_TIME_TIME_STATUS_ID
   */
  zb_uint8_t time_status;

  /** @copydoc ZB_ZCL_ATTR_TIME_TIME_ZONE_ID
   * @see ZB_ZCL_ATTR_TIME_TIME_ZONE_ID
   */
  zb_int32_t time_zone;

  /** @copydoc ZB_ZCL_ATTR_TIME_DST_START_ID
   * @see ZB_ZCL_ATTR_TIME_DST_START_ID
   */
  zb_uint32_t dst_start;

  /** @copydoc ZB_ZCL_ATTR_TIME_DST_END_ID
   * @see ZB_ZCL_ATTR_TIME_DST_END_ID
   */
  zb_uint32_t dst_end;

  /** @copydoc ZB_ZCL_ATTR_TIME_DST_SHIFT_ID
   * @see ZB_ZCL_ATTR_TIME_DST_SHIFT_ID
   */
  zb_uint32_t dst_shift;

  /** @copydoc ZB_ZCL_ATTR_TIME_STANDARD_TIME_ID
   * @see ZB_ZCL_ATTR_TIME_STANDARD_TIME_ID
   */
  zb_uint32_t standard_time;

  /** @copydoc ZB_ZCL_ATTR_TIME_LOCAL_TIME_ID
   * @see ZB_ZCL_ATTR_TIME_LOCAL_TIME_ID
   */
  zb_uint32_t local_time;

  /** @copydoc ZB_ZCL_ATTR_TIME_LAST_SET_TIME_ID
   * @see ZB_ZCL_ATTR_TIME_LAST_SET_TIME_ID
   */
  zb_uint32_t last_set_time;

  /** @copydoc ZB_ZCL_ATTR_TIME_VALID_UNTIL_TIME_ID
   * @see ZB_ZCL_ATTR_TIME_VALID_UNTIL_TIME_ID
   */
  zb_uint32_t valid_until_time;
} zb_zcl_time_attrs_t;


/** @brief Declare attribute list for Time cluster
 *  @param[in]  attr_list - attribute list variable name
 *  @param[in]  attrs - variable of @ref zb_zcl_time_attrs_s type (containing Time cluster attributes)
 */
#define ZB_ZCL_DECLARE_TIME_ATTR_LIST(attr_list, attrs)    \
  ZB_ZCL_DECLARE_TIME_ATTRIB_LIST(attr_list,               \
     &attrs.time, &attrs.time_status, &attrs.time_zone,    \
     &attrs.dst_start, &attrs.dst_end, &attrs.dst_shift,   \
     &attrs.standard_time, &attrs.local_time,              \
     &attrs.last_set_time, &attrs.valid_until_time )

/** @} */  /** ZB_ZCL_TIME */

/** @endcond */ /* DOXYGEN_ZCL_SECTION */

void zb_zcl_time_update_current_time(zb_uint8_t endpoint);

void zb_zcl_time_init_server(void);
void zb_zcl_time_init_client(void);
#define ZB_ZCL_CLUSTER_ID_TIME_SERVER_ROLE_INIT zb_zcl_time_init_server
#define ZB_ZCL_CLUSTER_ID_TIME_CLIENT_ROLE_INIT zb_zcl_time_init_client

#endif /* ZB_ZCL_TIME_H */
