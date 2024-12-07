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
/* PURPOSE: Frequency agility for GreenPower cluster
*/

#ifndef ZB_ZCL_GRPW_FREQ_AGILITY_H
#define ZB_ZCL_GRPW_FREQ_AGILITY_H 1

#define ZB_ZCL_CLUSTER_ID_GRPW_FREQ_AGILITY  0xFCFA

enum zb_zcl_grpw_freq_agility_attr_e
{
  ZB_ZCL_ATTR_GRPW_FREQ_AGILITY_BRIDGE_ADDR         = 0x0000,
  ZB_ZCL_ATTR_GRPW_FREQ_AGILITY_ACTIVE_CHANNEL_MASK = 0x0001,
};

#define ZB_ZCL_CLUSTER_ID_GRPW_FREQ_AGILITY_SERVER_ROLE_INIT (zb_zcl_cluster_init_t)NULL
#define ZB_ZCL_CLUSTER_ID_GRPW_FREQ_AGILITY_CLIENT_ROLE_INIT (zb_zcl_cluster_init_t)NULL

#endif /* ZB_ZCL_GRPW_FREQ_AGILITY_H */
