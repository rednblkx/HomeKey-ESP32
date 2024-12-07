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
/* PURPOSE: Metering cluster definitions
*/

#ifndef ZB_ZCL_METERING_H
#define ZB_ZCL_METERING_H 1

#include "zcl/zb_zcl_common.h"
#include "zcl/zb_zcl_commands.h"


/** @cond DOXYGEN_ZCL_SECTION */

/** @addtogroup ZB_ZCL_METERING
  * @{
  *   @details
  *   The Metering Cluster provides a mechanism to retrieve usage information
  *   from Electric, Gas, Water, and potentially Thermal metering devices.
  */

/** @defgroup ZB_ZCL_METERING_CLI_ATTRS Metering cluster client attributes and enumerations
 *  @{
 */

/** @brief Metering cluster client attribute sets
 *  @see SE 1.4 spec, table D.58.
 */
enum zb_zcl_metering_cli_attr_set_e
{
  ZB_ZCL_METERING_SET_CLI_NOTIFICATION_ATTRIBUTE  = 0x00,  /**< Notification Attribute Set */
};


/** @brief Metering cluster client attributes */
enum zb_zcl_metering_cli_attr_e
{
  ZB_ZCL_ATTR_METERING_CLI_FUNCTIONAL_NOTIFICATION_FLAGS = ZB_ZCL_ATTR_SET_WITH_ATTR_ID(ZB_ZCL_METERING_SET_CLI_NOTIFICATION_ATTRIBUTE, 0x00),
                                                /**< @e FunctionalNotificationFlags attribute is implemented as a set of bit flags
                                                 * which are have a predefined action associated with a bit that is not based on a specific command,
                                                 * but may require the Mirrored device to trigger some additional functionality within the system */
  ZB_ZCL_ATTR_METERING_CLI_NOTIFICATION_FLAGS2, /**< @e NotificationFlagsN are 32-bit bitmaps that each represent a series of flags.
                                                * Each flag represents an outstanding command that the Mirror is holding on behalf of the BOMD.
                                                * Each flag represents a different command.
                                                * The format of these attributes is dictated by the scheme that is currently in operation. */
  ZB_ZCL_ATTR_METERING_CLI_NOTIFICATION_FLAGS3, /**< @copydoc ZB_ZCL_ATTR_METERING_CLI_NOTIFICATION_FLAGS2 */
  ZB_ZCL_ATTR_METERING_CLI_NOTIFICATION_FLAGS4, /**< @copydoc ZB_ZCL_ATTR_METERING_CLI_NOTIFICATION_FLAGS2 */
  ZB_ZCL_ATTR_METERING_CLI_NOTIFICATION_FLAGS5, /**< @copydoc ZB_ZCL_ATTR_METERING_CLI_NOTIFICATION_FLAGS2 */
  ZB_ZCL_ATTR_METERING_CLI_NOTIFICATION_FLAGS6, /**< @copydoc ZB_ZCL_ATTR_METERING_CLI_NOTIFICATION_FLAGS2 */
  ZB_ZCL_ATTR_METERING_CLI_NOTIFICATION_FLAGS7, /**< @copydoc ZB_ZCL_ATTR_METERING_CLI_NOTIFICATION_FLAGS2 */
  ZB_ZCL_ATTR_METERING_CLI_NOTIFICATION_FLAGS8, /**< @copydoc ZB_ZCL_ATTR_METERING_CLI_NOTIFICATION_FLAGS2 */
};

/** @} */ /* ZB_ZCL_METERING_CLI_ATTRS */

/** @defgroup ZB_ZCL_METERING_SRV_ATTRS Metering cluster server attributes and enumerations
 *  @{
 */

/** @brief Metering cluster server attribute sets
 *  @see SE 1.4 spec, table D.10.
 */
enum zb_zcl_metering_srv_attr_set_e
{
  ZB_ZCL_METERING_SET_READ_INFO                           = 0x00,  /**< Reading Information Set */
  ZB_ZCL_METERING_SET_TOU_INFORMATION                     = 0x01,  /**< TOU Information Set */
  ZB_ZCL_METERING_SET_METER_STATUS                        = 0x02,  /**< Meter Status */
  ZB_ZCL_METERING_SET_FORMATTING                          = 0x03,  /**< Formatting */
  ZB_ZCL_METERING_SET_HISTORICAL_CONSUMPTION              = 0x04,  /**< Historical Consumption */
  ZB_ZCL_METERING_SET_LOAD_PROFILE_CONFIGURATION          = 0x05,  /**< Load Profile Configuration */
  ZB_ZCL_METERING_SET_SUPPLY_LIMIT                        = 0x06,  /**< Supply Limit */
  ZB_ZCL_METERING_SET_BLOCK_INFORMATION_DELIVERED         = 0x07,  /**< Block Information (Delivered) */
  ZB_ZCL_METERING_SET_ALARMS                              = 0x08,  /**< Alarms */
  ZB_ZCL_METERING_SET_BLOCK_INFORMATION_RECEIVED          = 0x09,  /**< Block Information (Received) */
  ZB_ZCL_METERING_SET_METER_BILLING                       = 0x0A,  /**< Meter Billing Attribute Set */
  ZB_ZCL_METERING_SET_SUPPLY_CONTROL                      = 0x0B,  /**< Supply Control Attribute Set */
  ZB_ZCL_METERING_SET_ALTERNATIVE_HISTORICAL_CONSUMPTION  = 0x0C,  /**< Alternative Historical Consumption */
  ZB_ZCL_METERING_SET_FOUR_QUADRANT_ELECTRICITY           = 0x0D,  /**< Four-Quadrant Electricity */
};


/** @brief Metering cluster server attributes */
enum zb_zcl_metering_attr_e
{
  /* Reading Information Attribute Set, table D.11 */
  ZB_ZCL_ATTR_METERING_CURRENT_SUMMATION_DELIVERED_ID = ZB_ZCL_ATTR_SET_WITH_ATTR_ID(ZB_ZCL_METERING_SET_READ_INFO, 0x00),   /**< @e CurrentSummationDelivered attribute
                                                                         * represents the most recent summed value of Energy, Gas, or Water delivered and consumed
                                                                         * in the premises. */
  ZB_ZCL_ATTR_METERING_CURRENT_SUMMATION_RECEIVED_ID,                   /**< @e CurrentSummationReceived attribute represents the most recent summed value of Energy,
                                                                         *   Gas, or Water generated and delivered from the premises. */
  ZB_ZCL_ATTR_METERING_CURRENT_MAX_DEMAND_DELIVERED_ID,                 /**< @e CurrentMaxDemandDelivered attribute represents the maximum demand or rate of delivered
                                                                         *   value of Energy, Gas, or Water being utilized at the premises. */
  ZB_ZCL_ATTR_METERING_CURRENT_MAX_DEMAND_RECEIVED_ID,                  /**< @e CurrentMaxDemandReceived attribute represents the maximum demand or rate of received
                                                                         *   value of Energy, Gas, or Water being utilized by the utility. */
  ZB_ZCL_ATTR_METERING_DFT_SUMMATION_ID,                                /**< @e DFTSummation attribute represents a snapshot of attribute CurrentSummationDelivered
                                                                         *   captured at the time indicated by attribute @ref ZB_ZCL_ATTR_METERING_DAILY_FREEZE_TIME_ID "DailyFreezeTime". */
  ZB_ZCL_ATTR_METERING_DAILY_FREEZE_TIME_ID,                            /**< @e DailyFreezeTime attribute represents the time of day when DFTSummation is captured
                                                                         *   (hour and minutes). */
  ZB_ZCL_ATTR_METERING_POWER_FACTOR_ID,                                 /**< @e PowerFactor attribute contains the Average Power Factor ratio in 1/100th. Valid
                                                                         *   values are 0 to 99. */
  ZB_ZCL_ATTR_METERING_READING_SNAPSHOT_TIME_ID,                        /**< @e ReadingSnapshotTime attribute represents the last time all of the
                                                                         *   @ref ZB_ZCL_ATTR_METERING_CURRENT_SUMMATION_DELIVERED_ID "CurrentSummationDelivered",
                                                                         *   @ref ZB_ZCL_ATTR_METERING_CURRENT_SUMMATION_RECEIVED_ID "CurrentSummationReceived",
                                                                         *   @ref ZB_ZCL_ATTR_METERING_CURRENT_MAX_DEMAND_DELIVERED_ID "CurrentMaxDemandDelivered" and
                                                                         *   @ref ZB_ZCL_ATTR_METERING_CURRENT_MAX_DEMAND_RECEIVED_ID "CurrentMaxDemandReceived"
                                                                         *   attributes that are supported by the device were updated. */
  ZB_ZCL_ATTR_METERING_CURRENT_MAX_DEMAND_DELIVERED_TIME_ID,            /**< @e CurrentMaxDemandDeliveredTime attribute represents the represents the time when
                                                                         *   @ref ZB_ZCL_ATTR_METERING_CURRENT_MAX_DEMAND_DELIVERED_ID "CurrentMaxDemandDelivered" reading
                                                                         *   was captured. */
  ZB_ZCL_ATTR_METERING_CURRENT_MAX_DEMAND_RECEIVED_TIME_ID,             /**< @e CurrentMaxDemandReceivedTime attribute represents the time when
                                                                         *   @ref ZB_ZCL_ATTR_METERING_CURRENT_MAX_DEMAND_RECEIVED_ID "CurrentMaxDemandReceived reading
                                                                         *   was captured. */
  ZB_ZCL_ATTR_METERING_DEFAULT_UPDATE_PERIOD_ID,                        /**< @e DefaultUpdatePeriod attribute represents the interval (seconds) at which the
                                                                         *   @ref ZB_ZCL_ATTR_METERING_INSTANTANEOUS_DEMAND_ID "InstantaneousDemand" attribute is
                                                                         *   updated when not in fast poll mode. */
  ZB_ZCL_ATTR_METERING_FAST_POLL_UPDATE_PERIOD_ID,                      /**< @e FastPollUpdatePeriod attribute represents the interval (seconds) at which the
                                                                         *   @ref ZB_ZCL_ATTR_METERING_INSTANTANEOUS_DEMAND_ID "InstantaneousDemand" attribute is
                                                                         *   updated when in fast poll mode. */
  ZB_ZCL_ATTR_METERING_CURRENT_BLOCK_PERIOD_CONSUMPTION_DELIVERED_ID,   /**< @e CurrentBlockPeriodConsumptionDelivered attribute represents the most recent summed
                                                                         *   value of Energy, Gas or Water delivered and consumed in the premises during the Block
                                                                         *   Tariff Period. */
  ZB_ZCL_ATTR_METERING_DAILY_CONSUMPTION_TARGET_ID,                     /**< @e DailyConsumptionTarget attribute is a daily target consumption amount that can be
                                                                         *   displayed to the consumer on a HAN device, with the intent that it can be used to compare
                                                                         *   to actual daily consumption. */
  ZB_ZCL_ATTR_METERING_CURRENT_BLOCK_ID,                                /**< @e CurrentBlock attribute is an 8-bit Enumeration which indicates the currently active
                                                                         *   block, when Block Tariffs are enabled. @see zb_zcl_metering_block_e */
  ZB_ZCL_ATTR_METERING_PROFILE_INTERVAL_PERIOD_ID,                      /**< @e ProfileIntervalPeriod attribute is currently included in the @e GetProfileResponse
                                                                         *   command payload, but does not appear in an attribute set. It represents the interval or
                                                                         *   time frame used to capture metered Energy, Gas, and Water consumption for profiling purposes. */
  /* 0x0010 attribute is deprecated */
  ZB_ZCL_ATTR_METERING_PRESET_READING_TIME_ID = ZB_ZCL_ATTR_SET_WITH_ATTR_ID(ZB_ZCL_METERING_SET_READ_INFO, 0x11),   /**< @e PresetReadingTime attribute */
  ZB_ZCL_ATTR_METERING_SUMMATION_DELIVERED_PER_REPORT_ID,               /**< @e SummationDeliveredPerReport attribute represents the summation increment per report
                                                                         *   from the water or gas meter. */
  ZB_ZCL_ATTR_METERING_FLOW_RESTRICTION_ID,                             /**< @e FlowRestriction attribute represents the volume per minute limit set in the flow
                                                                         *   restrictor. This applies to water but not for gas. A setting of 0xFF indicates this
                                                                         *   feature is disabled. */
  ZB_ZCL_ATTR_METERING_SUPPLY_STATUS_ID,                                /**< @e SupplyStatus attribute represents the state of the supply at the customer's premises.
                                                                         *   @see zb_zcl_metering_supply_status_e */
  ZB_ZCL_ATTR_METERING_CURRENT_INLET_ENERGY_CARRIER_SUMMATION_ID,          /**< @e CurrentInletEnergyCarrierSummation attribute is the current integrated volume of a
                                                                         *   given energy carrier measured on the inlet. */
  ZB_ZCL_ATTR_METERING_CURRENT_OUTLET_ENERGY_CARRIER_SUMMATION_ID,         /**< @e CurrentOutletEnergyCarrierSummation attribute is the current integrated volume of a
                                                                         *   given energy carrier measured on the outlet. */
  ZB_ZCL_ATTR_METERING_INLET_TEMPERATURE_ID,                            /**< @e InletTemperature attribute is the temperature measured on the energy carrier inlet. */
  ZB_ZCL_ATTR_METERING_OUTLET_TEMPERATURE_ID,                           /**< @e OutletTemperature attribute is the temperature measured on the energy carrier outlet. */
  ZB_ZCL_ATTR_METERING_CONTROL_TEMPERATURE_ID,                          /**< @e ControlTemperature attribute is a reference temperature measured on the meter used to
                                                                         *   validate the Inlet/Outlet temperatures. */
  ZB_ZCL_ATTR_METERING_CURRENT_INLET_ENERGY_CARRIER_DEMAND_ID,          /**< @e CurrentInletEnergyCarrierDemand attribute is the current absolute demand on the energy
                                                                         *   carrier inlet. */
  ZB_ZCL_ATTR_METERING_CURRENT_OUTLET_ENERGY_CARRIER_DEMAND_ID,         /**< @e CurrentOutletEnergyCarrierDemand attribute is the current absolute demand on the energy
                                                                         *   carrier outlet. */
  ZB_ZCL_ATTR_METERING_PREVIOUS_BLOCK_PERIOD_CONSUMPTION_DELIVERED_ID,  /**< @e PreviousBlockPeriodConsumptionDelivered attribute represents the total value of Energy,
                                                                         *   Gas or Water delivered and consumed in the premises at the end of the previous Block Tariff
                                                                         *   period. */
  ZB_ZCL_ATTR_METERING_CURRENT_BLOCK_PERIOD_CONSUMPTION_RECEIVED_ID,    /**< @e CurrentBlockPeriodConsumptionReceived attribute represents the most recent summed
                                                                         *   value of Energy, Gas or Water received by the energy supplier from the premises during
                                                                         *   the Block Tariff Period. */
  ZB_ZCL_ATTR_METERING_CURRENT_BLOCK_RECEIVED_ID,                       /**< @e CurrentBlockReceived attribute is an 8-bit Enumeration which indicates the currently
                                                                         *   active block, when Block Tariffs are enabled. */
  ZB_ZCL_ATTR_METERING_DFT_SUMMATION_RECEIVED_ID,                       /**< @e DFTSummationReceived attribute represents a snapshot of attribute
                                                                         *   @ref ZB_ZCL_ATTR_METERING_CURRENT_SUMMATION_RECEIVED_ID "CurrentSummationReceived"
                                                                         *   captured at the time indicated by the
                                                                         *   @ref ZB_ZCL_ATTR_METERING_DAILY_FREEZE_TIME_ID "DailyFreezeTime" attribute. */
  ZB_ZCL_ATTR_METERING_ACTIVE_REGISTER_TIER_DELIVERED_ID,               /**< @e ActiveRegisterTierDelivered attribute indicates the current register tier that the
                                                                         *   energy consumed is being accumulated against. @see SE 1.4 spec, table D.98. */
  ZB_ZCL_ATTR_METERING_ACTIVE_REGISTER_TIER_RECEIVED_ID,                /**< @e ActiveRegisterTierReceived attribute indicates the current register tier that the
                                                                         *   energy generated is being accumulated against. @see SE 1.4 spec, table D.101. */
  ZB_ZCL_ATTR_METERING_LAST_BLOCK_SWITCH_TIME_ID,                       /**< @e LastBlockSwitchTime attribute allows other devices to determine the time at which
                                                                         *   a meter switches from one block to another. */
  ZB_ZCL_ATTR_METERING_NUMBER_OF_TIERS_IN_USE,                          /**< @e NumberofTiersInUse */

  /* 0x0024 to 0x00FF reserved */


  /* TOU Information Attribute Set, table D.14 */
  ZB_ZCL_ATTR_METERING_CURRENT_TIER1_SUMMATION_DELIVERED_ID = ZB_ZCL_ATTR_SET_WITH_ATTR_ID(ZB_ZCL_METERING_SET_TOU_INFORMATION, 0x00),    /**< @e CurrentTierNSummationDelivered
                                                                  * attributes represent the most recent summed value of Energy, Gas, or Water delivered to the premises at a
                                                                  * specific price tier as defined by a TOU schedule or a real time pricing period. */
  ZB_ZCL_ATTR_METERING_CURRENT_TIER1_SUMMATION_RECEIVED_ID,      /**< @e CurrentTierNSummationReceived attributes represent the most recent summed value of Energy, Gas, or Water
                                                                  *   provided by the premises (i.e. received by the utility from the customer) at a specific price tier as defined
                                                                  *   by a TOU schedule or a real time pricing period. */
  ZB_ZCL_ATTR_METERING_CURRENT_TIER2_SUMMATION_DELIVERED_ID,     /**< @copydoc ZB_ZCL_ATTR_METERING_CURRENT_TIER1_SUMMATION_DELIVERED_ID */
  ZB_ZCL_ATTR_METERING_CURRENT_TIER2_SUMMATION_RECEIVED_ID,      /**< @copydoc ZB_ZCL_ATTR_METERING_CURRENT_TIER1_SUMMATION_RECEIVED_ID */
  ZB_ZCL_ATTR_METERING_CURRENT_TIER3_SUMMATION_DELIVERED_ID,     /**< @copydoc ZB_ZCL_ATTR_METERING_CURRENT_TIER1_SUMMATION_DELIVERED_ID */
  ZB_ZCL_ATTR_METERING_CURRENT_TIER3_SUMMATION_RECEIVED_ID,      /**< @copydoc ZB_ZCL_ATTR_METERING_CURRENT_TIER1_SUMMATION_RECEIVED_ID */
  ZB_ZCL_ATTR_METERING_CURRENT_TIER4_SUMMATION_DELIVERED_ID,     /**< @copydoc ZB_ZCL_ATTR_METERING_CURRENT_TIER1_SUMMATION_DELIVERED_ID */
  ZB_ZCL_ATTR_METERING_CURRENT_TIER4_SUMMATION_RECEIVED_ID,      /**< @copydoc ZB_ZCL_ATTR_METERING_CURRENT_TIER1_SUMMATION_RECEIVED_ID */
  ZB_ZCL_ATTR_METERING_CURRENT_TIER5_SUMMATION_DELIVERED_ID,     /**< @copydoc ZB_ZCL_ATTR_METERING_CURRENT_TIER1_SUMMATION_DELIVERED_ID */
  ZB_ZCL_ATTR_METERING_CURRENT_TIER5_SUMMATION_RECEIVED_ID,      /**< @copydoc ZB_ZCL_ATTR_METERING_CURRENT_TIER1_SUMMATION_RECEIVED_ID */
  ZB_ZCL_ATTR_METERING_CURRENT_TIER6_SUMMATION_DELIVERED_ID,     /**< @copydoc ZB_ZCL_ATTR_METERING_CURRENT_TIER1_SUMMATION_DELIVERED_ID */
  ZB_ZCL_ATTR_METERING_CURRENT_TIER6_SUMMATION_RECEIVED_ID,      /**< @copydoc ZB_ZCL_ATTR_METERING_CURRENT_TIER1_SUMMATION_RECEIVED_ID */
  ZB_ZCL_ATTR_METERING_CURRENT_TIER7_SUMMATION_DELIVERED_ID,     /**< @copydoc ZB_ZCL_ATTR_METERING_CURRENT_TIER1_SUMMATION_DELIVERED_ID */
  ZB_ZCL_ATTR_METERING_CURRENT_TIER7_SUMMATION_RECEIVED_ID,      /**< @copydoc ZB_ZCL_ATTR_METERING_CURRENT_TIER1_SUMMATION_RECEIVED_ID */
  ZB_ZCL_ATTR_METERING_CURRENT_TIER8_SUMMATION_DELIVERED_ID,     /**< @copydoc ZB_ZCL_ATTR_METERING_CURRENT_TIER1_SUMMATION_DELIVERED_ID */
  ZB_ZCL_ATTR_METERING_CURRENT_TIER8_SUMMATION_RECEIVED_ID,      /**< @copydoc ZB_ZCL_ATTR_METERING_CURRENT_TIER1_SUMMATION_RECEIVED_ID */
  ZB_ZCL_ATTR_METERING_CURRENT_TIER9_SUMMATION_DELIVERED_ID,     /**< @copydoc ZB_ZCL_ATTR_METERING_CURRENT_TIER1_SUMMATION_DELIVERED_ID */
  ZB_ZCL_ATTR_METERING_CURRENT_TIER9_SUMMATION_RECEIVED_ID,      /**< @copydoc ZB_ZCL_ATTR_METERING_CURRENT_TIER1_SUMMATION_RECEIVED_ID */
  ZB_ZCL_ATTR_METERING_CURRENT_TIER10_SUMMATION_DELIVERED_ID,    /**< @copydoc ZB_ZCL_ATTR_METERING_CURRENT_TIER1_SUMMATION_DELIVERED_ID */
  ZB_ZCL_ATTR_METERING_CURRENT_TIER10_SUMMATION_RECEIVED_ID,     /**< @copydoc ZB_ZCL_ATTR_METERING_CURRENT_TIER1_SUMMATION_RECEIVED_ID */
  ZB_ZCL_ATTR_METERING_CURRENT_TIER11_SUMMATION_DELIVERED_ID,    /**< @copydoc ZB_ZCL_ATTR_METERING_CURRENT_TIER1_SUMMATION_DELIVERED_ID */
  ZB_ZCL_ATTR_METERING_CURRENT_TIER11_SUMMATION_RECEIVED_ID,     /**< @copydoc ZB_ZCL_ATTR_METERING_CURRENT_TIER1_SUMMATION_RECEIVED_ID */
  ZB_ZCL_ATTR_METERING_CURRENT_TIER12_SUMMATION_DELIVERED_ID,    /**< @copydoc ZB_ZCL_ATTR_METERING_CURRENT_TIER1_SUMMATION_DELIVERED_ID */
  ZB_ZCL_ATTR_METERING_CURRENT_TIER12_SUMMATION_RECEIVED_ID,     /**< @copydoc ZB_ZCL_ATTR_METERING_CURRENT_TIER1_SUMMATION_RECEIVED_ID */
  ZB_ZCL_ATTR_METERING_CURRENT_TIER13_SUMMATION_DELIVERED_ID,    /**< @copydoc ZB_ZCL_ATTR_METERING_CURRENT_TIER1_SUMMATION_DELIVERED_ID */
  ZB_ZCL_ATTR_METERING_CURRENT_TIER13_SUMMATION_RECEIVED_ID,     /**< @copydoc ZB_ZCL_ATTR_METERING_CURRENT_TIER1_SUMMATION_RECEIVED_ID */
  ZB_ZCL_ATTR_METERING_CURRENT_TIER14_SUMMATION_DELIVERED_ID,    /**< @copydoc ZB_ZCL_ATTR_METERING_CURRENT_TIER1_SUMMATION_DELIVERED_ID */
  ZB_ZCL_ATTR_METERING_CURRENT_TIER14_SUMMATION_RECEIVED_ID,     /**< @copydoc ZB_ZCL_ATTR_METERING_CURRENT_TIER1_SUMMATION_RECEIVED_ID */
  ZB_ZCL_ATTR_METERING_CURRENT_TIER15_SUMMATION_DELIVERED_ID,    /**< @copydoc ZB_ZCL_ATTR_METERING_CURRENT_TIER1_SUMMATION_DELIVERED_ID */
  ZB_ZCL_ATTR_METERING_CURRENT_TIER15_SUMMATION_RECEIVED_ID,     /**< @copydoc ZB_ZCL_ATTR_METERING_CURRENT_TIER1_SUMMATION_RECEIVED_ID */
  ZB_ZCL_ATTR_METERING_CURRENT_TIER16_SUMMATION_DELIVERED_ID,    /**< @copydoc ZB_ZCL_ATTR_METERING_CURRENT_TIER1_SUMMATION_DELIVERED_ID */
  ZB_ZCL_ATTR_METERING_CURRENT_TIER16_SUMMATION_RECEIVED_ID,     /**< @copydoc ZB_ZCL_ATTR_METERING_CURRENT_TIER1_SUMMATION_RECEIVED_ID */
  ZB_ZCL_ATTR_METERING_CURRENT_TIER17_SUMMATION_DELIVERED_ID,    /**< @copydoc ZB_ZCL_ATTR_METERING_CURRENT_TIER1_SUMMATION_DELIVERED_ID */
  ZB_ZCL_ATTR_METERING_CURRENT_TIER17_SUMMATION_RECEIVED_ID,     /**< @copydoc ZB_ZCL_ATTR_METERING_CURRENT_TIER1_SUMMATION_RECEIVED_ID */
  ZB_ZCL_ATTR_METERING_CURRENT_TIER18_SUMMATION_DELIVERED_ID,    /**< @copydoc ZB_ZCL_ATTR_METERING_CURRENT_TIER1_SUMMATION_DELIVERED_ID */
  ZB_ZCL_ATTR_METERING_CURRENT_TIER18_SUMMATION_RECEIVED_ID,     /**< @copydoc ZB_ZCL_ATTR_METERING_CURRENT_TIER1_SUMMATION_RECEIVED_ID */
  ZB_ZCL_ATTR_METERING_CURRENT_TIER19_SUMMATION_DELIVERED_ID,    /**< @copydoc ZB_ZCL_ATTR_METERING_CURRENT_TIER1_SUMMATION_DELIVERED_ID */
  ZB_ZCL_ATTR_METERING_CURRENT_TIER19_SUMMATION_RECEIVED_ID,     /**< @copydoc ZB_ZCL_ATTR_METERING_CURRENT_TIER1_SUMMATION_RECEIVED_ID */
  ZB_ZCL_ATTR_METERING_CURRENT_TIER20_SUMMATION_DELIVERED_ID,    /**< @copydoc ZB_ZCL_ATTR_METERING_CURRENT_TIER1_SUMMATION_DELIVERED_ID */
  ZB_ZCL_ATTR_METERING_CURRENT_TIER20_SUMMATION_RECEIVED_ID,     /**< @copydoc ZB_ZCL_ATTR_METERING_CURRENT_TIER1_SUMMATION_RECEIVED_ID */
  ZB_ZCL_ATTR_METERING_CURRENT_TIER21_SUMMATION_DELIVERED_ID,    /**< @copydoc ZB_ZCL_ATTR_METERING_CURRENT_TIER1_SUMMATION_DELIVERED_ID */
  ZB_ZCL_ATTR_METERING_CURRENT_TIER21_SUMMATION_RECEIVED_ID,     /**< @copydoc ZB_ZCL_ATTR_METERING_CURRENT_TIER1_SUMMATION_RECEIVED_ID */
  ZB_ZCL_ATTR_METERING_CURRENT_TIER22_SUMMATION_DELIVERED_ID,    /**< @copydoc ZB_ZCL_ATTR_METERING_CURRENT_TIER1_SUMMATION_DELIVERED_ID */
  ZB_ZCL_ATTR_METERING_CURRENT_TIER22_SUMMATION_RECEIVED_ID,     /**< @copydoc ZB_ZCL_ATTR_METERING_CURRENT_TIER1_SUMMATION_RECEIVED_ID */
  ZB_ZCL_ATTR_METERING_CURRENT_TIER23_SUMMATION_DELIVERED_ID,    /**< @copydoc ZB_ZCL_ATTR_METERING_CURRENT_TIER1_SUMMATION_DELIVERED_ID */
  ZB_ZCL_ATTR_METERING_CURRENT_TIER23_SUMMATION_RECEIVED_ID,     /**< @copydoc ZB_ZCL_ATTR_METERING_CURRENT_TIER1_SUMMATION_RECEIVED_ID */
  ZB_ZCL_ATTR_METERING_CURRENT_TIER24_SUMMATION_DELIVERED_ID,    /**< @copydoc ZB_ZCL_ATTR_METERING_CURRENT_TIER1_SUMMATION_DELIVERED_ID */
  ZB_ZCL_ATTR_METERING_CURRENT_TIER24_SUMMATION_RECEIVED_ID,     /**< @copydoc ZB_ZCL_ATTR_METERING_CURRENT_TIER1_SUMMATION_RECEIVED_ID */
  ZB_ZCL_ATTR_METERING_CURRENT_TIER25_SUMMATION_DELIVERED_ID,    /**< @copydoc ZB_ZCL_ATTR_METERING_CURRENT_TIER1_SUMMATION_DELIVERED_ID */
  ZB_ZCL_ATTR_METERING_CURRENT_TIER25_SUMMATION_RECEIVED_ID,     /**< @copydoc ZB_ZCL_ATTR_METERING_CURRENT_TIER1_SUMMATION_RECEIVED_ID */
  ZB_ZCL_ATTR_METERING_CURRENT_TIER26_SUMMATION_DELIVERED_ID,    /**< @copydoc ZB_ZCL_ATTR_METERING_CURRENT_TIER1_SUMMATION_DELIVERED_ID */
  ZB_ZCL_ATTR_METERING_CURRENT_TIER26_SUMMATION_RECEIVED_ID,     /**< @copydoc ZB_ZCL_ATTR_METERING_CURRENT_TIER1_SUMMATION_RECEIVED_ID */
  ZB_ZCL_ATTR_METERING_CURRENT_TIER27_SUMMATION_DELIVERED_ID,    /**< @copydoc ZB_ZCL_ATTR_METERING_CURRENT_TIER1_SUMMATION_DELIVERED_ID */
  ZB_ZCL_ATTR_METERING_CURRENT_TIER27_SUMMATION_RECEIVED_ID,     /**< @copydoc ZB_ZCL_ATTR_METERING_CURRENT_TIER1_SUMMATION_RECEIVED_ID */
  ZB_ZCL_ATTR_METERING_CURRENT_TIER28_SUMMATION_DELIVERED_ID,    /**< @copydoc ZB_ZCL_ATTR_METERING_CURRENT_TIER1_SUMMATION_DELIVERED_ID */
  ZB_ZCL_ATTR_METERING_CURRENT_TIER28_SUMMATION_RECEIVED_ID,     /**< @copydoc ZB_ZCL_ATTR_METERING_CURRENT_TIER1_SUMMATION_RECEIVED_ID */
  ZB_ZCL_ATTR_METERING_CURRENT_TIER29_SUMMATION_DELIVERED_ID,    /**< @copydoc ZB_ZCL_ATTR_METERING_CURRENT_TIER1_SUMMATION_DELIVERED_ID */
  ZB_ZCL_ATTR_METERING_CURRENT_TIER29_SUMMATION_RECEIVED_ID,     /**< @copydoc ZB_ZCL_ATTR_METERING_CURRENT_TIER1_SUMMATION_RECEIVED_ID */
  ZB_ZCL_ATTR_METERING_CURRENT_TIER30_SUMMATION_DELIVERED_ID,    /**< @copydoc ZB_ZCL_ATTR_METERING_CURRENT_TIER1_SUMMATION_DELIVERED_ID */
  ZB_ZCL_ATTR_METERING_CURRENT_TIER30_SUMMATION_RECEIVED_ID,     /**< @copydoc ZB_ZCL_ATTR_METERING_CURRENT_TIER1_SUMMATION_RECEIVED_ID */
  ZB_ZCL_ATTR_METERING_CURRENT_TIER31_SUMMATION_DELIVERED_ID,    /**< @copydoc ZB_ZCL_ATTR_METERING_CURRENT_TIER1_SUMMATION_DELIVERED_ID */
  ZB_ZCL_ATTR_METERING_CURRENT_TIER31_SUMMATION_RECEIVED_ID,     /**< @copydoc ZB_ZCL_ATTR_METERING_CURRENT_TIER1_SUMMATION_RECEIVED_ID */
  ZB_ZCL_ATTR_METERING_CURRENT_TIER32_SUMMATION_DELIVERED_ID,    /**< @copydoc ZB_ZCL_ATTR_METERING_CURRENT_TIER1_SUMMATION_DELIVERED_ID */
  ZB_ZCL_ATTR_METERING_CURRENT_TIER32_SUMMATION_RECEIVED_ID,     /**< @copydoc ZB_ZCL_ATTR_METERING_CURRENT_TIER1_SUMMATION_RECEIVED_ID */
  ZB_ZCL_ATTR_METERING_CURRENT_TIER33_SUMMATION_DELIVERED_ID,    /**< @copydoc ZB_ZCL_ATTR_METERING_CURRENT_TIER1_SUMMATION_DELIVERED_ID */
  ZB_ZCL_ATTR_METERING_CURRENT_TIER33_SUMMATION_RECEIVED_ID,     /**< @copydoc ZB_ZCL_ATTR_METERING_CURRENT_TIER1_SUMMATION_RECEIVED_ID */
  ZB_ZCL_ATTR_METERING_CURRENT_TIER34_SUMMATION_DELIVERED_ID,    /**< @copydoc ZB_ZCL_ATTR_METERING_CURRENT_TIER1_SUMMATION_DELIVERED_ID */
  ZB_ZCL_ATTR_METERING_CURRENT_TIER34_SUMMATION_RECEIVED_ID,     /**< @copydoc ZB_ZCL_ATTR_METERING_CURRENT_TIER1_SUMMATION_RECEIVED_ID */
  ZB_ZCL_ATTR_METERING_CURRENT_TIER35_SUMMATION_DELIVERED_ID,    /**< @copydoc ZB_ZCL_ATTR_METERING_CURRENT_TIER1_SUMMATION_DELIVERED_ID */
  ZB_ZCL_ATTR_METERING_CURRENT_TIER35_SUMMATION_RECEIVED_ID,     /**< @copydoc ZB_ZCL_ATTR_METERING_CURRENT_TIER1_SUMMATION_RECEIVED_ID */
  ZB_ZCL_ATTR_METERING_CURRENT_TIER36_SUMMATION_DELIVERED_ID,    /**< @copydoc ZB_ZCL_ATTR_METERING_CURRENT_TIER1_SUMMATION_DELIVERED_ID */
  ZB_ZCL_ATTR_METERING_CURRENT_TIER36_SUMMATION_RECEIVED_ID,     /**< @copydoc ZB_ZCL_ATTR_METERING_CURRENT_TIER1_SUMMATION_RECEIVED_ID */
  ZB_ZCL_ATTR_METERING_CURRENT_TIER37_SUMMATION_DELIVERED_ID,    /**< @copydoc ZB_ZCL_ATTR_METERING_CURRENT_TIER1_SUMMATION_DELIVERED_ID */
  ZB_ZCL_ATTR_METERING_CURRENT_TIER37_SUMMATION_RECEIVED_ID,     /**< @copydoc ZB_ZCL_ATTR_METERING_CURRENT_TIER1_SUMMATION_RECEIVED_ID */
  ZB_ZCL_ATTR_METERING_CURRENT_TIER38_SUMMATION_DELIVERED_ID,    /**< @copydoc ZB_ZCL_ATTR_METERING_CURRENT_TIER1_SUMMATION_DELIVERED_ID */
  ZB_ZCL_ATTR_METERING_CURRENT_TIER38_SUMMATION_RECEIVED_ID,     /**< @copydoc ZB_ZCL_ATTR_METERING_CURRENT_TIER1_SUMMATION_RECEIVED_ID */
  ZB_ZCL_ATTR_METERING_CURRENT_TIER39_SUMMATION_DELIVERED_ID,    /**< @copydoc ZB_ZCL_ATTR_METERING_CURRENT_TIER1_SUMMATION_DELIVERED_ID */
  ZB_ZCL_ATTR_METERING_CURRENT_TIER39_SUMMATION_RECEIVED_ID,     /**< @copydoc ZB_ZCL_ATTR_METERING_CURRENT_TIER1_SUMMATION_RECEIVED_ID */
  ZB_ZCL_ATTR_METERING_CURRENT_TIER40_SUMMATION_DELIVERED_ID,    /**< @copydoc ZB_ZCL_ATTR_METERING_CURRENT_TIER1_SUMMATION_DELIVERED_ID */
  ZB_ZCL_ATTR_METERING_CURRENT_TIER40_SUMMATION_RECEIVED_ID,     /**< @copydoc ZB_ZCL_ATTR_METERING_CURRENT_TIER1_SUMMATION_RECEIVED_ID */
  ZB_ZCL_ATTR_METERING_CURRENT_TIER41_SUMMATION_DELIVERED_ID,    /**< @copydoc ZB_ZCL_ATTR_METERING_CURRENT_TIER1_SUMMATION_DELIVERED_ID */
  ZB_ZCL_ATTR_METERING_CURRENT_TIER41_SUMMATION_RECEIVED_ID,     /**< @copydoc ZB_ZCL_ATTR_METERING_CURRENT_TIER1_SUMMATION_RECEIVED_ID */
  ZB_ZCL_ATTR_METERING_CURRENT_TIER42_SUMMATION_DELIVERED_ID,    /**< @copydoc ZB_ZCL_ATTR_METERING_CURRENT_TIER1_SUMMATION_DELIVERED_ID */
  ZB_ZCL_ATTR_METERING_CURRENT_TIER42_SUMMATION_RECEIVED_ID,     /**< @copydoc ZB_ZCL_ATTR_METERING_CURRENT_TIER1_SUMMATION_RECEIVED_ID */
  ZB_ZCL_ATTR_METERING_CURRENT_TIER43_SUMMATION_DELIVERED_ID,    /**< @copydoc ZB_ZCL_ATTR_METERING_CURRENT_TIER1_SUMMATION_DELIVERED_ID */
  ZB_ZCL_ATTR_METERING_CURRENT_TIER43_SUMMATION_RECEIVED_ID,     /**< @copydoc ZB_ZCL_ATTR_METERING_CURRENT_TIER1_SUMMATION_RECEIVED_ID */
  ZB_ZCL_ATTR_METERING_CURRENT_TIER44_SUMMATION_DELIVERED_ID,    /**< @copydoc ZB_ZCL_ATTR_METERING_CURRENT_TIER1_SUMMATION_DELIVERED_ID */
  ZB_ZCL_ATTR_METERING_CURRENT_TIER44_SUMMATION_RECEIVED_ID,     /**< @copydoc ZB_ZCL_ATTR_METERING_CURRENT_TIER1_SUMMATION_RECEIVED_ID */
  ZB_ZCL_ATTR_METERING_CURRENT_TIER45_SUMMATION_DELIVERED_ID,    /**< @copydoc ZB_ZCL_ATTR_METERING_CURRENT_TIER1_SUMMATION_DELIVERED_ID */
  ZB_ZCL_ATTR_METERING_CURRENT_TIER45_SUMMATION_RECEIVED_ID,     /**< @copydoc ZB_ZCL_ATTR_METERING_CURRENT_TIER1_SUMMATION_RECEIVED_ID */
  ZB_ZCL_ATTR_METERING_CURRENT_TIER46_SUMMATION_DELIVERED_ID,    /**< @copydoc ZB_ZCL_ATTR_METERING_CURRENT_TIER1_SUMMATION_DELIVERED_ID */
  ZB_ZCL_ATTR_METERING_CURRENT_TIER46_SUMMATION_RECEIVED_ID,     /**< @copydoc ZB_ZCL_ATTR_METERING_CURRENT_TIER1_SUMMATION_RECEIVED_ID */
  ZB_ZCL_ATTR_METERING_CURRENT_TIER47_SUMMATION_DELIVERED_ID,    /**< @copydoc ZB_ZCL_ATTR_METERING_CURRENT_TIER1_SUMMATION_DELIVERED_ID */
  ZB_ZCL_ATTR_METERING_CURRENT_TIER47_SUMMATION_RECEIVED_ID,     /**< @copydoc ZB_ZCL_ATTR_METERING_CURRENT_TIER1_SUMMATION_RECEIVED_ID */
  ZB_ZCL_ATTR_METERING_CURRENT_TIER48_SUMMATION_DELIVERED_ID,    /**< @copydoc ZB_ZCL_ATTR_METERING_CURRENT_TIER1_SUMMATION_DELIVERED_ID */
  ZB_ZCL_ATTR_METERING_CURRENT_TIER48_SUMMATION_RECEIVED_ID,     /**< @copydoc ZB_ZCL_ATTR_METERING_CURRENT_TIER1_SUMMATION_RECEIVED_ID */
  /* 0x0160 to 0x01FB reserved */
  ZB_ZCL_ATTR_METERING_CPP1_SUMMATION_DELIVERED_ID = ZB_ZCL_ATTR_SET_WITH_ATTR_ID(ZB_ZCL_METERING_SET_TOU_INFORMATION, 0xFC),   /**< @e CPP1SummationDelivered attribute
                                                                 *  represents the most recent summed value of Energy, Gas, or Water delivered to the premises (i.e.
                                                                 *  delivered to the customer from the utility) while Critical Peak Price CPP1 was being applied. */
  /* 0x01FD reserved */
  ZB_ZCL_ATTR_METERING_CPP2_SUMMATION_DELIVERED_ID = ZB_ZCL_ATTR_SET_WITH_ATTR_ID(ZB_ZCL_METERING_SET_TOU_INFORMATION, 0xFE),   /**< @e CPP2SummationDelivered attribute
                                                                 *  represents the most recent summed value of Energy, Gas, or Water delivered to the premises (i.e.
                                                                 *  delivered to the customer from the utility) while Critical Peak Price CPP2 was being applied. */
  /* 0x01FF reserved */


  /* Meter Status Attribute Set, table D.15. */
  ZB_ZCL_ATTR_METERING_STATUS_ID = ZB_ZCL_ATTR_SET_WITH_ATTR_ID(ZB_ZCL_METERING_SET_METER_STATUS, 0x00),  /**< @e Status attribute provides indicators reflecting the current
                                                           * error conditions found by the metering device. Status depends on the device type.
                                                           * @see zb_zcl_metering_status_electricity_e
                                                           * @see zb_zcl_metering_status_gas_e
                                                           * @see zb_zcl_metering_status_water_e
                                                           * @see zb_zcl_metering_status_heat_cooling_e */
  ZB_ZCL_ATTR_METERING_REMAINING_BATTERY_LIFE_ID,         /**< @e RemainingBatteryLife attribute represents the estimated remaining life of the battery in % of capacity.
                                                           *   A setting of 0xFF indicates this feature is disabled. */
  ZB_ZCL_ATTR_METERING_HOURS_IN_OPERATION_ID,             /**< @e HoursInOperation attribute is a counter that increments once every hour during operation. */
  ZB_ZCL_ATTR_METERING_HOURS_IN_FAULT_ID,                 /**< @e HoursInFault attribute is a counter that increments once every hour when the device is in operation
                                                           *   with a fault detected. */
  ZB_ZCL_ATTR_METERING_EXTENDED_STATUS_ID,                /**< @e ExtendedStatus attribute reflects the state of items in a meter that the standard Status attribute
                                                           *   cannot show. The Extended Status BitMap is split into two groups of flags: general flags and metering
                                                           *   type specific flags.
                                                           *   @see zb_zcl_metering_extstatus_general_e
                                                           *   @see zb_zcl_metering_extstatus_electricity_e
                                                           *   @see zb_zcl_metering_extstatus_gas_e */
  ZB_ZCL_ATTR_METERING_REMAINING_BATTERY_LIFE_DAYS_ID,    /**< @e RemainingBatteryLifeInDays attribute represents the estimated remaining life of the battery in days
                                                           *   of capacity. The range is 0 - 0xFFFE, where 0xFFFF represents 'Invalid', 'Unused' and 'Disabled'. */
  ZB_ZCL_ATTR_METERING_CURRENT_METER_ID_ID,               /**< @e CurrentMeterID attribute is the current id for the Meter. This could be the current firmware
                                                           *   version supported on the meter. */
  ZB_ZCL_ATTR_METERING_AMBIENT_CONSUMPTION_INDICATOR_ID,  /**< @e AmbientConsumptionIndicator attribute is an 8-bit enumeration which provides a simple indication
                                                           *   (Low/Medium/High) of the amount of a commodity being consumed within the premises.
                                                           *   @see @ref zb_zcl_metering_low_medium_high_status_e. */
  ZB_ZCL_ATTR_METERING_SERVICE_DISCONNECT_REASON_ID,      /**< The Service Disconnect Reason attribute is an 8-bit enumeration which indicates why the supply
                                                           *   to the premises has been disconnected */
  ZB_ZCL_ATTR_METERING_LINKY_MODE_OF_OPERATION_ID,        /**< The @e LinkyModeOfOperation attribute is specific to Linky devices. It consists of a single flag
                                                           * (bit 0) which shall be set to FALSE (0) when in 'Simple' Mode and set to TRUE (1) when in
                                                           * Advanced Mode. Bits 1 to 7 are reserved. */
  /* 0x020A to 0x02FF reserved */


  /* Formatting Attribute Set, table D.25 */
  ZB_ZCL_ATTR_METERING_UNIT_OF_MEASURE_ID = ZB_ZCL_ATTR_SET_WITH_ATTR_ID(ZB_ZCL_METERING_SET_FORMATTING, 0x00),    /**< @e UnitOfMeasure attribute provides a label for
                                                                   *   the Energy, Gas, or Water being measured by the metering device.
                                                                   *   @see zb_zcl_metering_unit_of_measure_e */
  ZB_ZCL_ATTR_METERING_MULTIPLIER_ID,                             /**< @e Multiplier attribute provides a value to be multiplied against a raw or uncompensated sensor
                                                                   *   count of Energy, Gas, or Water being measured by the metering device. */
  ZB_ZCL_ATTR_METERING_DIVISOR_ID,                                /**< @e Divisor attribute provides a value to divide the results of applying the
                                                                   *   @ref ZB_ZCL_ATTR_METERING_MULTIPLIER_ID "Multiplier Attribute" against a raw or uncompensated
                                                                   *   sensor count of Energy, Gas, or Water being measured by the metering device. */
  ZB_ZCL_ATTR_METERING_SUMMATION_FORMATTING_ID,                   /**< @e SummationFormatting attribute provides a method to properly decipher the number of digits
                                                                   *   and the decimal location of the values found in the Summation Information Set of attributes. */
  ZB_ZCL_ATTR_METERING_DEMAND_FORMATTING_ID,                      /**< @e DemandFormatting attribute provides a method to properly decipher the number of digits and
                                                                   *   the decimal location of the values found in the Demand-related attributes. */
  ZB_ZCL_ATTR_METERING_HISTORICAL_CONSUMPTION_FORMATTING_ID,      /**< @e HistoricalConsumptionFormatting attribute provides a method to properly decipher the number
                                                                   *   of digits and the decimal location of the values found in the Historical Consumption Set of
                                                                   *   attributes. */
  ZB_ZCL_ATTR_METERING_METERING_DEVICE_TYPE_ID,                   /**< @e MeteringDeviceType attribute provides a label for identifying the type of metering device
                                                                   *   present (Energy, Gas, Water, Thermal, Heat, Cooling, and mirrored metering devices).
                                                                   *   @see zb_zcl_metering_device_type_e */
  ZB_ZCL_ATTR_METERING_SITE_ID_ID,                                /**< @e SiteID attribute is a text string, known in the UK as the MPAN number for electricity,
                                                                   *   MPRN for gas and 'Stand Point' in South Africa. */
  ZB_ZCL_ATTR_METERING_METER_SERIAL_NUMBER_ID,                    /**< @e MeterSerialNumber attribute is used to provide a unique identification of the metering
                                                                   *   device. */
  ZB_ZCL_ATTR_METERING_ENERGY_CARRIER_UNIT_OF_MEASURE_ID,         /**< @e EnergyCarrierUnitOfMeasure attribute specifies the unit of measure that the EnergyCarrier
                                                                   *   is measured in. */
  ZB_ZCL_ATTR_METERING_ENERGY_CARRIER_SUMMATION_FORMATTING_ID,    /**< @e EnergyCarrierSummationFormatting attribute provides a method to properly decipher the number
                                                                   *   of digits and the decimal location of the values found in the Summation- related attributes. */
  ZB_ZCL_ATTR_METERING_ENERGY_CARRIER_DEMAND_FORMATTING_ID,       /**< @e EnergyCarrierDemandFormatting attribute provides a method to properly decipher the number
                                                                   *   of digits and the decimal location of the values found in the Demand- related attributes. */
  ZB_ZCL_ATTR_METERING_TEMPERATURE_UNIT_OF_MEASURE_ID,            /**< @e TemperatureUnitOfMeasure attribute specifies the unit of measure that temperatures are
                                                                   *   measured in.
                                                                   *   @see zb_zcl_metering_temperature_unit_of_measure_e */
  ZB_ZCL_ATTR_METERING_TEMPERATURE_FORMATTING_ID,                 /**< @e TemperatureFormatting attribute provides a method to properly decipher the number of digits
                                                                   *   and the decimal location of the values found in the Temperature-related attributes. */
  ZB_ZCL_ATTR_METERING_MODULE_SERIAL_NUMBER_ID,                   /**< @e ModuleSerialNumber attribute represents the serial number (unique identifier) of the
                                                                   *   meter module. */
  ZB_ZCL_ATTR_METERING_OPERATING_TARIFF_LABEL_DELIVERED_ID,       /**< @e OperatingTariffLabelDelivered attribute is the meter's version of the @e TariffLabel attribute
                                                                   *   that is found within the Tariff Information attribute set of the Price Cluster. */
  ZB_ZCL_ATTR_METERING_OPERATING_TARIFF_LABEL_RECEIVED_ID,        /**< @e OperatingTariffLabelReceived attribute is the meter's version of the @e ReceivedTariffLabel
                                                                   *   attribute that is found within the Tariff Information attribute set of the Price Cluster. */
  ZB_ZCL_ATTR_METERING_CUSTOMER_ID_NUMBER_ID,                    /**< @e CustomerIDNumber attribute provides a customer identification which may be used to confirm
                                                                   *   the customer at the premises. */
  ZB_ZCL_ATTR_METERING_ALTERNATIVE_UNIT_OF_MEASURE_ID,            /**< @e AlternativeUnitOfMeasure attribute provides a base for the attributes in the Alternative
                                                                   *   Historical Consumption attribute set. */
  ZB_ZCL_ATTR_METERING_ALTERNATIVE_DEMAND_FORMATTING_ID,          /**< @e AlternativeDemandFormatting attribute provides a method to properly decipher the number of
                                                                   *   digits and the decimal location of the values found in the Alternative Demand-related attributes. */
  ZB_ZCL_ATTR_METERING_ALTERNATIVE_CONSUMPTION_FORMATTING_ID,     /**< @e AlternativeConsumptionFormatting attribute provides a method to properly decipher the
                                                                   *   number of digits and the decimal location of the consumption values found in the Alternative
                                                                   *   Historical Consumption Set of attributes. */
  /* 0x0313 to 0x03FF reserved */


  /* Historical Consumption Attribute Set, table D.29 */
  ZB_ZCL_ATTR_METERING_INSTANTANEOUS_DEMAND_ID = ZB_ZCL_ATTR_SET_WITH_ATTR_ID(ZB_ZCL_METERING_SET_HISTORICAL_CONSUMPTION, 0x00),   /**< @e InstantaneousDemand attribute
                                                                             * represents the current Demand of Energy, Gas, or Water delivered or received at the premises.*/
  ZB_ZCL_ATTR_METERING_CURRENT_DAY_CONSUMPTION_DELIVERED_ID,                        /**< @e CurrentDayConsumptionDelivered attribute represents the summed value of Energy,
                                                                             *   Gas, or Water delivered to the premises since the Historical Freeze Time (HFT).*/
  ZB_ZCL_ATTR_METERING_CURRENT_DAY_CONSUMPTION_RECEIVED_ID,                        /**< @e CurrentDayConsumptionReceived attribute represents the summed value of Energy,
                                                                             *   Gas, or Water received from the premises since the Historical Freeze Time (HFT). */
  ZB_ZCL_ATTR_METERING_PREVIOUS_DAY_CONSUMPTION_DELIVERED_ID,                        /**< @e PreviousDayConsumptionDelivered attribute represents the summed value of Energy,
                                                                             *   Gas, or Water delivered to the premises within the previous 24 hour period starting
                                                                             *   at the Historical Freeze Time (HFT). */
  ZB_ZCL_ATTR_METERING_PREVIOUS_DAY_CONSUMPTION_RECEIVED_ID,                        /**< @e PreviousDayConsumptionReceived attribute represents the summed value of Energy,
                                                                             *   Gas, or Water received from the premises within the previous 24 hour period starting
                                                                             *   at the Historical Freeze Time (HFT). */
  ZB_ZCL_ATTR_METERING_CURRENT_PARTIAL_PROFILE_INTERVAL_START_TIME_DELIVERED_ID,    /**< @e CurrentPartialProfileIntervalStartTimeDelivered attribute represents the start
                                                                             *   time of the current Load Profile interval being accumulated for commodity delivered. */
  ZB_ZCL_ATTR_METERING_CURRENT_PARTIAL_PROFILE_INTERVAL_START_TIME_RECEIVED_ID,    /**< @e CurrentPartialProfileIntervalStartTimeReceived attribute represents the start
                                                                             *   time of the current Load Profile interval being accumulated for commodity received. */
  ZB_ZCL_ATTR_METERING_CURRENT_PARTIAL_PROFILE_INTERVAL_VALUE_DELIVERED_ID,         /**< @e CurrentPartialProfileIntervalValueDelivered attribute represents the value of
                                                                             *   the current Load Profile interval being accumulated for commodity delivered. */
  ZB_ZCL_ATTR_METERING_CURRENT_PARTIAL_PROFILE_INTERVAL_VALUE_RECEIVED_ID,         /**< @e CurrentPartialProfileIntervalValueReceived attribute represents the value of
                                                                             *   the current Load Profile interval being accumulated for commodity delivered. */
  ZB_ZCL_ATTR_METERING_CURRENT_DAY_MAX_PRESSURE_ID,                            /**< @e CurrentDayMaxPressure attribute is the maximum pressure reported during a day
                                                                             *   from the water or gas meter. */
  ZB_ZCL_ATTR_METERING_CURRENT_DAY_MIN_PRESSURE_ID,                            /**< @e CurrentDayMinPressure attribute is the minimum pressure reported during a day
                                                                             *   from the water or gas meter. */
  ZB_ZCL_ATTR_METERING_PREVIOUS_DAY_MAX_PRESSURE_ID,                            /**< @e PreviousDayMaxPressure attribute is the maximum pressure reported during previous
                                                                             *   day from the water or gas meter. */
  ZB_ZCL_ATTR_METERING_PREVIOUS_DAY_MIN_PRESSURE_ID,                            /**< @e PreviousDayMinPressure attribute is the minimum pressure reported during previous
                                                                             *   day from the water or gas meter. */
  ZB_ZCL_ATTR_METERING_CURRENT_DAY_MAX_DEMAND_ID,                              /**< @e CurrentDayMaxDemand attribute represents the maximum demand or rate of delivered
                                                                             *   value of Energy, Gas, or Water being utilized at the premises. */
  ZB_ZCL_ATTR_METERING_PREVIOUS_DAY_MAX_DEMAND_ID,                              /**< @e PreviousDayMaxDemand attribute represents the maximum demand or rate of delivered
                                                                             *   value of Energy, Gas, or Water being utilized at the premises. */
  ZB_ZCL_ATTR_METERING_CURRENT_MONTH_MAX_DEMAND_ID,                            /**< @e CurrentMonthMaxDemand attribute is the maximum demand reported during a month
                                                                             *   from the meter. For electricity, heat and cooling meters this is the maximum power
                                                                             *   reported in a month.*/
  ZB_ZCL_ATTR_METERING_CURRENT_YEAR_MAX_DEMAND_ID,                             /**< @e CurrentYearMaxDemand attribute is the maximum demand reported during a year
                                                                             *   from the meter. For electricity, heat and cooling meters this is the maximum power
                                                                             *   reported in a year.*/
  ZB_ZCL_ATTR_METERING_CURRENT_DAY_MAX_ENERGY_CARRIER_DEMAND_ID,               /**< @e CurrentDayMaxEnergyCarrierDemand attribute is the maximum energy carrier demand
                                                                             *   reported during a day from the meter. For heat and cooling meters this is the maximum
                                                                             *   flow rate on the inlet reported in a day.
                                                                             *   @note At the end of a day the meter will transfer the @e CurrentDayMaxEnergyCarrierDemand
                                                                             *   into @ref ZB_ZCL_ATTR_METERING_PREVIOUS_DAY_MAX_ENERGY_CARRIER_DEMAND_ID "PreviousDayMaxEnergyCarrierDemand". */
  ZB_ZCL_ATTR_METERING_PREVIOUS_DAY_MAX_ENERGY_CARRIER_DEMAND_ID,               /**< @e PreviousDayMaxEnergyCarrierDemand attribute is the maximum energy carrier demand
                                                                             *   reported during the previous day from the meter. */
  ZB_ZCL_ATTR_METERING_CURRENT_MONTH_MAX_ENERGY_CARRIER_DEMAND_ID,             /**< @e CurrentMonthMaxEnergyCarrierDemand attribute is the maximum energy carrier demand
                                                                             *   reported during a month from the meter. For heat and cooling meters this is the
                                                                             *   maximum flow rate on the inlet reported in a month. */
  ZB_ZCL_ATTR_METERING_CURRENT_MONTH_MIN_ENERGY_CARRIER_DEMAND_ID,             /**< @e CurrentMonthMinEnergyCarrierDemand attribute is the minimum energy carrier demand
                                                                             *   reported during a month from the meter. For heat and cooling meters this is the
                                                                             *   minimum flow rate on the inlet reported in a month. */
  ZB_ZCL_ATTR_METERING_CURRENT_YEAR_MAX_ENERGY_CARRIER_DEMAND_ID,              /**< @e CurrentYearMaxEnergyCarrierDemand attribute is the maximum energy carrier demand
                                                                             *   reported during a year from the meter. For heat and cooling meters this is the maximum
                                                                             *   flow rate on the inlet reported in a year. */
  ZB_ZCL_ATTR_METERING_CURRENT_YEAR_MIN_ENERGY_CARRIER_DEMAND_ID,              /**< @e CurrentYearMinEnergyCarrierDemand attribute is the minimum energy carrier demand
                                                                             *   reported during a year from the heat meter. For heat and cooling meters this is the
                                                                             *   minimum flow rate on the inlet reported in a year. */
  /* 0x0417 to 0x041F reserved */
  ZB_ZCL_ATTR_METERING_PREVIOUS_DAY2_CONSUMPTION_DELIVERED_ID = ZB_ZCL_ATTR_SET_WITH_ATTR_ID(ZB_ZCL_METERING_SET_HISTORICAL_CONSUMPTION, 0x20),    /**< @e PreviousDayNConsumptionDelivered
                                                                             *   attribute represents the summed value of Energy, Gas, or Water delivered to the
                                                                             *   premises within the previous 24 hour period starting at the Historical Freeze Time (HFT). */
  ZB_ZCL_ATTR_METERING_PREVIOUS_DAY2_CONSUMPTION_RECEIVED_ID,                       /**< @e PreviousDayNConsumptionReceived attribute represents the summed value of Energy,
                                                                             *   Gas, or Water received from the premises within the previous 24 hour period starting
                                                                             *   at the Historical Freeze Time (HFT). */
  ZB_ZCL_ATTR_METERING_PREVIOUS_DAY3_CONSUMPTION_DELIVERED_ID,                       /**< @copydoc ZB_ZCL_ATTR_METERING_PREVIOUS_DAY2_CONSUMPTION_DELIVERED_ID */
  ZB_ZCL_ATTR_METERING_PREVIOUS_DAY3_CONSUMPTION_RECEIVED_ID,                       /**< @copydoc ZB_ZCL_ATTR_METERING_PREVIOUS_DAY2_CONSUMPTION_RECEIVED_ID */
  ZB_ZCL_ATTR_METERING_PREVIOUS_DAY4_CONSUMPTION_DELIVERED_ID,                       /**< @copydoc ZB_ZCL_ATTR_METERING_PREVIOUS_DAY2_CONSUMPTION_DELIVERED_ID */
  ZB_ZCL_ATTR_METERING_PREVIOUS_DAY4_CONSUMPTION_RECEIVED_ID,                       /**< @copydoc ZB_ZCL_ATTR_METERING_PREVIOUS_DAY2_CONSUMPTION_RECEIVED_ID */
  ZB_ZCL_ATTR_METERING_PREVIOUS_DAY5_CONSUMPTION_DELIVERED_ID,                       /**< @copydoc ZB_ZCL_ATTR_METERING_PREVIOUS_DAY2_CONSUMPTION_DELIVERED_ID */
  ZB_ZCL_ATTR_METERING_PREVIOUS_DAY5_CONSUMPTION_RECEIVED_ID,                       /**< @copydoc ZB_ZCL_ATTR_METERING_PREVIOUS_DAY2_CONSUMPTION_RECEIVED_ID */
  ZB_ZCL_ATTR_METERING_PREVIOUS_DAY6_CONSUMPTION_DELIVERED_ID,                       /**< @copydoc ZB_ZCL_ATTR_METERING_PREVIOUS_DAY2_CONSUMPTION_DELIVERED_ID */
  ZB_ZCL_ATTR_METERING_PREVIOUS_DAY6_CONSUMPTION_RECEIVED_ID,                       /**< @copydoc ZB_ZCL_ATTR_METERING_PREVIOUS_DAY2_CONSUMPTION_RECEIVED_ID */
  ZB_ZCL_ATTR_METERING_PREVIOUS_DAY7_CONSUMPTION_DELIVERED_ID,                       /**< @copydoc ZB_ZCL_ATTR_METERING_PREVIOUS_DAY2_CONSUMPTION_DELIVERED_ID */
  ZB_ZCL_ATTR_METERING_PREVIOUS_DAY7_CONSUMPTION_RECEIVED_ID,                       /**< @copydoc ZB_ZCL_ATTR_METERING_PREVIOUS_DAY2_CONSUMPTION_RECEIVED_ID */
  ZB_ZCL_ATTR_METERING_PREVIOUS_DAY8_CONSUMPTION_DELIVERED_ID,                       /**< @copydoc ZB_ZCL_ATTR_METERING_PREVIOUS_DAY2_CONSUMPTION_DELIVERED_ID */
  ZB_ZCL_ATTR_METERING_PREVIOUS_DAY8_CONSUMPTION_RECEIVED_ID,                       /**< @copydoc ZB_ZCL_ATTR_METERING_PREVIOUS_DAY2_CONSUMPTION_RECEIVED_ID */
  /* 0x042E to 0x042F reserved */
  ZB_ZCL_ATTR_METERING_CURRENT_WEEK_CONSUMPTION_DELIVERED_ID = ZB_ZCL_ATTR_SET_WITH_ATTR_ID(ZB_ZCL_METERING_SET_HISTORICAL_CONSUMPTION, 0x30),    /**< @e CurrentWeekConsumptionDelivered
                                                                             *   attribute represents the summed value of Energy, Gas, or Water delivered to the premises
                                                                             *   since the Historical Freeze Time (HFT) on Monday to the last HFT read. */
  ZB_ZCL_ATTR_METERING_CURRENT_WEEK_CONSUMPTION_RECEIVED_ID,                       /**< @e CurrentWeekConsumptionReceived attribute represents the summed value of Energy,
                                                                             *   Gas, or Water received from the premises since the Historical Freeze Time (HFT) on
                                                                             *   Monday to the last HFT read. */
  ZB_ZCL_ATTR_METERING_PREVIOUS_WEEK_CONSUMPTION_DELIVERED_ID,                       /**< @e PreviousWeekNConsumptionDelivered attribute represents the summed value of Energy,
                                                                             *   Gas, or Water delivered to the premises within the previous week period starting at
                                                                             *   the Historical Freeze Time (HFT) on the Monday to the Sunday. */
  ZB_ZCL_ATTR_METERING_PREVIOUS_WEEK_CONSUMPTION_RECEIVED_ID,                       /**< @e PreviousWeekNConsumptionReceived attribute represents the summed value of Energy,
                                                                             *   Gas, or Water received from the premises within the previous week period starting at
                                                                             *   the Historical Freeze Time (HFT) on the Monday to the Sunday. */
  ZB_ZCL_ATTR_METERING_PREVIOUS_WEEK2_CONSUMPTION_DELIVERED_ID,                      /**< @copydoc ZB_ZCL_ATTR_METERING_PREVIOUS_WEEK_CONSUMPTION_DELIVERED_ID */
  ZB_ZCL_ATTR_METERING_PREVIOUS_WEEK2_CONSUMPTION_RECEIVED_ID,                      /**< @copydoc ZB_ZCL_ATTR_METERING_PREVIOUS_WEEK_CONSUMPTION_RECEIVED_ID */
  ZB_ZCL_ATTR_METERING_PREVIOUS_WEEK3_CONSUMPTION_DELIVERED_ID,                      /**< @copydoc ZB_ZCL_ATTR_METERING_PREVIOUS_WEEK_CONSUMPTION_DELIVERED_ID */
  ZB_ZCL_ATTR_METERING_PREVIOUS_WEEK3_CONSUMPTION_RECEIVED_ID,                      /**< @copydoc ZB_ZCL_ATTR_METERING_PREVIOUS_WEEK_CONSUMPTION_RECEIVED_ID */
  ZB_ZCL_ATTR_METERING_PREVIOUS_WEEK4_CONSUMPTION_DELIVERED_ID,                      /**< @copydoc ZB_ZCL_ATTR_METERING_PREVIOUS_WEEK_CONSUMPTION_DELIVERED_ID */
  ZB_ZCL_ATTR_METERING_PREVIOUS_WEEK4_CONSUMPTION_RECEIVED_ID,                      /**< @copydoc ZB_ZCL_ATTR_METERING_PREVIOUS_WEEK_CONSUMPTION_RECEIVED_ID */
  ZB_ZCL_ATTR_METERING_PREVIOUS_WEEK5_CONSUMPTION_DELIVERED_ID,                      /**< @copydoc ZB_ZCL_ATTR_METERING_PREVIOUS_WEEK_CONSUMPTION_DELIVERED_ID */
  ZB_ZCL_ATTR_METERING_PREVIOUS_WEEK5_CONSUMPTION_RECEIVED_ID,                      /**< @copydoc ZB_ZCL_ATTR_METERING_PREVIOUS_WEEK_CONSUMPTION_RECEIVED_ID */
  /* 0x043C to 0x043F reserved */
  ZB_ZCL_ATTR_METERING_CURRENT_MONTH_CONSUMPTION_DELIVERED_ID = ZB_ZCL_ATTR_SET_WITH_ATTR_ID(ZB_ZCL_METERING_SET_HISTORICAL_CONSUMPTION, 0x40),   /**< @e CurrentMonthConsumptionDelivered
                                                                             *   attribute represents the summed value of Energy, Gas, or Water delivered to the premises
                                                                             *   since the Historical Freeze Time (HFT) on the 1 st of the month to the last HFT read. */
  ZB_ZCL_ATTR_METERING_CURRENT_MONTH_CONSUMPTION_RECEIVED_ID,                      /**< @e CurrentMonthConsumptionReceived attribute represents the summed value of Energy,
                                                                             *   Gas, or Water received from the premises since the Historical Freeze Time (HFT) on the
                                                                             *   1 st of the month to the last HFT read. */
  ZB_ZCL_ATTR_METERING_PREVIOUS_MONTH_CONSUMPTION_DELIVERED_ID,                      /**< @e PreviousMonthNConsumptionDelivered attribute represents the summed value of
                                                                             *   Energy, Gas, or Water delivered to the premises within the previous Month period
                                                                             *   starting at the Historical Freeze Time (HFT) on the 1 st of the month to the last
                                                                             *   day of the month.*/
  ZB_ZCL_ATTR_METERING_PREVIOUS_MONTH_CONSUMPTION_RECEIVED_ID,                      /**< @e PreviousMonthNConsumptionReceived attribute represents the summed value of
                                                                             *   Energy, Gas, or Water received from the premises within the previous month period
                                                                             *   starting at the Historical Freeze Time (HFT) on the 1 st of the month to the last
                                                                             *   day of the month.*/
  ZB_ZCL_ATTR_METERING_PREVIOUS_MONTH2_CONSUMPTION_DELIVERED_ID,                     /**< @copydoc ZB_ZCL_ATTR_METERING_PREVIOUS_MONTH_CONSUMPTION_DELIVERED_ID */
  ZB_ZCL_ATTR_METERING_PREVIOUS_MONTH2_CONSUMPTION_RECEIVED_ID,                     /**< @copydoc ZB_ZCL_ATTR_METERING_PREVIOUS_MONTH_CONSUMPTION_RECEIVED_ID */
  ZB_ZCL_ATTR_METERING_PREVIOUS_MONTH3_CONSUMPTION_DELIVERED_ID,                     /**< @copydoc ZB_ZCL_ATTR_METERING_PREVIOUS_MONTH_CONSUMPTION_DELIVERED_ID */
  ZB_ZCL_ATTR_METERING_PREVIOUS_MONTH3_CONSUMPTION_RECEIVED_ID,                     /**< @copydoc ZB_ZCL_ATTR_METERING_PREVIOUS_MONTH_CONSUMPTION_RECEIVED_ID */
  ZB_ZCL_ATTR_METERING_PREVIOUS_MONTH4_CONSUMPTION_DELIVERED_ID,                     /**< @copydoc ZB_ZCL_ATTR_METERING_PREVIOUS_MONTH_CONSUMPTION_DELIVERED_ID */
  ZB_ZCL_ATTR_METERING_PREVIOUS_MONTH4_CONSUMPTION_RECEIVED_ID,                     /**< @copydoc ZB_ZCL_ATTR_METERING_PREVIOUS_MONTH_CONSUMPTION_RECEIVED_ID */
  ZB_ZCL_ATTR_METERING_PREVIOUS_MONTH5_CONSUMPTION_DELIVERED_ID,                     /**< @copydoc ZB_ZCL_ATTR_METERING_PREVIOUS_MONTH_CONSUMPTION_DELIVERED_ID */
  ZB_ZCL_ATTR_METERING_PREVIOUS_MONTH5_CONSUMPTION_RECEIVED_ID,                     /**< @copydoc ZB_ZCL_ATTR_METERING_PREVIOUS_MONTH_CONSUMPTION_RECEIVED_ID */
  ZB_ZCL_ATTR_METERING_PREVIOUS_MONTH6_CONSUMPTION_DELIVERED_ID,                     /**< @copydoc ZB_ZCL_ATTR_METERING_PREVIOUS_MONTH_CONSUMPTION_DELIVERED_ID */
  ZB_ZCL_ATTR_METERING_PREVIOUS_MONTH6_CONSUMPTION_RECEIVED_ID,                     /**< @copydoc ZB_ZCL_ATTR_METERING_PREVIOUS_MONTH_CONSUMPTION_RECEIVED_ID */
  ZB_ZCL_ATTR_METERING_PREVIOUS_MONTH7_CONSUMPTION_DELIVERED_ID,                     /**< @copydoc ZB_ZCL_ATTR_METERING_PREVIOUS_MONTH_CONSUMPTION_DELIVERED_ID */
  ZB_ZCL_ATTR_METERING_PREVIOUS_MONTH7_CONSUMPTION_RECEIVED_ID,                     /**< @copydoc ZB_ZCL_ATTR_METERING_PREVIOUS_MONTH_CONSUMPTION_RECEIVED_ID */
  ZB_ZCL_ATTR_METERING_PREVIOUS_MONTH8_CONSUMPTION_DELIVERED_ID,                     /**< @copydoc ZB_ZCL_ATTR_METERING_PREVIOUS_MONTH_CONSUMPTION_DELIVERED_ID */
  ZB_ZCL_ATTR_METERING_PREVIOUS_MONTH8_CONSUMPTION_RECEIVED_ID,                     /**< @copydoc ZB_ZCL_ATTR_METERING_PREVIOUS_MONTH_CONSUMPTION_RECEIVED_ID */
  ZB_ZCL_ATTR_METERING_PREVIOUS_MONTH9_CONSUMPTION_DELIVERED_ID,                     /**< @copydoc ZB_ZCL_ATTR_METERING_PREVIOUS_MONTH_CONSUMPTION_DELIVERED_ID */
  ZB_ZCL_ATTR_METERING_PREVIOUS_MONTH9_CONSUMPTION_RECEIVED_ID,                     /**< @copydoc ZB_ZCL_ATTR_METERING_PREVIOUS_MONTH_CONSUMPTION_RECEIVED_ID */
  ZB_ZCL_ATTR_METERING_PREVIOUS_MONTH10_CONSUMPTION_DELIVERED_ID,                    /**< @copydoc ZB_ZCL_ATTR_METERING_PREVIOUS_MONTH_CONSUMPTION_DELIVERED_ID */
  ZB_ZCL_ATTR_METERING_PREVIOUS_MONTH10_CONSUMPTION_RECEIVED_ID,                    /**< @copydoc ZB_ZCL_ATTR_METERING_PREVIOUS_MONTH_CONSUMPTION_RECEIVED_ID */
  ZB_ZCL_ATTR_METERING_PREVIOUS_MONTH11_CONSUMPTION_DELIVERED_ID,                    /**< @copydoc ZB_ZCL_ATTR_METERING_PREVIOUS_MONTH_CONSUMPTION_DELIVERED_ID */
  ZB_ZCL_ATTR_METERING_PREVIOUS_MONTH11_CONSUMPTION_RECEIVED_ID,                    /**< @copydoc ZB_ZCL_ATTR_METERING_PREVIOUS_MONTH_CONSUMPTION_RECEIVED_ID */
  ZB_ZCL_ATTR_METERING_PREVIOUS_MONTH12_CONSUMPTION_DELIVERED_ID,                    /**< @copydoc ZB_ZCL_ATTR_METERING_PREVIOUS_MONTH_CONSUMPTION_DELIVERED_ID */
  ZB_ZCL_ATTR_METERING_PREVIOUS_MONTH12_CONSUMPTION_RECEIVED_ID,                    /**< @copydoc ZB_ZCL_ATTR_METERING_PREVIOUS_MONTH_CONSUMPTION_RECEIVED_ID */
  ZB_ZCL_ATTR_METERING_PREVIOUS_MONTH13_CONSUMPTION_DELIVERED_ID,                    /**< @copydoc ZB_ZCL_ATTR_METERING_PREVIOUS_MONTH_CONSUMPTION_DELIVERED_ID */
  ZB_ZCL_ATTR_METERING_PREVIOUS_MONTH13_CONSUMPTION_RECEIVED_ID,                    /**< @copydoc ZB_ZCL_ATTR_METERING_PREVIOUS_MONTH_CONSUMPTION_RECEIVED_ID */
  ZB_ZCL_ATTR_METERING_HISTORICAL_FREEZE_TIME_ID,               /**< @e HistoricalFreezeTime attribute represents the time of day, in Local Time, when
                                                                 *   Historical Consumption attributes and/or Alternative Historical Consumption
                                                                 *   attributes are captured (hour and minutes). */
  ZB_ZCL_ATTR_METERING_CURRENT_DAY_MAX_DEMAND_DELIVERED_ID,        /**< @e CurrentDayMaxDemandDelivered represents the maximum demand or rate of delivered value of
                                                                 *  Energy, Gas, or Water being utilized at the premises since the Historical Freeze Time (HFT). */
  ZB_ZCL_ATTR_METERING_CURRENT_DAY_MAX_DEMAND_DELIVERED_TIME_ID,   /**< The @e CurrentDayMaxDemandDeliveredTime attribute represents the time when
                                                                  *  @e CurrentDayMaxDemandDelivered reading was captured. */
  ZB_ZCL_ATTR_METERING_CURRENT_DAY_MAX_DEMAND_RECEIVED_ID,         /**< @e CurrentDayMaxDemandReceived represents the maximum demand or rate of received value of
                                                                  *  Energy, Gas, or Water being utilized by the utility from the premises sinc e the Historical
                                                                  *  Freeze Time (HFT). */
  ZB_ZCL_ATTR_METERING_CURRENT_DAY_MAX_DEMAND_RECEIVED_TIME_ID,    /**< The CurrentDayMaxDemandReceivedTime attribute represents the time CurrentDayMaxDemandReceived
                                                                  * reading was captured */
  ZB_ZCL_ATTR_METERING_PREVIOUS_DAY_MAX_DEMAND_DELIVERED_ID,       /**< @e PreviousDayMaxDemandDelivered represents the maximum demand or rate of delivered
                                                                  *  value of Energy, Gas, or Water that was utilized at the premises within the previous 24
                                                                  *  hour period starting at the Historical Freeze Time (HFT) */
  ZB_ZCL_ATTR_METERING_PREVIOUS_DAY_MAX_DEMAND_DELIVERED_TIME_ID,  /**< The @e PreviousDayMaxDemandDeliveredTime attribute represents the time when
                                                                  * PreviousDayMaxDemandDelivered reading was captured. */
  ZB_ZCL_ATTR_METERING_PREVIOUS_DAY_MAX_DEMAND_RECEIVED_ID,        /**< @e PreviousDayMaxDemandReceived represents the maximum demand or rate of received value
                                                                  * of Energy, Gas, or Water that was utilized by the utility from the premises within the
                                                                  * previous 24 hour period starting at the Historical Freeze Time (HFT).
                                                                  */
  ZB_ZCL_ATTR_METERING_PREVIOUS_DAY_MAX_DEMAND_RECEIVED_TIME_ID,   /**< The @e PreviousDayMaxDem andReceivedTime attribute represents the time when
                                                                  * @e PreviousDayMaxDemandReceived reading was captured.
                                                                  */
  /* 0x045D to 0x04FF reserved */


  /* Load Profile Configuration Attribute Set, table D.30 */
  ZB_ZCL_ATTR_METERING_MAX_NUMBER_OF_PERIODS_DELIVERED_ID = ZB_ZCL_ATTR_SET_WITH_ATTR_ID(ZB_ZCL_METERING_SET_LOAD_PROFILE_CONFIGURATION, 0x00),    /**< @e MaxNumberOfPeriodsDelivered
                                                                          * attribute represents the maximum number of intervals the device is capable of returning
                                                                          * in one Get Profile Response command. It is required @e MaxNumberOfPeriodsDelivered fit
                                                                          * within the default Fragmentation ASDU size of 128 bytes, or an optionally agreed upon
                                                                          * larger Fragmentation ASDU size supported by both devices.
                                                                          * @see SE spec, sub-clause 5.3.8. */
  /* 0x0501 to 0x05FF reserved */


  /* Supply Limit Attribute Set, table D.31 */
  ZB_ZCL_ATTR_METERING_CURRENT_DEMAND_DELIVERED_ID = ZB_ZCL_ATTR_SET_WITH_ATTR_ID(ZB_ZCL_METERING_SET_SUPPLY_LIMIT, 0x00),   /**< @e CurrentDemandDelivered attribute represents
                                                           *   the current Demand of Energy, Gas, or Water delivered at the premises. */
  ZB_ZCL_ATTR_METERING_DEMAND_LIMIT_ID,                   /**< @e DemandLimit attribute reflects the current supply demand limit set in the meter. This value can be
                                                           *   compared to the @ref ZB_ZCL_ATTR_METERING_CURRENT_DEMAND_DELIVERED_ID "CurrentDemandDelivered" attribute to
                                                           *   understand if limits are being approached or exceeded. A value of 0xFFFFFF indicates demand limiting
                                                           *   is switched off.*/
  ZB_ZCL_ATTR_METERING_DEMAND_INTEGRATION_PERIOD_ID,      /**< @e DemandIntegrationPeriod attribute is the number of minutes over which the
                                                           *   @ref ZB_ZCL_ATTR_METERING_CURRENT_DEMAND_DELIVERED_ID "CurrentDemandDelivered" attribute is calculated. Valid
                                                           *   range is 0x01 to 0xFF. 0x00 is a reserved value. */
  ZB_ZCL_ATTR_METERING_NUMBER_OF_DEMAND_SUBINTERVALS_ID,     /**< @e NumberOfDemandSubintervals attribute represents the number of subintervals used within the
                                                           *   @ref ZB_ZCL_ATTR_METERING_DEMAND_INTEGRATION_PERIOD_ID "DemandIntegrationPeriod". The subinterval
                                                           *   duration (in minutes) is obtained by dividing the @ref ZB_ZCL_ATTR_METERING_DEMAND_INTEGRATION_PERIOD_ID
                                                           *   "DemandIntegrationPeriod" by the @ref ZB_ZCL_ATTR_METERING_NUMBER_OF_DEMAND_SUBINTERVALS_ID "NumberOfDemandSubintervals".
                                                           *   The @ref ZB_ZCL_ATTR_METERING_CURRENT_DEMAND_DELIVERED_ID "CurrentDemandDelivered" attribute is updated at the
                                                           *   each of each subinterval. Valid range is 0x01 to 0xFF. 0x00 is a reserved value. */
  ZB_ZCL_ATTR_METERING_DEMAND_LIMIT_ARM_DURATION_ID,         /**< @e DemandLimitArmDuration attribute defines the length of time, in seconds, that the supply shall be
                                                           *   disconnected if the @ref ZB_ZCL_ATTR_METERING_DEMAND_LIMIT_ID "DemandLimit" attribute is enabled and
                                                           *   the limit is exceeded.*/
  ZB_ZCL_ATTR_METERING_LOAD_LIMIT_SUPPLY_STATE_ID,           /**< @e LoadLimitSupplyState attribute indicates the required status of the supply once device is in a load
                                                           *   limit state. @see SE 1.4 spec, table D.68. */
  ZB_ZCL_ATTR_METERING_LOAD_LIMIT_COUNTER_ID,                /**< @e LoadLimitCounter attribute is used for counting the number of times that the demand limit has
                                                           *   exceeded the set threshold. */
  ZB_ZCL_ATTR_METERING_SUPPLY_TAMPER_STATE_ID,               /**< @e SupplyTamperState attribute indicates the required status of the supply following the detection of a
                                                           *   tamper event within the metering device. @see SE 1.4 spec, table D.68. */
  ZB_ZCL_ATTR_METERING_SUPPLY_DEPLETION_STATE_ID,            /**< @e SupplyDepletionState attribute indicates the required status of the supply following detection of a
                                                           *   depleted battery within the metering device. @see SE 1.4 spec, table D.68. */
  ZB_ZCL_ATTR_METERING_SUPPLY_UNCONTROLLED_FLOW_STATE_ID,    /**< @e SupplyUncontrolledFlowState attribute indicates the required status of the supply following detection
                                                           *   of an uncontrolled flow event within the metering device. @see SE 1.4 spec, table D.68. */
  /* 0x060A to 0x06FF reserved */

  /*TODO: need to add set 0x07 - ZB_ZCL_METERING_SET_BLOCK_INFORMATION_DELIVERED*/
  /*FIXME: Does we need to use _ID suffix in attributes? */
  /*ZB_ZCL_METERING_SET_ALARMS attribute set*/
  ZB_ZCL_ATTR_METERING_GENERIC_ALARM_MASK_ID = ZB_ZCL_ATTR_SET_WITH_ATTR_ID(ZB_ZCL_METERING_SET_ALARMS, 0x00), /**< The AlarmMask attributes of the Alarm Attribute Set
                                                                                                              * specify whether each of the alarms listed in the
                                                                                                              * corresponding alarm group is enabled. When the bit number
                                                                                                              * corresponding to the alarm number (minus the group offset) is set to 1,
                                                                                                              * the alarm is enabled, else it is disabled. Bits not corresponding
                                                                                                              * to a code in the respective table are reserved.
                                                                                                              */
  ZB_ZCL_ATTR_METERING_ELECTRICITY_ALARM_MASK_ID,                /**<  @copydoc ZB_ZCL_ATTR_METERING_GENERIC_ALARM_MASK_ID */
  ZB_ZCL_ATTR_METERING_GENERIC_FLOW_PRESSURE_ALARM_MASK_ID,      /**<  @copydoc ZB_ZCL_ATTR_METERING_GENERIC_ALARM_MASK_ID */
  ZB_ZCL_ATTR_METERING_WATER_SPECIFIC_ALARM_MASK_ID,             /**<  @copydoc ZB_ZCL_ATTR_METERING_GENERIC_ALARM_MASK_ID */
  ZB_ZCL_ATTR_METERING_HEAT_AND_COOLING_SPECIFIC_ALARM_MASK_ID,  /**<  @copydoc ZB_ZCL_ATTR_METERING_GENERIC_ALARM_MASK_ID */
  ZB_ZCL_ATTR_METERING_GAS_SPECIFIC_ALARM_MASK_ID,               /**<  @copydoc ZB_ZCL_ATTR_METERING_GENERIC_ALARM_MASK_ID */
  ZB_ZCL_ATTR_METERING_EXTENDED_GENERIC_ALARM_MASK_ID,            /**<  @copydoc ZB_ZCL_ATTR_METERING_GENERIC_ALARM_MASK_ID */
  ZB_ZCL_ATTR_METERING_MANUFACTURER_ALARM_MASK_ID,               /**<  @copydoc ZB_ZCL_ATTR_METERING_GENERIC_ALARM_MASK_ID */

  /*TODO: need to add set 0x09 */

  /* Meter Billing Attribute Set, table D.44 */

  /**< @e BillToDateDelivered provides a value for the costs in the current billing period.*/
  ZB_ZCL_ATTR_METERING_BILL_TO_DATE_DELIVERED = ZB_ZCL_ATTR_SET_WITH_ATTR_ID(ZB_ZCL_METERING_SET_METER_BILLING, 0x00),
  ZB_ZCL_ATTR_METERING_BILL_TO_DATE_TIMESTAMP_DELIVERED, /**< @e BillToDateTimeStampDelivered The UTC timestamp
                                                         * when the associated BillToDateDelivered attribute was last updated.*/
  ZB_ZCL_ATTR_METERING_PROJECTED_BILL_DELIVERED, /**< @e ProjectedBillDelivered provides a value indicating what the estimated state of the account
                                                * will be at the end of the billing period based on past consumption.*/
  ZB_ZCL_ATTR_METERING_PROJECTED_BILL_TIME_STAMP_DELIVERED, /**< @e ProjectedBillTimeStampDelivered
                                                            * The UTC timestamp when the associated ProjectedBillDelivered attribute was last updated. */
  ZB_ZCL_ATTR_METERING_BILL_DELIVERED_TRAILING_DIGIT, /**< @e BillDeliveredTrailingDigit An 8-bit BitMap used to determine where the decimal point is located
                                                    * in the BillToDateDelivered and ProjectedBillDelivered attributes. */
  ZB_ZCL_ATTR_METERING_BILL_TO_DATE_RECEIVED = ZB_ZCL_ATTR_SET_WITH_ATTR_ID(ZB_ZCL_METERING_SET_METER_BILLING, 0x10),
                                                    /**< @e BillToDateReceived provides a value for the costs in the current billing period.
                                                    * This attribute is measured in a base unit of Currency with the decimal point located
                                                    * as indicated by the BillReceivedTrailingDigit attribute */
  ZB_ZCL_ATTR_METERING_BILL_TO_DATE_TIMESTAMP_RECEIVED, /**< @e BillToDateTimeStampReceived The UTC timestamp
                                                       * when the associated BillToDateReceived attribute was last updated */
  ZB_ZCL_ATTR_METERING_PROJECTED_BILL_RECEIVED, /**< @e ProjectedBillReceived provides a value indicating what the estimated state of the account
                                              * will be at the end of the billing period based on past generation.*/
  ZB_ZCL_ATTR_METERING_PROJECTED_BILL_TIME_STAMP_RECEIVED, /**< @e ProjectedBillTimeStampReceived
                                                          * The UTC timestamp when the associated ProjectedBillReceived attribute was last updated. */
  ZB_ZCL_ATTR_METERING_BILL_RECEIVED_TRAILING_DIGIT, /**< @e BillReceivedTrailingDigit
                                                    * An 8-bit BitMap used to determine where the decimal point is located
                                                    * in the BillToDateReceived and ProjectedBillReceived attributes.*/

  /* Supply Control Attribute, table D.45 */

  /**< @e ProposedChangeSupplyImplementationTime indicates the time
   * at which a proposed change to the supply is to be implemented.*/
  ZB_ZCL_ATTR_METERING_PROPOSED_CHANGE_SUPPLY_IMPLEMENTATION_TIME = ZB_ZCL_ATTR_SET_WITH_ATTR_ID(ZB_ZCL_METERING_SET_SUPPLY_CONTROL, 0x00),
  ZB_ZCL_ATTR_METERING_PROPOSED_CHANGE_SUPPLY_STATUS, /**< @e ProposedChangeSupplyStatus indicates the proposed status of the supply
                                                     * once the change to the supply has be been implemented.*/
  /**< @e UncontrolledFlowThreshold indicates the threshold above which a flow meter (e.g. Gas or Water)
   * shall detect an uncontrolled flow.*/
  ZB_ZCL_ATTR_METERING_UNCONTROLLED_FLOW_THRESHOLD = ZB_ZCL_ATTR_SET_WITH_ATTR_ID(ZB_ZCL_METERING_SET_SUPPLY_CONTROL, 0x10),
  ZB_ZCL_ATTR_METERING_UNCONTROLLED_FLOW_THRESHOLD_UNIT_OF_MEASURE, /**< @e UncontrolledFlowThresholdUnitOfMeasure indicates the unit of measure
                                                                     * used in conjunction with the Uncontrolled Flow Threshold attribute.*/
  ZB_ZCL_ATTR_METERING_UNCONTROLLED_FLOW_MULTIPLIER, /**< @e UncontrolledFlowMultiplier indicates the multiplier,
                                                    * to be used in conjunction with the Uncontrolled Flow Threshold and Uncontrolled Flow Divisor attributes,
                                                    * to determine the true flow threshold value.*/
  ZB_ZCL_ATTR_METERING_UNCONTROLLED_FLOW_DIVISOR, /**< @e UncontrolledFlowDivisor
                                                  * The Uncontrolled Flow Divisor attribute indicates the divisor, to be used in conjunction
                                                  * with the Uncontrolled Flow Threshold and Uncontrolled Flow Multiplier attributes,
                                                  * to determine the true flow threshold value.*/
  ZB_ZCL_ATTR_METERING_FLOW_STABILISATION_PERIOD, /**< @e FlowStabilisationPeriod indicates the time given to allow the flow to stabilize.*/
  ZB_ZCL_ATTR_METERING_FLOW_MEASUREMENT_PERIOD, /**< @e FlowMeasurementPeriodAttribute indicates the period
                                                * over which the flow is measured and compared against the Uncontrolled Flow Threshold attribute.*/

  /* Alternative Historical Consumption Attribute Set, table D.46 */
  /**< @e AlternativeInstantaneousDemand represents the current Demand delivered or received at the premises*/
  ZB_ZCL_ATTR_METERING_ALT_INSTANTANEOUS_DEMAND = ZB_ZCL_ATTR_SET_WITH_ATTR_ID(ZB_ZCL_METERING_SET_ALTERNATIVE_HISTORICAL_CONSUMPTION, 0x00),
  ZB_ZCL_ATTR_METERING_CURRENT_DAY_ALT_CONSUMPTION_DELIVERED, /**< @e CurrentDayAlternativeConsumptionDelivered represents the summed value
                                                              * delivered to the premises since the Historical Freeze Time (HFT). */
  ZB_ZCL_ATTR_METERING_CURRENT_DAY_ALT_CONSUMPTION_RECEIVED, /**< @e CurrentDayAlternativeConsumptionReceived represents the summed value
                                                            * received from the premises since the Historical Freeze Time (HFT).*/
  ZB_ZCL_ATTR_METERING_PREVIOUS_DAY_ALT_CONSUMPTION_DELIVERED, /**< @e PreviousDayAlternativeConsumptionDelivered represents the summed value
                                                              * delivered to the premises within the previous 24 hour period
                                                              * starting at the alternative Historical Freeze Time (HFT).*/
  ZB_ZCL_ATTR_METERING_PREVIOUS_DAY_ALT_CONSUMPTION_RECEIVED, /**< @e PreviousDayAlternativeConsumptionReceived represents the summed value
                                                              * received to the premises within the previous 24 hour period
                                                              * starting at the alternative Historical Freeze Time (HFT).*/
  ZB_ZCL_ATTR_METERING_CURRENT_ALT_PARTIAL_PROFILE_INTERVAL_START_TIME_DELIVERED, /**< @e CurrentAlternativePartialProfileIntervalStartTimeDelivered represents the start time
                                                                                  * of the current Load Profile interval being accumulated for commodity delivered */
  ZB_ZCL_ATTR_METERING_CURRENT_ALT_PARTIAL_PROFILE_INTERVAL_START_TIME_RECEIVED, /**< @e CurrentAlternativePartialProfileIntervalStartTimeReceived represents the start time
                                                                                  * of the current Load Profile interval being accumulated for commodity received */
  ZB_ZCL_ATTR_METERING_CURRENT_ALT_PARTIAL_PROFILE_INTERVAL_VALUE_DELIVERED, /**< @e CurrentAlternativePartialProfileIntervalValueDelivered represents the value
*                                                                            * of the current Load Profile interval being accumulated for commodity delivered.*/
  ZB_ZCL_ATTR_METERING_CURRENT_ALT_PARTIAL_PROFILE_INTERVAL_VALUE_RECEIVED, /**< @e CurrentAlternativePartialProfileIntervalValueReceived represents the value
*                                                                            * of the current Load Profile interval being accumulated for commodity received .*/
  ZB_ZCL_ATTR_METERING_CURRENT_DAY_ALT_MAX_PRESSURE, /**< @e CurrentDayAlternativeMaxPressure is the maximum pressure
                                                     * reported during a day from the water or gas meter. */
  ZB_ZCL_ATTR_METERING_CURRENT_DAY_ALT_MIN_PRESSURE, /**< @e CurrentDayAlternativeMinPressure is the minimum pressure
                                                     * reported during a day from the water or gas meter. */
  ZB_ZCL_ATTR_METERING_PREVIOUS_DAY_ALT_MAX_PRESSURE, /**< @e PreviousDayAlternativeMaxPressure represents the maximum pressure
                                                      * reported during previous day from the water or gas meter.*/
  ZB_ZCL_ATTR_METERING_PREVIOUS_DAY_ALT_MIN_PRESSURE, /**< @e PreviousDayAlternativeMinPressure  represents the minimum pressure
                                                      * reported during previous day from the water or gas meter.*/

  ZB_ZCL_ATTR_METERING_CURRENT_DAY_ALT_MAX_DEMAND, /**< @e CurrentDayAlternativeMaxDemand represents the maximum demand or rate
                                                   * of delivered value of Energy, Gas, or Water being utilized at the premises.*/
  ZB_ZCL_ATTR_METERING_PREVIOUS_DAY_ALT_MAX_DEMAND, /**< @e PreviousDayAlternativeMaxDemand represents represents the maximum demand or rate
                                                    * of delivered value of Energy, Gas, or Water being utilized at the premises.*/

  ZB_ZCL_ATTR_METERING_CURRENT_MONTH_ALT_MAX_DEMAND, /**< @e CurrentMonthAlternativeMaxDemand is the maximum demand reported during a month from the meter. */
  ZB_ZCL_ATTR_METERING_CURRENT_YEAR_ALT_MAX_DEMAND, /**< @e CurrentYearAlternativeMaxDemand is the maximum demand reported during a year from the meter.*/

  /**< @e PreviousDayNAlternativeConsumptionDelivered represents the summed value delivered to the premises
  * within the previous 24 hour period starting at the Historical Freeze Time (HFT).  */
  ZB_ZCL_ATTR_METERING_PREVIOUS_DAY2_ALT_CONSUMPTION_DELIVERED = ZB_ZCL_ATTR_SET_WITH_ATTR_ID(ZB_ZCL_METERING_SET_ALTERNATIVE_HISTORICAL_CONSUMPTION, 0x20),
  ZB_ZCL_ATTR_METERING_PREVIOUS_DAY2_ALT_CONSUMPTION_RECEIVED, /**< @e PreviousDayNAlternativeConsumptionReceived
                                                              * represents the summed value received from the premises
                                                              * within the previous 24 hour period starting at the Historical Freeze Time (HFT).*/

  /**< @e CurrentWeekAlternativeConsumptionDelivered represents the summed value
  * delivered to the premises since the Historical Freeze Time (HFT) on Monday to the last HFT read. */
  ZB_ZCL_ATTR_METERING_CURRENT_WEEK_ALT_CONSUMPTION_DELIVERED = ZB_ZCL_ATTR_SET_WITH_ATTR_ID(ZB_ZCL_METERING_SET_ALTERNATIVE_HISTORICAL_CONSUMPTION, 0x30),
  ZB_ZCL_ATTR_METERING_CURRENT_WEEK_ALT_CONSUMPTION_RECEIVED, /**< @e CurrentWeekAlternativeConsumptionReceived represents the summed value
                                                              * received from the premises since the Historical Freeze Time (HFT) on Monday to the last HFT read */

  ZB_ZCL_ATTR_METERING_PREVIOUS_WEEK_ALT_CONSUMPTION_DELIVERED, /**< @e PreviousWeekNAlternativeConsumptionDelivered represents the summed value
                                                                * delivered to the premises within the previous week period
                                                                * starting at the Historical Freeze Time (HFT) on the Monday to the Sunday.  */
  ZB_ZCL_ATTR_METERING_PREVIOUS_WEEK_ALT_CONSUMPTION_RECEIVED, /**< @e PreviousWeekNAlternativeConsumptionReceived represents the summed value
                                                                * received from the premises within the previous week period
                                                                * starting at the Historical Freeze Time (HFT) on the Monday to the Sunday*/
  /**< @e CurrentMonthAlternativeConsumptionDelivered represents the summed value delivered to the premises
  * since the Historical Freeze Time (HFT) on the 1st of the month to the last HFT read */
  ZB_ZCL_ATTR_METERING_CURRENT_MONTH_ALT_CONSUMPTION_DELIVERED = ZB_ZCL_ATTR_SET_WITH_ATTR_ID(ZB_ZCL_METERING_SET_ALTERNATIVE_HISTORICAL_CONSUMPTION, 0x40),
  ZB_ZCL_ATTR_METERING_CURRENT_MONTH_ALT_CONSUMPTION_RECEIVED, /**< @e CurrentMonthAlternativeConsumptionReceived represents the summed value
                                                                * received from the premises since the Historical Freeze Time (HFT)
                                                                * on the 1st of the month to the last HFT read. */

  ZB_ZCL_ATTR_METERING_PREVIOUS_MONTH_ALT_CONSUMPTION_DELIVERED, /**< @e PreviousMonthNAlternativeConsumptionDelivered represents the summed value
                                                                  * delivered to the premises within the previous Month period
                                                                  * starting at the Historical Freeze Time (HFT) on the 1st of the month to the last day of the month. */
  ZB_ZCL_ATTR_METERING_PREVIOUS_MONTH_ALT_CONSUMPTION_RECEIVED, /**< @e PreviousMonthNAlternativeConsumptionReceived represents the summed value
                                                                * received from the premises within the previous month period
                                                                * starting at the Historical Freeze Time (HFT) on the 1st of the month to the last day of the month. */

  /* ZB_ZCL_METERING_SET_FOUR_QUADRANT_ELECTRICITY attribute set */
  /*NOTE: the first attribute from this attribute set starts from 0x0D01 value instead of 0x0D00 */
  /*NOTE: attribute 0x0D04 in one spec it is ZB_ZCL_ATTR_METERING_CURRENT_ACTIVE_SUMMATION_Q4_ID,
          in another it is ZB_ZCL_ATTR_METERING_CURRENT_REACTIVE_SUMMATION_Q1_ID
    NOTE: It is supposed that ZB_ZCL_ATTR_METERING_CURRENT_ACTIVE_SUMMATION_Q1_ID = 0x0D00
   */
  ZB_ZCL_ATTR_METERING_CURRENT_ACTIVE_SUMMATION_Q1_ID = ZB_ZCL_ATTR_SET_WITH_ATTR_ID(ZB_ZCL_METERING_SET_FOUR_QUADRANT_ELECTRICITY, 0x01),  /**< This attribute represents the most
                                                                                                                                             * recent summed value of Active Energy (kWh)
                                                                                                                                             * delivered in the quadrant Q1.
                                                                                                                                             */
  ZB_ZCL_ATTR_METERING_CURRENT_ACTIVE_SUMMATION_Q2_ID,    /**< This attribute represents the most
                                                         * recent summed value of Active Energy (kWh)
                                                         * delivered in the quadrant Q2.
                                                         */
  ZB_ZCL_ATTR_METERING_CURRENT_ACTIVE_SUMMATION_Q3_ID,    /**< This attribute represents the most
                                                         * recent summed value of Active Energy (kWh)
                                                         * delivered in the quadrant Q3.
                                                         */
  ZB_ZCL_ATTR_METERING_CURRENT_ACTIVE_SUMMATION_Q4_ID,    /**< This attribute represents the most
                                                         * recent summed value of Active Energy (kWh)
                                                         * delivered in the quadrant Q4.
                                                         */
  ZB_ZCL_ATTR_METERING_CURRENT_REACTIVE_SUMMATION_Q1_ID,  /**< This attribute represents the most recent
                                                         * summed value of Reactive Energy (kVarh) delivered
                                                         * in quadrant Q1.
                                                         */
  ZB_ZCL_ATTR_METERING_CURRENT_REACTIVE_SUMMATION_Q2_ID,  /**< This attribute represents the most recent
                                                         * summed value of Reactive Energy (kVarh) delivered
                                                         * in quadrant Q2.
                                                         */
  ZB_ZCL_ATTR_METERING_CURRENT_REACTIVE_SUMMATION_Q3_ID,  /**< This attribute represents the most recent
                                                         * summed value of Reactive Energy (kVarh) delivered
                                                         * in quadrant Q3.
                                                         */
  ZB_ZCL_ATTR_METERING_CURRENT_REACTIVE_SUMMATION_Q4_ID   /**< This attribute represents the most recent
                                                         * summed value of Reactive Energy (kVarh) delivered
                                                         * in quadrant Q4.
                                                         */
};


/** @brief @e CurrentBlock attribute values
 *  @see @ref ZB_ZCL_ATTR_METERING_CURRENT_BLOCK_ID
 *  @see SE 1.4 spec, Table D.12.
 */
enum zb_zcl_metering_block_e
{
  ZB_ZCL_METERING_BLOCK_NOT_USED = 0x00,    /**< No blocks in use */
  ZB_ZCL_METERING_BLOCK_1        = 0x01,    /**< Block 1 */
  ZB_ZCL_METERING_BLOCK_2        = 0x02,    /**< Block 2 */
  ZB_ZCL_METERING_BLOCK_3        = 0x03,    /**< Block 3 */
  ZB_ZCL_METERING_BLOCK_4        = 0x04,    /**< Block 4 */
  ZB_ZCL_METERING_BLOCK_5        = 0x05,    /**< Block 5 */
  ZB_ZCL_METERING_BLOCK_6        = 0x06,    /**< Block 6 */
  ZB_ZCL_METERING_BLOCK_7        = 0x07,    /**< Block 7 */
  ZB_ZCL_METERING_BLOCK_8        = 0x08,    /**< Block 8 */
  ZB_ZCL_METERING_BLOCK_9        = 0x09,    /**< Block 9 */
  ZB_ZCL_METERING_BLOCK_10       = 0x0A,    /**< Block 10 */
  ZB_ZCL_METERING_BLOCK_11       = 0x0B,    /**< Block 11 */
  ZB_ZCL_METERING_BLOCK_12       = 0x0C,    /**< Block 12 */
  ZB_ZCL_METERING_BLOCK_13       = 0x0D,    /**< Block 13 */
  ZB_ZCL_METERING_BLOCK_14       = 0x0E,    /**< Block 14 */
  ZB_ZCL_METERING_BLOCK_15       = 0x0F,    /**< Block 15 */
  ZB_ZCL_METERING_BLOCK_16       = 0x10,    /**< Block 16 */
  /* 0x11 to 0xFF reserved */
};


/** @brief @e SupplyStatus attribute values
 *  @see @ref ZB_ZCL_ATTR_METERING_SUPPLY_STATUS_ID
 *  @see SE 1.4 spec, table D.13.
 */
enum zb_zcl_metering_supply_status_e
{
  ZB_ZCL_SUPPLY_STATUS_OFF   = 0x00,    /**< Supply OFF */
  ZB_ZCL_SUPPLY_STATUS_ARMED = 0x01,    /**< Supply OFF/ARMED */
  ZB_ZCL_SUPPLY_STATUS_ON    = 0x02     /**< Supply ON */
  /* 0x03 to 0xFF reserved for future use */
};


/** @brief Bit mapping of the @e Status attribute (Electricity)
 *  @see @ref ZB_ZCL_ATTR_METERING_STATUS_ID
 *  @see SE spec, subclause 3.2.2.3.1, table D.16.
 */
enum zb_zcl_metering_status_electricity_e
{
  ZB_ZCL_METERING_ELECTRICITY_CHECK_METER             = 1 << 0,   /**< @e CheckMeter bit is set to true when a non fatal problem has been detected on the meter such as
                                                                   *   a measurement error, memory error, self check error. */
  ZB_ZCL_METERING_ELECTRICITY_LOW_BATTERY             = 1 << 1,   /**< @e LowBattery bit is set to true when the battery needs maintenance. */
  ZB_ZCL_METERING_ELECTRICITY_TAMPER_DETECT           = 1 << 2,   /**< @e TamperDetect bit is set to true if a tamper event has been detected. */
  ZB_ZCL_METERING_ELECTRICITY_POWER_FAILURE           = 1 << 3,   /**< @e PowerFailure bit is set to true during a power outage. */
  ZB_ZCL_METERING_ELECTRICITY_POWER_QUALITY           = 1 << 4,   /**< @e PowerQuality bit is set to true if a power quality event has been detected such as a low
                                                                   *   voltage, high voltage. */
  ZB_ZCL_METERING_ELECTRICITY_LEAK_DETECT             = 1 << 5,   /**< @e LeakDetect bit is set to true when a leak has been detected. */
  ZB_ZCL_METERING_ELECTRICITY_SERVICE_DISCONNECT_OPEN = 1 << 6,   /**< @e ServiceDisconnectOpen bit is Set to true when the service has been disconnected to this premises. */
  ZB_ZCL_METERING_ELECTRICITY_RESERVED                = 1 << 7    /**< Reserved bit */
};


/** @brief Bit mapping of the @e Status attribute (Gas)
 *  @see @ref ZB_ZCL_ATTR_METERING_STATUS_ID
 *  @see SE spec, subclause 3.2.2.3.1, table D.17.
 */
enum zb_zcl_metering_status_gas_e
{
  ZB_ZCL_METERING_GAS_CHECK_METER                     = 1 << 0,   /**< @e CheckMeter bit is Set to true when a non fatal problem has been detected on the meter such as
                                                                   *   a measurement error, memory error, or self check error. */
  ZB_ZCL_METERING_GAS_LOW_BATTERY                     = 1 << 1,   /**< @e LowBattery bit is set to true when the battery needs maintenance. */
  ZB_ZCL_METERING_GAS_TAMPER_DETECT                   = 1 << 2,   /**< @e TamperDetect bit is set to true if a tamper event has been detected. */
  ZB_ZCL_METERING_GAS_RESERVED                        = 1 << 3,   /**< Reserved bit */
  ZB_ZCL_METERING_GAS_LOW_PRESSURE                    = 1 << 4,   /**< @e LowPressure bit is set to true when the pressure at the meter is below the meter's low
                                                                   *   pressure threshold value. */
  ZB_ZCL_METERING_GAS_LEAK_DETECT                     = 1 << 5,   /**< @e LeakDetect bit is set to true when a leak has been detected. */
  ZB_ZCL_METERING_GAS_SERVICE_DISCONNECT              = 1 << 6,   /**< @e ServiceDisconnect bit is set to true when the service has been disconnected to this premises.
                                                                   *   Ex. The valve is in the closed position preventing delivery of gas. */
  ZB_ZCL_METERING_GAS_REVERSE_FLOW                    = 1 << 7    /**< @e ReverseFlow bit is set to true if flow detected in the opposite direction to normal (from
                                                                   *   consumer to supplier). */
};


/** @brief Bit mapping of the @e Status attribute (Water)
 *  @see @ref ZB_ZCL_ATTR_METERING_STATUS_ID
 *  @see SE 1.4 spec, subclause 3.2.2.3.1, table D.18.
 */
enum zb_zcl_metering_status_water_e
{
  ZB_ZCL_METERING_WATER_CHECK_METER                   = 1 << 0,   /**< @e CheckMeter bit is Set to true when a non fatal problem has been detected on the meter such as
                                                                   *   a measurement error, memory error, or self check error. */
  ZB_ZCL_METERING_WATER_LOW_BATTERY                   = 1 << 1,   /**< @e LowBattery bit is set to true when the battery needs maintenance. */
  ZB_ZCL_METERING_WATER_TAMPER_DETECT                 = 1 << 2,   /**< @e TamperDetect bit set to true if a tamper event has been detected. */
  ZB_ZCL_METERING_WATER_PIPE_EMTPY                    = 1 << 3,   /**< @e PipeEmpty bit is set to true when the service pipe at the meter is empty and there is no flow
                                                                   *   in either direction. */
  ZB_ZCL_METERING_WATER_LOW_PRESSURE                  = 1 << 4,   /**< @e LowPressure bit is Set to true when the pressure at the meter is below the meter's low
                                                                   *   pressure threshold value. */
  ZB_ZCL_METERING_WATER_LEAK_DETECT                   = 1 << 5,   /**< @e LeakDetect bit is set to true when a leak has been detected. */
  ZB_ZCL_METERING_WATER_SERVICE_DISCONNECT            = 1 << 6,   /**< @e ServiceDisconnect bit is set to true when the service has been disconnected to this premises.
                                                                   *   Ex. The valve is in the closed position preventing delivery of water. */
  ZB_ZCL_METERING_WATER_REVERSE_FLOW                  = 1 << 7    /**< @e ReverseFlow bit is Set to true if flow detected in the opposite direction to normal (from
                                                                   *   consumer to supplier). */
};


/** @brief Bit mapping of the @e Status attribute (Heat and Cooling)
 *  @see @ref ZB_ZCL_ATTR_METERING_STATUS_ID
 *  @see SE 1.4 spec, subclause 3.2.2.3.1, table D.19.
 */
enum zb_zcl_metering_status_heat_cooling_e
{
  ZB_ZCL_METERING_HCOOL_CHECK_METER                   = 1 << 0,   /**< @e CheckMeter bit is Set to true when a non fatal problem has been detected on the meter such
                                                                   *   as a measurement error, memory error, or self check error. */
  ZB_ZCL_METERING_HCOOL_LOW_BATTERY                   = 1 << 1,   /**< @e LowBattery bit is set to true when the battery needs maintenance. */
  ZB_ZCL_METERING_HCOOL_TAMPER_DETECT                 = 1 << 2,   /**< @e TamperDetect bit is set to true if a tamper event has been detected. */
  ZB_ZCL_METERING_HCOOL_TEMPERATURE_SENSOR            = 1 << 3,   /**< @e TemperatureSensor bit is Set to true when an error is detected on a temperature sensor at
                                                                   *   this premises. */
  ZB_ZCL_METERING_HCOOL_BURST_DETECT                  = 1 << 4,   /**< @e BurstDetect bit is set to true when a burst is detected on pipes at this premises. */
  ZB_ZCL_METERING_HCOOL_LEAK_DETECT                   = 1 << 5,   /**< @e LeakDetect bit is set to true when a leak has been detected. */
  ZB_ZCL_METERING_HCOOL_SERVICE_DISCONNECT            = 1 << 6,   /**< @e ServiceDisconnect bit is Set to true when the service has been disconnected to this premises.
                                                                   *   Ex. The valve is in the closed position preventing delivery of heat or cooling. */
  ZB_ZCL_METERING_HCOOL_FLOW_SENSOR                   = 1 << 7    /**< @e FlowSensor bit is set to true when an error is detected on a flow sensor at this premises. */
};


/** @brief Bit mapping of the @e ExtendedStatus attribute (General Flags)
 *  @see @ref ZB_ZCL_ATTR_METERING_EXTENDED_STATUS_ID
 *  @see SE 1.4 spec, subclause D.3.2.2.3.5, table D.20
 */
enum zb_zcl_metering_extstatus_general_e
{
  ZB_ZCL_METERING_METER_COVER_REMOVED            = 1 << 0,    /**< @e MeterCoverRemoved bit is set to true when the device detects the meter
                                                                *   cover being removed. */
  ZB_ZCL_METERING_STRONG_MAGNETIC_FIELD_DETECTED = 1 << 1,    /**< @e StrongMagneticFieldDetected bit is set to true when the device detects
                                                                *   presence of a strong magnetic field */
  ZB_ZCL_METERING_BATTERY_FAILURE                = 1 << 2,    /**< @e BatteryFailure bit is set to true when the device detects that its battery
                                                                *   has failed. */
  ZB_ZCL_METERING_PROGRAM_MEMORY_ERROR           = 1 << 3,    /**< @e ProgramMemoryError bit is set to true when the device detects an error
                                                                *   within its program (non-volatile) memory. */
  ZB_ZCL_METERING_RAM_ERROR                      = 1 << 4,    /**< @e RAMError bit is set to true when the device detects an instance of a
                                                                *   Random Access Memory error within the device memory. */
  ZB_ZCL_METERING_NV_MEMORY_ERROR                = 1 << 5,    /**< @e NVMemoryError bit is set to true when the device detects an instance of a
                                                                *   Non Volatile memory error within the device memory - this is a fatal meter
                                                                *   error that will require the meter replacement */
  ZB_ZCL_METERING_MEASUREMENT_SYSTEM_ERROR       = 1 << 6,    /**< @e MeasurementSystemError bit is set to true when the device detects an error
                                                                *   within its measurement system. */
  ZB_ZCL_METERING_WATCHDOG_ERROR                 = 1 << 7,    /**< @e WatchdogError bit is set to true when the device has detected an instance
                                                                *   of a watchdog reset event (following a catastrophic fault within the device). */
  ZB_ZCL_METERING_SUPPLY_DISCONNECT_FAILURE      = 1 << 8,    /**< @e SupplyDisconnectFailure bit is set to true when the device has detected
                                                                *   that the valve has not closed as expected (for gas) or the contactor has not
                                                                *   opened as expected (for electricity). */
  ZB_ZCL_METERING_SUPPLY_CONNECT_FAILURE         = 1 << 9,    /**< @e SupplyConnectFailure bit is set to true when the device has detected that
                                                                *   the valve has not opened as expected (for gas) or the contactor has not closed
                                                                *   as expected (for electricity). */
  ZB_ZCL_METERING_MEASUREMENT_SW_CHANGED         = 1 << 10,   /**< @e MeasurementSWChanged/Tampered bit is set to true when the device detects that
                                                                *   its measurement software has changed. */
  ZB_ZCL_METERING_CLOCK_INVALID                  = 1 << 11,   /**< @e ClockInvalid bit is set to true when the device detects that its internal clock
                                                                *   is invalid. */
  ZB_ZCL_METERING_TEMPERATURE_EXCEEDED           = 1 << 12,   /**< @e TemperatureExceeded bit is set to true when the metering device's temperature
                                                                *   exceeds a predefined limit. There are various reasons for temperature rise in
                                                                *   metering devices. */
  ZB_ZCL_METERING_MOISTURE_DETECTED              = 1 << 13    /**< @e MoistureDetected bit is set to true when a sensor has detected the presence of
                                                                *   moisture e.g. moisture in a gas line which can cause a drop in gas pressure, or
                                                                *   moisture detected in the sealed component area within a water meter. */
  /* bits 14-23 reserved */
};


/** @brief Bit mapping of the @e ExtendedStatus attribute (Electricity Meter specific Flags)
 *  @see @ref ZB_ZCL_ATTR_METERING_EXTENDED_STATUS_ID
 *  @see SE 1.4 spec, subclause D.3.2.2.3.5, table D.21
 */
enum zb_zcl_metering_extstatus_electricity_e
{
  ZB_ZCL_METERING_ELECTRICITY_TERMINAL_COVER_REMOVED     = 1L << 24,     /**< @e TerminalCoverRemoved bit is set to true when the device detects that its
                                                                        *   terminal cover has been removed. */
  ZB_ZCL_METERING_ELECTRICITY_INCORRECT_POLARITY         = 1L << 25,     /**< @e IncorrectPolarity bit is set to true when the electricity meter detects
                                                                        *   incorrect polarity on the electricity supply. */
  ZB_ZCL_METERING_ELECTRICITY_CURRENT_WITH_NO_VOLTAGE    = 1L << 26,     /**< @e CurrentWithNoVoltage bit is set to true when the meter has been tampered
                                                                        *   with, to disconnect the measurement function from the supply. Electricity is
                                                                        *   still flowing but not being recorded. */
  ZB_ZCL_METERING_ELECTRICITY_LIMIT_THRESHOLD_EXCEEDED   = 1L << 27,     /**< @e LimitThresholdExceeded bit is set to true when the electricity meter detects
                                                                        *   that the load has exceeded the load limit threshold. */
  ZB_ZCL_METERING_ELECTRICITY_UNDER_VOLTAGE              = 1L << 28,     /**< @e UnderVoltage bit is set to true when the electricity meter indicates that
                                                                        *   the voltage measurement over the voltage measurement period is lower than the
                                                                        *   voltage threshold. */
  ZB_ZCL_METERING_ELECTRICITY_OVER_VOLTAGE               = 1L << 29,     /**< @e OverVoltage bit is set to true when the electricity meter indicates that the
                                                                       *   voltage measurement over the voltage measurement period is higher than the
                                                                       *   voltage threshold. */
  ZB_ZCL_METERING_ELECTRICITY_BIDIRECTION_OPERATION      = 1L << 30,     /**<Set to true when the meter is capable of measuring energy in both directions
                                                                           * (i.e. delivered and received). */
  /*Each enumerator that appears in the body of an enumeration specifier becomes an integer constant with type int in the enclosing scope.*/
  ZB_ZCL_METERING_ELECTRICITY_ACTIVE_POWER_RECEIVED      = -2147483648LL /**< Set to true when active power flows in the received direction
                                                                           * (generation from the premises to the grid). */
  /* bits 32-63 reserved */
};


/** @brief Bit mapping of the @e ExtendedStatus attribute (Gas Meter specific Flags)
 *  @see @ref ZB_ZCL_ATTR_METERING_EXTENDED_STATUS_ID
 *  @see SE 1.4 spec, subclause D.3.2.2.3.5, table D.22
 */
enum zb_zcl_metering_extstatus_gas_e
{
  ZB_ZCL_METERING_GAS_BATTERY_COVER_REMOVED              = 1L << 24,   /**< @e BatteryCoverRemoved bit is set to true when the gas meter detects that its
                                                                        *   battery cover has been removed. */
  ZB_ZCL_METERING_GAS_TILT_TAMPER                        = 1L << 25,   /**< @e TiltTamper bit is set to true when the meter detects a change in its physical
                                                                        *   properties (i.e. that it is being tilted, the tilt sensor has been activated or
                                                                        *   otherwise tampered with). */
  ZB_ZCL_METERING_GAS_EXCESS_FLOW                        = 1L << 26    /**< @e ExcessFlow bit is set to true when the gas meter detects excess flow (e.g. when
                                                                        *   local supply restoration is attempted). */
  /* bits 26-63 reserved */
};


/** @brief @e LowMediumHighStatus Attribute values
 *  @see @ref ZB_ZCL_ATTR_METERING_AMBIENT_CONSUMPTION_INDICATOR_ID
 *  @see SE 1.4 spec, table D.23.
 */
enum zb_zcl_metering_low_medium_high_status_e
{
  ZB_ZCL_METERING_STATUS_LOW    = 0x00,    /**< Low energy usage */
  ZB_ZCL_METERING_STATUS_MEDIUM = 0x01,    /**< Medium energy usage */
  ZB_ZCL_METERING_STATUS_HIGH   = 0x02,    /**< High energy usage */
};


/** @brief @e UnitOfMeasure Attribute values
 *  @see @ref ZB_ZCL_ATTR_METERING_UNIT_OF_MEASURE_ID
    @see SE 1.4 spec, subclause 3.2.2.4.1, table D.26
 */
enum zb_zcl_metering_unit_of_measure_e
{
  ZB_ZCL_METERING_UNIT_KW_KWH_BINARY       = 0x00,   /**< kW, kWh binary value */
  ZB_ZCL_METERING_UNIT_M3_M3H_BINARY       = 0x01,   /**< m3, m3/h binary value */
  ZB_ZCL_METERING_UNIT_FT3_FT3H_BINARY     = 0x02,   /**< ft3, ft3/h binary value */
  ZB_ZCL_METERING_UNIT_CCF_CCFH_BINARY     = 0x03,   /**< ccf, ccf/h binary value */
  ZB_ZCL_METERING_UNIT_USGL_USGLH_BINARY   = 0x04,   /**< US gl, US gl/h binary value */
  ZB_ZCL_METERING_UNIT_IMPGL_IMPGLH_BINARY = 0x05,   /**< IMP gl, IMP gl/h binary value */
  ZB_ZCL_METERING_UNIT_BTU_BTUH_BINARY     = 0x06,   /**< BTU, BTU/h binary value */
  ZB_ZCL_METERING_UNIT_L_LH_BINARY         = 0x07,   /**< l, l/h binary value */
  ZB_ZCL_METERING_UNIT_KPAG_BINARY         = 0x08,   /**< kPA (gauge) binary value */
  ZB_ZCL_METERING_UNIT_KPAA_BINARY         = 0x09,   /**< kPA (absolute) binary value */
  ZB_ZCL_METERING_UNIT_MCF_MCFH_BINARY     = 0x0A,   /**< mcf, mcf/h binary value */
  ZB_ZCL_METERING_UNIT_UNITLESS_BINARY     = 0x0B,   /**< Unitless binary value */
  ZB_ZCL_METERING_UNIT_MJ_MJS_BINARY       = 0x0C,   /**< MJ, MJ/h binary value */
  ZB_ZCL_METERING_UNIT_KVAR_KVARH_BINARY   = 0x0D,   /**< kVar, kVar/h binary value */

  ZB_ZCL_METERING_UNIT_BINARY_RESERVED     = 0x0E,   /**< 0x0E to 0x7F reserved for future use */


  ZB_ZCL_METERING_UNIT_KW_KWH_BCD          = 0x80,   /**< kW, kWh BCD value */
  ZB_ZCL_METERING_UNIT_M3_M3H_BCD          = 0x81,   /**< m3, m3/h BCD value */
  ZB_ZCL_METERING_UNIT_FT3_FT3H_BCD        = 0x82,   /**< ft3, ft3/h BCD value */
  ZB_ZCL_METERING_UNIT_CCF_CCFH_BCD        = 0x83,   /**< ccf, ccf/h BCD value */
  ZB_ZCL_METERING_UNIT_USGL_USGLH_BCD      = 0x84,   /**< US gl, US gl/h BCD value */
  ZB_ZCL_METERING_UNIT_IMPGL_IMPGLH_BCD    = 0x85,   /**< IMP gl, IMP gl/h BCD value */
  ZB_ZCL_METERING_UNIT_BTU_BTUH_BCD        = 0x86,   /**< BTU, BTU/h BCD value */
  ZB_ZCL_METERING_UNIT_L_LH_BCD            = 0x87,   /**< l, l/h BCD value */
  ZB_ZCL_METERING_UNIT_KPAG_BCD            = 0x88,   /**< kPA (gauge) BCD value */
  ZB_ZCL_METERING_UNIT_KPAA_BCD            = 0x89,   /**< kPA (absolute) BCD value */
  ZB_ZCL_METERING_UNIT_MCF_MCFH_BCD        = 0x8A,   /**< mcf, mcf/h BCD value */
  ZB_ZCL_METERING_UNIT_UNITLESS_BCD        = 0x8B,   /**< Unitless BCD value */
  ZB_ZCL_METERING_UNIT_MJ_MJS_BCD          = 0x8C,   /**< MJ, MJ/h BCD value */
  ZB_ZCL_METERING_UNIT_KVAR_KVARH_BCD      = 0x8D,   /**< kVar, kVar/h BCD value */

  ZB_ZCL_METERING_UNIT_BCD_RESERVED        = 0x8E    /**< 0x8E to 0xFF reserved for future use */
};


/** @brief @e DeviceType Attribute values
 *  @see @ref ZB_ZCL_ATTR_METERING_METERING_DEVICE_TYPE_ID
 *  @see SE 1.4 spec, subclause 3.2.2.4.7, table D.27
 */
enum zb_zcl_metering_device_type_e
{
  ZB_ZCL_METERING_ELECTRIC_METERING                   = 0,   /**< Electric metering device */
  ZB_ZCL_METERING_GAS_METERING                        = 1,   /**< Gas metering device */
  ZB_ZCL_METERING_WATER_METERING                      = 2,   /**< Water metering device */
  ZB_ZCL_METERING_PRESSURE_METERING                   = 4,   /**< Pressure metering device */
  ZB_ZCL_METERING_HEAT_METERING                       = 5,   /**< Heat metering device */
  ZB_ZCL_METERING_COOLING_METERING                    = 6,   /**< Cooling metering device */
  ZB_ZCL_METERING_EUMD                                = 7,   /**< End Use Measurement Device (EUMD) for metering electric vehicle charging */
  ZB_ZCL_METERING_PV_GENERATION_METERING              = 8,   /**< PV Generation Metering device */
  ZB_ZCL_METERING_WIND_TURBINE_GENERATION_METERING    = 9,   /**< Wind Turbine Generation Metering device */
  ZB_ZCL_METERING_WATER_TURBINE_GENERATION_METERING   = 10,  /**< Water Turbine Generation Metering device */
  ZB_ZCL_METERING_MICRO_GENERATION_METERING           = 11,  /**< Micro Generation Metering device */
  ZB_ZCL_METERING_SOLAR_HOT_WATER_GENERATION_METERING = 12,  /**< Solar Hot Water Generation Metering device */
  ZB_ZCL_METERING_ELECTRIC_METERING_ELEMENT_PHASE1    = 13,  /**< Electric Metering Element/Phase 1 */
  ZB_ZCL_METERING_ELECTRIC_METERING_ELEMENT_PHASE2    = 14,  /**< Electric Metering Element/Phase 2 */
  ZB_ZCL_METERING_ELECTRIC_METERING_ELEMENT_PHASE3    = 15,  /**< Electric Metering Element/Phase 3 */
  /* 16 to 126 reserved for future growth */


  ZB_ZCL_METERING_MIRRORED_ELECTRIC_METERING                   = 127,   /**< Mirrored Electric Metering device */
  ZB_ZCL_METERING_MIRRORED_GAS_METERING                        = 128,   /**< Mirrored Gas Metering device */
  ZB_ZCL_METERING_MIRRORED_WATER_METERING                      = 129,   /**< Mirrored Water Metering device */
  /* 130 == Mirrored Thermal Metering (deprecated) */
  ZB_ZCL_METERING_MIRRORED_PRESSURE_METERING                   = 131,   /**< Mirrored Pressure Metering device */
  ZB_ZCL_METERING_MIRRORED_HEAT_METERING                       = 132,   /**< Mirrored Heat Metering device */
  ZB_ZCL_METERING_MIRRORED_COOLING_METERING                    = 133,   /**< Mirrored Cooling Metering device */
  ZB_ZCL_METERING_MIRRORED_EUMD                                = 134,   /**< Mirrored End Use Measurement Device (EUMD) for metering electric vehicle charging */
  ZB_ZCL_METERING_MIRRORED_PV_GENERATION_METERING              = 135,   /**< Mirrored PV Generation Metering device */
  ZB_ZCL_METERING_MIRRORED_WIND_TURBINE_GENERATION_METERING    = 136,   /**< Mirrored Wind Turbine Generation Metering device */
  ZB_ZCL_METERING_MIRRORED_WATER_TURBINE_GENERATION_METERING   = 137,   /**< Mirrored Water Turbine Generation Metering device */
  ZB_ZCL_METERING_MIRRORED_MICRO_GENERATION_METERING           = 138,   /**< Mirrored Micro Generation Metering device */
  ZB_ZCL_METERING_MIRRORED_SOLAR_HOT_WATER_GENERATION_METERING = 139,   /**< Mirrored Solar Hot Water Generation Metering device */
  ZB_ZCL_METERING_MIRRORED_ELECTRIC_METERING_ELEMENT_PHASE1    = 140,   /**< Mirrored Electric Metering Element/Phase 1 */
  ZB_ZCL_METERING_MIRRORED_ELECTRIC_METERING_ELEMENT_PHASE2    = 141,   /**< Mirrored Electric Metering Element/Phase 2 */
  ZB_ZCL_METERING_MIRRORED_ELECTRIC_METERING_ELEMENT_PHASE3    = 142,   /**< Mirrored Electric Metering Element/Phase 3 */
  /* 143 to 255 reserved for future growth */

  ZB_ZCL_METERING_RESERVED                                     = 143
};


/** @brief @e TemperatureUnitOfMeasure Attribute values
 *  @see @ref ZB_ZCL_ATTR_METERING_TEMPERATURE_UNIT_OF_MEASURE_ID
 *  @see SE 1.4 spec, subclause 3.2.2.4.13, table D.28
 */
enum zb_zcl_metering_temperature_unit_of_measure_e
{
  ZB_ZCL_METERING_TEMPERATURE_UNIT_KELVIN_BINARY     = 0x00,   /**< K (Degrees Kelvin) in pure Binary format. */
  ZB_ZCL_METERING_TEMPERATURE_UNIT_CELSIUS_BINARY    = 0x01,   /**< C (Degrees Celsius) in pure Binary format. */
  ZB_ZCL_METERING_TEMPERATURE_UNIT_FAHRENHEIT_BINARY = 0x02,   /**< F (Degrees Fahrenheit) in pure Binary format. */
  /* 0x03 - 0x7F reserved for future use */
  ZB_ZCL_METERING_TEMPERATURE_UNIT_KELVIN_BCD        = 0x80,   /**< K (Degrees Kelvin) in BCD format. */
  ZB_ZCL_METERING_TEMPERATURE_UNIT_CELSIUS_BCD       = 0x81,   /**< C (Degrees Celsius) in BCD format. */
  ZB_ZCL_METERING_TEMPERATURE_UNIT_FAHRENHEIT_BCD    = 0x82,   /**< F (Degrees Fahrenheit) in BCD format. */
  /* 0x83 - 0xFF reserved for future use */
};

/**
 * The generic Alarm Group maps the status from the @e MeterStatus attribute into a corresponding
 * alarm.
 * @see SE spec, Table D-35
 */
enum zb_zcl_metering_generic_alarm_group_e
{
  ZB_ZCL_METERING_ALARM_CHECK_METER                = 0x00, /**< Check meter */
  ZB_ZCL_METERING_ALARM_LOW_BATTERY                = 0x01, /**< Low battery */
  ZB_ZCL_METERING_ALARM_TAMPER_DETECT              = 0x02, /**< Tamper detect */
  ZB_ZCL_METERING_ALARM_ELECTRICITY_POWER_FAILURE  = 0x03, /**< Electricity: Power Failure */
  ZB_ZCL_METERING_ALARM_WATER_PIPE_EMPTY           = 0x03, /**< Water: Pipe Empty */
  ZB_ZCL_METERING_ALARM_HEAT_TEMPERATURE_SENSOR    = 0x03, /**< Heat: Temperature Sensor */
  ZB_ZCL_METERING_ALARM_COOLING_TEMPERATURE_SENSOR = 0x03, /**< Cooling Temperature Sensor */
  ZB_ZCL_METERING_ALARM_ELECTRICITY_POWER_QUALITY    = 0x04, /**< Electricity: Power Quality */
  ZB_ZCL_METERING_ALARM_GAS_LOW_PRESSURE           = 0x04, /**< Gas: Low Pressure */
  ZB_ZCL_METERING_ALARM_WATER_LOW_PRESSURE         = 0x04, /**< Water: Low Pressure */
  ZB_ZCL_METERING_ALARM_HEAT_BURST_DETECT          = 0x04, /**< Heat: Burst Detect */
  ZB_ZCL_METERING_ALARM_COOLING_BURST_DETECT       = 0x04, /**< Cooling: Burst Detect */
  ZB_ZCL_METERING_ALARM_LEAK_DETECT                = 0x05, /**< Leak detect */
  ZB_ZCL_METERING_ALARM_SERVICE_DISCONNECT         = 0x06, /**< Service Disconnect */
  ZB_ZCL_METERING_ALARM_ELECTRICITY_RESERVED       = 0x07, /**< Electricity: Reserved */
  ZB_ZCL_METERING_ALARM_GAS_REVERSE_FLOW           = 0x07, /**< Gas: Reverse Flow */
  ZB_ZCL_METERING_ALARM_WATER_REVERSE_FLOW         = 0x07, /**< Water: Reverse Flow */
  ZB_ZCL_METERING_ALARM_HEAT_FLOW_SENSOR           = 0x07, /**< Heat: Flow Sensor */
  ZB_ZCL_METERING_ALARM_COOLING_FLOW_SENSOR        = 0x07, /**< Cooling: Flow Sensor */
  ZB_ZCL_METERING_ALARM_METER_COVER_REMOVED        = 0x08, /**< Meter Cover Removed */
  ZB_ZCL_METERING_ALARM_METER_COVER_CLOSED         = 0x09, /**< Meter Cover Closed */
  ZB_ZCL_METERING_ALARM_STRONG_MAGNETIC_FIELD      = 0x0A, /**< Strong Magnetic Field */
  ZB_ZCL_METERING_ALARM_NO_STRONG_MAGNETIC_FIELD   = 0x0B, /**< No Strong Magnetic Field */
  ZB_ZCL_METERING_ALARM_BATTERY_FAILURE            = 0x0C, /**< Battery Failure */
  ZB_ZCL_METERING_ALARM_PROGRAM_MEMORY_ERROR       = 0x0D, /**< Program Memory Error */
  ZB_ZCL_METERING_ALARM_RAM_ERROR                  = 0x0E, /**< Ram Error */
  ZB_ZCL_METERING_ALARM_NV_MEMORY_ERROR            = 0x0F  /**< NV Memory Error */
};


/**
 * The Electricity Alarm Group defines alarms specific for electricity meters.
 * @see SE spec, Table D-36
 */
enum zb_zcl_metering_electricity_alarm_group_e
{
  ZB_ZCL_METERING_ALARM_LOW_VOLTAGE_L1         = 0x10,  /**< Low Voltage L1 */
  ZB_ZCL_METERING_ALARM_HIGH_VOLTAGE_L1        = 0x11,  /**< High Voltage L1 */
  ZB_ZCL_METERING_ALARM_LOW_VOLTAGE_L2         = 0x12,  /**< Low Voltage L2 */
  ZB_ZCL_METERING_ALARM_HIGH_VOLTAGE_L2        = 0x13,  /**< High Voltage L2 */
  ZB_ZCL_METERING_ALARM_LOW_VOLTAGE_L3         = 0x14,  /**< Low Voltage L3 */
  ZB_ZCL_METERING_ALARM_HIGH_VOLTAGE_L3        = 0x15,  /**< High Voltage L3 */
  ZB_ZCL_METERING_ALARM_OVER_CURRENT_L1        = 0x16,  /**< Over Current L1 */
  ZB_ZCL_METERING_ALARM_OVER_CURRENT_L2        = 0x17,  /**< Over Current L2 */
  ZB_ZCL_METERING_ALARM_OVER_CURRENT_L3        = 0x18,  /**< Over Current L3 */
  ZB_ZCL_METERING_ALARM_FREQUENCY_TOO_LOW_L1   = 0x19,  /**< Frequency too Low L1 */
  ZB_ZCL_METERING_ALARM_FREQUENCY_TOO_HIGH_L1  = 0x1A,  /**< Frequency too High L1 */
  ZB_ZCL_METERING_ALARM_FREQUENCY_TOO_LOW_L2   = 0x1B,  /**< Frequency too Low L2 */
  ZB_ZCL_METERING_ALARM_FREQUENCY_TOO_HIGH_L2  = 0x1C,  /**< Frequency too High L2 */
  ZB_ZCL_METERING_ALARM_FREQUENCY_TOO_LOW_L3   = 0x1D,  /**< Frequency too Low L3 */
  ZB_ZCL_METERING_ALARM_FREQUENCY_TOO_HIGH_L3  = 0x1E,  /**< Frequency too High L3 */
  ZB_ZCL_METERING_ALARM_GROUND_FAULT           = 0x1F,  /**< Ground Fault */
  ZB_ZCL_METERING_ALARM_ELECTRIC_TAMPER_DETECT = 0x20,  /**< Electric Tamper Detect */
  ZB_ZCL_METERING_ALARM_INCORRECT_POLARITY     = 0x21,  /**< Incorrect Polarity */
  ZB_ZCL_METERING_ALARM_CURRENT_NO_VOLTAGE     = 0x22,  /**< Current No Voltage */
  ZB_ZCL_METERING_ALARM_UNDER_VOLTAGE          = 0x23,  /**< Under Voltage */
  ZB_ZCL_METERING_ALARM_OVER_VOLTAGE           = 0x24,  /**< Over Voltage */
  ZB_ZCL_METERING_ALARM_NORMAL_VOLTAGE         = 0x25,  /**< Normal Voltage */
  ZB_ZCL_METERING_ALARM_PF_BELOW_THRESHOLD     = 0x26,  /**< PF Below Threshold */
  ZB_ZCL_METERING_ALARM_PF_ABOVE_THRESHOLD     = 0x27,  /**< PF Above Threshold */
  ZB_ZCL_METERING_ALARM_TERMINAL_COVER_REMOVED = 0x28,  /**< Terminal Cover Removed */
  ZB_ZCL_METERING_ALARM_TERMINAL_COVER_CLOSED  = 0x29,  /**< Terminal Cover Closed */
};


/**
 * The Generic Flow/Pressure Alarm Group defines alarms specific for Flow/Pressure based meters
 * i.e. Water, Heat, Cooling, or Gas meters.
 * @see SE spec, Table D-37
 */
enum zb_zcl_metering_generic_flow_pressure_alarm_group_e
{
  ZB_ZCL_METERING_ALARM_BURST_DETECT                    = 0x30,  /**< Burst detect */
  ZB_ZCL_METERING_ALARM_PRESSURE_TOO_LOW                = 0x31,  /**< Pressure too low */
  ZB_ZCL_METERING_ALARM_PRESSURE_TOO_HIGH               = 0x32,  /**< Pressure too high */
  ZB_ZCL_METERING_ALARM_FLOW_SENSOR_COMMUNICATION_ERROR = 0x33,  /**< Flow sensor communication error */
  ZB_ZCL_METERING_ALARM_FLOW_SENSOR_MEASUREMENT_FAULT   = 0x34,  /**< Flow sensor measurement fault */
  ZB_ZCL_METERING_ALARM_FLOW_SENSOR_REVERSE_FLOW        = 0x35,  /**< Flow sensor reverse flow */
  ZB_ZCL_METERING_ALARM_FLOW_SENSOR_AIR_DETECT          = 0x36,  /**< Flow sensor air detect */
  ZB_ZCL_METERING_ALARM_PIPE_EMPTY                      = 0x37,  /**< Pipe empty */
};


/**
 * The Heat and Cooling Specific Alarm Group defines alarms specific for Heat or Cooling meters
 * @see SE spec, Table D-39
 */
enum zb_zcl_metering_heat_and_cooling_specific_alarm_group_e
{
  ZB_ZCL_METERING_ALARM_INLET_TEMPERATURE_SENSOR_FAULT  = 0x50,  /**< Inlet Temperature Sensor Fault */
  ZB_ZCL_METERING_ALARM_OUTLET_TEMPERATURE_SENSOR_FAULT = 0x51,  /**< Outlet Temperature Sensor Fault */
};


/**
 * The Gas Specific Alarm Group defines alarms specific for Gas meters as defined below.
 * @see SE spec, Table D-40
 */
enum zb_zcl_metering_gas_specific_alarm_group_e
{
  ZB_ZCL_METERING_ALARM_TILT_TAMPER           = 0x60,  /**< Tilt Tamper */
  ZB_ZCL_METERING_ALARM_BATTERY_COVER_REMOVED = 0x61,  /**< Battery Cover Removed */
  ZB_ZCL_METERING_ALARM_BATTERY_COVER_CLOSED  = 0x62,  /**< Battery Cover Closed */
  ZB_ZCL_METERING_ALARM_EXCESS_FLOW           = 0x63,  /**< Excess Flow */
  ZB_ZCL_METERING_ALARM_TILT_TAMPER_ENDED     = 0x64,  /**< Tilt Tamper Ended */
};


/**
 * The Extended Generic Alarm Group is an additional set of generic meter alarms.
 * @see SE spec, Table D-41
 */
enum zb_zcl_metering_extended_generic_alarm_group_e
{
  ZB_ZCM_METERING_ALARM_MEASUREMENT_SYSTEM_ERROR                     = 0x70,  /**< Measurement System Error */
  ZB_ZCM_METERING_ALARM_WATCHDOG_ERROR                               = 0x71,  /**< Watchdog Error */
  ZB_ZCM_METERING_ALARM_SUPPLY_DISCONNECT_FAILURE                    = 0x72,  /**< Supply Disconnect Failure */
  ZB_ZCM_METERING_ALARM_SUPPLY_CONNECT_FAILURE                       = 0x73,  /**< Supply Connect Failure */
  ZB_ZCM_METERING_ALARM_MEASUREMENT_SOFTWARE_CHANGED                  = 0x74,  /**< Measurement Software Changed */
  ZB_ZCM_METERING_ALARM_DST_ENABLED                                  = 0x75,  /**< DST enabled */
  ZB_ZCM_METERING_ALARM_DST_DISABLED                                 = 0x76,  /**< DST disabled */
  ZB_ZCM_METERING_ALARM_CLOCK_ADJ_BACKWARD                           = 0x77,  /**< Clock Adj Backward (the internal clock
                                                                             has applied a negative adjustment) */
  ZB_ZCM_METERING_ALARM_CLOCK_ADJ_FORWARD                            = 0x78,  /**< Clock Adj Forward (the internal clock
                                                                             has applied a positive adjustment) */
  ZB_ZCM_METERING_ALARM_CLOCK_INVALID                                = 0x79,  /**< Clock Invalid */
  ZB_ZCM_METERING_ALARM_COMMUNICATION_ERROR_HAN                      = 0x7A,  /**< Communication Error HAN */
  ZB_ZCM_METERING_ALARM_COMMUNICATION_OK_HAN                         = 0x7B,  /**< Communication OK HAN */
  ZB_ZCM_METERING_ALARM_METER_FRAUD_ATTEMPT                          = 0x7C,  /**< Meter Fraud Attempt */
  ZB_ZCM_METERING_ALARM_POWER_LOSS                                   = 0x7D,  /**< Power Loss */
  ZB_ZCM_METERING_ALARM_UNUSUAL_HAN_TRAFFIC                          = 0x7E,  /**< Unusual HAN Traffic */
  ZB_ZCM_METERING_ALARM_UNEXPECTED_CLOCK_CHANGE                      = 0x7F,  /**< Unexpected Clock Change */
  ZB_ZCM_METERING_ALARM_COMMS_USING_UNAUTHENTICATED_COMPONENT        = 0x80,  /**< Comms Using Unauthenticated Component */
  ZB_ZCM_METERING_ALARM_ERROR_REG_CLEAR                              = 0x81,  /**< Error Reg Clear */
  ZB_ZCM_METERING_ALARM_ALARM_REG_CLEAR                              = 0x82,  /**< Alarm Reg Clear */
  ZB_ZCM_METERING_ALARM_UNEXPECTED_HW_RESET                          = 0x83,  /**< Unexpected HW Reset */
  ZB_ZCM_METERING_ALARM_UNEXPECTED_PROGRAM_EXECUTION                 = 0x84,  /**< Unexpected Program Execution */
  ZB_ZCM_METERING_ALARM_EVENTLOG_CLEARED                             = 0x85,  /**< EventLog Cleared */
  ZB_ZCM_METERING_ALARM_LIMIT_THRESHOLD_EXCEEDED                     = 0x86,  /**< Limit Threshold Exceeded */
  ZB_ZCM_METERING_ALARM_LIMIT_THRESHOLD_OK                           = 0x87,  /**< Limit Threshold OK */
  ZB_ZCM_METERING_ALARM_LIMIT_THRESHOLD_CHANGED                      = 0x88,  /**< Limit Threshold Changed */
  ZB_ZCM_METERING_ALARM_MAXIMUM_DEMAND_EXCEEDED                      = 0x89,  /**< Maximum Demand Exceeded */
  ZB_ZCM_METERING_ALARM_PROFILE_CLEARED                              = 0x8A,  /**< Profile Cleared */
  ZB_ZCM_METERING_ALARM_SAMPLING_BUFFER_CLEARED                      = 0x8B,  /**< Sampling Buffer cleared */
  ZB_ZCM_METERING_ALARM_BATTERY_WARNING                              = 0x8C,  /**< Battery Warning */
  ZB_ZCM_METERING_ALARM_WRONG_SIGNATURE                              = 0x8D,  /**< Wrong Signature */
  ZB_ZCM_METERING_ALARM_NO_SIGNATURE                                 = 0x8E,  /**< No Signature */
  ZB_ZCM_METERING_ALARM_UNAUTHORISED_ACTION_FROM_HAN                 = 0x8F,  /**< Unauthorised Action from HAN */
  ZB_ZCM_METERING_ALARM_FAST_POLLING_START                           = 0x90,  /**< Fast Polling Start */
  ZB_ZCM_METERING_ALARM_FAST_POLLING_END                             = 0x91,  /**< Fast Polling End */
  ZB_ZCM_METERING_ALARM_METER_REPORTING_INTERVAL_CHANGED             = 0x92,  /**< Meter Reporting Interval Changed */
  ZB_ZCM_METERING_ALARM_DISCONNECT_DUE_TO_LOAD_LIMIT                 = 0x93,  /**< Disconnect Due to Load Limit */
  ZB_ZCM_METERING_ALARM_METER_SUPPLY_STATUS_REGISTER_CHANGED         = 0x94,  /**< Meter Supply Status Register Changed */
  ZB_ZCM_METERING_ALARM_METER_ALARM_STATUS_REGISTER_CHANGED          = 0x95,  /**< Meter Alarm Status Register Changed */
  ZB_ZCM_METERING_ALARM_EXTENDED_METER_ALARM_STATUS_REGISTER_CHANGED = 0x96,  /**< Extended Meter Alarm Status Register Changed. */
};


/**
 * The Manufacturer Specific Alarm Group defines alarms specific for any meters. These are used
 * for meter specific functionality that is not covered by the current smart energy specification.
 * @see SE spec, Table D-42
 */
enum zb_zcl_metering_manufacturer_specific_alarm_group_e
{
  ZB_ZCL_METERING_ALARM_MANUFACTURER_SPECIFIC_A = 0xB0, /**< Manufacturer Specific A */
  ZB_ZCL_METERING_ALARM_MANUFACTURER_SPECIFIC_B = 0xB1, /**< Manufacturer Specific B */
  ZB_ZCL_METERING_ALARM_MANUFACTURER_SPECIFIC_C = 0xB2, /**< Manufacturer Specific C */
  ZB_ZCL_METERING_ALARM_MANUFACTURER_SPECIFIC_D = 0xB3, /**< Manufacturer Specific D */
  ZB_ZCL_METERING_ALARM_MANUFACTURER_SPECIFIC_E = 0xB4, /**< Manufacturer Specific E */
  ZB_ZCL_METERING_ALARM_MANUFACTURER_SPECIFIC_F = 0xB5, /**< Manufacturer Specific F */
  ZB_ZCL_METERING_ALARM_MANUFACTURER_SPECIFIC_G = 0xB6, /**< Manufacturer Specific G */
  ZB_ZCL_METERING_ALARM_MANUFACTURER_SPECIFIC_H = 0xB7, /**< Manufacturer Specific H */
  ZB_ZCL_METERING_ALARM_MANUFACTURER_SPECIFIC_I = 0xB8, /**< Manufacturer Specific I */
};


/** Service Disconnect Reasons
 * @see Linky Metering Cluster spec, Table D-16
 */
enum zb_zcl_metering_service_disconnect_reasons_e
{
  ZB_ZCL_METERING_REASON_ON                                   = 0x00,  /**< ON (Service NOT disconnected) */
  ZB_ZCL_METERING_REASON_OFF_DUE_TO_OVER_POWER                = 0x01,  /**< OFF due to over power */
  ZB_ZCL_METERING_REASON_OFF_DUE_TO_OVER_VOLTAGE              = 0x02,  /**< OFF due to over voltage */
  ZB_ZCL_METERING_REASON_OFF_DUE_TO_REMOTE_DNO_LOAD_CONTROL   = 0x03,  /**< OFF due to Remote DNO Load Control */
  ZB_ZCL_METERING_REASON_OFF_BY_OTHER_REMOTE_COMMAND          = 0x04,  /**< OFF by other remote command */
  ZB_ZCL_METERING_REASON_OFF_DUE_TO_OVERHEATING_SHORT_CIRCUIT = 0x05,  /**< OFF due to overheating,
                                                                            I > In (Maximum Current) - short circuit */
  ZB_ZCL_METERING_REASON_OFF_DUE_TO_OVERHEATING_OTHER         = 0x06,  /**< OFF due to overheating,
                                                                            I < In (Maximum Current) - other */
};

/** @brief Default value for Metering cluster revision global attribute */
#define ZB_ZCL_METERING_CLUSTER_REVISION_DEFAULT ((zb_uint16_t)0x0002u)

/*! @brief If set, suppress leading zeros */
#define ZB_ZCL_METERING_FORMATTING_SUPPRESS_ZERO(b)    (((b) & 0x80)==0x80)

/*! @brief Number of Digits to the right of the Decimal Point */
#define ZB_ZCL_METERING_FORMATTING_LEFT(b)           (((b)>>3) & 0x07)

/*! @brief Number of Digits to the left of the Decimal Point */
#define ZB_ZCL_METERING_FORMATTING_RIGHT(b)            ((b) & 0x07)

/*! @brief Set summation formatting */
#define ZB_ZCL_METERING_FORMATTING_SET(suppress_zero, left, right)      \
  ( ((suppress_zero) ? 0x80 : 0) | (((left) & 7)<<3) | ((right) & 7) )

/** @brief Default value for Status attribute */
#define ZB_ZCL_METERING_STATUS_DEFAULT_VALUE 0x00

/** @brief Default value for Status attribute */
#define ZB_ZCL_METERING_UNIT_OF_MEASURE_DEFAULT_VALUE 0x00

/** @brief Default value for Instantaneous Demand attribute */
#define ZB_ZCL_METERING_INSTANTANEOUS_DEMAND_DEFAULT_VALUE ZB_INIT_UINT24(0x00, 0x0000)

  /** @brief Default value for DailyFreezeTime attribute */
#define ZB_ZCL_METERING_DAILY_FREEZE_TIME_DEFAULT_VALUE ((zb_uint16_t)0x0000)

/** @brief Default value for PowerFactor attribute */
#define ZB_ZCL_METERING_POWER_FACTOR_DEFAULT_VALUE ((zb_int8_t)0x00)

/** @brief Default value for DefaultUpdatePeriod attribute */
#define ZB_ZCL_METERING_DEFAULT_UPDATE_PERIOD_DEFAULT_VALUE ((zb_uint8_t)0x1E)

/** @brief Default value for FastPollUpdatePeriod attribute */
#define ZB_ZCL_METERING_FAST_POLL_UPDATE_PERIOD_DEFAULT_VALUE ((zb_uint8_t)0x05)

/** @brief Default value for PresetReadingTime attribute */
#define ZB_ZCL_METERING_PRESET_READING_TIME_DEFAULT_VALUE ((zb_uint16_t)0x0000)

/** @brief Default value for MaxNumberOfPeriodsDelivered attribute */
#define ZB_ZCL_METERING_MAX_NUMBER_OF_PERIODS_DELIVERED_DEFAULT_VALUE ((zb_uint8_t)0x18)

/** @brief Default value for GenericAlarmMask attribute */
#define ZB_ZCL_METERING_GENERIC_ALARM_MASK_DEFAULT_VALUE ((zb_uint16_t)0xFFFF)

/** @brief Default value for ElectricityAlarmMask attribute */
#define ZB_ZCL_METERING_ELECTRICITY_ALARM_MASK_DEFAULT_VALUE ((zb_uint32_t)0xFFFFFFFF)

/** @brief Default value for GenericFlowPressureAlarmMask attribute */
#define ZB_ZCL_METERING_GENERIC_FLOW_PRESSURE_ALARM_MASK_DEFAULT_VALUE ((zb_uint16_t)0xFFFF)

/** @brief Default value for WaterSpecificAlarmMask attribute */
#define ZB_ZCL_METERING_WATER_SPECIFIC_ALARM_MASK_DEFAULT_VALUE ((zb_uint16_t)0xFFFF)

/** @brief Default value for HeatAndCoolingSpecificAlarmMask attribute */
#define ZB_ZCL_METERING_HEAT_AND_COOLING_SPECIFIC_ALARM_MASK_DEFAULT_VALUE ((zb_uint16_t)0xFFFF)

/** @brief Default value for GasSpecificAlarmMask attribute */
#define ZB_ZCL_METERING_GAS_SPECIFIC_ALARM_MASK_DEFAULT_VALUE ((zb_uint16_t)0xFFFF)

/** @brief Default value for FastPollUpdatePeriod attribute */
#define ZB_ZCL_METERING_LINKY_MODE_ON_OPERATION_DEFAULT_VALUE ((zb_uint8_t)0x00)
/** @} */ /* ZB_ZCL_METERING_SRV_ATTRS */


/** @defgroup ZB_ZCL_METERING_CMDS Metering cluster commands
 *  @{
 */


/** Metering cluster client command identifiers.
 *  @see SE spec, subclause D.3.3.3.1
 */
typedef enum zb_zcl_metering_cli_cmd_e
{

  /* (O) */
  ZB_ZCL_METERING_CLI_CMD_GET_PROFILE                      = 0x00,  /**< @e GetProfile command */
  /* (O) */
  ZB_ZCL_METERING_CLI_CMD_REQUEST_MIRROR_RESPONSE,                  /**< The @e RequestMirrorResponse
                                                                      * command allows the ESI to inform a
                                                                      * sleepy Metering Device it has the
                                                                      * ability to store and mirror its data.
                                                                      */
  /* (O) */
  ZB_ZCL_METERING_CLI_CMD_MIRROR_REMOVED,                           /**<The @e MirrorRemovedCommand allows
                                                                      * the ESI to inform a sleepy Metering
                                                                      * Device mirroring support has been
                                                                      * removed or halted.
                                                                      */
  /* (O) */
  ZB_ZCL_METERING_CLI_CMD_REQUEST_FAST_POLL_MODE,                   /**< @e RequestFastPollMode command */
  /* (O) */
  ZB_ZCL_METERING_CLI_CMD_SCHEDULE_SNAPSHOT,                        /**< This command is used to set up a
                                                                     * schedule of when the device shall
                                                                     * create snapshot data.
                                                                     */
  /* (O) */
  ZB_ZCL_METERING_CLI_CMD_TAKE_SNAPSHOT,                            /**< This command is used to instruct the
                                                                      * cluster server to take a single snapshot.
                                                                      */
  /* (O) */
  ZB_ZCL_METERING_CLI_CMD_GET_SNAPSHOT,                             /**< This command is used to request
                                                                      * snapshot data from the cluster server. */
  /* (O) */
  ZB_ZCL_METERING_CLI_CMD_START_SAMPLING,                           /**< The sampling mechanism allows a set of
                                                                      * samples of the specified type of data to be taken,
                                                                      * commencing at the stipulated start time.
                                                                      */
  /* (O) */
  ZB_ZCL_METERING_CLI_CMD_GET_SAMPLED_DATA,                         /**< This command is used to request sampled
                                                                      * data from the server.
                                                                      */
  /* (O) */
  ZB_ZCL_METERING_CLI_CMD_MIRROR_REPORT_ATTRIBUTE_RESPONSE,         /**< This command is sent in response to the
                                                                      * @e ReportAttribute command when the
                                                                      * @e MirrorReporting attribute is set.
                                                                      */
  /* (O) */
  ZB_ZCL_METERING_CLI_CMD_RESET_LOAD_LIMIT_COUNTER,                 /**< The ResetLoadLimitCounter command shall
                                                                      * cause the LoadLimitCounter attribute
                                                                      * to be reset
                                                                      */
  /* (O) */
  ZB_ZCL_METERING_CLI_CMD_CHANGE_SUPPLY,                            /**< This command is sent from the Head-end or
                                                                      * ESI to the Metering Device to instruct it
                                                                      * to change the status of the valve or load
                                                                      * switch, i.e. the supply.
                                                                      */
  /* (O) */
  ZB_ZCL_METERING_CLI_CMD_LOCAL_CHANGE_SUPPLY,                      /**< This command is a simplified version of the
                                                                      * Change Supply command, intended to be sent
                                                                      * from an IHD to a meter as the consequence
                                                                      * of a user action on the IHD.
                                                                      */
  /* (O) */
  ZB_ZCL_METERING_CLI_CMD_SET_SUPPLY_STATUS,                        /**< This command is used to specify the required
                                                                      * status of the supply following the occurrence
                                                                      * of certain events on the meter.
                                                                      */
  /* (O) */
  ZB_ZCL_METERING_CLI_CMD_SET_UNCONTROLLED_FLOW_THRESHOLD,          /**< This command is used to update the 'Uncontrolled Flow Rate'
                                                                      * configuration data used by flow meters.
                                                                      */
} zb_zcl_metering_cli_cmd_t;


/** Metering cluster server command identifiers.
 *  @see SE spec, D.3.2.3.1
 */
typedef enum zb_zcl_metering_srv_cmd_e
{
  /* (O) */
  ZB_ZCL_METERING_SRV_CMD_GET_PROFILE_RESPONSE              = 0x00,   /**< Get Profile Response */
  /* (O) */
  ZB_ZCL_METERING_SRV_CMD_REQUEST_MIRROR,                             /**< This command is used to request the
                                                                        * ESI to mirror Metering Device data.
                                                                        */
  /* (O) */
  ZB_ZCL_METERING_SRV_CMD_REMOVE_MIRROR,                              /**< This command is used to request the ESI
                                                                        * to remove its mirror of Metering Device data.
                                                                        */
  /* (O) */
  ZB_ZCL_METERING_SRV_CMD_REQUEST_FAST_POLL_MODE_RESPONSE,            /**< @e RequestFastPollModeResponse command*/
  /* (O) */
  ZB_ZCL_METERING_SRV_CMD_SCHEDULE_SNAPSHOT_RESPONSE,                 /**< This command is generated in response to a
                                                                        * ScheduleSnapshot command, and is sent to confirm
                                                                        * whether the requested snapshot schedule has been set up.
                                                                        */
  /* (O) */
  ZB_ZCL_METERING_SRV_CMD_TAKE_SNAPSHOT_RESPONSE,                     /**< This command is generated in response to a TakeSnapshot
                                                                        * command, and is sent to confirm whether the requested
                                                                        * snapshot has been accepted and successfully taken.
                                                                        */
  /* (O) */
  ZB_ZCL_METERING_SRV_CMD_PUBLISH_SNAPSHOT,                           /**< This command is generated in response
                                                                        * to a GetSnapshot command or when a new
                                                                        * snapshot is created. */
  /* (O) */
  ZB_ZCL_METERING_SRV_CMD_GET_SAMPLED_DATA_RESPONSE,                  /**< This command is used to send the requested
                                                                        * sample data to the client. It is generated in response
                                                                        * to a @e GetSampledData command
                                                                        */
  /* (O) */
  ZB_ZCL_METERING_SRV_CMD_CONFIGURE_MIRROR,                           /**<Where 'Two Way Mirroring' is being implemented,
                                                                        * this command shall be sent to the mirror once the
                                                                        * mirror has been created. The command allows a BOMD
                                                                        * to provide the operational configuration of the
                                                                        * associated Mirror.
                                                                        */
  /* (O) */
  ZB_ZCL_METERING_SRV_CMD_CONFIGURE_NOTIFICATION_SCHEME,              /**< Where 'Two Way Mirroring' is being implemented,
                                                                        * and a non-default Notification Scheme is to be used,
                                                                        * this command shall be sent to the mirror once the
                                                                        * mirror has been created.
                                                                        */
  /* (O) */
  ZB_ZCL_METERING_SRV_CMD_CONFIGURE_NOTIFICATION_FLAG,                /**< Where 'Two Way Mirroring' is being implemented,
                                                                        * and a non-default Notification Scheme is to
                                                                        * be used, the @e ConfigureNotificationFlags command
                                                                        * allows a BOMD to set the commands relating to the
                                                                        * bit value for each NotificationFlags#N attribute
                                                                        * that the scheme is proposing to use.
                                                                        */
  /* (O) */
  ZB_ZCL_METERING_SRV_CMD_GET_NOTIFIED_MESSAGE,                       /**< The GetNotifiedMessage command is used only when a
                                                                        * BOMD is being mirrored. This command provides a method
                                                                        * for the BOMD to notify the Mirror message queue that
                                                                        * it wants to receive commands that the Mirror has queued.
                                                                        */
  /* (O) */
  ZB_ZCL_METERING_SRV_CMD_SUPPLY_STATUS_RESPONSE,                     /**< This command is transmitted by a Metering Device in response
                                                                        * to a Change Supply command.
                                                                        */
  /* (O) */
  ZB_ZCL_METERING_SRV_CMD_START_SAMPLING_RESPONSE,                    /**< This command is transmitted by a Metering Device in response
                                                                        * to a StartSampling command.
                                                                        */
} zb_zcl_metering_srv_cmd_t;

/* METERING cluster commands list : only for information - do not modify */
#define ZB_ZCL_CLUSTER_ID_METERING_SERVER_ROLE_GENERATED_CMD_LIST                              \
                                      ZB_ZCL_METERING_SRV_CMD_GET_PROFILE_RESPONSE,            \
                                      ZB_ZCL_METERING_SRV_CMD_REQUEST_FAST_POLL_MODE_RESPONSE, \
                                      ZB_ZCL_METERING_SRV_CMD_PUBLISH_SNAPSHOT,                \
                                      ZB_ZCL_METERING_SRV_CMD_GET_SAMPLED_DATA_RESPONSE

#define ZB_ZCL_CLUSTER_ID_METERING_CLIENT_ROLE_RECEIVED_CMD_LIST ZB_ZCL_CLUSTER_ID_METERING_SERVER_ROLE_GENERATED_CMD_LIST

#define ZB_ZCL_CLUSTER_ID_METERING_CLIENT_ROLE_GENERATED_CMD_LIST                     \
                                      ZB_ZCL_METERING_CLI_CMD_GET_PROFILE,            \
                                      ZB_ZCL_METERING_CLI_CMD_REQUEST_FAST_POLL_MODE, \
                                      ZB_ZCL_METERING_CLI_CMD_GET_SNAPSHOT,           \
                                      ZB_ZCL_METERING_CLI_CMD_GET_SAMPLED_DATA

#define ZB_ZCL_CLUSTER_ID_METERING_SERVER_ROLE_RECEIVED_CMD_LIST ZB_ZCL_CLUSTER_ID_METERING_CLIENT_ROLE_GENERATED_CMD_LIST

/** Interval Channel Values
 * @see SE spec, subclause D.3.3.3.1.1.1
 */
enum zb_zcl_metering_interval_channel_e
{
  ZB_ZCL_METERING_CONSUMPTION_DELIVERED          = 0,
  ZB_ZCL_METERING_CONSUMPTION_RECEIVED           = 1,
  ZB_ZCL_METERING_REACTIVE_CONSUMPTION_DELIVERED = 2,
  ZB_ZCL_METERING_REACTIVE_CONSUMPTION_RECEIVED  = 3,
  /* Value 4 is reserved. Values 5 - 255 is unused */
};


/** @ref ZB_ZCL_METERING_CLI_CMD_GET_PROFILE "GetProfile" command payload
 *  @see SE spec, subclause D.3.3.3.1.1.1
 */
typedef ZB_PACKED_PRE struct zb_zcl_metering_get_profile_payload_s
{
  /** Enumerated value used to select the quantity of interest returned by the
    * @e GetProfileResponse command.
    * @see @ref zb_zcl_metering_interval_channel_e
    */
  zb_uint8_t interval_channel;
  /** It is a 32-bit value (in UTCTime) used to select an Intervals block
   * from all the Intervals blocks available.
   */
  zb_uint32_t end_time;
  /** Represents the number of intervals being requested. */
  zb_uint8_t number_of_periods;
} ZB_PACKED_STRUCT zb_zcl_metering_get_profile_payload_t;


/** Status Field Values
 * @see SE spec, Table D-48
 */
/* FIXME: Discuss prefix */
enum zb_zcl_metering_status_field_e
{
  ZB_ZCL_METERING_SUCCESS                                       = 0x00,  /**< Success */
  ZB_ZCL_METERING_UNDEFINED_INTERVAL_CHANNEL_REQUESTED          = 0x01,  /**< Undefined Interval Channel requested */
  ZB_ZCL_METERING_INTERVAL_CHANNEL_NOT_SUPPORTED                = 0x02,  /**< Interval Channel not supported */
  ZB_ZCL_METERING_INVALID_END_TIME                              = 0x03,  /**< Invalid End Time */
  ZB_ZCL_METERING_MORE_PERIODS_REQUESTED_THAN_CAN_BE_RETURNED   = 0x04,  /**< More periods requested than can be returned */
  ZB_ZCL_METERING_NO_INTERVALS_AVAILABLE_FOR_THE_REQUESTED_TIME = 0x05   /**< No intervals available for the requested time */
};


/** Profile Interval Period
 * @see SE spec, Table D-49
 */
enum zb_zcl_metering_profile_interval_period_e
{
   ZB_ZCl_METERING_INTERVAL_PERIOD_DAILY       = 0,  /**< Daily */
   ZB_ZCl_METERING_INTERVAL_PERIOD_60_MINUTES  = 1,  /**< 60 minutes */
   ZB_ZCl_METERING_INTERVAL_PERIOD_30_MINUTES  = 2,  /**< 30 minutes */
   ZB_ZCl_METERING_INTERVAL_PERIOD_15_MINUTES  = 3,  /**< 15 minutes */
   ZB_ZCl_METERING_INTERVAL_PERIOD_10_MINUTES  = 4,  /**< 10 minutes */
   ZB_ZCl_METERING_INTERVAL_PERIOD_7_5_MINUTES = 5,  /**< 7.5 minutes */
   ZB_ZCl_METERING_INTERVAL_PERIOD_5_MINUTES   = 6,  /**< 5 minutes */
   ZB_ZCl_METERING_INTERVAL_PERIOD_2_5_MINUTES = 7,  /**< 2.5 minutes */
   ZB_ZCl_METERING_INTERVAL_PERIOD_1_MINUTE    = 8  /**< 1 minute */
};

#define ZB_ZCL_METERING_GET_PROFILE_PAYLOAD_SIZE_IS_VALID(current_size) \
  ((current_size >= sizeof(zb_zcl_metering_get_profile_payload_t) ? ZB_TRUE : ZB_FALSE))

/** @ref ZB_ZCL_METERING_SRV_CMD_GET_PROFILE_RESPONSE "GetProfileResponse" command payload
 *  @see SE spec, subclause D.3.2.3.1.1.1
 */
typedef ZB_PACKED_PRE struct zb_zcl_metering_get_profile_response_payload_s
{
  /** It is 32-bit value (in UTC) representing the end time of the most
   * chronologically recent interval being requested.
   */
  zb_uint32_t end_time;
  /** Status
   * @see zb_zcl_metering_status_field_e
   */
  zb_uint8_t status;
  /** Represents the interval or time frame used to capture metered Energy, Gas,
   * and Water consumption for profiling purposes.
   * @see zb_zcl_metering_profile_interval_period_e
   */
  zb_uint8_t profile_interval_period;
  /** Represents the number of intervals the device is returning.
   */
  zb_uint8_t number_of_periods_delivered;
  /** Series of interval data captured using the period specified by the
   * @e ProfileIntervalPeriod field. The content of the interval data depends of
   * the type of information requested using the @e Channel field in the
   * @ref ZB_ZCL_METERING_CLI_CMD_GET_PROFILE "GetProfileCommand", and will
   * represent the change in that information since the previous interval.
   */
  zb_uint24_t *intervals;
} ZB_PACKED_STRUCT zb_zcl_metering_get_profile_response_payload_t;


/** @ref ZB_ZCL_METERING_CLI_CMD_REQUEST_FAST_POLL_MODE "RequestFastPollMode" command payload
 *  @see SE spec, subclause D.3.3.3.1.4
 */
typedef ZB_PACKED_PRE struct zb_zcl_metering_request_fast_poll_mode_payload_s
{
  /** Desired FastPollUpdatePeriod attribute.
   */
  zb_uint8_t fast_poll_update_period;  /* (M) */
  /** Desired duration for the server to remain in fast poll mode not to exceed 15 minutes
   */
  zb_uint8_t duration_in_minutes;      /* (M) */
} ZB_PACKED_STRUCT zb_zcl_metering_request_fast_poll_mode_payload_t;


/** @ref ZB_ZCL_METERING_SRV_CMD_REQUEST_FAST_POLL_MODE_RESPONSE "RequestFastPollModeResponse" command payload
 *  @see SE spec, subclause D.3.3.3.1.4
 */
typedef ZB_PACKED_PRE struct zb_zcl_metering_request_fast_poll_mode_response_payload_s
{
  /** The period at which metering data shall be updated. This may be different than the requested fast poll.
   */
  zb_uint8_t applied_update_period_in_seconds;  /* (M) */
  /** UTC time that indicates when the metering server will terminate fast poll mode and resume
    * updating at the rate specified by @e DefaultUpdatePeriod.
   */
  zb_uint32_t fast_poll_mode_end_time;          /* (M) */
} ZB_PACKED_STRUCT zb_zcl_metering_request_fast_poll_mode_response_payload_t;


/** @ref ZB_ZCL_METERING_SRV_CMD_GET_SAMPLED_DATA_RESPONSE "GetSampledDataResponse" command payload
 *  @see SE spec, subclause D.3.2.3.1.8
 */
typedef ZB_PACKED_PRE struct zb_zcl_metering_get_sampled_data_response_payload_s
{
  /** Unique identifier allocated to this Sampling session. This field allows devices to match
   * response data with the appropriate request.
   */
  zb_uint16_t sample_id;               /* (M) */
  /** A UTC Time field to denote the time of the first sample returned in this response. */
  zb_uint32_t sample_start_time;        /* (M) */
  /** An 8-bit enumeration that identifies the type of data being sampled. */
  zb_uint8_t sample_type;               /* (M) */
  /** An unsigned 16-bit field representing the interval or time in seconds between samples. */
  zb_uint16_t sample_request_interval;  /* (M) */
  /** Represents the number of samples being requested, This value cannot exceed the size stipulated
   * in the @e MaxNumberOfSamples field in the @e StartSampling command.
   */
  zb_uint16_t number_of_samples;        /* (M) */
  /** Series of data samples captured using the interval specified by the @e SampleRequestInterval
   * field in the @e StartSampling command.
   */
  zb_uint24_t *samples;                 /* (M) */
} ZB_PACKED_STRUCT zb_zcl_metering_get_sampled_data_response_payload_t;


/** Sample Type Enumerations
 * @see SE spec, Table D-54
 */
enum zb_zcl_metering_sample_type_e
{
  ZB_ZCL_METERING_SAMPLE_TYPE_CONSUMPTION_DELIVERED          = 0,  /**< Consumption Delivered */
  ZB_ZCL_METERING_SAMPLE_TYPE_CONSUMPTION_RECEIVED           = 1,  /**< Consumption Received */
  ZB_ZCL_METERING_SAMPLE_TYPE_REACTIVE_CONSUMPTION_DELIVERED = 2,  /**< Reactive Consumption Delivered */
  ZB_ZCL_METERING_SAMPLE_TYPE_REACTIVE_CONSUMPTION_RECEIVED  = 3,  /**< Reactive Consumption Received */
  ZB_ZCL_METERING_SAMPLE_TYPE_INSTANTANEOUS_DEMAND           = 4   /**< InstantaneousDemand*/
};


/** @ref ZB_ZCL_METERING_CLI_CMD_GET_SAMPLED_DATA "GetSampledData" command payload
 *  @see SE spec, subclause D.3.2.3.1.8
 */
typedef ZB_PACKED_PRE struct zb_zcl_metering_get_sampled_data_payload_s
{
  /** Unique identifier allocated to this Sampling session. This field allows devices to match
   * response data with the appropriate request.
   */
  zb_uint16_t sample_id;               /* (M) */
  /** A UTC Timestamp indicating the earliest time of a sample to be returned. */
  zb_uint32_t earliest_sample_time;     /* (M) */
  /** An 8-bit enumeration that identifies the type of data being sampled. */
  zb_uint8_t sample_type;               /* (M) */
  /** Represents the number of samples being requested, This value cannot exceed the size stipulated
   * in the @e MaxNumberOfSamples field in the @e StartSampling command.
   */
  zb_uint16_t number_of_samples;        /* (M) */
} ZB_PACKED_STRUCT zb_zcl_metering_get_sampled_data_payload_t;


/** @ref ZB_ZCL_METERING_CLI_CMD_GET_SNAPSHOT "GetSnapshot" command payload
 *  @see SE spec, subclause D.3.3.3.1.7
 */
typedef ZB_PACKED_PRE struct zb_zcl_metering_get_snapshot_payload_s
{
  /** A UTC Timestamp indicating the earliest time of a snapshot to be returned by a corresponding
    * Publish Snapshot command.
    */
  zb_uint32_t earliest_start_time;  /* (M) */
  /** A UTC Timestamp indicating the latest time of a snapshot to be returned by a corresponding
    * Publish Snapshot command.
    */
  zb_uint32_t latest_end_time;      /* (M) */
  /** Where multiple snapshots satisfy the selection criteria specified by the other fields in this
    * command, this field identifies the individual snapshot to be returned.
    */
  zb_uint8_t snapshot_offset;       /* (M) */
  /** This field is used to select only snapshots that were taken due to a specific cause. */
  zb_uint32_t snapshot_cause;       /* (M) */
} ZB_PACKED_STRUCT zb_zcl_metering_get_snapshot_payload_t;


/** Snapshot Cause BitMap
 * @see SE spec, Table D-52
 */
enum zb_zcl_metering_snapshot_cause_e
{
  ZB_ZCL_METERING_CAUSE_GENERAL                         = 1 << 0,  /**< General */
  ZB_ZCL_METERING_CAUSE_END_OF_BILLING_PERIOD           = 1 << 1,  /**< End of Billing Period */
  ZB_ZCL_METERING_CAUSE_END_OF_BLOCK_PERIOD             = 1 << 2,  /**< End of Block Period */
  ZB_ZCL_METERING_CAUSE_CHANGE_OF_TARIFF_INFORMATION    = 1 << 3,  /**< Change of Tariff Information */
  ZB_ZCL_METERING_CAUSE_CHANGE_OF_PRICE_MATRIX          = 1 << 4,  /**< Change of Price Matrix */
  ZB_ZCL_METERING_CAUSE_CHANGE_OF_BLOCK_THRESHOLDS      = 1 << 5,  /**< Change of Block Thresholds */
  ZB_ZCL_METERING_CAUSE_CHANGE_OF_CV                    = 1 << 6,  /**< Change of CV */
  ZB_ZCL_METERING_CAUSE_CHANGE_OF_CF                    = 1 << 7,  /**< Change of CF */
  ZB_ZCL_METERING_CAUSE_CHANGE_OF_CALENDAR              = 1 << 8,  /**< Change of Calendar */
  ZB_ZCL_METERING_CAUSE_CRITICAL_PEAK_PRICING           = 1 << 9,  /**< Critical Peak Pricing */
  ZB_ZCL_METERING_CAUSE_MANUALLY_TRIGGERED_FROM_CLIENT  = 1 << 10,  /**< Manually Triggered from Client */
  ZB_ZCL_METERING_CAUSE_END_OF_RESOLVE_PERIOD           = 1 << 11,  /**< End of Resolve Period */
  ZB_ZCL_METERING_CAUSE_CHANGE_OF_TENANCY               = 1 << 12,  /**< Change of Tenancy */
  ZB_ZCL_METERING_CAUSE_CHANGE_OF_SUPPLIER              = 1 << 13,  /**< Change of Supplier */
  ZB_ZCL_METERING_CAUSE_CHANGE_OF_METER_MODE            = 1 << 14,  /**< Change of (Meter) Mode */
  ZB_ZCL_METERING_CAUSE_DEBT_PAYMENT                    = 1 << 15,  /**< Debt Payment */
  ZB_ZCL_METERING_CAUSE_SCHEDULED_SNAPSHOT              = 1 << 16,  /**< Scheduled Snapshot */
  ZB_ZCL_METERING_CAUSE_OTA_FIRMWARE_DOWNLOAD           = 1 << 17,   /**< OTA Firmware Download */
  ZB_ZCL_METERING_CAUSE_SELECT_ALL_SNAPSHOTS            = 0xFFFFFFFF  /**< Select All Snapshots */
};


/** Snapshot Payload Type
 * @see SE spec, Table D-53
 */
enum zb_zcl_metering_snapshot_payload_type_e
{
  ZB_ZCL_METERING_TOU_DELIVERED_REGISTERS         = 0,    /**< TOU Information Set DeliveredRegisters */
  ZB_ZCL_METERING_TOU_RECEIVED_REGISTERS          = 1,    /**< TOU Information Set Received Registers */
  ZB_ZCL_METERING_BLOCK_TIER_DELIVERED            = 2,    /**< Block Tier Information Set Delivered */
  ZB_ZCL_METERING_BLOCK_TIER_RECEIVED             = 3,    /**< Block Tier Information Set Received */
  ZB_ZCL_METERING_TOU_DELIVERED_NO_BILLING        = 4,    /**< TOU Information Set Delivered (No Billing) */
  ZB_ZCL_METERING_TOU_RECEIVED_NO_BILLING         = 5,    /**< TOU Information Set Received (No Billing) */
  ZB_ZCL_METERING_BLOCK_TIER_DELIVERED_NO_BILLING = 6,    /**< Block Tier Information Set Delivered (No Billing) */
  ZB_ZCL_METERING_BLOCK_TIER_RECEIVED_NO_BILLING  = 7,    /**< Block Tier Information Set Received (No Billing) */
  ZB_ZCL_METERING_DATA_UNAVAILABLE                = 128,  /**< Data Unavailable */
};


/** SnapshotPayloadType 0 = TOU Information Delivered Set
 * @see @ref ZB_ZCL_METERING_TOU_DELIVERED_REGISTERS
 */
typedef ZB_PACKED_PRE struct zb_zcl_metering_tou_delivered_payload_s
{
  /** An unsigned 48-bit integer that returns the value of the CurrentSummationDelivered
   * attribute at the stated snapshot timestamp.
   */
  zb_uint48_t current_summation_delivered;  /* (M) */
  /** An unsigned 32-bit integer that provides a value for the costs in the current billing period. */
  zb_uint32_t bill_to_date_delivered;  /* (M) */
  /** A UTC timestamp that indicates when the value of the associated BillToDateDelivered
   * parameter was last updated.
   */
  zb_uint32_t bill_to_date_time_stamp_delivered;  /* (M) */
  /** An unsigned 32-bit integer that provides a value indicating what the estimated state of the
   * account will be at the end of the billing period based on past consumption.
   */
  zb_uint32_t projected_bill_delivered;  /* (M) */
  /** A UTC timestamp that indicates when the associated ProjectedBillDelivered parameter was
   * last updated.
   */
  zb_uint32_t projected_bill_time_stamp_delivered;  /* (M) */
  /** An 8-bit BitMap used to determine where the decimal point is located in the
   * BillToDateDelivered and ProjectedBillDelivered fields.
   */
  zb_uint8_t bill_delivered_trailing_digit;  /* (M) */
  /** An 8-bit integer representing the number of tiers in use at the time the snapshot was taken. */
  zb_uint8_t number_of_tiers_in_use;  /* (M) */
  /** The Publish Snapshot command contains N elements of CurrentTierNSummationDelivered attributes
   * from the TOU Information Set. The Metering server shall send only the number of tiers in use,
   * as stated in this command.
   */
  zb_uint48_t *tier_summation;  /* (M) */
} ZB_PACKED_STRUCT zb_zcl_metering_tou_delivered_payload_t;


/** SnapshotPayloadType 1 = TOU Information Received Set
 * @see @ref ZB_ZCL_METERING_TOU_RECEIVED_REGISTERS
 */
typedef ZB_PACKED_PRE struct zb_zcl_metering_tou_received_payload_s
{
  /** An unsigned 48-bit integer that returns the value of the CurrentSummationReceived
   * attribute at the stated snapshot timestamp.
   */
  zb_uint48_t current_summation_received;  /* (M) */
  /** An unsigned 32-bit integer that provides a value for the costs in the current billing period. */
  zb_uint32_t bill_to_date_received;  /* (M) */
  /** A UTC timestamp that indicates when the value of the associated BillToDateReceived parameter
   * was last updated.
   */
  zb_uint32_t bill_to_date_time_stamp_received;  /* (M) */
  /** An unsigned 32-bit integer that provides a value indicating what the estimated state of the
   * account will be at the end of the billing period based on past generation.
   */
  zb_uint32_t projected_bill_received;  /* (M) */
  /** A UTC timestamp that indicates when the associated ProjectedBillReceived parameter was last
   * updated.
   */
  zb_uint32_t projected_bill_time_stamp_received;  /* (M) */
  /** An 8-bit BitMap used to determine where the decimal point is located in the
   * BillToDateReceived and ProjectedBillReceived fields.
   */
  zb_uint8_t bill_received_trailing_digit;  /* (M) */
  /** An 8-bit integer representing the number of tiers in use at the time the snapshot
   * was taken.
   */
  zb_uint8_t number_of_tiers_in_use;  /* (M) */
  /** The Publish Snapshot command contains N elements of CurrentTierNSummationReceived attributes
   * from the TOU Information Set. The Metering server shall send only the number of tiers in use,
   * as stated in this command.
   */
  zb_uint48_t *tier_summation;  /* (M) */
} ZB_PACKED_STRUCT zb_zcl_metering_tou_received_payload_t;


/** SnapshotPayloadType 2 = Block Information Delivered Set
 * @see @ref ZB_ZCL_METERING_BLOCK_TIER_DELIVERED
 */
typedef ZB_PACKED_PRE struct zb_zcl_metering_block_delivered_payload_s
{
  /** An unsigned 48-bit integer that returns the value of the CurrentSummationDelivered attribute
   * at the stated snapshot timestamp.
   */
  zb_uint48_t current_summation_delivered;  /* (M) */
  /** An unsigned 32-bit integer that provides a value for the costs in the current billing period. */
  zb_uint32_t bill_to_date_delivered;  /* (M) */
  /** A UTC timestamp that indicates when the value of the associated BillToDateDelivered
   * parameter was last updated.
   */
  zb_uint32_t bill_to_date_time_stamp_delivered;  /* (M) */
  /** An unsigned 32-bit integer that provides a value indicating what the estimated state of
   * the account will be at the end of the billing period based on past consumption.
   */
  zb_uint32_t projected_bill_delivered;  /* (M) */
  /** A UTC timestamp that indicates when the associated ProjectedBillDelivered parameter was
   * last updated.
   */
  zb_uint32_t projected_bill_time_stamp_delivered;  /* (M) */
  /** An 8-bit BitMap used to determine where the decimal point is located in the
   * BillToDateDelivered and ProjectedBillDelivered fields.
   */
  zb_uint8_t bill_delivered_trailing_digit;  /* (M) */
  /** An 8-bit integer representing the number of tiers in use at the time the snapshot was taken. */
  zb_uint8_t number_of_tiers_in_use;  /* (M) */
  /** The Publish Snapshot command contains N elements of CurrentTierNSummationDelivered
   * attributes from the TOU Information Set.
   */
  zb_uint48_t *tier_summation;  /* (M) */
  /** An 8-bit BitMap representing the number of tiers and block thresholds in use at the time the
   * snapshot was taken.
   */
  zb_uint8_t number_of_tiers_and_block_thresholds_in_use;  /* (M) */
  /** The Publish Snapshot command contains N elements of the Block Information Attribute Set
   * (Delivered). The metering server shall send only the number of Tiers and Blocks in use as
   * stated in this command.
   */
  zb_uint48_t *tier_block_summation;  /* (M) */
} ZB_PACKED_STRUCT zb_zcl_metering_block_delivered_payload_t;


/** SnapshotPayloadType 3 = Block Information Received Set
 * @see @ref ZB_ZCL_METERING_BLOCK_TIER_RECEIVED
 */
typedef ZB_PACKED_PRE struct zb_zcl_metering_block_received_payload_s
{
  /** An unsigned 48-bit integer that returns the value of the CurrentSummationReceived attribute
   * at the stated snapshot timestamp.
   */
  zb_uint48_t current_summation_received;                  /* (M) */
  /** An unsigned 32-bit integer that provides a value for the costs in the current billing period. */
  zb_uint32_t bill_to_date_received;                       /* (M) */
  /** A UTC timestamp that indicates when the value of the associated BillToDateReceived parameter
   * was last updated.
   */
  zb_uint32_t bill_to_date_time_stamp_received;            /* (M) */
  /** An unsigned 32-bit integer that provides a value indicating what the estimated state of the
   * account will be at the end of the billing period based on past generation.
   */
  zb_uint32_t projected_bill_received;                     /* (M) */
  /** A UTC timestamp that indicates when the associated ProjectedBillReceived parameter was last
   * updated.
   */
  zb_uint32_t projected_bill_time_stamp_received;          /* (M) */
  /** An 8-bit BitMap used to determine where the decimal point is located in the
   * BillToDateReceived and ProjectedBillReceived fields
   */
  zb_uint8_t bill_received_trailing_digit;                 /* (M) */
  /** An 8-bit integer representing the number of tiers in use at the time the snapshot was taken. */
  zb_uint8_t number_of_tiers_in_use;                       /* (M) */
  /** The Publish Snapshot command contains N elements of CurrentTierNSummationReceived attributes
   * from the TOU Information Set.
   */
  zb_uint48_t *tier_summation;                             /* (M) */
  /** An 8-bit BitMap representing the number of tiers and block thresholds in use at the time the
   * snapshot was taken.
   */
  zb_uint8_t number_of_tiers_and_block_thresholds_in_use;  /* (M) */
  /** The Publish Snapshot command contains N elements of the Block Information Attribute Set
   * (Received).
   */
  zb_uint48_t *tier_block_summation;                       /* (M) */
} ZB_PACKED_STRUCT zb_zcl_metering_block_received_payload_t;


/** SnapshotPayloadType 4 = TOU Information Set Delivered (No Billing)
 * @see @ref ZB_ZCL_METERING_TOU_DELIVERED_NO_BILLING
 */
typedef ZB_PACKED_PRE struct zb_zcl_metering_tou_delivered_no_billing_payload_s
{
  /** An unsigned 48-bit integer that returns the value of the CurrentSummationDelivered attribute
   * at the stated snapshot timestamp.
   */
  zb_uint48_t current_summation_delivered;  /* (M) */
  /** An 8-bit integer representing the number of tiers in use at the time the snapshot was taken. */
  zb_uint8_t number_of_tiers_in_use;  /* (M) */
  /** The Publish Snapshot command contains N elements of CurrentTierNSummationDelivered attributes
   * from the TOU Information Set.
   */
  zb_uint48_t *tier_summation;  /* (M) */
} ZB_PACKED_STRUCT zb_zcl_metering_tou_delivered_no_billing_payload_t;


/** SnapshotPayloadType 5 = TOU Information Set Received (No Billing)
 * @see @ref ZB_ZCL_METERING_TOU_RECEIVED_NO_BILLING
 */
typedef ZB_PACKED_PRE struct zb_zcl_metering_tou_received_no_billing_payload_s
{
  /** An unsigned 48-bit integer that returns the value of the CurrentSummationReceived attribute
   * at the stated snapshot timestamp.
   */
  zb_uint48_t current_summation_received; /* (M) */
  /** An 8-bit integer representing the number of tiers in use at the time the snapshot was taken. */
  zb_uint8_t number_of_tiers_in_use; /* (M) */
  /** The Publish Snapshot command contains N elements of CurrentTierNSummationReceived attributes
   * from the TOU Information Set.
   */
  zb_uint48_t *tier_summation; /* (M) */
} ZB_PACKED_STRUCT zb_zcl_metering_tou_received_no_billing_payload_t;


/** SnapshotPayloadType 6 = Block Tier Information Set Delivered (No Billing)
 * @see @ref ZB_ZCL_METERING_BLOCK_TIER_DELIVERED_NO_BILLING
 */
typedef ZB_PACKED_PRE struct zb_zcl_metering_block_tier_delivered_no_billing_payload_s
{
  /** An unsigned 48-bit integer that returns the value of the CurrentSummationDelivered attribute
   * at the stated snapshot timestamp.
   */
  zb_uint48_t current_summation_delivered;                 /* (M) */
  /** An 8-bit integer representing the number of tiers in use at the time the snapshot was taken. */
  zb_uint8_t number_of_tiers_in_use;                       /* (M) */
  /** The Publish Snapshot command contains N elements of CurrentTierNSummationDelivered attributes
   * from the TOU Information Set.
   */
  zb_uint48_t *tier_summation;                             /* (M) */
  /** An 8-bit BitMap representing the number of tiers and block thresholds in use at the time the
   * snapshot was taken.
   */
  zb_uint8_t number_of_tiers_and_block_thresholds_in_use;  /* (M) */
  /** The Publish Snapshot command contains N elements of the Block Information Attribute Set
   * (Delivered). The metering server shall send only the number of Tiers and Blocks in use as
   * stated in this command.
   */
  zb_uint48_t *tier_block_summation;                       /* (M) */
} ZB_PACKED_STRUCT zb_zcl_metering_block_tier_delivered_no_billing_payload_t;


/** SnapshotPayloadType 7 = Block Tier Information Set Received (No Billing)
 * @see @ref ZB_ZCL_METERING_BLOCK_TIER_RECEIVED_NO_BILLING
 */
typedef ZB_PACKED_PRE struct zb_zcl_metering_block_tier_received_no_billing_payload_s
{
  /** An unsigned 48-bit integer that returns the value of the CurrentSummationReceived attribute
   * at the stated snapshot timestamp.
   */
  zb_uint48_t current_summation_received;                  /* (M) */
  /** An 8-bit integer representing the number of tiers in use at the time the snapshot was taken.*/
  zb_uint8_t number_of_tiers_in_use;                       /* (M) */
  /** The Publish Snapshot command contains N elements of CurrentTierNSummationReceived attributes
   * from the TOU Information Set.
   */
  zb_uint48_t *tier_summation;                             /* (M) */
  /** An 8-bit BitMap representing the number of tiers and block thresholds in use at the time the
   * snapshot was taken.
   */
  zb_uint8_t number_of_tiers_and_block_thresholds_in_use;  /* (M) */
  /** The Publish Snapshot command contains N elements of the Block Information Attribute Set
   * (Received).
   */
  zb_uint48_t *tier_block_summation;                      /* (M) */
} ZB_PACKED_STRUCT zb_zcl_metering_block_tier_received_no_billing_payload_t;


/** The format of the Snapshot Sub-Payload differs depending on the SnapshotPayloadType
 * @see SE spec, Table D-53 and D.3.2.3.1.7
 */
typedef ZB_PACKED_PRE union zb_zcl_metering_snapshot_sub_payload_s
{
  /** @see @ref zb_zcl_metering_tou_delivered_payload_s
    * @see @ref ZB_ZCL_METERING_TOU_DELIVERED_REGISTERS */
  zb_zcl_metering_tou_delivered_payload_t tou_delivered;
  /** @see @ref zb_zcl_metering_tou_received_payload_s
    * @see @ref ZB_ZCL_METERING_TOU_RECEIVED_REGISTERS */
  zb_zcl_metering_tou_received_payload_t tou_received;
  /** @see @ref zb_zcl_metering_block_delivered_payload_s
    * @see @ref ZB_ZCL_METERING_BLOCK_TIER_DELIVERED */
  zb_zcl_metering_block_delivered_payload_t block_delivered;
  /** @see @ref zb_zcl_metering_block_received_payload_s
    * @see @ref ZB_ZCL_METERING_BLOCK_TIER_RECEIVED */
  zb_zcl_metering_block_received_payload_t block_received;
  /** @see @ref zb_zcl_metering_tou_delivered_no_billing_payload_s
    * @see @ref ZB_ZCL_METERING_TOU_DELIVERED_NO_BILLING */
  zb_zcl_metering_tou_delivered_no_billing_payload_t tou_delivered_no_billing;
  /** @see @ref zb_zcl_metering_tou_received_no_billing_payload_s
    * @see @ref ZB_ZCL_METERING_TOU_RECEIVED_NO_BILLING */
  zb_zcl_metering_tou_received_no_billing_payload_t tou_received_no_billing;
  /** @see @ref zb_zcl_metering_block_tier_delivered_no_billing_payload_s
    * @see @ref ZB_ZCL_METERING_BLOCK_TIER_DELIVERED_NO_BILLING */
  zb_zcl_metering_block_tier_delivered_no_billing_payload_t block_tier_delivered_no_billing;
  /** @see @ref zb_zcl_metering_block_tier_received_no_billing_payload_s
    * @see @ref ZB_ZCL_METERING_BLOCK_TIER_RECEIVED_NO_BILLING */
  zb_zcl_metering_block_tier_received_no_billing_payload_t block_tier_received_no_billing;

} ZB_PACKED_STRUCT zb_zcl_metering_snapshot_sub_payload_t;


/** @ref ZB_ZCL_METERING_SRV_CMD_PUBLISH_SNAPSHOT "PublishSnapshot" command payload
 *  @see SE spec, subclause D.3.2.3.1.7
 */
typedef ZB_PACKED_PRE struct zb_zcl_metering_publish_snapshot_payload_s
{
  /** Unique identifier allocated by the device creating the snapshot. */
  zb_uint32_t snapshot_id;  /* (M) */
  /** This is a 32-bit value (in UTC Time) representing the time at which the data snapshot was taken. */
  zb_uint32_t snapshot_time;  /* (M) */
  /** An 8-bit Integer indicating the number of snapshots found, based on the search criteria
    * defined in the associated GetSnapshot command.
    */
  zb_uint8_t total_snapshots_found;  /* (M) */
  /** The CommandIndex is used to count the payload fragments in the case where the entire payload
   * (snapshot) does not fit into one message
   */
  zb_uint8_t command_index;  /* (M) */
  /** In the case where the entire payload (snapshot) does not fit into one message, the
   * Total Number of Commands field indicates the total number of sub-commands that will be returned.
   */
  zb_uint8_t total_number_of_commands;  /* (M) */
  /** A 32-bit BitMap indicating the cause of the snapshot */
  zb_uint32_t snapshot_cause;  /* (M) */
  /** The SnapshotPayloadType is an 8-bit enumerator defining the format of the Snapshot Sub-Payload
   * in this message.
   */
  zb_uint8_t snapshot_payload_type; /* (M) */

  /** @see @ref zb_zcl_metering_snapshot_sub_payload_s */
  zb_zcl_metering_snapshot_sub_payload_t snapshot_sub_payload;
} ZB_PACKED_STRUCT zb_zcl_metering_publish_snapshot_payload_t;


/** Function for send @ref ZB_ZCL_METERING_CLI_CMD_GET_SNAPSHOT "GetSnapshot" command.
  * On receiver's side callback ZCL device callback @ref ZB_ZCL_REGISTER_DEVICE_CB will be called with
 * @ref ZB_ZCL_METERING_GET_SNAPSHOT_CB_ID
 * @param param - Reference to buffer.
 * @param dst_addr - Address of the device to send command to.
 * @param dst_addr_mode - Address mode for dst_addr.
 * @param dst_ep - Destination endpoint.
 * @param src_ep - Current endpoint.
 * @param payload - Packet payload (ref to @ref zb_zcl_metering_get_snapshot_payload_t).
 * @param cb - Callback which should be called when the ZCL stack receives APS ack.
 */
void zb_zcl_metering_send_cmd_get_snapshot(zb_uint8_t param, zb_addr_u *dst_addr,
                                                zb_aps_addr_mode_t dst_addr_mode, zb_uint8_t dst_ep,
                                                zb_uint8_t src_ep,
                                                zb_zcl_metering_get_snapshot_payload_t *payload,
                                                zb_callback_t cb);


/** Function for send @ref ZB_ZCL_METERING_CLI_CMD_GET_SAMPLED_DATA "GetSampledData" command.
 * On receiver's side callback ZCL device callback @ref ZB_ZCL_REGISTER_DEVICE_CB will be called with
 * @ref ZB_ZCL_METERING_GET_SAMPLED_DATA_CB_ID
 * @param param - Reference to buffer.
 * @param dst_addr - Address of the device to send command to.
 * @param dst_addr_mode - Address mode for dst_addr.
 * @param dst_ep - Destination endpoint.
 * @param src_ep - Current endpoint.
 * @param payload - Packet payload (ref to @ref zb_zcl_metering_get_sampled_data_payload_t).
 * @param cb - Callback which should be called when the ZCL stack receives APS ack.
 */
void zb_zcl_metering_send_cmd_get_sampled_data(zb_uint8_t param, zb_addr_u *dst_addr,
                                                    zb_aps_addr_mode_t dst_addr_mode, zb_uint8_t dst_ep,
                                                    zb_uint8_t src_ep,
                                                    zb_zcl_metering_get_sampled_data_payload_t *payload,
                                                    zb_callback_t cb);


/** Function for send @ref ZB_ZCL_METERING_CLI_CMD_REQUEST_FAST_POLL_MODE "RequestFastPollMode" command.
 * On receiver's side callback ZCL device callback @ref ZB_ZCL_REGISTER_DEVICE_CB will be called with
 * @ref ZB_ZCL_METERING_REQUEST_FAST_POLL_MODE_CB_ID
 * @param param - Reference to buffer.
 * @param dst_addr - Address of the device to send command to.
 * @param dst_addr_mode - Address mode for dst_addr.
 * @param dst_ep - Destination endpoint.
 * @param src_ep - Current endpoint.
 * @param payload - Packet payload (ref to @ref zb_zcl_metering_request_fast_poll_mode_payload_t).
 * @param cb - Callback which should be called when the ZCL stack receives APS ack.
 */
void zb_zcl_metering_send_cmd_request_fast_poll_mode(zb_uint8_t param, zb_addr_u *dst_addr,
                                                          zb_aps_addr_mode_t dst_addr_mode, zb_uint8_t dst_ep,
                                                          zb_uint8_t src_ep,
                                                          zb_zcl_metering_request_fast_poll_mode_payload_t *payload,
                                                          zb_callback_t cb);


/** Function for send @ref ZB_ZCL_METERING_CLI_CMD_GET_PROFILE "GetProfile" command.
 * On receiver's side callback ZCL device callback @ref ZB_ZCL_REGISTER_DEVICE_CB will be called with
 * @ref ZB_ZCL_METERING_GET_PROFILE_CB_ID
 * @param param - Reference to buffer.
 * @param dst_addr - Address of the device to send command to.
 * @param dst_addr_mode - Address mode for dst_addr.
 * @param dst_ep - Destination endpoint.
 * @param src_ep - Current endpoint.
 * @param payload - Packet payload (ref to @ref zb_zcl_metering_get_profile_payload_t).
 * @param cb - Callback which should be called when the ZCL stack receives APS ack.
 */
void zb_zcl_metering_send_cmd_get_profile(zb_uint8_t param, zb_addr_u *dst_addr,
                                               zb_aps_addr_mode_t dst_addr_mode, zb_uint8_t dst_ep,
                                               zb_uint8_t src_ep, zb_zcl_metering_get_profile_payload_t *payload,
                                               zb_callback_t cb);


/** Function for send @ref ZB_ZCL_METERING_SRV_CMD_PUBLISH_SNAPSHOT "PublishSnapshot" command.
 * On receiver's side callback ZCL device callback @ref ZB_ZCL_REGISTER_DEVICE_CB will be called with
 * @ref ZB_ZCL_METERING_PUBLISH_SNAPSHOT_CB_ID
 * @param param - Reference to buffer.
 * @param dst_addr - Address of the device to send command to.
 * @param dst_addr_mode - Address mode for dst_addr.
 * @param dst_ep - Destination endpoint.
 * @param src_ep - Current endpoint.
 * @param pl - Packet payload (ref to @ref zb_zcl_metering_publish_snapshot_payload_t).
 * @param cb - Callback which should be called when the ZCL stack receives APS ack.
 */
void zb_zcl_metering_send_cmd_publish_snapshot(zb_uint8_t param, const zb_addr_u *dst_addr,
                                                    zb_aps_addr_mode_t dst_addr_mode, zb_uint8_t dst_ep,
                                                    zb_uint8_t src_ep,
                                                    const zb_zcl_metering_publish_snapshot_payload_t *pl,
                                                    zb_callback_t cb);


/** Function for send @ref ZB_ZCL_METERING_SRV_CMD_REQUEST_FAST_POLL_MODE_RESPONSE "RequestFastPollMode" command.
 * On receiver's side callback ZCL device callback @ref ZB_ZCL_REGISTER_DEVICE_CB will be called with
 * @ref ZB_ZCL_METERING_REQUEST_FAST_POLL_MODE_CB_ID
 * @param param - Reference to buffer.
 * @param dst_addr - Address of the device to send command to.
 * @param dst_addr_mode - Address mode for dst_addr.
 * @param dst_ep - Destination endpoint.
 * @param src_ep - Current endpoint.
 * @param pl - Packet payload (ref to @ref zb_zcl_metering_request_fast_poll_mode_response_payload_t).
 * @param pl_size - size in bytes of input payload. Stack can calculate actual payload size.
 * Set pl_size to 0 for it.
 * @param tsn - transaction sequence number of response
 * @param cb - Callback which should be called when the ZCL stack receives APS ack.
 */
void zb_zcl_metering_send_cmd_request_fast_poll_mode_response(zb_uint8_t param, const zb_addr_u *dst_addr,
                                                                   zb_aps_addr_mode_t dst_addr_mode, zb_uint8_t dst_ep,
                                                                   zb_uint8_t src_ep,
                                                                   const zb_zcl_metering_request_fast_poll_mode_response_payload_t *pl,
                                                                   zb_uint8_t pl_size, zb_uint8_t tsn, zb_callback_t cb);


/** Function for send @ref ZB_ZCL_METERING_SRV_CMD_GET_PROFILE_RESPONSE "GetProfileResponse" command.
 * On receiver's side callback ZCL device callback @ref ZB_ZCL_REGISTER_DEVICE_CB will be called with
 * @ref ZB_ZCL_METERING_GET_PROFILE_RESPONSE_CB_ID
 * @param param - Reference to buffer.
 * @param dst_addr - Address of the device to send command to.
 * @param dst_addr_mode - Address mode for dst_addr.
 * @param dst_ep - Destination endpoint.
 * @param src_ep - Current endpoint.
 * @param pl - Packet payload (ref to @ref zb_zcl_metering_get_profile_response_payload_t).
 * @param pl_size - size in bytes of input payload. Stack can calculate actual payload size.
 * Set pl_size to 0 for it.
 * @param tsn - transaction sequence number of response
 * @param cb - Callback which should be called when the ZCL stack receives APS ack.
 */
void zb_zcl_metering_send_cmd_get_profile_response(zb_uint8_t param, const zb_addr_u *dst_addr,
                                                        zb_aps_addr_mode_t dst_addr_mode, zb_uint8_t dst_ep,
                                                        zb_uint8_t src_ep, const zb_zcl_metering_get_profile_response_payload_t *pl,
                                                        zb_uint8_t pl_size, zb_uint8_t tsn, zb_callback_t cb);


/** Function for send @ref ZB_ZCL_METERING_SRV_CMD_GET_PROFILE_RESPONSE "GetSampledDataResponse" command.
 * On receiver's side callback ZCL device callback @ref ZB_ZCL_REGISTER_DEVICE_CB will be called with
 * @ref ZB_ZCL_METERING_GET_SAMPLED_DATA_RESPONSE_CB_ID
 * @param param - Reference to buffer.
 * @param dst_addr - Address of the device to send command to.
 * @param dst_addr_mode - Address mode for dst_addr.
 * @param dst_ep - Destination endpoint.
 * @param src_ep - Current endpoint.
 * @param pl - Packet payload (ref to @ref zb_zcl_metering_get_sampled_data_response_payload_t).
 * @param pl_size - size in bytes of input payload. Stack can calculate actual payload size.
 * Set pl_size to 0 for it.
 * @param tsn - transaction sequence number of response
 * @param cb - Callback which should be called when the ZCL stack receives APS ack.
 */
void zb_zcl_metering_send_cmd_get_sampled_data_response(zb_uint8_t param, const zb_addr_u *dst_addr,
                                                             zb_aps_addr_mode_t dst_addr_mode, zb_uint8_t dst_ep,
                                                             zb_uint8_t src_ep,
                                                             const zb_zcl_metering_get_sampled_data_response_payload_t *pl,
                                                             zb_uint8_t pl_size, zb_uint8_t tsn, zb_callback_t cb);

/** Macro for call @ref zb_zcl_metering_send_cmd_get_snapshot function
 */
#define ZB_ZCL_METERING_SEND_CMD_GET_SNAPSHOT(_param, _dst_addr, _addr_mode,  \
                                              _dst_ep, _src_ep, _payload, cb) \
  zb_zcl_metering_send_cmd_get_snapshot(_param, _dst_addr, _addr_mode,        \
                                        _dst_ep, _src_ep, _payload, cb)


/** Macro for call @ref zb_zcl_metering_send_cmd_get_sampled_data function
 */
#define ZB_ZCL_METERING_SEND_CMD_GET_SAMPLED_DATA(_param, _dst_addr, _addr_mode,  \
                                                  _dst_ep, _src_ep, _payload, cb) \
  zb_zcl_metering_send_cmd_get_sampled_data(_param, _dst_addr, _addr_mode,        \
                                            _dst_ep, _src_ep, _payload, cb)



/** Macro for call @ref zb_zcl_metering_send_cmd_request_fast_poll_mode function
 */
#define ZB_ZCL_METERING_SEND_CMD_REQUEST_FAST_POLL_MODE(_param, _dst_addr, _addr_mode,  \
                                                        _dst_ep, _src_ep, _payload, cb) \
  zb_zcl_metering_send_cmd_request_fast_poll_mode(_param, _dst_addr, _addr_mode,        \
                                                  _dst_ep, _src_ep, _payload, cb)


/** Macro for call @ref zb_zcl_metering_send_cmd_get_profile function
 */
#define ZB_ZCL_METERING_SEND_CMD_GET_PROFILE(_param, _dst_addr, _addr_mode,  \
                                             _dst_ep, _src_ep, _payload, cb) \
  zb_zcl_metering_send_cmd_get_profile(_param, _dst_addr, _addr_mode,        \
                                       _dst_ep, _src_ep, _payload, cb)


/** Macro for call @ref zb_zcl_metering_send_cmd_publish_snapshot function
 */
#define ZB_ZCL_METERING_SEND_CMD_PUBLISH_SNAPSHOT(_param, _dst_addr, _addr_mode,  \
                                                  _dst_ep, _src_ep, _payload, cb) \
  zb_zcl_metering_send_cmd_publish_snapshot(_param, _dst_addr, _addr_mode,        \
                                            _dst_ep, _src_ep, _payload, cb)


/** Macro for call @ref zb_zcl_metering_send_cmd_request_fast_poll_mode_response function
 */
#define ZB_ZCL_METERING_SEND_CMD_REQUEST_FAST_POLL_MODE_RESPONSE(_param, _dst_addr, _addr_mode,  \
                                                                 _dst_ep, _src_ep, _payload, cb) \
  zb_zcl_metering_send_cmd_request_fast_poll_mode_response(_param, _dst_addr, _addr_mode,        \
                                                           _dst_ep, _src_ep, _payload, cb)


/** Macro for call @ref zb_zcl_metering_send_cmd_get_profile_response function
 */
#define ZB_ZCL_METERING_SEND_CMD_GET_PROFILE_RESPONSE(_param, _dst_addr, _addr_mode, _dst_ep,  \
                                                      _src_ep, _payload, _pl_size, cb)         \
  zb_zcl_metering_send_cmd_get_profile_response(_param, _dst_addr, _addr_mode, _dst_ep,        \
                                                _src_ep, _payload, _pl_size, cb)


/** Macro for call @ref zb_zcl_metering_send_cmd_get_sampled_data_response function
 */
#define ZB_ZCL_METERING_SEND_CMD_GET_SAMPLED_DATA_RESPONSE(_param, _dst_addr, _addr_mode, _dst_ep, \
                                                           _src_ep, _payload, _pl_size, cb)        \
  zb_zcl_metering_send_cmd_get_sampled_data_response(_param, _dst_addr, _addr_mode, _dst_ep,       \
                                                     _src_ep, _payload, _pl_size, cb)


/** @} */ /* ZB_ZCL_METERING_CMDS */


/*! @cond internals_doc */
/*! @internal @name Metering cluster internals
    Internal structures for attribute representation in cluster definitions.
     @{
*/

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_METERING_CURRENT_SUMMATION_DELIVERED_ID(data_ptr) \
{                                                                                            \
  ZB_ZCL_ATTR_METERING_CURRENT_SUMMATION_DELIVERED_ID,                                       \
  ZB_ZCL_ATTR_TYPE_U48,                                                                      \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY | ZB_ZCL_ATTR_ACCESS_REPORTING,                               \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                                        \
  (void*) data_ptr                                                                           \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_METERING_STATUS_ID(data_ptr)                      \
{                                                                                            \
  ZB_ZCL_ATTR_METERING_STATUS_ID,                                                            \
  ZB_ZCL_ATTR_TYPE_8BITMAP,                                                                  \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY | ZB_ZCL_ATTR_ACCESS_REPORTING,                               \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                                        \
  (void*) data_ptr                                                                           \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_METERING_UNIT_OF_MEASURE_ID(data_ptr)             \
{                                                                                            \
  ZB_ZCL_ATTR_METERING_UNIT_OF_MEASURE_ID,                                                   \
  ZB_ZCL_ATTR_TYPE_8BIT_ENUM,                                                                \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                                              \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                                        \
  (void*) data_ptr                                                                           \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_METERING_SUMMATION_FORMATTING_ID(data_ptr)        \
{                                                                                            \
  ZB_ZCL_ATTR_METERING_SUMMATION_FORMATTING_ID,                                              \
  ZB_ZCL_ATTR_TYPE_8BITMAP,                                                                  \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                                              \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                                        \
  (void*) data_ptr                                                                           \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_METERING_METERING_DEVICE_TYPE_ID(data_ptr)        \
{                                                                                            \
  ZB_ZCL_ATTR_METERING_METERING_DEVICE_TYPE_ID,                                              \
  ZB_ZCL_ATTR_TYPE_8BITMAP,                                                                  \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                                              \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                                        \
  (void*) data_ptr                                                                           \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_METERING_INSTANTANEOUS_DEMAND_ID(data_ptr)        \
{                                                                                            \
  ZB_ZCL_ATTR_METERING_INSTANTANEOUS_DEMAND_ID,                                              \
  ZB_ZCL_ATTR_TYPE_S24,                                                                      \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY | ZB_ZCL_ATTR_ACCESS_REPORTING,                               \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                                        \
  (void*) data_ptr                                                                           \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_METERING_DEMAND_FORMATTING_ID(data_ptr)           \
{                                                                                            \
  ZB_ZCL_ATTR_METERING_DEMAND_FORMATTING_ID,                                                 \
  ZB_ZCL_ATTR_TYPE_8BITMAP,                                                                  \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                                              \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                                        \
  (void*) data_ptr                                                                           \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_METERING_HISTORICAL_CONSUMPTION_FORMATTING_ID(data_ptr)      \
{                                                                                            \
  ZB_ZCL_ATTR_METERING_HISTORICAL_CONSUMPTION_FORMATTING_ID,                                 \
  ZB_ZCL_ATTR_TYPE_8BITMAP,                                                                  \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                                              \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                                        \
  (void*) data_ptr                                                                           \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_METERING_MULTIPLIER_ID(data_ptr)                  \
{                                                                                            \
  ZB_ZCL_ATTR_METERING_MULTIPLIER_ID,                                                        \
  ZB_ZCL_ATTR_TYPE_U24,                                                                      \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                                              \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                                        \
  (void*) data_ptr                                                                           \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_METERING_DIVISOR_ID(data_ptr)                     \
{                                                                                            \
  ZB_ZCL_ATTR_METERING_DIVISOR_ID,                                                           \
  ZB_ZCL_ATTR_TYPE_U24,                                                                      \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                                              \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                                        \
  (void*) data_ptr                                                                           \
}


/*! @internal Number of attributes mandatory for reporting in Metering cluster */
#define ZB_ZCL_METERING_REPORT_ATTR_COUNT 3

/*! @}
    @endcond */ /* Metering cluster internals */


/** @struct zb_zcl_metering_attrs_t
 *  @brief Metering cluster attributes
 */
/** @brief Declare attribute list for Electrical Measurement cluster
    @param attr_list - attribute list name
    @param curr_summ_delivered - pointer to variable to store Current Summation Delivered attribute value
    @param status - pointer to variable to store Status attribute value
    @param unit_of_measure - pointer to variable to store Unit Of Measure attribute value
    @param summation_formatting - pointer to variable to store Summation Formatting attribute value
    @param metering_device_type - pointer to variable to store Device Type attribute value
*/
#define ZB_ZCL_DECLARE_METERING_ATTRIB_LIST(attr_list, curr_summ_delivered, status, unit_of_measure, \
                                            summation_formatting, metering_device_type)              \
  ZB_ZCL_START_DECLARE_ATTRIB_LIST_CLUSTER_REVISION(attr_list, ZB_ZCL_METERING)                      \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_METERING_CURRENT_SUMMATION_DELIVERED_ID, (curr_summ_delivered))   \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_METERING_STATUS_ID, (status))                                     \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_METERING_UNIT_OF_MEASURE_ID, (unit_of_measure))                   \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_METERING_SUMMATION_FORMATTING_ID, (summation_formatting))         \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_METERING_METERING_DEVICE_TYPE_ID, (metering_device_type))         \
  ZB_ZCL_FINISH_DECLARE_ATTRIB_LIST


/** @brief Declare attribute list for Electrical Measurement cluster (extended attribute set)
    @param attr_list - attribute list name
    @param curr_summ_delivered - pointer to variable to store Current Summ Delivered attribute value
    @param status - pointer to variable to store Status attribute value
    @param unit_of_measure - pointer to variable to store Unit Of Measure attribute value
    @param summation_formatting - pointer to variable to store Summation Formatting attribute value
    @param metering_device_type - pointer to variable to store Device Type attribute value
    @param instantaneous_demand - pointer to variable to store Instantaneous Demand attribute value
    @param demand_formatting - pointer to variable to store Demand Formatting attribute value
    @param historical_consumption_formatting - pointer to variable to store Historical Consumption Formatting attribute value
    @param multiplier - pointer to variable to store Multiplier attribute value
    @param divisor - pointer to variable to store Divisor attribute value
*/
#define ZB_ZCL_DECLARE_METERING_ATTRIB_LIST_EXT(attr_list,                                           \
      curr_summ_delivered, status, unit_of_measure, summation_formatting, metering_device_type,      \
      instantaneous_demand, demand_formatting, historical_consumption_formatting, multiplier, divisor)     \
  ZB_ZCL_START_DECLARE_ATTRIB_LIST_CLUSTER_REVISION(attr_list, ZB_ZCL_METERING)                      \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_METERING_CURRENT_SUMMATION_DELIVERED_ID, (curr_summ_delivered))   \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_METERING_STATUS_ID, (status))                                     \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_METERING_UNIT_OF_MEASURE_ID, (unit_of_measure))                   \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_METERING_SUMMATION_FORMATTING_ID, (summation_formatting))         \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_METERING_METERING_DEVICE_TYPE_ID, (metering_device_type))         \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_METERING_INSTANTANEOUS_DEMAND_ID, (instantaneous_demand))         \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_METERING_DEMAND_FORMATTING_ID, (demand_formatting))               \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_METERING_HISTORICAL_CONSUMPTION_FORMATTING_ID, (historical_consumption_formatting))  \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_METERING_MULTIPLIER_ID, (multiplier))                             \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_METERING_DIVISOR_ID, (divisor))                                   \
  ZB_ZCL_FINISH_DECLARE_ATTRIB_LIST

typedef struct zb_zcl_metering_attrs_s
{

  /** @copydoc ZB_ZCL_ATTR_METERING_CURRENT_SUMMATION_DELIVERED_ID
   * @see ZB_ZCL_ATTR_METERING_CURRENT_SUMMATION_DELIVERED_ID
   */
  zb_uint48_t curr_summ_delivered;

  /** @copydoc ZB_ZCL_ATTR_METERING_STATUS_ID
   * @see ZB_ZCL_ATTR_METERING_STATUS_ID
   */
  zb_uint8_t status;

  /** @copydoc ZB_ZCL_ATTR_METERING_UNIT_OF_MEASURE_ID
   * @see ZB_ZCL_ATTR_METERING_UNIT_OF_MEASURE_ID
   */
  zb_uint8_t unit_of_measure;

  /** @copydoc ZB_ZCL_ATTR_METERING_SUMMATION_FORMATTING_ID
   * @see ZB_ZCL_ATTR_METERING_SUMMATION_FORMATTING_ID
   */
  zb_uint8_t summation_formatting;

  /** @copydoc ZB_ZCL_ATTR_METERING_METERING_DEVICE_TYPE_ID
   * @see ZB_ZCL_ATTR_METERING_METERING_DEVICE_TYPE_ID
   */
  zb_uint8_t device_type;
} zb_zcl_metering_attrs_t;


/** @brief Declare attribute list for Metering cluster cluster
 *  @param[in]  attr_list - attribute list variable name
 *  @param[in]  attrs - variable of @ref zb_zcl_metering_attrs_t type (containing Metering cluster attributes)
 */
#define ZB_ZCL_DECLARE_METERING_ATTR_LIST(attr_list, attrs) \
  ZB_ZCL_DECLARE_METERING_ATTRIB_LIST(attr_list, &attrs.curr_summ_delivered, \
    &attrs.status, &attrs.unit_of_measure, &attrs.summation_formatting,      \
    &attrs.device_type)


/** @} */ /* ZB_ZCL_METERING ZCL SE Metering cluster definitions */

/** @endcond */ /* DOXYGEN_ZCL_SECTION */

void zb_zcl_metering_init_server(void);
void zb_zcl_metering_init_client(void);


#define ZB_ZCL_CLUSTER_ID_METERING_SERVER_ROLE_INIT zb_zcl_metering_init_server
#define ZB_ZCL_CLUSTER_ID_METERING_CLIENT_ROLE_INIT zb_zcl_metering_init_client

#endif /* ZB_ZCL_METERING_H */
