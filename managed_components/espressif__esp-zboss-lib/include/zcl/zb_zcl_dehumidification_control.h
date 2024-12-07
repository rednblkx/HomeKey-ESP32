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
/* PURPOSE: Dehumidification Control cluster definitions
*/

#ifndef ZB_ZCL_DEHUMIDIFICATION_CONTROL_H
#define ZB_ZCL_DEHUMIDIFICATION_CONTROL_H 1

#include "zcl/zb_zcl_common.h"
#include "zcl/zb_zcl_commands.h"


/** @cond DOXYGEN_ZCL_SECTION */

/** @addtogroup ZB_ZCL_DEHUMIDIFICATION
 * @{
 */


/* Cluster ZB_ZCL_CLUSTER_ID_DEHUMIDIFICATION_CONTROL */

/*! @name Dehumidification Control cluster attributes
    @{
*/

/*! @brief Dehumidification Control cluster attribute identifiers
    @see ZCL spec, subclause 6.4.2.2
*/
enum zb_zcl_dehumidification_control_attr_e
{
  /** The RelativeHumidity attribute is an 8-bit value that represents the current
   *  relative humidity (in %) measured by a local or remote sensor. */
  ZB_ZCL_ATTR_DEHUMIDIFICATION_CONTROL_RELATIVE_HUMIDITY_ID           = 0x0000,
  /** @brief Dehumidification Cooling attribute */
  ZB_ZCL_ATTR_DEHUMIDIFICATION_CONTROL_DEHUMIDIFICATION_COOLING_ID    = 0x0001,
  /** @brief RHDehumidification Setpoint attribute */
  ZB_ZCL_ATTR_DEHUMIDIFICATION_CONTROL_RHDEHUMIDIFICATION_SETPOINT_ID = 0x0010,
  /** The RelativeHumidityMode attribute is an 8-bit value that specifies how
   *  the RelativeHumidity value is being updated. */
  ZB_ZCL_ATTR_DEHUMIDIFICATION_CONTROL_RELATIVE_HUMIDITY_MODE_ID      = 0x0011,
  /** The DehumidificationLockout attribute is an 8-bit value that specifies
   *  whether dehumidification is allowed or not. */
  ZB_ZCL_ATTR_DEHUMIDIFICATION_CONTROL_DEHUMIDIFICATION_LOCKOUT_ID    = 0x0012,
  /** @brief Dehumidification Hysteresis attribute */
  ZB_ZCL_ATTR_DEHUMIDIFICATION_CONTROL_DEHUMIDIFICATION_HYSTERESIS_ID = 0x0013,
  /** @brief Dehumidification Max Cool attribute */
  ZB_ZCL_ATTR_DEHUMIDIFICATION_CONTROL_DEHUMIDIFICATION_MAX_COOL_ID   = 0x0014,
  /** The RelativeHumidityDisplay attribute is an 8-bit value that specifies
   *  whether the RelativeHumidity value is displayed to the user or not. */
  ZB_ZCL_ATTR_DEHUMIDIFICATION_CONTROL_RELATIVE_HUMIDITY_DISPLAY_ID   = 0x0015,
};

/** @brief Default value for Dehumidification Control cluster revision global attribute */
#define ZB_ZCL_DEHUMIDIFICATION_CONTROL_CLUSTER_REVISION_DEFAULT ((zb_uint16_t)0x0001u)

/** @brief Min value for RHDehumidification Setpoint attribute */
#define ZB_ZCL_DEHUMIDIFICATION_CONTROL_RHDEHUMIDIFICATION_SETPOINT_MIN_VALUE 0x1e

/** @brief Max value for RHDehumidification Setpoint attribute */
#define ZB_ZCL_DEHUMIDIFICATION_CONTROL_RHDEHUMIDIFICATION_SETPOINT_MAX_VALUE 0x64

/** @brief Default value for RHDehumidification Setpoint attribute */
#define ZB_ZCL_DEHUMIDIFICATION_CONTROL_RHDEHUMIDIFICATION_SETPOINT_DEFAULT_VALUE 0x32

/** @brief Default value for RelativeHumidityMode attribute */
#define ZB_ZCL_DEHUMIDIFICATION_CONTROL_RELATIVE_HUMIDITY_MODE_DEFAULT_VALUE ((zb_uint8_t)0x00)

/** @brief Default value for DehumidificationLockout attribute */
#define ZB_ZCL_DEHUMIDIFICATION_CONTROL_DEHUMIDIFICATION_LOCKOUT_DEFAULT_VALUE ((zb_uint8_t)0x01)

/** @brief Min value for Dehumidification Hysteresis attribute */
#define ZB_ZCL_DEHUMIDIFICATION_CONTROL_DEHUMIDIFICATION_HYSTERESIS_MIN_VALUE 0x02

/** @brief Max value for Dehumidification Hysteresis attribute */
#define ZB_ZCL_DEHUMIDIFICATION_CONTROL_DEHUMIDIFICATION_HYSTERESIS_MAX_VALUE 0x14

/** @brief Default value for Dehumidification Hysteresis attribute */
#define ZB_ZCL_DEHUMIDIFICATION_CONTROL_DEHUMIDIFICATION_HYSTERESIS_DEFAULT_VALUE 0x02

/** @brief Min value for Dehumidification Max Cool attribute */
#define ZB_ZCL_DEHUMIDIFICATION_CONTROL_DEHUMIDIFICATION_MAX_COOL_MIN_VALUE 0x14

/** @brief Max value for Dehumidification Max Cool attribute */
#define ZB_ZCL_DEHUMIDIFICATION_CONTROL_DEHUMIDIFICATION_MAX_COOL_MAX_VALUE 0x64

/** @brief Default value for Dehumidification Max Cool attribute */
#define ZB_ZCL_DEHUMIDIFICATION_CONTROL_DEHUMIDIFICATION_MAX_COOL_DEFAULT_VALUE 0x14

/** @brief Default value for RelativeHumidityDisplay attribute */
#define ZB_ZCL_DEHUMIDIFICATION_CONTROL_RELATIVE_HUMIDITY_DISPLAY_DEFAULT_VALUE ((zb_uint8_t)0x00)

/** @brief Declare attribute list for Dehumidification Control cluster
    @param attr_list - attribute list name
    @param dehumid_cooling - pointer to variable to store Dehumidification Cooling attribute value
    @param dehumid_setpoint - pointer to variable to store Dehumidification Setpoint attribute value
    @param dehumid_hysteresis - pointer to variable to store Dehumidification Hysteresis attribute value
    @param dehumid_max_cool - pointer to variable to store Dehumidification Max Cool attribute value
*/
#define ZB_ZCL_DECLARE_DEHUMIDIFICATION_CONTROL_ATTRIB_LIST(attr_list, dehumid_cooling, dehumid_setpoint,         \
                                               dehumid_hysteresis, dehumid_max_cool)                              \
  ZB_ZCL_START_DECLARE_ATTRIB_LIST_CLUSTER_REVISION(attr_list, ZB_ZCL_DEHUMIDIFICATION_CONTROL)                   \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_DEHUMIDIFICATION_CONTROL_DEHUMIDIFICATION_COOLING_ID, (dehumid_cooling))       \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_DEHUMIDIFICATION_CONTROL_RHDEHUMIDIFICATION_SETPOINT_ID, (dehumid_setpoint))   \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_DEHUMIDIFICATION_CONTROL_DEHUMIDIFICATION_HYSTERESIS_ID, (dehumid_hysteresis)) \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_DEHUMIDIFICATION_CONTROL_DEHUMIDIFICATION_MAX_COOL_ID, (dehumid_max_cool))     \
  ZB_ZCL_FINISH_DECLARE_ATTRIB_LIST

/*! @} */ /* Dehumidification Control cluster attributes */

/*! @name Dehumidification Control cluster commands
    @{
*/

/*! @} */ /* Dehumidification Control cluster commands */

/** @cond internals_doc */
/*! @internal @name Dehumidification Control cluster internals
    Internal structures for attribute representation in cluster definitions.
    @{
*/

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_DEHUMIDIFICATION_CONTROL_DEHUMIDIFICATION_COOLING_ID(data_ptr)    \
{                                                                                                            \
  ZB_ZCL_ATTR_DEHUMIDIFICATION_CONTROL_DEHUMIDIFICATION_COOLING_ID,                                          \
  ZB_ZCL_ATTR_TYPE_U8,                                                                                       \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY | ZB_ZCL_ATTR_ACCESS_REPORTING,                                               \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                                                        \
  (void*) data_ptr                                                                                           \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_DEHUMIDIFICATION_CONTROL_RHDEHUMIDIFICATION_SETPOINT_ID(data_ptr) \
{                                                                                                            \
  ZB_ZCL_ATTR_DEHUMIDIFICATION_CONTROL_RHDEHUMIDIFICATION_SETPOINT_ID,                                       \
  ZB_ZCL_ATTR_TYPE_U8,                                                                                       \
  ZB_ZCL_ATTR_ACCESS_READ_WRITE,                                                                             \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                                                        \
  (void*) data_ptr                                                                                           \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_DEHUMIDIFICATION_CONTROL_DEHUMIDIFICATION_HYSTERESIS_ID(data_ptr) \
{                                                                                                            \
  ZB_ZCL_ATTR_DEHUMIDIFICATION_CONTROL_DEHUMIDIFICATION_HYSTERESIS_ID,                                       \
  ZB_ZCL_ATTR_TYPE_U8,                                                                                       \
  ZB_ZCL_ATTR_ACCESS_READ_WRITE,                                                                             \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                                                        \
  (void*) data_ptr                                                                                           \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_DEHUMIDIFICATION_CONTROL_DEHUMIDIFICATION_MAX_COOL_ID(data_ptr)   \
{                                                                                                            \
  ZB_ZCL_ATTR_DEHUMIDIFICATION_CONTROL_DEHUMIDIFICATION_MAX_COOL_ID,                                         \
  ZB_ZCL_ATTR_TYPE_U8,                                                                                       \
  ZB_ZCL_ATTR_ACCESS_READ_WRITE,                                                                             \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                                                        \
  (void*) data_ptr                                                                                           \
}

/*! @internal Number of attributes mandatory for reporting in Dehumidification Control cluster */
#define ZB_ZCL_DEHUMIDIFICATION_CONTROL_REPORT_ATTR_COUNT 1

/*! @} */ /* Dehumidification Control cluster internals */
/*! @}
 *  @endcond */ /* internals_doc */

/*! @} */ /* ZCL HA Dehumidification Control cluster definitions */

/** @endcond */ /* DOXYGEN_ZCL_SECTION */

void zb_zcl_dehumidification_control_init_server(void);
void zb_zcl_dehumidification_control_init_client(void);
#define ZB_ZCL_CLUSTER_ID_DEHUMID_CONTROL_SERVER_ROLE_INIT zb_zcl_dehumidification_control_init_server
#define ZB_ZCL_CLUSTER_ID_DEHUMID_CONTROL_CLIENT_ROLE_INIT zb_zcl_dehumidification_control_init_client

#endif /* ZB_ZCL_DEHUMID_CONTROL_H */
