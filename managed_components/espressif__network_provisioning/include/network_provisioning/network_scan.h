/*
 * SPDX-FileCopyrightText: 2019-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _PROV_NETWORK_SCAN_H_
#define _PROV_NETWORK_SCAN_H_

#ifdef __cplusplus
extern "C" {
#endif
#include <sdkconfig.h>
#ifdef CONFIG_NETWORK_PROV_NETWORK_TYPE_WIFI
#include <esp_wifi.h>

#define WIFI_SSID_LEN  sizeof(((wifi_ap_record_t *)0)->ssid)
#define WIFI_BSSID_LEN sizeof(((wifi_ap_record_t *)0)->bssid)
#endif // CONFIG_NETWORK_PROV_NETWORK_TYPE_WIFI

#ifdef CONFIG_NETWORK_PROV_NETWORK_TYPE_THREAD
#include <openthread/dataset.h>
#include <openthread/platform/radio.h>
#endif // CONFIG_NETWORK_PROV_NETWORK_TYPE_THREAD

/**
 * @brief   Type of context data passed to each get/set/apply handler
 *           function set in `network_prov_scan_handlers` structure.
 *
 * This is passed as an opaque pointer, thereby allowing it be defined
 * later in application code as per requirements.
 */
typedef struct network_prov_scan_ctx network_prov_scan_ctx_t;

#ifdef CONFIG_NETWORK_PROV_NETWORK_TYPE_WIFI
/**
 * @brief   Structure of entries in the scan results list
 */
typedef struct {
    /**
     * SSID of Wi-Fi AP
     */
    char ssid[WIFI_SSID_LEN];

    /**
     * BSSID of Wi-Fi AP
     */
    char bssid[WIFI_BSSID_LEN];

    /**
     * Wi-Fi channel number
     */
    uint8_t channel;

    /**
     * Signal strength
     */
    int rssi;

    /**
     * Wi-Fi security mode
     */
    uint8_t auth;
} network_prov_scan_wifi_result_t;
#endif // CONFIG_NETWORK_PROV_NETWORK_TYPE_WIFI

#ifdef CONFIG_NETWORK_PROV_NETWORK_TYPE_THREAD
typedef struct {
    uint16_t pan_id;
    uint8_t ext_pan_id[OT_EXT_ADDRESS_SIZE];
    char network_name[OT_NETWORK_NAME_MAX_SIZE + 1];
    uint16_t channel;
    uint8_t ext_addr[OT_EXT_ADDRESS_SIZE];
    int8_t rssi;
    uint8_t lqi;
} network_prov_scan_thread_result_t;
#endif // CONFIG_NETWORK_PROV_NETWORK_TYPE_THREAD

/**
 * @brief   Internal handlers for receiving and responding to protocomm
 *          requests from client
 *
 * This is to be passed as priv_data for protocomm request handler
 * (refer to `network_prov_scan_handler()`) when calling `protocomm_add_endpoint()`.
 */
typedef struct network_prov_scan_handlers {
#ifdef CONFIG_NETWORK_PROV_NETWORK_TYPE_WIFI
    /**
     * Handler function called when scan start command is received
     * with various scan parameters :
     *
     * blocking (input) - If true, the function should return only
     * when the scanning is finished
     *
     * passive (input) - If true, scan is to be started in passive
     * mode (this may be slower) instead of active mode
     *
     * group_channels (input) - This specifies whether to scan
     * all channels in one go (when zero) or perform scanning of
     * channels in groups, with 120ms delay between scanning of
     * consecutive groups, and the value of this parameter sets the
     * number of channels in each group. This is useful when transport
     * mode is SoftAP, where scanning all channels in one go may not
     * give the Wi-Fi driver enough time to send out beacons, and
     * hence may cause disconnection with any connected stations.
     * When scanning in groups, the manager will wait for atleast
     * 120ms after completing scan on a group of channels, and thus
     * allow the driver to send out the beacons. For example, given
     * that the total number of Wi-Fi channels is 14, then setting
     * group_channels to 4, will create 5 groups, with each group
     * having 3 channels, except the last one which will have
     * 14 % 3 = 2 channels. So, when scan is started, the first 3
     * channels will be scanned, followed by a 120ms delay, and then
     * the next 3 channels, and so on, until all the 14 channels have
     * been scanned. One may need to adjust this parameter as having
     * only few channels in a group may slow down the overall scan
     * time, while having too many may again cause disconnection.
     * Usually a value of 4 should work for most cases. Note that
     * for any other mode of transport, e.g. BLE, this can be safely
     * set to 0, and hence achieve the fastest overall scanning time.
     *
     * period_ms (input) - Scan parameter specifying how long to
     * wait on each channel (in milli-seconds)
     */
    esp_err_t (*wifi_scan_start)(bool blocking, bool passive,
                                 uint8_t group_channels, uint32_t period_ms,
                                 network_prov_scan_ctx_t **ctx);

    /**
     * Handler function called when scan status is requested. Status
     * is given the parameters :
     *
     * scan_finished (output) - When scan has finished this returns true
     *
     * result_count (output) - This gives the total number of results
     * obtained till now. If scan is yet happening this number will
     * keep on updating
     */
    esp_err_t (*wifi_scan_status)(bool *scan_finished,
                                  uint16_t *result_count,
                                  network_prov_scan_ctx_t **ctx);

    /**
     * Handler function called when scan result is requested. Parameters :
     *
     * scan_result - For fetching scan results. This can be called even
     * if scan is still on going
     *
     * start_index (input) - Starting index from where to fetch the
     * entries from the results list
     *
     * count (input) - Number of entries to fetch from the starting index
     *
     * entries (output) - List of entries returned. Each entry consists
     * of ssid, channel and rssi information
     */
    esp_err_t (*wifi_scan_result)(uint16_t result_index,
                                  network_prov_scan_wifi_result_t *result,
                                  network_prov_scan_ctx_t **ctx);
#endif // CONFIG_NETWORK_PROV_NETWORK_TYPE_WIFI

#ifdef CONFIG_NETWORK_PROV_NETWORK_TYPE_THREAD
    esp_err_t (*thread_scan_start)(bool blocking, uint32_t channel_mask, network_prov_scan_ctx_t **ctx);

    esp_err_t (*thread_scan_status)(bool *scan_finished,
                                    uint16_t *result_count,
                                    network_prov_scan_ctx_t **ctx);

    esp_err_t (*thread_scan_result)(uint16_t result_index,
                                    network_prov_scan_thread_result_t *result,
                                    network_prov_scan_ctx_t **ctx);
#endif // CONFIG_NETWORK_PROV_NETWORK_TYPE_THREAD

    /**
     * Context pointer to be passed to above handler functions upon invocation
     */
    network_prov_scan_ctx_t *ctx;
} network_prov_scan_handlers_t;

/**
 * @brief   Handler for sending on demand Wi-Fi scan results
 *
 * This is to be registered as the `prov-scan` endpoint handler
 * (protocomm `protocomm_req_handler_t`) using `protocomm_add_endpoint()`
 */
esp_err_t network_prov_scan_handler(uint32_t session_id, const uint8_t *inbuf, ssize_t inlen,
                                    uint8_t **outbuf, ssize_t *outlen, void *priv_data);

#ifdef __cplusplus
}
#endif

#endif
