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
/* PURPOSE: Price cluster definitions
*/

#ifndef ZB_ZCL_PRICE_H
#define ZB_ZCL_PRICE_H 1

#include "zcl/zb_zcl_common.h"
#include "zcl/zb_zcl_commands.h"

/** @cond DOXYGEN_ZCL_SECTION */

/** @addtogroup ZB_ZCL_PRICE
 *  @{
 *    @details
 *    The Price Cluster provides the mechanism for communicating Gas, Energy, or
 *    Water pricing information within the premises.
 */

/* Cluster ZB_ZCL_CLUSTER_ID_PRICE */

/** @defgroup ZB_ZCL_PRICE_ATTRS Price cluster attributes
 *  @{
 */

/** @defgroup ZB_ZCL_PRICE_SRV_ATTRIBUTES_GROUP Price cluster server attributes
 *  @{
 *    @details
 *    For convenience, the attributes defined in this cluster are arranged into sets
 *    of related attributes; each set can contain up to 256 attributes. Attribute
 *    identifiers are encoded such that the most significant Octet specifies the
 *    attribute set and the least significant Octet specifies the attribute within
 *    the set.
 */

/** @brief Price Cluster General server attributes
 *  @see SE spec, Table D-73
 */
enum zb_zcl_price_srv_attr_set_e
{
  ZB_ZCL_PRICE_SET_TIER_LABEL                          = 0x00,   /**< Provides a method for utilities to assign a label to the Price Tier. */
  ZB_ZCL_PRICE_SET_BLOCK_THRESHOLD                     = 0x01,   /**< Provides remote access to the Price server Block Thresholds. */
  ZB_ZCL_PRICE_SET_BLOCK_PERIOD                        = 0x02,   /**< Provides remote access to the Price server Block Threshold period. */
  ZB_ZCL_PRICE_SET_COMMODITY                           = 0x03,   /**< Represents items that are associated with a particular commodity. */
  ZB_ZCL_PRICE_SET_BLOCK_PRICE_INFORMATION             = 0x04,   /**< Provides remote access to the block prices. */
  ZB_ZCL_PRICE_SET_EXTENDED_PRICE_INFORMATION          = 0x05,   /**< Represents extended price information. */
  ZB_ZCL_PRICE_SET_TARIFF_INFORMATION                  = 0x06,   /**< Represents items that are associated with a particular Price Tariff. */
  ZB_ZCL_PRICE_SET_BILLING_INFORMATION                 = 0x07,   /**< Provides remote access to the Price server Billing information. */
  ZB_ZCL_PRICE_SET_CREDIT_PAYMENT                      = 0x08,   /**< Provides a method for the HAN (IHD) to understand the current status
                                                                  * of the credit-only payment made to the energy supplier. */

  /* reserved 0x09 - 0x7F */

  ZB_ZCL_PRICE_SET_RECEIVED_TIER_LABEL                 = 0x80,   /**< Provides a method for utilities to assign a label to Received Price Tiers. */
  ZB_ZCL_PRICE_SET_RECEIVED_BLOCK_THRESHOLD            = 0x81,   /**< Provides remote access to the Price server ReceivedBlockThresholds. */
  ZB_ZCL_PRICE_SET_RECEIVED_BLOCK_PERIOD               = 0x82,   /**< Provides remote access to the Price server Received Block Threshold period. */

  /* reserved 0x83 */

  ZB_ZCL_PRICE_SET_RECEIVED_BLOCK_PRICE_INFORMATION    = 0x84,   /**< @copydetails ZB_ZCL_PRICE_SET_BLOCK_PRICE_INFORMATION_GROUP */
  ZB_ZCL_PRICE_SET_RECEIVED_EXTENDED_PRICE_INFORMATION = 0x85,   /**< @copydetails ZB_ZCL_PRICE_SET_EXTENDED_PRICE_INFORMATION_GROUP */
  ZB_ZCL_PRICE_SET_RECEIVED_TARIFF_INFORMATION         = 0x86,   /**< @copydetails ZB_ZCL_PRICE_SET_TARIFF_INFORMATION_GROUP */
  ZB_ZCL_PRICE_SET_RECEIVED_BILLING_INFORMATION        = 0x87,   /**< @copydetails ZB_ZCL_PRICE_SET_BILLING_INFORMATION_GROUP */

  /* reserved 0x88 - 0xFF */
};

/******************************************************************************/

/** @defgroup ZB_ZCL_PRICE_SET_TIER_LABEL_GROUP Tier Label
 *  @{
 */

/** @brief Tier Label (Delivered) Attributes Set
 *  @see SE spec, D.4.2.2.1
 */
enum zb_zcl_price_set_tier_label_e
{
  /** The TierNPriceLabel attributes provide a method for utilities to assign a
   *  label to the Price Tier declared within the Publish Price command.
   */
  ZB_ZCL_ATTR_PRICE_SRV_TIER1_PRICE_LABEL = ZB_ZCL_ATTR_SET_WITH_ATTR_ID(ZB_ZCL_PRICE_SET_TIER_LABEL, 0x00),
  ZB_ZCL_ATTR_PRICE_SRV_TIER2_PRICE_LABEL,
  ZB_ZCL_ATTR_PRICE_SRV_TIER3_PRICE_LABEL,
  ZB_ZCL_ATTR_PRICE_SRV_TIER4_PRICE_LABEL,
  ZB_ZCL_ATTR_PRICE_SRV_TIER5_PRICE_LABEL,
  ZB_ZCL_ATTR_PRICE_SRV_TIER6_PRICE_LABEL,
  ZB_ZCL_ATTR_PRICE_SRV_TIER7_PRICE_LABEL,
  ZB_ZCL_ATTR_PRICE_SRV_TIER8_PRICE_LABEL,
  ZB_ZCL_ATTR_PRICE_SRV_TIER9_PRICE_LABEL,
  ZB_ZCL_ATTR_PRICE_SRV_TIER10_PRICE_LABEL,
  ZB_ZCL_ATTR_PRICE_SRV_TIER11_PRICE_LABEL,
  ZB_ZCL_ATTR_PRICE_SRV_TIER12_PRICE_LABEL,
  ZB_ZCL_ATTR_PRICE_SRV_TIER13_PRICE_LABEL,
  ZB_ZCL_ATTR_PRICE_SRV_TIER14_PRICE_LABEL,
  ZB_ZCL_ATTR_PRICE_SRV_TIER15_PRICE_LABEL,
  ZB_ZCL_ATTR_PRICE_SRV_TIER16_PRICE_LABEL,
  ZB_ZCL_ATTR_PRICE_SRV_TIER17_PRICE_LABEL,
  ZB_ZCL_ATTR_PRICE_SRV_TIER18_PRICE_LABEL,
  ZB_ZCL_ATTR_PRICE_SRV_TIER19_PRICE_LABEL,
  ZB_ZCL_ATTR_PRICE_SRV_TIER20_PRICE_LABEL,
  ZB_ZCL_ATTR_PRICE_SRV_TIER21_PRICE_LABEL,
  ZB_ZCL_ATTR_PRICE_SRV_TIER22_PRICE_LABEL,
  ZB_ZCL_ATTR_PRICE_SRV_TIER23_PRICE_LABEL,
  ZB_ZCL_ATTR_PRICE_SRV_TIER24_PRICE_LABEL,
  ZB_ZCL_ATTR_PRICE_SRV_TIER25_PRICE_LABEL,
  ZB_ZCL_ATTR_PRICE_SRV_TIER26_PRICE_LABEL,
  ZB_ZCL_ATTR_PRICE_SRV_TIER27_PRICE_LABEL,
  ZB_ZCL_ATTR_PRICE_SRV_TIER28_PRICE_LABEL,
  ZB_ZCL_ATTR_PRICE_SRV_TIER29_PRICE_LABEL,
  ZB_ZCL_ATTR_PRICE_SRV_TIER30_PRICE_LABEL,
  ZB_ZCL_ATTR_PRICE_SRV_TIER31_PRICE_LABEL,
  ZB_ZCL_ATTR_PRICE_SRV_TIER32_PRICE_LABEL,
  ZB_ZCL_ATTR_PRICE_SRV_TIER33_PRICE_LABEL,
  ZB_ZCL_ATTR_PRICE_SRV_TIER34_PRICE_LABEL,
  ZB_ZCL_ATTR_PRICE_SRV_TIER35_PRICE_LABEL,
  ZB_ZCL_ATTR_PRICE_SRV_TIER36_PRICE_LABEL,
  ZB_ZCL_ATTR_PRICE_SRV_TIER37_PRICE_LABEL,
  ZB_ZCL_ATTR_PRICE_SRV_TIER38_PRICE_LABEL,
  ZB_ZCL_ATTR_PRICE_SRV_TIER39_PRICE_LABEL,
  ZB_ZCL_ATTR_PRICE_SRV_TIER40_PRICE_LABEL,
  ZB_ZCL_ATTR_PRICE_SRV_TIER41_PRICE_LABEL,
  ZB_ZCL_ATTR_PRICE_SRV_TIER42_PRICE_LABEL,
  ZB_ZCL_ATTR_PRICE_SRV_TIER43_PRICE_LABEL,
  ZB_ZCL_ATTR_PRICE_SRV_TIER44_PRICE_LABEL,
  ZB_ZCL_ATTR_PRICE_SRV_TIER45_PRICE_LABEL,
  ZB_ZCL_ATTR_PRICE_SRV_TIER46_PRICE_LABEL,
  ZB_ZCL_ATTR_PRICE_SRV_TIER47_PRICE_LABEL,
  ZB_ZCL_ATTR_PRICE_SRV_TIER48_PRICE_LABEL,
};

/** @cond internals_doc */

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_TIERN_PRICE_LABEL_GNR(tierN, data_ptr) \
{                                                                               \
  ZB_ZCL_ATTR_PRICE_SRV_TIER ## tierN ## _PRICE_LABEL,                          \
  ZB_ZCL_ATTR_TYPE_OCTET_STRING,                                                \
  ZB_ZCL_ATTR_ACCESS_READ_WRITE,                                                \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                           \
  (void*) (data_ptr)                                                            \
}

/** @endcond */ /* internals_doc */

/** @} */ /* end of ZB_ZCL_PRICE_SET_TIER_LABEL_GROUP */


/******************************************************************************/

/** @defgroup ZB_ZCL_PRICE_SET_BLOCK_THRESHOLD_GROUP Block Threshold
 *  @{
 *    @details
 *     The following set of attributes provides remote access to the Price server
 *     Block Thresholds. Block Threshold values are crossed when the
 *     CurrentBlockPeriodConsumptionDelivered attribute value is greater than a
 *     BlockNThreshold attribute. The number of block thresholds is indicated by
 *     the Number of Block Thresholds field in the associated Publish Price command.
 *     The number of blocks is one greater than the number of thresholds.
 */

/** @brief Block Threshold (Delivered) Attributes Set
 *  @see SE spec, D.4.2.2.2
 */
enum zb_zcl_set_block_threshold_e
{
  /** Attributes Block1Threshold through Block15Threshold represent the block
   * threshold values for a given period (typically the billing cycle).
   */
  ZB_ZCL_ATTR_PRICE_SRV_BLOCK1_THRESHOLD = ZB_ZCL_ATTR_SET_WITH_ATTR_ID(ZB_ZCL_PRICE_SET_BLOCK_THRESHOLD, 0x00),
  ZB_ZCL_ATTR_PRICE_SRV_BLOCK2_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_BLOCK3_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_BLOCK4_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_BLOCK5_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_BLOCK6_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_BLOCK7_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_BLOCK8_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_BLOCK9_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_BLOCK10_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_BLOCK11_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_BLOCK12_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_BLOCK13_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_BLOCK14_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_BLOCK15_THRESHOLD,

  /** Where a single set of thresholds is used, the BlockThresholdCount
   *  attribute indicates the number of applicable BlockNThresholds. Where more
   *  than one set of thresholds is used, each set will be accompanied by an
   *  appropriate TierNBlockThresholdCount attribute
   */
  ZB_ZCL_ATTR_PRICE_SRV_BLOCK_THRESHOLD_COUNT,

  /** Attributes Tier1Block1Threshold through Tier15Block15Threshold represent
   *  the block threshold values applicable to a specific TOU tier for a given
   *  period (typically the billing cycle).
   */
  ZB_ZCL_ATTR_PRICE_SRV_TIER1_BLOCK1_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_TIER1_BLOCK2_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_TIER1_BLOCK3_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_TIER1_BLOCK4_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_TIER1_BLOCK5_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_TIER1_BLOCK6_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_TIER1_BLOCK7_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_TIER1_BLOCK8_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_TIER1_BLOCK9_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_TIER1_BLOCK10_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_TIER1_BLOCK11_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_TIER1_BLOCK12_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_TIER1_BLOCK13_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_TIER1_BLOCK14_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_TIER1_BLOCK15_THRESHOLD,

  /** The TierNBlockThresholdCount attributes hold the number of block
   *  thresholds applicable to a given tier.
   */
  ZB_ZCL_ATTR_PRICE_SRV_TIER1_BLOCK_THRESHOLD_COUNT,

  ZB_ZCL_ATTR_PRICE_SRV_TIER2_BLOCK1_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_TIER2_BLOCK2_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_TIER2_BLOCK3_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_TIER2_BLOCK4_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_TIER2_BLOCK5_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_TIER2_BLOCK6_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_TIER2_BLOCK7_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_TIER2_BLOCK8_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_TIER2_BLOCK9_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_TIER2_BLOCK10_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_TIER2_BLOCK11_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_TIER2_BLOCK12_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_TIER2_BLOCK13_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_TIER2_BLOCK14_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_TIER2_BLOCK15_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_TIER2_BLOCK_THRESHOLD_COUNT,

  ZB_ZCL_ATTR_PRICE_SRV_TIER3_BLOCK1_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_TIER3_BLOCK2_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_TIER3_BLOCK3_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_TIER3_BLOCK4_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_TIER3_BLOCK5_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_TIER3_BLOCK6_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_TIER3_BLOCK7_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_TIER3_BLOCK8_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_TIER3_BLOCK9_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_TIER3_BLOCK10_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_TIER3_BLOCK11_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_TIER3_BLOCK12_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_TIER3_BLOCK13_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_TIER3_BLOCK14_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_TIER3_BLOCK15_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_TIER3_BLOCK_THRESHOLD_COUNT,

  ZB_ZCL_ATTR_PRICE_SRV_TIER4_BLOCK1_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_TIER4_BLOCK2_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_TIER4_BLOCK3_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_TIER4_BLOCK4_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_TIER4_BLOCK5_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_TIER4_BLOCK6_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_TIER4_BLOCK7_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_TIER4_BLOCK8_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_TIER4_BLOCK9_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_TIER4_BLOCK10_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_TIER4_BLOCK11_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_TIER4_BLOCK12_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_TIER4_BLOCK13_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_TIER4_BLOCK14_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_TIER4_BLOCK15_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_TIER4_BLOCK_THRESHOLD_COUNT,

  ZB_ZCL_ATTR_PRICE_SRV_TIER5_BLOCK1_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_TIER5_BLOCK2_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_TIER5_BLOCK3_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_TIER5_BLOCK4_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_TIER5_BLOCK5_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_TIER5_BLOCK6_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_TIER5_BLOCK7_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_TIER5_BLOCK8_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_TIER5_BLOCK9_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_TIER5_BLOCK10_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_TIER5_BLOCK11_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_TIER5_BLOCK12_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_TIER5_BLOCK13_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_TIER5_BLOCK14_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_TIER5_BLOCK15_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_TIER5_BLOCK_THRESHOLD_COUNT,

  ZB_ZCL_ATTR_PRICE_SRV_TIER6_BLOCK1_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_TIER6_BLOCK2_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_TIER6_BLOCK3_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_TIER6_BLOCK4_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_TIER6_BLOCK5_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_TIER6_BLOCK6_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_TIER6_BLOCK7_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_TIER6_BLOCK8_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_TIER6_BLOCK9_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_TIER6_BLOCK10_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_TIER6_BLOCK11_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_TIER6_BLOCK12_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_TIER6_BLOCK13_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_TIER6_BLOCK14_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_TIER6_BLOCK15_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_TIER6_BLOCK_THRESHOLD_COUNT,

  ZB_ZCL_ATTR_PRICE_SRV_TIER7_BLOCK1_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_TIER7_BLOCK2_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_TIER7_BLOCK3_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_TIER7_BLOCK4_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_TIER7_BLOCK5_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_TIER7_BLOCK6_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_TIER7_BLOCK7_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_TIER7_BLOCK8_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_TIER7_BLOCK9_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_TIER7_BLOCK10_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_TIER7_BLOCK11_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_TIER7_BLOCK12_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_TIER7_BLOCK13_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_TIER7_BLOCK14_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_TIER7_BLOCK15_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_TIER7_BLOCK_THRESHOLD_COUNT,

  ZB_ZCL_ATTR_PRICE_SRV_TIER8_BLOCK1_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_TIER8_BLOCK2_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_TIER8_BLOCK3_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_TIER8_BLOCK4_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_TIER8_BLOCK5_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_TIER8_BLOCK6_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_TIER8_BLOCK7_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_TIER8_BLOCK8_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_TIER8_BLOCK9_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_TIER8_BLOCK10_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_TIER8_BLOCK11_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_TIER8_BLOCK12_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_TIER8_BLOCK13_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_TIER8_BLOCK14_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_TIER8_BLOCK15_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_TIER8_BLOCK_THRESHOLD_COUNT,

  ZB_ZCL_ATTR_PRICE_SRV_TIER9_BLOCK1_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_TIER9_BLOCK2_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_TIER9_BLOCK3_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_TIER9_BLOCK4_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_TIER9_BLOCK5_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_TIER9_BLOCK6_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_TIER9_BLOCK7_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_TIER9_BLOCK8_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_TIER9_BLOCK9_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_TIER9_BLOCK10_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_TIER9_BLOCK11_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_TIER9_BLOCK12_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_TIER9_BLOCK13_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_TIER9_BLOCK14_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_TIER9_BLOCK15_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_TIER9_BLOCK_THRESHOLD_COUNT,

  ZB_ZCL_ATTR_PRICE_SRV_TIER10_BLOCK1_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_TIER10_BLOCK2_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_TIER10_BLOCK3_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_TIER10_BLOCK4_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_TIER10_BLOCK5_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_TIER10_BLOCK6_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_TIER10_BLOCK7_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_TIER10_BLOCK8_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_TIER10_BLOCK9_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_TIER10_BLOCK10_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_TIER10_BLOCK11_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_TIER10_BLOCK12_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_TIER10_BLOCK13_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_TIER10_BLOCK14_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_TIER10_BLOCK15_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_TIER10_BLOCK_THRESHOLD_COUNT,

  ZB_ZCL_ATTR_PRICE_SRV_TIER11_BLOCK1_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_TIER11_BLOCK2_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_TIER11_BLOCK3_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_TIER11_BLOCK4_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_TIER11_BLOCK5_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_TIER11_BLOCK6_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_TIER11_BLOCK7_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_TIER11_BLOCK8_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_TIER11_BLOCK9_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_TIER11_BLOCK10_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_TIER11_BLOCK11_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_TIER11_BLOCK12_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_TIER11_BLOCK13_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_TIER11_BLOCK14_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_TIER11_BLOCK15_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_TIER11_BLOCK_THRESHOLD_COUNT,

  ZB_ZCL_ATTR_PRICE_SRV_TIER12_BLOCK1_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_TIER12_BLOCK2_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_TIER12_BLOCK3_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_TIER12_BLOCK4_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_TIER12_BLOCK5_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_TIER12_BLOCK6_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_TIER12_BLOCK7_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_TIER12_BLOCK8_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_TIER12_BLOCK9_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_TIER12_BLOCK10_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_TIER12_BLOCK11_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_TIER12_BLOCK12_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_TIER12_BLOCK13_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_TIER12_BLOCK14_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_TIER12_BLOCK15_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_TIER12_BLOCK_THRESHOLD_COUNT,

  ZB_ZCL_ATTR_PRICE_SRV_TIER13_BLOCK1_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_TIER13_BLOCK2_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_TIER13_BLOCK3_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_TIER13_BLOCK4_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_TIER13_BLOCK5_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_TIER13_BLOCK6_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_TIER13_BLOCK7_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_TIER13_BLOCK8_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_TIER13_BLOCK9_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_TIER13_BLOCK10_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_TIER13_BLOCK11_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_TIER13_BLOCK12_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_TIER13_BLOCK13_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_TIER13_BLOCK14_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_TIER13_BLOCK15_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_TIER13_BLOCK_THRESHOLD_COUNT,

  ZB_ZCL_ATTR_PRICE_SRV_TIER14_BLOCK1_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_TIER14_BLOCK2_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_TIER14_BLOCK3_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_TIER14_BLOCK4_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_TIER14_BLOCK5_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_TIER14_BLOCK6_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_TIER14_BLOCK7_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_TIER14_BLOCK8_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_TIER14_BLOCK9_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_TIER14_BLOCK10_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_TIER14_BLOCK11_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_TIER14_BLOCK12_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_TIER14_BLOCK13_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_TIER14_BLOCK14_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_TIER14_BLOCK15_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_TIER14_BLOCK_THRESHOLD_COUNT,

  ZB_ZCL_ATTR_PRICE_SRV_TIER15_BLOCK1_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_TIER15_BLOCK2_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_TIER15_BLOCK3_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_TIER15_BLOCK4_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_TIER15_BLOCK5_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_TIER15_BLOCK6_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_TIER15_BLOCK7_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_TIER15_BLOCK8_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_TIER15_BLOCK9_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_TIER15_BLOCK10_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_TIER15_BLOCK11_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_TIER15_BLOCK12_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_TIER15_BLOCK13_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_TIER15_BLOCK14_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_TIER15_BLOCK15_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_TIER15_BLOCK_THRESHOLD_COUNT,
};

/** @cond internals_doc */

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_BLOCKN_THRESHOLD_GNR(blockN, data_ptr)   \
{                                                                                             \
  ZB_ZCL_ATTR_PRICE_SRV_BLOCK ## blockN ## _THRESHOLD,                                        \
  ZB_ZCL_ATTR_TYPE_48BIT,                                                                     \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                                               \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                                         \
  (void*) data_ptr                                                                            \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_BLOCK_THRESHOLD_COUNT_GNR(data_ptr)  \
{                                                                                         \
  ZB_ZCL_ATTR_PRICE_SRV_BLOCK_THRESHOLD_COUNT,                                            \
  ZB_ZCL_ATTR_TYPE_U8,                                                                    \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                                           \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                                     \
  (void*) data_ptr                                                                        \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_TIERN_BLOCKM_THRESHOLD_GNR(blockN, tierN, data_ptr)  \
{                                                                                                         \
  ZB_ZCL_ATTR_PRICE_SRV_TIER ## tierN ## _BLOCK ## blockN ## _THRESHOLD,                                  \
  ZB_ZCL_ATTR_TYPE_8BIT_ENUM,                                                                             \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                                                           \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                                                     \
  (void*) (data_ptr)                                                                                      \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_TIERN_BLOCK_THRESHOLD_COUNT_GNR(tierN, data_ptr) \
{                                                                                                     \
  ZB_ZCL_ATTR_PRICE_SRV_TIER ## tierN ## _BLOCK_THRESHOLD_COUNT,                                      \
  ZB_ZCL_ATTR_TYPE_U8,                                                                                \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                                                       \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                                                 \
  (void*) data_ptr                                                                                    \
}

/** @endcond */ /* internals_doc */

/** @} */ /* end of ZB_ZCL_PRICE_SET_BLOCK_THRESHOLD_GROUP */


/******************************************************************************/

/** @defgroup ZB_ZCL_PRICE_SET_BLOCK_PERIOD_GROUP Block Period
 *  @{
 *    @details
 *    This set of attributes provides remote access to the Price server
 *    Block Threshold period (typically the billing cycle) information.
 */

/** @brief Block Period (Delivered) Attributes Set
 *  @see SE spec, D.4.2.2.3
 */
enum zb_zcl_price_set_block_period_e
{
  /** The StartOfBlockPeriod attribute represents the start time of the current
   *  block tariff period.
   */
  ZB_ZCL_ATTR_PRICE_SRV_START_OF_BLOCK_PERIOD = ZB_ZCL_ATTR_SET_WITH_ATTR_ID(ZB_ZCL_PRICE_SET_BLOCK_PERIOD, 0x00),

  /** The BlockPeriodDuration attribute represents the current block tariff
   *  period duration in units defined by the BlockPeriodDurationType attribute.
   */
  ZB_ZCL_ATTR_PRICE_SRV_BLOCK_PERIOD_DURATION,

  /** ThresholdMultiplier provides a value to be multiplied against Threshold
   *  attributes.
   */
  ZB_ZCL_ATTR_PRICE_SRV_THRESHOLD_MULTIPLIER,

  /** ThresholdDivisor provides a value to divide the result of applying
   *  the ThresholdMultiplier attribute to Block Threshold values to derive
   *  values That can be compared against the CurrentBlockPeriodConsumptionDelivered
   *  attribute within the Metering cluster.
   */
  ZB_ZCL_ATTR_PRICE_SRV_THRESHOLD_DIVISOR,

  /** The BlockPeriodDurationType attribute indicates the timebase used for the
   *  BlockPeriodDuration attribute.
   */
  ZB_ZCL_ATTR_PRICE_SRV_BLOCK_PERIOD_DURATION_TYPE,
  /* reserved 0x0205 - 0x02FF */
};

/** @cond internals_doc */

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_START_OF_BLOCK_PERIOD(data_ptr) \
{                                                                             \
  ZB_ZCL_ATTR_PRICE_SRV_START_OF_BLOCK_PERIOD,                                \
  ZB_ZCL_ATTR_TYPE_UTC_TIME,                                                  \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                               \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                         \
  (void*) (data_ptr)                                                          \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_BLOCK_PERIOD_DURATION(data_ptr) \
{                                                                             \
  ZB_ZCL_ATTR_PRICE_SRV_BLOCK_PERIOD_DURATION,                                \
  ZB_ZCL_ATTR_TYPE_U24,                                                       \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                               \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                         \
  (void*) (data_ptr)                                                          \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_THRESHOLD_MULTIPLIER(data_ptr) \
{                                                                             \
  ZB_ZCL_ATTR_PRICE_SRV_THRESHOLD_MULTIPLIER,                                 \
  ZB_ZCL_ATTR_TYPE_U24,                                                       \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                               \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                         \
  (void*) (data_ptr)                                                          \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_THRESHOLD_DIVISOR(data_ptr) \
{                                                                             \
  ZB_ZCL_ATTR_PRICE_SRV_THRESHOLD_DIVISOR,                                    \
  ZB_ZCL_ATTR_TYPE_U24,                                                       \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                               \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                         \
  (void*) (data_ptr)                                                          \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_BLOCK_PERIOD_DURATION_TYPE(data_ptr) \
{                                                                             \
  ZB_ZCL_ATTR_PRICE_SRV_BLOCK_PERIOD_DURATION_TYPE,                           \
  ZB_ZCL_ATTR_TYPE_8BITMAP,                                                   \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                               \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                         \
  (void*) (data_ptr)                                                          \
}

/** @endcond */ /* internals_doc */

/** @brief @e BlockPeriodDurationType attribute values
 *  @see SE spec, table D-105.
 *  @see ZB_ZCL_ATTR_PRICE_SRV_BLOCK_PERIOD_DURATION_TYPE
 */
typedef enum zb_zcl_price_block_period_duration_type_e
{
  ZB_ZCL_PRICE_BLOCK_PERIOD_DURATION_TYPE_MINUTES = 0x00,   /**< minutes (default) */
  ZB_ZCL_PRICE_BLOCK_PERIOD_DURATION_TYPE_DAYS    = 0x01,   /**< days */
  ZB_ZCL_PRICE_BLOCK_PERIOD_DURATION_TYPE_WEEKS   = 0x02,   /**< weeks */
  ZB_ZCL_PRICE_BLOCK_PERIOD_DURATION_TYPE_MONTHS  = 0x03,   /**< months */
} zb_zcl_price_block_period_duration_type_t;

/** @} */ /* end of ZB_ZCL_PRICE_SET_BLOCK_PERIOD_GROUP */


/******************************************************************************/

/** @defgroup ZB_ZCL_PRICE_SET_COMMODITY_GROUP Commodity
 *  @{
 *    @details
 *    This set of attributes represents items that are associated with a
 *    particular commodity.
 */

/** Values and Descriptions for the CalorificValueUnit Attribute
 * @see SE spec, subclause D.4.2.2.4.6
 */
typedef enum zb_zcl_price_calorific_value_unit_values_e
{
  ZB_ZCL_PRICE_CALORIFIC_VALUE_UNIT_MJ_M3 = 0x01,  /**< MJ/m3 */
  ZB_ZCL_PRICE_CALORIFIC_VALUE_UNIT_MJ_KG,         /**< MJ/kg */
} zb_zcl_price_calorific_value_unit_values_t;


/** @brief Commodity Attributes Set
 *  @see SE spec, D.4.2.2.4
 */
enum zb_zcl_price_set_commodity_e
{
  /** CommodityType provides a label for identifying the type of pricing server
   *  present.
   */
  ZB_ZCL_ATTR_PRICE_SRV_COMMODITY_TYPE = ZB_ZCL_ATTR_SET_WITH_ATTR_ID(ZB_ZCL_PRICE_SET_COMMODITY, 0x00),

  /** The value of the Standing Charge is a daily fixed charge associated with
   *  supplying the commodity, measured in base unit of Currency with the decimal
   *  point located as indicated by the Trailing Digits field of a Publish Price
   *  command or PriceTrailingDigit attribute.
   */
  ZB_ZCL_ATTR_PRICE_SRV_STANDING_CHARGE,

  /** The conversion factor is used for gas meter and takes into account changes
   *  in the volume of gas based on temperature and pressure.
   */
  ZB_ZCL_ATTR_PRICE_SRV_CONVERSION_FACTOR,

  /** An 8-bit BitMap used to determine where the decimal point is located in
   *  the ConversionFactor attribute.
   */
  ZB_ZCL_ATTR_PRICE_SRV_CONVERSION_FACTOR_TRAILING_DIGIT,

  /** The amount of heat generated when a given mass of fuel is completely burned.
   */
  ZB_ZCL_ATTR_PRICE_SRV_CALORIFIC_VALUE,

  /** This attribute defines the unit for the CalorificValue.
   */
  ZB_ZCL_ATTR_PRICE_SRV_CALORIFIC_VALUE_UNIT,

  /** An 8-bit BitMap used to determine where the decimal point is located in
   *  the CalorificValue attribute.
   */
  ZB_ZCL_ATTR_PRICE_SRV_CALORIFIC_VALUE_TRAILING_DIGIT,
};

/** @cond internals_doc */

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_COMMODITY_TYPE(data_ptr) \
{                                                                             \
  ZB_ZCL_ATTR_PRICE_SRV_COMMODITY_TYPE,                                       \
  ZB_ZCL_ATTR_TYPE_8BIT_ENUM,                                                 \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                               \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                         \
  (void*) data_ptr                                                            \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_STANDING_CHARGE(data_ptr)  \
{                                                                               \
  ZB_ZCL_ATTR_PRICE_SRV_STANDING_CHARGE,                                        \
  ZB_ZCL_ATTR_TYPE_U32,                                                         \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                                 \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                           \
  (void*) (data_ptr)                                                            \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_CONVERSION_FACTOR(data_ptr)  \
{                                                                                 \
  ZB_ZCL_ATTR_PRICE_SRV_CONVERSION_FACTOR,                                        \
  ZB_ZCL_ATTR_TYPE_U32,                                                           \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                                   \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                             \
  (void*) (data_ptr)                                                              \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_CONVERSION_FACTOR_TRAILING_DIGIT(data_ptr) \
{                                                                                               \
  ZB_ZCL_ATTR_PRICE_SRV_CONVERSION_FACTOR_TRAILING_DIGIT,                                       \
  ZB_ZCL_ATTR_TYPE_8BITMAP,                                                                     \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                                                 \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                                           \
  (void*) (data_ptr)                                                                            \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_CALORIFIC_VALUE(data_ptr)  \
{                                                                               \
  ZB_ZCL_ATTR_PRICE_SRV_CALORIFIC_VALUE,                                        \
  ZB_ZCL_ATTR_TYPE_U32,                                                         \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                                 \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                           \
  (void*) (data_ptr)                                                            \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_CALORIFIC_VALUE_UNIT(data_ptr) \
{                                                                                   \
  ZB_ZCL_ATTR_PRICE_SRV_CALORIFIC_VALUE_UNIT,                                       \
  ZB_ZCL_ATTR_TYPE_8BIT_ENUM,                                                       \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                                     \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                               \
  (void*) (data_ptr)                                                                \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_CALORIFIC_VALUE_TRAILING_DIGIT(data_ptr) \
{                                                                                             \
  ZB_ZCL_ATTR_PRICE_SRV_CALORIFIC_VALUE_TRAILING_DIGIT,                                       \
  ZB_ZCL_ATTR_TYPE_8BITMAP,                                                                   \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                                               \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                                         \
  (void*) (data_ptr)                                                                          \
}

/** @endcond */ /* internals_doc */

/** @} */ /* end of ZB_ZCL_PRICE_SET_COMMODITY_GROUP */


/******************************************************************************/

/** @defgroup ZB_ZCL_PRICE_SET_BLOCK_PRICE_INFORMATION_GROUP Block Price Information
 *  @{
 *    @details
 *    This attributes set provide remote access to the block prices.
 *    The Block Price Information attribute set supports Block and combined
 *    Tier-Block pricing, the number of blocks is one greater than the number of
 *    block thresholds defined in the Pricing cluster
 */

/** @brief Block Price Information (Delivered) Attributes Set
 *  @see SE spec, D.4.2.2.5
 */
enum zb_zcl_price_set_block_price_information_e
{
  /** Attributes PriceNoTierBlock1 through PriceTier15Block16 represent the
   *  price of Energy, Gas, or Water delivered to the premises (i.e. delivered
   *  to the customer from the utility) at a specific price tier as defined by
   *  a TOU schedule, Block Threshold or a real time pricing period.
   */
  ZB_ZCL_ATTR_PRICE_SRV_NO_TIER_BLOCK1_PRICE = ZB_ZCL_ATTR_SET_WITH_ATTR_ID(ZB_ZCL_PRICE_SET_BLOCK_PRICE_INFORMATION, 0x00),
  ZB_ZCL_ATTR_PRICE_SRV_NO_TIER_BLOCK2_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_NO_TIER_BLOCK3_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_NO_TIER_BLOCK4_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_NO_TIER_BLOCK5_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_NO_TIER_BLOCK6_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_NO_TIER_BLOCK7_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_NO_TIER_BLOCK8_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_NO_TIER_BLOCK9_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_NO_TIER_BLOCK10_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_NO_TIER_BLOCK11_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_NO_TIER_BLOCK12_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_NO_TIER_BLOCK13_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_NO_TIER_BLOCK14_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_NO_TIER_BLOCK15_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_NO_TIER_BLOCK16_PRICE,

  ZB_ZCL_ATTR_PRICE_SRV_TIER1_BLOCK1_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_TIER1_BLOCK2_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_TIER1_BLOCK3_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_TIER1_BLOCK4_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_TIER1_BLOCK5_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_TIER1_BLOCK6_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_TIER1_BLOCK7_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_TIER1_BLOCK8_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_TIER1_BLOCK9_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_TIER1_BLOCK10_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_TIER1_BLOCK11_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_TIER1_BLOCK12_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_TIER1_BLOCK13_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_TIER1_BLOCK14_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_TIER1_BLOCK15_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_TIER1_BLOCK16_PRICE,

  ZB_ZCL_ATTR_PRICE_SRV_TIER2_BLOCK1_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_TIER2_BLOCK2_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_TIER2_BLOCK3_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_TIER2_BLOCK4_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_TIER2_BLOCK5_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_TIER2_BLOCK6_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_TIER2_BLOCK7_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_TIER2_BLOCK8_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_TIER2_BLOCK9_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_TIER2_BLOCK10_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_TIER2_BLOCK11_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_TIER2_BLOCK12_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_TIER2_BLOCK13_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_TIER2_BLOCK14_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_TIER2_BLOCK15_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_TIER2_BLOCK16_PRICE,

  ZB_ZCL_ATTR_PRICE_SRV_TIER3_BLOCK1_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_TIER3_BLOCK2_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_TIER3_BLOCK3_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_TIER3_BLOCK4_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_TIER3_BLOCK5_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_TIER3_BLOCK6_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_TIER3_BLOCK7_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_TIER3_BLOCK8_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_TIER3_BLOCK9_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_TIER3_BLOCK10_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_TIER3_BLOCK11_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_TIER3_BLOCK12_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_TIER3_BLOCK13_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_TIER3_BLOCK14_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_TIER3_BLOCK15_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_TIER3_BLOCK16_PRICE,

  ZB_ZCL_ATTR_PRICE_SRV_TIER4_BLOCK1_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_TIER4_BLOCK2_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_TIER4_BLOCK3_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_TIER4_BLOCK4_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_TIER4_BLOCK5_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_TIER4_BLOCK6_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_TIER4_BLOCK7_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_TIER4_BLOCK8_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_TIER4_BLOCK9_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_TIER4_BLOCK10_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_TIER4_BLOCK11_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_TIER4_BLOCK12_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_TIER4_BLOCK13_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_TIER4_BLOCK14_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_TIER4_BLOCK15_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_TIER4_BLOCK16_PRICE,

  ZB_ZCL_ATTR_PRICE_SRV_TIER5_BLOCK1_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_TIER5_BLOCK2_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_TIER5_BLOCK3_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_TIER5_BLOCK4_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_TIER5_BLOCK5_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_TIER5_BLOCK6_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_TIER5_BLOCK7_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_TIER5_BLOCK8_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_TIER5_BLOCK9_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_TIER5_BLOCK10_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_TIER5_BLOCK11_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_TIER5_BLOCK12_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_TIER5_BLOCK13_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_TIER5_BLOCK14_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_TIER5_BLOCK15_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_TIER5_BLOCK16_PRICE,

  ZB_ZCL_ATTR_PRICE_SRV_TIER6_BLOCK1_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_TIER6_BLOCK2_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_TIER6_BLOCK3_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_TIER6_BLOCK4_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_TIER6_BLOCK5_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_TIER6_BLOCK6_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_TIER6_BLOCK7_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_TIER6_BLOCK8_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_TIER6_BLOCK9_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_TIER6_BLOCK10_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_TIER6_BLOCK11_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_TIER6_BLOCK12_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_TIER6_BLOCK13_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_TIER6_BLOCK14_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_TIER6_BLOCK15_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_TIER6_BLOCK16_PRICE,

  ZB_ZCL_ATTR_PRICE_SRV_TIER7_BLOCK1_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_TIER7_BLOCK2_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_TIER7_BLOCK3_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_TIER7_BLOCK4_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_TIER7_BLOCK5_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_TIER7_BLOCK6_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_TIER7_BLOCK7_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_TIER7_BLOCK8_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_TIER7_BLOCK9_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_TIER7_BLOCK10_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_TIER7_BLOCK11_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_TIER7_BLOCK12_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_TIER7_BLOCK13_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_TIER7_BLOCK14_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_TIER7_BLOCK15_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_TIER7_BLOCK16_PRICE,

  ZB_ZCL_ATTR_PRICE_SRV_TIER8_BLOCK1_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_TIER8_BLOCK2_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_TIER8_BLOCK3_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_TIER8_BLOCK4_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_TIER8_BLOCK5_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_TIER8_BLOCK6_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_TIER8_BLOCK7_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_TIER8_BLOCK8_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_TIER8_BLOCK9_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_TIER8_BLOCK10_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_TIER8_BLOCK11_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_TIER8_BLOCK12_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_TIER8_BLOCK13_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_TIER8_BLOCK14_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_TIER8_BLOCK15_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_TIER8_BLOCK16_PRICE,

  ZB_ZCL_ATTR_PRICE_SRV_TIER9_BLOCK1_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_TIER9_BLOCK2_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_TIER9_BLOCK3_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_TIER9_BLOCK4_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_TIER9_BLOCK5_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_TIER9_BLOCK6_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_TIER9_BLOCK7_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_TIER9_BLOCK8_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_TIER9_BLOCK9_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_TIER9_BLOCK10_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_TIER9_BLOCK11_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_TIER9_BLOCK12_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_TIER9_BLOCK13_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_TIER9_BLOCK14_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_TIER9_BLOCK15_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_TIER9_BLOCK16_PRICE,

  ZB_ZCL_ATTR_PRICE_SRV_TIER10_BLOCK1_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_TIER10_BLOCK2_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_TIER10_BLOCK3_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_TIER10_BLOCK4_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_TIER10_BLOCK5_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_TIER10_BLOCK6_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_TIER10_BLOCK7_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_TIER10_BLOCK8_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_TIER10_BLOCK9_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_TIER10_BLOCK10_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_TIER10_BLOCK11_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_TIER10_BLOCK12_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_TIER10_BLOCK13_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_TIER10_BLOCK14_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_TIER10_BLOCK15_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_TIER10_BLOCK16_PRICE,

  ZB_ZCL_ATTR_PRICE_SRV_TIER11_BLOCK1_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_TIER11_BLOCK2_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_TIER11_BLOCK3_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_TIER11_BLOCK4_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_TIER11_BLOCK5_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_TIER11_BLOCK6_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_TIER11_BLOCK7_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_TIER11_BLOCK8_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_TIER11_BLOCK9_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_TIER11_BLOCK10_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_TIER11_BLOCK11_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_TIER11_BLOCK12_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_TIER11_BLOCK13_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_TIER11_BLOCK14_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_TIER11_BLOCK15_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_TIER11_BLOCK16_PRICE,

  ZB_ZCL_ATTR_PRICE_SRV_TIER12_BLOCK1_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_TIER12_BLOCK2_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_TIER12_BLOCK3_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_TIER12_BLOCK4_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_TIER12_BLOCK5_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_TIER12_BLOCK6_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_TIER12_BLOCK7_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_TIER12_BLOCK8_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_TIER12_BLOCK9_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_TIER12_BLOCK10_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_TIER12_BLOCK11_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_TIER12_BLOCK12_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_TIER12_BLOCK13_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_TIER12_BLOCK14_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_TIER12_BLOCK15_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_TIER12_BLOCK16_PRICE,

  ZB_ZCL_ATTR_PRICE_SRV_TIER13_BLOCK1_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_TIER13_BLOCK2_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_TIER13_BLOCK3_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_TIER13_BLOCK4_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_TIER13_BLOCK5_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_TIER13_BLOCK6_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_TIER13_BLOCK7_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_TIER13_BLOCK8_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_TIER13_BLOCK9_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_TIER13_BLOCK10_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_TIER13_BLOCK11_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_TIER13_BLOCK12_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_TIER13_BLOCK13_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_TIER13_BLOCK14_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_TIER13_BLOCK15_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_TIER13_BLOCK16_PRICE,

  ZB_ZCL_ATTR_PRICE_SRV_TIER14_BLOCK1_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_TIER14_BLOCK2_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_TIER14_BLOCK3_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_TIER14_BLOCK4_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_TIER14_BLOCK5_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_TIER14_BLOCK6_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_TIER14_BLOCK7_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_TIER14_BLOCK8_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_TIER14_BLOCK9_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_TIER14_BLOCK10_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_TIER14_BLOCK11_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_TIER14_BLOCK12_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_TIER14_BLOCK13_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_TIER14_BLOCK14_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_TIER14_BLOCK15_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_TIER14_BLOCK16_PRICE,

  ZB_ZCL_ATTR_PRICE_SRV_TIER15_BLOCK1_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_TIER15_BLOCK2_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_TIER15_BLOCK3_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_TIER15_BLOCK4_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_TIER15_BLOCK5_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_TIER15_BLOCK6_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_TIER15_BLOCK7_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_TIER15_BLOCK8_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_TIER15_BLOCK9_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_TIER15_BLOCK10_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_TIER15_BLOCK11_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_TIER15_BLOCK12_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_TIER15_BLOCK13_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_TIER15_BLOCK14_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_TIER15_BLOCK15_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_TIER15_BLOCK16_PRICE,
};

/** @cond internals_doc */

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_NO_TIER_BLOCKN_PRICE_GNR(blockN, data_ptr) \
{                                                                                           \
  ZB_ZCL_ATTR_PRICE_SRV_NO_TIER_BLOCK ## blockN ## _PRICE,                                  \
  ZB_ZCL_ATTR_TYPE_U32,                                                                     \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                                             \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                                       \
  (void*)  (data_ptr)                                                                       \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_TIERN_BLOCKM_PRICE_GNR(tierN, blockM, data_ptr) \
{                                                                                           \
  ZB_ZCL_ATTR_PRICE_SRV_TIER ## tierN ## _BLOCK ## blockM ## _PRICE,                        \
  ZB_ZCL_ATTR_TYPE_U32,                                                                     \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                                             \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                                       \
  (void*) (data_ptr)                                                                        \
}

/** @endcond */ /* internals_doc */

/** @} */ /* end of ZB_ZCL_PRICE_SET_BLOCK_PRICE_INFORMATION_GROUP */


/******************************************************************************/

/** @defgroup ZB_ZCL_PRICE_SET_EXTENDED_PRICE_INFORMATION_GROUP Extended Price Information
 *  @{
 *    @details
 *    In case of TOU charging only, the price server allows support for up to 48
 *    TOU rates. To reduce the number of attributes, Tier1Block1Price through
 *    Tier15Block1Price attributes are reused to represent rates for tiers 1 to 15.
 *    Rates for tiers 16 to 48 are provided in the extended price information set.
 */

/** @brief Extended Price Information (Delivered) Attributes Set
 *  @see SE spec, D.4.2.2.6
 */
enum zb_zcl_price_set_extended_price_information_e
{
  /* reserved 0x0500 - 0x050E */

  /** Attributes PriceTier16 through PriceTier48 represent the price of Energy,
   *  Gas, or Water delivered to the premises (i.e. delivered to the customer
   *  from the utility) at a specific price tier.
   */
  ZB_ZCL_ATTR_PRICE_SRV_PRICE_TIER16 = ZB_ZCL_ATTR_SET_WITH_ATTR_ID(ZB_ZCL_PRICE_SET_EXTENDED_PRICE_INFORMATION, 0x0F),
  ZB_ZCL_ATTR_PRICE_SRV_PRICE_TIER17,
  ZB_ZCL_ATTR_PRICE_SRV_PRICE_TIER18,
  ZB_ZCL_ATTR_PRICE_SRV_PRICE_TIER19,
  ZB_ZCL_ATTR_PRICE_SRV_PRICE_TIER20,
  ZB_ZCL_ATTR_PRICE_SRV_PRICE_TIER21,
  ZB_ZCL_ATTR_PRICE_SRV_PRICE_TIER22,
  ZB_ZCL_ATTR_PRICE_SRV_PRICE_TIER23,
  ZB_ZCL_ATTR_PRICE_SRV_PRICE_TIER24,
  ZB_ZCL_ATTR_PRICE_SRV_PRICE_TIER25,
  ZB_ZCL_ATTR_PRICE_SRV_PRICE_TIER26,
  ZB_ZCL_ATTR_PRICE_SRV_PRICE_TIER27,
  ZB_ZCL_ATTR_PRICE_SRV_PRICE_TIER28,
  ZB_ZCL_ATTR_PRICE_SRV_PRICE_TIER29,
  ZB_ZCL_ATTR_PRICE_SRV_PRICE_TIER30,
  ZB_ZCL_ATTR_PRICE_SRV_PRICE_TIER31,
  ZB_ZCL_ATTR_PRICE_SRV_PRICE_TIER32,
  ZB_ZCL_ATTR_PRICE_SRV_PRICE_TIER33,
  ZB_ZCL_ATTR_PRICE_SRV_PRICE_TIER34,
  ZB_ZCL_ATTR_PRICE_SRV_PRICE_TIER35,
  ZB_ZCL_ATTR_PRICE_SRV_PRICE_TIER36,
  ZB_ZCL_ATTR_PRICE_SRV_PRICE_TIER37,
  ZB_ZCL_ATTR_PRICE_SRV_PRICE_TIER38,
  ZB_ZCL_ATTR_PRICE_SRV_PRICE_TIER39,
  ZB_ZCL_ATTR_PRICE_SRV_PRICE_TIER40,
  ZB_ZCL_ATTR_PRICE_SRV_PRICE_TIER41,
  ZB_ZCL_ATTR_PRICE_SRV_PRICE_TIER42,
  ZB_ZCL_ATTR_PRICE_SRV_PRICE_TIER43,
  ZB_ZCL_ATTR_PRICE_SRV_PRICE_TIER44,
  ZB_ZCL_ATTR_PRICE_SRV_PRICE_TIER45,
  ZB_ZCL_ATTR_PRICE_SRV_PRICE_TIER46,
  ZB_ZCL_ATTR_PRICE_SRV_PRICE_TIER47,
  ZB_ZCL_ATTR_PRICE_SRV_PRICE_TIER48,
  /* reserved 0x0530 - 0x05FD */

  /** Attribute CPP1 Price represents the price of Energy, Gas, or Water
   *  delivered to the premises (i.e. delivered to the customer from the utility)
   *  while Critical Peak Pricing 'CPP1' is being applied.
   */
  ZB_ZCL_ATTR_PRICE_SRV_CPP1_PRICE = ZB_ZCL_ATTR_SET_WITH_ATTR_ID(ZB_ZCL_PRICE_SET_EXTENDED_PRICE_INFORMATION, 0x0FE),

  /** Attribute CPP2 Price represents the price of Energy, Gas, or Water
   *  delivered to the premises (i.e. delivered to the customer from the utility)
   *  while Critical Peak Pricing 'CPP2' is being applied.
   */
  ZB_ZCL_ATTR_PRICE_SRV_CPP2_PRICE,
};

/** @cond internals_doc */

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_PRICE_TIERN_GNR(tierN, data_ptr) \
{                                                                                 \
  ZB_ZCL_ATTR_PRICE_SRV_PRICE_TIER ## tierN,                                      \
  ZB_ZCL_ATTR_TYPE_U32,                                                           \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                                   \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                             \
  (void*)  (data_ptr)                                                             \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_CPPN_PRICE_GNR(cppN, data_ptr)   \
{                                                                                 \
  ZB_ZCL_ATTR_PRICE_SRV_CPP ## cppN ## _PRICE,                                    \
  ZB_ZCL_ATTR_TYPE_U32,                                                           \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                                   \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                             \
  (void*) (data_ptr)                                                              \
}

/** @endcond */ /* internals_doc */

/** @} */ /* end of ZB_ZCL_PRICE_SET_EXTENDED_PRICE_INFORMATION_GROUP */


/******************************************************************************/

/** @defgroup ZB_ZCL_PRICE_SET_TARIFF_INFORMATION_GROUP Tariff Information
 *  @{
 *    @details
 *    This set of attributes represents items that are associated with a
 *    particular Price Tariff.
 *    @note The terms tier and rate are used
 *    interchangeably here, but do define the same thing.
 */

/** @brief Tariff Information (Delivered) Attributes Set
 *  @see SE spec, D.4.2.2.7
 */
enum zb_zcl_price_set_tariff_information_e
{
  /* reserved 0x0600 - 0x060F */

  /** The TariffLabel attribute provides a method for utilities to assign a
   *  label to an entire set of tariff information.
   */
  ZB_ZCL_ATTR_PRICE_SRV_TARIFF_LABEL = ZB_ZCL_ATTR_SET_WITH_ATTR_ID(ZB_ZCL_PRICE_SET_TARIFF_INFORMATION, 0x10),

  /** An 8-bit integer which indicates the number of price tiers used while this
   *  tariff is active.
   */
  ZB_ZCL_ATTR_PRICE_SRV_NUMBER_OF_PRICE_TIERS_IN_USE,

  /** An 8-bit integer which indicates the total number of block thresholds used
   *  in the currently active tariff.
   */
  ZB_ZCL_ATTR_PRICE_SRV_NUMBER_OF_BLOCK_THRESHOLDS_IN_USE,

  /** An 8-bit enumeration indicating how the mixed TOU / Block charging is to
   *  be applied.
   */
  ZB_ZCL_ATTR_PRICE_SRV_TIER_BLOCK_MODE,
  /* reserved 0x0614 */

  /** An 8-bit enumeration identifying the base unit of measure.
   */
  ZB_ZCL_ATTR_PRICE_SRV_UNIT_OF_MEASURE = ZB_ZCL_ATTR_SET_WITH_ATTR_ID(ZB_ZCL_PRICE_SET_TARIFF_INFORMATION, 0x15),

  /** An unsigned 16-bit integer containing identifying information concerning
   *  the local unit of currency used in the Price cluster.
   */
  ZB_ZCL_ATTR_PRICE_SRV_CURRENCY,

  /** An 8-bit BitMap used to determine where the decimal point is located for
   *  prices provided in the Standing Charge attribute and the Price Matrix command.
   */
  ZB_ZCL_ATTR_PRICE_SRV_PRICE_TRAILING_DIGIT,
  /* reserved 0x0618 */

  /** An 8-bit enumeration identifying the resolution period for Block Tariff.
   */
  ZB_ZCL_ATTR_PRICE_SRV_TARIFF_RESOLUTION_PERIOD = ZB_ZCL_ATTR_SET_WITH_ATTR_ID(ZB_ZCL_PRICE_SET_TARIFF_INFORMATION, 0x19),
  /* reserved 0x061A - 0x061F */

  /** Used to calculate the amount of carbon dioxide (CO2) produced from energy use.
   */
  ZB_ZCL_ATTR_PRICE_SRV_CO2 = ZB_ZCL_ATTR_SET_WITH_ATTR_ID(ZB_ZCL_PRICE_SET_TARIFF_INFORMATION, 0x20),

  /** This attribute is an 8-bit enumeration which defines the unit for the CO2 attribute.
   */
  ZB_ZCL_ATTR_PRICE_SRV_CO2_UNIT,

  /** An 8-bit Bit-Map used to determine where the decimal point is located in
   *  the CO2 attribute.
   */
  ZB_ZCL_ATTR_PRICE_SRV_CO2_TRAILING_DIGIT,
  /* reserved 0x628 - 0x06FF */
};

/** @cond internals_doc */

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_TARIFF_LABEL(data_ptr) \
{                                                                           \
  ZB_ZCL_ATTR_PRICE_SRV_TARIFF_LABEL,                                       \
  ZB_ZCL_ATTR_TYPE_OCTET_STRING,                                            \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                             \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                       \
  (void*) (data_ptr)                                                        \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_NUMBER_OF_PRICE_TIERS_IN_USE(data_ptr)  \
{                                                                                            \
  ZB_ZCL_ATTR_PRICE_SRV_NUMBER_OF_PRICE_TIERS_IN_USE,                                        \
  ZB_ZCL_ATTR_TYPE_U8,                                                                       \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                                              \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                                        \
  (void*) (data_ptr)                                                                         \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_NUMBER_OF_BLOCK_THRESHOLDS_IN_USE(data_ptr)  \
{                                                                                                 \
  ZB_ZCL_ATTR_PRICE_SRV_NUMBER_OF_BLOCK_THRESHOLDS_IN_USE,                                        \
  ZB_ZCL_ATTR_TYPE_U8,                                                                            \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                                                   \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                                             \
  (void*) (data_ptr)                                                                              \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_TIER_BLOCK_MODE(data_ptr)  \
{                                                                               \
  ZB_ZCL_ATTR_PRICE_SRV_TIER_BLOCK_MODE,                                        \
  ZB_ZCL_ATTR_TYPE_8BIT_ENUM,                                                   \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                                 \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                           \
  (void*) (data_ptr)                                                            \
}

/** @see SE spec, table D-82. */
#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_TIER_BLOCK_MODE(data_ptr)  \
{                                                                               \
  ZB_ZCL_ATTR_PRICE_SRV_TIER_BLOCK_MODE,                                        \
  ZB_ZCL_ATTR_TYPE_8BIT_ENUM,                                                   \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                                 \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                           \
  (void*) (data_ptr)                                                            \
}

/** @see @ref zb_zcl_metering_unit_of_measure_e */
#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_UNIT_OF_MEASURE(data_ptr)  \
{                                                                               \
  ZB_ZCL_ATTR_PRICE_SRV_UNIT_OF_MEASURE,                                        \
  ZB_ZCL_ATTR_TYPE_8BIT_ENUM,                                                   \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                                 \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                           \
  (void*) (data_ptr)                                                            \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_CURRENCY(data_ptr)         \
{                                                                               \
  ZB_ZCL_ATTR_PRICE_SRV_CURRENCY,                                               \
  ZB_ZCL_ATTR_TYPE_U16,                                                         \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                                 \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                           \
  (void*) (data_ptr)                                                            \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_PRICE_TRAILING_DIGIT(data_ptr)  \
{                                                                                    \
  ZB_ZCL_ATTR_PRICE_SRV_PRICE_TRAILING_DIGIT,                                        \
  ZB_ZCL_ATTR_TYPE_8BITMAP,                                                          \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                                      \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                                \
  (void*) (data_ptr)                                                                 \
}

/** @endcond */ /* internals_doc */

/** @brief @e TariffResolutionPeriod attribute values
 *  @see SE spec, Table D-107
 *  @see @ref ZB_ZCL_ATTR_PRICE_SRV_TARIFF_RESOLUTION_PERIOD
 */
typedef enum zb_zcl_price_tariff_resolution_period_e
{
  ZB_ZCL_ATTR_PRICE_TARIFF_RESOLUTION_PERIOD_NOT_DEFINED = 0x00,   /**< Not defined */
  ZB_ZCL_ATTR_PRICE_TARIFF_RESOLUTION_PERIOD_BLOCK_PERIOD,         /**< Block Period */
  ZB_ZCL_ATTR_PRICE_TARIFF_RESOLUTION_PERIOD_1DAY,                 /**< 1 Day */
} zb_zcl_price_tariff_resolution_period_t;

/** @cond internals_doc */

/** @see @ref zb_zcl_price_tariff_resolution_period_t. */
#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_TARIFF_RESOLUTION_PERIOD(data_ptr)   \
{                                                                                         \
  ZB_ZCL_ATTR_PRICE_SRV_TARIFF_RESOLUTION_PERIOD,                                         \
  ZB_ZCL_ATTR_TYPE_8BIT_ENUM,                                                             \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                                           \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                                     \
  (void*) (data_ptr)                                                                      \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_CO2(data_ptr)  \
{                                                                   \
  ZB_ZCL_ATTR_PRICE_SRV_CO2,                                        \
  ZB_ZCL_ATTR_TYPE_U32,                                             \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                     \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                               \
  (void*) (data_ptr)                                                \
}

/** @endcond */ /* internals_doc */

/** @brief @e CO2Unit attribute values
 *  @see SE spec, Table D-83.
 *  @see ZB_ZCL_ATTR_PRICE_SRV_CO2_UNIT
 */
typedef enum zb_zcl_price_co2_unit_e
{
  ZB_ZCL_ATTR_PRICE_CO2_UNIT_KG_PER_KWH                  = 0x01,   /**< kg per kWh */
  ZB_ZCL_ATTR_PRICE_CO2_UNIT_KG_PER_GALLON_OF_GASOLINE   = 0x02,   /**< kg per Gallon of Gasoline */
  ZB_ZCL_ATTR_PRICE_CO2_UNIT_KG_PER_THERM_OF_NATURAL_GAS = 0x03,   /**< kg per Therm of Natural Gas */
} zb_zcl_price_co2_unit_t;

/** @cond internals_doc */

/** @see @ref zb_zcl_price_co2_unit_t */
#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_CO2_UNIT(data_ptr)   \
{                                                                         \
  ZB_ZCL_ATTR_PRICE_SRV_CO2_UNIT,                                         \
  ZB_ZCL_ATTR_TYPE_8BIT_ENUM,                                             \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                           \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                     \
  (void*) (data_ptr)                                                      \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_CO2_TRAILING_DIGIT(data_ptr)   \
{                                                                         \
  ZB_ZCL_ATTR_PRICE_SRV_CO2_TRAILING_DIGIT,                               \
  ZB_ZCL_ATTR_TYPE_8BITMAP,                                               \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                           \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                     \
  (void*) (data_ptr)                                                      \
}

/** @endcond */ /* internals_doc */

/** @} */ /* end of ZB_ZCL_PRICE_SET_TARIFF_INFORMATION_GROUP */


/******************************************************************************/

/** @defgroup ZB_ZCL_PRICE_SET_BILLING_INFORMATION_GROUP Billing Information
 *  @{
 *    @details
 *    This set of attributes provides remote access to the Price server
 *    Billing information.
 */

/** @brief Billing Information (Delivered) Attributes Set
 *  @see SE spec, D.4.2.2.8
 */
enum zb_zcl_price_set_billing_information_e
{
  /** The CurrentBillingPeriodStart attribute represents the start time of the
   *  current billing period.
   */
  ZB_ZCL_ATTR_PRICE_SRV_CURRENT_BILLING_PERIOD_START = ZB_ZCL_ATTR_SET_WITH_ATTR_ID(ZB_ZCL_PRICE_SET_BILLING_INFORMATION, 0x00),

  /** The CurrentBillingPeriodDuration attribute represents the current billing
   *  period duration in minutes.
   */
  ZB_ZCL_ATTR_PRICE_SRV_CURRENT_BILLING_PERIOD_DURATION,

  /** The LastBillingPeriodStart attribute represents the start time of the last
   *  billing period.
   */
  ZB_ZCL_ATTR_PRICE_SRV_LAST_BILLING_PERIOD_START,

  /** The LastBillingPeriodDuration attribute is the duration of the last
   *  billing period in minutes (start to end of last billing period).
   */
  ZB_ZCL_ATTR_PRICE_SRV_LAST_BILLING_PERIOD_DURATION,

  /** The LastBillingPeriodConsolidatedBill attribute is an amount for the cost
   *  of the energy supplied from the date of the LastBillingPeriodStart
   *  attribute and until the duration of the LastBillingPeriodDuration attribute
   *  expires, measured in base unit of Currency with the decimal point located
   *  as indicated by the Trailing Digits attribute.
   */
  ZB_ZCL_ATTR_PRICE_SRV_LAST_BILLING_PERIOD_CONSOLIDATED_BILL,
  /* reserved 0x0705 - 0x07FF */
};

/** @cond internals_doc */

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_CURRENT_BILLING_PERIOD_START(data_ptr)   \
{                                                                         \
  ZB_ZCL_ATTR_PRICE_SRV_CURRENT_BILLING_PERIOD_START,                     \
  ZB_ZCL_ATTR_TYPE_UTC_TIME,                                              \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                           \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                     \
  (void*) (data_ptr)                                                      \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_CURRENT_BILLING_PERIOD_DURATION(data_ptr)   \
{                                                                                                \
  ZB_ZCL_ATTR_PRICE_SRV_CURRENT_BILLING_PERIOD_DURATION,                                         \
  ZB_ZCL_ATTR_TYPE_U24,                                                                          \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                                                  \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                                            \
  (void*) (data_ptr)                                                                             \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_LAST_BILLING_PERIOD_START(data_ptr)   \
{                                                                                          \
  ZB_ZCL_ATTR_PRICE_SRV_LAST_BILLING_PERIOD_START,                                         \
  ZB_ZCL_ATTR_TYPE_UTC_TIME,                                                               \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                                            \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                                      \
  (void*) (data_ptr)                                                                       \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_LAST_BILLING_PERIOD_DURATION(data_ptr)      \
{                                                                                                \
  ZB_ZCL_ATTR_PRICE_SRV_LAST_BILLING_PERIOD_DURATION,                                            \
  ZB_ZCL_ATTR_TYPE_U24,                                                                          \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                                                  \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                                            \
  (void*) (data_ptr)                                                                             \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_LAST_BILLING_PERIOD_CONSOLIDATED_BILL(data_ptr)   \
{                                                                                                      \
  ZB_ZCL_ATTR_PRICE_SRV_LAST_BILLING_PERIOD_CONSOLIDATED_BILL,                                         \
  ZB_ZCL_ATTR_TYPE_U32,                                                                                \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                                                        \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                                                  \
  (void*) (data_ptr)                                                                                   \
}

/** @endcond */ /* internals_doc */

/** @} */ /* end of ZB_ZCL_PRICE_SET_BILLING_INFORMATION_GROUP */


/******************************************************************************/

/** @defgroup ZB_ZCL_PRICE_SET_CREDIT_PAYMENT_GROUP Credit Payment
 *  @{
 *    @details
 *    The Credit Payments Attribute set provides a method for the HAN (IHD) to
 *    understand the current status of the credit-only payment made to the energy
 *    supplier. These payments are for a credit meter only and do not cover any
 *    Prepayment Top up or payment. This attribute set is used to display the bill
 *    on the IHD should this service be required. Devices that require this information
 *    should use standard ZCL commands to read this information.
 */

/** @brief Credit Payment Attributes Set
 *  @see SE spec, D.2.2.9
 */
enum zb_zcl_price_set_credit_payment_e
{
  /** The CreditPaymentDueDate attribute indicates the date and time when the
   *  next credit payment is due to be paid by the consumer to the supplier.
   */
  ZB_ZCL_ATTR_PRICE_SRV_CREDIT_PAYMENT_DUE_DATE = ZB_ZCL_ATTR_SET_WITH_ATTR_ID(ZB_ZCL_PRICE_SET_CREDIT_PAYMENT, 0x00),

  /** The CreditPaymentStatus attribute indicates the current status of the last payment.
   */
  ZB_ZCL_ATTR_PRICE_SRV_CREDIT_PAYMENT_STATUS,

  /** This is the total of the consolidated bill amounts accumulated since the
   *  last payment.
   */
  ZB_ZCL_ATTR_PRICE_SRV_CREDIT_PAYMENT_OVER_DUE_AMOUNT,
  /* reserved 0x0803 - 0x0809 */

  /** The PaymentDiscount attribute indicates the discount that the energy
   *  supplier has applied to the consolidated bill.
   */
  ZB_ZCL_ATTR_PRICE_SRV_CREDIT_PAYMENT = ZB_ZCL_ATTR_SET_WITH_ATTR_ID(ZB_ZCL_PRICE_SET_CREDIT_PAYMENT, 0x0A),

  /** The PaymentDiscountPeriod attribute indicates the period for which this
   *  discount shall be applied for.
   */
  ZB_ZCL_ATTR_PRICE_SRV_CREDIT_PAYMENT_PERIOD,
  /* reserved 0x080C - 0x080F */

  /** The CreditPayment attributes indicate the amount paid by the consumer to
   *  the energy supplier.
   */
  ZB_ZCL_ATTR_PRICE_SRV_CREDIT_PAYMENT_1 = ZB_ZCL_ATTR_SET_WITH_ATTR_ID(ZB_ZCL_PRICE_SET_CREDIT_PAYMENT, 0x10),

  /** The CreditPaymentDate attributes indicate the last time the consumer made
   *  a payment to the energy supplier.
   */
  ZB_ZCL_ATTR_PRICE_SRV_CREDIT_PAYMENT_DATE_1,

  /** The CreditPaymentRef attributes indicate the reference number given to the
   *  payment by the energy supplier.
   */
  ZB_ZCL_ATTR_PRICE_SRV_CREDIT_PAYMENT_REF_1,
  /* reserved 0x0813 - 0x081F */
  ZB_ZCL_ATTR_PRICE_SRV_CREDIT_PAYMENT_2 = ZB_ZCL_ATTR_SET_WITH_ATTR_ID(ZB_ZCL_PRICE_SET_CREDIT_PAYMENT, 0x20),
  ZB_ZCL_ATTR_PRICE_SRV_CREDIT_PAYMENT_DATE_2,
  ZB_ZCL_ATTR_PRICE_SRV_CREDIT_PAYMENT_REF_2,
  /* reserved 0x0823 - 0x082F */
  ZB_ZCL_ATTR_PRICE_SRV_CREDIT_PAYMENT_3 = ZB_ZCL_ATTR_SET_WITH_ATTR_ID(ZB_ZCL_PRICE_SET_CREDIT_PAYMENT, 0x30),
  ZB_ZCL_ATTR_PRICE_SRV_CREDIT_PAYMENT_DATE_3,
  ZB_ZCL_ATTR_PRICE_SRV_CREDIT_PAYMENT_REF_3,
  /* reserved 0x0833 - 0x083F */
  ZB_ZCL_ATTR_PRICE_SRV_CREDIT_PAYMENT_4 = ZB_ZCL_ATTR_SET_WITH_ATTR_ID(ZB_ZCL_PRICE_SET_CREDIT_PAYMENT, 0x40),
  ZB_ZCL_ATTR_PRICE_SRV_CREDIT_PAYMENT_DATE_4,
  ZB_ZCL_ATTR_PRICE_SRV_CREDIT_PAYMENT_REF_4,
  /* reserved 0x0843 - 0x084F */
  ZB_ZCL_ATTR_PRICE_SRV_CREDIT_PAYMENT_5 = ZB_ZCL_ATTR_SET_WITH_ATTR_ID(ZB_ZCL_PRICE_SET_CREDIT_PAYMENT, 0x50),
  ZB_ZCL_ATTR_PRICE_SRV_CREDIT_PAYMENT_DATE_5,
  ZB_ZCL_ATTR_PRICE_SRV_CREDIT_PAYMENT_REF_5,
  /* reserved 0x0853 - 0x08FF */
};

/** @cond internals_doc */

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_CREDIT_PAYMENT_DUE_DATE(data_ptr)   \
{                                                                                        \
  ZB_ZCL_ATTR_PRICE_SRV_CREDIT_PAYMENT_DUE_DATE,                                         \
  ZB_ZCL_ATTR_TYPE_U32,                                                                  \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                                          \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                                    \
  (void*) (data_ptr)                                                                     \
}

/** @endcond */ /* internals_doc */

/** @brief @e CreditPaymentStatus attribute values
 *  @see SE spec, Table D-86.
 *  @see ZB_ZCL_ATTR_PRICE_SRV_CREDIT_PAYMENT_STATUS
 */
typedef enum zb_zcl_price_credit_payment_status_e
{
  ZB_ZCL_PRICE_CREDIT_PAYMENT_PENDING            = 0x00,             /**< Pending */
  ZB_ZCL_PRICE_CREDIT_PAYMENT_RECEIVED           = 0x01,             /**< Received/Paid */
  ZB_ZCL_PRICE_CREDIT_PAYMENT_PAID               = ZB_ZCL_PRICE_CREDIT_PAYMENT_RECEIVED,    /**< @copydoc ZB_ZCL_PRICE_CREDIT_PAYMENT_RECEIVED */
  ZB_ZCL_PRICE_CREDIT_PAYMENT_OVERDUE            = 0x02,             /**< Overdue */
  ZB_ZCL_PRICE_CREDIT_PAYMENT_2PAYMENTS_OVERDUE  = 0x03,   /**< 2 Payments overdue */
  ZB_ZCL_PRICE_CREDIT_PAYMENT_3PAYMENTS_OVERDUE  = 0x04,   /**< 3 Payments overdue */
  /* reserved 0x05 - 0xFF */
} zb_zcl_price_credit_payment_status_t;


/** @cond internals_doc */

/** @see @ref zb_zcl_price_credit_payment_status_t */
#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_CREDIT_PAYMENT_STATUS(data_ptr) \
{                                                                                    \
  ZB_ZCL_ATTR_PRICE_SRV_CREDIT_PAYMENT_STATUS,                                       \
  ZB_ZCL_ATTR_TYPE_8BIT_ENUM,                                                        \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                                      \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                                \
  (void*) (data_ptr)                                                                 \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_CREDIT_PAYMENT_OVER_DUE_AMOUNT(data_ptr) \
{                                                                                        \
  ZB_ZCL_ATTR_PRICE_SRV_CREDIT_PAYMENT_OVER_DUE_AMOUNT,                                  \
  ZB_ZCL_ATTR_TYPE_S32,                                                                  \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                                          \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                                    \
  (void*) (data_ptr)                                                                     \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_CREDIT_PAYMENT(data_ptr)   \
{                                                                               \
  ZB_ZCL_ATTR_PRICE_SRV_CREDIT_PAYMENT,                                         \
  ZB_ZCL_ATTR_TYPE_S32,                                                         \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                                 \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                           \
  (void*) (data_ptr)                                                            \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_CREDIT_PAYMENT_PERIOD(data_ptr) \
{                                                                               \
  ZB_ZCL_ATTR_PRICE_SRV_CREDIT_PAYMENT_PERIOD,                                  \
  ZB_ZCL_ATTR_TYPE_S32,                                                         \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                                 \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                           \
  (void*) (data_ptr)                                                            \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_CREDIT_PAYMENT_N(paymentN, data_ptr) \
{                                                                               \
  ZB_ZCL_ATTR_PRICE_SRV_CREDIT_PAYMENT_ ## paymentN,                            \
  ZB_ZCL_ATTR_TYPE_U32,                                                         \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                                 \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                           \
  (void*) (data_ptr)                                                            \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_CREDIT_PAYMENT_DATE_N(paymentN, data_ptr) \
{                                                                               \
  ZB_ZCL_ATTR_PRICE_SRV_CREDIT_PAYMENT_DATE_ ## paymentN,                       \
  ZB_ZCL_ATTR_TYPE_UTC_TIME,                                                    \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                                 \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                           \
  (void*) (data_ptr)                                                            \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_CREDIT_PAYMENT_REF_N(paymentN, data_ptr) \
{                                                                               \
  ZB_ZCL_ATTR_PRICE_SRV_CREDIT_PAYMENT_REF_ ## paymentN,                        \
  ZB_ZCL_ATTR_TYPE_UTC_TIME,                                                    \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                                 \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                           \
  (void*) (data_ptr)                                                            \
}

/** @endcond */ /* internals_doc */

/** @} */ /* end of ZB_ZCL_PRICE_SET_CREDIT_PAYMENT_GROUP */


/******************************************************************************/

/** @defgroup ZB_ZCL_PRICE_SET_RECEIVED_TIER_LABEL_GROUP Tier Label
 *  @{
 */

/** @brief Received Tier Label Attributes Set
 *  @see SE spec, D.4.2.2.10
 */
enum zb_zcl_price_set_received_tier_label
{
  /** The ReceivedTierNPriceLabel attributes provide a method for utilities to
   *  assign a label to Received Price Tiers.
   */
  ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_TIER1_PRICE_LABEL = ZB_ZCL_ATTR_SET_WITH_ATTR_ID(ZB_ZCL_PRICE_SET_RECEIVED_TIER_LABEL, 0x00),
  ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_TIER2_PRICE_LABEL,
  ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_TIER3_PRICE_LABEL,
  ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_TIER4_PRICE_LABEL,
  ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_TIER5_PRICE_LABEL,
  ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_TIER6_PRICE_LABEL,
  ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_TIER7_PRICE_LABEL,
  ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_TIER8_PRICE_LABEL,
  ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_TIER9_PRICE_LABEL,
  ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_TIER10_PRICE_LABEL,
  ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_TIER11_PRICE_LABEL,
  ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_TIER12_PRICE_LABEL,
  ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_TIER13_PRICE_LABEL,
  ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_TIER14_PRICE_LABEL,
  ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_TIER15_PRICE_LABEL,
  ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_TIER16_PRICE_LABEL,
  ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_TIER17_PRICE_LABEL,
  ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_TIER18_PRICE_LABEL,
  ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_TIER19_PRICE_LABEL,
  ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_TIER20_PRICE_LABEL,
  ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_TIER21_PRICE_LABEL,
  ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_TIER22_PRICE_LABEL,
  ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_TIER23_PRICE_LABEL,
  ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_TIER24_PRICE_LABEL,
  ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_TIER25_PRICE_LABEL,
  ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_TIER26_PRICE_LABEL,
  ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_TIER27_PRICE_LABEL,
  ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_TIER28_PRICE_LABEL,
  ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_TIER29_PRICE_LABEL,
  ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_TIER30_PRICE_LABEL,
  ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_TIER31_PRICE_LABEL,
  ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_TIER32_PRICE_LABEL,
  ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_TIER33_PRICE_LABEL,
  ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_TIER34_PRICE_LABEL,
  ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_TIER35_PRICE_LABEL,
  ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_TIER36_PRICE_LABEL,
  ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_TIER37_PRICE_LABEL,
  ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_TIER38_PRICE_LABEL,
  ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_TIER39_PRICE_LABEL,
  ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_TIER40_PRICE_LABEL,
  ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_TIER41_PRICE_LABEL,
  ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_TIER42_PRICE_LABEL,
  ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_TIER43_PRICE_LABEL,
  ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_TIER44_PRICE_LABEL,
  ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_TIER45_PRICE_LABEL,
  ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_TIER46_PRICE_LABEL,
  ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_TIER47_PRICE_LABEL,
  ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_TIER48_PRICE_LABEL,
  /* reserved 0x8030 - 0x80FF */
};

/** @cond internals_doc */

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_TIERN_PRICE_LABEL_GNR(tierN, data_ptr) \
{                                                                               \
  ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_TIER ## tierN ## _PRICE_LABEL,                 \
  ZB_ZCL_ATTR_TYPE_OCTET_STRING,                                                \
  ZB_ZCL_ATTR_ACCESS_READ_WRITE,                                                \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                           \
  (void*) (data_ptr)                                                            \
}

/** @endcond */

/** @} */ /* end of ZB_ZCL_PRICE_SET_RECEIVED_TIER_LABEL_GROUP */


/******************************************************************************/

/** @defgroup ZB_ZCL_PRICE_SET_RECEIVED_BLOCK_THRESHOLD_GROUP Received Block Threshold
 *  @{
 *    @details
 *    This set of attributes provides remote access to the Price server
 *    ReceivedBlockThresholds. The number of block thresholds is indicated by the
 *    NumberOfBlockThresholds field in the associated PublishTariffInformation
 *    command. The number of blocks is one greater than the number of thresholds.
 */

/** @brief Received Block Threshold Attributes Set
 *  @see SE spec, D.4.2.2.11
 */
enum zb_zcl_price_set_received_block_threshold_e
{
  /** The format of these attributes is the same as for the 'Delivered' Block Thresholds
   */
  ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_BLOCK1_THRESHOLD = ZB_ZCL_ATTR_SET_WITH_ATTR_ID(ZB_ZCL_PRICE_SET_RECEIVED_BLOCK_THRESHOLD, 0x00),
  ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_BLOCK2_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_BLOCK3_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_BLOCK4_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_BLOCK5_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_BLOCK6_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_BLOCK7_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_BLOCK8_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_BLOCK9_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_BLOCK10_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_BLOCK11_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_BLOCK12_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_BLOCK13_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_BLOCK14_THRESHOLD,
  ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_BLOCK15_THRESHOLD,
  /* reserved 0x810F - 0x81FF */
};

/** @cond internals_doc */

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_BLOCKN_THRESHOLD_GNR(blockN, data_ptr)  \
{                                                                                       \
  ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_BLOCK ## blockN ## _THRESHOLD,                         \
  ZB_ZCL_ATTR_TYPE_48BIT,                                                               \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                                         \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                                   \
  (void*) (data_ptr)                                                                    \
}

/** @endcond */

/** @} */ /* end of ZB_ZCL_PRICE_SET_RECEIVED_BLOCK_THRESHOLD_GROUP */


/******************************************************************************/

/** @defgroup ZB_ZCL_PRICE_SET_RECEIVED_BLOCK_PERIOD_GROUP Received Block Period
 *  @{
 *    @details
 *    This set of attributes provides remote access to the Price server
 *    Received Block Threshold period (typically the billing cycle) information.
 */

/** @brief Received Block Period Attributes Set
 *  @see SE spec, D.4.2.2.12
 */
enum zb_zcl_price_set_received_block_period_e
{
  /** The format of this attribute is the same as for the 'Delivered'
   *  StartOfBlockPeriod attribute.
   */
  ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_START_OF_BLOCK_PERIOD = ZB_ZCL_ATTR_SET_WITH_ATTR_ID(ZB_ZCL_PRICE_SET_RECEIVED_BLOCK_PERIOD, 0x00),

  /** The format of this attribute is the same as for the 'Delivered'
   *  BlockPeriodDuration attribute.
   */
  ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_BLOCK_PERIOD_DURATION,

  /** The format of this attribute is the same as for the 'Delivered'
   *  ThresholdMultiplier attribute.
   */
  ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_THRESHOLD_MULTIPLIER,

  /** The format of this attribute is the same as for the 'Delivered'
   *  ThresholdDivisor attribute
   */
  ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_THRESHOLD_DIVISOR,
  /* reserved 0x8204 - 0x82FF */
};

/** @cond internals_doc */

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_START_OF_BLOCK_PERIOD(data_ptr)  \
{                                                                                     \
  ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_START_OF_BLOCK_PERIOD,                               \
  ZB_ZCL_ATTR_TYPE_UTC_TIME,                                                          \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                                       \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                                 \
  (void*) (data_ptr)                                                                  \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_BLOCK_PERIOD_DURATION(data_ptr)  \
{                                                                                     \
  ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_BLOCK_PERIOD_DURATION,                               \
  ZB_ZCL_ATTR_TYPE_U24,                                                               \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                                       \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                                 \
  (void*) (data_ptr)                                                                  \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_THRESHOLD_MULTIPLIER(data_ptr)  \
{                                                                                     \
  ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_THRESHOLD_MULTIPLIER,                                \
  ZB_ZCL_ATTR_TYPE_U24,                                                               \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                                       \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                                 \
  (void*) (data_ptr)                                                                  \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_THRESHOLD_DIVISOR(data_ptr)  \
{                                                                                     \
  ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_THRESHOLD_DIVISOR,                                   \
  ZB_ZCL_ATTR_TYPE_U24,                                                               \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                                       \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                                 \
  (void*) (data_ptr)                                                                  \
}

/** @endcond */ /* internals_doc */

/** @} */ /* end of ZB_ZCL_PRICE_SET_RECEIVED_BLOCK_PERIOD_GROUP */


/******************************************************************************/

/** @defgroup ZB_ZCL_PRICE_SET_RECEIVED_BLOCK_PRICE_INFORMATION_GROUP Received Block Price Information
 *  @{
 */

/** @brief Received Block Price Information Attributes Set
 *  @see SE spec, D.4.2.2.13
 */
enum zb_zcl_price_set_received_block_price_information_e
{
  /** The format and use of these attributes is the same as for the
   *  'Delivered' TierNBlockNPrice attributes.
   */
  ZB_ZCL_ATTR_PRICE_SRV_RX_NO_TIER_BLOCK1_PRICE = ZB_ZCL_ATTR_SET_WITH_ATTR_ID(ZB_ZCL_PRICE_SET_RECEIVED_BLOCK_PRICE_INFORMATION, 0x00),
  ZB_ZCL_ATTR_PRICE_SRV_RX_NO_TIER_BLOCK2_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_RX_NO_TIER_BLOCK3_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_RX_NO_TIER_BLOCK4_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_RX_NO_TIER_BLOCK5_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_RX_NO_TIER_BLOCK6_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_RX_NO_TIER_BLOCK7_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_RX_NO_TIER_BLOCK8_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_RX_NO_TIER_BLOCK9_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_RX_NO_TIER_BLOCK10_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_RX_NO_TIER_BLOCK11_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_RX_NO_TIER_BLOCK12_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_RX_NO_TIER_BLOCK13_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_RX_NO_TIER_BLOCK14_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_RX_NO_TIER_BLOCK15_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_RX_NO_TIER_BLOCK16_PRICE,

  ZB_ZCL_ATTR_PRICE_SRV_RX_TIER1_BLOCK1_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_RX_TIER1_BLOCK2_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_RX_TIER1_BLOCK3_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_RX_TIER1_BLOCK4_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_RX_TIER1_BLOCK5_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_RX_TIER1_BLOCK6_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_RX_TIER1_BLOCK7_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_RX_TIER1_BLOCK8_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_RX_TIER1_BLOCK9_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_RX_TIER1_BLOCK10_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_RX_TIER1_BLOCK11_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_RX_TIER1_BLOCK12_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_RX_TIER1_BLOCK13_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_RX_TIER1_BLOCK14_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_RX_TIER1_BLOCK15_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_RX_TIER1_BLOCK16_PRICE,

  ZB_ZCL_ATTR_PRICE_SRV_RX_TIER2_BLOCK1_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_RX_TIER2_BLOCK2_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_RX_TIER2_BLOCK3_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_RX_TIER2_BLOCK4_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_RX_TIER2_BLOCK5_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_RX_TIER2_BLOCK6_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_RX_TIER2_BLOCK7_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_RX_TIER2_BLOCK8_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_RX_TIER2_BLOCK9_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_RX_TIER2_BLOCK10_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_RX_TIER2_BLOCK11_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_RX_TIER2_BLOCK12_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_RX_TIER2_BLOCK13_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_RX_TIER2_BLOCK14_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_RX_TIER2_BLOCK15_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_RX_TIER2_BLOCK16_PRICE,

  ZB_ZCL_ATTR_PRICE_SRV_RX_TIER3_BLOCK1_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_RX_TIER3_BLOCK2_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_RX_TIER3_BLOCK3_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_RX_TIER3_BLOCK4_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_RX_TIER3_BLOCK5_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_RX_TIER3_BLOCK6_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_RX_TIER3_BLOCK7_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_RX_TIER3_BLOCK8_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_RX_TIER3_BLOCK9_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_RX_TIER3_BLOCK10_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_RX_TIER3_BLOCK11_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_RX_TIER3_BLOCK12_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_RX_TIER3_BLOCK13_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_RX_TIER3_BLOCK14_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_RX_TIER3_BLOCK15_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_RX_TIER3_BLOCK16_PRICE,

  ZB_ZCL_ATTR_PRICE_SRV_RX_TIER4_BLOCK1_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_RX_TIER4_BLOCK2_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_RX_TIER4_BLOCK3_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_RX_TIER4_BLOCK4_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_RX_TIER4_BLOCK5_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_RX_TIER4_BLOCK6_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_RX_TIER4_BLOCK7_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_RX_TIER4_BLOCK8_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_RX_TIER4_BLOCK9_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_RX_TIER4_BLOCK10_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_RX_TIER4_BLOCK11_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_RX_TIER4_BLOCK12_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_RX_TIER4_BLOCK13_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_RX_TIER4_BLOCK14_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_RX_TIER4_BLOCK15_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_RX_TIER4_BLOCK16_PRICE,

  ZB_ZCL_ATTR_PRICE_SRV_RX_TIER5_BLOCK1_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_RX_TIER5_BLOCK2_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_RX_TIER5_BLOCK3_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_RX_TIER5_BLOCK4_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_RX_TIER5_BLOCK5_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_RX_TIER5_BLOCK6_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_RX_TIER5_BLOCK7_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_RX_TIER5_BLOCK8_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_RX_TIER5_BLOCK9_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_RX_TIER5_BLOCK10_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_RX_TIER5_BLOCK11_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_RX_TIER5_BLOCK12_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_RX_TIER5_BLOCK13_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_RX_TIER5_BLOCK14_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_RX_TIER5_BLOCK15_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_RX_TIER5_BLOCK16_PRICE,

  ZB_ZCL_ATTR_PRICE_SRV_RX_TIER6_BLOCK1_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_RX_TIER6_BLOCK2_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_RX_TIER6_BLOCK3_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_RX_TIER6_BLOCK4_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_RX_TIER6_BLOCK5_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_RX_TIER6_BLOCK6_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_RX_TIER6_BLOCK7_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_RX_TIER6_BLOCK8_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_RX_TIER6_BLOCK9_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_RX_TIER6_BLOCK10_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_RX_TIER6_BLOCK11_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_RX_TIER6_BLOCK12_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_RX_TIER6_BLOCK13_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_RX_TIER6_BLOCK14_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_RX_TIER6_BLOCK15_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_RX_TIER6_BLOCK16_PRICE,

  ZB_ZCL_ATTR_PRICE_SRV_RX_TIER7_BLOCK1_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_RX_TIER7_BLOCK2_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_RX_TIER7_BLOCK3_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_RX_TIER7_BLOCK4_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_RX_TIER7_BLOCK5_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_RX_TIER7_BLOCK6_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_RX_TIER7_BLOCK7_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_RX_TIER7_BLOCK8_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_RX_TIER7_BLOCK9_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_RX_TIER7_BLOCK10_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_RX_TIER7_BLOCK11_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_RX_TIER7_BLOCK12_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_RX_TIER7_BLOCK13_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_RX_TIER7_BLOCK14_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_RX_TIER7_BLOCK15_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_RX_TIER7_BLOCK16_PRICE,

  ZB_ZCL_ATTR_PRICE_SRV_RX_TIER8_BLOCK1_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_RX_TIER8_BLOCK2_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_RX_TIER8_BLOCK3_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_RX_TIER8_BLOCK4_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_RX_TIER8_BLOCK5_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_RX_TIER8_BLOCK6_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_RX_TIER8_BLOCK7_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_RX_TIER8_BLOCK8_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_RX_TIER8_BLOCK9_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_RX_TIER8_BLOCK10_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_RX_TIER8_BLOCK11_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_RX_TIER8_BLOCK12_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_RX_TIER8_BLOCK13_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_RX_TIER8_BLOCK14_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_RX_TIER8_BLOCK15_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_RX_TIER8_BLOCK16_PRICE,

  ZB_ZCL_ATTR_PRICE_SRV_RX_TIER9_BLOCK1_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_RX_TIER9_BLOCK2_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_RX_TIER9_BLOCK3_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_RX_TIER9_BLOCK4_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_RX_TIER9_BLOCK5_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_RX_TIER9_BLOCK6_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_RX_TIER9_BLOCK7_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_RX_TIER9_BLOCK8_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_RX_TIER9_BLOCK9_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_RX_TIER9_BLOCK10_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_RX_TIER9_BLOCK11_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_RX_TIER9_BLOCK12_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_RX_TIER9_BLOCK13_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_RX_TIER9_BLOCK14_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_RX_TIER9_BLOCK15_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_RX_TIER9_BLOCK16_PRICE,

  ZB_ZCL_ATTR_PRICE_SRV_RX_TIER10_BLOCK1_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_RX_TIER10_BLOCK2_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_RX_TIER10_BLOCK3_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_RX_TIER10_BLOCK4_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_RX_TIER10_BLOCK5_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_RX_TIER10_BLOCK6_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_RX_TIER10_BLOCK7_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_RX_TIER10_BLOCK8_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_RX_TIER10_BLOCK9_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_RX_TIER10_BLOCK10_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_RX_TIER10_BLOCK11_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_RX_TIER10_BLOCK12_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_RX_TIER10_BLOCK13_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_RX_TIER10_BLOCK14_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_RX_TIER10_BLOCK15_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_RX_TIER10_BLOCK16_PRICE,

  ZB_ZCL_ATTR_PRICE_SRV_RX_TIER11_BLOCK1_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_RX_TIER11_BLOCK2_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_RX_TIER11_BLOCK3_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_RX_TIER11_BLOCK4_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_RX_TIER11_BLOCK5_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_RX_TIER11_BLOCK6_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_RX_TIER11_BLOCK7_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_RX_TIER11_BLOCK8_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_RX_TIER11_BLOCK9_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_RX_TIER11_BLOCK10_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_RX_TIER11_BLOCK11_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_RX_TIER11_BLOCK12_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_RX_TIER11_BLOCK13_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_RX_TIER11_BLOCK14_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_RX_TIER11_BLOCK15_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_RX_TIER11_BLOCK16_PRICE,

  ZB_ZCL_ATTR_PRICE_SRV_RX_TIER12_BLOCK1_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_RX_TIER12_BLOCK2_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_RX_TIER12_BLOCK3_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_RX_TIER12_BLOCK4_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_RX_TIER12_BLOCK5_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_RX_TIER12_BLOCK6_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_RX_TIER12_BLOCK7_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_RX_TIER12_BLOCK8_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_RX_TIER12_BLOCK9_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_RX_TIER12_BLOCK10_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_RX_TIER12_BLOCK11_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_RX_TIER12_BLOCK12_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_RX_TIER12_BLOCK13_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_RX_TIER12_BLOCK14_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_RX_TIER12_BLOCK15_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_RX_TIER12_BLOCK16_PRICE,

  ZB_ZCL_ATTR_PRICE_SRV_RX_TIER13_BLOCK1_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_RX_TIER13_BLOCK2_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_RX_TIER13_BLOCK3_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_RX_TIER13_BLOCK4_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_RX_TIER13_BLOCK5_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_RX_TIER13_BLOCK6_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_RX_TIER13_BLOCK7_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_RX_TIER13_BLOCK8_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_RX_TIER13_BLOCK9_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_RX_TIER13_BLOCK10_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_RX_TIER13_BLOCK11_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_RX_TIER13_BLOCK12_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_RX_TIER13_BLOCK13_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_RX_TIER13_BLOCK14_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_RX_TIER13_BLOCK15_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_RX_TIER13_BLOCK16_PRICE,

  ZB_ZCL_ATTR_PRICE_SRV_RX_TIER14_BLOCK1_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_RX_TIER14_BLOCK2_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_RX_TIER14_BLOCK3_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_RX_TIER14_BLOCK4_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_RX_TIER14_BLOCK5_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_RX_TIER14_BLOCK6_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_RX_TIER14_BLOCK7_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_RX_TIER14_BLOCK8_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_RX_TIER14_BLOCK9_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_RX_TIER14_BLOCK10_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_RX_TIER14_BLOCK11_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_RX_TIER14_BLOCK12_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_RX_TIER14_BLOCK13_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_RX_TIER14_BLOCK14_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_RX_TIER14_BLOCK15_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_RX_TIER14_BLOCK16_PRICE,

  ZB_ZCL_ATTR_PRICE_SRV_RX_TIER15_BLOCK1_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_RX_TIER15_BLOCK2_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_RX_TIER15_BLOCK3_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_RX_TIER15_BLOCK4_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_RX_TIER15_BLOCK5_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_RX_TIER15_BLOCK6_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_RX_TIER15_BLOCK7_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_RX_TIER15_BLOCK8_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_RX_TIER15_BLOCK9_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_RX_TIER15_BLOCK10_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_RX_TIER15_BLOCK11_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_RX_TIER15_BLOCK12_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_RX_TIER15_BLOCK13_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_RX_TIER15_BLOCK14_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_RX_TIER15_BLOCK15_PRICE,
  ZB_ZCL_ATTR_PRICE_SRV_RX_TIER15_BLOCK16_PRICE,
};

/** @cond internals_doc */

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_RX_NO_TIER_BLOCKN_PRICE_GNR(blockN, data_ptr) \
{                                                                                             \
  ZB_ZCL_ATTR_PRICE_SRV_RX_NO_TIER_BLOCK ## blockN ## _PRICE,                                 \
  ZB_ZCL_ATTR_TYPE_U32,                                                                       \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                                               \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                                         \
  (void*) (data_ptr)                                                                          \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_RX_TIERN_BLOCKM_PRICE_GNR(tierN, blockM, data_ptr)  \
{                                                                                                   \
  ZB_ZCL_ATTR_PRICE_SRV_RX_TIER ## tierN ## _BLOCK ## blockM ## _PRICE,                             \
  ZB_ZCL_ATTR_TYPE_U32,                                                                             \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                                                     \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                                               \
  (void*) (data_ptr)                                                                                \
}

/** @endcond */ /* internals_doc */

/** @} */ /* end of ZB_ZCL_PRICE_SET_RECEIVED_BLOCK_PRICE_INFORMATION_GROUP */


/******************************************************************************/

/** @defgroup ZB_ZCL_PRICE_SET_RECEIVED_EXTENDED_PRICE_INFORMATION_GROUP Received Extended Price Information
 *  @{
 *    @details
 *    In case of TOU charging only, the price server shall support up to 48 TOU
 *    rates. To reduce the number of attributes, RxTierNBlock1Price attributes
 *    are reused to represent rates for tiers 1 to 15. Rates for tiers 16 to 48
 *    are provided in the Received Extended Price Information Set.
 */

/** @brief Received Extended Price Information Attributes Set
 *  @see SE spec, D.4.2.2.14
 */
enum zb_zcl_price_set_received_extended_price_information_e
{
  /* reserved 0x8500 - 0x850E */

  /** The format and use of these attributes is the same as for the
   *  'Delivered' PriceTierN attributes.
   */
  ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_PRICE_TIER16 = ZB_ZCL_ATTR_SET_WITH_ATTR_ID(ZB_ZCL_PRICE_SET_RECEIVED_EXTENDED_PRICE_INFORMATION, 0x0F),
  ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_PRICE_TIER17,
  ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_PRICE_TIER18,
  ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_PRICE_TIER19,
  ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_PRICE_TIER20,
  ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_PRICE_TIER21,
  ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_PRICE_TIER22,
  ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_PRICE_TIER23,
  ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_PRICE_TIER24,
  ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_PRICE_TIER25,
  ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_PRICE_TIER26,
  ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_PRICE_TIER27,
  ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_PRICE_TIER28,
  ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_PRICE_TIER29,
  ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_PRICE_TIER30,
  ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_PRICE_TIER31,
  ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_PRICE_TIER32,
  ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_PRICE_TIER33,
  ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_PRICE_TIER34,
  ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_PRICE_TIER35,
  ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_PRICE_TIER36,
  ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_PRICE_TIER37,
  ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_PRICE_TIER38,
  ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_PRICE_TIER39,
  ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_PRICE_TIER40,
  ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_PRICE_TIER41,
  ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_PRICE_TIER42,
  ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_PRICE_TIER43,
  ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_PRICE_TIER44,
  ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_PRICE_TIER45,
  ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_PRICE_TIER46,
  ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_PRICE_TIER47,
  ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_PRICE_TIER48,
  /* reserved 0x8530 - 0x85FF */
};

/** @brief Default value for Price cluster revision global attribute */
#define ZB_ZCL_PRICE_CLUSTER_REVISION_DEFAULT ((zb_uint16_t)0x0002u)

/** @brief Default value for Tier1_PriceLabel attribute */
#define ZB_ZCL_PRICE_SRV_TIER1_PRICE_LABEL_DEFAULT_VALUE {0x54, 0x49, 0x45, 0x52, 0x20, 0x31 ,0x00}

/** @brief Default value for Tier2_PriceLabel attribute */
#define ZB_ZCL_PRICE_SRV_TIER2_PRICE_LABEL_DEFAULT_VALUE {0x54, 0x49, 0x45, 0x52, 0x20, 0x32 ,0x00}

/** @brief Default value for Tier3_PriceLabel attribute */
#define ZB_ZCL_PRICE_SRV_TIER3_PRICE_LABEL_DEFAULT_VALUE {0x54, 0x49, 0x45, 0x52, 0x20, 0x33 ,0x00}

/** @brief Default value for Tier4_PriceLabel attribute */
#define ZB_ZCL_PRICE_SRV_TIER4_PRICE_LABEL_DEFAULT_VALUE {0x54, 0x49, 0x45, 0x52, 0x20, 0x34 ,0x00}

/** @brief Default value for Tier5_PriceLabel attribute */
#define ZB_ZCL_PRICE_SRV_TIER5_PRICE_LABEL_DEFAULT_VALUE {0x54, 0x49, 0x45, 0x52, 0x20, 0x35 ,0x00}

/** @brief Default value for Tier6_PriceLabel attribute */
#define ZB_ZCL_PRICE_SRV_TIER6_PRICE_LABEL_DEFAULT_VALUE {0x54, 0x49, 0x45, 0x52, 0x20, 0x36 ,0x00}

/** @brief Default value for Tier7_PriceLabel attribute */
#define ZB_ZCL_PRICE_SRV_TIER7_PRICE_LABEL_DEFAULT_VALUE {0x54, 0x49, 0x45, 0x52, 0x20, 0x37 ,0x00}

/** @brief Default value for Tier8_PriceLabel attribute */
#define ZB_ZCL_PRICE_SRV_TIER8_PRICE_LABEL_DEFAULT_VALUE {0x54, 0x49, 0x45, 0x52, 0x20, 0x38 ,0x00}

/** @brief Default value for Tier9_PriceLabel attribute */
#define ZB_ZCL_PRICE_SRV_TIER9_PRICE_LABEL_DEFAULT_VALUE {0x54, 0x49, 0x45, 0x52, 0x20, 0x39 ,0x00}

/** @brief Default value for Tier10_PriceLabel attribute */
#define ZB_ZCL_PRICE_SRV_TIER10_PRICE_LABEL_DEFAULT_VALUE {0x54, 0x49, 0x45, 0x52, 0x20, 0x31 ,0x30, 0x00}

/** @brief Default value for Tier11_PriceLabel attribute */
#define ZB_ZCL_PRICE_SRV_TIER11_PRICE_LABEL_DEFAULT_VALUE {0x54, 0x49, 0x45, 0x52, 0x20, 0x31 ,0x31, 0x00}

/** @brief Default value for Tier12_PriceLabel attribute */
#define ZB_ZCL_PRICE_SRV_TIER12_PRICE_LABEL_DEFAULT_VALUE {0x54, 0x49, 0x45, 0x52, 0x20, 0x31 ,0x32, 0x00}

/** @brief Default value for Tier13_PriceLabel attribute */
#define ZB_ZCL_PRICE_SRV_TIER13_PRICE_LABEL_DEFAULT_VALUE {0x54, 0x49, 0x45, 0x52, 0x20, 0x31 ,0x33, 0x00}

/** @brief Default value for Tier14_PriceLabel attribute */
#define ZB_ZCL_PRICE_SRV_TIER14_PRICE_LABEL_DEFAULT_VALUE {0x54, 0x49, 0x45, 0x52, 0x20, 0x31 ,0x34, 0x00}

/** @brief Default value for Tier15_PriceLabel attribute */
#define ZB_ZCL_PRICE_SRV_TIER15_PRICE_LABEL_DEFAULT_VALUE {0x54, 0x49, 0x45, 0x52, 0x20, 0x31 ,0x35, 0x00}

/** @brief Default value for ConversionFactor attribute */
#define ZB_ZCL_PRICE_SRV_CONVERSION_FACTOR_DEFAULT_VALUE ((zb_uint32_t)0x10000000)

/** @brief Default value for ConversionFactorTrailingDigit attribute */
#define ZB_ZCL_PRICE_SRV_CONVERSION_FACTOR_TRAILING_DIGIT_DEFAULT_VALUE ((zb_uint8_t)0x70)

/** @brief Default value for CalorificValue attribute */
#define ZB_ZCL_PRICE_SRV_CALORIFIC_VALUE_DEFAULT_VALUE ((zb_uint32_t)0x2625A00)

/** @brief Default value for CalorificValueUnit attribute */
#define ZB_ZCL_PRICE_SRV_CALORIFIC_VALUE_UNIT_DEFAULT_VALUE ((zb_uint8_t)0x1)

/** @brief Default value for CalorificValueTrailingDigit attribute */
#define ZB_ZCL_PRICE_SRV_CALORIFIC_VALUE_TRAILING_DIGIT_DEFAULT_VALUE ((zb_uint8_t)0x60)
/** @cond internals_doc */

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_PRICE_TIERN_GNR(tierN, data_ptr)  \
{                                                                                               \
  ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_PRICE_TIER ## tierN,                                           \
  ZB_ZCL_ATTR_TYPE_U32,                                                                         \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                                                 \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                                           \
  (void*) (data_ptr)                                                                            \
}

/** @endcond */ /* internals_doc */

/** @} */ /* end of ZB_ZCL_PRICE_SET_RECEIVED_EXTENDED_PRICE_INFORMATION_GROUP */


/******************************************************************************/

/** @defgroup ZB_ZCL_PRICE_SET_RECEIVED_TARIFF_INFORMATION_GROUP Received Tariff Information
 *  @{
 *    @details
 *    The following set of attributes represents items that are associated with a
 *    particular Received Price Tariff.
 */

/** @brief Received Tariff Information Attributes Set
 *  @see SE spec, D.4.2.2.15
 */
enum zb_zcl_price_set_received_tariff_information_e
{
  /* reserved 0x8600 - 0x860F */

  /** The format and use of this attribute is the same as for the
   *  'Delivered' TariffLabel attribute.
   */
  ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_TARIFF_LABEL = ZB_ZCL_ATTR_SET_WITH_ATTR_ID(ZB_ZCL_PRICE_SET_RECEIVED_TARIFF_INFORMATION, 0x10),

  /** The format and use of this attribute is the same as for the
   *  'Delivered' NumberOfPriceTiersInUse attribute.
   */
  ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_NUMBER_OF_PRICE_TIERS_IN_USE,

  /** The format and use of this attribute is the same as for the
   *  'Delivered' NumberOfBlockThresholdsInUse attribute.
   */
  ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_NUMBER_OF_BLOCK_THRESHOLDS_IN_USE,

  /** The format and use of this attribute is the same as for the
   *  'Delivered' TierBlockMode attribute.
   */
  ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_TIER_BLOCK_MODE,
  /* reserved 0x8614 */

  /** An 8-bit enumeration identifying the resolution period for Block Tariff.
   */
  ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_TARIFF_RESOLUTION_PERIOD = ZB_ZCL_ATTR_SET_WITH_ATTR_ID(ZB_ZCL_PRICE_SET_RECEIVED_TARIFF_INFORMATION, 0x15),
  /* reserved 0x8616 - 0x8624 */

  /** The format and use of this attribute is the same as for the
   *  'Delivered' CO2 attribute.
   */
  ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_CO2 = ZB_ZCL_ATTR_SET_WITH_ATTR_ID(ZB_ZCL_PRICE_SET_RECEIVED_TARIFF_INFORMATION, 0x25),

  /** The format and use of this attribute is the same as for the
   *  'Delivered' CO2Unit attribute.
   */
  ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_CO2_UNIT,

  /** The format and use of this attribute is the same as for the
   *  'Delivered' CO2TrailingDigit attribute.
   */
  ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_CO2_TRAILING_DIGIT,
  /* reserved 0x8528 - 0x85FF */
};

/** @cond internals_doc */

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_TARIFF_LABEL(data_ptr)  \
{                                                                                     \
  ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_TARIFF_LABEL,                                        \
  ZB_ZCL_ATTR_TYPE_OCTET_STRING,                                                      \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                                       \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                                 \
  (void*) (data_ptr)                                                                  \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_NUMBER_OF_PRICE_TIERS_IN_USE(data_ptr)  \
{                                                                                                     \
  ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_NUMBER_OF_PRICE_TIERS_IN_USE,                                        \
  ZB_ZCL_ATTR_TYPE_U8,                                                                                \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                                                       \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                                                 \
  (void*) (data_ptr)                                                                                  \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_NUMBER_OF_BLOCK_THRESHOLDS_IN_USE(data_ptr)  \
{                                                                                                          \
  ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_NUMBER_OF_BLOCK_THRESHOLDS_IN_USE,                                        \
  ZB_ZCL_ATTR_TYPE_U8,                                                                                     \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                                                            \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                                                      \
  (void*) (data_ptr)                                                                                       \
}

/** @see SE spec, table D-82. */
#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_TIER_BLOCK_MODE(data_ptr)   \
{                                                                                         \
  ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_TIER_BLOCK_MODE,                                         \
  ZB_ZCL_ATTR_TYPE_8BIT_ENUM,                                                             \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                                           \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                                     \
  (void*) (data_ptr)                                                                      \
}

/** @see @ref zb_zcl_price_tariff_resolution_period_t. */
#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_TARIFF_RESOLUTION_PERIOD(data_ptr)    \
{                                                                                                   \
  ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_TARIFF_RESOLUTION_PERIOD,                                          \
  ZB_ZCL_ATTR_TYPE_8BIT_ENUM,                                                                       \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                                                     \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                                               \
  (void*) (data_ptr)                                                                                \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_CO2(data_ptr)   \
{                                                                             \
  ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_CO2,                                         \
  ZB_ZCL_ATTR_TYPE_U32,                                                       \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                               \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                         \
  (void*) (data_ptr)                                                          \
}

/** @see @ref zb_zcl_price_co2_unit_t */
#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_CO2_UNIT(data_ptr)    \
{                                                                                   \
  ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_CO2_UNIT,                                          \
  ZB_ZCL_ATTR_TYPE_8BIT_ENUM,                                                       \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                                     \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                               \
  (void*) (data_ptr)                                                                \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_CO2_TRAILING_DIGIT(data_ptr)    \
{                                                                                             \
  ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_CO2_TRAILING_DIGIT,                                          \
  ZB_ZCL_ATTR_TYPE_8BITMAP,                                                                   \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                                               \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                                         \
  (void*) (data_ptr)                                                                          \
}

/** @endcond */ /* internals_doc */

/** @} */ /* end of ZB_ZCL_PRICE_SET_RECEIVED_TARIFF_INFORMATION_GROUP */


/******************************************************************************/

/** @defgroup ZB_ZCL_PRICE_SET_RECEIVED_BILLING_INFORMATION_GROUP Received Billing Information
 *  @{
 *    @details
 *    The following set of attributes represents items that are associated with
 *    particular Received Billing information.
 */

/** @brief Received Billing Information Attributes Set
 *  @see SE spec, D.4.2.2.16
 */
enum zb_zcl_price_set_received_billing_information
{
  /** The format and use of this attribute is the same as for the
   *  'Delivered' CurrentBillingPeriodStart attribute.
   */
  ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_CURRENT_BILLING_PERIOD_START = ZB_ZCL_ATTR_SET_WITH_ATTR_ID(ZB_ZCL_PRICE_SET_RECEIVED_BILLING_INFORMATION, 0x00),

  /** The format and use of this attribute is the same as for the
   *  'Delivered' CurrentBillingPeriodDuration attribute.
   */
  ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_CURRENT_BILLING_PERIOD_DURATION,

  /** The format and use of this attribute is the same as for the
   *  'Delivered' LastBillingPeriodStart attribute.
   */
  ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_LAST_BILLING_PERIOD_START,

  /** The format and use of this attribute is the same as for the
   *  'Delivered' LastBillingPeriodDuration attribute.
   */
  ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_LAST_BILLING_PERIOD_DURATION,

  /** The format and use of this attribute is the same as for the
   *  'Delivered' LastBillingPeriodConsolidatedBill attribute.
   */
  ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_LAST_BILLING_PERIOD_CONSOLIDATED_BILL,
  /* reserved 0x8705 - 0x87FF */
};

/** @cond internals_doc */

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_CURRENT_BILLING_PERIOD_START(data_ptr)  \
{                                                                                                     \
  ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_CURRENT_BILLING_PERIOD_START,                                        \
  ZB_ZCL_ATTR_TYPE_UTC_TIME,                                                                          \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                                                       \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                                                 \
  (void*) (data_ptr)                                                                                  \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_CURRENT_BILLING_PERIOD_DURATION(data_ptr)   \
{                                                                                                         \
  ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_CURRENT_BILLING_PERIOD_DURATION,                                         \
  ZB_ZCL_ATTR_TYPE_U24,                                                                                   \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                                                           \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                                                     \
  (void*) (data_ptr)                                                                                      \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_LAST_BILLING_PERIOD_START(data_ptr) \
{                                                                                                 \
  ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_LAST_BILLING_PERIOD_START,                                       \
  ZB_ZCL_ATTR_TYPE_UTC_TIME,                                                                      \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                                                   \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                                             \
  (void*) (data_ptr)                                                                              \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_LAST_BILLING_PERIOD_DURATION(data_ptr)    \
{                                                                                                       \
  ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_LAST_BILLING_PERIOD_DURATION,                                          \
  ZB_ZCL_ATTR_TYPE_U24,                                                                                 \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                                                         \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                                                   \
  (void*) (data_ptr)                                                                                    \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_LAST_BILLING_PERIOD_CONSOLIDATED_BILL(data_ptr)   \
{                                                                                                               \
  ZB_ZCL_ATTR_PRICE_SRV_RECEIVED_LAST_BILLING_PERIOD_CONSOLIDATED_BILL,                                         \
  ZB_ZCL_ATTR_TYPE_U32,                                                                                         \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                                                                 \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                                                           \
  (void*) (data_ptr)                                                                                            \
}

/** @endcond */ /* internals_doc */

/** @} */ /* end of ZB_ZCL_PRICE_SET_RECEIVED_BILLING_INFORMATION_GROUP */


/******************************************************************************/

/** @def ZB_ZCL_DECLARE_PRICE_SRV_ATTRIB_LIST
 *  @brief Declare server-side Price cluster attributes
 *  @param attr_list - Attribute list name.
 *  @param commodity_type - pointer to variable to store @ref ZB_ZCL_ATTR_PRICE_SRV_COMMODITY_TYPE "CommodityType" attribute value.
*/
#define ZB_ZCL_DECLARE_PRICE_SRV_ATTRIB_LIST(attr_list, commodity_type)           \
  ZB_ZCL_START_DECLARE_ATTRIB_LIST_CLUSTER_REVISION(attr_list, ZB_ZCL_PRICE)    \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_PRICE_SRV_COMMODITY_TYPE, (commodity_type))  \
  ZB_ZCL_FINISH_DECLARE_ATTRIB_LIST

/** @} */ /* end of ZB_ZCL_PRICE_SRV_ATTRIBUTES_GROUP */


/******************************************************************************/

/** @defgroup ZB_ZCL_PRICE_CLIENT_ATTRIBUTES_GROUP Price cluster client attributes
 *  @{
 */

/** @brief Price Cluster client attributes */
typedef enum zb_zcl_price_cli_attr_e
{
  /** @e PriceIncreaseRandomizeMinutes attribute represents the maximum amount
   *  of time to be used when randomizing the response to a price increase.
   */
  ZB_ZCL_ATTR_PRICE_CLI_PRICE_INCREASE_RANDOMIZE_MINUTES  = 0x0000,

  /** @e PriceDecreaseRandomizeMinutes attribute represents the maximum number
   *  of minutes to be used when randomizing the response to a price decrease.
   */
  ZB_ZCL_ATTR_PRICE_CLI_PRICE_DECREASE_RANDOMIZE_MINUTES  = 0x0001,

  /** @e CommodityType attribute provides a label for identifying the type of pricing client present.
   */
  ZB_ZCL_ATTR_PRICE_CLI_COMMODITY_TYPE              = 0x0002,
} zb_zcl_price_cli_attr_t;

/** @cond internals_doc */

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_CLI_PRICE_INCREASE_RANDOMIZE_MINUTES(data_ptr)   \
{                                                                                                 \
  ZB_ZCL_ATTR_PRICE_CLI_PRICE_INCREASE_RANDOMIZE_MINUTES,                                         \
  ZB_ZCL_ATTR_TYPE_U8,                                                                            \
  ZB_ZCL_ATTR_ACCESS_READ_WRITE,                                                                  \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                                             \
  (void*) (data_ptr)                                                                              \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_CLI_PRICE_DECREASE_RANDOMIZE_MINUTES(data_ptr)   \
{                                                                                                 \
  ZB_ZCL_ATTR_PRICE_CLI_PRICE_DECREASE_RANDOMIZE_MINUTES,                                         \
  ZB_ZCL_ATTR_TYPE_U8,                                                                            \
  ZB_ZCL_ATTR_ACCESS_READ_WRITE,                                                                  \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                                             \
  (void*) (data_ptr)                                                                              \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_PRICE_CLI_COMMODITY_TYPE(data_ptr)   \
{                                                                               \
  ZB_ZCL_ATTR_PRICE_CLI_COMMODITY_TYPE,                                         \
  ZB_ZCL_ATTR_TYPE_8BIT_ENUM,                                                   \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                                 \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                           \
  (void*) (data_ptr)                                                            \
}

/** @endcond */ /* internals_doc */

/** @def ZB_ZCL_DECLARE_PRICE_CLI_ATTRIB_LIST
 *  @brief Declare client-side Price cluster attributes
 *  @param attr_list - Attribute list name
 *  @param inc_rand_min - pointer to variable to store @ref ZB_ZCL_ATTR_PRICE_CLI_PRICE_INCREASE_RANDOMIZE_MINUTES "PriceIncreaseRandomizeMinutes" attribute value.
 *  @param dec_rand_min - pointer to variable to store @ref ZB_ZCL_ATTR_PRICE_CLI_PRICE_DECREASE_RANDOMIZE_MINUTES "PriceDecreaseRandomizeMinutes" attribute value.
 *  @param commodity_type - pointer to variable to store "CommodityType" attribute value.
 */
#define ZB_ZCL_DECLARE_PRICE_CLI_ATTRIB_LIST(attr_list, inc_rand_min, dec_rand_min, commodity_type) \
  ZB_ZCL_START_DECLARE_ATTRIB_LIST_CLUSTER_REVISION(attr_list, ZB_ZCL_PRICE)                    \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_PRICE_CLI_PRICE_INCREASE_RANDOMIZE_MINUTES, (inc_rand_min))  \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_PRICE_CLI_PRICE_DECREASE_RANDOMIZE_MINUTES, (dec_rand_min))  \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_PRICE_CLI_COMMODITY_TYPE, (commodity_type))  \
  ZB_ZCL_FINISH_DECLARE_ATTRIB_LIST

/** @} */ /* end of ZB_ZCL_PRICE_CLIENT_ATTRIBUTES_GROUP */

/** @} */ /* end of ZB_ZCL_PRICE_ATTRS */


/******************************************************************************/

/** @defgroup ZB_ZCL_PRICE_CLUSTER_COMMANDS Price cluster commands
 *  @{
 */

/** @brief Price cluster client command identifiers.
 *  @see SE spec, subclause D.4.2.3.
 */
typedef enum zb_zcl_price_client_cmd_e
{
  /** GetCurrentPrice command  initiates a PublishPrice command for the current time.
   * @see SE spec, subclause D.4.2.3.2.
   * @see zb_zcl_price_get_current_price_payload_s
   */
  ZB_ZCL_PRICE_CLI_CMD_GET_CURRENT_PRICE             = 0x00,

  /** GetScheduledPrices command initiates a PublishPrice command for available
   *  price events.
   *  @see zb_zcl_price_get_scheduled_prices_payload_s
   * @see SE spec, subclause D.4.2.3.3. */
  ZB_ZCL_PRICE_CLI_CMD_GET_SCHEDULED_PRICES          = 0x01,

  /** PriceAcknowledgement command provides the ability to acknowledge a
   *  previously sent PublishPrice command.
   * @see SE spec, subclause D.4.2.3.4.
   * @see zb_zcl_price_ack_payload_s
   */
  ZB_ZCL_PRICE_CLI_CMD_PRICE_ACK                     = 0x02,

  /** GetBlockPeriod(s) command.
   * This command initiates a PublishBlockPeriod command for the currently
   * scheduled block periods.
   * @see SE spec, subclause D.4.2.3.5.
   * @see zb_zcl_price_get_block_period_payload_s
   */
  ZB_ZCL_PRICE_CLI_CMD_GET_BLOCK_PERIOD              = 0x03,

  /** GetConversionFactor command.
   * This command initiates a PublishConversionFactor command(s) for scheduled
   * conversion factor updates.
   * @see SE spec, subclause D.4.2.3.6
   * @see zb_zcl_price_get_conversion_factor_payload_s
   */
  ZB_ZCL_PRICE_CLI_CMD_GET_CONVERSION_FACTOR         = 0x04,

  /** GetCalorificValue command.
   * This command initiates a PublishCalorificValue command(s) for scheduled
   * calorific value updates.
   * @see SE spec, subclause D.4.2.3.7.
   * @see zb_zcl_price_client_get_calorific_value_payload_s
   */
  ZB_ZCL_PRICE_CLI_CMD_GET_CALORIFIC_VALUE           = 0x05,

  /** GetTariffInformation command.
   * This command initiates PublishTariffInformation command(s) for scheduled
   * tariff updates.
   * @see SE spec, subclause D.4.2.3.8.
   * @see zb_zcl_price_get_tariff_information_payload_s
   */
  ZB_ZCL_PRICE_CLI_CMD_GET_TARIFF_INFORMATION        = 0x06,

  /** GetPriceMatrix command.
   * This command initiates a PublishPriceMatrix command for the scheduled Price
   * Matrix updates.
   * @see SE spec, subclause D.4.2.3.9.
   * @see zb_zcl_price_get_price_matrix_payload_s
   */
  ZB_ZCL_PRICE_CLI_CMD_GET_PRICE_MATRIX              = 0x07,

  /** GetBlockThresholds command.
   * This command initiates a PublishBlockThreshold command for the scheduled
   * Block Threshold updates.
   * @see SE spec, subclause D.4.2.3.10.
   * @see zb_zcl_price_get_block_thresholds_payload_s
   */
  ZB_ZCL_PRICE_CLI_CMD_GET_BLOCK_THRESHOLDS          = 0x08,

  /** GetCO2Value command.
   * This command initiates PublishCO2Value command(s) for scheduled CO2
   * conversion factor updates.
   * @see SE spec, subclause D.4.2.3.11.
   * @see zb_zcl_price_get_co2_value_payload_s
   */
  ZB_ZCL_PRICE_CLI_CMD_GET_CO2_VALUE                 = 0x09,

  /** GetTierLabels command.
   * This command allows a client to retrieve the tier labels associated with a
   * given tariff; this command initiates a PublishTierLabels command from
   * the server.
   * @see SE spec, subclause D.4.2.3.12.
   * @see zb_zcl_price_get_tier_labels_payload_s
   */
  ZB_ZCL_PRICE_CLI_CMD_GET_TIER_LABELS               = 0x0A,

  /** GetBillingPeriod command.
   * This command initiates one or more PublishBillingPeriod commands for
   * currently scheduled billing periods.
   * @see SE spec, subclause D.4.2.3.13.
   * @see zb_zcl_price_get_billing_period_payload_s
   */
  ZB_ZCL_PRICE_CLI_CMD_GET_BILLING_PERIOD            = 0x0B,

  /** GetConsolidatedBill command.
   * This command initiates one or more PublishConsolidatedBill commands with
   * the requested billing information.
   * @see SE spec, subclause D.4.2.3.14.
   * @see zb_zcl_price_get_consolidated_bill_payload_s
   */
  ZB_ZCL_PRICE_CLI_CMD_GET_CONSOLIDATED_BILL         = 0x0C,

  /** CPPEventResponse command.
   * The CPPEventResponse command is sent from a Client (IHD) to the ESI to
   * notify it of a Critical Peak Pricing event authorization.
   *
   * @note The CPPEventResponse command in current revision of SE specification
   * is provisional and not certifiable. This feature may change before reaching
   * certifiable status in a future revision of SE specification.
   *
   * @see SE spec, subclause D.4.2.3.15.
   * @see zb_zcl_price_cpp_event_response_payload_s
   */
  ZB_ZCL_PRICE_CLI_CMD_CPP_EVENT_RESPONSE            = 0x0D,

  /** GetCreditPayment command.
   * This command initiates PublishCreditPayment commands for the requested
   * credit payment information.
   * @see SE spec, subclause D.4.2.3.16.
   * @see zb_zcl_price_get_credit_payment_payload_s
   */
  ZB_ZCL_PRICE_CLI_CMD_GET_CREDIT_PAYMENT            = 0x0E,

  /** GetCurrencyConversion command.
   * This command initiates a PublishCurrencyConversion command for the currency
   * conversion factor updates.
   * @see SE spec, subclause D.4.2.3.17.
   * @see zb_zcl_price_publish_currency_conversion_payload_s
   */
  ZB_ZCL_PRICE_CLI_CMD_GET_CURRENCY_CONVERSION       = 0x0F,

  /** GetTariffCancellation command.
   * This command initiates the return of the last CancelTariff command held on
   * the associated server.
   * @see SE spec, subclauseD.4.2.3.18.
   * @note This command has no payload.
   */
  ZB_ZCL_PRICE_CLI_CMD_GET_TARIFF_CANCELLATION       = 0x10,

  /* reserved 0x11 - 0xFF */
} zb_zcl_price_client_cmd_t;

/** @brief Price cluster server command identifiers
 *  @see SE spec, subclause D.4.2.4.
 */
typedef enum zb_zcl_price_server_cmd_e
{
  /** PublishPrice command.
   * The Publish Price command is generated in response to receiving a
   * GetCurrentPrice command, in response to a GetScheduledPrices command,
   * and when an update to the pricing information is available from the
   * commodity provider.
   * @see SE spec, subclause D.4.2.4.1.
   * @see zb_zcl_price_publish_price_payload_s
   */
  ZB_ZCL_PRICE_SRV_CMD_PUBLISH_PRICE                 = 0x00,

  /** PublishBlockPeriod command.
   * The Publish Block Period command is generated in response to receiving a
   * GetBlockPeriod(s) command (see sub-clause D.4.2.3.5) or when an update
   * to the block tariff schedule is available from the commodity provider.
   * @see SE spec, subclause D.4.2.4.2.
   * @see zb_zcl_price_publish_block_period_payload_s
   */
  ZB_ZCL_PRICE_SRV_CMD_PUBLISH_BLOCK_PERIOD          = 0x01,

  /** PublishConversionFactor command.
   * The PublishConversionFactor command is sent in response to a
   * GetConversionFactor command or if a new conversion factor is available.
   * @see SE spec, subclause D.4.2.4.3.
   * @see zb_zcl_price_publish_conversion_factor_payload_s
   */
  ZB_ZCL_PRICE_SRV_CMD_PUBLISH_CONVERSION_FACTOR     = 0x02,

  /** PublishCalorificValue command.
   * The PublishCalorificValue command is sent in response to a
   * GetCalorificValue command or if a new calorific value is available.
   * @see SE spec, subclause D.4.2.4.4.
   * @see zb_zcl_price_publish_calorific_value_s
   */
  ZB_ZCL_PRICE_SRV_CMD_PUBLISH_CALORIFIC_VALUE       = 0x03,

  /** PublishTariffInformation command.
   * The PublishTariffInformation command is sent in response to a
   * GetTariffInformation command or if new tariff information is available
   * (including Price Matrix and Block Thresholds).
   * @see SE spec, subclause D.4.2.4.5.
   * @see zb_zcl_price_publish_tariff_information_payload_s
   */
  ZB_ZCL_PRICE_SRV_CMD_PUBLISH_TARIFF_INFORMATION    = 0x04,

  /** PublishPriceMatrix command.
   * The PublishPriceMatrix command is used to publish the Block Price
   * Information Set (up to 15 tiers x 15 blocks) and
   * the Extended Price Information Set (up to 48 tiers).
   * The PublishPriceMatrix command is sent in response to
   * a GetPriceMatrix command.
   * @see SE spec, subclause D.4.2.4.6.
   * @see zb_zcl_price_publish_price_matrix_payload_s
   */
  ZB_ZCL_PRICE_SRV_CMD_PUBLISH_PRICE_MATRIX          = 0x05,

  /** PublishBlockThresholds command.
   * The PublishBlockThresholds command is sent in response to
   * a GetBlockThresholds command.
   * @see SE spec, subclause D.4.2.4.7.
   * @see zb_zcl_price_publish_block_thresholds_payload_s
   */
  ZB_ZCL_PRICE_SRV_CMD_PUBLISH_BLOCK_THRESHOLDS      = 0x06,

  /** PublishCO2Value command.
   * The PublishCO2Value command is sent in response to a GetCO2Value command or
   * if a new CO2 conversion factor is available.
   * @see SE spec, subclause D.4.2.4.8.
   * @see zb_zcl_price_publish_co2_value_payload_s
   */
  ZB_ZCL_PRICE_SRV_CMD_PUBLISH_CO2_VALUE             = 0x07,

  /** PublishTierLabels command.
   * The PublishTierLabels command is generated in response to receiving a
   * GetTierLabels command or when there is a tier label change.
   * @see SE spec, subclause D.4.2.4.9.
   * @see zb_zcl_price_publish_tier_labels_payload_s
   */
  ZB_ZCL_PRICE_SRV_CMD_PUBLISH_TIER_LABELS           = 0x08,

  /** PublishBillingPeriod command.
   * The PublishBillingPeriod command is generated in response to receiving a
   * GetBillingPeriod(s) command or when an update to the Billing schedule is
   * available from the commodity supplier.
   * @see SE spec, subclause D.4.2.4.10.
   * @see zb_zcl_price_publish_billing_period_payload_s
   */
  ZB_ZCL_PRICE_SRV_CMD_PUBLISH_BILLING_PERIOD        = 0x09,

  /** PublishConsolidatedBill command.
   * The PublishConsolidatedBill command is used to make consolidated billing
   * information from previous billing periods available to other end devices.
   * This command is issued in response to a GetConsolidatedBill command or
   * if new billing information is available.
   * @see SE spec, subclause D.4.2.4.11.
   * @see zb_zcl_price_publish_consolidated_bill_payload_s
   */
  ZB_ZCL_PRICE_SRV_CMD_PUBLISH_CONSOLIDATED_BILL     = 0x0A,

  /** PublishCPPEvent command.
   * The PublishCPPEvent command is sent from an ESI to its Price clients to
   * notify them of a Critical Peak Pricing (CPP) event.
   *
   * @note The CPPEventResponse command in current revision of SE specification
   * is provisional and not certifiable. This feature may change before reaching
   * certifiable status in a future revision of SE specification.
   *
   * @see SE spec, subclause D.4.2.4.12.
   * @see zb_zcl_price_publish_cpp_event_payload_s
   */
  ZB_ZCL_PRICE_SRV_CMD_PUBLISH_CPP_EVENT             = 0x0B,

  /** PublishCreditPayment command.
   * The PublishCreditPayment command is used to update the credit payment
   * information when available.
   * @see SE spec, subclause D.4.2.4.13.
   * @see zb_zcl_price_publish_credit_payment_payload_s
   */
  ZB_ZCL_PRICE_SRV_CMD_PUBLISH_CREDIT_PAYMENT        = 0x0C,

  /** PublishCurrencyConversion command.
   * The PublishCurrencyConversion command is sent in response to a
   * GetCurrencyConversion command or when a new currency becomes available.
   * @see SE spec, subclause D.4.2.4.14.
   * @see zb_zcl_price_publish_currency_conversion_payload_s
   */
  ZB_ZCL_PRICE_SRV_CMD_PUBLISH_CURRENCY_CONVERSION   = 0x0D,

  /** CancelTariff command.
   * The CancelTariff command indicates that all data associated with a
   * particular tariff instance should be discarded.
   * @see SE spec, subclause D.4.2.4.15.
   * @see zb_zcl_price_cancel_tariff_payload_s
   */
  ZB_ZCL_PRICE_SRV_CMD_CANCEL_TARIFF                 = 0x0E,

  /* reserved 0x0F - 0xFF */
} zb_zcl_price_server_cmd_t;

/* PRICE cluster commands list : only for information - do not modify */
#define ZB_ZCL_CLUSTER_ID_PRICE_SERVER_ROLE_GENERATED_CMD_LIST                 \
                                      ZB_ZCL_PRICE_SRV_CMD_PUBLISH_PRICE,      \
                                      ZB_ZCL_PRICE_SRV_CMD_PUBLISH_TIER_LABELS

#define ZB_ZCL_CLUSTER_ID_PRICE_CLIENT_ROLE_RECEIVED_CMD_LIST ZB_ZCL_CLUSTER_ID_PRICE_SERVER_ROLE_GENERATED_CMD_LIST

#define ZB_ZCL_CLUSTER_ID_PRICE_CLIENT_ROLE_GENERATED_CMD_LIST                   \
                                      ZB_ZCL_PRICE_CLI_CMD_GET_CURRENT_PRICE,    \
                                      ZB_ZCL_PRICE_CLI_CMD_GET_SCHEDULED_PRICES, \
                                      ZB_ZCL_PRICE_CLI_CMD_PRICE_ACK,            \
                                      ZB_ZCL_PRICE_CLI_CMD_GET_TIER_LABELS

#define ZB_ZCL_CLUSTER_ID_PRICE_SERVER_ROLE_RECEIVED_CMD_LIST ZB_ZCL_CLUSTER_ID_PRICE_CLIENT_ROLE_GENERATED_CMD_LIST


/** @brief @ref ZB_ZCL_PRICE_SRV_CMD_PUBLISH_PRICE "PublishPrice" command payload.
 *  @see SE spec, subclause D.4.2.3.1.1.
 */
typedef ZB_PACKED_PRE struct zb_zcl_price_publish_price_payload_s
{
  /** Unique identifier for the commodity provider. */
  zb_uint32_t provider_id;                                  /* (M) */

  /** Commodity provider-specific information regarding
   * the current billing rate.
   */
  zb_uint8_t rate_label[13];                                /* (M) */

  /** Unique identifier generated by the commodity provider
   * allows devices to determine which pricing information is newer.
   */
  zb_uint32_t issuer_event_id;                              /* (M) */

  /** UTC Time field containing the current time as determined
   * by the device.
   */
  zb_uint32_t current_time;                                 /* (M) */

  /** Field identifying the commodity as well as this base units of measure.
   * @see enum zb_zcl_metering_unit_of_measure_e
   */
  zb_uint8_t unit_of_measure;                               /* (M) */

  /** Field identifying information concerning the local unit of
   * currency used in the price field.
   * @see ISO-4217
   */
  zb_uint16_t currency;                                     /* (M) */

  /** Determines where the decimal point is located in the price field */
  zb_bitfield_t price_trailing_digit:4;                        /* (M) */

  /** Indicates the current price tier as chosen by the commodity provider */
  zb_bitfield_t price_tier:4;                                  /* (M) */

  /** Represents the maximum number of price tiers available. */
  zb_bitfield_t number_of_price_tiers:4;                       /* (M) */

  /** Indicates the register tier used with the
   * current Price Tier (@ref price_tier).
   */
  zb_bitfield_t register_tier:4;                               /* (M) */

  /** Denotes the time at which the signal becomes valid.
   * Value 0x00000000 is a special time denoting "now".
   */
  zb_uint32_t start_time;                                   /* (M) */

  /** Denotes amount of time in minutes after Start Time (@ref start_time)
   * during which the price signal is valid.
   */
  zb_uint16_t duration_in_minutes;                          /* (M) */

  /** Contains the price of the commodity measured in base unit of
   * Currency per Unit of Measure (@ref currency per @ref unit_of_measure)
   * with the decimal point located as indicated by
   * the Price Trailing Digit field (@ref price_trailing_digit) when
   * the commodity is delivered to premises.
   */
  zb_uint32_t price;  /* (M) */

  /* Optional fields. */

  /** Gives the ratio of the price denoted in the Price field (@ref price)
   * to the "normal" price, chosen by the commodity provider.
   */
  zb_uint8_t price_ratio;                                   /* (O) */

  /** Contains the price of the commodity measured in base unit of
   * Currency per Unit of Measure (@ref currency per @ref unit_of_measure)
   * with the decimal point located as indicated by
   * the Price Trailing Digit field (@ref price_trailing_digit) when
   * the commodity is received from premises.
   */
  zb_uint32_t generation_price;                             /* (O) */

  /** Gives the ratio of the price denoted in the Generation Price
   * field (@ref price) to the "normal" price,
   * chosen by the commodity provider.
   */
  zb_uint8_t generation_price_ratio;                        /* (O) */

  /** Provides mechanism to describe an alternative measure of the cost
   * of the energy consumed.
   */
  zb_uint32_t alternate_cost_delivered;                     /* (O) */

  /** Identifies the unit for Alternate Cost Delivered field
   * (@ref alternate_cost_delivered)
   */
  zb_uint8_t alternate_cost_unit;                           /* (O) */

  /** Determines where the decimal point is located in
   * the alternate cost field.
   */
  zb_uint8_t alternate_cost_trailing_digit;                 /* (O) */

  /** Indicates the number of block thresholds available. */
  zb_uint8_t number_of_block_thresholds;                    /* (O) */

  /** Identifies additional control options (PriceAcknowledgement policy and
   * total number of tier flag).
   */
  zb_uint8_t price_control;                                 /* (O) */

  /** Specifies the total number of generation tiers applicable in the
   * current tariff.
   */
  zb_uint8_t number_of_generation_tiers;                    /* (O) */

  /** Specifies the current generation tier. */
  zb_uint8_t generation_tier;                               /* (O) */

  /** Indicates a maximum number of tier available.
   * @see number_of_generation_tiers
   */
  zb_uint8_t extended_number_of_price_tiers;                /* (O) */

  /** Indicates the current price tier.
   * @see price_tier
   */
  zb_uint8_t extended_price_tier;                           /* (O) */

  /** Indicates the register tier used with the
   * current Price Tier (@ref register_tier).
   */
  zb_uint8_t extended_register_tier;                        /* (O) */

} ZB_PACKED_STRUCT zb_zcl_price_publish_price_payload_t;


/** @def ZB_ZCL_PRICE_PUBLISH_PRICE_PAYLOAD_SIZE_IS_VALID
 */
#define ZB_ZCL_PRICE_PUBLISH_PRICE_PAYLOAD_SIZE_IS_VALID(size) \
  (size > (sizeof(zb_zcl_price_publish_price_payload_t) \
           - ZB_SIZEOF_FIELD(zb_zcl_price_publish_price_payload_t, rate_label)))

/** @def ZB_ZCL_PRICE_PUBLISH_PRICE_PAYLOAD_EXPECTED_SIZE
 *  @brief Returns actual size of payload.
 *  @details PublishPrice packet payload doesn't have fixed length because of 'rate_label'
 *  field. This macro can be used to check payload size of incoming packet.
 */
#define ZB_ZCL_PRICE_PUBLISH_PRICE_PAYLOAD_EXPECTED_SIZE(pl) \
 (sizeof(*(pl)) \
  - (ZB_ZCL_STRING_CONST_SIZE((pl)->rate_label) - ZB_ZCL_GET_STRING_LENGTH((pl)->rate_label)))

/** @def ZB_ZCL_PRICE_DEFAULT_PRICE_CONTROL_VALUE
 *  @brief Controls PriceAck policy for outgoing PublishPrice packets.
 *  SE devices can enable this field (refer @ref zb_zcl_price_publish_price_payload_t::price_control)
 *  if they want to receive @ref ZB_ZCL_PRICE_CLI_CMD_PRICE_ACK "PriceAcknowledgement" command as
 *  response to @ref ZB_ZCL_PRICE_SRV_CMD_PUBLISH_PRICE "PublishPrice" command, but this behavior
 *  is valid only for SE version >= 1.1.
 *  SE 1.2 is used, so this field is set to 1 by default.
 */
#define ZB_ZCL_PRICE_DEFAULT_PRICE_CONTROL_VALUE 0x01

/** @def ZB_ZCL_PRICE_PUBLISH_PRICE_PAYLOAD_INIT
 *  @brief Macro for initializing @ref ZB_ZCL_PRICE_SRV_CMD_PUBLISH_PRICE "PublishPrice" Command payload.
 *  @see @ref zb_zcl_price_publish_price_payload_t
 *  @par Example
 *  @code{C}
 *    zb_zcl_price_publish_price_payload_t pl = ZB_ZCL_PRICE_PUBLISH_PRICE_PAYLOAD_INIT;
 *  @endcode
 */
#define ZB_ZCL_PRICE_PUBLISH_PRICE_PAYLOAD_INIT                     \
  (zb_zcl_price_publish_price_payload_t)                            \
{                                                             \
  .price_ratio = 0xFF,                                        \
  .generation_price = 0xFFFFFFFF,                             \
  .generation_price_ratio = 0xFF,                             \
  .alternate_cost_delivered = 0xFFFFFFFF,                     \
  .alternate_cost_unit = 0xFF,                                \
  .alternate_cost_trailing_digit = 0xFF,                      \
  .number_of_block_thresholds = 0xFF,                         \
  .price_control = ZB_ZCL_PRICE_DEFAULT_PRICE_CONTROL_VALUE,  \
  .number_of_generation_tiers = 0x00,                         \
  .generation_tier = 0x00,                                    \
  .extended_number_of_price_tiers = 0x00,                     \
  .extended_price_tier = 0x00,                                \
  .extended_register_tier = 0x00,                             \
}

/** @brief @ref ZB_ZCL_PRICE_CLI_CMD_PRICE_ACK "PriceAcknowledgement" Command payload
 *  @see SE spec, D.4.2.3.4.1
 */
typedef ZB_PACKED_PRE struct zb_zcl_price_ack_payload_s
{
  /** Unique identifier for the commodity provider. */
  zb_uint32_t provider_id;                                  /* (M) */

  /** Unique identifier generated by the commodity provider.
   * allows devices to determine which pricing information is newer.
   */
  zb_uint32_t issuer_event_id;                              /* (M) */

  /** @e UTCTime field containing the current time as determined
   * by the device.
   */
  zb_uint32_t current_time;                                 /* (M) */

  /** Identifies additional control options (PriceAcknowledgement policy and
   * total number of tier flag).
   */
  zb_uint8_t price_control;                                 /* (M) */
} ZB_PACKED_STRUCT zb_zcl_price_ack_payload_t;


/** @cond internals_doc */

/** Check if @ref ZB_ZCL_PRICE_CLI_CMD_PRICE_ACK "PriceAcknowledgement" Command payload is valid
 */
#define ZB_ZCL_PRICE_ACK_PAYLOAD_SIZE_IS_VALID(size) \
  ((size) >= sizeof(zb_zcl_price_ack_payload_t))

/** @ref zb_zcl_price_ack_payload_t initializer */
#define ZB_ZCL_PRICE_ACK_PAYLOAD_INIT (zb_zcl_price_ack_payload_t) {0}

/** Initialize @ref zb_zcl_price_ack_payload_t from
 * @ref zb_zcl_price_publish_price_payload_t values.
 */
#define ZB_ZCL_PRICE_ACK_PAYLOAD_INIT_FROM_PUBLISH_PRICE_PAYLOAD(pl) \
 (zb_zcl_price_ack_payload_t) { \
   .provider_id = (pl)->provider_id, \
   .issuer_event_id = (pl)->issuer_event_id, \
   .current_time = zb_get_utc_time(), \
   .price_control = (pl)->price_control, \
 }

/** @endcond */ /* internals_doc */

/** @brief @ref ZB_ZCL_PRICE_CLI_CMD_GET_CURRENT_PRICE "GetCurrentPrice" Command Payload Format
 */
typedef ZB_PACKED_PRE struct zb_zcl_price_get_current_price_payload_s
{
  /** @e CommandOptions field is 8 Bits in length
   * @note Zero bit is Requestor Rx On When Idle, others are reserved.
   */
  zb_uint8_t command_options;
} ZB_PACKED_STRUCT zb_zcl_price_get_current_price_payload_t;

/** @def ZB_ZCL_PRICE_GET_CURRENT_PRICE_PAYLOAD_INIT
 *  @brief Macro for initializing @ref ZB_ZCL_PRICE_CLI_CMD_GET_CURRENT_PRICE "GetCurrentPrice" Command payload
 */
#define ZB_ZCL_PRICE_GET_CURRENT_PRICE_PAYLOAD_INIT \
  (zb_zcl_price_get_current_price_payload_t) {0}

/** @def ZB_ZCL_PRICE_GET_CURRENT_PRICE_PAYLOAD_SIZE_IS_VALID
 *  @brief Check if @ref ZB_ZCL_PRICE_CLI_CMD_GET_CURRENT_PRICE "GetCurrentPrice" Command payload is valid
 */
#define ZB_ZCL_PRICE_GET_CURRENT_PRICE_PAYLOAD_SIZE_IS_VALID(size) \
  ((size) >= sizeof(zb_zcl_price_get_current_price_payload_t))

/** @brief @ref ZB_ZCL_PRICE_CLI_CMD_GET_SCHEDULED_PRICES "GetScheduledPrices" Command Payload Format
 *  @see SE spec, D.4.2.3.3.1
 */
typedef ZB_PACKED_PRE struct zb_zcl_price_get_scheduled_prices_payload_s
{
  /** @e StartTime represents the minimum ending time for any scheduled or
   * currently active pricing events to be resent.
   * @note Value  0x00000000 will be replaced with the current time stamp.
   */
  zb_uint32_t start_time;

  /** @e NumberOfEvents represents the maximum number of events to be sent.
   * @note Value 0 indicates no maximum limit.
   */
  zb_uint8_t  number_of_events;
} ZB_PACKED_STRUCT zb_zcl_price_get_scheduled_prices_payload_t;


/** @def ZB_ZCL_PRICE_GET_SCHEDULED_PRICES_PAYLOAD_INIT
 *  @brief Macro for initializing @ref ZB_ZCL_PRICE_CLI_CMD_GET_SCHEDULED_PRICES "GetScheduledPrices" Command payload
 */
#define ZB_ZCL_PRICE_GET_SCHEDULED_PRICES_PAYLOAD_INIT \
  (zb_zcl_price_get_scheduled_prices_payload_t) {0}

/** @def ZB_ZCL_PRICE_GET_SCHEDULED_PRICES_PAYLOAD_SIZE_IS_VALID
 *  @brief Check if @ref ZB_ZCL_PRICE_CLI_CMD_GET_SCHEDULED_PRICES "GetScheduledPrices" Command payload is valid
 */
#define ZB_ZCL_PRICE_GET_SCHEDULED_PRICES_PAYLOAD_SIZE_IS_VALID(size) \
  ((size) >= sizeof(zb_zcl_price_get_scheduled_prices_payload_t))

/** @ref ZB_ZCL_PRICE_CLI_CMD_GET_BLOCK_PERIOD "GetBlockPeriod" command payload
 * @see SE spec, subclause D.4.2.3.5
 */
typedef ZB_PACKED_PRE struct zb_zcl_price_get_block_period_payload_s
{
  /** UTC Timestamp representing the minimum ending time for any scheduled or currently block period
   *  events to be resent.
   */
  zb_uint32_t start_time;  /* (M) */

  /** An 8-bit Integer which indicates the maximum number of Publish Block Period commands
   * that can be sent.
   */
  zb_uint8_t number_of_events;  /* (M) */

  /** An 8-bit bitmap identifying the type of tariff published in this command. The least significant
   * nibble represents an enumeration of the tariff type.
   * @see SE spec, Table D-108
   * @see zb_zcl_price_tariff_type_t
   */
  zb_uint8_t tariff_type;  /* (O) */
} ZB_PACKED_STRUCT zb_zcl_price_client_get_block_period_payload_t;


/** @ref ZB_ZCL_PRICE_CLI_CMD_GET_CONVERSION_FACTOR "GetConversionFactor" command payload.
 * @see SE spec, subclause D.4.2.3.6.2
 */
typedef ZB_PACKED_PRE struct zb_zcl_price_get_conversion_factor_payload_s
{
  /** UTC Timestamp indicating the earliest start time of values to be returned by the corresponding
   * @e PublishConversionFactor command.
   * */
  zb_uint32_t earliest_start_time;  /* (M) */

  /** A 32-bit integer representing the minimum @e IssuerEventID of values to be returned by the
   * corresponding @e PublishCalorificValue command.
   */
  zb_uint32_t min_issuer_event_id;  /* (M) */

  /** An 8-bit integer which represents the maximum number of @e PublishConversionFactor commands that
   * the client is willing to receive in response to this command. A value of 0 indicates no maximum
   * limit.
   */
  zb_uint8_t number_of_commands;  /* (M) */
} ZB_PACKED_STRUCT zb_zcl_price_get_conversion_factor_payload_t;


/** @ref ZB_ZCL_PRICE_CLI_CMD_GET_CALORIFIC_VALUE "GetCalorificValue" command payload
  * @see SE spec, subclause D.4.2.3.7
  */
typedef ZB_PACKED_PRE struct zb_zcl_price_client_get_calorific_value_payload_s
{
  /** UTC Timestamp indicating the earliest start time of values to be returned by the
   * corresponding @e PublishCalorificValue command.
   * */
  zb_uint32_t earliest_start_time;  /* (M) */

  /** A 32-bit integer representing the minimum @e IssuerEventID of values to be returned by the
   * corresponding @e PublishCalorificValue command.
   */
  zb_uint32_t min_issuer_event_id;  /* (M) */

  /** An 8-bit Integer which represents the maximum number of @e PublishCalorificValue commands that the
   *  client is willing to receive in response to this command. A value of 0 indicates no maximum limit
   */
  zb_uint8_t number_of_commands;  /* (M) */
} ZB_PACKED_STRUCT zb_zcl_price_client_get_calorific_value_payload_t;


/** @ref ZB_ZCL_PRICE_CLI_CMD_GET_TARIFF_INFORMATION "GetTariffInformation" command payload
 * @see SE spec, subclause D.4.2.3.8.2
 */
typedef ZB_PACKED_PRE struct zb_zcl_price_get_tariff_information_payload_s
{
  /** UTC Timestamp indicating the earliest start time of tariffs to be returned by the
   * corresponding @e PublishTariffInformation command.
   */
  zb_uint32_t earliest_start_time;  /* (M) */

  /** A 32-bit integer representing the minimum @e IssuerEventID of tariffs to be returned by the
   * corresponding @e PublishTariffInformation command.
   */
  zb_uint32_t min_issuer_event_id;  /* (M) */

  /** An 8-bit integer which represents the maximum number of @e PublishTariffInformation commands that
   * the client is willing to receive in response to this command. A value of 0 would indicate all
   *  available @e PublishTariffInformation commands shall be returned.
   */
  zb_uint8_t number_of_commands;    /* (M) */

  /** An 8-bit bitmap identifying the type of tariff published in this command. The least significant
   * nibble represents an enumeration of the tariff type.
   * The most significant nibble is reserved
   * @see SE spec, Table D-108
   * @see zb_zcl_price_tariff_type_t
   */
  zb_uint8_t tariff_type;           /* (M) */
} ZB_PACKED_STRUCT zb_zcl_price_get_tariff_information_payload_t;


/** @ref ZB_ZCL_PRICE_CLI_CMD_GET_PRICE_MATRIX "GetPriceMatrix" command payload
 * @see SE spec, subclause D.4.2.3.9
 */
typedef ZB_PACKED_PRE struct zb_zcl_price_get_price_matrix_payload_s
{
  /** @e IssuerTariffID indicates the tariff to which the requested Price Matrix belongs. */
  zb_uint32_t issuer_tariff_id;
} ZB_PACKED_STRUCT zb_zcl_price_get_price_matrix_payload_t;


/** @ref ZB_ZCL_PRICE_CLI_CMD_GET_BLOCK_THRESHOLDS "GetBlockThresholds" command payload
 * @see SE spec, subclause D.4.2.3.10
 */
typedef ZB_PACKED_PRE struct zb_zcl_price_get_block_thresholds_payload_s
{
  /** @e IssuerTariffID indicates the tariff to which the requested Block
   * Thresholds belong.
   */
  zb_uint32_t issuer_tariff_id;  /* (M) */
} ZB_PACKED_STRUCT zb_zcl_price_get_block_thresholds_payload_t;


/** @ref ZB_ZCL_PRICE_CLI_CMD_GET_CO2_VALUE "GetCO2Value" command payload
 * @see SE spec, subclause D.4.2.3.11.2
 */
typedef ZB_PACKED_PRE struct zb_zcl_price_get_co2_value_payload_s
{
  /** UTC Timestamp indicating the earliest start time of values to be returned by the corresponding
   * @e PublishCO2Value command. The first returned @e PublishCO2Value command shall be the instance
   * which is active or becomes active at or after the stated @e EarliestStartTime. If more than one
   * instance is requested, the active and scheduled instances shall be sent with ascending ordered
   * @e StartTime.
   */
  zb_uint32_t earliest_start_time;  /* (M) */

  /** A 32-bit integer representing the minimum @e IssuerEventID of values to be returned by the
   * corresponding @e PublishCO2Value command.
   */
  zb_uint32_t min_issuer_event_id;  /* (M) */

  /** An 8-bit Integer which represents the maximum number of @e PublishCO2Value commands that the
   * client is willing to receive in response to this command. A value of 0 would indicate all
   * available @e PublishCO2Value commands shall be returned.
   */
  zb_uint8_t number_of_commands;    /* (M) */

  /** An 8-bit bitmap identifying the type of tariff published in this command. The least significant
   * nibble represents an enumeration of the tariff type.
   * @see SE spec, Table D-108
   * @see zb_zcl_price_tariff_type_t
   */
  zb_uint8_t tariff_type;           /* (O) */
} ZB_PACKED_STRUCT zb_zcl_price_get_co2_value_payload_t;


/** @ref ZB_ZCL_PRICE_CLI_CMD_GET_TIER_LABELS "GetTierLabels" command payload
 * @see SE spec, subclause D.4.2.3.12.2
 */
typedef ZB_PACKED_PRE struct zb_zcl_price_get_tier_labels_payload_s
{
  /** Unique identifier generated by the commodity supplier. This is used to
   * identify the tariff that the labels apply to.
   */
  zb_uint32_t issuer_tariff_id;  /* (M) */
} ZB_PACKED_STRUCT zb_zcl_price_get_tier_labels_payload_t;

#define ZB_ZCL_PRICE_GET_TIER_LABELS_PAYLOAD_SIZE_IS_VALID(size) \
  ((size) >= sizeof(zb_zcl_price_get_tier_labels_payload_t))

/** @ref ZB_ZCL_PRICE_CLI_CMD_GET_BILLING_PERIOD "GetBillingPeriod" command payload
 * @see SE spec, subclause D.4.2.3.13.2
 */
typedef ZB_PACKED_PRE struct zb_zcl_price_get_billing_period_payload_s
{
  /** UTC Timestamp indicating the earliest start time of billing periods to be
   *  returned by the corresponding @e PublishBillingPeriod command.
   */
  zb_uint32_t earliest_start_time;  /* (M) */

  /** A 32-bit integer representing the minimum @e IssuerEventID of billing
   * periods to be returned by the corresponding @e PublishBillingPeriod command.
   */
  zb_uint32_t min_issuer_event_id;  /* (M) */

  /** An 8-bit Integer which indicates the maximum number of @e PublishBillingPeriod commands
   * that the client is willing to receive in response to this command.
   */
  zb_uint8_t number_of_commands;    /* (M) */

  /** An 8-bit bitmap identifying the @e TariffType of the requested @e BillingPeriod information.
   * The least significant nibble represents an enumeration of the tariff type.
   * The most significant nibble is reserved.
   * @see SE spec, Table D-108
   * @see zb_zcl_price_tariff_type_t
   */
  zb_uint8_t tariff_type;           /* (O) */
} ZB_PACKED_STRUCT zb_zcl_price_get_billing_period_payload_t;


/** @ref ZB_ZCL_PRICE_CLI_CMD_GET_CONSOLIDATED_BILL "GetConsolidatedBill" command payload
 * @see SE spec, subclause D.4.2.3.14.2
 */
typedef ZB_PACKED_PRE struct zb_zcl_price_get_consolidated_bill_payload_s
{
  /** UTC Timestamp indicating the earliest start time of billing information to be
   *  returned by the corresponding @e PublishConsolidatedBill command.
   */
  zb_uint32_t earliest_start_time;  /* (M) */

  /** A 32-bit integer representing the minimum @e IssuerEventID of billing information
   *  to be returned by the corresponding @e PublishConsolidatedBill command.
   */
  zb_uint32_t min_issuer_event_id;  /* (M) */

  /** An 8-bit Integer which indicates the maximum number of @e PublishConsolidatedBill
   * commands that can be sent. A value of 0 would indicate all available @e PublishConsolidatedBill
   * commands shall be returned.
   */
  zb_uint8_t number_of_commands;    /* (M) */

  /** An 8-bit bitmap identifying the @e TariffType of the requested @e BillingPeriod information.
   * The least significant nibble represents an enumeration of the tariff type.
   * The most significant nibble is reserved.
   * @see SE spec, Table D-108
   * @see zb_zcl_price_tariff_type_t
   */
  zb_uint8_t tariff_type;           /* (O) */
} ZB_PACKED_STRUCT zb_zcl_price_get_consolidated_bill_payload_t;


/** @ref ZB_ZCL_PRICE_CLI_CMD_CPP_EVENT_RESPONSE "CPPEventResponse" command payload
 * @see SE spec, subclause D.4.2.3.15.2
 */
typedef ZB_PACKED_PRE struct zb_zcl_price_cpp_event_response_payload_s
{
  /** Unique identifier generated by the commodity provider. When new information is provided that
   * replaces older information for the same time period, this field allows devices
   * to determine which information is newer.
   */
  zb_uint32_t issuer_event_id;  /* (M) */

  /** An 8-bit enumeration identifying the status of the CPP event. This field shall contain the
   * 'Accepted' or 'Rejected' values
   * @see SE spec, Table D-113
   * @see zb_zcl_price_cpp_event_status_t
   */
  zb_uint8_t cpp_auth;          /* (M) */
} ZB_PACKED_STRUCT zb_zcl_price_cpp_event_response_payload_t;


/** @ref ZB_ZCL_PRICE_CLI_CMD_GET_CREDIT_PAYMENT "GetCreditPayment" command payload
 * @see SE spec, subclause D.4.2.3.16.2
 */
typedef ZB_PACKED_PRE struct zb_zcl_price_get_credit_payment_payload_s
{
  /** UTC timestamp indicating the latest @e CreditPaymentDate of records to be returned
   *  by the corresponding @e PublishCreditPayment commands. The first returned
   * @e PublishCreditPayment command shall be the most recent record with its
   * @e CreditPaymentDate equal to or older than the @e LatestEndTime provided.
   */
  zb_uint32_t latest_end_time;      /* (M) */

  /** An 8-bit integer that represents the maximum number of @e PublishCreditPayment
   * commands that the client is willing to receive in response to this command.
   */
  zb_uint8_t number_of_commands;    /* (M) */
} ZB_PACKED_STRUCT zb_zcl_price_get_credit_payment_payload_t;


/** Block Period Control Field BitMap
 * @see SE spec, Table D-104
 * @see zb_zcl_price_publish_block_period_payload_s::block_period_control
 */
typedef enum zb_zcl_price_block_period_control_field_bitmap_e
{
  ZB_ZCL_PRICE_CONTROL_PRICE_ACKNOWLEDGEMENT_REQUIRED = 1 << 0, /**< 1=Price Acknowledgement required,@n
                                                                 *  0=Price Acknowledgement not required
                                                                 */
  ZB_ZCL_PRICE_CONTROL_REPEATING_BLOCK                = 1 << 1, /**< 1=Repeating Block, @n
                                                                 *  0=Non Repeating Block
                                                                 */
} zb_zcl_price_block_period_control_field_t;


/** @brief Block Period Duration Control Enumeration
 * @see SE spec, Table D-106
 */
typedef enum zb_zcl_price_block_period_duration_control_e
{
  ZB_ZCL_PRICE_DURATION_START_OF_TIMEBASE = 0x00,  /**< Start of Timebase */
  ZB_ZCL_PRICE_DURATION_END_OF_TIMEBASE,           /**< End of Timebase */
  ZB_ZCL_PRICE_DURATION_NOT_SPECIFIED              /**< Not Specified */
} zb_zcl_price_block_period_duration_control_t;


/** @ref ZB_ZCL_PRICE_SRV_CMD_PUBLISH_BLOCK_PERIOD "PublishBlockPeriod" command payload
 * @see SE spec, subclause D.4.2.4.2.1
 */
typedef ZB_PACKED_PRE struct zb_zcl_price_publish_block_period_payload_s
{

  /** An unsigned 32-bit field containing a unique identifier for the commodity provider.
   * This field allows differentiation in deregulated markets where multiple
   * commodity providers may be available.
   */
  zb_uint32_t provider_id;                /* (M) */

  /** Unique identifier generated by the commodity provider. When new block period
   * information is provided that replaces older information for the same period,
   * this field allows devices to determine which information is newer.
   */
  zb_uint32_t issuer_event_id;            /* (M) */

  /** A @e UTCTime field to denote the time at which the block tariff period starts.
   */
  zb_uint32_t block_period_start_time;    /* (M) */

  /** An unsigned 24-bit field to denote the block tariff period .
   */
  zb_uint24_t block_period_duration;      /* (M) */

    /** Identifies additional control options for the block period event. A value of
     * 0x00 indicates field not used.
     */
  zb_uint8_t block_period_control;

  /** An 8-bit bitmap where the least significant nibble is an enumerated sub-field
   * indicating the time base used for the duration, and the most significant nibble
   * is an enumerated sub-field providing duration control.
   * @see zb_zcl_price_block_period_duration_type_e
   */
  zb_uint8_t block_period_duration_type;  /* (M) */

  /** An 8-bit bitmap identifying the type of tariff published in this command.
   * The least significant nibble represents an enumeration of the tariff type.
   * @see SE spec, Table D-108
   * @see zb_zcl_price_tariff_type_t
   */
  zb_uint8_t tariff_type;                 /* (M) */

  /** An 8-bit enumeration identifying the resolution period for the block tariff.
   * @see SE spec, Table D-107
   * @see zb_zcl_price_tariff_resolution_period_t
   */
  zb_uint8_t tariff_resolution_period;    /* (M) */
} ZB_PACKED_STRUCT zb_zcl_price_publish_block_period_payload_t;


/** @ref ZB_ZCL_PRICE_SRV_CMD_PUBLISH_CONVERSION_FACTOR "PublishConversionFactor" command payload
 * @see SE spec, subclause D.4.2.4.3.1
 */
typedef ZB_PACKED_PRE struct zb_zcl_price_publish_conversion_factor_payload_s
{

  /** Unique identifier generated by the commodity provider.
   */
  zb_uint32_t issuer_event_id;                  /* (M) */

  /** A @e UTCTime field to denote the time at which the value becomes valid.
   * The value remains valid until replaced by a newer one.
   */
  zb_uint32_t start_time;                       /* (M) */

  /** @see SE spec, subclause D.4.2.2.4.3 */
  zb_uint32_t conversion_factor;                /* (M) */

  /** @see SE spec, subclause D.4.2.2.4.4 */
  zb_uint8_t conversion_factor_trailing_digit;  /* (M) */
} ZB_PACKED_STRUCT zb_zcl_price_publish_conversion_factor_payload_t;


/** @ref ZB_ZCL_PRICE_SRV_CMD_PUBLISH_CALORIFIC_VALUE "PublishCalorificValue" command payload
 * @see SE spec, subclause D.4.2.4.4.
 */
typedef ZB_PACKED_PRE struct zb_zcl_price_publish_calorific_value_payload_s
{

  /** Unique identifier generated by the commodity provider.
   */
  zb_uint32_t issuer_event_id;                /* (M) */

  /** A @e UTCTime field to denote the time at which the value becomes valid.
   * The value remains valid until replaced by a newer one.
   */
  zb_uint32_t start_time;                     /* (M) */

  /** @see sub-clause D.4.2.2.4.5
   */
  zb_uint32_t calorific_value;                /* (M) */

  /** @see sub-clause D.4.2.2.4.6
   */
  zb_uint8_t calorific_value_unit;            /* (M) */

  /** @see sub-clause D.4.2.2.4.7
   */
  zb_uint8_t calorific_value_trailing_digit;  /* (M) */
} ZB_PACKED_STRUCT zb_zcl_price_publish_calorific_value_payload_t;


/** Tariff Type Enumeration
 * @see SE spec, Table D-108
 */
typedef enum zb_zcl_price_tariff_type_e
{
  ZB_ZCL_PRICE_TARIFF_TYPE_DELIVERED               = 0x00,  /**< Delivered Tariff */
  ZB_ZCL_PRICE_TARIFF_TYPE_RECEIVED,                        /**< Received Tariff */
  ZB_ZCL_PRICE_TARIFF_TYPE_DELIVERED_AND_RECEIVED,          /**< Delivered and Received Tariff */
} zb_zcl_price_tariff_type_t;


/** Tariff Charging Scheme Enumeration
 * @see SE spec, Table D-109
 */
typedef enum zb_zcl_price_tariff_charging_scheme_e
{
  ZB_ZCL_PRICE_TOU_TARIFF = 0x00,            /**< TOU Tariff */
  ZB_ZCL_PRICE_BLOCK_TARIFF,                 /**< Block Tariff */
  ZB_ZCL_PRICE_BLOCK_TOU_TARIFF_COMMON,      /**< Block/TOU Tariff with common thresholds */
  ZB_ZCL_PRICE_BLOCK_TOU_TARIFF_INDIVIDUAL,  /**< Block/TOU Tariff with individual thresholds per tier */
} zb_zcl_price_tariff_charging_scheme_t;


/** @ref ZB_ZCL_PRICE_SRV_CMD_PUBLISH_TARIFF_INFORMATION "PublishTariffInformation" command payload
 * @see SE spec, subclause D.4.2.4.5.2
 */
typedef ZB_PACKED_PRE struct zb_zcl_price_publish_tariff_information_payload_s
{


  /** A unique identifier for the commodity supplier. */
  zb_uint32_t provider_id;                        /* (M) */

  /** Unique identifier generated by the commodity provider. When new information
   * is provided that replaces older information for the same time period, this field
   * allows devices to determine which information is newer.
   */
  zb_uint32_t issuer_event_id;                    /* (M) */

  /** Unique identifier generated by the commodity supplier.
   */
  zb_uint32_t issuer_tariff_id;                   /* (M) */

  /** A UTC Time field to denote the time at which the price signal becomes valid.
   */
  zb_uint32_t start_time;                         /* (M) */

  /** An 8-bit bitmap identifying the type of tariff published in this command.
   * The least significant nibble represents an enumeration of the tariff type,
   * the most significant nibble represents an enumeration specifying the charging
   * scheme.
   * @see SE spec, Table D-108 and Table D-109
   * @see zb_zcl_price_tariff_type_t
   * @see zb_zcl_price_tariff_charging_scheme_e
   */
  zb_uint8_t tariff_type_charging_scheme;         /* (M) */

  /** The format and use of this field is the same as for the @e TariffLabel attribute
   * or @e ReceivedTariffLabel attribute
   */
  zb_uint8_t tariff_label[25];                    /* (M) */

  /** The format and use of this field is the same as for the @e NumberOfPriceTiersInUse
   * attribute or @e ReceivedNumberOfPriceTiersInUse attribute
   */
  zb_uint8_t number_of_price_tiers_in_use;        /* (M) */

  /** The format and use of this field is the same as for the @e NumberOfBlockThresholdsInUse
   * attribute or @e ReceivedNumberOfBlockThresholdsInUse attribute
   */
  zb_uint8_t number_of_block_thresholds_in_use;   /* (M) */

  /** The format and use of this field is the same as for the @e UnitOfMeasure attribute
   */
  zb_uint8_t unit_of_measure;                     /* (M) */

  /** The format and use of this field is the same as for the @e Currencyattribute
   */
  zb_uint16_t currency;                           /* (M) */

  /** The format and use of this field is the same as for the @e PriceTrailingDigit
   * attribute
   */
  zb_uint8_t price_trailing_digit;                /* (M) */

  /** The format and use of this field is the same as for the @e StandingCharge
   * attribute. A value of 0xFFFFFFFF indicates the field is not used
   */
  zb_uint32_t standing_charge;                    /* (M) */

  /** The format and use of this field is the same as for the @e TierBlockMode
   * attribute or @e ReceivedTierBlockMode attribute
   */
  zb_uint8_t tier_block_mode;                     /* (M) */

  /** @e BlockThresholdMultiplier provides a value to be multiplied against Threshold
   * parameter(s). If present, this attribute must be applied to all @e BlockThreshold
   * values to derive values that can be compared against the @e CurrentBlockPeriodConsumptionDelivered
   * attribute within the Metering cluster. This parameter must be used in conjunction
   * with the @e BlockThresholdDivisor parameter(s).
   */
  zb_uint24_t block_threshold_multiplier;         /* (M) */

  /** @e BlockThresholdDivisor provides a value to divide the result of applying the
   * @e ThresholdMultiplier attribute to @e BlockThreshold values to derive values that
   * can be compared against the @e CurrentBlockPeriodConsumptionDelivered attribute
   * within the Metering cluster. This attribute must be used in conjunction with
   * the @e BlockThresholdMultiplier parameter(s). In case no divisor is defined,
   * this field shall be set to 1.
   */
  zb_uint24_t block_threshold_devisor;            /* (M) */
} ZB_PACKED_STRUCT zb_zcl_price_publish_tariff_information_payload_t;


/** PublishPriceMatrix and PublishBlockThresholds Sub-Payload Control Bitmap
 * @see SE spec, Table D-110
 * @see SE spec, Table D-111
 * @see zb_zcl_price_publish_matrix_type_e
 * @see zb_zcl_price_publish_block_thresholds_type_e
 */
enum zb_zcl_price_sub_payload_control_bitmap_e
{
  ZB_ZCL_PRICE_SUB_PAYLOAD_TYPE = 0,  /**< 0-bit of bitfield */
};


/** PublishPriceMatrix Sub-Payload Control values.
 * @see SE spec, Table D-110
 * @see zb_zcl_price_sub_payload_control_bitmap_e
 */
typedef enum zb_zcl_price_publish_matrix_type_e
{
  ZB_ZCL_PRICE_BLOCK_TOU_BASED_INFORMATION = 0x00,  /**< The information stored in the
                                                     * sub payload is Block only or Block/TOU based
                                                     */
  ZB_ZCL_PRICE_TOU_BASED_INFORMATION       = 0x01,  /**< The information stored in the sub
                                                     * payload is TOU based.
                                                     */
} zb_zcl_price_publish_matrix_type_t;


/** PriceMatrix Sub-Payload
 * @see SE spec, subclause D.4.2.4.6.2.1
 */
typedef ZB_PACKED_PRE struct zb_zcl_price_price_matrix_sub_payload_s
{
  /** The @e Tier/Block ID specifies the TOU Tier or the TOU Tier and Block that
   * the subsequent @e Price field in the command applies to
   */
  zb_uint8_t tier_block_id;  /* (M) */

  /** This field holds the price information for the Block/TOU or TOU identified
   * by the previous @e Tier/Block @e ID field.
   */
  zb_uint32_t price;         /* (M) */
} ZB_PACKED_STRUCT zb_zcl_price_price_matrix_sub_payload_t;


/** @ref ZB_ZCL_PRICE_SRV_CMD_PUBLISH_PRICE_MATRIX "PublishPriceMatrix" command payload
 * @see SE spec, subclause D.4.2.4.6.2
 */
typedef ZB_PACKED_PRE struct zb_zcl_price_publish_price_matrix_payload_s
{
  /** A unique identifier for the commodity supplier. */
  zb_uint32_t provider_id;                                              /* (M) */

  /** Unique identifier generated by the commodity provider. When new information
   * is provided that replaces older information for the same time period, this field
   * allows devices to determine which information is newer.
   */
  zb_uint32_t issuer_event_id;                                          /* (M) */

  /** A @e UTCTime field to denote the time at which the price signal becomes valid.
   */
  zb_uint32_t start_time;                                               /* (M) */


  /** Unique identifier generated by the commodity supplier. This must match the
   * @e IssuerTariffID sent in the related @e PublishTariffInformation command.
   */
  zb_uint32_t issuer_tariff_id;                                         /* (M) */

  /** The @e CommandIndex is used to count the payload fragments in the case that
   * an entire payload does not fit into one message. The @e CommandIndex starts
   * at 0 and is incremented for each fragment belonging to the same command.
   */
  zb_uint8_t command_index;                                             /* (M) */

  /** In the case that an entire payload does not fit into one message, the
   * @e TotalNumberOfCommands field indicates the total number of sub-commands
   * in the message.
   */
  zb_uint8_t total_number_of_commands;                                  /* (M) */

  /** An 8-bit bitmap, the least significant bit of which specifies the information
   * type stored in the sub payload. The remaining bits are reserved.
   */
  zb_uint8_t sub_payload_control;                                        /* (M) */

  /** @see zb_zcl_price_price_matrix_sub_payload_s
   */
  zb_zcl_price_price_matrix_sub_payload_t *price_matrix_sub_payload;    /* (M) */
} ZB_PACKED_STRUCT zb_zcl_price_publish_price_matrix_payload_t;


/** PublishBlockThresholds Sub-Payload Control values
 * @see SE spec, Table D-111
 * @see zb_zcl_price_sub_payload_control_bitmap_e
 */
typedef enum zb_zcl_price_publish_block_thresholds_type_e
{
  ZB_ZCL_PRICE_SPECIFIC_TOU_TIER = 0x00,  /**< Block Thresholds supplied
                                                                                              * apply to a specific TOU tier
                                                                                              */
  ZB_ZCL_PRICE_ALL_TOU_TIER      = 0X01,  /**< Block Thresholds supplied apply to
                                                                                              * all TOU tiers or  when Block Only charging
                                                                                              * is in operation
                                                                                              */
} zb_zcl_price_publish_block_thresholds_type_t;


/** BlockThreshold Sub-Payload Format
 * @see SE spec, subclause D.4.2.4.7.2.1
 */
typedef ZB_PACKED_PRE struct zb_zcl_price_block_threshold_sub_payload_s
{
  /** The Tier/NumberOfBlockThresholds field is an 8 bitmap. The format of the
   * bitmap is decided by bit0 of the sub-payload control field.
   * @n If Bit0 of the Sub-Payload Control field is 0, then the least significant
   * nibble represents a value specifying the number of thresholds to follow in
   * the command. The most significant nibble represents the Tier that the subsequent
   * block threshold values apply to.
   * @n If Bit0 of the Sub-Payload Control field is 1, then the most significant
   * nibble is unused and should be set to 0.
   * @n Valid values for the NumberOfBlockThresholds sub-field are 0 to 15
   * reflecting no block in use (0) to block 15(15).
   * Valid values for the Tiers sub-field are from 0 to 15 reflecting no tier to tier
   * fifteen.
   */
  zb_uint8_t tier_number_of_block_thresholds;

  /** The Block Thresholds represent the threshold values applicable to an
   * individual block period and, where applicable, to a particular tier.
   */
  zb_uint48_t block_threshold[15];
} ZB_PACKED_STRUCT zb_zcl_price_block_threshold_sub_payload_t;


/** @ref ZB_ZCL_PRICE_SRV_CMD_PUBLISH_BLOCK_THRESHOLDS "PublishBlockThresholds" command payload
 * @see SE spec, subclause D.4.2.4.7.2
 */
typedef ZB_PACKED_PRE struct zb_zcl_price_publish_block_thresholds_payload_s
{

  /** An unsigned 32-bit field containing a unique identifier for the commodity
   *  provider.
   */
  zb_uint32_t provider_id;                                                  /* (M) */

  /** Unique identifier generated by the commodity provider. When new information
   * is provided that replaces older information for the same time period, this
   * field allows devices to determine which information is newer.
   */
  zb_uint32_t issuer_event_id;                                              /* (M) */

  /** A @e UTCTime field to denote the time at which the price signal becomes valid.
   */
  zb_uint32_t start_time;                                                   /* (M) */

  /** Unique identifier generated by the commodity supplier. This must match the
   * @e IssuerTariffID sent in the related @e PublishTariffInformation command.
   */
  zb_uint32_t issuer_tariff_id;                                             /* (M) */

  /** The @e CommandIndex is used to count the payload fragments in the case where
   * the entire payload does not fit into one message. The @e CommandIndex starts
   * at 0 and is incremented for each fragment belonging to the same command.
   */
  zb_uint8_t command_index;                                                 /* (M) */

  /** In the case where the entire payload does not fit into one message, the
   * @e TotalNumberOfCommands field indicates the total number of sub-commands
   * in the message.
   */
  zb_uint8_t total_number_of_commands;                                      /* (M) */

  /** The Sub-Payload Control bitmap specifies the usage of the information contained
   * within the Block Threshold Sub-Payload.
   * @see zb_zcl_price_publish_block_thresholds_sub_payload_control_bitmap_e
   */
  zb_uint8_t sub_payload_control;                                           /* (M) */

  /** The BlockThreshold Sub-Payload consists of multiple sets of data which
   * consist of a @e TierID, @e BlockThresholdCount and the threshold values
   * associated with the stated Tier.
   * @see zb_zcl_price_block_threshold_sub_payload_t
   */
  zb_zcl_price_block_threshold_sub_payload_t *block_threshold_sub_payload;  /* (M) */
} ZB_PACKED_STRUCT zb_zcl_price_publish_block_thresholds_payload_t;


/** @ref ZB_ZCL_PRICE_SRV_CMD_PUBLISH_CO2_VALUE "PublishCO2Value" command payload
 * @see SE spec, subclause D.4.2.4.8.2
 */
typedef ZB_PACKED_PRE struct zb_zcl_price_publish_co2_value_payload_s
{
  /** An unsigned 32-bit field containing a unique identifier for the commodity
   * provider.
   */
  zb_uint32_t provider_id;              /* (M) */

  /** Unique identifier generated by the commodity provider. When new information
   * is provided that replaces older information for the same time period, this field
   * allows devices to determine which information is newer.
   */
  zb_uint32_t issuer_event_id;          /* (M) */

  /** A @e UTCTime field to denote the time at which the @e CO2 value becomes valid.
   */
  zb_uint32_t start_time;               /* (M) */

  /** An 8-bit bitmap identifying the type of tariff published in this command.
   * The least significant nibble represents an enumeration of the tariff type.
   * The most significant nibble is reserved.
   * @see zb_zcl_price_tariff_type_t
   */
  zb_uint8_t tariff_type;               /* (M) */

  /** The format and use of this field is the same as for the @e CO2 attribute or
   * @e ReceivedCO2 attribute . A value of 0xFFFFFFFF indicates field not used.
   */
  zb_uint32_t co2_value;                /* (M) */

  /** The format and use of this field is the same as for the @e CO2Unit attribute
   * or @e ReceivedCO2Unit attribute. A value of 0xFF indicates field not used.
   */
  zb_uint8_t co2_value_unit;            /* (M) */

  /** The format and use of this field is the same as for the @e CO2TrailingDigit
   * attribute or @e ReceivedCO2TrailingDigit attribute.
   * A value of 0xFF indicates field not used.
   */
  zb_uint8_t co2_value_trailing_digit;  /* (M) */
} ZB_PACKED_STRUCT zb_zcl_price_publish_co2_value_payload_t;


/** @see SE spec, subclause D.4.2.4.9.2
 * @see zb_zcl_price_publish_tier_labels_payload_t::tier_labels
 */
typedef ZB_PACKED_PRE struct zb_zcl_price_publish_tier_labels_sub_payload_s
{
  /** The tier number that the associated @e TierLabel applies to. */
  zb_uint8_t tier_id;         /* (M) */

  /** Octet String field capable of storing a 12 character string (the first
    * character indicates the string length, represented in hexadecimal format)
    * encoded in the UTF-8 format.
    */
  zb_uint8_t tier_label[13];  /* (M) */
} ZB_PACKED_STRUCT zb_zcl_price_publish_tier_labels_sub_payload_t;


/** @ref ZB_ZCL_PRICE_SRV_CMD_PUBLISH_TIER_LABELS "PublishTierLabels" command payload
 * @see SE spec, subclause D.4.2.4.9.2
 */
typedef ZB_PACKED_PRE struct zb_zcl_price_publish_tier_labels_payload_s
{
  /** An unsigned 32-bit field containing a unique identifier for the commodity
   * provider.
   */
  zb_uint32_t provider_id;              /* (M) */

  /** Unique identifier generated by the commodity provider. When new information
   * is provided that replaces older information for the same time period, this field
   * allows devices to determine which information is newer.
   */
  zb_uint32_t issuer_event_id;          /* (M) */

  /** Unique identifier generated by the commodity supplier. This is used to
   * identify the tariff that the labels apply to.
   */
  zb_uint32_t issuer_tariff_id;         /* (M) */

  /** The @e CommandIndex is used to count the payload fragments in the case where
   * the entire payload does not fit into one message. The @e CommandIndex starts
   * at 0 and is incremented for each fragment belonging to the same command.
   */
  zb_uint8_t command_index;             /* (M) */

  /** In the case where the entire payload does not fit into one message, the
   * @e TotalNumberOfCommands field indicates the total number of sub-commands
   * in the message.
   */
  zb_uint8_t total_number_of_commands;  /* (M) */

  /** The number of @e TierID/Tier Label sets contained within the command.
   */
  zb_uint8_t number_of_labels;          /* (M) */

  /** @see zb_zcl_price_publish_tier_labels_sub_payload_t */
  zb_zcl_price_publish_tier_labels_sub_payload_t *tier_labels;

} ZB_PACKED_STRUCT zb_zcl_price_publish_tier_labels_payload_t;


/** Check if @ref ZB_ZCL_PRICE_SRV_CMD_PUBLISH_TIER_LABELS "PublishTierLabels" command payload size is valid
 * @param size - size of received data payload
 */
#define ZB_ZCL_PRICE_PUBLISH_TIER_LABELS_PAYLOAD_SIZE_IS_VALID(size) \
  ((size >= (sizeof(zb_zcl_price_publish_tier_labels_payload_t)-sizeof(void *)) ? ZB_TRUE : ZB_FALSE))



/** @ref ZB_ZCL_PRICE_SRV_CMD_PUBLISH_BILLING_PERIOD "PublishBillingPeriod" command payload
 * @see SE spec, D.4.2.4.10
 */
typedef ZB_PACKED_PRE struct zb_zcl_price_publish_billing_period_payload_s
{

  /** An unsigned 32-bit field containing a unique identifier for the commodity
   * provider.
   */
  zb_uint32_t provider_id;                  /* (M) */

  /** Unique identifier generated by the commodity provider. When new information is
   * provided that replaces older information for the same time period, this field
   * allows devices to determine which information is newer.
   */
  zb_uint32_t issuer_event_id;              /* (M) */

  /** A @e UTCTime field to denote the time at which the billing period starts.
   */
  zb_uint32_t billing_period_start_time;    /* (M) */

  /** An unsigned 24-bit field to denote the billing period duration. The
   * duration units are defined by the @e BillingPeriodDurationType field.
   */
  zb_uint24_t billing_period_duration;      /* (M) */

  /** An 8-bit bitmap where the least significant nibble is an enumerated
   * sub-field indicating the time base used for the duration, and the most
   * significant nibble is an enumerated sub-field providing duration control.
   * @see zb_zcl_price_block_period_duration_type_t
   * @see zb_zcl_price_block_period_duration_control_t
   */
  zb_uint8_t billing_period_duration_type;  /* (M) */

  /** An 8-bit bitmap identifying the type of tariff published in this command.
   * The least significant nibble represents an enumeration of the tariff type.
   * The most significant nibble is reserved.
   * @see zb_zcl_price_block_period_duration_control_t
   */
  zb_uint8_t tariff_type;                   /* (M) */
} ZB_PACKED_STRUCT zb_zcl_price_publish_billing_period_payload_t;


/** @ref ZB_ZCL_PRICE_SRV_CMD_PUBLISH_CONSOLIDATED_BILL "PublishConsolidatedBill" command payload
 * @see SE spec, subclause D.4.2.4.11.2
 */
typedef ZB_PACKED_PRE struct zb_zcl_price_publish_consolidated_bill_payload_s
{
  /** An unsigned 32-bit field containing a unique identifier for the commodity
   * provider.
   */
  zb_uint32_t provider_id;                  /* (M) */

  /** Unique identifier generated by the commodity provider. When new information
   * is provided that replaces older information for the same time period, this field
   * allows devices to determine which information is newer.
   */
  zb_uint32_t issuer_event_id;              /* (M) */

  /** A @e UTCTime field containing the start time of the related billing period.
   */
  zb_uint32_t billing_period_start_time;    /* (M) */

  /** An unsigned 24-bit field denoting the duration of the related billing period.
   */
  zb_uint24_t billing_period_duration;      /* (M) */

  /** An 8-bit bitmap where the least significant nibble is an enumerated sub-field
   * indicating the time base used for the duration, and the most significant nibble
   * is an enumerated sub-field providing duration control.
   * @see SE spec, Table D-105, Table D-106
   */
  zb_uint8_t billing_period_duration_type;  /* (M) */

  /** An 8-bit bitmap identifying the type of tariff published in this command.
   * The least significant nibble represents an enumeration of the tariff type.
   * The most significant nibble is reserved.
   * @see zb_zcl_price_tariff_type_t
   */
  zb_uint8_t tariff_type;                   /* (M) */

  /** An unsigned 32-bit field containing the consolidated bill value for the
   * stated billing period.
   */
  zb_uint32_t consolidated_bill;            /* (M) */

  /** An unsigned 16-bit field containing identifying information concerning the
   * local unit of currency used in the @e ConsolidatedBill field.
   */
  zb_uint16_t currency;                     /* (M) */

  /** An 8-bit field used to determine where the decimal point is located in the
   * @e ConsolidatedBill field.
   */
  zb_uint8_t bill_trailing_digit;           /* (M) */
} ZB_PACKED_STRUCT zb_zcl_price_publish_consolidated_bill_payload_t;


/** CPP Auth Enumeration
 * @see SE spec, Table D-112
 */
typedef enum zb_zcl_price_cpp_price_tier_e
{
  ZB_ZCL_PRICE_CPP_PRICE_TIER_CPP1 = 0,  /**< CPP1 */
  ZB_ZCL_PRICE_CPP_PRICE_TIER_CPP2 = 1,  /**< CPP2 */
} zb_zcl_price_cpp_price_tier_t;


/** CPP Auth Enumeration
 * @see SE spec, Table D-113
 */
typedef enum zb_zcl_price_cpp_event_status_e
{
  ZB_ZCL_PRICE_CPP_EVENT_STATUS_PENDING   = 0,  /**< Pending */
  ZB_ZCL_PRICE_CPP_EVENT_STATUS_ACCEPTED,       /**< Accepted */
  ZB_ZCL_PRICE_CPP_EVENT_STATUS_REJECTED,       /**< Rejected */
  ZB_ZCL_PRICE_CPP_EVENT_STATUS_FORCED,         /**< Forced  */
} zb_zcl_price_cpp_event_status_t;


/** @ref ZB_ZCL_PRICE_SRV_CMD_PUBLISH_CPP_EVENT "PublishCPPEvent" command payload
 * @see SE spec, subclause D.4.2.4.12.2
 */
typedef ZB_PACKED_PRE struct zb_zcl_price_publish_cpp_event_payload_s
{
  /** An unsigned 32-bit field containing a unique identifier for the commodity
   * provider.
   */
  zb_uint32_t provider_id;          /* (M) */

  /** Unique identifier generated by the commodity provider. When new information
   * is provided that replaces older information for the same time period, this field
   * allows devices to determine which information is newer.
   */
  zb_uint32_t issuer_event_id;      /* (M) */

  /** A @e UTCTime field to denote the time at which the CPP event begins. A start
   * date/time of 0x00000000 shall indicate that the command should be executed
   * immediately.
   */
  zb_uint32_t start_time;           /* (M) */


  /** Defines the duration of the CPP event.
   */
  zb_uint16_t duration_in_minutes;  /* (M) */

  /** An 8-bit bitmap identifying the type of tariff published in this command.
   * The least significant nibble represents an enumeration of the tariff type.
   * The most significant nibble is reserved.
   * @see zb_zcl_price_tariff_type_t
   */
  zb_uint8_t tariff_type;           /* (M) */

  /** An 8-bit enumeration identifying the price tier associated with this CPP event.
   * The price(s) contained in the active price matrix for that price tier will override
   * the normal pricing scheme.
   * @see zb_zcl_price_cpp_price_tier_t
   */
  zb_uint8_t cpp_price_tier;        /* (M) */

  /** An 8-bit enumeration identifying the status of the CPP event.
   * @see zb_zcl_price_cpp_event_status_t
   */
  zb_uint8_t cpp_auth;              /* (M) */
} ZB_PACKED_STRUCT zb_zcl_price_publish_cpp_event_payload_t;


/** @ref ZB_ZCL_PRICE_SRV_CMD_PUBLISH_CREDIT_PAYMENT "PublishCreditPayment" command payload.
 * @see SE spec, subclause D.4.2.4.13.2
 */
typedef ZB_PACKED_PRE struct zb_zcl_price_publish_credit_payment_payload_s
{

  /** An unsigned 32-bit field containing a unique identifier for the commodity
   *  provider.
   */
  zb_uint32_t provider_id;                    /* (M) */

  /** Unique identifier generated by the commodity provider. When new information
   * is provided that replaces older information for the same time period, this field
   * allows devices to determine which information is newer.
   */
  zb_uint32_t issuer_event_id;                /* (M) */

  /** A @e UTCTime field containing the time that the next credit payment is due. */
  zb_uint32_t credit_payment_due_date;        /* (M) */

  /** An unsigned 32-bit field denoting the current amount this is overdue from
   * the consumer. This field should be provided in the same currency as used in
   * the Price cluster.
   */
  zb_uint32_t credit_payment_overdue_amount;  /* (M) */

  /** An 8-bit enumeration identifying the current credit payment status.
   */
  zb_uint8_t credit_payment_status;           /* (M) */

  /** An unsigned 32-bit field denoting the last credit payment. This field should
   * be provided in the same currency as used in the Price cluster.
   */
  zb_uint32_t credit_payment;                 /* (M) */

  /** A @e UTCTime field containing the time at which the last credit payment
   * was made.
   */
  zb_uint32_t credit_payment_date;            /* (M) */

  /** An string of between 0-20 octets used to denote the last credit payment
   * reference used by the energy supplier.
   */
  zb_uint8_t credit_payment_ref[21];           /* (M) */
} ZB_PACKED_STRUCT zb_zcl_price_publish_credit_payment_payload_t;


/** Currency Change Control Enumeration
 * @see SE spec, Table D-114
 */
typedef enum zb_zcl_price_currency_change_control_e
{
  ZB_ZCL_PRICE_CLEAR_BILLING_INFORMATION    = 1 << 0,  /**<   1 = Clear Billing Information
                                                        * @n 0 = Do Not Clear Billing Information
                                                        */
  ZB_ZCL_PRICE_CONVERT_BILLING_INFORMATION  = 1 << 1,  /**<   1 = Convert Billing Information using the New Currency
                                                        * @n 0 = Do Not Convert Billing Information
                                                        */
  ZB_ZCL_PRICE_CLEAR_OLD_CONSUMPTION_DATA   = 1 << 2,  /**<   1 = Clear Old Consumption Data
                                                        * @n  0 = Do Not Clear Old Consumption Data
                                                        */
  ZB_ZCL_PRICE_CONVERT_OLD_CONSUMPTION_DATA = 1 << 3,  /**<  1 = Convert Old Consumption Data using the New Currency
                                                        * @n 0 = Do Not Convert Old Consumption Data
                                                        */
} zb_zcl_price_currency_cghange_control_t;


/** @ref ZB_ZCL_PRICE_SRV_CMD_PUBLISH_CURRENCY_CONVERSION "PublishCurrencyConversion" command payload.
 * @see SE spec, subclause D.4.2.4.14.2
 */
typedef ZB_PACKED_PRE struct zb_zcl_price_publish_currency_conversion_payload_s
{

  /** An unsigned 32-bit field containing a unique identifier for the commodity
   * provider.
   */
  zb_uint32_t provider_id;                      /* (M) */

  /** Unique identifier generated by the commodity provider. When new information
   *  is provided that replaces older information for the same time period, this
   * field allows devices to determine which information is newer.
   */
  zb_uint32_t issuer_event_id;                  /* (M) */

  /** A @e UTCTime field to denote the time at which the new currency becomes valid.
   */
  zb_uint32_t start_time;                       /* (M) */

  /** An unsigned 16-bit field containing identifying information concerning the
   *  old local unit of currency used in the Price cluster.
   */
  zb_uint16_t old_currency;                     /* (M) */

  /** An unsigned 16-bit field containing identifying information concerning the
   *  new local unit of currency used in the Price cluster. The value of the @e NewCurrency
   *  field should match the values defined by ISO 4217.
   */
  zb_uint16_t new_currency;                     /* (M) */

  /** The format and use of this field is the same as for the @e ConversionFactor
   * attribute as defined in D.4.2.2.4.3.
   */
  zb_uint32_t conversion_factor;                /* (M) */

  /** The format and use of this field is the same as for the
   * @e ConversionFactorTrailingDigit attribute as defined in D.4.2.2.4.4.
   */
  zb_uint8_t conversion_factor_trailing_digit;  /* (M) */

  /** A 32-bit mask that denotes the functions that are required to be carried
   * out on processing of this command.
   */
  zb_uint32_t currency_change_control_flags;     /* (M) */
} ZB_PACKED_STRUCT zb_zcl_price_publish_currency_conversion_payload_t;


/** @ref ZB_ZCL_PRICE_SRV_CMD_CANCEL_TARIFF "CancelTariff" command payload
 * @see SE spec, subclause D.4.2.4.15.2
 */
typedef ZB_PACKED_PRE struct zb_zcl_price_cancel_tariff_payload_s
{

  /** An unsigned 32-bit field containing a unique identifier for the commodity
   * provider.
   */
  zb_uint32_t provider_id;       /* (M) */

  /** Unique identifier generated by the commodity Supplier. All parts of a tariff
   * instance shall have the same @e IssuerTariffID.
   */
  zb_uint32_t issuer_tariff_id;  /* (M) */

  /** An 8-bit bitmap identifying the type of tariff to be cancelled by this command.
   * The least significant nibble represents an enumeration of the tariff type.
   * The most significant nibble is reserved.
   * @see SE spec, Table D-108
   * @see zb_zcl_price_tariff_type_t
   */
  zb_uint8_t tariff_type;        /* (M) */
} ZB_PACKED_STRUCT zb_zcl_price_cancel_tariff_payload_t;


/** @cond internals_doc */
/** @def ZB_ZCL_PRICE_GET_REQUESTOR_RX_ON_WHEN_IDLE
 *  @brief Macro for getting Requestor RX on When Idle state.
 *  @param x - zb_zcl_price_get_current_price_payload_t value.
 */
#define ZB_ZCL_PRICE_GET_REQUESTOR_RX_ON_WHEN_IDLE(x)  ZB_CHECK_BIT_IN_BIT_VECTOR(x, 0)

/** @def ZB_ZCL_PRICE_SET_REQUESTOR_RX_ON_WHEN_IDLE
 *  @brief Macro for setting Requestor RX on When Idle state.
 *  @param x - zb_zcl_price_get_current_price_payload_t value.
 */
#define ZB_ZCL_PRICE_SET_REQUESTOR_RX_ON_WHEN_IDLE(x)  ZB_SET_BIT_IN_BIT_VECTOR(x, 0)
/** @endcond */ /* internals_doc */

/** @def ZB_ZCL_PRICE_SEND_CMD_GET_CURRENT_PRICE
 *  @brief Macro for sending @ref ZB_ZCL_PRICE_CLI_CMD_GET_CURRENT_PRICE "GetCurrentPrice" command.
 *  @param _param - Reference to buffer.
 *  @param _dst_addr - Address of the device to send command to.
 *  @param _dst_addr_mode - Address mode for _dst_addr.
 *  @param _dst_ep - Destination endpoint.
 *  @param _src_ep - Current endpoint.
 *  @param _rx_on_when_idle - Helps to determine publishing
 *  policy for the server, (see SE spec, subclause D.4.2.3.2.1.1)
 */
#define ZB_ZCL_PRICE_SEND_CMD_GET_CURRENT_PRICE( \
  _param, _dst_addr, _dst_addr_mode, _dst_ep, _src_ep, _rx_on_when_idle) \
  zb_zcl_price_send_cmd_get_current_price( \
    _param, _dst_addr, _dst_addr_mode, _dst_ep, _src_ep, _rx_on_when_idle, NULL)

/** @def ZB_ZCL_PRICE_SEND_CMD_GET_SCHEDULED_PRICES
 *  @brief Macro for sending @ref ZB_ZCL_PRICE_CLI_CMD_GET_SCHEDULED_PRICES "GetScheduledPrices" command.
 *  @param _param - Reference to buffer.
 *  @param _dst_addr - Address of the device to send command to.
 *  @param _dst_addr_mode - Address mode for _dst_addr.
 *  @param _dst_ep - Destination endpoint.
 *  @param _src_ep - Current endpoint.
 *  @param _payload - Packet payload (@ref
 *  zb_zcl_price_get_scheduled_prices_payload_t).
 */
#define ZB_ZCL_PRICE_SEND_CMD_GET_SCHEDULED_PRICES( \
  _param, _dst_addr, _dst_addr_mode, _dst_ep, _src_ep, _payload) \
  zb_zcl_price_send_cmd_get_scheduled_prices( \
    _param, _dst_addr, _dst_addr_mode, _dst_ep, _src_ep, _payload, NULL)

/** @def ZB_ZCL_PRICE_SEND_CMD_PUBLISH_PRICE
 *  @brief Macro for sending @ref ZB_ZCL_PRICE_SEND_CMD_PUBLISH_PRICE "PublishPrice" command.
 *  @param _param - Reference to buffer.
 *  @param _dst_addr - Address of the device to send command to.
 *  @param _dst_addr_mode - Address mode for _dst_addr.
 *  @param _dst_ep - Destination endpoint.
 *  @param _src_ep - Current endpoint.
 *  @param _payload - Packet payload (@ref zb_zcl_price_publish_price_payload_t).
 */
#define ZB_ZCL_PRICE_SEND_CMD_PUBLISH_PRICE( \
  _param, _dst_addr, _dst_addr_mode, _dst_ep, _src_ep, _payload) \
  zb_zcl_price_send_cmd_publish_price( \
    _param, _dst_addr, _dst_addr_mode, _dst_ep, _src_ep, _payload, NULL)


/** @def ZB_ZCL_PRICE_SEND_CMD_GET_TIER_LABELS
 *  @brief Macro for sending @ref ZB_ZCL_PRICE_CLI_CMD_GET_TIER_LABELS "GetTierLabels" command.
 *  @param _param - Reference to buffer.
 *  @param _dst_addr - Address of the device to send command to.
 *  @param _dst_addr_mode - Address mode for _dst_addr.
 *  @param _dst_ep - Destination endpoint.
 *  @param _src_ep - Current endpoint.
 *  @param _payload - Packet payload (ref to @ref zb_zcl_price_get_tier_labels_payload_t).
 */
#define ZB_ZCL_PRICE_SEND_CMD_GET_TIER_LABELS(_param, _dst_addr, _dst_addr_mode, \
                                              _dst_ep, _src_ep, _payload)        \
  zb_zcl_price_send_cmd_get_tier_labels(_param, _dst_addr, _dst_addr_mode,       \
                                        _dst_ep, _src_ep, _payload, NULL)


/** @def ZB_ZCL_PRICE_SEND_CMD_PUBLISH_TIER_LABELS
 *  @brief Macro for sending @ref ZB_ZCL_PRICE_SRV_CMD_PUBLISH_TIER_LABELS "PublishTierLabels" command.
 *  @param _param - Reference to buffer.
 *  @param _dst_addr - Address of the device to send command to.
 *  @param _dst_addr_mode - Address mode for _dst_addr.
 *  @param _dst_ep - Destination endpoint.
 *  @param _src_ep - Current endpoint.
 *  @param _payload - Packet payload (ref to @ref zb_zcl_price_publish_tier_labels_payload_t).
 */
#define ZB_ZCL_PRICE_SEND_CMD_PUBLISH_TIER_LABELS(_param, _dst_addr, _dst_addr_mode, \
                                                  _dst_ep, _src_ep, _payload)        \
  zb_zcl_price_send_cmd_publish_tier_labels(_param, _dst_addr, _dst_addr_mode,       \
                                            _dst_ep, _src_ep, _payload, NULL)


/** @cond internals_doc */
/** @def ZB_ZCL_PRICE_SEND_CMD_PRICE_ACK
 *  @brief Macro for sending @ref ZB_ZCL_PRICE_CLI_CMD_PRICE_ACK "PriceAcknowledgement" command.
 *  @param _param - Reference to buffer.
 *  @param _dst_addr - Address of the device to send command to.
 *  @param _dst_addr_mode - Address mode for _dst_addr.
 *  @param _dst_ep - Destination endpoint.
 *  @param _src_ep - Current endpoint.
 *  @param _payload - Packet payload (@ref zb_zcl_price_ack_payload_t).
 */
#define ZB_ZCL_PRICE_SEND_CMD_PRICE_ACK( \
  _param, _dst_addr, _dst_addr_mode, _dst_ep, _src_ep, _payload) \
  zb_zcl_price_send_cmd_price_ack( \
    _param, _dst_addr, _dst_addr_mode, _dst_ep, _src_ep, _payload, NULL)

/** @cond internals_doc */
/** @def ZB_ZCL_PRICE_SEND_CMD_PRICE_ACK_TSN
 *  @brief Macro for sending @ref ZB_ZCL_PRICE_SEND_CMD_PRICE_ACK_TSN "PriceAcknowledgement" command.
 *  @param _param - Reference to buffer.
 *  @param _dst_addr - Address of the device to send command to.
 *  @param _dst_addr_mode - Address mode for _dst_addr.
 *  @param _dst_ep - Destination endpoint.
 *  @param _src_ep - Current endpoint.
 *  @param _payload - Packet payload (@ref zb_zcl_price_ack_payload_t).
 *  @param _tsn - sequence number of source command
 */
#define ZB_ZCL_PRICE_SEND_CMD_PRICE_ACK_TSN( \
  _param, _dst_addr, _dst_addr_mode, _dst_ep, _src_ep, _payload, _tsn) \
  zb_zcl_price_send_cmd_price_ack_tsn( \
    _param, _dst_addr, _dst_addr_mode, _dst_ep, _src_ep, _payload, _tsn, NULL)

void zb_zcl_price_send_cmd_publish_price(zb_uint8_t param, const zb_addr_u *dst_addr,
                                              zb_aps_addr_mode_t dst_addr_mode, zb_uint8_t dst_ep,
                                              zb_uint8_t src_ep,
                                              const zb_zcl_price_publish_price_payload_t *payload,
                                              zb_callback_t cb);

void zb_zcl_price_send_cmd_get_current_price(zb_uint8_t param, const zb_addr_u *dst_addr,
                                                  zb_aps_addr_mode_t dst_addr_mode, zb_uint8_t dst_ep,
                                                  zb_uint8_t src_ep, zb_uint8_t rx_on_when_idle,
                                                  zb_callback_t cb);

void zb_zcl_price_send_cmd_price_ack(zb_uint8_t param, const zb_addr_u *dst_addr,
                                          zb_aps_addr_mode_t dst_addr_mode,
                                          zb_uint8_t dst_ep, zb_uint8_t src_ep,
                                          const zb_zcl_price_ack_payload_t *payload,
                                          zb_callback_t cb);

/** Function for sending @ref ZB_ZCL_PRICE_CLI_CMD_PRICE_ACK_TSN "PriceAcknowledgement" command.
 *  @param _param - Reference to buffer.
 *  @param _dst_addr - Address of the device to send command to.
 *  @param _dst_addr_mode - Address mode for _dst_addr.
 *  @param _dst_ep - Destination endpoint.
 *  @param _src_ep - Current endpoint.
 *  @param _payload - Packet payload (@ref zb_zcl_price_ack_payload_t).
 *  @param _tsn - sequence number of source command
 *  @param cb - Callback which should be called when the ZCL stack receives APS ack.
 */
void zb_zcl_price_send_cmd_price_ack_tsn(zb_uint8_t param, const zb_addr_u *dst_addr,
                                          zb_aps_addr_mode_t dst_addr_mode,
                                          zb_uint8_t dst_ep, zb_uint8_t src_ep,
                                          const zb_zcl_price_ack_payload_t *payload,
                                          zb_uint8_t tsn,
                                          zb_callback_t cb);

void zb_zcl_price_send_cmd_get_scheduled_prices(zb_uint8_t param, const zb_addr_u *dst_addr,
                                                     zb_aps_addr_mode_t dst_addr_mode, zb_uint8_t dst_ep,
                                                     zb_uint8_t src_ep,
                                                     const zb_zcl_price_get_scheduled_prices_payload_t *payload,
                                                     zb_callback_t cb);


void zb_zcl_price_send_cmd_get_tier_labels(zb_uint8_t param, zb_addr_u *dst_addr,
                                                zb_aps_addr_mode_t dst_addr_mode, zb_uint8_t dst_ep,
                                                zb_uint8_t src_ep,
                                                zb_zcl_price_get_tier_labels_payload_t *payload,
                                                zb_callback_t cb);


void zb_zcl_price_send_cmd_publish_tier_labels(zb_uint8_t param, zb_addr_u *dst_addr,
                                                    zb_aps_addr_mode_t dst_addr_mode, zb_uint8_t dst_ep,
                                                    zb_uint8_t src_ep,
                                                    zb_zcl_price_publish_tier_labels_payload_t *payload,
                                                    zb_callback_t cb);

/** @endcond */ /* internals_doc */


/** @} */ /* end of ZB_ZCL_PRICE_CLUSTER_COMMANDS */

/** @cond internals_doc */
/*** Internal handler for Price Cluster commands */

void zb_zcl_price_init_server(void);
void zb_zcl_price_init_client(void);
#define ZB_ZCL_CLUSTER_ID_PRICE_SERVER_ROLE_INIT zb_zcl_price_init_server
#define ZB_ZCL_CLUSTER_ID_PRICE_CLIENT_ROLE_INIT zb_zcl_price_init_client

/** @endcond */ /* internals_doc */

/** @} */ /* Price cluster */
/** @endcond */ /* DOXYGEN_ZCL_SECTION */

#endif /* ZB_ZCL_PRICE_H */
