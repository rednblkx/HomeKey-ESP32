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
/* PURPOSE: Device Temperature Configuration cluster definitions */

#ifndef ZB_ZCL_DEVICE_TEMP_CONFIG_H
#define ZB_ZCL_DEVICE_TEMP_CONFIG_H 1

#include "zcl/zb_zcl_common.h"
#include "zcl/zb_zcl_commands.h"

/** @cond DOXYGEN_ZCL_SECTION */

/** @addtogroup ZB_ZCL_DEVICE_TEMP_CONFIG
 *  @{
 *    @details
 *    Attributes for determining information about a device’s internal
 *    temperature, and for configuring under/over temperature alarms for
 *    temperatures that are outside the device’s operating range.
 */

/* Cluster ZB_ZCL_CLUSTER_ID_DEVICE_TEMP_CONFIG */

/** @name Device Temperature Configuration cluster attributes
 *  @{
 */

/** @brief Device Temperature Configuration cluster attribute identifiers
 *  @see ZCL spec, Device Temperature Configuration Cluster 3.4.1.4
 */
enum zb_zcl_device_temp_config_attr_e
{
 /** @brief CurrentTemp attribute, ZCL spec 3.4.2.2.1 */
  ZB_ZCL_ATTR_DEVICE_TEMP_CONFIG_CURRENT_TEMP_ID           = 0x0000,
 /** @brief MinTempExperienced attribute, ZCL spec 3.4.2.2.1 */
  ZB_ZCL_ATTR_DEVICE_TEMP_CONFIG_MIN_TEMP_EXPERIENCED_ID   = 0x0001,
 /** @brief MaxTempExperienced attribute, ZCL spec 3.4.2.2.1 */
  ZB_ZCL_ATTR_DEVICE_TEMP_CONFIG_MAX_TEMP_EXPERIENCED_ID   = 0x0002,
 /** @brief OverTempTotalDwell attribute, ZCL spec 3.4.2.2.1 */
  ZB_ZCL_ATTR_DEVICE_TEMP_CONFIG_OVER_TEMP_TOTAL_DWELL_ID  = 0x0003,

 /** @brief DeviceTempAlarmMask attribute, ZCL spec 3.4.2.2.2 */
  ZB_ZCL_ATTR_DEVICE_TEMP_CONFIG_DEVICE_TEMP_ALARM_MASK_ID     = 0x0010,
 /** @brief LowTempThreshold attribute, ZCL spec 3.4.2.2.2 */
  ZB_ZCL_ATTR_DEVICE_TEMP_CONFIG_LOW_TEMP_THRESHOLD_ID         = 0x0011,
 /** @brief HighTempThreshold attribute, ZCL spec 3.4.2.2.2 */
  ZB_ZCL_ATTR_DEVICE_TEMP_CONFIG_HIGH_TEMP_THRESHOLD_ID        = 0x0012,
 /** @brief LowTempDwellTripPoint attribute, ZCL spec 3.4.2.2.2 */
  ZB_ZCL_ATTR_DEVICE_TEMP_CONFIG_LOW_TEMP_DWELL_TRIP_POINT_ID  = 0x0013,
 /** @brief HighTempDwellTripPoint attribute, ZCL spec 3.4.2.2.2 */
  ZB_ZCL_ATTR_DEVICE_TEMP_CONFIG_HIGH_TEMP_DWELL_TRIP_POINT_ID = 0x0014,
};

/** @brief Default value for Device Temperature Configuration cluster revision global attribute */
#define ZB_ZCL_DEVICE_TEMP_CONFIG_CLUSTER_REVISION_DEFAULT ((zb_uint16_t)0x0001u)

/** @brief Minimum value for CurrentTemp attribute */
#define ZB_ZCL_DEVICE_TEMP_CONFIG_CURRENT_TEMP_MIN_VALUE  (-200)
/** @brief Maximum value for CurrentTemp attribute */
#define ZB_ZCL_DEVICE_TEMP_CONFIG_CURRENT_TEMP_MAX_VALUE  (+200)

/** @brief Minimum value for MinTempExperienced attribute */
#define ZB_ZCL_DEVICE_TEMP_CONFIG_MIN_TEMP_EXPERIENCED_MIN_VALUE  (-200)
/** @brief Maximum value for MinTempExperienced attribute */
#define ZB_ZCL_DEVICE_TEMP_CONFIG_MIN_TEMP_EXPERIENCED_MAX_VALUE  (+200)

/** @brief Minimum value for MaxTempExperienced attribute */
#define ZB_ZCL_DEVICE_TEMP_CONFIG_MAX_TEMP_EXPERIENCED_MIN_VALUE  (-200)
/** @brief Maximum value for MaxTempExperienced attribute */
#define ZB_ZCL_DEVICE_TEMP_CONFIG_MAX_TEMP_EXPERIENCED_MAX_VALUE  (+200)

/** @brief Minimum value for OverTotalDwell attribute */
#define ZB_ZCL_DEVICE_TEMP_CONFIG_OVER_TEMP_TOTAL_DWELL_MIN_VALUE (0x0000)
/** @brief Maximum value for OverTotalDwell attribute */
#define ZB_ZCL_DEVICE_TEMP_CONFIG_OVER_TEMP_TOTAL_DWELL_MAX_VALUE (0xffff)

/** @brief Minimum value for DeviceTempAlarmMask attribute */
#define ZB_ZCL_DEVICE_TEMP_CONFIG_DEVICE_TEMP_ALARM_MASK_MIN_VALUE (0b00000000)
/** @brief Maximum value for DeviceTempAlarmMask attribute */
#define ZB_ZCL_DEVICE_TEMP_CONFIG_DEVICE_TEMP_ALARM_MASK_MAX_VALUE (0b00000011)

/** @brief Minimum value for LowTempThreshold attribute */
#define ZB_ZCL_DEVICE_TEMP_CONFIG_LOW_TEMP_THRESHOLD_MIN_VALUE  (-200)
/** @brief Maximum value for LowTempThreshold attribute */
#define ZB_ZCL_DEVICE_TEMP_CONFIG_LOW_TEMP_THRESHOLD_MAX_VALUE  (+200)

/** @brief Minimum value for HighTempThreshold attribute */
#define ZB_ZCL_DEVICE_TEMP_CONFIG_HIGH_TEMP_THRESHOLD_MIN_VALUE  (-200)
/** @brief Maximum value for HighTempThreshold attribute */
#define ZB_ZCL_DEVICE_TEMP_CONFIG_HIGH_TEMP_THRESHOLD_MAX_VALUE  (+200)

/** @brief Minimum value for LowTempTripPoint attribute */
#define ZB_ZCL_DEVICE_TEMP_CONFIG_LOW_TEMP_DWELL_TRIP_POINT_MIN_VALUE (0x000000)
/** @brief Maximum value for LowTempTripPoint attribute */
#define ZB_ZCL_DEVICE_TEMP_CONFIG_LOW_TEMP_DWELL_TRIP_POINT_MAX_VALUE (0xffffff)

/** @brief Minimum value for HighTempTripPoint attribute */
#define ZB_ZCL_DEVICE_TEMP_CONFIG_HIGH_TEMP_DWELL_TRIP_POINT_MIN_VALUE (0x000000)
/** @brief Maximum value for HighTempTripPoint attribute */
#define ZB_ZCL_DEVICE_TEMP_CONFIG_HIGH_TEMP_DWELL_TRIP_POINT_MAX_VALUE (0xffffff)

/** @brief Default value for CurrentTemp attribute */
#define ZB_ZCL_DEVICE_TEMP_CONFIG_CURRENT_TEMP_DEFAULT_VALUE                ZB_ZCL_NON_VALUE_INT16

/** @brief Default value for MinTempExperienced attribute */
#define ZB_ZCL_DEVICE_TEMP_CONFIG_MIN_TEMP_EXPERIENCED_DEFAULT_VALUE        ZB_ZCL_NON_VALUE_INT16

/** @brief Default value for MaxTempExperienced attribute */
#define ZB_ZCL_DEVICE_TEMP_CONFIG_MAX_TEMP_EXPERIENCED_DEFAULT_VALUE        ZB_ZCL_NON_VALUE_INT16

/** @brief Default value for OverTempTotalDwell attribute */
#define ZB_ZCL_DEVICE_TEMP_CONFIG_OVER_TEMP_TOTAL_DWELL_DEFAULT_VALUE       0

/** @brief Default value for DeviceTempAlarmMask attribute */
#define ZB_ZCL_DEVICE_TEMP_CONFIG_DEVICE_TEMP_ALARM_MASK_DEFAULT_VALUE      0

/** @brief Default value for LowTempThreshold attribute */
#define ZB_ZCL_DEVICE_TEMP_CONFIG_LOW_TEMP_THRESHOLD_DEFAULT_VALUE          ZB_ZCL_NON_VALUE_INT16

/** @brief Default value for HighTempThreshold attribute */
#define ZB_ZCL_DEVICE_TEMP_CONFIG_HIGH_TEMP_THRESHOLD_DEFAULT_VALUE         ZB_ZCL_NON_VALUE_INT16

/** @brief Default value for LowTempDwellTripPoint attribute */
#define ZB_ZCL_DEVICE_TEMP_CONFIG_LOW_TEMP_DWELL_TRIP_POINT_DEFAULT_VALUE   ZB_ZCL_NON_VALUE_UINT24

/** @brief Default value for HighTempDwellTripPoint attribute */
#define ZB_ZCL_DEVICE_TEMP_CONFIG_HIGH_TEMP_DWELL_TRIP_POINT_DEFAULT_VALUE  ZB_ZCL_NON_VALUE_UINT24


/** @cond internals_doc */
/** @name Device Temperature Configuration cluster internals
 *  Internal structures for Device Temperature Configuration cluster
 *  @{
 */

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_DEVICE_TEMP_CONFIG_CURRENT_TEMP_ID(data_ptr) \
{                                                                   \
  ZB_ZCL_ATTR_DEVICE_TEMP_CONFIG_CURRENT_TEMP_ID,                   \
  ZB_ZCL_ATTR_TYPE_S16,                                             \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                     \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                               \
  (void*) data_ptr                                                  \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_DEVICE_TEMP_CONFIG_MIN_TEMP_EXPERIENCED_ID(data_ptr) \
{                                                                   \
  ZB_ZCL_ATTR_DEVICE_TEMP_CONFIG_MIN_TEMP_EXPERIENCED_ID,           \
  ZB_ZCL_ATTR_TYPE_S16,                                             \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                     \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                               \
  (void*) data_ptr                                                  \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_DEVICE_TEMP_CONFIG_MAX_TEMP_EXPERIENCED_ID(data_ptr) \
{                                                                   \
  ZB_ZCL_ATTR_DEVICE_TEMP_CONFIG_MAX_TEMP_EXPERIENCED_ID,           \
  ZB_ZCL_ATTR_TYPE_S16,                                             \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                     \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                               \
  (void*) data_ptr                                                  \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_DEVICE_TEMP_CONFIG_OVER_TEMP_TOTAL_DWELL_ID(data_ptr) \
{                                                                   \
  ZB_ZCL_ATTR_DEVICE_TEMP_CONFIG_OVER_TEMP_TOTAL_DWELL_ID,          \
  ZB_ZCL_ATTR_TYPE_U16,                                             \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                     \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                               \
  (void*) data_ptr                                                  \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_DEVICE_TEMP_CONFIG_DEVICE_TEMP_ALARM_MASK_ID(data_ptr) \
{                                                                   \
  ZB_ZCL_ATTR_DEVICE_TEMP_CONFIG_DEVICE_TEMP_ALARM_MASK_ID,         \
  ZB_ZCL_ATTR_TYPE_8BITMAP,                                         \
  ZB_ZCL_ATTR_ACCESS_READ_WRITE,                                    \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                               \
  (void*) data_ptr                                                  \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_DEVICE_TEMP_CONFIG_LOW_TEMP_THRESHOLD_ID(data_ptr) \
{                                                                   \
  ZB_ZCL_ATTR_DEVICE_TEMP_CONFIG_LOW_TEMP_THRESHOLD_ID,             \
  ZB_ZCL_ATTR_TYPE_S16,                                             \
  ZB_ZCL_ATTR_ACCESS_READ_WRITE,                                    \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                               \
  (void*) data_ptr                                                  \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_DEVICE_TEMP_CONFIG_HIGH_TEMP_THRESHOLD_ID(data_ptr) \
{                                                                   \
  ZB_ZCL_ATTR_DEVICE_TEMP_CONFIG_HIGH_TEMP_THRESHOLD_ID,            \
  ZB_ZCL_ATTR_TYPE_S16,                                             \
  ZB_ZCL_ATTR_ACCESS_READ_WRITE,                                    \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                               \
  (void*) data_ptr                                                  \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_DEVICE_TEMP_CONFIG_LOW_TEMP_DWELL_TRIP_POINT_ID(data_ptr) \
{                                                                   \
  ZB_ZCL_ATTR_DEVICE_TEMP_CONFIG_LOW_TEMP_DWELL_TRIP_POINT_ID,      \
  ZB_ZCL_ATTR_TYPE_U24,                                             \
  ZB_ZCL_ATTR_ACCESS_READ_WRITE,                                    \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                               \
  (void*) data_ptr                                                  \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_DEVICE_TEMP_CONFIG_HIGH_TEMP_DWELL_TRIP_POINT_ID(data_ptr) \
{                                                                   \
  ZB_ZCL_ATTR_DEVICE_TEMP_CONFIG_HIGH_TEMP_DWELL_TRIP_POINT_ID,     \
  ZB_ZCL_ATTR_TYPE_U24,                                             \
  ZB_ZCL_ATTR_ACCESS_READ_WRITE,                                    \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                               \
  (void*) data_ptr                                                  \
}

/** Number of attributes mandatory for reporting in Device Temperature Configuration cluster */
#define ZB_ZCL_DEVICE_TEMP_CONFIG_REPORT_ATTR_COUNT 0

/** @} */
/** @endcond */ /* Device Temperature Configuration cluster internals */

/** @brief Declare attribute list for Device Temperature Configuration cluster
 *  @param attr_list - attribute list name
 *  @param current_temp - pointer to variable storing CurrentTemp attribute value
 *  @param min_temp_experienced - pointer to variable storing MinTempExperienced attribute value
 *  @param max_temp_experienced - pointer to variable storing MaxTempExperienced attribute value
 *  @param over_temp_total_dwell - pointer to variable storing OverTempTotalDwell attribute value
 *  @param device_temp_alarm_mask - pointer to variable storing DeviceTempAlarmMask attribute value
 *  @param low_temp_threshold - pointer to variable storing LowTempThreshold attribute value
 *  @param high_temp_threshold - pointer to variable storing HighTempThreshold attribute value
 *  @param low_temp_dwell_trip_point - pointer to variable storing LowTempDwellTripPoint attribute value
 *  @param high_temp_dwell_trip_point - pointer to variable storing HighTempDwellTripPoint attribute value
 */
#define ZB_ZCL_DECLARE_DEVICE_TEMP_CONFIG_ATTRIB_LIST(attr_list,                  \
                                                      current_temp,               \
                                                      min_temp_experienced,       \
                                                      max_temp_experienced,       \
                                                      over_temp_total_dwell,      \
                                                      device_temp_alarm_mask,     \
                                                      low_temp_threshold,         \
                                                      high_temp_threshold,        \
                                                      low_temp_dwell_trip_point,  \
                                                      high_temp_dwell_trip_point) \
\
  ZB_ZCL_START_DECLARE_ATTRIB_LIST_CLUSTER_REVISION(attr_list, ZB_ZCL_DEVICE_TEMP_CONFIG)                          \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_DEVICE_TEMP_CONFIG_CURRENT_TEMP_ID, (current_temp))                             \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_DEVICE_TEMP_CONFIG_MIN_TEMP_EXPERIENCED_ID, (min_temp_experienced))             \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_DEVICE_TEMP_CONFIG_MAX_TEMP_EXPERIENCED_ID, (max_temp_experienced))             \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_DEVICE_TEMP_CONFIG_OVER_TEMP_TOTAL_DWELL_ID, (over_temp_total_dwell))           \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_DEVICE_TEMP_CONFIG_DEVICE_TEMP_ALARM_MASK_ID, (device_temp_alarm_mask))         \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_DEVICE_TEMP_CONFIG_LOW_TEMP_THRESHOLD_ID, (low_temp_threshold))                 \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_DEVICE_TEMP_CONFIG_HIGH_TEMP_THRESHOLD_ID, (high_temp_threshold))               \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_DEVICE_TEMP_CONFIG_LOW_TEMP_DWELL_TRIP_POINT_ID, (low_temp_dwell_trip_point))   \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_DEVICE_TEMP_CONFIG_HIGH_TEMP_DWELL_TRIP_POINT_ID, (high_temp_dwell_trip_point)) \
  ZB_ZCL_FINISH_DECLARE_ATTRIB_LIST

/**
 *  @brief Device Temperature Configuration cluster attributes
 */
typedef struct zb_zcl_device_temp_config_attrs_s
{
  /** @copydoc ZB_ZCL_ATTR_DEVICE_TEMP_CONFIG_CURRENT_TEMP_ID
   * @see ZB_ZCL_ATTR_DEVICE_TEMP_CONFIG_CURRENT_TEMP_ID
   */
  zb_int16_t  current_temp;

  /** @copydoc ZB_ZCL_ATTR_DEVICE_TEMP_CONFIG_MIN_TEMP_EXPERIENCED_ID
   * @see ZB_ZCL_ATTR_DEVICE_TEMP_CONFIG_MIN_TEMP_EXPERIENCED_ID
   */
  zb_int16_t  min_temp_experienced;

  /** @copydoc ZB_ZCL_ATTR_DEVICE_TEMP_CONFIG_MAX_TEMP_EXPERIENCED_ID
   * @see ZB_ZCL_ATTR_DEVICE_TEMP_CONFIG_MAX_TEMP_EXPERIENCED_ID
   */
  zb_int16_t  max_temp_experienced;

  /** @copydoc ZB_ZCL_ATTR_DEVICE_TEMP_CONFIG_OVER_TEMP_TOTAL_DWELL_ID
   * @see ZB_ZCL_ATTR_DEVICE_TEMP_CONFIG_OVER_TEMP_TOTAL_DWELL_ID
   */
  zb_uint16_t over_temp_total_dwell;

  /** @copydoc ZB_ZCL_ATTR_DEVICE_TEMP_CONFIG_DEVICE_TEMP_ALARM_MASK_ID
   * @see ZB_ZCL_ATTR_DEVICE_TEMP_CONFIG_DEVICE_TEMP_ALARM_MASK_ID
   */
  zb_uint8_t  device_temp_alarm_mask;

  /** @copydoc ZB_ZCL_ATTR_DEVICE_TEMP_CONFIG_LOW_TEMP_THRESHOLD_ID
   * @see ZB_ZCL_ATTR_DEVICE_TEMP_CONFIG_LOW_TEMP_THRESHOLD_ID
   */
  zb_int16_t  low_temp_threshold;

  /** @copydoc ZB_ZCL_ATTR_DEVICE_TEMP_CONFIG_HIGH_TEMP_THRESHOLD_ID
   * @see ZB_ZCL_ATTR_DEVICE_TEMP_CONFIG_HIGH_TEMP_THRESHOLD_ID
   */
  zb_int16_t  high_temp_threshold;

  /** @copydoc ZB_ZCL_ATTR_DEVICE_TEMP_CONFIG_LOW_TEMP_DWELL_TRIP_POINT_ID
   * @see ZB_ZCL_ATTR_DEVICE_TEMP_CONFIG_LOW_TEMP_DWELL_TRIP_POINT_ID
   */
  zb_uint24_t low_temp_dwell_trip_point;

  /** @copydoc ZB_ZCL_ATTR_DEVICE_TEMP_CONFIG_HIGH_TEMP_DWELL_TRIP_POINT_ID
   * @see ZB_ZCL_ATTR_DEVICE_TEMP_CONFIG_HIGH_TEMP_DWELL_TRIP_POINT_ID
   */
  zb_uint24_t high_temp_dwell_trip_point;
} zb_zcl_device_temp_config_attrs_t;

/** @brief Declare attribute list for Device Temperature Configuration cluster
 *  @param[in]  attr_list - attribute list variable name
 *  @param[in]  attrs - variable of @ref zb_zcl_device_temp_config_attrs_t type
 *                      (containing Device Temperature Configuration cluster attributes)
 */
#define ZB_ZCL_DECLARE_DEVICE_TEMP_CONFIG_ATTR_LIST(attr_list, attrs)              \
  ZB_ZCL_DECLARE_DEVICE_TEMP_CONFIG_ATTRIB_LIST(attr_list,                         \
                                                &attrs.current_temp,               \
                                                &attrs.min_temp_experienced,       \
                                                &attrs.max_temp_experienced,       \
                                                &attrs.over_temp_total_dwell,      \
                                                &attrs.device_temp_alarm_mask,     \
                                                &attrs.low_temp_threshold,         \
                                                &attrs.high_temp_threshold,        \
                                                &attrs.low_temp_dwell_trip_point,  \
                                                &attrs.high_temp_dwell_trip_point)

/** @} */ /* end of ZB_ZCL_DEVICE_TEMP_CONFIG_ATTRS_GROUP group */

/** @} */ /* ZCL Device Temperature Configuration cluster definitions */

/** @endcond */ /* DOXYGEN_ZCL_SECTION */

void zb_zcl_device_temp_config_init_server(void);
void zb_zcl_device_temp_config_init_client(void);
#define ZB_ZCL_CLUSTER_ID_DEVICE_TEMP_CONFIG_SERVER_ROLE_INIT zb_zcl_device_temp_config_init_server
#define ZB_ZCL_CLUSTER_ID_DEVICE_TEMP_CONFIG_CLIENT_ROLE_INIT zb_zcl_device_temp_config_init_client

#endif /* ZB_ZCL_DEVICE_TEMP_CONFIG_H */
