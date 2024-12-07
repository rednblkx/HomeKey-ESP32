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
/* PURPOSE: ZBOSS Zigbee cluster library API header
*/
#ifndef ZBOSS_API_ZCL_H
#define ZBOSS_API_ZCL_H 1

#include "zb_types.h"

#include "zcl/zb_zcl_config.h"

#include "zcl/zb_zcl_common.h"
#include "zcl/zb_zcl_commands.h"

#if defined (ZB_NO_SINGLE_PRECISION_DATA_TYPE)

/*
 * The following ZCL clusters use zb_single_t, which is not supported
 * if ZB_NO_SINGLE_PRECISION_DATA_TYPE is defined.
 */

// TODO: Analog Input (Basic) also uses zb_single_t
#undef ZB_ZCL_SUPPORT_CLUSTER_ANALOG_VALUE
#undef ZB_ZCL_SUPPORT_CLUSTER_MULTISTATE_VALUE

/* All 4.13 Concentration Measurement clusters */
#undef ZB_ZCL_SUPPORT_CLUSTER_PM2_5_MEASUREMENT
#undef ZB_ZCL_SUPPORT_CLUSTER_CARBON_DIOXIDE_MEASUREMENT

#endif /* ZB_NO_SINGLE_PRECISION_DATA_TYPE */

#if !(defined ZB_ZCL_DISABLE_REPORTING)
#include "zcl/zb_zcl_reporting.h"
#endif

#if defined (ZB_ZCL_SUPPORT_CLUSTER_SCENES)
#include "zcl/zb_zcl_scenes.h"
#endif
#if defined (ZB_ZCL_SUPPORT_CLUSTER_BASIC)
#include "zcl/zb_zcl_basic.h"
#endif
#if defined (ZB_ZCL_SUPPORT_CLUSTER_ALARMS)
#include "zcl/zb_zcl_alarms.h"
#endif
#if defined (ZB_ZCL_SUPPORT_CLUSTER_POWER_CONFIG)
#include "zcl/zb_zcl_power_config.h"
#endif
#if defined(ZB_ZCL_SUPPORT_CLUSTER_IDENTIFY)
#include "zcl/zb_zcl_identify.h"
#endif
#if defined (ZB_ZCL_SUPPORT_CLUSTER_ON_OFF_SWITCH_CONFIG)
#include "zcl/zb_zcl_on_off_switch_conf.h"
#endif
#if defined (ZB_ZCL_SUPPORT_CLUSTER_ON_OFF) || defined (ZB_ZCL_SUPPORT_CLUSTER_LEVEL_CONTROL)
#include "zcl/zb_zcl_on_off.h"
#endif
#if defined (ZB_ZCL_SUPPORT_CLUSTER_GROUPS)
#include "zcl/zb_zcl_groups.h"
#endif
#if defined (ZB_ZCL_SUPPORT_CLUSTER_DOOR_LOCK)
#include "zcl/zb_zcl_door_lock.h"
#endif
#if defined (ZB_ZCL_SUPPORT_CLUSTER_ANALOG_INPUT)
#include "zcl/zb_zcl_analog_input.h"
#endif
#if defined (ZB_ZCL_SUPPORT_CLUSTER_ANALOG_VALUE)
#include "zcl/zb_zcl_analog_value.h"
#endif
#if defined (ZB_ZCL_SUPPORT_CLUSTER_BINARY_INPUT)
#include "zcl/zb_zcl_binary_input.h"
#endif
#if defined (ZB_ZCL_SUPPORT_CLUSTER_MULTISTATE_INPUT)
#include "zcl/zb_zcl_multistate_input.h"
#endif
#if defined (ZB_ZCL_SUPPORT_CLUSTER_MULTISTATE_VALUE)
#include "zcl/zb_zcl_multistate_value.h"
#endif
#if defined (ZB_ZCL_SUPPORT_CLUSTER_CARBON_DIOXIDE_MEASUREMENT)
#include "zcl/zb_zcl_carbon_dioxide_measurement.h"
#endif
#if defined (ZB_ZCL_SUPPORT_CLUSTER_PM2_5_MEASUREMENT)
#include "zcl/zb_zcl_pm2_5_measurement.h"
#endif
#if defined (ZB_ZCL_SUPPORT_CLUSTER_DEVICE_TEMP_CONFIG)
#include "zcl/zb_zcl_device_temp_config.h"
#endif
#if defined (ZB_ZCL_SUPPORT_CLUSTER_LEVEL_CONTROL)
#ifndef ZB_CVC_FEATURE_SUPPORT
#define ZB_CVC_FEATURE_SUPPORT
#endif
#include "zcl/zb_zcl_level_control.h"
#endif
#if defined ZB_ZCL_SUPPORT_CLUSTER_WINDOW_COVERING
#include "zcl/zb_zcl_window_covering.h"
#endif
#if defined (ZB_ZCL_SUPPORT_CLUSTER_COLOR_CONTROL)
#include "zcl/zb_zcl_color_control.h"
#endif
#if defined (ZB_ZCL_SUPPORT_CLUSTER_SHADE_CONFIG)
#include "zcl/zb_zcl_shade_config.h"
#endif
#include "zcl/zb_zcl_cvc_commands.h"

#ifdef ZB_CONTROL4_NETWORK_SUPPORT
#include "zcl/zb_zcl_control4_networking.h"
#endif

#if defined (ZB_ZCL_SUPPORT_CLUSTER_TEMP_MEASUREMENT)
#include "zcl/zb_zcl_temp_measurement.h"
#endif
#if defined (ZB_ZCL_SUPPORT_CLUSTER_PRESSURE_MEASUREMENT)
#include "zcl/zb_zcl_pressure_measurement.h"
#endif
#if defined (ZB_ZCL_SUPPORT_CLUSTER_REL_HUMIDITY_MEASUREMENT)
#include "zcl/zb_zcl_rel_humidity_measurement.h"
#endif
#if defined (ZB_ZCL_SUPPORT_CLUSTER_ILLUMINANCE_MEASUREMENT)
#include "zcl/zb_zcl_illuminance_measurement.h"
#endif

#if defined (ZB_ZCL_SUPPORT_CLUSTER_IAS_ACE)
#include "zcl/zb_zcl_ias_ace.h"
#endif
#if defined (ZB_ZCL_SUPPORT_CLUSTER_IAS_WD)
#include "zcl/zb_zcl_ias_wd.h"
#endif
#if defined (ZB_ZCL_SUPPORT_CLUSTER_IAS_ZONE)
#include "zcl/zb_zcl_ias_zone.h"
#endif
#if defined(ZB_ZCL_SUPPORT_CLUSTER_DIAGNOSTICS)
#include "zcl/zb_zcl_diagnostics.h"
#endif
#if defined (ZB_ZCL_SUPPORT_CLUSTER_CUSTOM_CLUSTER)
#include "zcl/zb_zcl_custom_cluster.h"
#endif

#if defined (ZB_ZCL_SUPPORT_CLUSTER_TIME)
#include "zcl/zb_zcl_time.h"
#endif
#if defined (ZB_ZCL_SUPPORT_CLUSTER_DEHUMIDIFICATION_CONTROL)
#include "zcl/zb_zcl_dehumidification_control.h"
#endif
#if defined (ZB_ZCL_SUPPORT_CLUSTER_ELECTRICAL_MEASUREMENT)
#include "zcl/zb_zcl_el_measurement.h"
#endif
#if defined (ZB_ZCL_SUPPORT_CLUSTER_FAN_CONTROL)
#include "zcl/zb_zcl_fan_control.h"
#endif
#if defined (ZB_ZCL_SUPPORT_CLUSTER_EN50523_APPLIANCE_EVENTS_AND_ALERTS)
#include "zcl/zb_zcl_en50523_appliance_events_and_alerts.h"
#endif
#if defined (ZB_ZCL_SUPPORT_CLUSTER_METERING)
#include "zcl/zb_zcl_metering.h"
#endif
#if defined (ZB_ZCL_SUPPORT_CLUSTER_THERMOSTAT)
#include "zcl/zb_zcl_thermostat.h"
#endif
#if defined (ZB_ZCL_SUPPORT_CLUSTER_THERMOSTAT_UI_CONFIG)
#include "zcl/zb_zcl_thermostat_ui_config.h"
#endif
#if defined (ZB_ZCL_SUPPORT_CLUSTER_OCCUPANCY_SENSING)
#include "zcl/zb_zcl_occupancy_sensing.h"
#endif
#if defined (ZB_ZCL_SUPPORT_CLUSTER_METER_IDENTIFICATION)
#include "zcl/zb_zcl_meter_identification.h"
#endif
#if defined (ZB_ZCL_SUPPORT_CLUSTER_WWAH)
#include "zcl/zb_zcl_wwah.h"
#endif /* ZB_ZCL_SUPPORT_CLUSTER_WWAH */

#if defined (ZB_ZCL_SUPPORT_CLUSTER_POLL_CONTROL)
#include "zcl/zb_zcl_poll_control.h"
#endif
#if defined (ZB_ZCL_SUPPORT_CLUSTER_OTA_UPGRADE)
#include "zcl/zb_zcl_ota_upgrade.h"
#endif

#if defined (ZB_ZCL_SUPPORT_CLUSTER_PRICE)
#include "zcl/zb_zcl_price.h"
#endif
#if defined (ZB_ZCL_SUPPORT_CLUSTER_DRLC)
#include "zcl/zb_zcl_drlc.h"
#endif
#if defined (ZB_ZCL_SUPPORT_CLUSTER_METERING)
#include "zcl/zb_zcl_metering.h"
#endif
#if defined (ZB_ZCL_SUPPORT_CLUSTER_MESSAGING)
#include "zcl/zb_zcl_messaging.h"
#endif
#if defined (ZB_ZCL_SUPPORT_CLUSTER_DAILY_SCHEDULE)
#include "zcl/zb_zcl_daily_schedule.h"
#endif
#if defined (ZB_ZCL_SUPPORT_CLUSTER_TUNNELING)
#include "zcl/zb_zcl_tunneling.h"
#endif

#if defined (ZB_ZCL_SUPPORT_CLUSTER_SUBGHZ)
#include "zcl/zb_zcl_subghz.h"
#endif
#if defined (ZB_ZCL_SUPPORT_CLUSTER_PREPAYMENT)
#include "zcl/zb_zcl_prepayment.h"
#endif
#if defined (ZB_ZCL_SUPPORT_CLUSTER_CALENDAR)
#include "zcl/zb_zcl_calendar.h"
#endif

#ifdef ZB_ENABLE_SE_CLUSTERS
#if defined (ZB_ZCL_SUPPORT_CLUSTER_EVENTS)
#include "zcl/zb_zcl_events.h"
#endif
#if defined (ZB_ZCL_SUPPORT_CLUSTER_ENERGY_MANAGEMENT)
#include "zcl/zb_zcl_energy_mgmt.h"
#endif
#if defined (ZB_ZCL_SUPPORT_CLUSTER_MDU_PAIRING)
#include "zcl/zb_zcl_mdu_pairing.h"
#endif
#if defined (ZB_ZCL_SUPPORT_CLUSTER_DEVICE_MANAGEMENT)
#include "zcl/zb_zcl_device_management.h"
#endif
#endif /* ZB_ENABLE_SE_CLUSTERS */

#ifdef ZB_ENABLE_CUSTOM_CLUSTERS
#include "zcl/zb_zcl_tunnel.h"
#include "zcl/zb_zcl_ir_blaster.h"
#endif /* ZB_ENABLE_CUSTOM_CLUSTERS */

#if defined ZB_ENABLE_ZGP_CLUSTER
//#include "zgp/zgp_internal.h"
#endif

#include "zcl/zb_zcl_grpw_bed_sensor.h"
#include "zcl/zb_zcl_grpw_freq_agility.h"

/**
 *  \addtogroup ZB_ZCL_INITIALIZATION
 *  @{
 *    @details
 *    Most of initialization is being handled automatically during device start. However,
 *    application may set additional callbacks for event handling. Particularly:

 *    @li @ref ZB_ZCL_SET_MODIFY_ATTR_VALUE_CB macro sets callback for attribute change handling.
 *    This event could arise, for example, as a result of @ref write_attr_cmd "Write attributes"
 *    command execution.
 *    @li @ref ZB_ZCL_SET_REPORT_ATTR_CB and @ref ZB_ZCL_SET_NO_REPORTING_CB macros set
 *    callbacks for handling "report attribute" and "no reporting" callbacks respectively. For more
 *    information see @ref ZB_ZCL_REPORTING "Attribute reporting" description.
 *
 *    Sample use ZB_ZCL_SET_REPORT_ATTR_CB
 *    @snippet light_sample/dimmable_light/bulb.c zb_zcl_set_report_attr_cb_example
 *    @snippet light_sample/dimmable_light/bulb.c zb_zcl_set_report_attr_cb
 *
 *    Sample use ZB_ZCL_SET_NO_REPORTING_CB
 *    @snippet doxygen_snippets.dox ZB_ZCL_SET_NO_REPORTING_CB_func_snippet_reporting_srv_th_c
 *    @snippet doxygen_snippets.dox ZB_ZCL_SET_NO_REPORTING_CB_snippet_reporting_srv_th_c
 *
 */

/** @brief ZCL version.
*     According ZCL 8 spec:
*     3.2.2.2.1 ZCLVersion Attribute
*     The ZCLVersion attribute represents a published set of foundation items (in Chapter 2),
*     such as global commands and functional descriptions.
*     For this version of the ZCL (this document), this attribute SHALL be set to 8.
*     In the future, this value SHALL align with the release revision of the ZCL(CCB 2722).
*/
#define ZB_ZCL_VERSION       8U

/**
 *  @brief Set modify attribute value callback.
 *  The macro sets a callback being called on external attribute change. The callback will be
 *  provided with all data necessary for correct value change handling.
 *  @param func_ptr - a callback to set (of type @ref zb_zcl_modify_attr_value_cb_t).
 *  @hideinitializer
 */
#define ZB_ZCL_SET_MODIFY_ATTR_VALUE_CB(func_ptr) ZCL_CTX().modify_attr_value_cb = (func_ptr)

#if !(defined ZB_ZCL_DISABLE_REPORTING) || defined(DOXYGEN)

/**
 *  @brief Set report attribute callback.
 *  The macro sets a callback being called on receive attribute report. The callback will
 *  be provided with all data necessary for correct attribute handling.
 *  @param func_ptr - callback to set (of type @ref zb_zcl_report_attr_cb_t).
 *  @hideinitializer
 */
#define ZB_ZCL_SET_REPORT_ATTR_CB(func_ptr) ZCL_CTX().report_attr_cb = (func_ptr)

/**
 *  @brief Set no reporting callback.
 *  The macro sets a callback being called on remote attribute reporting timeout. The callback will
 *  be provided with all data necessary for correct attribute report timeout handling.
 *  @param func_ptr - callback to set (of type @ref zb_zcl_no_reporting_cb_t).
 *  @hideinitializer
 */
#define ZB_ZCL_SET_NO_REPORTING_CB(func_ptr) ZCL_CTX().no_reporting_cb = (func_ptr)

#endif

/**
 *  @brief Set "Set default value all attribute" callback.
 *  The macro sets a callback being called for need to set attribute to default value.
 *  @param func_ptr - callback to set (of type @ref zb_zcl_set_default_value_attr_cb_t).
 *  @hideinitializer
 */
#define ZB_ZCL_SET_DEFAULT_VALUE_CB(func_ptr) ZCL_CTX().set_default_attr_cb = (func_ptr)

#if defined ZB_ENABLE_HA
/**
 *  @brief Set "Check attribute value" callback.
 *  The macro sets a callback being called for need to check attribute value.
 *
 *  @deprecated This callback will be removed in the next Major release after February 2023
 *              Use @ref ZB_ZCL_CHECK_ATTR_VALUE_MANUF_CB instead
 *
 *  @param func_ptr - callback to set (of type @ref zb_zcl_app_check_attr_value_cb_t).
 *  @hideinitializer
 */
#define ZB_ZCL_CHECK_ATTR_VALUE_CB(func_ptr) ZCL_CTX().app_check_attr_value_cb = (func_ptr)

/**
 *  @brief Set "Check attribute value" callback to check with manufacturer specific code.
 *  The macro sets a callback being called for need to check attribute value.
 *  @param func_ptr - callback to set (of type @ref zb_zcl_app_check_attr_value_manuf_cb_t).
 *  @hideinitializer
 */
#define ZB_ZCL_CHECK_ATTR_VALUE_MANUF_CB(func_ptr) ZCL_CTX().app_check_attr_value_manuf_cb = (func_ptr)

/**
    @brief Set "Discovery manufacture command" callback.
    The macro sets a callback that will be called to get a list of manufacture specific commands.
    @param func_ptr - callback to set (of type @ref zb_zcl_disc_manuf_cmd_cb_t).
    @hideinitializer

    Sample use Discovery manufacture command callback

    @par Example
    Example custom callback
    @code
    zb_uint8_t list_my_manufacture_command_generate[] = {
      MY_COMMAND_GENERATE_1,
      MY_COMMAND_GENERATE_2,
      MY_COMMAND_GENERATE_3,
      MY_COMMAND_GENERATE_4,
    };

    zb_uint8_t list_my_manufacture_command_receive[] = {
      MY_COMMAND_RECEIVE_1,
      MY_COMMAND_RECEIVE_2,
      MY_COMMAND_RECEIVE_3,
    };

    zb_uint8_t my_disc_manuf_cmd_cb(
        zb_uint8_t direction, zb_uint8_t ep, zb_uint16_t cluster_id, zb_uint16_t manuf_id, zb_bool_t recv_cmd_type, zb_uint8_t **value)
    {
       zb_uint8_t ret;

       if(manuf_id!=MY_MANUFACTURE_ID && manuf_id!=ZB_ZCL_MANUFACTURER_WILDCARD_ID)
         return 0;

      if (!recv_cmd_type)
      {
        value = &list_my_manufacture_command_generate;
        ret = sizeof(list_my_manufacture_command_generate);
      }
      else
      {
        value = &list_my_manufacture_command_receive;
        ret = sizeof(list_my_manufacture_command_receive);
      }
      return ret;
    }
    @endcode
    @par

    @par Example
    Example register callback
    @code
    ZB_ZCL_SET_DISC_MANUFACTURE_COMMAND_CB(&my_disc_manuf_cmd_cb);
    @endcode
    @par
 */
#define ZB_ZCL_SET_DISC_MANUFACTURE_COMMAND_CB(func_ptr) ZCL_CTX().disc_manuf_cmd_cb = (func_ptr)

#endif /* ZB_ENABLE_HA */

/**
 *  @brief Set user callback to notify an application a broadcast EP command received.
 *  @param func_ptr - callback to set (of type @ref zb_device_handler_t).
 *  @return processing status (see @ref zb_bool_t)
 *  Value of ZB_TRUE means that the application successfully processed a command - ZBOSS stack
 *  will do nothing in this case and will free a buffer automatically.
 *  If set to ZB_FALSE, default stack processing will be performed.
 *  @note Do not use this callback unless you are sure you need it.
 *  ZBOSS stack performs additional command checks that can be skipped by an application.
 */
#define ZB_ZCL_SET_BROADCAST_EP_CB(func_ptr) ZCL_CTX().broadcast_ep_cb = (func_ptr)

/** @cond internals_doc */
/**
 *  @brief Get current time in units.
 *  Uint = 1/10 sec
 */
#define ZB_GET_TIME_IN_UNITS()    ZB_SYS_TIMER_INTERVAL_TO_TIME_UNITS(ZB_TIMER_GET())

/**
 *  @brief Convert system timer interval to units
 *  Uint = 1/10 sec
 */
#define ZB_SYS_TIMER_INTERVAL_TO_TIME_UNITS(interval)    ZB_SYS_TIMER_INTERVAL_TO_MSEC(interval) / 100U

/**
 *  @brief Convert units to system timer interval.
 *  Uint = 1/10 sec
 */
#define ZB_TIME_UNITS_TO_SYS_TIMER_INTERVAL(uints)  ZB_MILLISECONDS_TO_SYS_TIMER_INTERVAL((uints)*100U)
/** @endcond */ /* internals_doc */

/**
 * Types of device user application callbacks
 * @see zb_zcl_device_callback_param_t
 */
typedef enum zb_zcl_device_callback_id_e
{
  /** Inform user about attribute value modification */
  ZB_ZCL_SET_ATTR_VALUE_CB_ID = 0,
/** @cond DOXYGEN_INTERNAL_DOC */
  /** Inform user about received attribute report */
  ZB_ZCL_REPORT_ATTR_CB_ID,
  /** Inform user that report was not received during defined time interval */
  ZB_ZCL_NO_REPORTING_CB_ID,
/** @endcond */ /* DOXYGEN_INTERNAL_DOC */
  /** Inform user about change On/Off effect @see ZCL spec 6.6.1.4.4.2 */
  ZB_ZCL_ON_OFF_WITH_EFFECT_VALUE_CB_ID,
  /** Inform user about ZCL Shade cluster shade position value modification */
  ZB_ZCL_SHADE_SET_VALUE_CB_ID,
  /** Inform user that shade_get_value_param of ZCL Shade custer could be changed */
  ZB_ZCL_SHADE_GET_VALUE_CB_ID,
  /** Inform user about call identify effect command @see ZCL spec 6.3.1.2.1 */
  ZB_ZCL_IDENTIFY_EFFECT_CB_ID,
  /** Inform user about ZCL Level Control cluster attributes value modification */
  ZB_ZCL_LEVEL_CONTROL_SET_VALUE_CB_ID,
  /** Inform user about enroll result command @see ZCL spec 8.2.2.3.1 */
  ZB_ZCL_IAS_ZONE_ENROLL_RESPONSE_VALUE_CB_ID,
  /** Inform user about call Start warning command @see ZCL spec 8.4.2.3.1 */
  ZB_ZCL_IAS_WD_START_WARNING_VALUE_CB_ID,
  /** Inform user about call Squawk command @see ZCL spec 8.4.2.3.2 */
  ZB_ZCL_IAS_WD_SQUAWK_VALUE_CB_ID,
  /** @b Server. Inform user about IAS ACE Arm command.
   *
   * User's application callback is initialized by RET_OK status of device
   * callback parameters.
   * @param[in] param_in @ref zb_zcl_ias_ace_arm_t
   * @param[out] param_out @ref zb_zcl_ias_ace_arm_resp_t
   *
   * One of the following statuses must be returned:
   * @return RET_OK - successfully handle command.
   *                  Send @ref ZB_ZCL_CMD_IAS_ACE_ARM_RESP_ID "Arm Response" command.
   * @return RET_ERROR - command is handled with errors.
   *
   */
  ZB_ZCL_IAS_ACE_ARM_CB_ID,
  /** @b Server. Inform user about IAS ACE Bypass command.
   *
   * User's application callback is initialized by RET_OK status of device
   * callback parameters.
   * @param[in] param_in @ref zb_zcl_ias_ace_bypass_t
   * @param[out] param_out @ref zb_zcl_ias_ace_bypass_resp_t
   *
   * One of the following statuses must be returned:
   * @return RET_OK - successfully handle command.
   *                  Send @ref ZB_ZCL_CMD_IAS_ACE_BYPASS_RESPONSE_ID "Bypass Response" command.
   * @return RET_ERROR - command is handled with errors.
   *
   */
  ZB_ZCL_IAS_ACE_BYPASS_CB_ID,
  /** @b Server. Inform user about IAS ACE Emergency command.
   *
   * User's application callback is initialized by RET_OK status of device
   * callback parameters.
   *
   * One of the following statuses must be returned:
   * @return RET_OK - successfully handle command. Default Response will be send if requested.
   * @return RET_ERROR - command is handled with errors.
   *
   */
  ZB_ZCL_IAS_ACE_EMERGENCY_CB_ID,
  /** @b Server. Inform user about IAS ACE Fire command.
   *
   * User's application callback is initialized by RET_OK status of device
   * callback parameters.
   *
   * One of the following statuses must be returned:
   * @return RET_OK - successfully handle command. Default Response will be send if requested.
   * @return RET_ERROR - command is handled with errors.
   *
   */
  ZB_ZCL_IAS_ACE_FIRE_CB_ID,
  /** @b Server. Inform user about IAS ACE Panic command.
   *
   * User's application callback is initialized by RET_OK status of device
   * callback parameters.
   *
   * One of the following statuses must be returned:
   * @return RET_OK - successfully handle command. Default Response will be send if requested.
   * @return RET_ERROR - command is handled with errors.
   *
   */
  ZB_ZCL_IAS_ACE_PANIC_CB_ID,
  /** @b Server. Inform user about IAS ACE Get Panel Status command.
   *
   * User's application callback is initialized by RET_OK status of device
   * callback parameters.
   * @param[out] param_out @ref zb_zcl_ias_ace_get_panel_status_resp_t
   *
   * One of the following statuses must be returned:
   * @return RET_OK - successfully handle command.
   *                  Send @ref ZB_ZCL_CMD_IAS_ACE_GET_PANEL_STATUS_RESPONSE_ID "Get Panel Status Response" command.
   * @return RET_ERROR - command is handled with errors.
   *
   */
  ZB_ZCL_IAS_ACE_GET_PANEL_STATUS_CB_ID,
  /** @b Server. Inform user about IAS ACE Get Bypassed Zone List command.
   *
   * User's application callback is initialized by RET_OK status of device
   * callback parameters.
   * @param[out] param_out @ref zb_zcl_ias_ace_set_bypassed_zone_list_t
   *
   * One of the following statuses must be returned:
   * @return RET_OK - successfully handle command.
   *                  Send @ref ZB_ZCL_CMD_IAS_ACE_SET_BYPASSED_ZONE_LIST_ID "Set Bypassed Zone List" command.
   * @return RET_ERROR - command is handled with errors.
   *
   */
  ZB_ZCL_IAS_ACE_GET_BYPASSED_ZONE_LIST_CB_ID,
  /** @b Server. Inform user about IAS ACE Get Zone Status command.
   *
   * User's application callback is initialized by RET_OK status of device
   * callback parameters.
   * @param[in] param_in @ref zb_zcl_ias_ace_get_zone_status_t
   * @param[out] param_out @ref zb_zcl_ias_ace_get_zone_status_resp_t
   *
   * One of the following statuses must be returned:
   * @return RET_OK - successfully handle command.
   *                  Send @ref ZB_ZCL_CMD_IAS_ACE_GET_ZONE_STATUS_RESPONSE_ID "Get Zone Status Response" command.
   * @return RET_ERROR - command is handled with errors.
   *
   */
  ZB_ZCL_IAS_ACE_GET_ZONE_STATUS_CB_ID,
  /** @b Client. Inform user about IAS ACE Arm Response command.
   *
   * User's application callback is initialized by RET_OK status of device
   * callback parameters.
   * @param[in] param_in @ref zb_zcl_ias_ace_arm_resp_t
   *
   * One of the following statuses must be returned:
   * @return RET_OK - successfully handle command. Default Response will be send if requested.
   * @return RET_ERROR - command is handled with errors.
   *
   */
  ZB_ZCL_IAS_ACE_ARM_RESP_CB_ID,
  /** @b Client. Inform user about IAS ACE Get Zone ID Map Response command.
   *
   * User's application callback is initialized by RET_OK status of device
   * callback parameters.
   * @param[in] param_in @ref zb_zcl_ias_ace_get_zone_id_map_resp_t
   *
   * One of the following statuses must be returned:
   * @return RET_OK - successfully handle command. Default Response will be send if requested.
   * @return RET_ERROR - command is handled with errors.
   *
   */
  ZB_ZCL_IAS_ACE_GET_ZONE_ID_MAP_RESP_CB_ID,
  /** @b Client. Inform user about IAS ACE Get Zone Info Response command.
   *
   * User's application callback is initialized by RET_OK status of device
   * callback parameters.
   * @param[in] param_in @ref zb_zcl_ias_ace_get_zone_info_resp_t
   *
   * One of the following statuses must be returned:
   * @return RET_OK - successfully handle command. Default Response will be send if requested.
   * @return RET_ERROR - command is handled with errors.
   *
   */
  ZB_ZCL_IAS_ACE_GET_ZONE_INFO_RESP_CB_ID,
  /** @b Client. Inform user about IAS ACE Zone Status Changed command.
   *
   * User's application callback is initialized by RET_OK status of device
   * callback parameters.
   * @param[in] param_in @ref zb_zcl_ias_ace_zone_status_changed_t
   *
   * One of the following statuses must be returned:
   * @return RET_OK - successfully handle command. Default Response will be send if requested.
   * @return RET_ERROR - command is handled with errors.
   *
   */
  ZB_ZCL_IAS_ACE_ZONE_STATUS_CHANGED_CB_ID,
  /** @b Client. Inform user about IAS ACE Panel Status Changed command.
   *
   * User's application callback is initialized by RET_OK status of device
   * callback parameters.
   * @param[in] param_in @ref zb_zcl_ias_ace_panel_status_changed_t
   *
   * One of the following statuses must be returned:
   * @return RET_OK - successfully handle command. Default Response will be send if requested.
   * @return RET_ERROR - command is handled with errors.
   *
   */
  ZB_ZCL_IAS_ACE_PANEL_STATUS_CHANGED_CB_ID,
  /** @b Client. Inform user about IAS ACE Get Panel Status Response command.
   *
   * User's application callback is initialized by RET_OK status of device
   * callback parameters.
   * @param[in] param_in @ref zb_zcl_ias_ace_get_panel_status_resp_t
   *
   * One of the following statuses must be returned:
   * @return RET_OK - successfully handle command. Default Response will be send if requested.
   * @return RET_ERROR - command is handled with errors.
   *
   */
  ZB_ZCL_IAS_ACE_GET_PANEL_STATUS_RESP_CB_ID,
  /** @b Client. Inform user about IAS ACE Set Bypassed Zone List command.
   *
   * User's application callback is initialized by RET_OK status of device
   * callback parameters.
   * @param[in] param_in @ref zb_zcl_ias_ace_set_bypassed_zone_list_t
   *
   * One of the following statuses must be returned:
   * @return RET_OK - successfully handle command. Default Response will be send if requested.
   * @return RET_ERROR - command is handled with errors.
   *
   */
  ZB_ZCL_IAS_ACE_SET_BYPASSED_ZONE_LIST_CB_ID,
  /** @b Client. Inform user about IAS ACE Bypass Response command.
   *
   * User's application callback is initialized by RET_OK status of device
   * callback parameters.
   * @param[in] param_in @ref zb_zcl_ias_ace_bypass_resp_t
   *
   * One of the following statuses must be returned:
   * @return RET_OK - successfully handle command. Default Response will be send if requested.
   * @return RET_ERROR - command is handled with errors.
   *
   */
  ZB_ZCL_IAS_ACE_BYPASS_RESP_CB_ID,
  /** @b Client. Inform user about IAS ACE Get Zone Status Response command.
   *
   * User's application callback is initialized by RET_OK status of device
   * callback parameters.
   * @param[in] param_in @ref zb_zcl_ias_ace_get_zone_status_resp_t
   *
   * One of the following statuses must be returned:
   * @return RET_OK - successfully handle command. Default Response will be send if requested.
   * @return RET_ERROR - command is handled with errors.
   *
   */
  ZB_ZCL_IAS_ACE_GET_ZONE_STATUS_RESP_CB_ID,
  /** Inform user about OTA Upgrade commands  */
  ZB_ZCL_OTA_UPGRADE_VALUE_CB_ID,
  /** Inform user about OTA Upgrade query image response command */
  ZB_ZCL_OTA_UPGRADE_QUERY_IMAGE_RESP_CB_ID,
  /** Inform user about Basic Reset to Factory Defaults commands  */
  ZB_ZCL_BASIC_RESET_CB_ID,
  /** Inform user about call Thermostat weekly schedule command @see HA spec 10.1.3.3 */
  ZB_ZCL_THERMOSTAT_WEEKLY_SCHEDULE_SET_CB_ID,
  /** Inform user about call Thermostat command @see HA spec 10.1.3.3 */
  ZB_ZCL_THERMOSTAT_VALUE_CB_ID,
  /** Inform user about Check-In command from polled device */
  ZB_ZCL_POLL_CONTROL_CHECK_IN_CLI_CB_ID,
  /** @b Server. Inform user about EN50523 Appliance Events and Alerts Get Alerts command.
   *
   * User's application callback is initialized by RET_OK status of device
   * callback parameters.
   * @param[out] param_out zb_zcl_en50523_appl_ev_and_alerts_get_alerts_resp_t
   *
   * One of the following statuses must be returned:
   * @return RET_OK - successfully handle command.
   * @return RET_ERROR - command is handled with errors.
   *
   */
  ZB_ZCL_EN50523_APPL_EV_AND_ALERTS_GET_ALERTS_CB_ID,
  /** @b Client. Inform user about EN50523 Appliance Events and Alerts Get Alerts Response command.
   *
   * User's application callback is initialized by RET_OK status of device
   * callback parameters.
   * @param[in] param_in zb_zcl_en50523_appl_ev_and_alerts_get_alerts_resp_t
   * One of the following statuses must be returned:
   * @return RET_OK - successfully handle command. Default Response will be send if requested.
   * @return RET_ERROR - command is handled with errors.
   *
   */
  ZB_ZCL_EN50523_APPL_EV_AND_ALERTS_GET_ALERTS_RESP_CB_ID,
  /** @b Client. Inform user about EN50523 Appliance Events and Alerts Alerts Notification command.
   *
   * User's application callback is initialized by RET_OK status of device
   * callback parameters.
   * @param[in] param_in zb_zcl_en50523_appl_ev_and_alerts_alerts_notif_t
   *
   * One of the following statuses must be returned:
   * @return RET_OK - successfully handle command. Default Response will be send if requested.
   * @return RET_ERROR - command is handled with errors.
   *
   */
  ZB_ZCL_EN50523_APPL_EV_AND_ALERTS_ALERTS_NOTIFICATION_CB_ID,
  /** @b Client. Inform user about EN50523 Appliance Events and Alerts Event Notification command.
   *
   * User's application callback is initialized by RET_OK status of device
   * callback parameters.
   * @param[in] param_in zb_zcl_en50523_appl_ev_and_alerts_event_notif_t
   * One of the following statuses must be returned:
   * @return RET_OK - successfully handle command. Default Response will be send if requested.
   * @return RET_ERROR - command is handled with errors.
   */
  ZB_ZCL_EN50523_APPL_EV_AND_ALERTS_EVENT_NOTIFICATION_CB_ID,
  /** @b Server. Inform user about Query next image request */
  ZB_ZCL_OTA_UPGRADE_SRV_QUERY_IMAGE_CB_ID,
  /** @b Server. Inform user about the first Image block request */
  ZB_ZCL_OTA_UPGRADE_SRV_UPGRADE_STARTED_CB_ID,
  /** @b Server. Inform user about the aborted Image block request  */
  ZB_ZCL_OTA_UPGRADE_SRV_UPGRADE_ABORTED_CB_ID,
  /** @b Server. Inform user about finished OTA upgrade */
  ZB_ZCL_OTA_UPGRADE_SRV_UPGRADE_END_CB_ID,
  /**@b Server. Ask user about payload for PublishPrice request.
   *
   * User's application callback is initialized by RET_NOT_FOUND status of device
   * callback parameters.
   * @param[in] param_in @ref zb_zcl_price_get_current_price_payload_t
   * @param[out] param_out @ref zb_zcl_price_publish_price_payload_t
   *
   * One of the following statuses must be returned:
   * @return RET_OK - price data is available.
   *                  Send @ref ZB_ZCL_PRICE_SRV_CMD_PUBLISH_PRICE "PublishPrice" command.
   * @return RET_NOT_FOUND - price data isn't available. Default Response will be send if requested.
   *
   */
  ZB_ZCL_PRICE_GET_CURRENT_PRICE_CB_ID,
  /**@b Server. Notify user about GetScheduledPrices request.
   *
   * User's application callback is initialized by RET_NOT_FOUND status of device
   * callback parameters.
   * @param[out] param_out param_in @ref zb_zcl_price_get_scheduled_prices_payload_t
   *
   * One of the following statuses must be returned:
   * @return RET_OK - price data is available.
   * @return RET_NOT_FOUND - price data isn't available. Default Response will be send if requested.
   *
   */
  ZB_ZCL_PRICE_GET_SCHEDULED_PRICES_CB_ID,
  /** @b Server. Notify user about GetTierLabels request.
   *
   * User's application callback is initialized by RET_NOT_FOUND status of device
   * callback parameters.
   * @param[in] param_out param_in @ref zb_zcl_price_get_tier_labels_payload_t
   *
   * One of the following statuses must be returned:
   * @return RET_OK - price data is available. In this case application must send reply using
   * new buffer. Stack free buffer in this case.
   * @return RET_ERROR - command is handled with errors. Default Response will be send if requested.
   * @return RET_NOT_FOUND - price data isn't available. Default Response will be send if requested.
   *
   */
  ZB_ZCL_PRICE_GET_TIER_LABELS_CB_ID,
  /** @b Client. Inform user about PublishPrice request.
   *
   * User's application callback is initialized by RET_OK status of device
   * callback parameters.
   * @param[in] param_in @ref zb_zcl_price_publish_price_payload_t
   * @param[out] param_out @ref zb_zcl_price_ack_payload_t
   *
   * One of the following statuses must be returned:
   * @return RET_OK - successfully handle command.
   *                  Send @ref ZB_ZCL_PRICE_CLI_CMD_PRICE_ACK "PriceAcknowledgement" command.
   * @return RET_ERROR - command is handled with errors
   *
   */
  ZB_ZCL_PRICE_PUBLISH_PRICE_CB_ID,
  /** @b Client. Inform user about PublishTierLabels request.
   *
   * User's application callback is initialized by RET_ERROR status of device
   * callback parameters.
   * @param[in] param_in @ref zb_zcl_price_publish_tier_labels_payload_t
   *
   * One of the following statuses must be returned:
   * @return RET_OK - successfully handle command. Default Response will be send if requested.
   * @return RET_ERROR - command is handled with errors
   *
   */
  ZB_ZCL_PRICE_PUBLISH_TIER_LABELS_CB_ID,
  /** @b Server. Inform user about PriceAck command.
   *
   * User's application callback is initialized by RET_ERROR status of device
   * callback parameters.
   * @param[in] param_in @ref zb_zcl_price_ack_payload_t
   *
   * One of the following statuses must be returned:
   * @return RET_OK - successfully handle command. Default Response will be send if requested.
   * @return RET_ERROR - command is handled with errors.
   *
   */
  ZB_ZCL_PRICE_PRICE_ACK_CB_ID,
  /** @b Server. Inform user about change of state for an active Load Control event
   *
   * User's application callback is initialized by RET_ERROR status of device
   * callback parameters.
   * @param[in] param_in @ref zb_zcl_drlc_report_event_status_payload_t
   *
   * One of the following statuses must be returned:
   * @return RET_OK - successfully handle command. Default Response will be send if requested.
   * @return RET_ERROR - command is handled with errors. Default Response will be send if requested.
   */
  ZB_ZCL_DRLC_REPORT_EVENT_STATUS_CB_ID,
  /** @b Server. Ask user about currently active or scheduled Load Control Events
   *
   * User's application callback is initialized by RET_ERROR status of device
   * callback parameters.
   * @param[in] param_in @ref zb_zcl_drlc_get_scheduled_events_payload_t
   * @param[out] param_out @ref zb_zcl_drlc_lce_payload_t
   *
   * One of the following statuses must be returned:
   * @return RET_OK - successfully handle command.
   *                  Send @ref ZB_ZCL_DRLC_SRV_CMD_LOAD_CONTROL_EVENT "LoadControlEvent" command
   * @return RET_ERROR - command is handled with errors.
   */
  ZB_ZCL_DRLC_GET_SCHEDULED_EVENTS_CB_ID,
  /** @b Client. Inform user about received Load Control Event
   *
   * User's application callback is initialized by RET_ERROR status of device
   * callback parameters.
   * @param[in] param_in @ref zb_zcl_drlc_lce_payload_t
   * @param[out] param_out @ref zb_zcl_drlc_report_event_status_payload_t
   *
   * One of the following statuses must be returned:
   * @return RET_OK - successfully handle command.
   *                  Send @ref ZB_ZCL_DRLC_CLI_CMD_REPORT_EVENT_STATUS "ReportEventStatus" in response.
   * @return RET_IGNORE - application doesn't received and resolved time. Drop event.
   *                      Default Response will be send if requested.
   * @return RET_ERROR - command is handled with errors. Default Response will be send if requested.
   */
  ZB_ZCL_DRLC_LOAD_CONTROL_EVENT_CB_ID,
  /** @b Client. Inform user about Load Control Event cancellation
   *
   * User's application callback is initialized by RET_ERROR status of device
   * callback parameters.
   * @param[in] param_in @ref zb_zcl_price_publish_price_payload_t
   * @param[out] param_out @ref zb_zcl_price_ack_payload_t
   *
   * One of the following statuses must be returned:
   * @return RET_OK - successfully handle command.
   *                  Send @ref ZB_ZCL_DRLC_CLI_CMD_REPORT_EVENT_STATUS "ReportEventStatus" in response.
   * @return RET_IGNORE - application doesn't received and resolved time. Drop event.
   *                      Default Response will be send if requested.
   * @return RET_ERROR - command is handled with errors. Default Response will be send if requested.
   */
  ZB_ZCL_DRLC_CANCEL_LOAD_CONTROL_EVENT_CB_ID,
  /** @b Client. Inform user about all Load Control Events cancellation
   *
   * User's application callback is initialized by RET_ERROR status of device
   * callback parameters.
   * @param[in] param_in @ref zb_zcl_drlc_cancel_lce_payload_t
   * @param[out] param_out @ref zb_zcl_drlc_report_event_status_payload_t
   *
   * One of the following statuses must be returned:
   * @return RET_OK - successfully handle command. Default Response will be send if requested.
   * @return RET_IGNORE - application doesn't received and resolved time. Drop events.
   *                      Default Response will be send if requested.
   * @return RET_ERROR - command is handled with errors. Default Response will be send if requested.
   */
  ZB_ZCL_DRLC_CANCEL_ALL_LOAD_CONTROL_EVENTS_CB_ID,
  /** @b Server. Inform user about received Message Confirmation request
   *
   * User's application callback is initialized by RET_ERROR status of device
   * callback parameters.
   * @param[in] param_in @ref zb_zcl_messaging_message_confirm_payload_t
   *
   * One of the following statuses must be returned:
   * @return RET_OK - successfully handle command. Default Response will be send if requested.
   * @return RET_ERROR - command is handled with errors. Default Response will be send if requested.
   */
  ZB_ZCL_MESSAGING_MSG_CONFIRMATION_CB_ID,
  /** @b Server. Ask user about payload for Display Message request
   *
   * User's application callback is initialized by RET_ERROR status of device
   * callback parameters.
   * @param[out] param_out @ref zb_zcl_messaging_get_last_message_response_t
   *
   * One of the following statuses must be returned:
   * @return RET_OK - successfully handle command.
   *                  Send @ref ZB_ZCL_MESSAGING_SRV_CMD_DISPLAY_MESSAGE "DisplayMessage" command
   * @return RET_NOT_FOUND - no messages is available. Default Response will be send if requested.
   * @return RET_ERROR - command is handled with errors. Default Response will be send if requested.
   */
  ZB_ZCL_MESSAGING_GET_LAST_MSG_CB_ID,
  /** @b Client. Inform user about received Cancel Message request
   *
   * User's application callback is initialized by RET_ERROR status of device
   * callback parameters.
   * @param[in] param_in @ref zb_zcl_messaging_cancel_message_payload_t
   *
   * One of the following statuses must be returned:
   * @return RET_OK - successfully handle command. Default Response will be send if requested.
   * @return RET_ERROR - command is handled with errors. Default Response will be send if requested.
   */
  ZB_ZCL_MESSAGING_CANCEL_MSG_CB_ID,
  /** @b Client. Inform user about received Display Message request
   *
   * User's application callback is initialized by RET_ERROR status of device
   * callback parameters.
   * @param[in] param_in @ref zb_zcl_messaging_display_message_payload_t
   *
   * One of the following statuses must be returned:
   * @return RET_OK - successfully handle command. Default Response will be send if requested.
   * @return RET_ERROR - command is handled with errors. Default Response will be send if requested.
   */
  ZB_ZCL_MESSAGING_DISPLAY_MSG_CB_ID,
  /** @b Server. Inform user about request to setup a tunnel association with the server
   *
   * User's application callback is initialized by RET_OK status of device
   * callback parameters.
   * @param[in] param_in @ref zb_zcl_tunneling_request_tunnel_t
   * @param[out] param_out tunnel_status
   *
   * One of the following statuses must be returned:
   * @return RET_OK - successfully handle command.
   * @return RET_ERROR - command is handled with errors.
   */
  ZB_ZCL_TUNNELING_REQUEST_TUNNEL_CB_ID,
  /** @b Client. Inform user about response to a RequestTunnel command
   *
   * User's application callback is initialized by RET_OK status of device
   * callback parameters.
   * @param[in] param_in @ref zb_zcl_tunneling_request_tunnel_response_t
   *
   * One of the following statuses must be returned:
   * @return RET_OK - successfully handle command. Default Response will be send if requested.
   * @return RET_ERROR - command is handled with errors. Default Response will be send if requested.
   */
  ZB_ZCL_TUNNELING_REQUEST_TUNNEL_RESPONSE_CB_ID,
  /** @b Server. Inform user about received TransferData request
   *
   * User's application callback is initialized by RET_OK status of device
   * callback parameters.
   * @param[in] param_in @ref zb_zcl_tunneling_transfer_data_payload_t
   *
   * One of the following statuses must be returned:
   * @return RET_OK - successfully handle command.
   * @return RET_ERROR - command is handled with errors.
   *                     Send @ref ZB_ZCL_TUNNELING_SRV_CMD_TRANSFER_DATA_ERROR "TransferDataError" command
   */
  ZB_ZCL_TUNNELING_TRANSFER_DATA_CLI_CB_ID,
  /** @b Client. Inform user about received TransferData request
   *
   * User's application callback is initialized by RET_OK status of device
   * callback parameters.
   * @param[in] param_in @ref zb_zcl_tunneling_transfer_data_payload_t
   *
   * One of the following statuses must be returned:
   * @return RET_OK - successfully handle command.
   * @return RET_ERROR - command is handled with errors.
   *                     Send @ref ZB_ZCL_TUNNELING_SRV_CMD_TRANSFER_DATA_ERROR "TransferDataError" command
   */
  ZB_ZCL_TUNNELING_TRANSFER_DATA_SRV_CB_ID,
  /** @b Server. Inform user about received TransferDataError request
   *
   * User's application callback is initialized by RET_OK status of device
   * callback parameters.
   * @param[in] param_in @ref zb_zcl_tunneling_transfer_data_error_t
   *
   * One of the following statuses must be returned:
   * @return RET_OK - successfully handle command.
   * @return RET_ERROR - command is handled with errors.
   */
  ZB_ZCL_TUNNELING_TRANSFER_DATA_ERROR_CLI_CB_ID,
  /** @b Client. Inform user about received TransferDataError request
   *
   * User's application callback is initialized by RET_OK status of device
   * callback parameters.
   * @param[in] param_in @ref zb_zcl_tunneling_transfer_data_error_t
   *
   * One of the following statuses must be returned:
   * @return RET_OK - successfully handle command.
   * @return RET_ERROR - command is handled with errors.
   */
  ZB_ZCL_TUNNELING_TRANSFER_DATA_ERROR_SRV_CB_ID,
  /** @b Server. Inform user about closing the tunnel with the server.
   *
   * User's application callback is initialized by RET_ERROR status of device
   * callback parameters.
   * @param[in] param_in @ref zb_zcl_tunneling_close_tunnel_t
   *
   * One of the following statuses must be returned:
   * @return RET_OK - successfully handle command. Default Response will be send if requested.
   * @return RET_NOT_FOUND - @e TunnelID value doesn't match an active tunnel on the server.
   *                         Default Response will be send if requested.
   * @return RET_ERROR - command is handled with errors. Default Response will be send if requested.
   */
  ZB_ZCL_TUNNELING_CLOSE_TUNNEL_CB_ID,

#if defined ZB_ENABLE_SE || defined ZB_ZCL_SUPPORT_CLUSTER_CALENDAR || defined DOXYGEN
  /** @cond DOXYGEN_ZCL_SECTION && DOXYGEN_SE_SECTION */
  /** @b Server. Inform user about GetProfile request.
   *
   * User's application callback is initialized by RET_ERROR status of device
   * callback parameters.
   * @param[in] param_in @ref zb_zcl_metering_get_profile_payload_t
   * @param[out] param_out @ref zb_zcl_metering_get_profile_response_payload_t
   *
   * One of the following statuses must be returned:
   * @return RET_OK - successfully handle command.
   *                  Send @ref ZB_ZCL_METERING_SRV_CMD_GET_PROFILE_RESPONSE "GetProfileResponse" command.
   * @return RET_ERROR - command is handled with errors. Default Response will be send if requested.
   *
   */
  /** @b Server. Ask user about payload for PublishCalendar request
   *
   * User's application callback is initialized by RET_ERROR status of device
   * callback parameters.
   * @param[in] param_in @ref zb_zcl_calendar_get_calendar_payload_t
   * @param[out] param_out @ref zb_zcl_calendar_publish_calendar_payload_t
   *
   * One of the following statuses must be returned:
   * @return RET_OK - successfully handle command.
   *                  Send @ref ZB_ZCL_CALENDAR_SRV_CMD_PUBLISH_CALENDAR "PublishCalendar" command.
   * @return RET_NOT_FOUND - no calendar is found. Default Response will be send if requested.
   * @return RET_ERROR - command is handled with errors. Default Response will be send if requested.
   */
  ZB_ZCL_CALENDAR_GET_CALENDAR_CB_ID,
  /** @b Server. Ask user about payload for PublishDayProfiles request
   *
   * User's application callback is initialized by RET_ERROR status of device
   * callback parameters.
   * @param[in] param_in @ref zb_zcl_calendar_get_day_profiles_payload_t
   * @param[out] param_out @ref zb_zcl_calendar_publish_day_profile_payload_t
   *
   * One of the following statuses must be returned:
   * @return RET_OK - successfully handle command.
   *                  Send @ref ZB_ZCL_CALENDAR_SRV_CMD_PUBLISH_DAY_PROFILE "PublishDayProfiles" command.
   * @return RET_NOT_FOUND - no calendar is found. Default Response will be send if requested.
   * @return RET_ERROR - command is handled with errors. Default Response will be send if requested.
   */
  ZB_ZCL_CALENDAR_GET_DAY_PROFILES_CB_ID,
  /** @b Server. Ask user about payload for PublishWeekProfiles request
   *
   * User's application callback is initialized by RET_ERROR status of device
   * callback parameters.
   * @param[in] param_in @ref zb_zcl_calendar_get_week_profiles_payload_t
   * @param[out] param_out @ref zb_zcl_calendar_publish_week_profile_payload_t
   *
   * One of the following statuses must be returned:
   * @return RET_OK - successfully handle command.
   *                  Send @ref ZB_ZCL_CALENDAR_SRV_CMD_PUBLISH_WEEK_PROFILE "PublishWeekProfiles" command.
   * @return RET_NOT_FOUND - no calendar is found. Default Response will be send if requested.
   * @return RET_ERROR - command is handled with errors. Default Response will be send if requested.
   */
  ZB_ZCL_CALENDAR_GET_WEEK_PROFILES_CB_ID,
  /** @b Server. Ask user about payload for PublishSeasons request
   *
   * User's application callback is initialized by RET_ERROR status of device
   * callback parameters.
   * @param[in] param_in @ref zb_zcl_calendar_get_seasons_payload_t
   * @param[out] param_out @ref zb_zcl_calendar_publish_seasons_payload_t
   *
   * One of the following statuses must be returned:
   * @return RET_OK - successfully handle command.
   *                  Send @ref ZB_ZCL_CALENDAR_SRV_CMD_PUBLISH_SEASONS "PublishSeasons" command.
   * @return RET_NOT_FOUND - no calendar is found. Default Response will be send if requested.
   * @return RET_ERROR - command is handled with errors. Default Response will be send if requested.
   */
  ZB_ZCL_CALENDAR_GET_SEASONS_CB_ID,
  /** @b Server. Ask user about payload for PublishSpecialDays request
   *
   * User's application callback is initialized by RET_ERROR status of device
   * callback parameters.
   * @param[in] param_in @ref zb_zcl_calendar_get_special_days_payload_t
   * @param[out] param_out @ref zb_zcl_calendar_publish_special_days_payload_t
   *
   * One of the following statuses must be returned:
   * @return RET_OK - successfully handle command.
   *                  Send @ref ZB_ZCL_CALENDAR_SRV_CMD_PUBLISH_SPECIAL_DAYS "PublishSpecialDays" command.
   * @return RET_NOT_FOUND - no calendar is found. Default Response will be send if requested.
   * @return RET_ERROR - command is handled with errors. Default Response will be send if requested.
   */
  ZB_ZCL_CALENDAR_GET_SPECIAL_DAYS_CB_ID,
  /** @b Server. Ask user about payload for CancelCalendar request
   *
   * User's application callback is initialized by RET_ERROR status of device
   * callback parameters.
   * @param[out] param_out @ref zb_zcl_calendar_cancel_calendar_payload_t
   *
   * One of the following statuses must be returned:
   * @return RET_OK - successfully handle command.
   *                  Send @ref ZB_ZCL_CALENDAR_SRV_CMD_CANCEL_CALENDAR "CancelCalendar" command.
   * @return RET_NOT_FOUND - no calendar is found. Default Response will be send if requested.
   * @return RET_ERROR - command is handled with errors. Default Response will be send if requested.
   */
  ZB_ZCL_CALENDAR_GET_CALENDAR_CANCELLATION_CB_ID,
  /** @b Client. Inform user about PublishCalendar request
   *
   * User's application callback is initialized by RET_ERROR status of device
   * callback parameters.
   * @param[in] param_in @ref zb_zcl_calendar_publish_calendar_payload_t
   *
   * One of the following statuses must be returned:
   * @return RET_OK - successfully handle command. Default Response will be send if requested.
   * @return RET_NOT_FOUND - IssuerCalendarID doesn't match with one of the stored
   *                         calendar instances. Default Response will be send if requested.
   * @return RET_NO_MEMORY - not able to store new calendar instance. Default Response will be send if requested.
   * @return RET_ERROR - command is handled with errors. Default Response will be send if requested.
   */
  ZB_ZCL_CALENDAR_PUBLISH_CALENDAR_CB_ID,
  /** @b Client. Calendar Inform user about PublishDayProfile request
   *
   * User's application callback is initialized by RET_ERROR status of device
   * callback parameters.
   * @param[in] param_in @ref zb_zcl_calendar_publish_day_profile_payload_t
   *
   * One of the following statuses must be returned:
   * @return RET_OK - successfully handle command. Default Response will be send if requested.
   * @return RET_NOT_FOUND - IssuerCalendarID doesn't match with one of the stored
   *                         calendar instances. Default Response will be send if requested.
   * @return RET_NO_MEMORY - not able to store new calendar instance. Default Response will be send if requested.
   * @return RET_ERROR - command is handled with errors. Default Response will be send if requested.
   */
  ZB_ZCL_CALENDAR_PUBLISH_DAY_PROFILE_CB_ID,
  /** @b Client. Calendar Inform user about PublishWeekProfile request
   *
   * User's application callback is initialized by RET_ERROR status of device
   * callback parameters.
   * @param[in] param_in @ref zb_zcl_calendar_publish_week_profile_payload_t
   *
   * One of the following statuses must be returned:
   * @return RET_OK - successfully handle command. Default Response will be send if requested.
   * @return RET_NOT_FOUND - IssuerCalendarID doesn't match with one of the stored
   *                         calendar instances. Default Response will be send if requested.
   * @return RET_NO_MEMORY - not able to store new calendar instance. Default Response will be send if requested.
   * @return RET_ERROR - command is handled with errors. Default Response will be send if requested.
   */
  ZB_ZCL_CALENDAR_PUBLISH_WEEK_PROFILE_CB_ID,
  /** @b Client. Calendar Inform user about PublishSeasons request
   *
   * User's application callback is initialized by RET_ERROR status of device
   * callback parameters.
   * @param[in] param_in @ref zb_zcl_calendar_publish_seasons_payload_t
   *
   * One of the following statuses must be returned:
   * @return RET_OK - successfully handle command. Default Response will be send if requested.
   * @return RET_NOT_FOUND - IssuerCalendarID doesn't match with one of the stored
   *                         calendar instances. Default Response will be send if requested.
   * @return RET_NO_MEMORY - not able to store new calendar instance. Default Response will be send if requested.
   * @return RET_ERROR - command is handled with errors. Default Response will be send if requested.
   */
  ZB_ZCL_CALENDAR_PUBLISH_SEASONS_CB_ID,
  /** @b Client. Calendar Inform user about PublishSpecialDays request
   *
   * User's application callback is initialized by RET_ERROR status of device
   * callback parameters.
   * @param[in] param_in @ref zb_zcl_calendar_publish_special_days_payload_t
   *
   * One of the following statuses must be returned:
   * @return RET_OK - successfully handle command. Default Response will be send if requested.
   * @return RET_NOT_FOUND - IssuerCalendarID doesn't match with one of the stored
   *                         calendar instances. Default Response will be send if requested.
   * @return RET_NO_MEMORY - not able to store new calendar instance. Default Response will be send if requested.
   * @return RET_ERROR - command is handled with errors. Default Response will be send if requested.
   */
  ZB_ZCL_CALENDAR_PUBLISH_SPECIAL_DAYS_CB_ID,
  /** @b Client. Calendar Inform user about CancelCalendar request
   *
   * User's application callback is initialized by RET_ERROR status of device
   * callback parameters.
   * @param[in] param_in @ref zb_zcl_calendar_cancel_calendar_payload_t
   *
   * One of the following statuses must be returned:
   * @return RET_OK - successfully handle command. Default Response will be send if requested.
   * @return RET_NOT_FOUND - IssuerCalendarID doesn't match with one of the stored
   *                         calendar instances. Default Response will be send if requested.
   * @return RET_NO_MEMORY - not able to store new calendar instance. Default Response will be send if requested.
   * @return RET_ERROR - command is handled with errors. Default Response will be send if requested.
   */
  ZB_ZCL_CALENDAR_CANCEL_CALENDAR_CB_ID,
  /** @endcond */ /* DOXYGEN_ZCL_SECTION && DOXYGEN_SE_SECTION */
#endif /* ZB_ENABLE_SE || ZB_ZCL_SUPPORT_CLUSTER_CALENDAR */

#if defined ZB_ENABLE_SE || defined ZB_ZCL_SUPPORT_CLUSTER_DAILY_SCHEDULE
  /** @cond DOXYGEN_ZCL_SECTION && DOXYGEN_SE_SECTION */
  /** @b Server. Inform user about GetScheduleCancellation request.
   *
   * @param[out] param_out @ref zb_zcl_daily_schedule_cancel_schedule_payload_t
   *
   * One of the following statuses must be returned:
   * @return RET_OK - command is handled successfully. Send @ref
   * ZB_ZCL_DAILY_SCHEDULE_SRV_CMD_CANCEL_SCHEDULE "CancelSchedule" command.
   * @return RET_ERROR - command is handled with errors.
   *
   */
  ZB_ZCL_DAILY_SCHEDULE_GET_SCHEDULE_CANCELLATION_CB_ID,
  /** @b Server. Inform user about GetDayProfile request.
   *
   * @param[in] param_in @ref zb_zcl_daily_schedule_get_day_profile_payload_t
   * @param[out] param_out @ref zb_zcl_daily_schedule_publish_day_profile_payload_t
   *
   * One of the following statuses must be returned:
   * @return RET_OK - command is handled successfully. Send @ref
   * ZB_ZCL_DAILY_SCHEDULE_SRV_CMD_PUBLISH_DAY_PROFILE "PublishDayProfile" command.
   * Application may put one or several day schedule entries.
   * @return RET_ERROR - command is handled with errors.
   *
   */
  ZB_ZCL_DAILY_SCHEDULE_GET_DAY_PROFILE_CB_ID,
  /** @b Server. Inform user about GetSchedule request.
   *
   * @param[in] param_in @ref zb_zcl_daily_schedule_get_schedule_payload_t
   * @param[out] param_out @ref zb_zcl_daily_schedule_publish_schedule_payload_t
   *
   * One of the following statuses must be returned:
   * @return RET_OK - command is handled successfully. Send @ref
   * ZB_ZCL_DAILY_SCHEDULE_SRV_CMD_PUBLISH_SCHEDULE "PublishSchedule" command.
   * @return RET_ERROR - command is handled with errors.
   *
   */
  ZB_ZCL_DAILY_SCHEDULE_GET_SCHEDULE_CB_ID,
  /** @b Client. Inform user about PublishSchedule cmd.
   *
   * @param[in] param_in @ref zb_zcl_daily_schedule_publish_schedule_payload_t
   *
   * One of the following statuses must be returned:
   * @return RET_OK - command is handled successfully.
   * @return RET_ERROR - command is handled with errors.
   *
   */
  ZB_ZCL_DAILY_SCHEDULE_PUBLISH_SCHEDULE_CB_ID,
  /** @b Client. Inform user about PublishDayProfile cmd.
   *
   * @param[in] param_in @ref zb_zcl_daily_schedule_publish_schedule_payload_t
   *
   * One of the following statuses must be returned:
   * @return RET_OK - command is handled successfully.
   * @return RET_ERROR - command is handled with errors.
   *
   */
  ZB_ZCL_DAILY_SCHEDULE_PUBLISH_DAY_PROFILE_CB_ID,
  /** @b Client. Inform user about CancelSchedule cmd.
   *
   * @param[in] param_in @ref zb_zcl_daily_schedule_cancel_schedule_payload_t
   *
   * One of the following statuses must be returned:
   * @return RET_OK - command is handled successfully.
   * @return RET_ERROR - command is handled with errors.
   *
   */
  ZB_ZCL_DAILY_SCHEDULE_CANCEL_SCHEDULE_CB_ID,
  /** @b Client. Inform user about CancelAllSchedules cmd.
   *
   * One of the following statuses must be returned:
   * @return RET_OK - command is handled successfully.
   * @return RET_ERROR - command is handled with errors.
   *
   */
  ZB_ZCL_DAILY_SCHEDULE_CANCEL_ALL_SCHEDULES_CB_ID,
  /** @endcond */ /* DOXYGEN_ZCL_SECTION && DOXYGEN_SE_SECTION */
#endif /* ZB_ENABLE_SE || ZB_ZCL_SUPPORT_CLUSTER_DAILY_SCHEDULE */

#if defined ZB_ENABLE_SE || defined ZB_ZCL_SUPPORT_CLUSTER_ENERGY_MANAGEMENT || defined DOXYGEN
  /** @cond DOXYGEN_ZCL_SECTION && DOXYGEN_SE_SECTION */
  /** @b Client. Inform user about ReportEventStatus request
   *
   * User's application callback is initialized by RET_OK status of device
   * callback parameters.
   * @param[in] param_in @ref zb_zcl_energy_management_report_event_status_payload_t
   *
   * One of the following statuses must be returned:
   * @return RET_OK - successfully handle command.
   * @return RET_ERROR - command is handled with errors.
   */
  ZB_ZCL_ENERGY_MANAGEMENT_REPORT_EVENT_STATUS_CB_ID,
  /** @b Server. Inform user about ManageEvent request
   *
   * User's application callback is initialized by RET_ERROR status of device
   * callback parameters.
   *
   * @param[in] param_in @ref zb_zcl_energy_management_manage_event_payload_t
   * @param[out] param_out @ref zb_zcl_energy_management_report_event_status_payload_t
   *
   * One of the following statuses must be returned:
   * @return RET_OK - successfully handle command.
   *                  Send @ref ZB_ZCL_ENERGY_MANAGEMENT_SRV_CMD_REPORT_EVENT_STATUS "ReportEventStatusCommandPayload" command.
   * @return RET_ERROR - command is handled with errors. Default Response will be send if requested.
   */
  ZB_ZCL_ENERGY_MANAGEMENT_MANAGE_EVENT_CB_ID,
  /** @endcond */ /* DOXYGEN_ZCL_SECTION && DOXYGEN_SE_SECTION */
#endif /* ZB_ENABLE_SE || ZB_ZCL_SUPPORT_CLUSTER_ENERGY_MANAGEMENT */

#if defined ZB_ENABLE_SE || defined ZB_ZCL_SUPPORT_CLUSTER_MDU_PAIRING
  /** @cond DOXYGEN_ZCL_SECTION && DOXYGEN_SE_SECTION */
  /** @b Server. Inform user about MDU Paring Request. You may provide MDU list as reply
   *
   * User's application callback is initialized by RET_ERROR status of device
   * callback parameters.
   *
   * @param[in] param_in @ref zb_zcl_mdu_pairing_request_t
   * @param[out] param_out @ref zb_zcl_mdu_pairing_response_t
   *
   * One of the following statuses must be returned:
   * @return RET_OK - successfully handle command.
   *                  Send @ref ZB_ZCL_MDU_PAIRING_SRV_CMD_PAIRING_RESPONSE "MDUPairingResponse" command
   * @return RET_ERROR - command is handled with errors. Default Response will be send if requested.
   */
  ZB_ZCL_MDU_PAIRING_REQUEST_CB_ID,
  /** @b Client. Inform user about MDU Pairing Response. List of MDU Paired devices passed
   *
   * User's application callback is initialized by RET_OK status of device
   * callback parameters.
   * @param[in] param_in @ref zb_zcl_mdu_pairing_response_t
   *
   * One of the following statuses must be returned:
   * @return RET_OK - successfully handle command.
   * @return RET_ERROR - command is handled with errors.
   */
  ZB_ZCL_MDU_PAIRING_RESPONSE_CB_ID,
  /** @endcond */ /* DOXYGEN_ZCL_SECTION && DOXYGEN_SE_SECTION */
#endif /* ZB_ENABLE_SE || ZB_ZCL_SUPPORT_CLUSTER_MDU_PAIRING */

  /** @b Server. Inform user about GetProfile request.
   *
   * User's application callback is initialized by RET_ERROR status of device
   * callback parameters.
   * @param[in] param_in @ref zb_zcl_metering_get_profile_payload_t
   * @param[out] param_out @ref zb_zcl_metering_get_profile_response_payload_t
   *
   * One of the following statuses must be returned:
   * @return RET_OK - successfully handle command.
   *                  Send @ref ZB_ZCL_METERING_SRV_CMD_GET_PROFILE_RESPONSE "GetProfileResponse" command.
   * @return RET_ERROR - command is handled with errors. Default Response will be send if requested.
   *
   */
  ZB_ZCL_METERING_GET_PROFILE_CB_ID,
  /** @b Server. Inform user about RequestFastPollMode request.
   *
   * User's application callback is initialized by RET_ERROR status of device
   * callback parameters.
   * @param[in] param_in @ref zb_zcl_metering_request_fast_poll_mode_payload_t
   * @param[out] param_out @ref zb_zcl_metering_request_fast_poll_mode_response_payload_t
   *
   * One of the following statuses must be returned:
   * @return RET_OK - successfully handle command.
   *                  Send @ref ZB_ZCL_METERING_SRV_CMD_REQUEST_FAST_POLL_MODE_RESPONSE "RequestFastPollModeResponse" command.
   * @return RET_ERROR - command is handled with errors. Default Response will be send if requested.
   *
   */
  ZB_ZCL_METERING_REQUEST_FAST_POLL_MODE_CB_ID,
  /** @b Server. Inform user about GetSnapshot request.
   *
   * User's application callback is initialized by RET_NOT_FOUND status of device
   * callback parameters.
   * @param[in] param_in @ref zb_zcl_metering_get_snapshot_payload_t
   *
   * One of the following statuses must be returned:
   * @return RET_OK - successfully handle command.  In this case application must send reply using
   *                  new buffer. Stack free buffer in this case.
   * @return RET_NOT_FOUND - price data isn't available. Default Response will be send if requested.
   * @return RET_ERROR - command is handled with errors. Default Response will be send if requested.
   *
   */
  ZB_ZCL_METERING_GET_SNAPSHOT_CB_ID,
  /** @b Server. Inform user about GetSampledData request.
   *
   * User's application callback is initialized by RET_NOT_FOUND status of device
   * callback parameters.
   * @param[in] param_in @ref zb_zcl_metering_get_sampled_data_payload_t
   * @param[out] param_out @ref zb_zcl_metering_get_sampled_data_response_payload_t
   *
   * One of the following statuses must be returned:
   * @return RET_OK - successfully handle command.
   *                  Send @ref ZB_ZCL_METERING_SRV_CMD_GET_SAMPLED_DATA_RESPONSE "GetSampledData" command.
   * @return RET_NOT_FOUND - price data isn't available. Default Response will be send if requested.
   * @return RET_ERROR - command is handled with errors. Default Response will be send if requested.
   *
   */
  ZB_ZCL_METERING_GET_SAMPLED_DATA_CB_ID,
  /** @b Client. Inform user about response to GetProfileResponse command.
   *
   * User's application callback is initialized by RET_ERROR status of device
   * callback parameters.
   * @param[in] param_in @ref zb_zcl_metering_get_profile_response_payload_t
   *
   * One of the following statuses must be returned:
   * @return RET_OK - successfully handle command. Default Response will be send if requested.
   * @return RET_ERROR - command is handled with errors. Default Response will be send if requested.
   *
   */
  ZB_ZCL_METERING_GET_PROFILE_RESPONSE_CB_ID,
  /** @b Client. Inform user about response to RequestFastPollMode command.
   *
   * User's application callback is initialized by RET_ERROR status of device
   * callback parameters.
   * @param[in] param_in @ref zb_zcl_metering_request_fast_poll_mode_response_payload_t
   *
   * One of the following statuses must be returned:
   * @return RET_OK - successfully handle command. Default Response will be send if requested.
   * @return RET_ERROR - command is handled with errors. Default Response will be send if requested.
   *
   */
  ZB_ZCL_METERING_REQUEST_FAST_POLL_MODE_RESPONSE_CB_ID,
  /** @b Client. Inform user about response to GetSnapshot command.
   *
   * User's application callback is initialized by RET_ERROR status of device
   * callback parameters.
   * @param[in] param_in @ref zb_zcl_metering_publish_snapshot_payload_t
   *
   * One of the following statuses must be returned:
   * @return RET_OK - successfully handle command. Default Response will be send if requested.
   * @return RET_ERROR - command is handled with errors. Default Response will be send if requested.
   *
   */
  ZB_ZCL_METERING_PUBLISH_SNAPSHOT_CB_ID,
  /** @b Client. Inform user about response to GetSampledData command.
   *
   * User's application callback is initialized by RET_ERROR status of device
   * callback parameters.
   * @param[in] param_in @ref zb_zcl_metering_get_sampled_data_response_payload_t
   *
   * One of the following statuses must be returned:
   * @return RET_OK - successfully handle command. Default Response will be send if requested.
   * @return RET_ERROR - command is handled with errors. Default Response will be send if requested.
   *
   */
  ZB_ZCL_METERING_GET_SAMPLED_DATA_RESPONSE_CB_ID,
/** @cond DOXYGEN_INTERNAL_DOC */
  /** Inform user about APS fragmented data transfer completion */
  ZB_ZCL_BIG_DATA_TRANSFER_COMPLETE_CB_ID,
/** @endcond */ /* DOXYGEN_INTERNAL_DOC */
#ifdef ZB_ENABLE_SE
  /** Inform user about Time receiving from Time server */
  ZB_ZCL_TIME_SYNC_CB_ID,
  ZB_ZCL_TIME_SYNC_FAILED_CB_ID,
#endif /* ZB_ENABLE_SE */
  /** @b Server. Inform user about Window Covering Up/Open command.
   *
   * User's application callback is initialized by RET_OK status of device
   * callback parameters.
   *
   * One of the following statuses must be returned:
   * @return RET_OK - successfully handle command. Default Response will be send if requested.
   * @return RET_ERROR - command is handled with errors.
   *
   */
  ZB_ZCL_WINDOW_COVERING_UP_OPEN_CB_ID,
  /** @b Server. Inform user about Window Covering Down/Close command.
   *
   * User's application callback is initialized by RET_OK status of device
   * callback parameters.
   *
   * One of the following statuses must be returned:
   * @return RET_OK - successfully handle command. Default Response will be send if requested.
   * @return RET_ERROR - command is handled with errors.
   *
   */
  ZB_ZCL_WINDOW_COVERING_DOWN_CLOSE_CB_ID,
  /** @b Server. Inform user about Window Covering Stop command.
   *
   * User's application callback is initialized by RET_OK status of device
   * callback parameters.
   *
   * One of the following statuses must be returned:
   * @return RET_OK - successfully handle command. Default Response will be send if requested.
   * @return RET_ERROR - command is handled with errors.
   *
   */
  ZB_ZCL_WINDOW_COVERING_STOP_CB_ID,

  /** @b Server. Inform user about Window Covering Go to Lift Value command.
   *
   * User's application callback is initialized by RET_OK status of device
   * callback parameters.
   * @param[in] param_in @ref zb_zcl_go_to_lift_value_req_t
   *
   * One of the following statuses must be returned:
   * @return RET_OK - successfully handle command. Default Response will be send if requested.
   * @return RET_ERROR - command is handled with errors.
   *
   */
  ZB_ZCL_WINDOW_COVERING_GO_TO_LIFT_VALUE_CB_ID,
  /** @b Server. Inform user about Window Covering Go to Lift Percentage command.
   *
   * User's application callback is initialized by RET_OK status of device
   * callback parameters.
   * @param[in] param_in @ref zb_zcl_go_to_lift_percentage_req_t
   *
   * One of the following statuses must be returned:
   * @return RET_OK - successfully handle command. Default Response will be send if requested.
   * @return RET_ERROR - command is handled with errors.
   *
   */
  ZB_ZCL_WINDOW_COVERING_GO_TO_LIFT_PERCENTAGE_CB_ID,
  /** @b Server. Inform user about Window Covering Go to Tilt Value command.
   *
   * User's application callback is initialized by RET_OK status of device
   * callback parameters.
   * @param[in] param_in @ref zb_zcl_go_to_tilt_value_req_t
   *
   * One of the following statuses must be returned:
   * @return RET_OK - successfully handle command. Default Response will be send if requested.
   * @return RET_ERROR - command is handled with errors.
   *
   */
  ZB_ZCL_WINDOW_COVERING_GO_TO_TILT_VALUE_CB_ID,
  /** @b Server. Inform user about Window Covering Go to Tilt Percentage command.
   *
   * User's application callback is initialized by RET_OK status of device
   * callback parameters.
   * @param[in] param_in @ref zb_zcl_go_to_tilt_percentage_req_t
   *
   * One of the following statuses must be returned:
   * @return RET_OK - successfully handle command. Default Response will be send if requested.
   * @return RET_ERROR - command is handled with errors.
   *
   */
  ZB_ZCL_WINDOW_COVERING_GO_TO_TILT_PERCENTAGE_CB_ID,
  /** @b Server. Inform application about incoming Add Scene request.
   *
   * User's application callback is initialized by RET_ERROR status of device
   * callback parameters.
   * @param[in] param_in @ref zb_zcl_scenes_add_scene_req_t Fixed part of Add Scene request. Scene
   * Extension Set is placed after this structure (no little-endian conversion).
   * @param[out] param_out @ref zb_uint8_t ZCL status - @ref ZB_ZCL_STATUS_INVALID_FIELD,
   * @ref ZB_ZCL_STATUS_SUCCESS or @ref ZB_ZCL_STATUS_INSUFF_SPACE
   *
   * One of the following statuses must be returned:
   * @return RET_OK - successfully handle command (except the case of updating existing entry). Send
   * @ref ZB_ZCL_CMD_SCENES_ADD_SCENE_RESPONSE "Add Scene Response" command (with ZCL status from
   * param_out).
   * @return RET_ALREADY_EXISTS - successfully handle command (update existing entry). Send
   * @ref ZB_ZCL_CMD_SCENES_ADD_SCENE_RESPONSE "Add Scene Response" command (with ZCL status from param_out).
   * @return RET_ERROR - command is handled with errors. Default Response will be send if requested.
   *
   */
  ZB_ZCL_SCENES_ADD_SCENE_CB_ID,
  /** @b Server. Inform application about incoming Store Scene request.
   *
   * User's application callback is initialized by RET_ERROR status of device
   * callback parameters.
   * @param[in] param_in @ref zb_zcl_scenes_store_scene_req_t
   * @param[out] param_out @ref zb_uint8_t ZCL status - @ref ZB_ZCL_STATUS_INVALID_FIELD,
   * @ref ZB_ZCL_STATUS_SUCCESS or @ref ZB_ZCL_STATUS_INSUFF_SPACE
   *
   * One of the following statuses must be returned:
   * @return RET_OK - successfully handle command (except the case of updating existing entry). Send
   * @ref ZB_ZCL_CMD_SCENES_STORE_SCENE_RESPONSE "Store Scene Response" command (with ZCL status from
   * param_out).
   * @return RET_ALREADY_EXISTS - successfully handle command (update existing entry). Send
   * @ref ZB_ZCL_CMD_SCENES_STORE_SCENE_RESPONSE "Store Scene Response" command (with ZCL status from param_out).
   * @return RET_ERROR - command is handled with errors. Default Response will be send if requested.
   *
   */
  ZB_ZCL_SCENES_STORE_SCENE_CB_ID,
  /** @b Server. Inform application about incoming View Scene request.
   *
   * User's application callback is initialized by RET_ERROR status of device
   * callback parameters.
   * @param[in] param_in @ref zb_zcl_scenes_view_scene_req_t
   *
   * One of the following statuses must be returned:
   * @return RET_OK - successfully handle command. In this case application must send
   * @ref ZB_ZCL_CMD_SCENES_VIEW_SCENE_RESPONSE "View Scene Response" using new buffer. Stack free
   * buffer in this case.
   * @return RET_ERROR - command is handled with errors. Default Response will be send if requested.
   *
   */
  ZB_ZCL_SCENES_VIEW_SCENE_CB_ID,
  /** @b Server. Inform application about incoming Remove Scene request.
   *
   * User's application callback is initialized by RET_ERROR status of device
   * callback parameters.
   * @param[in] param_in @ref zb_zcl_scenes_remove_scene_req_t
   * @param[out] param_out @ref zb_uint8_t ZCL status - @ref ZB_ZCL_STATUS_INVALID_FIELD,
   * @ref ZB_ZCL_STATUS_SUCCESS or @ref ZB_ZCL_STATUS_NOT_FOUND
   *
   * One of the following statuses must be returned:
   * @return RET_OK - successfully handle command. Send
   * @ref ZB_ZCL_CMD_SCENES_REMOVE_SCENE_RESPONSE "Remove Scene Response" command (with ZCL status
   * from param_out).
   * @return RET_ERROR - command is handled with errors. Default Response will be send if requested.
   *
   */
  ZB_ZCL_SCENES_REMOVE_SCENE_CB_ID,
  /** @b Server. Inform application about incoming Remove All Scenes request.
   *
   * User's application callback is initialized by RET_ERROR status of device
   * callback parameters.
   * @param[in] param_in @ref zb_zcl_scenes_remove_all_scenes_req_t
   * @param[out] param_out @ref zb_uint8_t ZCL status - @ref ZB_ZCL_STATUS_INVALID_FIELD or
   * @ref ZB_ZCL_STATUS_SUCCESS
   *
   * One of the following statuses must be returned:
   * @return RET_OK - successfully handle command. Send
   * @ref ZB_ZCL_CMD_SCENES_REMOVE_ALL_SCENES_RESPONSE "Remove All Scenes Response" command (with
   * ZCL status from param_out).
   * @return RET_ERROR - command is handled with errors. Default Response will be send if requested.
   *
   */
  ZB_ZCL_SCENES_REMOVE_ALL_SCENES_CB_ID,
  /** @b Server. Inform application about incoming Recall Scene request.
   *
   * User's application callback is initialized by RET_ERROR status of device
   * callback parameters.
   * @param[in] param_in @ref zb_zcl_scenes_recall_scene_req_t
   *
   * One of the following statuses must be returned:
   * @return RET_OK - successfully handle command.
   * @return RET_ERROR - command is handled with errors. Default Response will be send if requested.
   *
   */
  ZB_ZCL_SCENES_RECALL_SCENE_CB_ID,
  /** @b Server. Inform application about incoming Get Scene Membership request.
   *
   * User's application callback is initialized by RET_ERROR status of device
   * callback parameters.
   * @param[in] param_in @ref zb_zcl_scenes_get_scene_membership_req_t
   *
   * One of the following statuses must be returned:
   * @return RET_OK - successfully handle command.  In this case application must send
   * @ref ZB_ZCL_CMD_SCENES_GET_SCENE_MEMBERSHIP_RESPONSE "Get Scene Membership Response" using new
   * buffer. Stack free buffer in this case.
   * @return RET_ERROR - command is handled with errors. Default Response will be send if requested.
   *
   */
  ZB_ZCL_SCENES_GET_SCENE_MEMBERSHIP_CB_ID,
#ifndef ZB_ZCL_SCENES_OPTIONAL_COMMANDS_DISABLED
  /** @b Server. Inform application about incoming Enhanced Add Scene request.
   *
   * User's application callback is initialized by RET_ERROR status of device
   * callback parameters.
   * @param[in] param_in @ref zb_zcl_scenes_add_scene_req_t Fixed part of Enhanced Add Scene request - it is
   * the same as for the Add Scene request, but the Transition Time field is measured in tenths of a second
   * rather than in seconds. Scene Extension Set is placed after this structure (no little-endian conversion).
   * @param[out] param_out @ref zb_uint8_t ZCL status - @ref ZB_ZCL_STATUS_INVALID_FIELD,
   * @ref ZB_ZCL_STATUS_SUCCESS or @ref ZB_ZCL_STATUS_INSUFF_SPACE
   *
   * One of the following statuses must be returned:
   * @return RET_OK - successfully handle command (except the case of updating existing entry). Send
   * @ref ZB_ZCL_CMD_SCENES_ADD_SCENE_RESPONSE "Add Scene Response" command (with ZCL status from
   * param_out).
   * @return RET_ALREADY_EXISTS - successfully handle command (update existing entry). Send
   * @ref ZB_ZCL_CMD_SCENES_ADD_SCENE_RESPONSE "Add Scene Response" command (with ZCL status from param_out).
   * @return RET_ERROR - command is handled with errors. Default Response will be send if requested.
   *
   */
  ZB_ZCL_SCENES_ENHANCED_ADD_SCENE_CB_ID,
  /** @b Server. Inform application about incoming Enhanced View Scene request.
   *
   * User's application callback is initialized by RET_ERROR status of device
   * callback parameters.
   * @param[in] param_in @ref zb_zcl_scenes_view_scene_req_t - it is the same as for View Scene request.
   *
   * One of the following statuses must be returned:
   * @return RET_OK - successfully handle command. In this case application must send
   * @ref ZB_ZCL_CMD_SCENES_VIEW_SCENE_RESPONSE "View Scene Response" using new buffer. Stack free
   * buffer in this case.
   * @return RET_ERROR - command is handled with errors. Default Response will be send if requested.
   *
   */
  ZB_ZCL_SCENES_ENHANCED_VIEW_SCENE_CB_ID,
  /** @b Server. Inform application about incoming Copy Scene request.
   *
   * User's application callback is initialized by RET_ERROR status of device
   * callback parameters.
   * @param[in] param_in @ref zb_zcl_scenes_copy_scene_req_t
   * @param[out] param_out zb_uint8_t *num_copied_scenes - a number of copied scenes.
   * It is used to increase @ref ZB_ZCL_ATTR_SCENES_SCENE_COUNT_ID attribute value in case of successfully scenes copying.
   *
   * One of the following statuses must be returned:
   * @return RET_OK - successfully handle command. In this case application must send
   * @ref ZB_ZCL_CMD_SCENES_COPY_SCENE_RESPONSE "Copy Scene Response" using new buffer. Stack free
   * buffer in this case.
   * @return RET_ERROR - command is handled with errors. Default Response will be send if requested.
   *
   */
  ZB_ZCL_SCENES_COPY_SCENE_CB_ID,
#endif /* !ZB_ZCL_SCENES_OPTIONAL_COMMANDS_DISABLED */
  /** @b Server. Internal: indicate to application that it is needed to remove all scenes on all
   * endpoints for given group_id.
   *
   * User's application callback is initialized by RET_ERROR status of device
   * callback parameters.
   * @param[in] param_in @ref zb_zcl_scenes_remove_all_scenes_req_t
   *
   * One of the following statuses must be returned:
   * @return RET_OK - successfully handle command.
   * @return RET_ERROR - command is handled with errors.
   *
   */
  ZB_ZCL_SCENES_INTERNAL_REMOVE_ALL_SCENES_ALL_ENDPOINTS_CB_ID,
  /** @b Server. Internal: indicate to application that it is needed to remove all scenes on all
   * endpoints for all groups (full reset of scenes table).
   *
   * User's application callback is initialized by RET_ERROR status of device
   * callback parameters.
   *
   * One of the following statuses must be returned:
   * @return RET_OK - successfully handle command.
   * @return RET_ERROR - command is handled with errors.
   *
   */
  ZB_ZCL_SCENES_INTERNAL_REMOVE_ALL_SCENES_ALL_ENDPOINTS_ALL_GROUPS_CB_ID,
  /** @b Server. Inform user about Door Lock Lock Door command.
   *
   * User's application callback is initialized by RET_OK status of device
   * callback parameters.
   * @param[out] param_out @ref zb_zcl_door_lock_read_lock_door_res_payload_t
   *
   * One of the following statuses must be returned:
   * @return RET_OK - successfully handle command.
   *                  Send @ref ZB_ZCL_DOOR_LOCK_SEND_LOCK_DOOR_RES "Lock Door Response" command.
   * @return RET_ERROR - command is handled with errors.
   *
   */
  ZB_ZCL_DOOR_LOCK_LOCK_DOOR_CB_ID,
  /** @b Server. Inform user about Door Lock Unlock Door command.
   *
   * User's application callback is initialized by RET_OK status of device
   * callback parameters.
   * @param[out] param_out @ref zb_zcl_door_lock_read_unlock_door_res_payload_t
   *
   * One of the following statuses must be returned:
   * @return RET_OK - successfully handle command.
   *                  Send @ref ZB_ZCL_DOOR_LOCK_SEND_UNLOCK_DOOR_RES "Unlock Door Response" command.
   * @return RET_ERROR - command is handled with errors.
   *
   */
  ZB_ZCL_DOOR_LOCK_UNLOCK_DOOR_CB_ID,
  /** @b Client. Inform user about Door Lock Lock Door Response command.
   *
   * User's application callback is initialized by RET_OK status of device
   * callback parameters.
   * @param[in] param_in @ref zb_zcl_door_lock_read_lock_door_res_payload_t
   *
   * One of the following statuses must be returned:
   * @return RET_OK - successfully handle command. Default Response will be send if requested.
   * @return RET_ERROR - command is handled with errors.
   *
   */
  ZB_ZCL_DOOR_LOCK_LOCK_DOOR_RESP_CB_ID,
  /** @b Client. Inform user about Door Lock Unlock Door Response command.
   *
   * User's application callback is initialized by RET_OK status of device
   * callback parameters.
   * @param[in] param_in @ref zb_zcl_door_lock_read_unlock_door_res_payload_t
   *
   * One of the following statuses must be returned:
   * @return RET_OK - successfully handle command. Default Response will be send if requested.
   * @return RET_ERROR - command is handled with errors.
   *
   */
  ZB_ZCL_DOOR_LOCK_UNLOCK_DOOR_RESP_CB_ID,
  /** @b Server. Inform user about Alarms Reset Alarm command.
   *
   * User's application callback is initialized by RET_OK status of device
   * callback parameters.
   * @param[in] param_in @ref zb_zcl_alarms_reset_alarm_req_t
   *
   * One of the following statuses must be returned:
   * @return RET_OK - successfully handle command. Default Response will be send if requested.
   * @return RET_ERROR - command is handled with errors.
   *
   */
  ZB_ZCL_ALARMS_RESET_ALARM_CB_ID,
  /** @b Server. Inform user about Alarms Reset All Alarms command.
   *
   * User's application callback is initialized by RET_OK status of device
   * callback parameters.
   *
   * One of the following statuses must be returned:
   * @return RET_OK - successfully handle command. Default Response will be send if requested.
   * @return RET_ERROR - command is handled with errors.
   *
   */
  ZB_ZCL_ALARMS_RESET_ALL_ALARMS_CB_ID,
#ifdef ZB_ENABLE_CUSTOM_CLUSTERS
  ZB_ZCL_IR_BLASTER_TRANSMIT_IR_DATA_CB_ID,
  ZB_ZCL_IR_BLASTER_TRANSMISSION_STATUS_CB_ID,
  ZB_ZCL_IR_BLASTER_GET_IR_SIGNATURE_CB_ID,
  ZB_ZCL_IR_BLASTER_GET_IR_SIGNATURE_RESP_CB_ID,
#endif /* ZB_ENABLE_CUSTOM_CLUSTERS */
  /** @b Client. Inform user about Alarms Alarm command.
   * User's application callback is initialized by RET_OK status of device
   * callback parameters.
   * @param[in] param_in zb_zcl_alarms_alarm_res_t
   *
   * One of the following statuses must be returned:
   * @return RET_OK - successfully handled command. Response will be send if requested.
   * @return RET_ERROR - command is handled with errors
   *
  */
  ZB_ZCL_ALARMS_ALARM_CB_ID,
  /** @b Server. Inform user about receiving "Enable WWAH App Event Retry Algorithm" command.
   *
   * User's application callback is initialized by RET_NOT_FOUND status of device
   * callback parameters.
   * @param[in] param_in zb_zcl_wwah_enable_wwah_app_event_retry_algorithm_t
   *
   * One of the following statuses must be returned:
   * @return RET_OK - successfully handle command. Default Response will be send if requested.
   * @return RET_ERROR - command is handled with errors. Default Response will be send if requested.
   *
   */
  ZB_ZCL_WWAH_ENABLE_APP_EVENT_RETRY_ALGORITHM_CB_ID,
  /** @b Server. Inform user about receiving "Disable WWAH App Event Retry Algorithm" command.
   *
   * User's application callback is initialized by RET_NOT_FOUND status of device
   * callback parameters.
   * @param[in] param_in zb_zcl_wwah_enable_wwah_app_event_retry_algorithm_t
   *
   * One of the following statuses must be returned:
   * @return RET_OK - successfully handle command. Default Response will be send if requested.
   * @return RET_ERROR - command is handled with errors. Default Response will be send if requested.
   *
   */
  ZB_ZCL_WWAH_DISABLE_APP_EVENT_RETRY_ALGORITHM_CB_ID,
  /** @b Server. Inform user application about Control4 Network ZAP information
   * changes (event=ZB_ZCL_CONTROL4_NETWORK_ZAP_UPDATED) or failure to get changes
   * (event=ZB_ZCL_CONTROL4_NETWORK_ZAP_DISCOVER_FAILED) after expiring the
   * ZB_ZCL_CONTROL4_NETWORK_SEARCHING_ZAP_ATTEMPTS retries
   *
   * @param[in] param_in zb_zcl_control4_zap_info_notify_t
   */
  ZB_ZCL_CONTROL4_NETWORK_ZAP_INFO_CB_ID,
  /** @b Server. Inform user about receiving "Debug Report Query" command.
   * If debug report exists, application must return pointer to this report
   * User's application callback is initialized by RET_NOT_FOUND status of device
   * callback parameter.
   *
   * One of the following statuses must be returned:
   * @return RET_OK - successfully handle command. Debug Report Query Response will be send.
   *                  param out must point to zb_zcl_wwah_debug_report_t
   *                  For example:
   *                  zb_zcl_wwah_debug_report_t debug_report_table[10]
   *                  ZB_ZCL_DEVICE_CMD_PARAM_OUT_SET(param, &debug_report_table[4]);
   * @return RET_ERROR, RET_NOT_FOUND - command is handled with errors.
   *                  Default Response will be send.
   */
  ZB_ZCL_WWAH_DEBUG_REPORT_QUERY_CB_ID,
  /** @b Server. Inform user about receiving "Set IAS Zone Enrollment Method" command.
   *
   * One of the following statuses must be returned:
   * @return RET_OK - successfully handle command.
   * @return RET_ERROR - command is handled with errors.
   *                  Default Response will be send.
   */
  ZB_ZCL_WWAH_SET_IAS_ZONE_ENROLLMENT_METHOD_CB_ID,

} zb_zcl_device_callback_id_t;

/** @cond internals_doc */
/** Generic in/out params for cluster commands handlers. */
typedef struct zb_zcl_device_cmd_generic_param_s
{
  /** Input packet header metadata.
   * Represents packet header information for incoming packets.
   */
  const zb_zcl_parsed_hdr_t *in_cmd_info;

  /** Input packet payload. */
  const void *in;

  /** Output packet payload.
   * User application should fill the payload with specific data.
   * ZCL can fill some of fields of payload with default/calculated values.
   */
  void *out;
} zb_zcl_device_cmd_generic_param_t;
/** @endcond */ /* internals_doc */

/**
 *  @brief Data for Device user application callback.
 *  This structure has receive to User application callback (see @ref zb_callback_t).
 *  @hideinitializer
 * */
typedef struct zb_zcl_device_callback_param_s
{
  /** Type of device callback (see @ref zb_zcl_device_callback_id_e) */
  zb_zcl_device_callback_id_t device_cb_id;
  zb_uint8_t endpoint;
  zb_zcl_attr_access_t attr_type;
  /** Return status (see zb_ret_t) */
  zb_ret_t status;
  /** Callback custom data */
  union
  {
    zb_zcl_set_attr_value_param_t  set_attr_value_param;
#if defined (ZB_ZCL_SUPPORT_CLUSTER_ON_OFF)
    /* Off with effect command, On/Off cluster */
    zb_zcl_on_off_set_effect_value_param_t  on_off_set_effect_value_param;
    /* */
#endif
#if defined(ZB_ZCL_SUPPORT_CLUSTER_IDENTIFY)
    zb_zcl_identify_effect_value_param_t  identify_effect_value_param;
#endif

#if defined (ZB_ZCL_SUPPORT_CLUSTER_SHADE_CONFIG)
   // TODO: descript parameters struct for translate to User App ZB_ZCL_SHADE_SET_VALUE_CB_ID event
   zb_zcl_shade_set_value_param_t  shade_set_value_param;
   // TODO: descript parameters struct for translate to User App ZB_ZCL_SHADE_GET_VALUE_CB_ID event
   zb_zcl_shade_get_value_param_t  shade_get_value_param;
#endif
#if defined (ZB_ZCL_SUPPORT_CLUSTER_LEVEL_CONTROL)
   zb_zcl_level_control_set_value_param_t  level_control_set_value_param;
#endif
#if defined (ZB_ZCL_SUPPORT_CLUSTER_IAS_WD)
   zb_zcl_ias_wd_start_warning_value_param_t  start_warning_value_param;
#endif
#if defined ZB_ENABLE_HA
#if defined (ZB_ZCL_SUPPORT_CLUSTER_IAS_ZONE)
   zb_zcl_ias_zone_enroll_response_value_param_t enroll_response_value_param;
#endif
#endif /* defined ZB_ENABLE_HA */
    /* moved thermostat_value_param out of ZB_ENABLE_HA to be able to compile without that define */
#if defined ZB_ZCL_SUPPORT_CLUSTER_THERMOSTAT
   zb_zcl_thermostat_value_param_t thermostat_value_param;
   zb_zcl_thermostat_weekly_schedule_table_param_t thermostat_weekly_schedule_table_param;
#endif
#if defined (ZB_ZCL_SUPPORT_CLUSTER_IAS_WD)
   zb_zcl_ias_wd_squawk_value_param_t  squawk_value_param;
#endif
#ifdef ZB_ZCL_SUPPORT_CLUSTER_IR_BLASTER
   zb_zcl_ir_blaster_transmit_ir_data_value_param_t irb_tr_value_param;
   zb_zcl_ir_blaster_transmission_status_value_param_t irb_tr_status_value_param;
   zb_zcl_ir_blaster_get_ir_signature_value_param_t irb_get_ir_sig_value_param;
   zb_zcl_ir_blaster_get_ir_signature_resp_value_param_t irb_get_ir_sig_resp_value_param;
#endif
#if defined ZB_ENABLE_HA
#if defined ZB_HA_ENABLE_OTA_UPGRADE_CLIENT || defined DOXYGEN
   //
   zb_zcl_ota_upgrade_value_param_t  ota_value_param;
#endif /*defined ZB_HA_ENABLE_OTA_UPGRADE_CLIENT || defined DOXYGEN */
#if defined ZB_ZCL_SUPPORT_CLUSTER_POLL_CONTROL
   zb_zcl_poll_control_check_in_cli_param_t checkin_cli_param;
#endif /* defined ZB_ZCL_SUPPORT_CLUSTER_POLL_CONTROL */
#if defined ZB_HA_ENABLE_OTA_UPGRADE_SERVER
   zb_zcl_ota_upgrade_srv_query_img_param_t       ota_upgrade_srv_query_img_param;
   zb_zcl_ota_upgrade_srv_upgrade_started_param_t ota_upgrade_srv_upgrade_started_param;
   zb_zcl_ota_upgrade_srv_upgrade_aborted_param_t ota_upgrade_srv_upgrade_aborted_param;
   zb_zcl_ota_upgrade_srv_upgrade_end_param_t     ota_upgrade_srv_upgrade_end_param;
#endif
#if defined ZB_HA_ENABLE_OTA_UPGRADE_CLIENT
   zb_zcl_ota_upgrade_query_img_resp_param_t  ota_upgrade_query_img_resp_param;
#endif
#endif /* defined ZB_ENABLE_HA */
    zb_zcl_device_cmd_generic_param_t gnr;
  }
  cb_param;
} zb_zcl_device_callback_param_t;


/** @cond internals_doc */
/** The way of accessing to @ref zb_zcl_device_callback_param_t from buf ref. */
#define ZB_ZCL_DEVICE_CMD_PARAM(_param) \
    (ZB_BUF_GET_PARAM((_param), zb_zcl_device_callback_param_t))

/** Set INPUT device callback parameter with specific value. */
#define ZB_ZCL_DEVICE_CMD_PARAM_IN_SET(_param, _pvalue) \
  ((ZB_ZCL_DEVICE_CMD_PARAM(_param))->cb_param.gnr.in = _pvalue)

/** Set OUTPUT device callback parameter with specific value. */
#define ZB_ZCL_DEVICE_CMD_PARAM_OUT_SET(_param, _pvalue) \
  ((ZB_ZCL_DEVICE_CMD_PARAM(_param))->cb_param.gnr.out = _pvalue)

/** Init all fields of device callback params. */
#define ZB_ZCL_DEVICE_CMD_PARAM_INIT_WITH(_param, _cb_id, _status, _cmd_info, _in, _out)                              \
  (ZB_BZERO(ZB_ZCL_DEVICE_CMD_PARAM(_param), sizeof(*ZB_ZCL_DEVICE_CMD_PARAM(_param))),                               \
    (ZB_ZCL_DEVICE_CMD_PARAM_CB_ID(_param) = _cb_id,                                                                  \
      (ZB_ZCL_DEVICE_CMD_PARAM_ENDPOINT(_param) = (_cmd_info != NULL ?                                                \
       ZB_ZCL_PARSED_HDR_SHORT_DATA((typeof(ZB_ZCL_DEVICE_CMD_PARAM_CMD_INFO(_param)))_cmd_info).dst_endpoint : 0x00),\
        (ZB_ZCL_DEVICE_CMD_PARAM_STATUS(_param) = _status,                                                            \
         (ZB_ZCL_DEVICE_CMD_PARAM_CMD_INFO(_param) = _cmd_info,                                                       \
          (ZB_ZCL_DEVICE_CMD_PARAM_IN_SET(_param, _in),                                                               \
           (ZB_ZCL_DEVICE_CMD_PARAM_OUT_SET(_param, _out))))))))

/** @endcond */ /* internals_doc */

/** Get INPUT device callback parameter from buffer reference.
 * @param _param - Reference to buffer.
 * @param _ptype - Target data type.
 * @return - Pointer to @e _ptype data.
 * @see @ref zb_zcl_device_callback_param_t.
 */
#define ZB_ZCL_DEVICE_CMD_PARAM_IN_GET(_param, _ptype) \
  ((const _ptype *) ((ZB_ZCL_DEVICE_CMD_PARAM(_param))->cb_param.gnr.in))

/** Get OUTPUT device callback parameter from buffer reference.
 * @param _param - Reference to buffer.
 * @param _ptype - Target data type
 * @return - Pointer to @e _ptype data.
 */
#define ZB_ZCL_DEVICE_CMD_PARAM_OUT_GET(_param, _ptype) \
  ((_ptype *) ((ZB_ZCL_DEVICE_CMD_PARAM(_param))->cb_param.gnr.out))

/** Get access to @ref zb_zcl_parsed_hdr_t data from device callback parameter.
 * @param _param - Reference to buffer.
 * @return - Pointer to @ref zb_zcl_parsed_hdr_t structure.
 */
#define ZB_ZCL_DEVICE_CMD_PARAM_CMD_INFO(_param) \
  ((ZB_ZCL_DEVICE_CMD_PARAM(_param))->cb_param.gnr.in_cmd_info)

/** Get access to @ref zb_zcl_device_callback_param_t::device_cb_id of device callback parameters.
 * @param _param - Reference to buffer.
 */
#define ZB_ZCL_DEVICE_CMD_PARAM_CB_ID(_param) \
  ((ZB_ZCL_DEVICE_CMD_PARAM(_param))->device_cb_id)

/** Get access to @ref zb_zcl_device_callback_param_t::status of device callback parameters.
 * @param _param - Reference to buffer.
 * @return - zb_ret_t value of current status.
 */
#define ZB_ZCL_DEVICE_CMD_PARAM_STATUS(_param) \
  ((ZB_ZCL_DEVICE_CMD_PARAM(_param))->status)

/* Set ENDPOINT device callback parameter with specific value. */
#define ZB_ZCL_DEVICE_CMD_PARAM_ENDPOINT(_param) \
     ((ZB_ZCL_DEVICE_CMD_PARAM(_param))->endpoint)

/** @cond internals_doc */
/** @brief ZCL default handler.
  *
  * This function handles ZCL commands which was not processed by application.
  *
  * @param param - reference to the buffer containing ZCL packet.
  * @return @ref ZB_TRUE if packet was processed; @ref ZB_FALSE - otherwise
  */
zb_uint8_t zb_zcl_handle(zb_uint8_t param);
/**
   Get ZCL TSN stored in ZCL frame
   @param buffer - pointer to a buffer with ZCL frame
   @return TSN value
*/
zb_uint8_t zb_zcl_get_tsn_from_packet(zb_bufid_t buffer);

#if !(defined ZB_ZCL_DISABLE_REPORTING)

void zb_zcl_adjust_reporting_timer(zb_uint8_t param);

void zb_zcl_update_reporting_info(zb_zcl_reporting_info_t *rep_info);

zb_ret_t zb_zcl_put_default_reporting_info(zb_zcl_reporting_info_t* default_rep_info_ptr);

void zb_zcl_remove_default_reporting_info(zb_uint16_t cluster_id, zb_uint8_t cluster_role);

void zb_zcl_mark_report_not_sent(zb_zcl_reporting_info_t *rep_info);

#endif

/**
   Register device context.
   @param _device_ctx - pointer to device context

   Use @ref ZB_AF_REGISTER_DEVICE_CTX.
 */
void zb_zcl_register_device_ctx(zb_af_device_ctx_t *device_ctx);
void zb_zcl_init_endpoint(zb_af_endpoint_desc_t* ep);
zb_bool_t zb_zcl_check_cluster_list(void);
/** @endcond */ /* internals_doc */
/** @} */ /* ZB_ZCL_INITIALIZATION */


#ifdef ZB_BDB_MODE
/*! \addtogroup zboss_bdb_api
@{
 @defgroup zboss_bdb_comm_params BDB commissioning parameters
 @defgroup zboss_bdb_comm_start BDB commissioning start & status
 @defgroup zboss_bdb_comm_fb BDB Finding and Binding
@}
*/

/**
   @addtogroup zboss_bdb_comm_params
   @{
*/

/** @cond internals_doc */
/**
  @brief BDB error codes
  */
enum zb_bdb_error_codes_e
{
  ZB_BDB_STATUS_SUCCESS = 0,                 /*!<The commissioning sub-procedure was successful.*/
  ZB_BDB_STATUS_IN_PROGRESS,                 /*!< One of the commissioning sub-procedures has started but is not yet complete.*/
  ZB_BDB_STATUS_NOT_AA_CAPABLE,              /*!< The initiator is not address assignment capable during touchlink. */
  ZB_BDB_STATUS_NO_NETWORK,                  /*!< A network has not been found during network steering or touchlink.*/
  ZB_BDB_STATUS_TARGET_FAILURE,              /*!< A node has not joined a network when requested during touchlink.*/
  ZB_BDB_STATUS_FORMATION_FAILURE,           /*!< A network could not be formed during network formation. */
  ZB_BDB_STATUS_NO_IDENTIFY_QUERY_RESPONSE,  /*!< No response to an identify query command has been received during finding and binding.*/
  ZB_BDB_STATUS_BINDING_TABLE_FULL,          /*!< A binding table entry could not be created due to insufficient space in the binding table during finding and binding. */
  ZB_BDB_STATUS_NO_SCAN_RESPONSE,            /*!< No response to a scan request inter-PAN command has been received during touchlink. */
  ZB_BDB_STATUS_NOT_PERMITTED,               /*!< A touchlink (steal) attempt was made when a node is already connected to a centralized security network.
                                                  A node was instructed to form a network when it did not have a logical type of either Zigbee coordinator or Zigbee router.*/
  ZB_BDB_STATUS_TCLK_EX_FAILURE,             /*!< The Trust Center link key exchange procedure has failed attempting to join a centralized security network.*/
  ZB_BDB_STATUS_NOT_ON_A_NETWORK,            /*!< A commissioning procedure was forbidden since the node was not currently on a network.*/
  ZB_BDB_STATUS_ON_A_NETWORK,                /*!< A commissioning procedure was forbidden since the node was currently on a network.*/
  ZB_BDB_STATUS_CANCELLED,                    /*!< The current operation (steering or formation) was cancelled by an app */
  ZB_BDB_STATUS_DEV_ANNCE_SEND_FAILURE        /*!< A device announce sending has been failed (e.g. device announce haven't acked by parent router). */
};
/** @endcond */ /* internals_doc */
/** @} */

/**
   @addtogroup zboss_bdb_comm_start
   @{
*/

/** @brief BDB commissioning mode mask bits
 * This bitmask is out of BDB 3.1 spec but will continue to be used internally and as a parameter to the commissioning API
*/
typedef enum zb_bdb_commissioning_mode_mask_e
{
  /** @cond internals_doc */
  /** Used internally */
  ZB_BDB_INITIALIZATION = 0,
  /** @endcond */ /* internals_doc */
  /** @cond touchlink */
  /** Touchlink: 0 = Do not attempt Touchlink commissioning;
                  1 = Attempt Touchlink commissioning
   */
  ZB_BDB_TOUCHLINK_COMMISSIONING = 1,
  /** @endcond */ /* touchlink */

  /** Network steering: 0 = Do not attempt network steering;
                         1 = Attempt network steering
   */
  ZB_BDB_NETWORK_STEERING = 2,
  /** Network formation: 0 = Do not attempt to form a network;
                          1 = Attempt to form a network, according to device type2
   */
  ZB_BDB_NETWORK_FORMATION = 4,
  /** Finding and binding: 0 = Do not attempt finding and binding;
                          1 = Attempt finding and binding
    @note actually this mode does not call finding and binding procedure. Use
          @ref zb_bdb_finding_binding_target or @ref zb_bdb_finding_binding_initiator.
   */
  ZB_BDB_FINDING_N_BINDING = 8,

  /** @cond internals_doc */
  /* Used internally */
  ZB_BDB_LAST_COMMISSIONING_STEP = 0x10,
  /* Used internally */
  ZB_BDB_COMMISSIONING_STOP = 0x20,
  /** @endcond */ /* internals_doc */

  /** @cond touchlink */
  /* Used internally */
  ZB_BDB_TOUCHLINK_TARGET = 0x40,
  /** @endcond */ /* touchlink */
#ifndef BDB_OLD
  /** @cond internals_doc */
  /* Used internally */
  ZB_BDB_REJOIN = 0x80,
  /** @endcond */ /* internals_doc */
#endif
} zb_bdb_commissioning_mode_mask_t;

/**
   @brief Start device commissioning procedure specified step.

   Performs steering and network formation if appropriate for the device type. Finding and binding is not performed by this function (see note at @ref ZB_BDB_FINDING_N_BINDING)

   When the selected commissioning procedure finishes one of the following ZBOSS signals is generated:
    - @ref ZB_BDB_SIGNAL_STEERING
    - @ref ZB_BDB_SIGNAL_FORMATION

   @param mode_mask - bitmask of commissioning steps to perform, see @ref zb_bdb_commissioning_mode_mask_e

   @return ZB_TRUE - in case the device starts successfully
   @return ZB_FALSE - ZB_FALSE -- in case an error occurred (for example: the device has already been running)

   @b Example:
   @snippet linky_sample/erl_gw/erl_gw.c bdb_start_top_level_commissioning_snippet
  */
zb_bool_t bdb_start_top_level_commissioning(zb_uint8_t mode_mask);

/**
 * @brief Cannel touchlink target prodedure
 *
 * @return
 *    - RET_OK: On success
 *    - RET_ERROR: Not in touchlink target prodedure
 */
#ifdef ZB_BDB_TOUCHLINK
zb_ret_t bdb_cancel_touchlink_target(void);
#endif /* ZB_BDB_TOUCHLINK */

/**
   @brief Cancel Steering procedure for a node not on a network started with
   bdb_start_top_level_commissioning(ZB_BDB_NETWORK_STEERING).
   The ZBOSS signal ZB_BDB_SIGNAL_STEERING_CANCELLED with the status of this operation will be
   raised.
   Possible statuses:
   - RET_ILLEGAL_REQUEST (device is a ZC)
   - RET_INVALID_STATE (steering for a node not a network is not in progress)
   - RET_PENDING (it is too late to cancel a steering, it will be completed soon)
   - RET_IGNORE (cancellation was already requested)
   - RET_OK (steering is cancelled successfully)

   If the steering is cancelled the signal ZB_BDB_SIGNAL_STEERING with the status
   ZB_BDB_STATUS_CANCELLED will be raised as well.

   @param buf - a ZBOSS buffer
*/
void bdb_cancel_joining(zb_bufid_t buf);


/**
   @brief Cancel Formation procedure started with bdb_start_top_level_commissioning(ZB_BDB_NETWORK_FORMATION).
   The ZBOSS signal ZB_BDB_SIGNAL_FORMATION_CANCELLED with the status of the operation will be
   raised.
   Possible statuses:
   - RET_INVALID_STATE (formation is not in progress)
   - RET_PENDING (it is too late to cancel a formation, it will be completed soon)
   - RET_IGNORE (cancellation was already requested)
   - RET_OK (formation is cancelled successfully)

   If the formation is cancelled the signal ZB_BDB_SIGNAL_FORMATION with the status
   ZB_BDB_STATUS_FORMATION will be raised as well.

   @param buf - a ZBOSS buffer
*/
void bdb_cancel_formation(zb_bufid_t buf);


/**
   @brief Set scan duration for ED and Active scan

   @param duration - scan duration. Scan time is (aBaseSuperframeDuration * ((1<<duration) + 1)). In seconds - ((1l << duration) + 1) * 15360 / 1000000.

       For duration 8 ~4s
       For duration 5 ~0.5s
       For duration 2 ~0.08s
       For duration 1 ~0.05s (0.046s)
 */
void bdb_set_scan_duration(zb_uint8_t duration);

/**
 * @brief Close the network
 *
 * Implements BDB 3.0.1 - 8.1.1 "Local disabling of Network Steering."
 *
 * Will broadcast a Mgmt_Permit_Joining_req with PermitDuration of 0
 *
 * In case it is a router or coordinator it will also issue NLME-PERMIT-JOINING.request primitive with PermitDuration of 0
 * The ZBOSS signal @ref ZB_NWK_SIGNAL_PERMIT_JOIN_STATUS will be raised with @ref zb_zdo_mgmt_permit_joining_req_param_t.permit_duration of 0
 *
 * @param buf - a ZBOSS buffer, if zero is passed, a new buffer will be allocated
 * @return RET_OK if broadcast was successful
 * @return RET_NO_MEMORY if buffer allocation failed
 * @return RET_ERROR if any error occurred
 *
 * @snippet thermostat/thermostat_zr/thermostat_zr.c close_network_example
 */
zb_ret_t zb_bdb_close_network(zb_bufid_t buf);

/**
   Check if device is factory new.

   @return ZB_TRUE if factory new.
   @b Example:
@code
    if (!zb_bdb_is_factory_new())
    {
      zb_zdo_rejoin_backoff_start(ZB_FALSE);
    }
@endcode
*/
zb_bool_t zb_bdb_is_factory_new(void);

/** @} */

#if defined(ZB_BDB_ENABLE_FINDING_BINDING) || defined(DOXYGEN)

/**
   @addtogroup zboss_bdb_comm_fb
   @{
*/
/**
   @brief Starts EZ-Mode Finding and binding mechanism at the target's endpoint.

   Put device into identifying mode; default duration is 3 minutes.

   @param endpoint - target endpoint
   @return RET_OK if procedure was successfully started
   @return RET_INVALID_PARAMETER_1 - target endpoint not registered
   @return RET_INVALID_STATE - Finding and Binding already started or device is not joined

   @note: endpoint should be registered on target

   @b Example
   @snippet onoff_server/on_off_output_zc.c zb_bdb_finding_binding_target_usage
  */
zb_ret_t zb_bdb_finding_binding_target(zb_uint8_t endpoint);

  /**
   @brief Starts EZ-Mode Finding and binding mechanism at the target's endpoint with a given timeout

   @param endpoint - target endpoint
   @param commissioning_time_secs - time interval for device to be in identifying mode in seconds. Can't be less than 3 minutes

   @return RET_OK on success
   @return RET_INVALID_PARAMETER_1 - target endpoint not registered
   @return RET_INVALID_PARAMETER_2 - commissioning_time_secs is less than ZB_BDBC_MIN_COMMISSIONING_TIME_S
   @return RET_INVALID_STATE - Finding and Binding already started or device is not joined
  */
zb_ret_t zb_bdb_finding_binding_target_ext(zb_uint8_t endpoint, zb_uint16_t commissioning_time_secs);


/**
 * List of EZ-Mode binding callback states
 */
typedef enum zb_bdb_comm_binding_cb_state_e
{
  /** Previously user applied bind finished successfully */
  ZB_BDB_COMM_BIND_SUCCESS = 0,
  /** Previously user applied bind failed */
  ZB_BDB_COMM_BIND_FAIL = 1,
  /** Ask user whether to perform binding */
  ZB_BDB_COMM_BIND_ASK_USER = 2,
} zb_bdb_comm_binding_cb_state_t;

/**
 *  @brief BDB finding and binding callback template.
 *
 *  Function is used both to interact with user application, get decision
 *  if new binding is needed or not, and to report the binding result
 *
 *  @param status - status of the binding (ask user, success or fail) @see zb_bdb_comm_binding_cb_state_t
 *  @param addr - extended address of a device to bind
 *  @param ep - endpoint of a device to bind
 *  @param cluster - cluster ID to bind
 *  @return bool - agree or disagree
 *
 * @b Example:
 * @snippet onoff_server/on_off_switch_zed.c zb_bdb_finding_binding_initiator_cb_example
 */
typedef zb_bool_t (ZB_CODE * zb_bdb_comm_binding_callback_t)(
  zb_int16_t status, zb_ieee_addr_t addr, zb_uint8_t ep, zb_uint16_t cluster);

/**
 *  @brief Start BDB finding and binding procedure on initiator.
 *
 *  Summary: Finding and binding as initiator zb_bdb_finding_binding_initiator()
 *  returns RET_OK if procedure was started successfully, error code otherwise. To report procedure
 *  status, user callback is called. It may be called several times with Success status and only once
 *  with error status. If any error appears, finding and binding stops.
 *
 *  @param endpoint - initiator endpoint
 *  @param user_binding_cb - user callback, see @ref zb_bdb_comm_binding_callback_t
 *  @return bool - RET_OK or error code
 *
 * @b Example:
 *
 * Define callback:
 * @snippet onoff_server/on_off_switch_zed.c zb_bdb_finding_binding_initiator_cb_example
 *
 * Start finding and binding as initiator:
 * @snippet onoff_server/on_off_switch_zed.c zb_bdb_finding_binding_initiator
 */
zb_ret_t zb_bdb_finding_binding_initiator(zb_uint8_t endpoint, zb_bdb_comm_binding_callback_t user_binding_cb);

/**
 *  @brief Cancel previously started finding and binding procedure on all target endpoints
 */
void zb_bdb_finding_binding_target_cancel(void);

/**
 *  @brief  Cancel previously started finding and binding procedure on particular target endpoint
 *
 *  @param endpoint - target endpoint. The ZB_ZCL_BROADCAST_ENDPOINT(0xFF) value is treated as cancel on all target endpoints
 */
void zb_bdb_finding_binding_target_cancel_ep(zb_uint8_t endpoint);

/** Cancel previously started finding and binding procedure on initiator */
void zb_bdb_finding_binding_initiator_cancel(void);

/** @} */

#endif

/**
   @addtogroup zboss_bdb_comm_params
   @{
*/
/**
   Set the primary channel set for the BDB energy scan. Beacon request
   will be send on these channels
   @param channel_mask - Channel mask.

   @note Channel set is reset to zero after changing network role of the device
*/
void zb_set_bdb_primary_channel_set(zb_uint32_t channel_mask);
/**
   Get the primary channel set for the BDB energy scan.
   @return channel_mask - Channel mask.

   @note Channel set is reset to zero after changing network role of the device
*/
zb_uint32_t zb_get_bdb_primary_channel_set(void);

/**
   Set the secondary channel set for the BDB energy scan. Beacon request
   will be send on these channels if no network found after energy
   scan on the primary channels.
   @param channel_mask - Channel mask.
*/
void zb_set_bdb_secondary_channel_set(zb_uint32_t channel_mask);

/**
   Get the secondary channel set for the BDB energy scan.
   @return channel_mask - Channel mask.
*/
zb_uint32_t zb_get_bdb_secondary_channel_set(void);

/**
 Enable Zigbee PRO complaint commissioning support
 @param state - 1 to disable trust center require key exchange; 0 -
 to enable.
*/
void zb_bdb_set_legacy_device_support(zb_uint8_t state);

/** @} */

/**
   @addtogroup zboss_bdb_comm_start
   @{
*/
/**
  * @brief Set commissioning mode.
  * @param  commissioning_mode - bitfield with the bdbCommissioningMode
  * attribute. Set 1 to the corresponding bit to enable, 0 to disable.
  * All the possible mask bits are stored in @ref zb_bdb_commissioning_mode_mask_t
 */
void zb_set_bdb_commissioning_mode(zb_uint8_t commissioning_mode);

/** @} */

/**
   @addtogroup zboss_bdb_comm_params
   @{
*/
/**
 * Maximum endpoints of the "respondent" that can be served
 */
#define ZB_BDB_COMM_ACTIVE_ENDP_LIST_LEN 4

/**
 * Identify query responses queue size
 */
#define BDB_MAX_IDENTIFY_QUERY_RESP_NUMBER 4

/**
 * List of BDB commissioning states
 */
typedef enum zb_bdb_comm_state_e
{
  ZB_BDB_COMM_IDLE                       = 0,   /*!< EZ-Mode isn't invoked */
  ZB_BDB_COMM_FINDING_AND_BINDING        = 4,   /*!< EZ-Mode finding and binding in progress (on initiator) */
  ZB_BDB_COMM_FINDING_AND_BINDING_TARGET = 5,   /*!< EZ-Mode finding and binding in progress (on target) */
}
  zb_bdb_comm_state_t;

/** @} */

#endif /* ZB_BDB_MODE*/


/** \addtogroup ZB_ZCL_INITIALIZATION
 *  @{
 */

typedef struct zcl_cluster_handlers_s
{
  zb_uint16_t cluster_id;
  zb_uint8_t cluster_role;
  zb_zcl_cluster_handler_t cluster_handler;
  zb_zcl_cluster_check_value_t cluster_check_value;
  zb_zcl_cluster_write_attr_hook_t cluster_write_attr_hook;
} zcl_cluster_handlers_t;

#define ZB_ZCL_GENERAL_GET_CMD_LISTS_PARAM 0xFFU

#define ZB_ZCL_GATEWAY_EP_SIZE 5

#define ZB_ZCL_ACCEPTABLE_EP_INVALID_VALUE 0x00

#define ZB_ZCL_CLUSTER_HANDLERS_TABLE_SIZE 30

/* See ZCL8 spec, Table 2-10 Nomenclature for Data Value Range and Default */
/** ZCL8 non-value for int32 type, see subclause 2.6.2.8 */
#define ZB_ZCL_NON_VALUE_INT32 ((zb_int32_t)0x80000000)
/** ZCL8 non-value for int16 type, see subclause 2.6.2.8 */
#define ZB_ZCL_NON_VALUE_INT16 ((zb_int16_t)0x8000)
/** ZCL8 non-value for int8 type, see subclause 2.6.2.8 */
#define ZB_ZCL_NON_VALUE_INT8 ((zb_int8_t)0x80)
/** ZCL8 non-value for int24 type, see subclause 2.6.2.8 */
#define ZB_ZCL_NON_VALUE_INT24  ((zb_int24_t)  { .low = 0x0000U, .high = 0x80U })

/** ZCL8 non-value for uint8 type, see subclause 2.6.2.7 */
#define ZB_ZCL_NON_VALUE_UINT8  ((zb_uint8_t)0xff)
/** ZCL8 non-value for uint16 type, see subclause 2.6.2.7 */
#define ZB_ZCL_NON_VALUE_UINT16 ((zb_uint16_t)0xffff)
/** ZCL8 non-value for uint32 type, see subclause 2.6.2.7 */
#define ZB_ZCL_NON_VALUE_UINT32 ((zb_uint32_t)0xffffffff)
/** ZCL8 non-value for uint24 type, see subclause 2.6.2.7 */
#define ZB_ZCL_NON_VALUE_UINT24 ((zb_uint24_t) { .low = 0xffffU, .high = 0xffU })

typedef struct zb_discover_cmd_list
{
  zb_uint8_t received_cnt;
  zb_uint8_t *received;
  zb_uint8_t generated_cnt;
  zb_uint8_t *generated;
} zb_discover_cmd_list_t;

typedef zb_bool_t (*zb_zcl_is_high_freq_msg_func_t)(zb_bufid_t);
typedef zb_bool_t (*zb_zcl_block_zcl_cmd_t)(zb_zcl_parsed_hdr_t*);
typedef zb_bool_t (*zb_zcl_read_attr_resp_handler_t)(zb_bufid_t);

#if defined ZB_ENABLE_SE || defined ZB_BDB_ENABLE_FINDING_BINDING || defined ZB_ZCL_SUPPORT_CLUSTER_WWAH
typedef struct zb_zcl_func_selector_s
{
#ifdef ZB_ENABLE_SE
  zb_zcl_is_high_freq_msg_func_t is_high_freq_msg;
#endif /* ZB_ENABLE_SE */

#if defined ZB_SE_COMMISSIONING || (defined ZB_ZCL_SUPPORT_CLUSTER_WWAH && defined ZB_ZCL_ENABLE_WWAH_SERVER)
  zb_zcl_block_zcl_cmd_t block_zcl_cmd;
  zb_zcl_read_attr_resp_handler_t read_attr_resp_handler;
#endif /* ZB_SE_COMMISSIONING || (ZB_ZCL_SUPPORT_CLUSTER_WWAH && ZB_ZCL_ENABLE_WWAH_SERVER) */

#ifdef ZB_BDB_ENABLE_FINDING_BINDING
  zb_callback_t process_identify_query_res;
#endif /* ZB_BDB_ENABLE_FINDING_BINDING */
} zb_zcl_func_selector_t;

#endif /* ZB_ENABLE_SE || ZB_BDB_ENABLE_FINDING_BINDING || ZB_ZCL_SUPPORT_CLUSTER_WWAH */

/** @cond internals_doc */
typedef struct zb_zcl_globals_s
{
  /** @internal ZCL sequence number used when send zcl frames */
  zb_uint8_t seq_number;
  /** @internal Buffer to store runtime data during ZCL commands processing */

//TODO: can be used for SHADE PROCESSING?
  zb_bufid_t runtime_buf;

  /** @internal Store device context information */
  zb_af_device_ctx_t *device_ctx;
  /** @internal Array for internal device ctx and user device ctx. To be used
   * by ZGP. */
  zb_af_device_ctx_t *device_ctx_arr[2];
  /** @internal ZCL transaction hash */
  zcl_cb_hash_ent_t zcl_cb[ZCL_TRAN_TABLE_SIZE];
  /** @internal User defined callback, used to inform user about attribute value  modification */
  zb_zcl_modify_attr_value_cb_t modify_attr_value_cb;

#if !(defined ZB_ZCL_DISABLE_REPORTING)
  /** @internal User defined callback, is used to inform user about received attribute report */
  zb_zcl_report_attr_cb_t report_attr_cb;
  /** @internal User defined callback, is used to inform user that report was not received during
    * defined time interval */
  zb_zcl_no_reporting_cb_t no_reporting_cb;
  /** @internal User defined API callback, is used to connection with User Application */
#endif

  zb_callback_t device_cb;
  zb_callback_t checkin_cb;

#if !(defined ZB_ZCL_DISABLE_REPORTING)
  /** @internal Stores context information for reporting */
  zb_zcl_reporting_ctx_t reporting_ctx;
#endif

  /** @internal Set default value all attributes */
  zb_zcl_set_default_value_attr_cb_t set_default_attr_cb;
  /** @internal Check attribute value
   *
   * @deprecated This callback will be removed in the next Major release after February 2023
   *             Use @ref app_check_attr_value_manuf_cb instead
   */
  zb_zcl_app_check_attr_value_cb_t app_check_attr_value_cb;
  /** @internal Check attribute value with manufacturer specific code */
  zb_zcl_app_check_attr_value_manuf_cb_t app_check_attr_value_manuf_cb;
#ifdef ZB_HA_ENABLE_OTA_UPGRADE_SERVER
  zb_zcl_ota_upgrade_next_data_ind_t ota_upgrade_next_data_ind_cb;
#endif /* ZB_HA_ENABLE_OTA_UPGRADE_SERVER */
#ifdef ZB_HA_ENABLE_OTA_UPGRADE_CLIENT
  zb_zcl_ota_upgrade_cli_ctx_t ota_cli;
#endif

#if defined ZB_ENABLE_HA
  zb_zcl_disc_manuf_cmd_cb_t disc_manuf_cmd_cb;
#endif /* ZB_ENABLE_HA */

  zcl_cluster_handlers_t zcl_handlers[ZB_ZCL_CLUSTER_HANDLERS_TABLE_SIZE];
#ifdef ZB_CONTROL4_NETWORK_SUPPORT
  /* Control4 Network cluster specific handlers. Server and Client cluster roles */
  zcl_cluster_handlers_t control4_zcl_handlers[2];
#endif /* ZB_CONTROL4_NETWORK_SUPPORT */
  zb_uint8_t zcl_handlers_cnt;
  zb_discover_cmd_list_t *zb_zcl_cluster_cmd_list;
#ifdef ZB_ZCL_SUPPORT_CLUSTER_SUBGHZ
  zb_zcl_subghz_ctx_t subghz_ctx;         /* Sub-GHz cluster context of current device */
#endif /* ZB_ZCL_SUPPORT_CLUSTER_SUBGHZ */
#if (defined ZB_ZCL_SUPPORT_CLUSTER_WWAH && (defined ZB_ZCL_ENABLE_WWAH_SERVER || defined ZB_ZCL_ENABLE_WWAH_CLIENT))
  /* TODO: Split (client/server) ! */
  zb_zcl_wwah_context_t wwah_ctx;
#endif

#if defined ZB_ENABLE_SE || defined ZB_BDB_ENABLE_FINDING_BINDING || defined ZB_ZCL_SUPPORT_CLUSTER_WWAH
  zb_zcl_func_selector_t selector;
#endif /* ZB_ENABLE_SE || ZB_BDB_ENABLE_FINDING_BINDING || ZB_ZCL_SUPPORT_CLUSTER_WWAH */

  /** @internal ZCL backward compatibility mode for ZCL8 support, packets format depends on the mode */
  zb_uint8_t backward_comp_mode;

  /** @internal Through the callback ZBOSS requests an application for a peer Cluster revision attribute value */
  zb_zcl_peer_revision_cb_t peer_revision_cb;

  /** @internal Backward compatible status values mode. When enabled if diversifies some common statuses as it was in zcl6 and zcl7*/
  zb_uint8_t backward_compatible_statuses_mode;

  /** User callback to notify an application a broadcast EP command received */
  zb_device_handler_t broadcast_ep_cb;

  /** Allow these endpoints without cluster */
  zb_uint8_t gateway_ep[ZB_ZCL_GATEWAY_EP_SIZE];
} zb_zcl_globals_t;

#define ZCL_SELECTOR() ZG->zcl.selector

/**
   Returns ZCL context.
   @return pointer ZCL context
  */
zb_zcl_globals_t *zb_zcl_get_ctx(void);

/** @endcond */ /* internals_doc */

/** Returns pointer to ZCL context */
#define ZCL_CTX() (*zb_zcl_get_ctx())

/*! @} */ /* ZB_ZCL_INITIALIZATION */

#define ZB_ZCL_MAX_EP_NUMBER 10U

typedef zb_uint8_t *(*zb_zcl_put_payload_cb_t)(zb_uint8_t *, const void *);

void zb_zcl_send_cmd(
  zb_uint8_t param,
  const zb_addr_u *dst_addr,
  zb_aps_addr_mode_t dst_addr_mode,
  zb_uint8_t dst_ep,
  zb_zcl_frame_direction_t direction,
  zb_uint8_t src_ep,
  const void *payload,
  zb_uint8_t payload_size,
  zb_zcl_put_payload_cb_t put_payload,
  zb_zcl_cluster_id_t cluster_id,
  zb_zcl_disable_default_response_t def_resp,
  zb_uint8_t cmd_id,
  zb_callback_t cb
);

void zb_zcl_send_cmd_tsn(
  zb_uint8_t param,
  const zb_addr_u *dst_addr,
  zb_aps_addr_mode_t dst_addr_mode,
  zb_uint8_t dst_ep,
  zb_zcl_frame_direction_t direction,
  zb_uint8_t src_ep,
  const void *payload,
  zb_uint8_t payload_size,
  zb_zcl_put_payload_cb_t put_payload,
  zb_zcl_cluster_id_t cluster_id,
  zb_zcl_disable_default_response_t def_resp,
  zb_uint8_t cmd_id,
  zb_uint8_t tsn,
  zb_callback_t cb
);

zb_bool_t zb_zcl_send_default_handler(zb_uint8_t param,
  const zb_zcl_parsed_hdr_t *cmd_info, zb_zcl_status_t status);

void zb_zcl_send_default_resp_ext(zb_uint8_t param,
  const zb_zcl_parsed_hdr_t *cmd_info, zb_zcl_status_t status);

#if defined ZB_APS_ENCRYPTION_PER_CLUSTER
void zb_zcl_set_cluster_encryption(zb_uint8_t endpoint_id, zb_uint16_t cluster_id, zb_uint8_t encrypt);
#endif /* ZB_APS_ENCRYPTION_PER_CLUSTER */

/**
   Convert deprecated statuses into ZCL8 statuses.
   Status enumerations (see ZCL8 spec 2.6.3) list was changed in ZCL8 spec.
     LIMIT_REACHED, DUPLICATE_EXISTS, SUCCESS ==> SUCCESS
     INCONSISTENT_STARTUP_STATE, DEFINED_OUT_OF_BAND, ACTION_DENIED, HARDWARE_FAILURE, SOFTWARE_FAILURE ==> FAILURE
     WRITE_ONLY, NOT_AUTHORIZED ==> NOT_AUTHORIZED
     MALFORMED_COMMAND, INVALID_FIELD ==> INVALID_FIELD
     UNSUP_CLUSTER_COMMAND, UNSUP_GENERAL_COMMAND, UNSUP_MANUF_CLUSTER_COMMAND, UNSUP_MANUF_GENERAL_COMMAND ==> UNSUP_COMMAND
     INCONSISTENT, CALIBRATION_ERROR, RESERVED ==> RESERVED
   The function will convert statuses in ZB_ZCL_AUTO_MODE and ZB_ZCL_COMPATIBILITY_MODE ZCL8 backward compatibility modes.
   ZB_ZCL_LEGACY_MODE implies that application can still use obsolete statuses, so in that mode the function doesn't make any conversion.
   @param status - Status got over the air.
   @return - Converted status.
*/
zb_zcl_status_t zb_zcl_zcl8_statuses_conversion(zb_zcl_status_t status);

/* Gateway endpoints configuration */
zb_ret_t zb_zcl_add_gateway_endpoint(zb_uint8_t endpoint);
zb_ret_t zb_zcl_remove_gateway_endpoint(zb_uint8_t endpoint);

#endif /* ZBOSS_API_ZCL_H */
