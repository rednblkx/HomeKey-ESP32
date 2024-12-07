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
/* PURPOSE: WWAH cluster definitions
*/

#ifndef ZB_ZCL_WWAH_H
#define ZB_ZCL_WWAH_H 1

#include "zcl/zb_zcl_common.h"
#include "zcl/zb_zcl_commands.h"

/** @cond DOXYGEN_ZCL_SECTION */

/* Cluster ZB_ZCL_CLUSTER_ID_WWAH */

/** @addtogroup ZB_ZCL_WWAH
 *  @{
 *     @details
 *     This cluster provides an opportunity to use various features
 *     of the Work With All Hubs cluster.
 *
 *     The WWAH cluster shall exist on only 1 endpoint of the Zigbee node,
 *     it should be identified using cluster ID 0xfc57 and manufacturer ID 0x1217.
 *
 *     All configurations/command settings in this cluster shall be persisted across power cycles
 *     and rejoins. Only a device factory reset can reset this cluster and put it back to default
 *     settings.
 *
 *     For more information see Zigbee WWAH Cluster definition (document number in Zigbee Alliance
 *     17-01067-022).
 *
 */


/*! @defgroup ZB_ZCL_WWAH_ATTR WWAH cluster attributes
    @{
*/

/* Amazon Lab 126 manufacturer code */
#define WWAH_MANUFACTURER_CODE 0x1217

/*! @brief WWAH cluster attribute identifiers
    @see WWAH spec, subclause 6.3.2.2
*/
enum zb_zcl_wwah_attr_e
{
  /** If set to TRUE, the device SHALL disallow downgrades of it's firmware. */
  ZB_ZCL_ATTR_WWAH_DISABLE_OTA_DOWNGRADES_ID                   = 0x0002,
  /** If set to FALSE, the node SHALL ignore MGMT Leave Without Rejoin commands. */
  ZB_ZCL_ATTR_WWAH_MGMT_LEAVE_WITHOUT_REJOIN_ENABLED_ID        = 0x0003,
  /** This attribute is a fixed value containing the number of Network Level Retries
   *  the device performs when sending a unicast command, not including APS retries. */
  ZB_ZCL_ATTR_WWAH_NWK_RETRY_COUNT_ID                          = 0x0004,
  /** This attribute is a fixed value containing the number of MAC Level Retries the
   *  device performs when sending a unicast command, not including APS retries. */
  ZB_ZCL_ATTR_WWAH_MAC_RETRY_COUNT_ID                          = 0x0005,
  /** This attribute is set to TRUE if the router's check-in algorithm has been enabled
   *  through the 'Enable Periodic Router Check-Ins' command detailed below. */
  ZB_ZCL_ATTR_WWAH_ROUTER_CHECK_IN_ENABLED_ID                  = 0x0006,
  /** This attribute is set to FALSE if support for Touchlink Interpan messages has been
   *  disabled on the device. */
  ZB_ZCL_ATTR_WWAH_TOUCHLINK_INTERPAN_ENABLED_ID               = 0x0007,
  /** If set to TRUE, the device SHALL enable the WWAH Parent Classification
   *  Advertisements feature. */
  ZB_ZCL_ATTR_WWAH_WWAH_PARENT_CLASSIFICATION_ENABLED_ID       = 0x0008,
  /** This attribute is TRUE if the WWAH Application Event Retry Algorithm is enabled. */
  ZB_ZCL_ATTR_WWAH_WWAH_APP_EVENT_RETRY_ENABLED_ID             = 0x0009,
  /** This attribute is the queue size for re-delivery attempts in the WWAH Application
   *  Event Retry Algorithm. */
  ZB_ZCL_ATTR_WWAH_WWAH_APP_EVENT_RETRY_QUEUE_SIZE_ID          = 0x000A,
  /** This attribute is set to TRUE if the WWAH Rejoin Algorithm is enabled. */
  ZB_ZCL_ATTR_WWAH_WWAH_REJOIN_ENABLED_ID                      = 0x000B,
  /** This attribute is the time in seconds the device waits before retrying a data poll
   *  when a MAC level data poll fails for any reason. */
  ZB_ZCL_ATTR_WWAH_MAC_POLL_FAILURE_WAIT_TIME_ID               = 0x000C,
  /** This attribute indicates whether the device is able to be configured using ZDO
   *  commands that are not encrypted using the Trust Center Link Key. */
  ZB_ZCL_ATTR_WWAH_CONFIGURATION_MODE_ENABLED_ID               = 0x000D,
  /** This attribute contains the ID of the current debug report stored on the node. */
  ZB_ZCL_ATTR_WWAH_CURRENT_DEBUG_REPORT_ID_ID                  = 0x000E,
  /** If this attribute is set to TRUE, the node SHALL only process network
   *  key rotation commands which are sent via unicast and are encrypted by
   *  the Trust Center Link Key. */
  ZB_ZCL_ATTR_WWAH_TC_SECURITY_ON_NWK_KEY_ROTATION_ENABLED_ID  = 0x000F,
  /** If this attribute is set to TRUE, the endpoint device SHALL enable
   *  the WWAH Bad Parent Recovery feature. */
  ZB_ZCL_ATTR_WWAH_WWAH_BAD_PARENT_RECOVERY_ENABLED_ID         = 0x0010,
  /** This attribute contains the channel number of the only channel the device
   *  SHALL accept in a ZDO Mgmt Network Update command. */
  ZB_ZCL_ATTR_WWAH_PENDING_NETWORK_UPDATE_CHANNEL_ID           = 0x0011,
  /** This attribute contains the only short PAN ID the device SHALL accept in
   *  a NLME Network Update command. */
  ZB_ZCL_ATTR_WWAH_PENDING_NETWORK_UPDATE_PANID_ID             = 0x0012,
  /** This attribute contains the maximum time in seconds the device may be unavailable
   *  after completing its OTA download (while restarting, etc). */
  ZB_ZCL_ATTR_WWAH_OTA_MAX_OFFLINE_DURATION_ID                 = 0x0013,
};

/** @brief Minimum value for Cluster Revision attribute */
#define ZB_ZCL_WWAH_CLUSTER_REVISION_MIN_VALUE 0x0001

/** @brief Default value for WWAH cluster revision global attribute */
#define ZB_ZCL_WWAH_CLUSTER_REVISION_DEFAULT ZB_ZCL_WWAH_CLUSTER_REVISION_MIN_VALUE

/** @brief Default value for MGMTLeaveWithoutRejoinEnabled attribute */
#define ZB_ZCL_WWAH_MGMT_LEAVE_WITHOUT_REJOIN_ENABLED_DEFAULT_VALUE ZB_TRUE

/** @brief Minimum value for NWKRetryCount attribute */
#define ZB_ZCL_WWAH_NWK_RETRY_COUNT_MIN_VALUE 0x03

/** @brief Maximum value for NWKRetryCount attribute */
#define ZB_ZCL_WWAH_NWK_RETRY_COUNT_MAX_VALUE 0xFF

/** @brief Minimum value for MACRetryCount attribute */
#define ZB_ZCL_WWAH_MAC_RETRY_COUNT_MIN_VALUE 0x03

/** @brief Maximum value for MACRetryCount attribute */
#define ZB_ZCL_WWAH_MAC_RETRY_COUNT_MAX_VALUE 0xFF

/** @brief Default value for RouterCheckInEnabled attribute */
#define ZB_ZCL_WWAH_ROUTER_CHECK_IN_ENABLED_DEFAULT_VALUE ZB_FALSE

/** @brief Default value for WWAHParentClassificationEnabled attribute */
#define ZB_ZCL_WWAH_WWAH_PARENT_CLASSIFICATION_ENABLED_DEFAULT_VALUE ZB_FALSE

/** @brief Default value for WWAHAppEventRetryEnabled attribute */
#define ZB_ZCL_WWAH_WWAH_APP_EVENT_RETRY_ENABLED_DEFAULT_VALUE ZB_TRUE

/** @brief Minimum value for WWAHAppEventRetryQueueSize attribute */
#define ZB_ZCL_WWAH_WWAH_APP_EVENT_RETRY_QUEUE_SIZE_MIN_VALUE 10

/** @brief Maximum value for WWAHAppEventRetryQueueSize attribute */
#define ZB_ZCL_WWAH_WWAH_APP_EVENT_RETRY_QUEUE_SIZE_MAX_VALUE 0xFF

/** @brief Default value for WWAHAppEventRetryQueueSize attribute */
#define ZB_ZCL_WWAH_WWAH_APP_EVENT_RETRY_QUEUE_SIZE_DEFAULT_VALUE 10

/** @brief Default value for WWAHRejoinEnabled attribute */
#define ZB_ZCL_WWAH_WWAH_REJOIN_ENABLED_DEFAULT_VALUE ZB_FALSE

/** @brief Minimum value for MACPollFailureWaitTime attribute */
#define ZB_ZCL_WWAH_MAC_POLL_FAILURE_WAIT_TIME_MIN_VALUE 0x00

/** @brief Maximum value for MACPollFailureWaitTime attribute */
#define ZB_ZCL_WWAH_MAC_POLL_FAILURE_WAIT_TIME_MAX_VALUE 0xFF

/** @brief Default value for MACPollFailureWaitTime attribute */
#define ZB_ZCL_WWAH_MAC_POLL_FAILURE_WAIT_TIME_DEFAULT_VALUE 3

/** @brief Default value for ConfigurationModeEnabled attribute */
#define ZB_ZCL_WWAH_CONFIGURATION_MODE_ENABLED_DEFAULT_VALUE ZB_TRUE

/** @brief Minimum value for CurrentDebugReportID attribute */
#define ZB_ZCL_WWAH_CURRENT_DEBUG_REPORT_ID_MIN_VALUE 0x00

/** @brief Maximum value for CurrentDebugReportID attribute */
#define ZB_ZCL_WWAH_CURRENT_DEBUG_REPORT_ID_MAX_VALUE 0xFF

/** @brief Default value for TCSecurityOnNwkKeyRotationEnabled attribute */
#define ZB_ZCL_WWAH_TC_SECURITY_ON_NWK_KEY_ROTATION_ENABLED_DEFAULT_VALUE ZB_FALSE

/** @brief Default value for WWAHBadParentRecoveryEnabled attribute */
#define ZB_ZCL_WWAH_WWAH_BAD_PARENT_RECOVERY_ENABLED_DEFAULT_VALUE ZB_FALSE

/** @brief Minimum value for PendingNetworkUpdateChannel attribute */
#define ZB_ZCL_WWAH_PENDING_NETWORK_UPDATE_CHANNEL_MIN_VALUE 0x00

/** @brief Maximum value for PendingNetworkUpdateChannel attribute */
#define ZB_ZCL_WWAH_PENDING_NETWORK_UPDATE_CHANNEL_MAX_VALUE 0xFF

/** @brief Default value for PendingNetworkUpdateChannel attribute */
#define ZB_ZCL_WWAH_PENDING_NETWORK_UPDATE_CHANNEL_DEFAULT_VALUE 0xFF

/** @brief Minimum value for PendingNetworkUpdatePANID attribute */
#define ZB_ZCL_WWAH_PENDING_NETWORK_UPDATE_PANID_MIN_VALUE 0x0000

/** @brief Maximum value for PendingNetworkUpdatePANID attribute */
#define ZB_ZCL_WWAH_PENDING_NETWORK_UPDATE_PANID_MAX_VALUE 0xFFFF

/** @brief Default value for PendingNetworkUpdatePANID attribute */
#define ZB_ZCL_WWAH_PENDING_NETWORK_UPDATE_PANID_DEFAULT_VALUE 0xFFFF

/** @brief Minimum value for OTAMaxOfflineDuration attribute */
#define ZB_ZCL_WWAH_OTA_MAX_OFFLINE_DURATION_MIN_VALUE 0x0000

/** @brief Maximum value for OTAMaxOfflineDuration attribute */
#define ZB_ZCL_WWAH_OTA_MAX_OFFLINE_DURATION_MAX_VALUE 0xFFFF

/** @brief Additional MAC Poll retry count */
#define ZB_ZCL_WWAH_MAC_POLL_RETRY_COUNT 2

/*! @} */ /* WWAH cluster attributes */

/*! @defgroup ZB_ZCL_WWAH_COMMANDS WWAH cluster commands
    @{
*/

/*! @brief WWAH cluster command identifiers
    @see WWAH spec
*/
enum zb_zcl_wwah_cmd_e
{
  /** Enable APS Link Key Authorization command */
  ZB_ZCL_CMD_WWAH_ENABLE_APS_LINK_KEY_AUTHORIZATION_ID = 0x00,
  /** Disable APS Link Key Authorization command */
  ZB_ZCL_CMD_WWAH_DISABLE_APS_LINK_KEY_AUTHORIZATION_ID = 0x01,
  /** APS Link Key Authorization Query command */
  ZB_ZCL_CMD_WWAH_APS_LINK_KEY_AUTHORIZATION_QUERY_ID = 0x02,
  /** Request New APS Link Key command */
  ZB_ZCL_CMD_WWAH_REQUEST_NEW_APS_LINK_KEY_ID = 0x03,
  /** Enable WWAH App Event Retry Algorithm command */
  ZB_ZCL_CMD_WWAH_ENABLE_WWAH_APP_EVENT_RETRY_ALGORITHM_ID = 0x04,
  /** Disable WWAH App Event Retry Algorithm command */
  ZB_ZCL_CMD_WWAH_DISABLE_WWAH_APP_EVENT_RETRY_ALGORITHM_ID = 0x05,
  /** Request Time command */
  ZB_ZCL_CMD_WWAH_REQUEST_TIME_ID = 0x06,
  /** Enable WWAH Rejoin Algorithm command */
  ZB_ZCL_CMD_WWAH_ENABLE_WWAH_REJOIN_ALGORITHM_ID = 0x07,
  /** Disable WWAH Rejoin Algorithm command */
  ZB_ZCL_CMD_WWAH_DISABLE_WWAH_REJOIN_ALGORITHM_ID = 0x08,
  /** Set IAS Zone Enrollment Method command */
  ZB_ZCL_CMD_WWAH_SET_IAS_ZONE_ENROLLMENT_METHOD_ID = 0x09,
  /** Clear Binding Table command */
  ZB_ZCL_CMD_WWAH_CLEAR_BINDING_TABLE_ID = 0x0A,
  /** Enable Periodic Router Check Ins command */
  ZB_ZCL_CMD_WWAH_ENABLE_PERIODIC_ROUTER_CHECK_INS_ID = 0x0B,
  /** Disable Periodic Router Check Ins command */
  ZB_ZCL_CMD_WWAH_DISABLE_PERIODIC_ROUTER_CHECK_INS_ID = 0x0C,
  /** Set MAC Poll CCA Wait Time command */
  ZB_ZCL_CMD_WWAH_SET_MAC_POLL_CCA_WAIT_TIME_ID = 0x0D,
  /** Set Pending Network Update command */
  ZB_ZCL_CMD_WWAH_SET_PENDING_NETWORK_UPDATE_ID = 0x0E,
  /** Require APS ACKs on Unicasts command */
  ZB_ZCL_CMD_WWAH_REQUIRE_APS_ACKS_ON_UNICASTS_ID = 0x0F,
  /** Remove APS ACKs on Unicasts Requirement command */
  ZB_ZCL_CMD_WWAH_REMOVE_APS_ACKS_ON_UNICASTS_REQUIREMENT_ID = 0x10,
  /** APS ACK Requirement Query command */
  ZB_ZCL_CMD_WWAH_APS_ACK_REQUIREMENT_QUERY_ID = 0x11,
  /** Debug Report Query command */
  ZB_ZCL_CMD_WWAH_DEBUG_REPORT_QUERY_ID = 0x12,
  /** Survey Beacons command */
  ZB_ZCL_CMD_WWAH_SURVEY_BEACONS_ID = 0x13,
  /** Disable OTA Downgrades command */
  ZB_ZCL_CMD_WWAH_DISABLE_OTA_DOWNGRADES_ID = 0x14,
  /** Disable MGMT Leave Without Rejoin command */
  ZB_ZCL_CMD_WWAH_DISABLE_MGMT_LEAVE_WITHOUT_REJOIN_ID = 0x15,
  /** Disable Touchlink Interpan Message Support command */
  ZB_ZCL_CMD_WWAH_DISABLE_TOUCHLINK_INTERPAN_MESSAGE_SUPPORT_ID = 0x16,
  /** Enable WWAH Parent Classification command */
  ZB_ZCL_CMD_WWAH_ENABLE_WWAH_PARENT_CLASSIFICATION_ID = 0x17,
  /** Disable WWAH Parent Classification command */
  ZB_ZCL_CMD_WWAH_DISABLE_WWAH_PARENT_CLASSIFICATION_ID = 0x18,
  /** Enable TC Security On Nwk Key Rotation command */
  ZB_ZCL_CMD_WWAH_ENABLE_TC_SECURITY_ON_NWK_KEY_ROTATION_ID = 0x19,
  /** Enable WWAH Bad Parent Recovery command */
  ZB_ZCL_CMD_WWAH_ENABLE_WWAH_BAD_PARENT_RECOVERY_ID = 0x1A,
  /** Disable WWAH Bad Parent Recovery command */
  ZB_ZCL_CMD_WWAH_DISABLE_WWAH_BAD_PARENT_RECOVERY_ID = 0x1B,
  /** Enable Configuration Mode command */
  ZB_ZCL_CMD_WWAH_ENABLE_CONFIGURATION_MODE_ID = 0x1C,
  /** Disable Configuration Mode command */
  ZB_ZCL_CMD_WWAH_DISABLE_CONFIGURATION_MODE_ID = 0x1D,
  /** Use Trust Center for Cluster command */
  ZB_ZCL_CMD_WWAH_USE_TRUST_CENTER_FOR_CLUSTER_ID = 0x1E,
  /** Trust Center for Cluster Server Query command */
  ZB_ZCL_CMD_WWAH_TRUST_CENTER_FOR_CLUSTER_SERVER_QUERY_ID = 0x1F,
};

enum zb_zcl_wwah_cmd_resp_e
{
  /** APS Link Key Authorization Query Response command */
  ZB_ZCL_CMD_WWAH_APS_LINK_KEY_AUTHORIZATION_QUERY_RESPONSE_ID = 0x00,
  /** Powering Off Notification command */
  ZB_ZCL_CMD_WWAH_POWERING_OFF_NOTIFICATION_ID = 0x01,
  /** Powering On Notification command */
  ZB_ZCL_CMD_WWAH_POWERING_ON_NOTIFICATION_ID = 0x02,
  /** Short Address Change command */
  ZB_ZCL_CMD_WWAH_SHORT_ADDRESS_CHANGE_ID = 0x03,
  /** APS ACK Requirement Query Response command */
  ZB_ZCL_CMD_WWAH_APS_ACK_REQUIREMENT_QUERY_RESPONSE_ID = 0x04,
  /** Power Descriptor Change command */
  ZB_ZCL_CMD_WWAH_POWER_DESCRIPTOR_CHANGE_ID = 0x05,
  /** New Debug Report Notification command */
  ZB_ZCL_CMD_WWAH_NEW_DEBUG_REPORT_NOTIFICATION_ID = 0x06,
  /** Debug Report Query Response command */
  ZB_ZCL_CMD_WWAH_DEBUG_REPORT_QUERY_RESPONSE_ID = 0x07,
  /** Trust Center for Cluster Server Query Response command */
  ZB_ZCL_CMD_WWAH_TRUST_CENTER_FOR_CLUSTER_SERVER_QUERY_RESPONSE_ID = 0x08,
  /** Survey Beacons Response command */
  ZB_ZCL_CMD_WWAH_SURVEY_BEACONS_RESPONSE_ID = 0x09,
};


/* WWAH control cluster commands list : only for information - do not modify */
#define ZB_ZCL_CLUSTER_ID_WWAH_SERVER_ROLE_GENERATED_CMD_LIST        \
  ZB_ZCL_CMD_WWAH_APS_LINK_KEY_AUTHORIZATION_QUERY_RESPONSE_ID,      \
  ZB_ZCL_CMD_WWAH_POWERING_OFF_NOTIFICATION_ID,                      \
  ZB_ZCL_CMD_WWAH_POWERING_ON_NOTIFICATION_ID,                       \
  ZB_ZCL_CMD_WWAH_SHORT_ADDRESS_CHANGE_ID,                           \
  ZB_ZCL_CMD_WWAH_APS_ACK_REQUIREMENT_QUERY_RESPONSE_ID,             \
  ZB_ZCL_CMD_WWAH_POWER_DESCRIPTOR_CHANGE_ID,                        \
  ZB_ZCL_CMD_WWAH_NEW_DEBUG_REPORT_NOTIFICATION_ID,                  \
  ZB_ZCL_CMD_WWAH_DEBUG_REPORT_QUERY_RESPONSE_ID,                    \
  ZB_ZCL_CMD_WWAH_TRUST_CENTER_FOR_CLUSTER_SERVER_QUERY_RESPONSE_ID, \
  ZB_ZCL_CMD_WWAH_SURVEY_BEACONS_RESPONSE_ID

#define ZB_ZCL_CLUSTER_ID_WWAH_CLIENT_ROLE_RECEIVED_CMD_LIST ZB_ZCL_CLUSTER_ID_WWAH_SERVER_ROLE_GENERATED_CMD_LIST

#define ZB_ZCL_CLUSTER_ID_WWAH_CLIENT_ROLE_GENERATED_CMD_LIST        \
  ZB_ZCL_CMD_WWAH_ENABLE_APS_LINK_KEY_AUTHORIZATION_ID,              \
  ZB_ZCL_CMD_WWAH_DISABLE_APS_LINK_KEY_AUTHORIZATION_ID,             \
  ZB_ZCL_CMD_WWAH_APS_LINK_KEY_AUTHORIZATION_QUERY_ID,               \
  ZB_ZCL_CMD_WWAH_REQUEST_NEW_APS_LINK_KEY_ID,                       \
  ZB_ZCL_CMD_WWAH_ENABLE_WWAH_APP_EVENT_RETRY_ALGORITHM_ID,          \
  ZB_ZCL_CMD_WWAH_DISABLE_WWAH_APP_EVENT_RETRY_ALGORITHM_ID,         \
  ZB_ZCL_CMD_WWAH_REQUEST_TIME_ID,                                   \
  ZB_ZCL_CMD_WWAH_ENABLE_WWAH_REJOIN_ALGORITHM_ID,                   \
  ZB_ZCL_CMD_WWAH_DISABLE_WWAH_REJOIN_ALGORITHM_ID,                  \
  ZB_ZCL_CMD_WWAH_SET_IAS_ZONE_ENROLLMENT_METHOD_ID,                 \
  ZB_ZCL_CMD_WWAH_CLEAR_BINDING_TABLE_ID,                            \
  ZB_ZCL_CMD_WWAH_ENABLE_PERIODIC_ROUTER_CHECK_INS_ID,               \
  ZB_ZCL_CMD_WWAH_DISABLE_PERIODIC_ROUTER_CHECK_INS_ID,              \
  ZB_ZCL_CMD_WWAH_SET_MAC_POLL_CCA_WAIT_TIME_ID,                     \
  ZB_ZCL_CMD_WWAH_SET_PENDING_NETWORK_UPDATE_ID,                     \
  ZB_ZCL_CMD_WWAH_REQUIRE_APS_ACKS_ON_UNICASTS_ID,                   \
  ZB_ZCL_CMD_WWAH_REMOVE_APS_ACKS_ON_UNICASTS_REQUIREMENT_ID,        \
  ZB_ZCL_CMD_WWAH_APS_ACK_REQUIREMENT_QUERY_ID,                      \
  ZB_ZCL_CMD_WWAH_DEBUG_REPORT_QUERY_ID,                             \
  ZB_ZCL_CMD_WWAH_SURVEY_BEACONS_ID,                                 \
  ZB_ZCL_CMD_WWAH_DISABLE_OTA_DOWNGRADES_ID,                         \
  ZB_ZCL_CMD_WWAH_DISABLE_MGMT_LEAVE_WITHOUT_REJOIN_ID,              \
  ZB_ZCL_CMD_WWAH_DISABLE_TOUCHLINK_INTERPAN_MESSAGE_SUPPORT_ID,     \
  ZB_ZCL_CMD_WWAH_ENABLE_WWAH_PARENT_CLASSIFICATION_ID,              \
  ZB_ZCL_CMD_WWAH_DISABLE_WWAH_PARENT_CLASSIFICATION_ID,             \
  ZB_ZCL_CMD_WWAH_ENABLE_TC_SECURITY_ON_NWK_KEY_ROTATION_ID,         \
  ZB_ZCL_CMD_WWAH_ENABLE_WWAH_BAD_PARENT_RECOVERY_ID,                \
  ZB_ZCL_CMD_WWAH_DISABLE_WWAH_BAD_PARENT_RECOVERY_ID,               \
  ZB_ZCL_CMD_WWAH_ENABLE_CONFIGURATION_MODE_ID,                      \
  ZB_ZCL_CMD_WWAH_DISABLE_CONFIGURATION_MODE_ID,                     \
  ZB_ZCL_CMD_WWAH_USE_TRUST_CENTER_FOR_CLUSTER_ID,                   \
  ZB_ZCL_CMD_WWAH_TRUST_CENTER_FOR_CLUSTER_SERVER_QUERY_ID

#define ZB_ZCL_CLUSTER_ID_WWAH_SERVER_ROLE_RECEIVED_CMD_LIST ZB_ZCL_CLUSTER_ID_WWAH_CLIENT_ROLE_GENERATED_CMD_LIST


/** @brief Size of APS ACK Exempt Table */
#define ZB_ZCL_WWAH_APS_ACK_EXEMPT_TABLE_SIZE 10
ZB_ASSERT_VALUE_ALIGNED_TO_4(ZB_ZCL_WWAH_APS_ACK_EXEMPT_TABLE_SIZE * sizeof(zb_uint16_t));

/** @brief Size of APS Link Key Authorization Table */
#define ZB_ZCL_WWAH_APS_LINK_KEY_AUTHORIZATION_TABLE_SIZE 10
ZB_ASSERT_VALUE_ALIGNED_TO_4(ZB_ZCL_WWAH_APS_LINK_KEY_AUTHORIZATION_TABLE_SIZE * sizeof(zb_uint16_t));

/** @brief Size of Use Trust Center for Cluster Table */
#define ZB_ZCL_WWAH_USE_TRUST_CENTER_FOR_CLUSTER_TABLE_SIZE 4
ZB_ASSERT_VALUE_ALIGNED_TO_4(ZB_ZCL_WWAH_USE_TRUST_CENTER_FOR_CLUSTER_TABLE_SIZE * sizeof(zb_uint16_t));

#define ZB_ZCL_WWAH_CLUSTER_LIST_MAX_SIZE ZB_ZCL_WWAH_APS_ACK_EXEMPT_TABLE_SIZE

/** @brief WWAH Cluster arrays "record is free" flag */
#define ZB_ZCL_WWAH_CLUSTER_ID_FREE_RECORD 0xFFFF

/** @brief Debug Report ID field reserved value
 * The value 0x00 is reserved to indicate that there are no debug reports and SHALL not be used
 * as IDs of a debug report.*/
#define ZB_ZCL_WWAH_RESERVED_DEBUG_REPORT_ID 0x00

#define ZB_ZCL_WWAH_DEBUG_REPORT_FREE_RECORD (zb_zcl_wwah_debug_report_t){ZB_ZCL_WWAH_RESERVED_DEBUG_REPORT_ID, 0, NULL}

/** @brief Context WWAH cluster  */

typedef ZB_PACKED_PRE struct zb_zcl_wwah_debug_report_s
{
  zb_uint8_t report_id;   /**< Report ID. 0x00 means lack of report */
  zb_uint32_t report_size; /**< Report size */
  zb_char_t *report;     /**< Pointer to report data*/
}
ZB_PACKED_STRUCT zb_zcl_wwah_debug_report_t;

typedef ZB_PACKED_PRE struct zb_zcl_wwah_classification_mask_s
{
  zb_bitfield_t tc_connectivity  :1; /**< TC Connectivity value */
  zb_bitfield_t long_uptime :1; /**< Long Uptime value */
  zb_bitfield_t reserved :6; /**< Reserved */
}
ZB_PACKED_STRUCT
zb_zcl_wwah_classification_mask_t;

#define ZB_ZCL_WWAH_PERIODIC_CHECKINS_CLUSTER_MATCH_DESC_TIME (ZB_TIME_ONE_SECOND * 20)
#define ZB_ZCL_WWAH_PERIODIC_CHECKINS_MAX_FAILURE_CNT 3

#define ZB_ZCL_WWAH_PERIODIC_CHECKINS_READ_KEEPALIVE 0
#define ZB_ZCL_WWAH_PERIODIC_CHECKINS_READ_CLUSTER_REVISION 1
#define ZB_ZCL_WWAH_PERIODIC_CHECKINS_NOT_SUPPORTED 2

typedef ZB_PACKED_PRE struct zb_zcl_wwah_periodic_checkins_data_s
{
  zb_uint8_t poll_method;
  zb_uint8_t tsn;
  zb_uint8_t endpoint;
  zb_uint8_t failure_cnt;
  zb_uint32_t countdown;
  zb_uint16_t keepalive_base; /* Base timeout in seconds*/
  zb_uint16_t keepalive_jitter;
} ZB_PACKED_STRUCT zb_zcl_wwah_periodic_checkins_data_t;

typedef enum zb_zcl_wwah_bad_parent_recovery_signal_e
{
  ZB_ZCL_WWAH_BAD_PARENT_RECOVERY_RSSI_WITH_PARENT_BAD = 0,
  ZB_ZCL_WWAH_BAD_PARENT_RECOVERY_RSSI_WITH_PARENT_OK,

  ZB_ZCL_WWAH_BAD_PARENT_RECOVERY_APS_ACK_FAILED,
  ZB_ZCL_WWAH_BAD_PARENT_RECOVERY_APS_ACK_OK,

  ZB_ZCL_WWAH_BAD_PARENT_RECOVERY_POLL_CONTROL_CHECK_IN_FAILED,
  ZB_ZCL_WWAH_BAD_PARENT_RECOVERY_POLL_CONTROL_CHECK_IN_OK,

  ZB_ZCL_WWAH_BAD_PARENT_RECOVERY_RESERVED
} zb_zcl_wwah_bad_parent_recovery_signal_t;

typedef ZB_PACKED_PRE struct zb_zcl_wwah_bad_parent_recovery_data_s
{
  zb_bitfield_t started:1;
  zb_bitfield_t poll_control_checkin_failed_cnt:2;
  zb_bitfield_t reserved:5;
} ZB_PACKED_STRUCT zb_zcl_wwah_bad_parent_recovery_data_t;

typedef enum zb_zcl_wwah_behavior_e
{
  ZB_ZCL_WWAH_BEHAVIOR_OFF = 0,
  ZB_ZCL_WWAH_BEHAVIOR_CLIENT,
  ZB_ZCL_WWAH_BEHAVIOR_SERVER,

  ZB_ZCL_WWAH_BEHAVIOR_RESERVED
} zb_zcl_wwah_behavior_t;

/*! @} */ /* WWAH cluster commands */

/*!
    @cond internals_doc
    @internal @name WWAH cluster internals
    Internal structures for attribute representation in cluster definitions.
    @{
*/

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_WWAH_DISABLE_OTA_DOWNGRADES_ID(data_ptr)                  \
{                                                                                                    \
  ZB_ZCL_ATTR_WWAH_DISABLE_OTA_DOWNGRADES_ID,                                                        \
  ZB_ZCL_ATTR_TYPE_BOOL,                                                                             \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                                                      \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                                                \
  (void*) data_ptr                                                                                   \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_WWAH_MGMT_LEAVE_WITHOUT_REJOIN_ENABLED_ID(data_ptr)       \
{                                                                                                    \
  ZB_ZCL_ATTR_WWAH_MGMT_LEAVE_WITHOUT_REJOIN_ENABLED_ID,                                             \
  ZB_ZCL_ATTR_TYPE_BOOL,                                                                             \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                                                      \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                                                \
  (void*) data_ptr                                                                                   \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_WWAH_NWK_RETRY_COUNT_ID(data_ptr)                         \
{                                                                                                    \
  ZB_ZCL_ATTR_WWAH_NWK_RETRY_COUNT_ID,                                                               \
  ZB_ZCL_ATTR_TYPE_U8,                                                                               \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                                                      \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                                                \
  (void*) data_ptr                                                                                   \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_WWAH_MAC_RETRY_COUNT_ID(data_ptr)                         \
{                                                                                                    \
  ZB_ZCL_ATTR_WWAH_MAC_RETRY_COUNT_ID,                                                               \
  ZB_ZCL_ATTR_TYPE_U8,                                                                               \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                                                      \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                                                \
  (void*) data_ptr                                                                                   \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_WWAH_ROUTER_CHECK_IN_ENABLED_ID(data_ptr)                 \
{                                                                                                    \
  ZB_ZCL_ATTR_WWAH_ROUTER_CHECK_IN_ENABLED_ID,                                                       \
  ZB_ZCL_ATTR_TYPE_BOOL,                                                                             \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                                                      \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                                                \
  (void*) data_ptr                                                                                   \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_WWAH_TOUCHLINK_INTERPAN_ENABLED_ID(data_ptr)              \
{                                                                                                    \
  ZB_ZCL_ATTR_WWAH_TOUCHLINK_INTERPAN_ENABLED_ID,                                                    \
  ZB_ZCL_ATTR_TYPE_BOOL,                                                                             \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                                                      \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                                                \
  (void*) data_ptr                                                                                   \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_WWAH_WWAH_PARENT_CLASSIFICATION_ENABLED_ID(data_ptr)      \
{                                                                                                    \
  ZB_ZCL_ATTR_WWAH_WWAH_PARENT_CLASSIFICATION_ENABLED_ID,                                            \
  ZB_ZCL_ATTR_TYPE_BOOL,                                                                             \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                                                      \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                                                \
  (void*) data_ptr                                                                                   \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_WWAH_WWAH_APP_EVENT_RETRY_ENABLED_ID(data_ptr)            \
{                                                                                                    \
  ZB_ZCL_ATTR_WWAH_WWAH_APP_EVENT_RETRY_ENABLED_ID,                                                  \
  ZB_ZCL_ATTR_TYPE_BOOL,                                                                             \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                                                      \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                                                \
  (void*) data_ptr                                                                                   \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_WWAH_WWAH_APP_EVENT_RETRY_QUEUE_SIZE_ID(data_ptr)         \
{                                                                                                    \
  ZB_ZCL_ATTR_WWAH_WWAH_APP_EVENT_RETRY_QUEUE_SIZE_ID,                                               \
  ZB_ZCL_ATTR_TYPE_U8,                                                                               \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                                                      \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                                                \
  (void*) data_ptr                                                                                   \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_WWAH_WWAH_REJOIN_ENABLED_ID(data_ptr)                     \
{                                                                                                    \
  ZB_ZCL_ATTR_WWAH_WWAH_REJOIN_ENABLED_ID,                                                           \
  ZB_ZCL_ATTR_TYPE_BOOL,                                                                             \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                                                      \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                                                \
  (void*) data_ptr                                                                                   \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_WWAH_MAC_POLL_FAILURE_WAIT_TIME_ID(data_ptr)              \
{                                                                                                    \
  ZB_ZCL_ATTR_WWAH_MAC_POLL_FAILURE_WAIT_TIME_ID,                                                    \
  ZB_ZCL_ATTR_TYPE_U8,                                                                               \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                                                      \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                                                \
  (void*) data_ptr                                                                                   \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_WWAH_CONFIGURATION_MODE_ENABLED_ID(data_ptr)              \
{                                                                                                    \
  ZB_ZCL_ATTR_WWAH_CONFIGURATION_MODE_ENABLED_ID,                                                    \
  ZB_ZCL_ATTR_TYPE_BOOL,                                                                             \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                                                      \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                                                \
  (void*) data_ptr                                                                                   \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_WWAH_CURRENT_DEBUG_REPORT_ID_ID(data_ptr)                 \
{                                                                                                    \
  ZB_ZCL_ATTR_WWAH_CURRENT_DEBUG_REPORT_ID_ID,                                                       \
  ZB_ZCL_ATTR_TYPE_U8,                                                                               \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                                                      \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                                                \
  (void*) data_ptr                                                                                   \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_WWAH_TC_SECURITY_ON_NWK_KEY_ROTATION_ENABLED_ID(data_ptr)\
{                                                                                                    \
  ZB_ZCL_ATTR_WWAH_TC_SECURITY_ON_NWK_KEY_ROTATION_ENABLED_ID,                                       \
  ZB_ZCL_ATTR_TYPE_BOOL,                                                                             \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                                                      \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                                                \
  (void*) data_ptr                                                                                   \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_WWAH_WWAH_BAD_PARENT_RECOVERY_ENABLED_ID(data_ptr)        \
{                                                                                                    \
  ZB_ZCL_ATTR_WWAH_WWAH_BAD_PARENT_RECOVERY_ENABLED_ID,                                              \
  ZB_ZCL_ATTR_TYPE_BOOL,                                                                             \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                                                      \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                                                \
  (void*) data_ptr                                                                                   \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_WWAH_PENDING_NETWORK_UPDATE_CHANNEL_ID(data_ptr)          \
{                                                                                                    \
  ZB_ZCL_ATTR_WWAH_PENDING_NETWORK_UPDATE_CHANNEL_ID,                                                \
  ZB_ZCL_ATTR_TYPE_U8,                                                                               \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                                                      \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                                                \
  (void*) data_ptr                                                                                   \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_WWAH_PENDING_NETWORK_UPDATE_PANID_ID(data_ptr)            \
{                                                                                                    \
  ZB_ZCL_ATTR_WWAH_PENDING_NETWORK_UPDATE_PANID_ID,                                                  \
  ZB_ZCL_ATTR_TYPE_U16,                                                                              \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                                                      \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                                                \
  (void*) data_ptr                                                                                   \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_WWAH_OTA_MAX_OFFLINE_DURATION_ID(data_ptr)                \
{                                                                                                    \
  ZB_ZCL_ATTR_WWAH_OTA_MAX_OFFLINE_DURATION_ID,                                                      \
  ZB_ZCL_ATTR_TYPE_U16,                                                                              \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                                                      \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                                                \
  (void*) data_ptr                                                                                   \
}

typedef struct zb_zcl_wwah_cluster_list_s
{
  zb_uint8_t number_of_clusters;                              /**< Number of Clusters */
  zb_uint8_t alignment[3];                                    /**< Alignment */
  zb_uint16_t cluster_id[ZB_ZCL_WWAH_CLUSTER_LIST_MAX_SIZE];  /**< Cluster list */
}
zb_zcl_wwah_cluster_list_t;

/*!
  @brief Parses various commands with cluster list variable length payload and fills data request structure.
  @param data_buf - pointer to zb_buf_t buffer containing command request data
  @param req - variable to save command request
  @param status - success or not read parameters - ZB_TRUE or ZB_FALSE
  @note data_buf buffer should contain command request payload without ZCL header.
*/

#define ZB_ZCL_WWAH_GET_CLUSTER_LIST_CMD(data_buf, req, status)                                \
{                                                                                              \
  zb_uint8_t *src_ptr = (zb_uint8_t*)zb_buf_begin((data_buf));                                 \
  zb_uint8_t number_of_clusters = *src_ptr;                                                    \
  (status) = ZB_ZCL_PARSE_STATUS_FAILURE;                                                      \
  if (zb_buf_len((data_buf)) >= sizeof(zb_uint8_t))                                            \
  {                                                                                            \
    (req).number_of_clusters = number_of_clusters;                                             \
    if (number_of_clusters > 0)                                                                \
    {                                                                                          \
      if (zb_buf_len((data_buf)) >= sizeof(zb_uint8_t) +                                       \
          number_of_clusters * sizeof(zb_uint16_t))                                            \
      {                                                                                        \
        ZB_MEMCPY(                                                                             \
          (zb_uint8_t*)((req).cluster_id),                                                     \
          (src_ptr + 1),                                                                       \
          number_of_clusters * sizeof(zb_uint16_t));                                           \
        (status) = ZB_ZCL_PARSE_STATUS_SUCCESS;                                                \
      }                                                                                        \
    }                                                                                          \
    else                                                                                       \
    {                                                                                          \
      (status) = ZB_ZCL_PARSE_STATUS_SUCCESS;                                                  \
    }                                                                                          \
  }                                                                                            \
}

#define ZB_ZCL_WWAH_APS_LINK_KEY_AUTHORIZATION_QUERY_PAYLOAD_LEN (sizeof(zb_uint16_t))

/*!
  @brief Parses APS Link Key Authorization Query command and fills data request structure.
  @param data_buf - pointer to zb_buf_t buffer containing command request data
  @param req - variable to save command request
  @param status - success or not read parameters - ZB_TRUE or ZB_FALSE
  @note data_buf buffer should contain command request payload without ZCL header.
*/
#define ZB_ZCL_WWAH_GET_APS_LINK_KEY_AUTHORIZATION_QUERY(data_buf, req, status)           \
{                                                                                         \
  zb_uint16_t *req_ptr =                                                                  \
     zb_buf_len(data_buf) >= ZB_ZCL_WWAH_APS_LINK_KEY_AUTHORIZATION_QUERY_PAYLOAD_LEN ?   \
         (zb_uint16_t*)zb_buf_begin(data_buf) : NULL;                                     \
  if (req_ptr)                                                                            \
  {                                                                                       \
    ZB_MEMCPY(&(req), req_ptr, ZB_ZCL_WWAH_APS_LINK_KEY_AUTHORIZATION_QUERY_PAYLOAD_LEN); \
    status = ((req) == ZB_ZCL_WWAH_CLUSTER_ID_FREE_RECORD) ?                              \
      ZB_ZCL_PARSE_STATUS_FAILURE : ZB_ZCL_PARSE_STATUS_SUCCESS;                          \
                                                                                          \
  }                                                                                       \
  else                                                                                    \
  {                                                                                       \
    status = ZB_ZCL_PARSE_STATUS_FAILURE;                                                 \
  }                                                                                       \
}

/** @brief Enable WWAH App Event Retry Algorithm command structure */
typedef ZB_PACKED_PRE struct zb_zcl_wwah_enable_wwah_app_event_retry_algorithm_s
{
  zb_uint8_t first_backoff_time_in_seconds;          /**< First Backoff Time In Seconds */
  zb_uint8_t backoff_sequence_common_ratio;          /**< Backoff Sequence Common Ratio */
  zb_uint32_t max_backoff_time_in_seconds;           /**< Max Backoff Time In Seconds */
  zb_uint8_t max_re_delivery_attempts;               /**< Max Re-Delivery Attempts */
}
ZB_PACKED_STRUCT
zb_zcl_wwah_enable_wwah_app_event_retry_algorithm_t;

#define ZB_ZCL_WWAH_ENABLE_WWAH_APP_EVENT_RETRY_ALGORITHM_PAYLOAD_LEN (sizeof(zb_zcl_wwah_enable_wwah_app_event_retry_algorithm_t))
/*!
  @brief Parses Enable WWAH App Event Retry Algorithm command and fills data request structure.
  @param data_buf - pointer to zb_buf_t buffer containing command request data
  @param req - variable to save command request
  @param status - success or not read parameters - ZB_TRUE or ZB_FALSE
  @note data_buf buffer should contain command request payload without ZCL header.
*/
#define ZB_ZCL_WWAH_GET_ENABLE_WWAH_APP_EVENT_RETRY_ALGORITHM(data_buf, req, status)           \
{                                                                                              \
  zb_zcl_wwah_enable_wwah_app_event_retry_algorithm_t *req_ptr =                               \
     zb_buf_len(data_buf) >= ZB_ZCL_WWAH_ENABLE_WWAH_APP_EVENT_RETRY_ALGORITHM_PAYLOAD_LEN ?   \
         (zb_zcl_wwah_enable_wwah_app_event_retry_algorithm_t*)zb_buf_begin(data_buf) : NULL;  \
  if (req_ptr)                                                                                 \
  {                                                                                            \
    ZB_MEMCPY(&(req), req_ptr, ZB_ZCL_WWAH_ENABLE_WWAH_APP_EVENT_RETRY_ALGORITHM_PAYLOAD_LEN); \
    (status) = ZB_ZCL_PARSE_STATUS_SUCCESS;                                                    \
  }                                                                                            \
  else                                                                                         \
  {                                                                                            \
    (status) = ZB_ZCL_PARSE_STATUS_FAILURE;                                                    \
  }                                                                                            \
}

/** @brief Enable WWAH Rejoin Algorithm command structure */
typedef ZB_PACKED_PRE struct zb_zcl_wwah_enable_wwah_rejoin_algorithm_s
{
  zb_uint16_t fast_rejoin_timeout_in_seconds;           /**< Fast Rejoin Timeout In Seconds */
  zb_uint16_t duration_between_each_rejoin_in_seconds;  /**< Duration Between Each Rejoin In Seconds */
  zb_uint16_t fast_rejoin_first_backoff_in_seconds;     /**< Fast Rejoin First Backoff In Seconds */
  zb_uint16_t rejoin_max_backoff_time_in_seconds;       /**< Max Backoff Time In Seconds */
  zb_uint16_t max_backoff_iterations;                   /**< Max Backoff Iterations */
}
ZB_PACKED_STRUCT
zb_zcl_wwah_enable_wwah_rejoin_algorithm_t;

#define ZB_ZCL_WWAH_ENABLE_WWAH_REJOIN_ALGORITHM_PAYLOAD_LEN (sizeof(zb_zcl_wwah_enable_wwah_rejoin_algorithm_t))

/*!
  @brief Parses Enable WWAH Rejoin Algorithm command and fills data request structure.
  @param data_buf - pointer to zb_buf_t buffer containing command request data
  @param req - variable to save command request
  @param status - success or not read parameters - ZB_TRUE or ZB_FALSE
   @note data_buf buffer should contain command request payload without ZCL header.
*/
#define ZB_ZCL_WWAH_GET_ENABLE_WWAH_REJOIN_ALGORITHM(data_buf, req, status)           \
{                                                                                     \
  zb_zcl_wwah_enable_wwah_rejoin_algorithm_t *req_ptr =                               \
     zb_buf_len(data_buf) >= ZB_ZCL_WWAH_ENABLE_WWAH_REJOIN_ALGORITHM_PAYLOAD_LEN ?   \
         (zb_zcl_wwah_enable_wwah_rejoin_algorithm_t*)zb_buf_begin(data_buf) : NULL;  \
  if (req_ptr)                                                                        \
  {                                                                                   \
    ZB_MEMCPY(&(req), req_ptr, ZB_ZCL_WWAH_ENABLE_WWAH_REJOIN_ALGORITHM_PAYLOAD_LEN); \
    (status) = ZB_ZCL_PARSE_STATUS_SUCCESS;                                           \
  }                                                                                   \
  else                                                                                \
  {                                                                                   \
    (status) = ZB_ZCL_PARSE_STATUS_FAILURE;                                           \
  }                                                                                   \
}

#define ZB_ZCL_WWAH_SET_IAS_ZONE_ENROLLMENT_METHOD_PAYLOAD_LEN (sizeof(zb_uint8_t))
/*!
  @brief Parses Set IAS Zone Enrollment Method command and fills data request structure.
  @param data_buf - pointer to zb_buf_t buffer containing command request data
  @param req - variable to save command request
  @param status - success or not read parameters - ZB_TRUE or ZB_FALSE
  @note data_buf buffer should contain command request payload without ZCL header.
*/
#define ZB_ZCL_WWAH_GET_SET_IAS_ZONE_ENROLLMENT_METHOD(data_buf, req, status)           \
{                                                                                       \
  zb_uint8_t *req_ptr =                                                                 \
     zb_buf_len(data_buf) >= ZB_ZCL_WWAH_SET_IAS_ZONE_ENROLLMENT_METHOD_PAYLOAD_LEN ?   \
         (zb_uint8_t*)zb_buf_begin(data_buf) : NULL;                                    \
  if (req_ptr)                                                                          \
  {                                                                                     \
    ZB_MEMCPY(&(req), req_ptr, ZB_ZCL_WWAH_SET_IAS_ZONE_ENROLLMENT_METHOD_PAYLOAD_LEN); \
    switch(*(zb_uint8_t*)req_ptr)                                                       \
    {                                                                                   \
      case ZB_ZCL_WWAH_ENROLLMENT_MODE_TRIP_TO_PAIR:                                    \
      case ZB_ZCL_WWAH_ENROLLMENT_MODE_AUTO_ENROLL_RESPONSE:                            \
      case ZB_ZCL_WWAH_ENROLLMENT_MODE_AUTO_ENROLL_REQUEST:                             \
      {                                                                                 \
        status = ZB_ZCL_PARSE_STATUS_SUCCESS;                                           \
        break;                                                                          \
      }                                                                                 \
      default:                                                                          \
      {                                                                                 \
        status = ZB_ZCL_PARSE_STATUS_FAILURE;                                           \
        break;                                                                          \
      }                                                                                 \
    }                                                                                   \
  }                                                                                     \
  else                                                                                  \
  {                                                                                     \
    status = ZB_ZCL_PARSE_STATUS_FAILURE;                                               \
  }                                                                                     \
}

#define ZB_ZCL_WWAH_ENABLE_PERIODIC_ROUTER_CHECK_INS_PAYLOAD_LEN (sizeof(zb_uint16_t))

/*!
  @brief Parses Enable Periodic Router Check-Ins command and fills data request structure.
  @param data_buf - pointer to zb_buf_t buffer containing command request data
  @param req - variable to save command request
  @param status - success or not read parameters - ZB_TRUE or ZB_FALSE
  @note data_buf buffer should contain command request payload without ZCL header.
*/
#define ZB_ZCL_WWAH_GET_ENABLE_PERIODIC_ROUTER_CHECK_INS(data_buf, req, status)           \
{                                                                                         \
  zb_uint16_t *req_ptr =                                                                  \
     zb_buf_len(data_buf) >= ZB_ZCL_WWAH_ENABLE_PERIODIC_ROUTER_CHECK_INS_PAYLOAD_LEN ?   \
         (zb_uint16_t*)zb_buf_begin(data_buf) : NULL;                                     \
  if (req_ptr)                                                                            \
  {                                                                                       \
    ZB_MEMCPY(&(req), req_ptr, ZB_ZCL_WWAH_ENABLE_PERIODIC_ROUTER_CHECK_INS_PAYLOAD_LEN); \
    status = ZB_ZCL_PARSE_STATUS_SUCCESS;                                                 \
  }                                                                                       \
  else                                                                                    \
  {                                                                                       \
    status = ZB_ZCL_PARSE_STATUS_FAILURE;                                                 \
  }                                                                                       \
}

#define ZB_ZCL_WWAH_SET_MAC_POLL_CCA_WAIT_TIME_PAYLOAD_LEN (sizeof(zb_uint8_t))

/*!
  @brief Parses Set MAC Poll CCA Wait Time command and fills data request structure.
  @param data_buf - pointer to zb_buf_t buffer containing command request data
  @param req - variable to save command request
  @param status - success or not read parameters - ZB_TRUE or ZB_FALSE
  @note data_buf buffer should contain command request payload without ZCL header.
*/
#define ZB_ZCL_WWAH_GET_SET_MAC_POLL_CCA_WAIT_TIME(data_buf, req, status)           \
{                                                                                   \
  zb_uint8_t *req_ptr =                                                             \
     zb_buf_len(data_buf) >= ZB_ZCL_WWAH_SET_MAC_POLL_CCA_WAIT_TIME_PAYLOAD_LEN ?   \
         (zb_uint8_t*)zb_buf_begin(data_buf) : NULL;                                \
  if (req_ptr)                                                                      \
  {                                                                                 \
    ZB_MEMCPY(&(req), req_ptr, ZB_ZCL_WWAH_SET_MAC_POLL_CCA_WAIT_TIME_PAYLOAD_LEN); \
    status = ZB_ZCL_PARSE_STATUS_SUCCESS;                                           \
  }                                                                                 \
  else                                                                              \
  {                                                                                 \
    status = ZB_ZCL_PARSE_STATUS_FAILURE;                                           \
  }                                                                                 \
}

/** @brief Set Pending Network Update command structure */
typedef ZB_PACKED_PRE struct zb_zcl_wwah_set_pending_network_update_s
{
  zb_uint8_t channel;  /**< Channel */
  zb_uint16_t pan_id;  /**< PAN ID */
}
ZB_PACKED_STRUCT
zb_zcl_wwah_set_pending_network_update_t;

#define ZB_ZCL_WWAH_SET_PENDING_NETWORK_UPDATE_PAYLOAD_LEN (sizeof(zb_zcl_wwah_set_pending_network_update_t))
/*!
  @brief Parses Set Pending Network Update command and fills data request structure.
  @param data_buf - pointer to zb_buf_t buffer containing command request data
  @param req - variable to save command request
  @param status - success or not read parameters - ZB_TRUE or ZB_FALSE
  @note data_buf buffer should contain command request payload without ZCL header.
*/
#define ZB_ZCL_WWAH_GET_SET_PENDING_NETWORK_UPDATE(data_buf, req, status)           \
{                                                                                   \
  zb_zcl_wwah_set_pending_network_update_t *req_ptr =                               \
     zb_buf_len(data_buf) >= ZB_ZCL_WWAH_SET_PENDING_NETWORK_UPDATE_PAYLOAD_LEN ?   \
         (zb_zcl_wwah_set_pending_network_update_t*)zb_buf_begin(data_buf) : NULL;  \
  if (req_ptr)                                                                      \
  {                                                                                 \
    ZB_MEMCPY(&(req), req_ptr, ZB_ZCL_WWAH_SET_PENDING_NETWORK_UPDATE_PAYLOAD_LEN); \
    status = ZB_ZCL_PARSE_STATUS_SUCCESS;                                           \
  }                                                                                 \
  else                                                                              \
  {                                                                                 \
    status = ZB_ZCL_PARSE_STATUS_FAILURE;                                           \
  }                                                                                 \
}

#define ZB_ZCL_WWAH_DEBUG_REPORT_QUERY_PAYLOAD_LEN (sizeof(zb_uint8_t))
/*!
  @brief Parses Debug Report Query command and fills data request structure.
  @param data_buf - pointer to zb_buf_t buffer containing command request data
  @param req - variable to save command request
  @param status - success or not read parameters - ZB_TRUE or ZB_FALSE
  @note data_buf buffer should contain command request payload without ZCL header.
*/
#define ZB_ZCL_WWAH_GET_DEBUG_REPORT_QUERY(data_buf, req, status)           \
{                                                                           \
  zb_uint8_t *req_ptr =                                                     \
     zb_buf_len(data_buf) >= ZB_ZCL_WWAH_DEBUG_REPORT_QUERY_PAYLOAD_LEN ?   \
         (zb_uint8_t*)zb_buf_begin(data_buf) : NULL;                        \
  if (req_ptr)                                                              \
  {                                                                         \
    ZB_MEMCPY(&(req), req_ptr, ZB_ZCL_WWAH_DEBUG_REPORT_QUERY_PAYLOAD_LEN); \
    status = ZB_ZCL_PARSE_STATUS_SUCCESS;                                   \
  }                                                                         \
  else                                                                      \
  {                                                                         \
    status = ZB_ZCL_PARSE_STATUS_FAILURE;                                   \
  }                                                                         \
}

#define ZB_ZCL_WWAH_SURVEY_BEACONS_PAYLOAD_LEN (sizeof(zb_uint8_t))
/*!
  @brief Parses Survey Beacons command and fills data request structure.
  @param data_buf - pointer to zb_buf_t buffer containing command request data
  @param req - variable to save command request
  @param status - success or not read parameters - ZB_TRUE or ZB_FALSE
  @note data_buf buffer should contain command request payload without ZCL header.
*/
#define ZB_ZCL_WWAH_GET_SURVEY_BEACONS(data_buf, req, status)           \
{                                                                       \
  zb_uint8_t *req_ptr =                                                 \
     zb_buf_len(data_buf) >= ZB_ZCL_WWAH_SURVEY_BEACONS_PAYLOAD_LEN ?   \
         (zb_uint8_t*)zb_buf_begin(data_buf) : NULL;                    \
  if (req_ptr)                                                          \
  {                                                                     \
    ZB_MEMCPY(&(req), req_ptr, ZB_ZCL_WWAH_SURVEY_BEACONS_PAYLOAD_LEN); \
    status = ZB_ZCL_PARSE_STATUS_SUCCESS;                               \
  }                                                                     \
  else                                                                  \
  {                                                                     \
    status = ZB_ZCL_PARSE_STATUS_FAILURE;                               \
  }                                                                     \
}

/** @brief APS Link Key Authorization Query Response command structure */
typedef ZB_PACKED_PRE struct zb_zcl_wwah_aps_link_key_authorization_query_response_s
{
  zb_uint16_t cluster_id;                        /**< Cluster ID */
  zb_uint8_t aps_link_key_authorization_status;  /**< APS Link Key Authorization Status  */
}
ZB_PACKED_STRUCT
zb_zcl_wwah_aps_link_key_authorization_query_response_t;

#define ZB_ZCL_WWAH_APS_LINK_KEY_AUTHORIZATION_QUERY_RESPONSE_PAYLOAD_LEN (sizeof(zb_zcl_wwah_aps_link_key_authorization_query_response_t))
/*!
  @brief Parses APS Link Key Authorization Query Response command and fills data request structure.
  @param data_buf - pointer to zb_buf_t buffer containing command request data
  @param req - variable to save command request
  @param status - success or not read parameters - ZB_TRUE or ZB_FALSE
  @note data_buf buffer should contain command request payload without ZCL header.
*/
#define ZB_ZCL_WWAH_GET_APS_LINK_KEY_AUTHORIZATION_QUERY_RESPONSE(data_buf, req, status)           \
{                                                                                                  \
  zb_zcl_wwah_aps_link_key_authorization_query_response_t *req_ptr =                               \
     zb_buf_len(data_buf) >= ZB_ZCL_WWAH_APS_LINK_KEY_AUTHORIZATION_QUERY_RESPONSE_PAYLOAD_LEN ?   \
         (zb_zcl_wwah_aps_link_key_authorization_query_response_t*)zb_buf_begin(data_buf) : NULL;  \
  if (req_ptr)                                                                                     \
  {                                                                                                \
    ZB_MEMCPY(&(req), req_ptr, ZB_ZCL_WWAH_APS_LINK_KEY_AUTHORIZATION_QUERY_RESPONSE_PAYLOAD_LEN); \
    status = ZB_ZCL_PARSE_STATUS_SUCCESS;                                                          \
  }                                                                                                \
  else                                                                                             \
  {                                                                                                \
    status = ZB_ZCL_PARSE_STATUS_FAILURE;                                                          \
  }                                                                                                \
}

/** @brief Short Address Change command structure */
typedef ZB_PACKED_PRE struct zb_zcl_wwah_short_address_change_s
{
  zb_ieee_addr_t device_eui64;  /**< DeviceEUI64 */
  zb_uint16_t device_short;     /**< DeviceShort */
}
ZB_PACKED_STRUCT
zb_zcl_wwah_short_address_change_t;

#define ZB_ZCL_WWAH_SHORT_ADDRESS_CHANGE_PAYLOAD_LEN (sizeof(zb_zcl_wwah_short_address_change_t))
/*!
  @brief Parses Short Address Change command and fills data request structure.
  @param data_buf - pointer to zb_buf_t buffer containing command request data
  @param req - variable to save command request
  @param status - success or not read parameters - ZB_TRUE or ZB_FALSE
  @note data_buf buffer should contain command request payload without ZCL header.
*/
#define ZB_ZCL_WWAH_GET_SHORT_ADDRESS_CHANGE(data_buf, req, status)           \
{                                                                             \
  zb_zcl_wwah_short_address_change_t *req_ptr =                               \
     zb_buf_len(data_buf) >= ZB_ZCL_WWAH_SHORT_ADDRESS_CHANGE_PAYLOAD_LEN ?   \
         (zb_zcl_wwah_short_address_change_t*)zb_buf_begin(data_buf) : NULL;  \
  if (req_ptr)                                                                \
  {                                                                           \
    ZB_MEMCPY(&(req), req_ptr, ZB_ZCL_WWAH_SHORT_ADDRESS_CHANGE_PAYLOAD_LEN); \
    status = ZB_ZCL_PARSE_STATUS_SUCCESS;                                     \
  }                                                                           \
  else                                                                        \
  {                                                                           \
    status = ZB_ZCL_PARSE_STATUS_FAILURE;                                     \
  }                                                                           \
}

/** @brief Power Descriptor Change command structure */
typedef ZB_PACKED_PRE struct zb_zcl_wwah_power_descriptor_change_s
{
  zb_uint32_t current_power_mode;         /**< Current power mode */
  zb_uint32_t available_power_sources;    /**< Available power sources */
  zb_uint32_t current_power_source;       /**< Current power source */
  zb_uint32_t current_power_source_level; /**< Current power source level */
}
ZB_PACKED_STRUCT
zb_zcl_wwah_power_descriptor_change_t;


/*!
  @brief Parses Power Descriptor Change command and fills data request structure.
  @param data_buf - pointer to zb_buf_t buffer containing command request data
  @param req - variable to save command request
  @param status - success or not read parameters - ZB_TRUE or ZB_FALSE
  @note data_buf buffer should contain command request payload without ZCL header.
*/
#define ZB_ZCL_WWAH_GET_POWER_DESCRIPTOR_CHANGE(data_buf, req, status)           \
{                                                                                       \
  zb_zcl_wwah_power_descriptor_change_t *req_ptr =                               \
     zb_buf_len(data_buf) >= ZB_ZCL_WWAH_POWER_DESCRIPTOR_CHANGE_PAYLOAD_LEN ?   \
         (zb_zcl_wwah_power_descriptor_change_t*)zb_buf_begin(data_buf) : NULL;  \
  if (req_ptr)                                                                          \
  {                                                                                     \
    ZB_MEMCPY(&(req), req_ptr, sizeof(zb_zcl_wwah_power_descriptor_change_t));   \
    status = ZB_ZCL_PARSE_STATUS_SUCCESS;                                               \
  }                                                                                     \
  else                                                                                  \
  {                                                                                     \
    status = ZB_ZCL_PARSE_STATUS_FAILURE;                                               \
  }                                                                                     \
}

/** @brief Powering On/Off Notification command structure */
typedef ZB_PACKED_PRE struct zb_zcl_wwah_powering_on_off_notification_s
{
  zb_uint8_t power_notification_reason;  /**< PowerNotificationReason */
  zb_uint16_t manufacturer_id;  /**< Manufacturer ID */
  zb_uint8_t manufacturer_power_notification_reason_length;  /**< Manufacturer Power Notification Reason Length */
  zb_uint8_t *manufacturer_power_notification_reason;  /**< Manufacturer Power Notification Reason */
}
ZB_PACKED_STRUCT
zb_zcl_wwah_powering_on_off_notification_t;

#define ZB_ZCL_WWAH_POWERING_ON_OFF_NOTIFICATION_PAYLOAD_LEN (sizeof(zb_uint8_t) + sizeof(zb_uint16_t)+ sizeof(zb_uint8_t))
/*!
  @brief Parses Powering On/Off Notification command and fills data request structure.
  @param data_buf - pointer to zb_buf_t buffer containing command request data
  @param req - variable to save command request
  @param status - success or not read parameters - ZB_TRUE or ZB_FALSE
  @note data_buf buffer should contain command request payload without ZCL header.
*/
#define ZB_ZCL_WWAH_GET_POWERING_ON_OFF_NOTIFICATION(data_buf, req, status)                    \
{                                                                                              \
  zb_zcl_wwah_powering_on_off_notification_t *src_ptr =                                        \
    (zb_zcl_wwah_powering_on_off_notification_t*)zb_buf_begin((data_buf));                     \
  (status) = ZB_ZCL_PARSE_STATUS_FAILURE;                                                      \
  if (zb_buf_len((data_buf)) >= ZB_ZCL_WWAH_POWERING_ON_OFF_NOTIFICATION_PAYLOAD_LEN)          \
  {                                                                                            \
    ZB_MEMCPY(&(req), req_ptr, ZB_ZCL_WWAH_POWERING_ON_OFF_NOTIFICATION_PAYLOAD_LEN);          \
    if (src_ptr->manufacturer_power_notification_reason_length)                                \
    {                                                                                          \
      if (zb_buf_len((data_buf)) >= ZB_ZCL_WWAH_POWERING_ON_OFF_NOTIFICATION_PAYLOAD_LEN +     \
          src_ptr->manufacturer_power_notification_reason_length * sizeof(zb_uint8_t))         \
      {                                                                                        \
        (req)->manufacturer_power_notification_reason =                                        \
          (zb_uint8_t*)(&(src_ptr->manufacturer_power_notification_reason));                   \
        (status) = ZB_ZCL_PARSE_STATUS_SUCCESS;                                                \
      }                                                                                        \
    }                                                                                          \
    else                                                                                       \
    {                                                                                          \
      (status) = ZB_ZCL_PARSE_STATUS_SUCCESS;                                                  \
    }                                                                                          \
  }                                                                                            \
}

/** @brief New Debug Report Notification command structure */
typedef ZB_PACKED_PRE struct zb_zcl_wwah_new_debug_report_notification_s
{
  zb_uint8_t debug_report_id;  /**< Debug Report ID */
  zb_uint32_t size_of_report;  /**< Size of Report */
}
ZB_PACKED_STRUCT
zb_zcl_wwah_new_debug_report_notification_t;

#define ZB_ZCL_WWAH_NEW_DEBUG_REPORT_NOTIFICATION_PAYLOAD_LEN (sizeof(zb_zcl_wwah_new_debug_report_notification_t))
/*!
  @brief Parses New Debug Report Notification command and fills data request structure.
  @param data_buf - pointer to zb_buf_t buffer containing command request data
  @param req - variable to save command request
  @param status - success or not read parameters - ZB_TRUE or ZB_FALSE
  @note data_buf buffer should contain command request payload without ZCL header.
*/
#define ZB_ZCL_WWAH_GET_NEW_DEBUG_REPORT_NOTIFICATION(data_buf, req, status)            \
{                                                                                       \
  zb_zcl_wwah_new_debug_report_notification_t *req_ptr =                                \
     zb_buf_len(data_buf) >= ZB_ZCL_WWAH_NEW_DEBUG_REPORT_NOTIFICATION_PAYLOAD_LEN ?    \
         (zb_zcl_wwah_new_debug_report_notification_t*)zb_buf_begin(data_buf) : NULL;   \
  if (req_ptr)                                                                          \
  {                                                                                     \
    ZB_MEMCPY(&(req), req_ptr, ZB_ZCL_WWAH_NEW_DEBUG_REPORT_NOTIFICATION_PAYLOAD_LEN);  \
    status = ZB_ZCL_PARSE_STATUS_SUCCESS;                                               \
  }                                                                                     \
  else                                                                                  \
  {                                                                                     \
    status = ZB_ZCL_PARSE_STATUS_FAILURE;                                               \
  }                                                                                     \
}

/** @brief Debug Report Query Response command structure */
typedef ZB_PACKED_PRE struct zb_zcl_wwah_debug_report_query_response_s
{
  zb_uint8_t debug_report_id;  /**< Debug Report ID */
  zb_uint8_t *report_data;  /**< Report data */
}
ZB_PACKED_STRUCT
zb_zcl_wwah_debug_report_query_response_t;

/*!
  @brief Parses Debug Report Query Response command and fills data request structure.
  @param data_buf - pointer to zb_buf_t buffer containing command request data
  @param req - variable to save command request
  @param status - success or not read parameters - ZB_TRUE or ZB_FALSE
  @note data_buf buffer should contain command request payload without ZCL header.
*/
#define ZB_ZCL_WWAH_GET_DEBUG_REPORT_QUERY_RESPONSE(data_buf, req, status)                     \
{                                                                                              \
  zb_zcl_wwah_debug_report_query_response_t *src_ptr =                                         \
    (zb_zcl_wwah_debug_report_query_response_t*)zb_buf_begin((data_buf));                      \
  (status) = ZB_ZCL_PARSE_STATUS_FAILURE;                                                      \
  if (zb_buf_len((data_buf)) > sizeof(zb_uint8_t))                                             \
  {                                                                                            \
    (req)->debug_report_id = src_ptr->debug_report_id;                                         \
    (req)->report_data = (zb_uint8_t*)(&(src_ptr->report_data));                               \
    (status) = ZB_ZCL_PARSE_STATUS_SUCCESS;                                                    \
  }                                                                                            \
}

/*! @brief Structure saved all WWAH attributes */
typedef struct zb_zcl_wwah_attr_s
{
  zb_uint16_t cluster_revision;
  zb_bool_t disable_ota_downgrades;
  zb_bool_t mgmt_leave_without_rejoin_enabled;
  zb_uint8_t nwk_retry_count;
  zb_uint8_t mac_retry_count;
  zb_bool_t router_check_in_enabled;
  zb_bool_t touchlink_interpan_enabled;
  zb_bool_t wwah_parent_classification_enabled;
  zb_bool_t wwah_app_event_retry_enabled;
  zb_uint8_t wwah_app_event_retry_queue_size;
  zb_bool_t wwah_rejoin_enabled;
  zb_uint8_t mac_poll_failure_wait_time;
  zb_bool_t configuration_mode_enabled;
  zb_uint8_t current_debug_report_id;
  zb_bool_t tc_security_on_nwk_key_rotation_enabled;
  zb_bool_t wwah_bad_parent_recovery_enabled;
  zb_uint8_t pending_network_update_channel;
  zb_uint16_t pending_network_update_panid;
  zb_uint16_t ota_max_offline_duration;
} zb_zcl_wwah_attr_t;

extern zb_zcl_wwah_attr_t wwah_attr;

/** @internal @brief Declare attribute list for WWAH cluster
    @param attr_list - attribute list name
*/

#define ZB_ZCL_DECLARE_WWAH_ATTRIB_LIST(attr_list)  \
  zb_zcl_attr_t attr_list [] = {                    \
  {                                                 \
    ZB_ZCL_ATTR_GLOBAL_CLUSTER_REVISION_ID,         \
    ZB_ZCL_ATTR_TYPE_U16,                           \
    ZB_ZCL_ATTR_ACCESS_READ_ONLY,                   \
    (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),             \
    (void*) &(wwah_attr.cluster_revision)           \
  },                                                \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_WWAH_DISABLE_OTA_DOWNGRADES_ID, &(wwah_attr.disable_ota_downgrades)) \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_WWAH_MGMT_LEAVE_WITHOUT_REJOIN_ENABLED_ID, &(wwah_attr.mgmt_leave_without_rejoin_enabled)) \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_WWAH_NWK_RETRY_COUNT_ID, &(wwah_attr.nwk_retry_count)) \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_WWAH_MAC_RETRY_COUNT_ID, &(wwah_attr.mac_retry_count)) \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_WWAH_ROUTER_CHECK_IN_ENABLED_ID, &(wwah_attr.router_check_in_enabled)) \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_WWAH_TOUCHLINK_INTERPAN_ENABLED_ID, &(wwah_attr.touchlink_interpan_enabled)) \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_WWAH_WWAH_PARENT_CLASSIFICATION_ENABLED_ID, &(wwah_attr.wwah_parent_classification_enabled)) \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_WWAH_WWAH_APP_EVENT_RETRY_ENABLED_ID, &(wwah_attr.wwah_app_event_retry_enabled)) \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_WWAH_WWAH_APP_EVENT_RETRY_QUEUE_SIZE_ID, &(wwah_attr.wwah_app_event_retry_queue_size)) \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_WWAH_WWAH_REJOIN_ENABLED_ID, &(wwah_attr.wwah_rejoin_enabled)) \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_WWAH_MAC_POLL_FAILURE_WAIT_TIME_ID, &(wwah_attr.mac_poll_failure_wait_time)) \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_WWAH_CONFIGURATION_MODE_ENABLED_ID, &(wwah_attr.configuration_mode_enabled)) \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_WWAH_CURRENT_DEBUG_REPORT_ID_ID, &(wwah_attr.current_debug_report_id)) \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_WWAH_TC_SECURITY_ON_NWK_KEY_ROTATION_ENABLED_ID, &(wwah_attr.tc_security_on_nwk_key_rotation_enabled)) \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_WWAH_WWAH_BAD_PARENT_RECOVERY_ENABLED_ID, &(wwah_attr.wwah_bad_parent_recovery_enabled)) \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_WWAH_PENDING_NETWORK_UPDATE_CHANNEL_ID, &(wwah_attr.pending_network_update_channel)) \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_WWAH_PENDING_NETWORK_UPDATE_PANID_ID, &(wwah_attr.pending_network_update_panid)) \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_WWAH_OTA_MAX_OFFLINE_DURATION_ID, &(wwah_attr.ota_max_offline_duration)) \
  ZB_ZCL_FINISH_DECLARE_ATTRIB_LIST

  /* Declare Cluster Revision Attribute */
#define ZB_ZCL_DECLARE_WWAH_CLIENT_ATTRIB_LIST(attr_list) \
  ZB_ZCL_START_DECLARE_ATTRIB_LIST_CLUSTER_REVISION(attr_list, ZB_ZCL_WWAH)      \
  ZB_ZCL_FINISH_DECLARE_ATTRIB_LIST                \

/*! @internal Number of attributes mandatory for reporting in WWAH cluster */
#define ZB_ZCL_WWAH_REPORT_ATTR_COUNT 0

/***************************** commands *****************************/

/** @internal Structure for beaconSurvey data type
 */

/**
 *  @brief Determine a parent choose priority (WWAH-Requirements C-20)
 *
 *  End Devices examine all beacons in the Good Link Quality group
 *  (if received beacon with an RSSI above @see minRssiForReceivingPackets
 *  +30 dbm) and choose the parent with highest parent priority, as shown below.
 *  If no suitable parents exist in the Good Link Quality group, then the End
 *  Device shall examine all beacons in the Marginal Link Quality group.
 */
typedef enum zb_zcl_wwah_parent_priority_e
{
  /*! Invalid value for parent priority */
  ZB_ZCL_WWAH_PARENT_PRIORITY_INVALID   = 0,

  /*! 0b00 - no TC connectivity and Short Uptime or no WWAH parent */
  ZB_ZCL_WWAH_PARENT_PRIORITY_VERY_LOW  = 1,

  /*! 0b01 - no TC connectivity and Long Uptime */
  ZB_ZCL_WWAH_PARENT_PRIORITY_LOW       = 2,

  /*! 0b10 - TC connectivity and Short Uptime */
  ZB_ZCL_WWAH_PARENT_PRIORITY_HIGH      = 3,

  /*! 0b11 - TC connectivity and Long Uptime */
  ZB_ZCL_WWAH_PARENT_PRIORITY_VERY_HIGH = 4
}
zb_zcl_wwah_parent_priority_t;

typedef ZB_PACKED_PRE struct zb_zcl_wwah_beacon_survey_s
{
  /*! The DeviceShort field contains the 16 bit short address of the beaconing device. */
  zb_uint16_t device_short;
  /*! This is the RSSI of the beacon, expressed in dBm. */
  zb_int8_t rssi;
  /*! This is the parent classification mask. */
  zb_uint8_t classification_mask;
} ZB_PACKED_STRUCT zb_zcl_wwah_beacon_survey_t;

/*! @brief The PowerNotificationReason field is an enum containing all possible reasons for the power notification.
 */
enum zb_zcl_wwah_power_notification_reason_e
{
  /*! Unknown value */
  ZB_ZCL_WWAH_POWER_NOTIFICATION_REASON_UNKNOWN                 = 0x00,
  /*! Battery value */
  ZB_ZCL_WWAH_POWER_NOTIFICATION_REASON_BATTERY                 = 0x01,
  /*! Brownout value */
  ZB_ZCL_WWAH_POWER_NOTIFICATION_REASON_BROWNOUT                = 0x02,
  /*! Watchdog value */
  ZB_ZCL_WWAH_POWER_NOTIFICATION_REASON_WATCHDOG                = 0x03,
  /*! Reset Pin value */
  ZB_ZCL_WWAH_POWER_NOTIFICATION_REASON_RESET_PIN               = 0x04,
  /*! Memory/HW Fault value */
  ZB_ZCL_WWAH_POWER_NOTIFICATION_REASON_MEMORY_OR_HW_FAULT      = 0x05,
  /*! Software Exception value */
  ZB_ZCL_WWAH_POWER_NOTIFICATION_REASON_SOFTWARE_EXCEPTION      = 0x06,
  /*! OTA / Bootload Success value */
  ZB_ZCL_WWAH_POWER_NOTIFICATION_REASON_OTA_OR_BOOTLOAD_SUCCESS = 0x07,
  /*! Software Reset value */
  ZB_ZCL_WWAH_POWER_NOTIFICATION_REASON_SOFTWARE_RESET          = 0x08,
  /*! Power Button value */
  ZB_ZCL_WWAH_POWER_NOTIFICATION_REASON_POWER_BUTTON            = 0x09,
  /*! Temperature value */
  ZB_ZCL_WWAH_POWER_NOTIFICATION_REASON_TEMPERATURE             = 0x0A,
  /*! Bootload Failure value */
  ZB_ZCL_WWAH_POWER_NOTIFICATION_REASON_BOOTLOAD_FAILURE        = 0x0B,
};

/*! @brief The Enrollment Mode field is an enum containing the possible enrollment methods for an IAS Zone server.
 */


enum zb_zcl_wwah_enrollment_mode_e
{
  /*! Trip-to-pair value */
  ZB_ZCL_WWAH_ENROLLMENT_MODE_TRIP_TO_PAIR         = 0x00,
  /*! Auto-Enroll-Response value */
  ZB_ZCL_WWAH_ENROLLMENT_MODE_AUTO_ENROLL_RESPONSE = 0x01,
  /*! Auto-Enroll-Request value */
  ZB_ZCL_WWAH_ENROLLMENT_MODE_AUTO_ENROLL_REQUEST  = 0x02
};

/*! @}
    @endcond */ /* WWAH cluster internals */

/*! @} */ /* ZCL WWAH cluster definitions */

/*! @brief Add cluster_id to Enable APS Link Key Authorization command
    @param ptr - [in/out] (zb_uint8_t*) current position
    @param cluster_id - Cluster ID value
*/
#define ZB_ZCL_WWAH_SEND_ENABLE_APS_LINK_KEY_AUTHORIZATION_ADD(ptr, cluster_id)       \
{                                                                                     \
   ZB_ZCL_PACKET_PUT_DATA16(ptr, (cluster_id));                                       \
}

/*! @brief Start Enable APS Link Key Authorization command
    @param buffer - to put packet to
    @param def_resp - enable/disable default response
    @param number_of_clusters_to_exempt - Number of Clusters To Exempt value
    @param ptr - [out] (zb_uint8_t*) current position for ZB_ZCL_WWAH_SEND_ENABLE_APS_LINK_KEY_AUTHORIZATION_ADD
*/
#define ZB_ZCL_WWAH_SEND_ENABLE_APS_LINK_KEY_AUTHORIZATION_START(                     \
  buffer, def_resp, number_of_clusters_to_exempt, ptr)                                \
{                                                                                     \
  ptr = ZB_ZCL_START_PACKET(buffer);                                                  \
  ZB_ZCL_CONSTRUCT_SPECIFIC_COMMAND_REQ_FRAME_CONTROL_A(ptr,                          \
    ZB_ZCL_FRAME_DIRECTION_TO_SRV, ZB_ZCL_MANUFACTURER_SPECIFIC, def_resp);           \
  ZB_ZCL_CONSTRUCT_COMMAND_HEADER_EXT(ptr, ZB_ZCL_GET_SEQ_NUM(),                      \
    ZB_ZCL_MANUFACTURER_SPECIFIC, WWAH_MANUFACTURER_CODE,                             \
    ZB_ZCL_CMD_WWAH_ENABLE_APS_LINK_KEY_AUTHORIZATION_ID);                            \
  ZB_ZCL_PACKET_PUT_DATA8(ptr, (number_of_clusters_to_exempt));                       \
}

/*! @brief End form Enable APS Link Key Authorization command
    @param ptr - (zb_uint8_t*) current position
    @param buffer - to put packet to
    @param addr - address to send packet to
    @param dst_addr_mode - addressing mode
    @param dst_ep - destination endpoint
    @param ep - sending endpoint
    @param prfl_id - profile identifier
    @param cb - callback for getting command send status
*/
#define ZB_ZCL_WWAH_SEND_ENABLE_APS_LINK_KEY_AUTHORIZATION_END(                       \
  ptr, buffer, addr, dst_addr_mode, dst_ep, ep, prfl_id, cb)                          \
{                                                                                     \
  ZB_ZCL_FINISH_PACKET(buffer, ptr)                                                   \
  ZB_ZCL_SEND_COMMAND_SHORT(                                                          \
    buffer, addr, dst_addr_mode, dst_ep, ep, prfl_id, ZB_ZCL_CLUSTER_ID_WWAH, cb);    \
}

/*! @brief Start Disable APS Link Key Authorization command
    @param buffer - to put packet to
    @param def_resp - enable/disable default response
    @param number_of_clusters_to_exempt - Number of Clusters To Exempt value
    @param ptr - [out] (zb_uint8_t*) current position for ZB_ZCL_WWAH_SEND_DISABLE_APS_LINK_KEY_AUTHORIZATION_ADD
*/
#define ZB_ZCL_WWAH_SEND_DISABLE_APS_LINK_KEY_AUTHORIZATION_START(                    \
  buffer, def_resp, number_of_clusters_to_exempt, ptr)                                \
{                                                                                     \
  ptr = ZB_ZCL_START_PACKET(buffer);                                                  \
  ZB_ZCL_CONSTRUCT_SPECIFIC_COMMAND_REQ_FRAME_CONTROL_A(ptr,                          \
    ZB_ZCL_FRAME_DIRECTION_TO_SRV, ZB_ZCL_MANUFACTURER_SPECIFIC, def_resp);           \
  ZB_ZCL_CONSTRUCT_COMMAND_HEADER_EXT(ptr, ZB_ZCL_GET_SEQ_NUM(),                      \
    ZB_ZCL_MANUFACTURER_SPECIFIC, WWAH_MANUFACTURER_CODE,                             \
    ZB_ZCL_CMD_WWAH_DISABLE_APS_LINK_KEY_AUTHORIZATION_ID);                           \
  ZB_ZCL_PACKET_PUT_DATA8(ptr, (number_of_clusters_to_exempt));                       \
}

/*! @brief Add cluster_id to Disable APS Link Key Authorization command
    @param ptr - [in/out] (zb_uint8_t*) current position
    @param cluster_id - Cluster ID value
*/
#define ZB_ZCL_WWAH_SEND_DISABLE_APS_LINK_KEY_AUTHORIZATION_ADD(ptr, cluster_id)      \
{                                                                                     \
   ZB_ZCL_PACKET_PUT_DATA16(ptr, (cluster_id));                                       \
}

/*! @brief End form Disable APS Link Key Authorization command
    @param ptr - (zb_uint8_t*) current position
    @param buffer - to put packet to
    @param addr - address to send packet to
    @param dst_addr_mode - addressing mode
    @param dst_ep - destination endpoint
    @param ep - sending endpoint
    @param prfl_id - profile identifier
    @param cb - callback for getting command send status
*/
#define ZB_ZCL_WWAH_SEND_DISABLE_APS_LINK_KEY_AUTHORIZATION_END(                      \
  ptr, buffer, addr, dst_addr_mode, dst_ep, ep, prfl_id, cb)                          \
{                                                                                     \
  ZB_ZCL_FINISH_PACKET(buffer, ptr)                                                   \
  ZB_ZCL_SEND_COMMAND_SHORT(                                                          \
    buffer, addr, dst_addr_mode, dst_ep, ep, prfl_id, ZB_ZCL_CLUSTER_ID_WWAH, cb);    \
}

/*! @brief Send APS Link Key Authorization Query command
    @param buffer - to put packet to
    @param addr - address to send packet to
    @param dst_addr_mode - addressing mode
    @param dst_ep - destination endpoint
    @param ep - sending endpoint
    @param prfl_id - profile identifier
    @param def_resp - enable/disable default response
    @param cb - callback for getting command send status
    @param cluster_id - Cluster ID value*/
#define ZB_ZCL_WWAH_SEND_APS_LINK_KEY_AUTHORIZATION_QUERY(                            \
  buffer, addr, dst_addr_mode, dst_ep, ep, prfl_id, def_resp, cb, cluster_id)         \
{                                                                                     \
  zb_uint8_t* ptr = ZB_ZCL_START_PACKET(buffer);                                      \
  ZB_ZCL_CONSTRUCT_SPECIFIC_COMMAND_REQ_FRAME_CONTROL_A(ptr,                          \
    ZB_ZCL_FRAME_DIRECTION_TO_SRV, ZB_ZCL_MANUFACTURER_SPECIFIC, def_resp);           \
  ZB_ZCL_CONSTRUCT_COMMAND_HEADER_EXT(ptr, ZB_ZCL_GET_SEQ_NUM(),                      \
    ZB_ZCL_MANUFACTURER_SPECIFIC, WWAH_MANUFACTURER_CODE,                             \
    ZB_ZCL_CMD_WWAH_APS_LINK_KEY_AUTHORIZATION_QUERY_ID);                             \
  ZB_ZCL_PACKET_PUT_DATA16(ptr, (cluster_id));                                        \
  ZB_ZCL_FINISH_PACKET(buffer, ptr)                                                   \
  ZB_ZCL_SEND_COMMAND_SHORT(                                                          \
    buffer, addr, dst_addr_mode, dst_ep, ep, prfl_id, ZB_ZCL_CLUSTER_ID_WWAH, cb);    \
}

/*! @brief Send Request New APS Link Key command
    @param buffer - to put packet to
    @param addr - address to send packet to
    @param dst_addr_mode - addressing mode
    @param dst_ep - destination endpoint
    @param ep - sending endpoint
    @param prfl_id - profile identifier
    @param def_resp - enable/disable default response
    @param cb - callback for getting command send status
*/
#define ZB_ZCL_WWAH_SEND_REQUEST_NEW_APS_LINK_KEY(                                    \
  buffer, addr, dst_addr_mode, dst_ep, ep, prfl_id, def_resp, cb)                     \
{                                                                                     \
  zb_uint8_t* ptr = ZB_ZCL_START_PACKET(buffer);                                      \
  ZB_ZCL_CONSTRUCT_SPECIFIC_COMMAND_REQ_FRAME_CONTROL_A(ptr,                          \
    ZB_ZCL_FRAME_DIRECTION_TO_SRV, ZB_ZCL_MANUFACTURER_SPECIFIC, def_resp);           \
  ZB_ZCL_CONSTRUCT_COMMAND_HEADER_EXT(ptr, ZB_ZCL_GET_SEQ_NUM(),                      \
    ZB_ZCL_MANUFACTURER_SPECIFIC, WWAH_MANUFACTURER_CODE,                             \
    ZB_ZCL_CMD_WWAH_REQUEST_NEW_APS_LINK_KEY_ID);                                     \
  ZB_ZCL_FINISH_PACKET(buffer, ptr)                                                   \
  ZB_ZCL_SEND_COMMAND_SHORT(                                                          \
    buffer, addr, dst_addr_mode, dst_ep, ep, prfl_id, ZB_ZCL_CLUSTER_ID_WWAH, cb);    \
}

/*! @brief Send Enable WWAH App Event Retry Algorithm command
    @param buffer - to put packet to
    @param addr - address to send packet to
    @param dst_addr_mode - addressing mode
    @param dst_ep - destination endpoint
    @param ep - sending endpoint
    @param prfl_id - profile identifier
    @param def_resp - enable/disable default response
    @param cb - callback for getting command send status
    @param first_backoff_time_in_seconds - First Backoff Time In Seconds value
    @param backoff_sequence_common_ratio - Backoff Sequence Common Ratio value
    @param max_backoff_time_in_seconds - Max Backoff Time In Seconds value
    @param max_re_delivery_attempts - Max Re_Delivery Attempts value*/
#define ZB_ZCL_WWAH_SEND_ENABLE_WWAH_APP_EVENT_RETRY_ALGORITHM(                       \
  buffer, addr, dst_addr_mode, dst_ep, ep, prfl_id, def_resp, cb,                     \
  first_backoff_time_in_seconds, backoff_sequence_common_ratio,                       \
  max_backoff_time_in_seconds, max_re_delivery_attempts)                              \
{                                                                                     \
  zb_uint8_t* ptr = ZB_ZCL_START_PACKET(buffer);                                      \
  ZB_ZCL_CONSTRUCT_SPECIFIC_COMMAND_REQ_FRAME_CONTROL_A(ptr,                          \
    ZB_ZCL_FRAME_DIRECTION_TO_SRV, ZB_ZCL_MANUFACTURER_SPECIFIC, def_resp);           \
  ZB_ZCL_CONSTRUCT_COMMAND_HEADER_EXT(ptr, ZB_ZCL_GET_SEQ_NUM(),                      \
    ZB_ZCL_MANUFACTURER_SPECIFIC, WWAH_MANUFACTURER_CODE,                             \
    ZB_ZCL_CMD_WWAH_ENABLE_WWAH_APP_EVENT_RETRY_ALGORITHM_ID);                        \
  ZB_ZCL_PACKET_PUT_DATA8(ptr, (first_backoff_time_in_seconds));                      \
  ZB_ZCL_PACKET_PUT_DATA8(ptr, (backoff_sequence_common_ratio));                      \
  ZB_ZCL_PACKET_PUT_DATA32(ptr, (max_backoff_time_in_seconds));                       \
  ZB_ZCL_PACKET_PUT_DATA8(ptr, (max_re_delivery_attempts));                           \
  ZB_ZCL_FINISH_PACKET(buffer, ptr)                                                   \
  ZB_ZCL_SEND_COMMAND_SHORT(                                                          \
    buffer, addr, dst_addr_mode, dst_ep, ep, prfl_id, ZB_ZCL_CLUSTER_ID_WWAH, cb);    \
}

/*! @brief Send Disable WWAH App Event Retry Algorithm command
    @param buffer - to put packet to
    @param addr - address to send packet to
    @param dst_addr_mode - addressing mode
    @param dst_ep - destination endpoint
    @param ep - sending endpoint
    @param prfl_id - profile identifier
    @param def_resp - enable/disable default response
    @param cb - callback for getting command send status
*/
#define ZB_ZCL_WWAH_SEND_DISABLE_WWAH_APP_EVENT_RETRY_ALGORITHM(                      \
  buffer, addr, dst_addr_mode, dst_ep, ep, prfl_id, def_resp, cb)                     \
{                                                                                     \
  zb_uint8_t* ptr = ZB_ZCL_START_PACKET(buffer);                                      \
  ZB_ZCL_CONSTRUCT_SPECIFIC_COMMAND_REQ_FRAME_CONTROL_A(ptr,                          \
    ZB_ZCL_FRAME_DIRECTION_TO_SRV, ZB_ZCL_MANUFACTURER_SPECIFIC, def_resp);           \
  ZB_ZCL_CONSTRUCT_COMMAND_HEADER_EXT(ptr, ZB_ZCL_GET_SEQ_NUM(),                      \
    ZB_ZCL_MANUFACTURER_SPECIFIC, WWAH_MANUFACTURER_CODE,                             \
    ZB_ZCL_CMD_WWAH_DISABLE_WWAH_APP_EVENT_RETRY_ALGORITHM_ID);                       \
  ZB_ZCL_FINISH_PACKET(buffer, ptr)                                                   \
  ZB_ZCL_SEND_COMMAND_SHORT(                                                          \
    buffer, addr, dst_addr_mode, dst_ep, ep, prfl_id, ZB_ZCL_CLUSTER_ID_WWAH, cb);    \
}

/*! @brief Send Request Time command
    @param buffer - to put packet to
    @param addr - address to send packet to
    @param dst_addr_mode - addressing mode
    @param dst_ep - destination endpoint
    @param ep - sending endpoint
    @param prfl_id - profile identifier
    @param def_resp - enable/disable default response
    @param cb - callback for getting command send status
*/
#define ZB_ZCL_WWAH_SEND_REQUEST_TIME(                                                \
  buffer, addr, dst_addr_mode, dst_ep, ep, prfl_id, def_resp, cb)                     \
{                                                                                     \
  zb_uint8_t* ptr = ZB_ZCL_START_PACKET(buffer);                                      \
  ZB_ZCL_CONSTRUCT_SPECIFIC_COMMAND_REQ_FRAME_CONTROL_A(ptr,                          \
    ZB_ZCL_FRAME_DIRECTION_TO_SRV, ZB_ZCL_MANUFACTURER_SPECIFIC, def_resp);           \
  ZB_ZCL_CONSTRUCT_COMMAND_HEADER_EXT(ptr, ZB_ZCL_GET_SEQ_NUM(),                      \
    ZB_ZCL_MANUFACTURER_SPECIFIC, WWAH_MANUFACTURER_CODE,                             \
    ZB_ZCL_CMD_WWAH_REQUEST_TIME_ID);                                                 \
  ZB_ZCL_FINISH_PACKET(buffer, ptr)                                                   \
  ZB_ZCL_SEND_COMMAND_SHORT(                                                          \
    buffer, addr, dst_addr_mode, dst_ep, ep, prfl_id, ZB_ZCL_CLUSTER_ID_WWAH, cb);    \
}

/*! @brief Send Enable WWAH Rejoin Algorithm command
    @param buffer - to put packet to
    @param addr - address to send packet to
    @param dst_addr_mode - addressing mode
    @param dst_ep - destination endpoint
    @param ep - sending endpoint
    @param prfl_id - profile identifier
    @param def_resp - enable/disable default response
    @param cb - callback for getting command send status
    @param fast_rejoin_timeout_in_seconds - Fast Rejoin Timeout In Seconds value
    @param duration_between_each_rejoin_in_seconds - Duration Between Each Rejoin In Seconds value
    @param fast_rejoin_first_backoff_in_seconds - Fast Rejoin First Backoff In Seconds value
    @param max_backoff_time_in_seconds - Max Backoff Time In Seconds value
    @param max_backoff_iterations - max backoff iterations*/
#define ZB_ZCL_WWAH_SEND_ENABLE_WWAH_REJOIN_ALGORITHM(                                \
  buffer, addr, dst_addr_mode, dst_ep, ep, prfl_id, def_resp, cb,                     \
  fast_rejoin_timeout_in_seconds, duration_between_each_rejoin_in_seconds,            \
  fast_rejoin_first_backoff_in_seconds, max_backoff_time_in_seconds,                  \
  max_backoff_iterations)                                                             \
{                                                                                     \
  zb_uint8_t* ptr = ZB_ZCL_START_PACKET(buffer);                                      \
  ZB_ZCL_CONSTRUCT_SPECIFIC_COMMAND_REQ_FRAME_CONTROL_A(ptr,                          \
    ZB_ZCL_FRAME_DIRECTION_TO_SRV, ZB_ZCL_MANUFACTURER_SPECIFIC, def_resp);           \
  ZB_ZCL_CONSTRUCT_COMMAND_HEADER_EXT(ptr, ZB_ZCL_GET_SEQ_NUM(),                      \
    ZB_ZCL_MANUFACTURER_SPECIFIC, WWAH_MANUFACTURER_CODE,                             \
    ZB_ZCL_CMD_WWAH_ENABLE_WWAH_REJOIN_ALGORITHM_ID);                                 \
  ZB_ZCL_PACKET_PUT_DATA16(ptr, (fast_rejoin_timeout_in_seconds));                    \
  ZB_ZCL_PACKET_PUT_DATA16(ptr, (duration_between_each_rejoin_in_seconds));           \
  ZB_ZCL_PACKET_PUT_DATA16(ptr, (fast_rejoin_first_backoff_in_seconds));              \
  ZB_ZCL_PACKET_PUT_DATA16(ptr, (max_backoff_time_in_seconds));                       \
  ZB_ZCL_PACKET_PUT_DATA16(ptr, (max_backoff_iterations));                            \
  ZB_ZCL_FINISH_PACKET(buffer, ptr)                                                   \
  ZB_ZCL_SEND_COMMAND_SHORT(                                                          \
    buffer, addr, dst_addr_mode, dst_ep, ep, prfl_id, ZB_ZCL_CLUSTER_ID_WWAH, cb);    \
}

/*! @brief Send Disable WWAH Rejoin Algorithm command
    @param buffer - to put packet to
    @param addr - address to send packet to
    @param dst_addr_mode - addressing mode
    @param dst_ep - destination endpoint
    @param ep - sending endpoint
    @param prfl_id - profile identifier
    @param def_resp - enable/disable default response
    @param cb - callback for getting command send status
*/
#define ZB_ZCL_WWAH_SEND_DISABLE_WWAH_REJOIN_ALGORITHM(                               \
  buffer, addr, dst_addr_mode, dst_ep, ep, prfl_id, def_resp, cb)                     \
{                                                                                     \
  zb_uint8_t* ptr = ZB_ZCL_START_PACKET(buffer);                                      \
  ZB_ZCL_CONSTRUCT_SPECIFIC_COMMAND_REQ_FRAME_CONTROL_A(ptr,                          \
    ZB_ZCL_FRAME_DIRECTION_TO_SRV, ZB_ZCL_MANUFACTURER_SPECIFIC, def_resp);           \
  ZB_ZCL_CONSTRUCT_COMMAND_HEADER_EXT(ptr, ZB_ZCL_GET_SEQ_NUM(),                      \
    ZB_ZCL_MANUFACTURER_SPECIFIC, WWAH_MANUFACTURER_CODE,                             \
    ZB_ZCL_CMD_WWAH_DISABLE_WWAH_REJOIN_ALGORITHM_ID);                                \
  ZB_ZCL_FINISH_PACKET(buffer, ptr)                                                   \
  ZB_ZCL_SEND_COMMAND_SHORT(                                                          \
    buffer, addr, dst_addr_mode, dst_ep, ep, prfl_id, ZB_ZCL_CLUSTER_ID_WWAH, cb);    \
}

/*! @brief Send Set IAS Zone Enrollment Method command
    @param buffer - to put packet to
    @param addr - address to send packet to
    @param dst_addr_mode - addressing mode
    @param dst_ep - destination endpoint
    @param ep - sending endpoint
    @param prfl_id - profile identifier
    @param def_resp - enable/disable default response
    @param cb - callback for getting command send status
    @param enrollment_mode - Enrollment Mode value*/
#define ZB_ZCL_WWAH_SEND_SET_IAS_ZONE_ENROLLMENT_METHOD(                              \
  buffer, addr, dst_addr_mode, dst_ep, ep, prfl_id, def_resp, cb, enrollment_mode)    \
{                                                                                     \
  zb_uint8_t* ptr = ZB_ZCL_START_PACKET(buffer);                                      \
  ZB_ZCL_CONSTRUCT_SPECIFIC_COMMAND_REQ_FRAME_CONTROL_A(ptr,                          \
    ZB_ZCL_FRAME_DIRECTION_TO_SRV, ZB_ZCL_MANUFACTURER_SPECIFIC, def_resp);           \
  ZB_ZCL_CONSTRUCT_COMMAND_HEADER_EXT(ptr, ZB_ZCL_GET_SEQ_NUM(),                      \
    ZB_ZCL_MANUFACTURER_SPECIFIC, WWAH_MANUFACTURER_CODE,                             \
    ZB_ZCL_CMD_WWAH_SET_IAS_ZONE_ENROLLMENT_METHOD_ID);                               \
  ZB_ZCL_PACKET_PUT_DATA8(ptr, (enrollment_mode));                                    \
  ZB_ZCL_FINISH_PACKET(buffer, ptr)                                                   \
  ZB_ZCL_SEND_COMMAND_SHORT(                                                          \
    buffer, addr, dst_addr_mode, dst_ep, ep, prfl_id, ZB_ZCL_CLUSTER_ID_WWAH, cb);    \
}

/*! @brief Send Clear Binding Table command
    @param buffer - to put packet to
    @param addr - address to send packet to
    @param dst_addr_mode - addressing mode
    @param dst_ep - destination endpoint
    @param ep - sending endpoint
    @param prfl_id - profile identifier
    @param def_resp - enable/disable default response
    @param cb - callback for getting command send status
*/
#define ZB_ZCL_WWAH_SEND_CLEAR_BINDING_TABLE(                                         \
  buffer, addr, dst_addr_mode, dst_ep, ep, prfl_id, def_resp, cb)                     \
{                                                                                     \
  zb_uint8_t* ptr = ZB_ZCL_START_PACKET(buffer);                                      \
  ZB_ZCL_CONSTRUCT_SPECIFIC_COMMAND_REQ_FRAME_CONTROL_A(ptr,                          \
    ZB_ZCL_FRAME_DIRECTION_TO_SRV, ZB_ZCL_MANUFACTURER_SPECIFIC, def_resp);           \
  ZB_ZCL_CONSTRUCT_COMMAND_HEADER_EXT(ptr, ZB_ZCL_GET_SEQ_NUM(),                      \
    ZB_ZCL_MANUFACTURER_SPECIFIC, WWAH_MANUFACTURER_CODE,                             \
    ZB_ZCL_CMD_WWAH_CLEAR_BINDING_TABLE_ID);                                          \
  ZB_ZCL_FINISH_PACKET(buffer, ptr)                                                   \
  ZB_ZCL_SEND_COMMAND_SHORT(                                                          \
    buffer, addr, dst_addr_mode, dst_ep, ep, prfl_id, ZB_ZCL_CLUSTER_ID_WWAH, cb);    \
}

/*! @brief Send Enable Periodic Router Check Ins command
    @param buffer - to put packet to
    @param addr - address to send packet to
    @param dst_addr_mode - addressing mode
    @param dst_ep - destination endpoint
    @param ep - sending endpoint
    @param prfl_id - profile identifier
    @param def_resp - enable/disable default response
    @param cb - callback for getting command send status
    @param check_in_interval - Check_In Interval value*/
#define ZB_ZCL_WWAH_SEND_ENABLE_PERIODIC_ROUTER_CHECK_INS(                            \
  buffer, addr, dst_addr_mode, dst_ep, ep, prfl_id, def_resp, cb, check_in_interval)  \
{                                                                                     \
  zb_uint8_t* ptr = ZB_ZCL_START_PACKET(buffer);                                      \
  ZB_ZCL_CONSTRUCT_SPECIFIC_COMMAND_REQ_FRAME_CONTROL_A(ptr,                          \
    ZB_ZCL_FRAME_DIRECTION_TO_SRV, ZB_ZCL_MANUFACTURER_SPECIFIC, def_resp);           \
  ZB_ZCL_CONSTRUCT_COMMAND_HEADER_EXT(ptr, ZB_ZCL_GET_SEQ_NUM(),                      \
    ZB_ZCL_MANUFACTURER_SPECIFIC, WWAH_MANUFACTURER_CODE,                             \
    ZB_ZCL_CMD_WWAH_ENABLE_PERIODIC_ROUTER_CHECK_INS_ID);                             \
  ZB_ZCL_PACKET_PUT_DATA16(ptr, (check_in_interval));                                 \
  ZB_ZCL_FINISH_PACKET(buffer, ptr)                                                   \
  ZB_ZCL_SEND_COMMAND_SHORT(                                                          \
    buffer, addr, dst_addr_mode, dst_ep, ep, prfl_id, ZB_ZCL_CLUSTER_ID_WWAH, cb);    \
}

/*! @brief Send Disable Periodic Router Check Ins command
    @param buffer - to put packet to
    @param addr - address to send packet to
    @param dst_addr_mode - addressing mode
    @param dst_ep - destination endpoint
    @param ep - sending endpoint
    @param prfl_id - profile identifier
    @param def_resp - enable/disable default response
    @param cb - callback for getting command send status
*/
#define ZB_ZCL_WWAH_SEND_DISABLE_PERIODIC_ROUTER_CHECK_INS(                           \
  buffer, addr, dst_addr_mode, dst_ep, ep, prfl_id, def_resp, cb)                     \
{                                                                                     \
  zb_uint8_t* ptr = ZB_ZCL_START_PACKET(buffer);                                      \
  ZB_ZCL_CONSTRUCT_SPECIFIC_COMMAND_REQ_FRAME_CONTROL_A(ptr,                          \
    ZB_ZCL_FRAME_DIRECTION_TO_SRV, ZB_ZCL_MANUFACTURER_SPECIFIC, def_resp);           \
  ZB_ZCL_CONSTRUCT_COMMAND_HEADER_EXT(ptr, ZB_ZCL_GET_SEQ_NUM(),                      \
    ZB_ZCL_MANUFACTURER_SPECIFIC, WWAH_MANUFACTURER_CODE,                             \
    ZB_ZCL_CMD_WWAH_DISABLE_PERIODIC_ROUTER_CHECK_INS_ID);                            \
  ZB_ZCL_FINISH_PACKET(buffer, ptr)                                                   \
  ZB_ZCL_SEND_COMMAND_SHORT(                                                          \
    buffer, addr, dst_addr_mode, dst_ep, ep, prfl_id, ZB_ZCL_CLUSTER_ID_WWAH, cb);    \
}

/*! @brief Send Set MAC Poll CCA Wait Time command
    @param buffer - to put packet to
    @param addr - address to send packet to
    @param dst_addr_mode - addressing mode
    @param dst_ep - destination endpoint
    @param ep - sending endpoint
    @param prfl_id - profile identifier
    @param def_resp - enable/disable default response
    @param cb - callback for getting command send status
    @param wait_time - Wait Time value*/
#define ZB_ZCL_WWAH_SEND_SET_MAC_POLL_CCA_WAIT_TIME(                                  \
  buffer, addr, dst_addr_mode, dst_ep, ep, prfl_id, def_resp, cb, wait_time)          \
{                                                                                     \
  zb_uint8_t* ptr = ZB_ZCL_START_PACKET(buffer);                                      \
  ZB_ZCL_CONSTRUCT_SPECIFIC_COMMAND_REQ_FRAME_CONTROL_A(ptr,                          \
    ZB_ZCL_FRAME_DIRECTION_TO_SRV, ZB_ZCL_MANUFACTURER_SPECIFIC, def_resp);           \
  ZB_ZCL_CONSTRUCT_COMMAND_HEADER_EXT(ptr, ZB_ZCL_GET_SEQ_NUM(),                      \
    ZB_ZCL_MANUFACTURER_SPECIFIC, WWAH_MANUFACTURER_CODE,                             \
    ZB_ZCL_CMD_WWAH_SET_MAC_POLL_CCA_WAIT_TIME_ID);                                   \
  ZB_ZCL_PACKET_PUT_DATA8(ptr, (wait_time));                                          \
  ZB_ZCL_FINISH_PACKET(buffer, ptr)                                                   \
  ZB_ZCL_SEND_COMMAND_SHORT(                                                          \
    buffer, addr, dst_addr_mode, dst_ep, ep, prfl_id, ZB_ZCL_CLUSTER_ID_WWAH, cb);    \
}

/*! @brief Send Set Pending Network Update command
    @param buffer - to put packet to
    @param addr - address to send packet to
    @param dst_addr_mode - addressing mode
    @param dst_ep - destination endpoint
    @param ep - sending endpoint
    @param prfl_id - profile identifier
    @param def_resp - enable/disable default response
    @param cb - callback for getting command send status
    @param channel - Channel value
    @param pan_id - PAN ID value*/
#define ZB_ZCL_WWAH_SEND_SET_PENDING_NETWORK_UPDATE(                                  \
  buffer, addr, dst_addr_mode, dst_ep, ep, prfl_id, def_resp, cb, channel, pan_id)    \
{                                                                                     \
  zb_uint8_t* ptr = ZB_ZCL_START_PACKET(buffer);                                      \
  ZB_ZCL_CONSTRUCT_SPECIFIC_COMMAND_REQ_FRAME_CONTROL_A(ptr,                          \
    ZB_ZCL_FRAME_DIRECTION_TO_SRV, ZB_ZCL_MANUFACTURER_SPECIFIC, def_resp);           \
  ZB_ZCL_CONSTRUCT_COMMAND_HEADER_EXT(ptr, ZB_ZCL_GET_SEQ_NUM(),                      \
    ZB_ZCL_MANUFACTURER_SPECIFIC, WWAH_MANUFACTURER_CODE,                             \
    ZB_ZCL_CMD_WWAH_SET_PENDING_NETWORK_UPDATE_ID);                                   \
  ZB_ZCL_PACKET_PUT_DATA8(ptr, (channel));                                            \
  ZB_ZCL_PACKET_PUT_DATA16(ptr, (pan_id));                                            \
  ZB_ZCL_FINISH_PACKET(buffer, ptr)                                                   \
  ZB_ZCL_SEND_COMMAND_SHORT(                                                          \
    buffer, addr, dst_addr_mode, dst_ep, ep, prfl_id, ZB_ZCL_CLUSTER_ID_WWAH, cb);    \
}

/*! @brief Add cluster_id to Require APS ACKs on Unicasts command
    @param ptr - [in/out] (zb_uint8_t*) current position
    @param cluster_id - Cluster ID value
*/
#define ZB_ZCL_WWAH_SEND_REQUIRE_APS_ACKS_ON_UNICASTS_ADD(ptr, cluster_id)            \
{                                                                                     \
   ZB_ZCL_PACKET_PUT_DATA16(ptr, (cluster_id));                                       \
}

/*! @brief Start Require APS ACKs on Unicasts command
    @param buffer - to put packet to
    @param def_resp - enable/disable default response
    @param number_of_clusters_to_exempt - Number of Clusters to Exempt value
    @param ptr - [out] (zb_uint8_t*) current position for ZB_ZCL_WWAH_SEND_REQUIRE_APS_ACKS_ON_UNICASTS_ADD
*/
#define ZB_ZCL_WWAH_SEND_REQUIRE_APS_ACKS_ON_UNICASTS_START(                          \
  buffer, def_resp, number_of_clusters_to_exempt, ptr)                                \
{                                                                                     \
  ptr = ZB_ZCL_START_PACKET(buffer);                                                  \
  ZB_ZCL_CONSTRUCT_SPECIFIC_COMMAND_REQ_FRAME_CONTROL_A(ptr,                          \
    ZB_ZCL_FRAME_DIRECTION_TO_SRV, ZB_ZCL_MANUFACTURER_SPECIFIC, def_resp);           \
  ZB_ZCL_CONSTRUCT_COMMAND_HEADER_EXT(ptr, ZB_ZCL_GET_SEQ_NUM(),                      \
    ZB_ZCL_MANUFACTURER_SPECIFIC, WWAH_MANUFACTURER_CODE,                             \
    ZB_ZCL_CMD_WWAH_REQUIRE_APS_ACKS_ON_UNICASTS_ID);                                 \
  ZB_ZCL_PACKET_PUT_DATA8(ptr, (number_of_clusters_to_exempt));                       \
}

/*! @brief End form Require APS ACKs on Unicasts command
    @param ptr - (zb_uint8_t*) current position
    @param buffer - to put packet to
    @param addr - address to send packet to
    @param dst_addr_mode - addressing mode
    @param dst_ep - destination endpoint
    @param ep - sending endpoint
    @param prfl_id - profile identifier
    @param cb - callback for getting command send status
*/
#define ZB_ZCL_WWAH_SEND_REQUIRE_APS_ACKS_ON_UNICASTS_END(                            \
  ptr, buffer, addr, dst_addr_mode, dst_ep, ep, prfl_id, cb)                          \
{                                                                                     \
  ZB_ZCL_FINISH_PACKET(buffer, ptr)                                                   \
  ZB_ZCL_SEND_COMMAND_SHORT(                                                          \
    buffer, addr, dst_addr_mode, dst_ep, ep, prfl_id, ZB_ZCL_CLUSTER_ID_WWAH, cb);    \
}

/*! @brief Send Remove APS ACKs on Unicasts Requirement command
    @param buffer - to put packet to
    @param addr - address to send packet to
    @param dst_addr_mode - addressing mode
    @param dst_ep - destination endpoint
    @param ep - sending endpoint
    @param prfl_id - profile identifier
    @param def_resp - enable/disable default response
    @param cb - callback for getting command send status
*/
#define ZB_ZCL_WWAH_SEND_REMOVE_APS_ACKS_ON_UNICASTS_REQUIREMENT(                     \
  buffer, addr, dst_addr_mode, dst_ep, ep, prfl_id, def_resp, cb)                     \
{                                                                                     \
  zb_uint8_t* ptr = ZB_ZCL_START_PACKET(buffer);                                      \
  ZB_ZCL_CONSTRUCT_SPECIFIC_COMMAND_REQ_FRAME_CONTROL_A(ptr,                          \
    ZB_ZCL_FRAME_DIRECTION_TO_SRV, ZB_ZCL_MANUFACTURER_SPECIFIC, def_resp);           \
  ZB_ZCL_CONSTRUCT_COMMAND_HEADER_EXT(ptr, ZB_ZCL_GET_SEQ_NUM(),                      \
    ZB_ZCL_MANUFACTURER_SPECIFIC, WWAH_MANUFACTURER_CODE,                             \
    ZB_ZCL_CMD_WWAH_REMOVE_APS_ACKS_ON_UNICASTS_REQUIREMENT_ID);                      \
  ZB_ZCL_FINISH_PACKET(buffer, ptr)                                                   \
  ZB_ZCL_SEND_COMMAND_SHORT(                                                          \
    buffer, addr, dst_addr_mode, dst_ep, ep, prfl_id, ZB_ZCL_CLUSTER_ID_WWAH, cb);    \
}

/*! @brief Send APS ACK Requirement Query command
    @param buffer - to put packet to
    @param addr - address to send packet to
    @param dst_addr_mode - addressing mode
    @param dst_ep - destination endpoint
    @param ep - sending endpoint
    @param prfl_id - profile identifier
    @param def_resp - enable/disable default response
    @param cb - callback for getting command send status
*/
#define ZB_ZCL_WWAH_SEND_APS_ACK_REQUIREMENT_QUERY(                                   \
  buffer, addr, dst_addr_mode, dst_ep, ep, prfl_id, def_resp, cb)                     \
{                                                                                     \
  zb_uint8_t* ptr = ZB_ZCL_START_PACKET(buffer);                                      \
  ZB_ZCL_CONSTRUCT_SPECIFIC_COMMAND_REQ_FRAME_CONTROL_A(ptr,                          \
    ZB_ZCL_FRAME_DIRECTION_TO_SRV, ZB_ZCL_MANUFACTURER_SPECIFIC, def_resp);           \
  ZB_ZCL_CONSTRUCT_COMMAND_HEADER_EXT(ptr, ZB_ZCL_GET_SEQ_NUM(),                      \
    ZB_ZCL_MANUFACTURER_SPECIFIC, WWAH_MANUFACTURER_CODE,                             \
    ZB_ZCL_CMD_WWAH_APS_ACK_REQUIREMENT_QUERY_ID);                                    \
  ZB_ZCL_FINISH_PACKET(buffer, ptr)                                                   \
  ZB_ZCL_SEND_COMMAND_SHORT(                                                          \
    buffer, addr, dst_addr_mode, dst_ep, ep, prfl_id, ZB_ZCL_CLUSTER_ID_WWAH, cb);    \
}

/*! @brief Send Debug Report Query command
    @param buffer - to put packet to
    @param addr - address to send packet to
    @param dst_addr_mode - addressing mode
    @param dst_ep - destination endpoint
    @param ep - sending endpoint
    @param prfl_id - profile identifier
    @param def_resp - enable/disable default response
    @param cb - callback for getting command send status
    @param debug_report_id - Debug Report ID value*/
#define ZB_ZCL_WWAH_SEND_DEBUG_REPORT_QUERY(                                          \
  buffer, addr, dst_addr_mode, dst_ep, ep, prfl_id, def_resp, cb, debug_report_id)    \
{                                                                                     \
  zb_uint8_t* ptr = ZB_ZCL_START_PACKET(buffer);                                      \
  ZB_ZCL_CONSTRUCT_SPECIFIC_COMMAND_REQ_FRAME_CONTROL_A(ptr,                          \
    ZB_ZCL_FRAME_DIRECTION_TO_SRV, ZB_ZCL_MANUFACTURER_SPECIFIC, def_resp);           \
  ZB_ZCL_CONSTRUCT_COMMAND_HEADER_EXT(ptr, ZB_ZCL_GET_SEQ_NUM(),                      \
    ZB_ZCL_MANUFACTURER_SPECIFIC, WWAH_MANUFACTURER_CODE,                             \
    ZB_ZCL_CMD_WWAH_DEBUG_REPORT_QUERY_ID);                                           \
  ZB_ZCL_PACKET_PUT_DATA8(ptr, (debug_report_id));                                    \
  ZB_ZCL_FINISH_PACKET(buffer, ptr)                                                   \
  ZB_ZCL_SEND_COMMAND_SHORT(                                                          \
    buffer, addr, dst_addr_mode, dst_ep, ep, prfl_id, ZB_ZCL_CLUSTER_ID_WWAH, cb);    \
}

/*! @brief Send Survey Beacons command
    @param buffer - to put packet to
    @param addr - address to send packet to
    @param dst_addr_mode - addressing mode
    @param dst_ep - destination endpoint
    @param ep - sending endpoint
    @param prfl_id - profile identifier
    @param def_resp - enable/disable default response
    @param cb - callback for getting command send status
    @param standard_beacons - Standard Beacons value*/
#define ZB_ZCL_WWAH_SEND_SURVEY_BEACONS(                                              \
  buffer, addr, dst_addr_mode, dst_ep, ep, prfl_id, def_resp, cb, standard_beacons)   \
{                                                                                     \
  zb_uint8_t* ptr = ZB_ZCL_START_PACKET(buffer);                                      \
  ZB_ZCL_CONSTRUCT_SPECIFIC_COMMAND_REQ_FRAME_CONTROL_A(ptr,                          \
    ZB_ZCL_FRAME_DIRECTION_TO_SRV, ZB_ZCL_MANUFACTURER_SPECIFIC, def_resp);           \
  ZB_ZCL_CONSTRUCT_COMMAND_HEADER_EXT(ptr, ZB_ZCL_GET_SEQ_NUM(),                      \
    ZB_ZCL_MANUFACTURER_SPECIFIC, WWAH_MANUFACTURER_CODE,                             \
    ZB_ZCL_CMD_WWAH_SURVEY_BEACONS_ID);                                               \
  ZB_ZCL_PACKET_PUT_DATA8(ptr, (standard_beacons));                                   \
  ZB_ZCL_FINISH_PACKET(buffer, ptr)                                                   \
  ZB_ZCL_SEND_COMMAND_SHORT(                                                          \
    buffer, addr, dst_addr_mode, dst_ep, ep, prfl_id, ZB_ZCL_CLUSTER_ID_WWAH, cb);    \
}

/*! @brief Send Disable OTA Downgrades command
    @param buffer - to put packet to
    @param addr - address to send packet to
    @param dst_addr_mode - addressing mode
    @param dst_ep - destination endpoint
    @param ep - sending endpoint
    @param prfl_id - profile identifier
    @param def_resp - enable/disable default response
    @param cb - callback for getting command send status
*/
#define ZB_ZCL_WWAH_SEND_DISABLE_OTA_DOWNGRADES(                                      \
  buffer, addr, dst_addr_mode, dst_ep, ep, prfl_id, def_resp, cb)                     \
{                                                                                     \
  zb_uint8_t* ptr = ZB_ZCL_START_PACKET(buffer);                                      \
  ZB_ZCL_CONSTRUCT_SPECIFIC_COMMAND_REQ_FRAME_CONTROL_A(ptr,                          \
    ZB_ZCL_FRAME_DIRECTION_TO_SRV, ZB_ZCL_MANUFACTURER_SPECIFIC, def_resp);           \
  ZB_ZCL_CONSTRUCT_COMMAND_HEADER_EXT(ptr, ZB_ZCL_GET_SEQ_NUM(),                      \
    ZB_ZCL_MANUFACTURER_SPECIFIC, WWAH_MANUFACTURER_CODE,                             \
    ZB_ZCL_CMD_WWAH_DISABLE_OTA_DOWNGRADES_ID);                                       \
  ZB_ZCL_FINISH_PACKET(buffer, ptr)                                                   \
  ZB_ZCL_SEND_COMMAND_SHORT(                                                          \
    buffer, addr, dst_addr_mode, dst_ep, ep, prfl_id, ZB_ZCL_CLUSTER_ID_WWAH, cb);    \
}

/*! @brief Send Disable MGMT Leave Without Rejoin command
    @param buffer - to put packet to
    @param addr - address to send packet to
    @param dst_addr_mode - addressing mode
    @param dst_ep - destination endpoint
    @param ep - sending endpoint
    @param prfl_id - profile identifier
    @param def_resp - enable/disable default response
    @param cb - callback for getting command send status
*/
#define ZB_ZCL_WWAH_SEND_DISABLE_MGMT_LEAVE_WITHOUT_REJOIN(                           \
  buffer, addr, dst_addr_mode, dst_ep, ep, prfl_id, def_resp, cb)                     \
{                                                                                     \
  zb_uint8_t* ptr = ZB_ZCL_START_PACKET(buffer);                                      \
  ZB_ZCL_CONSTRUCT_SPECIFIC_COMMAND_REQ_FRAME_CONTROL_A(ptr,                          \
    ZB_ZCL_FRAME_DIRECTION_TO_SRV, ZB_ZCL_MANUFACTURER_SPECIFIC, def_resp);           \
  ZB_ZCL_CONSTRUCT_COMMAND_HEADER_EXT(ptr, ZB_ZCL_GET_SEQ_NUM(),                      \
    ZB_ZCL_MANUFACTURER_SPECIFIC, WWAH_MANUFACTURER_CODE,                             \
    ZB_ZCL_CMD_WWAH_DISABLE_MGMT_LEAVE_WITHOUT_REJOIN_ID);                            \
  ZB_ZCL_FINISH_PACKET(buffer, ptr)                                                   \
  ZB_ZCL_SEND_COMMAND_SHORT(                                                          \
    buffer, addr, dst_addr_mode, dst_ep, ep, prfl_id, ZB_ZCL_CLUSTER_ID_WWAH, cb);    \
}

/*! @brief Send Disable Touchlink Interpan Message Support command
    @param buffer - to put packet to
    @param addr - address to send packet to
    @param dst_addr_mode - addressing mode
    @param dst_ep - destination endpoint
    @param ep - sending endpoint
    @param prfl_id - profile identifier
    @param def_resp - enable/disable default response
    @param cb - callback for getting command send status
*/
#define ZB_ZCL_WWAH_SEND_DISABLE_TOUCHLINK_INTERPAN_MESSAGE_SUPPORT(                  \
  buffer, addr, dst_addr_mode, dst_ep, ep, prfl_id, def_resp, cb)                     \
{                                                                                     \
  zb_uint8_t* ptr = ZB_ZCL_START_PACKET(buffer);                                      \
  ZB_ZCL_CONSTRUCT_SPECIFIC_COMMAND_REQ_FRAME_CONTROL_A(ptr,                          \
    ZB_ZCL_FRAME_DIRECTION_TO_SRV, ZB_ZCL_MANUFACTURER_SPECIFIC, def_resp);           \
  ZB_ZCL_CONSTRUCT_COMMAND_HEADER_EXT(ptr, ZB_ZCL_GET_SEQ_NUM(),                      \
    ZB_ZCL_MANUFACTURER_SPECIFIC, WWAH_MANUFACTURER_CODE,                             \
    ZB_ZCL_CMD_WWAH_DISABLE_TOUCHLINK_INTERPAN_MESSAGE_SUPPORT_ID);                   \
  ZB_ZCL_FINISH_PACKET(buffer, ptr)                                                   \
  ZB_ZCL_SEND_COMMAND_SHORT(                                                          \
    buffer, addr, dst_addr_mode, dst_ep, ep, prfl_id, ZB_ZCL_CLUSTER_ID_WWAH, cb);    \
}

/*! @brief Send Enable WWAH Parent Classification command
    @param buffer - to put packet to
    @param addr - address to send packet to
    @param dst_addr_mode - addressing mode
    @param dst_ep - destination endpoint
    @param ep - sending endpoint
    @param prfl_id - profile identifier
    @param def_resp - enable/disable default response
    @param cb - callback for getting command send status
*/
#define ZB_ZCL_WWAH_SEND_ENABLE_WWAH_PARENT_CLASSIFICATION(                           \
  buffer, addr, dst_addr_mode, dst_ep, ep, prfl_id, def_resp, cb)                     \
{                                                                                     \
  zb_uint8_t* ptr = ZB_ZCL_START_PACKET(buffer);                                      \
  ZB_ZCL_CONSTRUCT_SPECIFIC_COMMAND_REQ_FRAME_CONTROL_A(ptr,                          \
    ZB_ZCL_FRAME_DIRECTION_TO_SRV, ZB_ZCL_MANUFACTURER_SPECIFIC, def_resp);           \
  ZB_ZCL_CONSTRUCT_COMMAND_HEADER_EXT(ptr, ZB_ZCL_GET_SEQ_NUM(),                      \
    ZB_ZCL_MANUFACTURER_SPECIFIC, WWAH_MANUFACTURER_CODE,                             \
    ZB_ZCL_CMD_WWAH_ENABLE_WWAH_PARENT_CLASSIFICATION_ID);                            \
  ZB_ZCL_FINISH_PACKET(buffer, ptr)                                                   \
  ZB_ZCL_SEND_COMMAND_SHORT(                                                          \
    buffer, addr, dst_addr_mode, dst_ep, ep, prfl_id, ZB_ZCL_CLUSTER_ID_WWAH, cb);    \
}
/*! @brief Send Disable WWAH Parent Classification command
    @param buffer - to put packet to
    @param addr - address to send packet to
    @param dst_addr_mode - addressing mode
    @param dst_ep - destination endpoint
    @param ep - sending endpoint
    @param prfl_id - profile identifier
    @param def_resp - enable/disable default response
    @param cb - callback for getting command send status
*/
#define ZB_ZCL_WWAH_SEND_DISABLE_WWAH_PARENT_CLASSIFICATION(                          \
  buffer, addr, dst_addr_mode, dst_ep, ep, prfl_id, def_resp, cb)                     \
{                                                                                     \
  zb_uint8_t* ptr = ZB_ZCL_START_PACKET(buffer);                                      \
  ZB_ZCL_CONSTRUCT_SPECIFIC_COMMAND_REQ_FRAME_CONTROL_A(ptr,                          \
    ZB_ZCL_FRAME_DIRECTION_TO_SRV, ZB_ZCL_MANUFACTURER_SPECIFIC, def_resp);           \
  ZB_ZCL_CONSTRUCT_COMMAND_HEADER_EXT(ptr, ZB_ZCL_GET_SEQ_NUM(),                      \
    ZB_ZCL_MANUFACTURER_SPECIFIC, WWAH_MANUFACTURER_CODE,                             \
    ZB_ZCL_CMD_WWAH_DISABLE_WWAH_PARENT_CLASSIFICATION_ID);                           \
  ZB_ZCL_FINISH_PACKET(buffer, ptr)                                                   \
  ZB_ZCL_SEND_COMMAND_SHORT(                                                          \
    buffer, addr, dst_addr_mode, dst_ep, ep, prfl_id, ZB_ZCL_CLUSTER_ID_WWAH, cb);    \
}

/*! @brief Send Enable TC Security On NWK Key Rotation command
    @param buffer - to put packet to
    @param addr - address to send packet to
    @param dst_addr_mode - addressing mode
    @param dst_ep - destination endpoint
    @param ep - sending endpoint
    @param prfl_id - profile identifier
    @param def_resp - enable/disable default response
    @param cb - callback for getting command send status
*/
#define ZB_ZCL_WWAH_SEND_ENABLE_TC_SECURITY_ON_NWK_KEY_ROTATION(                      \
  buffer, addr, dst_addr_mode, dst_ep, ep, prfl_id, def_resp, cb)                     \
{                                                                                     \
  zb_uint8_t* ptr = ZB_ZCL_START_PACKET(buffer);                                      \
  ZB_ZCL_CONSTRUCT_SPECIFIC_COMMAND_REQ_FRAME_CONTROL_A(ptr,                          \
    ZB_ZCL_FRAME_DIRECTION_TO_SRV, ZB_ZCL_MANUFACTURER_SPECIFIC, def_resp);           \
  ZB_ZCL_CONSTRUCT_COMMAND_HEADER_EXT(ptr, ZB_ZCL_GET_SEQ_NUM(),                      \
    ZB_ZCL_MANUFACTURER_SPECIFIC, WWAH_MANUFACTURER_CODE,                             \
    ZB_ZCL_CMD_WWAH_ENABLE_TC_SECURITY_ON_NWK_KEY_ROTATION_ID);                       \
  ZB_ZCL_FINISH_PACKET(buffer, ptr)                                                   \
  ZB_ZCL_SEND_COMMAND_SHORT(                                                          \
    buffer, addr, dst_addr_mode, dst_ep, ep, prfl_id, ZB_ZCL_CLUSTER_ID_WWAH, cb);    \
}
/*! @brief Send Enable WWAH Bad Parent Recovery command
    @param buffer - to put packet to
    @param addr - address to send packet to
    @param dst_addr_mode - addressing mode
    @param dst_ep - destination endpoint
    @param ep - sending endpoint
    @param prfl_id - profile identifier
    @param def_resp - enable/disable default response
    @param cb - callback for getting command send status
*/
#define ZB_ZCL_WWAH_SEND_ENABLE_WWAH_BAD_PARENT_RECOVERY(                             \
  buffer, addr, dst_addr_mode, dst_ep, ep, prfl_id, def_resp, cb)                     \
{                                                                                     \
  zb_uint8_t* ptr = ZB_ZCL_START_PACKET(buffer);                                      \
  ZB_ZCL_CONSTRUCT_SPECIFIC_COMMAND_REQ_FRAME_CONTROL_A(ptr,                          \
    ZB_ZCL_FRAME_DIRECTION_TO_SRV, ZB_ZCL_MANUFACTURER_SPECIFIC, def_resp);           \
  ZB_ZCL_CONSTRUCT_COMMAND_HEADER_EXT(ptr, ZB_ZCL_GET_SEQ_NUM(),                      \
    ZB_ZCL_MANUFACTURER_SPECIFIC, WWAH_MANUFACTURER_CODE,                             \
    ZB_ZCL_CMD_WWAH_ENABLE_WWAH_BAD_PARENT_RECOVERY_ID);                              \
  ZB_ZCL_FINISH_PACKET(buffer, ptr)                                                   \
  ZB_ZCL_SEND_COMMAND_SHORT(                                                          \
    buffer, addr, dst_addr_mode, dst_ep, ep, prfl_id, ZB_ZCL_CLUSTER_ID_WWAH, cb);    \
}

/*! @brief Send Disable WWAH Bad Parent Recovery command
    @param buffer - to put packet to
    @param addr - address to send packet to
    @param dst_addr_mode - addressing mode
    @param dst_ep - destination endpoint
    @param ep - sending endpoint
    @param prfl_id - profile identifier
    @param def_resp - enable/disable default response
    @param cb - callback for getting command send status
*/
#define ZB_ZCL_WWAH_SEND_DISABLE_WWAH_BAD_PARENT_RECOVERY(                            \
  buffer, addr, dst_addr_mode, dst_ep, ep, prfl_id, def_resp, cb)                     \
{                                                                                     \
  zb_uint8_t* ptr = ZB_ZCL_START_PACKET(buffer);                                      \
  ZB_ZCL_CONSTRUCT_SPECIFIC_COMMAND_REQ_FRAME_CONTROL_A(ptr,                          \
    ZB_ZCL_FRAME_DIRECTION_TO_SRV, ZB_ZCL_MANUFACTURER_SPECIFIC, def_resp);           \
  ZB_ZCL_CONSTRUCT_COMMAND_HEADER_EXT(ptr, ZB_ZCL_GET_SEQ_NUM(),                      \
    ZB_ZCL_MANUFACTURER_SPECIFIC, WWAH_MANUFACTURER_CODE,                             \
    ZB_ZCL_CMD_WWAH_DISABLE_WWAH_BAD_PARENT_RECOVERY_ID);                             \
  ZB_ZCL_FINISH_PACKET(buffer, ptr)                                                   \
  ZB_ZCL_SEND_COMMAND_SHORT(                                                          \
    buffer, addr, dst_addr_mode, dst_ep, ep, prfl_id, ZB_ZCL_CLUSTER_ID_WWAH, cb);    \
}

/*! @brief Send Enable Configuration Mode command
    @param buffer - to put packet to
    @param addr - address to send packet to
    @param dst_addr_mode - addressing mode
    @param dst_ep - destination endpoint
    @param ep - sending endpoint
    @param prfl_id - profile identifier
    @param def_resp - enable/disable default response
    @param cb - callback for getting command send status
*/
#define ZB_ZCL_WWAH_SEND_ENABLE_CONFIGURATION_MODE(                                   \
  buffer, addr, dst_addr_mode, dst_ep, ep, prfl_id, def_resp, cb)                     \
{                                                                                     \
  zb_uint8_t* ptr = ZB_ZCL_START_PACKET(buffer);                                      \
  ZB_ZCL_CONSTRUCT_SPECIFIC_COMMAND_REQ_FRAME_CONTROL_A(ptr,                          \
    ZB_ZCL_FRAME_DIRECTION_TO_SRV, ZB_ZCL_MANUFACTURER_SPECIFIC, def_resp);           \
  ZB_ZCL_CONSTRUCT_COMMAND_HEADER_EXT(ptr, ZB_ZCL_GET_SEQ_NUM(),                      \
    ZB_ZCL_MANUFACTURER_SPECIFIC, WWAH_MANUFACTURER_CODE,                             \
    ZB_ZCL_CMD_WWAH_ENABLE_CONFIGURATION_MODE_ID);                                    \
  ZB_ZCL_FINISH_PACKET(buffer, ptr)                                                   \
  ZB_ZCL_SEND_COMMAND_SHORT(                                                          \
    buffer, addr, dst_addr_mode, dst_ep, ep, prfl_id, ZB_ZCL_CLUSTER_ID_WWAH, cb);    \
}

/*! @brief Send Disable Configuration Mode command
    @param buffer - to put packet to
    @param addr - address to send packet to
    @param dst_addr_mode - addressing mode
    @param dst_ep - destination endpoint
    @param ep - sending endpoint
    @param prfl_id - profile identifier
    @param def_resp - enable/disable default response
    @param cb - callback for getting command send status
*/
#define ZB_ZCL_WWAH_SEND_DISABLE_CONFIGURATION_MODE(                                  \
  buffer, addr, dst_addr_mode, dst_ep, ep, prfl_id, def_resp, cb)                     \
{                                                                                     \
  zb_uint8_t* ptr = ZB_ZCL_START_PACKET(buffer);                                      \
  ZB_ZCL_CONSTRUCT_SPECIFIC_COMMAND_REQ_FRAME_CONTROL_A(ptr,                          \
    ZB_ZCL_FRAME_DIRECTION_TO_SRV, ZB_ZCL_MANUFACTURER_SPECIFIC, def_resp);           \
  ZB_ZCL_CONSTRUCT_COMMAND_HEADER_EXT(ptr, ZB_ZCL_GET_SEQ_NUM(),                      \
    ZB_ZCL_MANUFACTURER_SPECIFIC, WWAH_MANUFACTURER_CODE,                             \
    ZB_ZCL_CMD_WWAH_DISABLE_CONFIGURATION_MODE_ID);                                   \
  ZB_ZCL_FINISH_PACKET(buffer, ptr)                                                   \
  ZB_ZCL_SEND_COMMAND_SHORT(                                                          \
    buffer, addr, dst_addr_mode, dst_ep, ep, prfl_id, ZB_ZCL_CLUSTER_ID_WWAH, cb);    \
}

/*! @brief Add cluster_id to Use Trust Center for Cluster command
    @param ptr - [in/out] (zb_uint8_t*) current position
    @param cluster_id - Cluster ID value
*/
#define ZB_ZCL_WWAH_SEND_USE_TRUST_CENTER_FOR_CLUSTER_ADD(ptr, cluster_id)            \
{                                                                                     \
   ZB_ZCL_PACKET_PUT_DATA16(ptr, (cluster_id));                                       \
}

/*! @brief Start Use Trust Center for Cluster command
    @param buffer - to put packet to
    @param def_resp - enable/disable default response
    @param number_of_clusters - Number of Clusters value
    @param ptr - [out] (zb_uint8_t*) current position for ZB_ZCL_WWAH_SEND_USE_TRUST_CENTER_FOR_CLUSTER_ADD
*/
#define ZB_ZCL_WWAH_SEND_USE_TRUST_CENTER_FOR_CLUSTER_START(                          \
  buffer, def_resp, number_of_clusters, ptr)                                          \
{                                                                                     \
  ptr = ZB_ZCL_START_PACKET(buffer);                                                  \
  ZB_ZCL_CONSTRUCT_SPECIFIC_COMMAND_REQ_FRAME_CONTROL_A(ptr,                          \
    ZB_ZCL_FRAME_DIRECTION_TO_SRV, ZB_ZCL_MANUFACTURER_SPECIFIC, def_resp);           \
  ZB_ZCL_CONSTRUCT_COMMAND_HEADER_EXT(ptr, ZB_ZCL_GET_SEQ_NUM(),                      \
    ZB_ZCL_MANUFACTURER_SPECIFIC, WWAH_MANUFACTURER_CODE,                             \
    ZB_ZCL_CMD_WWAH_USE_TRUST_CENTER_FOR_CLUSTER_ID);                                 \
  ZB_ZCL_PACKET_PUT_DATA8(ptr, (number_of_clusters));                                 \
}

/*! @brief End form Use Trust Center for Cluster command
    @param ptr - (zb_uint8_t*) current position
    @param buffer - to put packet to
    @param addr - address to send packet to
    @param dst_addr_mode - addressing mode
    @param dst_ep - destination endpoint
    @param ep - sending endpoint
    @param prfl_id - profile identifier
    @param cb - callback for getting command send status
*/
#define ZB_ZCL_WWAH_SEND_USE_TRUST_CENTER_FOR_CLUSTER_END(                            \
  ptr, buffer, addr, dst_addr_mode, dst_ep, ep, prfl_id, cb)                          \
{                                                                                     \
  ZB_ZCL_FINISH_PACKET(buffer, ptr)                                                   \
  ZB_ZCL_SEND_COMMAND_SHORT(                                                          \
    buffer, addr, dst_addr_mode, dst_ep, ep, prfl_id, ZB_ZCL_CLUSTER_ID_WWAH, cb);    \
}

/*! @brief Send Trust Center for Cluster Server Query command
    @param buffer - to put packet to
    @param addr - address to send packet to
    @param dst_addr_mode - addressing mode
    @param dst_ep - destination endpoint
    @param ep - sending endpoint
    @param prfl_id - profile identifier
    @param def_resp - enable/disable default response
    @param cb - callback for getting command send status
*/
#define ZB_ZCL_WWAH_SEND_TRUST_CENTER_FOR_CLUSTER_SERVER_QUERY(                       \
  buffer, addr, dst_addr_mode, dst_ep, ep, prfl_id, def_resp, cb)                     \
{                                                                                     \
  zb_uint8_t* ptr = ZB_ZCL_START_PACKET(buffer);                                      \
  ZB_ZCL_CONSTRUCT_SPECIFIC_COMMAND_REQ_FRAME_CONTROL_A(ptr,                          \
    ZB_ZCL_FRAME_DIRECTION_TO_SRV, ZB_ZCL_MANUFACTURER_SPECIFIC, def_resp);           \
  ZB_ZCL_CONSTRUCT_COMMAND_HEADER_EXT(ptr, ZB_ZCL_GET_SEQ_NUM(),                      \
    ZB_ZCL_MANUFACTURER_SPECIFIC, WWAH_MANUFACTURER_CODE,                             \
    ZB_ZCL_CMD_WWAH_TRUST_CENTER_FOR_CLUSTER_SERVER_QUERY_ID);                        \
  ZB_ZCL_FINISH_PACKET(buffer, ptr)                                                   \
  ZB_ZCL_SEND_COMMAND_SHORT(                                                          \
    buffer, addr, dst_addr_mode, dst_ep, ep, prfl_id, ZB_ZCL_CLUSTER_ID_WWAH, cb);    \
}

/*! @brief Send APS Link Key Authorization Query Response command
    @param buffer - to put packet to
    @param seq - sequence
    @param addr - address to send packet to
    @param dst_addr_mode - addressing mode
    @param dst_ep - destination endpoint
    @param ep - sending endpoint
    @param prfl_id - profile identifier
    @param cb - callback for getting command send status
    @param cluster_id - Cluster ID value
    @param aps_link_key_authorization_status - APS Link Key Authorization Status value*/
#define ZB_ZCL_WWAH_SEND_APS_LINK_KEY_AUTHORIZATION_QUERY_RESPONSE(                   \
  buffer, seq, addr, dst_addr_mode, dst_ep, ep, prfl_id, cb, cluster_id,              \
  aps_link_key_authorization_status)                                                  \
{                                                                                     \
  zb_uint8_t* ptr = ZB_ZCL_START_PACKET(buffer);                                      \
  ZB_ZCL_CONSTRUCT_SPECIFIC_COMMAND_RESP_FRAME_CONTROL_A(ptr,                         \
    ZB_ZCL_FRAME_DIRECTION_TO_CLI, ZB_ZCL_MANUFACTURER_SPECIFIC);                     \
  ZB_ZCL_CONSTRUCT_COMMAND_HEADER_EXT(ptr, (seq),                                     \
    ZB_ZCL_MANUFACTURER_SPECIFIC, WWAH_MANUFACTURER_CODE,                             \
    ZB_ZCL_CMD_WWAH_APS_LINK_KEY_AUTHORIZATION_QUERY_RESPONSE_ID);                    \
  ZB_ZCL_PACKET_PUT_DATA16(ptr, (cluster_id));                                        \
  ZB_ZCL_PACKET_PUT_DATA8(ptr, (aps_link_key_authorization_status));                  \
  ZB_ZCL_FINISH_PACKET(buffer, ptr)                                                   \
  ZB_ZCL_SEND_COMMAND_SHORT(                                                          \
    buffer, addr, dst_addr_mode, dst_ep, ep, prfl_id, ZB_ZCL_CLUSTER_ID_WWAH, cb);    \
}

/*! @brief Send Powering Off Notification command
    @param buffer - to put packet to
    @param addr - address to send packet to
    @param dst_addr_mode - addressing mode
    @param dst_ep - destination endpoint
    @param ep - sending endpoint
    @param prfl_id - profile identifier
    @param cb - callback for getting command send status
    @param power_notification_reason - PowerNotificationReason value
    @param manufacturer_id - Manufacturer ID value
    @param manufacturer_power_notification_reason_length - Manufacturer Power Notification Reason Length value
    @param manufacturer_power_notification_reason - Manufacturer Power Notification Reason value*/
#define ZB_ZCL_WWAH_SEND_POWERING_OFF_NOTIFICATION(                                   \
  buffer, addr, dst_addr_mode, dst_ep, ep, prfl_id, cb,                               \
  power_notification_reason, manufacturer_id,                                         \
  manufacturer_power_notification_reason_length,                                      \
  manufacturer_power_notification_reason)                                             \
{                                                                                     \
  zb_uint8_t* ptr = ZB_ZCL_START_PACKET(buffer);                                      \
  ZB_ZCL_CONSTRUCT_SPECIFIC_COMMAND_RESP_FRAME_CONTROL_A(ptr,                         \
    ZB_ZCL_FRAME_DIRECTION_TO_CLI, ZB_ZCL_MANUFACTURER_SPECIFIC);                     \
  ZB_ZCL_CONSTRUCT_COMMAND_HEADER_EXT(ptr, ZB_ZCL_GET_SEQ_NUM(),                      \
    ZB_ZCL_MANUFACTURER_SPECIFIC, WWAH_MANUFACTURER_CODE,                             \
    ZB_ZCL_CMD_WWAH_POWERING_OFF_NOTIFICATION_ID);                                    \
  ZB_ZCL_PACKET_PUT_DATA8(ptr, (power_notification_reason));                          \
  ZB_ZCL_PACKET_PUT_DATA16(ptr, (manufacturer_id));                                   \
  ZB_ZCL_PACKET_PUT_DATA8(ptr, (manufacturer_power_notification_reason_length));      \
  ZB_ZCL_PACKET_PUT_DATA_N(ptr, (manufacturer_power_notification_reason),             \
    (manufacturer_power_notification_reason_length));                                 \
  ZB_ZCL_FINISH_PACKET(buffer, ptr)                                                   \
  ZB_ZCL_SEND_COMMAND_SHORT(                                                          \
    buffer, addr, dst_addr_mode, dst_ep, ep, prfl_id, ZB_ZCL_CLUSTER_ID_WWAH, cb);    \
}

/*! @brief Send Powering On Notification command
    @param buffer - to put packet to
    @param addr - address to send packet to
    @param dst_addr_mode - addressing mode
    @param dst_ep - destination endpoint
    @param ep - sending endpoint
    @param prfl_id - profile identifier
    @param cb - callback for getting command send status
    @param power_notification_reason - PowerNotificationReason value
    @param manufacturer_id - Manufacturer ID value
    @param manufacturer_power_notification_reason_length - Manufacturer Power Notification Reason Length value
    @param manufacturer_power_notification_reason - Manufacturer Power Notification Reason value*/
#define ZB_ZCL_WWAH_SEND_POWERING_ON_NOTIFICATION(                                    \
  buffer, addr, dst_addr_mode, dst_ep, ep, prfl_id, cb,                               \
  power_notification_reason, manufacturer_id,                                         \
  manufacturer_power_notification_reason_length,                                      \
  manufacturer_power_notification_reason)                                             \
{                                                                                     \
  zb_uint8_t* ptr = ZB_ZCL_START_PACKET(buffer);                                      \
  ZB_ZCL_CONSTRUCT_SPECIFIC_COMMAND_RESP_FRAME_CONTROL_A(ptr,                         \
    ZB_ZCL_FRAME_DIRECTION_TO_CLI, ZB_ZCL_MANUFACTURER_SPECIFIC);                     \
  ZB_ZCL_CONSTRUCT_COMMAND_HEADER_EXT(ptr, ZB_ZCL_GET_SEQ_NUM(),                      \
    ZB_ZCL_MANUFACTURER_SPECIFIC, WWAH_MANUFACTURER_CODE,                             \
    ZB_ZCL_CMD_WWAH_POWERING_ON_NOTIFICATION_ID);                                     \
  ZB_ZCL_PACKET_PUT_DATA8(ptr, (power_notification_reason));                          \
  ZB_ZCL_PACKET_PUT_DATA16(ptr, (manufacturer_id));                                   \
  ZB_ZCL_PACKET_PUT_DATA8(ptr, (manufacturer_power_notification_reason_length));      \
  ZB_ZCL_PACKET_PUT_DATA_N(ptr, (manufacturer_power_notification_reason),             \
    (manufacturer_power_notification_reason_length));                                 \
  ZB_ZCL_FINISH_PACKET(buffer, ptr)                                                   \
  ZB_ZCL_SEND_COMMAND_SHORT(                                                          \
    buffer, addr, dst_addr_mode, dst_ep, ep, prfl_id, ZB_ZCL_CLUSTER_ID_WWAH, cb);    \
}

/*! @brief Send Short Address Change command
    @param buffer - to put packet to
    @param addr - address to send packet to
    @param dst_addr_mode - addressing mode
    @param dst_ep - destination endpoint
    @param ep - sending endpoint
    @param prfl_id - profile identifier
    @param cb - callback for getting command send status
    @param device_eui64 - DeviceEUI64 value
    @param device_short - DeviceShort value*/
#define ZB_ZCL_WWAH_SEND_SHORT_ADDRESS_CHANGE(                                        \
  buffer, addr, dst_addr_mode, dst_ep, ep, prfl_id, cb, device_eui64, device_short)   \
{                                                                                     \
  zb_uint8_t* ptr = ZB_ZCL_START_PACKET(buffer);                                      \
  ZB_ZCL_CONSTRUCT_SPECIFIC_COMMAND_RESP_FRAME_CONTROL_A(ptr,                         \
    ZB_ZCL_FRAME_DIRECTION_TO_CLI, ZB_ZCL_MANUFACTURER_SPECIFIC);                     \
  ZB_ZCL_CONSTRUCT_COMMAND_HEADER_EXT(ptr, ZB_ZCL_GET_SEQ_NUM(),                      \
    ZB_ZCL_MANUFACTURER_SPECIFIC, WWAH_MANUFACTURER_CODE,                             \
    ZB_ZCL_CMD_WWAH_SHORT_ADDRESS_CHANGE_ID);                                         \
  ZB_ZCL_PACKET_PUT_DATA64(ptr, (device_eui64));                                      \
  ZB_ZCL_PACKET_PUT_DATA16(ptr, (device_short));                                      \
  ZB_ZCL_FINISH_PACKET(buffer, ptr)                                                   \
  ZB_ZCL_SEND_COMMAND_SHORT(                                                          \
    buffer, addr, dst_addr_mode, dst_ep, ep, prfl_id, ZB_ZCL_CLUSTER_ID_WWAH, cb);    \
}

/*! @brief Add cluster_id to APS ACK Requirement Query Response command
    @param ptr - [in/out] (zb_uint8_t*) current position
    @param cluster_id - Cluster ID value
*/
#define ZB_ZCL_WWAH_SEND_APS_ACK_REQUIREMENT_QUERY_RESPONSE_ADD(ptr, cluster_id)      \
{                                                                                     \
   ZB_ZCL_PACKET_PUT_DATA16(ptr, (cluster_id));                                       \
}

/*! @brief Start APS ACK Requirement Query Response command
    @param buffer - to put packet to
    @param seq - sequence
    @param number_of_clusters_to_exempt - Number of Clusters to Exempt value
    @param ptr - [out] (zb_uint8_t*) current position for ZB_ZCL_WWAH_SEND_APS_ACK_REQUIREMENT_QUERY_RESPONSE_ADD
*/
#define ZB_ZCL_WWAH_SEND_APS_ACK_REQUIREMENT_QUERY_RESPONSE_START(                    \
  buffer, seq, number_of_clusters_to_exempt, ptr)                                     \
{                                                                                     \
  (ptr) = ZB_ZCL_START_PACKET(buffer);                                                \
  ZB_ZCL_CONSTRUCT_SPECIFIC_COMMAND_RESP_FRAME_CONTROL_A(ptr,                         \
    ZB_ZCL_FRAME_DIRECTION_TO_CLI, ZB_ZCL_MANUFACTURER_SPECIFIC);                     \
  ZB_ZCL_CONSTRUCT_COMMAND_HEADER_EXT(ptr, (seq),                                     \
    ZB_ZCL_MANUFACTURER_SPECIFIC, WWAH_MANUFACTURER_CODE,                             \
    ZB_ZCL_CMD_WWAH_APS_ACK_REQUIREMENT_QUERY_RESPONSE_ID);                           \
  ZB_ZCL_PACKET_PUT_DATA8(ptr, (number_of_clusters_to_exempt));                       \
}

/*! @brief End form APS ACK Requirement Query Response command
    @param ptr - (zb_uint8_t*) current position
    @param buffer - to put packet to
    @param addr - address to send packet to
    @param dst_addr_mode - addressing mode
    @param dst_ep - destination endpoint
    @param ep - sending endpoint
    @param prfl_id - profile identifier
*/
#define ZB_ZCL_WWAH_SEND_APS_ACK_REQUIREMENT_QUERY_RESPONSE_END(                      \
  ptr, buffer, addr, dst_addr_mode, dst_ep, ep, prfl_id)                              \
{                                                                                     \
  ZB_ZCL_FINISH_PACKET(buffer, ptr)                                                   \
  ZB_ZCL_SEND_COMMAND_SHORT(                                                          \
    buffer, addr, dst_addr_mode, dst_ep, ep, prfl_id, ZB_ZCL_CLUSTER_ID_WWAH, NULL);  \
}

/*! @brief Send Power Descriptor Change command
    @param buffer - to put packet to
    @param addr - address to send packet to
    @param dst_addr_mode - addressing mode
    @param dst_ep - destination endpoint
    @param ep - sending endpoint
    @param prfl_id - profile identifier
    @param cb - callback for getting command send status
    @param node_power_desc - Node Power Descriptor value */
#define ZB_ZCL_WWAH_SEND_POWER_DESCRIPTOR_CHANGE(                                     \
  buffer, addr, dst_addr_mode, dst_ep, ep, prfl_id, cb, node_power_desc)              \
{                                                                                     \
  zb_uint8_t* ptr = ZB_ZCL_START_PACKET(buffer);                                      \
  ZB_ZCL_CONSTRUCT_SPECIFIC_COMMAND_RESP_FRAME_CONTROL_A(ptr,                         \
    ZB_ZCL_FRAME_DIRECTION_TO_CLI, ZB_ZCL_MANUFACTURER_SPECIFIC);                     \
  ZB_ZCL_CONSTRUCT_COMMAND_HEADER_EXT(ptr, ZB_ZCL_GET_SEQ_NUM(),                      \
    ZB_ZCL_MANUFACTURER_SPECIFIC, WWAH_MANUFACTURER_CODE,                             \
    ZB_ZCL_CMD_WWAH_POWER_DESCRIPTOR_CHANGE_ID);                                      \
  ZB_ZCL_PACKET_PUT_DATA16(ptr, (node_power_desc));                                   \
  ZB_ZCL_FINISH_PACKET(buffer, ptr)                                                   \
  ZB_ZCL_SEND_COMMAND_SHORT(                                                          \
    buffer, addr, dst_addr_mode, dst_ep, ep, prfl_id, ZB_ZCL_CLUSTER_ID_WWAH, cb);    \
}

/*! @brief Send New Debug Report Notification command
    @param buffer - to put packet to
    @param addr - address to send packet to
    @param dst_addr_mode - addressing mode
    @param dst_ep - destination endpoint
    @param ep - sending endpoint
    @param prfl_id - profile identifier
    @param cb - callback for getting command send status
    @param debug_report_id - Debug Report ID value
    @param size_of_report - Size of Report value*/
#define ZB_ZCL_WWAH_SEND_NEW_DEBUG_REPORT_NOTIFICATION(                               \
  buffer, addr, dst_addr_mode, dst_ep, ep, prfl_id, cb, debug_report_id,              \
  size_of_report)                                                                     \
{                                                                                     \
  zb_uint8_t* ptr = ZB_ZCL_START_PACKET(buffer);                                      \
  ZB_ZCL_CONSTRUCT_SPECIFIC_COMMAND_RESP_FRAME_CONTROL_A(ptr,                         \
    ZB_ZCL_FRAME_DIRECTION_TO_CLI, ZB_ZCL_MANUFACTURER_SPECIFIC);                     \
  ZB_ZCL_CONSTRUCT_COMMAND_HEADER_EXT(ptr, ZB_ZCL_GET_SEQ_NUM(),                      \
    ZB_ZCL_MANUFACTURER_SPECIFIC, WWAH_MANUFACTURER_CODE,                             \
    ZB_ZCL_CMD_WWAH_NEW_DEBUG_REPORT_NOTIFICATION_ID);                                \
  ZB_ZCL_PACKET_PUT_DATA8(ptr, (debug_report_id));                                    \
  ZB_ZCL_PACKET_PUT_DATA32(ptr, (size_of_report));                                    \
  ZB_ZCL_FINISH_PACKET(buffer, ptr)                                                   \
  ZB_ZCL_SEND_COMMAND_SHORT(                                                          \
    buffer, addr, dst_addr_mode, dst_ep, ep, prfl_id, ZB_ZCL_CLUSTER_ID_WWAH, cb);    \
}

/*! @brief Send Debug Report Query Response command
    @param buffer - to put packet to
    @param seq - sequence
    @param addr - address to send packet to
    @param dst_addr_mode - addressing mode
    @param dst_ep - destination endpoint
    @param ep - sending endpoint
    @param prfl_id - profile identifier
    @param cb - callback for getting command send status
    @param debug_report_id - Debug Report ID value
    @param report_data - Report data value
    @param size_of_report - Report data size */

#define ZB_ZCL_WWAH_SEND_DEBUG_REPORT_QUERY_RESPONSE(                                      \
  buffer, seq, addr, dst_addr_mode, dst_ep, ep, prfl_id, cb, debug_report_id, report_data, \
  size_of_report)                                                                          \
{                                                                                          \
  zb_uint8_t* ptr = ZB_ZCL_START_PACKET(buffer);                                           \
  ZB_ZCL_CONSTRUCT_SPECIFIC_COMMAND_RESP_FRAME_CONTROL_A(ptr,                              \
    ZB_ZCL_FRAME_DIRECTION_TO_CLI, ZB_ZCL_MANUFACTURER_SPECIFIC);                          \
  ZB_ZCL_CONSTRUCT_COMMAND_HEADER_EXT(ptr, (seq),                                          \
    ZB_ZCL_MANUFACTURER_SPECIFIC, WWAH_MANUFACTURER_CODE,                                  \
    ZB_ZCL_CMD_WWAH_DEBUG_REPORT_QUERY_RESPONSE_ID);                                       \
  ZB_ZCL_PACKET_PUT_DATA8(ptr, (debug_report_id));                                         \
  ZB_ZCL_PACKET_PUT_DATA_N(ptr, (report_data), (size_of_report));                          \
  ZB_ZCL_FINISH_PACKET(buffer, ptr)                                                        \
  ZB_ZCL_SEND_COMMAND_SHORT(                                                               \
    buffer, addr, dst_addr_mode, dst_ep, ep, prfl_id, ZB_ZCL_CLUSTER_ID_WWAH, cb);         \
}

/*! @brief Add cluster_id to Trust Center for Cluster Server Query Response command
    @param ptr - [in/out] (zb_uint8_t*) current position
    @param cluster_id - Cluster ID value
*/
#define ZB_ZCL_WWAH_SEND_TRUST_CENTER_FOR_CLUSTER_SERVER_QUERY_RESPONSE_ADD(ptr, cluster_id) \
{                                                                                            \
   ZB_ZCL_PACKET_PUT_DATA16(ptr, (cluster_id));                                              \
}

/*! @brief Start Trust Center for Cluster Server Query Response command
    @param buffer - to put packet to
    @param seq - sequence
    @param number_of_clusters - Number of Clusters value
    @param ptr - [out] (zb_uint8_t*) current position for ZB_ZCL_WWAH_SEND_TRUST_CENTER_FOR_CLUSTER_SERVER_QUERY_RESPONSE_ADD
*/
#define ZB_ZCL_WWAH_SEND_TRUST_CENTER_FOR_CLUSTER_SERVER_QUERY_RESPONSE_START(        \
  buffer, seq, number_of_clusters, ptr)                                               \
{                                                                                     \
  (ptr) = ZB_ZCL_START_PACKET(buffer);                                                \
  ZB_ZCL_CONSTRUCT_SPECIFIC_COMMAND_RESP_FRAME_CONTROL_A(ptr,                         \
    ZB_ZCL_FRAME_DIRECTION_TO_CLI, ZB_ZCL_MANUFACTURER_SPECIFIC);                     \
  ZB_ZCL_CONSTRUCT_COMMAND_HEADER_EXT(ptr, (seq),                                     \
    ZB_ZCL_MANUFACTURER_SPECIFIC, WWAH_MANUFACTURER_CODE,                             \
    ZB_ZCL_CMD_WWAH_TRUST_CENTER_FOR_CLUSTER_SERVER_QUERY_RESPONSE_ID);               \
  ZB_ZCL_PACKET_PUT_DATA8(ptr, (number_of_clusters));                                 \
}

/*! @brief End form Trust Center for Cluster Server Query Response command
    @param ptr - (zb_uint8_t*) current position
    @param buffer - to put packet to
    @param addr - address to send packet to
    @param dst_addr_mode - addressing mode
    @param dst_ep - destination endpoint
    @param ep - sending endpoint
    @param prfl_id - profile identifier
*/
#define ZB_ZCL_WWAH_SEND_TRUST_CENTER_FOR_CLUSTER_SERVER_QUERY_RESPONSE_END(          \
  ptr, buffer, addr, dst_addr_mode, dst_ep, ep, prfl_id)                              \
{                                                                                     \
  ZB_ZCL_FINISH_PACKET(buffer, ptr)                                                   \
  ZB_ZCL_SEND_COMMAND_SHORT(                                                          \
    buffer, addr, dst_addr_mode, dst_ep, ep, prfl_id, ZB_ZCL_CLUSTER_ID_WWAH, NULL);  \
}

/*! @brief Start Survey Beacons Response command
    @param buffer - to put packet to
    @param seq - sequence
*/
#define ZB_ZCL_WWAH_SEND_SURVEY_BEACONS_RESPONSE_START(                               \
  buffer, seq)                                                                        \
{                                                                                     \
  zb_uint8_t *ptr = zb_buf_begin(buffer);                                             \
  ZB_ZCL_CONSTRUCT_SPECIFIC_COMMAND_RESP_FRAME_CONTROL_A(ptr,                         \
    ZB_ZCL_FRAME_DIRECTION_TO_CLI, ZB_ZCL_MANUFACTURER_SPECIFIC);                     \
  ZB_ZCL_CONSTRUCT_COMMAND_HEADER_EXT(ptr, (seq),                                     \
    ZB_ZCL_MANUFACTURER_SPECIFIC, WWAH_MANUFACTURER_CODE,                             \
    ZB_ZCL_CMD_WWAH_SURVEY_BEACONS_RESPONSE_ID);                                      \
}

/*! @brief Add beacons number and beacon survey info to Survey Beacons Response command
    @param ptr - [in/out] (zb_uint8_t*) current position
    @param src - source pointer to the first Beacon value
    @param num - number of beacons
*/
#define ZB_ZCL_WWAH_SEND_SURVEY_BEACONS_RESPONSE_ADD_ALL(ptr, src, num)                   \
{                                                                                         \
  ZB_ZCL_PACKET_PUT_DATA8((ptr), (num));                                                  \
  ZB_MEMMOVE((ptr), (src), ((num) * sizeof(zb_zcl_wwah_beacon_survey_t)));                \
  (ptr) += ((num) * sizeof(zb_zcl_wwah_beacon_survey_t));                                 \
}

/*! @brief End form Survey Beacons Response command
    @param ptr - (zb_uint8_t*) current position
    @param buffer - to put packet to
    @param addr - address to send packet to
    @param dst_addr_mode - addressing mode
    @param dst_ep - destination endpoint
    @param ep - sending endpoint
    @param prfl_id - profile identifier
*/
#define ZB_ZCL_WWAH_SEND_SURVEY_BEACONS_RESPONSE_END(                                 \
  ptr, buffer, addr, dst_addr_mode, dst_ep, ep, prfl_id)                              \
{                                                                                     \
  ZB_ZCL_FINISH_PACKET(buffer, ptr)                                                   \
  ZB_ZCL_SEND_COMMAND_SHORT(                                                          \
    buffer, addr, dst_addr_mode, dst_ep, ep, prfl_id, ZB_ZCL_CLUSTER_ID_WWAH, NULL);  \
}
zb_bool_t zb_is_wwah_server(void);
void wwah_post_commissioning_actions(void);
zb_bool_t zb_zcl_wwah_check_new_channel(zb_uint32_t new_channel_mask);
zb_bool_t zb_zcl_wwah_check_new_panid(zb_uint16_t new_panid);
zb_uint8_t zb_zcl_wwah_mac_poll_cca_retry_count(void);
zb_time_t zb_zcl_wwah_mac_poll_failure_wait_time(void);
zb_bool_t zb_zcl_wwah_check_if_forced_to_use_tc(zb_uint16_t cluster_id);
zb_bool_t zb_zcl_wwah_check_zdo_command(zb_apsde_data_indication_t *di);
zb_bool_t zb_zcl_wwah_check_if_aps_ack_needed(zb_uint16_t cluster_id);
zb_bool_t zb_zcl_wwah_check_if_aps_link_key_authorization_needed(zb_uint16_t cluster_id);
zb_bool_t zb_zcl_wwah_check_cluster_permission(zb_uint16_t nwk_addr, zb_uint16_t cluster_id);
zb_bool_t zb_zcl_wwah_check_if_leave_without_rejoin_allowed(void);
zb_bool_t zb_zcl_wwah_check_if_wwah_rejoin_enabled(void);
void zb_zcl_wwah_schedule_send_power_descriptor_change(void);
zb_bool_t zb_zcl_wwah_check_zdo_command(zb_apsde_data_indication_t *di);
zb_bool_t zb_zcl_wwah_check_if_interpan_supported(void);
zb_bool_t zb_zcl_wwah_check_if_downgrade_disabled(void);
zb_bool_t zb_zcl_wwah_periodic_checkin_read_attr_handle(zb_uint8_t param);
zb_bool_t zb_zcl_wwah_periodic_checkin_block_zcl_cmd(zb_zcl_parsed_hdr_t *cmd_info);
void zb_zcl_wwah_periodic_checkin_tc_poll(zb_uint8_t param);
void zb_zcl_wwah_periodic_checkin_match_desc_req(zb_uint8_t param);
void zb_zcl_wwah_periodic_checkin_match_desc_req_delayed(zb_uint8_t param);
void zb_zcl_wwah_stop_periodic_checkin(void);
void zb_zcl_wwah_start_periodic_checkin(void);
zb_bool_t zb_zcl_wwah_check_src_addr_for_cluster(zb_uint16_t short_addr, zb_uint16_t cluster_id);

void zb_zcl_wwah_set_wwah_behavior(zb_uint8_t behavior);
void zb_zcl_wwah_stop_bad_parent_recovery(void);
void zb_zcl_wwah_start_bad_parent_recovery(void);

#ifdef ZB_JOIN_CLIENT
zb_ret_t zb_zcl_wwah_get_rejoin_tmo(zb_uint16_t attempt, zb_time_t *tmo);
#endif

/** @endcond */ /* DOXYGEN_ZCL_SECTION */

#define ZB_ZCL_WWAH_REPORT_ATTR_COUNT 0

#if (defined ZB_ZCL_SUPPORT_CLUSTER_WWAH && (defined ZB_ZCL_ENABLE_WWAH_SERVER || defined ZB_ZCL_ENABLE_WWAH_CLIENT))
typedef ZB_PACKED_PRE struct zb_zcl_wwah_context_s
{
  zb_uint8_t wwah_behavior;
#ifdef ZB_ZCL_ENABLE_WWAH_SERVER
  zb_uint8_t wwah_hub_endpoint;
  zb_uint16_t wwah_hub_address;
  zb_uint16_t time_server_addr;
  zb_uint8_t time_server_endpoint;

  zb_uint8_t aps_ack_exempt_table_cnt;
  /* This is a list of clusters that should be exempt from requiring APS ACKs on unicasts. */
  zb_uint16_t aps_ack_exempt_table[ZB_ZCL_WWAH_APS_ACK_EXEMPT_TABLE_SIZE];

  /* ZB_TRUE - enforces that all cluster commands for the cluster have APS level security
   * except for those in the exemption list*/
  /* ZB_FALSE - enforces that all cluster commands for the cluster doesn't have APS level security
   * except for those in the exemption list. This is default value*/
  zb_bool_t aps_link_key_enabled_by_default;

  zb_uint8_t aps_link_key_authorization_table_cnt;
  /* This is a exemption list of clusters for requiring APS level security */
  zb_uint16_t aps_link_key_authorization_table[ZB_ZCL_WWAH_APS_LINK_KEY_AUTHORIZATION_TABLE_SIZE];

  zb_uint8_t use_trust_center_for_cluster_table_cnt;
  /* This is a list of clusters that should be forced to use the Trust Center. */
  zb_uint16_t use_trust_center_for_cluster_table[ZB_ZCL_WWAH_USE_TRUST_CENTER_FOR_CLUSTER_TABLE_SIZE];

  /* ======= Survey Beacon related context ======= */
  zb_uint8_t survey_beacons_seq_number;
  zb_uint8_t survey_beacons_in_progress;

  zb_zcl_wwah_periodic_checkins_data_t periodic_checkins;

  zb_zcl_wwah_enable_wwah_rejoin_algorithm_t rejoin_alg;
  zb_zcl_wwah_bad_parent_recovery_data_t bad_parent_recovery;
#endif
#if !(defined ZB_ZCL_ENABLE_WWAH_SERVER || defined ZB_ZCL_ENABLE_WWAH_CLIENT)
  zb_uint8_t make_iso_compiler_happy;
#endif
} zb_zcl_wwah_context_t;
#endif

#define ZB_ZCL_WWAH_REJOIN_SLEEPY_ED_TIMEOUT (ZB_TIME_ONE_SECOND * 60 * 15)
#define ZB_ZCL_WWAH_REJOIN_NON_SLEEPY_ED_ROUTER_TIMEOUT (ZB_TIME_ONE_SECOND * 60 * 60 * 24)
/* For debug purpose: #define ZB_ZCL_WWAH_REJOIN_NON_SLEEPY_ED_ROUTER_TIMEOUT (ZB_TIME_ONE_SECOND * 60)*/

#define ZB_ZCL_WWAH_BAD_PARENT_RECOVERY_TIMEOUT (ZB_TIME_ONE_SECOND * 60 * 60 * 24)
/* For debug purpose: #define ZB_ZCL_WWAH_BAD_PARENT_RECOVERY_TIMEOUT (ZB_TIME_ONE_SECOND * 30)*/
#define ZB_ZCL_WWAH_BAD_PARENT_RECOVERY_POLL_CONTROL_CHECK_IN_FAILED_CNT 3
/* FIXME: Is it defined somewhere in WWAH spec? */
#define ZB_ZCL_WWAH_BAD_PARENT_RECOVERY_RSSI_IS_GOOD(rssi) (rssi >= -100)
void zb_zcl_wwah_bad_parent_recovery_signal(zb_bufid_t sig);

zb_ret_t zb_zcl_wwah_request_new_aps_link_key_handler(zb_uint8_t param);
zb_ret_t zb_zcl_wwah_enable_wwah_app_event_retry_algorithm_handler(zb_uint8_t param);
zb_ret_t zb_zcl_wwah_disable_wwah_app_event_retry_algorithm_handler(zb_uint8_t param);
zb_ret_t zb_zcl_wwah_request_time_handler(zb_uint8_t param);
zb_ret_t zb_zcl_wwah_enable_wwah_rejoin_algorithm_handler(zb_uint8_t param);
zb_ret_t zb_zcl_wwah_disable_wwah_rejoin_algorithm_handler(zb_uint8_t param);

void zb_zcl_wwah_init_server(void);
void zb_zcl_wwah_init_server_attr(void);
void zb_zcl_wwah_init_client(void);
zb_ret_t zb_zcl_wwah_update_time(zb_uint8_t param);
#define ZB_ZCL_CLUSTER_ID_WWAH_SERVER_ROLE_INIT zb_zcl_wwah_init_server
#define ZB_ZCL_CLUSTER_ID_WWAH_CLIENT_ROLE_INIT zb_zcl_wwah_init_client

#endif /* ZB_ZCL_WWAH_H */
