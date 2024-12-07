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
/* PURPOSE: Analog Value cluster definitions */

#ifndef ZB_ZCL_ANALOG_VALUE_H
#define ZB_ZCL_ANALOG_VALUE_H 1

#include "zcl/zb_zcl_common.h"
#include "zcl/zb_zcl_commands.h"

/** @cond DOXYGEN_ZCL_SECTION */

/** @addtogroup ZB_ZCL_ANALOG_VALUE
 *  @{
 *    @details
 *    The Analog Value (Basic) cluster provides an interface for setting
 *    an analog value, typically used as a control system parameter, and
 *    accessing various characteristics of that value.
 */


/* ZB_ZCL_CLUSTER_ID_ANALOG_VALUE = 0x000e defined in zb_zcl_common.h ZCL spec 3.14.4 */

/** @name Analog Value cluster attributes
 *  @{
 */

/** @brief Analog Value cluster attribute identifiers
 *  @see ZCL spec, Analog Value (Basic) Cluster 3.14.4.4.2
 */
enum zb_zcl_analog_value_attr_e
{
  /** @brief Description attribute, ZCL spec 3.14.11.4 */
  ZB_ZCL_ATTR_ANALOG_VALUE_DESCRIPTION_ID        = 0x001c,
  /** @brief OutOfService attribute, ZCL spec 3.14.11.1 */
  ZB_ZCL_ATTR_ANALOG_VALUE_OUT_OF_SERVICE_ID     = 0x0051,
  /** @brief PresentValue attribute, ZCL spec 3.14.11.2 */
  ZB_ZCL_ATTR_ANALOG_VALUE_PRESENT_VALUE_ID      = 0x0055,

  // TODO: Support PriorityArray attribute
  // @brief PriorityArray attribute, ZCL spec 3.14.11.6
  //ZB_ZCL_ATTR_ANALOG_VALUE_PRIORITY_ARRAY_ID     = 0x0057,

  /** @brief Reliability attribute, ZCL spec 3.14.11.9 */
  ZB_ZCL_ATTR_ANALOG_VALUE_RELIABILITY_ID        = 0x0067,
  /** @brief RelinquishDefault attribute, ZCL spec 3.14.11.7 */
  ZB_ZCL_ATTR_ANALOG_VALUE_RELINQUISH_DEFAULT_ID = 0x0068,
  /** @brief StatusFlags attribute, ZCL spec 3.14.11.3 */
  ZB_ZCL_ATTR_ANALOG_VALUE_STATUS_FLAGS_ID       = 0x006f,
  /** @brief EngineeringUnits attribute, ZCL spec 3.14.11.10 */
  ZB_ZCL_ATTR_ANALOG_VALUE_ENGINEERING_UNITS_ID  = 0x0075,
  /** @brief ApplicationType attribute, ZCL spec 3.14.11.19 */
  ZB_ZCL_ATTR_ANALOG_VALUE_APPLICATION_TYPE_ID   = 0x0100,
};

/**
 *  @brief StatusFlags attribute values.
 *  @see ZCL spec 3.14.11.3 for details.
 */
enum zb_zcl_analog_value_status_flags_value_e
{
  ZB_ZCL_ANALOG_VALUE_STATUS_FLAG_NORMAL         = 0x00,  /**< Normal (default) state. */
  ZB_ZCL_ANALOG_VALUE_STATUS_FLAG_IN_ALARM       = 0x01,  /**< In alarm bit. */
  ZB_ZCL_ANALOG_VALUE_STATUS_FLAG_FAULT          = 0x02,  /**< Fault bit. */
  ZB_ZCL_ANALOG_VALUE_STATUS_FLAG_OVERRIDDEN     = 0x04,  /**< Overridden bit. */
  ZB_ZCL_ANALOG_VALUE_STATUS_FLAG_OUT_OF_SERVICE = 0x08,  /**< Out of service bit. */
};

/** @brief Reliability attribute values.
 *  @see ZCL spec 3.14.11.9 for details.
 */
enum zb_zcl_analog_value_reliability_value_e
{
  ZB_ZCL_ANALOG_VALUE_RELIABILITY_NO_FAULT_DETECTED   = 0x00, /**< No fault detected */
  ZB_ZCL_ANALOG_VALUE_RELIABILITY_NO_SENSOR           = 0x01, /**< No sensor */
  ZB_ZCL_ANALOG_VALUE_RELIABILITY_OVER_RANGE          = 0x02, /**< Over range */
  ZB_ZCL_ANALOG_VALUE_RELIABILITY_UNDER_RANGE         = 0x03, /**< Under range */
  ZB_ZCL_ANALOG_VALUE_RELIABILITY_OPEN_LOOP           = 0x04, /**< Open loop */
  ZB_ZCL_ANALOG_VALUE_RELIABILITY_SHORTED_LOOP        = 0x05, /**< Shorted loop */
  ZB_ZCL_ANALOG_VALUE_RELIABILITY_NO_OUTPUT           = 0x06, /**< No output */
  ZB_ZCL_ANALOG_VALUE_RELIABILITY_UNRELIABLE_OTHER    = 0x07, /**< Unreliable other */
  ZB_ZCL_ANALOG_VALUE_RELIABILITY_PROCESS_ERROR       = 0x08, /**< Process error */
  /* 0x09 is for multistate clusters only */
  ZB_ZCL_ANALOG_VALUE_RELIABILITY_CONFIGURATION_ERROR = 0x0a, /**< Configuration error */
};

/** @name Analog Value cluster application types
 *  @{
 *    @details
 *      The ApplicationType attribute indicates the specific application usage
 *      for the cluster. ApplicationType is subdivided into Group, Type and
 *      an Index number, as follows:
 *
 *      - Group = Bits 24 to 31
 *        An indication of the cluster this attribute is part of. Analog Value
 *        clusters have Group = 0x02.
 *
 *      - Type = Bits 16 to 23
 *        The physical quantity that the PresentValue attribute of the cluster
 *        represents.
 *
 *      - Index = Bits 0 to 15
 *        The specific application usage of the cluster.
 */

#define ZB_ZCL_AV_GROUP_ID 0x02
#define ZB_ZCL_AV_SET_APP_TYPE_WITH_ID(_type, _id) ((ZB_ZCL_AV_GROUP_ID << 24) | ((_type & 0xff) << 16) | (_id & 0xffff))

/** @defgroup ZB_ZCL_ANALOG_VALUE_APP_TYPES Analog Value cluster application types */
/** @{ */

/** @brief Type values for Analog Value cluster applications
 *  @see ZCL spec 3.14.11.19.3
 */
enum zb_zcl_av_application_types_e
{
  ZB_ZCL_AV_APP_TYPE_TEMPERATURE  = 0x00,  /**< Temperature */
  ZB_ZCL_AV_APP_TYPE_AREA         = 0x01,  /**< Area */
  ZB_ZCL_AV_APP_TYPE_MULTIPLIER   = 0x02,  /**< Multiplier */
  ZB_ZCL_AV_APP_TYPE_FLOW         = 0x03,  /**< Flow */
  ZB_ZCL_AV_APP_TYPE_OTHER        = 0xff,  /**< Other */
};

/** @name Analog Value cluster representing temperature
 *  @{
 */

/** @brief Values for 'Temperature in degrees Celsius' type of Analog Value cluster
 *  @see ZCL spec 3.14.11.19.3.1
 */
enum zb_zcl_av_temperature_in_degrees_celsius_e
{
  ZB_ZCL_AV_TEMPERATURE_SETPOINT_OFFSET_0                      /**< Setpoint Offset */
  = ZB_ZCL_AV_SET_APP_TYPE_WITH_ID(ZB_ZCL_AV_APP_TYPE_TEMPERATURE, 0x0000),
  ZB_ZCL_AV_TEMPERATURE_TEMP_DEADBAND,                         /**< Temp deadband */
  ZB_ZCL_AV_TEMPERATURE_OCCUPIED_HEATING_SETPOINT,             /**< Occupied heating setpoint */
  ZB_ZCL_AV_TEMPERATURE_UNOCCUPIED_HEATING_SETPOINT,           /**< Unoccupied heating setpoint */
  ZB_ZCL_AV_TEMPERATURE_OCCUPIED_COOLING_SETPOINT,             /**< Occupied cooling setpoint */
  ZB_ZCL_AV_TEMPERATURE_UNOCCUPIED_COOLING_SETPOINT,           /**< Unoccupied cooling setpoint */
  ZB_ZCL_AV_TEMPERATURE_STANDBY_HEAT_SETPOINT,                 /**< Standby heat setpoint */
  ZB_ZCL_AV_TEMPERATURE_STANDBY_COOLING_SETPOINT,              /**< Standby cooling setpoint */
  ZB_ZCL_AV_TEMPERATURE_EFFECTIVE_OCCUPIED_HEATING_SETPOINT,   /**< Effective occupied heating setpoint */
  ZB_ZCL_AV_TEMPERATURE_EFFECTIVE_UNOCCUPIED_HEATING_SETPOINT, /**< Effective unoccupied heating setpoint */
  ZB_ZCL_AV_TEMPERATURE_EFFECTIVE_OCCUPIED_COOLING_SETPOINT,   /**< Effective occupied cooling setpoint */
  ZB_ZCL_AV_TEMPERATURE_EFFECTIVE_UNOCCUPIED_COOLING_SETPOINT, /**< Effective unoccupied cooling setpoint */
  ZB_ZCL_AV_TEMPERATURE_EFFECTIVE_STANDBY_HEAT_SETPOINT,       /**< Effective standby heat setpoint */
  ZB_ZCL_AV_TEMPERATURE_EFFECTIVE_STANDBY_COOLING_SETPOINT,    /**< Effective standby cooling setpoint */
  ZB_ZCL_AV_TEMPERATURE_SETPOINT_OFFSET_2,                     /**< Setpoint offset 2 */
  ZB_ZCL_AV_TEMPERATURE_SETPOINT_SHIFT,                        /**< Setpoint shift */

  /* 0x0200 to 0xfffe are Vendor defined */

  ZB_ZCL_AV_TEMPERATURE_OTHER                                  /**< Other*/
  = ZB_ZCL_AV_SET_APP_TYPE_WITH_ID(ZB_ZCL_AV_APP_TYPE_TEMPERATURE, 0xffff)
};

/** @} */ /* end of ZB_ZCL_AV_TEMPERATURE_APP_TYPE group */

/** @name Analog Value cluster representing Area
 *  @{
 */

/** @brief Values for 'Area in Square Metres' type of Analog Value cluster
 *  @see 3.14.11.19.3.2
 */
enum zb_zcl_av_area_in_squares_meters
{
  ZB_ZCL_AV_AREA_DUCT_AREA   /**< Duct Area */
  = ZB_ZCL_AV_SET_APP_TYPE_WITH_ID(ZB_ZCL_AV_APP_TYPE_AREA, 0x0000),

  /* 0x0200 to 0xffffe are Vendor defined */

  ZB_ZCL_AV_AREA_OTHER       /**< Other */
  = ZB_ZCL_AV_SET_APP_TYPE_WITH_ID(ZB_ZCL_AV_APP_TYPE_AREA, 0xffff)
};

/** @} */ /* end of ZB_ZCL_AV_AREA_APP_TYPE group */

/** @brief Values for 'Multiplier - Number' type of Analog Value cluster
 *  @see 3.14.11.19.3.3
 */
enum zb_zcl_av_multiplier_number_e
{
  ZB_ZCL_AV_MULTIPLIER_DUCT_MULTIPLIER      /**< Duct Area */
  = ZB_ZCL_AV_SET_APP_TYPE_WITH_ID(ZB_ZCL_AV_APP_TYPE_MULTIPLIER, 0x0000),

  /* 0x0200 to 0xffffe are Vendor defined */

  ZB_ZCL_AV_MULTIPLIER_OTHER                /**< Other */
  = ZB_ZCL_AV_SET_APP_TYPE_WITH_ID(ZB_ZCL_AV_APP_TYPE_MULTIPLIER, 0xffff)
};

/** @} */ /* end of ZB_ZCL_AV_AREA_APP_TYPE group */

/** @name Analog Value cluster representing flow
 *  @{
 */

/** @brief Values for 'Flow in Litres per Second' type of Analog Value cluster
 *  @see ZCL spec 3.14.11.19.3.4
 */
enum zb_zcl_av_flow_in_liters_per_second_e
{
  ZB_ZCL_AV_FLOW_MINIMUM_AIR_FLOW             /**< Minimum Air Flow */
  = ZB_ZCL_AV_SET_APP_TYPE_WITH_ID(ZB_ZCL_AV_APP_TYPE_FLOW, 0x0000),
  ZB_ZCL_AV_FLOW_MAXIMUM_AIR_FLOW,            /**< Maximum Air Flow */
  ZB_ZCL_AV_FLOW_HEATING_MINIMUM_AIR_FLOW,    /**< Heating Minimum Air Flow */
  ZB_ZCL_AV_FLOW_HEATING_MAXIMUM_AIR_FLOW,    /**< Heating Maximum Air Flow */
  ZB_ZCL_AV_FLOW_STANDBY_MINIMUM_AIR_FLOW,    /**< Standby Minimum Air Flow */
  ZB_ZCL_AV_FLOW_STANDBY_MAXIMUM_AIR_FLOW,    /**< Standby Maximum Air Flow */

  /* 0x0200 to 0xfffe are Vendor defined */

  ZB_ZCL_AV_FLOW_OTHER                        /**< Other */
  = ZB_ZCL_AV_SET_APP_TYPE_WITH_ID(ZB_ZCL_AV_APP_TYPE_FLOW, 0xffff)
};

/** @} */ /* end of ZB_ZCL_AV_FLOW_APP_TYPE group */

/** @} */ /* end of ZB_ZCL_ANALOG_VALUE_APPLICATION_TYPES */

/** @brief Default value for Analog Value cluster revision global attribute */
#define ZB_ZCL_ANALOG_VALUE_CLUSTER_REVISION_DEFAULT ((zb_uint16_t)0x0001u)

/** @brief Default value for Description attribute */
#define ZB_ZCL_ANALOG_VALUE_DESCRIPTION_DEFAULT_VALUE {0}

/** @brief Default value for OutOfService attribute */
#define ZB_ZCL_ANALOG_VALUE_OUT_OF_SERVICE_DEFAULT_VALUE ZB_FALSE

/** @brief Default value for Reliability attribute */
#define ZB_ZCL_ANALOG_VALUE_RELIABILITY_DEFAULT_VALUE ZB_ZCL_ANALOG_VALUE_RELIABILITY_NO_FAULT_DETECTED

/** @brief Default value for StatusFlags attribute */
#define ZB_ZCL_ANALOG_VALUE_STATUS_FLAGS_DEFAULT_VALUE ZB_ZCL_ANALOG_VALUE_STATUS_FLAG_NORMAL

/** @brief StatusFlags attribute minimum value */
#define ZB_ZCL_ANALOG_VALUE_STATUS_FLAGS_MIN_VALUE 0

/** @brief StatusFlags attribute maximum value */
#define ZB_ZCL_ANALOG_VALUE_STATUS_FLAGS_MAX_VALUE 0x0f

/** @cond internals_doc */
/** @name Analog Value cluster internals
 *  Internal structures for Analog Value cluster
 *  @{
 */

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_ANALOG_VALUE_DESCRIPTION_ID(data_ptr) \
{                                                                   \
  ZB_ZCL_ATTR_ANALOG_VALUE_DESCRIPTION_ID,                          \
  ZB_ZCL_ATTR_TYPE_CHAR_STRING,                                     \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY | ZB_ZCL_ATTR_ACCESS_WRITE_OPTIONAL, \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                               \
  (void*) data_ptr                                                  \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_ANALOG_VALUE_OUT_OF_SERVICE_ID(data_ptr) \
{                                                                   \
  ZB_ZCL_ATTR_ANALOG_VALUE_OUT_OF_SERVICE_ID,                       \
  ZB_ZCL_ATTR_TYPE_BOOL,                                            \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY | ZB_ZCL_ATTR_ACCESS_WRITE_OPTIONAL, \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                               \
  (void*) data_ptr                                                  \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_ANALOG_VALUE_PRESENT_VALUE_ID(data_ptr) \
{                                                                   \
  ZB_ZCL_ATTR_ANALOG_VALUE_PRESENT_VALUE_ID,                        \
  ZB_ZCL_ATTR_TYPE_SINGLE,                                          \
  ZB_ZCL_ATTR_ACCESS_READ_WRITE,                                    \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                               \
  (void*) data_ptr                                                  \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_ANALOG_VALUE_RELIABILITY_ID(data_ptr) \
{                                                                   \
  ZB_ZCL_ATTR_ANALOG_VALUE_RELIABILITY_ID,                          \
  ZB_ZCL_ATTR_TYPE_8BIT_ENUM,                                       \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY | ZB_ZCL_ATTR_ACCESS_WRITE_OPTIONAL, \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                               \
  (void*) data_ptr                                                  \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_ANALOG_VALUE_RELINQUISH_DEFAULT_ID(data_ptr) \
{                                                                   \
  ZB_ZCL_ATTR_ANALOG_VALUE_RELINQUISH_DEFAULT_ID,                   \
  ZB_ZCL_ATTR_TYPE_SINGLE,                                          \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY | ZB_ZCL_ATTR_ACCESS_WRITE_OPTIONAL, \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                               \
  (void*) data_ptr                                                  \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_ANALOG_VALUE_STATUS_FLAGS_ID(data_ptr) \
{                                                                   \
  ZB_ZCL_ATTR_ANALOG_VALUE_STATUS_FLAGS_ID,                         \
  ZB_ZCL_ATTR_TYPE_8BITMAP,                                         \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                     \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                               \
  (void*) data_ptr                                                  \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_ANALOG_VALUE_ENGINEERING_UNITS_ID(data_ptr) \
{                                                                   \
  ZB_ZCL_ATTR_ANALOG_VALUE_ENGINEERING_UNITS_ID,                    \
  ZB_ZCL_ATTR_TYPE_16BIT_ENUM,                                      \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY | ZB_ZCL_ATTR_ACCESS_WRITE_OPTIONAL, \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                               \
  (void*) data_ptr                                                  \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_ANALOG_VALUE_APPLICATION_TYPE_ID(data_ptr) \
{                                                                   \
  ZB_ZCL_ATTR_ANALOG_VALUE_APPLICATION_TYPE_ID,                     \
  ZB_ZCL_ATTR_TYPE_U32,                                             \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                     \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                               \
  (void*) data_ptr                                                  \
}

/** Number of attributes mandatory for reporting in Analog Value cluster */
#define ZB_ZCL_ANALOG_VALUE_REPORT_ATTR_COUNT 0

/** @} */
/** @endcond */ /* Analog Value cluster internals */

/** @brief Declare attribute list for Analog Value cluster
 *  @param attr_list - attribute list name
 *  @param description - pointer to variable storing Description attribute value
 *  @param out_of_service - pointer to variable storing OutOfService attribute value
 *  @param present_value -  pointer to variable storing PresentValue attribute value
 *  @param reliability - pointer to variable storing Reliability attribute value
 *  @param relinquish_default - pointer to variable storing RelinquishDefault attribute value
 *  @param status_flags - pointer to variable storing StatusFlags attribute value
 *  @param engineering_units - pointer to variable storing EngineeringUnits attribute value
 *  @param app_type - pointer to variable storing ApplicationType attribute value
 */
#define ZB_ZCL_DECLARE_ANALOG_VALUE_ATTRIB_LIST(                                           \
    attr_list,                                                                             \
    description,                                                                           \
    out_of_service,                                                                        \
    present_value,                                                                         \
    reliability,                                                                           \
    relinquish_default,                                                                    \
    status_flags,                                                                          \
    engineering_units,                                                                     \
    app_type)                                                                              \
  ZB_ZCL_START_DECLARE_ATTRIB_LIST_CLUSTER_REVISION(attr_list, ZB_ZCL_ANALOG_VALUE)        \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_ANALOG_VALUE_DESCRIPTION_ID, (description))             \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_ANALOG_VALUE_OUT_OF_SERVICE_ID, (out_of_service))       \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_ANALOG_VALUE_PRESENT_VALUE_ID, (present_value))         \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_ANALOG_VALUE_RELIABILITY_ID, (reliability))             \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_ANALOG_VALUE_RELINQUISH_DEFAULT_ID, (relinquish_default)) \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_ANALOG_VALUE_STATUS_FLAGS_ID, (status_flags))           \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_ANALOG_VALUE_ENGINEERING_UNITS_ID, (engineering_units)) \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_ANALOG_VALUE_APPLICATION_TYPE_ID, (app_type))           \
  ZB_ZCL_FINISH_DECLARE_ATTRIB_LIST

/*! Maximum length of Description string field */
#define ZB_ZCL_ANALOG_VALUE_DESCRIPTION_MAX_LEN 16

/**
 *  @brief Analog Value cluster attributes
 */
typedef struct zb_zcl_analog_value_attrs_s
{
  /** @copydoc ZB_ZCL_ATTR_ANALOG_VALUE_DESCRIPTION_ID
   * @see ZB_ZCL_ATTR_ANALOG_VALUE_DESCRIPTION_ID
   */
  zb_char_t description[ZB_ZCL_ANALOG_VALUE_DESCRIPTION_MAX_LEN + 1];

  /** @copydoc ZB_ZCL_ATTR_ANALOG_VALUE_OUT_OF_SERVICE_ID
   * @see ZB_ZCL_ATTR_ANALOG_VALUE_OUT_OF_SERVICE_ID
   */
  zb_bool_t out_of_service;

  /** @copydoc ZB_ZCL_ATTR_ANALOG_VALUE_PRESENT_VALUE_ID
   * @see ZB_ZCL_ATTR_ANALOG_VALUE_PRESENT_VALUE_ID
   */
  zb_single_t present_value;

  /** @copydoc ZB_ZCL_ATTR_ANALOG_VALUE_RELIABILITY_ID
   * @see ZB_ZCL_ATTR_ANALOG_VALUE_RELIABILITY_ID
   */
  zb_uint8_t reliability;

  /** @copydoc ZB_ZCL_ATTR_ANALOG_VALUE_RELINQUISH_DEFAULT_ID
   * @see ZB_ZCL_ATTR_ANALOG_VALUE_RELINQUISH_DEFAULT_ID
   */
  zb_single_t relinquish_default;

  /** @copydoc ZB_ZCL_ATTR_ANALOG_VALUE_STATUS_FLAGS_ID
   * @see ZB_ZCL_ATTR_ANALOG_VALUE_STATUS_FLAGS_ID
   */
  zb_uint8_t status_flags;

  /** @copydoc ZB_ZCL_ATTR_ANALOG_VALUE_ENGINEERING_UNITS_ID
   * @see ZB_ZCL_ATTR_ANALOG_VALUE_ENGINEERING_UNITS_ID
   */
  zb_uint16_t engineering_units;

  /** @copydoc ZB_ZCL_ATTR_ANALOG_VALUE_APPLICATION_TYPE_ID
   * @see ZB_ZCL_ATTR_ANALOG_VALUE_APPLICATION_TYPE_ID
   */
  zb_uint32_t application_type;
} zb_zcl_analog_value_attrs_t;

/** @brief Declare attribute list for Analog Value cluster
 *  @param[in]  attr_list - attribute list variable name
 *  @param[in]  attrs - variable of @ref zb_zcl_analog_value_attrs_t type
 *                      (containing Analog Value cluster attributes)
 */
#define ZB_ZCL_DECLARE_ANALOG_VALUE_ATTR_LIST(attr_list, attrs)           \
  ZB_ZCL_DECLARE_ANALOG_VALUE_ATTRIB_LIST(attr_list,                      \
                                        &attrs.description,               \
                                        &attrs.out_of_service,            \
                                        &attrs.present_value,             \
                                        &attrs.reliability,               \
                                        &attrs.relinquish_default,        \
                                        &attrs.status_flags,              \
                                        &attrs.engineering_units,         \
                                        &attrs.application_type)

/** @} */ /* end of ZB_ZCL_ANALOG_VALUE_ATTRS_GROUP group */

/** @name Analog Value cluster attribute value manipulation API
 *  @{
 */

/** @brief Set normal operating mode
 *  @param ep - endpoint number
 */
#define ZB_ZCL_ANALOG_VALUE_SET_NORMAL_MODE(ep)                           \
{                                                                         \
  zb_uint8_t val;                                                         \
                                                                          \
  val = ZB_FALSE;                                                         \
  ZB_ZCL_SET_ATTRIBUTE(ep, ZB_ZCL_CLUSTER_ID_ANALOG_VALUE,                \
                       ZB_ZCL_CLUSTER_SERVER_ROLE,                        \
                       ZB_ZCL_ATTR_ANALOG_VALUE_OUT_OF_SERVICE_ID,        \
                       &val, ZB_FALSE);                                   \
                                                                          \
  val = ZB_ZCL_ANALOG_VALUE_STATUS_FLAG_NORMAL;                           \
  ZB_ZCL_SET_ATTRIBUTE(ep, ZB_ZCL_CLUSTER_ID_ANALOG_VALUE,                \
                       ZB_ZCL_CLUSTER_SERVER_ROLE,                        \
                       ZB_ZCL_ATTR_ANALOG_VALUE_STATUS_FLAGS_ID,          \
                       &val, ZB_FALSE);                                   \
}

/** @brief Set Out of service operating mode
 *  @param ep - endpoint number
 */
#define ZB_ZCL_ANALOG_VALUE_SET_OUT_OF_SERVICE(ep)                                          \
{                                                                                           \
  zb_zcl_attr_t *attr_desc;                                                                 \
  zb_uint8_t val;                                                                           \
                                                                                            \
  val = ZB_TRUE;                                                                            \
  ZB_ZCL_SET_ATTRIBUTE(ep, ZB_ZCL_CLUSTER_ID_ANALOG_VALUE,                                  \
                       ZB_ZCL_CLUSTER_SERVER_ROLE,                                          \
                       ZB_ZCL_ATTR_ANALOG_VALUE_OUT_OF_SERVICE_ID,                          \
                       &val, ZB_FALSE);                                                     \
                                                                                            \
  attr_desc = zb_zcl_get_attr_desc_a(ep, ZB_ZCL_CLUSTER_ID_ANALOG_VALUE,                    \
                                     ZB_ZCL_CLUSTER_SERVER_ROLE,                            \
                                     ZB_ZCL_ATTR_ANALOG_VALUE_STATUS_FLAGS_ID);             \
  if (attr_desc)                                                                            \
  {                                                                                         \
    val = *(zb_uint8_t*)attr_desc->data_p | ZB_ZCL_ANALOG_VALUE_STATUS_FLAG_OUT_OF_SERVICE; \
    ZB_ZCL_SET_ATTRIBUTE(ep, ZB_ZCL_CLUSTER_ID_ANALOG_VALUE,                                \
                         ZB_ZCL_CLUSTER_SERVER_ROLE,                                        \
                         ZB_ZCL_ATTR_ANALOG_VALUE_STATUS_FLAGS_ID,                          \
                         &val, ZB_FALSE);                                                   \
  }                                                                                         \
}

/** @brief Set overridden operating mode
 * @param ep - endpoint number
 */
#define ZB_ZCL_ANALOG_VALUE_SET_OVERRIDDEN_MODE(ep)                                    \
{                                                                                      \
  zb_zcl_attr_t *attr_desc;                                                            \
  zb_uint8_t val;                                                                      \
                                                                                       \
  attr_desc = zb_zcl_get_attr_desc_a(ep, ZB_ZCL_CLUSTER_ID_ANALOG_VALUE,               \
                                     ZB_ZCL_CLUSTER_SERVER_ROLE,                       \
                                     ZB_ZCL_ATTR_ANALOG_VALUE_STATUS_FLAGS_ID);        \
  if (attr_desc)                                                                       \
  {                                                                                    \
    val = *(zb_uint8_t*)attr_desc->data_p | ZB_ZCL_ANALOG_VALUE_STATUS_FLAG_OVERRIDDEN;\
    ZB_ZCL_SET_ATTRIBUTE(ep, ZB_ZCL_CLUSTER_ID_ANALOG_VALUE,                           \
                         ZB_ZCL_CLUSTER_SERVER_ROLE,                                   \
                         ZB_ZCL_ATTR_ANALOG_VALUE_STATUS_FLAGS_ID,                     \
                         &val, ZB_FALSE);                                              \
  }                                                                                    \
}

/** @brief Set fault status flag
 * @param ep - endpoint number
 */
#define ZB_ZCL_ANALOG_VALUE_SET_FAULT_STATUS(ep)                                       \
{                                                                                      \
  zb_zcl_attr_t *attr_desc;                                                            \
  zb_uint8_t val;                                                                      \
                                                                                       \
  attr_desc = zb_zcl_get_attr_desc_a(ep, ZB_ZCL_CLUSTER_ID_ANALOG_VALUE,               \
                                     ZB_ZCL_CLUSTER_SERVER_ROLE,                       \
                                     ZB_ZCL_ATTR_ANALOG_VALUE_STATUS_FLAGS_ID);        \
  if (attr_desc)                                                                       \
  {                                                                                    \
    val = *(zb_uint8_t*)attr_desc->data_p | ZB_ZCL_ANALOG_VALUE_STATUS_FLAG_FAULT;     \
    ZB_ZCL_SET_ATTRIBUTE(ep, ZB_ZCL_CLUSTER_ID_ANALOG_VALUE,                           \
                         ZB_ZCL_CLUSTER_SERVER_ROLE,                                   \
                         ZB_ZCL_ATTR_ANALOG_VALUE_STATUS_FLAGS_ID,                     \
                         &val, ZB_FALSE);                                              \
  }                                                                                    \
}

/** @} */ /* end of ZB_ZCL_ANALOG_VALUE_ATTR_API group */

/** @} */ /* ZCL Analog Value cluster definitions */

/** @endcond */ /* DOXYGEN_ZCL_SECTION */

void zb_zcl_analog_value_init_server(void);
void zb_zcl_analog_value_init_client(void);
#define ZB_ZCL_CLUSTER_ID_ANALOG_VALUE_SERVER_ROLE_INIT zb_zcl_analog_value_init_server
#define ZB_ZCL_CLUSTER_ID_ANALOG_VALUE_CLIENT_ROLE_INIT zb_zcl_analog_value_init_client

#endif /* ZB_ZCL_ANALOG_VALUE_H */    /**< H */
