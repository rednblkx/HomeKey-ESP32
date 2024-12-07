/*
 * SPDX-FileCopyrightText: 2019-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <protocomm.h>
#include <protocomm_security.h>

#include "network_provisioning/manager.h"
#include "network_provisioning/network_config.h"
#include "network_provisioning/network_scan.h"
#include "network_ctrl.h"

#ifdef CONFIG_NETWORK_PROV_NETWORK_TYPE_THREAD
#include "openthread/link.h"
#endif // CONFIG_NETWORK_PROV_NETWORK_TYPE_THREAD

/**
 * @brief   Notify manager that provisioning is done
 *
 * Stops the provisioning. This is called by the get_status_handler()
 * when the status is connected. This has no effect if main application
 * has disabled auto stop on completion by calling
 * network_prov_mgr_disable_auto_stop()
 *
 * @return
 *  - ESP_OK      : Provisioning will be stopped
 *  - ESP_FAIL    : Failed to stop provisioning
 */
esp_err_t network_prov_mgr_done(void);

#ifdef CONFIG_NETWORK_PROV_NETWORK_TYPE_WIFI
/**
 * @brief   Start Wi-Fi AP Scan
 *
 * @param[in] blocking        Set true to return only after scanning is complete
 * @param[in] passive         Set true to perform passive scan instead of default active scan
 * @param[in] group_channels  Number of channels to scan in one go
 *                            (set to 0 for scanning all channels in one go)
 * @param[in] period_ms       Scan time (in milli-seconds) on each channel
 *
 * @return
 *  - ESP_OK    : Successfully started Wi-Fi scanning
 *  - ESP_FAIL  : Provisioning app not running
 */
esp_err_t network_prov_mgr_wifi_scan_start(bool blocking, bool passive,
        uint8_t group_channels,
        uint32_t period_ms);

/**
 * @brief   Use to query the state of Wi-Fi scan
 *
 * @return
 *  - true   : Scan finished
 *  - false  : Scan running
 */
bool network_prov_mgr_wifi_scan_finished(void);

/**
 * @brief   Get the count of results in the scan list
 *
 * @return
 *  - count  : Number of Wi-Fi Access Points detected while scanning
 */
uint16_t network_prov_mgr_wifi_scan_result_count(void);

/**
 * @brief   Get AP record for a particular index in the scan list result
 *
 * @param[out] index  Index of the result to fetch
 *
 * @return
 *  - result : Pointer to Access Point record
 */
const wifi_ap_record_t *network_prov_mgr_wifi_scan_result(uint16_t index);
#endif // CONFIG_NETWORK_PROV_NETWORK_TYPE_WIFI

#ifdef CONFIG_NETWORK_PROV_NETWORK_TYPE_THREAD
/**
 * @brief   Start Thread network Scan
 *
 * @param[in] blocking        Set true to return only after scanning is complete
 *
 * @return
 *  - ESP_OK    : Successfully started Thread scanning
 *  - ESP_FAIL  : Provisioning app not running
 */
esp_err_t network_prov_mgr_thread_scan_start(bool blocking, uint32_t channel_mask);

/**
 * @brief   Use to query the state of Thread scan
 *
 * @return
 *  - true   : Scan finished
 *  - false  : Scan running
 */
bool network_prov_mgr_thread_scan_finished(void);

/**
 * @brief   Get the count of results in the scan list
 *
 * @return
 *  - count  : Number of Thread networks detected while scanning
 */
uint16_t network_prov_mgr_thread_scan_result_count(void);

/**
 * @brief   Get Thread network record for a particular index in the scan list result
 *
 * @param[out] index  Index of the result to fetch
 *
 * @return
 *  - result : Pointer to Thread network record
 */
const otActiveScanResult *network_prov_mgr_thread_scan_result(uint16_t index);

#endif // CONFIG_NETWORK_PROV_NETWORK_TYPE_THREAD

/**
 * @brief   Get protocomm handlers for network_config provisioning endpoint
 *
 * @param[out] ptr   pointer to structure to be set
 *
 * @return
 *  - ESP_OK   : success
 *  - ESP_ERR_INVALID_ARG : null argument
 */
esp_err_t get_network_prov_handlers(network_prov_config_handlers_t *ptr);

/**
 * @brief   Get protocomm handlers for network_scan provisioning endpoint
 *
 * @param[out] ptr   pointer to structure to be set
 *
 * @return
 *  - ESP_OK   : success
 *  - ESP_ERR_INVALID_ARG : null argument
 */
esp_err_t get_network_scan_handlers(network_prov_scan_handlers_t *ptr);

/**
 * @brief   Get protocomm handlers for network_ctrl provisioning endpoint
 *
 * @param[in] ptr   pointer to structure to be set
 *
 * @return
 *  - ESP_OK   : success
 *  - ESP_ERR_INVALID_ARG : null argument
 */
esp_err_t get_network_ctrl_handlers(network_ctrl_handlers_t *ptr);
