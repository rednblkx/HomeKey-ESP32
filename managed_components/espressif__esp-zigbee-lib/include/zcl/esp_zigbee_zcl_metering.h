/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once
#ifdef __cplusplus
extern "C" {
#endif

#include "esp_zigbee_type.h"

/** @brief Metering cluster client attribute sets */
typedef enum {
    ESP_ZB_ZCL_METERING_SET_CLI_NOTIFICATION_ATTRIBUTE = 0x00, /*!< Notification Attribute Set */
} esp_zb_zcl_metering_cli_attr_set_t;

/** @brief Metering cluster client attributes */
typedef enum {
    ESP_ZB_ZCL_ATTR_METERING_CLI_FUNCTIONAL_NOTIFICATION_FLAGS = ESP_ZB_ZCL_ATTR_SET_WITH_ATTR_ID(
        ESP_ZB_ZCL_METERING_SET_CLI_NOTIFICATION_ATTRIBUTE, 0x00), /*!< FunctionalNotificationFlags attribute is implemented as a set of bit flags
                                                                    *   which are have a predefined action associated with a bit that is not based on a specific command,
                                                                    *   but may require the Mirrored device to trigger some additional functionality within the system */
    ESP_ZB_ZCL_ATTR_METERING_CLI_NOTIFICATION_FLAGS2,              /*!< NotificationFlagsN are 32-bit bitmaps that each represent a series of flags.
                                                                    *   Each flag represents an outstanding command that the Mirror is holding on behalf of the BOMD.
                                                                    *   Each flag represents a different command.
                                                                    *   The format of these attributes is dictated by the scheme that is currently in operation */
    ESP_ZB_ZCL_ATTR_METERING_CLI_NOTIFICATION_FLAGS3,              /*!< ESP_ZB_ZCL_ATTR_METERING_CLI_NOTIFICATION_FLAGS3 */
    ESP_ZB_ZCL_ATTR_METERING_CLI_NOTIFICATION_FLAGS4,              /*!< ESP_ZB_ZCL_ATTR_METERING_CLI_NOTIFICATION_FLAGS4 */
    ESP_ZB_ZCL_ATTR_METERING_CLI_NOTIFICATION_FLAGS5,              /*!< ESP_ZB_ZCL_ATTR_METERING_CLI_NOTIFICATION_FLAGS5 */
    ESP_ZB_ZCL_ATTR_METERING_CLI_NOTIFICATION_FLAGS6,              /*!< ESP_ZB_ZCL_ATTR_METERING_CLI_NOTIFICATION_FLAGS6 */
    ESP_ZB_ZCL_ATTR_METERING_CLI_NOTIFICATION_FLAGS7,              /*!< ESP_ZB_ZCL_ATTR_METERING_CLI_NOTIFICATION_FLAGS7 */
    ESP_ZB_ZCL_ATTR_METERING_CLI_NOTIFICATION_FLAGS8,              /*!< ESP_ZB_ZCL_ATTR_METERING_CLI_NOTIFICATION_FLAGS8 */
} esp_zb_zcl_metering_cli_attr_t;

/** @brief Metering cluster server attribute sets */
typedef enum {
    ESP_ZB_ZCL_METERING_SET_READ_INFO                          = 0x00, /*!< Reading Information Set */
    ESP_ZB_ZCL_METERING_SET_TOU_INFORMATION                    = 0x01, /*!< TOU Information Set */
    ESP_ZB_ZCL_METERING_SET_METER_STATUS                       = 0x02, /*!< Meter Status */
    ESP_ZB_ZCL_METERING_SET_FORMATTING                         = 0x03, /*!< Formatting */
    ESP_ZB_ZCL_METERING_SET_HISTORICAL_CONSUMPTION             = 0x04, /*!< Historical Consumption */
    ESP_ZB_ZCL_METERING_SET_LOAD_PROFILE_CONFIGURATION         = 0x05, /*!< Load Profile Configuration */
    ESP_ZB_ZCL_METERING_SET_SUPPLY_LIMIT                       = 0x06, /*!< Supply Limit */
    ESP_ZB_ZCL_METERING_SET_BLOCK_INFORMATION_DELIVERED        = 0x07, /*!< Block Information (Delivered) */
    ESP_ZB_ZCL_METERING_SET_ALARMS                             = 0x08, /*!< Alarms */
    ESP_ZB_ZCL_METERING_SET_BLOCK_INFORMATION_RECEIVED         = 0x09, /*!< Block Information (Received) */
    ESP_ZB_ZCL_METERING_SET_METER_BILLING                      = 0x0A, /*!< Meter Billing Attribute Set */
    ESP_ZB_ZCL_METERING_SET_SUPPLY_CONTROL                     = 0x0B, /*!< Supply Control Attribute Set */
    ESP_ZB_ZCL_METERING_SET_ALTERNATIVE_HISTORICAL_CONSUMPTION = 0x0C, /*!< Alternative Historical Consumption */
} esp_zb_zcl_metering_srv_attr_set_t;

/** @brief Metering cluster server attributes */
typedef enum {
    /* Reading Information Attribute Set */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_SUMMATION_DELIVERED_ID = ESP_ZB_ZCL_ATTR_SET_WITH_ATTR_ID(ESP_ZB_ZCL_METERING_SET_READ_INFO, 0x00), /*!< CurrentSummationDelivered attribute
                                                                                                                                          *   represents the most recent summed value of Energy, Gas,
                                                                                                                                          *   or Water delivered and consumed in the premises */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_SUMMATION_RECEIVED_ID,        /*!< CurrentSummationReceived attribute represents the most recent summed value of Energy,
                                                                    *   Gas, or Water generated and delivered from the premises
                                                                    */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_MAX_DEMAND_DELIVERED_ID,      /*!< CurrentMaxDemandDelivered attribute represents the maximum demand or rate of delivered
                                                                    *   value of Energy, Gas, or Water being utilized at the premises
                                                                    */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_MAX_DEMAND_RECEIVED_ID,       /*!< CurrentMaxDemandReceived attribute represents the maximum demand or rate of received
                                                                    *   value of Energy, Gas, or Water being utilized by the utility
                                                                    */
    ESP_ZB_ZCL_ATTR_METERING_DFT_SUMMATION_ID,                     /*!< DFTSummation attribute represents a snapshot of attribute CurrentSummationDelivered
                                                                    *   captured at the time indicated by attribute "DailyFreezeTime"
                                                                    */
    ESP_ZB_ZCL_ATTR_METERING_DAILY_FREEZE_TIME_ID,                 /*!< DailyFreezeTime attribute represents the time of day when DFTSummation is captured (hour and minutes) */
    ESP_ZB_ZCL_ATTR_METERING_POWER_FACTOR_ID,                      /*!< PowerFactor attribute contains the Average Power Factor ratio in 1/100th. Valid values are 0 to 99 */
    ESP_ZB_ZCL_ATTR_METERING_READING_SNAPSHOT_TIME_ID,             /*!< ReadingSnapshotTime attribute represents the last time all of the "CurrentSummationDelivered", "CurrentSummationReceived",
                                                                    *   "CurrentMaxDemandDelivered" and "CurrentMaxDemandReceived" attributes that are supported by the device were updated
                                                                    */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_MAX_DEMAND_DELIVERED_TIME_ID, /*!< CurrentMaxDemandDeliveredTime attribute represents the represents the time when
                                                                    *   "CurrentMaxDemandDelivered" reading was captured
                                                                    */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_MAX_DEMAND_RECEIVED_TIME_ID,  /*!< CurrentMaxDemandReceivedTime attribute represents the time when
                                                                    *   "CurrentMaxDemandReceived reading was captured
                                                                    */
    ESP_ZB_ZCL_ATTR_METERING_DEFAULT_UPDATE_PERIOD_ID,             /*!< DefaultUpdatePeriod attribute represents the interval (seconds) at which the
                                                                    *   "InstantaneousDemand" attribute is updated when not in fast poll mode
                                                                    */
    ESP_ZB_ZCL_ATTR_METERING_FAST_POLL_UPDATE_PERIOD_ID,           /*!< FastPollUpdatePeriod attribute represents the interval (seconds) at which the
                                                                    *   "InstantaneousDemand" attribute is updated when in fast poll mode
                                                                    */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_BLOCK_PERIOD_CONSUMPTION_DELIVERED_ID, /*!< CurrentBlockPeriodConsumptionDelivered attribute represents the most recent summed
                                                                             *   value of Energy, Gas or Water delivered and consumed in the premises during the Block Tariff Period
                                                                             */
    ESP_ZB_ZCL_ATTR_METERING_DAILY_CONSUMPTION_TARGET_ID,                   /*!< DailyConsumptionTarget attribute is a daily target consumption amount that can be
                                                                             *   displayed to the consumer on a HAN device, with the intent that it can be used to compare
                                                                             *   to actual daily consumption
                                                                             */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_BLOCK_ID,                              /*!< CurrentBlock attribute is an 8-bit Enumeration which indicates the currently active
                                                                             *   block, when Block Tariffs are enabled
                                                                             */
    ESP_ZB_ZCL_ATTR_METERING_PROFILE_INTERVAL_PERIOD_ID,                    /*!< ProfileIntervalPeriod attribute is currently included in the GetProfileResponse
                                                                             *   command payload, but does not appear in an attribute set. It represents the interval or
                                                                             *   time frame used to capture metered Energy, Gas, and Water consumption for profiling purposes
                                                                             */
    /* 0x0010 attribute is deprecated */

    ESP_ZB_ZCL_ATTR_METERING_PRESET_READING_TIME_ID = ESP_ZB_ZCL_ATTR_SET_WITH_ATTR_ID(ESP_ZB_ZCL_METERING_SET_READ_INFO, 0x11), /*!< PresetReadingTime attribute */
    ESP_ZB_ZCL_ATTR_METERING_SUMMATION_DELIVERED_PER_REPORT_ID,              /*!< SummationDeliveredPerReport attribute represents the summation increment per report
                                                                              *   from the water or gas meter
                                                                              */
    ESP_ZB_ZCL_ATTR_METERING_FLOW_RESTRICTION_ID,                            /*!< FlowRestriction attribute represents the volume per minute limit set in the flow
                                                                              *   restrictor. This applies to water but not for gas. A setting of 0xFF indicates this feature is disabled
                                                                              */
    ESP_ZB_ZCL_ATTR_METERING_SUPPLY_STATUS_ID,                               /*!< SupplyStatus attribute represents the state of the supply at the customer's premises */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_INLET_ENERGY_CARRIER_SUMMATION_ID,      /*!< CurrentInletEnergyCarrierSummation attribute is the current integrated volume of a
                                                                              *   given energy carrier measured on the inlet
                                                                              */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_OUTLET_ENERGY_CARRIER_SUMMATION_ID,     /*!< CurrentOutletEnergyCarrierSummation attribute is the current integrated volume of a
                                                                              *   given energy carrier measured on the outlet
                                                                              */
    ESP_ZB_ZCL_ATTR_METERING_INLET_TEMPERATURE_ID,                           /*!< InletTemperature attribute is the temperature measured on the energy carrier inlet */
    ESP_ZB_ZCL_ATTR_METERING_OUTLET_TEMPERATURE_ID,                          /*!< OutletTemperature attribute is the temperature measured on the energy carrier outlet */
    ESP_ZB_ZCL_ATTR_METERING_CONTROL_TEMPERATURE_ID,                         /*!< ControlTemperature attribute is a reference temperature measured on the meter used to
                                                                              *   validate the Inlet/Outlet temperatures
                                                                              */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_INLET_ENERGY_CARRIER_DEMAND_ID,         /*!< CurrentInletEnergyCarrierDemand attribute is the current absolute demand on the energy carrier inlet */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_OUTLET_ENERGY_CARRIER_DEMAND_ID,        /*!< CurrentOutletEnergyCarrierDemand attribute is the current absolute demand on the energy carrier outlet */
    ESP_ZB_ZCL_ATTR_METERING_PREVIOUS_BLOCK_PERIOD_CONSUMPTION_DELIVERED_ID, /*!< PreviousBlockPeriodConsumptionDelivered attribute represents the total value of Energy,
                                                                              *   Gas or Water delivered and consumed in the premises at the end of the previous Block Tariff period
                                                                              */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_BLOCK_PERIOD_CONSUMPTION_RECEIVED_ID,   /*!< CurrentBlockPeriodConsumptionReceived attribute represents the most recent summed
                                                                              *   value of Energy, Gas or Water received by the energy supplier from the premises during the Block Tariff Period
                                                                              */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_BLOCK_RECEIVED_ID,                      /*!< CurrentBlockReceived attribute is an 8-bit Enumeration which indicates the currently
                                                                              *   active block, when Block Tariffs are enabled
                                                                              */
    ESP_ZB_ZCL_ATTR_METERING_DFT_SUMMATION_RECEIVED_ID,                      /*!< DFTSummationReceived attribute represents a snapshot of attribute
                                                                              *   "CurrentSummationReceived" captured at the time indicated by the "DailyFreezeTime" attribute
                                                                              */
    ESP_ZB_ZCL_ATTR_METERING_ACTIVE_REGISTER_TIER_DELIVERED_ID,              /*!< ActiveRegisterTierDelivered attribute indicates the current register tier that the
                                                                              *   energy consumed is being accumulated against
                                                                              */
    ESP_ZB_ZCL_ATTR_METERING_ACTIVE_REGISTER_TIER_RECEIVED_ID,               /*!< ActiveRegisterTierReceived attribute indicates the current register tier that the
                                                                              *   energy generated is being accumulated against
                                                                              */
    ESP_ZB_ZCL_ATTR_METERING_LAST_BLOCK_SWITCH_TIME_ID,                      /*!< LastBlockSwitchTime attribute allows other devices to determine the time at which
                                                                              *   a meter switches from one block to another
                                                                              */
    /* 0x0023 to 0x00FF reserved */


    /* TOU Information Attribute Set */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER1_SUMMATION_DELIVERED_ID =
        ESP_ZB_ZCL_ATTR_SET_WITH_ATTR_ID(ESP_ZB_ZCL_METERING_SET_TOU_INFORMATION, 0x00), /*!< CurrentTierNSummationDelivered
                                                                                          *   attributes represent the most recent summed value of Energy, Gas, or Water delivered to the premises at a
                                                                                          *   specific price tier as defined by a TOU schedule or a real time pricing period
                                                                                          */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER1_SUMMATION_RECEIVED_ID,                        /*!< CurrentTierNSummationReceived attributes represent the most recent summed value of Energy, Gas, or Water
                                                                                          *   provided by the premises (i.e. received by the utility from the customer) at a specific price tier as defined
                                                                                          *   by a TOU schedule or a real time pricing period
                                                                                          */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER2_SUMMATION_DELIVERED_ID,                       /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER2_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER2_SUMMATION_RECEIVED_ID,                        /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER2_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER3_SUMMATION_DELIVERED_ID,                       /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER3_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER3_SUMMATION_RECEIVED_ID,                        /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER3_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER4_SUMMATION_DELIVERED_ID,                       /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER4_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER4_SUMMATION_RECEIVED_ID,                        /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER4_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER5_SUMMATION_DELIVERED_ID,                       /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER5_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER5_SUMMATION_RECEIVED_ID,                        /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER5_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER6_SUMMATION_DELIVERED_ID,                       /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER6_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER6_SUMMATION_RECEIVED_ID,                        /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER6_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER7_SUMMATION_DELIVERED_ID,                       /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER7_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER7_SUMMATION_RECEIVED_ID,                        /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER7_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER8_SUMMATION_DELIVERED_ID,                       /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER8_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER8_SUMMATION_RECEIVED_ID,                        /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER8_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER9_SUMMATION_DELIVERED_ID,                       /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER9_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER9_SUMMATION_RECEIVED_ID,                        /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER9_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER10_SUMMATION_DELIVERED_ID,                      /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER10_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER10_SUMMATION_RECEIVED_ID,                       /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER10_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER11_SUMMATION_DELIVERED_ID,                      /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER11_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER11_SUMMATION_RECEIVED_ID,                       /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER11_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER12_SUMMATION_DELIVERED_ID,                      /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER12_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER12_SUMMATION_RECEIVED_ID,                       /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER12_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER13_SUMMATION_DELIVERED_ID,                      /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER13_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER13_SUMMATION_RECEIVED_ID,                       /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER13_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER14_SUMMATION_DELIVERED_ID,                      /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER14_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER14_SUMMATION_RECEIVED_ID,                       /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER14_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER15_SUMMATION_DELIVERED_ID,                      /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER15_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER15_SUMMATION_RECEIVED_ID,                       /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER15_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER16_SUMMATION_DELIVERED_ID,                      /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER16_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER16_SUMMATION_RECEIVED_ID,                       /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER16_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER17_SUMMATION_DELIVERED_ID,                      /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER17_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER17_SUMMATION_RECEIVED_ID,                       /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER17_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER18_SUMMATION_DELIVERED_ID,                      /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER18_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER18_SUMMATION_RECEIVED_ID,                       /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER18_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER19_SUMMATION_DELIVERED_ID,                      /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER19_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER19_SUMMATION_RECEIVED_ID,                       /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER19_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER20_SUMMATION_DELIVERED_ID,                      /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER20_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER20_SUMMATION_RECEIVED_ID,                       /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER20_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER21_SUMMATION_DELIVERED_ID,                      /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER21_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER21_SUMMATION_RECEIVED_ID,                       /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER21_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER22_SUMMATION_DELIVERED_ID,                      /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER22_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER22_SUMMATION_RECEIVED_ID,                       /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER22_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER23_SUMMATION_DELIVERED_ID,                      /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER23_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER23_SUMMATION_RECEIVED_ID,                       /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER23_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER24_SUMMATION_DELIVERED_ID,                      /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER24_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER24_SUMMATION_RECEIVED_ID,                       /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER24_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER25_SUMMATION_DELIVERED_ID,                      /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER25_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER25_SUMMATION_RECEIVED_ID,                       /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER25_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER26_SUMMATION_DELIVERED_ID,                      /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER26_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER26_SUMMATION_RECEIVED_ID,                       /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER26_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER27_SUMMATION_DELIVERED_ID,                      /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER27_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER27_SUMMATION_RECEIVED_ID,                       /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER27_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER28_SUMMATION_DELIVERED_ID,                      /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER28_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER28_SUMMATION_RECEIVED_ID,                       /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER28_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER29_SUMMATION_DELIVERED_ID,                      /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER29_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER29_SUMMATION_RECEIVED_ID,                       /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER29_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER30_SUMMATION_DELIVERED_ID,                      /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER30_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER30_SUMMATION_RECEIVED_ID,                       /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER30_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER31_SUMMATION_DELIVERED_ID,                      /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER31_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER31_SUMMATION_RECEIVED_ID,                       /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER31_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER32_SUMMATION_DELIVERED_ID,                      /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER32_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER32_SUMMATION_RECEIVED_ID,                       /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER32_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER33_SUMMATION_DELIVERED_ID,                      /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER33_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER33_SUMMATION_RECEIVED_ID,                       /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER33_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER34_SUMMATION_DELIVERED_ID,                      /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER34_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER34_SUMMATION_RECEIVED_ID,                       /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER34_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER35_SUMMATION_DELIVERED_ID,                      /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER35_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER35_SUMMATION_RECEIVED_ID,                       /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER35_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER36_SUMMATION_DELIVERED_ID,                      /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER36_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER36_SUMMATION_RECEIVED_ID,                       /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER36_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER37_SUMMATION_DELIVERED_ID,                      /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER37_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER37_SUMMATION_RECEIVED_ID,                       /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER37_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER38_SUMMATION_DELIVERED_ID,                      /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER38_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER38_SUMMATION_RECEIVED_ID,                       /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER38_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER39_SUMMATION_DELIVERED_ID,                      /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER39_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER39_SUMMATION_RECEIVED_ID,                       /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER39_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER40_SUMMATION_DELIVERED_ID,                      /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER40_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER40_SUMMATION_RECEIVED_ID,                       /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER40_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER41_SUMMATION_DELIVERED_ID,                      /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER41_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER41_SUMMATION_RECEIVED_ID,                       /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER41_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER42_SUMMATION_DELIVERED_ID,                      /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER42_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER42_SUMMATION_RECEIVED_ID,                       /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER42_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER43_SUMMATION_DELIVERED_ID,                      /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER43_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER43_SUMMATION_RECEIVED_ID,                       /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER43_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER44_SUMMATION_DELIVERED_ID,                      /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER44_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER44_SUMMATION_RECEIVED_ID,                       /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER44_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER45_SUMMATION_DELIVERED_ID,                      /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER45_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER45_SUMMATION_RECEIVED_ID,                       /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER45_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER46_SUMMATION_DELIVERED_ID,                      /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER46_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER46_SUMMATION_RECEIVED_ID,                       /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER46_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER47_SUMMATION_DELIVERED_ID,                      /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER47_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER47_SUMMATION_RECEIVED_ID,                       /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER47_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER48_SUMMATION_DELIVERED_ID,                      /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER48_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER48_SUMMATION_RECEIVED_ID,                       /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER48_SUMMATION_RECEIVED_ID */

    /* 0x0160 to 0x01FB reserved */

    ESP_ZB_ZCL_ATTR_METERING_CPP1_SUMMATION_DELIVERED_ID =
        ESP_ZB_ZCL_ATTR_SET_WITH_ATTR_ID(ESP_ZB_ZCL_METERING_SET_TOU_INFORMATION, 0xFC), /*!< CPP1SummationDelivered attribute
                                                                                          *   represents the most recent summed value of Energy, Gas, or Water delivered to the premises (i.e.
                                                                                          *   delivered to the customer from the utility) while Critical Peak Price CPP1 was being applied
                                                                                          */
    /* 0x01FD reserved */

    ESP_ZB_ZCL_ATTR_METERING_CPP2_SUMMATION_DELIVERED_ID =
        ESP_ZB_ZCL_ATTR_SET_WITH_ATTR_ID(ESP_ZB_ZCL_METERING_SET_TOU_INFORMATION, 0xFE), /*!< CPP2SummationDelivered attribute
                                                                                          *   represents the most recent summed value of Energy, Gas, or Water delivered to the premises (i.e.
                                                                                          *   delivered to the customer from the utility) while Critical Peak Price CPP2 was being applied
                                                                                          */
    /* 0x01FF reserved */


    /* Meter Status Attribute Set */
    ESP_ZB_ZCL_ATTR_METERING_STATUS_ID =
        ESP_ZB_ZCL_ATTR_SET_WITH_ATTR_ID(ESP_ZB_ZCL_METERING_SET_METER_STATUS, 0x00), /*!< Status attribute provides indicators reflecting the current
                                                                                       *   error conditions found by the metering device. Status depends on the device type
                                                                                       */
    ESP_ZB_ZCL_ATTR_METERING_REMAINING_BATTERY_LIFE_ID,                               /*!< RemainingBatteryLife attribute represents the estimated remaining life of the battery in % of capacity.
                                                                                       *   A setting of 0xFF indicates this feature is disabled
                                                                                       */
    ESP_ZB_ZCL_ATTR_METERING_HOURS_IN_OPERATION_ID,                                   /*!< HoursInOperation attribute is a counter that increments once every hour during operation */
    ESP_ZB_ZCL_ATTR_METERING_HOURS_IN_FAULT_ID,                                       /*!< HoursInFault attribute is a counter that increments once every hour when the device is in operation
                                                                                       *   with a fault detected
                                                                                       */
    ESP_ZB_ZCL_ATTR_METERING_EXTENDED_STATUS_ID,                                      /*!< ExtendedStatus attribute reflects the state of items in a meter that the standard Status attribute
                                                                                       *   cannot show. The Extended Status BitMap is split into two groups of flags: general flags and metering
                                                                                       *   type specific flags
                                                                                       */
    ESP_ZB_ZCL_ATTR_METERING_REMAINING_BATTERY_LIFE_DAYS_ID,                          /*!< RemainingBatteryLifeInDays attribute represents the estimated remaining life of the battery in days
                                                                                       *   of capacity. The range is 0 - 0xFFFE, where 0xFFFF represents 'Invalid', 'Unused' and 'Disabled' */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_METER_ID_ID,                                     /*!< CurrentMeterID attribute is the current id for the Meter. This could be the current firmware
                                                                                       *   version supported on the meter
                                                                                       */
    ESP_ZB_ZCL_ATTR_METERING_AMBIENT_CONSUMPTION_INDICATOR_ID,                        /*!< AmbientConsumptionIndicator attribute is an 8-bit typedef enumeration which provides a simple indication
                                                                                       *   (Low/Medium/High) of the amount of a commodity being consumed within the premises
                                                                                       */
    /* 0x0208 to 0x02FF reserved */


    /* Formatting Attribute Set */
    ESP_ZB_ZCL_ATTR_METERING_UNIT_OF_MEASURE_ID =
        ESP_ZB_ZCL_ATTR_SET_WITH_ATTR_ID(ESP_ZB_ZCL_METERING_SET_FORMATTING, 0x00), /*!< UnitOfMeasure attribute provides a label for the Energy, Gas, or Water being measured by the metering device */
    ESP_ZB_ZCL_ATTR_METERING_MULTIPLIER_ID,                                         /*!< Multiplier attribute provides a value to be multiplied against a raw or uncompensated sensor
                                                                                     *   count of Energy, Gas, or Water being measured by the metering device
                                                                                     */
    ESP_ZB_ZCL_ATTR_METERING_DIVISOR_ID,                                            /*!< Divisor attribute provides a value to divide the results of applying the
                                                                                     *   "Multiplier Attribute" against a raw or uncompensated
                                                                                     *   sensor count of Energy, Gas, or Water being measured by the metering device
                                                                                     */
    ESP_ZB_ZCL_ATTR_METERING_SUMMATION_FORMATTING_ID,                               /*!< SummationFormatting attribute provides a method to properly decipher the number of digits
                                                                                     *   and the decimal location of the values found in the Summation Information Set of attributes
                                                                                     */
    ESP_ZB_ZCL_ATTR_METERING_DEMAND_FORMATTING_ID,                                  /*!< DemandFormatting attribute provides a method to properly decipher the number of digits and
                                                                                     *   the decimal location of the values found in the Demand-related attributes
                                                                                     */
    ESP_ZB_ZCL_ATTR_METERING_HISTORICAL_CONSUMPTION_FORMATTING_ID,                  /*!< HistoricalConsumptionFormatting attribute provides a method to properly decipher the number
                                                                                     *   of digits and the decimal location of the values found in the Historical Consumption Set of attributes
                                                                                     */
    ESP_ZB_ZCL_ATTR_METERING_METERING_DEVICE_TYPE_ID,                               /*!< MeteringDeviceType attribute provides a label for identifying the type of metering device
                                                                                     *   present (Energy, Gas, Water, Thermal, Heat, Cooling, and mirrored metering devices)
                                                                                     */
    ESP_ZB_ZCL_ATTR_METERING_SITE_ID_ID,                                            /*!< SiteID attribute is a text string, known in the UK as the MPAN number for electricity,
                                                                                     *   MPRN for gas and 'Stand Point' in South Africa
                                                                                     */
    ESP_ZB_ZCL_ATTR_METERING_METER_SERIAL_NUMBER_ID,                                /*!< MeterSerialNumber attribute is used to provide a unique identification of the metering device */
    ESP_ZB_ZCL_ATTR_METERING_ENERGY_CARRIER_UNIT_OF_MEASURE_ID,                     /*!< EnergyCarrierUnitOfMeasure attribute specifies the unit of measure that the EnergyCarrier is measured in
                                                                                     */
    ESP_ZB_ZCL_ATTR_METERING_ENERGY_CARRIER_SUMMATION_FORMATTING_ID,                /*!< EnergyCarrierSummationFormatting attribute provides a method to properly decipher the number
                                                                                     *   of digits and the decimal location of the values found in the Summation- related attributes
                                                                                     */
    ESP_ZB_ZCL_ATTR_METERING_ENERGY_CARRIER_DEMAND_FORMATTING_ID,                   /*!< EnergyCarrierDemandFormatting attribute provides a method to properly decipher the number
                                                                                     *   of digits and the decimal location of the values found in the Demand- related attributes
                                                                                     */
    ESP_ZB_ZCL_ATTR_METERING_TEMPERATURE_UNIT_OF_MEASURE_ID,                        /*!< TemperatureUnitOfMeasure attribute specifies the unit of measure that temperatures are measured in
                                                                                     */
    ESP_ZB_ZCL_ATTR_METERING_TEMPERATURE_FORMATTING_ID,                             /*!< TemperatureFormatting attribute provides a method to properly decipher the number of digits
                                                                                     *   and the decimal location of the values found in the Temperature-related attributes
                                                                                     */
    ESP_ZB_ZCL_ATTR_METERING_MODULE_SERIAL_NUMBER_ID,                               /*!< ModuleSerialNumber attribute represents the serial number (unique identifier) of the meter module */
    ESP_ZB_ZCL_ATTR_METERING_OPERATING_TARIFF_LABEL_DELIVERED_ID,                   /*!< OperatingTariffLabelDelivered attribute is the meter's version of the TariffLabel attribute
                                                                                     *   that is found within the Tariff Information attribute set of the Price Cluster
                                                                                     */
    ESP_ZB_ZCL_ATTR_METERING_OPERATING_TARIFF_LABEL_RECEIVED_ID,                    /*!< OperatingTariffLabelReceived attribute is the meter's version of the ReceivedTariffLabel
                                                                                     *   attribute that is found within the Tariff Information attribute set of the Price Cluster
                                                                                     */
    ESP_ZB_ZCL_ATTR_METERING_CUSTOMER_ID_NUMBER_ID,                                 /*!< CustomerIDNumber attribute provides a customer identification which may be used to confirm
                                                                                     *   the customer at the premises
                                                                                     */
    ESP_ZB_ZCL_ATTR_METERING_ALTERNATIVE_UNIT_OF_MEASURE_ID,                        /*!< AlternativeUnitOfMeasure attribute provides a base for the attributes in the Alternative
                                                                                     *   Historical Consumption attribute set
                                                                                     */
    ESP_ZB_ZCL_ATTR_METERING_ALTERNATIVE_DEMAND_FORMATTING_ID,                      /*!< AlternativeDemandFormatting attribute provides a method to properly decipher the number of
                                                                                     *   digits and the decimal location of the values found in the Alternative Demand-related attributes
                                                                                     */
    ESP_ZB_ZCL_ATTR_METERING_ALTERNATIVE_CONSUMPTION_FORMATTING_ID,                 /*!< AlternativeConsumptionFormatting attribute provides a method to properly decipher the
                                                                                     *   number of digits and the decimal location of the consumption values found in the Alternative
                                                                                     *   Historical Consumption Set of attributes
                                                                                     */
    /* 0x0315 to 0x03FF reserved */


    /* Historical Consumption Attribute Set */
    ESP_ZB_ZCL_ATTR_METERING_INSTANTANEOUS_DEMAND_ID =
        ESP_ZB_ZCL_ATTR_SET_WITH_ATTR_ID(ESP_ZB_ZCL_METERING_SET_HISTORICAL_CONSUMPTION, 0x00), /*!< InstantaneousDemand attribute
                                                                                                 *   represents the current Demand of Energy, Gas, or Water delivered or received at the premises
                                                                                                 */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_DAY_CONSUMPTION_DELIVERED_ID,                              /*!< CurrentDayConsumptionDelivered attribute represents the summed value of Energy,
                                                                                                 *   Gas, or Water delivered to the premises since the Historical Freeze Time (HFT)
                                                                                                 */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_DAY_CONSUMPTION_RECEIVED_ID,                               /*!< CurrentDayConsumptionReceived attribute represents the summed value of Energy,
                                                                                                 *   Gas, or Water received from the premises since the Historical Freeze Time (HFT)
                                                                                                 */
    ESP_ZB_ZCL_ATTR_METERING_PREVIOUS_DAY_CONSUMPTION_DELIVERED_ID,                             /*!< PreviousDayConsumptionDelivered attribute represents the summed value of Energy,
                                                                                                 *   Gas, or Water delivered to the premises within the previous 24 hour period starting
                                                                                                 *   at the Historical Freeze Time (HFT)
                                                                                                 */
    ESP_ZB_ZCL_ATTR_METERING_PREVIOUS_DAY_CONSUMPTION_RECEIVED_ID,                              /*!< PreviousDayConsumptionReceived attribute represents the summed value of Energy,
                                                                                                 *   Gas, or Water received from the premises within the previous 24 hour period starting
                                                                                                 *   at the Historical Freeze Time (HFT)
                                                                                                 */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_PARTIAL_PROFILE_INTERVAL_START_TIME_DELIVERED_ID,          /*!< CurrentPartialProfileIntervalStartTimeDelivered attribute represents the start
                                                                                                 *   time of the current Load Profile interval being accumulated for commodity delivered
                                                                                                 */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_PARTIAL_PROFILE_INTERVAL_START_TIME_RECEIVED_ID,           /*!< CurrentPartialProfileIntervalStartTimeReceived attribute represents the start
                                                                                                 *   time of the current Load Profile interval being accumulated for commodity received
                                                                                                 */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_PARTIAL_PROFILE_INTERVAL_VALUE_DELIVERED_ID,               /*!< CurrentPartialProfileIntervalValueDelivered attribute represents the value of
                                                                                                 *   the current Load Profile interval being accumulated for commodity delivered
                                                                                                 */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_PARTIAL_PROFILE_INTERVAL_VALUE_RECEIVED_ID,                /*!< CurrentPartialProfileIntervalValueReceived attribute represents the value of
                                                                                                 *   the current Load Profile interval being accumulated for commodity delivered
                                                                                                 */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_DAY_MAX_PRESSURE_ID,                                       /*!< CurrentDayMaxPressure attribute is the maximum pressure reported during a day
                                                                                                 *   from the water or gas meter
                                                                                                 */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_DAY_MIN_PRESSURE_ID,                                       /*!< CurrentDayMinPressure attribute is the minimum pressure reported during a day
                                                                                                 *   from the water or gas meter
                                                                                                 */
    ESP_ZB_ZCL_ATTR_METERING_PREVIOUS_DAY_MAX_PRESSURE_ID,                                      /*!< PreviousDayMaxPressure attribute is the maximum pressure reported during previous
                                                                                                 *   day from the water or gas meter
                                                                                                 */
    ESP_ZB_ZCL_ATTR_METERING_PREVIOUS_DAY_MIN_PRESSURE_ID,                                      /*!< PreviousDayMinPressure attribute is the minimum pressure reported during previous
                                                                                                 *   day from the water or gas meter
                                                                                                 */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_DAY_MAX_DEMAND_ID,                                         /*!< CurrentDayMaxDemand attribute represents the maximum demand or rate of delivered
                                                                                                 *   value of Energy, Gas, or Water being utilized at the premises
                                                                                                 */
    ESP_ZB_ZCL_ATTR_METERING_PREVIOUS_DAY_MAX_DEMAND_ID,                                        /*!< PreviousDayMaxDemand attribute represents the maximum demand or rate of delivered
                                                                                                 *   value of Energy, Gas, or Water being utilized at the premises
                                                                                                 */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_MONTH_MAX_DEMAND_ID,                                       /*!< CurrentMonthMaxDemand attribute is the maximum demand reported during a month
                                                                                                 *   from the meter. For electricity, heat and cooling meters this is the maximum power
                                                                                                 *   reported in a month
                                                                                                 */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_YEAR_MAX_DEMAND_ID,                                        /*!< CurrentYearMaxDemand attribute is the maximum demand reported during a year
                                                                                                 *   from the meter. For electricity, heat and cooling meters this is the maximum power
                                                                                                 *   reported in a year
                                                                                                 */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_DAY_MAX_ENERGY_CARRIER_DEMAND_ID,                          /*!< CurrentDayMaxEnergyCarrierDemand attribute is the maximum energy carrier demand
                                                                                                 *   reported during a day from the meter. For heat and cooling meters this is the maximum
                                                                                                 *   flow rate on the inlet reported in a day.
                                                                                                 *   At the end of a day the meter will transfer the CurrentDayMaxEnergyCarrierDemand
                                                                                                 *   into "PreviousDayMaxEnergyCarrierDemand"
                                                                                                 */
    ESP_ZB_ZCL_ATTR_METERING_PREVIOUS_DAY_MAX_ENERGY_CARRIER_DEMAND_ID,                         /*!< PreviousDayMaxEnergyCarrierDemand attribute is the maximum energy carrier demand
                                                                                                 *   reported during the previous day from the meter
                                                                                                 */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_MONTH_MAX_ENERGY_CARRIER_DEMAND_ID,                        /*!< CurrentMonthMaxEnergyCarrierDemand attribute is the maximum energy carrier demand
                                                                                                 *   reported during a month from the meter. For heat and cooling meters this is the
                                                                                                 *   maximum flow rate on the inlet reported in a month
                                                                                                 */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_MONTH_MIN_ENERGY_CARRIER_DEMAND_ID,                        /*!< CurrentMonthMinEnergyCarrierDemand attribute is the minimum energy carrier demand
                                                                                                 *   reported during a month from the meter. For heat and cooling meters this is the
                                                                                                 *   minimum flow rate on the inlet reported in a month
                                                                                                 */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_YEAR_MAX_ENERGY_CARRIER_DEMAND_ID,                         /*!< CurrentYearMaxEnergyCarrierDemand attribute is the maximum energy carrier demand
                                                                                                 *   reported during a year from the meter. For heat and cooling meters this is the maximum
                                                                                                 *   flow rate on the inlet reported in a year
                                                                                                 */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_YEAR_MIN_ENERGY_CARRIER_DEMAND_ID,                         /*!< CurrentYearMinEnergyCarrierDemand attribute is the minimum energy carrier demand
                                                                                                 *   reported during a year from the heat meter. For heat and cooling meters this is the
                                                                                                 *   minimum flow rate on the inlet reported in a year
                                                                                                 */
    /* 0x0417 to 0x041F reserved */

    ESP_ZB_ZCL_ATTR_METERING_PREVIOUS_DAY2_CONSUMPTION_DELIVERED_ID =
        ESP_ZB_ZCL_ATTR_SET_WITH_ATTR_ID(ESP_ZB_ZCL_METERING_SET_HISTORICAL_CONSUMPTION, 0x20), /*!< PreviousDayNConsumptionDelivered
                                                                                                 *   attribute represents the summed value of Energy, Gas, or Water delivered to the
                                                                                                 *   premises within the previous 24 hour period starting at the Historical Freeze Time (HFT)
                                                                                                 */
    ESP_ZB_ZCL_ATTR_METERING_PREVIOUS_DAY2_CONSUMPTION_RECEIVED_ID,                             /*!< PreviousDayNConsumptionReceived attribute represents the summed value of Energy,
                                                                                                 *   Gas, or Water received from the premises within the previous 24 hour period starting
                                                                                                 *   at the Historical Freeze Time (HFT)
                                                                                                 */
    ESP_ZB_ZCL_ATTR_METERING_PREVIOUS_DAY3_CONSUMPTION_DELIVERED_ID,                            /*!< ESP_ZB_ZCL_ATTR_METERING_PREVIOUS_DAY3_CONSUMPTION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_PREVIOUS_DAY3_CONSUMPTION_RECEIVED_ID,                             /*!< ESP_ZB_ZCL_ATTR_METERING_PREVIOUS_DAY3_CONSUMPTION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_PREVIOUS_DAY4_CONSUMPTION_DELIVERED_ID,                            /*!< ESP_ZB_ZCL_ATTR_METERING_PREVIOUS_DAY4_CONSUMPTION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_PREVIOUS_DAY4_CONSUMPTION_RECEIVED_ID,                             /*!< ESP_ZB_ZCL_ATTR_METERING_PREVIOUS_DAY4_CONSUMPTION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_PREVIOUS_DAY5_CONSUMPTION_DELIVERED_ID,                            /*!< ESP_ZB_ZCL_ATTR_METERING_PREVIOUS_DAY5_CONSUMPTION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_PREVIOUS_DAY5_CONSUMPTION_RECEIVED_ID,                             /*!< ESP_ZB_ZCL_ATTR_METERING_PREVIOUS_DAY5_CONSUMPTION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_PREVIOUS_DAY6_CONSUMPTION_DELIVERED_ID,                            /*!< ESP_ZB_ZCL_ATTR_METERING_PREVIOUS_DAY6_CONSUMPTION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_PREVIOUS_DAY6_CONSUMPTION_RECEIVED_ID,                             /*!< ESP_ZB_ZCL_ATTR_METERING_PREVIOUS_DAY6_CONSUMPTION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_PREVIOUS_DAY7_CONSUMPTION_DELIVERED_ID,                            /*!< ESP_ZB_ZCL_ATTR_METERING_PREVIOUS_DAY7_CONSUMPTION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_PREVIOUS_DAY7_CONSUMPTION_RECEIVED_ID,                             /*!< ESP_ZB_ZCL_ATTR_METERING_PREVIOUS_DAY7_CONSUMPTION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_PREVIOUS_DAY8_CONSUMPTION_DELIVERED_ID,                            /*!< ESP_ZB_ZCL_ATTR_METERING_PREVIOUS_DAY8_CONSUMPTION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_PREVIOUS_DAY8_CONSUMPTION_RECEIVED_ID,                             /*!< ESP_ZB_ZCL_ATTR_METERING_PREVIOUS_DAY8_CONSUMPTION_RECEIVED_ID */

    /* 0x042E to 0x042F reserved */

    ESP_ZB_ZCL_ATTR_METERING_CURRENT_WEEK_CONSUMPTION_DELIVERED_ID =
        ESP_ZB_ZCL_ATTR_SET_WITH_ATTR_ID(ESP_ZB_ZCL_METERING_SET_HISTORICAL_CONSUMPTION, 0x30), /*!< CurrentWeekConsumptionDelivered
                                                                                                 *   attribute represents the summed value of Energy, Gas, or Water delivered to the premises
                                                                                                 *   since the Historical Freeze Time (HFT) on Monday to the last HFT read
                                                                                                 */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_WEEK_CONSUMPTION_RECEIVED_ID,                              /*!< CurrentWeekConsumptionReceived attribute represents the summed value of Energy,
                                                                                                 *   Gas, or Water received from the premises since the Historical Freeze Time (HFT) on
                                                                                                 *   Monday to the last HFT read
                                                                                                 */
    ESP_ZB_ZCL_ATTR_METERING_PREVIOUS_WEEK_CONSUMPTION_DELIVERED_ID,                            /*!< PreviousWeekNConsumptionDelivered attribute represents the summed value of Energy,
                                                                                                 *   Gas, or Water delivered to the premises within the previous week period starting at
                                                                                                 *   the Historical Freeze Time (HFT) on the Monday to the Sunday
                                                                                                 */
    ESP_ZB_ZCL_ATTR_METERING_PREVIOUS_WEEK_CONSUMPTION_RECEIVED_ID,                             /*!< PreviousWeekNConsumptionReceived attribute represents the summed value of Energy,
                                                                                                 *   Gas, or Water received from the premises within the previous week period starting at
                                                                                                 *   the Historical Freeze Time (HFT) on the Monday to the Sunday
                                                                                                 */
    ESP_ZB_ZCL_ATTR_METERING_PREVIOUS_WEEK2_CONSUMPTION_DELIVERED_ID,                           /*!< ESP_ZB_ZCL_ATTR_METERING_PREVIOUS_WEEK_CONSUMPTION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_PREVIOUS_WEEK2_CONSUMPTION_RECEIVED_ID,                            /*!< ESP_ZB_ZCL_ATTR_METERING_PREVIOUS_WEEK_CONSUMPTION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_PREVIOUS_WEEK3_CONSUMPTION_DELIVERED_ID,                           /*!< ESP_ZB_ZCL_ATTR_METERING_PREVIOUS_WEEK_CONSUMPTION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_PREVIOUS_WEEK3_CONSUMPTION_RECEIVED_ID,                            /*!< ESP_ZB_ZCL_ATTR_METERING_PREVIOUS_WEEK_CONSUMPTION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_PREVIOUS_WEEK4_CONSUMPTION_DELIVERED_ID,                           /*!< ESP_ZB_ZCL_ATTR_METERING_PREVIOUS_WEEK_CONSUMPTION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_PREVIOUS_WEEK4_CONSUMPTION_RECEIVED_ID,                            /*!< ESP_ZB_ZCL_ATTR_METERING_PREVIOUS_WEEK_CONSUMPTION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_PREVIOUS_WEEK5_CONSUMPTION_DELIVERED_ID,                           /*!< ESP_ZB_ZCL_ATTR_METERING_PREVIOUS_WEEK_CONSUMPTION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_PREVIOUS_WEEK5_CONSUMPTION_RECEIVED_ID,                            /*!< ESP_ZB_ZCL_ATTR_METERING_PREVIOUS_WEEK_CONSUMPTION_RECEIVED_ID */

    /* 0x043C to 0x043F reserved */

    ESP_ZB_ZCL_ATTR_METERING_CURRENT_MONTH_CONSUMPTION_DELIVERED_ID =
        ESP_ZB_ZCL_ATTR_SET_WITH_ATTR_ID(ESP_ZB_ZCL_METERING_SET_HISTORICAL_CONSUMPTION, 0x40), /*!< CurrentMonthConsumptionDelivered
                                                                                                 *   attribute represents the summed value of Energy, Gas, or Water delivered to the premises
                                                                                                 *   since the Historical Freeze Time (HFT) on the 1 st of the month to the last HFT read
                                                                                                 */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_MONTH_CONSUMPTION_RECEIVED_ID,                             /*!< CurrentMonthConsumptionReceived attribute represents the summed value of Energy,
                                                                                                 *   Gas, or Water received from the premises since the Historical Freeze Time (HFT) on the
                                                                                                 *   1st of the month to the last HFT read
                                                                                                 */
    ESP_ZB_ZCL_ATTR_METERING_PREVIOUS_MONTH_CONSUMPTION_DELIVERED_ID,                           /*!< PreviousMonthNConsumptionDelivered attribute represents the summed value of
                                                                                                 *   Energy, Gas, or Water delivered to the premises within the previous Month period
                                                                                                 *   starting at the Historical Freeze Time (HFT) on the 1 st of the month to the last
                                                                                                 *   day of the month
                                                                                                 */
    ESP_ZB_ZCL_ATTR_METERING_PREVIOUS_MONTH_CONSUMPTION_RECEIVED_ID,                            /*!< PreviousMonthNConsumptionReceived attribute represents the summed value of
                                                                                                 *   Energy, Gas, or Water received from the premises within the previous month period
                                                                                                 *   starting at the Historical Freeze Time (HFT) on the 1 st of the month to the last
                                                                                                 *   day of the month
                                                                                                 */
    ESP_ZB_ZCL_ATTR_METERING_PREVIOUS_MONTH2_CONSUMPTION_DELIVERED_ID,                          /*!< ESP_ZB_ZCL_ATTR_METERING_PREVIOUS_MONTH_CONSUMPTION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_PREVIOUS_MONTH2_CONSUMPTION_RECEIVED_ID,                           /*!< ESP_ZB_ZCL_ATTR_METERING_PREVIOUS_MONTH_CONSUMPTION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_PREVIOUS_MONTH3_CONSUMPTION_DELIVERED_ID,                          /*!< ESP_ZB_ZCL_ATTR_METERING_PREVIOUS_MONTH_CONSUMPTION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_PREVIOUS_MONTH3_CONSUMPTION_RECEIVED_ID,                           /*!< ESP_ZB_ZCL_ATTR_METERING_PREVIOUS_MONTH_CONSUMPTION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_PREVIOUS_MONTH4_CONSUMPTION_DELIVERED_ID,                          /*!< ESP_ZB_ZCL_ATTR_METERING_PREVIOUS_MONTH_CONSUMPTION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_PREVIOUS_MONTH4_CONSUMPTION_RECEIVED_ID,                           /*!< ESP_ZB_ZCL_ATTR_METERING_PREVIOUS_MONTH_CONSUMPTION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_PREVIOUS_MONTH5_CONSUMPTION_DELIVERED_ID,                          /*!< ESP_ZB_ZCL_ATTR_METERING_PREVIOUS_MONTH_CONSUMPTION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_PREVIOUS_MONTH5_CONSUMPTION_RECEIVED_ID,                           /*!< ESP_ZB_ZCL_ATTR_METERING_PREVIOUS_MONTH_CONSUMPTION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_PREVIOUS_MONTH6_CONSUMPTION_DELIVERED_ID,                          /*!< ESP_ZB_ZCL_ATTR_METERING_PREVIOUS_MONTH_CONSUMPTION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_PREVIOUS_MONTH6_CONSUMPTION_RECEIVED_ID,                           /*!< ESP_ZB_ZCL_ATTR_METERING_PREVIOUS_MONTH_CONSUMPTION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_PREVIOUS_MONTH7_CONSUMPTION_DELIVERED_ID,                          /*!< ESP_ZB_ZCL_ATTR_METERING_PREVIOUS_MONTH_CONSUMPTION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_PREVIOUS_MONTH7_CONSUMPTION_RECEIVED_ID,                           /*!< ESP_ZB_ZCL_ATTR_METERING_PREVIOUS_MONTH_CONSUMPTION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_PREVIOUS_MONTH8_CONSUMPTION_DELIVERED_ID,                          /*!< ESP_ZB_ZCL_ATTR_METERING_PREVIOUS_MONTH_CONSUMPTION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_PREVIOUS_MONTH8_CONSUMPTION_RECEIVED_ID,                           /*!< ESP_ZB_ZCL_ATTR_METERING_PREVIOUS_MONTH_CONSUMPTION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_PREVIOUS_MONTH9_CONSUMPTION_DELIVERED_ID,                          /*!< ESP_ZB_ZCL_ATTR_METERING_PREVIOUS_MONTH_CONSUMPTION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_PREVIOUS_MONTH9_CONSUMPTION_RECEIVED_ID,                           /*!< ESP_ZB_ZCL_ATTR_METERING_PREVIOUS_MONTH_CONSUMPTION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_PREVIOUS_MONTH10_CONSUMPTION_DELIVERED_ID,                         /*!< ESP_ZB_ZCL_ATTR_METERING_PREVIOUS_MONTH_CONSUMPTION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_PREVIOUS_MONTH10_CONSUMPTION_RECEIVED_ID,                          /*!< ESP_ZB_ZCL_ATTR_METERING_PREVIOUS_MONTH_CONSUMPTION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_PREVIOUS_MONTH11_CONSUMPTION_DELIVERED_ID,                         /*!< ESP_ZB_ZCL_ATTR_METERING_PREVIOUS_MONTH_CONSUMPTION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_PREVIOUS_MONTH11_CONSUMPTION_RECEIVED_ID,                          /*!< ESP_ZB_ZCL_ATTR_METERING_PREVIOUS_MONTH_CONSUMPTION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_PREVIOUS_MONTH12_CONSUMPTION_DELIVERED_ID,                         /*!< ESP_ZB_ZCL_ATTR_METERING_PREVIOUS_MONTH_CONSUMPTION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_PREVIOUS_MONTH12_CONSUMPTION_RECEIVED_ID,                          /*!< ESP_ZB_ZCL_ATTR_METERING_PREVIOUS_MONTH_CONSUMPTION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_PREVIOUS_MONTH13_CONSUMPTION_DELIVERED_ID,                         /*!< ESP_ZB_ZCL_ATTR_METERING_PREVIOUS_MONTH_CONSUMPTION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_PREVIOUS_MONTH13_CONSUMPTION_RECEIVED_ID,                          /*!< ESP_ZB_ZCL_ATTR_METERING_PREVIOUS_MONTH_CONSUMPTION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_HISTORICAL_FREEZE_TIME_ID,                                         /*!< HistoricalFreezeTime attribute represents the time of day, in Local Time, when
                                                                                                 *   Historical Consumption attributes and/or Alternative Historical Consumption
                                                                                                 *   attributes are captured (hour and minutes)
                                                                                                 */
    /* 0x045D to 0x04FF reserved */


    /* Load Profile Configuration Attribute Set */
    ESP_ZB_ZCL_ATTR_METERING_MAX_NUMBER_OF_PERIODS_DELIVERED_ID =
        ESP_ZB_ZCL_ATTR_SET_WITH_ATTR_ID(ESP_ZB_ZCL_METERING_SET_LOAD_PROFILE_CONFIGURATION, 0x00), /*!< MaxNumberOfPeriodsDelivered
                                                                                                     *   attribute represents the maximum number of intervals the device is capable of returning
                                                                                                     *   in one Get Profile Response command. It is required MaxNumberOfPeriodsDelivered fit
                                                                                                     *   within the default Fragmentation ASDU size of 128 bytes, or an optionally agreed upon
                                                                                                     *   larger Fragmentation ASDU size supported by both devices
                                                                                                     */
    /* 0x0501 to 0x05FF reserved */


    /* Supply Limit Attribute Set */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_DEMAND_DELIVERED_ID =
        ESP_ZB_ZCL_ATTR_SET_WITH_ATTR_ID(ESP_ZB_ZCL_METERING_SET_SUPPLY_LIMIT, 0x00), /*!< CurrentDemandDelivered attribute represents
                                                                                       *   the current Demand of Energy, Gas, or Water delivered at the premises
                                                                                       */
    ESP_ZB_ZCL_ATTR_METERING_DEMAND_LIMIT_ID,                                         /*!< DemandLimit attribute reflects the current supply demand limit set in the meter. This value can be
                                                                                       *   compared to the ESP_ZB_ZCL_ATTR_METERING_CURRENT_DEMAND_DELIVERED_ID "CurrentDemandDelivered" attribute to
                                                                                       *   understand if limits are being approached or exceeded. A value of 0xFFFFFF indicates demand limiting
                                                                                       *   is switched off
                                                                                       */
    ESP_ZB_ZCL_ATTR_METERING_DEMAND_INTEGRATION_PERIOD_ID,                            /*!< DemandIntegrationPeriod attribute is the number of minutes over which the
                                                                                       *   ESP_ZB_ZCL_ATTR_METERING_CURRENT_DEMAND_DELIVERED_ID "CurrentDemandDelivered" attribute is calculated. Valid
                                                                                       *   range is 0x01 to 0xFF. 0x00 is a reserved value
                                                                                       */
    ESP_ZB_ZCL_ATTR_METERING_NUMBER_OF_DEMAND_SUBINTERVALS_ID,                        /*!< NumberOfDemandSubintervals attribute represents the number of subintervals used within the
                                                                                       *   ESP_ZB_ZCL_ATTR_METERING_DEMAND_INTEGRATION_PERIOD_ID "DemandIntegrationPeriod". The subinterval
                                                                                       *   duration (in minutes) is obtained by dividing the "DemandIntegrationPeriod" by the "NumberOfDemandSubintervals".
                                                                                       *   The "CurrentDemandDelivered" attribute is updated at the each of each subinterval.
                                                                                       *   Valid range is 0x01 to 0xFF. 0x00 is a reserved value
                                                                                       */
    ESP_ZB_ZCL_ATTR_METERING_DEMAND_LIMIT_ARM_DURATION_ID,                            /*!< DemandLimitArmDuration attribute defines the length of time, in seconds, that the supply shall be
                                                                                       *   disconnected if the ESP_ZB_ZCL_ATTR_METERING_DEMAND_LIMIT_ID "DemandLimit" attribute is enabled and
                                                                                       *   the limit is exceeded
                                                                                       */
    ESP_ZB_ZCL_ATTR_METERING_LOAD_LIMIT_SUPPLY_STATE_ID,                              /*!< LoadLimitSupplyState attribute indicates the required status of the supply once device is in a load
                                                                                       *   limit state
                                                                                       */
    ESP_ZB_ZCL_ATTR_METERING_LOAD_LIMIT_COUNTER_ID,                                   /*!< LoadLimitCounter attribute is used for counting the number of times that the demand limit has
                                                                                       *   exceeded the set threshold */
    ESP_ZB_ZCL_ATTR_METERING_SUPPLY_TAMPER_STATE_ID,                                  /*!< SupplyTamperState attribute indicates the required status of the supply following the detection of a
                                                                                       *   tamper event within the metering device
                                                                                       */
    ESP_ZB_ZCL_ATTR_METERING_SUPPLY_DEPLETION_STATE_ID,                               /*!< SupplyDepletionState attribute indicates the required status of the supply following detection of a
                                                                                       *   depleted battery within the metering device
                                                                                       */
    ESP_ZB_ZCL_ATTR_METERING_SUPPLY_UNCONTROLLED_FLOW_STATE_ID,                       /*!< SupplyUncontrolledFlowState attribute indicates the required status of the supply following detection
                                                                                       *   of an uncontrolled flow event within the metering device
                                                                                       */
    /* 0x060A to 0x06FF reserved */


    /* Block Information Attribute Set (Delivered) */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_NO_TIER_BLOCK1_SUMMATION_DELIVERED_ID =
        ESP_ZB_ZCL_ATTR_SET_WITH_ATTR_ID(ESP_ZB_ZCL_METERING_SET_BLOCK_INFORMATION_DELIVERED, 0x00), /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_NO_TIER_BLOCK1_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_NO_TIER_BLOCK2_SUMMATION_DELIVERED_ID,                         /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_NO_TIER_BLOCK2_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_NO_TIER_BLOCK3_SUMMATION_DELIVERED_ID,                         /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_NO_TIER_BLOCK3_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_NO_TIER_BLOCK4_SUMMATION_DELIVERED_ID,                         /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_NO_TIER_BLOCK4_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_NO_TIER_BLOCK5_SUMMATION_DELIVERED_ID,                         /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_NO_TIER_BLOCK5_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_NO_TIER_BLOCK6_SUMMATION_DELIVERED_ID,                         /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_NO_TIER_BLOCK6_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_NO_TIER_BLOCK7_SUMMATION_DELIVERED_ID,                         /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_NO_TIER_BLOCK7_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_NO_TIER_BLOCK8_SUMMATION_DELIVERED_ID,                         /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_NO_TIER_BLOCK8_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_NO_TIER_BLOCK9_SUMMATION_DELIVERED_ID,                         /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_NO_TIER_BLOCK9_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_NO_TIER_BLOCK10_SUMMATION_DELIVERED_ID,                        /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_NO_TIER_BLOCK10_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_NO_TIER_BLOCK11_SUMMATION_DELIVERED_ID,                        /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_NO_TIER_BLOCK11_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_NO_TIER_BLOCK12_SUMMATION_DELIVERED_ID,                        /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_NO_TIER_BLOCK12_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_NO_TIER_BLOCK13_SUMMATION_DELIVERED_ID,                        /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_NO_TIER_BLOCK13_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_NO_TIER_BLOCK14_SUMMATION_DELIVERED_ID,                        /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_NO_TIER_BLOCK14_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_NO_TIER_BLOCK15_SUMMATION_DELIVERED_ID,                        /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_NO_TIER_BLOCK15_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_NO_TIER_BLOCK16_SUMMATION_DELIVERED_ID,                        /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_NO_TIER_BLOCK16_SUMMATION_DELIVERED_ID */

    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER1_BLOCK1_SUMMATION_DELIVERED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER1_BLOCK1_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER1_BLOCK2_SUMMATION_DELIVERED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER1_BLOCK2_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER1_BLOCK3_SUMMATION_DELIVERED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER1_BLOCK3_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER1_BLOCK4_SUMMATION_DELIVERED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER1_BLOCK4_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER1_BLOCK5_SUMMATION_DELIVERED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER1_BLOCK5_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER1_BLOCK6_SUMMATION_DELIVERED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER1_BLOCK6_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER1_BLOCK7_SUMMATION_DELIVERED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER1_BLOCK7_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER1_BLOCK8_SUMMATION_DELIVERED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER1_BLOCK8_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER1_BLOCK9_SUMMATION_DELIVERED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER1_BLOCK9_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER1_BLOCK10_SUMMATION_DELIVERED_ID, /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER1_BLOCK10_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER1_BLOCK11_SUMMATION_DELIVERED_ID, /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER1_BLOCK11_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER1_BLOCK12_SUMMATION_DELIVERED_ID, /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER1_BLOCK12_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER1_BLOCK13_SUMMATION_DELIVERED_ID, /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER1_BLOCK13_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER1_BLOCK14_SUMMATION_DELIVERED_ID, /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER1_BLOCK14_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER1_BLOCK15_SUMMATION_DELIVERED_ID, /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER1_BLOCK15_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER1_BLOCK16_SUMMATION_DELIVERED_ID, /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER1_BLOCK16_SUMMATION_DELIVERED_ID */

    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER2_BLOCK1_SUMMATION_DELIVERED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER2_BLOCK1_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER2_BLOCK2_SUMMATION_DELIVERED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER2_BLOCK2_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER2_BLOCK3_SUMMATION_DELIVERED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER2_BLOCK3_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER2_BLOCK4_SUMMATION_DELIVERED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER2_BLOCK4_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER2_BLOCK5_SUMMATION_DELIVERED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER2_BLOCK5_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER2_BLOCK6_SUMMATION_DELIVERED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER2_BLOCK6_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER2_BLOCK7_SUMMATION_DELIVERED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER2_BLOCK7_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER2_BLOCK8_SUMMATION_DELIVERED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER2_BLOCK8_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER2_BLOCK9_SUMMATION_DELIVERED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER2_BLOCK9_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER2_BLOCK10_SUMMATION_DELIVERED_ID, /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER2_BLOCK10_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER2_BLOCK11_SUMMATION_DELIVERED_ID, /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER2_BLOCK11_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER2_BLOCK12_SUMMATION_DELIVERED_ID, /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER2_BLOCK12_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER2_BLOCK13_SUMMATION_DELIVERED_ID, /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER2_BLOCK13_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER2_BLOCK14_SUMMATION_DELIVERED_ID, /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER2_BLOCK14_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER2_BLOCK15_SUMMATION_DELIVERED_ID, /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER2_BLOCK15_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER2_BLOCK16_SUMMATION_DELIVERED_ID, /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER2_BLOCK16_SUMMATION_DELIVERED_ID */

    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER3_BLOCK1_SUMMATION_DELIVERED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER3_BLOCK1_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER3_BLOCK2_SUMMATION_DELIVERED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER3_BLOCK2_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER3_BLOCK3_SUMMATION_DELIVERED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER3_BLOCK3_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER3_BLOCK4_SUMMATION_DELIVERED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER3_BLOCK4_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER3_BLOCK5_SUMMATION_DELIVERED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER3_BLOCK5_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER3_BLOCK6_SUMMATION_DELIVERED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER3_BLOCK6_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER3_BLOCK7_SUMMATION_DELIVERED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER3_BLOCK7_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER3_BLOCK8_SUMMATION_DELIVERED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER3_BLOCK8_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER3_BLOCK9_SUMMATION_DELIVERED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER3_BLOCK9_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER3_BLOCK10_SUMMATION_DELIVERED_ID, /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER3_BLOCK10_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER3_BLOCK11_SUMMATION_DELIVERED_ID, /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER3_BLOCK11_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER3_BLOCK12_SUMMATION_DELIVERED_ID, /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER3_BLOCK12_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER3_BLOCK13_SUMMATION_DELIVERED_ID, /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER3_BLOCK13_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER3_BLOCK14_SUMMATION_DELIVERED_ID, /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER3_BLOCK14_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER3_BLOCK15_SUMMATION_DELIVERED_ID, /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER3_BLOCK15_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER3_BLOCK16_SUMMATION_DELIVERED_ID, /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER3_BLOCK16_SUMMATION_DELIVERED_ID */

    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER4_BLOCK1_SUMMATION_DELIVERED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER4_BLOCK1_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER4_BLOCK2_SUMMATION_DELIVERED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER4_BLOCK2_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER4_BLOCK3_SUMMATION_DELIVERED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER4_BLOCK3_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER4_BLOCK4_SUMMATION_DELIVERED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER4_BLOCK4_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER4_BLOCK5_SUMMATION_DELIVERED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER4_BLOCK5_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER4_BLOCK6_SUMMATION_DELIVERED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER4_BLOCK6_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER4_BLOCK7_SUMMATION_DELIVERED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER4_BLOCK7_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER4_BLOCK8_SUMMATION_DELIVERED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER4_BLOCK8_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER4_BLOCK9_SUMMATION_DELIVERED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER4_BLOCK9_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER4_BLOCK10_SUMMATION_DELIVERED_ID, /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER4_BLOCK10_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER4_BLOCK11_SUMMATION_DELIVERED_ID, /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER4_BLOCK11_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER4_BLOCK12_SUMMATION_DELIVERED_ID, /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER4_BLOCK12_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER4_BLOCK13_SUMMATION_DELIVERED_ID, /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER4_BLOCK13_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER4_BLOCK14_SUMMATION_DELIVERED_ID, /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER4_BLOCK14_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER4_BLOCK15_SUMMATION_DELIVERED_ID, /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER4_BLOCK15_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER4_BLOCK16_SUMMATION_DELIVERED_ID, /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER4_BLOCK16_SUMMATION_DELIVERED_ID */

    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER5_BLOCK1_SUMMATION_DELIVERED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER5_BLOCK1_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER5_BLOCK2_SUMMATION_DELIVERED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER5_BLOCK2_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER5_BLOCK3_SUMMATION_DELIVERED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER5_BLOCK3_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER5_BLOCK4_SUMMATION_DELIVERED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER5_BLOCK4_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER5_BLOCK5_SUMMATION_DELIVERED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER5_BLOCK5_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER5_BLOCK6_SUMMATION_DELIVERED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER5_BLOCK6_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER5_BLOCK7_SUMMATION_DELIVERED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER5_BLOCK7_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER5_BLOCK8_SUMMATION_DELIVERED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER5_BLOCK8_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER5_BLOCK9_SUMMATION_DELIVERED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER5_BLOCK9_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER5_BLOCK10_SUMMATION_DELIVERED_ID, /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER5_BLOCK10_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER5_BLOCK11_SUMMATION_DELIVERED_ID, /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER5_BLOCK11_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER5_BLOCK12_SUMMATION_DELIVERED_ID, /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER5_BLOCK12_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER5_BLOCK13_SUMMATION_DELIVERED_ID, /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER5_BLOCK13_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER5_BLOCK14_SUMMATION_DELIVERED_ID, /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER5_BLOCK14_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER5_BLOCK15_SUMMATION_DELIVERED_ID, /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER5_BLOCK15_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER5_BLOCK16_SUMMATION_DELIVERED_ID, /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER5_BLOCK16_SUMMATION_DELIVERED_ID */

    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER6_BLOCK1_SUMMATION_DELIVERED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER6_BLOCK1_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER6_BLOCK2_SUMMATION_DELIVERED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER6_BLOCK2_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER6_BLOCK3_SUMMATION_DELIVERED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER6_BLOCK3_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER6_BLOCK4_SUMMATION_DELIVERED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER6_BLOCK4_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER6_BLOCK5_SUMMATION_DELIVERED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER6_BLOCK5_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER6_BLOCK6_SUMMATION_DELIVERED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER6_BLOCK6_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER6_BLOCK7_SUMMATION_DELIVERED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER6_BLOCK7_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER6_BLOCK8_SUMMATION_DELIVERED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER6_BLOCK8_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER6_BLOCK9_SUMMATION_DELIVERED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER6_BLOCK9_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER6_BLOCK10_SUMMATION_DELIVERED_ID, /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER6_BLOCK10_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER6_BLOCK11_SUMMATION_DELIVERED_ID, /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER6_BLOCK11_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER6_BLOCK12_SUMMATION_DELIVERED_ID, /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER6_BLOCK12_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER6_BLOCK13_SUMMATION_DELIVERED_ID, /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER6_BLOCK13_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER6_BLOCK14_SUMMATION_DELIVERED_ID, /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER6_BLOCK14_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER6_BLOCK15_SUMMATION_DELIVERED_ID, /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER6_BLOCK15_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER6_BLOCK16_SUMMATION_DELIVERED_ID, /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER6_BLOCK16_SUMMATION_DELIVERED_ID */

    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER7_BLOCK1_SUMMATION_DELIVERED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER7_BLOCK1_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER7_BLOCK2_SUMMATION_DELIVERED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER7_BLOCK2_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER7_BLOCK3_SUMMATION_DELIVERED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER7_BLOCK3_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER7_BLOCK4_SUMMATION_DELIVERED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER7_BLOCK4_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER7_BLOCK5_SUMMATION_DELIVERED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER7_BLOCK5_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER7_BLOCK6_SUMMATION_DELIVERED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER7_BLOCK6_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER7_BLOCK7_SUMMATION_DELIVERED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER7_BLOCK7_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER7_BLOCK8_SUMMATION_DELIVERED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER7_BLOCK8_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER7_BLOCK9_SUMMATION_DELIVERED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER7_BLOCK9_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER7_BLOCK10_SUMMATION_DELIVERED_ID, /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER7_BLOCK10_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER7_BLOCK11_SUMMATION_DELIVERED_ID, /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER7_BLOCK11_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER7_BLOCK12_SUMMATION_DELIVERED_ID, /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER7_BLOCK12_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER7_BLOCK13_SUMMATION_DELIVERED_ID, /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER7_BLOCK13_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER7_BLOCK14_SUMMATION_DELIVERED_ID, /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER7_BLOCK14_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER7_BLOCK15_SUMMATION_DELIVERED_ID, /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER7_BLOCK15_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER7_BLOCK16_SUMMATION_DELIVERED_ID, /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER7_BLOCK16_SUMMATION_DELIVERED_ID */

    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER8_BLOCK1_SUMMATION_DELIVERED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER8_BLOCK1_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER8_BLOCK2_SUMMATION_DELIVERED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER8_BLOCK2_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER8_BLOCK3_SUMMATION_DELIVERED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER8_BLOCK3_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER8_BLOCK4_SUMMATION_DELIVERED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER8_BLOCK4_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER8_BLOCK5_SUMMATION_DELIVERED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER8_BLOCK5_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER8_BLOCK6_SUMMATION_DELIVERED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER8_BLOCK6_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER8_BLOCK7_SUMMATION_DELIVERED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER8_BLOCK7_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER8_BLOCK8_SUMMATION_DELIVERED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER8_BLOCK8_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER8_BLOCK9_SUMMATION_DELIVERED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER8_BLOCK9_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER8_BLOCK10_SUMMATION_DELIVERED_ID, /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER8_BLOCK10_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER8_BLOCK11_SUMMATION_DELIVERED_ID, /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER8_BLOCK11_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER8_BLOCK12_SUMMATION_DELIVERED_ID, /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER8_BLOCK12_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER8_BLOCK13_SUMMATION_DELIVERED_ID, /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER8_BLOCK13_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER8_BLOCK14_SUMMATION_DELIVERED_ID, /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER8_BLOCK14_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER8_BLOCK15_SUMMATION_DELIVERED_ID, /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER8_BLOCK15_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER8_BLOCK16_SUMMATION_DELIVERED_ID, /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER8_BLOCK16_SUMMATION_DELIVERED_ID */

    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER9_BLOCK1_SUMMATION_DELIVERED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER9_BLOCK1_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER9_BLOCK2_SUMMATION_DELIVERED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER9_BLOCK2_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER9_BLOCK3_SUMMATION_DELIVERED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER9_BLOCK3_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER9_BLOCK4_SUMMATION_DELIVERED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER9_BLOCK4_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER9_BLOCK5_SUMMATION_DELIVERED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER9_BLOCK5_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER9_BLOCK6_SUMMATION_DELIVERED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER9_BLOCK6_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER9_BLOCK7_SUMMATION_DELIVERED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER9_BLOCK7_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER9_BLOCK8_SUMMATION_DELIVERED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER9_BLOCK8_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER9_BLOCK9_SUMMATION_DELIVERED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER9_BLOCK9_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER9_BLOCK10_SUMMATION_DELIVERED_ID, /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER9_BLOCK10_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER9_BLOCK11_SUMMATION_DELIVERED_ID, /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER9_BLOCK11_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER9_BLOCK12_SUMMATION_DELIVERED_ID, /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER9_BLOCK12_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER9_BLOCK13_SUMMATION_DELIVERED_ID, /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER9_BLOCK13_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER9_BLOCK14_SUMMATION_DELIVERED_ID, /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER9_BLOCK14_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER9_BLOCK15_SUMMATION_DELIVERED_ID, /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER9_BLOCK15_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER9_BLOCK16_SUMMATION_DELIVERED_ID, /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER9_BLOCK16_SUMMATION_DELIVERED_ID */

    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER10_BLOCK1_SUMMATION_DELIVERED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER10_BLOCK1_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER10_BLOCK2_SUMMATION_DELIVERED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER10_BLOCK2_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER10_BLOCK3_SUMMATION_DELIVERED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER10_BLOCK3_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER10_BLOCK4_SUMMATION_DELIVERED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER10_BLOCK4_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER10_BLOCK5_SUMMATION_DELIVERED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER10_BLOCK5_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER10_BLOCK6_SUMMATION_DELIVERED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER10_BLOCK6_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER10_BLOCK7_SUMMATION_DELIVERED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER10_BLOCK7_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER10_BLOCK8_SUMMATION_DELIVERED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER10_BLOCK8_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER10_BLOCK9_SUMMATION_DELIVERED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER10_BLOCK9_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER10_BLOCK10_SUMMATION_DELIVERED_ID, /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER10_BLOCK10_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER10_BLOCK11_SUMMATION_DELIVERED_ID, /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER10_BLOCK11_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER10_BLOCK12_SUMMATION_DELIVERED_ID, /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER10_BLOCK12_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER10_BLOCK13_SUMMATION_DELIVERED_ID, /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER10_BLOCK13_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER10_BLOCK14_SUMMATION_DELIVERED_ID, /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER10_BLOCK14_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER10_BLOCK15_SUMMATION_DELIVERED_ID, /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER10_BLOCK15_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER10_BLOCK16_SUMMATION_DELIVERED_ID, /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER10_BLOCK16_SUMMATION_DELIVERED_ID */

    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER11_BLOCK1_SUMMATION_DELIVERED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER11_BLOCK1_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER11_BLOCK2_SUMMATION_DELIVERED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER11_BLOCK2_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER11_BLOCK3_SUMMATION_DELIVERED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER11_BLOCK3_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER11_BLOCK4_SUMMATION_DELIVERED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER11_BLOCK4_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER11_BLOCK5_SUMMATION_DELIVERED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER11_BLOCK5_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER11_BLOCK6_SUMMATION_DELIVERED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER11_BLOCK6_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER11_BLOCK7_SUMMATION_DELIVERED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER11_BLOCK7_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER11_BLOCK8_SUMMATION_DELIVERED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER11_BLOCK8_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER11_BLOCK9_SUMMATION_DELIVERED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER11_BLOCK9_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER11_BLOCK10_SUMMATION_DELIVERED_ID, /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER11_BLOCK10_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER11_BLOCK11_SUMMATION_DELIVERED_ID, /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER11_BLOCK11_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER11_BLOCK12_SUMMATION_DELIVERED_ID, /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER11_BLOCK12_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER11_BLOCK13_SUMMATION_DELIVERED_ID, /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER11_BLOCK13_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER11_BLOCK14_SUMMATION_DELIVERED_ID, /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER11_BLOCK14_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER11_BLOCK15_SUMMATION_DELIVERED_ID, /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER11_BLOCK15_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER11_BLOCK16_SUMMATION_DELIVERED_ID, /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER11_BLOCK16_SUMMATION_DELIVERED_ID */

    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER12_BLOCK1_SUMMATION_DELIVERED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER12_BLOCK1_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER12_BLOCK2_SUMMATION_DELIVERED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER12_BLOCK2_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER12_BLOCK3_SUMMATION_DELIVERED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER12_BLOCK3_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER12_BLOCK4_SUMMATION_DELIVERED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER12_BLOCK4_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER12_BLOCK5_SUMMATION_DELIVERED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER12_BLOCK5_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER12_BLOCK6_SUMMATION_DELIVERED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER12_BLOCK6_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER12_BLOCK7_SUMMATION_DELIVERED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER12_BLOCK7_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER12_BLOCK8_SUMMATION_DELIVERED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER12_BLOCK8_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER12_BLOCK9_SUMMATION_DELIVERED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER12_BLOCK9_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER12_BLOCK10_SUMMATION_DELIVERED_ID, /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER12_BLOCK10_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER12_BLOCK11_SUMMATION_DELIVERED_ID, /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER12_BLOCK11_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER12_BLOCK12_SUMMATION_DELIVERED_ID, /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER12_BLOCK12_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER12_BLOCK13_SUMMATION_DELIVERED_ID, /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER12_BLOCK13_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER12_BLOCK14_SUMMATION_DELIVERED_ID, /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER12_BLOCK14_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER12_BLOCK15_SUMMATION_DELIVERED_ID, /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER12_BLOCK15_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER12_BLOCK16_SUMMATION_DELIVERED_ID, /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER12_BLOCK16_SUMMATION_DELIVERED_ID */

    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER13_BLOCK1_SUMMATION_DELIVERED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER13_BLOCK1_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER13_BLOCK2_SUMMATION_DELIVERED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER13_BLOCK2_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER13_BLOCK3_SUMMATION_DELIVERED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER13_BLOCK3_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER13_BLOCK4_SUMMATION_DELIVERED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER13_BLOCK4_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER13_BLOCK5_SUMMATION_DELIVERED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER13_BLOCK5_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER13_BLOCK6_SUMMATION_DELIVERED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER13_BLOCK6_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER13_BLOCK7_SUMMATION_DELIVERED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER13_BLOCK7_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER13_BLOCK8_SUMMATION_DELIVERED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER13_BLOCK8_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER13_BLOCK9_SUMMATION_DELIVERED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER13_BLOCK9_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER13_BLOCK10_SUMMATION_DELIVERED_ID, /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER13_BLOCK10_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER13_BLOCK11_SUMMATION_DELIVERED_ID, /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER13_BLOCK11_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER13_BLOCK12_SUMMATION_DELIVERED_ID, /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER13_BLOCK12_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER13_BLOCK13_SUMMATION_DELIVERED_ID, /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER13_BLOCK13_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER13_BLOCK14_SUMMATION_DELIVERED_ID, /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER13_BLOCK14_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER13_BLOCK15_SUMMATION_DELIVERED_ID, /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER13_BLOCK15_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER13_BLOCK16_SUMMATION_DELIVERED_ID, /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER13_BLOCK16_SUMMATION_DELIVERED_ID */

    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER14_BLOCK1_SUMMATION_DELIVERED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER14_BLOCK1_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER14_BLOCK2_SUMMATION_DELIVERED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER14_BLOCK2_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER14_BLOCK3_SUMMATION_DELIVERED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER14_BLOCK3_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER14_BLOCK4_SUMMATION_DELIVERED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER14_BLOCK4_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER14_BLOCK5_SUMMATION_DELIVERED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER14_BLOCK5_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER14_BLOCK6_SUMMATION_DELIVERED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER14_BLOCK6_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER14_BLOCK7_SUMMATION_DELIVERED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER14_BLOCK7_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER14_BLOCK8_SUMMATION_DELIVERED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER14_BLOCK8_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER14_BLOCK9_SUMMATION_DELIVERED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER14_BLOCK9_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER14_BLOCK10_SUMMATION_DELIVERED_ID, /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER14_BLOCK10_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER14_BLOCK11_SUMMATION_DELIVERED_ID, /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER14_BLOCK11_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER14_BLOCK12_SUMMATION_DELIVERED_ID, /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER14_BLOCK12_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER14_BLOCK13_SUMMATION_DELIVERED_ID, /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER14_BLOCK13_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER14_BLOCK14_SUMMATION_DELIVERED_ID, /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER14_BLOCK14_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER14_BLOCK15_SUMMATION_DELIVERED_ID, /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER14_BLOCK15_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER14_BLOCK16_SUMMATION_DELIVERED_ID, /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER14_BLOCK16_SUMMATION_DELIVERED_ID */

    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER15_BLOCK1_SUMMATION_DELIVERED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER15_BLOCK1_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER15_BLOCK2_SUMMATION_DELIVERED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER15_BLOCK2_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER15_BLOCK3_SUMMATION_DELIVERED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER15_BLOCK3_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER15_BLOCK4_SUMMATION_DELIVERED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER15_BLOCK4_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER15_BLOCK5_SUMMATION_DELIVERED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER15_BLOCK5_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER15_BLOCK6_SUMMATION_DELIVERED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER15_BLOCK6_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER15_BLOCK7_SUMMATION_DELIVERED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER15_BLOCK7_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER15_BLOCK8_SUMMATION_DELIVERED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER15_BLOCK8_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER15_BLOCK9_SUMMATION_DELIVERED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER15_BLOCK9_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER15_BLOCK10_SUMMATION_DELIVERED_ID, /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER15_BLOCK10_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER15_BLOCK11_SUMMATION_DELIVERED_ID, /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER15_BLOCK11_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER15_BLOCK12_SUMMATION_DELIVERED_ID, /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER15_BLOCK12_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER15_BLOCK13_SUMMATION_DELIVERED_ID, /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER15_BLOCK13_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER15_BLOCK14_SUMMATION_DELIVERED_ID, /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER15_BLOCK14_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER15_BLOCK15_SUMMATION_DELIVERED_ID, /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER15_BLOCK15_SUMMATION_DELIVERED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER15_BLOCK16_SUMMATION_DELIVERED_ID, /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER15_BLOCK16_SUMMATION_DELIVERED_ID */


    /* Alarms Set */
    ESP_ZB_ZCL_ATTR_METERING_GENERIC_ALARM_MASK_ID = ESP_ZB_ZCL_ATTR_SET_WITH_ATTR_ID(ESP_ZB_ZCL_METERING_SET_ALARMS, 0x00), /*!< The AlarmMask attributes of the Alarm Attribute Set
                                                                                                                              * specify whether each of the alarms listed in the
                                                                                                                              * corresponding alarm group is enabled. When the bit number
                                                                                                                              * corresponding to the alarm number (minus the group offset) is set to 1,
                                                                                                                              * the alarm is enabled, else it is disabled. Bits not corresponding
                                                                                                                              * to a code in the respective table are reserved.
                                                                                                                              */
    ESP_ZB_ZCL_ATTR_METERING_ELECTRICITY_ALARM_MASK_ID,                                                                      /*!<  ESP_ZB_ZCL_ATTR_METERING_GENERIC_ALARM_MASK_ID */
    ESP_ZB_ZCL_ATTR_METERING_GENERIC_FLOW_PRESSURE_ALARM_MASK_ID,                                                            /*!<  ESP_ZB_ZCL_ATTR_METERING_GENERIC_ALARM_MASK_ID */
    ESP_ZB_ZCL_ATTR_METERING_WATER_SPECIFIC_ALARM_MASK_ID,                                                                   /*!<  ESP_ZB_ZCL_ATTR_METERING_GENERIC_ALARM_MASK_ID */
    ESP_ZB_ZCL_ATTR_METERING_HEAT_AND_COOLING_SPECIFIC_ALARM_MASK_ID,                                                        /*!<  ESP_ZB_ZCL_ATTR_METERING_GENERIC_ALARM_MASK_ID */
    ESP_ZB_ZCL_ATTR_METERING_GAS_SPECIFIC_ALARM_MASK_ID,                                                                     /*!<  ESP_ZB_ZCL_ATTR_METERING_GENERIC_ALARM_MASK_ID */
    ESP_ZB_ZCL_ATTR_METERING_EXTENDED_GENERIC_ALARM_MASK_ID,                                                                 /*!<  ESP_ZB_ZCL_ATTR_METERING_GENERIC_ALARM_MASK_ID */
    ESP_ZB_ZCL_ATTR_METERING_MANUFACTURER_ALARM_MASK_ID,                                                                     /*!<  ESP_ZB_ZCL_ATTR_METERING_GENERIC_ALARM_MASK_ID */

    /* 0x0808 to 0x08FF reserved */


    /* Block Information Attribute Set (Received) */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_NO_TIER_BLOCK1_SUMMATION_RECEIVED_ID =
        ESP_ZB_ZCL_ATTR_SET_WITH_ATTR_ID(ESP_ZB_ZCL_METERING_SET_BLOCK_INFORMATION_RECEIVED, 0x00), /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_NO_TIER_BLOCK1_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_NO_TIER_BLOCK2_SUMMATION_RECEIVED_ID,                         /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_NO_TIER_BLOCK2_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_NO_TIER_BLOCK3_SUMMATION_RECEIVED_ID,                         /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_NO_TIER_BLOCK3_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_NO_TIER_BLOCK4_SUMMATION_RECEIVED_ID,                         /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_NO_TIER_BLOCK4_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_NO_TIER_BLOCK5_SUMMATION_RECEIVED_ID,                         /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_NO_TIER_BLOCK5_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_NO_TIER_BLOCK6_SUMMATION_RECEIVED_ID,                         /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_NO_TIER_BLOCK6_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_NO_TIER_BLOCK7_SUMMATION_RECEIVED_ID,                         /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_NO_TIER_BLOCK7_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_NO_TIER_BLOCK8_SUMMATION_RECEIVED_ID,                         /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_NO_TIER_BLOCK8_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_NO_TIER_BLOCK9_SUMMATION_RECEIVED_ID,                         /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_NO_TIER_BLOCK9_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_NO_TIER_BLOCK10_SUMMATION_RECEIVED_ID,                        /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_NO_TIER_BLOCK10_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_NO_TIER_BLOCK11_SUMMATION_RECEIVED_ID,                        /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_NO_TIER_BLOCK11_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_NO_TIER_BLOCK12_SUMMATION_RECEIVED_ID,                        /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_NO_TIER_BLOCK12_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_NO_TIER_BLOCK13_SUMMATION_RECEIVED_ID,                        /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_NO_TIER_BLOCK13_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_NO_TIER_BLOCK14_SUMMATION_RECEIVED_ID,                        /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_NO_TIER_BLOCK14_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_NO_TIER_BLOCK15_SUMMATION_RECEIVED_ID,                        /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_NO_TIER_BLOCK15_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_NO_TIER_BLOCK16_SUMMATION_RECEIVED_ID,                        /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_NO_TIER_BLOCK16_SUMMATION_RECEIVED_ID */

    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER1_BLOCK1_SUMMATION_RECEIVED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER1_BLOCK1_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER1_BLOCK2_SUMMATION_RECEIVED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER1_BLOCK2_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER1_BLOCK3_SUMMATION_RECEIVED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER1_BLOCK3_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER1_BLOCK4_SUMMATION_RECEIVED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER1_BLOCK4_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER1_BLOCK5_SUMMATION_RECEIVED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER1_BLOCK5_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER1_BLOCK6_SUMMATION_RECEIVED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER1_BLOCK6_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER1_BLOCK7_SUMMATION_RECEIVED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER1_BLOCK7_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER1_BLOCK8_SUMMATION_RECEIVED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER1_BLOCK8_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER1_BLOCK9_SUMMATION_RECEIVED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER1_BLOCK9_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER1_BLOCK10_SUMMATION_RECEIVED_ID, /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER1_BLOCK10_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER1_BLOCK11_SUMMATION_RECEIVED_ID, /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER1_BLOCK11_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER1_BLOCK12_SUMMATION_RECEIVED_ID, /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER1_BLOCK12_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER1_BLOCK13_SUMMATION_RECEIVED_ID, /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER1_BLOCK13_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER1_BLOCK14_SUMMATION_RECEIVED_ID, /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER1_BLOCK14_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER1_BLOCK15_SUMMATION_RECEIVED_ID, /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER1_BLOCK15_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER1_BLOCK16_SUMMATION_RECEIVED_ID, /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER1_BLOCK16_SUMMATION_RECEIVED_ID */

    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER2_BLOCK1_SUMMATION_RECEIVED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER2_BLOCK1_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER2_BLOCK2_SUMMATION_RECEIVED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER2_BLOCK2_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER2_BLOCK3_SUMMATION_RECEIVED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER2_BLOCK3_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER2_BLOCK4_SUMMATION_RECEIVED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER2_BLOCK4_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER2_BLOCK5_SUMMATION_RECEIVED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER2_BLOCK5_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER2_BLOCK6_SUMMATION_RECEIVED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER2_BLOCK6_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER2_BLOCK7_SUMMATION_RECEIVED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER2_BLOCK7_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER2_BLOCK8_SUMMATION_RECEIVED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER2_BLOCK8_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER2_BLOCK9_SUMMATION_RECEIVED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER2_BLOCK9_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER2_BLOCK10_SUMMATION_RECEIVED_ID, /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER2_BLOCK10_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER2_BLOCK11_SUMMATION_RECEIVED_ID, /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER2_BLOCK11_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER2_BLOCK12_SUMMATION_RECEIVED_ID, /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER2_BLOCK12_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER2_BLOCK13_SUMMATION_RECEIVED_ID, /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER2_BLOCK13_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER2_BLOCK14_SUMMATION_RECEIVED_ID, /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER2_BLOCK14_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER2_BLOCK15_SUMMATION_RECEIVED_ID, /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER2_BLOCK15_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER2_BLOCK16_SUMMATION_RECEIVED_ID, /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER2_BLOCK16_SUMMATION_RECEIVED_ID */

    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER3_BLOCK1_SUMMATION_RECEIVED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER3_BLOCK1_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER3_BLOCK2_SUMMATION_RECEIVED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER3_BLOCK2_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER3_BLOCK3_SUMMATION_RECEIVED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER3_BLOCK3_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER3_BLOCK4_SUMMATION_RECEIVED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER3_BLOCK4_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER3_BLOCK5_SUMMATION_RECEIVED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER3_BLOCK5_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER3_BLOCK6_SUMMATION_RECEIVED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER3_BLOCK6_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER3_BLOCK7_SUMMATION_RECEIVED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER3_BLOCK7_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER3_BLOCK8_SUMMATION_RECEIVED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER3_BLOCK8_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER3_BLOCK9_SUMMATION_RECEIVED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER3_BLOCK9_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER3_BLOCK10_SUMMATION_RECEIVED_ID, /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER3_BLOCK10_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER3_BLOCK11_SUMMATION_RECEIVED_ID, /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER3_BLOCK11_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER3_BLOCK12_SUMMATION_RECEIVED_ID, /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER3_BLOCK12_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER3_BLOCK13_SUMMATION_RECEIVED_ID, /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER3_BLOCK13_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER3_BLOCK14_SUMMATION_RECEIVED_ID, /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER3_BLOCK14_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER3_BLOCK15_SUMMATION_RECEIVED_ID, /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER3_BLOCK15_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER3_BLOCK16_SUMMATION_RECEIVED_ID, /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER3_BLOCK16_SUMMATION_RECEIVED_ID */

    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER4_BLOCK1_SUMMATION_RECEIVED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER4_BLOCK1_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER4_BLOCK2_SUMMATION_RECEIVED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER4_BLOCK2_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER4_BLOCK3_SUMMATION_RECEIVED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER4_BLOCK3_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER4_BLOCK4_SUMMATION_RECEIVED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER4_BLOCK4_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER4_BLOCK5_SUMMATION_RECEIVED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER4_BLOCK5_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER4_BLOCK6_SUMMATION_RECEIVED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER4_BLOCK6_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER4_BLOCK7_SUMMATION_RECEIVED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER4_BLOCK7_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER4_BLOCK8_SUMMATION_RECEIVED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER4_BLOCK8_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER4_BLOCK9_SUMMATION_RECEIVED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER4_BLOCK9_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER4_BLOCK10_SUMMATION_RECEIVED_ID, /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER4_BLOCK10_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER4_BLOCK11_SUMMATION_RECEIVED_ID, /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER4_BLOCK11_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER4_BLOCK12_SUMMATION_RECEIVED_ID, /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER4_BLOCK12_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER4_BLOCK13_SUMMATION_RECEIVED_ID, /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER4_BLOCK13_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER4_BLOCK14_SUMMATION_RECEIVED_ID, /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER4_BLOCK14_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER4_BLOCK15_SUMMATION_RECEIVED_ID, /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER4_BLOCK15_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER4_BLOCK16_SUMMATION_RECEIVED_ID, /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER4_BLOCK16_SUMMATION_RECEIVED_ID */

    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER5_BLOCK1_SUMMATION_RECEIVED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER5_BLOCK1_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER5_BLOCK2_SUMMATION_RECEIVED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER5_BLOCK2_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER5_BLOCK3_SUMMATION_RECEIVED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER5_BLOCK3_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER5_BLOCK4_SUMMATION_RECEIVED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER5_BLOCK4_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER5_BLOCK5_SUMMATION_RECEIVED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER5_BLOCK5_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER5_BLOCK6_SUMMATION_RECEIVED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER5_BLOCK6_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER5_BLOCK7_SUMMATION_RECEIVED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER5_BLOCK7_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER5_BLOCK8_SUMMATION_RECEIVED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER5_BLOCK8_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER5_BLOCK9_SUMMATION_RECEIVED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER5_BLOCK9_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER5_BLOCK10_SUMMATION_RECEIVED_ID, /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER5_BLOCK10_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER5_BLOCK11_SUMMATION_RECEIVED_ID, /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER5_BLOCK11_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER5_BLOCK12_SUMMATION_RECEIVED_ID, /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER5_BLOCK12_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER5_BLOCK13_SUMMATION_RECEIVED_ID, /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER5_BLOCK13_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER5_BLOCK14_SUMMATION_RECEIVED_ID, /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER5_BLOCK14_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER5_BLOCK15_SUMMATION_RECEIVED_ID, /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER5_BLOCK15_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER5_BLOCK16_SUMMATION_RECEIVED_ID, /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER5_BLOCK16_SUMMATION_RECEIVED_ID */

    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER6_BLOCK1_SUMMATION_RECEIVED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER6_BLOCK1_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER6_BLOCK2_SUMMATION_RECEIVED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER6_BLOCK2_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER6_BLOCK3_SUMMATION_RECEIVED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER6_BLOCK3_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER6_BLOCK4_SUMMATION_RECEIVED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER6_BLOCK4_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER6_BLOCK5_SUMMATION_RECEIVED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER6_BLOCK5_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER6_BLOCK6_SUMMATION_RECEIVED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER6_BLOCK6_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER6_BLOCK7_SUMMATION_RECEIVED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER6_BLOCK7_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER6_BLOCK8_SUMMATION_RECEIVED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER6_BLOCK8_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER6_BLOCK9_SUMMATION_RECEIVED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER6_BLOCK9_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER6_BLOCK10_SUMMATION_RECEIVED_ID, /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER6_BLOCK10_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER6_BLOCK11_SUMMATION_RECEIVED_ID, /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER6_BLOCK11_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER6_BLOCK12_SUMMATION_RECEIVED_ID, /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER6_BLOCK12_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER6_BLOCK13_SUMMATION_RECEIVED_ID, /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER6_BLOCK13_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER6_BLOCK14_SUMMATION_RECEIVED_ID, /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER6_BLOCK14_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER6_BLOCK15_SUMMATION_RECEIVED_ID, /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER6_BLOCK15_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER6_BLOCK16_SUMMATION_RECEIVED_ID, /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER6_BLOCK16_SUMMATION_RECEIVED_ID */

    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER7_BLOCK1_SUMMATION_RECEIVED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER7_BLOCK1_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER7_BLOCK2_SUMMATION_RECEIVED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER7_BLOCK2_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER7_BLOCK3_SUMMATION_RECEIVED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER7_BLOCK3_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER7_BLOCK4_SUMMATION_RECEIVED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER7_BLOCK4_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER7_BLOCK5_SUMMATION_RECEIVED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER7_BLOCK5_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER7_BLOCK6_SUMMATION_RECEIVED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER7_BLOCK6_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER7_BLOCK7_SUMMATION_RECEIVED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER7_BLOCK7_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER7_BLOCK8_SUMMATION_RECEIVED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER7_BLOCK8_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER7_BLOCK9_SUMMATION_RECEIVED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER7_BLOCK9_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER7_BLOCK10_SUMMATION_RECEIVED_ID, /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER7_BLOCK10_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER7_BLOCK11_SUMMATION_RECEIVED_ID, /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER7_BLOCK11_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER7_BLOCK12_SUMMATION_RECEIVED_ID, /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER7_BLOCK12_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER7_BLOCK13_SUMMATION_RECEIVED_ID, /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER7_BLOCK13_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER7_BLOCK14_SUMMATION_RECEIVED_ID, /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER7_BLOCK14_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER7_BLOCK15_SUMMATION_RECEIVED_ID, /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER7_BLOCK15_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER7_BLOCK16_SUMMATION_RECEIVED_ID, /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER7_BLOCK16_SUMMATION_RECEIVED_ID */

    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER8_BLOCK1_SUMMATION_RECEIVED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER8_BLOCK1_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER8_BLOCK2_SUMMATION_RECEIVED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER8_BLOCK2_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER8_BLOCK3_SUMMATION_RECEIVED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER8_BLOCK3_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER8_BLOCK4_SUMMATION_RECEIVED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER8_BLOCK4_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER8_BLOCK5_SUMMATION_RECEIVED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER8_BLOCK5_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER8_BLOCK6_SUMMATION_RECEIVED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER8_BLOCK6_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER8_BLOCK7_SUMMATION_RECEIVED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER8_BLOCK7_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER8_BLOCK8_SUMMATION_RECEIVED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER8_BLOCK8_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER8_BLOCK9_SUMMATION_RECEIVED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER8_BLOCK9_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER8_BLOCK10_SUMMATION_RECEIVED_ID, /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER8_BLOCK10_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER8_BLOCK11_SUMMATION_RECEIVED_ID, /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER8_BLOCK11_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER8_BLOCK12_SUMMATION_RECEIVED_ID, /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER8_BLOCK12_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER8_BLOCK13_SUMMATION_RECEIVED_ID, /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER8_BLOCK13_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER8_BLOCK14_SUMMATION_RECEIVED_ID, /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER8_BLOCK14_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER8_BLOCK15_SUMMATION_RECEIVED_ID, /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER8_BLOCK15_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER8_BLOCK16_SUMMATION_RECEIVED_ID, /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER8_BLOCK16_SUMMATION_RECEIVED_ID */

    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER9_BLOCK1_SUMMATION_RECEIVED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER9_BLOCK1_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER9_BLOCK2_SUMMATION_RECEIVED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER9_BLOCK2_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER9_BLOCK3_SUMMATION_RECEIVED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER9_BLOCK3_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER9_BLOCK4_SUMMATION_RECEIVED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER9_BLOCK4_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER9_BLOCK5_SUMMATION_RECEIVED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER9_BLOCK5_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER9_BLOCK6_SUMMATION_RECEIVED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER9_BLOCK6_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER9_BLOCK7_SUMMATION_RECEIVED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER9_BLOCK7_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER9_BLOCK8_SUMMATION_RECEIVED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER9_BLOCK8_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER9_BLOCK9_SUMMATION_RECEIVED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER9_BLOCK9_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER9_BLOCK10_SUMMATION_RECEIVED_ID, /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER9_BLOCK10_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER9_BLOCK11_SUMMATION_RECEIVED_ID, /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER9_BLOCK11_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER9_BLOCK12_SUMMATION_RECEIVED_ID, /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER9_BLOCK12_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER9_BLOCK13_SUMMATION_RECEIVED_ID, /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER9_BLOCK13_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER9_BLOCK14_SUMMATION_RECEIVED_ID, /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER9_BLOCK14_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER9_BLOCK15_SUMMATION_RECEIVED_ID, /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER9_BLOCK15_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER9_BLOCK16_SUMMATION_RECEIVED_ID, /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER9_BLOCK16_SUMMATION_RECEIVED_ID */

    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER10_BLOCK1_SUMMATION_RECEIVED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER10_BLOCK1_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER10_BLOCK2_SUMMATION_RECEIVED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER10_BLOCK2_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER10_BLOCK3_SUMMATION_RECEIVED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER10_BLOCK3_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER10_BLOCK4_SUMMATION_RECEIVED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER10_BLOCK4_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER10_BLOCK5_SUMMATION_RECEIVED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER10_BLOCK5_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER10_BLOCK6_SUMMATION_RECEIVED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER10_BLOCK6_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER10_BLOCK7_SUMMATION_RECEIVED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER10_BLOCK7_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER10_BLOCK8_SUMMATION_RECEIVED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER10_BLOCK8_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER10_BLOCK9_SUMMATION_RECEIVED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER10_BLOCK9_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER10_BLOCK10_SUMMATION_RECEIVED_ID, /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER10_BLOCK10_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER10_BLOCK11_SUMMATION_RECEIVED_ID, /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER10_BLOCK11_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER10_BLOCK12_SUMMATION_RECEIVED_ID, /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER10_BLOCK12_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER10_BLOCK13_SUMMATION_RECEIVED_ID, /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER10_BLOCK13_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER10_BLOCK14_SUMMATION_RECEIVED_ID, /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER10_BLOCK14_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER10_BLOCK15_SUMMATION_RECEIVED_ID, /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER10_BLOCK15_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER10_BLOCK16_SUMMATION_RECEIVED_ID, /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER10_BLOCK16_SUMMATION_RECEIVED_ID */

    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER11_BLOCK1_SUMMATION_RECEIVED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER11_BLOCK1_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER11_BLOCK2_SUMMATION_RECEIVED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER11_BLOCK2_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER11_BLOCK3_SUMMATION_RECEIVED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER11_BLOCK3_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER11_BLOCK4_SUMMATION_RECEIVED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER11_BLOCK4_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER11_BLOCK5_SUMMATION_RECEIVED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER11_BLOCK5_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER11_BLOCK6_SUMMATION_RECEIVED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER11_BLOCK6_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER11_BLOCK7_SUMMATION_RECEIVED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER11_BLOCK7_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER11_BLOCK8_SUMMATION_RECEIVED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER11_BLOCK8_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER11_BLOCK9_SUMMATION_RECEIVED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER11_BLOCK9_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER11_BLOCK10_SUMMATION_RECEIVED_ID, /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER11_BLOCK10_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER11_BLOCK11_SUMMATION_RECEIVED_ID, /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER11_BLOCK11_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER11_BLOCK12_SUMMATION_RECEIVED_ID, /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER11_BLOCK12_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER11_BLOCK13_SUMMATION_RECEIVED_ID, /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER11_BLOCK13_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER11_BLOCK14_SUMMATION_RECEIVED_ID, /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER11_BLOCK14_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER11_BLOCK15_SUMMATION_RECEIVED_ID, /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER11_BLOCK15_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER11_BLOCK16_SUMMATION_RECEIVED_ID, /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER11_BLOCK16_SUMMATION_RECEIVED_ID */

    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER12_BLOCK1_SUMMATION_RECEIVED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER12_BLOCK1_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER12_BLOCK2_SUMMATION_RECEIVED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER12_BLOCK2_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER12_BLOCK3_SUMMATION_RECEIVED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER12_BLOCK3_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER12_BLOCK4_SUMMATION_RECEIVED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER12_BLOCK4_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER12_BLOCK5_SUMMATION_RECEIVED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER12_BLOCK5_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER12_BLOCK6_SUMMATION_RECEIVED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER12_BLOCK6_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER12_BLOCK7_SUMMATION_RECEIVED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER12_BLOCK7_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER12_BLOCK8_SUMMATION_RECEIVED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER12_BLOCK8_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER12_BLOCK9_SUMMATION_RECEIVED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER12_BLOCK9_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER12_BLOCK10_SUMMATION_RECEIVED_ID, /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER12_BLOCK10_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER12_BLOCK11_SUMMATION_RECEIVED_ID, /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER12_BLOCK11_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER12_BLOCK12_SUMMATION_RECEIVED_ID, /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER12_BLOCK12_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER12_BLOCK13_SUMMATION_RECEIVED_ID, /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER12_BLOCK13_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER12_BLOCK14_SUMMATION_RECEIVED_ID, /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER12_BLOCK14_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER12_BLOCK15_SUMMATION_RECEIVED_ID, /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER12_BLOCK15_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER12_BLOCK16_SUMMATION_RECEIVED_ID, /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER12_BLOCK16_SUMMATION_RECEIVED_ID */

    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER13_BLOCK1_SUMMATION_RECEIVED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER13_BLOCK1_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER13_BLOCK2_SUMMATION_RECEIVED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER13_BLOCK2_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER13_BLOCK3_SUMMATION_RECEIVED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER13_BLOCK3_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER13_BLOCK4_SUMMATION_RECEIVED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER13_BLOCK4_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER13_BLOCK5_SUMMATION_RECEIVED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER13_BLOCK5_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER13_BLOCK6_SUMMATION_RECEIVED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER13_BLOCK6_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER13_BLOCK7_SUMMATION_RECEIVED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER13_BLOCK7_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER13_BLOCK8_SUMMATION_RECEIVED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER13_BLOCK8_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER13_BLOCK9_SUMMATION_RECEIVED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER13_BLOCK9_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER13_BLOCK10_SUMMATION_RECEIVED_ID, /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER13_BLOCK10_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER13_BLOCK11_SUMMATION_RECEIVED_ID, /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER13_BLOCK11_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER13_BLOCK12_SUMMATION_RECEIVED_ID, /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER13_BLOCK12_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER13_BLOCK13_SUMMATION_RECEIVED_ID, /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER13_BLOCK13_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER13_BLOCK14_SUMMATION_RECEIVED_ID, /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER13_BLOCK14_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER13_BLOCK15_SUMMATION_RECEIVED_ID, /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER13_BLOCK15_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER13_BLOCK16_SUMMATION_RECEIVED_ID, /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER13_BLOCK16_SUMMATION_RECEIVED_ID */

    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER14_BLOCK1_SUMMATION_RECEIVED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER14_BLOCK1_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER14_BLOCK2_SUMMATION_RECEIVED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER14_BLOCK2_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER14_BLOCK3_SUMMATION_RECEIVED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER14_BLOCK3_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER14_BLOCK4_SUMMATION_RECEIVED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER14_BLOCK4_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER14_BLOCK5_SUMMATION_RECEIVED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER14_BLOCK5_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER14_BLOCK6_SUMMATION_RECEIVED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER14_BLOCK6_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER14_BLOCK7_SUMMATION_RECEIVED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER14_BLOCK7_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER14_BLOCK8_SUMMATION_RECEIVED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER14_BLOCK8_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER14_BLOCK9_SUMMATION_RECEIVED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER14_BLOCK9_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER14_BLOCK10_SUMMATION_RECEIVED_ID, /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER14_BLOCK10_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER14_BLOCK11_SUMMATION_RECEIVED_ID, /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER14_BLOCK11_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER14_BLOCK12_SUMMATION_RECEIVED_ID, /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER14_BLOCK12_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER14_BLOCK13_SUMMATION_RECEIVED_ID, /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER14_BLOCK13_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER14_BLOCK14_SUMMATION_RECEIVED_ID, /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER14_BLOCK14_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER14_BLOCK15_SUMMATION_RECEIVED_ID, /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER14_BLOCK15_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER14_BLOCK16_SUMMATION_RECEIVED_ID, /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER14_BLOCK16_SUMMATION_RECEIVED_ID */

    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER15_BLOCK1_SUMMATION_RECEIVED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER15_BLOCK1_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER15_BLOCK2_SUMMATION_RECEIVED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER15_BLOCK2_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER15_BLOCK3_SUMMATION_RECEIVED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER15_BLOCK3_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER15_BLOCK4_SUMMATION_RECEIVED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER15_BLOCK4_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER15_BLOCK5_SUMMATION_RECEIVED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER15_BLOCK5_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER15_BLOCK6_SUMMATION_RECEIVED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER15_BLOCK6_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER15_BLOCK7_SUMMATION_RECEIVED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER15_BLOCK7_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER15_BLOCK8_SUMMATION_RECEIVED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER15_BLOCK8_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER15_BLOCK9_SUMMATION_RECEIVED_ID,  /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER15_BLOCK9_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER15_BLOCK10_SUMMATION_RECEIVED_ID, /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER15_BLOCK10_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER15_BLOCK11_SUMMATION_RECEIVED_ID, /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER15_BLOCK11_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER15_BLOCK12_SUMMATION_RECEIVED_ID, /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER15_BLOCK12_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER15_BLOCK13_SUMMATION_RECEIVED_ID, /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER15_BLOCK13_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER15_BLOCK14_SUMMATION_RECEIVED_ID, /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER15_BLOCK14_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER15_BLOCK15_SUMMATION_RECEIVED_ID, /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER15_BLOCK15_SUMMATION_RECEIVED_ID */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER15_BLOCK16_SUMMATION_RECEIVED_ID, /*!< ESP_ZB_ZCL_ATTR_METERING_CURRENT_TIER15_BLOCK16_SUMMATION_RECEIVED_ID */


    /* Meter Billing Attribute Set */
    ESP_ZB_ZCL_ATTR_METERING_BILL_TO_DATE_DELIVERED =
        ESP_ZB_ZCL_ATTR_SET_WITH_ATTR_ID(ESP_ZB_ZCL_METERING_SET_METER_BILLING, 0x00), /*!< BillToDateDelivered provides a value for the costs in the current billing period */
    ESP_ZB_ZCL_ATTR_METERING_BILL_TO_DATE_TIMESTAMP_DELIVERED,                         /*!< BillToDateTimeStampDelivered The UTC timestamp
                                                                                        *   when the associated BillToDateDelivered attribute was last updated
                                                                                        */
    ESP_ZB_ZCL_ATTR_METERING_PROJECTED_BILL_DELIVERED,                                 /*!< ProjectedBillDelivered provides a value indicating what the estimated state of the account
                                                                                        *   will be at the end of the billing period based on past consumption
                                                                                        */
    ESP_ZB_ZCL_ATTR_METERING_PROJECTED_BILL_TIME_STAMP_DELIVERED,                      /*!< ProjectedBillTimeStampDelivered
                                                                                        *   The UTC timestamp when the associated ProjectedBillDelivered attribute was last updated
                                                                                        */
    ESP_ZB_ZCL_ATTR_METERING_BILL_DELIVERED_TRAILING_DIGIT,                            /*!< BillDeliveredTrailingDigit An 8-bit BitMap used to determine where the decimal point is located
                                                                                        *   in the BillToDateDelivered and ProjectedBillDelivered attributes
                                                                                        */
    /* 0x0A05 to 0x0A0F reserved */
    
    ESP_ZB_ZCL_ATTR_METERING_BILL_TO_DATE_RECEIVED =
        ESP_ZB_ZCL_ATTR_SET_WITH_ATTR_ID(ESP_ZB_ZCL_METERING_SET_METER_BILLING, 0x10), /*!< BillToDateReceived provides a value for the costs in the current billing period.
                                                                                        *   This attribute is measured in a base unit of Currency with the decimal point located
                                                                                        *   as indicated by the BillReceivedTrailingDigit attribute
                                                                                        */
    ESP_ZB_ZCL_ATTR_METERING_BILL_TO_DATE_TIMESTAMP_RECEIVED,                          /*!< BillToDateTimeStampReceived The UTC timestamp
                                                                                        *   when the associated BillToDateReceived attribute was last updated
                                                                                        */
    ESP_ZB_ZCL_ATTR_METERING_PROJECTED_BILL_RECEIVED,                                  /*!< ProjectedBillReceived provides a value indicating what the estimated state of the account
                                                                                        *   will be at the end of the billing period based on past generation
                                                                                        */
    ESP_ZB_ZCL_ATTR_METERING_PROJECTED_BILL_TIME_STAMP_RECEIVED,                       /*!< ProjectedBillTimeStampReceived
                                                                                        *   The UTC timestamp when the associated ProjectedBillReceived attribute was last updated
                                                                                        */
    ESP_ZB_ZCL_ATTR_METERING_BILL_RECEIVED_TRAILING_DIGIT,                             /*!< BillReceivedTrailingDigit
                                                                                        *   An 8-bit BitMap used to determine where the decimal point is located
                                                                                        *   in the BillToDateReceived and ProjectedBillReceived attributes
                                                                                        */
    /* 0x0A15 to 0x0AFF reserved */


    /* Supply Control Attribute Set */
    ESP_ZB_ZCL_ATTR_METERING_PROPOSED_CHANGE_SUPPLY_IMPLEMENTATION_TIME = ESP_ZB_ZCL_ATTR_SET_WITH_ATTR_ID(
        ESP_ZB_ZCL_METERING_SET_SUPPLY_CONTROL, 0x00),      /*!< ProposedChangeSupplyImplementationTime indicates the time at which a proposed change to the supply is to be implemented */
    ESP_ZB_ZCL_ATTR_METERING_PROPOSED_CHANGE_SUPPLY_STATUS, /*!< ProposedChangeSupplyStatus indicates the proposed status of the supply
                                                             *   once the change to the supply has be been implemented
                                                             */
    /* 0x0B02 to 0x0B0F reserved */

    ESP_ZB_ZCL_ATTR_METERING_UNCONTROLLED_FLOW_THRESHOLD = ESP_ZB_ZCL_ATTR_SET_WITH_ATTR_ID(
        ESP_ZB_ZCL_METERING_SET_SUPPLY_CONTROL, 0x10), /*!< UncontrolledFlowThreshold indicates the threshold above which a flow meter (e.g. Gas or Water) shall detect an uncontrolled flow */
    ESP_ZB_ZCL_ATTR_METERING_UNCONTROLLED_FLOW_THRESHOLD_UNIT_OF_MEASURE, /*!< UncontrolledFlowThresholdUnitOfMeasure indicates the unit of measure
                                                                           *   used in conjunction with the Uncontrolled Flow Threshold attribute
                                                                           */
    ESP_ZB_ZCL_ATTR_METERING_UNCONTROLLED_FLOW_MULTIPLIER,                /*!< UncontrolledFlowMultiplier indicates the multiplier,
                                                                           *   to be used in conjunction with the Uncontrolled Flow Threshold and Uncontrolled Flow Divisor attributes,
                                                                           *   to determine the true flow threshold value
                                                                           */
    ESP_ZB_ZCL_ATTR_METERING_UNCONTROLLED_FLOW_DIVISOR,                   /*!< UncontrolledFlowDivisor
                                                                           *   The Uncontrolled Flow Divisor attribute indicates the divisor, to be used in conjunction
                                                                           *   with the Uncontrolled Flow Threshold and Uncontrolled Flow Multiplier attributes,
                                                                           *   to determine the true flow threshold value
                                                                           */
    ESP_ZB_ZCL_ATTR_METERING_FLOW_STABILISATION_PERIOD,                   /*!< FlowStabilisationPeriod indicates the time given to allow the flow to stabilize */
    ESP_ZB_ZCL_ATTR_METERING_FLOW_MEASUREMENT_PERIOD,                     /*!< FlowMeasurementPeriodAttribute indicates the period
                                                                           *   over which the flow is measured and compared against the Uncontrolled Flow Threshold attribute
                                                                           */
    /* 0x0B16 to 0x0BFF reserved */


    /* Alternative Historical Consumption Attribute Set */
    ESP_ZB_ZCL_ATTR_METERING_ALT_INSTANTANEOUS_DEMAND = ESP_ZB_ZCL_ATTR_SET_WITH_ATTR_ID(
        ESP_ZB_ZCL_METERING_SET_ALTERNATIVE_HISTORICAL_CONSUMPTION, 0x00),              /*!< AlternativeInstantaneousDemand represents the current Demand delivered or received at the premises */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_DAY_ALT_CONSUMPTION_DELIVERED,                     /*!< CurrentDayAlternativeConsumptionDelivered represents the summed value
                                                                                         *   delivered to the premises since the Historical Freeze Time (HFT)
                                                                                         */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_DAY_ALT_CONSUMPTION_RECEIVED,                      /*!< CurrentDayAlternativeConsumptionReceived represents the summed value
                                                                                         *   received from the premises since the Historical Freeze Time (HFT)
                                                                                         */
    ESP_ZB_ZCL_ATTR_METERING_PREVIOUS_DAY_ALT_CONSUMPTION_DELIVERED,                    /*!< PreviousDayAlternativeConsumptionDelivered represents the summed value
                                                                                         *   delivered to the premises within the previous 24 hour period
                                                                                         *   starting at the alternative Historical Freeze Time (HFT)
                                                                                         */
    ESP_ZB_ZCL_ATTR_METERING_PREVIOUS_DAY_ALT_CONSUMPTION_RECEIVED,                     /*!< PreviousDayAlternativeConsumptionReceived represents the summed value
                                                                                         *   received to the premises within the previous 24 hour period
                                                                                         *   starting at the alternative Historical Freeze Time (HFT)
                                                                                         */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_ALT_PARTIAL_PROFILE_INTERVAL_START_TIME_DELIVERED, /*!< CurrentAlternativePartialProfileIntervalStartTimeDelivered represents the start time
                                                                                         *   of the current Load Profile interval being accumulated for commodity delivered
                                                                                         */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_ALT_PARTIAL_PROFILE_INTERVAL_START_TIME_RECEIVED,  /*!< CurrentAlternativePartialProfileIntervalStartTimeReceived represents the start time
                                                                                         *   of the current Load Profile interval being accumulated for commodity received
                                                                                         */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_ALT_PARTIAL_PROFILE_INTERVAL_VALUE_DELIVERED,      /*!< CurrentAlternativePartialProfileIntervalValueDelivered represents the value
                                                                                         *   of the current Load Profile interval being accumulated for commodity delivered
                                                                                         */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_ALT_PARTIAL_PROFILE_INTERVAL_VALUE_RECEIVED,       /*!< CurrentAlternativePartialProfileIntervalValueReceived represents the value
                                                                                         *   of the current Load Profile interval being accumulated for commodity received
                                                                                         */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_DAY_ALT_MAX_PRESSURE,                              /*!< CurrentDayAlternativeMaxPressure is the maximum pressure
                                                                                         *   reported during a day from the water or gas meter
                                                                                         */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_DAY_ALT_MIN_PRESSURE,                              /*!< CurrentDayAlternativeMinPressure is the minimum pressure
                                                                                         *   reported during a day from the water or gas meter
                                                                                         */
    ESP_ZB_ZCL_ATTR_METERING_PREVIOUS_DAY_ALT_MAX_PRESSURE,                             /*!< PreviousDayAlternativeMaxPressure represents the maximum pressure
                                                                                         *   reported during previous day from the water or gas meter
                                                                                         */
    ESP_ZB_ZCL_ATTR_METERING_PREVIOUS_DAY_ALT_MIN_PRESSURE,                             /*!< PreviousDayAlternativeMinPressure  represents the minimum pressure
                                                                                         *   reported during previous day from the water or gas meter
                                                                                         */

    ESP_ZB_ZCL_ATTR_METERING_CURRENT_DAY_ALT_MAX_DEMAND,  /*!< CurrentDayAlternativeMaxDemand represents the maximum demand or rate
                                                           *   of delivered value of Energy, Gas, or Water being utilized at the premises
                                                           */
    ESP_ZB_ZCL_ATTR_METERING_PREVIOUS_DAY_ALT_MAX_DEMAND, /*!< PreviousDayAlternativeMaxDemand represents represents the maximum demand or rate
                                                           *   of delivered value of Energy, Gas, or Water being utilized at the premises
                                                           */

    ESP_ZB_ZCL_ATTR_METERING_CURRENT_MONTH_ALT_MAX_DEMAND, /*!< CurrentMonthAlternativeMaxDemand is the maximum demand reported during a month from the meter */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_YEAR_ALT_MAX_DEMAND,  /*!< CurrentYearAlternativeMaxDemand is the maximum demand reported during a year from the meter */

    /* 0x0C11 to 0x0C1F reserved */

    ESP_ZB_ZCL_ATTR_METERING_PREVIOUS_DAY2_ALT_CONSUMPTION_DELIVERED = ESP_ZB_ZCL_ATTR_SET_WITH_ATTR_ID(
        ESP_ZB_ZCL_METERING_SET_ALTERNATIVE_HISTORICAL_CONSUMPTION, 0x20), /*!< PreviousDayNAlternativeConsumptionDelivered represents the summed value delivered to the premises within the previous 24
                                                                            *   hour period starting at the Historical Freeze Time (HFT).
                                                                            */
    ESP_ZB_ZCL_ATTR_METERING_PREVIOUS_DAY2_ALT_CONSUMPTION_RECEIVED,       /*!< PreviousDayNAlternativeConsumptionReceived
                                                                            *   represents the summed value received from the premises
                                                                            *   within the previous 24 hour period starting at the Historical Freeze Time (HFT)
                                                                            */
    ESP_ZB_ZCL_ATTR_METERING_PREVIOUS_DAY3_ALT_CONSUMPTION_DELIVERED,      /*!< ESP_ZB_ZCL_ATTR_METERING_PREVIOUS_DAY3_ALT_CONSUMPTION_DELIVERED */
    ESP_ZB_ZCL_ATTR_METERING_PREVIOUS_DAY3_ALT_CONSUMPTION_RECEIVED,       /*!< ESP_ZB_ZCL_ATTR_METERING_PREVIOUS_DAY3_ALT_CONSUMPTION_RECEIVED */
    ESP_ZB_ZCL_ATTR_METERING_PREVIOUS_DAY4_ALT_CONSUMPTION_DELIVERED,      /*!< ESP_ZB_ZCL_ATTR_METERING_PREVIOUS_DAY4_ALT_CONSUMPTION_DELIVERED */
    ESP_ZB_ZCL_ATTR_METERING_PREVIOUS_DAY4_ALT_CONSUMPTION_RECEIVED,       /*!< ESP_ZB_ZCL_ATTR_METERING_PREVIOUS_DAY4_ALT_CONSUMPTION_RECEIVED */
    ESP_ZB_ZCL_ATTR_METERING_PREVIOUS_DAY5_ALT_CONSUMPTION_DELIVERED,      /*!< ESP_ZB_ZCL_ATTR_METERING_PREVIOUS_DAY5_ALT_CONSUMPTION_DELIVERED */
    ESP_ZB_ZCL_ATTR_METERING_PREVIOUS_DAY5_ALT_CONSUMPTION_RECEIVED,       /*!< ESP_ZB_ZCL_ATTR_METERING_PREVIOUS_DAY5_ALT_CONSUMPTION_RECEIVED */
    ESP_ZB_ZCL_ATTR_METERING_PREVIOUS_DAY6_ALT_CONSUMPTION_DELIVERED,      /*!< ESP_ZB_ZCL_ATTR_METERING_PREVIOUS_DAY6_ALT_CONSUMPTION_DELIVERED */
    ESP_ZB_ZCL_ATTR_METERING_PREVIOUS_DAY6_ALT_CONSUMPTION_RECEIVED,       /*!< ESP_ZB_ZCL_ATTR_METERING_PREVIOUS_DAY6_ALT_CONSUMPTION_RECEIVED */
    ESP_ZB_ZCL_ATTR_METERING_PREVIOUS_DAY7_ALT_CONSUMPTION_DELIVERED,      /*!< ESP_ZB_ZCL_ATTR_METERING_PREVIOUS_DAY7_ALT_CONSUMPTION_DELIVERED */
    ESP_ZB_ZCL_ATTR_METERING_PREVIOUS_DAY7_ALT_CONSUMPTION_RECEIVED,       /*!< ESP_ZB_ZCL_ATTR_METERING_PREVIOUS_DAY7_ALT_CONSUMPTION_RECEIVED */
    ESP_ZB_ZCL_ATTR_METERING_PREVIOUS_DAY8_ALT_CONSUMPTION_DELIVERED,      /*!< ESP_ZB_ZCL_ATTR_METERING_PREVIOUS_DAY8_ALT_CONSUMPTION_DELIVERED */
    ESP_ZB_ZCL_ATTR_METERING_PREVIOUS_DAY8_ALT_CONSUMPTION_RECEIVED,       /*!< ESP_ZB_ZCL_ATTR_METERING_PREVIOUS_DAY8_ALT_CONSUMPTION_RECEIVED */

    /* 0x0C2E to 0x0C2F reserved */

    ESP_ZB_ZCL_ATTR_METERING_CURRENT_WEEK_ALT_CONSUMPTION_DELIVERED = ESP_ZB_ZCL_ATTR_SET_WITH_ATTR_ID(
        ESP_ZB_ZCL_METERING_SET_ALTERNATIVE_HISTORICAL_CONSUMPTION, 0x30), /*!< CurrentWeekAlternativeConsumptionDelivered represents the summed value
                                                                            *   delivered to the premises since the Historical Freeze Time (HFT) on Monday to the last HFT read
                                                                            */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_WEEK_ALT_CONSUMPTION_RECEIVED,        /*!< CurrentWeekAlternativeConsumptionReceived represents the summed value
                                                                            *   received from the premises since the Historical Freeze Time (HFT) on Monday to the last HFT read
                                                                            */

    ESP_ZB_ZCL_ATTR_METERING_PREVIOUS_WEEK_ALT_CONSUMPTION_DELIVERED,  /*!< PreviousWeekNAlternativeConsumptionDelivered represents the summed value
                                                                        *   delivered to the premises within the previous week period
                                                                        *   starting at the Historical Freeze Time (HFT) on the Monday to the Sunday
                                                                        */
    ESP_ZB_ZCL_ATTR_METERING_PREVIOUS_WEEK_ALT_CONSUMPTION_RECEIVED,   /*!< PreviousWeekNAlternativeConsumptionReceived represents the summed value
                                                                        *   received from the premises within the previous week period
                                                                        *   starting at the Historical Freeze Time (HFT) on the Monday to the Sunday
                                                                        */
    ESP_ZB_ZCL_ATTR_METERING_PREVIOUS_WEEK2_ALT_CONSUMPTION_DELIVERED, /*!< ESP_ZB_ZCL_ATTR_METERING_PREVIOUS_WEEK2_ALT_CONSUMPTION_DELIVERED */
    ESP_ZB_ZCL_ATTR_METERING_PREVIOUS_WEEK2_ALT_CONSUMPTION_RECEIVED,  /*!< ESP_ZB_ZCL_ATTR_METERING_PREVIOUS_WEEK2_ALT_CONSUMPTION_RECEIVED */
    ESP_ZB_ZCL_ATTR_METERING_PREVIOUS_WEEK3_ALT_CONSUMPTION_DELIVERED, /*!< ESP_ZB_ZCL_ATTR_METERING_PREVIOUS_WEEK3_ALT_CONSUMPTION_DELIVERED */
    ESP_ZB_ZCL_ATTR_METERING_PREVIOUS_WEEK3_ALT_CONSUMPTION_RECEIVED,  /*!< ESP_ZB_ZCL_ATTR_METERING_PREVIOUS_WEEK3_ALT_CONSUMPTION_RECEIVED */
    ESP_ZB_ZCL_ATTR_METERING_PREVIOUS_WEEK4_ALT_CONSUMPTION_DELIVERED, /*!< ESP_ZB_ZCL_ATTR_METERING_PREVIOUS_WEEK4_ALT_CONSUMPTION_DELIVERED */
    ESP_ZB_ZCL_ATTR_METERING_PREVIOUS_WEEK4_ALT_CONSUMPTION_RECEIVED,  /*!< ESP_ZB_ZCL_ATTR_METERING_PREVIOUS_WEEK4_ALT_CONSUMPTION_RECEIVED */
    ESP_ZB_ZCL_ATTR_METERING_PREVIOUS_WEEK5_ALT_CONSUMPTION_DELIVERED, /*!< ESP_ZB_ZCL_ATTR_METERING_PREVIOUS_WEEK5_ALT_CONSUMPTION_DELIVERED */
    ESP_ZB_ZCL_ATTR_METERING_PREVIOUS_WEEK5_ALT_CONSUMPTION_RECEIVED,  /*!< ESP_ZB_ZCL_ATTR_METERING_PREVIOUS_WEEK5_ALT_CONSUMPTION_RECEIVED */

    /* 0x0C3C to 0x0C3F reserved */
    
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_MONTH_ALT_CONSUMPTION_DELIVERED = ESP_ZB_ZCL_ATTR_SET_WITH_ATTR_ID(
        ESP_ZB_ZCL_METERING_SET_ALTERNATIVE_HISTORICAL_CONSUMPTION, 0x40), /*!< CurrentMonthAlternativeConsumptionDelivered represents the summed value delivered to the premises
                                                                            *   since the Historical Freeze Time (HFT) on the 1st of the month to the last HFT read
                                                                            */
    ESP_ZB_ZCL_ATTR_METERING_CURRENT_MONTH_ALT_CONSUMPTION_RECEIVED,       /*!< CurrentMonthAlternativeConsumptionReceived represents the summed value
                                                                            *   received from the premises since the Historical Freeze Time (HFT)
                                                                            *   on the 1st of the month to the last HFT read
                                                                            */

    ESP_ZB_ZCL_ATTR_METERING_PREVIOUS_MONTH_ALT_CONSUMPTION_DELIVERED,   /*!< PreviousMonthNAlternativeConsumptionDelivered represents the summed value
                                                                          *   delivered to the premises within the previous Month period
                                                                          *   starting at the Historical Freeze Time (HFT) on the 1st of the month to the last day of the month
                                                                          */
    ESP_ZB_ZCL_ATTR_METERING_PREVIOUS_MONTH_ALT_CONSUMPTION_RECEIVED,    /*!< PreviousMonthNAlternativeConsumptionReceived represents the summed value
                                                                          *   received from the premises within the previous month period
                                                                          *   starting at the Historical Freeze Time (HFT) on the 1st of the month to the last day of the month
                                                                          */
    ESP_ZB_ZCL_ATTR_METERING_PREVIOUS_MONTH2_ALT_CONSUMPTION_DELIVERED,  /*!< ESP_ZB_ZCL_ATTR_METERING_PREVIOUS_MONTH2_ALT_CONSUMPTION_DELIVERED */
    ESP_ZB_ZCL_ATTR_METERING_PREVIOUS_MONTH2_ALT_CONSUMPTION_RECEIVED,   /*!< ESP_ZB_ZCL_ATTR_METERING_PREVIOUS_MONTH2_ALT_CONSUMPTION_RECEIVED */
    ESP_ZB_ZCL_ATTR_METERING_PREVIOUS_MONTH3_ALT_CONSUMPTION_DELIVERED,  /*!< ESP_ZB_ZCL_ATTR_METERING_PREVIOUS_MONTH3_ALT_CONSUMPTION_DELIVERED */
    ESP_ZB_ZCL_ATTR_METERING_PREVIOUS_MONTH3_ALT_CONSUMPTION_RECEIVED,   /*!< ESP_ZB_ZCL_ATTR_METERING_PREVIOUS_MONTH3_ALT_CONSUMPTION_RECEIVED */
    ESP_ZB_ZCL_ATTR_METERING_PREVIOUS_MONTH4_ALT_CONSUMPTION_DELIVERED,  /*!< ESP_ZB_ZCL_ATTR_METERING_PREVIOUS_MONTH4_ALT_CONSUMPTION_DELIVERED */
    ESP_ZB_ZCL_ATTR_METERING_PREVIOUS_MONTH4_ALT_CONSUMPTION_RECEIVED,   /*!< ESP_ZB_ZCL_ATTR_METERING_PREVIOUS_MONTH4_ALT_CONSUMPTION_RECEIVED */
    ESP_ZB_ZCL_ATTR_METERING_PREVIOUS_MONTH5_ALT_CONSUMPTION_DELIVERED,  /*!< ESP_ZB_ZCL_ATTR_METERING_PREVIOUS_MONTH5_ALT_CONSUMPTION_DELIVERED */
    ESP_ZB_ZCL_ATTR_METERING_PREVIOUS_MONTH5_ALT_CONSUMPTION_RECEIVED,   /*!< ESP_ZB_ZCL_ATTR_METERING_PREVIOUS_MONTH5_ALT_CONSUMPTION_RECEIVED */
    ESP_ZB_ZCL_ATTR_METERING_PREVIOUS_MONTH6_ALT_CONSUMPTION_DELIVERED,  /*!< ESP_ZB_ZCL_ATTR_METERING_PREVIOUS_MONTH6_ALT_CONSUMPTION_DELIVERED */
    ESP_ZB_ZCL_ATTR_METERING_PREVIOUS_MONTH6_ALT_CONSUMPTION_RECEIVED,   /*!< ESP_ZB_ZCL_ATTR_METERING_PREVIOUS_MONTH6_ALT_CONSUMPTION_RECEIVED */
    ESP_ZB_ZCL_ATTR_METERING_PREVIOUS_MONTH7_ALT_CONSUMPTION_DELIVERED,  /*!< ESP_ZB_ZCL_ATTR_METERING_PREVIOUS_MONTH7_ALT_CONSUMPTION_DELIVERED */
    ESP_ZB_ZCL_ATTR_METERING_PREVIOUS_MONTH7_ALT_CONSUMPTION_RECEIVED,   /*!< ESP_ZB_ZCL_ATTR_METERING_PREVIOUS_MONTH7_ALT_CONSUMPTION_RECEIVED */
    ESP_ZB_ZCL_ATTR_METERING_PREVIOUS_MONTH8_ALT_CONSUMPTION_DELIVERED,  /*!< ESP_ZB_ZCL_ATTR_METERING_PREVIOUS_MONTH8_ALT_CONSUMPTION_DELIVERED */
    ESP_ZB_ZCL_ATTR_METERING_PREVIOUS_MONTH8_ALT_CONSUMPTION_RECEIVED,   /*!< ESP_ZB_ZCL_ATTR_METERING_PREVIOUS_MONTH8_ALT_CONSUMPTION_RECEIVED */
    ESP_ZB_ZCL_ATTR_METERING_PREVIOUS_MONTH9_ALT_CONSUMPTION_DELIVERED,  /*!< ESP_ZB_ZCL_ATTR_METERING_PREVIOUS_MONTH9_ALT_CONSUMPTION_DELIVERED */
    ESP_ZB_ZCL_ATTR_METERING_PREVIOUS_MONTH9_ALT_CONSUMPTION_RECEIVED,   /*!< ESP_ZB_ZCL_ATTR_METERING_PREVIOUS_MONTH9_ALT_CONSUMPTION_RECEIVED */
    ESP_ZB_ZCL_ATTR_METERING_PREVIOUS_MONTH10_ALT_CONSUMPTION_DELIVERED, /*!< ESP_ZB_ZCL_ATTR_METERING_PREVIOUS_MONTH10_ALT_CONSUMPTION_DELIVERED */
    ESP_ZB_ZCL_ATTR_METERING_PREVIOUS_MONTH10_ALT_CONSUMPTION_RECEIVED,  /*!< ESP_ZB_ZCL_ATTR_METERING_PREVIOUS_MONTH10_ALT_CONSUMPTION_RECEIVED */
    ESP_ZB_ZCL_ATTR_METERING_PREVIOUS_MONTH11_ALT_CONSUMPTION_DELIVERED, /*!< ESP_ZB_ZCL_ATTR_METERING_PREVIOUS_MONTH11_ALT_CONSUMPTION_DELIVERED */
    ESP_ZB_ZCL_ATTR_METERING_PREVIOUS_MONTH11_ALT_CONSUMPTION_RECEIVED,  /*!< ESP_ZB_ZCL_ATTR_METERING_PREVIOUS_MONTH11_ALT_CONSUMPTION_RECEIVED */
    ESP_ZB_ZCL_ATTR_METERING_PREVIOUS_MONTH12_ALT_CONSUMPTION_DELIVERED, /*!< ESP_ZB_ZCL_ATTR_METERING_PREVIOUS_MONTH12_ALT_CONSUMPTION_DELIVERED */
    ESP_ZB_ZCL_ATTR_METERING_PREVIOUS_MONTH12_ALT_CONSUMPTION_RECEIVED,  /*!< ESP_ZB_ZCL_ATTR_METERING_PREVIOUS_MONTH12_ALT_CONSUMPTION_RECEIVED */
    ESP_ZB_ZCL_ATTR_METERING_PREVIOUS_MONTH13_ALT_CONSUMPTION_DELIVERED, /*!< ESP_ZB_ZCL_ATTR_METERING_PREVIOUS_MONTH13_ALT_CONSUMPTION_DELIVERED */
    ESP_ZB_ZCL_ATTR_METERING_PREVIOUS_MONTH13_ALT_CONSUMPTION_RECEIVED,  /*!< ESP_ZB_ZCL_ATTR_METERING_PREVIOUS_MONTH13_ALT_CONSUMPTION_RECEIVED */

    /* 0x0C5C to 0xFFFF reserved */
} esp_zb_zcl_metering_attr_t;

/** @brief Values fot CurrentBlock attribute */
typedef enum {
    ESP_ZB_ZCL_METERING_BLOCK_NOT_USED = 0x00, /*!< No blocks in use */
    ESP_ZB_ZCL_METERING_BLOCK_1        = 0x01, /*!< Block 1 */
    ESP_ZB_ZCL_METERING_BLOCK_2        = 0x02, /*!< Block 2 */
    ESP_ZB_ZCL_METERING_BLOCK_3        = 0x03, /*!< Block 3 */
    ESP_ZB_ZCL_METERING_BLOCK_4        = 0x04, /*!< Block 4 */
    ESP_ZB_ZCL_METERING_BLOCK_5        = 0x05, /*!< Block 5 */
    ESP_ZB_ZCL_METERING_BLOCK_6        = 0x06, /*!< Block 6 */
    ESP_ZB_ZCL_METERING_BLOCK_7        = 0x07, /*!< Block 7 */
    ESP_ZB_ZCL_METERING_BLOCK_8        = 0x08, /*!< Block 8 */
    ESP_ZB_ZCL_METERING_BLOCK_9        = 0x09, /*!< Block 9 */
    ESP_ZB_ZCL_METERING_BLOCK_10       = 0x0A, /*!< Block 10 */
    ESP_ZB_ZCL_METERING_BLOCK_11       = 0x0B, /*!< Block 11 */
    ESP_ZB_ZCL_METERING_BLOCK_12       = 0x0C, /*!< Block 12 */
    ESP_ZB_ZCL_METERING_BLOCK_13       = 0x0D, /*!< Block 13 */
    ESP_ZB_ZCL_METERING_BLOCK_14       = 0x0E, /*!< Block 14 */
    ESP_ZB_ZCL_METERING_BLOCK_15       = 0x0F, /*!< Block 15 */
    ESP_ZB_ZCL_METERING_BLOCK_16       = 0x10, /*!< Block 16 */
    /* 0x11 to 0xFF reserved */
} esp_zb_zcl_metering_block_t;

/** @brief Values for SupplyStatus attribute */
typedef enum {
    ESP_ZB_ZCL_SUPPLY_STATUS_OFF   = 0x00, /*!< Supply OFF */
    ESP_ZB_ZCL_SUPPLY_STATUS_ARMED = 0x01, /*!< Supply OFF/ARMED */
    ESP_ZB_ZCL_SUPPLY_STATUS_ON    = 0x02  /*!< Supply ON */
    /* 0x03 to 0xFF reserved */
} esp_zb_zcl_metering_supply_status_t;

/** @brief Bit mapping of the Status attribute (Electricity) */
typedef enum {
    ESP_ZB_ZCL_METERING_ELECTRICITY_CHECK_METER             = 1 << 0, /*!< CheckMeter bit is set to true when a non fatal problem has been detected on the meter */
    ESP_ZB_ZCL_METERING_ELECTRICITY_LOW_BATTERY             = 1 << 1, /*!< LowBattery bit is set to true when the battery needs maintenance */
    ESP_ZB_ZCL_METERING_ELECTRICITY_TAMPER_DETECT           = 1 << 2, /*!< TamperDetect bit is set to true if a tamper event has been detected */
    ESP_ZB_ZCL_METERING_ELECTRICITY_POWER_FAILURE           = 1 << 3, /*!< PowerFailure bit is set to true during a power outage */
    ESP_ZB_ZCL_METERING_ELECTRICITY_POWER_QUALITY           = 1 << 4, /*!< PowerQuality bit is set to true if a power quality event has been detected such as a low voltage, high voltage */
    ESP_ZB_ZCL_METERING_ELECTRICITY_LEAK_DETECT             = 1 << 5, /*!< LeakDetect bit is set to true when a leak has been detected */
    ESP_ZB_ZCL_METERING_ELECTRICITY_SERVICE_DISCONNECT_OPEN = 1 << 6, /*!< ServiceDisconnectOpen bit is Set to true when the service has been disconnected to this premises */
    ESP_ZB_ZCL_METERING_ELECTRICITY_RESERVED                = 1 << 7  /*!< Reserved bit */
} esp_zb_zcl_metering_status_electricity_t;

/** @brief Bit mapping of the Status attribute (Gas) */
typedef enum {
    ESP_ZB_ZCL_METERING_GAS_CHECK_METER        = 1 << 0, /*!< CheckMeter bit is Set to true when a non fatal problem has been detected on the meter */
    ESP_ZB_ZCL_METERING_GAS_LOW_BATTERY        = 1 << 1, /*!< LowBattery bit is set to true when the battery needs maintenance */
    ESP_ZB_ZCL_METERING_GAS_TAMPER_DETECT      = 1 << 2, /*!< TamperDetect bit is set to true if a tamper event has been detected */
    ESP_ZB_ZCL_METERING_GAS_RESERVED           = 1 << 3, /*!< Reserved bit */
    ESP_ZB_ZCL_METERING_GAS_LOW_PRESSURE       = 1 << 4, /*!< LowPressure bit is set to true when the pressure at the meter is below the meter's low pressure threshold value */
    ESP_ZB_ZCL_METERING_GAS_LEAK_DETECT        = 1 << 5, /*!< LeakDetect bit is set to true when a leak has been detected */
    ESP_ZB_ZCL_METERING_GAS_SERVICE_DISCONNECT = 1 << 6, /*!< ServiceDisconnect bit is set to true when the service has been disconnected to this premises */
    ESP_ZB_ZCL_METERING_GAS_REVERSE_FLOW       = 1 << 7  /*!< ReverseFlow bit is set to true if flow detected in the opposite direction to normal (from consumer to supplier) */
} esp_zb_zcl_metering_status_gas_t;

/** @brief Bit mapping of the Status attribute (Water) */
typedef enum {
    ESP_ZB_ZCL_METERING_WATER_CHECK_METER        = 1 << 0, /*!< CheckMeter bit is Set to true when a non fatal problem has been detected on the meter */
    ESP_ZB_ZCL_METERING_WATER_LOW_BATTERY        = 1 << 1, /*!< LowBattery bit is set to true when the battery needs maintenance */
    ESP_ZB_ZCL_METERING_WATER_TAMPER_DETECT      = 1 << 2, /*!< TamperDetect bit set to true if a tamper event has been detected */
    ESP_ZB_ZCL_METERING_WATER_PIPE_EMTPY         = 1 << 3, /*!< PipeEmpty bit is set to true when the service pipe at the meter is empty and there is no flow in either direction */
    ESP_ZB_ZCL_METERING_WATER_LOW_PRESSURE       = 1 << 4, /*!< LowPressure bit is Set to true when the pressure at the meter is below the meter's low pressure threshold value */
    ESP_ZB_ZCL_METERING_WATER_LEAK_DETECT        = 1 << 5, /*!< LeakDetect bit is set to true when a leak has been detected */
    ESP_ZB_ZCL_METERING_WATER_SERVICE_DISCONNECT = 1 << 6, /*!< ServiceDisconnect bit is set to true when the service has been disconnected to this premises */
    ESP_ZB_ZCL_METERING_WATER_REVERSE_FLOW       = 1 << 7  /*!< ReverseFlow bit is Set to true if flow detected in the opposite direction to normal (from consumer to supplier) */
} esp_zb_zcl_metering_status_water_t;

/** @brief Bit mapping of the Status attribute (Heat and Cooling) */
typedef enum {
    ESP_ZB_ZCL_METERING_HCOOL_CHECK_METER        = 1 << 0, /*!< CheckMeter bit is Set to true when a non fatal problem has been detected on the meter */
    ESP_ZB_ZCL_METERING_HCOOL_LOW_BATTERY        = 1 << 1, /*!< LowBattery bit is set to true when the battery needs maintenance */
    ESP_ZB_ZCL_METERING_HCOOL_TAMPER_DETECT      = 1 << 2, /*!< TamperDetect bit is set to true if a tamper event has been detected */
    ESP_ZB_ZCL_METERING_HCOOL_TEMPERATURE_SENSOR = 1 << 3, /*!< TemperatureSensor bit is Set to true when an error is detected on a temperature sensor at this premises */
    ESP_ZB_ZCL_METERING_HCOOL_BURST_DETECT       = 1 << 4, /*!< BurstDetect bit is set to true when a burst is detected on pipes at this premises */
    ESP_ZB_ZCL_METERING_HCOOL_LEAK_DETECT        = 1 << 5, /*!< LeakDetect bit is set to true when a leak has been detected */
    ESP_ZB_ZCL_METERING_HCOOL_SERVICE_DISCONNECT = 1 << 6, /*!< ServiceDisconnect bit is Set to true when the service has been disconnected to this premises */
    ESP_ZB_ZCL_METERING_HCOOL_FLOW_SENSOR        = 1 << 7  /*!< FlowSensor bit is set to true when an error is detected on a flow sensor at this premises */
} esp_zb_zcl_metering_status_heat_cooling_t;

/** @brief Bit mapping of the ExtendedStatus attribute (General Flags) */
typedef enum {
    ESP_ZB_ZCL_METERING_METER_COVER_REMOVED            = 1 << 0, /*!< MeterCoverRemoved bit is set to true when the device detects the meter cover being removed */
    ESP_ZB_ZCL_METERING_STRONG_MAGNETIC_FIELD_DETECTED = 1 << 1, /*!< StrongMagneticFieldDetected bit is set to true when the device detects presence of a strong magnetic field */
    ESP_ZB_ZCL_METERING_BATTERY_FAILURE                = 1 << 2, /*!< BatteryFailure bit is set to true when the device detects that its battery has failed */
    ESP_ZB_ZCL_METERING_PROGRAM_MEMORY_ERROR           = 1 << 3, /*!< ProgramMemoryError bit is set to true when the device detects an error within its program (non-volatile) memory */
    ESP_ZB_ZCL_METERING_RAM_ERROR                      = 1 << 4, /*!< RAMError bit is set to true when the device detects an instance of a Random Access Memory error within the device memory */
    ESP_ZB_ZCL_METERING_NV_MEMORY_ERROR                = 1 << 5, /*!< NVMemoryError bit is set to true when the device detects an instance of a Non Volatile memory error within the device memory
                                                                  *   this is a fatal meter error that will require the meter replacement
                                                                  */
    ESP_ZB_ZCL_METERING_MEASUREMENT_SYSTEM_ERROR = 1 << 6,       /*!< MeasurementSystemError bit is set to true when the device detects an error within its measurement system */
    ESP_ZB_ZCL_METERING_WATCHDOG_ERROR           = 1 << 7,       /*!< WatchdogError bit is set to true when the device has detected an instance
                                                                  *   of a watchdog reset event (following a catastrophic fault within the device)
                                                                  */
    ESP_ZB_ZCL_METERING_SUPPLY_DISCONNECT_FAILURE = 1 << 8,      /*!< SupplyDisconnectFailure bit is set to true when the device has detected
                                                                  *   that the valve has not closed as expected (for gas) or the contactor has not opened as expected (for electricity)
                                                                  */
    ESP_ZB_ZCL_METERING_SUPPLY_CONNECT_FAILURE = 1 << 9,         /*!< SupplyConnectFailure bit is set to true when the device has detected that
                                                                  *   the valve has not opened as expected (for gas) or the contactor has not closed as expected (for electricity)
                                                                  */
    ESP_ZB_ZCL_METERING_MEASUREMENT_SW_CHANGED = 1 << 10,        /*!< MeasurementSWChanged/Tampered bit is set to true when the device detects that its measurement software has changed */
    ESP_ZB_ZCL_METERING_CLOCK_INVALID          = 1 << 11,        /*!< ClockInvalid bit is set to true when the device detects that its internal clock is invalid */
    ESP_ZB_ZCL_METERING_TEMPERATURE_EXCEEDED   = 1 << 12,        /*!< TemperatureExceeded bit is set to true when the metering device's temperature
                                                                  *   exceeds a predefined limit. There are various reasons for temperature rise in metering devices
                                                                  */
    ESP_ZB_ZCL_METERING_MOISTURE_DETECTED = 1 << 13              /*!< MoistureDetected bit is set to true when a sensor has detected the presence of moisture
                                                                  *   e.g. moisture in a gas line which can cause a drop in gas pressure, or moisture detected in the sealed component area within a water meter
                                                                  */
    /* bits 14-23 reserved */
} esp_zb_zcl_metering_extstatus_general_t;

/** @brief Bit mapping of the ExtendedStatus attribute (Electricity Meter specific Flags) */
typedef enum {
    ESP_ZB_ZCL_METERING_ELECTRICITY_TERMINAL_COVER_REMOVED  = 1L << 24,  /*!< TerminalCoverRemoved bit is set to true when the device detects that its terminal cover has been removed */
    ESP_ZB_ZCL_METERING_ELECTRICITY_INCORRECT_POLARITY      = 1L << 25,  /*!< IncorrectPolarity bit is set to true when the electricity meter detects incorrect polarity on the electricity supply */
    ESP_ZB_ZCL_METERING_ELECTRICITY_CURRENT_WITH_NO_VOLTAGE = 1L << 26,  /*!< CurrentWithNoVoltage bit is set to true when the meter has been tampered
                                                                          *   with, to disconnect the measurement function from the supply. Electricity is still flowing but not being recorded
                                                                          */
    ESP_ZB_ZCL_METERING_ELECTRICITY_LIMIT_THRESHOLD_EXCEEDED = 1L << 27, /*!< LimitThresholdExceeded bit is set to true when the electricity meter detects
                                                                          *   that the load has exceeded the load limit threshold
                                                                          */
    ESP_ZB_ZCL_METERING_ELECTRICITY_UNDER_VOLTAGE = 1L << 28,            /*!< UnderVoltage bit is set to true when the electricity meter indicates that
                                                                          *   the voltage measurement over the voltage measurement period is lower than the voltage threshold
                                                                          */
    ESP_ZB_ZCL_METERING_ELECTRICITY_OVER_VOLTAGE = 1L << 29,             /*!< OverVoltage bit is set to true when the electricity meter indicates that the
                                                                          *   voltage measurement over the voltage measurement period is higher than the voltage threshold
                                                                          */
    ESP_ZB_ZCL_METERING_ELECTRICITY_BIDIRECTION_OPERATION = 1L << 30,    /*!<Set to true when the meter is capable of measuring energy in both directions (i.e. delivered and received) */
    /* Each typedef enumerator that appears in the body of an typedef enumeration specifier becomes an integer constant with type int in the enclosing scope */
    ESP_ZB_ZCL_METERING_ELECTRICITY_ACTIVE_POWER_RECEIVED = -2147483648LL /*!< Set to true when active power flows in the received direction (generation from the premises to the grid) */
    /* bits 32-63 reserved */
} esp_zb_zcl_metering_extstatus_electricity_t;

/** @brief Bit mapping of the ExtendedStatus attribute (Gas Meter specific Flags) */
typedef enum {
    ESP_ZB_ZCL_METERING_GAS_BATTERY_COVER_REMOVED = 1L << 24, /*!< BatteryCoverRemoved bit is set to true when the gas meter detects that its battery cover has been removed */
    ESP_ZB_ZCL_METERING_GAS_TILT_TAMPER           = 1L << 25, /*!< TiltTamper bit is set to true when the meter detects a change in its physical
                                                               *   properties (i.e. that it is being tilted, the tilt sensor has been activated or otherwise tampered with)
                                                               */
    ESP_ZB_ZCL_METERING_GAS_EXCESS_FLOW = 1L << 26            /*!< ExcessFlow bit is set to true when the gas meter detects excess flow (e.g. when local supply restoration is attempted) */
    /* bits 26-63 reserved */
} esp_zb_zcl_metering_extstatus_gas_t;

/** @brief Values for LowMediumHighStatus Attribute */
typedef enum {
    ESP_ZB_ZCL_METERING_STATUS_LOW    = 0x00, /*!< Low energy usage */
    ESP_ZB_ZCL_METERING_STATUS_MEDIUM = 0x01, /*!< Medium energy usage */
    ESP_ZB_ZCL_METERING_STATUS_HIGH   = 0x02, /*!< High energy usage */
} esp_zb_zcl_metering_low_medium_high_status_t;

/** @brief Values for UnitOfMeasure Attribute */
typedef enum {
    ESP_ZB_ZCL_METERING_UNIT_KW_KWH_BINARY       = 0x00, /*!< kW, kWh binary value */
    ESP_ZB_ZCL_METERING_UNIT_M3_M3H_BINARY       = 0x01, /*!< m3, m3/h binary value */
    ESP_ZB_ZCL_METERING_UNIT_FT3_FT3H_BINARY     = 0x02, /*!< ft3, ft3/h binary value */
    ESP_ZB_ZCL_METERING_UNIT_CCF_CCFH_BINARY     = 0x03, /*!< ccf, ccf/h binary value */
    ESP_ZB_ZCL_METERING_UNIT_USGL_USGLH_BINARY   = 0x04, /*!< US gl, US gl/h binary value */
    ESP_ZB_ZCL_METERING_UNIT_IMPGL_IMPGLH_BINARY = 0x05, /*!< IMP gl, IMP gl/h binary value */
    ESP_ZB_ZCL_METERING_UNIT_BTU_BTUH_BINARY     = 0x06, /*!< BTU, BTU/h binary value */
    ESP_ZB_ZCL_METERING_UNIT_L_LH_BINARY         = 0x07, /*!< l, l/h binary value */
    ESP_ZB_ZCL_METERING_UNIT_KPAG_BINARY         = 0x08, /*!< kPA (gauge) binary value */
    ESP_ZB_ZCL_METERING_UNIT_KPAA_BINARY         = 0x09, /*!< kPA (absolute) binary value */
    ESP_ZB_ZCL_METERING_UNIT_MCF_MCFH_BINARY     = 0x0A, /*!< mcf, mcf/h binary value */
    ESP_ZB_ZCL_METERING_UNIT_UNITLESS_BINARY     = 0x0B, /*!< Unitless binary value */
    ESP_ZB_ZCL_METERING_UNIT_MJ_MJS_BINARY       = 0x0C, /*!< MJ, MJ/h binary value */
    ESP_ZB_ZCL_METERING_UNIT_KVAR_KVARH_BINARY   = 0x0D, /*!< kVar, kVar/h binary value */

    /* 0x0E to 0x7F reserved for future use */

    ESP_ZB_ZCL_METERING_UNIT_KW_KWH_BCD       = 0x80, /*!< kW, kWh BCD value */
    ESP_ZB_ZCL_METERING_UNIT_M3_M3H_BCD       = 0x81, /*!< m3, m3/h BCD value */
    ESP_ZB_ZCL_METERING_UNIT_FT3_FT3H_BCD     = 0x82, /*!< ft3, ft3/h BCD value */
    ESP_ZB_ZCL_METERING_UNIT_CCF_CCFH_BCD     = 0x83, /*!< ccf, ccf/h BCD value */
    ESP_ZB_ZCL_METERING_UNIT_USGL_USGLH_BCD   = 0x84, /*!< US gl, US gl/h BCD value */
    ESP_ZB_ZCL_METERING_UNIT_IMPGL_IMPGLH_BCD = 0x85, /*!< IMP gl, IMP gl/h BCD value */
    ESP_ZB_ZCL_METERING_UNIT_BTU_BTUH_BCD     = 0x86, /*!< BTU, BTU/h BCD value */
    ESP_ZB_ZCL_METERING_UNIT_L_LH_BCD         = 0x87, /*!< l, l/h BCD value */
    ESP_ZB_ZCL_METERING_UNIT_KPAG_BCD         = 0x88, /*!< kPA (gauge) BCD value */
    ESP_ZB_ZCL_METERING_UNIT_KPAA_BCD         = 0x89, /*!< kPA (absolute) BCD value */
    ESP_ZB_ZCL_METERING_UNIT_MCF_MCFH_BCD     = 0x8A, /*!< mcf, mcf/h BCD value */
    ESP_ZB_ZCL_METERING_UNIT_UNITLESS_BCD     = 0x8B, /*!< Unitless BCD value */
    ESP_ZB_ZCL_METERING_UNIT_MJ_MJS_BCD       = 0x8C, /*!< MJ, MJ/h BCD value */
    ESP_ZB_ZCL_METERING_UNIT_KVAR_KVARH_BCD   = 0x8D, /*!< kVar, kVar/h BCD value */

    ESP_ZB_ZCL_METERING_UNIT_BCD_RESERVED = 0x8E 
    /*!< 0x8E to 0xFF reserved */
} esp_zb_zcl_metering_unit_of_measure_t;

/** @brief Values for DeviceType Attribute */
typedef enum {
    ESP_ZB_ZCL_METERING_ELECTRIC_METERING                   = 0,  /*!< Electric metering device */
    ESP_ZB_ZCL_METERING_GAS_METERING                        = 1,  /*!< Gas metering device */
    ESP_ZB_ZCL_METERING_WATER_METERING                      = 2,  /*!< Water metering device */
    ESP_ZB_ZCL_METERING_PRESSURE_METERING                   = 4,  /*!< Pressure metering device */
    ESP_ZB_ZCL_METERING_HEAT_METERING                       = 5,  /*!< Heat metering device */
    ESP_ZB_ZCL_METERING_COOLING_METERING                    = 6,  /*!< Cooling metering device */
    ESP_ZB_ZCL_METERING_EUMD                                = 7,  /*!< End Use Measurement Device (EUMD) for metering electric vehicle charging */
    ESP_ZB_ZCL_METERING_PV_GENERATION_METERING              = 8,  /*!< PV Generation Metering device */
    ESP_ZB_ZCL_METERING_WIND_TURBINE_GENERATION_METERING    = 9,  /*!< Wind Turbine Generation Metering device */
    ESP_ZB_ZCL_METERING_WATER_TURBINE_GENERATION_METERING   = 10, /*!< Water Turbine Generation Metering device */
    ESP_ZB_ZCL_METERING_MICRO_GENERATION_METERING           = 11, /*!< Micro Generation Metering device */
    ESP_ZB_ZCL_METERING_SOLAR_HOT_WATER_GENERATION_METERING = 12, /*!< Solar Hot Water Generation Metering device */
    ESP_ZB_ZCL_METERING_ELECTRIC_METERING_ELEMENT_PHASE1    = 13, /*!< Electric Metering Element/Phase 1 */
    ESP_ZB_ZCL_METERING_ELECTRIC_METERING_ELEMENT_PHASE2    = 14, /*!< Electric Metering Element/Phase 2 */
    ESP_ZB_ZCL_METERING_ELECTRIC_METERING_ELEMENT_PHASE3    = 15, /*!< Electric Metering Element/Phase 3 */

    /* 16 to 126 reserved */

    ESP_ZB_ZCL_METERING_MIRRORED_ELECTRIC_METERING = 127, /*!< Mirrored Electric Metering device */
    ESP_ZB_ZCL_METERING_MIRRORED_GAS_METERING      = 128, /*!< Mirrored Gas Metering device */
    ESP_ZB_ZCL_METERING_MIRRORED_WATER_METERING    = 129, /*!< Mirrored Water Metering device */
    /* 130 == Mirrored Thermal Metering (deprecated) */
    ESP_ZB_ZCL_METERING_MIRRORED_PRESSURE_METERING                   = 131, /*!< Mirrored Pressure Metering device */
    ESP_ZB_ZCL_METERING_MIRRORED_HEAT_METERING                       = 132, /*!< Mirrored Heat Metering device */
    ESP_ZB_ZCL_METERING_MIRRORED_COOLING_METERING                    = 133, /*!< Mirrored Cooling Metering device */
    ESP_ZB_ZCL_METERING_MIRRORED_EUMD                                = 134, /*!< Mirrored End Use Measurement Device (EUMD) for metering electric vehicle charging */
    ESP_ZB_ZCL_METERING_MIRRORED_PV_GENERATION_METERING              = 135, /*!< Mirrored PV Generation Metering device */
    ESP_ZB_ZCL_METERING_MIRRORED_WIND_TURBINE_GENERATION_METERING    = 136, /*!< Mirrored Wind Turbine Generation Metering device */
    ESP_ZB_ZCL_METERING_MIRRORED_WATER_TURBINE_GENERATION_METERING   = 137, /*!< Mirrored Water Turbine Generation Metering device */
    ESP_ZB_ZCL_METERING_MIRRORED_MICRO_GENERATION_METERING           = 138, /*!< Mirrored Micro Generation Metering device */
    ESP_ZB_ZCL_METERING_MIRRORED_SOLAR_HOT_WATER_GENERATION_METERING = 139, /*!< Mirrored Solar Hot Water Generation Metering device */
    ESP_ZB_ZCL_METERING_MIRRORED_ELECTRIC_METERING_ELEMENT_PHASE1    = 140, /*!< Mirrored Electric Metering Element/Phase 1 */
    ESP_ZB_ZCL_METERING_MIRRORED_ELECTRIC_METERING_ELEMENT_PHASE2    = 141, /*!< Mirrored Electric Metering Element/Phase 2 */
    ESP_ZB_ZCL_METERING_MIRRORED_ELECTRIC_METERING_ELEMENT_PHASE3    = 142, /*!< Mirrored Electric Metering Element/Phase 3 */

    /* 143 to 255 reserved */

    ESP_ZB_ZCL_METERING_RESERVED = 143
} esp_zb_zcl_metering_device_type_t;

/** @brief Values for TemperatureUnitOfMeasure Attribute */
typedef enum {
    ESP_ZB_ZCL_METERING_TEMPERATURE_UNIT_KELVIN_BINARY     = 0x00, /*!< K (Degrees Kelvin) in pure Binary format */
    ESP_ZB_ZCL_METERING_TEMPERATURE_UNIT_CELSIUS_BINARY    = 0x01, /*!< C (Degrees Celsius) in pure Binary format */
    ESP_ZB_ZCL_METERING_TEMPERATURE_UNIT_FAHRENHEIT_BINARY = 0x02, /*!< F (Degrees Fahrenheit) in pure Binary format */

    /* 0x03 - 0x7F reserved */

    ESP_ZB_ZCL_METERING_TEMPERATURE_UNIT_KELVIN_BCD     = 0x80, /*!< K (Degrees Kelvin) in BCD format */
    ESP_ZB_ZCL_METERING_TEMPERATURE_UNIT_CELSIUS_BCD    = 0x81, /*!< C (Degrees Celsius) in BCD format */
    ESP_ZB_ZCL_METERING_TEMPERATURE_UNIT_FAHRENHEIT_BCD = 0x82, /*!< F (Degrees Fahrenheit) in BCD format */
    /* 0x83 - 0xFF reserved */
} esp_zb_zcl_metering_temperature_unit_of_measure_t;

/** @brief The generic Alarm Group maps the status from the MeterStatus attribute into a corresponding alarm */
typedef enum {
    ESP_ZB_ZCL_METERING_ALARM_CHECK_METER                = 0x00, /*!< Check meter */
    ESP_ZB_ZCL_METERING_ALARM_LOW_BATTERY                = 0x01, /*!< Low battery */
    ESP_ZB_ZCL_METERING_ALARM_TAMPER_DETECT              = 0x02, /*!< Tamper detect */
    ESP_ZB_ZCL_METERING_ALARM_ELECTRICITY_POWER_FAILURE  = 0x03, /*!< Electricity: Power Failure */
    ESP_ZB_ZCL_METERING_ALARM_WATER_PIPE_EMPTY           = 0x03, /*!< Water: Pipe Empty */
    ESP_ZB_ZCL_METERING_ALARM_HEAT_TEMPERATURE_SENSOR    = 0x03, /*!< Heat: Temperature Sensor */
    ESP_ZB_ZCL_METERING_ALARM_COOLING_TEMPERATURE_SENSOR = 0x03, /*!< Cooling Temperature Sensor */
    ESP_ZB_ZCL_METERING_ALARM_ELECTRICITY_POWER_QUALITY  = 0x04, /*!< Electricity: Power Quality */
    ESP_ZB_ZCL_METERING_ALARM_GAS_LOW_PRESSURE           = 0x04, /*!< Gas: Low Pressure */
    ESP_ZB_ZCL_METERING_ALARM_WATER_LOW_PRESSURE         = 0x04, /*!< Water: Low Pressure */
    ESP_ZB_ZCL_METERING_ALARM_HEAT_BURST_DETECT          = 0x04, /*!< Heat: Burst Detect */
    ESP_ZB_ZCL_METERING_ALARM_COOLING_BURST_DETECT       = 0x04, /*!< Cooling: Burst Detect */
    ESP_ZB_ZCL_METERING_ALARM_LEAK_DETECT                = 0x05, /*!< Leak detect */
    ESP_ZB_ZCL_METERING_ALARM_SERVICE_DISCONNECT         = 0x06, /*!< Service Disconnect */
    ESP_ZB_ZCL_METERING_ALARM_ELECTRICITY_RESERVED       = 0x07, /*!< Electricity: Reserved */
    ESP_ZB_ZCL_METERING_ALARM_GAS_REVERSE_FLOW           = 0x07, /*!< Gas: Reverse Flow */
    ESP_ZB_ZCL_METERING_ALARM_WATER_REVERSE_FLOW         = 0x07, /*!< Water: Reverse Flow */
    ESP_ZB_ZCL_METERING_ALARM_HEAT_FLOW_SENSOR           = 0x07, /*!< Heat: Flow Sensor */
    ESP_ZB_ZCL_METERING_ALARM_COOLING_FLOW_SENSOR        = 0x07, /*!< Cooling: Flow Sensor */
    ESP_ZB_ZCL_METERING_ALARM_METER_COVER_REMOVED        = 0x08, /*!< Meter Cover Removed */
    ESP_ZB_ZCL_METERING_ALARM_METER_COVER_CLOSED         = 0x09, /*!< Meter Cover Closed */
    ESP_ZB_ZCL_METERING_ALARM_STRONG_MAGNETIC_FIELD      = 0x0A, /*!< Strong Magnetic Field */
    ESP_ZB_ZCL_METERING_ALARM_NO_STRONG_MAGNETIC_FIELD   = 0x0B, /*!< No Strong Magnetic Field */
    ESP_ZB_ZCL_METERING_ALARM_BATTERY_FAILURE            = 0x0C, /*!< Battery Failure */
    ESP_ZB_ZCL_METERING_ALARM_PROGRAM_MEMORY_ERROR       = 0x0D, /*!< Program Memory Error */
    ESP_ZB_ZCL_METERING_ALARM_RAM_ERROR                  = 0x0E, /*!< Ram Error */
    ESP_ZB_ZCL_METERING_ALARM_NV_MEMORY_ERROR            = 0x0F  /*!< NV Memory Error */
} esp_zb_zcl_metering_generic_alarm_group_t;

/** @brief The Electricity Alarm Group defines alarms specific for electricity meters */
typedef enum {
    ESP_ZB_ZCL_METERING_ALARM_LOW_VOLTAGE_L1         = 0x10, /*!< Low Voltage L1 */
    ESP_ZB_ZCL_METERING_ALARM_HIGH_VOLTAGE_L1        = 0x11, /*!< High Voltage L1 */
    ESP_ZB_ZCL_METERING_ALARM_LOW_VOLTAGE_L2         = 0x12, /*!< Low Voltage L2 */
    ESP_ZB_ZCL_METERING_ALARM_HIGH_VOLTAGE_L2        = 0x13, /*!< High Voltage L2 */
    ESP_ZB_ZCL_METERING_ALARM_LOW_VOLTAGE_L3         = 0x14, /*!< Low Voltage L3 */
    ESP_ZB_ZCL_METERING_ALARM_HIGH_VOLTAGE_L3        = 0x15, /*!< High Voltage L3 */
    ESP_ZB_ZCL_METERING_ALARM_OVER_CURRENT_L1        = 0x16, /*!< Over Current L1 */
    ESP_ZB_ZCL_METERING_ALARM_OVER_CURRENT_L2        = 0x17, /*!< Over Current L2 */
    ESP_ZB_ZCL_METERING_ALARM_OVER_CURRENT_L3        = 0x18, /*!< Over Current L3 */
    ESP_ZB_ZCL_METERING_ALARM_FREQUENCY_TOO_LOW_L1   = 0x19, /*!< Frequency too Low L1 */
    ESP_ZB_ZCL_METERING_ALARM_FREQUENCY_TOO_HIGH_L1  = 0x1A, /*!< Frequency too High L1 */
    ESP_ZB_ZCL_METERING_ALARM_FREQUENCY_TOO_LOW_L2   = 0x1B, /*!< Frequency too Low L2 */
    ESP_ZB_ZCL_METERING_ALARM_FREQUENCY_TOO_HIGH_L2  = 0x1C, /*!< Frequency too High L2 */
    ESP_ZB_ZCL_METERING_ALARM_FREQUENCY_TOO_LOW_L3   = 0x1D, /*!< Frequency too Low L3 */
    ESP_ZB_ZCL_METERING_ALARM_FREQUENCY_TOO_HIGH_L3  = 0x1E, /*!< Frequency too High L3 */
    ESP_ZB_ZCL_METERING_ALARM_GROUND_FAULT           = 0x1F, /*!< Ground Fault */
    ESP_ZB_ZCL_METERING_ALARM_ELECTRIC_TAMPER_DETECT = 0x20, /*!< Electric Tamper Detect */
    ESP_ZB_ZCL_METERING_ALARM_INCORRECT_POLARITY     = 0x21, /*!< Incorrect Polarity */
    ESP_ZB_ZCL_METERING_ALARM_CURRENT_NO_VOLTAGE     = 0x22, /*!< Current No Voltage */
    ESP_ZB_ZCL_METERING_ALARM_UNDER_VOLTAGE          = 0x23, /*!< Under Voltage */
    ESP_ZB_ZCL_METERING_ALARM_OVER_VOLTAGE           = 0x24, /*!< Over Voltage */
    ESP_ZB_ZCL_METERING_ALARM_NORMAL_VOLTAGE         = 0x25, /*!< Normal Voltage */
    ESP_ZB_ZCL_METERING_ALARM_PF_BELOW_THRESHOLD     = 0x26, /*!< PF Below Threshold */
    ESP_ZB_ZCL_METERING_ALARM_PF_ABOVE_THRESHOLD     = 0x27, /*!< PF Above Threshold */
    ESP_ZB_ZCL_METERING_ALARM_TERMINAL_COVER_REMOVED = 0x28, /*!< Terminal Cover Removed */
    ESP_ZB_ZCL_METERING_ALARM_TERMINAL_COVER_CLOSED  = 0x29, /*!< Terminal Cover Closed */
} esp_zb_zcl_metering_electricity_alarm_group_t;

/** @brief The Generic Flow/Pressure Alarm Group defines alarms specific for Flow/Pressure based meters, i.e. Water, Heat, Cooling, or Gas meters */
typedef enum {
    ESP_ZB_ZCL_METERING_ALARM_BURST_DETECT                    = 0x30, /*!< Burst detect */
    ESP_ZB_ZCL_METERING_ALARM_PRESSURE_TOO_LOW                = 0x31, /*!< Pressure too low */
    ESP_ZB_ZCL_METERING_ALARM_PRESSURE_TOO_HIGH               = 0x32, /*!< Pressure too high */
    ESP_ZB_ZCL_METERING_ALARM_FLOW_SENSOR_COMMUNICATION_ERROR = 0x33, /*!< Flow sensor communication error */
    ESP_ZB_ZCL_METERING_ALARM_FLOW_SENSOR_MEASUREMENT_FAULT   = 0x34, /*!< Flow sensor measurement fault */
    ESP_ZB_ZCL_METERING_ALARM_FLOW_SENSOR_REVERSE_FLOW        = 0x35, /*!< Flow sensor reverse flow */
    ESP_ZB_ZCL_METERING_ALARM_FLOW_SENSOR_AIR_DETECT          = 0x36, /*!< Flow sensor air detect */
    ESP_ZB_ZCL_METERING_ALARM_PIPE_EMPTY                      = 0x37, /*!< Pipe empty */
} esp_zb_zcl_metering_generic_flow_pressure_alarm_group_t;

/** @brief The Heat and Cooling Specific Alarm Group defines alarms specific for Heat or Cooling meters */
typedef enum {
    ESP_ZB_ZCL_METERING_ALARM_INLET_TEMPERATURE_SENSOR_FAULT  = 0x50, /*!< Inlet Temperature Sensor Fault */
    ESP_ZB_ZCL_METERING_ALARM_OUTLET_TEMPERATURE_SENSOR_FAULT = 0x51, /*!< Outlet Temperature Sensor Fault */
} esp_zb_zcl_metering_heat_and_cooling_specific_alarm_group_t;

/** @brief The Gas Specific Alarm Group defines alarms specific for Gas meters */
typedef enum {
    ESP_ZB_ZCL_METERING_ALARM_TILT_TAMPER           = 0x60, /*!< Tilt Tamper */
    ESP_ZB_ZCL_METERING_ALARM_BATTERY_COVER_REMOVED = 0x61, /*!< Battery Cover Removed */
    ESP_ZB_ZCL_METERING_ALARM_BATTERY_COVER_CLOSED  = 0x62, /*!< Battery Cover Closed */
    ESP_ZB_ZCL_METERING_ALARM_EXCESS_FLOW           = 0x63, /*!< Excess Flow */
    ESP_ZB_ZCL_METERING_ALARM_TILT_TAMPER_ENDED     = 0x64, /*!< Tilt Tamper Ended */
} esp_zb_zcl_metering_gas_specific_alarm_group_t;

/** @brief The Extended Generic Alarm Group is an additional set of generic meter alarms */
typedef enum {
    ESP_ZB_ZCM_METERING_ALARM_MEASUREMENT_SYSTEM_ERROR                     = 0x70, /*!< Measurement System Error */
    ESP_ZB_ZCM_METERING_ALARM_WATCHDOG_ERROR                               = 0x71, /*!< Watchdog Error */
    ESP_ZB_ZCM_METERING_ALARM_SUPPLY_DISCONNECT_FAILURE                    = 0x72, /*!< Supply Disconnect Failure */
    ESP_ZB_ZCM_METERING_ALARM_SUPPLY_CONNECT_FAILURE                       = 0x73, /*!< Supply Connect Failure */
    ESP_ZB_ZCM_METERING_ALARM_MEASUREMENT_SOFTWARE_CHANGED                 = 0x74, /*!< Measurement Software Changed */
    ESP_ZB_ZCM_METERING_ALARM_DST_ENABLED                                  = 0x75, /*!< DST enabled */
    ESP_ZB_ZCM_METERING_ALARM_DST_DISABLED                                 = 0x76, /*!< DST disabled */
    ESP_ZB_ZCM_METERING_ALARM_CLOCK_ADJ_BACKWARD                           = 0x77, /*!< Clock Adj Backward (the internal clock has applied a negative adjustment) */
    ESP_ZB_ZCM_METERING_ALARM_CLOCK_ADJ_FORWARD                            = 0x78, /*!< Clock Adj Forward (the internal clock has applied a positive adjustment) */
    ESP_ZB_ZCM_METERING_ALARM_CLOCK_INVALID                                = 0x79, /*!< Clock Invalid */
    ESP_ZB_ZCM_METERING_ALARM_COMMUNICATION_ERROR_HAN                      = 0x7A, /*!< Communication Error HAN */
    ESP_ZB_ZCM_METERING_ALARM_COMMUNICATION_OK_HAN                         = 0x7B, /*!< Communication OK HAN */
    ESP_ZB_ZCM_METERING_ALARM_METER_FRAUD_ATTEMPT                          = 0x7C, /*!< Meter Fraud Attempt */
    ESP_ZB_ZCM_METERING_ALARM_POWER_LOSS                                   = 0x7D, /*!< Power Loss */
    ESP_ZB_ZCM_METERING_ALARM_UNUSUAL_HAN_TRAFFIC                          = 0x7E, /*!< Unusual HAN Traffic */
    ESP_ZB_ZCM_METERING_ALARM_UNEXPECTED_CLOCK_CHANGE                      = 0x7F, /*!< Unexpected Clock Change */
    ESP_ZB_ZCM_METERING_ALARM_COMMS_USING_UNAUTHENTICATED_COMPONENT        = 0x80, /*!< Comms Using Unauthenticated Component */
    ESP_ZB_ZCM_METERING_ALARM_ERROR_REG_CLEAR                              = 0x81, /*!< Error Reg Clear */
    ESP_ZB_ZCM_METERING_ALARM_ALARM_REG_CLEAR                              = 0x82, /*!< Alarm Reg Clear */
    ESP_ZB_ZCM_METERING_ALARM_UNEXPECTED_HW_RESET                          = 0x83, /*!< Unexpected HW Reset */
    ESP_ZB_ZCM_METERING_ALARM_UNEXPECTED_PROGRAM_EXECUTION                 = 0x84, /*!< Unexpected Program Execution */
    ESP_ZB_ZCM_METERING_ALARM_EVENTLOG_CLEARED                             = 0x85, /*!< EventLog Cleared */
    ESP_ZB_ZCM_METERING_ALARM_LIMIT_THRESHOLD_EXCEEDED                     = 0x86, /*!< Limit Threshold Exceeded */
    ESP_ZB_ZCM_METERING_ALARM_LIMIT_THRESHOLD_OK                           = 0x87, /*!< Limit Threshold OK */
    ESP_ZB_ZCM_METERING_ALARM_LIMIT_THRESHOLD_CHANGED                      = 0x88, /*!< Limit Threshold Changed */
    ESP_ZB_ZCM_METERING_ALARM_MAXIMUM_DEMAND_EXCEEDED                      = 0x89, /*!< Maximum Demand Exceeded */
    ESP_ZB_ZCM_METERING_ALARM_PROFILE_CLEARED                              = 0x8A, /*!< Profile Cleared */
    ESP_ZB_ZCM_METERING_ALARM_SAMPLING_BUFFER_CLEARED                      = 0x8B, /*!< Sampling Buffer cleared */
    ESP_ZB_ZCM_METERING_ALARM_BATTERY_WARNING                              = 0x8C, /*!< Battery Warning */
    ESP_ZB_ZCM_METERING_ALARM_WRONG_SIGNATURE                              = 0x8D, /*!< Wrong Signature */
    ESP_ZB_ZCM_METERING_ALARM_NO_SIGNATURE                                 = 0x8E, /*!< No Signature */
    ESP_ZB_ZCM_METERING_ALARM_UNAUTHORISED_ACTION_FROM_HAN                 = 0x8F, /*!< Unauthorised Action from HAN */
    ESP_ZB_ZCM_METERING_ALARM_FAST_POLLING_START                           = 0x90, /*!< Fast Polling Start */
    ESP_ZB_ZCM_METERING_ALARM_FAST_POLLING_END                             = 0x91, /*!< Fast Polling End */
    ESP_ZB_ZCM_METERING_ALARM_METER_REPORTING_INTERVAL_CHANGED             = 0x92, /*!< Meter Reporting Interval Changed */
    ESP_ZB_ZCM_METERING_ALARM_DISCONNECT_DUE_TO_LOAD_LIMIT                 = 0x93, /*!< Disconnect Due to Load Limit */
    ESP_ZB_ZCM_METERING_ALARM_METER_SUPPLY_STATUS_REGISTER_CHANGED         = 0x94, /*!< Meter Supply Status Register Changed */
    ESP_ZB_ZCM_METERING_ALARM_METER_ALARM_STATUS_REGISTER_CHANGED          = 0x95, /*!< Meter Alarm Status Register Changed */
    ESP_ZB_ZCM_METERING_ALARM_EXTENDED_METER_ALARM_STATUS_REGISTER_CHANGED = 0x96, /*!< Extended Meter Alarm Status Register Changed */
} esp_zb_zcl_metering_extended_generic_alarm_group_t;

/** @brief The Manufacturer Specific Alarm Group defines alarms specific for any meters. These are used
 * for meter specific functionality that is not covered by the current smart energy specification */
typedef enum {
    ESP_ZB_ZCL_METERING_ALARM_MANUFACTURER_SPECIFIC_A = 0xB0, /*!< Manufacturer Specific A */
    ESP_ZB_ZCL_METERING_ALARM_MANUFACTURER_SPECIFIC_B = 0xB1, /*!< Manufacturer Specific B */
    ESP_ZB_ZCL_METERING_ALARM_MANUFACTURER_SPECIFIC_C = 0xB2, /*!< Manufacturer Specific C */
    ESP_ZB_ZCL_METERING_ALARM_MANUFACTURER_SPECIFIC_D = 0xB3, /*!< Manufacturer Specific D */
    ESP_ZB_ZCL_METERING_ALARM_MANUFACTURER_SPECIFIC_E = 0xB4, /*!< Manufacturer Specific E */
    ESP_ZB_ZCL_METERING_ALARM_MANUFACTURER_SPECIFIC_F = 0xB5, /*!< Manufacturer Specific F */
    ESP_ZB_ZCL_METERING_ALARM_MANUFACTURER_SPECIFIC_G = 0xB6, /*!< Manufacturer Specific G */
    ESP_ZB_ZCL_METERING_ALARM_MANUFACTURER_SPECIFIC_H = 0xB7, /*!< Manufacturer Specific H */
    ESP_ZB_ZCL_METERING_ALARM_MANUFACTURER_SPECIFIC_I = 0xB8, /*!< Manufacturer Specific I */
} esp_zb_zcl_metering_manufacturer_specific_alarm_group_t;

/** @brief Service Disconnect Reasons */
typedef enum {
    ESP_ZB_ZCL_METERING_REASON_ON                                   = 0x00, /*!< ON (Service NOT disconnected) */
    ESP_ZB_ZCL_METERING_REASON_OFF_DUE_TO_OVER_POWER                = 0x01, /*!< OFF due to over power */
    ESP_ZB_ZCL_METERING_REASON_OFF_DUE_TO_OVER_VOLTAGE              = 0x02, /*!< OFF due to over voltage */
    ESP_ZB_ZCL_METERING_REASON_OFF_DUE_TO_REMOTE_DNO_LOAD_CONTROL   = 0x03, /*!< OFF due to Remote DNO Load Control */
    ESP_ZB_ZCL_METERING_REASON_OFF_BY_OTHER_REMOTE_COMMAND          = 0x04, /*!< OFF by other remote command */
    ESP_ZB_ZCL_METERING_REASON_OFF_DUE_TO_OVERHEATING_SHORT_CIRCUIT = 0x05, /*!< OFF due to overheating, I > In (Maximum Current) - short circuit */
    ESP_ZB_ZCL_METERING_REASON_OFF_DUE_TO_OVERHEATING_OTHER         = 0x06, /*!< OFF due to overheating, I < In (Maximum Current) - other */
} esp_zb_zcl_metering_service_disconnect_reasons_t;

/** @brief Metering cluster client command identifiers */
typedef enum {
    ESP_ZB_ZCL_METERING_CLI_CMD_GET_PROFILE = 0x00,      /*!< GetProfile command */
    ESP_ZB_ZCL_METERING_CLI_CMD_REQUEST_MIRROR_RESPONSE, /*!< This command allows the ESI to inform a sleepy Metering Device it has the ability to store and mirror its data */
    ESP_ZB_ZCL_METERING_CLI_CMD_MIRROR_REMOVED,          /*!< This command allows the ESI to inform a sleepy Metering Device mirroring support has been removed or halted */
    ESP_ZB_ZCL_METERING_CLI_CMD_REQUEST_FAST_POLL_MODE,  /*!< RequestFastPollMode command */
    ESP_ZB_ZCL_METERING_CLI_CMD_SCHEDULE_SNAPSHOT,       /*!< This command is used to set up a schedule of when the device shall create snapshot data */
    ESP_ZB_ZCL_METERING_CLI_CMD_TAKE_SNAPSHOT,           /*!< This command is used to instruct the cluster server to take a single snapshot */
    ESP_ZB_ZCL_METERING_CLI_CMD_GET_SNAPSHOT,            /*!< This command is used to request snapshot data from the cluster server */
    ESP_ZB_ZCL_METERING_CLI_CMD_START_SAMPLING,          /*!< The sampling mechanism allows a set of samples of the specified type of data to be taken, commencing at the stipulated start time */
    ESP_ZB_ZCL_METERING_CLI_CMD_GET_SAMPLED_DATA,        /*!< This command is used to request sampled data from the server */
    ESP_ZB_ZCL_METERING_CLI_CMD_MIRROR_REPORT_ATTRIBUTE_RESPONSE, /*!< This command is sent in response to the ReportAttribute command when the MirrorReporting attribute is set */
    ESP_ZB_ZCL_METERING_CLI_CMD_RESET_LOAD_LIMIT_COUNTER,         /*!< This command shall cause the LoadLimitCounter attribute to be reset */
    ESP_ZB_ZCL_METERING_CLI_CMD_CHANGE_SUPPLY,                    /*!< This command is sent to instruct the Metering Device to change the status of the valve or load switch */
    ESP_ZB_ZCL_METERING_CLI_CMD_LOCAL_CHANGE_SUPPLY,              /*!< This command is intended to be sent from an IHD to a meter as the consequence of a user action on the IHD. */
    ESP_ZB_ZCL_METERING_CLI_CMD_SET_SUPPLY_STATUS,                /*!< This command is used to specify the required status of the supply following the occurrence of certain events on the meter */
    ESP_ZB_ZCL_METERING_CLI_CMD_SET_UNCONTROLLED_FLOW_THRESHOLD,  /*!< This command is used to update the 'Uncontrolled Flow Rate' configuration data used by flow meters */
} esp_zb_zcl_metering_cli_cmd_t;

/** @brief Metering cluster server command identifiers */
typedef enum {
    ESP_ZB_ZCL_METERING_SRV_CMD_GET_PROFILE_RESPONSE = 0x00,     /*!< Get Profile Response */
    ESP_ZB_ZCL_METERING_SRV_CMD_REQUEST_MIRROR,                  /*!< This command is used to request the ESI to mirror Metering Device data */
    ESP_ZB_ZCL_METERING_SRV_CMD_REMOVE_MIRROR,                   /*!< This command is used to request the ESI to remove its mirror of Metering Device data */
    ESP_ZB_ZCL_METERING_SRV_CMD_REQUEST_FAST_POLL_MODE_RESPONSE, /*!< RequestFastPollModeResponse command */
    ESP_ZB_ZCL_METERING_SRV_CMD_SCHEDULE_SNAPSHOT_RESPONSE,      /*!< This command responses to a ScheduleSnapshot command to confirm whether the requested snapshot schedule has been set up */
    ESP_ZB_ZCL_METERING_SRV_CMD_TAKE_SNAPSHOT_RESPONSE,    /*!< This command responses to a TakeSnapshot command to confirm whether the requested snapshot has been accepted and successfully taken */
    ESP_ZB_ZCL_METERING_SRV_CMD_PUBLISH_SNAPSHOT,          /*!< This command responses to a GetSnapshot command or when a new snapshot is created */
    ESP_ZB_ZCL_METERING_SRV_CMD_GET_SAMPLED_DATA_RESPONSE, /*!< This command responses to a GetSampledData command to send the requested sample data to the client */

    ESP_ZB_ZCL_METERING_SRV_CMD_CONFIGURE_MIRROR,              /*!< Where 'Two Way Mirroring' is being implemented, this command shall be sent to the mirror once the
                                                                *   mirror has been created. The command allows a BOMD to provide the operational configuration of the associated Mirror
                                                                */
    ESP_ZB_ZCL_METERING_SRV_CMD_CONFIGURE_NOTIFICATION_SCHEME, /*!< Where 'Two Way Mirroring' is being implemented, and a non-default Notification Scheme is to be used,
                                                                *   this command shall be sent to the mirror once the mirror has been created
                                                                */
    ESP_ZB_ZCL_METERING_SRV_CMD_CONFIGURE_NOTIFICATION_FLAG,   /*!< Where 'Two Way Mirroring' is being implemented, and a non-default Notification Scheme is to be used, the ConfigureNotificationFlags
                                                                *   command allows a BOMD to set the commands relating to the bit value for each NotificationFlags#N attribute that the scheme is
                                                                *   proposing to use
                                                                */
    ESP_ZB_ZCL_METERING_SRV_CMD_GET_NOTIFIED_MESSAGE,          /*!< The GetNotifiedMessage command is used only when a BOMD is being mirrored. This command provides a method
                                                                *   for the BOMD to notify the Mirror message queue that it wants to receive commands that the Mirror has queued
                                                                */
    ESP_ZB_ZCL_METERING_SRV_CMD_SUPPLY_STATUS_RESPONSE,        /*!< This command is transmitted by a Metering Device in response to a Change Supply command */
    ESP_ZB_ZCL_METERING_SRV_CMD_START_SAMPLING_RESPONSE,       /*!< This command is transmitted by a Metering Device in response to a StartSampling command */
} esp_zb_zcl_metering_srv_cmd_t;

/** @brief Values for Interval */
typedef enum {
    ESP_ZB_ZCL_METERING_CONSUMPTION_DELIVERED          = 0,
    ESP_ZB_ZCL_METERING_CONSUMPTION_RECEIVED           = 1,
    ESP_ZB_ZCL_METERING_REACTIVE_CONSUMPTION_DELIVERED = 2,
    ESP_ZB_ZCL_METERING_REACTIVE_CONSUMPTION_RECEIVED  = 3,
    /* Value 4 is reserved. Values 5 - 255 is unused */
} esp_zb_zcl_metering_interval_channel_t;

/** @brief Values for Status Field */
typedef enum {
    ESP_ZB_ZCL_METERING_SUCCESS                                       = 0x00, /*!< Success */
    ESP_ZB_ZCL_METERING_UNDEFINED_INTERVAL_CHANNEL_REQUESTED          = 0x01, /*!< Undefined Interval Channel requested */
    ESP_ZB_ZCL_METERING_INTERVAL_CHANNEL_NOT_SUPPORTED                = 0x02, /*!< Interval Channel not supported */
    ESP_ZB_ZCL_METERING_INVALID_END_TIME                              = 0x03, /*!< Invalid End Time */
    ESP_ZB_ZCL_METERING_MORE_PERIODS_REQUESTED_THAN_CAN_BE_RETURNED   = 0x04, /*!< More periods requested than can be returned */
    ESP_ZB_ZCL_METERING_NO_INTERVALS_AVAILABLE_FOR_THE_REQUESTED_TIME = 0x05  /*!< No intervals available for the requested time */
} esp_zb_zcl_metering_status_field_t;

/** @brief Profile Interval Period */
typedef enum {
    ESP_ZB_ZCL_METERING_INTERVAL_PERIOD_DAILY       = 0, /*!< Daily */
    ESP_ZB_ZCL_METERING_INTERVAL_PERIOD_60_MINUTES  = 1, /*!< 60 minutes */
    ESP_ZB_ZCL_METERING_INTERVAL_PERIOD_30_MINUTES  = 2, /*!< 30 minutes */
    ESP_ZB_ZCL_METERING_INTERVAL_PERIOD_15_MINUTES  = 3, /*!< 15 minutes */
    ESP_ZB_ZCL_METERING_INTERVAL_PERIOD_10_MINUTES  = 4, /*!< 10 minutes */
    ESP_ZB_ZCL_METERING_INTERVAL_PERIOD_7_5_MINUTES = 5, /*!< 7.5 minutes */
    ESP_ZB_ZCL_METERING_INTERVAL_PERIOD_5_MINUTES   = 6, /*!< 5 minutes */
    ESP_ZB_ZCL_METERING_INTERVAL_PERIOD_2_5_MINUTES = 7, /*!< 2.5 minutes */
    ESP_ZB_ZCL_METERING_INTERVAL_PERIOD_1_MINUTE    = 8  /*!< 1 minute */
} esp_zb_zcl_metering_profile_interval_period_t;

/** @brief Sample Type Enumerations */
typedef enum {
    ESP_ZB_ZCL_METERING_SAMPLE_TYPE_CONSUMPTION_DELIVERED          = 0, /*!< Consumption Delivered */
    ESP_ZB_ZCL_METERING_SAMPLE_TYPE_CONSUMPTION_RECEIVED           = 1, /*!< Consumption Received */
    ESP_ZB_ZCL_METERING_SAMPLE_TYPE_REACTIVE_CONSUMPTION_DELIVERED = 2, /*!< Reactive Consumption Delivered */
    ESP_ZB_ZCL_METERING_SAMPLE_TYPE_REACTIVE_CONSUMPTION_RECEIVED  = 3, /*!< Reactive Consumption Received */
    ESP_ZB_ZCL_METERING_SAMPLE_TYPE_INSTANTANEOUS_DEMAND           = 4  /*!< InstantaneousDemand*/
} esp_zb_zcl_metering_sample_type_t;

/** @brief Snapshot Cause BitMap */
typedef enum {
    ESP_ZB_ZCL_METERING_CAUSE_GENERAL                        = 1 << 0,    /*!< General */
    ESP_ZB_ZCL_METERING_CAUSE_END_OF_BILLING_PERIOD          = 1 << 1,    /*!< End of Billing Period */
    ESP_ZB_ZCL_METERING_CAUSE_END_OF_BLOCK_PERIOD            = 1 << 2,    /*!< End of Block Period */
    ESP_ZB_ZCL_METERING_CAUSE_CHANGE_OF_TARIFF_INFORMATION   = 1 << 3,    /*!< Change of Tariff Information */
    ESP_ZB_ZCL_METERING_CAUSE_CHANGE_OF_PRICE_MATRIX         = 1 << 4,    /*!< Change of Price Matrix */
    ESP_ZB_ZCL_METERING_CAUSE_CHANGE_OF_BLOCK_THRESHOLDS     = 1 << 5,    /*!< Change of Block Thresholds */
    ESP_ZB_ZCL_METERING_CAUSE_CHANGE_OF_CV                   = 1 << 6,    /*!< Change of CV */
    ESP_ZB_ZCL_METERING_CAUSE_CHANGE_OF_CF                   = 1 << 7,    /*!< Change of CF */
    ESP_ZB_ZCL_METERING_CAUSE_CHANGE_OF_CALENDAR             = 1 << 8,    /*!< Change of Calendar */
    ESP_ZB_ZCL_METERING_CAUSE_CRITICAL_PEAK_PRICING          = 1 << 9,    /*!< Critical Peak Pricing */
    ESP_ZB_ZCL_METERING_CAUSE_MANUALLY_TRIGGERED_FROM_CLIENT = 1 << 10,   /*!< Manually Triggered from Client */
    ESP_ZB_ZCL_METERING_CAUSE_END_OF_RESOLVE_PERIOD          = 1 << 11,   /*!< End of Resolve Period */
    ESP_ZB_ZCL_METERING_CAUSE_CHANGE_OF_TENANCY              = 1 << 12,   /*!< Change of Tenancy */
    ESP_ZB_ZCL_METERING_CAUSE_CHANGE_OF_SUPPLIER             = 1 << 13,   /*!< Change of Supplier */
    ESP_ZB_ZCL_METERING_CAUSE_CHANGE_OF_METER_MODE           = 1 << 14,   /*!< Change of (Meter) Mode */
    ESP_ZB_ZCL_METERING_CAUSE_DEBT_PAYMENT                   = 1 << 15,   /*!< Debt Payment */
    ESP_ZB_ZCL_METERING_CAUSE_SCHEDULED_SNAPSHOT             = 1 << 16,   /*!< Scheduled Snapshot */
    ESP_ZB_ZCL_METERING_CAUSE_OTA_FIRMWARE_DOWNLOAD          = 1 << 17,   /*!< OTA Firmware Download */
    ESP_ZB_ZCL_METERING_CAUSE_SELECT_ALL_SNAPSHOTS           = 0xFFFFFFFF /*!< Select All Snapshots */
} esp_zb_zcl_metering_snapshot_cause_t;

/** @brief Snapshot Payload Type */
typedef enum {
    ESP_ZB_ZCL_METERING_TOU_DELIVERED_REGISTERS         = 0,   /*!< TOU Information Set DeliveredRegisters */
    ESP_ZB_ZCL_METERING_TOU_RECEIVED_REGISTERS          = 1,   /*!< TOU Information Set Received Registers */
    ESP_ZB_ZCL_METERING_BLOCK_TIER_DELIVERED            = 2,   /*!< Block Tier Information Set Delivered */
    ESP_ZB_ZCL_METERING_BLOCK_TIER_RECEIVED             = 3,   /*!< Block Tier Information Set Received */
    ESP_ZB_ZCL_METERING_TOU_DELIVERED_NO_BILLING        = 4,   /*!< TOU Information Set Delivered (No Billing) */
    ESP_ZB_ZCL_METERING_TOU_RECEIVED_NO_BILLING         = 5,   /*!< TOU Information Set Received (No Billing) */
    ESP_ZB_ZCL_METERING_BLOCK_TIER_DELIVERED_NO_BILLING = 6,   /*!< Block Tier Information Set Delivered (No Billing) */
    ESP_ZB_ZCL_METERING_BLOCK_TIER_RECEIVED_NO_BILLING  = 7,   /*!< Block Tier Information Set Received (No Billing) */
    ESP_ZB_ZCL_METERING_DATA_UNAVAILABLE                = 128, /*!< Data Unavailable */
} esp_zb_zcl_metering_snapshot_payload_type_t;

/** @brief Set summation formatting */
#define ESP_ZB_ZCL_METERING_FORMATTING_SET(suppress_zero, left, right)      \
  ( ((suppress_zero) ? 0x80 : 0) | (((left) & 15)<<3) | ((right) & 7) )

/** @brief Default value for Status attribute */
#define ESP_ZB_ZCL_METERING_STATUS_DEFAULT_VALUE 0x00

/** @brief Default value for Status attribute */
#define ESP_ZB_ZCL_METERING_UNIT_OF_MEASURE_DEFAULT_VALUE 0x00

/** @brief Default value for Instantaneous Demand attribute */
#define ESP_ZB_ZCL_METERING_INSTANTANEOUS_DEMAND_DEFAULT_VALUE ZB_INIT_UINT24(0x00, 0x0000)

/** @brief Default value for DailyFreezeTime attribute */
#define ESP_ZB_ZCL_METERING_DAILY_FREEZE_TIME_DEFAULT_VALUE ((uint16_t)0x0000)

/** @brief Default value for PowerFactor attribute */
#define ESP_ZB_ZCL_METERING_POWER_FACTOR_DEFAULT_VALUE ((int8_t)0x00)

/** @brief Default value for DefaultUpdatePeriod attribute */
#define ESP_ZB_ZCL_METERING_DEFAULT_UPDATE_PERIOD_DEFAULT_VALUE ((uint8_t)0x1E)

/** @brief Default value for FastPollUpdatePeriod attribute */
#define ESP_ZB_ZCL_METERING_FAST_POLL_UPDATE_PERIOD_DEFAULT_VALUE ((uint8_t)0x05)

/** @brief Default value for PresetReadingTime attribute */
#define ESP_ZB_ZCL_METERING_PRESET_READING_TIME_DEFAULT_VALUE ((uint16_t)0x0000)

/** @brief Default value for MaxNumberOfPeriodsDelivered attribute */
#define ESP_ZB_ZCL_METERING_MAX_NUMBER_OF_PERIODS_DELIVERED_DEFAULT_VALUE ((uint8_t)0x18)

/** @brief Default value for GenericAlarmMask attribute */
#define ESP_ZB_ZCL_METERING_GENERIC_ALARM_MASK_DEFAULT_VALUE ((uint16_t)0xFFFF)

/** @brief Default value for ElectricityAlarmMask attribute */
#define ESP_ZB_ZCL_METERING_ELECTRICITY_ALARM_MASK_DEFAULT_VALUE ((uint32_t)0xFFFFFFFF)

/** @brief Default value for GenericFlowPressureAlarmMask attribute */
#define ESP_ZB_ZCL_METERING_GENERIC_FLOW_PRESSURE_ALARM_MASK_DEFAULT_VALUE ((uint16_t)0xFFFF)

/** @brief Default value for WaterSpecificAlarmMask attribute */
#define ESP_ZB_ZCL_METERING_WATER_SPECIFIC_ALARM_MASK_DEFAULT_VALUE ((uint16_t)0xFFFF)

/** @brief Default value for HeatAndCoolingSpecificAlarmMask attribute */
#define ESP_ZB_ZCL_METERING_HEAT_AND_COOLING_SPECIFIC_ALARM_MASK_DEFAULT_VALUE ((uint16_t)0xFFFF)

/** @brief Default value for GasSpecificAlarmMask attribute */
#define ESP_ZB_ZCL_METERING_GAS_SPECIFIC_ALARM_MASK_DEFAULT_VALUE ((uint16_t)0xFFFF)

/** @brief Default value for FastPollUpdatePeriod attribute */
#define ESP_ZB_ZCL_METERING_LINKY_MODE_ON_OPERATION_DEFAULT_VALUE ((uint8_t)0x00)

#ifdef __cplusplus
}
#endif
