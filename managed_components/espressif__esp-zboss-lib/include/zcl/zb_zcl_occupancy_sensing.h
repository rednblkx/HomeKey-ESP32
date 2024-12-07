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
/* PURPOSE: Occupancy Sensing definitoins
*/

#ifndef ZB_ZCL_OCCUPANCY_SENSING_H
#define ZB_ZCL_OCCUPANCY_SENSING_H 1

#include "zcl/zb_zcl_common.h"
#include "zcl/zb_zcl_commands.h"

/** @cond DOXYGEN_ZCL_SECTION */

/** @addtogroup ZB_ZCL_OCCUPANCY_SENSING
 *  @{
 *    @details
 *    Occupancy Sensing cluster has no cluster-specific command support. Cluster
 *    attributes can be queried with @ref ZB_ZCL_COMMANDS "common commands".
 */

/*! @name Occupancy Sensing cluster attributes
    @{
*/

/*! @brief Occupancy Sensing cluster attribute identifiers
    @see ZCL spec, subclause 4.8.2.2
*/
enum zb_zcl_occupancy_sensing_attr_e
{
  /** Occupancy attribute identifier */
  ZB_ZCL_ATTR_OCCUPANCY_SENSING_OCCUPANCY_ID                                         = 0x0000,
  /** Occupancy Sensor Type attribute identifier */
  ZB_ZCL_ATTR_OCCUPANCY_SENSING_OCCUPANCY_SENSOR_TYPE_ID                             = 0x0001,
  /** The OccupancySensorTypeBitmap attribute specifies the types of the occupancy
   *  sensor. */
  ZB_ZCL_ATTR_OCCUPANCY_SENSING_OCCUPANCY_SENSOR_TYPE_BITMAP_ID                      = 0x0002,
  /** PIROccupiedToUnoccupiedDelay identifier */
  ZB_ZCL_ATTR_OCCUPANCY_SENSING_PIR_OCC_TO_UNOCC_DELAY_ID                            = 0x0010,
  /** PIRUnoccupiedToOccupiedDelay identifier */
  ZB_ZCL_ATTR_OCCUPANCY_SENSING_PIR_UNOCC_TO_OCC_DELAY_ID                            = 0x0011,
  /** PIRUnoccupiedToOccupiedThreshold identifier */
  ZB_ZCL_ATTR_OCCUPANCY_SENSING_PIR_UNOCC_TO_OCC_THRESHOLD_ID                        = 0x0012,
  /** The UltrasonicOccupiedToUnoccupiedDelay attribute is 16 bits in length and
   *  specifies the time delay, in seconds, before the Ultrasonic sensor changes to its
   *  unoccupied state after the last detection of movement in the sensed area. */
  ZB_ZCL_ATTR_OCCUPANCY_SENSING_ULTRASONIC_OCCUPIED_TO_UNOCCUPIED_DELAY_ID           = 0x0020,
  /** The UltrasonicUnoccupiedToOccupiedDelay attribute is 16 bits in length and
   *  specifies the time delay, in seconds, before the Ultrasonic sensor changes
   *  to its occupied state after the detection of movement in the sensed area. */
  ZB_ZCL_ATTR_OCCUPANCY_SENSING_ULTRASONIC_UNOCCUPIED_TO_OCCUPIED_DELAY_ID           = 0x0021,
  /** The UltrasonicUnoccupiedToOccupiedThreshold attribute is 8 bits in length and
   *  specifies the number of movement detection events that must occur in the period
   *  UltrasonicUnoccupiedToOccupiedDelay, before the Ultrasonic sensor changes to its
   *  occupied state. */
  ZB_ZCL_ATTR_OCCUPANCY_SENSING_ULTRASONIC_UNOCCUPIED_TO_OCCUPIED_THRESHOLD_ID       = 0x0022,
  /** The PhysicalContactOccupiedToUnoccupiedDelay attribute is 16 bits in length and
   *  specifies the time delay, in seconds, before the physical contact occupancy sensor
   *  changes to its unoccupied state after detecting the unoccupied event. */
  ZB_ZCL_ATTR_OCCUPANCY_SENSING_PHYSICAL_CONTACT_OCCUPIED_TO_UNOCCUPIED_DELAY_ID     = 0x0030,
  /** The PhysicalContactUnoccupiedToOccupiedDelay attribute is 16 bits in length and
   *  specifies the time delay, in seconds, before the physical contact sensor changes
   *  to its occupied state after the detection of the occupied event. */
  ZB_ZCL_ATTR_OCCUPANCY_SENSING_PHYSICAL_CONTACT_UNOCCUPIED_TO_OCCUPIED_DELAY_ID     = 0x0031,
  /** The PhysicalContactUnoccupiedToOccupiedThreshold attribute is 8 bits in length and
   *  specifies the number of movement detection events that must occur in the period
   *  PhysicalContactUnoccupiedToOccupiedDelay, before the PIR sensor changes to its
   *  occupied state. */
  ZB_ZCL_ATTR_OCCUPANCY_SENSING_PHYSICAL_CONTACT_UNOCCUPIED_TO_OCCUPIED_THRESHOLD_ID = 0x0032,
  /* Parking Sensor, Attributes for device Configuration, 2.5.1 */
  /** @cond internals_doc */
  ZB_ZCL_ATTR_OCCUPANCY_SENSING_CUSTOM_REPORTING_MODE_ID                                   = 0x8001,
  ZB_ZCL_ATTR_OCCUPANCY_SENSING_CUSTOM_DEBUG_MODE_ID                                       = 0x8002,
  ZB_ZCL_ATTR_OCCUPANCY_SENSING_CUSTOM_SENSING_INTERVAL_ID                                 = 0x8003,
  ZB_ZCL_ATTR_OCCUPANCY_SENSING_CUSTOM_MAX_NBR_CONFIRM_PINGS_ID                            = 0x8004,
  ZB_ZCL_ATTR_OCCUPANCY_SENSING_CUSTOM_CONFIRM_PING_DELAY_ID                               = 0x8005,
  ZB_ZCL_ATTR_OCCUPANCY_SENSING_CUSTOM_NBR_CONFIRM_PINGS_TO_VALID_ID                       = 0x8006,
  ZB_ZCL_ATTR_OCCUPANCY_SENSING_CUSTOM_LOCAL_TEMPERATURE_OVERRIDE_ID                       = 0x8007,
  ZB_ZCL_ATTR_OCCUPANCY_SENSING_CUSTOM_REMOTE_TEMPERATURE_VALUE_ID                         = 0x8008,
  ZB_ZCL_ATTR_OCCUPANCY_SENSING_CUSTOM_HUMIDITY_COMPENSATION_ID                            = 0x800A,
  ZB_ZCL_ATTR_OCCUPANCY_SENSING_CUSTOM_TRANDUCER_SENSITIVITY_CALIBRATION_SOURCE_ID         = 0x800B,
  ZB_ZCL_ATTR_OCCUPANCY_SENSING_CUSTOM_TRANDUCER_SENSITIVITY_CALIBRATION_OVERRIDE_VALUE_ID = 0x800C,
  ZB_ZCL_ATTR_OCCUPANCY_SENSING_CUSTOM_SENSOR_TIME_VS_GAIN_LUT_ID                          = 0x800D,
  ZB_ZCL_ATTR_OCCUPANCY_SENSING_CUSTOM_LEARN_ID                                            = 0x800E,
  ZB_ZCL_ATTR_OCCUPANCY_SENSING_CUSTOM_RINGDOWN_DISTANCE_ID                                = 0x800F,
  ZB_ZCL_ATTR_OCCUPANCY_SENSING_CUSTOM_REFERENCE_FLOOR_DISTANCE_ID                         = 0x8010,
  ZB_ZCL_ATTR_OCCUPANCY_SENSING_CUSTOM_DETECTION_ALGORITHM_SELECTOR_ID                     = 0x8011,
  ZB_ZCL_ATTR_OCCUPANCY_SENSING_CUSTOM_FLOOR_DEADBAND_ID                                   = 0x8012,
  ZB_ZCL_ATTR_OCCUPANCY_SENSING_CUSTOM_MAIN_OBJECT_DEADBAND_ID                             = 0x8013,
  ZB_ZCL_ATTR_OCCUPANCY_SENSING_CUSTOM_DETECTION_THRESHOLD_ID                              = 0x8014,
  ZB_ZCL_ATTR_OCCUPANCY_SENSING_CUSTOM_FORCE_REPORT_ID                                     = 0x801C,

  /* Parking Sensor, Reportable Attributes, 2.5.2 */
  ZB_ZCL_ATTR_OCCUPANCY_SENSING_CUSTOM_LOCAL_TEMPERATURE_VALUE_ID              = 0x8009,
  ZB_ZCL_ATTR_OCCUPANCY_SENSING_CUSTOM_CONFIDENCE_ID                           = 0x8015,
  ZB_ZCL_ATTR_OCCUPANCY_SENSING_CUSTOM_ECHO_RETURN_ARRAY_ID                    = 0x8017,
  ZB_ZCL_ATTR_OCCUPANCY_SENSING_CUSTOM_RX_SAMPLES_ARRAY_1_ID                   = 0x8018,
  ZB_ZCL_ATTR_OCCUPANCY_SENSING_CUSTOM_RX_SAMPLES_ARRAY_2_ID                   = 0x8019,
  ZB_ZCL_ATTR_OCCUPANCY_SENSING_CUSTOM_RX_SAMPLES_ARRAY_3_ID                   = 0x801A,
  ZB_ZCL_ATTR_OCCUPANCY_SENSING_CUSTOM_RX_SAMPLES_ARRAY_4_ID                   = 0x801B
  /*! @}
   *  @endcond */ /* internals_doc */
};

/** @brief Default value for Occupancy Sensing cluster revision global attribute */
#define ZB_ZCL_OCCUPANCY_SENSING_CLUSTER_REVISION_DEFAULT ((zb_uint16_t)0x0002u)

/** @brief Minimal value for PIROccToUnoccDelay attribute */
#define ZB_ZCL_OCCUPANCY_SENSING_PIR_OCC_TO_UNOCC_DELAY_MIN_VALUE ((zb_uint16_t)0x0000)

/** @brief Maximal value for PIROccToUnoccDelay attribute */
#define ZB_ZCL_OCCUPANCY_SENSING_PIR_OCC_TO_UNOCC_DELAY_MAX_VALUE ((zb_uint16_t)0xfffe)

/** @brief Default value for PIROccToUnoccDelay attribute */
#define ZB_ZCL_OCCUPANCY_SENSING_PIR_OCC_TO_UNOCC_DELAY_DEFAULT_VALUE ZB_ZCL_OCCUPANCY_SENSING_PIR_OCC_TO_UNOCC_DELAY_MIN_VALUE

/** @brief Minimal value for PIRUnoccToOccDelay attribute */
#define ZB_ZCL_OCCUPANCY_SENSING_PIR_UNOCC_TO_OCC_DELAY_MIN_VALUE ((zb_uint16_t)0x0000)

/** @brief Maximal value for PIRUnoccToOccDelay attribute */
#define ZB_ZCL_OCCUPANCY_SENSING_PIR_UNOCC_TO_OCC_DELAY_MAX_VALUE ((zb_uint16_t)0xfffe)

/** @brief Default value for PIRUnoccToOccDelay attribute */
#define ZB_ZCL_OCCUPANCY_SENSING_PIR_UNOCC_TO_OCC_DELAY_DEFAULT_VALUE ZB_ZCL_OCCUPANCY_SENSING_PIR_UNOCC_TO_OCC_DELAY_MIN_VALUE

/** @brief Minimal value for PIRUnoccToOccThreshold attribute */
#define ZB_ZCL_OCCUPANCY_SENSING_PIR_UNOCC_TO_OCC_THRESHOLD_MIN_VALUE ((zb_uint8_t)0x01)

/** @brief Maximal value for PIRUnoccToOccThreshold attribute */
#define ZB_ZCL_OCCUPANCY_SENSING_PIR_UNOCC_TO_OCC_THRESHOLD_MAX_VALUE ((zb_uint8_t)0xfe)

/** @brief Default value for PIRUnoccToOccThreshold attribute */
#define ZB_ZCL_OCCUPANCY_SENSING_PIR_UNOCC_TO_OCC_THRESHOLD_DEFAULT_VALUE ZB_ZCL_OCCUPANCY_SENSING_PIR_UNOCC_TO_OCC_THRESHOLD_MIN_VALUE

/** @brief Minimal value for UltrasonicOccupiedToUnoccupiedDelay attribute */
#define ZB_ZCL_OCCUPANCY_SENSING_ULTRASONIC_OCCUPIED_TO_UNOCCUPIED_DELAY_MIN_VALUE ((zb_uint16_t)0x0000)

/** @brief Maximal value for UltrasonicOccupiedToUnoccupiedDelay attribute */
#define ZB_ZCL_OCCUPANCY_SENSING_ULTRASONIC_OCCUPIED_TO_UNOCCUPIED_DELAY_MAX_VALUE ((zb_uint16_t)0xfffe)

/** @brief Default value for UltrasonicOccupiedToUnoccupiedDelay attribute */
#define ZB_ZCL_OCCUPANCY_SENSING_ULTRASONIC_OCCUPIED_TO_UNOCCUPIED_DELAY_DEFAULT_VALUE ZB_ZCL_OCCUPANCY_SENSING_ULTRASONIC_OCCUPIED_TO_UNOCCUPIED_DELAY_MIN_VALUE

/** @brief Minimal value for UltrasonicUnoccupiedToOccupiedDelay attribute */
#define ZB_ZCL_OCCUPANCY_SENSING_ULTRASONIC_UNOCCUPIED_TO_OCCUPIED_DELAY_MIN_VALUE ((zb_uint16_t)0x0000)

/** @brief Maximal value for UltrasonicUnoccupiedToOccupiedDelay attribute */
#define ZB_ZCL_OCCUPANCY_SENSING_ULTRASONIC_UNOCCUPIED_TO_OCCUPIED_DELAY_MAX_VALUE ((zb_uint16_t)0xfffe)

/** @brief Default value for UltrasonicUnoccupiedToOccupiedDelay attribute */
#define ZB_ZCL_OCCUPANCY_SENSING_ULTRASONIC_UNOCCUPIED_TO_OCCUPIED_DELAY_DEFAULT_VALUE ZB_ZCL_OCCUPANCY_SENSING_ULTRASONIC_UNOCCUPIED_TO_OCCUPIED_DELAY_MIN_VALUE

/** @brief Minimal value for UltrasonicUnoccupiedToOccupiedThreshold attribute */
#define ZB_ZCL_OCCUPANCY_SENSING_ULTRASONIC_UNOCCUPIED_TO_OCCUPIED_THRESHOLD_MIN_VALUE ((zb_uint8_t)0x01)

/** @brief Maximal value for UltrasonicUnoccupiedToOccupiedThreshold attribute */
#define ZB_ZCL_OCCUPANCY_SENSING_ULTRASONIC_UNOCCUPIED_TO_OCCUPIED_THRESHOLD_MAX_VALUE ((zb_uint8_t)0xfe)

/** @brief Default value for UltrasonicUnoccupiedToOccupiedThreshold attribute */
#define ZB_ZCL_OCCUPANCY_SENSING_ULTRASONIC_UNOCCUPIED_TO_OCCUPIED_THRESHOLD_DEFAULT_VALUE ZB_ZCL_OCCUPANCY_SENSING_ULTRASONIC_UNOCCUPIED_TO_OCCUPIED_THRESHOLD_MIN_VALUE

/** @brief Default value for PhysicalContactOccupiedToUnoccupiedDelay attribute */
#define ZB_ZCL_OCCUPANCY_SENSING_PHYSICAL_CONTACT_OCCUPIED_TO_UNOCCUPIED_DELAY_DEFAULT_VALUE ((zb_uint16_t)0x0000)

/** @brief No-reporting value for PhysicalContactOccupiedToUnoccupiedDelay attribute */
#define ZB_ZCL_OCCUPANCY_SENSING_PHYSICAL_CONTACT_OCCUPIED_TO_UNOCCUPIED_DELAY_NO_REPORTING_VALUE ((zb_uint16_t)0xffff)

/** @brief Default value for PhysicalContactUnoccupiedToOccupiedDelay attribute */
#define ZB_ZCL_OCCUPANCY_SENSING_PHYSICAL_CONTACT_UNOCCUPIED_TO_OCCUPIED_DELAY_DEFAULT_VALUE ((zb_uint16_t)0x0000)

/** @brief No-reporting value for PhysicalContactUnoccupiedToOccupiedDelay attribute */
#define ZB_ZCL_OCCUPANCY_SENSING_PHYSICAL_CONTACT_UNOCCUPIED_TO_OCCUPIED_DELAY_NO_REPORTING_VALUE ((zb_uint16_t)0xffff)

/** @brief Minimal value for PhysicalContactUnoccupiedToOccupiedThreshold attribute */
#define ZB_ZCL_OCCUPANCY_SENSING_PHYSICAL_CONTACT_UNOCCUPIED_TO_OCCUPIED_THRESHOLD_MIN_VALUE ((zb_uint8_t)0x01)

/** @brief Maximal value for PhysicalContactUnoccupiedToOccupiedThreshold attribute */
#define ZB_ZCL_OCCUPANCY_SENSING_PHYSICAL_CONTACT_UNOCCUPIED_TO_OCCUPIED_THRESHOLD_MAX_VALUE ((zb_uint8_t)0xfe)

/** @brief Default value for PhysicalContactUnoccupiedToOccupiedThreshold attribute */
#define ZB_ZCL_OCCUPANCY_SENSING_PHYSICAL_CONTACT_UNOCCUPIED_TO_OCCUPIED_THRESHOLD_DEFAULT_VALUE ZB_ZCL_OCCUPANCY_SENSING_PHYSICAL_CONTACT_UNOCCUPIED_TO_OCCUPIED_THRESHOLD_DEFAULT_VALUE

/*! @brief Permissible values for Occupancy attribute
    @see ZCL spec, subclause 4.8.2.2.1.1
*/
enum zb_zcl_occupancy_sensing_occupancy_e
{
  /*! Unoccupied value */
  ZB_ZCL_OCCUPANCY_SENSING_OCCUPANCY_UNOCCUPIED = 0,
  /*! Occupied value */
  ZB_ZCL_OCCUPANCY_SENSING_OCCUPANCY_OCCUPIED   = 1
};

/* Custom Attributes values */

/*! @brief Maximal value for switch type attribute */
#define ZB_ZCL_OCCUPANCY_SENSING_OCCUPANCY_MAX_VALUE \
  (ZB_ZCL_OCCUPANCY_SENSING_OCCUPANCY_UNOCCUPIED)


/* Switch type attribute has no default value */

/*! @brief Permissible values for Occupancy Sensor Type attribute
    @see ZCL spec, subclause 4.8.2.2.1.2
*/
enum zb_zcl_occupancy_sensing_occupancy_sensor_type_e
{
  /*! PIR value */
  ZB_ZCL_OCCUPANCY_SENSING_OCCUPANCY_SENSOR_TYPE_PIR                = 0,
  /*! Ultrasonic value */
  ZB_ZCL_OCCUPANCY_SENSING_OCCUPANCY_SENSOR_TYPE_ULTRASONIC         = 1,
  /*! PIR and Ultrasonic value */
  ZB_ZCL_OCCUPANCY_SENSING_OCCUPANCY_SENSOR_TYPE_PIR_AND_ULTRASONIC = 2,
  /*! Reserved value */
  ZB_ZCL_OCCUPANCY_SENSING_OCCUPANCY_SENSOR_TYPE_RESERVED           = 3
};

/** @cond internals_doc */
enum zb_zcl_custom_reporting_mode_type_e
{
  ZB_ZCL_CUSTOM_OCCUPANCY_SENSING_REPORING_MODE_TYPE_REGULAR_REPORTING                = 0,
  ZB_ZCL_CUSTOM_OCCUPANCY_SENSING_REPORING_MODE_TYPE_LOW_POWER_REPORTING              = 1
};


enum zb_zcl_custom_debug_mode_type_e
{
  ZB_ZCL_OCCUPANCY_SENSING_CUSTOM_DEBUG_MODE_TYPE_SHORT                = 0,
  ZB_ZCL_OCCUPANCY_SENSING_CUSTOM_DEBUG_MODE_TYPE_ENHANCED             = 1,
  ZB_ZCL_OCCUPANCY_SENSING_CUSTOM_DEBUG_MODE_TYPE_DEBUG                = 2
};

enum zb_zcl_custom_learn_type_e
{
  ZB_ZCL_OCCUPANCY_SENSING_CUSTOM_LEARN_NOT_YET_PERFORMED       = 0,
  ZB_ZCL_OCCUPANCY_SENSING_CUSTOM_LEARN_IN_PROGRESS             = 1,
  ZB_ZCL_OCCUPANCY_SENSING_CUSTOM_LEARN_FINISHED_WITH_A_FAILURE = 2,
  ZB_ZCL_OCCUPANCY_SENSING_CUSTOM_LEARN_FINISHED_SUCCSESSFULLY  = 3
};


#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_OCCUPANCY_SENSING_OCCUPANCY_ID(data_ptr) \
  {                                                                                 \
    ZB_ZCL_ATTR_OCCUPANCY_SENSING_OCCUPANCY_ID,                                     \
    ZB_ZCL_ATTR_TYPE_8BITMAP,                                                       \
    ZB_ZCL_ATTR_ACCESS_READ_ONLY | ZB_ZCL_ATTR_ACCESS_REPORTING,                    \
    (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                             \
    (void*) data_ptr                                                                \
  }

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_OCCUPANCY_SENSING_OCCUPANCY_SENSOR_TYPE_ID(data_ptr) \
  {                                                                                             \
    ZB_ZCL_ATTR_OCCUPANCY_SENSING_OCCUPANCY_SENSOR_TYPE_ID,                                     \
    ZB_ZCL_ATTR_TYPE_8BIT_ENUM,                                                                 \
    ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                                               \
    (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                                         \
    (void*) data_ptr                                                                            \
  }

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_OCCUPANCY_SENSING_OCCUPANCY_SENSOR_TYPE_BITMAP_ID(data_ptr) \
  {                                                                                                    \
    ZB_ZCL_ATTR_OCCUPANCY_SENSING_OCCUPANCY_SENSOR_TYPE_BITMAP_ID,                                     \
    ZB_ZCL_ATTR_TYPE_8BITMAP,                                                                          \
    ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                                                      \
    (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                                                \
    (void*) data_ptr                                                                                   \
  }
/** @internal Number of attributes mandatory for reporting on Occupancy Sensing cluster */
#define ZB_ZCL_OCCUPANCY_SENSING_REPORT_ATTR_COUNT 1

/*! @}
 *  @endcond */ /* internals_doc */

/** @brief Declare attribute list for Occupancy Sensing cluster
    @param attr_list - attribute list name
    @param occupancy - pointer to variable to store Occupancy attribute value
    @param occupancy_sensor_type - pointer to variable to store Occupancy Sensor Type attribute value
    @param occupancy_sensor_type_bitmap - pointer to variable to store Occupancy Sensor Type Bitmap attribute value
*/
#define ZB_ZCL_DECLARE_OCCUPANCY_SENSING_ATTRIB_LIST(attr_list, occupancy, occupancy_sensor_type, occupancy_sensor_type_bitmap) \
  ZB_ZCL_START_DECLARE_ATTRIB_LIST_CLUSTER_REVISION(attr_list, ZB_ZCL_OCCUPANCY_SENSING)                                        \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_OCCUPANCY_SENSING_OCCUPANCY_ID, (occupancy))                                                 \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_OCCUPANCY_SENSING_OCCUPANCY_SENSOR_TYPE_ID, (occupancy_sensor_type))                         \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_OCCUPANCY_SENSING_OCCUPANCY_SENSOR_TYPE_BITMAP_ID, (occupancy_sensor_type_bitmap))           \
  ZB_ZCL_FINISH_DECLARE_ATTRIB_LIST

/*! @} */ /* Occupancy Sensing cluster attribute structures */

/*! @} */ /* ZCL Occupancy Sensing cluster definitions */

/** @endcond */ /* DOXYGEN_ZCL_SECTION */

#define ZB_ZCL_CLUSTER_ID_OCCUPANCY_SENSING_SERVER_ROLE_INIT (zb_zcl_cluster_init_t)NULL
#define ZB_ZCL_CLUSTER_ID_OCCUPANCY_SENSING_CLIENT_ROLE_INIT (zb_zcl_cluster_init_t)NULL

#endif /* ZB_ZCL_OCCUPANCY_SENSING_H */
