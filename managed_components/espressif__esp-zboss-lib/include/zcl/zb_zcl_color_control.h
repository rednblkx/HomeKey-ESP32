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
/* PURPOSE: Color control cluster definitions
*/

#ifndef ZB_ZCL_COLOR_CONTROL_H
#define ZB_ZCL_COLOR_CONTROL_H 1

#include "zcl/zb_zcl_common.h"
#include "zcl/zb_zcl_commands.h"

/** @cond DOXYGEN_ZCL_SECTION */

/** @addtogroup ZB_ZCL_COLOR_CONTROL
    @{
*/

/* ZB_ZCL_CLUSTER_ID_COLOR_CONTROL = 0x0300 defined in zb_zcl_common.h */

/*! @name Color Control cluster attributes
    @{
*/

/** @brief Color Control cluster information attribute identifiers
    @see ZCL spec, subclause 5.2.2.2.3
*/

/**
 * @brief Color control attribute list
 */
enum zb_zcl_color_control_attr_e
{
  /** @internal Current_HUE attribute */
  ZB_ZCL_ATTR_COLOR_CONTROL_CURRENT_HUE_ID                           = 0x0000,
  /** @internal Current Saturation attribute */
  ZB_ZCL_ATTR_COLOR_CONTROL_CURRENT_SATURATION_ID                    = 0x0001,
  /** @internal Remaining Time attribute */
  ZB_ZCL_ATTR_COLOR_CONTROL_REMAINING_TIME_ID                        = 0x0002,
  /** @internal CurrentX attribute */
  ZB_ZCL_ATTR_COLOR_CONTROL_CURRENT_X_ID                             = 0x0003,
  /** @internal CurrentY attribute */
  ZB_ZCL_ATTR_COLOR_CONTROL_CURRENT_Y_ID                             = 0x0004,
  /** The DriftCompensation attribute indicates what mechanism, if any, is in use for
   *  compensation for color/intensity drift over time. */
  ZB_ZCL_ATTR_COLOR_CONTROL_DRIFT_COMPENSATION_ID                    = 0x0005,
  /** The CompensationText attribute holds a textual indication of what mechanism,
   *  if any, is in use to compensate for color/intensity drift over time. */
  ZB_ZCL_ATTR_COLOR_CONTROL_COMPENSATION_TEXT_ID                     = 0x0006,
  /** @internal Color Temperature Mireds attribute */
  ZB_ZCL_ATTR_COLOR_CONTROL_COLOR_TEMPERATURE_ID                     = 0x0007,
  /** @internal Color Mode attribute */
  ZB_ZCL_ATTR_COLOR_CONTROL_COLOR_MODE_ID                            = 0x0008,
  /** The Options attribute is a bitmap that determines the default behavior of some
   *  cluster commands. */
  ZB_ZCL_ATTR_COLOR_CONTROL_OPTIONS_ID                               = 0x000f,
  /** The EnhancedCurrentHue attribute represents non-equidistant steps along the
   *  CIE 1931 color triangle, and it provides 16-bits precision. */
  ZB_ZCL_ATTR_COLOR_CONTROL_ENHANCED_CURRENT_HUE_ID                  = 0x4000,
  /** The EnhancedColorMode attribute specifies which attributes are currently
   *  determining the color of the device. */
  ZB_ZCL_ATTR_COLOR_CONTROL_ENHANCED_COLOR_MODE_ID                   = 0x4001,
  /** The ColorLoopActive attribute specifies the current active status of the color
   *  loop. */
  ZB_ZCL_ATTR_COLOR_CONTROL_COLOR_LOOP_ACTIVE_ID                     = 0x4002,
  /** The ColorLoopDirection attribute specifies the current direction of the color
   *  loop. */
  ZB_ZCL_ATTR_COLOR_CONTROL_COLOR_LOOP_DIRECTION_ID                  = 0x4003,
  /** The ColorLoopTime attribute specifies the number of seconds it SHALL take to
   *  perform a full color loop, i.e., to cycle all values of the EnhancedCurrentHue
   *  attribute (between 0x0000 and 0xffff). */
  ZB_ZCL_ATTR_COLOR_CONTROL_COLOR_LOOP_TIME_ID                       = 0x4004,
  /** The ColorLoopStartEnhancedHue attribute specifies the value of the
   *  EnhancedCurrentHue attribute from which the color loop SHALL be started. */
  ZB_ZCL_ATTR_COLOR_CONTROL_COLOR_LOOP_START_ENHANCED_HUE_ID         = 0x4005,
  /** The ColorLoopStoredEnhancedHue attribute specifies the value of the
   *  EnhancedCurrentHue attribute before the color loop was started. */
  ZB_ZCL_ATTR_COLOR_CONTROL_COLOR_LOOP_STORED_ENHANCED_HUE_ID        = 0x4006,
  /** The ColorCapabilities attribute specifies the color capabilities of the device
   *  supporting the color control cluster. */
  ZB_ZCL_ATTR_COLOR_CONTROL_COLOR_CAPABILITIES_ID                    = 0x400a,
  /** The ColorTempPhysicalMinMireds attribute indicates the minimum mired value
   *  supported by the hardware. */
  ZB_ZCL_ATTR_COLOR_CONTROL_COLOR_TEMP_PHYSICAL_MIN_MIREDS_ID        = 0x400b,
  /** The ColorTempPhysicalMaxMireds attribute indicates the maximum mired value
   *  supported by the hardware. */
  ZB_ZCL_ATTR_COLOR_CONTROL_COLOR_TEMP_PHYSICAL_MAX_MIREDS_ID        = 0x400c,
  /** The CoupleColorTempToLevelMinMireds attribute specifies a lower bound on the
   *  value of the ColorTemperatureMireds attribute for the purposes of coupling
   *  the ColorTemperatureMireds attribute to the CurrentLevel attribute when the
   *  CoupleColorTempToLevel bit of the Options attribute of the Level Control cluster
   *  is equal to 1. */
  ZB_ZCL_ATTR_COLOR_CONTROL_COUPLE_COLOR_TEMP_TO_LEVEL_MIN_MIREDS_ID = 0x400d,
  /** The StartUpColorTemperatureMireds attribute SHALL define the desired startup color
   *  temperature value a lamp SHALL use when it is supplied with power and this value
   *  SHALL be reflected in the ColorTemperatureMireds attribute. */
  ZB_ZCL_ATTR_COLOR_CONTROL_START_UP_COLOR_TEMPERATURE_MIREDS_ID     = 0x4010,
   /** @internal Number Of Primaries attribute */
  ZB_ZCL_ATTR_COLOR_CONTROL_NUMBER_OF_PRIMARIES_ID                   = 0x0010,
  /** @internal Primary 1 X attribute */
  ZB_ZCL_ATTR_COLOR_CONTROL_PRIMARY_1_X_ID                           = 0x0011,
  /** @internal Primary 1 Y attribute */
  ZB_ZCL_ATTR_COLOR_CONTROL_PRIMARY_1_Y_ID                           = 0x0012,
  /** @internal Primary 1 Intensity attribute */
  ZB_ZCL_ATTR_COLOR_CONTROL_PRIMARY_1_INTENSITY_ID                   = 0x0013,
  /** @internal Primary 2 X attribute */
  ZB_ZCL_ATTR_COLOR_CONTROL_PRIMARY_2_X_ID                           = 0x0015,
  /** @internal Primary 2 Y attribute */
  ZB_ZCL_ATTR_COLOR_CONTROL_PRIMARY_2_Y_ID                           = 0x0016,
  /** @internal Primary 2 Intensity attribute */
  ZB_ZCL_ATTR_COLOR_CONTROL_PRIMARY_2_INTENSITY_ID                   = 0x0017,
  /** @internal Primary 3 X attribute */
  ZB_ZCL_ATTR_COLOR_CONTROL_PRIMARY_3_X_ID                           = 0x0019,
  /** @internal Primary 3 Y attribute */
  ZB_ZCL_ATTR_COLOR_CONTROL_PRIMARY_3_Y_ID                           = 0x001a,
  /** @internal Primary 3 Intensity attribute */
  ZB_ZCL_ATTR_COLOR_CONTROL_PRIMARY_3_INTENSITY_ID                   = 0x001b,
  /** @internal Primary 4 X attribute */
  ZB_ZCL_ATTR_COLOR_CONTROL_PRIMARY_4_X_ID                           = 0x0020,
  /** @internal Primary 4 Y attribute */
  ZB_ZCL_ATTR_COLOR_CONTROL_PRIMARY_4_Y_ID                           = 0x0021,
  /** @internal Primary 4 Intensity attribute */
  ZB_ZCL_ATTR_COLOR_CONTROL_PRIMARY_4_INTENSITY_ID                   = 0x0022,
  /** @internal Primary 5 X attribute */
  ZB_ZCL_ATTR_COLOR_CONTROL_PRIMARY_5_X_ID                           = 0x0024,
  /** @internal Primary 5 Y attribute */
  ZB_ZCL_ATTR_COLOR_CONTROL_PRIMARY_5_Y_ID                           = 0x0025,
  /** @internal Primary 5 Intensity attribute */
  ZB_ZCL_ATTR_COLOR_CONTROL_PRIMARY_5_INTENSITY_ID                   = 0x0026,
  /** @internal Primary 6 X attribute */
  ZB_ZCL_ATTR_COLOR_CONTROL_PRIMARY_6_X_ID                           = 0x0028,
  /** @internal Primary 6 Y attribute */
  ZB_ZCL_ATTR_COLOR_CONTROL_PRIMARY_6_Y_ID                           = 0x0029,
  /** @internal Primary 6 Intensity attribute */
  ZB_ZCL_ATTR_COLOR_CONTROL_PRIMARY_6_INTENSITY_ID                   = 0x002a,
  /** The WhitePointX attribute contains the normalized chromaticity value x, as defined
   *  in the CIE xyY Color Space, of the current white point of the device. */
  ZB_ZCL_ATTR_COLOR_CONTROL_WHITE_POINT_X_ID                         = 0x0030,
  /** The WhitePointY attribute contains the normalized chromaticity value y, as defined
   *  in the CIE xyY Color Space, of the current white point of the device. */
  ZB_ZCL_ATTR_COLOR_CONTROL_WHITE_POINT_Y_ID                         = 0x0031,
  /** The ColorPointRX attribute contains the normalized chromaticity value x, as defined
   *  in the CIE xyY Color Space, of the red color point of the device. */
  ZB_ZCL_ATTR_COLOR_CONTROL_COLOR_POINT_RX_ID                        = 0x0032,
  /** The ColorPointRY attribute contains the normalized chromaticity value y, as defined
   *  in the CIE xyY Color Space, of the red color point of the device. */
  ZB_ZCL_ATTR_COLOR_CONTROL_COLOR_POINT_RY_ID                        = 0x0033,
  /** The ColorPointRIntensity attribute contains a representation of the relative
   *  intensity of the red color point as defined in the Dimming Light Curve in the
   *  Ballast Configuration cluster. */
  ZB_ZCL_ATTR_COLOR_CONTROL_COLOR_POINT_R_INTENSITY_ID               = 0x0034,
  /** The ColorPointGX attribute contains the normalized chromaticity value x, as defined
   *  in the CIE xyY Color Space, of the green color point of the device. */
  ZB_ZCL_ATTR_COLOR_CONTROL_COLOR_POINT_GX_ID                        = 0x0036,
  /** The ColorPointGY attribute contains the normalized chromaticity value y, as defined
   *  in the CIE xyY Color Space, of the green color point of the device. */
  ZB_ZCL_ATTR_COLOR_CONTROL_COLOR_POINT_GY_ID                        = 0x0037,
  /** The ColorPointGIntensity attribute contains a representation of the relative
   *  intensity of the green color point as defined in the Dimming Light Curve in the
   *  Ballast Configuration cluster. */
  ZB_ZCL_ATTR_COLOR_CONTROL_COLOR_POINT_G_INTENSITY_ID               = 0x0038,
  /** The ColorPointBX attribute contains the normalized chromaticity value x, as defined
   *  in the CIE xyY Color Space, of the blue color point of the device. */
  ZB_ZCL_ATTR_COLOR_CONTROL_COLOR_POINT_BX_ID                        = 0x003a,
  /** The ColorPointBY attribute contains the normalized chromaticity value y, as defined
   *  in the CIE xyY Color Space, of the blue color point of the device. */
  ZB_ZCL_ATTR_COLOR_CONTROL_COLOR_POINT_BY_ID                        = 0x003b,
  /** The ColorPointBIntensity attribute contains a representation of the relative
   *  intensity of the blue color point as defined in the Dimming Light Curve in the
   *  Ballast Configuration cluster. */
  ZB_ZCL_ATTR_COLOR_CONTROL_COLOR_POINT_B_INTENSITY_ID               = 0x003c,
};

/** @brief Default value for Color control cluster revision global attribute */
#define ZB_ZCL_COLOR_CONTROL_CLUSTER_REVISION_DEFAULT ((zb_uint16_t)0x0003u)

/** @brief Maximal value for implemented Color control cluster revision global attribute */
#define ZB_ZCL_COLOR_CONTROL_CLUSTER_REVISION_MAX ZB_ZCL_COLOR_CONTROL_CLUSTER_REVISION_DEFAULT

/** @brief Current Hue attribute minimum value */
#define ZB_ZCL_COLOR_CONTROL_CURRENT_HUE_MIN_VALUE          0

/** @brief Current Hue attribute maximum value */
#define ZB_ZCL_COLOR_CONTROL_CURRENT_HUE_MAX_VALUE          0xfe

/** Default OptionsMask value */
#define ZB_ZCL_COLOR_CONTROL_OPTIONS_MASK_DEFAULT_FIELD_VALUE ((zb_uint8_t)0x00)

/** Default OptionsOverride value */
#define ZB_ZCL_COLOR_CONTROL_OPTIONS_OVERRIDE_DEFAULT_FIELD_VALUE ((zb_uint8_t)0x00)

/**
 * @brief Color control Hue value
 */
enum zb_zcl_color_control_hue_e
{
/** @brief Current Hue attribute red value */
  ZB_ZCL_COLOR_CONTROL_HUE_RED  = 0,

/** @brief Current Hue attribute green value */
  ZB_ZCL_COLOR_CONTROL_HUE_GREEN = 85 /*254 / 3*/,

/** @brief Current Hue attribute blue value */
  ZB_ZCL_COLOR_CONTROL_HUE_BLUE =  169/*2*254 / 3*/
};

/** @brief Current Saturation attribute minimum value */
#define ZB_ZCL_COLOR_CONTROL_CURRENT_SATURATION_MIN_VALUE   0

/** @brief Current Saturation attribute maximum value */
#define ZB_ZCL_COLOR_CONTROL_CURRENT_SATURATION_MAX_VALUE   0xfe

/** @brief Remaining Time attribute minimum value */
#define ZB_ZCL_COLOR_CONTROL_REMAINING_TIME_MIN_VALUE       0

/** @brief Remaining Time attribute maximum value */
#define ZB_ZCL_COLOR_CONTROL_REMAINING_TIME_MAX_VALUE       0xfffe


/** @brief Current X attribute default value */
#define ZB_ZCL_COLOR_CONTROL_CURRENT_X_DEF_VALUE            0x616b

/** @brief Default value for CurrentHue attribute */
#define ZB_ZCL_COLOR_CONTROL_CURRENT_HUE_DEFAULT_VALUE ((zb_uint8_t)0x00)

/** @brief Default value for CurrentSaturation attribute */
#define ZB_ZCL_COLOR_CONTROL_CURRENT_SATURATION_DEFAULT_VALUE ((zb_uint8_t)0x00)

/** @brief Default value for RemainingTime attribute */
#define ZB_ZCL_COLOR_CONTROL_REMAINING_TIME_DEFAULT_VALUE ((zb_uint16_t)0x00)

/** @brief Current X attribute minimum value */
#define ZB_ZCL_COLOR_CONTROL_CURRENT_X_MIN_VALUE            0

/** @brief Current X attribute maximum value */
#define ZB_ZCL_COLOR_CONTROL_CURRENT_X_MAX_VALUE            0xfeff

/** @brief Current Y attribute default value */
#define ZB_ZCL_COLOR_CONTROL_CURRENT_Y_DEF_VALUE            0x607d

/** @brief Current Y attribute minimum value */
#define ZB_ZCL_COLOR_CONTROL_CURRENT_Y_MIN_VALUE            0

/** @brief Current Y attribute maximum value */
#define ZB_ZCL_COLOR_CONTROL_CURRENT_Y_MAX_VALUE            0xfeff

/** @brief Current Hue attribute red value */
#define ZB_ZCL_COLOR_CONTROL_COLOR_X_RED    (zb_uint16_t)(0.735484*ZB_ZCL_COLOR_CONTROL_CURRENT_X_MAX_VALUE)

/** @brief Current Hue attribute green value */
#define ZB_ZCL_COLOR_CONTROL_COLOR_X_GREEN  (zb_uint16_t)(0.0138702*ZB_ZCL_COLOR_CONTROL_CURRENT_X_MAX_VALUE)

/** @brief Current Hue attribute blue value */
#define ZB_ZCL_COLOR_CONTROL_COLOR_X_BLUE   (zb_uint16_t)(0.0995467*ZB_ZCL_COLOR_CONTROL_CURRENT_X_MAX_VALUE)


/** @brief Current Hue attribute red value */
#define ZB_ZCL_COLOR_CONTROL_COLOR_Y_RED   (zb_uint16_t)(0.264516*ZB_ZCL_COLOR_CONTROL_CURRENT_Y_MAX_VALUE)

/** @brief Current Hue attribute green value */
#define ZB_ZCL_COLOR_CONTROL_COLOR_Y_GREEN (zb_uint16_t)(0.750186*ZB_ZCL_COLOR_CONTROL_CURRENT_Y_MAX_VALUE)

/** @brief Current Hue attribute blue value */
#define ZB_ZCL_COLOR_CONTROL_COLOR_Y_BLUE  (zb_uint16_t)(0.11196*ZB_ZCL_COLOR_CONTROL_CURRENT_Y_MAX_VALUE)


/** @brief Color Temperature default value (4000K) */
#define ZB_ZCL_COLOR_CONTROL_COLOR_TEMPERATURE_DEF_VALUE    0x00fa

/** @brief Color Temperature minimum value */
#define ZB_ZCL_COLOR_CONTROL_COLOR_TEMPERATURE_MIN_VALUE    0

/** @brief Color Temperature maximum value */
#define ZB_ZCL_COLOR_CONTROL_COLOR_TEMPERATURE_MAX_VALUE    0xfeff

/** @brief Default value for ColorMode attribute */
#define ZB_ZCL_COLOR_CONTROL_COLOR_MODE_DEFAULT_VALUE ((zb_uint8_t)0x01)

/** @brief Default value for Options attribute */
#define ZB_ZCL_COLOR_CONTROL_OPTIONS_DEFAULT_VALUE ((zb_uint8_t)0x00)

/** @brief Default value for EnhancedCurrentHue attribute */
#define ZB_ZCL_COLOR_CONTROL_ENHANCED_CURRENT_HUE_DEFAULT_VALUE ((zb_uint16_t)0x0000)

/** @brief Default value for EnhancedColorMode attribute */
#define ZB_ZCL_COLOR_CONTROL_ENHANCED_COLOR_MODE_DEFAULT_VALUE ((zb_uint8_t)0x01)

/** @brief Default value for ColorLoopActive attribute */
#define ZB_ZCL_COLOR_CONTROL_COLOR_LOOP_ACTIVE_DEFAULT_VALUE ((zb_uint8_t)0x00)

/** @brief Default value for ColorLoopDirection attribute */
#define ZB_ZCL_COLOR_CONTROL_COLOR_LOOP_DIRECTION_DEFAULT_VALUE ((zb_uint8_t)0x00)

/** @brief Invalid value for Physical Mireds attribute */
#define ZB_ZCL_COLOR_CONTROL_COLOR_TEMP_PHYSICAL_MIREDS_INVALID_VALUE ((zb_uint16_t)0xffff)

/*! @brief Color Mode attribute enumeration
    @see ZCL spec, subclause 5.2.2.2.1.9
*/
enum zb_zcl_color_control_color_mode_e
{
  /** CurrentHue and CurrentSaturation */
  ZB_ZCL_COLOR_CONTROL_COLOR_MODE_HUE_SATURATION    = 0x00,
  /** CurrentX and CurrentY */
  ZB_ZCL_COLOR_CONTROL_COLOR_MODE_CURRENT_X_Y       = 0x01,
  /** ColorTemperature */
  ZB_ZCL_COLOR_CONTROL_COLOR_MODE_TEMPERATURE       = 0x02,
};


/** @brief Number Of Primaries attribute maximum value */
#define ZB_ZCL_COLOR_CONTROL_NUMBER_OF_PRIMARIES_MAX_VALUE    6

/** @brief Primary X attribute maximum value */
#define ZB_ZCL_COLOR_CONTROL_PRIMARY_X_MAX_VALUE            0xfeff

/** @brief Primary Y attribute maximum value */
#define ZB_ZCL_COLOR_CONTROL_PRIMARY_Y_MAX_VALUE            0xfeff

/** @brief Primary Intensity attribute maximum value */
#define ZB_ZCL_COLOR_CONTROL_PRIMARY_INTENSITY_MAX_VALUE    0xfe

/** @brief Primary Intensity attribute not used value */
#define ZB_ZCL_COLOR_CONTROL_PRIMARY_INTENSITY_NON_USED_VALUE    0xff

/** @brief Enhanced Hue Min attribute default value */
#define ZB_ZCL_COLOR_CONTROL_ENHANCED_HUE_MIN_DEF_VALUE        0

/** @brief Enhanced Hue Max attribute default value */
#define ZB_ZCL_COLOR_CONTROL_ENHANCED_HUE_MAX_DEF_VALUE        0xffff

/** @brief Current Hue attribute red value */
#define ZB_ZCL_COLOR_CONTROL_ENHANCED_HUE_RED   (zb_uint16_t)(ZB_ZCL_COLOR_CONTROL_HUE_RED << 8)

/** @brief Current Hue attribute green value */
#define ZB_ZCL_COLOR_CONTROL_ENHANCED_GREEN     (zb_uint16_t)(ZB_ZCL_COLOR_CONTROL_HUE_GREEN << 8)

/** @brief Current Hue attribute blue value */
#define ZB_ZCL_COLOR_CONTROL_ENHANCED_BLUE      (zb_uint16_t)(ZB_ZCL_COLOR_CONTROL_HUE_BLUE << 8)

/** @brief Color Loop Time attribute default value */
#define ZB_ZCL_COLOR_CONTROL_COLOR_LOOP_TIME_DEF_VALUE          0x0019

/** @brief Color Loop Start Enhanced Hue attribute default value */
#define ZB_ZCL_COLOR_CONTROL_COLOR_LOOP_START_DEF_VALUE         0x2300

/** @brief Default value for ColorLoopStoredEnhancedHue attribute */
#define ZB_ZCL_COLOR_CONTROL_COLOR_LOOP_STORED_ENHANCED_HUE_DEFAULT_VALUE ((zb_uint16_t)0x0000)
/** @brief Default value for ColorCapabilities attribute */
#define ZB_ZCL_COLOR_CONTROL_COLOR_CAPABILITIES_DEFAULT_VALUE ((zb_uint16_t)0x0000)

/** @brief Default value for ColorTempPhysicalMinMireds attribute */
#define ZB_ZCL_COLOR_CONTROL_COLOR_TEMP_PHYSICAL_MIN_MIREDS_DEFAULT_VALUE ((zb_uint16_t)0x0000)

/** @brief Default value for ColorTempPhysicalMaxMireds attribute */
#define ZB_ZCL_COLOR_CONTROL_COLOR_TEMP_PHYSICAL_MAX_MIREDS_DEFAULT_VALUE ((zb_uint16_t)0xFEFF)

/** @brief Color Capabilities attribute maximum value */
#define ZB_ZCL_COLOR_CONTROL_COLOR_CAPABILITIES_MAX_VALUE       0x001f

/** @brief Start Up Color Temperature Mireds Max value */
#define ZB_ZCL_COLOR_CONTROL_START_UP_COLOR_TEMPERATURE_MAX_VALUE            ZB_ZCL_COLOR_CONTROL_COLOR_TEMPERATURE_MAX_VALUE

/** @brief Start Up Color Temperature Mireds Use Previous value */
#define ZB_ZCL_COLOR_CONTROL_START_UP_COLOR_TEMPERATURE_USE_PREVIOUS_VALUE   0xffff

/*! @brief Options attribute flags
    @see ZCL spec, subclause 5.2.2.1.1.10
*/
enum zb_zcl_color_control_options_e
{
  /** Execute command if the On/Off cluster, OnOff attribute is 0x00 (FALSE) */
  ZB_ZCL_COLOR_CONTROL_OPTIONS_EXECUTE_IF_OFF  = 0,
};

/*! @brief Enhanced Color Mode attribute enumeration
    @see ZCL spec, subclause 5.2.2.2.1.2
*/
enum zb_zcl_color_control_enhanced_color_mode_e
{
  /** CurrentHue and CurrentSaturation */
  ZB_ZCL_COLOR_CONTROL_COLOR_EX_MODE_HUE_SATURATION    = 0x00,
  /** CurrentX and CurrentY */
  ZB_ZCL_COLOR_CONTROL_COLOR_EX_MODE_CURRENT_X_Y       = 0x01,
  /** ColorTemperature */
  ZB_ZCL_COLOR_CONTROL_COLOR_EX_MODE_TEMPERATURE       = 0x02,
  /** EnhancedCurrentHue and EnhancedCurrentSaturation */
  ZB_ZCL_COLOR_CONTROL_COLOR_EX_MODE_HUE_SATURATION_EX = 0x03,
};

/*! @brief Enhanced Color Capabilities attribute flags
    @see ZCL spec, subclause 5.2.2.2.1.8
*/
enum zb_zcl_color_control_color_capabilities_e
{
  /** Hue/saturation supported*/
  ZB_ZCL_COLOR_CONTROL_CAPABILITIES_HUE_SATURATION  = 1 << 0,
  /** Enhanced hue supported*/
  ZB_ZCL_COLOR_CONTROL_CAPABILITIES_EX_HUE          = 1 << 1,
  /** Color loop supported*/
  ZB_ZCL_COLOR_CONTROL_CAPABILITIES_COLOR_LOOP      = 1 << 2,
  /** XY supported*/
  ZB_ZCL_COLOR_CONTROL_CAPABILITIES_X_Y             = 1 << 3,
  /** Color temperature supported*/
  ZB_ZCL_COLOR_CONTROL_CAPABILITIES_COLOR_TEMP      = 1 << 4,
};

/*!
  @brief Declare attribute list for Color Control cluster (extended attribute set)
  @param attr_list - attribute list name
  @param current_hue - pointer to variable to store current_hue attribute value
  @param current_saturation - pointer to variable to store current_saturation attribute value
  @param remaining_time - pointer to variable to store remaining_time attribute value
  @param current_X - pointer to variable to store current_X attribute value
  @param current_Y - pointer to variable to store current_Y attribute value
  @param color_temperature - pointer to variable to store color_temperature attribute value
  @param color_mode - pointer to variable to store color_mode attribute value
  @param options - pointer to variable to store options attribute value
  @param number_primaries - pointer to variable to store number_primaries attribute value
  @param primary_1_X - pointer to variable to store primary_1_X attribute value
  @param primary_1_Y - pointer to variable to store primary_1_Y attribute value
  @param primary_1_intensity - pointer to variable to store primary_1_intensity attribute value
  @param primary_2_X - pointer to variable to store primary_2_X attribute value
  @param primary_2_Y - pointer to variable to store primary_2_Y attribute value
  @param primary_2_intensity - pointer to variable to store primary_2_intensity attribute value
  @param primary_3_X - pointer to variable to store primary_3_X attribute value
  @param primary_3_Y - pointer to variable to store primary_3_Y attribute value
  @param primary_3_intensity - pointer to variable to store primary_3_intensity attribute value
  @param primary_4_X - pointer to variable to store primary_4_X attribute value
  @param primary_4_Y - pointer to variable to store primary_4_Y attribute value
  @param primary_4_intensity - pointer to variable to store primary_4_intensity attribute value
  @param primary_5_X - pointer to variable to store primary_5_X attribute value
  @param primary_5_Y - pointer to variable to store primary_5_Y attribute value
  @param primary_5_intensity - pointer to variable to store primary_5_intensity attribute value
  @param primary_6_X - pointer to variable to store primary_6_X attribute value
  @param primary_6_Y - pointer to variable to store primary_6_Y attribute value
  @param primary_6_intensity - pointer to variable to store primary_6_intensity attribute value
  @param enhanced_current_hue - pointer to variable to store Enhanced Current Hue attribute value
  @param enhanced_color_mode - pointer to variable to store Enhanced Color Mode attribute value
  @param color_loop_active - pointer to variable to store Color Loop Active attribute value
  @param color_loop_direction - pointer to variable to store Color Loop Direction attribute value
  @param color_loop_time - pointer to variable to store Color Loop Time attribute value
  @param color_loop_start - pointer to variable to store Color Loop Start Enhanced Hue attribute value
  @param color_loop_stored - pointer to variable to store Color Loop Stored Enhanced Hue attribute value
  @param color_capabilities - pointer to variable to store Color Capabilities attribute value
  @param color_temp_physical_min - pointer to variable to store Color Temp Physical Min attribute value
  @param color_temp_physical_max - pointer to variable to store Color Temp Physical Max attribute value
  @param couple_color_temp_to_level_min - pointer to variable to store Couple Color Temp To Level Min Mireds attribute value
  @param start_up_color_temp - pointer to variable to store Start Up Color Temperature Mireds attribute value
*/
#define ZB_ZCL_DECLARE_COLOR_CONTROL_ATTRIB_LIST_EXT(attr_list, current_hue,                       \
    current_saturation, remaining_time, current_X, current_Y, color_temperature,                   \
    color_mode, options, number_primaries, primary_1_X, primary_1_Y, primary_1_intensity,          \
    primary_2_X, primary_2_Y, primary_2_intensity, primary_3_X, primary_3_Y,                       \
    primary_3_intensity, primary_4_X, primary_4_Y, primary_4_intensity, primary_5_X,               \
    primary_5_Y, primary_5_intensity, primary_6_X, primary_6_Y, primary_6_intensity,               \
    enhanced_current_hue, enhanced_color_mode, color_loop_active, color_loop_direction,            \
    color_loop_time, color_loop_start, color_loop_stored, color_capabilities,                      \
    color_temp_physical_min, color_temp_physical_max,                                              \
    couple_color_temp_to_level_min, start_up_color_temp)                                           \
  ZB_ZCL_START_DECLARE_ATTRIB_LIST_CLUSTER_REVISION(attr_list, ZB_ZCL_COLOR_CONTROL)               \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_COLOR_CONTROL_CURRENT_HUE_ID, (current_hue))                    \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_COLOR_CONTROL_CURRENT_SATURATION_ID, (current_saturation))      \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_COLOR_CONTROL_REMAINING_TIME_ID, (remaining_time))              \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_COLOR_CONTROL_CURRENT_X_ID, (current_X))                        \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_COLOR_CONTROL_CURRENT_Y_ID, (current_Y))                        \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_COLOR_CONTROL_COLOR_TEMPERATURE_ID, (color_temperature))        \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_COLOR_CONTROL_COLOR_MODE_ID, (color_mode))                      \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_COLOR_CONTROL_OPTIONS_ID, (options))                            \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_COLOR_CONTROL_NUMBER_OF_PRIMARIES_ID, (number_primaries))       \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_COLOR_CONTROL_PRIMARY_1_X_ID, (primary_1_X))                    \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_COLOR_CONTROL_PRIMARY_1_Y_ID, (primary_1_Y))                    \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_COLOR_CONTROL_PRIMARY_1_INTENSITY_ID, (primary_1_intensity))    \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_COLOR_CONTROL_PRIMARY_2_X_ID, (primary_2_X))                    \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_COLOR_CONTROL_PRIMARY_2_Y_ID, (primary_2_Y))                    \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_COLOR_CONTROL_PRIMARY_2_INTENSITY_ID, (primary_2_intensity))    \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_COLOR_CONTROL_PRIMARY_3_X_ID, (primary_3_X))                    \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_COLOR_CONTROL_PRIMARY_3_Y_ID, (primary_3_Y))                    \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_COLOR_CONTROL_PRIMARY_3_INTENSITY_ID, (primary_3_intensity))    \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_COLOR_CONTROL_PRIMARY_4_X_ID, (primary_4_X))                    \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_COLOR_CONTROL_PRIMARY_4_Y_ID, (primary_4_Y))                    \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_COLOR_CONTROL_PRIMARY_4_INTENSITY_ID, (primary_4_intensity))    \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_COLOR_CONTROL_PRIMARY_5_X_ID, (primary_5_X))                    \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_COLOR_CONTROL_PRIMARY_5_Y_ID, (primary_5_Y))                    \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_COLOR_CONTROL_PRIMARY_5_INTENSITY_ID, (primary_5_intensity))    \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_COLOR_CONTROL_PRIMARY_6_X_ID, (primary_6_X))                    \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_COLOR_CONTROL_PRIMARY_6_Y_ID, (primary_6_Y))                    \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_COLOR_CONTROL_PRIMARY_6_INTENSITY_ID, (primary_6_intensity))    \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_COLOR_CONTROL_ENHANCED_CURRENT_HUE_ID, (enhanced_current_hue))  \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_COLOR_CONTROL_ENHANCED_COLOR_MODE_ID, (enhanced_color_mode))    \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_COLOR_CONTROL_COLOR_LOOP_ACTIVE_ID, (color_loop_active))        \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_COLOR_CONTROL_COLOR_LOOP_DIRECTION_ID, (color_loop_direction))  \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_COLOR_CONTROL_COLOR_LOOP_TIME_ID, (color_loop_time))            \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_COLOR_CONTROL_COLOR_LOOP_START_ENHANCED_HUE_ID, (color_loop_start))          \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_COLOR_CONTROL_COLOR_LOOP_STORED_ENHANCED_HUE_ID, (color_loop_stored))        \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_COLOR_CONTROL_COLOR_CAPABILITIES_ID, (color_capabilities))                   \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_COLOR_CONTROL_COLOR_TEMP_PHYSICAL_MIN_MIREDS_ID, (color_temp_physical_min))  \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_COLOR_CONTROL_COLOR_TEMP_PHYSICAL_MAX_MIREDS_ID, (color_temp_physical_max))  \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_COLOR_CONTROL_COUPLE_COLOR_TEMP_TO_LEVEL_MIN_MIREDS_ID, (couple_color_temp_to_level_min))  \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_COLOR_CONTROL_START_UP_COLOR_TEMPERATURE_MIREDS_ID, (start_up_color_temp))                 \
  ZB_ZCL_FINISH_DECLARE_ATTRIB_LIST

/*!
  @brief Declare attribute list for Color Control cluster (mandatory attribute set)
  @param attr_list - attribute list name
  @param current_X - pointer to variable to store current_X attribute value
  @param current_Y - pointer to variable to store current_Y attribute value
*/
#define ZB_ZCL_DECLARE_COLOR_CONTROL_ATTRIB_LIST(attr_list, current_X, current_Y)               \
  ZB_ZCL_START_DECLARE_ATTRIB_LIST_CLUSTER_REVISION(attr_list, ZB_ZCL_COLOR_CONTROL)            \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_COLOR_CONTROL_CURRENT_X_ID, (current_X))                     \
  ZB_ZCL_SET_ATTR_DESC(ZB_ZCL_ATTR_COLOR_CONTROL_CURRENT_Y_ID, (current_Y))                     \
  ZB_ZCL_FINISH_DECLARE_ATTRIB_LIST

/*! @} */ /* Color Control cluster attributes */

/** @cond internals_doc */
/*! @name Color Control cluster internals
    Internal structures for Color Control cluster
    @internal
    @{
*/

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_COLOR_CONTROL_CURRENT_HUE_ID(data_ptr)   \
{                                                                                   \
  ZB_ZCL_ATTR_COLOR_CONTROL_CURRENT_HUE_ID,                                         \
  ZB_ZCL_ATTR_TYPE_U8,                                                              \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY | ZB_ZCL_ATTR_ACCESS_REPORTING,                      \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                               \
  (void*) data_ptr                                                                  \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_COLOR_CONTROL_CURRENT_SATURATION_ID(data_ptr)    \
{                                                                                           \
  ZB_ZCL_ATTR_COLOR_CONTROL_CURRENT_SATURATION_ID,                                          \
  ZB_ZCL_ATTR_TYPE_U8,                                                                      \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY | ZB_ZCL_ATTR_ACCESS_REPORTING | ZB_ZCL_ATTR_ACCESS_SCENE,   \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                                       \
  (void*) data_ptr                                                                          \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_COLOR_CONTROL_REMAINING_TIME_ID(data_ptr) \
{                                                                                    \
  ZB_ZCL_ATTR_COLOR_CONTROL_REMAINING_TIME_ID,                                       \
  ZB_ZCL_ATTR_TYPE_U16,                                                              \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                                      \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                                \
  (void*) data_ptr                                                                   \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_COLOR_CONTROL_CURRENT_X_ID(data_ptr)           \
{                                                                                         \
  ZB_ZCL_ATTR_COLOR_CONTROL_CURRENT_X_ID,                                                 \
  ZB_ZCL_ATTR_TYPE_U16,                                                                   \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY | ZB_ZCL_ATTR_ACCESS_REPORTING | ZB_ZCL_ATTR_ACCESS_SCENE, \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                                     \
  (void*) data_ptr                                                                        \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_COLOR_CONTROL_CURRENT_Y_ID(data_ptr)           \
{                                                                                         \
  ZB_ZCL_ATTR_COLOR_CONTROL_CURRENT_Y_ID,                                                 \
  ZB_ZCL_ATTR_TYPE_U16,                                                                   \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY | ZB_ZCL_ATTR_ACCESS_REPORTING | ZB_ZCL_ATTR_ACCESS_SCENE, \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                                     \
  (void*) data_ptr                                                                        \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_COLOR_CONTROL_DRIFT_COMPENSATION_ID(data_ptr)  \
{                                                                                         \
  ZB_ZCL_ATTR_COLOR_CONTROL_DRIFT_COMPENSATION_ID,                                        \
  ZB_ZCL_ATTR_TYPE_8BIT_ENUM,                                                             \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                                           \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                                     \
  (void*) data_ptr                                                                        \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_COLOR_CONTROL_COMPENSATION_TEXT_ID(data_ptr)   \
{                                                                                         \
  ZB_ZCL_ATTR_COLOR_CONTROL_COMPENSATION_TEXT_ID,                                         \
  ZB_ZCL_ATTR_TYPE_CHAR_STRING,                                                           \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                                           \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                                     \
  (void*) data_ptr                                                                        \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_COLOR_CONTROL_COLOR_TEMPERATURE_ID(data_ptr)   \
{                                                                                         \
  ZB_ZCL_ATTR_COLOR_CONTROL_COLOR_TEMPERATURE_ID,                                         \
  ZB_ZCL_ATTR_TYPE_U16,                                                                   \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY | ZB_ZCL_ATTR_ACCESS_REPORTING | ZB_ZCL_ATTR_ACCESS_SCENE, \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                                     \
  (void*) data_ptr                                                                        \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_COLOR_CONTROL_COLOR_MODE_ID(data_ptr)    \
{                                                                                   \
  ZB_ZCL_ATTR_COLOR_CONTROL_COLOR_MODE_ID,                                          \
  ZB_ZCL_ATTR_TYPE_8BIT_ENUM,                                                       \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                                     \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                               \
  (void*) data_ptr                                                                  \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_COLOR_CONTROL_OPTIONS_ID(data_ptr)       \
{                                                                                   \
  ZB_ZCL_ATTR_COLOR_CONTROL_OPTIONS_ID,                                             \
  ZB_ZCL_ATTR_TYPE_8BITMAP,                                                         \
  ZB_ZCL_ATTR_ACCESS_READ_WRITE,                                                    \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                               \
  (void*) data_ptr                                                                  \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_COLOR_CONTROL_ENHANCED_CURRENT_HUE_ID(data_ptr)  \
{                                                                                   \
  ZB_ZCL_ATTR_COLOR_CONTROL_ENHANCED_CURRENT_HUE_ID,                                \
  ZB_ZCL_ATTR_TYPE_U16,                                                             \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY | ZB_ZCL_ATTR_ACCESS_SCENE,                          \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                               \
  (void*) data_ptr                                                                  \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_COLOR_CONTROL_ENHANCED_COLOR_MODE_ID(data_ptr)  \
{                                                                                   \
  ZB_ZCL_ATTR_COLOR_CONTROL_ENHANCED_COLOR_MODE_ID,                                 \
  ZB_ZCL_ATTR_TYPE_8BIT_ENUM,                                                       \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                                     \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                               \
  (void*) data_ptr                                                                  \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_COLOR_CONTROL_COLOR_LOOP_ACTIVE_ID(data_ptr)  \
{                                                                                   \
  ZB_ZCL_ATTR_COLOR_CONTROL_COLOR_LOOP_ACTIVE_ID,                                   \
  ZB_ZCL_ATTR_TYPE_U8,                                                              \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY | ZB_ZCL_ATTR_ACCESS_SCENE,                          \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                               \
  (void*) data_ptr                                                                  \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_COLOR_CONTROL_COLOR_LOOP_DIRECTION_ID(data_ptr)  \
{                                                                                   \
  ZB_ZCL_ATTR_COLOR_CONTROL_COLOR_LOOP_DIRECTION_ID,                                \
  ZB_ZCL_ATTR_TYPE_U8,                                                              \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY | ZB_ZCL_ATTR_ACCESS_SCENE,                          \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                               \
  (void*) data_ptr                                                                  \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_COLOR_CONTROL_COLOR_LOOP_TIME_ID(data_ptr)  \
{                                                                                   \
  ZB_ZCL_ATTR_COLOR_CONTROL_COLOR_LOOP_TIME_ID,                                     \
  ZB_ZCL_ATTR_TYPE_U16,                                                             \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY | ZB_ZCL_ATTR_ACCESS_SCENE,                          \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                               \
  (void*) data_ptr                                                                  \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_COLOR_CONTROL_COLOR_LOOP_START_ENHANCED_HUE_ID(data_ptr)  \
{                                                                                   \
  ZB_ZCL_ATTR_COLOR_CONTROL_COLOR_LOOP_START_ENHANCED_HUE_ID,                       \
  ZB_ZCL_ATTR_TYPE_U16,                                                             \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                                     \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                               \
  (void*) data_ptr                                                                  \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_COLOR_CONTROL_COLOR_LOOP_STORED_ENHANCED_HUE_ID(data_ptr)  \
{                                                                                   \
  ZB_ZCL_ATTR_COLOR_CONTROL_COLOR_LOOP_STORED_ENHANCED_HUE_ID,                      \
  ZB_ZCL_ATTR_TYPE_U16,                                                             \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                                     \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                               \
  (void*) data_ptr                                                                  \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_COLOR_CONTROL_COLOR_CAPABILITIES_ID(data_ptr)  \
{                                                                                   \
  ZB_ZCL_ATTR_COLOR_CONTROL_COLOR_CAPABILITIES_ID,                                  \
  ZB_ZCL_ATTR_TYPE_16BITMAP,                                                        \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                                     \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                               \
  (void*) data_ptr                                                                  \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_COLOR_CONTROL_COLOR_TEMP_PHYSICAL_MIN_MIREDS_ID(data_ptr)  \
{                                                                                   \
  ZB_ZCL_ATTR_COLOR_CONTROL_COLOR_TEMP_PHYSICAL_MIN_MIREDS_ID,                      \
  ZB_ZCL_ATTR_TYPE_U16,                                                             \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                                     \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                               \
  (void*) data_ptr                                                                  \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_COLOR_CONTROL_COLOR_TEMP_PHYSICAL_MAX_MIREDS_ID(data_ptr)  \
{                                                                                   \
  ZB_ZCL_ATTR_COLOR_CONTROL_COLOR_TEMP_PHYSICAL_MAX_MIREDS_ID,                      \
  ZB_ZCL_ATTR_TYPE_U16,                                                             \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                                     \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                               \
  (void*) data_ptr                                                                  \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_COLOR_CONTROL_COUPLE_COLOR_TEMP_TO_LEVEL_MIN_MIREDS_ID(data_ptr)  \
{                                                                                   \
  ZB_ZCL_ATTR_COLOR_CONTROL_COUPLE_COLOR_TEMP_TO_LEVEL_MIN_MIREDS_ID,               \
  ZB_ZCL_ATTR_TYPE_U16,                                                             \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                                     \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                               \
  (void*) data_ptr                                                                  \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_COLOR_CONTROL_START_UP_COLOR_TEMPERATURE_MIREDS_ID(data_ptr)  \
{                                                                                   \
  ZB_ZCL_ATTR_COLOR_CONTROL_START_UP_COLOR_TEMPERATURE_MIREDS_ID,                   \
  ZB_ZCL_ATTR_TYPE_U16,                                                             \
  ZB_ZCL_ATTR_ACCESS_READ_WRITE,                                                    \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                               \
  (void*) data_ptr                                                                  \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_COLOR_CONTROL_NUMBER_OF_PRIMARIES_ID(data_ptr) \
{                                                                                   \
  ZB_ZCL_ATTR_COLOR_CONTROL_NUMBER_OF_PRIMARIES_ID,                                 \
  ZB_ZCL_ATTR_TYPE_U8,                                                              \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                                     \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                               \
  (void*) data_ptr                                                                  \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_COLOR_CONTROL_PRIMARY_1_X_ID(data_ptr)   \
{                                                                                   \
  ZB_ZCL_ATTR_COLOR_CONTROL_PRIMARY_1_X_ID,                                         \
  ZB_ZCL_ATTR_TYPE_U16,                                                             \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                                     \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                               \
  (void*) data_ptr                                                                  \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_COLOR_CONTROL_PRIMARY_1_Y_ID(data_ptr)   \
{                                                                                   \
  ZB_ZCL_ATTR_COLOR_CONTROL_PRIMARY_1_Y_ID,                                         \
  ZB_ZCL_ATTR_TYPE_U16,                                                             \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                                     \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                               \
  (void*) data_ptr                                                                  \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_COLOR_CONTROL_PRIMARY_1_INTENSITY_ID(data_ptr) \
{                                                                                   \
  ZB_ZCL_ATTR_COLOR_CONTROL_PRIMARY_1_INTENSITY_ID,                                 \
  ZB_ZCL_ATTR_TYPE_U8,                                                              \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                                     \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                               \
  (void*) data_ptr                                                                  \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_COLOR_CONTROL_PRIMARY_2_X_ID(data_ptr)   \
{                                                                                   \
  ZB_ZCL_ATTR_COLOR_CONTROL_PRIMARY_2_X_ID,                                         \
  ZB_ZCL_ATTR_TYPE_U16,                                                             \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                                     \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                               \
  (void*) data_ptr                                                                  \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_COLOR_CONTROL_PRIMARY_2_Y_ID(data_ptr)   \
{                                                                                   \
  ZB_ZCL_ATTR_COLOR_CONTROL_PRIMARY_2_Y_ID,                                         \
  ZB_ZCL_ATTR_TYPE_U16,                                                             \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                                     \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                               \
  (void*) data_ptr                                                                  \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_COLOR_CONTROL_PRIMARY_2_INTENSITY_ID(data_ptr) \
{                                                                                   \
  ZB_ZCL_ATTR_COLOR_CONTROL_PRIMARY_2_INTENSITY_ID,                                 \
  ZB_ZCL_ATTR_TYPE_U8,                                                              \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                                     \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                               \
  (void*) data_ptr                                                                  \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_COLOR_CONTROL_PRIMARY_3_X_ID(data_ptr)   \
{                                                                                   \
  ZB_ZCL_ATTR_COLOR_CONTROL_PRIMARY_3_X_ID,                                         \
  ZB_ZCL_ATTR_TYPE_U16,                                                             \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                                     \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                               \
  (void*) data_ptr                                                                  \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_COLOR_CONTROL_PRIMARY_3_Y_ID(data_ptr)   \
{                                                                                   \
  ZB_ZCL_ATTR_COLOR_CONTROL_PRIMARY_3_Y_ID,                                         \
  ZB_ZCL_ATTR_TYPE_U16,                                                             \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                                     \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                               \
  (void*) data_ptr                                                                  \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_COLOR_CONTROL_PRIMARY_3_INTENSITY_ID(data_ptr) \
{                                                                                   \
  ZB_ZCL_ATTR_COLOR_CONTROL_PRIMARY_3_INTENSITY_ID,                                 \
  ZB_ZCL_ATTR_TYPE_U8,                                                              \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                                     \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                               \
  (void*) data_ptr                                                                  \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_COLOR_CONTROL_PRIMARY_4_X_ID(data_ptr)   \
{                                                                                   \
  ZB_ZCL_ATTR_COLOR_CONTROL_PRIMARY_4_X_ID,                                         \
  ZB_ZCL_ATTR_TYPE_U16,                                                             \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                                     \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                               \
  (void*) data_ptr                                                                  \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_COLOR_CONTROL_PRIMARY_4_Y_ID(data_ptr)   \
{                                                                                   \
  ZB_ZCL_ATTR_COLOR_CONTROL_PRIMARY_4_Y_ID,                                         \
  ZB_ZCL_ATTR_TYPE_U16,                                                             \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                                     \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                               \
  (void*) data_ptr                                                                  \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_COLOR_CONTROL_PRIMARY_4_INTENSITY_ID(data_ptr) \
{                                                                                   \
  ZB_ZCL_ATTR_COLOR_CONTROL_PRIMARY_4_INTENSITY_ID,                                 \
  ZB_ZCL_ATTR_TYPE_U8,                                                              \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                                     \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                               \
  (void*) data_ptr                                                                  \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_COLOR_CONTROL_PRIMARY_5_X_ID(data_ptr)   \
{                                                                                   \
  ZB_ZCL_ATTR_COLOR_CONTROL_PRIMARY_5_X_ID,                                         \
  ZB_ZCL_ATTR_TYPE_U16,                                                             \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                                     \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                               \
  (void*) data_ptr                                                                  \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_COLOR_CONTROL_PRIMARY_5_Y_ID(data_ptr)   \
{                                                                                   \
  ZB_ZCL_ATTR_COLOR_CONTROL_PRIMARY_5_Y_ID,                                         \
  ZB_ZCL_ATTR_TYPE_U16,                                                             \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                                     \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                               \
  (void*) data_ptr                                                                  \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_COLOR_CONTROL_PRIMARY_5_INTENSITY_ID(data_ptr) \
{                                                                                   \
  ZB_ZCL_ATTR_COLOR_CONTROL_PRIMARY_5_INTENSITY_ID,                                 \
  ZB_ZCL_ATTR_TYPE_U8,                                                              \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                                     \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                               \
  (void*) data_ptr                                                                  \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_COLOR_CONTROL_PRIMARY_6_X_ID(data_ptr)   \
{                                                                                   \
  ZB_ZCL_ATTR_COLOR_CONTROL_PRIMARY_6_X_ID,                                         \
  ZB_ZCL_ATTR_TYPE_U16,                                                             \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                                     \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                               \
  (void*) data_ptr                                                                  \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_COLOR_CONTROL_PRIMARY_6_Y_ID(data_ptr)   \
{                                                                                   \
  ZB_ZCL_ATTR_COLOR_CONTROL_PRIMARY_6_Y_ID,                                         \
  ZB_ZCL_ATTR_TYPE_U16,                                                             \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                                     \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                               \
  (void*) data_ptr                                                                  \
}

#define ZB_SET_ATTR_DESCR_WITH_ZB_ZCL_ATTR_COLOR_CONTROL_PRIMARY_6_INTENSITY_ID(data_ptr) \
{                                                                                   \
  ZB_ZCL_ATTR_COLOR_CONTROL_PRIMARY_6_INTENSITY_ID,                                 \
  ZB_ZCL_ATTR_TYPE_U8,                                                              \
  ZB_ZCL_ATTR_ACCESS_READ_ONLY,                                                     \
  (ZB_ZCL_NON_MANUFACTURER_SPECIFIC),                                               \
  (void*) data_ptr                                                                  \
}

/** @internal Structure of Move variables for Move alarm
 */
typedef struct zb_zcl_color_control_move_loop_s
{
  /** Command header */
  zb_zcl_parsed_hdr_t cmd_info;

  /** @brief Time for last process move loop command */
  zb_time_t time_last;

  /** @brief attribute id */
  zb_uint16_t attr_id;
  /** @brief rate */
  zb_int16_t rate;
  /** @brief limit */
  zb_uint16_t limit;
  /** @brief time mod - different last time - process time*/
  zb_uint16_t time_mod;

  /** @brief attribute id2 */
  zb_uint16_t attr_id2;
  /** @brief rate2 */
  zb_int16_t rate2;
  /** @brief limit2 */
  zb_uint16_t limit2;
  /** @brief time mod - different last time - process time*/
  zb_uint16_t time_mod2;

} zb_zcl_color_control_move_loop_t;

#define ZB_ZCL_COLOR_CONTROL_FILL_MOVE_DATA(buf, cmd_info_,                     \
          attr_id_, rate_, limit_)                                              \
{                                                                               \
  zb_zcl_color_control_move_loop_t *loop_data =                                 \
          ZB_BUF_GET_PARAM((buf), zb_zcl_color_control_move_loop_t);            \
  ZB_MEMMOVE(&(loop_data->cmd_info), (cmd_info_), sizeof(zb_zcl_parsed_hdr_t)); \
  loop_data->time_last = ZB_TIMER_GET();                                        \
  loop_data->attr_id = (attr_id_);                                              \
  loop_data->rate = (rate_);                                                    \
  loop_data->limit = (limit_);                                                  \
  loop_data->time_mod = 0;                                                      \
  loop_data->attr_id2 = 0;                                                      \
  loop_data->time_mod2 = 0;                                                     \
}

/*! @internal
 *  @brief Structure for loop of move to command */
typedef struct zb_zcl_color_control_move_to_loop_s
{
  /** Command header */
  zb_zcl_parsed_hdr_t cmd_info;

  /** Attribute_ID */
  zb_uint16_t attr_id;
  /** Start value */
  zb_uint16_t start_value;
  /** Finish value */
  zb_uint16_t finish_value;
  /** Is increment direction */
  zb_bool_t incr_direction;

  /** Attribute_ID */
  zb_uint16_t attr_id2;
  /** Start value */
  zb_uint16_t start_value2;
  /** Finish value */
  zb_uint16_t finish_value2;

  /** Start time - beacon interval */
  zb_time_t start_time;
  /** transition time - time unit = 1/10sec */
  zb_time_t transition_time;
} zb_zcl_color_control_move_to_loop_t;

#define ZB_ZCL_COLOR_CONTROL_FILL_MOVE_TO_DATA(buf, cmd_info_,                              \
            attr_id_, start_, finish_, incr_direction_, transition_time_)                   \
{                                                                                           \
  zb_zcl_color_control_move_to_loop_t *loop_data =                                          \
          ZB_BUF_GET_PARAM((buf), zb_zcl_color_control_move_to_loop_t);                     \
    ZB_MEMMOVE(&(loop_data->cmd_info), (cmd_info_), sizeof(zb_zcl_parsed_hdr_t));           \
    loop_data->attr_id = (attr_id_);                                                        \
    loop_data->start_value = start_;                                                        \
    loop_data->finish_value = finish_;                                                      \
    loop_data->attr_id2 = 0;                                                                \
    loop_data->incr_direction = (zb_bool_t)(incr_direction_);                               \
    loop_data->start_time = ZB_TIMER_GET();                                                 \
    loop_data->transition_time = transition_time_;                                          \
}

/*! @internal
 *  @brief Structure for loop of step command */
typedef struct zb_zcl_color_control_step_loop_s
{
  /** Command header */
  zb_zcl_parsed_hdr_t cmd_info;

  /** Attribute_ID */
  zb_uint16_t attr_id;
  /** Rate value */
  zb_uint16_t rate;
  /** @brief limit */
  zb_uint16_t limit;

  /** last time - beacon interval */
  zb_time_t last_time;
  /** time bitween two steps - beacon interval */
  zb_time_t step_time;

  /** Attribute_ID second */
  zb_uint16_t attr_id2;
  /** Rate value second */
  zb_uint16_t rate2;
  /** @brief limit2 */
  zb_uint16_t limit2;

} zb_zcl_color_control_step_loop_t;

#define ZB_ZCL_COLOR_CONTROL_FILL_STEP_DATA(buf, cmd_info_,  attr_id_, rate_,               \
                                                       transition_time, limit_)             \
{                                                                                           \
  zb_zcl_color_control_step_loop_t *loop_data =                                             \
          ZB_BUF_GET_PARAM((buf), zb_zcl_color_control_step_loop_t);                        \
    ZB_MEMMOVE(&(loop_data->cmd_info), (cmd_info_), sizeof(zb_zcl_parsed_hdr_t));           \
    loop_data->attr_id = (attr_id_);                                                        \
    loop_data->attr_id2 = 0;                                                                \
    loop_data->rate = (rate_);                                                              \
    loop_data->last_time = ZB_TIMER_GET();                                                  \
    loop_data->step_time = ZB_TIME_UNITS_TO_SYS_TIMER_INTERVAL(transition_time);               \
    loop_data->limit = (limit_);                                                            \
}

/*! Number of attributes mandatory for reporting in Color Control cluster
 * Attribute: CurrentX, CurrentY, CurrentHue, CurrentSaturation, ColorTemperature
 *  */
#define ZB_ZCL_COLOR_CONTROL_REPORT_ATTR_COUNT 5

/*! @} */ /* Color Control cluster internals */
/*! @}
 *  @endcond */ /* internals_doc */

/*! @name Color Control cluster commands
    @{
*/

/************************** Color Control cluster command definitions ****************************/

/*! @brief Color control cluster command identifiers
    @see ZCL spec, subclause 5.2.2.3
*/
enum zb_zcl_color_control_cmd_e
{
  /** Move To Hue command */
  ZB_ZCL_CMD_COLOR_CONTROL_MOVE_TO_HUE                      = 0x00,
  /** Move Hue command */
  ZB_ZCL_CMD_COLOR_CONTROL_MOVE_HUE                         = 0x01,
  /** Step Hue command */
  ZB_ZCL_CMD_COLOR_CONTROL_STEP_HUE                         = 0x02,
  /** Move To Saturation command */
  ZB_ZCL_CMD_COLOR_CONTROL_MOVE_TO_SATURATION               = 0x03,
  /** Move Saturation command */
  ZB_ZCL_CMD_COLOR_CONTROL_MOVE_SATURATION                  = 0x04,
  /** Step Saturation command */
  ZB_ZCL_CMD_COLOR_CONTROL_STEP_SATURATION                  = 0x05,
  /** Move To Hue and Saturation command */
  ZB_ZCL_CMD_COLOR_CONTROL_MOVE_TO_HUE_SATURATION           = 0x06,
  /** Move To Color command */
  ZB_ZCL_CMD_COLOR_CONTROL_MOVE_TO_COLOR                    = 0x07,
  /** Move Color command */
  ZB_ZCL_CMD_COLOR_CONTROL_MOVE_COLOR                       = 0x08,
  /** Step Color command */
  ZB_ZCL_CMD_COLOR_CONTROL_STEP_COLOR                       = 0x09,
  /** Move to color temperature command */
  ZB_ZCL_CMD_COLOR_CONTROL_MOVE_TO_COLOR_TEMPERATURE        = 0x0a,
  /** Enhanced move to hue command */
  ZB_ZCL_CMD_COLOR_CONTROL_ENHANCED_MOVE_TO_HUE             = 0x40,
  /** Enhanced move hue command */
  ZB_ZCL_CMD_COLOR_CONTROL_ENHANCED_MOVE_HUE                = 0x41,
  /** Enhanced step hue command */
  ZB_ZCL_CMD_COLOR_CONTROL_ENHANCED_STEP_HUE                = 0x42,
  /** Enhanced move to hue and saturation command */
  ZB_ZCL_CMD_COLOR_CONTROL_ENHANCED_MOVE_TO_HUE_SATURATION  = 0x43,
  /** Color loop set command */
  ZB_ZCL_CMD_COLOR_CONTROL_COLOR_LOOP_SET                   = 0x44,
  /** Stop move step command */
  ZB_ZCL_CMD_COLOR_CONTROL_STOP_MOVE_STEP                   = 0x47,
  /** Move color temperature command */
  ZB_ZCL_CMD_COLOR_CONTROL_MOVE_COLOR_TEMPERATURE           = 0x4b,
  /** Step color temperature command */
  ZB_ZCL_CMD_COLOR_CONTROL_STEP_COLOR_TEMPERATURE           = 0x4c,
};

/** @cond internals_doc */
/* Color control cluster commands list : only for information - do not modify */
#define ZB_ZCL_CLUSTER_ID_COLOR_CONTROL_SERVER_ROLE_GENERATED_CMD_LIST

#define ZB_ZCL_CLUSTER_ID_COLOR_CONTROL_CLIENT_ROLE_RECEIVED_CMD_LIST ZB_ZCL_CLUSTER_ID_COLOR_CONTROL_SERVER_ROLE_GENERATED_CMD_LIST

#define ZB_ZCL_CLUSTER_ID_COLOR_CONTROL_CLIENT_ROLE_GENERATED_CMD_LIST                              \
                                          ZB_ZCL_CMD_COLOR_CONTROL_MOVE_TO_HUE,                     \
                                          ZB_ZCL_CMD_COLOR_CONTROL_MOVE_HUE,                        \
                                          ZB_ZCL_CMD_COLOR_CONTROL_STEP_HUE,                        \
                                          ZB_ZCL_CMD_COLOR_CONTROL_MOVE_TO_SATURATION,              \
                                          ZB_ZCL_CMD_COLOR_CONTROL_MOVE_SATURATION,                 \
                                          ZB_ZCL_CMD_COLOR_CONTROL_STEP_SATURATION,                 \
                                          ZB_ZCL_CMD_COLOR_CONTROL_MOVE_TO_HUE_SATURATION,          \
                                          ZB_ZCL_CMD_COLOR_CONTROL_MOVE_TO_COLOR,                   \
                                          ZB_ZCL_CMD_COLOR_CONTROL_MOVE_COLOR,                      \
                                          ZB_ZCL_CMD_COLOR_CONTROL_STEP_COLOR,                      \
                                          ZB_ZCL_CMD_COLOR_CONTROL_MOVE_TO_COLOR_TEMPERATURE,       \
                                          ZB_ZCL_CMD_COLOR_CONTROL_ENHANCED_MOVE_TO_HUE,            \
                                          ZB_ZCL_CMD_COLOR_CONTROL_ENHANCED_MOVE_HUE,               \
                                          ZB_ZCL_CMD_COLOR_CONTROL_ENHANCED_STEP_HUE,               \
                                          ZB_ZCL_CMD_COLOR_CONTROL_ENHANCED_MOVE_TO_HUE_SATURATION, \
                                          ZB_ZCL_CMD_COLOR_CONTROL_COLOR_LOOP_SET,                  \
                                          ZB_ZCL_CMD_COLOR_CONTROL_STOP_MOVE_STEP,                  \
                                          ZB_ZCL_CMD_COLOR_CONTROL_MOVE_COLOR_TEMPERATURE,          \
                                          ZB_ZCL_CMD_COLOR_CONTROL_STEP_COLOR_TEMPERATURE

#define ZB_ZCL_CLUSTER_ID_COLOR_CONTROL_SERVER_ROLE_RECEIVED_CMD_LIST ZB_ZCL_CLUSTER_ID_COLOR_CONTROL_CLIENT_ROLE_GENERATED_CMD_LIST
/*! @}
 *  @endcond */ /* internals_doc */


/* command parameters enumerates */

/** @brief Direction of Move Hue command.
 *  See specs 5.2.2.3.3.2, 5.2.2.3.6.2 */
enum zb_zcl_color_control_move_direction_e
{
  /** Stop */
  ZB_ZCL_CMD_COLOR_CONTROL_MOVE_STOP    = 0x00,
  /** Up */
  ZB_ZCL_CMD_COLOR_CONTROL_MOVE_UP      = 0x01,
  /** Down */
  ZB_ZCL_CMD_COLOR_CONTROL_MOVE_DOWN     = 0x03
};

/** @brief Direction of Step command.
 *  See specs 5.2.2.3.4.2, 5.2.2.3.7.2 */
enum zb_zcl_color_control_step_mode_e
{
  /** Up */
  ZB_ZCL_CMD_COLOR_CONTROL_STEP_UP       = 0x01,
  /** Down */
  ZB_ZCL_CMD_COLOR_CONTROL_STEP_DOWN     = 0x03
};


/* command request structure */

/*! @brief Structured representation of Level Control command payload (optional part)
    @see ZCL spec, subclause 3.10.2.3.1.1*/
typedef ZB_PACKED_PRE struct zb_zcl_color_control_req_options_s
{
  /** OptionsMask field */
  zb_uint8_t options_mask;
  /** OptionsOverride field */
  zb_uint8_t options_override;
} ZB_PACKED_STRUCT zb_zcl_color_control_req_options_t;

/** @brief Move to Level payload length macro */
#define ZB_ZCL_COLOR_CONTROL_REQ_OPTIONS_PAYLOAD_LEN \
  sizeof(zb_zcl_color_control_req_options_t)

/** @internal Macro for getting Move to Color command */
#define ZB_ZCL_COLOR_CONTROL_GET_CMD_OPTIONS(data_buf, req_options)         \
{                                                                           \
  zb_zcl_color_control_req_options_t *req_options_ptr;                      \
  (req_options_ptr) = zb_buf_len(data_buf) >=                               \
    ZB_ZCL_COLOR_CONTROL_REQ_OPTIONS_PAYLOAD_LEN ?                          \
    (zb_zcl_color_control_req_options_t*)zb_buf_begin(data_buf) : NULL;     \
  if (req_options_ptr != NULL)                                              \
  {                                                                         \
    req_options.options_mask = req_options_ptr->options_mask;               \
    req_options.options_override = req_options_ptr->options_override;       \
    (void)zb_buf_cut_left(data_buf, sizeof(zb_zcl_color_control_req_options_t)); \
  }                                                                         \
  else                                                                      \
  {                                                                         \
    req_options.options_mask = (zb_uint8_t)0x00;                            \
    req_options.options_override = (zb_uint8_t)0x00;                        \
  }                                                                         \
}

/******************************* MOVE_TO_HUE ******************************/

/** @brief Direction of Move to Hue command.
 *  See ZCL8 spec 5.2.2.3.4.3 */
enum zb_zcl_color_control_move_to_hue_direction_e
{
  /** Shortest distance */
  ZB_ZCL_CMD_COLOR_CONTROL_MOVE_TO_HUE_SHORTEST = 0x00,
  /** Longest distance */
  ZB_ZCL_CMD_COLOR_CONTROL_MOVE_TO_HUE_LONGEST  = 0x01,
  /** Up */
  ZB_ZCL_CMD_COLOR_CONTROL_MOVE_TO_HUE_UP       = 0x02,
  /** Down */
  ZB_ZCL_CMD_COLOR_CONTROL_MOVE_TO_HUE_DOWN     = 0x03
};

/*! @brief Structure representation of Move To Hue command payload
    @see ZCL8 spec, subclause 5.2.2.3.4.1 */
typedef ZB_PACKED_PRE struct zb_zcl_color_control_move_to_hue_req_s
{
  /** Hue */
  zb_uint8_t hue;
  /** Direction, see @ref zb_zcl_color_control_move_to_hue_direction_e */
  zb_uint8_t direction;
  /** Transition time field */
  zb_uint16_t transition_time;
} ZB_PACKED_STRUCT zb_zcl_color_control_move_to_hue_req_t;

/** @brief Move Hue payload length macro */
#define ZB_ZCL_COLOR_CONTROL_MOVE_TO_HUE_REQ_PAYLOAD_LEN \
  sizeof(zb_zcl_color_control_move_to_hue_req_t)

/*! @brief Send Move to Hue command
    @param buffer - to put packet to
    @param dst_addr - address to send packet to
    @param dst_addr_mode - addressing mode
    @param dst_ep - destination endpoint
    @param ep - sending endpoint
    @param prof_id - profile identifier
    @param def_resp - enable/disable default response
    @param cb - callback for getting command send status
    @param hue - Hue value
    @param direction - Direction value, see @ref zb_zcl_color_control_move_to_hue_direction_e
    @param transition_time - Transition Time value
    @param options_mask - Options Mask
    @param options_override - Options Override
*/
void zb_zcl_color_control_send_move_to_hue_req_zcl8(zb_bufid_t buffer, const zb_addr_u *dst_addr,
                                                    zb_uint8_t dst_addr_mode, zb_uint8_t dst_ep,
                                                    zb_uint8_t ep, zb_uint16_t prof_id,
                                                    zb_uint8_t def_resp, zb_callback_t cb,
                                                    zb_uint8_t hue, zb_uint8_t direction,
                                                    zb_uint16_t transition_time,
                                                    zb_uint8_t options_mask, zb_uint8_t options_override);

/*! @brief Send Move to Hue command (pre-ZCL8)
    Use @ref zb_zcl_color_control_send_move_to_hue_req_zcl8 for ZCL8 revision call.
    @param buffer - to put packet to
    @param dst_addr - address to send packet to
    @param dst_addr_mode - addressing mode
    @param dst_ep - destination endpoint
    @param ep - sending endpoint
    @param prof_id - profile identifier
    @param def_resp - enable/disable default response
    @param cb - callback for getting command send status
    @param hue - Hue value
    @param direction - Direction value, see @ref zb_zcl_color_control_move_to_hue_direction_e
    @param transition_time - Transition Time value
*/
void zb_zcl_color_control_send_move_to_hue_req(zb_bufid_t buffer, const zb_addr_u *dst_addr,
                                               zb_uint8_t dst_addr_mode, zb_uint8_t dst_ep,
                                               zb_uint8_t ep, zb_uint16_t prof_id,
                                               zb_uint8_t def_resp, zb_callback_t cb,
                                               zb_uint8_t hue, zb_uint8_t direction,
                                               zb_uint16_t transition_time);

/** Macro for calling @ref zb_zcl_color_control_send_move_to_hue_req_zcl8 function
 */
#define ZB_ZCL_COLOR_CONTROL_SEND_MOVE_TO_HUE_REQ_ZCL8(buffer, addr, dst_addr_mode,                       \
                                                       dst_ep, ep, prfl_id, def_resp, cb, hue, direction, \
                                                       transition_time, options_mask, options_override)   \
{                                                                                                         \
  zb_zcl_color_control_send_move_to_hue_req_zcl8(buffer, ZB_ADDR_U_CAST(addr), dst_addr_mode,             \
                                                 dst_ep, ep, prfl_id, def_resp, cb, hue, direction,       \
                                                 transition_time, options_mask, options_override);        \
}

/** Macro for calling @ref zb_zcl_color_control_send_move_to_hue_req function
 */
#define ZB_ZCL_COLOR_CONTROL_SEND_MOVE_TO_HUE_REQ(buffer, addr, dst_addr_mode,                               \
                                                  dst_ep, ep, prfl_id, def_resp, cb, hue, direction,         \
                                                  transition_time)                                           \
{                                                                                                            \
  zb_zcl_color_control_send_move_to_hue_req(buffer, ZB_ADDR_U_CAST(addr), dst_addr_mode,                     \
                                            dst_ep, ep, prfl_id, def_resp, cb, hue, direction,               \
                                            transition_time);                                                \
}

/** @brief Macro for getting Move to Hue command
  * @attention Assumes that ZCL header already cut.
  * @param buffer containing the packet (by pointer).
  * @param move_to_hue_req - pointer to a variable of type @ref
  * zb_zcl_color_control_move_to_hue_req_s.
  * @param status - variable to put parse status to (see @ref zb_zcl_parse_status_t).
  */
#define ZB_ZCL_COLOR_CONTROL_GET_MOVE_TO_HUE_REQ(buffer, move_to_hue_req, status)            \
{                                                                                            \
  zb_zcl_color_control_move_to_hue_req_t *move_to_hue_req_ptr;                               \
  (move_to_hue_req_ptr) = zb_buf_len(buffer) >=                                              \
    ZB_ZCL_COLOR_CONTROL_MOVE_TO_HUE_REQ_PAYLOAD_LEN ?                                       \
    (zb_zcl_color_control_move_to_hue_req_t*)zb_buf_begin(buffer) : NULL;                    \
  if (move_to_hue_req_ptr != NULL)                                                           \
  {                                                                                          \
    ZB_MEMCPY(&(move_to_hue_req), move_to_hue_req_ptr, ZB_ZCL_COLOR_CONTROL_MOVE_TO_HUE_REQ_PAYLOAD_LEN); \
    ZB_HTOLE16_ONPLACE((move_to_hue_req).transition_time);                                   \
    (void)zb_buf_cut_left(buffer, ZB_ZCL_COLOR_CONTROL_MOVE_TO_HUE_REQ_PAYLOAD_LEN);         \
    status = ZB_ZCL_PARSE_STATUS_SUCCESS;                                                    \
  }                                                                                          \
  else                                                                                       \
  {                                                                                          \
    status = ZB_ZCL_PARSE_STATUS_FAILURE;                                                    \
  }                                                                                          \
}
/******************************* MOVE_HUE ******************************/

/*! @brief Structure representation of Move Hue command payload
    @see ZCL8 spec, subclause 5.2.2.3.5.1 */
typedef ZB_PACKED_PRE struct zb_zcl_color_control_move_hue_req_s
{
  /** Move mode, see @ref zb_zcl_color_control_move_direction_e */
  zb_uint8_t move_mode;
  /** Rate */
  zb_uint8_t rate;
} ZB_PACKED_STRUCT zb_zcl_color_control_move_hue_req_t;

/** @brief Move Hue payload length macro */
#define ZB_ZCL_COLOR_CONTROL_MOVE_HUE_REQ_PAYLOAD_LEN \
  sizeof(zb_zcl_color_control_move_hue_req_t)

/*! @brief Send Move Hue command
    @param buffer - to put packet to
    @param dst_addr - address to send packet to
    @param dst_addr_mode - addressing mode
    @param dst_ep - destination endpoint
    @param ep - sending endpoint
    @param prof_id - profile identifier
    @param def_resp - enable/disable default response
    @param cb - callback for getting command send status
    @param move_mode - Move mode, see @ref zb_zcl_color_control_move_direction_e
    @param rate - Rate
    @param options_mask - Options Mask
    @param options_override - Options Override
*/
void zb_zcl_color_control_send_move_hue_req_zcl8(zb_bufid_t buffer, const zb_addr_u *dst_addr,
                                                 zb_uint8_t dst_addr_mode, zb_uint8_t dst_ep,
                                                 zb_uint8_t ep, zb_uint16_t prof_id,
                                                 zb_uint8_t def_resp, zb_callback_t cb,
                                                 zb_uint8_t move_mode, zb_uint8_t rate,
                                                 zb_uint8_t options_mask, zb_uint8_t options_override);

/*! @brief Send Move Hue command (pre-ZCL8)
    Use @ref zb_zcl_color_control_send_move_hue_req_zcl8 for ZCL8 revision call.
    @param buffer - to put packet to
    @param dst_addr - address to send packet to
    @param dst_addr_mode - addressing mode
    @param dst_ep - destination endpoint
    @param ep - sending endpoint
    @param prof_id - profile identifier
    @param def_resp - enable/disable default response
    @param cb - callback for getting command send status
    @param move_mode - Move mode, see @ref zb_zcl_color_control_move_direction_e
    @param rate - Rate
*/
void zb_zcl_color_control_send_move_hue_req(zb_bufid_t buffer, const zb_addr_u *dst_addr,
                                            zb_uint8_t dst_addr_mode, zb_uint8_t dst_ep,
                                            zb_uint8_t ep, zb_uint16_t prof_id,
                                            zb_uint8_t def_resp, zb_callback_t cb,
                                            zb_uint8_t move_mode, zb_uint8_t rate);

/** Macro for calling @ref zb_zcl_color_control_send_move_hue_req_zcl8 function
 */
#define ZB_ZCL_COLOR_CONTROL_SEND_MOVE_HUE_REQ_ZCL8(buffer, addr, dst_addr_mode,                    \
                                                    dst_ep, ep, prfl_id, def_resp, cb, move_mode,   \
                                                    rate, options_mask, options_override)           \
{                                                                                                   \
  zb_zcl_color_control_send_move_hue_req_zcl8(buffer, ZB_ADDR_U_CAST(addr), dst_addr_mode,          \
                                              dst_ep, ep, prfl_id, def_resp, cb, move_mode,         \
                                              rate, options_mask, options_override);                \
}

/** Macro for calling @ref zb_zcl_color_control_send_move_hue_req function
 */
#define ZB_ZCL_COLOR_CONTROL_SEND_MOVE_HUE_REQ(buffer, addr, dst_addr_mode,                   \
  dst_ep, ep, prfl_id, def_resp, cb, move_mode, rate)                                         \
{                                                                                             \
  zb_zcl_color_control_send_move_hue_req(buffer, ZB_ADDR_U_CAST(addr), dst_addr_mode,         \
                                         dst_ep, ep, prfl_id, def_resp, cb, move_mode, rate); \
}

/** @brief Macro for getting Move Hue command
  * @attention Assumes that ZCL header already cut.
  * @param buffer containing the packet (by pointer).
  * @param move_hue_req - variable of type @ref
  * zb_zcl_color_control_move_hue_req_s.
  * @param status - variable to put parse status to (see @ref zb_zcl_parse_status_t).
  */
#define ZB_ZCL_COLOR_CONTROL_GET_MOVE_HUE_REQ(buffer, move_hue_req, status)         \
{                                                                                   \
  zb_zcl_color_control_move_hue_req_t *move_hue_req_ptr;                            \
  (move_hue_req_ptr) = zb_buf_len(buffer) >=                                        \
    ZB_ZCL_COLOR_CONTROL_MOVE_HUE_REQ_PAYLOAD_LEN ?                                 \
    (zb_zcl_color_control_move_hue_req_t*)zb_buf_begin(buffer) : NULL;              \
  if (move_hue_req_ptr != NULL)                                                     \
  {                                                                                 \
    move_hue_req.move_mode = move_hue_req_ptr->move_mode;                           \
    move_hue_req.rate = move_hue_req_ptr->rate;                                     \
    (void)zb_buf_cut_left(buffer, ZB_ZCL_COLOR_CONTROL_MOVE_HUE_REQ_PAYLOAD_LEN);   \
    status = ZB_ZCL_PARSE_STATUS_SUCCESS;                                           \
  }                                                                                 \
  else                                                                              \
  {                                                                                 \
    status = ZB_ZCL_PARSE_STATUS_FAILURE;                                           \
  }                                                                                 \
}

/******************************* STEP_HUE ******************************/

/*! @brief Structure representation of Step Hue command payload
    @see ZCL8 spec, subclause 5.2.2.3.6.1 */
typedef ZB_PACKED_PRE struct zb_zcl_color_control_step_hue_req_s
{
  /** Step mode, see @ref zb_zcl_color_control_step_mode_e */
  zb_uint8_t step_mode;
  /** Step size */
  zb_uint8_t step_size;
  /** Transition time field */
  zb_uint8_t transition_time;
} ZB_PACKED_STRUCT zb_zcl_color_control_step_hue_req_t;

/** @brief Step Hue payload length macro */
#define ZB_ZCL_COLOR_CONTROL_STEP_HUE_REQ_PAYLOAD_LEN \
  sizeof(zb_zcl_color_control_step_hue_req_t)

/*! @brief Send Step Hue command
    @param buffer - to put packet to
    @param dst_addr - address to send packet to
    @param dst_addr_mode - addressing mode
    @param dst_ep - destination endpoint
    @param ep - sending endpoint
    @param prof_id - profile identifier
    @param def_resp - enable/disable default response
    @param cb - callback for getting command send status
    @param step_mode - step mode value, see @ref zb_zcl_color_control_step_mode_e
    @param step_size - step size value
    @param transition_time - Transition Time value
    @param options_mask - Options Mask
    @param options_override - Options Override
*/
void zb_zcl_color_control_send_step_hue_req_zcl8(zb_bufid_t buffer, const zb_addr_u *dst_addr,
                                                 zb_uint8_t dst_addr_mode, zb_uint8_t dst_ep,
                                                 zb_uint8_t ep, zb_uint16_t prof_id,
                                                 zb_uint8_t def_resp, zb_callback_t cb,
                                                 zb_uint8_t step_mode, zb_uint8_t step_size,
                                                 zb_uint8_t transition_time,
                                                 zb_uint8_t options_mask, zb_uint8_t options_override);

/*! @brief Send Step Hue command (pre-ZCL8)
    Use @ref zb_zcl_color_control_send_step_hue_req_zcl8 for ZCL8 revision call.
    @param buffer - to put packet to
    @param dst_addr - address to send packet to
    @param dst_addr_mode - addressing mode
    @param dst_ep - destination endpoint
    @param ep - sending endpoint
    @param prof_id - profile identifier
    @param def_resp - enable/disable default response
    @param cb - callback for getting command send status
    @param step_mode - step mode value, see @ref zb_zcl_color_control_step_mode_e
    @param step_size - step size value
    @param transition_time - Transition Time value
*/
void zb_zcl_color_control_send_step_hue_req(zb_bufid_t buffer, const zb_addr_u *dst_addr,
                                            zb_uint8_t dst_addr_mode, zb_uint8_t dst_ep,
                                            zb_uint8_t ep, zb_uint16_t prof_id,
                                            zb_uint8_t def_resp, zb_callback_t cb,
                                            zb_uint8_t step_mode, zb_uint8_t step_size,
                                            zb_uint8_t transition_time);

/** Macro for calling @ref zb_zcl_color_control_send_step_hue_req_zcl8 function
 */
#define ZB_ZCL_COLOR_CONTROL_SEND_STEP_HUE_REQ_ZCL8(buffer, addr, dst_addr_mode,                  \
                                                    dst_ep, ep, prfl_id, def_resp, cb, step_mode, \
                                                    step_size, transition_time, options_mask,     \
                                                    options_override)                             \
{                                                                                                 \
  zb_zcl_color_control_send_step_hue_req_zcl8(buffer, ZB_ADDR_U_CAST(addr), dst_addr_mode,        \
                                             dst_ep, ep, prfl_id, def_resp, cb, step_mode,        \
                                             step_size, transition_time, options_mask,            \
                                             options_override);                                   \
}

/** Macro for calling @ref zb_zcl_color_control_send_step_hue_req function
 */
#define ZB_ZCL_COLOR_CONTROL_SEND_STEP_HUE_REQ(buffer, addr, dst_addr_mode,                                         \
  dst_ep, ep, prfl_id, def_resp, cb, step_mode, step_size, transition_time)                                         \
{                                                                                                                   \
  zb_zcl_color_control_send_step_hue_req(buffer, ZB_ADDR_U_CAST(addr), dst_addr_mode,                               \
                                         dst_ep, ep, prfl_id, def_resp, cb, step_mode, step_size, transition_time); \
}

/** @brief Macro for getting Step Hue command
  * @attention Assumes that ZCL header already cut.
  * @param buffer containing the packet (by pointer).
  * @param step_hue_req - pointer to a variable of type @ref
  * zb_zcl_color_control_step_hue_req_s.
  * @param status - variable to put parse status to (see @ref zb_zcl_parse_status_t).
  */
#define ZB_ZCL_COLOR_CONTROL_GET_STEP_HUE_REQ(buffer, step_hue_req, status)            \
{                                                                                      \
  zb_zcl_color_control_step_hue_req_t *step_hue_req_ptr;                               \
  (step_hue_req_ptr) = zb_buf_len(buffer) >=                                           \
    ZB_ZCL_COLOR_CONTROL_STEP_HUE_REQ_PAYLOAD_LEN ?                                    \
    (zb_zcl_color_control_step_hue_req_t*)zb_buf_begin(buffer) : NULL;                 \
  if (step_hue_req_ptr != NULL)                                                        \
  {                                                                                    \
    step_hue_req.step_mode = step_hue_req_ptr->step_mode;                              \
    step_hue_req.step_size = step_hue_req_ptr->step_size;                              \
    step_hue_req.transition_time = step_hue_req_ptr->transition_time;                  \
    (void)zb_buf_cut_left(buffer, ZB_ZCL_COLOR_CONTROL_STEP_HUE_REQ_PAYLOAD_LEN);      \
    status = ZB_ZCL_PARSE_STATUS_SUCCESS;                                              \
  }                                                                                    \
  else                                                                                 \
  {                                                                                    \
    status = ZB_ZCL_PARSE_STATUS_FAILURE;                                              \
  }                                                                                    \
}

/******************************* Move to Saturation ******************************/

/*! @brief Structure representation of Move To Saturation command payload
    @see ZCL8 spec, subclause 5.2.2.3.7.1 */
typedef ZB_PACKED_PRE struct zb_zcl_color_control_move_to_saturation_req_s
{
  /** Saturation */
  zb_uint8_t saturation;
  /** Transition time field */
  zb_uint16_t transition_time;
} ZB_PACKED_STRUCT zb_zcl_color_control_move_to_saturation_req_t;

/** @brief Move To Saturation payload length macro */
#define ZB_ZCL_COLOR_CONTROL_MOVE_TO_SATURATION_REQ_PAYLOAD_LEN \
  sizeof(zb_zcl_color_control_move_to_saturation_req_t)

/*! @brief Send Move to Saturation command
    @param buffer - to put packet to
    @param dst_addr - address to send packet to
    @param dst_addr_mode - addressing mode
    @param dst_ep - destination endpoint
    @param ep - sending endpoint
    @param prof_id - profile identifier
    @param def_resp - enable/disable default response
    @param cb - callback for getting command send status
    @param saturation - saturation
    @param transition_time - Transition Time value
    @param options_mask - Options Mask
    @param options_override - Options Override
*/
void zb_zcl_color_control_send_move_to_saturation_req_zcl8(zb_bufid_t buffer, const zb_addr_u *dst_addr,
                                                           zb_uint8_t dst_addr_mode, zb_uint8_t dst_ep,
                                                           zb_uint8_t ep, zb_uint16_t prof_id,
                                                           zb_uint8_t def_resp, zb_callback_t cb,
                                                           zb_uint8_t saturation,
                                                           zb_uint16_t transition_time,
                                                           zb_uint8_t options_mask, zb_uint8_t options_override);

/*! @brief Send Move to Saturation command (pre-ZCL8)
    Use @ref zb_zcl_color_control_send_move_to_saturation_req_zcl8 for ZCL8 revision call.
    @param buffer - to put packet to
    @param dst_addr - address to send packet to
    @param dst_addr_mode - addressing mode
    @param dst_ep - destination endpoint
    @param ep - sending endpoint
    @param prof_id - profile identifier
    @param def_resp - enable/disable default response
    @param cb - callback for getting command send status
    @param saturation - saturation
    @param transition_time - Transition Time value
*/
void zb_zcl_color_control_send_move_to_saturation_req(zb_bufid_t buffer, const zb_addr_u *dst_addr,
                                                      zb_uint8_t dst_addr_mode, zb_uint8_t dst_ep,
                                                      zb_uint8_t ep, zb_uint16_t prof_id,
                                                      zb_uint8_t def_resp, zb_callback_t cb,
                                                      zb_uint8_t saturation,
                                                      zb_uint16_t transition_time);

/** Macro for calling @ref zb_zcl_color_control_send_move_to_saturation_req_zcl8 function
 */
#define ZB_ZCL_COLOR_CONTROL_SEND_MOVE_TO_SATURATION_REQ_ZCL8(buffer, addr, dst_addr_mode,               \
                                                              dst_ep, ep, prfl_id, def_resp, cb,         \
                                                              saturation, transition_time, options_mask, \
                                                              options_override)                          \
{                                                                                                        \
  zb_zcl_color_control_send_move_to_saturation_req_zcl8(buffer, ZB_ADDR_U_CAST(addr), dst_addr_mode,     \
                                                        dst_ep, ep, prfl_id, def_resp, cb,               \
                                                        saturation, transition_time, options_mask,       \
                                                        options_override);                               \
}

/** Macro for calling @ref zb_zcl_color_control_send_move_to_saturation_req function
 */
#define ZB_ZCL_COLOR_CONTROL_SEND_MOVE_TO_SATURATION_REQ(buffer, addr, dst_addr_mode,           \
  dst_ep, ep, prfl_id, def_resp, cb, saturation, transition_time)                               \
{                                                                                               \
  zb_zcl_color_control_send_move_to_saturation_req(buffer, ZB_ADDR_U_CAST(addr), dst_addr_mode, \
  dst_ep, ep, prfl_id, def_resp, cb, saturation, transition_time);                              \
}

/** @brief Macro for getting Move To Saturation command
  * @attention Assumes that ZCL header already cut.
  * @param buffer containing the packet (by pointer).
  * @param move_to_saturation_req - variable of type @ref
  * zb_zcl_color_control_move_to_saturation_req_s.
  * @param status - variable to put parse status to (see @ref zb_zcl_parse_status_t).
  */
#define ZB_ZCL_COLOR_CONTROL_GET_MOVE_TO_SATURATION_REQ(buffer, move_to_saturation_req, status)            \
{                                                                                                          \
  zb_zcl_color_control_move_to_saturation_req_t *move_to_saturation_req_ptr;                               \
  (move_to_saturation_req_ptr) = zb_buf_len(buffer) >=                                                     \
    ZB_ZCL_COLOR_CONTROL_MOVE_TO_SATURATION_REQ_PAYLOAD_LEN ?                                              \
    (zb_zcl_color_control_move_to_saturation_req_t*)zb_buf_begin(buffer) : NULL;                           \
  if (move_to_saturation_req_ptr != NULL)                                                                  \
  {                                                                                                        \
    move_to_saturation_req.saturation = move_to_saturation_req_ptr->saturation;                            \
    ZB_HTOLE16(&(move_to_saturation_req).transition_time, &(move_to_saturation_req_ptr->transition_time)); \
    (void)zb_buf_cut_left(buffer, ZB_ZCL_COLOR_CONTROL_MOVE_TO_SATURATION_REQ_PAYLOAD_LEN);                \
    status = ZB_ZCL_PARSE_STATUS_SUCCESS;                                                                  \
  }                                                                                                        \
  else                                                                                                     \
  {                                                                                                        \
    status = ZB_ZCL_PARSE_STATUS_FAILURE;                                                                  \
  }                                                                                                        \
}
/******************************* Move Saturation ******************************/

/*! @brief Structure representation of Move Saturation command payload
    @see ZCL8 spec, subclause 5.2.2.3.8.1 */
typedef ZB_PACKED_PRE struct zb_zcl_color_control_move_saturation_req_s
{
  /** Move mode, see @ref zb_zcl_color_control_move_direction_e */
  zb_uint8_t move_mode;
  /** Rate */
  zb_uint8_t rate;
} ZB_PACKED_STRUCT zb_zcl_color_control_move_saturation_req_t;

/** @brief Move Saturation payload length macro */
#define ZB_ZCL_COLOR_CONTROL_MOVE_SATURATION_REQ_PAYLOAD_LEN \
  sizeof(zb_zcl_color_control_move_saturation_req_t)

/*! @brief Send Move Saturation command
    @param buffer - to put packet to
    @param dst_addr - address to send packet to
    @param dst_addr_mode - addressing mode
    @param dst_ep - destination endpoint
    @param ep - sending endpoint
    @param prof_id - profile identifier
    @param def_resp - enable/disable default response
    @param cb - callback for getting command send status
    @param move_mode - Move mode, see @ref zb_zcl_color_control_move_direction_e
    @param rate - Rate
    @param options_mask - Options Mask
    @param options_override - Options Override
*/
void zb_zcl_color_control_send_move_saturation_req_zcl8(zb_bufid_t buffer, const zb_addr_u *dst_addr,
                                                        zb_uint8_t dst_addr_mode, zb_uint8_t dst_ep,
                                                        zb_uint8_t ep, zb_uint16_t prof_id,
                                                        zb_uint8_t def_resp, zb_callback_t cb,
                                                        zb_uint8_t move_mode, zb_uint8_t rate,
                                                        zb_uint8_t options_mask, zb_uint8_t options_override);

/*! @brief Send Move Saturation command (pew-ZCL8)
    Use @ref zb_zcl_color_control_send_move_saturation_req_zcl8 for ZCL8 revision call.
    @param buffer - to put packet to
    @param dst_addr - address to send packet to
    @param dst_addr_mode - addressing mode
    @param dst_ep - destination endpoint
    @param ep - sending endpoint
    @param prof_id - profile identifier
    @param def_resp - enable/disable default response
    @param cb - callback for getting command send status
    @param move_mode - Move mode, see @ref zb_zcl_color_control_move_direction_e
    @param rate - Rate
*/
void zb_zcl_color_control_send_move_saturation_req(zb_bufid_t buffer, const zb_addr_u *dst_addr,
                                                   zb_uint8_t dst_addr_mode, zb_uint8_t dst_ep,
                                                   zb_uint8_t ep, zb_uint16_t prof_id,
                                                   zb_uint8_t def_resp, zb_callback_t cb,
                                                   zb_uint8_t move_mode, zb_uint8_t rate);

/** Macro for calling @ref zb_zcl_color_control_send_move_saturation_req_zcl8 function
 */
#define ZB_ZCL_COLOR_CONTROL_SEND_MOVE_SATURATION_REQ_ZCL8(buffer, addr, dst_addr_mode,                  \
                                                           dst_ep, ep, prfl_id, def_resp, cb, move_mode, \
                                                           rate, options_mask, options_override)         \
{                                                                                                        \
  zb_zcl_color_control_send_move_saturation_req_zcl8(buffer, ZB_ADDR_U_CAST(addr), dst_addr_mode,        \
                                                     dst_ep, ep, prfl_id, def_resp, cb, move_mode,       \
                                                     rate, options_mask, options_override);              \
}

/** Macro for calling @ref zb_zcl_color_control_send_move_saturation_req function
 */
#define ZB_ZCL_COLOR_CONTROL_SEND_MOVE_SATURATION_REQ(buffer, addr, dst_addr_mode,           \
  dst_ep, ep, prfl_id, def_resp, cb, move_mode, rate)                                        \
{                                                                                            \
  zb_zcl_color_control_send_move_saturation_req(buffer, ZB_ADDR_U_CAST(addr), dst_addr_mode, \
  dst_ep, ep, prfl_id, def_resp, cb, move_mode, rate);                                       \
}

/** @brief Macro for getting Move Saturation command
  * @attention Assumes that ZCL header already cut.
  * @param buffer containing the packet (by pointer).
  * @param move_saturation_req - variable of type @ref
  * zb_zcl_color_control_move_saturation_req_s.
  * @param status - variable to put parse status to (see @ref zb_zcl_parse_status_t).
  */
#define ZB_ZCL_COLOR_CONTROL_GET_MOVE_SATURATION_REQ(buffer, move_saturation_req, status) \
{                                                                                         \
  zb_zcl_color_control_move_saturation_req_t *move_saturation_req_ptr;                    \
  (move_saturation_req_ptr) = zb_buf_len(buffer) >=                                       \
    ZB_ZCL_COLOR_CONTROL_MOVE_SATURATION_REQ_PAYLOAD_LEN ?                                \
    (zb_zcl_color_control_move_saturation_req_t*)zb_buf_begin(buffer) : NULL;             \
  if (move_saturation_req_ptr != NULL)                                                    \
  {                                                                                       \
    move_saturation_req.move_mode = move_saturation_req_ptr->move_mode;                   \
    move_saturation_req.rate = move_saturation_req_ptr->rate;                             \
    (void)zb_buf_cut_left(buffer, ZB_ZCL_COLOR_CONTROL_MOVE_SATURATION_REQ_PAYLOAD_LEN);  \
    status = ZB_ZCL_PARSE_STATUS_SUCCESS;                                                 \
  }                                                                                       \
  else                                                                                    \
  {                                                                                       \
    status = ZB_ZCL_PARSE_STATUS_FAILURE;                                                 \
  }                                                                                       \
}

/******************************* STEP_SATURATION ******************************/

/*! @brief Structure representation of Step Saturation command payload
    @see ZCL8 spec, subclause 5.2.2.3.9.1 */
typedef ZB_PACKED_PRE struct zb_zcl_color_control_step_saturation_req_s
{
  /** Step mode, see @ref zb_zcl_color_control_step_mode_e */
  zb_uint8_t step_mode;
  /** Step size */
  zb_uint8_t step_size;
  /** Transition time field */
  zb_uint8_t transition_time;
} ZB_PACKED_STRUCT zb_zcl_color_control_step_saturation_req_t;

/** @brief Move Saturation payload length macro */
#define ZB_ZCL_COLOR_CONTROL_STEP_SATURATION_REQ_PAYLOAD_LEN \
  sizeof(zb_zcl_color_control_step_saturation_req_t)

/*! @brief Send Step Saturation command
    @param buffer - to put packet to
    @param dst_addr - address to send packet to
    @param dst_addr_mode - addressing mode
    @param dst_ep - destination endpoint
    @param ep - sending endpoint
    @param prof_id - profile identifier
    @param def_resp - enable/disable default response
    @param cb - callback for getting command send status
    @param step_mode - step mode value, see @ref zb_zcl_color_control_step_mode_e
    @param step_size - step size value
    @param transition_time - Transition Time value
    @param options_mask - Options Mask
    @param options_override - Options Override
*/
void zb_zcl_color_control_send_step_saturation_req_zcl8(zb_bufid_t buffer, const zb_addr_u *dst_addr,
                                                        zb_uint8_t dst_addr_mode, zb_uint8_t dst_ep,
                                                        zb_uint8_t ep, zb_uint16_t prof_id,
                                                        zb_uint8_t def_resp, zb_callback_t cb,
                                                        zb_uint8_t step_mode, zb_uint8_t step_size,
                                                        zb_uint8_t transition_time,
                                                        zb_uint8_t options_mask, zb_uint8_t options_override);

/*! @brief Send Step Saturation command (pre-ZCL8)
    Use @ref zb_zcl_color_control_send_step_saturation_req_zcl8 for ZCL8 revision call.
    @param buffer - to put packet to
    @param dst_addr - address to send packet to
    @param dst_addr_mode - addressing mode
    @param dst_ep - destination endpoint
    @param ep - sending endpoint
    @param prof_id - profile identifier
    @param def_resp - enable/disable default response
    @param cb - callback for getting command send status
    @param step_mode - step mode value, see @ref zb_zcl_color_control_step_mode_e
    @param step_size - step size value
    @param transition_time - Transition Time value
*/
void zb_zcl_color_control_send_step_saturation_req(zb_bufid_t buffer, const zb_addr_u *dst_addr,
                                                   zb_uint8_t dst_addr_mode, zb_uint8_t dst_ep,
                                                   zb_uint8_t ep, zb_uint16_t prof_id,
                                                   zb_uint8_t def_resp, zb_callback_t cb,
                                                   zb_uint8_t step_mode, zb_uint8_t step_size,
                                                   zb_uint8_t transition_time);

/** Macro for calling @ref zb_zcl_color_control_send_step_saturation_req_zcl8 function
 */
#define ZB_ZCL_COLOR_CONTROL_SEND_STEP_SATURATION_REQ_ZCL8(buffer, addr, dst_addr_mode,                  \
                                                           dst_ep, ep, prfl_id, def_resp, cb, step_mode, \
                                                           step_size, transition_time, options_mask,     \
                                                           options_override)                             \
{                                                                                                        \
  zb_zcl_color_control_send_step_saturation_req_zcl8(buffer, ZB_ADDR_U_CAST(addr), dst_addr_mode,        \
                                                     dst_ep, ep, prfl_id, def_resp, cb, step_mode,       \
                                                     step_size, transition_time, options_mask,           \
                                                     options_override);                                  \
}

/** Macro for calling @ref zb_zcl_color_control_send_step_saturation_req function
 */
#define ZB_ZCL_COLOR_CONTROL_SEND_STEP_SATURATION_REQ(buffer, addr, dst_addr_mode,                                         \
  dst_ep, ep, prfl_id, def_resp, cb, step_mode, step_size, transition_time)                                                \
{                                                                                                                          \
  zb_zcl_color_control_send_step_saturation_req(buffer, ZB_ADDR_U_CAST(addr), dst_addr_mode,                               \
                                                dst_ep, ep, prfl_id, def_resp, cb, step_mode, step_size, transition_time); \
}

/** @brief Macro for getting Step Saturation command
  * @attention Assumes that ZCL header already cut.
  * @param buffer containing the packet (by pointer).
  * @param step_saturation_req - variable of type @ref
  * zb_zcl_color_control_step_saturation_req_s.
  * @param status - variable to put parse status to (see @ref zb_zcl_parse_status_t).
  */
#define ZB_ZCL_COLOR_CONTROL_GET_STEP_SATURATION_REQ(buffer, step_saturation_req, status) \
{                                                                                         \
  zb_zcl_color_control_step_saturation_req_t *step_saturation_req_ptr;                    \
  (step_saturation_req_ptr) = zb_buf_len(buffer) >=                                       \
    ZB_ZCL_COLOR_CONTROL_STEP_SATURATION_REQ_PAYLOAD_LEN ?                                \
    (zb_zcl_color_control_step_saturation_req_t*)zb_buf_begin(buffer) : NULL;             \
  if (step_saturation_req_ptr != NULL)                                                    \
  {                                                                                       \
    step_saturation_req.step_mode = step_saturation_req_ptr->step_mode;                   \
    step_saturation_req.step_size = step_saturation_req_ptr->step_size;                   \
    step_saturation_req.transition_time = step_saturation_req_ptr->transition_time;       \
    (void)zb_buf_cut_left(buffer, ZB_ZCL_COLOR_CONTROL_STEP_SATURATION_REQ_PAYLOAD_LEN);  \
    status = ZB_ZCL_PARSE_STATUS_SUCCESS;                                                 \
  }                                                                                       \
  else                                                                                    \
  {                                                                                       \
    status = ZB_ZCL_PARSE_STATUS_FAILURE;                                                 \
  }                                                                                       \
}

/******************************* Move to Hue and Saturation ******************************/

/*! @brief Structure representation of Move To Hue and Saturation command payload
    @see ZCL8 spec, subclause 5.2.2.3.10.1 */
typedef ZB_PACKED_PRE struct zb_zcl_color_control_move_to_hue_saturation_req_s
{
  /** Hue */
  zb_uint8_t hue;
  /** Saturation */
  zb_uint8_t saturation;
  /** Transition time field */
  zb_uint16_t transition_time;
} ZB_PACKED_STRUCT zb_zcl_color_control_move_to_hue_saturation_req_t;

/** @brief Move to Hue and Saturation payload length macro */
#define ZB_ZCL_COLOR_CONTROL_MOVE_TO_HUE_SATURATION_REQ_PAYLOAD_LEN \
  sizeof(zb_zcl_color_control_move_to_hue_saturation_req_t)

/*! @brief Send Move to Hue and Saturation command
    @param buffer - to put packet to
    @param dst_addr - address to send packet to
    @param dst_addr_mode - addressing mode
    @param dst_ep - destination endpoint
    @param ep - sending endpoint
    @param prof_id - profile identifier
    @param def_resp - enable/disable default response
    @param cb - callback for getting command send status
    @param hue - hue
    @param saturation - saturation
    @param transition_time - Transition Time value
    @param options_mask - Options Mask
    @param options_override - Options Override
*/
void zb_zcl_color_control_send_move_to_hue_saturation_req_zcl8(zb_bufid_t buffer, const zb_addr_u *dst_addr,
                                                               zb_uint8_t dst_addr_mode, zb_uint8_t dst_ep,
                                                               zb_uint8_t ep, zb_uint16_t prof_id,
                                                               zb_uint8_t def_resp, zb_callback_t cb,
                                                               zb_uint8_t hue,
                                                               zb_uint8_t saturation,
                                                               zb_uint16_t transition_time,
                                                               zb_uint8_t options_mask, zb_uint8_t options_override);

/*! @brief Send Move to Hue and Saturation command (pre-ZCL8)
    Use @ref zb_zcl_color_control_send_move_to_hue_saturation_req_zcl8 for ZCL8 revision call.
    @param buffer - to put packet to
    @param dst_addr - address to send packet to
    @param dst_addr_mode - addressing mode
    @param dst_ep - destination endpoint
    @param ep - sending endpoint
    @param prof_id - profile identifier
    @param def_resp - enable/disable default response
    @param cb - callback for getting command send status
    @param hue - hue
    @param saturation - saturation
    @param transition_time - Transition Time value
*/
void zb_zcl_color_control_send_move_to_hue_saturation_req(zb_bufid_t buffer, const zb_addr_u *dst_addr,
                                                          zb_uint8_t dst_addr_mode, zb_uint8_t dst_ep,
                                                          zb_uint8_t ep, zb_uint16_t prof_id,
                                                          zb_uint8_t def_resp, zb_callback_t cb,
                                                          zb_uint8_t hue,
                                                          zb_uint8_t saturation,
                                                          zb_uint16_t transition_time);

/** Macro for calling @ref zb_zcl_color_control_send_move_to_hue_saturation_req_zcl8 function
 */
#define ZB_ZCL_COLOR_CONTROL_SEND_MOVE_TO_HUE_SATURATION_REQ_ZCL8(buffer, addr, dst_addr_mode,           \
                                                                  dst_ep, ep, prfl_id, def_resp, cb,     \
                                                                  hue, saturation, transition_time,      \
                                                                  options_mask, options_override)        \
{                                                                                                        \
  zb_zcl_color_control_send_move_to_hue_saturation_req_zcl8(buffer, ZB_ADDR_U_CAST(addr), dst_addr_mode, \
                                                            dst_ep, ep, prfl_id, def_resp, cb,           \
                                                            hue, saturation, transition_time,            \
                                                            options_mask, options_override);             \
}

/** Macro for calling @ref zb_zcl_color_control_send_move_to_hue_saturation_req function
 */
#define ZB_ZCL_COLOR_CONTROL_SEND_MOVE_TO_HUE_SATURATION_REQ(buffer, addr, dst_addr_mode,                                    \
  dst_ep, ep, prfl_id, def_resp, cb, hue, saturation, transition_time)                                                       \
{                                                                                                                            \
  zb_zcl_color_control_send_move_to_hue_saturation_req(buffer, ZB_ADDR_U_CAST(addr), dst_addr_mode,                          \
                                                       dst_ep, ep, prfl_id, def_resp, cb, hue, saturation, transition_time); \
}

/** @brief Macro for getting Move To Hue and Saturation command
  * @attention Assumes that ZCL header already cut.
  * @param buffer containing the packet (by pointer).
  * @param move_to_hue_saturation_req - pointer to a variable of type @ref
  * zb_zcl_color_control_move_to_hue_saturation_req_s.
  * @param status - variable to put parse status to (see @ref zb_zcl_parse_status_t).
  */
#define ZB_ZCL_COLOR_CONTROL_GET_MOVE_TO_HUE_SATURATION_REQ(buffer, move_to_hue_saturation_req, status)            \
{                                                                                       \
  zb_zcl_color_control_move_to_hue_saturation_req_t *move_to_hue_saturation_req_ptr;                               \
  (move_to_hue_saturation_req_ptr) = zb_buf_len(buffer) >=                                                         \
    ZB_ZCL_COLOR_CONTROL_MOVE_TO_HUE_SATURATION_REQ_PAYLOAD_LEN ?                                                  \
    (zb_zcl_color_control_move_to_hue_saturation_req_t*)zb_buf_begin(buffer) : NULL;                               \
  if (move_to_hue_saturation_req_ptr != NULL)                                           \
  {                                                                                     \
    move_to_hue_saturation_req.hue = move_to_hue_saturation_req_ptr->hue;                                          \
    move_to_hue_saturation_req.saturation = move_to_hue_saturation_req_ptr->saturation;                            \
    ZB_HTOLE16(&(move_to_hue_saturation_req).transition_time, &(move_to_hue_saturation_req_ptr->transition_time)); \
    (void)zb_buf_cut_left(buffer, ZB_ZCL_COLOR_CONTROL_MOVE_TO_HUE_SATURATION_REQ_PAYLOAD_LEN);                    \
    status = ZB_ZCL_PARSE_STATUS_SUCCESS;                                                                          \
  }                                                                                     \
  else                                                                                  \
  {                                                                                     \
    status = ZB_ZCL_PARSE_STATUS_FAILURE;                                                                          \
  }                                                                                     \
}

/******************************* Move to Color ******************************/

/*! @brief Structure representation of Move To Color command payload
    @see ZCL8 spec, subclause 5.2.2.3.11.1 */
typedef ZB_PACKED_PRE struct zb_zcl_color_control_move_to_color_req_s
{
  /** ColorX */
  zb_uint16_t color_x;
  /** ColorY */
  zb_uint16_t color_y;
  /** Transition time field */
  zb_uint16_t transition_time;
} ZB_PACKED_STRUCT zb_zcl_color_control_move_to_color_req_t;

/** @brief Move to Color payload length macro */
#define ZB_ZCL_COLOR_CONTROL_MOVE_TO_COLOR_REQ_PAYLOAD_LEN \
  sizeof(zb_zcl_color_control_move_to_color_req_t)

/*! @brief Send Move to Color command
    @param buffer - to put packet to
    @param dst_addr - address to send packet to
    @param dst_addr_mode - addressing mode
    @param dst_ep - destination endpoint
    @param ep - sending endpoint
    @param prof_id - profile identifier
    @param def_resp - enable/disable default response
    @param cb - callback for getting command send status
    @param color_x - colorX
    @param color_y - colorY
    @param transition_time - Transition Time value
    @param options_mask - Options Mask
    @param options_override - Options Override
*/
void zb_zcl_color_control_send_move_to_color_req_zcl8(zb_bufid_t buffer, const zb_addr_u *dst_addr,
                                                      zb_uint8_t dst_addr_mode, zb_uint8_t dst_ep,
                                                      zb_uint8_t ep, zb_uint16_t prof_id,
                                                      zb_uint8_t def_resp, zb_callback_t cb,
                                                      zb_uint16_t color_x,
                                                      zb_uint16_t color_y,
                                                      zb_uint16_t transition_time,
                                                      zb_uint8_t options_mask, zb_uint8_t options_override);

/*! @brief Send Move to Color command (pre-ZCL8)
    Use @ref zb_zcl_color_control_send_move_to_color_req_zcl8 for ZCL8 revision call.
    @param buffer - to put packet to
    @param dst_addr - address to send packet to
    @param dst_addr_mode - addressing mode
    @param dst_ep - destination endpoint
    @param ep - sending endpoint
    @param prof_id - profile identifier
    @param def_resp - enable/disable default response
    @param cb - callback for getting command send status
    @param color_x - colorX
    @param color_y - colorY
    @param transition_time - Transition Time value
*/
void zb_zcl_color_control_send_move_to_color_req(zb_bufid_t buffer, const zb_addr_u *dst_addr,
                                                 zb_uint8_t dst_addr_mode, zb_uint8_t dst_ep,
                                                 zb_uint8_t ep, zb_uint16_t prof_id,
                                                 zb_uint8_t def_resp, zb_callback_t cb,
                                                 zb_uint16_t color_x,
                                                 zb_uint16_t color_y,
                                                 zb_uint16_t transition_time);

/** Macro for calling @ref zb_zcl_color_control_send_move_to_color_req_zcl8 function
 */
#define ZB_ZCL_COLOR_CONTROL_SEND_MOVE_TO_COLOR_REQ_ZCL8(buffer, addr, dst_addr_mode,                \
                                                         dst_ep, ep, prfl_id, def_resp, cb, color_x, \
                                                         color_y, transition_time,                   \
                                                         options_mask, options_override)             \
{                                                                                                    \
  zb_zcl_color_control_send_move_to_color_req_zcl8(buffer, ZB_ADDR_U_CAST(addr), dst_addr_mode,      \
                                                   dst_ep, ep, prfl_id, def_resp, cb, color_x,       \
                                                   color_y, transition_time,                         \
                                                   options_mask, options_override);                  \
}

/** Macro for calling @ref zb_zcl_color_control_send_move_to_color_req function
 */
#define ZB_ZCL_COLOR_CONTROL_SEND_MOVE_TO_COLOR_REQ(buffer, addr, dst_addr_mode,                                     \
  dst_ep, ep, prfl_id, def_resp, cb, color_x, color_y, transition_time)                                              \
{                                                                                                                    \
  zb_zcl_color_control_send_move_to_color_req(buffer, ZB_ADDR_U_CAST(addr), dst_addr_mode,                           \
                                              dst_ep, ep, prfl_id, def_resp, cb, color_x, color_y, transition_time); \
}


/** @brief Macro for getting Move To Color command
  * @attention Assumes that ZCL header already cut.
  * @param buffer  containing the packet (by pointer).
  * zb_zcl_color_control_move_to_color_req_s.
  * @param move_to_color_req containing the packet (by pointer).
  * @param status - variable to put parse status to (see @ref zb_zcl_parse_status_t).
  */
#define ZB_ZCL_COLOR_CONTROL_GET_MOVE_TO_COLOR_REQ(buffer, move_to_color_req, status)                \
{                                                                                                    \
  zb_zcl_color_control_move_to_color_req_t *move_to_color_req_ptr;                                   \
  (move_to_color_req_ptr) = zb_buf_len(buffer) >=                                                    \
    ZB_ZCL_COLOR_CONTROL_MOVE_TO_COLOR_REQ_PAYLOAD_LEN ?                                             \
    (zb_zcl_color_control_move_to_color_req_t*)zb_buf_begin(buffer) : NULL;                          \
  if (move_to_color_req_ptr != NULL)                                                                 \
  {                                                                                                  \
    ZB_HTOLE16(&(move_to_color_req).color_x, &(move_to_color_req_ptr->color_x));                     \
    ZB_HTOLE16(&(move_to_color_req).color_y, &(move_to_color_req_ptr->color_y));                     \
    ZB_HTOLE16(&(move_to_color_req).transition_time, &(move_to_color_req_ptr->transition_time));     \
    (void)zb_buf_cut_left(buffer, ZB_ZCL_COLOR_CONTROL_MOVE_TO_COLOR_REQ_PAYLOAD_LEN);               \
    status = ZB_ZCL_PARSE_STATUS_SUCCESS;                                                            \
  }                                                                                                  \
  else                                                                                               \
  {                                                                                                  \
    status = ZB_ZCL_PARSE_STATUS_FAILURE;                                                            \
  }                                                                                                  \
}

/******************************* Move Color ******************************/

/*! @brief Structure representation of Move Color command payload
    @see ZCL8 spec, subclause 5.2.2.3.12.1 */
typedef ZB_PACKED_PRE struct zb_zcl_color_control_move_color_req_s
{
  /** RateX */
  zb_int16_t rate_x;
  /** RateY */
  zb_int16_t rate_y;
} ZB_PACKED_STRUCT zb_zcl_color_control_move_color_req_t;

/** @brief Move Color payload length macro */
#define ZB_ZCL_COLOR_CONTROL_MOVE_COLOR_REQ_PAYLOAD_LEN \
  sizeof(zb_zcl_color_control_move_color_req_t)

/*! @brief Send Move Color command
    @param buffer - to put packet to
    @param dst_addr - address to send packet to
    @param dst_addr_mode - addressing mode
    @param dst_ep - destination endpoint
    @param ep - sending endpoint
    @param prof_id - profile identifier
    @param def_resp - enable/disable default response
    @param cb - callback for getting command send status
    @param rate_x - RateX
    @param rate_y - RateY
    @param options_mask - Options Mask
    @param options_override - Options Override
*/
void zb_zcl_color_control_send_move_color_req_zcl8(zb_bufid_t buffer, const zb_addr_u *dst_addr,
                                                   zb_uint8_t dst_addr_mode, zb_uint8_t dst_ep,
                                                   zb_uint8_t ep, zb_uint16_t prof_id,
                                                   zb_uint8_t def_resp, zb_callback_t cb,
                                                   zb_uint16_t rate_x,
                                                   zb_uint16_t rate_y,
                                                   zb_uint8_t options_mask, zb_uint8_t options_override);

/*! @brief Send Move Color command (pre-ZCL8)
    Use @ref zb_zcl_color_control_send_move_color_req_zcl8 for ZCL8 revision call.
    @param buffer - to put packet to
    @param dst_addr - address to send packet to
    @param dst_addr_mode - addressing mode
    @param dst_ep - destination endpoint
    @param ep - sending endpoint
    @param prof_id - profile identifier
    @param def_resp - enable/disable default response
    @param cb - callback for getting command send status
    @param rate_x - RateX
    @param rate_y - RateY
*/
void zb_zcl_color_control_send_move_color_req(zb_bufid_t buffer, const zb_addr_u *dst_addr,
                                              zb_uint8_t dst_addr_mode, zb_uint8_t dst_ep,
                                              zb_uint8_t ep, zb_uint16_t prof_id,
                                              zb_uint8_t def_resp, zb_callback_t cb,
                                              zb_uint16_t rate_x,
                                              zb_uint16_t rate_y);

/** Macro for calling @ref zb_zcl_color_control_send_move_color_req_zcl8 function
 */
#define ZB_ZCL_COLOR_CONTROL_SEND_MOVE_COLOR_REQ_ZCL8(buffer, addr, dst_addr_mode,                       \
                                                      dst_ep, ep, prfl_id, def_resp, cb, rate_x, rate_y, \
                                                      options_mask, options_override)                    \
{                                                                                                        \
  zb_zcl_color_control_send_move_color_req_zcl8(buffer, ZB_ADDR_U_CAST(addr), dst_addr_mode,             \
                                                dst_ep, ep, prfl_id, def_resp, cb, rate_x, rate_y,       \
                                                options_mask, options_override);                         \
}

/** Macro for calling @ref zb_zcl_color_control_send_move_color_req function
 */
#define ZB_ZCL_COLOR_CONTROL_SEND_MOVE_COLOR_REQ(buffer, addr, dst_addr_mode,                  \
    dst_ep, ep, prfl_id, def_resp, cb, rate_x, rate_y)                                         \
{                                                                                              \
  zb_zcl_color_control_send_move_color_req(buffer, ZB_ADDR_U_CAST(addr), dst_addr_mode,        \
                                           dst_ep, ep, prfl_id, def_resp, cb, rate_x, rate_y); \
}

/** @brief Macro for getting Move Color command
  * @attention Assumes that ZCL header already cut.
  * @param buffer containing the packet (by pointer).
  * @param move_color_req - variable of type @ref
  * zb_zcl_color_control_move_color_req_s.
  * @param status - variable to put parse status to (see @ref zb_zcl_parse_status_t).
  */
#define ZB_ZCL_COLOR_CONTROL_GET_MOVE_COLOR_REQ(buffer, move_color_req, status)                      \
{                                                                                                    \
  zb_zcl_color_control_move_color_req_t *move_color_req_ptr;                                         \
  (move_color_req_ptr) = zb_buf_len(buffer) >=                                                       \
    ZB_ZCL_COLOR_CONTROL_MOVE_COLOR_REQ_PAYLOAD_LEN ?                                                \
    (zb_zcl_color_control_move_color_req_t*)zb_buf_begin(buffer) : NULL;                             \
  if (move_color_req_ptr != NULL)                                                                    \
  {                                                                                                  \
    ZB_HTOLE16(&(move_color_req).rate_x, &(move_color_req_ptr->rate_x));                             \
    ZB_HTOLE16(&(move_color_req).rate_y, &(move_color_req_ptr->rate_y));                             \
    (void)zb_buf_cut_left(buffer, ZB_ZCL_COLOR_CONTROL_MOVE_COLOR_REQ_PAYLOAD_LEN);                  \
    status = ZB_ZCL_PARSE_STATUS_SUCCESS;                                                            \
  }                                                                                                  \
  else                                                                                               \
  {                                                                                                  \
    status = ZB_ZCL_PARSE_STATUS_FAILURE;                                                            \
  }                                                                             \
}

/******************************* Step Color ******************************/

/*! @brief Structure representation of Step Color command payload
    @see ZCL8 spec, subclause 5.2.2.3.13.1 */
typedef ZB_PACKED_PRE struct zb_zcl_color_control_step_color_req_s
{
  /** StepX */
  zb_uint16_t step_x;
  /** StepY */
  zb_uint16_t step_y;
  /** Transition time field */
  zb_uint16_t transition_time;
} ZB_PACKED_STRUCT zb_zcl_color_control_step_color_req_t;

/** @brief Step Color payload length macro */
#define ZB_ZCL_COLOR_CONTROL_STEP_COLOR_REQ_PAYLOAD_LEN \
  sizeof(zb_zcl_color_control_step_color_req_t)

/*! @brief Send Step Color command
    @param buffer - to put packet to
    @param dst_addr - address to send packet to
    @param dst_addr_mode - addressing mode
    @param dst_ep - destination endpoint
    @param ep - sending endpoint
    @param prof_id - profile identifier
    @param def_resp - enable/disable default response
    @param cb - callback for getting command send status
    @param step_x - StepX
    @param step_y - StepY
    @param transition_time - Transition Time value
    @param options_mask - Options Mask
    @param options_override - Options Override
*/
void zb_zcl_color_control_send_step_color_req_zcl8(zb_bufid_t buffer, const zb_addr_u *dst_addr,
                                                   zb_uint8_t dst_addr_mode, zb_uint8_t dst_ep,
                                                   zb_uint8_t ep, zb_uint16_t prof_id,
                                                   zb_uint8_t def_resp, zb_callback_t cb,
                                                   zb_uint16_t step_x,
                                                   zb_uint16_t step_y,
                                                   zb_uint16_t transition_time,
                                                   zb_uint8_t options_mask, zb_uint8_t options_override);

/*! @brief Send Step Color command (pre-ZCL8)
    Use @ref zb_zcl_color_control_send_step_color_req_zcl8 for ZCL8 revision call.
    @param buffer - to put packet to
    @param dst_addr - address to send packet to
    @param dst_addr_mode - addressing mode
    @param dst_ep - destination endpoint
    @param ep - sending endpoint
    @param prof_id - profile identifier
    @param def_resp - enable/disable default response
    @param cb - callback for getting command send status
    @param step_x - StepX
    @param step_y - StepY
    @param transition_time - Transition Time value
*/
void zb_zcl_color_control_send_step_color_req(zb_bufid_t buffer, const zb_addr_u *dst_addr,
                                              zb_uint8_t dst_addr_mode, zb_uint8_t dst_ep,
                                              zb_uint8_t ep, zb_uint16_t prof_id,
                                              zb_uint8_t def_resp, zb_callback_t cb,
                                              zb_uint16_t step_x,
                                              zb_uint16_t step_y,
                                              zb_uint16_t transition_time);

/** Macro for calling @ref zb_zcl_color_control_send_step_color_req_zcl8 function
 */
#define ZB_ZCL_COLOR_CONTROL_SEND_STEP_COLOR_REQ_ZCL8(buffer, addr, dst_addr_mode,                      \
                                                      dst_ep, ep, prfl_id, def_resp, cb, step_x, step_y,\
                                                      transition_time,                                  \
                                                      options_mask, options_override)                   \
{                                                                                                       \
  zb_zcl_color_control_send_step_color_req_zcl8(buffer, ZB_ADDR_U_CAST(addr), dst_addr_mode,            \
                                                dst_ep, ep, prfl_id, def_resp, cb, step_x, step_y,      \
                                                transition_time,                                        \
                                                options_mask, options_override);                        \
}

/** Macro for calling @ref zb_zcl_color_control_send_step_color_req function
 */
#define ZB_ZCL_COLOR_CONTROL_SEND_STEP_COLOR_REQ(buffer, addr, dst_addr_mode,                                   \
  dst_ep, ep, prfl_id, def_resp, cb, step_x, step_y, transition_time)                                           \
{                                                                                                               \
  zb_zcl_color_control_send_step_color_req(buffer, ZB_ADDR_U_CAST(addr), dst_addr_mode,                         \
                                           dst_ep, ep, prfl_id, def_resp, cb, step_x, step_y, transition_time); \
}

/** @brief Macro for getting Move Color command
  * @attention Assumes that ZCL header already cut.
  * @param buffer Buffer containing the packet.
  * @param step_color_req - variable of type @ref zb_zcl_color_control_step_color_req_s.
  * @param status - variable to put parse status to (see @ref zb_zcl_parse_status_t).
  */
#define ZB_ZCL_COLOR_CONTROL_GET_STEP_COLOR_REQ(buffer, step_color_req, status)                      \
{                                                                                                    \
  zb_zcl_color_control_step_color_req_t *step_color_req_ptr;                                         \
  (step_color_req_ptr) = zb_buf_len(buffer) >=                                                       \
    ZB_ZCL_COLOR_CONTROL_STEP_COLOR_REQ_PAYLOAD_LEN ?                                                \
    (zb_zcl_color_control_step_color_req_t*)zb_buf_begin(buffer) : NULL;                             \
  if (step_color_req_ptr != NULL)                                                                    \
  {                                                                                                  \
    ZB_HTOLE16(&(step_color_req).step_x, &(step_color_req_ptr->step_x));                             \
    ZB_HTOLE16(&(step_color_req).step_y, &(step_color_req_ptr->step_y));                             \
    ZB_HTOLE16(&(step_color_req).transition_time, &(step_color_req_ptr->transition_time));           \
    (void)zb_buf_cut_left(buffer, ZB_ZCL_COLOR_CONTROL_STEP_COLOR_REQ_PAYLOAD_LEN);                  \
    status = ZB_ZCL_PARSE_STATUS_SUCCESS;                                                            \
  }                                                                                                  \
  else                                                                                               \
  {                                                                                                  \
    status = ZB_ZCL_PARSE_STATUS_FAILURE;                                                            \
  }                                                                                                  \
}

/******************************* Move to Color Temperature ******************************/

/*! @brief Structure representation of Move To Color Temperature command payload
    @see ZCL8 spec, subclause 5.2.2.3.14.1 */
typedef ZB_PACKED_PRE struct zb_zcl_color_control_move_to_color_temperature_req_s
{
  /** Color Temperature */
  zb_uint16_t color_temperature;
  /** Transition time field */
  zb_uint16_t transition_time;
} ZB_PACKED_STRUCT zb_zcl_color_control_move_to_color_temperature_req_t;

/** @brief Move to Color Temperature payload length macro */
#define ZB_ZCL_COLOR_CONTROL_MOVE_TO_COLOR_TEMP_REQ_PAYLOAD_LEN \
  sizeof(zb_zcl_color_control_move_to_color_temperature_req_t)

/*! @brief Send Move To Color Temperature command
    @param buffer - to put packet to
    @param dst_addr - address to send packet to
    @param dst_addr_mode - addressing mode
    @param dst_ep - destination endpoint
    @param ep - sending endpoint
    @param prof_id - profile identifier
    @param def_resp - enable/disable default response
    @param cb - callback for getting command send status
    @param color_temperature - Color Temperature
    @param transition_time - Transition Time value
    @param options_mask - Options Mask
    @param options_override - Options Override
*/
void zb_zcl_color_control_send_move_to_color_temperature_req_zcl8(zb_bufid_t buffer, const zb_addr_u *dst_addr,
                                                                  zb_uint8_t dst_addr_mode, zb_uint8_t dst_ep,
                                                                  zb_uint8_t ep, zb_uint16_t prof_id,
                                                                  zb_uint8_t def_resp, zb_callback_t cb,
                                                                  zb_uint16_t color_temperature,
                                                                  zb_uint16_t transition_time,
                                                                  zb_uint8_t options_mask, zb_uint8_t options_override);

/*! @brief Send Move To Color Temperature command (pre-ZCL8)
    Use @ref zb_zcl_color_control_send_move_to_color_temperature_req_zcl8 for ZCL8 revision call.
    @param buffer - to put packet to
    @param dst_addr - address to send packet to
    @param dst_addr_mode - addressing mode
    @param dst_ep - destination endpoint
    @param ep - sending endpoint
    @param prof_id - profile identifier
    @param def_resp - enable/disable default response
    @param cb - callback for getting command send status
    @param color_temperature - Color Temperature
    @param transition_time - Transition Time value
*/
void zb_zcl_color_control_send_move_to_color_temperature_req(zb_bufid_t buffer, const zb_addr_u *dst_addr,
                                                             zb_uint8_t dst_addr_mode, zb_uint8_t dst_ep,
                                                             zb_uint8_t ep, zb_uint16_t prof_id,
                                                             zb_uint8_t def_resp, zb_callback_t cb,
                                                             zb_uint16_t color_temperature,
                                                             zb_uint16_t transition_time);

/** Macro for calling @ref zb_zcl_color_control_send_move_to_color_temperature_req_zcl8 function
 */
#define ZB_ZCL_COLOR_CONTROL_SEND_MOVE_TO_COLOR_TEMPERATURE_REQ_ZCL8(buffer, addr, dst_addr_mode,           \
                                                                     dst_ep, ep, prfl_id, def_resp, cb,     \
                                                                     color_temperature, transition_time,    \
                                                                     options_mask, options_override)        \
{                                                                                                           \
  zb_zcl_color_control_send_move_to_color_temperature_req_zcl8(buffer, ZB_ADDR_U_CAST(addr), dst_addr_mode, \
                                                               dst_ep, ep, prfl_id, def_resp, cb,           \
                                                               color_temperature, transition_time,          \
                                                               options_mask, options_override);             \
}

/** Macro for calling @ref zb_zcl_color_control_send_move_to_color_temperature_req function
 */
#define ZB_ZCL_COLOR_CONTROL_SEND_MOVE_TO_COLOR_TEMPERATURE_REQ(buffer, addr, dst_addr_mode,                                      \
  dst_ep, ep, prfl_id, def_resp, cb, color_temperature, transition_time)                                                          \
{                                                                                                                                 \
  zb_zcl_color_control_send_move_to_color_temperature_req(buffer, ZB_ADDR_U_CAST(addr), dst_addr_mode,                            \
                                                          dst_ep, ep, prfl_id, def_resp, cb, color_temperature, transition_time); \
}

/** @brief Macro for getting Move to Color Temperature command
  * @attention Assumes that ZCL header already cut.
  * @param buffer containing the packet (by pointer).
  * @param move_to_color_temperature_req - variable of type @ref
  * zb_zcl_color_control_move_to_color_temperature_req_s.
  * @param status - variable to put parse status to (see @ref zb_zcl_parse_status_t).
  */
#define ZB_ZCL_COLOR_CONTROL_GET_MOVE_TO_COLOR_TEMPERATURE_REQ(buffer, move_to_color_temperature_req, status)         \
{                                                                                                                     \
  zb_zcl_color_control_move_to_color_temperature_req_t *move_to_color_temp_req_ptr;                                   \
  (move_to_color_temp_req_ptr) = zb_buf_len(buffer) >=                                                                \
    ZB_ZCL_COLOR_CONTROL_MOVE_TO_COLOR_TEMP_REQ_PAYLOAD_LEN ?                                                         \
    (zb_zcl_color_control_move_to_color_temperature_req_t*)zb_buf_begin(buffer) : NULL;                               \
  if (move_to_color_temp_req_ptr != NULL)                                                                             \
  {                                                                                                                   \
    ZB_HTOLE16(&(move_to_color_temperature_req).color_temperature, &(move_to_color_temp_req_ptr->color_temperature)); \
    ZB_HTOLE16(&(move_to_color_temperature_req).transition_time, &(move_to_color_temp_req_ptr->transition_time));     \
    (void)zb_buf_cut_left(buffer, ZB_ZCL_COLOR_CONTROL_MOVE_TO_COLOR_TEMP_REQ_PAYLOAD_LEN);                           \
    status = ZB_ZCL_PARSE_STATUS_SUCCESS;                                                                             \
  }                                                                                                                   \
  else                                                                                                                \
  {                                                                                                                   \
    status = ZB_ZCL_PARSE_STATUS_FAILURE;                                                                             \
  }                                                                                                                   \
}

/******************************* ENHANCED_MOVE_TO_HUE ******************************/

/*! @brief Structure representation of Enhanced Move To Hue command payload
    @see ZCL8 spec, subclause 5.2.2.3.15.1 */
typedef ZB_PACKED_PRE struct zb_zcl_color_control_enhanced_move_to_hue_req_s
{
  /** Enhanced Hue */
  zb_uint16_t enhanced_hue;
  /** Direction, see @ref zb_zcl_color_control_move_to_hue_direction_e */
  zb_uint8_t direction;
  /** Transition time field */
  zb_uint16_t transition_time;
} ZB_PACKED_STRUCT zb_zcl_color_control_enhanced_move_to_hue_req_t;

/** @brief Enhanced Move to Hue payload length macro */
#define ZB_ZCL_COLOR_CONTROL_ENHANCED_MOVE_TO_HUE_REQ_PAYLOAD_LEN \
  sizeof(zb_zcl_color_control_enhanced_move_to_hue_req_t)

/*! @brief Send Enhanced Move to Hue command
    @param buffer - to put packet to
    @param dst_addr - address to send packet to
    @param dst_addr_mode - addressing mode
    @param dst_ep - destination endpoint
    @param ep - sending endpoint
    @param prof_id - profile identifier
    @param def_resp - enable/disable default response
    @param cb - callback for getting command send status
    @param enhanced_hue - Enhanced Hue value
    @param direction - Direction value, see @ref zb_zcl_color_control_move_to_hue_direction_e
    @param transition_time - Transition Time value
    @param options_mask - Options Mask
    @param options_override - Options Override
*/
void zb_zcl_color_control_send_enhanced_move_to_hue_req_zcl8(zb_bufid_t buffer, const zb_addr_u *dst_addr,
                                                             zb_uint8_t dst_addr_mode, zb_uint8_t dst_ep,
                                                             zb_uint8_t ep, zb_uint16_t prof_id,
                                                             zb_uint8_t def_resp, zb_callback_t cb,
                                                             zb_uint16_t enhanced_hue,
                                                             zb_uint8_t direction,
                                                             zb_uint16_t transition_time,
                                                             zb_uint8_t options_mask, zb_uint8_t options_override);

/*! @brief Send Enhanced Move to Hue command (pre-ZCL8)
    Use @ref zb_zcl_color_control_send_enhanced_move_to_hue_req_zcl8 for ZCL8 revision call.
    @param buffer - to put packet to
    @param dst_addr - address to send packet to
    @param dst_addr_mode - addressing mode
    @param dst_ep - destination endpoint
    @param ep - sending endpoint
    @param prof_id - profile identifier
    @param def_resp - enable/disable default response
    @param cb - callback for getting command send status
    @param enhanced_hue - Enhanced Hue value
    @param direction - Direction value, see @ref zb_zcl_color_control_move_to_hue_direction_e
    @param transition_time - Transition Time value
*/
void zb_zcl_color_control_send_enhanced_move_to_hue_req(zb_bufid_t buffer, const zb_addr_u *dst_addr,
                                                        zb_uint8_t dst_addr_mode, zb_uint8_t dst_ep,
                                                        zb_uint8_t ep, zb_uint16_t prof_id,
                                                        zb_uint8_t def_resp, zb_callback_t cb,
                                                        zb_uint16_t enhanced_hue,
                                                        zb_uint8_t direction,
                                                        zb_uint16_t transition_time);

/** Macro for calling @ref zb_zcl_color_control_send_enhanced_move_to_hue_req_zcl8 function
 */
#define ZB_ZCL_COLOR_CONTROL_SEND_ENHANCED_MOVE_TO_HUE_REQ_ZCL8(buffer, addr, dst_addr_mode,          \
                                                                dst_ep, ep, prfl_id, def_resp, cb,    \
                                                                ex_hue, direction, transition_time,   \
                                                                options_mask, options_override)       \
{                                                                                                     \
  zb_zcl_color_control_send_enhanced_move_to_hue_req_zcl8(buffer, ZB_ADDR_U_CAST(addr), dst_addr_mode,\
                                                          dst_ep, ep, prfl_id, def_resp, cb,          \
                                                          ex_hue, direction, transition_time,         \
                                                          options_mask, options_override);            \
}

/** Macro for calling @ref zb_zcl_color_control_send_enhanced_move_to_hue_req function
 */
#define ZB_ZCL_COLOR_CONTROL_SEND_ENHANCED_MOVE_TO_HUE_REQ(buffer, addr, dst_addr_mode,                                     \
  dst_ep, ep, prfl_id, def_resp, cb, ex_hue, direction, transition_time)                                                    \
{                                                                                                                           \
  zb_zcl_color_control_send_enhanced_move_to_hue_req(buffer, ZB_ADDR_U_CAST(addr), dst_addr_mode,                           \
                                                    dst_ep, ep, prfl_id, def_resp, cb, ex_hue, direction, transition_time); \
}

/** @brief Macro for getting Enhanced Move to Hue command
  * @attention Assumes that ZCL header already cut.
  * @param buffer containing the packet (by pointer).
  * @param enhanced_move_to_hue_req - variable of type @ref
  * zb_zcl_color_control_enhanced_move_to_hue_req_s.
  * @param status - variable to put parse status to (see @ref zb_zcl_parse_status_t).
  */
#define ZB_ZCL_COLOR_CONTROL_GET_ENHANCED_MOVE_TO_HUE_REQ(buffer, enhanced_move_to_hue_req, status)            \
{                                                                                                              \
  zb_zcl_color_control_enhanced_move_to_hue_req_t *enhanced_move_to_hue_req_ptr;                               \
  (enhanced_move_to_hue_req_ptr) = zb_buf_len(buffer) >=                                                       \
    ZB_ZCL_COLOR_CONTROL_ENHANCED_MOVE_TO_HUE_REQ_PAYLOAD_LEN ?                                                \
    (zb_zcl_color_control_enhanced_move_to_hue_req_t*)zb_buf_begin(buffer) : NULL;                             \
  if (enhanced_move_to_hue_req_ptr != NULL)                                                                    \
  {                                                                                                            \
    ZB_HTOLE16(&(enhanced_move_to_hue_req).enhanced_hue, &(enhanced_move_to_hue_req_ptr->enhanced_hue));       \
    enhanced_move_to_hue_req.direction = enhanced_move_to_hue_req_ptr->direction;                              \
    ZB_HTOLE16(&(enhanced_move_to_hue_req).transition_time, &(enhanced_move_to_hue_req_ptr->transition_time)); \
    (void)zb_buf_cut_left(buffer, ZB_ZCL_COLOR_CONTROL_ENHANCED_MOVE_TO_HUE_REQ_PAYLOAD_LEN);                  \
    status = ZB_ZCL_PARSE_STATUS_SUCCESS;                                                                      \
  }                                                                                                            \
  else                                                                                                         \
  {                                                                                                            \
    status = ZB_ZCL_PARSE_STATUS_FAILURE;                                                                      \
  }                                                                                                            \
}

/******************************* ENHANCED_MOVE_HUE ******************************/

/*! @brief Structure representation of Enhanced Move Hue command payload
    @see ZCL8 spec, subclause 5.2.2.3.16 */
typedef ZB_PACKED_PRE struct zb_zcl_color_control_enhanced_move_hue_req_s
{
  /** Move mode, see @ref zb_zcl_color_control_move_direction_e */
  zb_uint8_t move_mode;
  /** Rate */
  zb_uint16_t rate;
} ZB_PACKED_STRUCT zb_zcl_color_control_enhanced_move_hue_req_t;

/** @brief Enhanced Move to Hue payload length macro */
#define ZB_ZCL_COLOR_CONTROL_ENHANCED_MOVE_HUE_REQ_PAYLOAD_LEN \
  sizeof(zb_zcl_color_control_enhanced_move_hue_req_t)

/*! @brief Send Enhanced Move Hue command
    @param buffer - to put packet to
    @param dst_addr - address to send packet to
    @param dst_addr_mode - addressing mode
    @param dst_ep - destination endpoint
    @param ep - sending endpoint
    @param prof_id - profile identifier
    @param def_resp - enable/disable default response
    @param cb - callback for getting command send status
    @param move_mode - Move mode, see @ref zb_zcl_color_control_move_direction_e
    @param rate - Rate
    @param options_mask - Options Mask
    @param options_override - Options Override
*/
void zb_zcl_color_control_send_enhanced_move_hue_req_zcl8(zb_bufid_t buffer, const zb_addr_u *dst_addr,
                                                          zb_uint8_t dst_addr_mode, zb_uint8_t dst_ep,
                                                          zb_uint8_t ep, zb_uint16_t prof_id,
                                                          zb_uint8_t def_resp, zb_callback_t cb,
                                                          zb_uint8_t move_mode,
                                                          zb_uint16_t rate,
                                                          zb_uint8_t options_mask, zb_uint8_t options_override);

/*! @brief Send Enhanced Move Hue command (pre-ZCL8)
    Use @ref zb_zcl_color_control_send_enhanced_move_hue_req_zcl8 for ZCL8 revision call.
    @param buffer - to put packet to
    @param dst_addr - address to send packet to
    @param dst_addr_mode - addressing mode
    @param dst_ep - destination endpoint
    @param ep - sending endpoint
    @param prof_id - profile identifier
    @param def_resp - enable/disable default response
    @param cb - callback for getting command send status
    @param move_mode - Move mode, see @ref zb_zcl_color_control_move_direction_e
    @param rate - Rate
*/
void zb_zcl_color_control_send_enhanced_move_hue_req(zb_bufid_t buffer, const zb_addr_u *dst_addr,
                                                     zb_uint8_t dst_addr_mode, zb_uint8_t dst_ep,
                                                     zb_uint8_t ep, zb_uint16_t prof_id,
                                                     zb_uint8_t def_resp, zb_callback_t cb,
                                                     zb_uint8_t move_mode,
                                                     zb_uint16_t rate);

/** Macro for calling @ref zb_zcl_color_control_send_enhanced_move_hue_req_zcl8 function
 */
#define ZB_ZCL_COLOR_CONTROL_SEND_ENHANCED_MOVE_HUE_REQ_ZCL8(buffer, addr, dst_addr_mode,           \
                                                             dst_ep, ep, prfl_id, def_resp, cb,     \
                                                             move_mode, rate,                       \
                                                             options_mask, options_override)        \
{                                                                                                   \
  zb_zcl_color_control_send_enhanced_move_hue_req_zcl8(buffer, ZB_ADDR_U_CAST(addr), dst_addr_mode, \
                                                       dst_ep, ep, prfl_id, def_resp, cb,           \
                                                       move_mode, rate,                             \
                                                       options_mask, options_override);             \
}

/** Macro for calling @ref zb_zcl_color_control_send_enhanced_move_hue_req function
 */
#define ZB_ZCL_COLOR_CONTROL_SEND_ENHANCED_MOVE_HUE_REQ(buffer, addr, dst_addr_mode,                   \
  dst_ep, ep, prfl_id, def_resp, cb, move_mode, rate)                                                  \
{                                                                                                      \
  zb_zcl_color_control_send_enhanced_move_hue_req(buffer, ZB_ADDR_U_CAST(addr), dst_addr_mode,         \
                                                  dst_ep, ep, prfl_id, def_resp, cb, move_mode, rate); \
}

/** @brief Macro for getting Enhanced Move Hue command
  * @attention Assumes that ZCL header already cut.
  * @param buffer containing the packet (by pointer).
  * @param enhanced_move_hue_req - variable of type @ref
  * zb_zcl_color_control_enhanced_move_hue_req_s.
  * @param status - variable to put parse status to (see @ref zb_zcl_parse_status_t).
  */
#define ZB_ZCL_COLOR_CONTROL_GET_ENHANCED_MOVE_HUE_REQ(buffer, enhanced_move_hue_req, status)    \
{                                                                                                \
  zb_zcl_color_control_enhanced_move_hue_req_t *enhanced_move_hue_req_ptr;                       \
  (enhanced_move_hue_req_ptr) = zb_buf_len(buffer) >=                                            \
    ZB_ZCL_COLOR_CONTROL_ENHANCED_MOVE_HUE_REQ_PAYLOAD_LEN ?                                     \
    (zb_zcl_color_control_enhanced_move_hue_req_t*)zb_buf_begin(buffer) : NULL;                  \
  if (enhanced_move_hue_req_ptr != NULL)                                                         \
  {                                                                                              \
    enhanced_move_hue_req.move_mode = enhanced_move_hue_req_ptr->move_mode;                      \
    ZB_HTOLE16(&(enhanced_move_hue_req).rate, &(enhanced_move_hue_req_ptr->rate));               \
    (void)zb_buf_cut_left(buffer, ZB_ZCL_COLOR_CONTROL_ENHANCED_MOVE_HUE_REQ_PAYLOAD_LEN);       \
    status = ZB_ZCL_PARSE_STATUS_SUCCESS;                                                        \
  }                                                                                              \
  else                                                                                           \
  {                                                                                              \
    status = ZB_ZCL_PARSE_STATUS_FAILURE;                                                        \
  }                                                                                              \
}

/******************************* ENHANCED_STEP_HUE ******************************/

/*! @brief Structure representation of Enhanced Step Hue command payload
    @see ZCL8 spec, subclause 5.2.2.3.17 */
typedef ZB_PACKED_PRE struct zb_zcl_color_control_enhanced_step_hue_req_s
{
  /** Step mode, see @ref zb_zcl_color_control_step_mode_e */
  zb_uint8_t step_mode;
  /** Step size */
  zb_uint16_t step_size;
  /** Transition time field */
  zb_uint16_t transition_time;
} ZB_PACKED_STRUCT zb_zcl_color_control_enhanced_step_hue_req_t;

/** @brief Enhanced Move to Hue payload length macro */
#define ZB_ZCL_COLOR_CONTROL_ENHANCED_STEP_HUE_REQ_PAYLOAD_LEN \
  sizeof(zb_zcl_color_control_enhanced_step_hue_req_t)

/*! @brief Send Enhanced Step Hue command
    @param buffer - to put packet to
    @param dst_addr - address to send packet to
    @param dst_addr_mode - addressing mode
    @param dst_ep - destination endpoint
    @param ep - sending endpoint
    @param prof_id - profile identifier
    @param def_resp - enable/disable default response
    @param cb - callback for getting command send status
    @param step_mode - step mode value, see @ref zb_zcl_color_control_step_mode_e
    @param step_size - step size value
    @param transition_time - Transition Time value
    @param options_mask - Options Mask
    @param options_override - Options Override
*/
void zb_zcl_color_control_send_enhanced_step_hue_req_zcl8(zb_bufid_t buffer, const zb_addr_u *dst_addr,
                                                          zb_uint8_t dst_addr_mode, zb_uint8_t dst_ep,
                                                          zb_uint8_t ep, zb_uint16_t prof_id,
                                                          zb_uint8_t def_resp, zb_callback_t cb,
                                                          zb_uint8_t step_mode,
                                                          zb_uint16_t step_size,
                                                          zb_uint16_t transition_time,
                                                          zb_uint8_t options_mask, zb_uint8_t options_override);

/*! @brief Send Enhanced Step Hue command (pre-ZCL8)
    Use @ref zb_zcl_color_control_send_enhanced_step_hue_req_zcl8 for ZCL8 revision call.
    @param buffer - to put packet to
    @param dst_addr - address to send packet to
    @param dst_addr_mode - addressing mode
    @param dst_ep - destination endpoint
    @param ep - sending endpoint
    @param prof_id - profile identifier
    @param def_resp - enable/disable default response
    @param cb - callback for getting command send status
    @param step_mode - step mode value, see @ref zb_zcl_color_control_step_mode_e
    @param step_size - step size value
    @param transition_time - Transition Time value
*/
void zb_zcl_color_control_send_enhanced_step_hue_req(zb_bufid_t buffer, const zb_addr_u *dst_addr,
                                                     zb_uint8_t dst_addr_mode, zb_uint8_t dst_ep,
                                                     zb_uint8_t ep, zb_uint16_t prof_id,
                                                     zb_uint8_t def_resp, zb_callback_t cb,
                                                     zb_uint8_t step_mode,
                                                     zb_uint16_t step_size,
                                                     zb_uint16_t transition_time);

/** Macro for calling @ref zb_zcl_color_control_send_enhanced_step_hue_req_zcl8 function
 */
#define ZB_ZCL_COLOR_CONTROL_SEND_ENHANCED_STEP_HUE_REQ_ZCL8(buffer, addr, dst_addr_mode,           \
                                                             dst_ep, ep, prfl_id, def_resp, cb,     \
                                                             step_mode, step_size, transition_time, \
                                                             options_mask, options_override)        \
{                                                                                                   \
  zb_zcl_color_control_send_enhanced_step_hue_req_zcl8(buffer, ZB_ADDR_U_CAST(addr), dst_addr_mode, \
                                                       dst_ep, ep, prfl_id, def_resp, cb,           \
                                                       step_mode, step_size, transition_time,       \
                                                       options_mask, options_override);             \
}

/** Macro for calling @ref zb_zcl_color_control_send_enhanced_step_hue_req function
 */
#define ZB_ZCL_COLOR_CONTROL_SEND_ENHANCED_STEP_HUE_REQ(buffer, addr, dst_addr_mode,                                         \
  dst_ep, ep, prfl_id, def_resp, cb, step_mode, step_size, transition_time)                                                  \
{                                                                                                                            \
  zb_zcl_color_control_send_enhanced_step_hue_req(buffer, ZB_ADDR_U_CAST(addr), dst_addr_mode,                               \
                                                  dst_ep, ep, prfl_id, def_resp, cb, step_mode, step_size, transition_time); \
}

/** @brief Macro for getting Enhanced Step Hue command
  * @attention Assumes that ZCL header already cut.
  * @param buffer containing the packet (by pointer).
  * @param enhanced_step_hue_req - variable of type @ref
  * zb_zcl_color_control_enhanced_step_hue_req_s.
  * @param status - variable to put parse status to (see @ref zb_zcl_parse_status_t).
  */
#define ZB_ZCL_COLOR_CONTROL_GET_ENHANCED_STEP_HUE_REQ(buffer, enhanced_step_hue_req, status)            \
{                                                                                                        \
  zb_zcl_color_control_enhanced_step_hue_req_t *enhanced_step_hue_req_ptr;                               \
  (enhanced_step_hue_req_ptr) = zb_buf_len(buffer) >=                                                    \
    ZB_ZCL_COLOR_CONTROL_ENHANCED_STEP_HUE_REQ_PAYLOAD_LEN ?                                             \
    (zb_zcl_color_control_enhanced_step_hue_req_t*)zb_buf_begin(buffer) : NULL;                          \
  if (enhanced_step_hue_req_ptr != NULL)                                                                 \
  {                                                                                                      \
    enhanced_step_hue_req.step_mode = enhanced_step_hue_req_ptr->step_mode;                              \
    ZB_HTOLE16(&(enhanced_step_hue_req).step_size, &(enhanced_step_hue_req_ptr->step_size));             \
    ZB_HTOLE16(&(enhanced_step_hue_req).transition_time, &(enhanced_step_hue_req_ptr->transition_time)); \
    (void)zb_buf_cut_left(buffer, ZB_ZCL_COLOR_CONTROL_ENHANCED_STEP_HUE_REQ_PAYLOAD_LEN);               \
    status = ZB_ZCL_PARSE_STATUS_SUCCESS;                                                                \
  }                                                                                                      \
  else                                                                                                   \
  {                                                                                                      \
    status = ZB_ZCL_PARSE_STATUS_FAILURE;                                                                \
  }                                                                                                      \
}

/******************************* Enhanced Move to Hue and Saturation ******************************/

/*! @brief Structure representation of Move To Hue and Saturation command payload
    @see ZCL spec, subclause 5.2.2.3.17 */
typedef ZB_PACKED_PRE struct zb_zcl_color_control_enhanced_move_to_hue_saturation_req_s
{
  /** Enhanced Hue */
  zb_uint16_t enhanced_hue;
  /** Saturation */
  zb_uint8_t saturation;
  /** Transition time field */
  zb_uint16_t transition_time;
} ZB_PACKED_STRUCT zb_zcl_color_control_enhanced_move_to_hue_saturation_req_t;

/** @brief Enhanced Move to Hue payload length macro */
#define ZB_ZCL_COLOR_CONTROL_ENHANCED_MOVE_TO_HUE_SATURATION_REQ_PAYLOAD_LEN \
  sizeof(zb_zcl_color_control_enhanced_move_to_hue_saturation_req_t)

/*! @brief Send Enhanced Move to Hue and Saturation command
    @param buffer - to put packet to
    @param dst_addr - address to send packet to
    @param dst_addr_mode - addressing mode
    @param dst_ep - destination endpoint
    @param ep - sending endpoint
    @param prof_id - profile identifier
    @param def_resp - enable/disable default response
    @param cb - callback for getting command send status
    @param enhanced_hue - enhanced hue
    @param saturation - saturation
    @param transition_time - Transition Time value
    @param options_mask - Options Mask
    @param options_override - Options Override
*/
void zb_zcl_color_control_send_enhanced_move_to_hue_saturation_req_zcl8(zb_bufid_t buffer, const zb_addr_u *dst_addr,
                                                                        zb_uint8_t dst_addr_mode, zb_uint8_t dst_ep,
                                                                        zb_uint8_t ep, zb_uint16_t prof_id,
                                                                        zb_uint8_t def_resp, zb_callback_t cb,
                                                                        zb_uint16_t enhanced_hue,
                                                                        zb_uint8_t saturation,
                                                                        zb_uint16_t transition_time,
                                                                        zb_uint8_t options_mask, zb_uint8_t options_override);

/*! @brief Send Enhanced Move to Hue and Saturation command (pre-ZCL8)
    Use @ref zb_zcl_color_control_send_enhanced_move_to_hue_saturation_req_zcl8 for ZCL8 revision call.
    @param buffer - to put packet to
    @param dst_addr - address to send packet to
    @param dst_addr_mode - addressing mode
    @param dst_ep - destination endpoint
    @param ep - sending endpoint
    @param prof_id - profile identifier
    @param def_resp - enable/disable default response
    @param cb - callback for getting command send status
    @param enhanced_hue - enhanced hue
    @param saturation - saturation
    @param transition_time - Transition Time value
*/
void zb_zcl_color_control_send_enhanced_move_to_hue_saturation_req(zb_bufid_t buffer, const zb_addr_u *dst_addr,
                                                                   zb_uint8_t dst_addr_mode, zb_uint8_t dst_ep,
                                                                   zb_uint8_t ep, zb_uint16_t prof_id,
                                                                   zb_uint8_t def_resp, zb_callback_t cb,
                                                                   zb_uint16_t enhanced_hue,
                                                                   zb_uint8_t saturation,
                                                                   zb_uint16_t transition_time);

/** Macro for calling @ref zb_zcl_color_control_send_enhanced_move_to_hue_saturation_req_zcl8 function
 */
#define ZB_ZCL_COLOR_CONTROL_SEND_ENHANCED_MOVE_TO_HUE_SATURATION_REQ_ZCL8(buffer, addr, dst_addr_mode,           \
                                                                           dst_ep, ep, prfl_id, def_resp,         \
                                                                           cb, enhanced_hue, saturation,          \
                                                                           transition_time,                       \
                                                                           options_mask, options_override)        \
{                                                                                                                 \
  zb_zcl_color_control_send_enhanced_move_to_hue_saturation_req_zcl8(buffer, ZB_ADDR_U_CAST(addr), dst_addr_mode, \
                                                                     dst_ep, ep, prfl_id, def_resp,               \
                                                                     cb, enhanced_hue, saturation,                \
                                                                     transition_time,                             \
                                                                     options_mask, options_override);             \
}

/** Macro for calling @ref zb_zcl_color_control_send_enhanced_move_to_hue_saturation_req function
 */
#define ZB_ZCL_COLOR_CONTROL_SEND_ENHANCED_MOVE_TO_HUE_SATURATION_REQ(buffer, addr, dst_addr_mode,                                             \
  dst_ep, ep, prfl_id, def_resp, cb, enhanced_hue, saturation, transition_time)                                                                \
{                                                                                                                                              \
  zb_zcl_color_control_send_enhanced_move_to_hue_saturation_req(buffer, ZB_ADDR_U_CAST(addr), dst_addr_mode,                                   \
                                                                dst_ep, ep, prfl_id, def_resp, cb, enhanced_hue, saturation, transition_time); \
}

/** @brief Macro for getting Enhanced Move To Hue and Saturation command
  * @attention Assumes that ZCL header already cut.
  * @param buffer containing the packet (by pointer).
  * @param enhanced_move_to_hue_saturation_req - variable of type @ref
  * zb_zcl_color_control_enhanced_move_to_hue_saturation_req_s.
  * @param status - variable to put parse status to (see @ref zb_zcl_parse_status_t).
  */
#define ZB_ZCL_COLOR_CONTROL_GET_ENHANCED_MOVE_TO_HUE_SATURATION_REQ(buffer, enhanced_move_to_hue_saturation_req, status)            \
{                                                                                                   \
  zb_zcl_color_control_enhanced_move_to_hue_saturation_req_t *enhanced_move_to_hue_saturation_req_ptr;                               \
  (enhanced_move_to_hue_saturation_req_ptr) = zb_buf_len(buffer) >=                                                                  \
    ZB_ZCL_COLOR_CONTROL_ENHANCED_MOVE_TO_HUE_SATURATION_REQ_PAYLOAD_LEN ?                                                           \
    (zb_zcl_color_control_enhanced_move_to_hue_saturation_req_t*)zb_buf_begin(buffer) : NULL;                                        \
  if (enhanced_move_to_hue_saturation_req_ptr != NULL)                                                                               \
  {                                                                                                 \
    ZB_HTOLE16(&(enhanced_move_to_hue_saturation_req).enhanced_hue, &(enhanced_move_to_hue_saturation_req_ptr->enhanced_hue));       \
    enhanced_move_to_hue_saturation_req.saturation = enhanced_move_to_hue_saturation_req_ptr->saturation;                            \
    ZB_HTOLE16(&(enhanced_move_to_hue_saturation_req).transition_time, &(enhanced_move_to_hue_saturation_req_ptr->transition_time)); \
    (void)zb_buf_cut_left(buffer, ZB_ZCL_COLOR_CONTROL_ENHANCED_MOVE_TO_HUE_SATURATION_REQ_PAYLOAD_LEN);                             \
    status = ZB_ZCL_PARSE_STATUS_SUCCESS;                                                                                            \
  }                                                                                                 \
  else                                                                                              \
  {                                                                                                 \
    status = ZB_ZCL_PARSE_STATUS_FAILURE;                                                                                            \
  }                                                                                                 \
}

/******************************* Color Loop Set ******************************/

/*! @brief Structure representation of Color Loop Set command payload
    @see ZCL8 spec, subclause 5.2.2.3.19 */
typedef ZB_PACKED_PRE struct zb_zcl_color_control_color_loop_set_req_s
{
  /** Update flags, see @ref zb_zcl_color_control_color_loop_update_e */
  zb_uint8_t update_flags;
  /** Action, see @ref zb_zcl_color_control_color_loop_action_e */
  zb_uint8_t action;
  /** Direction, see @ref zb_zcl_color_control_color_loop_direction_e */
  zb_uint8_t direction;
  /** Time */
  zb_uint16_t time;
  /** Start hue */
  zb_uint16_t start_hue;
} ZB_PACKED_STRUCT zb_zcl_color_control_color_loop_set_req_t;

/** @brief Enhanced Color Loop Set payload length macro */
#define ZB_ZCL_COLOR_CONTROL_COLOR_LOOR_SET_REQ_PAYLOAD_LEN \
  sizeof(zb_zcl_color_control_color_loop_set_req_t)

/** @brief Update flag for Color Loop Set command
 *  See ZCL specs 5.2.2.3.18.1 */
enum zb_zcl_color_control_color_loop_update_e
{
  /** Update action */
  ZB_ZCL_CMD_COLOR_CONTROL_LOOP_UPDATE_ACTION       = 1 << 0,
  /** Update direction */
  ZB_ZCL_CMD_COLOR_CONTROL_LOOP_UPDATE_DIRECTION    = 1 << 1,
  /** Update time */
  ZB_ZCL_CMD_COLOR_CONTROL_LOOP_UPDATE_TIME         = 1 << 2,
  /** Update start hue */
  ZB_ZCL_CMD_COLOR_CONTROL_LOOP_UPDATE_START_HUE    = 1 << 3,
};

/** @brief Action enumerate for Color Loop Set command
 *  See ZCL specs 5.2.2.3.18.2 */
enum zb_zcl_color_control_color_loop_action_e
{
  /** De-activate the color loop */
  ZB_ZCL_CMD_COLOR_CONTROL_LOOP_ACTION_DEACTIVATE   = 0,
  /** Activate the color loop from the value in the Color Loop Start Enhanced Hue field*/
  ZB_ZCL_CMD_COLOR_CONTROL_LOOP_ACTION_START_HUE    = 1,
  /** Activate the color loop from the value of the Enhanced Current Hue attribute*/
  ZB_ZCL_CMD_COLOR_CONTROL_LOOP_ACTION_CURRENT_HUE  = 2,
};

/** @brief Direction enumerate for Color Loop Set command
 *  See ZCL specs 5.2.2.3.18.3 */
enum zb_zcl_color_control_color_loop_direction_e
{
  /** Decrement the hue in the color loop */
  ZB_ZCL_CMD_COLOR_CONTROL_LOOP_DIRECTION_DECREMENT     = 0,
  /** Increment the hue in the color loop */
  ZB_ZCL_CMD_COLOR_CONTROL_LOOP_DIRECTION_INCREMENT     = 1,
};

/*! @internal
 *  @brief Structure for loop of Color Loop Set command */
typedef struct zb_zcl_color_control_color_loop_set_s
{
  /** Command header */
  zb_zcl_parsed_hdr_t cmd_info;

  /** Last time - beacon interval */
  zb_time_t last_time;

} zb_zcl_color_control_color_loop_set_t;

#define ZB_ZCL_COLOR_CONTROL_FILL_COLOR_LOOP_SET_DATA(buf, cmd_info_)           \
{                                                                               \
  zb_zcl_color_control_color_loop_set_t *loop_data =                            \
          ZB_BUF_GET_PARAM((buf), zb_zcl_color_control_color_loop_set_t);       \
  ZB_MEMMOVE(&(loop_data->cmd_info), (cmd_info_), sizeof(zb_zcl_parsed_hdr_t)); \
  loop_data->last_time = ZB_TIMER_GET();                                        \
}

/*! @brief Send Color Loop Set command
    @param buffer - to put packet to
    @param dst_addr - address to send packet to
    @param dst_addr_mode - addressing mode
    @param dst_ep - destination endpoint
    @param ep - sending endpoint
    @param prof_id - profile identifier
    @param def_resp - enable/disable default response
    @param cb - callback for getting command send status
    @param update_flags - update flags, see @ref zb_zcl_color_control_color_loop_update_e
    @param action - action, see @ref zb_zcl_color_control_color_loop_action_e
    @param direction - direction, see @ref zb_zcl_color_control_color_loop_direction_e
    @param time - time
    @param start_hue - start hue
    @param options_mask - Options Mask
    @param options_override - Options Override
*/
void zb_zcl_color_control_send_color_loop_set_req_zcl8(zb_bufid_t buffer, const zb_addr_u *dst_addr,
                                                       zb_uint8_t dst_addr_mode, zb_uint8_t dst_ep,
                                                       zb_uint8_t ep, zb_uint16_t prof_id,
                                                       zb_uint8_t def_resp, zb_callback_t cb,
                                                       zb_uint8_t update_flags,
                                                       zb_uint8_t action,
                                                       zb_uint8_t direction,
                                                       zb_uint16_t time,
                                                       zb_uint16_t start_hue,
                                                       zb_uint8_t options_mask, zb_uint8_t options_override);

/*! @brief Send Color Loop Set command (pre-ZCL8)
    Use @ref zb_zcl_color_control_send_color_loop_set_req_zcl8 for ZCL8 revision call.
    @param buffer - to put packet to
    @param dst_addr - address to send packet to
    @param dst_addr_mode - addressing mode
    @param dst_ep - destination endpoint
    @param ep - sending endpoint
    @param prof_id - profile identifier
    @param def_resp - enable/disable default response
    @param cb - callback for getting command send status
    @param update_flags - update flags, see @ref zb_zcl_color_control_color_loop_update_e
    @param action - action, see @ref zb_zcl_color_control_color_loop_action_e
    @param direction - direction, see @ref zb_zcl_color_control_color_loop_direction_e
    @param time - time
    @param start_hue - start hue
*/
void zb_zcl_color_control_send_color_loop_set_req(zb_bufid_t buffer, const zb_addr_u *dst_addr,
                                                  zb_uint8_t dst_addr_mode, zb_uint8_t dst_ep,
                                                  zb_uint8_t ep, zb_uint16_t prof_id,
                                                  zb_uint8_t def_resp, zb_callback_t cb,
                                                  zb_uint8_t update_flags,
                                                  zb_uint8_t action,
                                                  zb_uint8_t direction,
                                                  zb_uint16_t time,
                                                  zb_uint16_t start_hue);

/** Macro for calling @ref zb_zcl_color_control_send_color_loop_set_req_zcl8 function
 */
#define ZB_ZCL_COLOR_CONTROL_SEND_COLOR_LOOP_SET_REQ_ZCL8(buffer, addr, dst_addr_mode,                    \
                                                          dst_ep, ep, prfl_id, def_resp, cb, update_flags,\
                                                          action, direction, time, start_hue,             \
                                                          options_mask, options_override)                 \
{                                                                                                         \
  zb_zcl_color_control_send_color_loop_set_req_zcl8(buffer, ZB_ADDR_U_CAST(addr), dst_addr_mode,          \
                                                    dst_ep, ep, prfl_id, def_resp, cb, update_flags,      \
                                                    action, direction, time, start_hue,                   \
                                                    options_mask, options_override);                      \
}

/** Macro for calling @ref zb_zcl_color_control_send_color_loop_set_req function
 */
#define ZB_ZCL_COLOR_CONTROL_SEND_COLOR_LOOP_SET_REQ(buffer, addr, dst_addr_mode,                                                   \
  dst_ep, ep, prfl_id, def_resp, cb, update_flags, action, direction, time, start_hue)                                              \
{                                                                                                                                   \
  zb_zcl_color_control_send_color_loop_set_req(buffer, ZB_ADDR_U_CAST(addr), dst_addr_mode,                                         \
                                               dst_ep, ep, prfl_id, def_resp, cb, update_flags, action, direction, time, start_hue);\
}

/** @brief Macro for getting Color Loop Set command
  * @attention Assumes that ZCL header already cut.
  * @param buffer containing the packet (by pointer).
  * @param color_loop_set_req - pointer to a variable of
  * type @ref zb_zcl_color_control_color_loop_set_req_t
  * @param status - variable to put parse status to (see @ref zb_zcl_parse_status_t).
  */
#define ZB_ZCL_COLOR_CONTROL_GET_COLOR_LOOP_SET_REQ(buffer, color_loop_set_req, status)  \
{                                                                                        \
  zb_zcl_color_control_color_loop_set_req_t *color_loop_set_req_ptr;                     \
  (color_loop_set_req_ptr) = zb_buf_len(buffer) >=                                       \
    ZB_ZCL_COLOR_CONTROL_COLOR_LOOR_SET_REQ_PAYLOAD_LEN ?                                \
    (zb_zcl_color_control_color_loop_set_req_t*)zb_buf_begin(buffer) : NULL;             \
  if (color_loop_set_req_ptr != NULL)                                                    \
  {                                                                                      \
    color_loop_set_req.update_flags = color_loop_set_req_ptr->update_flags;              \
    color_loop_set_req.action = color_loop_set_req_ptr->action;                          \
    color_loop_set_req.direction = color_loop_set_req_ptr->direction;                    \
    ZB_HTOLE16(&(color_loop_set_req).time, &(color_loop_set_req_ptr->time));             \
    ZB_HTOLE16(&(color_loop_set_req).start_hue, &(color_loop_set_req_ptr->start_hue));   \
    (void)zb_buf_cut_left(buffer, ZB_ZCL_COLOR_CONTROL_COLOR_LOOR_SET_REQ_PAYLOAD_LEN);  \
    status = ZB_ZCL_PARSE_STATUS_SUCCESS;                                                \
  }                                                                                      \
  else                                                                                   \
  {                                                                                      \
    status = ZB_ZCL_PARSE_STATUS_FAILURE;                                                \
  }                                                                                      \
}

/******************************* Stop move step ******************************/

/*! @brief Stop move step command, see ZCL8 spec 5.2.2.3.20
    @param buffer - to put packet to
    @param dst_addr - address to send packet to
    @param dst_addr_mode - addressing mode
    @param dst_ep - destination endpoint
    @param ep - sending endpoint
    @param prof_id - profile identifier
    @param def_resp - enable/disable default response
    @param cb - callback for getting command send status
    @param options_mask - Options Mask
    @param options_override - Options Override
*/
void zb_zcl_color_control_send_stop_move_step_req_zcl8(zb_bufid_t buffer, const zb_addr_u *dst_addr,
                                                       zb_uint8_t dst_addr_mode, zb_uint8_t dst_ep,
                                                       zb_uint8_t ep, zb_uint16_t prof_id,
                                                       zb_uint8_t def_resp, zb_callback_t cb,
                                                       zb_uint8_t options_mask, zb_uint8_t options_override);

/*! @brief Stop move step command (pre-ZCL8)
    Use @ref zb_zcl_color_control_send_stop_move_step_req_zcl8 for ZCL8 revision call.
    @param buffer - to put packet to
    @param dst_addr - address to send packet to
    @param dst_addr_mode - addressing mode
    @param dst_ep - destination endpoint
    @param ep - sending endpoint
    @param prof_id - profile identifier
    @param def_resp - enable/disable default response
    @param cb - callback for getting command send status
*/
void zb_zcl_color_control_send_stop_move_step_req(zb_bufid_t buffer, const zb_addr_u *dst_addr,
                                                  zb_uint8_t dst_addr_mode, zb_uint8_t dst_ep,
                                                  zb_uint8_t ep, zb_uint16_t prof_id,
                                                  zb_uint8_t def_resp, zb_callback_t cb);

/** Macro for calling @ref zb_zcl_color_control_send_stop_move_step_req_zcl8 function
 */
#define ZB_ZCL_COLOR_CONTROL_SEND_STOP_MOVE_STEP_REQ_ZCL8(buffer, addr, dst_addr_mode,           \
                                                          dst_ep, ep, prfl_id, def_resp, cb,     \
                                                          options_mask, options_override)        \
{                                                                                                \
  zb_zcl_color_control_send_stop_move_step_req_zcl8(buffer, ZB_ADDR_U_CAST(addr), dst_addr_mode, \
                                                    dst_ep, ep, prfl_id, def_resp, cb,           \
                                                    options_mask, options_override);             \
}

/** Macro for calling @ref zb_zcl_color_control_send_stop_move_step_req function
 */
#define ZB_ZCL_COLOR_CONTROL_SEND_STOP_MOVE_STEP_REQ(buffer, addr, dst_addr_mode,           \
  dst_ep, ep, prfl_id, def_resp, cb)                                                        \
{                                                                                           \
  zb_zcl_color_control_send_stop_move_step_req(buffer, ZB_ADDR_U_CAST(addr), dst_addr_mode, \
                                               dst_ep, ep, prfl_id, def_resp, cb);          \
}

/******************************* Move color temperature command ******************************/

/*! @brief Structure representation of Move color temperature command payload
    @see ZCL8 spec, subclause 5.2.2.3.21 */
typedef ZB_PACKED_PRE struct zb_zcl_color_control_move_color_temp_req_s
{
  /** Move mode, see @ref zb_zcl_color_control_move_direction_e */
  zb_uint8_t move_mode;
  /** Rate */
  zb_uint16_t rate;
  /** Color temperature minimum */
  zb_uint16_t color_temp_min;
  /** Color temperature maximum */
  zb_uint16_t color_temp_max;
} ZB_PACKED_STRUCT zb_zcl_color_control_move_color_temp_req_t;

/** @brief Enhanced Move Color Temperature payload length macro */
#define ZB_ZCL_COLOR_CONTROL_MOVE_COLOR_TEMP_REQ_PAYLOAD_LEN \
  sizeof(zb_zcl_color_control_move_color_temp_req_t)

/*! @brief Send Move color temperature command
    @param buffer - to put packet to
    @param dst_addr - address to send packet to
    @param dst_addr_mode - addressing mode
    @param dst_ep - destination endpoint
    @param ep - sending endpoint
    @param prof_id - profile identifier
    @param def_resp - enable/disable default response
    @param cb - callback for getting command send status
    @param move_mode - move mode, see @ref zb_zcl_color_control_move_direction_e
    @param rate - rate
    @param color_temp_min - color temperature minimum
    @param color_temp_max - color temperature maximum
    @param options_mask - Options Mask
    @param options_override - Options Override
*/
void zb_zcl_color_control_send_move_color_temp_req_zcl8(zb_bufid_t buffer, const zb_addr_u *dst_addr,
                                                       zb_uint8_t dst_addr_mode, zb_uint8_t dst_ep,
                                                       zb_uint8_t ep, zb_uint16_t prof_id,
                                                       zb_uint8_t def_resp, zb_callback_t cb,
                                                       zb_uint8_t move_mode,
                                                       zb_uint16_t rate,
                                                       zb_uint16_t color_temp_min,
                                                       zb_uint16_t color_temp_max,
                                                       zb_uint8_t options_mask, zb_uint8_t options_override);

/*! @brief Send Move color temperature command (pre-ZCL8)
    Use @ref zb_zcl_color_control_send_move_color_temp_req_zcl8 for ZCL8 revision call.
    @param buffer - to put packet to
    @param dst_addr - address to send packet to
    @param dst_addr_mode - addressing mode
    @param dst_ep - destination endpoint
    @param ep - sending endpoint
    @param prof_id - profile identifier
    @param def_resp - enable/disable default response
    @param cb - callback for getting command send status
    @param move_mode - move mode, see @ref zb_zcl_color_control_move_direction_e
    @param rate - rate
    @param color_temp_min - color temperature minimum
    @param color_temp_max - color temperature maximum
*/
void zb_zcl_color_control_send_move_color_temp_req(zb_bufid_t buffer, const zb_addr_u *dst_addr,
                                                  zb_uint8_t dst_addr_mode, zb_uint8_t dst_ep,
                                                  zb_uint8_t ep, zb_uint16_t prof_id,
                                                  zb_uint8_t def_resp, zb_callback_t cb,
                                                  zb_uint8_t move_mode,
                                                  zb_uint16_t rate,
                                                  zb_uint16_t color_temp_min,
                                                  zb_uint16_t color_temp_max);

/** Macro for calling @ref zb_zcl_color_control_send_move_color_temp_req_zcl8 function
 */
#define ZB_ZCL_COLOR_CONTROL_SEND_MOVE_COLOR_TEMP_REQ_ZCL8(buffer, addr, dst_addr_mode,                  \
                                                           dst_ep, ep, prfl_id, def_resp, cb, move_mode, \
                                                           rate, color_temp_min, color_temp_max,         \
                                                           options_mask, options_override)               \
{                                                                                                        \
  zb_zcl_color_control_send_move_color_temp_req_zcl8(buffer, ZB_ADDR_U_CAST(addr), dst_addr_mode,        \
                                                     dst_ep, ep, prfl_id, def_resp, cb, move_mode,       \
                                                     rate, color_temp_min, color_temp_max,               \
                                                     options_mask, options_override);                    \
}

/** Macro for calling @ref zb_zcl_color_control_send_move_color_temp_req function
 */
#define ZB_ZCL_COLOR_CONTROL_SEND_MOVE_COLOR_TEMP_REQ(buffer, addr, dst_addr_mode,                                                   \
  dst_ep, ep, prfl_id, def_resp, cb, move_mode, rate, color_temp_min, color_temp_max)                                                \
{                                                                                                                                    \
  zb_zcl_color_control_send_move_color_temp_req(buffer, ZB_ADDR_U_CAST(addr), dst_addr_mode,                                         \
                                                dst_ep, ep, prfl_id, def_resp, cb, move_mode, rate, color_temp_min, color_temp_max); \
}

/** @brief Macro for getting Move color temperature command
  * @attention Assumes that ZCL header already cut.
  * @param buffer containing the packet (by pointer).
  * @param move_color_temp_req - variable of type @ref
  * zb_zcl_color_control_move_color_temp_req_s.
  * @param status - variable to put parse status to (see @ref zb_zcl_parse_status_t).
  */
#define ZB_ZCL_COLOR_CONTROL_GET_MOVE_COLOR_TEMP_REQ(buffer, move_color_temp_req, status)          \
{                                                                                                  \
  zb_zcl_color_control_move_color_temp_req_t *move_color_temp_req_ptr;                             \
  (move_color_temp_req_ptr) = zb_buf_len(buffer) >=                                                \
    ZB_ZCL_COLOR_CONTROL_MOVE_COLOR_TEMP_REQ_PAYLOAD_LEN ?                                         \
    (zb_zcl_color_control_move_color_temp_req_t*)zb_buf_begin(buffer) : NULL;                      \
  if (move_color_temp_req_ptr != NULL)                                                             \
  {                                                                                                \
    ZB_MEMCPY(&(move_color_temp_req), move_color_temp_req_ptr, ZB_ZCL_COLOR_CONTROL_MOVE_COLOR_TEMP_REQ_PAYLOAD_LEN); \
    ZB_HTOLE16_ONPLACE((move_color_temp_req).rate);                                                \
    ZB_HTOLE16_ONPLACE((move_color_temp_req).color_temp_min);                                      \
    ZB_HTOLE16_ONPLACE((move_color_temp_req).color_temp_max);                                      \
    (void)zb_buf_cut_left(buffer, ZB_ZCL_COLOR_CONTROL_MOVE_COLOR_TEMP_REQ_PAYLOAD_LEN);           \
    status = ZB_ZCL_PARSE_STATUS_SUCCESS;                                                          \
  }                                                                                                \
  else                                                                                             \
  {                                                                                                \
    status = ZB_ZCL_PARSE_STATUS_FAILURE;                                                          \
  }                                                                                                \
}

/******************************* Step color temperature command ******************************/

/*! @brief Structure representation of Step color temperature command command payload
    @see ZCL8 spec, subclause 5.2.2.3.22 */
typedef ZB_PACKED_PRE struct zb_zcl_color_control_step_color_temp_req_s
{
  /** Step mode, see @ref zb_zcl_color_control_step_mode_e */
  zb_uint8_t step_mode;
  /** Step size */
  zb_uint16_t step_size;
  /** Transition time field */
  zb_uint16_t transition_time;
  /** Color temperature minimum */
  zb_uint16_t color_temp_min;
  /** Color temperature maximum */
  zb_uint16_t color_temp_max;
} ZB_PACKED_STRUCT zb_zcl_color_control_step_color_temp_req_t;

/** @brief Enhanced Move Color Temperature payload length macro */
#define ZB_ZCL_COLOR_CONTROL_STEP_COLOR_TEMP_REQ_PAYLOAD_LEN \
  sizeof(zb_zcl_color_control_step_color_temp_req_t)

/*! @brief Step color temperature command
    @param buffer - to put packet to
    @param dst_addr - address to send packet to
    @param dst_addr_mode - addressing mode
    @param dst_ep - destination endpoint
    @param ep - sending endpoint
    @param prof_id - profile identifier
    @param def_resp - enable/disable default response
    @param cb - callback for getting command send status
    @param step_mode - move mode, see @ref zb_zcl_color_control_step_mode_e
    @param step_size - step size
    @param transition_time - Transition Time value
    @param color_temp_min - color temperature minimum
    @param color_temp_max - color temperature maximum
    @param options_mask - Options Mask
    @param options_override - Options Override
*/
void zb_zcl_color_control_send_step_color_temp_req_zcl8(zb_bufid_t buffer, const zb_addr_u *dst_addr,
                                                        zb_uint8_t dst_addr_mode, zb_uint8_t dst_ep,
                                                        zb_uint8_t ep, zb_uint16_t prof_id,
                                                        zb_uint8_t def_resp, zb_callback_t cb,
                                                        zb_uint8_t step_mode,
                                                        zb_uint16_t step_size,
                                                        zb_uint16_t transition_time,
                                                        zb_uint16_t color_temp_min,
                                                        zb_uint16_t color_temp_max,
                                                        zb_uint8_t options_mask, zb_uint8_t options_override);

/*! @brief Step color temperature command (pre-ZCL8)
    Use @ref zb_zcl_color_control_send_step_color_temp_req_zcl8 for ZCL8 revision call.
    @param buffer - to put packet to
    @param dst_addr - address to send packet to
    @param dst_addr_mode - addressing mode
    @param dst_ep - destination endpoint
    @param ep - sending endpoint
    @param prof_id - profile identifier
    @param def_resp - enable/disable default response
    @param cb - callback for getting command send status
    @param step_mode - move mode, see @ref zb_zcl_color_control_step_mode_e
    @param step_size - step size
    @param transition_time - Transition Time value
    @param color_temp_min - color temperature minimum
    @param color_temp_max - color temperature maximum
*/
void zb_zcl_color_control_send_step_color_temp_req(zb_bufid_t buffer, const zb_addr_u *dst_addr,
                                                   zb_uint8_t dst_addr_mode, zb_uint8_t dst_ep,
                                                   zb_uint8_t ep, zb_uint16_t prof_id,
                                                   zb_uint8_t def_resp, zb_callback_t cb,
                                                   zb_uint8_t step_mode,
                                                   zb_uint16_t step_size,
                                                   zb_uint16_t transition_time,
                                                   zb_uint16_t color_temp_min,
                                                   zb_uint16_t color_temp_max);

/** Macro for calling @ref zb_zcl_color_control_send_step_color_temp_req_zcl8 function
 */
#define ZB_ZCL_COLOR_CONTROL_SEND_STEP_COLOR_TEMP_REQ_ZCL8(buffer, addr, dst_addr_mode,                  \
                                                           dst_ep, ep, prfl_id, def_resp, cb, step_mode, \
                                                           step_size, transition_time, color_temp_min,   \
                                                           color_temp_max,                               \
                                                           options_mask, options_override)               \
{                                                                                                        \
  zb_zcl_color_control_send_step_color_temp_req_zcl8(buffer, ZB_ADDR_U_CAST(addr), dst_addr_mode,        \
                                                     dst_ep, ep, prfl_id, def_resp, cb, step_mode,       \
                                                     step_size, transition_time, color_temp_min,         \
                                                     color_temp_max,                                     \
                                                     options_mask, options_override);                    \
}

/** Macro for calling @ref zb_zcl_color_control_send_step_color_temp_req function
 */
#define ZB_ZCL_COLOR_CONTROL_SEND_STEP_COLOR_TEMP_REQ(buffer, addr, dst_addr_mode,                                                                         \
  dst_ep, ep, prfl_id, def_resp, cb, step_mode, step_size, transition_time, color_temp_min, color_temp_max)                                                \
{                                                                                                                                                          \
  zb_zcl_color_control_send_step_color_temp_req(buffer, ZB_ADDR_U_CAST(addr), dst_addr_mode,                                                               \
                                                dst_ep, ep, prfl_id, def_resp, cb, step_mode, step_size, transition_time, color_temp_min, color_temp_max); \
}

/** @brief Macro for getting Move color temperature command
  * @attention Assumes that ZCL header already cut.
  * @param buffer containing the packet (by pointer).
  * @param step_color_temp_req - variable of type @ref
  * zb_zcl_color_control_step_color_temp_req_s.
  * @param status - variable to put parse status to (see @ref zb_zcl_parse_status_t).
  */
#define ZB_ZCL_COLOR_CONTROL_GET_STEP_COLOR_TEMP_REQ(buffer, step_color_temp_req, status)            \
{                                                                                                    \
  zb_zcl_color_control_step_color_temp_req_t *step_color_temp_req_ptr;                               \
  (step_color_temp_req_ptr) = zb_buf_len(buffer) >=                                                  \
    ZB_ZCL_COLOR_CONTROL_STEP_COLOR_TEMP_REQ_PAYLOAD_LEN ?                                           \
    (zb_zcl_color_control_step_color_temp_req_t*)zb_buf_begin(buffer) : NULL;                        \
  if (step_color_temp_req_ptr != NULL)                                                               \
  {                                                                                                  \
    step_color_temp_req.step_mode = step_color_temp_req_ptr->step_mode;                              \
    ZB_HTOLE16(&(step_color_temp_req).step_size, &(step_color_temp_req_ptr->step_size));             \
    ZB_HTOLE16(&(step_color_temp_req).transition_time, &(step_color_temp_req_ptr->transition_time)); \
    ZB_HTOLE16(&(step_color_temp_req).color_temp_min, &(step_color_temp_req_ptr->color_temp_min));   \
    ZB_HTOLE16(&(step_color_temp_req).color_temp_max, &(step_color_temp_req_ptr->color_temp_max));   \
    (void)zb_buf_cut_left(buffer, ZB_ZCL_COLOR_CONTROL_STEP_COLOR_TEMP_REQ_PAYLOAD_LEN);             \
    status = ZB_ZCL_PARSE_STATUS_SUCCESS;                                                            \
  }                                                                                                  \
  else                                                                                               \
  {                                                                                                  \
    status = ZB_ZCL_PARSE_STATUS_FAILURE;                                                            \
  }                                                                                                  \
}

/*! @} */ /* Color Control cluster commands */

/** @cond internals_doc
 *
    @name Added struct and define for specific Color Control cluster commands
    @{
*/

#define ZB_ZCL_COLOR_CONTROL_SEC_TO_MSEC        1000

/** Standard Color Control time uint = 1/10 sec */
#define ZB_ZCL_COLOR_CONTROL_TIMER_INTERVAL    100

/** Standard Color Control time uint = 1/10 sec, beacon */
#define ZB_ZCL_COLOR_CONTROL_TIMER_SYS_INTERVAL    \
    ZB_MILLISECONDS_TO_SYS_TIMER_INTERVAL(ZB_ZCL_COLOR_CONTROL_TIMER_INTERVAL)

/**
 * @brief Struct for process one iteration of move command for one attribute
 */
typedef struct zb_zcl_color_control_loop_element_s
{
  zb_uint8_t endpoint;      /** Device endpoint */
  zb_uint8_t buffer_id;     /** buffer for invoke User App */

  zb_uint16_t attr_id;      /** Attribute ID */
  zb_int16_t value;         /** delta value */
  zb_bool_t is_continue;   /** If ZB_TRUE attribute when is already limit value, command
                                contentious from another limit. If ZB_FALSE attribute when
                                is already limit value, command stop */
  zb_uint16_t limit;        /** limit of attribute value */

} zb_zcl_color_control_loop_element_t;

/**
 * @brief Define for fill struct process step command for one attribute,
 * see @ref zb_zcl_color_control_loop_element_s
 *
 * @param el_data - struct see @ref zb_zcl_color_control_loop_element_s 'zb_zcl_color_control_loop_element_t'
 * @param endp - endpoint
 * @param param - buffer for invoke User App
 * @param attr - attribute ID
 * @param value_ - value
 * @param is_continue_ - continue if limit dest
 * @param limit_ - limit of attribute value, min or max - depend of sing of rate field
 */
#define ZB_ZCL_COLOR_CONTROL_FILL_LOOP_ELEMENT(el_data, endp,   \
               param, attr, value_, is_continue_, limit_)       \
  (el_data).endpoint = (endp);                                  \
  (el_data).buffer_id = (param);                                \
  (el_data).attr_id = (attr);                                   \
  (el_data).value = (value_);                                   \
  (el_data).is_continue = (is_continue_);                       \
  (el_data).limit = (limit_);


/**
 * @brief Copy 16bit attribute value for Color Control cluster
 *
 * @param endpoint - device endpoint
 * @param attr_id_to - destination attribute ID
 * @param attr_id_from - source attribute ID
 */
#define ZB_ZCL_COLOR_CONTROL_COPY_ATTRIBUTE_16(endpoint, attr_id_to, attr_id_from)      \
{                                                                                       \
  zb_uint16_t value;                                                                    \
  zb_zcl_attr_t * attr_desc = zb_zcl_get_attr_desc_a((endpoint),                        \
      ZB_ZCL_CLUSTER_ID_COLOR_CONTROL, ZB_ZCL_CLUSTER_SERVER_ROLE, (attr_id_from));     \
  ZB_ASSERT(attr_desc);                                                                 \
  value = ZB_ZCL_GET_ATTRIBUTE_VAL_16(attr_desc);                                       \
                                                                                        \
  attr_desc = zb_zcl_get_attr_desc_a((endpoint),                                        \
        ZB_ZCL_CLUSTER_ID_COLOR_CONTROL, ZB_ZCL_CLUSTER_SERVER_ROLE, (attr_id_to));     \
  ZB_ASSERT(attr_desc);                                                                 \
  ZB_ZCL_SET_DIRECTLY_ATTR_VAL16(attr_desc, value);                                     \
}

/**
 * @brief Struct invoke User App and set attribute value
 */
typedef struct zb_zcl_color_control_invoke_user_s
{
  zb_uint8_t endpoint;
  zb_uint16_t attr_id;
  zb_uint16_t new_value;        /** New value attribute */
} zb_zcl_color_control_invoke_user_t;

/** @}
 *
 * @endcond */ /* internals_doc */


/*! @} */ /* ZCL Color Control cluster definitions */

/** @endcond */ /* DOXYGEN_ZCL_SECTION */

void zb_zcl_color_control_init_server(void);
void zb_zcl_color_control_init_client(void);

#define ZB_ZCL_CLUSTER_ID_COLOR_CONTROL_SERVER_ROLE_INIT zb_zcl_color_control_init_server
#define ZB_ZCL_CLUSTER_ID_COLOR_CONTROL_CLIENT_ROLE_INIT zb_zcl_color_control_init_client

#endif /* ZB_ZCL_COLOR_CONTROL_H */
