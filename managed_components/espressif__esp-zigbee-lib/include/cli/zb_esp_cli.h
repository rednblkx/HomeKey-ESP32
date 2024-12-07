/*
 * Copyright (c) 2021-2023 Espressif Systems (Shanghai) CO LTD
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

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief ESP Zigbee cli console initialization.
 */
void zb_cli_console_init(void);

/**
 * @brief ESP Zigbee cli console task.
 *
 * @param[in] argv Pointer to the command.
 */
void zb_cli_console_task(void *arg);

/**
 * @brief Set ESP Zigbee cli endpoint.
 *
 * @param[in] ep ESP Zigbee cli endpoint.
 */
void zb_cli_set_endpoint(uint8_t ep);

/**
 * @brief Get ESP Zigbee cli endpoint.
 *
 * @return ESP Zigbee cli endpoint value.
 */
uint8_t zb_cli_get_endpoint(void);

/**
 * @brief Get ESP Zigbee cli endpoint.
 *
 * @return
 *      - ZB_TRUE  Zigbee commissioning is started.
 *      - ZB_FALSE Zigbee commissioning is not started.
 */
bool zb_cli_is_stack_started(void);

/**
 * @brief The Handler to process zcl general command coming from the ESP Zigbee cli endpoint.
 *
 * @param[in] bufid Reference to a Zigbee stack buffer.
 *
 * @return
 *      - ZB_TRUE  ESP Zigbee zcl general command process successfully.
 *      - ZB_FALSE ESP Zigbee zcl general command process failed.
 */
bool esp_zb_cli_agent_ep_handler_general_cmd(uint8_t bufid);

/**
 * @brief The Handler to process attribute write/read command coming from the ESP Zigbee cli endpoint.
 *
 * @param[in] bufid Reference to a Zigbee stack buffer.
 *
 * @return
 *      - ZB_TRUE  ESP Zigbee zcl attribute command process successfully.
 *      - ZB_FALSE ESP Zigbee zcl attribute command process failed.
 */
bool esp_zb_cli_agent_ep_handler_attr(uint8_t bufid);

/**
 * @brief The Handler to process attribute report command coming from the ESP Zigbee cli endpoint.
 *
 * @param[in] bufid Reference to a Zigbee stack buffer.
 *
 * @return
 *      - ZB_TRUE  ESP Zigbee zcl attribute report command process successfully.
 *      - ZB_FALSE ESP Zigbee zcl attribute report command process failed.
 */
bool esp_zb_cli_agent_ep_handler_report(uint8_t bufid);

/**
 * @brief The Handler to process ping command coming from the ESP Zigbee cli endpoint.
 *
 * @param[in] bufid Reference to a Zigbee stack buffer.
 *
 * @return
 *      - ZB_TRUE  ESP Zigbee zcl ping command process successfully.
 *      - ZB_FALSE ESP Zigbee zcl ping command process failed.
 */
bool esp_zb_cli_agent_ep_handler_ping(uint8_t bufid);

/**
 * @brief Register the esp zigbee cli command
 *
 */
void zb_cli_register_command(void);

#ifdef __cplusplus
}
#endif
