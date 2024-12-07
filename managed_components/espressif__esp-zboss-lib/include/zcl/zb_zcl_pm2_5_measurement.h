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
/* PURPOSE: PM2.5 Measurement cluster definitions */

#ifndef ZB_ZCL_PM2_5_MEASUREMENT_H
#define ZB_ZCL_PM2_5_MEASUREMENT_H 1

#include "zcl/zb_zcl_common.h"
#include "zcl/zb_zcl_commands.h"

/** @cond DOXYGEN_ZCL_SECTION */

/** @addtogroup ZB_ZCL_CONCENTRATION_MEASUREMENT
 *  @{
 *    @details
 *    The server cluster provides an interface to concentration measurement
 *    functionality. The measurement is reportable and may be configured for
 *    reporting. Concentration measurements include, but are not limited to,
 *    levels in gases, such as CO, CO2, and ethylene, or in fluids and
 *    solids, such as dissolved oxygen, chemi8721 cals & pesticides.
 *
 *    PM2.5 (Particulate Matter 2.5 microns or less)
 */

/* Cluster ZB_ZCL_CLUSTER_ID_PM2_5_MEASUREMENT */

/** @name PM2.5 Measurement cluster attributes
 *  @{
 */

/** @brief PM2.5 Measurement cluster attribute identifiers
 *  @see ZCL spec, PM2.5 Measurement Cluster 4.14.1.4
 */
enum zb_zcl_pm2_5_measurement_attr_e
{
  /** @brief MeasuredValue attribute, ZCL spec 4.13.2.1.1 */
  ZB_ZCL_ATTR_PM2_5_MEASUREMENT_MEASURED_VALUE_ID     = 0x0000,
  /** @brief MinMeasuredValue attribute, ZCL spec 4.13.2.1.2 */
  ZB_ZCL_ATTR_PM2_5_MEASUREMENT_MIN_MEASURED_VALUE_ID = 0x0001,
  /** @brief MaxMeasuredValue attribute, ZCL spec 4.13.2.1.3 */
  ZB_ZCL_ATTR_PM2_5_MEASUREMENT_MAX_MEASURED_VALUE_ID = 0x0002,
  /** @brief Tolerance attribute, ZCL spec 4.13.2.1.4 */
  ZB_ZCL_ATTR_PM2_5_MEASUREMENT_TOLERANCE_ID          = 0x0003,
};

/** @brief Default value for PM2.5 Measurement cluster revision global attribute */
#define ZB_ZCL_PM2_5_MEASUREMENT_CLUSTER_REVISION_DEFAULT ((zb_uint16_t)0x0001u)

/** @brief Default value for MeasuredValue attribute */
#define ZB_ZCL_PM2_5_MEASUREMENT_MEASURED_VALUE_DEFAULT_VALUE     (.0/.0)
        
/** @brief Default value for MinMeasuredValue attribute */
#define ZB_ZCL_PM2_5_MEASUREMENT_MIN_MEASURED_VALUE_DEFAULT_VALUE (.0/.0)

/** @brief Default value for MaxMeasuredValue attribute */
#define ZB_ZCL_PM2_5_MEASUREMENT_MAX_MEASURED_VALUE_DEFAULT_VALUE (.0/.0)


/** @brief MeasuredValue attribute unknown */
#define ZB_ZCL_PM2_5_MEASUREMENT_MEASURED_VALUE_UNKNOWN       (.0/.0)

/** @brief MinMeasuredValue attribute undefined */
#define ZB_ZCL_PM2_5_MEASUREMENT_MIN_MEASURED_VALUE_UNDEFINED (.0/.0)

/** @brief MaxMeasuredValue attribute undefined */
#define ZB_ZCL_PM2_5_MEASUREMENT_MAX_MEASURED_VALUE_UNDEFINED (.0/.0)

/** @brief MinMeasuredValue attribute minimum value */
#define ZB_ZCL_PM2_5_MEASUREMENT_MIN_MEASURED_VALUE_MIN_VALUE 0.0

/** @brief MaxMeasuredValue attribute maximum value */
#define ZB_ZCL_PM2_5_MEASUREMENT_MAX_MEASURED_VALUE_MAX_VALUE 1.0

/** @cond internals_doc */
/** @name PM2.5 Measurement cluster internals
 *  Internal structures for PM2.5 Measurement cluster
 *  @{
 */

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PM2_5_MEASUREMENT_MEASURED_VALUE_ID(data_ptr) \
{                                                                   \
  ZB_ZCL_ATTR_PM2_5_MEASUREMENT_MEASURED_VALUE_ID,                  \
  ZB_ZCL_ATTR_TYPE_SINGLE,                                          \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY | ZB_ZCL_ATTR_ACCESS_REPORTING,      \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                               \
  (void*) data_ptr                                                  \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PM2_5_MEASUREMENT_MIN_MEASURED_VALUE_ID(data_ptr) \
{                                                                   \
  ZB_ZCL_ATTR_PM2_5_MEASUREMENT_MIN_MEASURED_VALUE_ID,              \
  ZB_ZCL_ATTR_TYPE_SINGLE,                                          \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                     \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                               \
  (void*) data_ptr                                                  \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PM2_5_MEASUREMENT_MAX_MEASURED_VALUE_ID(data_ptr) \
{                                                                   \
  ZB_ZCL_ATTR_PM2_5_MEASUREMENT_MAX_MEASURED_VALUE_ID,              \
  ZB_ZCL_ATTR_TYPE_SINGLE,                                          \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                     \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                               \
  (void*) data_ptr                                                  \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PM2_5_MEASUREMENT_TOLERANCE_ID(data_ptr) \
{                                                                   \
  ZB_ZCL_ATTR_PM2_5_MEASUREMENT_TOLERANCE_ID,                       \
  ZB_ZCL_ATTR_TYPE_SINGLE,                                          \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                     \
  (ZB_ZCL_MANUFACTURER_SPECIFIC),                                   \
  (void*) data_ptr                                                  \
}

/** Number of attributes mandatory for reporting in PM2.5 Measurement cluster */
#define ZB_ZCL_PM2_5_MEASUREMENT_REPORT_ATTR_COUNT 1

/** @} */
/** @endcond */ /* PM2.5 Measurement cluster internals */

/** @brief Declare attribute list for PM2.5 Measurement cluster
 *  @param attr_list - attribute list name
 *  @param measured_value -  pointer to variable storing MeasuredValue attribute value
 *  @param min_measured_value - pointer to variable storing MinMeasuredValue attribute value
 *  @param max_measured_value - pointer to variable storing MaxMeasuredValue attribute value
 *  @param tolerance - pointer to variable storing Tolerance attribute value
 */
#define ZB_ZCL_DECLARE_PM2_5_MEASUREMENT_ATTRIB_LIST(                             \
    attr_list,                                                                    \
    measured_value,                                                               \
    min_measured_value,                                                           \
    max_measured_value,                                                           \
    tolerance)                                                                    \
  ZB_ZCL_START_DECLARE_ATTRIB_LIST_CLUSTER_REVISION(attr_list, ZB_ZCL_PM2_5_MEASUREMENT)          \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_PM2_5_MEASUREMENT_MEASURED_VALUE_ID, (measured_value))         \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_PM2_5_MEASUREMENT_MIN_MEASURED_VALUE_ID, (min_measured_value)) \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_PM2_5_MEASUREMENT_MAX_MEASURED_VALUE_ID, (max_measured_value)) \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_PM2_5_MEASUREMENT_TOLERANCE_ID, (tolerance))                   \
  ZB_ZCL_FINISH_DECLARE_ATTRIB_LIST

/**
 *  @brief PM2.5 Measurement cluster attributes
 */
typedef struct zb_zcl_pm2_5_measurement_attrs_s
{
  /** @copydoc ZB_ZCL_ATTR_PM2_5_MEASUREMENT_MEASURED_VALUE_ID
   * @see ZB_ZCL_ATTR_PM2_5_MEASUREMENT_MEASURED_VALUE_ID
   */
  zb_single_t measured_value;

  /** @copydoc ZB_ZCL_ATTR_PM2_5_MEASUREMENT_MIN_MEASURED_VALUE_ID
   * @see ZB_ZCL_ATTR_PM2_5_MEASUREMENT_MIN_MEASURED_VALUE_ID
   */
  zb_single_t min_measured_value;

  /** @copydoc ZB_ZCL_ATTR_PM2_5_MEASUREMENT_MAX_MEASURED_VALUE_ID
   * @see ZB_ZCL_ATTR_PM2_5_MEASUREMENT_MAX_MEASURED_VALUE_ID
   */
  zb_single_t max_measured_value;

  /** @copydoc ZB_ZCL_ATTR_PM2_5_MEASUREMENT_TOLERANCE_ID
   * @see ZB_ZCL_ATTR_PM2_5_MEASUREMENT_TOLERANCE_ID
   */
  zb_single_t tolerance;
} zb_zcl_pm2_5_measurement_attrs_t;

/** @brief Declare attribute list for PM2.5 Measurement cluster
 *  @param[in]  attr_list - attribute list variable name
 *  @param[in]  attrs - variable of @ref zb_zcl_pm2_5_measurement_attrs_t type
 *                      (containing PM2.5 Measurement cluster attributes)
 */
#define ZB_ZCL_DECLARE_PM2_5_MEASUREMENT_ATTR_LIST(attr_list, attrs)      \
  ZB_ZCL_DECLARE_PM2_5_MEASUREMENT_ATTRIB_LIST(attr_list,                 \
                                               &attrs.measured_value,     \
                                               &attrs.min_measured_value, \
                                               &attrs.max_measured_value, \
                                               &attrs.tolerance)


/** @} */ /* end of ZB_ZCL_PM2_5_MEASUREMENT_ATTRS_GROUP group */

/** @} */ /* ZCL PM2.5 Measurement cluster definitions */

/** @endcond */ /* DOXYGEN_ZCL_SECTION */

void zb_zcl_pm2_5_measurement_init_server(void);
void zb_zcl_pm2_5_measurement_init_client(void);
#define ZB_ZCL_CLUSTER_ID_PM2_5_MEASUREMENT_SERVER_ROLE_INIT zb_zcl_pm2_5_measurement_init_server
#define ZB_ZCL_CLUSTER_ID_PM2_5_MEASUREMENT_CLIENT_ROLE_INIT zb_zcl_pm2_5_measurement_init_client

#endif /* ZB_ZCL_PM2_5_MEASUREMENT_H */
