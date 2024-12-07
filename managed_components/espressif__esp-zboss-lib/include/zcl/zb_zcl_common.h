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
/* PURPOSE: Zigbee cluster library types and macros common for all clusters
*/

#ifndef ZB_ZCL_COMMON_H
#define ZB_ZCL_COMMON_H 1

#include <zboss_api_core.h> /* int types, zb_bufid_t */

/** @cond DOXYGEN_ZCL_SECTION */

#ifdef DEBUG
/* If defined, samples included to zcl source files will be compiled - need for development only */
#define ZB_COMPILE_ZCL_SAMPLE
#endif


/** @addtogroup ZB_ZCL_COMMON
 *  @{
 */

/**
 *  @defgroup zb_zcl_common_types ZCL common types
 *  @{
 *    @details
 *    The section contains enumerations defining cluster identifiers and inter-device operation
 *    statuses, and a callback type used to notify application on attribute value change (see @ref
 *    write_attr_cmd "Write attributes" command description).
 */

/*************************** ZCL cluster definitions ***********************************/

/*! ZCL payload maximum size; is used to calculate number of
 *  available bytes in variable-size commands */
/* TODO: current max size is defined for the worst case (nwk security
 * enabled, long address is specified). Possibly - recalculate this
 * max size in run-time taking into account current parameters */
/* TODO: check this value - calculate it correctly!!! */
#define ZB_MAX_PHY_PAYLOAD_SIZE 127U
/*
  Let's count:
- max PHY payload - 127 bytes
- MAC hdr (w/o Pan ID completion, short src/dest) - (FC 2, Seq 1, Pan ID 2, DST addr 2, SRC addr
2, FCS 2) = 11b MAC
- NWK HDR 8 + 8*2 (addresses). Note: not use source routing, else have more fields.
- NWK security 18b
- APS HDR (without security) 8b
Total 61 (45 without ieee addresses).
So, rest is 127-61 = 66b
So, rest is 127-45 = 82b
 */
/* #define ZB_ZCL_MAX_PAYLOAD_SIZE 55 */
/*! Maximal payload size */
#define ZB_ZCL_HI_MAX_PAYLOAD_SIZE 66U
/*! Maximal payload size without long address sending */
#define ZB_ZCL_HI_WO_IEEE_MAX_PAYLOAD_SIZE 82U

struct zb_zcl_attr_s; /* Forward declaration */

/** @brief ZCL Cluster Init Handler. This handler is called on registering device context (@ref
    ZB_AF_REGISTER_DEVICE_CTX). Initialization of the cluster should include @ref
    zb_zcl_add_cluster_handlers call, if any of the cluster handlers are implemented.

    Cluster Init handler is bound to the cluster declaration via ZB_ZCL_CLUSTER_DESC macro. Every
    cluster should implement "<cluster_id>_<cluster_role>_INIT" macro, for example:
    @code
    #define ZB_ZCL_CLUSTER_ID_ON_OFF_SERVER_ROLE_INIT zb_zcl_on_off_init_server
    #define ZB_ZCL_CLUSTER_ID_ON_OFF_CLIENT_ROLE_INIT zb_zcl_on_off_init_client
    @endcode

    If cluster does not have any initialization steps and does not need any cluster handlers,
    Cluster Init handler may be NULL, for example:
    @code
    #define ZB_ZCL_CLUSTER_ID_OCCUPANCY_SENSING_SERVER_ROLE_INIT (zb_zcl_cluster_init_t)NULL
    #define ZB_ZCL_CLUSTER_ID_OCCUPANCY_SENSING_CLIENT_ROLE_INIT (zb_zcl_cluster_init_t)NULL
    @endcode
*/
typedef void (*zb_zcl_cluster_init_t)(void);
/** @brief ZCL Cluster Command Handler. This handler is called on incoming ZCL command for
    corresponding cluster.
    @param param - pointer to buffer with ZCL command

    @return ZB_TRUE if command was handled (no additional actions are needed),
            ZB_FALSE otherwise
*/
typedef zb_bool_t (*zb_zcl_cluster_handler_t)(zb_uint8_t param);
/** @brief ZCL Cluster Check Value Handler. This handler is called before attribute change (for
    corresponding cluster) and checks if new value is in correct range and can be applied.
    @param attr_id - ZCL Attribute ID
    @param endpoint - Device Endpoint
    @param value - pointer to the new Attribute Value

    @return RET_OK if check is passed successfully (attribute can be changed),
            RET_OUT_OF_RANGE if the value is out of range (TODO: describe),
            RET_ERROR otherwise
*/
typedef zb_ret_t (*zb_zcl_cluster_check_value_t)(zb_uint16_t attr_id, zb_uint8_t endpoint, zb_uint8_t *value);
/** @brief ZCL Cluster Write Attribute Hook Handler. This handler is called before attribute change
    (if any cluster-specific action needs to be bound to attribute change, it can be placed in this
    handler).
    @param endpoint - Device Endpoint
    @param attr_id - ZCL Attribute ID
    @param new_value - pointer to the new Attribute Value
    @param manuf_code - manufacturer specific code
*/
typedef void (*zb_zcl_cluster_write_attr_hook_t)(zb_uint8_t endpoint, zb_uint16_t attr_id, zb_uint8_t *new_value, zb_uint16_t manuf_code);

/** @brief ZCL cluster description. */
typedef ZB_PACKED_PRE struct zb_zcl_cluster_desc_s
{
  zb_uint16_t cluster_id; /**< ZCL 16-bit cluster id. @see zcl_cluster_id */
  zb_uint16_t attr_count;  /**< Attributes number supported by the cluster */
  struct zb_zcl_attr_s *attr_desc_list; /**< List of cluster attributes,
         declared using ZB_ZCL_DECLARE_<CLUSTER_NAME>_ATTRIB_LIST() macro */
  zb_uint8_t role_mask;   /**< Cluster role, refer to zcl_cluster_role */
  zb_uint16_t manuf_code; /**< Manufacturer code for cluster and its attributes */
  zb_zcl_cluster_init_t cluster_init;
} ZB_PACKED_STRUCT
zb_zcl_cluster_desc_t;

/**
 * @brief Add ZCL cluster handlers for Cluster ID and Cluster Role.
 *
 * @param cluster_id - ZCL Cluster ID
 * @param cluster_role - ZCL Cluster Role, see @ref zcl_cluster_role
 * @param cluster_check_value - "Check Value" handler, see @ref zb_zcl_cluster_check_value_t; may
 * be NULL if this handler is not needed
 * @param cluster_write_attr_hook - "Write Attr Hook" handler, see @ref
 * zb_zcl_cluster_write_attr_hook_t; may be NULL if this handler is not needed
 * @param cluster_handler - "Cluster Commands" handler, see @ref zb_zcl_cluster_handler_t; may
 * be NULL if this handler is not needed
 *
 * @return RET_OK if ZCL handlers were installed
 * @return RET_ALREADY_EXISTS if ZCL handlers are already installed (for given Cluster ID and Role),
 *         new handlers will not be installed in that case
 * @return RET_EMPTY if ZCL handlers were not installed
 */
zb_ret_t zb_zcl_add_cluster_handlers(zb_uint16_t cluster_id, zb_uint8_t cluster_role,
  zb_zcl_cluster_check_value_t cluster_check_value,
  zb_zcl_cluster_write_attr_hook_t cluster_write_attr_hook,
  zb_zcl_cluster_handler_t cluster_handler);

/** @cond internals_doc */
zb_zcl_cluster_handler_t zb_zcl_get_cluster_handler(zb_uint16_t cluster_id, zb_uint8_t cluster_role);
zb_zcl_cluster_check_value_t zb_zcl_get_cluster_check_value(zb_uint16_t cluster_id, zb_uint8_t cluster_role);
zb_zcl_cluster_write_attr_hook_t zb_zcl_get_cluster_write_attr_hook(zb_uint16_t cluster_id, zb_uint8_t cluster_role);

#ifdef ZB_CONTROL4_NETWORK_SUPPORT
/** Functions for adding and getting Control4 Network cluster specific handlers */
zb_ret_t zb_zcl_add_control4_cluster_handlers(zb_uint16_t cluster_id, zb_uint8_t cluster_role,
                                              zb_zcl_cluster_check_value_t cluster_check_value,
                                              zb_zcl_cluster_write_attr_hook_t cluster_write_attr_hook,
                                              zb_zcl_cluster_handler_t cluster_handler);

zb_zcl_cluster_handler_t zb_zcl_get_control4_cluster_handler(zb_uint16_t cluster_id, zb_uint8_t cluster_role);
zb_zcl_cluster_check_value_t zb_zcl_get_control4_cluster_check_value(zb_uint16_t cluster_id, zb_uint8_t cluster_role);
zb_zcl_cluster_write_attr_hook_t zb_zcl_get_control4_cluster_write_attr_hook(zb_uint16_t cluster_id, zb_uint8_t cluster_role);
#endif /* ZB_CONTROL4_NETWORK_SUPPORT */
/** @endcond */ /* internals_doc */

/**
 * @name Cluster roles in message exchange
 * @anchor zcl_cluster_role
 *
 * Note: These values were members of `enum zb_zcl_cluster_role_e` type but were converted to a
 * set of macros due to MISRA violations.
 */
/** @{ */
/** Server cluster role */
#define ZB_ZCL_CLUSTER_SERVER_ROLE 0x01U
/** Client cluster role */
#define ZB_ZCL_CLUSTER_CLIENT_ROLE 0x02U
/** Internal value - used for searching any role (on the moment when cluster role is not known yet). */
#define ZB_ZCL_CLUSTER_ANY_ROLE 0x03U
/** Reverts cluster role */
#define ZB_ZCL_REVERT_CLUSTER_ROLE(_d) \
  (((_d) != ZB_ZCL_CLUSTER_ANY_ROLE) ? (((_d) == ZB_ZCL_CLUSTER_SERVER_ROLE) ? ZB_ZCL_CLUSTER_CLIENT_ROLE : ZB_ZCL_CLUSTER_SERVER_ROLE) :  \
   ZB_ZCL_CLUSTER_ANY_ROLE)
/** @} */

/**
 * @name ZCL cluster identifiers
 * @anchor zcl_cluster_id
 * @see ZCL spec, subclause 2.2.2
 */
/** @{ */
#define ZB_ZCL_CLUSTER_ID_BASIC                0x0000U /*!< Basic cluster identifier. */
#define ZB_ZCL_CLUSTER_ID_POWER_CONFIG         0x0001U /*!< Power configuration cluster identifier. */
#define ZB_ZCL_CLUSTER_ID_DEVICE_TEMP_CONFIG   0x0002U /*!< Device temperature configuration cluster identifier. */
#define ZB_ZCL_CLUSTER_ID_IDENTIFY             0x0003U /*!< Identify cluster identifier. */
#define ZB_ZCL_CLUSTER_ID_GROUPS               0x0004U /*!< Groups cluster identifier. */
#define ZB_ZCL_CLUSTER_ID_SCENES               0x0005U /*!< Scenes cluster identifier. */
#define ZB_ZCL_CLUSTER_ID_ON_OFF               0x0006U /*!< On/Off cluster identifier. */
#define ZB_ZCL_CLUSTER_ID_ON_OFF_SWITCH_CONFIG 0x0007U /*!< On/Off switch configuration cluster identifier. */
#define ZB_ZCL_CLUSTER_ID_LEVEL_CONTROL        0x0008U /*!< Level control cluster identifier. */
#define ZB_ZCL_CLUSTER_ID_ALARMS               0x0009U /*!< Alarms cluster identifier. */
#define ZB_ZCL_CLUSTER_ID_TIME                 0x000aU /*!< Time cluster identifier. */
#define ZB_ZCL_CLUSTER_ID_RSSI_LOCATION        0x000bU /*!< RSSI location cluster identifier. */
#define ZB_ZCL_CLUSTER_ID_ANALOG_INPUT         0x000cU /*!< Analog input (basic) cluster identifier. */
#define ZB_ZCL_CLUSTER_ID_ANALOG_OUTPUT        0x000dU /*!< Analog output (basic) cluster identifier. */
#define ZB_ZCL_CLUSTER_ID_ANALOG_VALUE         0x000eU /*!< Analog value (basic) cluster identifier. */
#define ZB_ZCL_CLUSTER_ID_BINARY_INPUT         0x000fU /*!< Binary input (basic) cluster identifier. */
#define ZB_ZCL_CLUSTER_ID_BINARY_OUTPUT        0x0010U /*!< Binary output (basic) cluster identifier. */
#define ZB_ZCL_CLUSTER_ID_BINARY_VALUE         0x0011U /*!< Binary value (basic) cluster identifier. */
#define ZB_ZCL_CLUSTER_ID_MULTI_INPUT          0x0012U /*!< Multistate input (basic) cluster identifier. */
#define ZB_ZCL_CLUSTER_ID_MULTI_OUTPUT         0x0013U /*!< Multistate output (basic) cluster identifier. */
#define ZB_ZCL_CLUSTER_ID_MULTI_VALUE          0x0014U /*!< Multistate value (basic) cluster identifier. */
#define ZB_ZCL_CLUSTER_ID_COMMISSIONING        0x0015U /*!< Commissioning cluster identifier. */

#define ZB_ZCL_CLUSTER_ID_OTA_UPGRADE 0x0019U /*!< Over The Air cluster identifier. */

/* General clusters */
#define ZB_ZCL_CLUSTER_ID_POLL_CONTROL 0x0020U /*!< Poll control cluster identifier. */
#define ZB_ZCL_CLUSTER_ID_GREEN_POWER  0x0021U /*!< Green Power cluster identifier. */

#define ZB_ZCL_CLUSTER_ID_KEEP_ALIVE 0x0025U /*!< Keep Alive cluster identifier. */

/* Closures clusters */
#define ZB_ZCL_CLUSTER_ID_SHADE_CONFIG    0x0100U /*!< Shade configuration cluster identifier. */
#define ZB_ZCL_CLUSTER_ID_DOOR_LOCK       0x0101U /*!< Door lock cluster identifier. */
#define ZB_ZCL_CLUSTER_ID_WINDOW_COVERING 0x0102U /*!< Window covering cluster identifier. */
/* HVAC clusters */
#define ZB_ZCL_CLUSTER_ID_PUMP_CONFIG_CONTROL  0x200U /*!< Pump configuration and control cluster identifier. */
#define ZB_ZCL_CLUSTER_ID_THERMOSTAT           0x201U /*!< Thermostat cluster identifier. */
#define ZB_ZCL_CLUSTER_ID_FAN_CONTROL          0x202U /*!< Fan control cluster identifier. */
#define ZB_ZCL_CLUSTER_ID_DEHUMID_CONTROL      0x203U /*!< Dehumidification control cluster identifier. */
#define ZB_ZCL_CLUSTER_ID_THERMOSTAT_UI_CONFIG 0x204U /*!< Thermostat user interface configuration cluster identifier. */

/* Lighting clusters */
#define ZB_ZCL_CLUSTER_ID_COLOR_CONTROL  0x0300U /*!< Color control cluster identifier. */
#define ZB_ZCL_CLUSTER_ID_BALLAST_CONFIG 0x0301U /*!< Ballast configuration cluster identifier. */

/* Measurement and Sensing */
#define ZB_ZCL_CLUSTER_ID_ILLUMINANCE_MEASUREMENT  0x0400U /*!< Illuminance measurement */
#define ZB_ZCL_CLUSTER_ID_TEMP_MEASUREMENT         0x0402U /*!< Temperature measurement */
#define ZB_ZCL_CLUSTER_ID_PRESSURE_MEASUREMENT     0x0403U /*!< Pressure measurement */
#define ZB_ZCL_CLUSTER_ID_REL_HUMIDITY_MEASUREMENT 0x0405U /*!< Relative humidity measurement */
#define ZB_ZCL_CLUSTER_ID_OCCUPANCY_SENSING        0x0406U /*!< Occupancy sensing */

/* Measurement and Sensing: Concentration Measurement */
#define ZB_ZCL_CLUSTER_ID_CARBON_DIOXIDE_MEASUREMENT 0x040DU /*!< Carbon Dioxide (CO2) measurement */
#define ZB_ZCL_CLUSTER_ID_PM2_5_MEASUREMENT          0x042AU /*!< PM2.5 measurement */

/* IAS clusters */
#define ZB_ZCL_CLUSTER_ID_IAS_ZONE 0x0500U /*!< IAS Zone cluster identifier */
#define ZB_ZCL_CLUSTER_ID_IAS_ACE  0x0501U /*!< IAS ACE cluster identifier */
#define ZB_ZCL_CLUSTER_ID_IAS_WD   0x0502U /*!< IAS WD cluster identifier */

/* Smart Energy */
#define ZB_ZCL_CLUSTER_ID_PRICE             0x0700U /*!< Price cluster identifier. */
#define ZB_ZCL_CLUSTER_ID_DRLC              0x0701U /*!< Demand-Response cluster identifier. */
#define ZB_ZCL_CLUSTER_ID_METERING          0x0702U /*!< Metering cluster identifier. */
#define ZB_ZCL_CLUSTER_ID_MESSAGING         0x0703U /*!< Messaging cluster identifier. */
#define ZB_ZCL_CLUSTER_ID_TUNNELING         0x0704U /*!< Tunneling cluster identifier. */
#define ZB_ZCL_CLUSTER_ID_PREPAYMENT        0x0705U /*!< Prepayment cluster identifier. */
#define ZB_ZCL_CLUSTER_ID_ENERGY_MANAGEMENT 0x0706U /*!< Energy Management cluster identifier. */
#define ZB_ZCL_CLUSTER_ID_CALENDAR          0x0707U /*!< Calendar cluster identifier. */
#define ZB_ZCL_CLUSTER_ID_DEVICE_MANAGEMENT 0x0708U /*!< Device Management cluster identifier. */
#define ZB_ZCL_CLUSTER_ID_EVENTS            0x0709U /*!< Events cluster identifier. */
#define ZB_ZCL_CLUSTER_ID_MDU_PAIRING       0x070AU /*!< MDU Pairing cluster identifier. */
#define ZB_ZCL_CLUSTER_ID_SUB_GHZ           0x070BU /*!< Sub-GHz cluster identifier. */
#define ZB_ZCL_CLUSTER_ID_DAILY_SCHEDULE    0x070DU /*!< Daily Schedule cluster identifier. */
#define ZB_ZCL_CLUSTER_ID_KEY_ESTABLISHMENT 0x0800U /*!< Key Establishment cluster identifier. */


/* Home Automation */
#define ZB_ZCL_CLUSTER_ID_APPLIANCE_EVENTS_AND_ALERTS 0x0b02U /*!< Appliance events and alerts cluster identifier */
/* HA specU rev. 29U ver. 1.2 - Electricity MeasurementU not sure that its Electrical Measurement */
#define ZB_ZCL_CLUSTER_ID_ELECTRICAL_MEASUREMENT      0x0b04U /*!< Electrical Measurement cluster identifier. */
#define ZB_ZCL_CLUSTER_ID_DIAGNOSTICS                 0x0b05U /*!< Home Automation Diagnostics */

#define ZB_ZCL_CLUSTER_ID_WWAH                    0xFC57U /*!< Works with All Hubs cluster identifier */
/** @cond touchlink */
#define ZB_ZCL_CLUSTER_ID_TOUCHLINK_COMMISSIONING 0x1000U       /*!< Touchlink Commissioning cluster identifier */
/** @endcond */ /* touchlink */

/************** Manufacturer specific clusters ****************/
#define ZB_ZCL_CLUSTER_CUSTOM_ID_MIN_VAL 0xfc00U /*!< Manufacturer specific minimum custom cluster */

#define ZB_ZCL_CLUSTER_ID_TUNNEL     0xfc00U /*!< Manufacturer specific Tunnel cluster */
#define ZB_ZCL_CLUSTER_ID_IR_BLASTER 0xfc01U /*!< Manufacturer specific IR Blaster cluster */

#define ZB_ZCL_CLUSTER_ID_CUSTOM_ATTR 0xffeeU /*!< Custom Attributes */

#define ZB_ZCL_CLUSTER_ID_METER_IDENTIFICATION 0x0B01U /*!< Meter Identification cluster identifier */

/* TODO: define other clusters IDs */
/* Measurements and sensing clusters */
/* Security and safety clusters */

/** @} */

/**
 * @brief Type for cluster roles in message exchange.
 *
 * Holds one of @ref zcl_cluster_id. Kept for backward compatibility as
 * @ref zcl_cluster_id were declared previously as enum.
 */
typedef zb_uint16_t zb_zcl_cluster_id_t;

#ifdef ZB_CONTROL4_NETWORK_SUPPORT
#define ZB_ZCL_CLUSTER_ID_CONTROL4_NETWORKING ((zb_uint16_t)0x0001)
#endif

/** @brief Check whether cluster is manufacturer specific. */
#define ZB_ZCL_IS_CLUSTER_MANUF_SPEC(_cluster_id) ((_cluster_id) > 0xfbff)

/** @brief Internal invalid manufacturer code value. */
#define ZB_ZCL_MANUF_CODE_INVALID 0x0000


/** @brief Parse frame operation status. */
typedef enum zb_zcl_parse_status_e
{
  ZB_ZCL_PARSE_STATUS_SUCCESS,  /**< Frame parsed successfully. */
  ZB_ZCL_PARSE_STATUS_FAILURE   /**< Frame parse operation failed. */
} zb_zcl_parse_status_t;

/** @brief Attribute Reporting Status. */
typedef enum zb_zcl_attribute_reporting_status_e
{
  ZB_ZCL_AR_STATUS_PENDING  = 0x00,  /**< Pending. */
  ZB_ZCL_AR_STATUS_COMPLETE = 0x01  /**< Attribute Reporting Complete. */
} zb_zcl_attribute_reporting_status_t;

/** @brief Security use option. */
typedef enum zb_zcl_security_use_option_e
{
  ZB_ZCL_SU_DISABLED,
  ZB_ZCL_SU_ENABLED,
  ZB_ZCL_SU_AUTO
} zb_zcl_security_use_option_t;

/**
 * @name ZCL status values
 * @anchor zcl_status
 * @see ZCL8 spec, subclause 2.6.3, Table 2-12
 * @note The enumeration does not contain statuses not used in ZB HA profile
 */
/** @{ */
#define ZB_ZCL_STATUS_SUCCESS                       0x00U /*!< ZCL Success */
#define ZB_ZCL_STATUS_FAIL                          0x01U /*!< ZCL Fail */
#define ZB_ZCL_STATUS_NOT_AUTHORIZED                0x7EU /*!< The sender of the command does not have authorization to carry out this command. */
#define ZB_ZCL_STATUS_RESERVED                      0x7FU /*!< CCB 2318 a reserved field may be used in the future and then be non-zero. */
#define ZB_ZCL_STATUS_MALFORMED_CMD                 0x80U /*!< Malformed command */
#define ZB_ZCL_STATUS_UNSUP_CMD                     0x81U /*!< ZCL8: Unsupported command */
#define ZB_ZCL_STATUS_UNSUP_CLUST_CMD               0x81U /*!< ZCL7: Unsupported cluster command, ZCL8: use UNSUP_COMMAND*/
#define ZB_ZCL_STATUS_UNSUP_GEN_CMD                 0x82U /*!< ZCL7: Unsupported general command, ZCL8: use UNSUP_COMMAND*/
#define ZB_ZCL_STATUS_UNSUP_MANUF_CLUST_CMD         0x83U /*!< ZCL7: Unsupported manuf-specific clust command, ZCL8: use UNSUP_COMMAND*/
#define ZB_ZCL_STATUS_UNSUP_MANUF_GEN_CMD           0x84U /*!< ZCL7: Unsupported manuf-specific general command, ZCL8: use UNSUP_COMMAND*/
#define ZB_ZCL_STATUS_INVALID_FIELD                 0x85U /*!< Invalid field */
#define ZB_ZCL_STATUS_UNSUP_ATTRIB                  0x86U /*!< Unsupported attribute */
#define ZB_ZCL_STATUS_INVALID_VALUE                 0x87U /*!< Invalid value */
#define ZB_ZCL_STATUS_READ_ONLY                     0x88U /*!< Read only */
#define ZB_ZCL_STATUS_INSUFF_SPACE                  0x89U /*!< Insufficient space */
#define ZB_ZCL_STATUS_DUPE_EXISTS                   0x8aU /*!< ZCL7: Duplicate exists, ZCL8: use SUCCESS*/
#define ZB_ZCL_STATUS_NOT_FOUND                     0x8bU /*!< Not found */
#define ZB_ZCL_STATUS_UNREPORTABLE_ATTRIB           0x8cU /*!< Unreportable attribute */
#define ZB_ZCL_STATUS_INVALID_TYPE                  0x8dU /*!< Invalid type */
#define ZB_ZCL_STATUS_WRITE_ONLY                    0x8fU /*!< ZCL7: Write only, ZCL8: use NOT_AUTHORIZED*/
#define ZB_ZCL_STATUS_INCONSISTENT_STARTUP_STATE    0x90U /*!< ZCL7: The value can put the device in an inconsistent state on startup, ZCL8: FAILURE, CCB 2477 Status Code Cleanup: never used */
#define ZB_ZCL_STATUS_DEFINED_OUT_OF_BAND           0x91U /*!< ZCL7: Out-of-band method to write an attribute, ZCL8: FAILURE, CCB 2477 Status Code Cleanup: never used */
#define ZB_ZCL_STATUS_INCONSISTENT                  0x92U /*!< ZCL7: Supplied values are inconsistent, ZCL8: reserved, CCB 2477 Status Code Cleanup: never used */
/** ZCL7: The credentials presented by the device sending the command are not sufficient to perform this
 * action.
 * ZCL8: use FAILURE*/
#define ZB_ZCL_STATUS_ACTION_DENIED                 0x93U
#define ZB_ZCL_STATUS_TIMEOUT                       0x94U /*!< Timeout */
#define ZB_ZCL_STATUS_ABORT                         0x95U /*!< Abort */
#define ZB_ZCL_STATUS_INVALID_IMAGE                 0x96U /*!< Invalid OTA upgrade image */
#define ZB_ZCL_STATUS_WAIT_FOR_DATA                 0x97U /*!< Server does not have data block available yet */
/**< No OTA upgrade image available for a particular client. */
#define ZB_ZCL_STATUS_NO_IMAGE_AVAILABLE            0x98U
/** The client still requires more OTA upgrade image files in order to successfully upgrade. */
#define ZB_ZCL_STATUS_REQUIRE_MORE_IMAGE            0x99U
/** The command has been received and is being processed.*/
#define ZB_ZCL_STATUS_NOTIFICATION_PENDING          0x9AU
#define ZB_ZCL_STATUS_HW_FAIL                       0xc0U /*!< ZCL7: Hardware failure, ZCL8: use FAILURE */
#define ZB_ZCL_STATUS_SW_FAIL                       0xc1U /*!< ZCL7: Software failure, ZCL8: use FAILURE */
#define ZB_ZCL_STATUS_CALIB_ERR                     0xc2U /*!< ZCL7: Calibration error, ZCL8: reserved, CCB 2477 Status Code Cleanup: never used  */
#define ZB_ZCL_STATUS_UNSUP_CLUST                   0xc3U /*!< Cluster is not found on the target endpoint */
#define ZB_ZCL_STATUS_LIMIT_REACHED                 0xc4U /*!< ZCL7: Cluster is not found on the target endpoint, ZCL8: use SUCCESS */
/** @} */

/**
 * @brief Type for ZCL status values.
 *
 * Holds one of @ref zcl_status. Kept only for backward compatibility as
 * @ref zcl_status were declared previously as enum.
 */
typedef zb_uint8_t zb_zcl_status_t;

/** @brief ZCL global attribute: cluster revision returned by default.
    Used if the GLOBAL_CLUSTER_REVISION attribute is undefined for the cluster/role.
*/
#define ZB_ZCL_GLOBAL_CLUSTER_REVISION_DEFAULT 4

/** @brief ZCL pre-ZCL8 API default cluster revision (minimal Cluster revision)
*/
#define ZB_ZCL_CLUSTER_REV_MIN    ZB_ZCL_GLOBAL_CLUSTER_REVISION_DEFAULT

/** @brief ZCL unknown peer Cluster revision value
*/
#define ZB_ZCL_PEER_CLUSTER_REV_UNKNOWN 0xFFFFu

/**
 * @name ZCL backward compatibility modes
 * @anchor zcl_backward_compatibility_modes
 * @brief ZCL backward compatibility modes, introduced for ZCL8:
 *  ZB_ZCL_LEGACY_MODE (default mode) - pre-ZCL8 API sends packets in pre-ZCL8 format as is, no conversion applied
 *                                      ZCL8 API transforms packets into format according to the revision set in Cluster revision attribute value
 *  ZB_ZCL_AUTO_MODE - both pre-ZCL8 API and ZCL8 API transforms packets into format according to revision set in Cluster revision attribute value
 *  ZB_ZCL_COMPATIBILITY_MODE - both pre-ZCL8 API and ZCL8 API transforms packets into format according to peer Cluster revision provided by the application
 *                              if the peer Cluster revision is greater than Cluster revision attribute value, then the Cluster revision will be used
 *                              if there is no peer Cluster revision, the packet format is converted to the lowest possible Cluster revision
 */
/** @{ */
#define ZB_ZCL_LEGACY_MODE        0x00U /*!< ZBOSS ZCL legacy mode */
#define ZB_ZCL_AUTO_MODE          0x01U /*!< ZBOSS ZCL auto mode */
#define ZB_ZCL_COMPATIBILITY_MODE 0x02U /*!< ZBOSS ZCL compatibility mode */
/** @} */

/**
 * @name ZCL backward compatibility status modes
 * @anchor zcl_backward_compatibility_status_modes
 * @brief ZCL backward compatibility status modes, introduced for ZCL8:
 * ZCL8 spec merges some statuses defined in previous ZCL specs into one
 *   LIMIT_REACHED, DUPLICATE_EXISTS, SUCCESS ==> SUCCESS
 *   INCONSISTENT_STARTUP_STATE, DEFINED_OUT_OF_BAND, ACTION_DENIED, HARDWARE_FAILURE, SOFTWARE_FAILURE ==> FAILURE
 *   WRITE_ONLY, NOT_AUTHORIZED ==> NOT_AUTHORIZED
 *   MALFORMED_COMMAND, INVALID_FIELD ==> INVALID_FIELD
 *   UNSUP_CLUSTER_COMMAND, UNSUP_GENERAL_COMMAND, UNSUP_MANUF_CLUSTER_COMMAND, UNSUP_MANUF_GENERAL_COMMAND ==> UNSUP_COMMAND
 *   INCONSISTENT, CALIBRATION_ERROR, RESERVED ==> RESERVED
 * There is no well-defined mechanism how to define which version of ZCL is used by the peer. It looks like the optimal solution is to
 * send statuses according to ZCL6 and ZCL7 specifications. In that case there will be no miscommunication with legacy devices.
 * Other ZCL8 devices should process obsolete statuses correctly because of backward compatibility required by ZCL8 spec.
 * If application requires send statuses defined strictly according to ZCL8 spec, the device can be switch to this mode (and vice versa)
 * in runtime to ZCL8 statuses mode via zb_zcl_set_backward_compatible_statuses_mode API function
 *   ZB_ZCL_STATUSES_PRE_ZCL8_MODE (default mode) - statuses will be send according to pre ZCL8 specifications
 *   ZB_ZCL_STATUSES_ZCL8_MODE                    - statuses will be send according to ZCL8 specification
*/
/** @{ */
#define ZB_ZCL_STATUSES_PRE_ZCL8_MODE 0x00U /*!< ZBOSS ZCL statuses Pre ZCL8 mode */
#define ZB_ZCL_STATUSES_ZCL8_MODE     0x01U /*!< ZBOSS ZCL statuses ZCL8 mode */
/** @} */

#if defined ZB_ENABLE_HA

/** @brief Discovery manufacture specific command
    @param direction - direction 0 (client to server) or 1 (server to client)
    @param ep - endpoint number
    @param cluster_id - cluster ID
    @param manuf_id - manufacture ID
    @param recv_cmd_type - need list of received (ZB_TRUE) or generated (ZB_FALSE) command
    @param value - (out) pointer to list command ID

    @return number of value elements
*/
typedef zb_uint8_t (*zb_zcl_disc_manuf_cmd_cb_t)(
    zb_uint8_t direction, zb_uint8_t ep, zb_uint16_t cluster_id, zb_uint16_t manuf_id, zb_bool_t recv_cmd_type, zb_uint8_t **value);

#endif /* ZB_ENABLE_HA */

/********************************** ZCL attributes definitions ************************************/

/** @brief Informs application that some attribute value is being updated by Write attribute command
    @param ep - endpoint number
    @param cluster_id - cluster ID
    @param attr_id - attribute ID of the attribute being updated
    @param value - pointer to new data
*/
typedef void (*zb_zcl_modify_attr_value_cb_t)(
    zb_uint8_t ep, zb_uint16_t cluster_id, zb_uint16_t attr_id, zb_uint8_t *value);

/*! @defgroup zb_zcl_common_attrs General attributes' description
    Types and macros shared among all attributes' definitions.
    @{
    Use @ref ZB_ZCL_SET_ATTRIBUTE
    @par
*/

/** @brief Sets correctly ZCL char string value */
#define ZB_ZCL_SET_STRING_VAL(str, val, len)                                \
  (ZB_MEMCPY((zb_uint8_t*)(str) + 1, (val), (len)), *(zb_uint8_t*)(str) = (len))

/** @brief Get ZCL string length */
#define ZB_ZCL_GET_STRING_LENGTH(str) ((str)[0])

#define ZB_ZCL_SET_STRING_LENGTH(str, _newlen) (((str))[0] = (_newlen))

/** @brief Returns size of a string constant */
#define ZB_ZCL_STRING_CONST_SIZE(str) (zb_uint8_t)(sizeof(str) - 1)

/** Get pointer to first char in the string. */
#define ZB_ZCL_GET_STRING_BEGIN(_str) (&((_str)[1]))

/** Get pointer to (begin + length) char in the string. */
#define ZB_ZCL_GET_STRING_END(_str) (ZB_ZCL_GET_STRING_BEGIN(_str) + ZB_ZCL_GET_STRING_LENGTH(_str))

/** Get pointer to char in string at specified pos. */
#define ZB_ZCL_GET_STRING_REF_AT(_str, _pos) (ZB_ZCL_GET_STRING_BEGIN(_str) + _pos)

/** Get char by specified pos. */
#define ZB_ZCL_GET_STRING_CHAR_AT(_str, _pos) *ZB_ZCL_GET_STRING_REF_AT(_str, _pos)

/** Turn ZCL Octet string into string with null-term. */
#define ZB_ZCL_STRING_TO_C_STRING(_str)  \
{                                        \
  zb_uint8_t _len = _str[0];             \
  ZB_MEMMOVE(_str, _str + 1, _len);      \
  _str[_len] = 0;                        \
}

/** Set size of ZCL Octet String into 0 .*/
#define ZB_ZCL_STRING_CLEAR(_str) (_str[0] = 0)

/** Add some data to ZCL Octet String. */
static ZB_INLINE zb_uint16_t zb_zcl_string_append_data(zb_uint8_t *zcl_str,
                                                    zb_uint8_t zcl_str_max_size,
                                                    const zb_uint8_t *c_str,
                                                    zb_uint8_t c_str_len)
{
  zb_uint16_t newlen = ZB_ZCL_GET_STRING_LENGTH(zcl_str) + (zb_uint16_t)c_str_len;

  if (newlen < (zb_uint16_t) zcl_str_max_size)
  {
    ZB_MEMCPY(ZB_ZCL_GET_STRING_END(zcl_str), c_str, c_str_len);
    ZB_ZCL_SET_STRING_LENGTH(zcl_str, (zb_uint8_t)newlen);
  }
  else
  {
    newlen = 0;
  }

  return newlen;
}

/** Add single byte to ZCL Octet String. */
static ZB_INLINE zb_uint16_t zb_zcl_string_append_byte(zb_uint8_t *zcl_str,
                                                    zb_uint8_t zcl_str_max_size,
                                                    zb_uint8_t value)
{
  zb_uint16_t newlen = (zb_uint16_t)ZB_ZCL_GET_STRING_LENGTH(zcl_str) + 1U;

  if (newlen < (zb_uint16_t) zcl_str_max_size)
  {
    *ZB_ZCL_GET_STRING_END(zcl_str) = value;
    ZB_ASSERT(newlen <= ZB_UINT8_MAX);
    ZB_ZCL_SET_STRING_LENGTH(zcl_str, (zb_uint8_t)newlen);
  }
  else
  {
    newlen = 0;
  }

  return newlen;
}

/** Append C-string to ZCL string. */
#define ZB_ZCL_STRING_APPEND_C_STR(_zcl_str, _zcl_str_max_size, _c_str) \
  zb_zcl_string_append_data(_zcl_str, _zcl_str_max_size, (const zb_uint8_t *) _c_str, strlen(_c_str))

/** Append C-string to ZCL static string. */
#define ZB_ZCL_STATIC_STRING_APPEND_C_STR(_zcl_s_str, _c_str) \
  ZB_ZCL_STRING_APPEND_C_STR((zb_uint8_t *) _zcl_s_str, sizeof(_zcl_s_str), _c_str)

/** Append char to ZCL static string. */
#define ZB_ZCL_STATIC_STRING_APPEND_CHAR(_zcl_s_str, _char) \
  zb_zcl_string_append_byte(_zcl_s_str, sizeof(_zcl_s_str), (zb_uint8_t) _char)


/**
 * @name ZCL attribute data type values
 * @anchor zcl_attr_type
 * @note The enumeration is not full, and does not contain ZCL types not used in HA profile.
 * @see ZCL spec., subclause 2.5.2, Table 2-10. Data Types
 */
/** @{ */
#define ZB_ZCL_ATTR_TYPE_NULL              0x00U /*!< Null data type */
#define ZB_ZCL_ATTR_TYPE_8BIT              0x08U /*!< 8-bit value data type */
#define ZB_ZCL_ATTR_TYPE_16BIT             0x09U /*!< 16-bit value data type */
#define ZB_ZCL_ATTR_TYPE_24BIT             0x0aU /*!< 24-bit value data type */
#define ZB_ZCL_ATTR_TYPE_32BIT             0x0bU /*!< 32-bit value data type */
#define ZB_ZCL_ATTR_TYPE_40BIT             0x0cU /*!< 40-bit value data type */
#define ZB_ZCL_ATTR_TYPE_48BIT             0x0dU /*!< 48-bit value data type */
#define ZB_ZCL_ATTR_TYPE_56BIT             0x0eU /*!< 56-bit value data type */
#define ZB_ZCL_ATTR_TYPE_64BIT             0x0fU /*!< 64-bit value data type */
#define ZB_ZCL_ATTR_TYPE_BOOL              0x10U /*!< Boolean data type */
#define ZB_ZCL_ATTR_TYPE_8BITMAP           0x18U /*!< 8-bit bitmap data type */
#define ZB_ZCL_ATTR_TYPE_16BITMAP          0x19U /*!< 16-bit bitmap data type */
#define ZB_ZCL_ATTR_TYPE_24BITMAP          0x1aU /*!< 24-bit bitmap data type */
#define ZB_ZCL_ATTR_TYPE_32BITMAP          0x1bU /*!< 32-bit bitmap data type */
#define ZB_ZCL_ATTR_TYPE_40BITMAP          0x1cU /*!< 40-bit bitmap data type */
#define ZB_ZCL_ATTR_TYPE_48BITMAP          0x1dU /*!< 48-bit bitmap data type */
#define ZB_ZCL_ATTR_TYPE_56BITMAP          0x1eU /*!< 56-bit bitmap data type */
#define ZB_ZCL_ATTR_TYPE_64BITMAP          0x1fU /*!< 64-bit bitmap data type */
#define ZB_ZCL_ATTR_TYPE_U8                0x20U /*!< Unsigned 8-bit value data type */
#define ZB_ZCL_ATTR_TYPE_U16               0x21U /*!< Unsigned 16-bit value data type */
#define ZB_ZCL_ATTR_TYPE_U24               0x22U /*!< Unsigned 24-bit value data type */
#define ZB_ZCL_ATTR_TYPE_U32               0x23U /*!< Unsigned 32-bit value data type */
#define ZB_ZCL_ATTR_TYPE_U40               0x24U /*!< Unsigned 40-bit value data type */
#define ZB_ZCL_ATTR_TYPE_U48               0x25U /*!< Unsigned 48-bit value data type */
#define ZB_ZCL_ATTR_TYPE_U56               0x26U /*!< Unsigned 56-bit value data type */
#define ZB_ZCL_ATTR_TYPE_U64               0x27U /*!< Unsigned 64-bit value data type */
#define ZB_ZCL_ATTR_TYPE_S8                0x28U /*!< Signed 8-bit value data type */
#define ZB_ZCL_ATTR_TYPE_S16               0x29U /*!< Signed 16-bit value data type */
#define ZB_ZCL_ATTR_TYPE_S24               0x2aU /*!< Signed 24-bit value data type */
#define ZB_ZCL_ATTR_TYPE_S32               0x2bU /*!< Signed 32-bit value data type */
#define ZB_ZCL_ATTR_TYPE_S40               0x2cU /*!< Signed 40-bit value data type */
#define ZB_ZCL_ATTR_TYPE_S48               0x2dU /*!< Signed 48-bit value data type */
#define ZB_ZCL_ATTR_TYPE_S56               0x2eU /*!< Signed 56-bit value data type */
#define ZB_ZCL_ATTR_TYPE_S64               0x2fU /*!< Signed 64-bit value data type */
#define ZB_ZCL_ATTR_TYPE_8BIT_ENUM         0x30U /*!< 8-bit enumeration (U8 discrete) data type */
#define ZB_ZCL_ATTR_TYPE_16BIT_ENUM        0x31U /*!< 16-bit enumeration (U16 discrete) data type */
#define ZB_ZCL_ATTR_TYPE_SEMI              0x38U /*!< 2 byte floating point */
#define ZB_ZCL_ATTR_TYPE_SINGLE            0x39U /*!< 4 byte floating point */
#define ZB_ZCL_ATTR_TYPE_DOUBLE            0x3aU /*!< 8 byte floating point */
#define ZB_ZCL_ATTR_TYPE_OCTET_STRING      0x41U /*!< Octet string data type */
#define ZB_ZCL_ATTR_TYPE_CHAR_STRING       0x42U /*!< Character string (array) data type */
#define ZB_ZCL_ATTR_TYPE_LONG_OCTET_STRING 0x43U /*!< Long octet string */
#define ZB_ZCL_ATTR_TYPE_LONG_CHAR_STRING  0x44U /*!< Long character string */
#define ZB_ZCL_ATTR_TYPE_ARRAY             0x48U /*!< Array data type 2 + sum of content len */
#define ZB_ZCL_ATTR_TYPE_STRUCTURE         0x4cU /*!< Structure data type 2 + sum of content len */
#define ZB_ZCL_ATTR_TYPE_SET               0x50U /*!< Collection:set, size = sum of len of content */
#define ZB_ZCL_ATTR_TYPE_BAG               0x51U /*!< Collection:bag, size = sum of len of content */
#define ZB_ZCL_ATTR_TYPE_TIME_OF_DAY       0xe0U /*!< Time of day, 4 bytes */
#define ZB_ZCL_ATTR_TYPE_DATE              0xe1U /*!< Date, 4 bytes */
#define ZB_ZCL_ATTR_TYPE_UTC_TIME          0xe2U /*!< UTC Time, 4 bytes */
#define ZB_ZCL_ATTR_TYPE_CLUSTER_ID        0xe8U /*!< Cluster ID, 2 bytes */
#define ZB_ZCL_ATTR_TYPE_ATTRIBUTE_ID      0xe9U /*!< Attribute ID, 2 bytes */
#define ZB_ZCL_ATTR_TYPE_BACNET_OID        0xeaU /*!< BACnet OID, 4 bytes */
#define ZB_ZCL_ATTR_TYPE_IEEE_ADDR         0xf0U /*!< IEEE address (U64) type */
#define ZB_ZCL_ATTR_TYPE_128_BIT_KEY       0xf1U /*!< 128-bit security key */

/** Custom array of 16 elems data type */
#define ZB_ZCL_ATTR_TYPE_CUSTOM_16ARRAY 0x49U

/** Custom array of 32 elems data type (now is equal to ZB_ZCL_ATTR_TYPE_ARRAY) */
#define ZB_ZCL_ATTR_TYPE_CUSTOM_32ARRAY 0x4aU

#define ZB_ZCL_ATTR_TYPE_INVALID 0xffU /*!< Invalid data type */
/** @} */

/**
 * @brief Type for ZCL attribute data type values.
 *
 * @deprecated holds one of @ref zcl_attr_type. Kept only for backward compatibility as
 * @ref zcl_attr_type were declared previously as enum. Can be removed in future releases.
 */
typedef zb_uint8_t zb_zcl_attr_type_t;

/**
 * @name ZCL attribute access values
 * @anchor zcl_attr_access
 */
/** @{ */
#define ZB_ZCL_ATTR_ACCESS_READ_ONLY  0x01U
#define ZB_ZCL_ATTR_ACCESS_WRITE_ONLY 0x02U /*!< Attribute is write only */
/** Attribute is read/write */
#define ZB_ZCL_ATTR_ACCESS_READ_WRITE (ZB_ZCL_ATTR_ACCESS_READ_ONLY | ZB_ZCL_ATTR_ACCESS_WRITE_ONLY)
#define ZB_ZCL_ATTR_ACCESS_REPORTING  0x04U /*!< Attribute is allowed for reporting */
/** @cond internals_doc */
/** Attribute is read only, but may be marked as writable due to ZCL specification (using
 * ZB_ZCL_SET_ATTR_WRITABLE) */
#define ZB_ZCL_ATTR_ACCESS_WRITE_OPTIONAL (ZB_ZCL_ATTR_ACCESS_READ_ONLY)
/** @endcond */ /* internals_doc */
#define ZB_ZCL_ATTR_ACCESS_SINGLETON  0x08U /*!< Attribute is singleton */
#define ZB_ZCL_ATTR_ACCESS_SCENE      0x10U /*!< Attribute is accessed through scene */

/* Use free bit in access attribute field to save RAM */
#define ZB_ZCL_ATTR_MANUF_SPEC        0x20U /*!< Attribute is manufacturer specific */
#define ZB_ZCL_ATTR_ACCESS_INTERNAL   0x40U /*!< ZBOSS Internal access only Attribute */
/** @} */

/**
 * @brief Type for possible values of ZCL attribute access values.
 *
 * Holds one of @ref zcl_attr_access. Kept for backward compatibility as
 * @ref zcl_attr_access were declared previously as enum.
 */
typedef zb_uint8_t zb_zcl_attr_access_t;

#define ZB_ZCL_ATTR_SET_WITH_ATTR_ID(_set, _id) ((_set << 8) | (_id & 0xFF))

/** @cond internals_doc */

/**
 *  @brief Sets attribute access type to WRITABLE, if it is optionally permitted.
 *  @param attr_desc - pointer to attribute descriptor
 *  @hideinitializer
 */
#define ZB_ZCL_SET_ATTR_WRITABLE(attr_desc)                  \
{                                                            \
  if (attr_desc->access & ZB_ZCL_ATTR_ACCESS_WRITE_OPTIONAL) \
  {                                                          \
    attr_desc->access |= ZB_ZCL_ATTR_ACCESS_WRITE_ONLY;      \
  }                                                          \
}

/** @endcond */ /* internals_doc */

/*! @brief ZCL attribute definition structure */
typedef ZB_PACKED_PRE  struct zb_zcl_attr_s
{
  zb_uint16_t id;     /*!< Attribute id */
  zb_uint8_t type;    /*!< Attribute type see @ref zcl_attr_type */
  zb_uint8_t access;  /*!< Attribute access options according to @ref zcl_attr_access */
  zb_uint16_t manuf_code; /*!< Manufactirer specific ID */
  void* data_p;  /*!< Pointer to data */
} ZB_PACKED_STRUCT
zb_zcl_attr_t;


/**
 * @name Global attributes that are included to each cluster
 * @anchor zcl_attr_global
 * @see ZCL spec - 6.0
 */
/** @{ */
#define ZB_ZCL_ATTR_GLOBAL_CLUSTER_REVISION_ID           0xfffdU /*!< Cluster revision */
#define ZB_ZCL_ATTR_GLOBAL_ATTRIBUTE_REPORTING_STATUS_ID 0xfffeU /*!< Attribute reporting status */
/** @} */

/** @brief ZCL 6.0: Default value for cluster revision global attribute,
 *  see @ref zcl_attr_global
 */
#define ZB_ZCL_CLUSTER_REVISION_DEFAULT 4

/** @cond internals_doc */

/*! @internal @brief Set attribute description.
    @param attr_id - attribute identifier (defined individually for any particular cluster).
    @param data_ptr - pointer to attribute's value storage.

    Creates attribute description value (@ref zb_zcl_attr_s) initialized with provided pointer to
    attribute's data.
*/
#define ZB_ZCL_SET_ATTR_DESC(attr_id, data_ptr) ZB_SET_ATTR_DESCR_WITH_##attr_id(data_ptr),


/*! @internal @brief Set manufacturer specific attribute description.
    @param attr_id - attribute identifier (defined individually for any particular cluster).
    @param attr_type - attribute data type @see @ref zcl_attr_type
    @param attr_access - attribute access bitmask @see @ref zcl_attr_access
    @param manuf_code - attribute manufacturer specific ID
    @param data_ptr - pointer to attribute's value storage.

    Creates attribute description value (@ref zb_zcl_attr_s) initialized with provided pointer to
    attribute's data.
*/
#define ZB_ZCL_SET_MANUF_SPEC_ATTR_DESC(attr_id, attr_type, attr_access, manuf_code, data_ptr) \
  {                                                                                \
    attr_id,                                                                       \
    attr_type,                                                                     \
    attr_access | ZB_ZCL_ATTR_MANUF_SPEC,                                          \
    (manuf_code),                                                                  \
    (void*) data_ptr                                                               \
  },


/*! @internal @brief Start declaration of attributes list */
#define ZB_ZCL_START_DECLARE_ATTRIB_LIST(attrs_desc_name)                                          \
  zb_uint16_t cluster_revision_##attrs_desc_name = ZB_ZCL_CLUSTER_REVISION_DEFAULT;                \
  zb_zcl_attr_t attrs_desc_name [] = {                                                             \
  {                                                                                                \
    ZB_ZCL_ATTR_GLOBAL_CLUSTER_REVISION_ID,                                                        \
    ZB_ZCL_ATTR_TYPE_U16,                                                                          \
    ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                                                  \
    ZB_ZCL_NON_MANUFACTURER_SPECIFIC,                                                              \
    (void*) &(cluster_revision_##attrs_desc_name)                                                  \
  },

/*! @internal @brief Start declaration of static attributes list */
#define ZB_ZCL_START_DECLARE_ATTRIB_LIST_STATIC(attrs_desc_name)                                   \
  static zb_uint16_t cluster_revision_##attrs_desc_name = ZB_ZCL_CLUSTER_REVISION_DEFAULT;         \
  static zb_zcl_attr_t attrs_desc_name [] = {                                                      \
  {                                                                                                \
    ZB_ZCL_ATTR_GLOBAL_CLUSTER_REVISION_ID,                                                        \
    ZB_ZCL_ATTR_TYPE_U16,                                                                          \
    ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                                                  \
    ZB_ZCL_NON_MANUFACTURER_SPECIFIC,                                                              \
    (void*) &(cluster_revision_##attrs_desc_name)                                                  \
  },

/*! @internal @brief Start declaration of attributes list with cluster revision */
#define ZB_ZCL_START_DECLARE_ATTRIB_LIST_CLUSTER_REVISION(attrs_desc_name, cluster_name)           \
  zb_uint16_t cluster_revision_##attrs_desc_name = cluster_name##_CLUSTER_REVISION_DEFAULT;        \
  zb_zcl_attr_t attrs_desc_name [] = {                                                             \
  {                                                                                                \
    ZB_ZCL_ATTR_GLOBAL_CLUSTER_REVISION_ID,                                                        \
    ZB_ZCL_ATTR_TYPE_U16,                                                                          \
    ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                                                  \
    ZB_ZCL_NON_MANUFACTURER_SPECIFIC,                                                              \
    (void*) &(cluster_revision_##attrs_desc_name)                                                  \
  },

/*! @internal @brief Start declaration of static attributes list with cluster revision */
#define ZB_ZCL_START_DECLARE_ATTRIB_LIST_CLUSTER_REVISION_STATIC(attrs_desc_name, cluster_name)    \
  static zb_uint16_t cluster_revision_##attrs_desc_name = cluster_name##_CLUSTER_REVISION_DEFAULT; \
  static zb_zcl_attr_t attrs_desc_name [] = {                                                      \
  {                                                                                                \
    ZB_ZCL_ATTR_GLOBAL_CLUSTER_REVISION_ID,                                                        \
    ZB_ZCL_ATTR_TYPE_U16,                                                                          \
    ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                                                  \
    ZB_ZCL_NON_MANUFACTURER_SPECIFIC,                                                              \
    (void*) &(cluster_revision_##attrs_desc_name)                                                  \
  },

/*! @internal @brief End declaration of attributes list */
#define ZB_ZCL_FINISH_DECLARE_ATTRIB_LIST \
  {                                       \
    ZB_ZCL_NULL_ID,                       \
    0,                                    \
    0,                                    \
    ZB_ZCL_NON_MANUFACTURER_SPECIFIC,     \
    NULL                                  \
  }                                       \
}

#define ZB_ZCL_SET_ATTR_DESC_M(id, data_ptr, type, attr)                \
{                                                                       \
  (id), (type), (attr), ZB_ZCL_NON_MANUFACTURER_SPECIFIC, (data_ptr)    \
},

#if !(defined ZB_ZCL_DISABLE_REPORTING) || defined(DOXYGEN)

/*! @internal @brief Check, if attribute is reportable */
#define ZB_ZCL_IS_ATTR_REPORTABLE(attr_desc) ((attr_desc)->access & ZB_ZCL_ATTR_ACCESS_REPORTING)

#endif

/*! @internal @brief Check, if attribute is singleton */
#define ZB_ZCL_IS_ATTR_SINGLETON(attr_desc) ZB_BIT_IS_SET((attr_desc)->access, ZB_ZCL_ATTR_ACCESS_SINGLETON)

/*! @internal @brief Check, if attribute is accessed through scene */
#define ZB_ZCL_IS_ATTR_SCENE_ACCESSED(attr_desc) ZB_BIT_IS_SET((attr_desc)->access, ZB_ZCL_ATTR_ACCESS_SCENE)

/*! @internal @brief Check, if attribute is manufacturer specific */
#define ZB_ZCL_IS_ATTR_MANUF_SPEC(attr_desc) ZB_BIT_IS_SET((attr_desc)->access, ZB_ZCL_ATTR_MANUF_SPEC)

/*! Mark attribute ID to distinguish if it is common/manufacturer
    specific/internal attribute:
    - set bit 15 (the highest) for internal attributes
    - set bit 14 for manufacturer specific attributes
*/
/*! @internal @brief Mark attribute Id as custom/manufacture specific attribute */
#define ZB_ZCL_ATTR_INTERNAL_ID(_attr_id) ((_attr_id) | (1U << 15))

/*! @internal @brief Check if attribute Id is custom/manufacture specific */
#define ZB_ZCL_ATTR_CHECK_INTERNAL_ID(_attr_id) ((_attr_id) & (1U << 15))


/*! @internal @brief Mark attribute Id as custom/manufacture specific attribute */
#define ZB_ZCL_ATTR_CUSTOM_ID(_attr_id) ((_attr_id) | (1U << 14))

/*! @internal @brief Check if attribute Id is custom/manufacture specific */
#define ZB_ZCL_ATTR_CHECK_CUSTOM_ID(_attr_id) ((_attr_id) & (1U << 14))


/*! @internal @brief Start number for custom attributes */
#define ZB_ZCL_ATTR_CUSTOM_START_NUMBER    0xfff0

/*! @internal @brief Check boolean type attribute value */
#define ZB_ZCL_CHECK_BOOL_VALUE(x) ((x) == ZB_FALSE || (x) == ZB_TRUE)


/** @endcond */ /* internals_doc */

/**
 *  @brief Sets attribute value, perform all needed checks before and after setting new value,
 *  including read-only check and marking for reporting.
 *  @param ep - end point number.
 *  @param cluster_id - cluster identifier.
 *  @param cluster_role - role (see @ref zcl_cluster_role)
 *  @param attr_id - attribute identifier.
 *  @param value_ptr - pointer to new attribute value.
 *  @param check_access - boolean flag to specify if to perform access check or not.
 *  @hideinitializer
 */
#define ZB_ZCL_SET_ATTRIBUTE(ep, cluster_id, cluster_role, attr_id, value_ptr, check_access) \
  ZVUNUSED(zb_zcl_set_attr_val(ep, cluster_id, cluster_role, attr_id, value_ptr, check_access))


/**
 *  @brief Set attribute 8-bit value without any check. Use this macro
 *  on your own risk, if and only if you are absolutely sure that no check for Access
 *  rights, Reporting and attribute type is needed.
 *  @param attr_desc - pointer to an attribute description structure @ref zb_zcl_attr_s.
 *  @param value - 8-bit value to be set.
 *  @hideinitializer
 */
#define ZB_ZCL_SET_DIRECTLY_ATTR_VAL8(attr_desc, value)    \
  (*(zb_uint8_t*)attr_desc->data_p = (zb_uint8_t)(value))

/**
 *  @brief Set attribute 16-bit value without any check. Use this macro
 *  on your own risk, if and only if you are absolutely sure that no check for Access
 *  rights, Reporting and attribute type is needed.
 *  @param attr_desc - pointer to an attribute description structure @ref zb_zcl_attr_s.
 *  @param value - 16-bit value to be set.
 *  @hideinitializer
 */
#define ZB_ZCL_SET_DIRECTLY_ATTR_VAL16(attr_desc, value)  \
  (*(zb_uint16_t*)attr_desc->data_p = (zb_uint16_t)(value))

/**
 *  @brief Set attribute 32-bit value without any check. Use this macro
 *  on your own risk, if and only if you are absolutely sure that no check for Access
 *  rights, Reporting and attribute type is needed.
 *  @param attr_desc - pointer to an attribute description structure @ref zb_zcl_attr_s.
 *  @param value - 32-bit value to be set.
 *  @hideinitializer
 */
#define ZB_ZCL_SET_DIRECTLY_ATTR_VAL32(attr_desc, value)  \
  (*(zb_uint32_t*)attr_desc->data_p = (zb_uint32_t)(value))


/*! Get 8-bit unsigned attribute value (without any check) */
#define ZB_ZCL_GET_ATTRIBUTE_VAL_8(attr_desc)           \
  (*(zb_uint8_t*)attr_desc->data_p)

/*! Get 8-bit signed attribute value (without any check) */
#define ZB_ZCL_GET_ATTRIBUTE_VAL_S8(attr_desc)           \
  (*(zb_int8_t*)attr_desc->data_p)

/*! Get 16-bit unsigned attribute value (without any check) */
#define ZB_ZCL_GET_ATTRIBUTE_VAL_16(attr_desc)          \
  (*(zb_uint16_t*)attr_desc->data_p)

/*! Get 16-bit signed attribute value (without any check) */
#define ZB_ZCL_GET_ATTRIBUTE_VAL_S16(attr_desc)          \
  (*(zb_int16_t*)attr_desc->data_p)

/*! Get 32-bit unsigned attribute value (without any check) */
#define ZB_ZCL_GET_ATTRIBUTE_VAL_32(attr_desc)          \
  (*(zb_uint32_t*)attr_desc->data_p)

/*! Get 32-bit unsigned attribute value (without any check) */
#define ZB_ZCL_GET_ATTRIBUTE_VAL_S32(attr_desc)          \
  (*(zb_int32_t*)attr_desc->data_p)

/*! Get 32-bit unsigned attribute value (without any check) */
#define ZB_ZCL_GET_ATTRIBUTE_VAL_SINGLE(attr_desc)        \
  (*(zb_single_t*)attr_desc->data_p)


/*! @} */ /* General attributes' description */


/************************************** ZCL frame definitions ************************************/

/*! @defgroup zb_zcl_common_frame General frame description
    Types and macros required for ZCL frame manipulations
    @{

*/

/** @cond internals_doc */

/*! @brief ZCL header size */
#define ZB_ZCL_GET_HEADER_SIZE(frm_ctl)    \
  (   (ZB_ZCL_IS_MANUF_SPECIFIC(frm_ctl)) \
   ? sizeof(zb_zcl_frame_hdr_full_t)       \
   : sizeof(zb_zcl_frame_hdr_short_t))

/** @internal @brief Cuts ZCL header form a buffer. */
#define ZB_ZCL_CUT_HEADER(buf)                                                   \
  (void)zb_buf_cut_left(buf, ZB_ZCL_GET_HEADER_SIZE(*(zb_uint8_t*)zb_buf_begin(buf)))

/** @endcond */ /* internals_doc */

/** @name ZCL frame type
 * @brief Defines Permitted values for Frame Type subfield of Frame Control field in ZCL frame.
 * @anchor zcl_frame_type
 */
/** @{ */
#define ZB_ZCL_FRAME_TYPE_COMMON           0x00U /*!< Command acts across the entire profile  */
#define ZB_ZCL_FRAME_TYPE_CLUSTER_SPECIFIC 0x01U /*!< Command is specific to a cluster */
/** @} */

/**
 * @brief Type for ZCL frame type.
 *
 * @deprecated holds one of @ref zcl_frame_type. Kept only for backward compatibility as
 * @ref zcl_frame_type were declared previously as enum. Can be removed in future releases.
 */
typedef zb_uint8_t zb_zcl_frame_type_t;

/**
 * @name Values for Manufacturer-specific subfield of FCF in ZCL frame
 * @anchor zcl_manufacturer_specific
 * @see ZCL spec, subclause 2.3.1.1.2
 */
/** @{ */
/** Standard profile command. Manufacturer code field shall @b not be included into ZCL frame
 * header. */
#define ZB_ZCL_NOT_MANUFACTURER_SPECIFIC 0x00U
/** Command refers to manufacturer-specific profile extension. Manufacturer code field shall be
 * included into ZCL frame header. */
#define ZB_ZCL_MANUFACTURER_SPECIFIC     0x01U
/** @} */

/**
 * @brief Type for return statuses of mathematical operations.
 *
 * @deprecated holds one of @ref zcl_manufacturer_specific. Kept only for backward compatibility as
 * @ref zcl_manufacturer_specific were declared previously as enum. Can be removed in future
 * releases.
 */
typedef zb_uint8_t zb_zcl_manufacturer_specific_t;

#define ZB_ZCL_MANUFACTURER_WILDCARD_ID 0xFFFFU

#define ZB_ZCL_NON_MANUFACTURER_SPECIFIC 0xFFFFU

/**
 * @name Disable default response subfield of FCF in ZCL frame
 * @anchor zcl_disable_default_response
 * @see ZCL spec, subclause 2.3.1.1.3
 */
/** @{ */
/** Enable default response */
#define ZB_ZCL_ENABLE_DEFAULT_RESPONSE  0x00U
/** Disable default response */
#define ZB_ZCL_DISABLE_DEFAULT_RESPONSE 0x01U
/** @} */

/**
 * @brief Type for possible values of Disable default response subfield of FCF in ZCL frame.
 *
 * Holds one of @ref zcl_disable_default_response. Kept for backward compatibility as
 * @ref zcl_disable_default_response were declared previously as enum.
 */
typedef zb_uint8_t zb_zcl_disable_default_response_t;


/**
 * @name ZCL frame direction
 * @anchor zcl_frame_direction
 * @see ZCL spec, subclause 2.3.1.1.3.
 */
/** @{ */
/*! Command sent from client to a server */
#define ZB_ZCL_FRAME_DIRECTION_TO_SRV 0x00U
/*! Command sent from server to a client */
#define ZB_ZCL_FRAME_DIRECTION_TO_CLI 0x01U
/** @} */

/**
 * @brief Type for ZCL frame direction.
 *
 * Holds one of @ref zcl_frame_direction. Kept only for backward compatibility as
 * @ref zcl_frame_direction were declared previously as enum.
 */
typedef zb_uint8_t zb_zcl_frame_direction_t;

/*! @brief Reverts ZCL direction value */
#define ZB_ZCL_REVERT_DIRECTION(_d) \
  (((_d) == ZB_ZCL_FRAME_DIRECTION_TO_SRV) ? ZB_ZCL_FRAME_DIRECTION_TO_CLI : ZB_ZCL_FRAME_DIRECTION_TO_SRV)

/*! @brief ZCL frame control field
    @see ZCL spec, subclause 2.3.1.1
*/
typedef ZB_PACKED_PRE struct zb_zcl_frame_ctrl_s
{
  zb_bitfield_t frame_type:2;       /*!< Frame type @ref zcl_frame_type */
  zb_bitfield_t manufacturer:1;     /*!< Manufacturer specific frame @ref zcl_manufacturer_specific*/
  zb_bitfield_t direction:1;        /*!< Direction @ref zcl_frame_direction */
  zb_bitfield_t disable_def_resp:1; /*!< Disable default response */
  zb_bitfield_t reserved:3;
} ZB_PACKED_STRUCT zb_zcl_frame_ctrl_t;

/*! @brief ZCL frame header with manufacturer code
    @see ZCL spec, subclause 2.3.1
*/
typedef ZB_PACKED_PRE  struct zb_zcl_frame_hdr_full_s
{
  zb_zcl_frame_ctrl_t frame_ctrl;         /*!< Frame control filed see @ref zb_zcl_frame_ctrl_s */
  zb_uint16_t         manufacturer_code;  /*!< Manufacturer Code */
  zb_uint8_t          seq_number;         /*!< Transaction Sequence Number */
  zb_uint8_t          command_id;         /*!< Command Identifier Field */
} ZB_PACKED_STRUCT
zb_zcl_frame_hdr_full_t;

#define ZB_ZCL_FULL_HDR_SIZE sizeof(zb_zcl_frame_hdr_full_t)

/*! @brief ZCL frame header without manufacturer code
    @see ZCL spec, subclause 2.3.1
*/
typedef ZB_PACKED_PRE  struct zb_zcl_frame_hdr_short_s
{
  zb_zcl_frame_ctrl_t frame_ctrl;   /*!< Frame control field see @ref zb_zcl_frame_ctrl_s */
  zb_uint8_t          seq_number;   /*!< Transaction Sequence Number */
  zb_uint8_t          command_id;   /*!< Command Identifier Field */
} ZB_PACKED_STRUCT
zb_zcl_frame_hdr_short_t;

/**
 * @name ZCL address type
 * @brief Possible address types of devices from which ZCL message is received.
 * @anchor zcl_addr_type
 */
/** @{ */
#define ZB_ZCL_ADDR_TYPE_SHORT      0U
#define ZB_ZCL_ADDR_TYPE_IEEE_GPD   1U
#define ZB_ZCL_ADDR_TYPE_SRC_ID_GPD 2U
#define ZB_ZCL_ADDR_TYPE_IEEE       3U
/** @} */

/**
 * @name ZCL GPD address type
 * @brief Macro helps to see whether addr_type belongs to GPD subgroup
 */
/** @{ */
#define ZB_ZCL_ADDR_TYPE_IS_GPD(_addr_type)     \
  ((_addr_type) == ZB_ZCL_ADDR_TYPE_IEEE_GPD || \
   (_addr_type) == ZB_ZCL_ADDR_TYPE_SRC_ID_GPD)
/** @} */

/**
 * @brief Type to represent source address of ZCL message
 *
 * It is needed if ZGP sink is enabled and therefore messages can
 * be received from ZGPD devices which are identified by 32-bit
 * source Id.
 */
typedef ZB_PACKED_PRE struct zb_zcl_addr_s
{
  zb_uint8_t addr_type;  /**< address type (see @ref zcl_addr_type) */
  ZB_PACKED_PRE union zb_zcl_addr_u
  {
    zb_uint16_t    short_addr; /**< Zigbee short address */
    zb_uint32_t    src_id;     /**< Source ID of ZGPD */
    zb_ieee_addr_t ieee_addr;  /**< Full IEEE-address of ZGPD */
  } u;
} ZB_PACKED_STRUCT
zb_zcl_addr_t;

/*! Parsed ZCL header */
typedef ZB_PACKED_PRE  struct zb_zcl_parsed_hdr_s
{
  /* Values from APS data indication */
  ZB_PACKED_PRE union
  {
    ZB_PACKED_PRE struct
    {
      //zb_uint16_t src_addr;
      zb_zcl_addr_t source;
      zb_uint16_t dst_addr;
      zb_uint8_t  src_endpoint;
      zb_uint8_t  dst_endpoint;
      zb_uint8_t  fc;
#if (defined ZB_ENABLE_SE) || (defined ZB_ZCL_SUPPORT_CLUSTER_WWAH)
      zb_uint8_t aps_key_source:1;
      zb_uint8_t aps_key_attrs:2;
      zb_uint8_t aps_key_from_tc:1;
      zb_uint8_t reserved:4;
#endif
    } ZB_PACKED_STRUCT common_data;
#if defined ZB_ENABLE_ZLL
    ZB_PACKED_PRE struct
    {
/* 07/26/2017 EE CR:MINOR Kill that data structure. Utilize src_addr_ext. Kill upper one nesting level. */
      zb_ieee_addr_t src_addr;  /**< Sender device extended address. */
    } ZB_PACKED_STRUCT intrp_data;
#endif /* defined ZB_ENABLE_ZLL */
  } addr_data;
#if defined ZB_ENABLE_ZLL
  zb_int8_t rssi;          /**< RSSI of the packet. */
#endif /* defined ZB_ENABLE_ZLL */
  zb_uint16_t cluster_id;
  zb_uint16_t profile_id;

  /* ZCL specific fields */
  zb_uint8_t  cmd_id;                   /**< Command identifier. */
  zb_uint8_t  cmd_direction;            /**< Command direction identifier. */
  zb_uint8_t  seq_number;               /**< ZCL transaction sequence number. */
  zb_bool_t   is_common_command;        /**< "Common command" flag. */
  zb_bool_t   disable_default_response; /**< "Disable default response" flag. */
  zb_bool_t   is_manuf_specific;        /**< "Has manufacturer-specific data" flag. */
  zb_uint16_t manuf_specific;           /**< Manufacturer-specific data. */
} ZB_PACKED_STRUCT
zb_zcl_parsed_hdr_t;


/*! Attribute address info structure */
typedef struct zb_zcl_attr_addr_info_s
{
  zb_uint8_t src_ep;          /**< Source endpoint */
  zb_uint8_t dst_ep;          /**< Destination endpoint */
  zb_uint16_t dst_short_addr; /**< Destination short address */

  zb_uint16_t profile_id;     /**< Profile id */
  zb_uint16_t cluster_id;     /**< Cluster id */
  zb_uint8_t cluster_role;    /**< Cluster role */
  zb_uint16_t attr_id;        /**< Attribute id */
  zb_uint16_t manuf_code;
}
zb_zcl_attr_addr_info_t;


/** @brief Extract common addressing data from parsed ZCL structure.
  * @param header - pointer to the parsed ZCL header structure (see @ref zb_zcl_parsed_hdr_s).
  */
#define ZB_ZCL_PARSED_HDR_SHORT_DATA(header) ((header)->addr_data.common_data)

/**
 *  @brief Get ZCL frame type @ref zcl_frame_type.
 *  @hideinitializer
 */
#define ZB_ZCL_GET_FRAME_TYPE(v)     ((v) & 0x3U)

/**
 *  @brief Check is ZCL frame manufacturer specific @ref zcl_manufacturer_specific.
 *  @hideinitializer
 */
#define ZB_ZCL_GET_MANUF_SPECIFIC(v) (((v) >> 2U) & 0x1U)

#define ZB_ZCL_IS_MANUF_SPECIFIC(v) (ZB_ZCL_GET_MANUF_SPECIFIC(v) != 0U)

/**
 *  @brief Get ZCL frame direction @ref zcl_frame_direction.
 *  @hideinitializer
 */
#define ZB_ZCL_GET_DIRECTION(v)      (((v) >> 3U) & 0x1U)

/**
 *  @brief Get ZCL disable default response field @ref zcl_disable_default_response.
 *  @hideinitializer
 */
#define ZB_ZCL_GET_DISABLE_DEFAULT_RESPONSE(v) (((v) >> 4U) & 0x1U)

/**
 *  @brief Set ZCL frame type @ref zcl_frame_type.
 *  @hideinitializer
 */
#define ZB_ZCL_SET_FRAME_TYPE(v, val)     ((v) |= (val))

/**
 * @brief  Set ZCL frame manufacturer specific @ref zcl_manufacturer_specific.
 * @hideinitializer
 */
#define ZB_ZCL_SET_MANUF_SPECIFIC(v, val) ((v) |= ((val) << 2))

/**
 *  @brief Set ZCL frame direction @ref zcl_frame_direction.
 *  @hideinitializer
 */
#define ZB_ZCL_SET_DIRECTION(v, val)      ((v) |= ((val) << 3))

/**
 *  @brief Set ZCL disable default response field @ref zcl_disable_default_response.
 *  @hideinitializer
 */
#define ZB_ZCL_SET_DISABLE_DEFAULT_RESPONSE(v, val) ((v) |= ((val) << 4))

/**
  * @brief Construct ZCL command header in the buffer
  * @param zbbuf buffer ID
  * @param frame_ctl Frame Control field for ZCL packet
  * @param manuf_code manufacturer code, not used manufacturer-specific flag is not set
  * @param cmd_id ZCL command id
  * @param tsn pointer to variable (may be NULL) for TSN value of ZCL packet (generated by the function)
  *
  * Returns pointer to the buffer data begin after ZCL header (where command payload can be placed if needed).
  */
void *zb_zcl_start_command_header(zb_bufid_t zbbuf, zb_uint8_t frame_ctl, zb_uint16_t manuf_code, zb_uint8_t cmd_id, zb_uint8_t* tsn);

/** @cond internals_doc */

/** @internal @brief Construct ZCL header frame control value */
#define ZB_ZCL_CONSTRUCT_FRAME_CONTROL(                                 \
  frame_type, manuf_specific, direction, disable_default_resp)          \
  ((  (frame_type)                                                      \
      | ((manuf_specific) << 2)                                         \
      | ((direction) << 3)                                              \
      | ((disable_default_resp) << 4)))

#define ZB_ZCL_CONSTRUCT_SET_FRAME_CONTROL(                             \
    frame_ctl, frame_type, manuf_specific, direction, disable_default_resp) \
  ((frame_ctl) = ZB_ZCL_CONSTRUCT_FRAME_CONTROL((frame_type),           \
                                                (manuf_specific),       \
                                                (direction),            \
                                                (disable_default_resp)))

/* NOTE: We suppose all general requests/responses currently are not manufacturer specific */
/** @internal @brief Construct ZCL header frame control value for General command request */
#define ZB_ZCL_CONSTRUCT_GENERAL_COMMAND_REQ_FRAME_CONTROL(buf_ptr, def_resp) \
  (ZB_ZCL_CONSTRUCT_SET_FRAME_CONTROL(*buf_ptr,                                   \
    ZB_ZCL_FRAME_TYPE_COMMON,                                                 \
    ZB_ZCL_NOT_MANUFACTURER_SPECIFIC,                                         \
    ZB_ZCL_FRAME_DIRECTION_TO_SRV,                                            \
    (def_resp)),                                                              \
   buf_ptr++)

#define ZB_ZCL_CONSTRUCT_GENERAL_COMMAND_REQ_FRAME_CONTROL_A(buf_ptr,   \
                  direction, is_manufacturer_specific, def_resp)        \
  (ZB_ZCL_CONSTRUCT_SET_FRAME_CONTROL(*buf_ptr,                             \
    ZB_ZCL_FRAME_TYPE_COMMON,                                           \
    (is_manufacturer_specific),                                         \
    (direction),                                                        \
    (def_resp)),                                                        \
   buf_ptr++)

/*! @internal Construct ZCL header frame control value for General
 *  command request, specifying extra parameters */
#define ZB_ZCL_CONSTRUCT_GENERAL_COMMAND_REQ_FRAME_CONTROL_EXT(buf_ptr, \
                _is_manuf_specific, direction, def_resp)                \
  (ZB_ZCL_CONSTRUCT_SET_FRAME_CONTROL(*buf_ptr,                             \
    ZB_ZCL_FRAME_TYPE_COMMON,                                           \
    (_is_manuf_specific),                                               \
    (direction),                                                        \
    (def_resp)),                                                        \
   buf_ptr++)


/* To do not change existing code too much: 3 consequent macro create single function call. */
#define ZB_ZCL_START_PACKET_REQ(zbbuf)  zb_zcl_start_command_header((zbbuf),
#define ZB_ZCL_CONSTRUCT_SPECIFIC_COMMAND_REQ_FRAME_CONTROL(buf_ptr, def_resp) \
  ZB_ZCL_CONSTRUCT_FRAME_CONTROL(ZB_ZCL_FRAME_TYPE_CLUSTER_SPECIFIC, ZB_ZCL_NOT_MANUFACTURER_SPECIFIC, \
                                 ZB_ZCL_FRAME_DIRECTION_TO_SRV, (def_resp)), \
    0,                          /* No manuf_code */
#define ZB_ZCL_CONSTRUCT_COMMAND_HEADER_REQ(data_ptr, tsn, cmd_id) (cmd_id), NULL)

/* TODO: Implement via zb_zcl_start_command_header() */
#define ZB_ZCL_CONSTRUCT_SPECIFIC_COMMAND_REQ_FRAME_CONTROL_O(buf_ptr, def_resp) \
  (ZB_ZCL_CONSTRUCT_SET_FRAME_CONTROL(*buf_ptr,                                    \
    ZB_ZCL_FRAME_TYPE_CLUSTER_SPECIFIC,                                        \
    ZB_ZCL_NOT_MANUFACTURER_SPECIFIC,                                          \
    ZB_ZCL_FRAME_DIRECTION_TO_SRV,                                             \
    def_resp),                                                                 \
   buf_ptr++)

#define ZB_ZCL_CONSTRUCT_SPECIFIC_COMMAND_REQ_FRAME_CONTROL_A(buf_ptr, direction,                  \
                                                              is_manufacturer_specific, def_resp)  \
  (ZB_ZCL_CONSTRUCT_SET_FRAME_CONTROL(*(buf_ptr),                                                  \
                                      ZB_ZCL_FRAME_TYPE_CLUSTER_SPECIFIC,                          \
                                      ((is_manufacturer_specific) ? ZB_ZCL_MANUFACTURER_SPECIFIC   \
                                                            : ZB_ZCL_NOT_MANUFACTURER_SPECIFIC),   \
                                      (direction),                                                 \
                                      (def_resp)),                                                 \
   (buf_ptr)++)

#define ZB_ZCL_CONSTRUCT_GENERAL_COMMAND_RESP_FRAME_CONTROL_A(buf_ptr, direction,                  \
                                                              is_manufacturer_specific)            \
  (ZB_ZCL_CONSTRUCT_SET_FRAME_CONTROL(*(buf_ptr),                                                  \
                                      ZB_ZCL_FRAME_TYPE_COMMON,                                    \
                                      ((is_manufacturer_specific)                                  \
                                           ? ZB_ZCL_MANUFACTURER_SPECIFIC                          \
                                           : ZB_ZCL_NOT_MANUFACTURER_SPECIFIC),                    \
                                      (direction),                                                 \
                                      ZB_ZCL_DISABLE_DEFAULT_RESPONSE),                            \
   (buf_ptr)++)

#define ZB_ZCL_CONSTRUCT_SPECIFIC_COMMAND_RESP_FRAME_CONTROL_A(buf_ptr, direction,                 \
                                                               is_manufacturer_specific)           \
  (ZB_ZCL_CONSTRUCT_SET_FRAME_CONTROL(*(buf_ptr),                                                  \
                                      ZB_ZCL_FRAME_TYPE_CLUSTER_SPECIFIC,                          \
                                      ((is_manufacturer_specific)                                  \
                                           ? ZB_ZCL_MANUFACTURER_SPECIFIC                          \
                                           : ZB_ZCL_NOT_MANUFACTURER_SPECIFIC),                    \
                                      (direction),                                                 \
                                      ZB_ZCL_DISABLE_DEFAULT_RESPONSE),                            \
   (buf_ptr)++)

/*! @internal Construct ZCL header frame control valueZB_ZCL_CONSTRUCT_GENERAL_COMMAND_RESP_FRAME_CONTROL for General command response */
#define ZB_ZCL_CONSTRUCT_GENERAL_COMMAND_RESP_FRAME_CONTROL(buf_ptr)                               \
  (ZB_ZCL_CONSTRUCT_SET_FRAME_CONTROL(*(buf_ptr), ZB_ZCL_FRAME_TYPE_COMMON,                        \
                                      ZB_ZCL_NOT_MANUFACTURER_SPECIFIC,                            \
                                      ZB_ZCL_FRAME_DIRECTION_TO_CLI,                               \
                                      ZB_ZCL_DISABLE_DEFAULT_RESPONSE),                            \
   (buf_ptr)++)


/*! @internal Construct ZCL header frame control value for General
 *  command response, specifying extra parameters */
#define ZB_ZCL_CONSTRUCT_GENERAL_COMMAND_RESP_FRAME_CONTROL_EXT(_buf_ptr, _is_manuf_specific,      \
                                                                _direction)                        \
  (ZB_ZCL_CONSTRUCT_SET_FRAME_CONTROL(*(_buf_ptr),                                                 \
                                      ZB_ZCL_FRAME_TYPE_COMMON,                                    \
                                      ((_is_manuf_specific) ? ZB_ZCL_MANUFACTURER_SPECIFIC         \
                                                            : ZB_ZCL_NOT_MANUFACTURER_SPECIFIC),   \
                                      (_direction),                                                \
                                      ZB_ZCL_DISABLE_DEFAULT_RESPONSE),                            \
   (_buf_ptr)++)

/** @endcond */ /* internals_doc */

/*! @brief Construct ZCL header frame control value for cluster-specific command response
 *
 *  @snippet wwah_door_lock/dl_device/dl_door_lock.c zcl_construct_specific_frame_header
 * */
#define ZB_ZCL_CONSTRUCT_SPECIFIC_COMMAND_RES_FRAME_CONTROL(buf_ptr) \
  (ZB_ZCL_CONSTRUCT_SET_FRAME_CONTROL(*(buf_ptr),                    \
        ZB_ZCL_FRAME_TYPE_CLUSTER_SPECIFIC,                          \
        ZB_ZCL_NOT_MANUFACTURER_SPECIFIC,                            \
        ZB_ZCL_FRAME_DIRECTION_TO_CLI,                               \
        ZB_ZCL_DISABLE_DEFAULT_RESPONSE),                            \
    (buf_ptr)++)

/**
 *  @brief Construct ZCL header
 *  @note To add Manufacturer specific field, use ZB_ZCL_CONSTRUCT_COMMAND_HEADER_EXT(), macro.
 *  @hideinitializer
 */
#define ZB_ZCL_CONSTRUCT_COMMAND_HEADER(data_ptr, tsn, cmd_id)                                     \
  (ZB_ZCL_PACKET_PUT_DATA8((data_ptr), (tsn)), ZB_ZCL_PACKET_PUT_DATA8((data_ptr), (cmd_id)))

/*! @brief Construct ZCL header, Manufacturer specific value is conditionally supported */
#define ZB_ZCL_CONSTRUCT_COMMAND_HEADER_EXT(_data_ptr, _tsn, _is_manuf_spec, _manuf_specific, _cmd_id) \
  {                                                                     \
    if ((_is_manuf_spec))                                               \
    {                                                                   \
      ZB_ZCL_PACKET_PUT_DATA16_VAL((_data_ptr), (_manuf_specific));     \
    }                                                                   \
    ZB_ZCL_PACKET_PUT_DATA8((_data_ptr), (_tsn));                       \
    ZB_ZCL_PACKET_PUT_DATA8((_data_ptr), (_cmd_id));                    \
  }

/** @cond internals_doc */

/*! @internal @brief Calculate ZCL frame header size */
#define ZB_ZCL_FRAME_HDR_GET_SIZE(p) ( ZB_ZCL_GET_MANUF_SPECIFIC(p) ?    \
                                       2*sizeof(zb_uint8_t)+sizeof(zb_uint16_t) : \
                                       2*sizeof(zb_uint8_t) )

/*! @internal @brief Get ZCL frame manufacturer code from the packet */
#define ZB_ZCL_FRAME_HDR_GET_MANUFACTURER_CODE(p)                       \
    ( *(zb_uint16_t *)((zb_uint8_t *)p + sizeof(zb_zcl_frame_ctrl_t)) )

/*! @internal Get ZCL frame sequence number from the packet. */
#define ZB_ZCL_FRAME_HDR_GET_SEQ_NUM(p)                            \
    (   *((zb_uint8_t *)p                                          \
     +  sizeof(zb_zcl_frame_ctrl_t)                                \
     + (ZB_ZCL_GET_MANUF_SPECIFIC(p) ? sizeof(zb_uint16_t) : 0)) )

/*! @internal Get ZCL frame command identifier from the packet. */
#define ZB_ZCL_FRAME_HDR_GET_COMMAND_ID(p)                                              \
    (   *((zb_uint8_t *)p                                                               \
     + sizeof(zb_zcl_frame_ctrl_t)                                                      \
     + (ZB_ZCL_GET_MANUF_SPECIFIC(p) ? sizeof(zb_uint16_t) : 0) + sizeof(zb_uint8_t)) )

/** @endcond */ /* internals_doc */

/**
 *  @brief Return next sequence number for ZCL frame.
 *  @hideinitializer
 */
#define ZB_ZCL_GET_SEQ_NUM() (ZCL_CTX().seq_number++)

/*! @brief Copy parsed ZCL header from buffer
    @param buf - ID zb_bufid_t of buffer containing parsed ZCL header as parameter
    @param dst - pointer to the memory area to copy header to
*/
#define ZB_ZCL_COPY_PARSED_HEADER(buf, dst)         \
  (ZB_MEMCPY(                                       \
      (dst),                                        \
      ZB_BUF_GET_PARAM((buf), zb_zcl_parsed_hdr_t), \
      sizeof(zb_zcl_parsed_hdr_t)))


/*! @brief General macro for sending a command with empty payload
    @param buffer to put command payload and headers to
    @param addr address to send to
    @param dst_addr_mode destination address mode
    @param dst_ep destination endpoint
    @param ep source endpoint
    @param prof_id profile the command belongs to
    @param dis_default_resp "Disable default response" flag
    @param cluster_id cluster identifier
    @param command_id command identifier
    @param cb command send status callback

    @code
      #define ZB_ZCL_DOOR_LOCK_SEND_LOCK_DOOR_REQ(buffer,           \
                                                  dst_addr,         \
                                                  dst_addr_mode,    \
                                                  dst_ep,           \
                                                  ep,               \
                                                  dis_default_resp, \
                                                  cb)               \
        ZB_ZCL_SEND_CMD(  buffer,                                   \
                          dst_addr,                                 \
                          dst_addr_mode,                            \
                          dst_ep,                                   \
                          ep,                                       \
                          ZB_AF_HA_PROFILE_ID,                      \
                          dis_default_resp,                         \
                          ZB_ZCL_CLUSTER_ID_DOOR_LOCK,              \
                          ZB_ZCL_CMD_DOOR_LOCK_LOCK_DOOR,           \
                          cb)
    @endcode
*/
#define ZB_ZCL_SEND_CMD(                                                                       \
    buffer,                                                                                    \
    addr,                                                                                      \
    dst_addr_mode,                                                                             \
    dst_ep,                                                                                    \
    ep,                                                                                        \
    prof_id,                                                                                   \
    dis_default_resp,                                                                          \
    cluster_id,                                                                                \
    command_id, cb)                                                                            \
{                                                                                              \
  zb_uint8_t* ptr = ZB_ZCL_START_PACKET_REQ(buffer)                                            \
  ZB_ZCL_CONSTRUCT_SPECIFIC_COMMAND_REQ_FRAME_CONTROL(ptr, dis_default_resp)                   \
  ZB_ZCL_CONSTRUCT_COMMAND_HEADER_REQ(ptr, ZB_ZCL_GET_SEQ_NUM(), command_id);                  \
  ZB_ZCL_FINISH_PACKET(buffer, ptr)                                                           \
  ZB_ZCL_SEND_COMMAND_SHORT(buffer, addr, dst_addr_mode, dst_ep, ep, prof_id, cluster_id, cb); \
}

/*! @} */ /* General frame description */


/******************************* ZCL frame basic manipulations *******************************/

/*! @defgroup zb_zcl_common_pckt_fill Fill ZCL packet low-level API
    Types and macros shared fill Fill ZCL packet.
    @{
      @par Example
      @snippet HA_samples/write_attr_test/sample_zed.c ZB_ZCL_PACKET
      @par
*/

/**
 *  @brief Initializes zb_buf_t buffer and returns pointer to the beginning of array.
 *  @hideinitializer
 */
#define ZB_ZCL_START_PACKET(zbbuf)                    \
  zb_buf_reuse(zbbuf)

/**
 *  @brief Return number of bytes written to the buffer.
 *  @hideinitializer
 */
#define ZB_ZCL_GET_BYTES_WRITTEN(zbbuf, ptr) \
  zb_buf_get_ptr_off((zbbuf), (ptr))

zb_bool_t zb_zcl_can_cluster_be_fragmented(zb_uint16_t profile_id, zb_uint16_t cluster_id);

#define ZB_ZCL_GET_BYTES_AVAILABLE_WITH_FRAGMENTATION(zbbuf, ptr) \
  ((zb_buf_get_max_size(zbbuf) -\
   (ZB_MAX_PHY_PAYLOAD_SIZE-ZB_ZCL_HI_WO_IEEE_MAX_PAYLOAD_SIZE))-\
   ZB_ZCL_GET_BYTES_WRITTEN(zbbuf, ptr))

#define ZB_ZCL_GET_BYTES_AVAILABLE_WO_FRAGMENTATION(zbbuf, ptr) \
  (ZB_ZCL_HI_WO_IEEE_MAX_PAYLOAD_SIZE - ZB_ZCL_GET_BYTES_WRITTEN(zbbuf, ptr))

/**
 *  @brief Return number of bytes available in a packet.
 *  @hideinitializer
 */
#ifdef APS_FRAGMENTATION
#define ZB_ZCL_GET_BYTES_AVAILABLE(zbbuf, ptr, profile_id, cluster_id)     \
  (zb_zcl_can_cluster_be_fragmented(profile_id, cluster_id) ?              \
   ZB_ZCL_GET_BYTES_AVAILABLE_WITH_FRAGMENTATION(zbbuf, ptr) :             \
   ZB_ZCL_GET_BYTES_AVAILABLE_WO_FRAGMENTATION(zbbuf, ptr))
#else
#define ZB_ZCL_GET_BYTES_AVAILABLE(zbbuf, ptr, profile_id, cluster_id)     \
  ZB_ZCL_GET_BYTES_AVAILABLE_WO_FRAGMENTATION(zbbuf, ptr)
#endif /* APS_FRAGMENTATION */

#define ZB_ZCL_ATTR_SZ_RAW(attr_size) ((sizeof(zb_zcl_read_attr_res_t)      \
                                        - sizeof(zb_uint8_t)) + (attr_size))

/**
 *  @brief Put 8-bit value to packet.
 *  @hideinitializer
 */
#define ZB_ZCL_PACKET_PUT_DATA8(ptr, val) (*(ptr) = (val), (ptr)++)

/*!
  Put 16-bit value to packet
  @param ptr - pointer to the place to put value to
  @param val - pointer to the value to put into packet
*/
#define ZB_ZCL_PACKET_PUT_DATA16(ptr, val) \
do                                         \
{                                          \
  ZB_HTOLE16((ptr), (val));                \
  (ptr) += 2;                              \
} while (ZB_FALSE)

/*!
  Put 16-bit value to packet
  @param ptr - pointer to the place to put value to
  @param val - value to put into packet
*/
//#define ZB_ZCL_PACKET_PUT_DATA16_VAL(ptr, val) ZB_PUT_NEXT_HTOLE16(ptr, val)
#define ZB_ZCL_PACKET_PUT_DATA16_VAL(ptr, val) (ptr) = zb_put_next_htole16((ptr), val)

#define ZB_ZCL_PACKET_PUT_2DATA16_VAL(ptr, val1, val2) (ptr) = zb_put_next_2_htole16((ptr),(val1),(val2))

#define ZB_ZCL_PACKET_PUT_2DATA32_VAL(ptr, val1, val2) (ptr) = zb_put_next_2_htole32((ptr),(val1),(val2))

/**
 *  @brief Put 32-bit value to packet.
 *  @param ptr - pointer to the place to put value to
 *  @param val - pointer to the value to put into packet
 *  @hideinitializer
 */
#define ZB_ZCL_PACKET_PUT_DATA32_VAL(ptr, val)  ZB_PUT_NEXT_HTOLE32((ptr),(val))

/**
 *  @brief Put 32-bit value to packet.
 *  @param ptr - pointer to the place to put value to
 *  @param val - pointer to the value to put into packet
 *  @hideinitializer
 */
#define ZB_ZCL_PACKET_PUT_DATA32(ptr, val)       \
   (ZB_HTOLE32(ptr, val), (ptr) += 4)

/**
 *  @brief Put 48-bit value to packet.
 *  @param ptr - pointer to the place to put value to
 *  @param val - value to put into packet
 *  @hideinitializer
 */
#define ZB_ZCL_PACKET_PUT_DATA48_VAL(ptr, val) \
    (PUT_DATA48_VAL(ptr, val))

/**
 *  @brief Put 48-bit value to packet.
 *  @param ptr - pointer to the place to put value to
 *  @param val - pointer to the value to put into packet
 *  @hideinitializer
 */
#define ZB_ZCL_PACKET_PUT_DATA48(ptr, val)       \
   (PUT_DATA48(ptr, val))

/**
 *  @brief Put 24-bit value to packet.
 *  @param ptr - pointer to the place to put value to
 *  @param val - value to put into packet
 *  @hideinitializer
 */
#define ZB_ZCL_PACKET_PUT_DATA24_VAL(ptr, val) \
    (PUT_DATA24_VAL(ptr, val))

/**
 *  @brief Put 24-bit value to packet.
 *  @param ptr - pointer to the place to put value to
 *  @param val - pointer to the value to put into packet
 *  @hideinitializer
 */
#define ZB_ZCL_PACKET_PUT_DATA24(ptr, val)       \
    (PUT_DATA24(ptr, val))



/**
 *  @brief Put 64-bit value to packet.
 *  @param ptr - pointer to the place to put value to
 *  @param val - pointer to the value to put into packet
 *  @hideinitializer
 */
#define ZB_ZCL_PACKET_PUT_DATA64(ptr, val) (ZB_HTOLE64(ptr, val), (ptr) += 8)

/** @brief Get 8-bit value from packet.
  * @param dst_ptr - pointer to the memory to store value (pointer value preserved).
  * @param src_ptr - pointer to the memory to get value from (will be incremented).
  */
#define ZB_ZCL_PACKET_GET_DATA8(dst_ptr, src_ptr) \
{                                                 \
  *(dst_ptr) = *((src_ptr)++);                    \
}

/**
 *  @brief Put N byte data to packet.
 *  @param ptr - pointer to the place to put value to.
 *  @param val - pointer to the value to put into packet.
 *  @param n - number of bytes to be copied.
 *  @hideinitializer
 */
#define ZB_ZCL_PACKET_PUT_DATA_N(ptr, val, n) \
    (ZB_MEMCPY(ptr, val, n), (ptr) += (n))

/** @brief Get 16-bit value from packet.
  * @param dst_ptr - pointer to the memory to store value (pointer value preserved).
  * @param src_ptr - pointer to the memory to get value from (will be incremented by value size).
  */
#define ZB_ZCL_PACKET_GET_DATA16(dst_ptr, src_ptr) \
{                                                  \
  ZB_LETOH16((dst_ptr), (src_ptr));                \
  (src_ptr) += sizeof(zb_uint16_t);                \
}

/** @brief Get 24-bit value from packet.
  * @param dst_ptr - pointer to the memory to store value (pointer value preserved).
  * @param src_ptr - pointer to the memory to get value from (will be incremented by value size).
  */
#define ZB_ZCL_PACKET_GET_DATA24(dst_ptr, src_ptr) \
{                                                  \
  ZB_LETOH24(dst_ptr, src_ptr);                    \
  (src_ptr) += ZB_24BIT_SIZE;                \
}


/** @brief Get 48-bit value from packet.
  * @param dst_ptr - pointer to the memory to store value (pointer value preserved).
  * @param src_ptr - pointer to the memory to get value from (will be incremented by value size).
  */
#define ZB_ZCL_PACKET_GET_DATA48(dst_ptr, src_ptr) \
{                                                  \
  ZB_LETOH48(dst_ptr, src_ptr);                    \
  (src_ptr) += ZB_48BIT_SIZE;                \
}


/** @brief Get 32-bit value from packet.
  * @param dst_ptr - pointer to the memory to store value (pointer value preserved).
  * @param src_ptr - pointer to the memory to get value from (will be incremented by value size).
  */
#define ZB_ZCL_PACKET_GET_DATA32(dst_ptr, src_ptr) \
{                                                  \
  ZB_LETOH32((dst_ptr), (src_ptr));                \
  (src_ptr) += sizeof(zb_uint32_t);                \
}

/** @brief Get 64-bit value from packet.
  * @param dst_ptr - pointer to the memory to store value (pointer value preserved).
  * @param src_ptr - pointer to the memory to get value from (will be incremented by value size).
  */
#define ZB_ZCL_PACKET_GET_DATA64(dst_ptr, src_ptr) \
{                                                  \
  ZB_LETOH64((dst_ptr), (src_ptr));                \
  (src_ptr) += 8;                                  \
}

/** @brief Get IEEE address from packet.
  * @param dst_ptr - pointer to the memory to store value (pointer value preserved).
  * @param src_ptr - pointer to the memory to get value from (will be incremented by value size).
  */
#define ZB_ZCL_PACKET_GET_DATA_IEEE(dst_ptr, src_ptr) \
{                                                     \
  ZB_IEEE_ADDR_COPY((dst_ptr), (src_ptr));            \
  (src_ptr) += sizeof(zb_ieee_addr_t);                \
}

/** @brief Put IEEE address into packet.
  * @param dst_ptr - pointer to the memory to store value (will be incremented by value size).
  * @param src_ptr - pointer to the memory to get value from (pointer value preserved).
  */
#define ZB_ZCL_PACKET_PUT_DATA_IEEE(dst_ptr, src_ptr) \
{                                                     \
  ZB_IEEE_ADDR_COPY((dst_ptr), (src_ptr));            \
  (dst_ptr) += sizeof(zb_ieee_addr_t);                \
}

/** Put ZCL Octet String to packet */
#define ZB_ZCL_PACKET_PUT_STRING(_ptr, _val)                          \
{                                                                     \
 ZB_ZCL_PACKET_PUT_DATA8 (_ptr, ZB_ZCL_GET_STRING_LENGTH(_val));      \
 ZB_ZCL_PACKET_PUT_DATA_N(_ptr, ZB_ZCL_GET_STRING_BEGIN(_val),        \
                          ZB_ZCL_GET_STRING_LENGTH(_val));            \
}

/** Get ZCL String from packet.
 * @note If string is invalid (cannot be stored in buffer)
 * this macro sets data to NULL.
 *
 * @param _zcl_str - Destination buffer.
 * @param data - Pointer to incoming data.
 * @param _zcl_str_max_size - Size of destination buffer.
 */
#define ZB_ZCL_PACKET_GET_STRING(_zcl_str, data, _zcl_str_max_size)         \
{                                                                           \
  ZB_ZCL_PACKET_GET_DATA8(&_zcl_str[0], data);                              \
  if (_zcl_str_max_size > ZB_ZCL_GET_STRING_LENGTH(_zcl_str))               \
  {                                                                         \
    ZB_ZCL_PACKET_GET_DATA_N(ZB_ZCL_GET_STRING_BEGIN(_zcl_str),             \
                             data,                                          \
                             ZB_ZCL_GET_STRING_LENGTH(_zcl_str));           \
  }                                                                         \
  else                                                                      \
  {                                                                         \
    data = NULL;                                                            \
  }                                                                         \
}

/** Get ZCL String from packet and put it into array with fixed size. */
#define ZB_ZCL_PACKET_GET_STATIC_STRING(_zcl_str, data) \
  ZB_ZCL_PACKET_GET_STRING(_zcl_str, data, sizeof(_zcl_str))


/** @brief Get N-byte value from packet.
  * @param dst_ptr - pointer to the memory to store value (will be incremented by value size).
  * @param src_ptr - pointer to the memory to get value from (pointer value preserved).
  * @param n - data length.
  */
#define ZB_ZCL_PACKET_GET_DATA_N(dst_ptr, src_ptr, n) \
{                                                     \
  ZB_MEMCPY((dst_ptr), (src_ptr), (n));              \
  (src_ptr) += (n);                                  \
}

/*! @brief ZCL finish and send packet
    @param buffer pointer to the buffer where ZCL cmd is stored
    @param ptr pointer to the end of ZCL cmd in buffer
    @param dst_addr destination address (see zb_addr_u)
    @param dst_addr_mode destination address mode (only @ref ZB_APS_ADDR_MODE_16_GROUP_ENDP_NOT_PRESENT and
    @ref ZB_APS_ADDR_MODE_16_ENDP_PRESENT are supported)
    @param dst_ep destination endpoint
    @param ep source endpoint
    @param prof_id profile the command belongs to
    @param cluster_id cluster identifier
    @param cb command send status callback
 */
zb_ret_t zb_zcl_finish_and_send_packet(zb_bufid_t buffer, zb_uint8_t *ptr,
                                   const zb_addr_u *dst_addr, zb_uint8_t dst_addr_mode,
                                   zb_uint8_t dst_ep, zb_uint8_t ep,
                                   zb_uint16_t prof_id, zb_uint16_t cluster_id,
                                   zb_callback_t cb);

zb_ret_t zb_zcl_finish_and_send_packet_new(zb_bufid_t buffer, zb_uint8_t *ptr,
                                   const zb_addr_u *dst_addr, zb_uint8_t dst_addr_mode,
                                   zb_uint8_t dst_ep, zb_uint8_t ep,
                                   zb_uint16_t prof_id, zb_uint16_t cluster_id,
                                   zb_callback_t cb, zb_bool_t aps_secured,
                                   zb_bool_t disable_aps_ack, zb_uint16_t delay);


/*!
  Finish ZCL packet filling. Used bytes number are marked in a buffer.
  @param zbbuf - pointer to zb_buf_t buffer.
  @param ptr - pointer to the byte after the last written byte in the buffer.
  @attention After ZB_ZCL_FINISH_PACKET() call, pointer ptr is set to the beginning of the buffer
  data.
  @note Please note that this macro does not return ret code, so if this macro is used without callback,
        this may lead to buffer leaks (if function is failed to send the packet).
        Please use @ref zb_zcl_finish_and_send_packet() function directly to be able to check ret code.
 */
/* Glue ZB_ZCL_FINISH_PACKET and ZB_ZCL_SEND_COMMAND_SHORT into one call */
#define ZB_ZCL_FINISH_PACKET(zbbuf, ptr) (void) zb_zcl_finish_and_send_packet((zbbuf), (ptr),

#define ZB_ZCL_SEND_COMMAND_SHORT(buffer, addr, dst_addr_mode, dst_ep, ep, prof_id, cluster_id,    \
                                  cb)                                                              \
  (const zb_addr_u *)(const void *)(&(addr)), (dst_addr_mode), (dst_ep), (ep), (prof_id), (cluster_id), (cb))


#define ZB_ZCL_FINISH_N_SEND_PACKET(zbbuf, ptr, addr, dst_addr_mode, dst_ep, ep, prof_id, cluster_id, cb)                          \
  (void) zb_zcl_finish_and_send_packet((zbbuf), (ptr),(const zb_addr_u *)(const void *)(&(addr)), dst_addr_mode, dst_ep, ep, prof_id, cluster_id, cb)

#define ZB_ZCL_FINISH_N_SEND_PACKET_NEW(zbbuf, ptr, addr, dst_addr_mode, dst_ep, ep, prof_id, cluster_id, cb, aps_secured, disable_aps_ack, delay)                         \
  (void) zb_zcl_finish_and_send_packet_new((zbbuf), (ptr),(const zb_addr_u *)(const void *)(&(addr)), dst_addr_mode, dst_ep, ep, prof_id, cluster_id, cb, aps_secured, disable_aps_ack, delay)

/* TODO: Implement via zb_zcl_finish_and_send_packet() */
#define ZB_ZCL_FINISH_PACKET_O(zbbuf, ptr)                              \
  (ptr) = zb_buf_alloc_left((zbbuf), ZB_ZCL_GET_BYTES_WRITTEN((zbbuf), (ptr)));

/*! @}*/ /* Fill ZCL packet */

/* align func */
#ifdef ZB_NEED_ALIGN

zb_uint16_t zb_zcl_attr_get16(zb_uint8_t *value);
zb_int16_t zb_zcl_attr_gets16(zb_uint8_t *value);
zb_uint32_t zb_zcl_attr_get32(zb_uint8_t *value);
zb_int32_t zb_zcl_attr_gets32(zb_uint8_t *value);
#define ZB_ZCL_ATTR_GET16(value) zb_zcl_attr_get16(value)
#define ZB_ZCL_ATTR_GETS16(value) zb_zcl_attr_gets16(value)
#define ZB_ZCL_ATTR_GET32(value) zb_zcl_attr_get32(value)
#define ZB_ZCL_ATTR_GETS32(value) zb_zcl_attr_gets32(value)

zb_int24_t zb_zcl_attr_get24(zb_uint8_t *value);
zb_uint48_t zb_zcl_attr_get48(zb_uint8_t *value);
#define ZB_ZCL_ATTR_GET24(value) zb_zcl_attr_get24(value)
#define ZB_ZCL_ATTR_GET48(value) zb_zcl_attr_get48(value)

zb_single_t zb_zcl_attr_getsingle(zb_uint8_t *value);
#define ZB_ZCL_ATTR_GETSINGLE(value) zb_zcl_attr_getsingle(value)

#else

#define ZB_ZCL_ATTR_GET16(value) (*((zb_uint16_t *)value))
#define ZB_ZCL_ATTR_GETS16(value) (*((zb_int16_t *)value))
#define ZB_ZCL_ATTR_GET32(value) (*((zb_uint32_t *)value))
#define ZB_ZCL_ATTR_GETS32(value) (*((zb_int32_t *)value))
#define ZB_ZCL_ATTR_GETSINGLE(value) (*((zb_single_t *)value))

#define ZB_ZCL_ATTR_GET24(value) (*((zb_int24_t *)value))
#define ZB_ZCL_ATTR_GET48(value) (*((zb_uint48_t *)value))

#endif

/***************************** ZCL general functionality definitions ******************************/

/** @cond internals_doc */
/** @internal @brief Calculates array size */
#define ZB_ZCL_ARRAY_SIZE(ar, type) (sizeof(ar)/sizeof(type))

/** @internal @brief Calculates byte array size (add 2 bytes for full length). */
#define ZB_ZCL_ARRAY_GET_SIZE(ar, val) ZB_LETOH16(ar, val)
#define ZB_ZCL_ARRAY_SET_SIZE(ar, val) ZB_HTOLE16_VAL(ar, val)

/** @internal @brief Calculates 16-byte array size (add 2 bytes for full length). */
#define ZB_BYTE_16ARRAY_GET_SIZE(ar, val) { ZB_ZCL_ARRAY_GET_SIZE(ar, val); *(zb_uint16_t*)(ar) *= 2U; }
#define ZB_BYTE_16ARRAY_SET_SIZE(ar, val) { ZB_ZCL_ARRAY_SET_SIZE(ar, val); *(zb_uint16_t*)(ar) /= 2U; }

/** @internal @brief Calculates 32-byte array size (add 2 bytes for full length). */
#define ZB_BYTE_32ARRAY_GET_SIZE(ar, val) { ZB_ZCL_ARRAY_GET_SIZE(ar, val); *(zb_uint16_t*)(ar) *= 4U; }
#define ZB_BYTE_32ARRAY_SET_SIZE(ar, val) { ZB_ZCL_ARRAY_SET_SIZE(ar, val); *(zb_uint16_t*)(ar) /= 4U; }

#define ZB_ZCL_NULL_EP_ID (zb_uint8_t)(-1)
#define ZB_ZCL_NULL_ID (zb_uint16_t)(-1)
#define ZB_ZCL_NULL_STRING (zb_uint8_t)(0)

/** @internal @brief Maximum size of Character String (with Length octet) */
#define ZB_ZCL_MAX_STRING_SIZE 0xFFU
#define ZB_ZCL_INVALID_STRING_VALUE 0xFFU
#define ZB_ZCL_INVALID_ARRAY_VALUE 0xFFFFU

/** @endcond */

/*! @} */ /* ZCL common types and definitions */

#ifdef DOXYGEN
/** @brief Fix data endian, if needed, and store it in the same buffer
    @param data_ptr - pointer to data value
    @param data_type - ZCL data type
 */
#define ZB_ZCL_FIX_ENDIAN(data_ptr, data_type)
#endif

/** @cond internals_doc */
#if ! defined ZB_LITTLE_ENDIAN

/** @internal @brief Fix data endian, if needed, and store it in the same buffer
    @param data_ptr - pointer to data value
    @param data_type - ZCL data type
 */
#define ZB_ZCL_FIX_ENDIAN(data_ptr, data_type) zb_zcl_fix_endian(data_ptr, data_type)

void zb_zcl_fix_endian(zb_uint8_t *data_ptr, zb_uint8_t data_type);

#define ZB_ZCL_HTOLE16_INPLACE(data_ptr)      \
  {                                           \
    zb_uint16_t tmp_place;                    \
    ZB_HTOLE16(&tmp_place, (data_ptr));       \
    ZB_ASSIGN_UINT16((data_ptr), &tmp_place); \
  }

#else

#define ZB_ZCL_FIX_ENDIAN(data_ptr, data_type) (ZVUNUSED((data_ptr)), ZVUNUSED((data_type)))

#define ZB_ZCL_HTOLE16_INPLACE(data_ptr) (ZVUNUSED((data_ptr)))

#endif /* ZB_LITTLE_ENDIAN */

#define ZB_ZCL_TRAN_TABLE_HASH_PARAM(param) ZB_1INT_HASH_FUNC(param) % ZCL_TRAN_TABLE_SIZE

/** @internal @brief ZCL hash entity, stores callback functions for commands specified by
 * buffer index */
typedef struct zcl_cb_hash_ent_s
{
  zb_callback_t func; /*!< callback pointer */
  zb_uint8_t    buf;  /*!< buffer index */
}
zcl_cb_hash_ent_t;

/** @endcond */

/** Command send status structure */
typedef struct zb_zcl_command_send_status_s
{
  zb_ret_t    status;         /*!< command send status */
  zb_uint8_t  dst_endpoint;   /*!< Destination endpoint */
  zb_zcl_addr_t dst_addr;     /*!< Destination address */
  zb_uint8_t  src_endpoint;   /*!< Source endpoint */
  /* add more fields here if needed */
}
zb_zcl_command_send_status_t;

/** @brief Set default value attribute callback
    @param ep - endpoint number
*/
typedef void (*zb_zcl_set_default_value_attr_cb_t)(zb_uint8_t ep);

/** @brief Check attribute value callback
    @param cluster_id - Cluster ID
    @param cluster_role - cluster role (@ref zcl_cluster_role)
    @param endpoint - endpoint number
    @param attr_id - attribute ID
    @param value - new attribute value

    @return RET_OK - if new value is valid, RET_ERROR - if new value is wrong or
            RET_IGNORE - if use default Check attribute functions
*/
typedef zb_ret_t (*zb_zcl_app_check_attr_value_cb_t)
    (zb_uint16_t cluster_id, zb_uint8_t cluster_role, zb_uint8_t endpoint, zb_uint16_t attr_id, zb_uint8_t *value);

/** @brief Check attribute value callback with manufacturer specific code
    @param cluster_id - Cluster ID
    @param cluster_role - cluster role (@ref zcl_cluster_role)
    @param endpoint - endpoint number
    @param attr_id - attribute ID
    @param manuf_code - manufacturer specific code
    @param value - new attribute value

    @return RET_OK - if new value is valid, RET_ERROR - if new value is wrong or
            RET_IGNORE - if use default Check attribute functions
*/
typedef zb_ret_t (*zb_zcl_app_check_attr_value_manuf_cb_t)
(zb_uint16_t cluster_id, zb_uint8_t cluster_role, zb_uint8_t endpoint, zb_uint16_t attr_id, zb_uint16_t manuf_code, zb_uint8_t *value);

/** @brief Get peer Cluster revision callback
    @param ieee_addr - IEEE address
    @param cluster_id - Cluster ID
    @param cluster_role - cluster role (@ref zcl_cluster_role)
    @param endpoint - endpoint number

    @return peer Cluster revision attribute value, 0xFFFF - if the revision is unknown
*/
typedef zb_uint16_t (*zb_zcl_peer_revision_cb_t)
    (zb_ieee_addr_t ieee_addr, zb_uint16_t cluster_id, zb_uint8_t cluster_role, zb_uint8_t endpoint);

/* ZCL common functions */

/*! @brief Get registered attribute description by cluster descriptor
    @param cluster_desc - cluster descriptor
    @param attr_id - attribute identifier
    @returns Pointer to attribute description, or NULL if no attribute description found.
*/
zb_zcl_attr_t* zb_zcl_get_attr_desc(zb_zcl_cluster_desc_t *cluster_desc, zb_uint16_t attr_id);

/*! @brief Get registered attribute description by cluster descriptor
    @param cluster_desc - cluster descriptor
    @param attr_id - attribute identifier
    @param manuf_code - manufacturer specific ID
    @returns Pointer to attribute description, or NULL if no attribute description found.
*/
zb_zcl_attr_t* zb_zcl_get_attr_desc_manuf(zb_zcl_cluster_desc_t *cluster_desc, zb_uint16_t attr_id, zb_uint16_t manuf_code);

/** @brief Check registered attribute is writable one
    @param attr_desc - attribute descriptor
    @param endpoint - endpoint
    @param cluster_id - cluster identifier
    @param cluster_role - cluster role (@ref zcl_cluster_role)
    @param new_value - new value for an attribute
    @param check_access - check READ_ONLY access or not
    @returns ZB_ZCL_STATUS_SUCCESS on success, error status otherwise
*/
zb_uint8_t zb_zcl_check_attribute_writable(
  zb_zcl_attr_t* attr_desc,
  zb_uint8_t endpoint,
  zb_uint16_t cluster_id,
  zb_uint8_t cluster_role,
  zb_uint8_t *new_value,
  zb_bool_t check_access);

/*!
  Hook on Write Attribute command
  @param endpoint - endpoint
  @param cluster_id - cluster ID
  @param cluster_role - cluster role (@ref zcl_cluster_role)
  @param attr_id - attribute ID
  @param new_value - pointer to newly assigned value
  @param manuf_code - manufacturer specific code
*/
void zb_zcl_write_attr_hook(zb_uint8_t endpoint, zb_uint16_t cluster_id, zb_uint8_t cluster_role, zb_uint16_t attr_id, zb_uint8_t *new_value, zb_uint16_t manuf_code);

/*!
  Set attribute value cluster specific postprocessing
  @param cmd_info - cluster role (@ref zcl_cluster_role)
  @param attr_id - attribute ID
  @param value - new value of attribute
*/
void zb_zcl_set_attr_val_post_process_cluster_specific(zb_zcl_parsed_hdr_t *cmd_info,
                                                       zb_uint16_t attr_id,
                                                       zb_uint8_t *value);
/**
 * @brief Get size of value of given attribute type
 *
 * @param attr_type - attribute type in question (see @ref zcl_attr_type)
 * @param attr_value - pointer to value in question (needed for variable sized types)
 *
 * @return size in bytes or 0xFFFF if type is invalid
 */
zb_uint16_t zb_zcl_get_attribute_size(zb_uint8_t attr_type, zb_uint8_t *attr_value);

/**
 * @brief Get size of analog data type
 * @param attr_type - attribute type in question (see @ref zcl_attr_type)
 *
 * @return size in bytes of 0xFF in type is not analog or invalid
 */
zb_uint8_t zb_zcl_get_analog_attribute_size(zb_uint8_t attr_type);

/**
 * @brief Check whether type of ZCL attribute is analog
 * @param attr_type - attribute type in question (see @ref zcl_attr_type)
 *
 * @return ZB_TRUE if type is analog, ZB_FALSE otherwise
 */
zb_bool_t zb_zcl_is_analog_data_type(zb_uint8_t attr_type);

/**
 * @brief Check if attribute value is valid or not
 *
 * @deprecated This function will be removed in the next Major release after February 2023
 *             Use @ref zb_zcl_check_attr_value_manuf instead
 *
 * @param cluster_id - cluster ID
 * @param cluster_role - cluster role (@ref zcl_cluster_role)
 * @param endpoint - endpoint
 * @param attr_id - attribute ID
 * @param value - pointer to attribute data
 *
 * @return RET_OK if data value is valid, some error code otherwise
 */
zb_ret_t zb_zcl_check_attr_value(zb_uint16_t cluster_id, zb_uint8_t cluster_role, zb_uint8_t endpoint, zb_uint16_t attr_id, zb_uint8_t *value);

/**
 * @brief Check if attribute value is valid or not (with manufacturer specific code)
 *
 * @param cluster_id - cluster ID
 * @param cluster_role - cluster role (@ref zcl_cluster_role)
 * @param endpoint - endpoint
 * @param attr_id - attribute ID
 * @param manuf_code - manufacturer specific code
 * @param value - pointer to attribute data
 *
 * @return ZB_TRUE if data value is valid, ZB_FALSE otherwise
 */
zb_ret_t zb_zcl_check_attr_value_manuf(zb_uint16_t cluster_id, zb_uint8_t cluster_role, zb_uint8_t endpoint, zb_uint16_t attr_id, zb_uint16_t manuf_code, zb_uint8_t *value);

zb_bool_t zb_zcl_is_target_endpoint(zb_af_endpoint_desc_t *ep_desc, zb_uint16_t profile_id);

zb_uint8_t zb_zcl_get_next_target_endpoint(
  zb_uint8_t start_from_ep, zb_uint16_t cluster_id, zb_uint8_t cluster_role, zb_uint16_t profile_id);

/**
 * @brief Search endpoint for cluster with given id
 *
 * @param ep_desc - application endpoint description
 * @param cluster_id - cluster ID (see @ref zcl_cluster_id)
 * @param cluster_role - role (see @ref zcl_cluster_role)
 * @return cluster descriptor or NULL if not present
 */
zb_zcl_cluster_desc_t* get_cluster_desc(const zb_af_endpoint_desc_t* ep_desc, zb_uint16_t cluster_id, zb_uint8_t cluster_role);

/**
 * @brief Find endpoint which has cluster with given ID
 *
 * @param cluster_id - cluster ID (see @ref zcl_cluster_id)
 * @param cluster_role - role (see @ref zcl_cluster_role)
 * @return endpoint number or 0 if not found
 */
zb_uint8_t get_endpoint_by_cluster(zb_uint16_t cluster_id, zb_uint8_t cluster_role);

/**
 * @brief Find if device has cluster with given ID
 *
 * @param cluster_id - cluster ID (see @ref zcl_cluster_id)
 * @return ZB_TRUE if cluster found
 */
zb_bool_t is_cluster_present(zb_uint16_t cluster_id);

zb_uint16_t get_profile_id_by_endpoint(zb_uint8_t endpoint_id);

/**
 * @brief Find endpoint which has cluster with given ID and role
 *
 * @param cluster_id - cluster ID (see @ref zcl_cluster_id)
 * @param cluster_role - role (see @ref zcl_cluster_role)
 *
 * @return endpoint number or 0 if not found
 */
zb_af_endpoint_desc_t *get_endpoint_by_cluster_with_role(
    zb_uint16_t cluster_id,
    zb_uint8_t cluster_role);

/**
 * Find attribute descriptor by given endpoint number, cluster ID and attribute ID
 *
 * @param ep - endpoint number (must exist)
 * @param cluster_id - cluster ID (must exist on given ep, see @ref zb_zcl_cluster_id_t)
 * @param cluster_role - role (see @ref zcl_cluster_role)
 * @param attr_id - attribute ID
 *
 * @return attribute descriptor
 */
zb_zcl_attr_t* zb_zcl_get_attr_desc_a(zb_uint8_t ep, zb_uint16_t cluster_id, zb_uint8_t cluster_role, zb_uint16_t attr_id);

/**
 * Find attribute descriptor by given endpoint number, cluster ID, attribute ID, and manuf code
 *
 * @param ep - endpoint number (must exist)
 * @param cluster_id - cluster ID (must exist on given ep, see @ref zb_zcl_cluster_id_t)
 * @param cluster_role - role (see @ref zcl_cluster_role)
 * @param attr_id - attribute ID
 * @param manuf_code - manufacturer code
 *
 * @return attribute descriptor
 */
zb_zcl_attr_t* zb_zcl_get_attr_desc_manuf_a(zb_uint8_t ep,
                                            zb_uint16_t cluster_id,
                                            zb_uint8_t cluster_role,
                                            zb_uint16_t attr_id,
                                            zb_uint16_t manuf_code);

/**
 * Put attribute value to command packet, fix endian if needed.
 *
 * @param cmd_ptr - pointer to destination
 * @param attr_type - type of attribute being written (see @ref zcl_attr_type)
 * @param attr_value - pointer to value to be put into packet
 *
 * @return command pointer adjusted to point at the tail of written value
*/
zb_uint8_t* zb_zcl_put_value_to_packet(
    zb_uint8_t *cmd_ptr, zb_uint8_t attr_type, zb_uint8_t *attr_value);


/**
 * @brief Sets value for manufacture-specific attribute. Also, perform all needed
 * checks before and after setting new value
 *
 * @param ep - endpoint number
 * @param cluster_id - cluster ID (see @ref zb_zcl_cluster_id_t)
 * @param cluster_role - role (see @ref zcl_cluster_role)
 * @param attr_id - ID of the attribute being set
 * @param value - pointer to new value
 * @param check_access - whether read-only check should be performed
 * @param manuf_code - manufacturer code attribute
 *
 * @return ZB_ZCL_STATUS_SUCCESS if value was written \n
 *         ZB_ZCL_STATUS_READ_ONLY if attribute is read only and check_access is false
 *         ZB_ZCL_STATUS_UNSUP_ATTRIB if attribute is not supported by cluster
 *         ZB_ZCL_STATUS_INVALID_VALUE if current value of attribute is invalid
 * @note given endpoint with given cluster ID should exist
 */
zb_zcl_status_t zb_zcl_set_attr_val_manuf(zb_uint8_t ep,
                                          zb_uint16_t cluster_id,
                                          zb_uint8_t cluster_role,
                                          zb_uint16_t attr_id,
                                          zb_uint16_t manuf_code,
                                          zb_uint8_t *value,
                                          zb_bool_t check_access);

/**
 * @brief Sets attribute value, perform all needed checks before and after setting new value
 *
 * @param ep - endpoint number
 * @param cluster_id - cluster ID (see @ref zb_zcl_cluster_id_t)
 * @param cluster_role - role (see @ref zcl_cluster_role)
 * @param attr_id - ID of the attribute being set
 * @param value - pointer to new value
 * @param check_access - whether read-only check should be performed
 *
 * @return ZB_ZCL_STATUS_SUCCESS if value was written \n
 *         ZB_ZCL_STATUS_READ_ONLY if attribute is read only and check_access is false
 *         ZB_ZCL_STATUS_UNSUP_ATTRIB if attribute is not supported by cluster
 *         ZB_ZCL_STATUS_INVALID_VALUE if current value of attribute is invalid
 * @note given endpoint with given cluster ID should exist
 */
zb_zcl_status_t zb_zcl_set_attr_val(zb_uint8_t ep, zb_uint16_t cluster_id, zb_uint8_t cluster_role,
                             zb_uint16_t attr_id, zb_uint8_t *value, zb_bool_t check_access);

/**
 * @brief Register callback that should be called when APS acknowledge is received for ZCL command
 *
 * @param param - pointer to buffer with ZCL command
 * @param cb - pointer to callback function
 */
zb_ret_t zb_zcl_register_cb(zb_uint8_t param, zb_callback_t cb);

/**
 * @brief Call callback on receiving APS acknowledge for ZCL command
 *
 * @param param - buffer with ZCL command
 *
 * @return RET_OK if callback was found and called
 *         RET_NOT_FOUND if no callback was registered
 */
zb_ret_t zb_zcl_ack_callback(zb_uint8_t param);

/**
 * Check whether ZCL command should be handled by stack
 *
 * @param param - reference to buffer with parsed ZCL command
 *
 * @return ZB_ZCL_STATUS_SUCCESS if command should be handled
 *         ZB_ZCL_STATUS_UNSUP_MANUF_CLUST_CMD if manufacturer specific command is not supported
 *         ZB_ZCL_STATUS_UNSUP_CLUST_CMD if general command is not supported
 *         ZB_ZCL_STATUS_UNSUP_CLUST if cluster with role specified in ZCL header is not supported
 */
zb_uint8_t zb_zcl_check_accept_command(zb_uint8_t param);

/** @cond DOXYGEN_INTERNAL_DOC */
/*! Get endpoint ID that is under initialization at the moment
    @return endpoint ID
 */
zb_uint8_t zb_get_current_endpoint_id(void);

/** @brief Common continue command

    Usually use after invoke User App. If invoke User App finished with error (status!=RET_OK),
    send error response, else if not disabled default response send it or free buffer

    @param buffer - buffer for send response
    @param pcmd_info - pointer of cmd_info (see @ref zb_zcl_parsed_hdr_s)
    @param status - status of operation (see @ref zb_zcl_status_t)
*/
void zb_zcl_process_command_finish(zb_bufid_t buffer, zb_zcl_parsed_hdr_t *pcmd_info, zb_uint8_t status);
#define ZB_ZCL_PROCESS_COMMAND_FINISH(buffer, pcmd_info, status)        \
  zb_zcl_process_command_finish(buffer, pcmd_info, status)

void zb_zcl_process_command_finish_new(zb_bufid_t buffer, zb_zcl_parsed_hdr_t *pcmd_info, zb_uint8_t status);
#define ZB_ZCL_PROCESS_COMMAND_FINISH_NEW(buffer, pcmd_info, status)        \
  zb_zcl_process_command_finish_new(buffer, pcmd_info, status)

/** @brief Declare for change attribute for User Application
 *
 * @note Size of values must be twice as large as maximum size of a possible attribute
*/
typedef struct zb_zcl_set_attr_value_param_s
{
  zb_uint16_t cluster_id;   /**< Cluster ID*/
  zb_uint16_t attr_id;      /**< Attribute ID*/
  /*
   * Constant size of values - for correct alloc place in buffer tail, as
   * "ZB_BUF_GET_PARAM(buffer, zb_zcl_device_callback_param_t)".
   * Current size = maximum of fixed size of any attribute.
   * If will need to translate attribute with variable size then need
   * set value as size and pointer of attribute.
   * */
  union
  {
    zb_uint8_t data8;           /**< 8bit data */
    zb_uint16_t data16;         /**< 16bit data */
    zb_uint24_t data24;         /**< 24bit data */
    zb_uint32_t data32;         /**< 32bit data */
    zb_uint48_t data48;         /**< 48bit data */
    zb_ieee_addr_t data_ieee;   /**< 64bit data or IEEE address */
    struct
    {
      zb_uint8_t size;          /**< Size of data (byte) */
      zb_uint8_t *p_data;       /**< Pointer of data */
    } data_variable;            /**< Variable size data - string or byte array */
  }
  values;
} zb_zcl_set_attr_value_param_t;

/** @cond internals_doc */
/** @internal @brief Inform User Application about change attribute
 *  @param buffer - buffer for transmit data
 *  @param clusterId - cluster ID
 *  @param attrDesc - attribute description
 *  @param p_value - pointer of new value attribute
 *  @param result - [out] result user callback
*/
#define ZB_ZCL_INVOKE_USER_APP_SET_ATTR_WITH_RESULT(                           \
  buffer, ep, clusterId, attrDesc, p_value, result)                            \
{                                                                              \
  (result) = RET_OK;                                                           \
                                                                               \
  if (ZCL_CTX().device_cb != NULL)                                             \
  {                                                                            \
    zb_zcl_device_callback_param_t *data =                                     \
      ZB_BUF_GET_PARAM((buffer), zb_zcl_device_callback_param_t);              \
    zb_uint8_t size =                                                          \
      zb_zcl_get_attribute_size((attrDesc)->type, (attrDesc)->data_p);         \
                                                                               \
    switch ((attrDesc)->type)                              \
    {                                                                          \
      case ZB_ZCL_ATTR_TYPE_CHAR_STRING:                                       \
      case ZB_ZCL_ATTR_TYPE_OCTET_STRING:                                      \
      case ZB_ZCL_ATTR_TYPE_LONG_OCTET_STRING:                                 \
      case ZB_ZCL_ATTR_TYPE_ARRAY:                                             \
      case ZB_ZCL_ATTR_TYPE_CUSTOM_32ARRAY:                                    \
      case ZB_ZCL_ATTR_TYPE_128_BIT_KEY:                                       \
      {                                                                        \
        zb_uint8_t *ptr = (zb_uint8_t*)zb_buf_initial_alloc((buffer), size);   \
        data = ZB_BUF_GET_PARAM((buffer), zb_zcl_device_callback_param_t);     \
                                                                               \
        ZB_MEMCPY(ptr, (p_value), size);                                       \
        data->cb_param.set_attr_value_param.values.data_variable.size = size;  \
        data->cb_param.set_attr_value_param.values.data_variable.p_data = ptr; \
        break;                                                                 \
      }                                                                        \
                                                                               \
      default:                                                                 \
        ZB_MEMCPY(&(data->cb_param.set_attr_value_param.values),               \
                  (p_value),                                                   \
                  size);                                                       \
      break;                                                                   \
    }                                                                          \
                                                                               \
    if (data != NULL)                                                          \
    {                                                                          \
      data->device_cb_id = ZB_ZCL_SET_ATTR_VALUE_CB_ID;                        \
      data->endpoint = (ep);                                                   \
      data->attr_type = (attrDesc)->type;                                      \
      data->cb_param.set_attr_value_param.cluster_id = (clusterId);            \
      data->cb_param.set_attr_value_param.attr_id = (attrDesc)->id;            \
      (ZCL_CTX().device_cb)((buffer));                                         \
      (result) = data->status;                                                 \
    }                                                                          \
  }                                                                            \
}

/** @endcond */

/**
 * @brief Set ZCL backward compatibility modes
 *
 * @ref ZB_ZCL_LEGACY_MODE is default mode
 *
 * @param mode - see @ref zcl_backward_compatibility_modes
 *
 * @return RET_OK if ZCL backward compatibility modes were set
 * @return RET_OUT_OF_RANGE if parameter is out of range
 */
zb_ret_t zb_zcl_set_backward_comp_mode(zb_uint8_t mode);

/**
 * @brief Get ZCL backward compatibility modes
 *
 * @return current backward compatibility mode - see @ref zcl_backward_compatibility_modes
 */
zb_uint8_t zb_zcl_get_backward_comp_mode(void);

/**
 * @brief Get cluster revision depending on current ZCL backward compatibility mode, see @ref zcl_backward_compatibility_modes
 *
 * @param api_revision -
 * @param dst_addr destination address pointer (see zb_addr_u)
 * @param dst_addr_mode destination address mode (only @ref ZB_APS_ADDR_MODE_16_GROUP_ENDP_NOT_PRESENT and
 * @ref ZB_APS_ADDR_MODE_16_ENDP_PRESENT are supported)
 * @param dst_ep destination endpoint
 * @param cluster_id cluster identifier
 * @param cluster_role cluster role
 * @param src_ep source endpoint
 *
 * @return cluster revision for the specific cluster
 */
zb_uint16_t zb_zcl_get_cluster_rev_by_mode(zb_uint16_t api_revision, const zb_addr_u *dst_addr,
                                           zb_uint8_t dst_addr_mode, zb_uint8_t dst_ep,
                                           zb_uint16_t cluster_id, zb_uint8_t cluster_role,
                                           zb_uint8_t src_ep);

/**
 * @brief Set peer revision callback, see @ref zb_zcl_peer_revision_cb_t
 * If the callback is set, ZB_ZCL_COMPATIBILITY_MODE will be enabled automatically
 *
 * @param cb - peer revision callback, NULL if the callback should be removed
 *
 * @return RET_OK if callback was set, otherwise an error
 */
zb_ret_t zb_zcl_set_peer_revision_callback(zb_zcl_peer_revision_cb_t cb);

/**
 * @brief Returns Backward compatible status values mode
 * When enabled it diversifies some common statuses as it was in ZCL6 and ZCL7
 *
 * @return @ref zcl_backward_compatibility_status_modes
 */
zb_uint8_t zb_zcl_get_backward_compatible_statuses_mode(void);

/**
 * @brief Set Backward compatible status values mode
 *
 * @param statuses_mode - see @ref zcl_backward_compatibility_status_modes
 *
 * @return RET_OK if the mode was set, RET_OUT_OF_RANGE if value is invalid
 */
zb_ret_t zb_zcl_set_backward_compatible_statuses_mode(zb_uint8_t statuses_mode);

/**
 * @}
 */

void zb_zcl_schedule_status_abort(zb_bufid_t  buffer, zb_addr_u *addr,
                                  zb_uint8_t dst_addr_mode, zb_uint8_t dst_ep,
                                  zb_uint8_t ep, zb_callback_t cb);

zb_bool_t zb_zcl_handle_specific_commands(zb_uint8_t param);
zb_bool_t cluster_needs_aps_encryption(zb_uint8_t endpoint_id, zb_uint16_t cluster_id);

/** @endcond */ /* DOXYGEN_ZCL_SECTION */
#endif /* ZB_ZCL_COMMON_H */
