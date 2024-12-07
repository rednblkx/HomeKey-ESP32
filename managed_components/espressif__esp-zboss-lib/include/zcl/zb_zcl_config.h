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
/* PURPOSE: Configuration for Zigbee cluster library
*/

#ifndef ZB_ZCL_CONFIG_H
#define ZB_ZCL_CONFIG_H 1

/** @cond DOXYGEN_ZCL_SECTION */

/************ ZCL specific configuration  ******************/

/**
 ZCL: transactions table size
*/
#define ZCL_TRAN_TABLE_SIZE 16U

/* DA: range check error */
/* #define ZB_DISABLE_TEMP_MEASUREMENT_TOLERANCE_ID */

/* #define ZB_ZCL_OTA_INCREMENTAL_QUERY_INTERVAL */

/* Enable CVC feature  */
#define ZB_ZCL_ENABLE_CVC

/* Disable Poll Control Check-Ins during OTA upgrade (to reduce the load) */
#define xZB_ZCL_NO_CHECKINS_DURING_OTA


/*********************** Cluster to command configuration **********************/

/* General commands default processing */
#define ZB_ZCL_ENABLE_DEFAULT_REPORT_ATTR_PROCESSING

/* Control per-cluster default command processing */

#if defined (ZB_ZCL_SUPPORT_CLUSTER_GROUPS)
#define ZB_ZCL_ENABLE_DEFAULT_GROUPS_PROCESSING
#endif /* ZB_ZCL_SUPPORT_CLUSTER_GROUPS */

#if defined (ZB_ZCL_SUPPORT_CLUSTER_GROUPS_CLIENT) 
#define ZB_ZCL_ENABLE_DEFAULT_GROUPS_PROCESSING_CLIENT
#endif /* ZB_ZCL_SUPPORT_CLUSTER_GROUPS_CLIENT */

/* Control per-cluster default scene extension processing */
#if defined ZB_ZCL_SUPPORT_CLUSTER_SCENES

#if defined ZB_ZCL_SUPPORT_CLUSTER_THERMOSTAT
#define ZB_ZCL_SUPPORT_THERMOSTAT_SCENE_EXTENSIONS
#endif /* defined ZB_ZCL_SUPPORT_CLUSTER_THERMOSTAT */

#endif /* defined ZB_ZCL_SUPPORT_CLUSTER_SCENES */

#if defined ZB_ZCL_SUPPORT_CLUSTER_POLL_CONTROL
#define ZB_HA_ENABLE_POLL_CONTROL_SERVER
#endif /* defined ZB_ZCL_SUPPORT_CLUSTER_POLL_CONTROL */

/* TODO: Divide Server and Client processing and turn on server part only when it is really needed -
 * OTA cluster is rather big in terms of ROM size! */
#if defined ZB_ZCL_SUPPORT_CLUSTER_OTA_UPGRADE
#define ZB_ZCL_ENABLE_DEFAULT_OTA_UPGRADE_PROCESSING
#define ZB_HA_ENABLE_OTA_UPGRADE_SERVER     1
#define ZB_HA_ENABLE_OTA_UPGRADE_CLIENT     1
#endif /* defined ZB_ZCL_SUPPORT_CLUSTER_OTA_UPGRADE */

#if defined ZB_ZCL_SUPPORT_CLUSTER_TUNNEL
#define ZB_ZCL_ENABLE_DEFAULT_TUNNEL_PROCESSING
#endif /* defined ZB_ENABLE_TUNNEL_CLUSTER */

#if defined ZB_ZCL_SUPPORT_CLUSTER_IR_BLASTER
#define ZB_ZCL_ENABLE_DEFAULT_IR_BLASTER_PROCESSING
#endif /* defined ZB_ENABLE_IR_BLASTER_CLUSTER */

#if defined  ZB_ZCL_SUPPORT_CLUSTER_DIAGNOSTICS
#define ZB_MAC_DIAGNOSTICS
#define ZDO_DIAGNOSTICS
#endif /* ZB_ZCL_SUPPORT_CLUSTER_DIAGNOSTICS */

/** @endcond */ /* DOXYGEN_ZCL_SECTION */

#endif /* ZB_ZCL_CONFIG_H */
