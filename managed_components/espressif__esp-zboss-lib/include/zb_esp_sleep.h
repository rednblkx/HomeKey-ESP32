/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once
#include "sdkconfig.h"

#ifdef __cplusplus
extern "C" {
#endif

#if CONFIG_PM_ENABLE && CONFIG_FREERTOS_USE_TICKLESS_IDLE
#include "esp_err.h"
/**
 * @brief This function initializes the Zigbee sleep.
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_FAIL on failure
 *
 */
esp_err_t zb_esp_sleep_init(void);

/**
 * @brief This function performs the Zigbee sleep process.
 *
 */
void zb_esp_sleep_process(uint32_t sleep_time_ms);

/**
 * @brief This function performs the Zigbee wakeup process.
 *
 */
void zb_esp_wakeup_process(void);

/**
 * @brief This function enable the Zigbee sleep.
 *
 * @param[in] enable  Zigbee sleep enable set
 *
 */
void zb_esp_sleep_enable_set(bool enable);

/**
 * @brief This function get the Zigbee sleep enable or not.
 *
 * @return
 *      - True Zigbee sleep has been enable
 *      - False Zigbee sleep has been disable
 *
 */
bool zb_esp_sleep_enable_get(void);

/**
 * @brief The sleep task iteration.
 *
 * @param[in] time_ms  The max block time for each iteration, in milliseconds.
 *
 */
void zb_osif_sleep_iteration(uint32_t time_ms);

#endif // CONFIG_PM_ENABLE && CONFIG_FREERTOS_USE_TICKLESS_IDLE 

#ifdef __cplusplus
}
#endif
