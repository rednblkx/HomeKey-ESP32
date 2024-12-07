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
/* PURPOSE: Device Management cluster definitions
*/

#ifndef ZB_ZCL_DEVICE_MANAGEMENT_H_
#define ZB_ZCL_DEVICE_MANAGEMENT_H_

#include "zboss_api_core.h" /* ZBOSS core API types */

/** @cond DOXYGEN_ZCL_SECTION && DOXYGEN_SE_SECTION */

/** @addtogroup ZB_ZCL_DEVICE_MANAGEMENT
  * @{
  *   @details
  *   The Device Management cluster provides an interface to the functionality
  *   of devices within a Smart energy Network.
  *   The cluster will support the following functions:
  *  <ul>
  *    <li>
  *      Supplier Control
  *    </li>
  *    <li>
  *      Tenancy Control
  *    </li>
  *    <li>
  *      Password Control
  *    </li>
  *    <li>
  *      Event Configuration
  *    </li>
  *  </ul>
  */

/** @defgroup ZB_ZCL_DEVICE_MANAGEMENT_ATTRS Device Management cluster attributes
 * @{
 */

/** @brief Device Management Server Attribute Sets
 *  @see SE spec, subclause 10.2.2
 */
typedef enum zb_zcl_device_management_srv_attr_sets_e
{
  ZB_ZCL_DEVICE_MANAGEMENT_SRV_SET_SUPPLIER_CONTROL = 0x01,  /**< Supplier Control Attribute Set */
  ZB_ZCL_DEVICE_MANAGEMENT_SRV_SET_TENANCY_CONTROL  = 0x02,  /**< Tenancy Control Attribute Set */
  ZB_ZCL_DEVICE_MANAGEMENT_SRV_SET_BACKHAUL_CONTROL = 0x03,  /**< Backhaul Control Attribute Set */
  ZB_ZCL_DEVICE_MANAGEMENT_SRV_SET_HAN_CONTROL      = 0x04,  /**< HAN Control Attribute Set */
} zb_zcl_device_management_srv_attr_sets_t;


/** @brief Supplier Control Attribute Set
 *  @see SE spec, subclause 10.2.2.1
 */
typedef enum zb_zcl_device_management_supplier_control_attr_set_e
{
  /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_SRV_PROVIDER_ID                      = 0x0100,    /**< The ProviderID attribute represents a unique identifier for the current
                                                                          *  commodity supplier.
                                                                          */
  /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_SRV_PROVIDER_NAME,                                /**< The Provider Name attribute represents the name of the current supplier
                                                                           *  of the commodity to the device.
                                                                           */
  /* (O) */
   ZB_ZCL_ATTR_DEVICE_MANAGEMENT_SRV_PROVIDER_CONTACT_DETAILS,                    /**< The Provider Contact Details attribute represents the contact details of
                                                                          *  the current Provider delivering a commodity to the premises.
                                                                          */
  /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_SRV_PROPOSED_PROVIDER_ID             = 0x0110,    /**< The Proposed Provider ID attribute represents a unique identifier for the
                                                                          *  commodity supplier associated with the proposed change to the supply of
                                                                          *  the commodity.
                                                                          */
  /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_SRV_PROPOSED_PROVIDER_NAME,                       /**< The Proposed Provider Name attribute represents the name for the commodity
                                                                          *  supplier associated with the proposed change to the supply of energy.
                                                                          */
  /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_SRV_PROPOSED_PROVIDER_CHANGE_DATE_TIME,           /**< The Proposed Provider Change Date/Time attribute represents a UTC time
                                                                          *  that defines the time and date when the new supplier will take over the
                                                                          *  supply of the commodity to the Meter/HAN.
                                                                          */
  /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_SRV_PROPOSED_PROVIDER_CHANGE_CONTROL,             /**< The Proposed Provider Change Control attribute represents the functions
                                                                          *  that are required to be carried out on processing of the change of supplier.
                                                                          */
  /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_SRV_PROPOSED_PROVIDER_CONTACT_DETAILS,            /**< The Proposed Provider Contact Details attribute represents the contact
                                                                          *  details of the Provider associated with the proposed change of supply of
                                                                          *  the commodity delivered to the premises.
                                                                          */
  /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_SRV_RECEIVED_PROVIDER_ID             = 0x0120,    /**< The ReceivedProviderID attribute represents a unique identifier for the
                                                                          *  commodity supplier receiving the Received energy.
                                                                          */
  /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_SRV_RECEIVED_PROVIDER_NAME,                       /**< The ReceivedProviderName attribute represents the name of the current
                                                                          *  supplier of Received energy services to the device.
                                                                          */
  /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_SRV_RECEIVED_PROVIDER_CONTACT_DETAILS,            /**< The ReceivedProviderContactDetails attribute represents the contact
                                                                          *  details of the current Provider receiving a commodity from the premises.
                                                                          */
  /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_SRV_RECEIVED_PROPOSED_PROVIDER_ID    = 0x0130,    /**< The ReceivedProposedProviderID attribute represents the name for the
                                                                          *  commodity supplier associated with the proposed change to
                                                                          *  the Receiving of energy.
                                                                          */
  /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_SRV_RECEIVED_PROPOSED_PROVIDER_NAME,              /**< The ReceivedProposedProviderName attribute represents a unique identifier
                                                                          *  for the commodity supplier associated with the proposed change to
                                                                          *  the Receiving of energy.
                                                                          */
  /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_SRV_RECEIVED_PROPOSED_PROVIDER_CHANGE_DATE_TIME,  /**< The ReceivedProposedProviderChangeDate/Time attribute represents a UTC
                                                                          *  time that defines the time and date that the new supplier will take over
                                                                          *  the Received of energy from the Meter/HAN.
                                                                          */
  /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_SRV_RECEIVED_PROPOSED_PROVIDER_CHANGE_CONTROL,    /**< The ReceivedProposedProviderChangeControl attribute represents the
                                                                          *  functions that are required to be carried out on processing of the change
                                                                          *  of supplier.
                                                                          */
  /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_SRV_RECEIVED_PROPOSED_PROVIDER_CONTACT_DETAILS,   /**< The Received Proposed Provider Contact Details attribute represents the contact
                                                                          *  details of the Provider associated with the proposed change of receipt
                                                                          *  of the commodity from the premises.
                                                                          */
} zb_zcl_device_management_supplier_control_attr_set_t;


/** @brief Proposed Change Control Bitmap definition
 *  @see SE spec, Table D-164
 */
typedef enum zb_zcl_device_management_proposed_change_ctrl_attr_e
{
  ZB_ZCL_DEVICE_MANAGEMENT_PRE_SNAPSHOTS                        = 0,       /**< A snapshot shall be triggered
                                                                             */

  ZB_ZCL_DEVICE_MANAGEMENT_POST_SNAPSHOTS                       = 1 << 1,  /**< A snapshot shall be triggered
                                                                             */

  ZB_ZCL_DEVICE_MANAGEMENT_RESET_CREDIT_REGISTER                = 1 << 2,  /**< All Credit Registers shall be reset to their default value
                                                                             */

  ZB_ZCL_DEVICE_MANAGEMENT_RESET_DEBIT_REGISTER                 = 1 << 3,  /**< All Debt Registers shall be reset to their default value
                                                                            */

  ZB_ZCL_DEVICE_MANAGEMENT_RESET_BILLING_PERIOD                 = 1 << 4,  /**< All Billing periods shall be reset to their default value
                                                                            */

  ZB_ZCL_DEVICE_MANAGEMENT_CLEAR_TARIFF_PLAN                    = 1 << 5,  /**< The tariff shall be reset to its default value
                                                                            */

  ZB_ZCL_DEVICE_MANAGEMENT_CLEAR_STANDING_CHARGE                = 1 << 6,  /**< The Standing Charge shall be reset to its default value
                                                                            */

  ZB_ZCL_DEVICE_MANAGEMENT_BLOCK_HIST_LOAD_PROFILE_INFORMATION  = 1 << 7,  /**< Historical LP information shall no longer be available to be
                                                                            *  published to the HA
                                                                            */

  ZB_ZCL_DEVICE_MANAGEMENT_CLEAR_HIST_LOAD_PROFILE_INFORMATION  = 1 << 8,  /**< Historical LP information shall be cleared from all devices
                                                                            */

  ZB_ZCL_DEVICE_MANAGEMENT_CLEAR_IHD_DATA_CONSUMER              = 1 << 9,  /**< All consumer data shall be removed
                                                                            */

  ZB_ZCL_DEVICE_MANAGEMENT_CLEAR_IHD_DATA_SUPPLIER              = 1 << 10, /**< All supplier data shall be removed
                                                                            */

  ZB_ZCL_DEVICE_MANAGEMENT_METER_CONTACTOR_SUPPLY_OFF_ARMED     = 1 << 11, /**< The required status of the meter contactor post action.
                                                                            *  Two Bits Combination. If bits are zero - Meter Contactor State is SUPPLY_OFF.
                                                                            *  @note In certain markets, this value cannot trigger automatic reconnection
                                                                            *  of the supply, only maintain the current status of, disconnect or ARM the
                                                                            *  supply.
                                                                            *  @see SE spec, Table D-164 and Table D-165
                                                                            */

  ZB_ZCL_DEVICE_MANAGEMENT_METER_CONTACTOR_SUPPLY_ON            = 2 << 11,  /**< Meter Contactor Supply On */

  ZB_ZCL_DEVICE_MANAGEMENT_METER_CONTACTOR_SUPPLY_UNCHANGED     = 3 << 11,  /**< Meter Contactor Supply Unchanged */

  ZB_ZCL_DEVICE_MANAGEMENT_CLEAR_TRANSACTION_LOG                = 1 << 13,  /**< All transaction logs shall be cleared from all devices
                                                                                                  */

  ZB_ZCL_DEVICE_MANAGEMENT_CLEAR_PREPAYMENT_LOG                 = 1 << 14,  /**< All Prepayment Registers shall be reset to their default state
                                                                                                  */
} zb_zcl_device_management_proposed_change_ctrl_attr_t;


/** @brief Tenancy Control Attribute Set
 *  @see SE spec, subclause 10.2.2.2
 */
typedef enum zb_zcl_device_management_tenancy_control_attr_set_e
{
  /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_SRV_CHANGE_OF_TENANCY_UPDATE_DATE_TIME = 0x0200,  /**< The ChangeofTenancyUpdateDate/Time attribute represents the time at
                                                                          *  which a proposed change to the tenancy is to be implemented.
                                                                          */
  /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_SRV_PROPOSED_TENANCY_CHANGE_CONTROL,              /**< The ProposedTenancyChangeControl attribute represents the functions that
                                                                          *  are required to be carried out on processing of the change of tenancy.
                                                                          */
} zb_zcl_device_management_tenancy_control_attr_set_t;


/** @brief Backhaul Control Attribute Set
 *  @see SE spec, subclause 10.2.2.3
 */
typedef enum zb_zcl_device_management_backhaul_control_attr_set_e
{
  /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_SRV_WAN_STATUS = 0x0300,  /**< The WAN Status attribute represents an 8-bit enumeration defining the
                                                  *  state of the WAN (Wide Area Network) connection
                                                  */
} zb_zcl_device_management_backhaul_control_attr_set_t;


/** @brief State of the WAN Connection enumeration
 *  @see SE spec, Table D-168
 */
typedef enum zb_zcl_device_management_wan_status_attr_e
{
  ZB_ZCL_DEVICE_MANAGEMENT_WAN_STATUS_CONNECTION_NOT_AVAILABLE = 0,    /**< Connection to WAN is not available
                                                                        */

  ZB_ZCL_DEVICE_MANAGEMENT_WAN_STATUS_CONNECTION_AVAILABLE,            /**< Connection to WAN is available
                                                                        */
} zb_zcl_device_management_wan_status_attr_t;


/** @brief HAN Control Attribute Set
 *  @see SE spec, subclause 10.2.2.4
 */
typedef enum zb_zcl_device_management_han_control_attr_set_e
{
  /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_SRV_LOW_MEDIUM_THRESHOLD  = 0x0400,  /**< The Low Medium Threshold attribute represents the threshold at which
                                                             *  the value of Instantaneous Demand is deemed to have moved from low
                                                             *  energy usage to medium usage.
                                                             */

  /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_SRV_MEDIUM_HIGH_THRESHOLD,           /**< The Medium High Threshold attribute represents the threshold at which
                                                              *  the value of Instantaneous Demand is deemed to have moved from medium
                                                              *  energy usage to high usage.
                                                              */
} zb_zcl_device_management_han_control_attr_set_t;


/** @brief Device Management Client Attribute Sets
 *  @see SE spec, subclause 10.3.2
 */
typedef enum zb_zcl_device_management_cli_attr_sets_e
{
  ZB_ZCL_DEVICE_MANAGEMENT_CLI_SET_SUPPLIER                         = 0x00,  /**< Supplier Attribute Set */
  ZB_ZCL_DEVICE_MANAGEMENT_CLI_SET_PRICE_EVENT_CONFIGURATION        = 0x01,  /**< Price Event Configuration Attribute Set */
  ZB_ZCL_DEVICE_MANAGEMENT_CLI_SET_METERING_EVENT_CONFIGURATION     = 0x02,  /**< Metering Event Configuration Attribute Set */
  ZB_ZCL_DEVICE_MANAGEMENT_CLI_SET_MESSAGING_EVENT_CONFIGURATION    = 0x03,  /**< Messaging Event Configuration Attribute set */
  ZB_ZCL_DEVICE_MANAGEMENT_CLI_SET_PREPAY_EVENT_CONFIGURATION       = 0x04,  /**< Prepay Event Configuration Attribute Set */
  ZB_ZCL_DEVICE_MANAGEMENT_CLI_SET_CALENDAR_EVENT_CONFIGURATION     = 0x05,  /**< Calendar Event Configuration Attribute set */
  ZB_ZCL_DEVICE_MANAGEMENT_CLI_SET_DEVICE_MGMT_EVENT_CONFIGURATION  = 0x06,  /**< Device Management Event Configuration Attribute set */
  ZB_ZCL_DEVICE_MANAGEMENT_CLI_SET_TUNNEL_EVENT_CONFIGURATION       = 0x07,  /**< Tunnel Event Configuration Attribute set */
  ZB_ZCL_DEVICE_MANAGEMENT_CLI_SET_OTA_EVENT_CONFIGURATION          = 0x08,  /**< OTA Event Configuration Attribute set */
} zb_zcl_device_management_cli_attr_sets_t;


/** @brief Supplier Attribute Set
 *  @see SE spec, subclause 10.3.2.1
 */
typedef enum zb_zcl_device_management_supplier_attr_set_e
{
  /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_SUPPLIER_PROVIDER_ID          = 0x0000,  /**< The ProviderID attribute represents a unique identifier for the
                                                                      *  commodity provider to whom this attribute relates.
                                                                      */

  /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_SUPPLIER_RECEIVED_PROVIDER_ID = 0x0010,  /**< The ReceivedProviderID attribute represents a unique identifier for
                                                                      *  the commodity provider to whom this attribute relates.
                                                                      */
} zb_zcl_device_management_supplier_attr_set_t;


/** @brief Price Event Configuration Attribute Set.
 *  These attributes allow events related to pricing to be configured.
 *  @see SE spec, subclause 10.3.2.2
 */
typedef enum zb_zcl_device_management_price_event_cfg_attr_set_e
{
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_TOUT_TARIFF_ACTIVATION                   = 0x0100, /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_BLOCK_TARIFF_ACTIVATED,                            /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_BLOCK_TOUT_TARIFF_ACTIVATED,                       /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_SINGLE_TARIFF_RATE_ACTIVATED,                      /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_ASYNCHRONOUS_BILLING_OCCURED,                      /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_SYNCHRONOUS_BILLING_OCCURED,                       /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_TARIFF_NOT_SUPPORTED,                              /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_PRICE_CLUSTER_NOT_FOUND,                           /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_CURRENTLY_CHANGE_PASSIVE_ACTIVATED,                /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_CURRENTLY_CHANGE_PASSIVE_UPDATED,                  /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_PRICE_MATRIX_PASSIVE_ACTIVATED,                    /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_PRICE_MATRIX_PASSIVE_UPDATED,                      /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_TARIFF_CHANGED_PASSIVE_ACTIVATED,                  /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_TARIFF_CHANGED_PASSIVE_UPDATED,                    /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_PUBLISH_PRICE_RECEIVED                   = 0x01B0, /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_PUBLISH_PRICE_ACTIONED,                            /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_PUBLISH_PRICE_CANCELLED,                           /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_PUBLISH_PRICE_REJECTED,                            /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_PUBLISH_TARIFF_INFORMATION_RECEIVED,               /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_PUBLISH_TARIFF_INFORMATION_ACTIONED,               /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_PUBLISH_TARIFF_INFORMATION_CANCELLED,              /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_PUBLISH_TARIFF_INFORMATION_REJECTED,               /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_PUBLISH_PRICE_MATRIX_RECEIVED,                     /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_PUBLISH_PRICE_MATRIX_ACTIONED,                     /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_PUBLISH_PRICE_MATRIX_CANCELLED,                    /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_PUBLISH_PRICE_MATRIX_REJECTED,                     /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_PUBLISH_BLOCK_THRESHOLDS_RECEIVED,                 /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_PUBLISH_BLOCK_THRESHOLDS_ACTIONED,                 /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_PUBLISH_BLOCK_THRESHOLDS_CANCELLED,                /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_PUBLISH_BLOCK_THRESHOLDS_REJECTED,                 /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_PUBLISH_CALORIFIC_VALUE_RECEIVED,                  /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_PUBLISH_CALORIFIC_VALUE_ACTIONED,                  /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_PUBLISH_CALORIFIC_VALUE_CANCELLED,                 /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_PUBLISH_CALORIFIC_VALUE_REJECTED,                  /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_PUBLISH_CONVERSION_FACTOR_RECEIVED,                /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_PUBLISH_CONVERSION_FACTOR_ACTIONED,                /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_PUBLISH_CONVERSION_FACTOR_CANCELLED,               /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_PUBLISH_CONVERSION_FACTOR_REJECTED,                /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_PUBLISH_CO2_VALUE_RECEIVED,                        /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_PUBLISH_CO2_VALUE_ACTIONED,                        /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_PUBLISH_CO2_VALUE_CANCELLED,                       /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_PUBLISH_CO2_VALUE_REJECTED,                        /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_PUBLISH_CPP_EVENT_RECEIVED,                        /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_PUBLISH_CPP_EVENT_ACTIONED,                        /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_PUBLISH_CPP_EVENT_CANCELLED,                       /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_PUBLISH_CPP_EVENT_REJECTED,                        /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_PUBLISH_TIER_LABELS_RECEIVED,                      /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_PUBLISH_TIER_LABELS_ACTIONED,                      /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_PUBLISH_TIER_LABELS_CANCELLED,                     /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_PUBLISH_TIER_LABELS_REJECTED,                      /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_PUBLISH_BILLING_PERIOD_RECEIVED,                   /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_PUBLISH_BILLING_PERIOD_ACTIONED,                   /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_PUBLISH_BILLING_PERIOD_CANCELLED,                  /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_PUBLISH_BILLING_PERIOD_REJECTED,                   /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_PUBLISH_CONSOLIDATED_BILL_RECEIVED,                /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_PUBLISH_CONSOLIDATED_BILL_ACTIONED,                /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_PUBLISH_CONSOLIDATED_BILL_CANCELLED,               /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_PUBLISH_CONSOLIDATED_BILL_REJECTED,                /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_PUBLISH_BLOCK_PERIOD_RECEIVED,                     /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_PUBLISH_BLOCK_PERIOD_ACTIONED,                     /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_PUBLISH_BLOCK_PERIOD_CANCELLED,                    /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_PUBLISH_BLOCK_PERIOD_REJECTED,                     /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_PUBLISH_CREDIT_PAYMENT_INFO_RECEIVED,              /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_PUBLISH_CREDIT_PAYMENT_INFO_ACTIONED,              /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_PUBLISH_CREDIT_PAYMENT_INFO_CANCELLED,             /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_PUBLISH_CREDIT_PAYMENT_INFO_REJECTED,              /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_PUBLISH_CURRENCY_CONVERSION_RECEIVED,              /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_PUBLISH_CURRENCY_CONVERSION_ACTIONED,              /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_PUBLISH_CURRENCY_CONVERSION_CANCELLED,             /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_PUBLISH_CURRENCY_CONVERSION_REJECTED,              /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_RESERVED_FOR_PRICE_CLUSTER_GROUP_ID      = 0x01FF, /* (O) */
} zb_zcl_device_management_price_event_cfg_attr_set_t;


/** @brief Metering Event Configuration Attribute Set.
 *  These attributes allow events related to meter to be configured.
 *  @see SE spec, subclause 10.3.2.3
 */
typedef enum zb_zcl_device_management_metering_event_cfg_attr_set_e
{
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_CHECK_METER                              = 0x0200, /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_LOW_BATTERY,                                       /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_TAMPER_DETECT,                                     /* (O) */

  /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_SUPPLY_STATUS,      /**< Electricity: Power Failure. Gas: Not Defined. Water: Pipe Empty.
                                                *  Heat/Cooling: Temperature Sensor.
                                                */

  /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_SUPPLY_QUALITY,     /**< Electricity: Power Quality. Gas: Low Pressure. Water: Low Pressure.
                                                *  Heat/Cooling: Heat/Cooling: Burst Detect.
                                                */

  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_LEAK_DETECT,                                       /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_SERVICE_DISCONNECT,                                /* (O) */

  /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_REVERSE_FLOW,       /**< Electricity: Reserved. Gas: Reverse Flow. Water: Reverse Flow.
                                                *  Heat/Cooling: Heat/Cooling: Reverse Flow.
                                                */

  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_METER_COVER_REMOVED,                               /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_METER_COVER_CLOSED,                                /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_STRONG_MAGNETIC_FIELD,                             /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_NO_STRONG_MAGNETIC_FIELD,                          /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_BATTERY_FAILURE,                                   /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_PROGRAM_MEMORY_ERROR,                              /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_RAM_ERROR,                                         /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_NVMEMORY_ERROR,                                    /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_LOW_VOLTAGE_L1,                                    /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_HIGH_VOLTAGE_L1,                                   /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_LOW_VOLTAGE_L2,                                    /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_HIGH_VOLTAGE_L2,                                   /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_LOW_VOLTAGE_L3,                                    /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_HIGH_VOLTAGE_L3,                                   /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_OVER_CURRENT_L1,                                   /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_OVER_CURRENT_L2,                                   /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_OVER_CURRENT_L3,                                   /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_FREQUENCY_TOO_LOW_L1,                              /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_FREQUENCY_TOO_HIGH_L1,                             /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_FREQUENCY_TOO_LOW_L2,                              /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_FREQUENCY_TOO_HIGH_L2,                             /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_FREQUENCY_TOO_LOW_L3,                              /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_FREQUENCY_TOO_HIGH_L3,                             /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_GROUND_FAULT,                                      /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_ELECTRIC_TAMPER_DETECT,                            /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_INCORRECT_POLARITY,                                /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_CURRENT_NO_VOLTAGE,                                /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_UNDER_VOLTAGE,                                     /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_OVER_VOLTAGE,                                      /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_NORMAL_VOLTAGE,                                    /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_PF_BELOW_THRESHOLD,                                /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_PF_ABOVE_THRESHOLD,                                /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_TERMINAL_COVER_REMOVED,                            /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_TERMINAL_COVER_CLOSED,                             /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_BURST_DETECT                             = 0x0230, /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_PRESSURE_TOO_LOW,                                  /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_PRESSURE_TOO_HIGH,                                 /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_FLOW_SENSOR_COMMUNICATION_ERROR,                   /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_FLOW_SENSOR_MEASUREMENT_FAULT,                     /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_FLOW_SENSOR_REVERSE_FLOW,                          /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_FLOW_SENSOR_AIR_DETECT,                            /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_PIPE_EMPTY,                                        /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_INLET_TEMPERATURE_SENSOR_FAULT           = 0x0250, /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_OUTLET_TEMPERATURE_SENSOR_FAULT,                   /* (O) */

  /*  NOTE: in the SE spec D.10.3.2.3 there are two attributes with name
   *  Reverse Flow for Metering Event Configuration Attribute Set.
   */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_REVERSE_FLOW_OTHER                       = 0x0260, /* (O) */

  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_TILT_TAMPER,                                       /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_BATTERY_COVER_REMOVED,                             /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_BATTERY_COVER_CLOSED,                              /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_EXCESS_FLOW,                                       /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_TILT_TAMPER_ENDED,                                 /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_MEASUREMENT_SYSTEM_ERROR                 = 0x0270, /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_WATCHDOG_ERROR,                                    /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_SUPPLY_DISCONNECT_FAILURE,                         /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_SUPPLY_CONNECT_FAILURE,                            /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_MEASUREMENT_SOFTWARE_CHANGED,                      /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_DST_ENABLED,                                       /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_DST_DISABLED,                                      /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_CLOCK_ADJ_BACKWARD,                                /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_CLOCK_ADJ_FORWARD,                                 /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_CLOCK_INVALID,                                     /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_COMMUNICATION_ERROR_HAN,                           /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_COMMUNICATION_OK_HAN,                              /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_METER_FRAUD_ATTEMPT,                               /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_POWER_LOSS,                                        /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_UNUSUAL_HAN_TRAFFIC,                               /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_UNEXPECTED_CLOCK_CHANGE,                           /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_COMMS_USING_UNAUTHENTICATED_COMPONENT,             /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_METERING_ERROR_REG_CLEAR,                          /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_METERING_ALARM_REG_CLEAR,                          /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_UNEXPECTED_HW_RESET,                               /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_UNEXPECTED_PROGRAM_EXECUTION,                      /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_LIMIT_THRESHOLD_EXCEEDED,                          /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_LIMIT_THRESHOLD_OK,                                /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_LIMIT_THRESHOLD_CHANGED,                           /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_MAXIMUM_DEMAND_EXCEEDED,                           /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_PROFILE_CLEARED,                                   /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_LOAD_PROFILE_CLEARED,                              /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_BATTERY_WARNING,                                   /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_WRONG_SIGNATURE,                                   /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_NO_SIGNATURE,                                      /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_SIGNATURE_NOT_VALID,                               /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_UNAUTHORISED_ACTION_FROM_HAN,                      /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_FAST_POLLING_START,                                /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_FAST_POLLING_END,                                  /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_METER_REPORTING_INTERVAL_CHANGED,                  /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_DISCONNECT_TO_LOAD_LIMIT,                          /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_METER_SUPPLY_STATUS_REGISTER_CHANGED,              /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_METER_ALARM_STATUS_REGISTER_CHANGED,               /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_EXT_METER_ALARM_STATUS_REGISTER_CHANGED,           /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_DATA_ACCESS_VIA_LOCAL_PORT,                        /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_CONFIGURE_MIRROR_SUCCESS,                          /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_CONFIGURE_MIRROR_FAILURE,                          /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_CONFIGURE_NOTIFICATION_FLAG_SCHEME_SUCCESS,        /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_CONFIGURE_NOTIFICATION_FLAG_SCHEME_FAILURE,        /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_CONFIGURE_NOTIFICATION_FLAGS_SUCCESS,              /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_CONFIGURE_NOTIFICATION_FLAGS_FAILURE,              /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_STAY_AWAKE_REQUEST_HAN,                            /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_STAY_AWAKE_REQUEST_WAN,                            /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_MANUFACTURER_SPECIFIC_A                  = 0x02B0, /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_MANUFACTURER_SPECIFIC_B,                           /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_MANUFACTURER_SPECIFIC_C,                           /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_MANUFACTURER_SPECIFIC_D,                           /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_MANUFACTURER_SPECIFIC_E,                           /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_MANUFACTURER_SPECIFIC_F,                           /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_MANUFACTURER_SPECIFIC_G,                           /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_MANUFACTURER_SPECIFIC_H,                           /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_MANUFACTURER_SPECIFIC_I,                           /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_GET_PROFILE_COMMAND_RECEIVED             = 0x02C0, /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_GET_PROFILE_COMMAND_ACTIONED,                      /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_GET_PROFILE_COMMAND_CANCELLED,                     /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_GET_PROFILE_COMMAND_REJECTED,                      /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_REQUEST_MIRROR_RESPONSE_COMMAND_RECEIVED,          /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_REQUEST_MIRROR_RESPONSE_COMMAND_ACTIONED,          /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_REQUEST_MIRROR_RESPONSE_COMMAND_CANCELLED,         /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_REQUEST_MIRROR_RESPONSE_COMMAND_REJECTED,          /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_MIRROR_REMOVED_COMMAND_RECEIVED,                   /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_MIRROR_REMOVED_COMMAND_ACTIONED,                   /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_MIRROR_REMOVED_COMMAND_CANCELLED,                  /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_MIRROR_REMOVED_COMMAND_REJECTED,                   /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_GET_SNAPSHOT_COMMAND_RECEIVED,                     /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_GET_SNAPSHOT_COMMAND_ACTIONED,                     /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_GET_SNAPSHOT_COMMAND_CANCELLED,                    /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_GET_SNAPSHOT_COMMAND_REJECTED,                     /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_TAKE_SNAPSHOT_COMMAND_RECEIVED,                    /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_TAKE_SNAPSHOT_COMMAND_ACTIONED,                    /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_TAKE_SNAPSHOT_COMMAND_CANCELLED,                   /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_TAKE_SNAPSHOT_COMMAND_REJECTED,                    /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_MIRROR_REPORT_ATTR_RESPONSE_COMMAND_RECEIVED,      /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_MIRROR_REPORT_ATTR_RESPONSE_COMMAND_ACTIONED,      /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_MIRROR_REPORT_ATTR_RESPONSE_COMMAND_CANCELLED,     /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_MIRROR_REPORT_ATTR_RESPONSE_COMMAND_REJECTED,      /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_SCHEDULE_SNAPSOT_COMMAND_RECEIVED,                 /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_SCHEDULE_SNAPSOT_COMMAND_ACTIONED,                 /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_SCHEDULE_SNAPSOT_COMMAND_CANCELLED,                /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_SCHEDULE_SNAPSOT_COMMAND_REJECTED,                 /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_START_SAMPLING_COMMAND_RECEIVED,                   /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_START_SAMPLING_COMMAND_ACTIONED,                   /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_START_SAMPLING_COMMAND_CANCELLED,                  /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_START_SAMPLING_COMMAND_REJECTED,                   /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_GET_SAMPLED_DATA_COMMAND_RECEIVED,                 /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_GET_SAMPLED_DATA_COMMAND_ACTIONED,                 /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_GET_SAMPLED_DATA_COMMAND_CANCELLED,                /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_GET_SAMPLED_DATA_COMMAND_REJECTED,                 /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_SUPPLY_ON,                                         /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_SUPPLY_ARMED,                                      /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_SUPPLY_OFF,                                        /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_DISCONNECTED_DUE_TO_TAMPER_DETECTED,               /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_MANUAL_DISCONNECT,                                 /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_MANUAL_CONNECT,                                    /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_REMOTE_DISCONNECTION,                              /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_REMOTE_CONNECT,                                    /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_LOCAL_DISCONNECTION,                               /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_LOCAL_CONNECT,                                     /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_CHANGE_SUPPLY_RECEIVED,                            /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_CHANGE_SUPPLY_ACTIONED,                            /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_CHANGE_SUPPLY_CANCELLED,                           /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_CHANGE_SUPPLY_REJECTED,                            /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_LOCAL_CHANGE_SUPPLY_RECEIVED,                      /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_LOCAL_CHANGE_SUPPLY_ACTIONED,                      /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_LOCAL_CHANGE_SUPPLY_CANCELLED,                     /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_LOCAL_CHANGE_SUPPLY_REJECTED,                      /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_PUBLISH_UNCONTROLLED_FLOW_THRESHOLD_RECEIVED,          /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_PUBLISH_UNCONTROLLED_FLOW_THRESHOLD_ACTIONED,          /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_PUBLISH_UNCONTROLLED_FLOW_THRESHOLD_CANCELLED,         /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_PUBLISH_UNCONTROLLED_FLOW_THRESHOLD_REJECTED,          /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_RESERVED_FOR_METERING_CLUSTER_GROUP_ID   = 0x02FF, /* (O) */
} zb_zcl_device_management_metering_event_cfg_attr_set_t;


/** @brief Messaging Event Configuration Attribute Set.
 *  These attributes allow events related to messaging to be configured.
 *  @see SE spec, subclause 10.3.2.4
 */
typedef enum zb_zcl_device_management_messaging_event_cfg_attr_set_e
{
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_MESSAGE_CONFIRMATION_SENT                = 0x0300, /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_DISPLAY_MESSAGE_RECEIVED                 = 0x03C0, /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_DISPLAY_MESSAGE_ACTIONED,                          /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_DISPLAY_MESSAGE_CANCELLED,                         /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_DISPLAY_MESSAGE_REJECTED,                          /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_CANCEL_MESSAGE_RECEIVED,                           /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_CANCEL_MESSAGE_ACTIONED,                           /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_CANCEL_MESSAGE_CANCELLED,                          /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_CANCEL_MESSAGE_REJECTED,                           /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_RESERVED_FOR_MESSAGING_CLUSTER_GROUP_ID  = 0x03FF, /* (O) */
} zb_zcl_device_management_messaging_cfg_attr_set_t;


/** @brief Prepayment Event Configuration Attribute Set.
 *  These attributes allow events related to prepayment to be configured.
 *  @see SE spec, subclause 10.3.2.5
 */
typedef enum zb_zcl_device_management_prepayment_event_cfg_attr_set_e
{
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_LOW_CREDIT                               = 0x0400, /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_NO_CREDIT,                                         /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_CREDIT_EXHAUSTED,                                  /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_EMERGENCY_CREDIT_ENABLED,                          /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_EMERGENCY_CREDIT_EXHAUSTED,                        /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_IHD_LOW_CREDIT_WARNING,                            /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_PHYSICAL_ATTACK_ON_THE_PREPAY_METER      = 0x0420, /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_ELECTRONIC_ATTACK_ON_THE_PREPAY_METER,             /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_DISCOUNT_APPLIED,                                  /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_CREDIT_ADJUSTMENT,                                 /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_CREDIT_ADJUST_FAIL,                                /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_DEBT_ADJUSTMENT,                                   /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_DEBT_ADJUST_FAIL,                                  /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_MODE_CHANGE,                                       /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_TOPUP_CODE_ERROR,                                  /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_TOPUP_ALREADY_USED,                                /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_TOPUP_CODE_INVALID,                                /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_TOPUP_ACCEPTED_VIA_REMOTE,                         /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_TOPUP_ACCEPTED_VIA_MANUAL_ENTRY,                   /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_FRIENDLY_CREDIT_IN_USE,                            /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_FRIENDLY_CREDIT_PERIOD_END_WARNING,                /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_FRIENDLY_CREDIT_PERIOD_END,                        /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_PREPAYMENT_ERROR_REG_CLEAR,                        /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_PREPAYMENT_ALARM_REG_CLEAR,                        /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_PREPAY_CLUSTER_NOT_FOUND,                          /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_TOPUP_VALUE_TOO_LARGE,                             /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_MODE_CREDIT_TO_PREPAY                    = 0x0441, /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_MODE_PREPAY_TO_CREDIT,                             /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_MODE_DEFAULT,                                      /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_SELECT_AVAILABLE_EMERGENCY_CREDIT_RECEIVED = 0x04C0, /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_SELECT_AVAILABLE_EMERGENCY_CREDIT_ACTIONED,          /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_SELECT_AVAILABLE_EMERGENCY_CREDIT_CANCELLED,         /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_SELECT_AVAILABLE_EMERGENCY_CREDIT_REJECTED,          /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_CHANGE_DEBT_RECEIVED,                              /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_CHANGE_DEBT_ACTIONED,                              /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_CHANGE_DEBT_CANCELLED,                             /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_CHANGE_DEBT_REJECTED,                              /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_EMERGENCY_CREDIT_SETUP_RECEIVED,                   /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_EMERGENCY_CREDIT_SETUP_ACTIONED,                   /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_EMERGENCY_CREDIT_SETUP_CANCELLED,                  /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_EMERGENCY_CREDIT_SETUP_REJECTED,                   /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_CONSUMER_TOPUP_RECEIVED,                           /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_CONSUMER_TOPUP_ACTIONED,                           /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_CONSUMER_TOPUP_CANCELLED,                          /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_CONSUMER_TOPUP_REJECTED,                           /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_CREDIT_ADJUSTMENT_RECEIVED,                        /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_CREDIT_ADJUSTMENT_ACTIONED,                        /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_CREDIT_ADJUSTMENT_CANCELLED,                       /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_CREDIT_ADJUSTMENT_REJECTED,                        /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_CHANGE_PAYMENT_MODE_RECEIVED,                      /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_CHANGE_PAYMENT_MODE_ACTIONED,                      /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_CHANGE_PAYMENT_MODE_CANCELLED,                     /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_CHANGE_PAYMENT_MODE_REJECTED,                      /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_GET_PREPAY_SNAPSHOT_RECEIVED,                      /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_GET_PREPAY_SNAPSHOT_ACTIONED,                      /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_GET_PREPAY_SNAPSHOT_CANCELLED,                     /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_GET_PREPAY_SNAPSHOT_REJECTED,                      /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_GET_TOPUP_LOG_RECEIVED,                            /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_GET_TOPUP_LOG_ACTIONED,                            /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_GET_TOPUP_LOG_CANCELLED,                           /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_GET_TOPUP_LOG_REJECTED,                            /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_SET_LOW_CREDIT_WARNING_LEVEL_RECEIVED,             /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_SET_LOW_CREDIT_WARNING_LEVEL_ACTIONED,             /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_SET_LOW_CREDIT_WARNING_LEVEL_CANCELLED,            /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_SET_LOW_CREDIT_WARNING_LEVEL_REJECTED,             /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_GET_DEBT_REPAY_LOG_RECEIVED,                       /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_GET_DEBT_REPAY_LOG_ACTIONED,                       /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_GET_DEBT_REPAY_LOG_CANCELLED,                      /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_GET_DEBT_REPAY_LOG_REJECTED,                       /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_SET_MAXIMUM_CREDIT_LIMIT_RECEIVED,                 /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_SET_MAXIMUM_CREDIT_LIMIT_ACTIONED,                 /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_SET_MAXIMUM_CREDIT_LIMIT_CANCELLED,                /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_SET_MAXIMUM_CREDIT_LIMIT_REJECTED,                 /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_SET_OVERALL_DEBT_CAP_RECEIVED,                     /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_SET_OVERALL_DEBT_CAP_ACTIONED,                     /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_SET_OVERALL_DEBT_CAP_CANCELLED,                    /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_SET_OVERALL_DEBT_CAP_REJECTED,                     /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_RESERVED_FOR_PREPAYMENT_CLUSTER_GROUP_ID = 0x04FF, /* (O) */
} zb_zcl_device_management_prepayment_event_cfg_attr_set_t;


/** @brief Calendar Event Configuration Attribute Set.
 *  These attributes allow events related to calendars to be configured.
 *  @see SE spec, subclause 10.3.2.6
 */
typedef enum zb_zcl_device_management_calendar_event_cfg_attr_set_e
{
  /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_CALENDAR_CLUSTER_NOT_FOUND               = 0x0500,  /**< Calendar Cluster Not Found */
  /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_CALENDAR_CHANGE_PASSIVE_ACTIVATED,                  /**< Calendar Change Passive Activated */
  /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_CALENDAR_CHANGE_PASSIVE_UPDATED,                    /**< Calendar Change Passive Updated */
  /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_PUBLISH_CALENDAR_RECEIVED                = 0x05C0,  /**< PublishCalendar Received */
  /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_PUBLISH_CALENDAR_ACTIONED,                          /**< PublishCalendar Actioned */
  /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_PUBLISH_CALENDAR_CANCELLED,                         /**< PublishCalendar Cancelled */
  /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_PUBLISH_CALENDAR_REJECTED,                          /**< PublishCalendar Rejected */
  /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_PUBLISH_DAY_PROFILE_RECEIVED,                       /**< Publish Day Profile Received */
  /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_PUBLISH_DAY_PROFILE_ACTIONED,                       /**< Publish Day Profile Actioned */
  /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_PUBLISH_DAY_PROFILE_CANCELLED,                      /**< Publish Day Profile Cancelled */
  /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_PUBLISH_DAY_PROFILE_REJECTED,                       /**< Publish Day Profile Rejected */
  /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_PUBLISH_WEEK_PROFILE_RECEIVED,                      /**< Publish Week Profile Received */
  /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_PUBLISH_WEEK_PROFILE_ACTIONED,                      /**< Publish Week Profile Actioned */
  /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_PUBLISH_WEEK_PROFILE_CANCELLED,                     /**< Publish Week Profile Cancelled */
  /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_PUBLISH_WEEK_PROFILE_REJECTED,                      /**< Publish Week Profile Rejected */
  /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_PUBLISH_SEASONS_PROFILE_RECEIVED,                   /**< Publish Seasons Received */
  /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_PUBLISH_SEASONS_PROFILE_ACTIONED,                   /**< Publish Seasons Actioned */
  /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_PUBLISH_SEASONS_PROFILE_CANCELLED,                  /**< Publish Seasons Cancelled */
  /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_PUBLISH_SEASONS_PROFILE_REJECTED,                   /**< Publish Seasons Rejected */
  /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_PUBLISH_SPECIAL_DAYS_PROFILE_RECEIVED,              /**< Publish Special Days Received */
  /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_PUBLISH_SPECIAL_DAYS_PROFILE_ACTIONED,              /**< Publish Special Days Actioned */
  /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_PUBLISH_SPECIAL_DAYS_PROFILE_CANCELLED,             /**< Publish Special Days Cancelled */
  /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_PUBLISH_SPECIAL_DAYS_PROFILE_REJECTED,              /**< Publish Special Days Rejected */
  /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_RESERVED_FOR_CALENDAR_CLUSTER_GROUP_ID   = 0x05FF,  /**< Reserved for Calendar cluster Group ID */
} zb_zcl_device_management_calendar_event_cfg_attr_set_t;


/** @brief Device Management Event Configuration Attribute Set.
 *  These attributes allow events related to device management to be configured.
 *  @see SE spec, subclause 10.3.2.7
 */
typedef enum zb_zcl_device_management_dev_mgmt_event_cfg_attr_set_e
{
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_PASSWORD_1_CHANGE                        = 0x0600, /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_PASSWORD_2_CHANGE,                                 /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_PASSWORD_3_CHANGE,                                 /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_PASSWORD_4_CHANGE,                                 /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_EVENT_LOG_CLEARED,                                 /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_ZB_APS_TIMEOUT                           = 0x0610, /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_ZB_IEEE_TRANSMISS_FAILURE_OVER_THRESHOLD,          /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_ZB_IEEE_FRAME_CHECK_SEQUENCE_THRESHOLD,            /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_ERROR_CERTIFICATE,                                 /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_ERROR_SIGNATURE,                                   /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_ERROR_PROGRAM_STORAGE,                             /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_PUBLISH_COT_RECEIVED                     = 0x06C0, /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_PUBLISH_COT_ACTIONED,                              /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_PUBLISH_COT_CANCELLED,                             /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_PUBLISH_COT_REJECTED,                              /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_PUBLISH_COS_RECEIVED,                              /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_PUBLISH_COS_ACTIONED,                              /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_PUBLISH_COS_CANCELLED,                             /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_PUBLISH_COS_REJECTED,                              /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_CHANGE_PASSWORD_RECEIVED,                          /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_CHANGE_PASSWORD_ACTIONED,                          /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_CHANGE_PASSWORD_CANCELLED,                         /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_CHANGE_PASSWORD_REJECTED,                          /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_SET_EVENT_CONFIGURATION_RECEIVED,                  /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_SET_EVENT_CONFIGURATION_ACTIONED,                  /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_SET_EVENT_CONFIGURATION_CANCELLED,                 /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_SET_EVENT_CONFIGURATION_REJECTED,                  /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_UPDATE_SITE_ID_RECEIVED,                           /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_UPDATE_SITE_ID_ACTIONED,                           /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_UPDATE_SITE_ID_CANCELLED,                          /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_UPDATE_SITE_ID_REJECTED,                           /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_UPDATE_CIN_RECEIVED,                               /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_UPDATE_CIN_ACTIONED,                               /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_UPDATE_CIN_CANCELLED,                              /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_UPDATE_CIN_REJECTED,                               /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_RESERVED_FOR_DEV_MGMT_CLUSTER_GROUP_ID   = 0x06FF, /* (O) */
} zb_zcl_device_management_dev_mgmt_event_cfg_attr_set_t;


/** @brief Tunnel Event Configuration Attribute Set.
 *  These attributes allow events related to tunnel to be configured.
 *  @see SE spec, subclause 10.3.2.8
 */
typedef enum zb_zcl_device_management_tunnel_event_cfg_attr_set_e
{
  /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_TUNNELING_CLUSTER_NOT_FOUND              = 0x0700,  /**< Tunneling Cluster Not Found */
  /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_UNSUPPORTED_PROTOCOL,                               /**< Unsupported Protocol */
  /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_INCORRECT_PROTOCOL,                                 /**< IncorrectProtocol */
  /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_REQUEST_TUNNEL_COMMAND_RECEIVED          = 0x07C0,  /**< RequestTunnel Command Received */
  /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_REQUEST_TUNNEL_COMMAND_REJECTED,                    /**< RequestTunnel Command Rejected */
  /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_REQUEST_TUNNEL_COMMAND_GENERATED,                   /**< RequestTunnel Command Generated */
  /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_CLOSE_TUNNEL_COMMAND_RECEIVED,                      /**< CloseTunnel Command Received */
  /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_CLOSE_TUNNEL_COMMAND_REJECTED,                      /**< CloseTunnel Command Rejected */
  /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_CLOSE_TUNNEL_COMMAND_GENERATED,                     /**< CloseTunnel Command Generated */
  /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_TRANSFER_DATA_COMMAND_RECEIVED,                     /**< TransferData Command Received */
  /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_TRANSFER_DATA_COMMAND_REJECTED,                     /**< TransferData Command Rejected */
  /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_TRANSFER_DATA_COMMAND_GENERATED,                    /**< TransferData Command Generated */
  /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_TRANSFER_DATA_ERROR_COMMAND_RECEIVED,               /**< TransferDataError Command Received */
  /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_TRANSFER_DATA_ERROR_COMMAND_REJECTED,               /**< TransferDataError Command Rejected */
  /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_TRANSFER_DATA_ERROR_COMMAND_GENERATED,              /**< TransferDataError Command Generated */
  /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_ACK_TRANSFER_DATA_COMMAND_RECEIVED,                 /**< AckTransferData Command Received */
  /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_ACK_TRANSFER_DATA_COMMAND_REJECTED,                 /**< AckTransferData Command Rejected */
  /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_ACK_TRANSFER_DATA_COMMAND_GENERATED,                /**< AckTransferData Command Generated */
  /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_READY_DATA_COMMAND_RECEIVED,                        /**< ReadyData Command Received */
  /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_READY_DATA_COMMAND_REJECTED,                        /**< ReadyData Command Rejected */
  /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_READY_DATA_COMMAND_GENERATED,                       /**< ReadyData Command Generated */
  /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_GET_SUPP_TUNNEL_PROTOC_COMMAND_RECEIVED,            /**< GetSupportedTunnelProtocols Command Received */
  /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_GET_SUPP_TUNNEL_PROTOC_COMMAND_REJECTED,            /**< GetSupportedTunnelProtocols Command Rejected */
  /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_GET_SUPP_TUNNEL_PROTOC_COMMAND_GENERATED,           /**< GetSupportedTunnelProtocols Command Generated */
  /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_RESERVED_FOR_TUNNEL_CLUSTER_GROUP_ID     = 0x07FF,  /**< Reserved for Tunnel cluster Group ID */
} zb_zcl_device_management_tunnel_event_cfg_attr_set_t;


/** @brief OTA Event Configuration Attribute Set.
 *  These attributes allow events related to OTA to be configured.
 *  @see SE spec, subclause 10.3.2.9
 */
typedef enum zb_zcl_device_management_ota_event_cfg_attr_set_e
{
  /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_FIRMWARE_READY_FOR_ACTIVATION            = 0x0800,  /**< FirmwareReadyForActivation */
  /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_FIRMWARE_ACTIVATED,                                 /**< FirmwareActivated */
  /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_FIRMWARE_ACTIVATION_FAILURE,                        /**< Firmware Activation Failure */
  /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_PATCH_READY_FOR_ACTIVATION,                         /**< Patch Ready For Activation */
  /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_PATCH_ACTIVATED,                                    /**< Patch Activated */
  /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_PATCH_FAILURE,                                      /**< Patch Failure */
  /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_IMAGE_NOTIFY_COMMAND_RECEIVED            = 0x08C0,  /**< Image Notify Command Received */
  /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_IMAGE_NOTIFY_COMMAND_REJECTED,                      /**< Image Notify Command Rejected */
  /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_QUERY_NEXT_IMAGE_REQ_COMMAND_GENERATED,             /**< Query Next Image Request Generated */
  /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_QUERY_NEXT_IMAGE_RESP_COMMAND_RECEIVED,             /**< Query Next Image Response Received */
  /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_QUERY_NEXT_IMAGE_RESP_COMMAND_REJECTED,             /**< Query Next Image Response Rejected */
  /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_IMAGE_BLOCK_REQ_COMMAND_GENERATED,                  /**< Image Block Request Generated */
  /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_IMAGE_PAGE_REQ_COMMAND_GENERATED,                   /**< Image Page Request Generated */
  /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_IMAGE_BLOCK_RESP_COMMAND_RECEIVED,                  /**< Image Block Response Received */
  /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_IMAGE_BLOCK_RESP_COMMAND_REJECTED,                  /**< Image Block Response Rejected */
  /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_UPGRADE_END_REQ_COMMAND_GENERATED,                  /**< Upgrade End Request Generated */
  /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_UPGRADE_END_RESP_COMMAND_RECEIVED,                  /**< Upgrade End Response Received */
  /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_UPGRADE_END_RESP_COMMAND_REJECTED,                  /**< Upgrade End Response Rejected */
  /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_QUERY_SPECIF_FILE_REQ_COMMAND_GENERATED,            /**< Query Specific File Request Generated */
  /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_QUERY_SPECIF_FILE_RESP_COMMAND_RECEIVED,            /**< Query Specific File Response Received */
  /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_QUERY_SPECIF_FILE_RESP_COMMAND_REJECTED,            /**< Query Specific File Response Rejected */
  /* (O) */
  ZB_ZCL_ATTR_DEVICE_MANAGEMENT_CLI_RESERVED_FOR_OTA_CLUSTER_GROUP_ID        = 0x08FF,  /**< Reserved for  OTA cluster Group ID */
} zb_zcl_device_management_ota_event_cfg_attr_set_t;


/** @brief Event Configuration Bitmap definition. Enumerated part (Event Log type).
 *  @see SE spec, Table D-177
 */
typedef enum zb_zcl_device_management_event_cfg_enum_attr_e
{
  ZB_ZCL_DEVICE_MANAGEMENT_EVENT_CFG_DO_NOT_LOG            = 0,  /**< Do not Log */
  ZB_ZCL_DEVICE_MANAGEMENT_EVENT_CFG_LOG_AS_TAMPER         = 1,  /**< Log as Tamper */
  ZB_ZCL_DEVICE_MANAGEMENT_EVENT_CFG_LOG_AS_FAULT          = 2,  /**< Log as Fault */
  ZB_ZCL_DEVICE_MANAGEMENT_EVENT_CFG_LOG_AS_GENERAL_EVENT  = 3,  /**< Log as General Event */
  ZB_ZCL_DEVICE_MANAGEMENT_EVENT_CFG_LOG_AS_SECURITY_EVENT = 4,  /**< Log as Security Event */
  ZB_ZCL_DEVICE_MANAGEMENT_EVENT_CFG_LOG_AS_NETWORK_EVENT  = 5,  /**< Log as Network Event */
} zb_zcl_device_management_event_cfg_enum_attr_t;


/** @brief Event Configuration Bitmap definition. Bitmap part.
 *  @note This enumeration defines bit position (bit-shift) of specified value
 *  in bitmap
 *  @see SE spec, Table D-177
 */
typedef enum zb_zcl_device_management_event_cfg_bitmap_attr_e
{
  ZB_ZCL_DEVICE_MANAGEMENT_EVENT_CFG_PUSH_EVENT_TO_WAN    = 3,  /**< Push Event to WAN */
  ZB_ZCL_DEVICE_MANAGEMENT_EVENT_CFG_PUSH_EVENT_TO_HAN    = 4,  /**< Push Event to HAN */
  ZB_ZCL_DEVICE_MANAGEMENT_EVENT_CFG_RAISE_ALARM_ZIGBEE   = 5,  /**< Raise Alarm (Zigbee) */
  ZB_ZCL_DEVICE_MANAGEMENT_EVENT_CFG_RAISE_ALARM_PHYSICAL = 6,  /**< Raise Alarm (Physical i.e. audible/visible) */

  ZB_ZCL_DEVICE_MANAGEMENT_PUSH_EVENT_TO_WAN    = ZB_ZCL_DEVICE_MANAGEMENT_EVENT_CFG_PUSH_EVENT_TO_WAN,  /**< Deprecated */
  ZB_ZCL_DEVICE_MANAGEMENT_PUSH_EVENT_TO_HAN    = ZB_ZCL_DEVICE_MANAGEMENT_EVENT_CFG_PUSH_EVENT_TO_HAN,  /**< Deprecated */
  ZB_ZCL_DEVICE_MANAGEMENT_RAISE_ALARM_ZIGBEE   = ZB_ZCL_DEVICE_MANAGEMENT_EVENT_CFG_RAISE_ALARM_ZIGBEE,  /**< Deprecated */
  ZB_ZCL_DEVICE_MANAGEMENT_RAISE_ALARM_PHYSICAL = ZB_ZCL_DEVICE_MANAGEMENT_EVENT_CFG_RAISE_ALARM_PHYSICAL,  /**< Deprecated */
} zb_zcl_device_management_event_cfg_bitmap_attr_t;


/** @brief Event Configuration Bitmap definition.
 *  @see SE spec, Table D-177
 *
 *  The least-significant 3 bits of the Event Configuration bitmaps
 *  indicate how the event should be logged (enumerated part).
 *  @see zb_zcl_device_management_event_cfg_enum_attr_t enum.
 *
 *  zb_bitfield_t enum_part   :3;
 *
 *  The remaining bits provide options for treatment rules to be applied
 *  (bitmap part).
 *  @see zb_zcl_device_management_event_cfg_bitmap_attr_t bitmap.
 *
 *  zb_bitfield_t bitmap_part :5;
 *  @see zb_zcl_device_management_set_event_cfg_payload_t::zb_zcl_device_management_set_event_cfg_by_cfg_match_payload_s::event_cfg_match
 *  @see zb_zcl_device_management_set_event_cfg_cmd_payload_t::event_cfg
 */
typedef zb_uint8_t zb_zcl_device_management_event_cfg_attr_t;


/*  Macros that should be used for set/get the Event Configuration Bitmap value
 */
/** @brief Get Event Log type in the Event Configuration Bitmap (enumerated part).
 *  @see zb_zcl_device_management_event_cfg_enum_attr_t definition.
 *  @param x - @ref zb_zcl_device_management_event_cfg_attr_t value.
 *  @returns @ref zb_zcl_device_management_event_cfg_enum_attr_t value.
 */
#define ZB_ZCL_DEVICE_MANAGEMENT_EVENT_CFG_GET_LOG_TYPE(x)   ((x) & 0x7)

/** @brief Set Event Log type from the Event Configuration Bitmap (enumerated part).
 *  @see zb_zcl_device_management_event_cfg_enum_attr_t definition.
 *  @param x - @ref zb_zcl_device_management_event_cfg_attr_t value.
 *  @param y - @ref zb_zcl_device_management_event_cfg_enum_attr_t new value.
 */
#define ZB_ZCL_DEVICE_MANAGEMENT_EVENT_CFG_SET_LOG_TYPE(x,y) ((x) = ((x) & (~0x7)) | ((y) & 0x7))


/** @brief Get Push Event to WAN state from the Event Configuration Bitmap (bitmap part).
 *  @see zb_zcl_device_management_event_cfg_bitmap_attr_t definition.
 *  @param x - @ref zb_zcl_device_management_event_cfg_attr_t value.
 *  @returns true or false.
 */
#define ZB_ZCL_DEVICE_MANAGEMENT_EVENT_CFG_GET_PUSH_EVENT_TO_WAN(x)                                       \
               ZB_CHECK_BIT_IN_BIT_VECTOR((&(x)), ZB_ZCL_DEVICE_MANAGEMENT_EVENT_CFG_PUSH_EVENT_TO_WAN)

/** @brief Set Push Event to WAN state from the Event Configuration Bitmap (bitmap part).
 *  @see zb_zcl_device_management_event_cfg_bitmap_attr_t definition.
 *  @param x - @ref zb_zcl_device_management_event_cfg_attr_t value.
 */
#define ZB_ZCL_DEVICE_MANAGEMENT_EVENT_CFG_SET_PUSH_EVENT_TO_WAN(x)                                       \
               ZB_SET_BIT_IN_BIT_VECTOR((&(x)), ZB_ZCL_DEVICE_MANAGEMENT_EVENT_CFG_PUSH_EVENT_TO_WAN)

/** @brief Clear Push Event to WAN state from the Event Configuration Bitmap (bitmap part)
 *  @see zb_zcl_device_management_event_cfg_bitmap_attr_t definition.
 *  @param x - @ref zb_zcl_device_management_event_cfg_attr_t value.
 */
#define ZB_ZCL_DEVICE_MANAGEMENT_EVENT_CFG_CLR_PUSH_EVENT_TO_WAN(x)                                       \
               ZB_CLR_BIT_IN_BIT_VECTOR((&(x)), ZB_ZCL_DEVICE_MANAGEMENT_EVENT_CFG_PUSH_EVENT_TO_WAN)


/** @brief Get Push Event to HAN state from the Event Configuration Bitmap (bitmap part).
 *  @see zb_zcl_device_management_event_cfg_bitmap_attr_t definition.
 *  @param x - @ref zb_zcl_device_management_event_cfg_attr_t value.
 *  @returns true or false.
 */
#define ZB_ZCL_DEVICE_MANAGEMENT_EVENT_CFG_GET_PUSH_EVENT_TO_HAN(x)                                       \
               ZB_CHECK_BIT_IN_BIT_VECTOR((&(x)), ZB_ZCL_DEVICE_MANAGEMENT_EVENT_CFG_PUSH_EVENT_TO_HAN)

/** @brief Set Push Event to HAN state from the Event Configuration Bitmap (bitmap part).
 *  @see zb_zcl_device_management_event_cfg_bitmap_attr_t definition.
 *  @param x - @ref zb_zcl_device_management_event_cfg_attr_t value.
 */
#define ZB_ZCL_DEVICE_MANAGEMENT_EVENT_CFG_SET_PUSH_EVENT_TO_HAN(x)                                       \
               ZB_SET_BIT_IN_BIT_VECTOR((&(x)), ZB_ZCL_DEVICE_MANAGEMENT_EVENT_CFG_PUSH_EVENT_TO_HAN)

/** @brief Clear Push Event to HAN state from the Event Configuration Bitmap (bitmap part).
 *  @see zb_zcl_device_management_event_cfg_bitmap_attr_t definition.
 *  @param x - @ref zb_zcl_device_management_event_cfg_attr_t value.
 */
#define ZB_ZCL_DEVICE_MANAGEMENT_EVENT_CFG_CLR_PUSH_EVENT_TO_HAN(x)                                       \
               ZB_CLR_BIT_IN_BIT_VECTOR((&(x)), ZB_ZCL_DEVICE_MANAGEMENT_EVENT_CFG_PUSH_EVENT_TO_HAN)


/** @brief Get Raise alarm Zigbee state from the Event Configuration Bitmap (bitmap part).
 *  @see zb_zcl_device_management_event_cfg_bitmap_attr_t definition.
 *  @param x - @ref zb_zcl_device_management_event_cfg_attr_t value.
 *  @returns true or false.
 */
#define ZB_ZCL_DEVICE_MANAGEMENT_EVENT_CFG_GET_RAISE_ALARM_ZIGBEE(x)                                      \
               ZB_CHECK_BIT_IN_BIT_VECTOR((&(x)), ZB_ZCL_DEVICE_MANAGEMENT_EVENT_CFG_RAISE_ALARM_ZIGBEE)

/** @brief Set Raise alarm Zigbee state from the Event Configuration Bitmap (bitmap part).
 *  @see zb_zcl_device_management_event_cfg_bitmap_attr_t definition.
 *  @param x - @ref zb_zcl_device_management_event_cfg_attr_t value.
 */
#define ZB_ZCL_DEVICE_MANAGEMENT_EVENT_CFG_SET_RAISE_ALARM_ZIGBEE(x)                                      \
               ZB_SET_BIT_IN_BIT_VECTOR((&(x)), ZB_ZCL_DEVICE_MANAGEMENT_EVENT_CFG_RAISE_ALARM_ZIGBEE)

/** @brief Clear Raise alarm Zigbee state from the Event Configuration Bitmap (bitmap part).
 *  @see zb_zcl_device_management_event_cfg_bitmap_attr_t definition.
 *  @param x - @ref zb_zcl_device_management_event_cfg_attr_t value.
 */
#define ZB_ZCL_DEVICE_MANAGEMENT_EVENT_CFG_CLR_RAISE_ALARM_ZIGBEE(x)                                      \
               ZB_CLR_BIT_IN_BIT_VECTOR((&(x)), ZB_ZCL_DEVICE_MANAGEMENT_EVENT_CFG_RAISE_ALARM_ZIGBEE)


/** @brief Get Raise alarm Physical state from the Event Configuration Bitmap (bitmap part).
 *  @see zb_zcl_device_management_event_cfg_bitmap_attr_t definition.
 *  @param x - @ref zb_zcl_device_management_event_cfg_attr_t value.
 *  @returns true or false.
 */
#define ZB_ZCL_DEVICE_MANAGEMENT_EVENT_CFG_GET_RAISE_ALARM_PHYSICAL(x)                                     \
               ZB_CHECK_BIT_IN_BIT_VECTOR((&(x)), ZB_ZCL_DEVICE_MANAGEMENT_EVENT_CFG_RAISE_ALARM_PHYSICAL)

/** @brief Set Raise alarm Physical state from the Event Configuration Bitmap (bitmap part).
 *  @see zb_zcl_device_management_event_cfg_bitmap_attr_t definition.
 *  @param x - @ref zb_zcl_device_management_event_cfg_attr_t value.
 */
#define ZB_ZCL_DEVICE_MANAGEMENT_EVENT_CFG_SET_RAISE_ALARM_PHYSICAL(x)                                     \
               ZB_SET_BIT_IN_BIT_VECTOR((&(x)), ZB_ZCL_DEVICE_MANAGEMENT_EVENT_CFG_RAISE_ALARM_PHYSICAL)

/** @brief Clear Raise alarm Physical state from the Event Configuration Bitmap (bitmap part).
 *  @see zb_zcl_device_management_event_cfg_bitmap_attr_t definition.
 *  @param x - @ref zb_zcl_device_management_event_cfg_attr_t value.
 */
#define ZB_ZCL_DEVICE_MANAGEMENT_EVENT_CFG_RAISE_ALARM_PHYSICAL(x)                                         \
               ZB_CLR_BIT_IN_BIT_VECTOR((&(x)), ZB_ZCL_DEVICE_MANAGEMENT_EVENT_CFG_RAISE_ALARM_PHYSICAL)

/** @} */ /* ZB_ZCL_DEVICE_MANAGEMENT_ATTRS */

/** @defgroup ZB_ZCL_DEVICE_MANAGEMENT_COMMANDS Device Management cluster commands
 * @{
 */

/** @brief Commands are generated by Device Management Client.
 *  @see SE spec, subclause 10.2.3
 */
typedef enum zb_zcl_device_management_cli_cmd_e
{
  /* (O) */
  ZB_ZCL_DEVICE_MANAGEMENT_CLI_CMD_GET_CHANGE_OF_TENANCY     = 0x00,   /**< This command is used to request the ESI to respond with information
                                                                        *  regarding any available change of tenancy.
                                                                        *  @note There are no payloads for this command
                                                                        */
  /* (O) */
  ZB_ZCL_DEVICE_MANAGEMENT_CLI_CMD_GET_CHANGE_OF_SUPPLIER,             /**< This command is used to request the ESI to respond with information
                                                                        *  regarding any available change of supplier.
                                                                        *  @note There are no payloads for this command
                                                                        */
  /* (O) */
  ZB_ZCL_DEVICE_MANAGEMENT_CLI_CMD_REQUEST_NEW_PASSWORD,                /**< This command is used to request the current Password from the server.
                                                                         * @see zb_zcl_device_management_request_new_password_cmd_payload_t
                                                                         */
  /* (O) */
  ZB_ZCL_DEVICE_MANAGEMENT_CLI_CMD_GET_SITE_ID,                         /**< This command is used to request the ESI to respond with information
                                                                         *  regarding any pending change of Site ID.
                                                                         *  @note There are no payloads for this command
                                                                         */
  /* (O) */
  ZB_ZCL_DEVICE_MANAGEMENT_CLI_CMD_REPORT_EVENT_CONFIGURATION,          /**< This command is sent in response to a GetEventConfiguration command.
                                                                         * @see zb_zcl_device_management_report_event_cfg_payload_t
                                                                         */
  /* (O) */
  ZB_ZCL_DEVICE_MANAGEMENT_CLI_CMD_GET_CIN,                             /**< This command is used to request the ESI to respond with information
                                                                         *  regarding any pending change of Customer ID Number.
                                                                         *  @note There are no payloads for this command
                                                                         */
} zb_zcl_device_management_cli_cmd_t;


/** @brief Configuration Control Enumeration
 *  @see SE spec, Table D-173
 *  @see zb_zcl_device_management_set_event_cfg_cmd_payload_t::cfg_control
 */
typedef enum zb_zcl_device_management_cfg_control_e
{
  ZB_ZCL_DEVICE_MANAGEMENT_APPLY_BY_LIST         = 0x00,  /**< Apply by List */
  ZB_ZCL_DEVICE_MANAGEMENT_APPLY_BY_EVENT_GROUP  = 0x01,  /**< Apply by Event Group */
  ZB_ZCL_DEVICE_MANAGEMENT_APPLY_BY_LOG_TYPE     = 0x02,  /**< Apply by Log Type */
  ZB_ZCL_DEVICE_MANAGEMENT_APPLY_BY_CONFIG_MATCH = 0x03,  /**< Apply by Configuration Match */
} zb_zcl_device_management_cfg_control_t;


/** @brief Password Type Enumeration
 *  @see SE spec, Table D-172
 */
typedef enum zb_zcl_device_management_password_type_e
{
  ZB_ZCL_DEVICE_MANAGEMENT_PASSWORD_TYPE_1                   = 0x01,  /**< Used for access to the Service menu */

  ZB_ZCL_DEVICE_MANAGEMENT_PASSWORD_TYPE_2                   = 0x02,  /**< Used for access to the Consumer menu */

  ZB_ZCL_DEVICE_MANAGEMENT_PASSWORD_TYPE_3                   = 0x03,  /**< TBD */

  ZB_ZCL_DEVICE_MANAGEMENT_PASSWORD_TYPE_4                   = 0x04,  /**< TBD */
} zb_zcl_device_management_password_type_t;


/** @brief @ref ZB_ZCL_DEVICE_MANAGEMENT_CLI_CMD_REQUEST_NEW_PASSWORD "RequestNewPassword" Command Payload Format
 *  @see SE spec, subclause 10.2.3.3
 */
typedef ZB_PACKED_PRE struct zb_zcl_device_management_request_new_password_cmd_payload_s
{
  /** The Password Type field represents requested password.
   *  @see zb_zcl_device_management_password_type_t enum.
   */
  zb_uint8_t password_type;                                                /* (M) */
} ZB_PACKED_STRUCT zb_zcl_device_management_request_new_password_cmd_payload_t;


/** @brief @e ReportEventConfiguration Payload Format
 *  @see SE spec, Figure D-157
 *  @see zb_zcl_device_management_report_event_cfg_cmd_payload_t::event_cfg_payload
 */
typedef ZB_PACKED_PRE struct zb_zcl_device_management_report_event_cfg_payload_s
{
  /** The Event ID is the attribute ID of the Event Configuration attribute.
   */
  zb_uint16_t event_id;                                                    /* (M) */

  /** The configuration bitmap applicable to the event.
   * @see zb_zcl_device_management_report_event_cfg_cmd_payload_t::event_cfg_payload
   */
  zb_zcl_device_management_event_cfg_attr_t  event_cfg;                    /* (M) */
} ZB_PACKED_STRUCT zb_zcl_device_management_report_event_cfg_payload_t;


/** @brief @ref ZB_ZCL_DEVICE_MANAGEMENT_CLI_CMD_REPORT_EVENT_CONFIGURATION "ReportEventConfigurationCommandPayloadFormat"
 *  @see SE spec, subclause 10.2.3.5
 */
typedef ZB_PACKED_PRE struct zb_zcl_device_management_report_event_cfg_cmd_payload_s
{
  /** The Command Index is used to count the payload fragments in the case where
   *  the entire payload does not fit into one message.
   */
  zb_uint8_t command_index;                                                /* (M) */

  /** This parameter holds the total number of responses.
   */
  zb_uint8_t total_commands;                                               /* (M) */

  /** The log payload is a series of events, in time sequential order.
   * @see zb_zcl_device_management_report_event_cfg_payload_t
   */
  zb_zcl_device_management_report_event_cfg_payload_t *event_cfg_payload;  /* (M) */

  /* An 8-bit integer representing the number of event entries in this command
   */
   zb_uint8_t number_of_entries_in_this_command;                       /* (ZBOSS) */
} ZB_PACKED_STRUCT zb_zcl_device_management_report_event_cfg_cmd_payload_t;


/** @brief Commands are generated by Device Management Server.
 *  @see SE spec, subclause 10.2.4
 */
typedef enum zb_zcl_device_management_srv_cmd_e
{
  /* (O) */
  ZB_ZCL_DEVICE_MANAGEMENT_SRV_CMD_PUBLISH_CHANGE_OF_TENANCY    = 0x00, /**< This command is used to change the tenancy of a meter.
                                                                         * @see zb_zcl_device_management_publish_change_of_tenancy_cmd_payload_t
                                                                         */

  /* (O) */
  ZB_ZCL_DEVICE_MANAGEMENT_SRV_CMD_PUBLISH_CHANGE_OF_SUPPLIER,          /**< This command is used to change the Supplier (commodity provider) that
                                                                         *  is supplying the property.
                                                                         *  @see zb_zcl_device_management_publish_change_of_supplier_cmd_payload_t
                                                                         */

  /* (O) */
  ZB_ZCL_DEVICE_MANAGEMENT_SRV_CMD_REQUEST_NEW_PASSWORD_RESPONSE,       /**< This command is used to send the current password to the client.
                                                                         * @see zb_zcl_device_management_request_new_password_resp_cmd_payload_t
                                                                         */

  /* (O) */
  ZB_ZCL_DEVICE_MANAGEMENT_SRV_CMD_UPDATE_SITE_ID,                      /**< This command is used to set the SiteID attribute on a meter.
                                                                         * @see zb_zcl_device_management_update_site_id_cmd_payload_t
                                                                         */
  /* (O) */
  ZB_ZCL_DEVICE_MANAGEMENT_SRV_CMD_SET_EVENT_CONFIGURATION,             /**< This command provides a method to set the event configuration attributes,
                                                                         *  held in a client device.
                                                                         *  @see zb_zcl_device_management_set_event_cfg_cmd_payload_t
                                                                         */

  /* (O) */
  ZB_ZCL_DEVICE_MANAGEMENT_SRV_CMD_GET_EVENT_CONFIGURATION,             /**< This command allows the server to request details of event configurations.
                                                                         * @see zb_zcl_device_management_get_event_cfg_cmd_payload_t
                                                                         */

  /* (O) */
  ZB_ZCL_DEVICE_MANAGEMENT_SRV_CMD_UPDATE_CIN,                          /**< This command is used to set the CustomerIDNumber attribute held in
                                                                         *  the Metering cluster.
                                                                         *  @see zb_zcl_device_management_update_cin_cmd_payload_t
                                                                         */
} zb_zcl_device_management_srv_cmd_t;


/** @brief The Proposed Provider Name field should be capable of storing a 16 character
 *  string (the first octet indicates length) encoded in the UTF-8 format.
 */
#define ZB_ZCL_DEVICE_MANAGEMENT_PROP_PROVIDER_NAME_MAX_SIZE         17

/** @brief The Proposed Provider Contact Details field should be capable of storing a 19
 *  character string (the first octet indicates length) encoded in the UTF-8 format.
 */
#define ZB_ZCL_DEVICE_MANAGEMENT_PROP_PROVIDER_CONTACT_DET_MAX_SIZE  20

/** @brief The Password field should be capable of storing a 11 character string
 *  (the first octet indicates length).
 */
#define ZB_ZCL_DEVICE_MANAGEMENT_PASSWORD_NAME_MAX_SIZE              11

/** @brief The Site ID field should be capable of storing a 32 character string
 *  (the first Octet indicates length) encoded in UTF-8 format.
 */
#define ZB_ZCL_DEVICE_MANAGEMENT_SITE_ID_MAX_SIZE                    33

/** @brief The Customer ID field should be capable of storing a 24 character string
 *  (the first Octet indicates length) encoded in UTF-8 format.
 */
#define ZB_ZCL_DEVICE_MANAGEMENT_CUSTOMER_ID_MAX_SIZE                25


/** @brief Tariff Type Enumeration
 *  @see SE spec, Table D-108
 */
typedef enum zb_zcl_device_management_tariff_type_e
{
  ZB_ZCL_DEVICE_MANAGEMENT_TARIFF_TYPE_DELIVERED                 = 0x00,  /**< Delivered Tariff */
  ZB_ZCL_DEVICE_MANAGEMENT_TARIFF_TYPE_RECEIVED                  = 0x01,  /**< Received Tariff */
  ZB_ZCL_DEVICE_MANAGEMENT_TARIFF_TYPE_DELIVERED_AND_RECEIVED    = 0x02,  /**< Delivered and Received Tariff */
} zb_zcl_device_management_tariff_type_t;


/** @brief @ref ZB_ZCL_DEVICE_MANAGEMENT_SRV_CMD_PUBLISH_CHANGE_OF_TENANCY "PublishChangeOfTenancy" Command Payload Format
 *  @see SE spec, subclause 10.2.4.1
 */
typedef ZB_PACKED_PRE struct zb_zcl_device_management_publish_change_of_tenancy_cmd_payload_s
{
  /** The Provider ID field represents a unique identifier for the commodity
   *  provider to whom this command relates.
   */
  zb_uint32_t provider_id;                                                 /* (M) */

  /** The Issuer Event ID field represents a unique identifier generated
   *  by the commodity provider.
   */
  zb_uint32_t issuer_event_id;                                             /* (M) */

  /** The Tariff Type field represents the type of tariff published in this
   *  command.
   *  @see zb_zcl_device_management_tariff_type_t enum.
   */
  zb_uint8_t  tariff_type;                                                 /* (M) */

  /** The Implementation Date/Time field represents a UTC Time field to
   *  indicate the date from which the change of tenancy is to be applied.
   */
  zb_uint32_t implement_time;                                              /* (M) */

  /** The Proposed Tenancy Change Control field represents the functions
   *  that are required to be carried out on processing of this command.
   *  @see zb_zcl_device_management_proposed_change_ctrl_attr_t enum.
   */
  zb_uint32_t proposed_change_ctrl;                                        /* (M) */

} ZB_PACKED_STRUCT zb_zcl_device_management_publish_change_of_tenancy_cmd_payload_t;


/** @brief @ref ZB_ZCL_DEVICE_MANAGEMENT_SRV_CMD_PUBLISH_CHANGE_OF_SUPPLIER "PublishChangeOfSupplier" Command Payload Format
 *  @see SE spec, subclause 10.2.4.2
 */
typedef ZB_PACKED_PRE struct zb_zcl_device_management_publish_change_of_supplier_cmd_payload_s
{
  /** The Current Provider ID field represents a unique identifier for the current
   *  commodity provider to whom this command relates.
   */
  zb_uint32_t cur_provider_id;                                             /* (M) */

  /** The Issuer Event ID field represents a unique identifier generated
   *  by the commodity provider.
   */
  zb_uint32_t issuer_event_id;                                             /* (M) */

  /** The Tariff Type field represents the type of tariff published in this
   *  command.
   *  @see zb_zcl_device_management_tariff_type_t enum.
   */
  zb_uint8_t  tariff_type;                                                 /* (M) */

  /** The Proposed Provider ID field represents a unique identifier for the
   *  commodity provider associated with the proposed change to the supply.
   */
  zb_uint32_t prop_provider_id;                                            /* (M) */

  /** The Provider Change Implementation Time field represents a UTC Time
   *  to indicate the date/time at which a proposed change to the provider
   *  is to be implemented.
   */
  zb_uint32_t implement_time;                                              /* (M) */

  /** The Provider Change Control field represents the functions that are
   *  required to be carried out on processing of this command.
   *  @see zb_zcl_device_management_proposed_change_ctrl_attr_t enum.
   */
  zb_uint32_t provider_change_ctrl;                                        /* (M) */

  /** The Proposed Provider Name field represents the name of the new commodity
   *  provider (see max size below).
   *  @see ZB_ZCL_DEVICE_MANAGEMENT_PROP_PROVIDER_NAME_MAX_SIZE
   */

  zb_uint8_t  prop_provider_name[ZB_ZCL_DEVICE_MANAGEMENT_PROP_PROVIDER_NAME_MAX_SIZE];              /* (M) */

  /** The Proposed Provider Contact Details field represents the contact
   *  details of the new commodity provider (see max size below).
   *  @see ZB_ZCL_DEVICE_MANAGEMENT_PROP_PROVIDER_CONTACT_DET_MAX_SIZE
   */
  zb_uint8_t  prop_provider_contact_det[ZB_ZCL_DEVICE_MANAGEMENT_PROP_PROVIDER_CONTACT_DET_MAX_SIZE];/* (M) */
} ZB_PACKED_STRUCT zb_zcl_device_management_publish_change_of_supplier_cmd_payload_t;


/** @brief @ref ZB_ZCL_DEVICE_MANAGEMENT_SRV_CMD_GET_EVENT_CONFIGURATION "RequestNewPasswordResponse" Command Payload Format
 *  @see SE spec, subclause 10.2.4.3
 */
typedef ZB_PACKED_PRE struct zb_zcl_device_management_request_new_password_resp_cmd_payload_s
{
  /** The Issuer Event ID field represents a unique identifier generated
   *  by the commodity provider.
   */
  zb_uint32_t issuer_event_id;                                             /* (M) */

  /** The Implementation Date/Time field represents a UTC Time indicate
   *  the date at which the originating command was to be applied.
   */
  zb_uint32_t implement_time;                                              /* (M) */

  /** The Duration in minutes field represents the duration in minutes
   *  that the password is valid for.
   */
  zb_uint16_t duration;                                                    /* (M) */

  /** The Password Type field represents which password should be changed.
   *  @see zb_zcl_device_management_password_type_t enum.
   */
  zb_uint8_t  password_type;                                               /* (M) */

  /** The Password field represents the password (the first octet is
   *  the length, allowing 10 octets for the password).
   *  @see ZB_ZCL_DEVICE_MANAGEMENT_PASSWORD_NAME_MAX_SIZE
   */
  zb_uint8_t  password[ZB_ZCL_DEVICE_MANAGEMENT_PASSWORD_NAME_MAX_SIZE];   /* (M) */
} ZB_PACKED_STRUCT zb_zcl_device_management_request_new_password_resp_cmd_payload_t;


/** @brief @ref ZB_ZCL_DEVICE_MANAGEMENT_SRV_CMD_UPDATE_SITE_ID "UpdateSiteID" Command Payload Format
 *  @see SE spec, subclause 10.2.4.4
 */
typedef ZB_PACKED_PRE struct zb_zcl_device_management_update_site_id_cmd_payload_s
{
  /** The Issuer Event ID field represents a unique identifier generated
   *  by the commodity provider.
   */
  zb_uint32_t  issuer_event_id;                                            /* (M) */

  /** The SiteID Time field represents a UTC time at which the update of SiteID
   *  will take place.
   */
  zb_uint32_t  site_id_time;                                               /* (M) */

  /** The Provider ID field represents a unique identifier for the commodity
   *  provider to whom this command relates.
   */
  zb_uint32_t  provider_id;                                                /* (M) */

  /** The Site ID field represents the Site ID (see max size below).
   * @see ZB_ZCL_DEVICE_MANAGEMENT_SITE_ID_MAX_SIZE
   */

  zb_uint32_t  site_id[ZB_ZCL_DEVICE_MANAGEMENT_SITE_ID_MAX_SIZE];         /* (M) */
} ZB_PACKED_STRUCT zb_zcl_device_management_update_site_id_cmd_payload_t;


/** @brief @e SetEventConfiguration Payload Format
 *  @see SE spec, subclauses 10.2.4.5.2.1 to 10.2.4.5.2.4
 *  @see zb_zcl_device_management_set_event_cfg_cmd_payload_t::event_cfg_payload
 */
typedef ZB_PACKED_PRE union zb_zcl_device_management_set_event_cfg_payload_s
{
  /** Event Configuration Payload Format if Configuration Control field
   *  value is set to Apply by List.
   */
  ZB_PACKED_PRE struct zb_zcl_device_management_set_event_cfg_by_list_payload_s
  {
    /** The Number of Events field represents the number of events contained
     * within the command.
     */
    zb_uint8_t    event_num;                                              /* (M) */

    /** The Event ID field represents the attribute ID of the event
     *  configuration attribute.
     */
    zb_uint16_t  *event_id;                                               /* (M) */
  } zb_zcl_device_management_set_event_cfg_by_list_payload_s;

  /** Event Configuration Payload Format if Configuration Control field
   *  value is set to Apply by Event Group.
   */
  ZB_PACKED_PRE struct zb_zcl_device_management_set_event_cfg_by_ev_group_payload_s
  {
    /** The Event Group field represents which attribute set the event
     *  belongs to.
     */
    zb_uint16_t   event_group;                                            /* (M) */
  } zb_zcl_device_management_set_event_cfg_by_ev_group_payload_s;

  /** Event Configuration Payload Format if Configuration Control field
   *  value is set to Apply by Log type.
   */
  ZB_PACKED_PRE struct zb_zcl_device_management_set_event_cfg_by_log_type_payload_s
  {
    /** The Log ID field represents the log ID of events to be updated
     *  with the new Configuration  Value field passed in the command.
     */
    zb_uint8_t    log_id;                                                 /* (M) */
  } zb_zcl_device_management_set_event_cfg_by_log_type_payload_s;

  /** Event Configuration Payload Format if Configuration Control field
   *  value is set to Apply by Configuration Match.
   */
  ZB_PACKED_PRE struct zb_zcl_device_management_set_event_cfg_by_cfg_match_payload_s
  {
    /** The Configuration Value Match field represents that any configuration
     *  attribute which matches this value shall be assigned the new configuration
     *  value passed in the Event Configuration field of the main command payload.
     *  @see zb_zcl_device_management_event_cfg_attr_t
     */
    zb_zcl_device_management_event_cfg_attr_t  event_cfg_match;           /* (M) */
  } zb_zcl_device_management_set_event_cfg_by_cfg_match_payload_s;
} ZB_PACKED_STRUCT zb_zcl_device_management_set_event_cfg_payload_t;


/** @brief @ref ZB_ZCL_DEVICE_MANAGEMENT_SRV_CMD_SET_EVENT_CONFIGURATION "SetEventConfiguration" Command Payload Format
 *  @see SE spec, subclause 10.2.4.5
 */
typedef ZB_PACKED_PRE struct zb_zcl_device_management_set_event_cfg_cmd_payload_s
{
  /** The Issuer Event ID field represents a unique identifier generated by the
   *  commodity provider.
   */
  zb_uint32_t issuer_event_id;                                            /* (M) */

  /** The Start Date/Time field represents a UTC date and time at which the new
   *  configuration is to be applied.
   */
  zb_uint32_t start_time;                                                 /* (M) */

  /** The Event Configuration field represents the new event configuration to be
   *  applied.
   *  @see zb_zcl_device_management_event_cfg_attr_t
   */
  zb_zcl_device_management_event_cfg_attr_t event_cfg;                    /* (M) */

  /** The Configuration Control field allows the new configuration value to be
   *  applied to several events via a single command.
   *  @see zb_zcl_device_management_cfg_control_t enum.
   */
  zb_uint8_t cfg_control;                                                /* (M) */

  /** Set Event Configuration payload that depends on configuration control
   *  value.
   *  @see zb_zcl_device_management_set_event_cfg_payload_t
   */
  zb_zcl_device_management_set_event_cfg_payload_t *event_cfg_payload;    /* (M) */
} ZB_PACKED_STRUCT zb_zcl_device_management_set_event_cfg_cmd_payload_t;


/** @brief @ref ZB_ZCL_DEVICE_MANAGEMENT_SRV_CMD_GET_EVENT_CONFIGURATION "GetEventConfigurationCommand" Payload Format
 *  @see SE spec, subclause 10.2.4.6
 */
typedef ZB_PACKED_PRE struct zb_zcl_device_management_get_event_cfg_cmd_payload_s
{
  /** The Event ID field represents particular event to be queried.
   */
  zb_uint16_t event_id;                                                   /* (M) */
} ZB_PACKED_STRUCT zb_zcl_device_management_get_event_cfg_cmd_payload_t;


/** @brief @ref ZB_ZCL_DEVICE_MANAGEMENT_SRV_CMD_UPDATE_CIN "UpdateCIN" Command Payload Format
 *  @see SE spec, subclause 10.2.4.7
 */
typedef ZB_PACKED_PRE struct zb_zcl_device_management_update_cin_cmd_payload_s
{
  /** The Issuer Event ID field represents a unique identifier generated
   *  by the commodity provider.
   */
  zb_uint32_t issuer_event_id;                                            /* (M) */

  /** The CIN Implementation Time field represents a UTC date and time at which
   *  the updated CustomerIDNumber will become active.
   */
  zb_uint32_t implement_time;                                             /* (M) */

  /** The Provider ID field represents a unique identifier for the commodity
   *  provider to whom this command relates.
   */
  zb_uint32_t provider_id;                                                /* (M) */

  /** The CustomerID Number field the Customer ID Number (see max size below).
   * @see ZB_ZCL_DEVICE_MANAGEMENT_CUSTOMER_ID_MAX_SIZE
   */
  zb_uint8_t  customer_id[ZB_ZCL_DEVICE_MANAGEMENT_CUSTOMER_ID_MAX_SIZE]; /* (M) */
} ZB_PACKED_STRUCT zb_zcl_device_management_update_cin_cmd_payload_t;

/** @} */ /* ZB_ZCL_DEVICE_MANAGEMENT_COMMANDS */

/** @} */ /* ZCL Device Management cluster definitions */

/** @endcond */ /* DOXYGEN_ZCL_SECTION */

void zb_zcl_device_management_init_server(void);
void zb_zcl_device_management_init_client(void);
#define ZB_ZCL_CLUSTER_ID_DEVICE_MANAGEMENT_SERVER_ROLE_INIT (zb_zcl_cluster_init_t)zb_zcl_device_management_init_server
#define ZB_ZCL_CLUSTER_ID_DEVICE_MANAGEMENT_CLIENT_ROLE_INIT (zb_zcl_cluster_init_t)zb_zcl_device_management_init_client

#endif /* ZB_ZCL_DEVICE_MANAGEMENT_H_ */
