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
/* PURPOSE: Pressure Measurement cluster definitions
*/

#ifndef ZB_ZCL_PRESSURE_MEASUREMENT_H
#define ZB_ZCL_PRESSURE_MEASUREMENT_H 1

#include "zcl/zb_zcl_common.h"
#include "zcl/zb_zcl_commands.h"

/** @cond DOXYGEN_ZCL_SECTION */

/** @addtogroup ZB_ZCL_PRESSURE_MEASUREMENT
 *  @{
 *    @details
 *    All commands in the cluster have only request form, and carry no payload.
 */


/* Cluster ZB_ZCL_CLUSTER_ID_PRESSURE_MEASUREMENT */

/*! @name Pressure Measurement cluster attributes
    @{
*/

#if !defined(ZB_DISABLE_PRESSURE_MEASUREMENT_SCALE_TOLERANCE_ID) && defined(ZB_DISABLE_PRESSURE_MEASUREMENT_SCALE_ID)
# error If scale tolerance attribute is enabled, all scale attributes shall be enabled
#endif

/*! @brief Pressure Measurement cluster attribute identifiers
    @see ZCL spec, Pressure Measurement Cluster 4.5.2.2.1
*/
enum zb_zcl_pressure_measurement_attr_e
{
  /*! @brief MeasuredValue, ZCL spec 4.5.2.2.1.1 */
  ZB_ZCL_ATTR_PRESSURE_MEASUREMENT_VALUE_ID = 0x0000,
  /*! @brief MinMeasuredValue, ZCL spec 4.5.2.2.1.2 */
  ZB_ZCL_ATTR_PRESSURE_MEASUREMENT_MIN_VALUE_ID = 0x0001,
  /*! @brief MaxMeasuredValue, ZCL spec 4.5.2.2.1.3 */
  ZB_ZCL_ATTR_PRESSURE_MEASUREMENT_MAX_VALUE_ID = 0x0002,
  /*! @brief Tolerance, ZCL spec 4.5.2.2.1.4 */
#ifndef ZB_DISABLE_PRESSURE_MEASUREMENT_TOLERANCE_ID
  ZB_ZCL_ATTR_PRESSURE_MEASUREMENT_TOLERANCE_ID = 0x0003,
#else
  ZB_ZCL_ATTR_PRESSURE_MEASUREMENT_TOLERANCE_ID = 0xF003,
#endif

  /*! @brief ScaledValue, ZCL spec 4.5.2.1.2.1 */
#ifndef ZB_DISABLE_PRESSURE_MEASUREMENT_SCALE_ID
  ZB_ZCL_ATTR_PRESSURE_MEASUREMENT_SCALED_VALUE_ID = 0x0010,
#else
  ZB_ZCL_ATTR_PRESSURE_MEASUREMENT_SCALED_VALUE_ID = 0xF010,
#endif
  /*! @brief MinScaledValue, ZCL spec 4.5.2.1.2.2 */
#ifndef ZB_DISABLE_PRESSURE_MEASUREMENT_SCALE_ID
  ZB_ZCL_ATTR_PRESSURE_MEASUREMENT_MIN_SCALED_VALUE_ID = 0x0011,
#else
  ZB_ZCL_ATTR_PRESSURE_MEASUREMENT_MIN_SCALED_VALUE_ID = 0xF011,
#endif
  /*! @brief MaxScaledValue, ZCL spec 4.5.2.1.2.3 */
#ifndef ZB_DISABLE_PRESSURE_MEASUREMENT_SCALE_ID
  ZB_ZCL_ATTR_PRESSURE_MEASUREMENT_MAX_SACLED_VALUE_ID = 0x0012,
#else
  ZB_ZCL_ATTR_PRESSURE_MEASUREMENT_MAX_SACLED_VALUE_ID = 0xF012,
#endif
  /*! @brief ScaledTolerance, ZCL spec 4.5.2.1.2.4 */
#ifndef ZB_DISABLE_PRESSURE_MEASUREMENT_SCALE_TOLERANCE_ID
  ZB_ZCL_ATTR_PRESSURE_MEASUREMENT_SCALED_TOLERANCE_ID = 0x0013,
#else
  ZB_ZCL_ATTR_PRESSURE_MEASUREMENT_SCALED_TOLERANCE_ID = 0xF013,
#endif
  /*! @brief Scale, ZCL spec 4.5.2.1.2.5 */
#ifndef ZB_DISABLE_PRESSURE_MEASUREMENT_SCALE_ID
  ZB_ZCL_ATTR_PRESSURE_MEASUREMENT_SCALE_ID = 0x0014,
#else
  ZB_ZCL_ATTR_PRESSURE_MEASUREMENT_SCALE_ID = 0xF014,
#endif
};

/** @brief Default value for Pressure Measurement cluster revision global attribute */
#define ZB_ZCL_PRESSURE_MEASUREMENT_CLUSTER_REVISION_DEFAULT ((zb_uint16_t)0x0002u)

 /* (See: Table 4.15 Pressure Measurement Information Attribute Set) */

/** @brief MeasuredValue attribute unknown value */
#define ZB_ZCL_ATTR_PRESSURE_MEASUREMENT_VALUE_UNKNOWN                  ((zb_int16_t)0x8000)

/** @brief MinMeasuredValue attribute minimum value */
#define ZB_ZCL_ATTR_PRESSURE_MEASUREMENT_MIN_VALUE_MIN_VALUE            ((zb_int16_t)0x8001)

/** @brief MinMeasuredValue attribute maximum value */
#define ZB_ZCL_ATTR_PRESSURE_MEASUREMENT_MIN_VALUE_MAX_VALUE            ((zb_int16_t)0x7ffe)

/** @brief MinMeasuredValue attribute invalid value */
#define ZB_ZCL_ATTR_PRESSURE_MEASUREMENT_MIN_VALUE_INVALID              ((zb_int16_t)0x8000)

/** @brief MaxMeasuredValue attribute minimum value */
#define ZB_ZCL_ATTR_PRESSURE_MEASUREMENT_MAX_VALUE_MIN_VALUE            ((zb_int16_t)0x8002)

/** @brief MaxMeasuredValue attribute maximum value */
#define ZB_ZCL_ATTR_PRESSURE_MEASUREMENT_MAX_VALUE_MAX_VALUE            ((zb_int16_t)0x7fff)

/** @brief MaxMeasuredValue attribute invalid value */
#define ZB_ZCL_ATTR_PRESSURE_MEASUREMENT_MAX_VALUE_INVALID              ((zb_int16_t)0x8000)

/** @brief Tolerance attribute minimum value */
#define ZB_ZCL_ATTR_PRESSURE_MEASUREMENT_TOLERANCE_MIN_VALUE            ((zb_uint16_t)0x0000)

/** @brief Tolerance attribute maximum value */
#define ZB_ZCL_ATTR_PRESSURE_MEASUREMENT_TOLERANCE_MAX_VALUE            ((zb_uint16_t)0x0800)

/** @brief Default value for Value attribute */
#define ZB_ZCL_ATTR_PRESSURE_MEASUREMENT_VALUE_DEFAULT_VALUE            ((zb_int16_t)0xFFFF)

/** @brief Default value for MinValue attribute */
#define ZB_ZCL_PATTR_RESSURE_MEASUREMENT_MIN_VALUE_DEFAULT_VALUE        ((zb_int16_t)0x8000)

/** @brief Default value for MaxValue attribute */
#define ZB_ZCL_PATTR_RESSURE_MEASUREMENT_MAX_VALUE_DEFAULT_VALUE        ((zb_int16_t)0x8000)


 /* (See: Table 4.16 Extended Pressure Measurement Information Attribute Set) */

/** @brief MinScaledValue attribute unknown value */
#define ZB_ZCL_ATTR_PRESSURE_MEASUREMENT_MIN_SCALED_VALUE_UNKNOWN       ((zb_int16_t)0x8000)

/** @brief MinScaledValue attribute minimum value */
#define ZB_ZCL_ATTR_PRESSURE_MEASUREMENT_MIN_SCALED_VALUE_MIN_VALUE     ((zb_int16_t)0x8001)

/** @brief MinScaledValue attribute maximum value */
#define ZB_ZCL_ATTR_PRESSURE_MEASUREMENT_MIN_SCALED_VALUE_MAX_VALUE     ((zb_int16_t)0x7ffe)

/** @brief MaxScaledValue attribute unknown value */
#define ZB_ZCL_ATTR_PRESSURE_MEASUREMENT_MAX_SCALED_VALUE_UNKNOWN       ((zb_int16_t)0x8000)

/** @brief MaxScaledValue attribute minimum value */
#define ZB_ZCL_ATTR_PRESSURE_MEASUREMENT_MAX_SCALED_VALUE_MIN_VALUE     ((zb_int16_t)0x8002)

/** @brief MaxScaledValue attribute maximum value */
#define ZB_ZCL_ATTR_PRESSURE_MEASUREMENT_MAX_SCALED_VALUE_MAX_VALUE     ((zb_int16_t)0x7fff)

/** @brief MaxScaledValue attribute minimum value */
#define ZB_ZCL_ATTR_PRESSURE_MEASUREMENT_SCALED_TOLERANCE_MIN_VALUE     ((zb_uint16_t)0x0000)

/** @brief MaxScaledValue attribute maximum value */
#define ZB_ZCL_ATTR_PRESSURE_MEASUREMENT_SCALED_TOLERANCE_MAX_VALUE     ((zb_uint16_t)0x0800)

/** @brief MaxScaledValue attribute minimum value */
#define ZB_ZCL_ATTR_PRESSURE_MEASUREMENT_SCALE_MIN_VALUE                ((zb_int8_t)0x81)

/** @brief MaxScaledValue attribute maximum value */
#define ZB_ZCL_ATTR_PRESSURE_MEASUREMENT_SCALE_MAX_VALUE                ((zb_int8_t)0x7f)

/** @cond internals_doc */
/*! @internal @name Pressure Measurement cluster internals
    Internal structures for attribute representation in cluster definitions.
    @{
*/

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRESSURE_MEASUREMENT_VALUE_ID(data_ptr) \
{                                                               \
  ZB_ZCL_ATTR_PRESSURE_MEASUREMENT_VALUE_ID,                    \
  ZB_ZCL_ATTR_TYPE_S16,                                         \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY | ZB_ZCL_ATTR_ACCESS_REPORTING,  \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                           \
  (void*) data_ptr                                              \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRESSURE_MEASUREMENT_MIN_VALUE_ID(data_ptr) \
{                                                       \
  ZB_ZCL_ATTR_PRESSURE_MEASUREMENT_MIN_VALUE_ID,        \
  ZB_ZCL_ATTR_TYPE_S16,                                 \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                         \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                   \
  (void*) data_ptr                                      \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRESSURE_MEASUREMENT_MAX_VALUE_ID(data_ptr) \
{                                                       \
  ZB_ZCL_ATTR_PRESSURE_MEASUREMENT_MAX_VALUE_ID,        \
  ZB_ZCL_ATTR_TYPE_S16,                                 \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                         \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                   \
  (void*) data_ptr                                      \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRESSURE_MEASUREMENT_TOLERANCE_ID(data_ptr) \
{                                                              \
  ZB_ZCL_ATTR_PRESSURE_MEASUREMENT_TOLERANCE_ID,               \
  ZB_ZCL_ATTR_TYPE_U16,                                        \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                          \
  (void*) data_ptr                                             \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRESSURE_MEASUREMENT_SCALED_VALUE_ID(data_ptr) \
{                                                              \
  ZB_ZCL_ATTR_PRESSURE_MEASUREMENT_SCALED_VALUE_ID,            \
  ZB_ZCL_ATTR_TYPE_S16,                                        \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                          \
  (void*) data_ptr                                             \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRESSURE_MEASUREMENT_MIN_SCALED_VALUE_ID(data_ptr) \
{                                                              \
  ZB_ZCL_ATTR_PRESSURE_MEASUREMENT_MIN_SCALED_VALUE_ID,        \
  ZB_ZCL_ATTR_TYPE_S16,                                        \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                          \
  (void*) data_ptr                                             \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRESSURE_MEASUREMENT_MAX_SCALED_VALUE_ID(data_ptr) \
{                                                              \
  ZB_ZCL_ATTR_PRESSURE_MEASUREMENT_MAX_SCALED_VALUE_ID,        \
  ZB_ZCL_ATTR_TYPE_S16,                                        \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                          \
  (void*) data_ptr                                             \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRESSURE_MEASUREMENT_SCALED_TOLERANCE_ID(data_ptr) \
{                                                              \
  ZB_ZCL_ATTR_PRESSURE_MEASUREMENT_SCALED_TOLERANCE_ID,        \
  ZB_ZCL_ATTR_TYPE_U16,                                        \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                          \
  (void*) data_ptr                                             \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRESSURE_MEASUREMENT_SCALE_ID(data_ptr) \
{                                                              \
  ZB_ZCL_ATTR_PRESSURE_MEASUREMENT_SCALE_ID,                   \
  ZB_ZCL_ATTR_TYPE_S8,                                         \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                          \
  (void*) data_ptr                                             \
}

/*! @internal Number of attributes mandatory for reporting in Pressure Measurement cluster */
#define ZB_ZCL_PRESSURE_MEASUREMENT_REPORT_ATTR_COUNT 1

/*! Write attribute hook is used to set min/max Pressure values */
void zb_zcl_pressure_measurement_write_attr_hook(
  zb_uint8_t endpoint, zb_uint16_t attr_id, zb_uint8_t *new_value);

/*! @} */ /* Pressure Measurement cluster internals */
/*! @}
 *  @endcond */ /* internals_doc */

/** @brief Declare attribute list for Pressure Measurement cluster - server side
    @param attr_list - attribute list name
    @param value - pointer to variable to store MeasuredValue attribute
    @param min_value - pointer to variable to store MinMeasuredValue attribute
    @param max_value - pointer to variable to store MAxMeasuredValue attribute
    @param tolerance - pointer to variable to store Tolerance attribute
*/
#define ZB_ZCL_DECLARE_PRESSURE_MEASUREMENT_ATTRIB_LIST(attr_list,                  \
    value, min_value, max_value, tolerance)                                         \
  ZB_ZCL_START_DECLARE_ATTRIB_LIST_CLUSTER_REVISION(attr_list, ZB_ZCL_PRESSURE_MEASUREMENT) \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_PRESSURE_MEASUREMENT_VALUE_ID, (value))          \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_PRESSURE_MEASUREMENT_MIN_VALUE_ID, (min_value))  \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_PRESSURE_MEASUREMENT_MAX_VALUE_ID, (max_value))  \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_PRESSURE_MEASUREMENT_TOLERANCE_ID, (tolerance))  \
  ZB_ZCL_FINISH_DECLARE_ATTRIB_LIST

/*! @} */ /* Pressure Measurement cluster attributes */

/*! @name Pressure Measurement cluster commands
    @{
*/

/*! @} */ /* Pressure Measurement cluster commands */

/*! @} */ /* ZCL Pressure Measurement cluster definitions */

/** @endcond */ /* DOXYGEN_ZCL_SECTION */

void zb_zcl_pressure_measurement_init_server(void);
void zb_zcl_pressure_measurement_init_client(void);
#define ZB_ZCL_CLUSTER_ID_PRESSURE_MEASUREMENT_SERVER_ROLE_INIT zb_zcl_pressure_measurement_init_server
#define ZB_ZCL_CLUSTER_ID_PRESSURE_MEASUREMENT_CLIENT_ROLE_INIT zb_zcl_pressure_measurement_init_client


#endif /* ZB_ZCL_PRESSURE_MEASUREMENT_H */
