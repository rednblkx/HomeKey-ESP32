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
/* PURPOSE: Prepayment cluster definitions
*/

#ifndef ZB_ZCL_PREPAYMENT_H_
#define ZB_ZCL_PREPAYMENT_H_

#include "zboss_api_core.h" /* ZBOSS core API types */

/** @cond DOXYGEN_ZCL_SECTION && DOXYGEN_SE_SECTION */

/** @addtogroup ZB_ZCL_PREPAYMENT
  * @{
  *   @details
  *   The Prepayment Cluster provides the facility to pass messages relating to
  *   the accounting functionality of a meter between devices on the HAN. It allows
  *   for the implementation of a system conforming to the set of standards relating
  *   to Payment Electricity Meters (IEC 62055) and also for the case where the
  *   accounting function is remote from the meter. Prepayment issued in situations
  *   where the supply of a service may be interrupted or enabled under the control
  *   of the meter or system in relation to a payment tariff. The accounting
  *   process may be within the meter or elsewhere in the system. The amount of
  *   available credit is decremented as the service is consumed and is incremented
  *   through payments made by the consumer. Such a system allows the consumer
  *   to better manage their energy consumption and reduces the risk of bad debt
  *   owing to the supplier.
  */


/** @defgroup ZB_ZCL_PREPAYMENT_ATTRS Prepayment cluster attributes
 * @{
 */

/** @brief Default value for Prepayment cluster revision global attribute */
#define ZB_ZCL_PREPAYMENT_CLUSTER_REVISION_DEFAULT ((zb_uint16_t)0x0001u)

/** Prepayment Server Attribute Sets
 *  @see SE spec, subclause 7.2.2
 */
typedef enum zb_zcl_prepayment_attr_sets_e
{
  /** The following set of attributes provides access to the standard information
   * relating to a Prepayment meter.
   */
  ZB_ZCL_PREPAYMENT_SET_PREPAYMENT_INFORMATION = 0x00,

  /** The following set of attributes provides access to previous successful
   * credit top-ups on a prepayment meter. #1 is the most recent, based on
   * time.
   */
  ZB_ZCL_PREPAYMENT_SET_TOP_UP,

  /** This set of attributes provides access to information on debt held on a
   *  Prepayment meter.
   */
  ZB_ZCL_PREPAYMENT_SET_DEBT,

  /** The following set of attributes provides a means to control which
   * prepayment alarms may be generated from the meter.
   */
  ZB_ZCL_PREPAYMENT_SET_ALARM = 0x04,

  /** Historical Cost Consumption Information Attribute Set */
  ZB_ZCL_PREPAYMENT_SET_HISTORICAL_COST_CONSUMPTION_INFORMATION,
} zb_zcl_prepayment_attr_sets_t;


/** The Prepayment Information Attribute Set provides access to the
 *  standard information relating to a Prepayment meter.
 *  @see SE spec, subclause 7.2.2.1
 */
typedef enum zb_zcl_prepayment_prepayment_information_attr_set_e
{

  /** The PaymentControlConfiguration attribute represents the payment mechanisms
   * currently enabled within the Metering Device.
   */
  ZB_ZCL_ATTR_PREPAYMENT_PAYMENT_CONTROL_CONFIGURATION = 0x0000,  /* (M) */

  /** The Credit Remaining attribute represents the amount of credit remaining
   * on the Metering Device.
   */
  ZB_ZCL_ATTR_PREPAYMENT_CREDIT_REMAINING,                        /* (O) */

  /** The Emergency Credit Remaining attribute represents the amount of
   * Emergency Credit still available on the Metering Device.
   */
  ZB_ZCL_ATTR_PREPAYMENT_EMERGENCY_CREDIT_REMAINING,              /* (O) */

  /** The Credit Status attribute represents the current status of credit within
   * the Metering Device.
   */
  ZB_ZCL_ATTR_PREPAYMENT_CREDIT_STATUS,                          /* (O) */

  /** The UTC time at which the Credit Remaining attribute was last populated. */
  ZB_ZCL_ATTR_PREPAYMENT_CREDIT_REMAINING_TIMESTAMP,              /* (O) */

  /** The AccumulatedDebt attribute represents the total amount of debt remaining
   *  on the Metering Device.
   */
  ZB_ZCL_ATTR_PREPAYMENT_ACCUMULATED_DEBT,                        /* (O) */

  /** The OverallDebtCap attribute represents the total amount of debt that can
   * be taken from top-ups (in the case of multiple instantiated top-up based debts
   * on the Metering Device).
   */
  ZB_ZCL_ATTR_PREPAYMENT_OVERALL_DEBT_CAP,                        /* (O) */

  /** The EmergencyCreditLimit/Allowance attribute may be updated by the utility
   * company. This is the amount of Emergency Credit available to loan to the
   * consumer when the remaining balance goes below the low credit threshold.
   */
  ZB_ZCL_ATTR_PREPAYMENT_EMERGENCY_CREDIT_LIMIT = 0x0010,         /* (O) */

  /** When credit (or emergency credit) falls below this threshold, an alarm is
   * raised to warn the consumer of imminent supply interruption and, if available,
   * to offer Emergency Credit.
   */
  ZB_ZCL_ATTR_PREPAYMENT_EMERGENCY_CREDIT_THRESHOLD,              /* (O) */

  /** An unsigned 48-bit integer value indicating running total of credit topped
   * up to date.
   */
  ZB_ZCL_ATTR_PREPAYMENT_TOTAL_CREDIT_ADDED = 0x0020,             /* (O) */

  /** An unsigned 32-bit integer value indicating the maximum credit balance
   * allowed on a meter.
   */
  ZB_ZCL_ATTR_PREPAYMENT_MAX_CREDIT_LIMIT,                        /* (O) */

  /** An unsigned 32-bit integer value indicating the maximum credit per top-up.
   * Any single top-up greater than this threshold will cause the meter to reject
   * the top-up
   */
  ZB_ZCL_ATTR_PREPAYMENT_MAX_CREDIT_PER_TOP_UP,                   /* (O) */

  /** An unsigned 8-bit integer value indicating the amount of time, in minutes,
   * before the Friendly Credit Period End Warning alarm flag is triggered.
   */
  ZB_ZCL_ATTR_PREPAYMENT_FRIENDLY_CREDIT_WARNING = 0x0030,        /* (O) */

  /** An unsigned 32-bit integer that defines the utility low credit value below
   * which the Low Credit warning should sound.
   */
  ZB_ZCL_ATTR_PREPAYMENT_LOW_CREDIT_WARNING,                      /* (O) */

  /** An unsigned 32-bit integer that is defined by the consumer for a low credit
   * value below which a Low Credit warning should sound.
   */
  ZB_ZCL_ATTR_PREPAYMENT_IHD_LOW_CREDIT_WARNING,                  /* (O) */

  /** When the end of a configured non-disconnect period is reached and the
   *  supply is to be interrupted due to insufficient credit being available,
   *  the meter will provide visual and audible alerts and the interruption will be
   *  suspended for a further period of minutes defined by this attribute.
   */
  ZB_ZCL_ATTR_PREPAYMENT_INTERRUPT_SUSPEND_TIME,                  /* (O) */

  /** An unsigned 16-bit integer value indicating the amount of time remaining,
   * in minutes, in a currently active Friendly Credit period.
   */
  ZB_ZCL_ATTR_PREPAYMENT_REMAINING_FRIENDLY_CREDIT_TIMES,         /* (O) */

  /** The UTC time at which the next Friendly Credit period is due to commence. */
  ZB_ZCL_ATTR_PREPAYMENT_NEXT_FRIENDLY_CREDIT_PERIOD,             /* (O) */

  /** This attribute is a signed 32-bit integer that shall either be zero or a
   * negative value (in all known cases).
   */
  ZB_ZCL_ATTR_PREPAYMENT_CUT_OFF_VALUE = 0x0040,                  /* (O) */

  /** The TokenCarrierId attribute provides a method for utilities to publish the
   * payment card number that is used with this meter set.
   */
  ZB_ZCL_ATTR_PREPAYMENT_TOKEN_CARRIER_ID = 0x0080,               /* (O) */
} zb_zcl_prepayment_prepayment_information_attr_set_t;

/** @endcond */ /* DOXYGEN_ZCL_SECTION && DOXYGEN_SE_SECTION */
/** @cond internals_doc */
#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PREPAYMENT_PAYMENT_CONTROL_CONFIGURATION(data_ptr) \
{                                                                                             \
  ZB_ZCL_ATTR_PREPAYMENT_PAYMENT_CONTROL_CONFIGURATION,                                       \
  ZB_ZCL_ATTR_TYPE_16BITMAP,                                                                  \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                                               \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                                         \
  (void*) data_ptr                                                                            \
}
/** @endcond */ /* internals_doc */
/** @cond DOXYGEN_ZCL_SECTION && DOXYGEN_SE_SECTION */

/** @brief Declare attribute list for Prepayment cluster
    @param attr_list - attribute list name
    @param payment_control_configuration - pointer to variable to store Payment Control Configuration attribute value
*/
#define ZB_ZCL_DECLARE_PREPAYMENT_ATTRIB_LIST(attr_list, payment_control_configuration)                  \
  ZB_ZCL_START_DECLARE_ATTRIB_LIST_CLUSTER_REVISION(attr_list, ZB_ZCL_PREPAYMENT)                             \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_PREPAYMENT_PAYMENT_CONTROL_CONFIGURATION, (payment_control_configuration)) \
  ZB_ZCL_FINISH_DECLARE_ATTRIB_LIST

/**
 *  @brief Prepayment cluster Mandatory attributes
 */
typedef struct zb_zcl_prepayment_attrs_s
{
  /** @see ZB_ZCL_PREPAYMENT_PAYMENT_CONTROL_CONFIGURATION
  */
  zb_uint16_t payment_control_configuration;
} zb_zcl_prepayment_attrs_t;

/** Declare attribute list for Prepayment cluster
 * @param[in]  attr_list - attribute list variable name
 * @param[in]  attrs - variable of @ref zb_zcl_prepayment_attrs_t type (containing Prepaylent cluster attributes)
 */
#define ZB_ZCL_DECLARE_PREPAYMENT_ATTR_LIST(attr_list, attrs)                  \
  ZB_ZCL_DECLARE_PREPAYMENT_ATTRIB_LIST(attr_list, &attrs.payment_control_configuration)


/** The PaymentControlConfiguration attribute represents the payment mechanisms
 *  currently enabled within the Metering Device.
 *  @see SE spec, Table D-130
 */
typedef enum zb_zcl_prepayment_payment_control_configuration_attr_e
{

  /** Indicates whether the metering device is to disconnect the energy supply on
   * expiry of available credit.
   */
  PREPAYMENT_DISCONNECTION_ENABLED                   = 1 << 0,

  /** Indicates if the meter is a prepayment meter; if this value is 0,
   * the meter is considered to be a credit meter.
   */
  PREPAYMENT_PREPAYMENT_ENABLED                      = 1 << 1,

  /** Indicates whether the metering device should manage accounting functionality
   * according to available tariff information.
   */
  PREPAYMENT_CREDIT_MANAGEMENT_ENABLED               = 1 << 2,

  /** Indicates whether the metering device should display the credit status. */
  PREPAYMENT_CREDIT_DISPLAY_ENABLED                  = 1 << 4,

  /** Indicates whether the metering device is running in Monetary (0) or
   * Unit based (1) units.
   */
  PREPAYMENT_ACCOUNT_BASE                            = 1 << 5,

  /** Indicates whether the metering device is fitted with a Contactor
   * i.e. is capable if disconnecting the energy supply.
   */
  PREPAYMENT_CONTACTOR_FITTED                        = 1 << 7,

  /** Indicates whether the standing charge collection is halted when the prepaid
   * credit is exhausted.
   */
  PREPAYMENT_STANDING_CHARGE_CONFIGURATION           = 1 << 8,

  /** Indicates whether the standing charge collection is halted when the device
   * is in Emergency Credit mode.
   */
  PREPAYMENT_EMERGENCY_STANDING_CHARGE_CONFIGURATION = 1 << 9,

  /** Indicates whether the debt collection is halted when the prepaid credit is
   * exhausted.
   */
  PREPAYMENT_DEBT_CONFIGURATION                      = 1 << 10,

  /** Indicates whether the debt is collected when the device is in Emergency
   * Credit mode.
   */
  PREPAYMENT_EMERGENCY_DEBT_CONFIGURATION            = 1 << 11,
} zb_zcl_prepayment_payment_control_configuration_attr_t;


/** The Credit Status attribute represents the current status of credit within
 *  the Metering Device.
 *  @see SE spec, Table D-131
 */
typedef enum zb_zcl_prepayment_credit_status_attr_e
{
    ZB_ZCL_PREPAYMENT_CREDIT_STATUS_CREDIT_OK                  = 1 << 0,  /**< Credit Ok */
    ZB_ZCL_PREPAYMENT_CREDIT_STATUS_LOW_CREDIT                 = 1 << 1,  /**< Low Credit */
    ZB_ZCL_PREPAYMENT_CREDIT_STATUS_EMERGENCY_CREDIT_ENABLED   = 1 << 2,  /**< Emergency Credit Enabled */
    ZB_ZCL_PREPAYMENT_CREDIT_STATUS_EMERGENCY_CREDIT_AVAILABLE = 1 << 3,  /**< Emergency Credit Available */
    ZB_ZCL_PREPAYMENT_CREDIT_STATUS_EMERGENCY_CREDIT_SELECTED  = 1 << 4,  /**< Emergency Credit Selected */
    ZB_ZCL_PREPAYMENT_CREDIT_STATUS_EMERGENCY_CREDIT_IN_USE    = 1 << 5,  /**< Emergency Credit In Use */
    ZB_ZCL_PREPAYMENT_CREDIT_STATUS_CREDIT_EXHAUSTED           = 1 << 6,  /**< Credit Exhausted */
} zb_zcl_prepayment_credit_status_attr_t;


/** The Top-up Attribute Set provides access to previous successful
 *  credit top-ups on a prepayment meter. #1 is the most recent, based on time.
 *  @see SE spec, subclause 7.2.2.2
 */
typedef enum zb_zcl_prepayment_top_up_attr_set_e
{
  /** The Top up Date/Time attribute represents the time that the credit was
   * topped up on the Metering Device. There are five records containing this
   * attribute, one for each of the last five top-ups.
   */
  ZB_ZCL_ATTR_PREPAYMENT_TOP_UP_TOP_UP_DATE_1 = 0x0100,  /* (O) */

  /** The Top up Amount attribute represents the amount of credit that was added
   * to the Metering Device during the top up.
   */
  ZB_ZCL_ATTR_PREPAYMENT_TOP_UP_TOP_UP_AMOUNT_1,         /* (O) */

  /** The Originating Device attribute represents the SE device that was the
   * source of the top-up command.
   */
  ZB_ZCL_ATTR_PREPAYMENT_TOP_UP_ORIGINATING_DEVICE_1,    /* (O) */

  /** The Top up Code attribute represents any encrypted number that was used to
   * apply the credit to the meter; the octet string shall be as it was received,
   * i.e. not decoded.
   */
  ZB_ZCL_ATTR_PREPAYMENT_TOP_UP_TOP_UP_CODE_1,           /* (O) */
  ZB_ZCL_ATTR_PREPAYMENT_TOP_UP_TOP_UP_DATE_2 = 0x0110,  /* (O) */
  ZB_ZCL_ATTR_PREPAYMENT_TOP_UP_TOP_UP_AMOUNT_2,         /* (O) */
  ZB_ZCL_ATTR_PREPAYMENT_TOP_UP_ORIGINATING_DEVICE_2,    /* (O) */
  ZB_ZCL_ATTR_PREPAYMENT_TOP_UP_TOP_UP_CODE_2,           /* (O) */
  ZB_ZCL_ATTR_PREPAYMENT_TOP_UP_TOP_UP_DATE_3 = 0x0120,  /* (O) */
  ZB_ZCL_ATTR_PREPAYMENT_TOP_UP_TOP_UP_AMOUNT_3,         /* (O) */
  ZB_ZCL_ATTR_PREPAYMENT_TOP_UP_ORIGINATING_DEVICE_3,    /* (O) */
  ZB_ZCL_ATTR_PREPAYMENT_TOP_UP_TOP_UP_CODE_3,           /* (O) */
  ZB_ZCL_ATTR_PREPAYMENT_TOP_UP_TOP_UP_DATE_4 = 0x0130,  /* (O) */
  ZB_ZCL_ATTR_PREPAYMENT_TOP_UP_TOP_UP_AMOUNT_4,         /* (O) */
  ZB_ZCL_ATTR_PREPAYMENT_TOP_UP_ORIGINATING_DEVICE_4,    /* (O) */
  ZB_ZCL_ATTR_PREPAYMENT_TOP_UP_TOP_UP_CODE_4,           /* (O) */
  ZB_ZCL_ATTR_PREPAYMENT_TOP_UP_TOP_UP_DATE_5 = 0x0140,  /* (O) */
  ZB_ZCL_ATTR_PREPAYMENT_TOP_UP_TOP_UP_AMOUNT_5,         /* (O) */
  ZB_ZCL_ATTR_PREPAYMENT_TOP_UP_ORIGINATING_DEVICE_5,    /* (O) */
  ZB_ZCL_ATTR_PREPAYMENT_TOP_UP_TOP_UP_CODE_5,           /* (O) */
} zb_zcl_prepayment_top_up_attr_set_t;

/** Debt Attribute Set provides access to information on debt held on a
 * Prepayment meter.
 *  @see SE spec, subclause 7.2.2.3
 */
typedef enum zb_zcl_prepayment_debt_attr_set_e
{
  /** The DebtLabel#n attribute provides a method for utilities to assign a name
   * to a particular type of debt. The DebtLabel#n attribute is a ZCL Octet String
   * field capable of storing a 12 character string (the first Octet indicates length)
   * encoded in the UTF-8 format.
   */
  ZB_ZCL_ATTR_PREPAYMENT_DEBT_LABEL_1 = 0x0210,                       /* (O) */

  /** An unsigned 32-bit field to denote the amount of Debt remaining on the
   * Metering Device. This parameter shall be measured in base unit of Currency
   * with the decimal point located as indicated by the Trailing Digits field,
   * as defined in the Price Cluster.
   */
  ZB_ZCL_ATTR_PREPAYMENT_DEBT_AMOUNT_1,                               /* (O) */

  /** An enumerated attribute denoting the debt recovery method used
   * for this debt type
   */
  ZB_ZCL_ATTR_PREPAYMENT_DEBT_RECOVERY_METHOD_1,                      /* (O) */

  /** A UTC Time field to denote the time at which the debt collection should
   * start. This applies to all debt recovery methods.
   */
  ZB_ZCL_ATTR_PREPAYMENT_DEBT_RECOVERY_START_TIME_1,                  /* (O) */

  /** An unsigned 16-bit field denoting the time of day when the debt collection
   * takes place.
   */
  ZB_ZCL_ATTR_PREPAYMENT_DEBT_RECOVERY_COLLECTION_TIME_1,             /* (O) */

  /** The DebtRecoveryFrequency#N attribute represents the period over which each
   * DebtRecoveryAmount#N is recovered.
   */
  ZB_ZCL_ATTR_PREPAYMENT_DEBT_RECOVERY_FREQUENCY_1 = 0x0216,          /* (O) */

  /** The DebtRecoveryAmount#N attribute represents the amount of Debt recovered
   * each period specified by DebtRecoveryFrequency#N, measured in base unit of
   * Currency with the decimal point located as indicated by the Trailing Digits
   * field, as defined in the Price Cluster.
   */
  ZB_ZCL_ATTR_PREPAYMENT_DEBT_RECOVERY_AMOUNT_1,                      /* (O) */

  /** An unsigned 16-bit field used in Percentage based recovery to denote the
   * percentage from a top- up amount to be deducted from the debt.
   */
  ZB_ZCL_ATTR_PREPAYMENT_DEBT_RECOVERY_TOP_UP_PERCENTAGE_1 = 0x0219,  /* (O) */
  ZB_ZCL_ATTR_PREPAYMENT_DEBT_LABEL_2 = 0x0220,                       /* (O) */
  ZB_ZCL_ATTR_PREPAYMENT_DEBT_AMOUNT_2,                               /* (O) */
  ZB_ZCL_ATTR_PREPAYMENT_DEBT_RECOVERY_METHOD_2,                      /* (O) */
  ZB_ZCL_ATTR_PREPAYMENT_DEBT_RECOVERY_START_TIME_2,                  /* (O) */
  ZB_ZCL_ATTR_PREPAYMENT_DEBT_RECOVERY_COLLECTION_TIME_2,             /* (O) */
  ZB_ZCL_ATTR_PREPAYMENT_DEBT_RECOVERY_FREQUENCY_2 = 0x0226,          /* (O) */
  ZB_ZCL_ATTR_PREPAYMENT_DEBT_RECOVERY_AMOUNT_2,                      /* (O) */
  ZB_ZCL_ATTR_PREPAYMENT_DEBT_RECOVERY_TOP_UP_PERCENTAGE_2 = 0x0229,  /* (O) */
  ZB_ZCL_ATTR_PREPAYMENT_DEBT_LABEL_3 = 0x0230,                       /* (O) */
  ZB_ZCL_ATTR_PREPAYMENT_DEBT_AMOUNT_3,                               /* (O) */
  ZB_ZCL_ATTR_PREPAYMENT_DEBT_RECOVERY_METHOD_3,                      /* (O) */
  ZB_ZCL_ATTR_PREPAYMENT_DEBT_RECOVERY_START_TIME_3,                  /* (O) */
  ZB_ZCL_ATTR_PREPAYMENT_DEBT_RECOVERY_COLLECTION_TIME_3,             /* (O) */
  ZB_ZCL_ATTR_PREPAYMENT_DEBT_RECOVERY_FREQUENCY_3 =0x0236,           /* (O) */
  ZB_ZCL_ATTR_PREPAYMENT_DEBT_RECOVERY_AMOUNT_3,                      /* (O) */
  ZB_ZCL_ATTR_PREPAYMENT_DEBT_RECOVERY_TOP_UP_PERCENTAGE_3 = 0x0239,  /* (O) */
} zb_zcl_prepayment_debt_attr_set_t;

/** Debt Recovery Method Enumerations
 *  @see SE spec, Table D-135
 */
typedef enum zb_zcl_prepayment_debt_recovery_method_e
{
  ZB_ZCL_PREPAYMENT_TIME_BASED = 0x00,  /**< Time based */
  ZB_ZCL_PREPAYMENT_PERCENTAGE_BASED,   /**< Percentage based */
  ZB_ZCL_PREPAYMENT_CATCH_UP_BASED      /**< Catch UP based */
} zb_zcl_prepayment_debt_recovery_method_t;


/** Recovery Frequency Field Enumerations
 *  @see SE spec, Table D-136
 */
typedef enum zb_zcl_prepayment_recovery_frequency_e
{
  ZB_ZCL_PREPAYMENT_PER_HOUR = 0x00,  /**< Per Hour */
  ZB_ZCL_PREPAYMENT_PER_DAY,          /**< Per Day */
  ZB_ZCL_PREPAYMENT_PER_WEEK,         /**< Per Week */
  ZB_ZCL_PREPAYMENT_PER_MONTH,        /**< Per Month */
  ZB_ZCL_PREPAYMENT_PER_QUARTER       /**< Per Quarter */
} zb_zcl_prepayment_recovery_frequency_t;


/** The Alarms Attribute Set provides a means to control which prepayment alarms
 *  may be generated from the meter.
 *  @see SE spec, subclause 7.2.2.5
 */
typedef enum zb_zcl_prepayment_alarms_attr_set_e
{

  /** The PrepaymentAlarmStatus attribute provides indicators reflecting the current
   *  error conditions found by the prepayment metering device.
   */
  ZB_ZCL_ATTR_PREPAYMENT_PREPAYMENT_ALARM_STATUS = 0x0400,  /* (O) */

  /** The Alarm Mask attributes of the Alarms Attribute Set specify whether each
   *  of the alarms listed in the corresponding alarm group in Table D-139 through
   *  Table D-142 is enabled.
   */
  ZB_ZCL_ATTR_PREPAYMENT_PREPAY_GENERIC_ALARM_MASK,         /* (O) */

  /**  */
  ZB_ZCL_ATTR_PREPAYMENT_PREPAY_SWITCH_ALARM_MASK,          /* (O) */

  /**  */
  ZB_ZCL_ATTR_PREPAYMENT_PREPAY_EVENT_ALARM_MASK            /* (O) */
} zb_zcl_prepayment_alarms_attr_set_t;


/** Prepayment Alarm Status Indicators
 *  @see SE spec, Table D-138
 */
typedef enum zb_zcl_prepayment_prepayment_alarm_status_indicators_e
{

  /** An alarm triggered by a configured threshold. */
  ZB_ZCL_PREPAYMENT_ALARM_LOW_CREDIT_WARNING                 = 1 << 0,

  /** The Top up code has been sent but it is too long or short for the meter */
  ZB_ZCL_PREPAYMENT_ALARM_TOP_UP_CODE_ERROR                  = 1 << 1,

  /** The Top up code has been sent but the credit value for this top up code
   * has already been applied and this is a duplicate request.
   */
  ZB_ZCL_PREPAYMENT_ALARM_TOP_UP_CODE_ALREADY_USED           = 1 << 2,

  /** The Top up code is a correct length but is not a valid top up code. */
  ZB_ZCL_PREPAYMENT_ALARM_TOP_UP_CODE_INVALID                = 1 << 3,

  /** The meter is in a Friendly Credit period and Friendly Credit is being used
   * due to no actual credit being available on the meter.
   */
  ZB_ZCL_PREPAYMENT_ALARM_FRIENDLY_CREDIT_IN_USE             = 1 << 4,

  /** This is triggered when the time remaining in a Friendly Credit period
   * falls below the value of the FriendlyCreditWarning attribute (default 1hr)
   * and the above Friendly Credit In Use flag is set.
   */
  ZB_ZCL_PREPAYMENT_ALARM_FRIENDLY_CREDIT_PERIOD_END_WARNING = 1 << 5,

  /** An alarm triggered when Emergency credit is available to be selected */
  ZB_ZCL_PREPAYMENT_ALARM_EC_AVAILABLE                       = 1 << 6,

  /** GAS: Valve Fault and unauthorised gas is being provided to the home
   * ELECTRICITY: Disconnection Fault and unauthorised electricity is
   * being provided to the house.
   */
  ZB_ZCL_PREPAYMENT_ALARM_UNAUTHORISED_ENERGY_USE            = 1 << 7,

  /** Supply has been disconnected due to no credit on meter. Cleared by
   * addition of credit or by selecting Emergency Credit
   */
  ZB_ZCL_PREPAYMENT_ALARM_DISCONNECTED_SUPPLY_DUE_TO_CREDIT  = 1 << 8,

  /** Supply has been disconnected due to a tamper detect on the meter. It can
   * also be due to a fault on the meter that is not covered by another flag.
   */
  ZB_ZCL_PREPAYMENT_ALARM_DISCONNECTED_SUPPLY_DUE_TO_TAMPER  = 1 << 9,

  /** This is normally due to the HES cutting the supply */
  ZB_ZCL_PREPAYMENT_ALARM_DISCONNECTED_SUPPLY_DUE_TO_HES     = 1 << 10,

  /** Physical attack on the Prepayment Meter */
  ZB_ZCL_PREPAYMENT_ALARM_PHYSICAL_ATTACK                    = 1 << 11,

  /** Electronic attack on the Prepayment Meter */
  ZB_ZCL_PREPAYMENT_ALARM_ELECTRONIC_ATTACK                  = 1 << 12,

  /** Manufacture Alarm Code A */
  ZB_ZCL_PREPAYMENT_ALARM_MANUFACTURE_ALARM_CODE_A           = 1 << 13,

  /** Manufacture Alarm Code B */
  ZB_ZCL_PREPAYMENT_ALARM_MANUFACTURE_ALARM_CODE_B           = 1 << 14
} zb_zcl_prepayment_prepayment_alarm_status_indicators_t;


/** PrepayGenericAlarmGroup
 *  @see SE spec, Table D-140
 */
typedef enum zb_zcl_prepayment_prepay_generic_alarm_group_e
{
  ZB_ZCL_PREPAYMENT_GROUP_LOW_CREDIT = 0x00,
  ZB_ZCL_PREPAYMENT_GROUP_NO_CREDIT,
  ZB_ZCL_PREPAYMENT_GROUP_CREDIT_EXHAUSTED,
  ZB_ZCL_PREPAYMENT_GROUP_EMERGENCY_CREDIT_ENABLED,
  ZB_ZCL_PREPAYMENT_GROUP_EMERGENCY_CREDIT_EXHAUSTED,
  ZB_ZCL_PREPAYMENT_GROUP_IHD_LOW_CREDIT_WARNING,
  ZB_ZCL_PREPAYMENT_GROUP_EVENT_LOG_CLEARED
} zb_zcl_prepayment_prepay_generic_alarm_group_t;


/** PrepaySwitchAlarmGroup
 *  @see SE spec, Table D-141
 */
typedef enum zb_zcl_prepayment_prepay_switch_alarm_group_e
{
  ZB_ZCL_PREPAYMENT_GROUP_SUPPLY_ON = 0x10,
  ZB_ZCL_PREPAYMENT_GROUP_SUPPLY_ARM,
  ZB_ZCL_PREPAYMENT_GROUP_SUPPLY_OFF,
  ZB_ZCL_PREPAYMENT_GROUP_DISCONNECTION_FAILURE,
  ZB_ZCL_PREPAYMENT_GROUP_DISCONNECTION_TAMPER_DETECTED,
  ZB_ZCL_PREPAYMENT_GROUP_DISCONNECTED_DUE_TO_CUT_OFF_VALUE,
  ZB_ZCL_PREPAYMENT_GROUP_REMOTE_DISCONNECTED
} zb_zcl_prepayment_prepay_switch_alarm_group_t;


/** PrepayEventAlarmGroup
 *  @see SE spec, Table D-142
 */
typedef enum zb_zcl_prepayment_prepay_event_alarm_group_e
{
  ZB_ZCL_PREPAYMENT_GROUP_PHYSICAL_ATTACK_ON_PREPAY_METER = 0x20,
  ZB_ZCL_PREPAYMENT_GROUP_ELECTRONIC_ATTACK_ON_PREPAY_METER,
  ZB_ZCL_PREPAYMENT_GROUP_DISCOUNT_APPLIED,
  ZB_ZCL_PREPAYMENT_GROUP_CREDIT_ADJUSTMENT,
  ZB_ZCL_PREPAYMENT_GROUP_CREDIT_ADJUSTMENT_FAIL,
  ZB_ZCL_PREPAYMENT_GROUP_DEBT_ADJUSTMENT,
  ZB_ZCL_PREPAYMENT_GROUP_DEBT_ADJUSTMENT_FAIL,
  ZB_ZCL_PREPAYMENT_GROUP_MODE_CHANGE,
  ZB_ZCL_PREPAYMENT_GROUP_TOP_UP_CODE_ERROR,
  ZB_ZCL_PREPAYMENT_GROUP_TOP_UP_ALREADY_USED,
  ZB_ZCL_PREPAYMENT_GROUP_TOP_UP_CODE_INVALID,
  ZB_ZCL_PREPAYMENT_GROUP_FRIENDLY_CREDIT_IN_USE,
  ZB_ZCL_PREPAYMENT_GROUP_FRIENDLY_CREDIT_PERIOD_END_WARNING,
  ZB_ZCL_PREPAYMENT_GROUP_FRIENDLY_CREDIT_PERIOD_END,
  ZB_ZCL_PREPAYMENT_GROUP_ERROR_REG_CLEAR = 0x30,
  ZB_ZCL_PREPAYMENT_GROUP_ALARM_REG_CLEAR,
  ZB_ZCL_PREPAYMENT_GROUP_PREPAY_CLUSTER_NOT_FOUND,
  ZB_ZCL_PREPAYMENT_GROUP_MODE_CREDIT_2_PREPAY = 0x41,
  ZB_ZCL_PREPAYMENT_GROUP_MODE_PREPAY_2_CREDIT,
  ZB_ZCL_PREPAYMENT_GROUP_MODE_DEFAULT
} zb_zcl_prepayment_prepay_event_alarm_group_t;


/** Historical Cost Consumption Information Set
 *  @see SE spec, subclause 7.2.2.6
 */
typedef enum zb_zcl_prepayment_historical_cost_consumption_information_set_e
{
  /** HistoricalCostConsumptionFormatting provides a method to properly decipher
   * the decimal point location for the values found in the Historical Cost
   * Consumption Set of attributes
   */
  ZB_ZCL_ATTR_PREPAYMENT_HISTORICAL_COST_CONSUMPTION_FORMATING = 0x0500,   /* (O) */

  /** ConsumptionUnitOfMeasurement provides a label for the Energy, Gas, or Water
   * being measured by the metering device.
   */
  ZB_ZCL_ATTR_PREPAYMENT_CONSUMPTION_UNIT_OF_MEASUREMENT,                   /* (O) */

  /** CurrencyScalingFactor provides a scaling factor for the Currency attribute
   * for the Energy, Gas, or Water being measured by the metering device.
   */
  ZB_ZCL_ATTR_PREPAYMENT_CURRENCY_SCALING_FACTOR,                           /* (O) */

  /** The Currency attribute provides the currency for the Energy, Gas, or Water
   * being measured by the prepayment device.
   */
  ZB_ZCL_ATTR_PREPAYMENT_CURRENCY,                                          /* (O) */

  /** It represents the summed value of Energy, Gas, or Water delivered to the
   * premises since the HFT.
   */
  ZB_ZCL_ATTR_PREPAYMENT_CURRENT_DAY_COST_CONSUMPTION_DELIVERED = 0x051C,   /* (O) */

  /** represents the summed value of Energy, Gas, or Water received from the
   * premises since the HFT.
   */
  ZB_ZCL_ATTR_PREPAYMENT_CURRENT_DAY_COST_CONSUMPTION_RECEIVED,             /* (O) */

  /** PreviousDayNCostConsumptionDelivered represents the summed value of Energy,
   * Gas, or Water delivered to the premises within the previous 24 hour period
   * starting at the HFT.
   */
  ZB_ZCL_ATTR_PREPAYMENT_PREVIOUS_DAY_COST_CONSUMPTION_DELIVERED,           /* (O) */

  /** PreviousDayNCostConsumptionReceived represents the summed value of Energy,
   * Gas, or Water received from the premises within the previous 24 hour period
   * starting at the HFT.
   */
  ZB_ZCL_ATTR_PREPAYMENT_PREVIOUS_DAY_COST_CONSUMPTION_RECEIVED,            /* (O) */
  ZB_ZCL_ATTR_PREPAYMENT_PREVIOUS_DAY_COST_CONSUMPTION_DELIVERED_2,         /* (O) */
  ZB_ZCL_ATTR_PREPAYMENT_PREVIOUS_DAY_COST_CONSUMPTION_RECEIVED_2,          /* (O) */
  ZB_ZCL_ATTR_PREPAYMENT_PREVIOUS_DAY_COST_CONSUMPTION_DELIVERED_3,         /* (O) */
  ZB_ZCL_ATTR_PREPAYMENT_PREVIOUS_DAY_COST_CONSUMPTION_RECEIVED_3,          /* (O) */
  ZB_ZCL_ATTR_PREPAYMENT_PREVIOUS_DAY_COST_CONSUMPTION_DELIVERED_4,         /* (O) */
  ZB_ZCL_ATTR_PREPAYMENT_PREVIOUS_DAY_COST_CONSUMPTION_RECEIVED_4,          /* (O) */
  ZB_ZCL_ATTR_PREPAYMENT_PREVIOUS_DAY_COST_CONSUMPTION_DELIVERED_5,         /* (O) */
  ZB_ZCL_ATTR_PREPAYMENT_PREVIOUS_DAY_COST_CONSUMPTION_RECEIVED_5,          /* (O) */
  ZB_ZCL_ATTR_PREPAYMENT_PREVIOUS_DAY_COST_CONSUMPTION_DELIVERED_6,         /* (O) */
  ZB_ZCL_ATTR_PREPAYMENT_PREVIOUS_DAY_COST_CONSUMPTION_RECEIVED_6,          /* (O) */
  ZB_ZCL_ATTR_PREPAYMENT_PREVIOUS_DAY_COST_CONSUMPTION_DELIVERED_7,         /* (O) */
  ZB_ZCL_ATTR_PREPAYMENT_PREVIOUS_DAY_COST_CONSUMPTION_RECEIVED_7,          /* (O) */
  ZB_ZCL_ATTR_PREPAYMENT_PREVIOUS_DAY_COST_CONSUMPTION_DELIVERED_8,         /* (O) */
  ZB_ZCL_ATTR_PREPAYMENT_PREVIOUS_DAY_COST_CONSUMPTION_RECEIVED_8,          /* (O) */

  /** CurrentWeekCostConsumptionDelivered represents the summed value of Energy,
   * Gas, or Water delivered to the premises since the HFT on Monday to the
   * last HFT read.
   */
  ZB_ZCL_ATTR_PREPAYMENT_CURRENT_WEEK_COST_CONSUMPTION_DELIVERED = 0x0530,  /* (O) */

  /** CurrentWeekCostConsumptionReceived represents the summed value of Energy,
   * Gas, or Water received from the premises since the HFT on Monday to the
   * last HFT read. */
  ZB_ZCL_ATTR_PREPAYMENT_CURRENT_WEEK_COST_CONSUMPTION_RECEIVED,            /* (O) */

  /** PreviousWeekNCostConsumptionDelivered represents the summed value of
   * Energy, Gas, or Water delivered to the premises within the previous week
   * period starting at the HFT on the Monday to the Sunday.
   */
  ZB_ZCL_ATTR_PREPAYMENT_PREVIOUS_WEEK_COST_CONSUMPTION_DELIVERED,          /* (O) */

  /** PreviousWeekNCostConsumptionReceived represents the summed value of Energy,
   *  Gas, or Water received from the premises within the previous week period
   * starting at the HFT on the Monday to the Sunday
   */
  ZB_ZCL_ATTR_PREPAYMENT_PREVIOUS_WEEK_COST_CONSUMPTION_RECEIVED,           /* (O) */
  ZB_ZCL_ATTR_PREPAYMENT_PREVIOUS_WEEK_COST_CONSUMPTION_DELIVERED_2,        /* (O) */
  ZB_ZCL_ATTR_PREPAYMENT_PREVIOUS_WEEK_COST_CONSUMPTION_RECEIVED_2,         /* (O) */
  ZB_ZCL_ATTR_PREPAYMENT_PREVIOUS_WEEK_COST_CONSUMPTION_DELIVERED_3,        /* (O) */
  ZB_ZCL_ATTR_PREPAYMENT_PREVIOUS_WEEK_COST_CONSUMPTION_RECEIVED_3,         /* (O) */
  ZB_ZCL_ATTR_PREPAYMENT_PREVIOUS_WEEK_COST_CONSUMPTION_DELIVERED_4,        /* (O) */
  ZB_ZCL_ATTR_PREPAYMENT_PREVIOUS_WEEK_COST_CONSUMPTION_RECEIVED_4,         /* (O) */
  ZB_ZCL_ATTR_PREPAYMENT_PREVIOUS_WEEK_COST_CONSUMPTION_DELIVERED_5,        /* (O) */
  ZB_ZCL_ATTR_PREPAYMENT_PREVIOUS_WEEK_COST_CONSUMPTION_RECEIVED_5,         /* (O) */

  /** CurrentMonthCostConsumptionDelivered represents the summed value of Energy,
   *  Gas, or Water delivered to the premises since the HFT on the 1 st of the
   * month to the last HFT read.
   */
  ZB_ZCL_ATTR_PREPAYMENT_CURRENT_MONTH_COST_CONSUMPTION_DELIVERED = 0x0540, /* (O) */

  /** CurrentMonthCostConsumptionReceived represents the summed value of Energy,
   * Gas, or Water received from the premises since the HFT on the 1 st of the
   * month to the last HFT read.
   */
  ZB_ZCL_ATTR_PREPAYMENT_CURRENT_MONTH_COST_CONSUMPTION_RECEIVED,           /* (O) */

  /** PreviousMonthNCostConsumptionDelivered represents the summed value of
   * Energy, Gas, or Water delivered to the premises within the previous Month
   * period starting at the HFT on the 1 st of the month to the last day of the
   * month.
   */
  ZB_ZCL_ATTR_PREPAYMENT_PREVIOUS_MONTH_COST_CONSUMPTION_DELIVERED,         /* (O) */

  /** PreviousMonthNCostConsumptionReceived represents the summed value of
   * Energy, Gas, or Water received from the premises within the previous month
   * period starting at the HFT on the 1 st of the month to the last day of the
   * month.
   */
  ZB_ZCL_ATTR_PREPAYMENT_PREVIOUS_MONTH_COST_CONSUMPTION_RECEIVED,          /* (O) */
  ZB_ZCL_ATTR_PREPAYMENT_PREVIOUS_MONTH_COST_CONSUMPTION_DELIVERED_2,       /* (O) */
  ZB_ZCL_ATTR_PREPAYMENT_PREVIOUS_MONTH_COST_CONSUMPTION_RECEIVED_2,        /* (O) */
  ZB_ZCL_ATTR_PREPAYMENT_PREVIOUS_MONTH_COST_CONSUMPTION_DELIVERED_3,       /* (O) */
  ZB_ZCL_ATTR_PREPAYMENT_PREVIOUS_MONTH_COST_CONSUMPTION_RECEIVED_3,        /* (O) */
  ZB_ZCL_ATTR_PREPAYMENT_PREVIOUS_MONTH_COST_CONSUMPTION_DELIVERED_4,       /* (O) */
  ZB_ZCL_ATTR_PREPAYMENT_PREVIOUS_MONTH_COST_CONSUMPTION_RECEIVED_4,        /* (O) */
  ZB_ZCL_ATTR_PREPAYMENT_PREVIOUS_MONTH_COST_CONSUMPTION_DELIVERED_5,       /* (O) */
  ZB_ZCL_ATTR_PREPAYMENT_PREVIOUS_MONTH_COST_CONSUMPTION_RECEIVED_5,        /* (O) */
  ZB_ZCL_ATTR_PREPAYMENT_PREVIOUS_MONTH_COST_CONSUMPTION_DELIVERED_6,       /* (O) */
  ZB_ZCL_ATTR_PREPAYMENT_PREVIOUS_MONTH_COST_CONSUMPTION_RECEIVED_6,        /* (O) */
  ZB_ZCL_ATTR_PREPAYMENT_PREVIOUS_MONTH_COST_CONSUMPTION_DELIVERED_7,       /* (O) */
  ZB_ZCL_ATTR_PREPAYMENT_PREVIOUS_MONTH_COST_CONSUMPTION_RECEIVED_7,        /* (O) */
  ZB_ZCL_ATTR_PREPAYMENT_PREVIOUS_MONTH_COST_CONSUMPTION_DELIVERED_8,       /* (O) */
  ZB_ZCL_ATTR_PREPAYMENT_PREVIOUS_MONTH_COST_CONSUMPTION_RECEIVED_8,        /* (O) */
  ZB_ZCL_ATTR_PREPAYMENT_PREVIOUS_MONTH_COST_CONSUMPTION_DELIVERED_9,       /* (O) */
  ZB_ZCL_ATTR_PREPAYMENT_PREVIOUS_MONTH_COST_CONSUMPTION_RECEIVED_9,        /* (O) */
  ZB_ZCL_ATTR_PREPAYMENT_PREVIOUS_MONTH_COST_CONSUMPTION_DELIVERED_10,      /* (O) */
  ZB_ZCL_ATTR_PREPAYMENT_PREVIOUS_MONTH_COST_CONSUMPTION_RECEIVED_10,       /* (O) */
  ZB_ZCL_ATTR_PREPAYMENT_PREVIOUS_MONTH_COST_CONSUMPTION_DELIVERED_11,      /* (O) */
  ZB_ZCL_ATTR_PREPAYMENT_PREVIOUS_MONTH_COST_CONSUMPTION_RECEIVED_11,       /* (O) */
  ZB_ZCL_ATTR_PREPAYMENT_PREVIOUS_MONTH_COST_CONSUMPTION_DELIVERED_12,      /* (O) */
  ZB_ZCL_ATTR_PREPAYMENT_PREVIOUS_MONTH_COST_CONSUMPTION_RECEIVED_12,       /* (O) */
  ZB_ZCL_ATTR_PREPAYMENT_PREVIOUS_MONTH_COST_CONSUMPTION_DELIVERED_13,      /* (O) */
  ZB_ZCL_ATTR_PREPAYMENT_PREVIOUS_MONTH_COST_CONSUMPTION_RECEIVED_13,       /* (O) */

  /** HistoricalFreezeTime represents the time of day, in Local Time, when
   * Historical Cost Consumption attributes are captured.
   */
  ZB_ZCL_ATTR_PREPAYMENT_HISTORICAL_FREEZE_TIME                             /* (O) */
} zb_zcl_prepayment_historical_cost_consumption_information_set_t;


/** CurrencyScalingFactor Enumerations
 *  @see SE spec, Table D-144
 */
typedef enum zb_zcl_prepayment_currency_scaling_factor_e
{
  ZB_ZCL_PREPAYMENT_SCALE_DOT_000001 = 0x00,
  ZB_ZCL_PREPAYMENT_SCALE_DOT_00001,
  ZB_ZCL_PREPAYMENT_SCALE_DOT_0001,
  ZB_ZCL_PREPAYMENT_SCALE_DOT_001,
  ZB_ZCL_PREPAYMENT_SCALE_DOT_01,
  ZB_ZCL_PREPAYMENT_SCALE_DOT_1,
  ZB_ZCL_PREPAYMENT_SCALE_1,
  ZB_ZCL_PREPAYMENT_SCALE_10,
  ZB_ZCL_PREPAYMENT_SCALE_100,
  ZB_ZCL_PREPAYMENT_SCALE_1000,
  ZB_ZCL_PREPAYMENT_SCALE_10000,
  ZB_ZCL_PREPAYMENT_SCALE_100000,
  ZB_ZCL_PREPAYMENT_SCALE_1000000
} zb_zcl_prepayment_currency_scaling_factor_t;

/** @} */ /* ZB_ZCL_PREPAYMENT_ATTRS */

/** @defgroup ZB_ZCL_PREPAYMENT_COMMANDS Prepayment cluster commands
 * @{
 */

/** Commands are generated by Prepayment Client.
 *  @see SE spec, subclause 7.2.3
 */
typedef enum zb_zcl_prepayment_cli_cmd_e
{
  /** This command is sent to the Metering Device to activate the use of any
   * Emergency Credit available on the Metering Device.
   */
  ZB_ZCL_PREPAYMENT_CLI_CMD_SELECT_AVAILABLE_EMERGENCY_CREDIT = 0x00,  /* (O) */

  /** The ChangeDebt command is sent to the Metering Device to change the debt
   * values.
   */
  ZB_ZCL_PREPAYMENT_CLI_CMD_CHANGE_DEBT                       = 0x02,  /* (O) */

  /** This command provides a method to set up the parameters for
   * the Emergency Credit.
   */
  ZB_ZCL_PREPAYMENT_CLI_CMD_EMERGENCY_CREDIT_SETUP,                   /* (O) */

  /** The follow command is used by the IHD and the ESI as a method to apply
   * credit top up values to a prepayment meter.
   */
  ZB_ZCL_PREPAYMENT_CLI_CMD_CONSUMER_TOP_UP,                           /* (O) */

  /** The follow command is sent to update the Credit Remaining attribute on a
   * Prepayment meter. It shall only be sent from an ESI to the Meter.
   */
  ZB_ZCL_PREPAYMENT_CLI_CMD_CREDIT_ADJUSTMENT,                         /* (O) */

  /** This command is sent to a Metering Device to instruct it to change its mode
   * of operation, e.g. from Credit to Prepayment.
   */
  ZB_ZCL_PREPAYMENT_CLI_CMD_CHANGE_PAYMENT_MODE,                       /* (O) */

  /** This command is used to request the cluster server for snapshot data. */
  ZB_ZCL_PREPAYMENT_CLI_CMD_GET_PREPAY_SNAPSHOT,                       /* (O) */

  /** This command is sent to the Metering Device to retrieve the log of Top Up
   * codes received by the meter.
   */
  ZB_ZCL_PREPAYMENT_CLI_CMD_GET_TOP_UP_LOG,                            /* (O) */

  /** This command is sent from client to a Prepayment server to set the warning
   * level for low credit.
   */
  ZB_ZCL_PREPAYMENT_CLI_CMD_SET_LOW_CREDIT_WARNING_LEVEL,              /* (O) */

  /** This command is used to request the contents of the Repayment log. */
  ZB_ZCL_PREPAYMENT_CLI_CMD_GET_DEBT_REPAYMENT_LOG,                    /* (O) */

  /** This command is sent from a client to the Prepayment server to set the
   * maximum credit level allowed in the meter.
   */
  ZB_ZCL_PREPAYMENT_CLI_CMD_SET_MAXIMUM_CREDIT_LIMIT,                  /* (O) */

  /** This command is sent from a client to the Prepayment server to set the
   * overall debt cap allowed in the meter.
   */
  ZB_ZCL_PREPAYMENT_CLI_CMD_SET_OVERALL_DEBT_CAP                       /* (O) */
} zb_zcl_prepayment_cli_cmd_t;

/** Originating Device Field Enumerations
 *  @see SE spec, Table D-146
 */
typedef enum zb_zcl_prepayment_originating_device_e
{
  ZB_ZCL_PREPAYMENT_ORIGINATING_DEVICE_ESI = 0x00,
  ZB_ZCL_PREPAYMENT_ORIGINATING_DEVICE_METERING,
  ZB_ZCL_PREPAYMENT_ORIGINATING_DEVICE_IHD
} zb_zcl_prepayment_originating_device_t;

/** Select Available Emergency Credit Command Payload
 *  @see SE spec, subclause 7.2.3.1
 */
typedef ZB_PACKED_PRE struct zb_zcl_prepayment_select_available_emergency_credit_payload_s
{
  /** A UTCTime field to indicate the date and time at which the selection
   *  command was issued.
   */
  zb_uint32_t command_issue_date;  /* (M) */

  /** An 8-bit enumeration field identifying the SE device issuing the selection
   * command
   */
  zb_uint8_t originating_device;   /* (M) */
} ZB_PACKED_STRUCT zb_zcl_prepayment_select_available_emergency_credit_payload_t;

/** Debt Amount Type Field Enumerations
 *  @see SE spec, Table D-147
 */
typedef enum zb_zcl_prepayment_debt_amount_type_e
{
  ZB_ZCL_PREPAYMENT_DEBT_AMOUNT_TYPE_1_ABSOLUTE = 0x00,
  ZB_ZCL_PREPAYMENT_DEBT_AMOUNT_TYPE_1_INCREMENTAL,
  ZB_ZCL_PREPAYMENT_DEBT_AMOUNT_TYPE_2_ABSOLUTE,
  ZB_ZCL_PREPAYMENT_DEBT_AMOUNT_TYPE_2_INCREMENTAL,
  ZB_ZCL_PREPAYMENT_DEBT_AMOUNT_TYPE_3_ABSOLUTE,
  ZB_ZCL_PREPAYMENT_DEBT_AMOUNT_TYPE_3_INCREMENTAL
} zb_zcl_prepayment_debt_amount_type_t;

/** Change Debt Command Payload
 *  @see SE spec, subclause 7.2.3.3
 */
typedef ZB_PACKED_PRE struct zb_zcl_prepayment_change_debt_command_payload_s
{

  /** Unique identifier generated by the commodity provider. When new information
   * is provided that replaces older information for the same time period,
   * this field allows devices to determine which information is newer.
   */
  zb_uint32_t issuer_event_id;                   /* (M) */

  /** The format and use of this field is the same as for the DebtLabel#N
   * attribute as defined in D.7.2.2.3.1.
   */
  zb_uint8_t debt_label[13];                     /* (M) */

  /** The format and use of this field is the same as for
   * the DebtAmount#N attribute as defined in D.7.2.2.3.2.
   */
  zb_int32_t debt_amount;                        /* (M) */

  /** The format and use of this field is the same as for the DebtRecoveryMethod#N
   *  attribute as defined in D.7.2.2.3.3.
   */
  zb_uint8_t debt_recovery_method;               /* (M) */

  /** An 8-bit enumeration field identifying the type of debt information to be
   * issued within this command.
   */
  zb_uint8_t debt_amount_type;                   /* (M) */

  /** The format and use of this field is the same as for the
   * DebtRecoveryStartTime#N attribute as defined in D.7.2.2.3.4.
   */
  zb_uint32_t debt_recovery_start_time;          /* (M) */

  /** The format and use of this field is the same as for the
   * DebtRecoveryCollectionTime#N attribute as defined in D.7.2.2.3.5.
   */
  zb_uint16_t debt_recovery_collection_time;     /* (M) */

  /** The format and use of this field is the same as for the
   * DebtRecoveryFrequency#N attribute as defined in D.7.2.2.3.6.
   */
  zb_uint8_t debt_recovery_frequency;            /* (M) */

  /** The format and use of this field is the same as for the
   * DebtRecoveryAmount#N attribute as defined in D.7.2.2.3.7.
   */
  zb_int32_t debt_recovery_amount;               /* (M) */

  /** The format and use of this field is the same as for the
   * DebtRecoveryTopUpPercentage#N attribute as defined in D.7.2.2.3.8.
   */
  zb_uint16_t debt_recovery_balance_percentage;  /* (M) */
} ZB_PACKED_STRUCT zb_zcl_prepayment_change_debt_command_payload_t;


/** Emergency Credit Setup Command Payload
 *  @see SE spec, subclause D.7.2.3.4
 */
typedef ZB_PACKED_PRE struct zb_zcl_prepayment_emergency_credit_setup_payload_s
{
  /** Unique identifier generated by the commodity provider. When new information
   * is provided that replaces older information for the same time period, this
   * field allows devices to determine which information is newer.
   */
  zb_uint32_t issuer_event_id;              /* (M) */

  /** A UTC Time field to denote the time at which the Emergency Credit settings
   * become valid. A start date/time of 0x00000000 shall indicate that the
   * command should be executed immediately.
   */
  zb_uint32_t start_time;                   /* (M) */

  /** An unsigned 32-bit field to denote the Emergency Credit limit on the
   * Metering Device, measured in base unit of Currency
   * or in Units with the decimal point.
   */
  zb_uint32_t emergency_credit_limit;       /* (M) */

  /** An unsigned 32-bit field to denote the amount of credit remaining on the
   * Metering Device below which the Emergency Credit facility can be selected.
   * The value is measured in base unit of Currency or in Units with the decimal
   * points.
   */
  zb_uint32_t emergency_credit_threshold;   /* (M) */

} ZB_PACKED_STRUCT zb_zcl_prepayment_emergency_credit_setup_payload_t;


/** Consumer Top Up Command is used by the IHD and the ESI as a method to apply
 *  credit top up values to a prepayment meter.
 *  @see SE spec, subclause D.7.2.3.5
 */
typedef ZB_PACKED_PRE struct zb_zcl_prepayment_consumer_top_up_payload_s
{
  /** An 8-bit enumeration field identifying the Smart Energy device issuing the
   * selection command
   * @see  SE spec, Table D-146.
   */
  zb_uint8_t originating_device;  /* (M) */

  /** An octet string of between 1 and 26 characters (the first character indicates
   * the string length).
   */
  zb_uint8_t top_up_code[26];     /* (M) */
} ZB_PACKED_STRUCT zb_zcl_prepayment_consumer_top_up_payload_t;


/** Credit Type Field Enumerations
 *  @see SE spec, Table D-148
 */
typedef enum zb_zcl_prepayment_credit_type_e
{
  ZB_ZCL_PREPAYMENT_CREDIT_INCREMENTAL = 0x00,
  ZB_ZCL_PREPAYMENT_CREDIT_ABSOLUTE
} zb_zcl_prepayment_credit_type_t;

/** The Credit Adjustment command Payload
 *  @see SE spec, subclause D.7.2.3.6
 */
typedef ZB_PACKED_PRE struct zb_zcl_prepayment_credit_adjustment_payload_s
{
  /** Unique identifier generated by the commodity provider. When new information
   * is provided that replaces older information for the same time period, this
   * field allows devices to determine which information is newer.
   */
  zb_uint32_t issuer_event_id;         /* (M) */

  /** A UTC Time field to denote the time at which the credit adjustment settings
   * become valid. A start date/time of 0x00000000 shall indicate that the
   * command should be executed immediately.
   */
  zb_uint32_t start_time;              /* (M) */

  /** An 8-bit enumeration field identifying the type of credit adjustment to be
   * issued out within this command.
   */
  zb_uint8_t credit_adjustment_type;   /* (M) */

  /** A signed 32-bit field to denote the value of the credit adjustment,
   * measured in base unit of Currency or in Units with the decimal point
   */
  zb_int32_t credit_adjustment_value;  /* (M) */
} ZB_PACKED_STRUCT zb_zcl_prepayment_credit_adjustment_payload_t;


/** Change Payment Mode Command Payload
 *  @see SE spec, subclause D.7.2.3.7
 */
typedef ZB_PACKED_PRE struct zb_zcl_prepayment_change_payment_mode_payload_s
{
  /** An unsigned 32-bit field containing a unique identifier for the commodity
   * supplier to whom this command relates.
   */
  zb_uint32_t provider_id;                              /* (M) */

  /** Unique identifier generated by the commodity provider. When new information
   * is provided that replaces older information for the same time period, this
   * field allows devices to determine which information is newer.
   */
  zb_uint32_t issuer_event_id;                          /* (M) */

  /** A UTC Time field to indicate the date from which the payment mode change is
   * to be applied. An Implementation Date/Time value of 0x00000000 shall
   * indicate that the command should be executed immediately.
   */
  zb_uint32_t implementation_date;                      /* (M) */

  /** An 16-bit BitMap indicating the actions required in relation to switching
   * the payment mode.
   */
  zb_uint16_t proposed_payment_control_configuration;   /* (M) */

  /** This value is a signed 32-bit integer that shall either be zero or a
   * negative value (in all known cases). This value represents a threshold
   * relating to the absolute value of the CreditRemaining attribute, that when
   * reached (when credit is decrementing) causes the supply of service to be
   * disconnected.
   */
  zb_int32_t cut_off_value;                             /* (M) */
} ZB_PACKED_STRUCT zb_zcl_prepayment_change_payment_mode_payload_t;


/** Get Prepay Snapshot Command Payload
 *  @see SE spec, subclause D.7.2.3.8
 */
typedef ZB_PACKED_PRE struct zb_zcl_prepayment_get_prepay_snapshot_payload_s
{

  /** A UTC Timestamp indicating the earliest time of a snapshot to be returned
   * by a corresponding Publish Prepay Snapshot command.
   */
  zb_uint32_t earliest_start_time;  /* (M) */

  /** A UTC Timestamp indicating the latest time of a snapshot to be returned by
   * a corresponding Publish Prepay Snapshot command.
   */
  zb_uint32_t latest_end_time;      /* (M) */

  /** Where multiple snapshots satisfy the selection criteria specified by the
   * other fields in this command, this field identifies the individual snapshot
   * to be returned.
   */
  zb_uint8_t snapshot_offset;       /* (M) */

  /** This field is used to request only snapshots for a specific cause. */
  zb_uint32_t snapshot_cause;       /* (M) */
} ZB_PACKED_STRUCT zb_zcl_prepayment_get_prepay_snapshot_payload_t;


/** Get Top Up Log Payload
 *  @see SE spec, subclause D.7.2.3.9
 */
typedef ZB_PACKED_PRE struct zb_zcl_prepayment_get_top_up_log_payload_s
{

  /** UTC timestamp indicating the latest TopUp Time of Top Up records to be
   * returned by the corresponding Publish Top Up Log commands.
   */
  zb_uint32_t latest_end_time;  /* (M) */

  /** An 8-bit integer which represents the maximum number of records that the
   * client is willing to receive in response to this command. A value of 0
   * would indicate all available records shall be returned.
   */
  zb_uint8_t records_number;    /* (M) */
} ZB_PACKED_STRUCT zb_zcl_prepayment_get_top_up_log_payload_t;

/** Set Low Credit Warning Level Payload.
 * @see SE spec, subclause D.7.2.3.10
 */
typedef zb_uint32_t zb_zcl_prepayment_set_low_credit_warning_level_payload_t;


/** Debt Type Field Enumerations
 *  @see SE spec, Table D-149
 */
typedef enum zb_zcl_prepayment_debt_type_e
{
  ZB_ZCL_PREPAYMENT_DEBT_TYPE_DEBT_1 = 0x00,
  ZB_ZCL_PREPAYMENT_DEBT_TYPE_DEBT_2,
  ZB_ZCL_PREPAYMENT_DEBT_TYPE_DEBT_3,
  ZB_ZCL_PREPAYMENT_DEBT_TYPE_ALL_DEBTS = 0xFF
} zb_zcl_prepayment_debt_type_t;


/** Get Debt Repayment Log Command Payload
 *  @see SE spec, table D.7.2.3.11
 */
typedef ZB_PACKED_PRE struct zb_zcl_prepayment_get_debt_repayment_log_payload_s
{
  /** UTC timestamp indicating the latest Collection Time of debt repayment
   * records to be returned by the corresponding Publish Debt Log commands.
   */
  zb_uint32_t latest_end_time;  /* (M) */

  /** An 8-bit integer which represents the maximum number of debt repayment
   * records that the client is willing to receive in response to this command.
   */
  zb_uint8_t debts_number;      /* (M) */

  /** An 8-bit enumeration field identifying the type of debt record(s) to be
   * returned
   */
  zb_uint8_t debt_type;         /* (O) */
} ZB_PACKED_STRUCT zb_zcl_prepayment_get_debt_repayment_log_payload_t;


/** Set Maximum Credit Limit Payload
 *  @see SE spec, subclause D.7.2.3.12
 */
typedef ZB_PACKED_PRE struct zb_zcl_prepayment_set_maximum_credit_limit_payload_s
{

  /** An unsigned 32-bit field containing a unique identifier for the commodity
   * supplier to whom this command relates.
   */
  zb_uint32_t provider_id;            /* (M) */

  /** Unique identifier generated by the commodity provider. When new information
   * is provided that replaces older information for the same time period, this
   * field allows devices to determine which information is newer.
   */
  zb_uint32_t issuer_event_id;        /* (M) */

  /** A UTC Time field to indicate the date from which the maximum credit level
   * is to be applied.
   */
  zb_uint32_t implementation_date;    /* (M) */

  /** An unsigned 32-bit integer value indicating the maximum credit balance
   * allowed on a meter. Any further top-up amount that will cause the meter to
   * exceed this limit will be rejected.
   */
  zb_uint32_t max_credit_level;       /* (M) */

  /** An unsigned 32-bit integer value indicating the maximum credit per top-up.
   * Any single top-up greater than this threshold will cause the meter to
   * reject the top-up.
   */
  zb_uint32_t max_credit_per_top_up;  /* (M) */
} ZB_PACKED_STRUCT zb_zcl_prepayment_set_maximum_credit_limit_payload_t;


/** Set Overall Debt Cap Payload
 * @see SE spec, subclause D.7.2.3.13
 */
typedef ZB_PACKED_PRE struct zb_zcl_prepayment_set_overall_debt_cap_payload_s
{

  /** An unsigned 32-bit field containing a unique identifier for the commodity
   * supplier to whom this command relates.
   */
  zb_uint32_t provider_id;          /* (M) */

  /** Unique identifier generated by the commodity provider. When new information
   * is provided that replaces older information for the same time period, this
   * field allows devices to determine which information is newer.
   */
  zb_uint32_t issuer_event_id;      /* (M) */

  /** A UTC Time field to indicate the date from which the overall debt cap is to
   * be applied. An Implementation Date/Time of 0x00000000 shall indicate that
   * the command should be executed immediately.
   */
  zb_uint32_t implementation_date;  /* (M) */

  /** A signed 32-bit integer that defines the total amount of debt that can be
   * taken from top-ups. This field is always a monetary value, and as such the
   * field is measured in a base unit of Currency with the decimal point.
   */
  zb_int32_t overall_debt_cap;      /* (O) */
} ZB_PACKED_STRUCT zb_zcl_prepayment_set_overall_debt_cap_payload_t;


/** Commands are generated by Prepayment Server.
 * @see SE spec, subclause D.7.2.4
 */
typedef enum zb_zcl_prepayment_srv_cmd_e
{

  /** This command is generated in response to a GetPrepaySnapshot command or
   * when a new snapshot is created. It is used to return a single snapshot
   * to the client.
   */
  ZB_ZCL_PREPAYMENT_SRV_CMD_PUBLISH_PREPAY_SNAPSHOT      = 0x01,  /* (O) */

  /** This command is sent in response to the ChangePaymentMode command. This command
   * shall only inform the ESI of the current default setting that would affect
   * the meter when entering into Prepayment/PAYG or Credit mode. Should these
   * values require changing then other commands within the Prepayment & Price
   * cluster should be used.
   */
  ZB_ZCL_PREPAYMENT_SRV_CMD_CHANGE_PAYMENT_MODE_RESPONSE,         /* (O) */

  /**  */
  ZB_ZCL_PREPAYMENT_SRV_CMD_CONSUMER_TOP_UP_RESPONSE,             /* (O) */

  /** This command is used to send the Top Up Code Log entries to the Prepayment
   * client. The command shall be sent in response to a Get Top Up Log command
   * and MAY be sent unsolicited whenever a new Top Up code is received and
   * successfully processed
   */
  ZB_ZCL_PREPAYMENT_SRV_CMD_PUBLISH_TOP_UP_LOG           = 0x05,  /* (O) */

  /** This command is used to send the contents of the Repayment Log. */
  ZB_ZCL_PREPAYMENT_SRV_CMD_PUBLISH_DEBT_LOG                      /* (O) */
} zb_zcl_prepayment_srv_cmd_t;


/** Snapshot Payload Cause
 *  @see SE spec, Table D-151
 */
typedef enum zb_zcl_prepayment_snapshot_payload_cause_e
{
  ZB_ZCL_PREPAYMENT_CAUSE_GENERAL                        = 1 << 0,
  ZB_ZCL_PREPAYMENT_CAUSE_END_OF_BILLING_PERIOD          = 1 << 1,
  /* 1 << 2 is reserved for metering cluster */
  ZB_ZCL_PREPAYMENT_CAUSE_TARIFF_INFORMATION_CHANGE      = 1 << 3,
  ZB_ZCL_PREPAYMENT_CAUSE_PRICE_MATRIX_CHANGE            = 1 << 4,
  /* 1 << 5 is reserved for metering cluster */
  /* 1 << 6 is reserved for metering cluster */
  /* 1 << 7 is reserved for metering cluster */
  /* 1 << 8 is reserved for metering cluster */
  /* 1 << 9 is reserved for metering cluster */
  ZB_ZCL_PREPAYMENT_CAUSE_MANUALLY_TRIGGERED_FROM_CLIENT = 1 << 10,
  /* 1 << 11 is reserved for metering cluster */
  ZB_ZCL_PREPAYMENT_CAUSE_TENANCY_CHANGE                 = 1 << 12,
  ZB_ZCL_PREPAYMENT_CAUSE_SUPPLIER_CHANGE                = 1 << 13,
  ZB_ZCL_PREPAYMENT_CAUSE_METER_MODE_CHANGE              = 1 << 14,
  /* 1 << 15 is reserved for metering cluster */
  /* 1 << 16 is reserved for metering cluster */
  /* 1 << 17 is reserved for metering cluster */
  ZB_ZCL_PREPAYMENT_CAUSE_TOP_UP_ADDITION                = 1 << 18,
  ZB_ZCL_PREPAYMENT_CAUSE_DEBT_AND_CREDIT_ADDITION       = 1 << 19
} zb_zcl_prepayment_snapshot_payload_cause_t;


/** Snapshot Payload Type
 *  @see SE spec, Table D-152
 */
typedef enum zb_zcl_prepayment_snapshot_payload_type_e
{
  ZB_ZCL_PREPAYMENT_SNAPSHOT_PAYLOAD_TYPE_DEBT_AND_CREDIT_STATUS = 0x00,
} zb_zcl_prepayment_snapshot_payload_type_t;


/** Debt/Credit Status Command Payload
 *  @see SE spec, Figure D-128
 */
typedef ZB_PACKED_PRE struct zb_zcl_prepayment_snapshot_payload_s
{
  /* It is a part of zb_zcl_prepayment_publish_prepay_snapshot_payload_t */
  zb_uint8_t snapshot_payload_type;          /* (M) */

  ZB_PACKED_PRE union
  {
    ZB_PACKED_PRE struct
    {
      /** The AccumulatedDebt field represents the total amount of debt remaining on
       * the Metering Device, measured in a base unit of Currency with the decimal
       * point located as indicated by the Trailing Digits field, as defined in
       * the Price cluster.
       */
      zb_int32_t accumulated_debt;            /* (M) */

      /** The Type1DebtRemaining field represents the amount of Type 1 debt remaining
       * on the Metering Device, measured in base unit of Currency with the decimal
       * point located as indicated by the Trailing Digits field, as defined in the
       * Price cluster.
       */
      zb_uint32_t type_1_debt_remaining;      /* (M) */

      /** The Type2DebtRemaining field represents the amount of Type 2 debt remaining
       * on the Metering Device, measured in base unit of Currency with the decimal
       * point located as indicated by the TrailingDigits field, as defined in the
       * Price cluster.
       */
      zb_uint32_t type_2_debt_remaining;      /* (M) */

      /** The Type3DebtRemaining field represents the amount of Type 3 debt remaining
       * on the Metering Device, measured in base unit of Currency with the decimal
       * point located as indicated by the TrailingDigits field, as defined in the
       * Price cluster.
       */
      zb_uint32_t type_3_debt_remaining;      /* (M) */

      /** The EmergencyCreditRemaining field represents the amount of Emergency Credit
       * still available on the Metering Device. If Monetary based, this field is
       * measured in a base unit of Currency or in Units, with the decimal point
       */
      zb_int32_t emergency_credit_remaining;  /* (M) */

      /** The CreditRemaining field represents the amount of credit
       * remaining on the Metering Device. If Monetary based, this field is measured
       * in a base unit of Currency or in Units, with the decimal point.
       */
      zb_int32_t credit_remaining;            /* (M) */
    } ZB_PACKED_STRUCT debt_credit_status;
  } ZB_PACKED_STRUCT u;
} ZB_PACKED_STRUCT zb_zcl_prepayment_snapshot_payload_t;

/** Publish Prepay Snapshot Command Payload
 *  @see SE spec, subclause 7.2.4.2
 */
typedef ZB_PACKED_PRE struct zb_zcl_prepayment_publish_prepay_snapshot_payload_s
{

  /** Unique identifier allocated by the device creating the snapshot. */
  zb_uint32_t snapshot_id;              /* (M) */

  /** This is a 32-bit value (in UTC Time) representing the time at which the
   * data snapshot was taken.
   */
  zb_uint32_t snapshot_time;            /* (M) */

  /** An 8-bit Integer indicating the number of snapshots found, based on the
   * search criteria defined in the associated GetPrepaySnapshot command. If the
   * value is greater than 1, the client is able to request the next snapshot by
   * incrementing the Snapshot Offset field in an otherwise repeated
   * get_prepay_snapshot command.
   */
  zb_uint8_t total_snapshots_found;     /* (M) */

  /** The follow field is uses to count the payload fragments in the case where
   * the entire payload does not fit into one message. The Command Index starts
   * at 0 and is incremented for each fragment belonging to the same command.
   */
  zb_uint8_t command_index;             /* (M) */

  /** In the case where the entire payload does not fit into one message, the
   * Total Number of Commands field indicates the total number of sub-commands
   * in the message.
   */
  zb_uint8_t total_number_of_commands;  /* (M) */

  /** A 32-bit BitMap indicating the cause of the snapshot. */
  zb_uint32_t snapshot_cause;           /* (M) */

  zb_zcl_prepayment_snapshot_payload_t snapshot_payload;  /* (M) */
} ZB_PACKED_STRUCT zb_zcl_prepayment_publish_prepay_snapshot_payload_t;


/** Friendly Credit BitMap
 *  @see SE spec, Table D-153
 */
typedef enum zb_zcl_prepayment_friendly_credit_bitmap_e
{
  PREPAYMENT_FRIENDLY_CREDIT_ENABLED = 0,
} zb_zcl_prepayment_friendly_credit_bitmap_t;


/** Change Payment Mode Response Command Payload
 *  @see SE spec, subclause 7.2.4.3
 */
typedef ZB_PACKED_PRE struct zb_zcl_prepayment_payment_mode_response_payload_s
{

  /** An 8-bit BitMap to show if the meter has a Friendly Credit calendar and that
   * this calendar shall be enabled.
   */
  zb_uint8_t friendly_credit;               /* (M) */

  /** An unsigned 32-bit field to denote the IssuerCalendarID that shall be used
   * for the friendly credit periods
   */
  zb_uint32_t friendly_credit_calendar_id;  /* (M) */

  /** An unsigned 32-bit field to denote the emergency credit limit on the Metering
   * Device, measured in base unit of Currency with the decimal point located as
   * indicated by the TrailingDigits field, as defined in the Price cluster.
   */
  zb_uint32_t emergency_credit_limit;       /* (M) */

  /** An unsigned 32-bit field to denote the amount of credit remaining on the
   * Metering Device below which the Emergency Credit facility can be selected.
   * The value is measured in base unit of Currency with the decimal point located
   * as indicated by the TrailingDigits field, as defined in the Price cluster.
   */
  zb_uint32_t emergency_credit_threshold;   /* (M) */
} ZB_PACKED_STRUCT zb_zcl_prepayment_payment_mode_response_payload_t;


/** Result Type Field Enumerations
 *  @see SE spec, Table D-154
 */
typedef enum zb_zcl_prepayment_result_type_e
{
  ZB_ZCL_PREPAYMENT_RESULT_ACCEPTED = 0x00,
  ZB_ZCL_PREPAYMENT_RESULT_REJECTED_INVALID_TOP_UP,
  ZB_ZCL_PREPAYMENT_RESULT_REJECTED_DUPLICATE_TOP_UP,
  ZB_ZCL_PREPAYMENT_RESULT_REJECTED_ERROR,
  ZB_ZCL_PREPAYMENT_RESULT_REJECTED_MAX_CREDIT_REACHED,
  ZB_ZCL_PREPAYMENT_RESULT_REJECTED_KAYPAD_LOCK,
  ZB_ZCL_PREPAYMENT_RESULT_REJECTED_TOP_UP_VALUE_TOO_LARGE,
  ZB_ZCL_PREPAYMENT_RESULT_ACCEPTED_SUPPLY_ENABLED = 0x10,
  ZB_ZCL_PREPAYMENT_RESULT_ACCEPTED_SUPPLY_DISABLED,
  ZB_ZCL_PREPAYMENT_RESULT_ACCEPTED_SUPPLY_ARMED
} zb_zcl_prepayment_result_type_t;


/** Consumer Top Up Response Command Payload
 *  @see SE spec, subclause 7.2.4.4
 */
typedef ZB_PACKED_PRE struct zb_zcl_prepayment_consumer_top_up_response_payload_s
{

  /** An 8-bit enumerated value indicating whether the Metering Device accepted
   * or rejected the top up.
   */
  zb_uint8_t result_type;          /* (M) */

  /** A signed 32-bit integer field representing the Top Up value available in
   * the top up content. If it is Monetary based, this field is measured in a
   * base unit of Currency with the decimal point located as indicated by the
   * Trailing Digits field, as defined in the Price cluster.
   */
  zb_int32_t top_up_value;         /* (M) */

  /** An 8-bit enumeration indicating the device that has issued the top up */
  zb_uint8_t source_of_top_up;     /* (M) */

  /** The Credit Remaining field represents the amount of credit remaining on the
   *  Metering Device after addition of a top up.
   */
  zb_int32_t credit_remaining;     /* (M) */

} ZB_PACKED_STRUCT zb_zcl_prepayment_consumer_top_up_response_payload_t;


/** Top Up Payload Details
 *  @see SE spec, Figure D-132
 */
typedef ZB_PACKED_PRE struct zb_zcl_prepayment_top_up_payload_details_s
{

  /** This is the value of the Top Up code stored in the log. */
  zb_uint8_t top_up_code[26];           /* (M) */

  /** This is the amount of credit that was added to the Metering Device during
   * this Top Up.
   */
  zb_int32_t top_up_amount;             /* (M) */

  /** This is the UTC Timestamp when the Top Up was applied to the Metering Device. */
  zb_uint32_t top_up_time;              /* (M) */
  zb_uint8_t top_up_code_plus_1[26];    /* (M) */
  zb_int32_t top_up_amount_plus_1;      /* (M) */
  zb_uint32_t top_up_time_plus_1;       /* (M) */
  zb_uint8_t top_up_code_plus_n[26];    /* (M) */
  zb_int32_t top_up_amount_plus_n;      /* (M) */
  zb_uint32_t top_up_time_plus_n;       /* (M) */
} ZB_PACKED_STRUCT zb_zcl_prepayment_top_up_payload_details_t;


/** Publish Top Up Log Command Payload
 *  @see SE spec, subclause 7.2.4.5
 */
typedef ZB_PACKED_PRE struct zb_zcl_prepayment_publish_top_up_log_payload_s
{

  /** The Command Index is used to count the payload fragments in the case where
   * the entire payload does not fit into one message. The Command Index starts
   * at 0 and is incremented for each fragment belonging to the same command
   */
  zb_uint8_t command_index;                                   /* (M) */

  /** In the case that an entire payload does not fit into one message, the Total
   * Number of Commands field indicates the total number of sub-commands in the
   * message.
   */
  zb_uint8_t commands_total_number;                           /* (M) */

  /** see @ref zb_zcl_prepayment_top_up_payload_details_t */
  zb_zcl_prepayment_top_up_payload_details_t top_up_payload;  /* (O) */
} ZB_PACKED_STRUCT zb_zcl_prepayment_publish_top_up_log_payload_t;


/** Format of a Debt Payload Record
 *  @see SE spec, Figure D-134
 */
typedef ZB_PACKED_PRE struct zb_zcl_prepayment_debt_payload_record_s
{

  /** An UTC time field identifying the time when the collection occurred. */
  zb_uint32_t collection_time;    /* (M) */

  /** An unsigned 32-bit field to denote the amount of debt collected at this
   * time.
   */
  zb_uint32_t amount_collected;  /* (M) */

  /** An 8-bit enumeration field identifying the type of debt the record refers to.
   * The enumerations are defined in Table D-149.
   */
  zb_uint8_t debt_type;          /* (M) */

  /** An unsigned 32-bit field to denote the amount of debt still outstanding
   * after the debt was collected.
   */
  zb_uint32_t outstanding_debt;  /* (M) */
} ZB_PACKED_STRUCT zb_zcl_prepayment_debt_payload_record_t;


/** Publish Debt Log Command Payload
 *  @see SE spec, subclause 7.2.4.6
 */
typedef ZB_PACKED_PRE struct zb_zcl_prepayment_publish_debt_log_payload_s
{

  /** An UTC time field identifying the time when the collection occurred. */
  zb_uint8_t command_index;                                /* (M) */

  /** An unsigned 32-bit field to denote the amount of debt collected at this
   * time.
   */
  zb_uint8_t commands_total_number;                        /* (M) */

  /** The Debt Payload shall contain one or more debt records
   * @ref zb_zcl_prepayment_debt_payload_record_t
   */
  zb_zcl_prepayment_debt_payload_record_t *debt_payload;   /* (M) */
} ZB_PACKED_STRUCT zb_zcl_prepayment_publish_debt_log_payload_t;


/** @cond internals_doc */

void zb_zcl_prepayment_init_server(void);
void zb_zcl_prepayment_init_client(void);
#define ZB_ZCL_CLUSTER_ID_PREPAYMENT_SERVER_ROLE_INIT zb_zcl_prepayment_init_server
#define ZB_ZCL_CLUSTER_ID_PREPAYMENT_CLIENT_ROLE_INIT zb_zcl_prepayment_init_client

/** @endcond */  /* internals_doc */

/** @} */ /* ZB_ZCL_PREPAYMENT_COMMANDS */

/** @} */ /* ZB_ZCL_PREPAYMENT */

/** @endcond */ /* DOXYGEN_ZCL_SECTION */
#endif /* ZB_ZCL_PREPAYMENT_H_ */
