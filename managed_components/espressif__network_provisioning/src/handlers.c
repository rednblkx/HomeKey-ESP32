/*
 * SPDX-FileCopyrightText: 2019-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <string.h>
#include <esp_err.h>
#include <esp_log.h>

#ifdef CONFIG_NETWORK_PROV_NETWORK_TYPE_WIFI
#include <esp_wifi.h>
#endif
#ifdef CONFIG_NETWORK_PROV_NETWORK_TYPE_THREAD
#include <esp_openthread.h>
#endif
#include <esp_netif.h>

#include "network_provisioning/network_config.h"
#include "network_provisioning/network_scan.h"
#include "network_ctrl.h"
#include "network_provisioning/manager.h"
#include "network_provisioning_priv.h"

static const char *TAG = "network_prov_handlers";

/* Provide definition of network_prov_ctx_t */
struct network_prov_ctx {
#ifdef CONFIG_NETWORK_PROV_NETWORK_TYPE_WIFI
    wifi_config_t wifi_cfg;
#endif
#ifdef CONFIG_NETWORK_PROV_NETWORK_TYPE_THREAD
    otOperationalDatasetTlvs thread_dataset;
#endif
};

static void free_network_prov_ctx(network_prov_ctx_t **ctx)
{
    free(*ctx);
    *ctx = NULL;
}

#ifdef CONFIG_NETWORK_PROV_NETWORK_TYPE_WIFI
static wifi_config_t *get_wifi_config(network_prov_ctx_t **ctx)
{
    return (*ctx ? & (*ctx)->wifi_cfg : NULL);
}

static wifi_config_t *new_wifi_config(network_prov_ctx_t **ctx)
{
    free(*ctx);
    (*ctx) = (network_prov_ctx_t *) calloc(1, sizeof(network_prov_ctx_t));
    return get_wifi_config(ctx);
}

static esp_err_t wifi_get_status_handler(network_prov_config_get_wifi_data_t *resp_data, network_prov_ctx_t **ctx)
{
    /* Initialize to zero */
    memset(resp_data, 0, sizeof(network_prov_config_get_wifi_data_t));

    if (network_prov_mgr_get_wifi_state(&resp_data->wifi_state) != ESP_OK) {
        ESP_LOGW(TAG, "Network provisioning manager for Wi-Fi not running");
        return ESP_ERR_INVALID_STATE;
    }

    if (resp_data->wifi_state == NETWORK_PROV_WIFI_STA_CONNECTED) {
        ESP_LOGD(TAG, "Got state : connected");

        /* IP Addr assigned to STA */
        esp_netif_ip_info_t ip_info;
        esp_netif_get_ip_info(esp_netif_get_handle_from_ifkey("WIFI_STA_DEF"), &ip_info);
        esp_ip4addr_ntoa(&ip_info.ip, resp_data->conn_info.ip_addr, sizeof(resp_data->conn_info.ip_addr));


        /* AP information to which STA is connected */
        wifi_ap_record_t ap_info;
        esp_wifi_sta_get_ap_info(&ap_info);
        memcpy(resp_data->conn_info.bssid, (char *)ap_info.bssid, sizeof(ap_info.bssid));
        memcpy(resp_data->conn_info.ssid,  (char *)ap_info.ssid,  sizeof(ap_info.ssid));
        resp_data->conn_info.channel   = ap_info.primary;
        resp_data->conn_info.auth_mode = ap_info.authmode;

        /* Tell manager to stop provisioning service */
        network_prov_mgr_done();
    } else if (resp_data->wifi_state == NETWORK_PROV_WIFI_STA_DISCONNECTED) {
        ESP_LOGD(TAG, "Got state : disconnected");

        /* If disconnected, convey reason */
        network_prov_mgr_get_wifi_disconnect_reason(&resp_data->fail_reason);
    } else {
        ESP_LOGD(TAG, "Got state : connecting");
    }
    return ESP_OK;
}

static esp_err_t wifi_set_config_handler(const network_prov_config_set_wifi_data_t *req_data, network_prov_ctx_t **ctx)
{
    wifi_config_t *wifi_cfg = get_wifi_config(ctx);
    if (wifi_cfg) {
        free_network_prov_ctx(ctx);
    }

    wifi_cfg = new_wifi_config(ctx);
    if (!wifi_cfg) {
        ESP_LOGE(TAG, "Unable to allocate Wi-Fi config");
        return ESP_ERR_NO_MEM;
    }

    ESP_LOGD(TAG, "Wi-Fi Credentials Received");

    /* Using memcpy allows the max SSID length to be 32 bytes (as per 802.11 standard).
     * But this doesn't guarantee that the saved SSID will be null terminated, because
     * wifi_cfg->sta.ssid is also 32 bytes long (without extra 1 byte for null character).
     * Although, this is not a matter for concern because esp_wifi library reads the SSID
     * upto 32 bytes in absence of null termination */
    const size_t ssid_len = strnlen(req_data->ssid, sizeof(wifi_cfg->sta.ssid));
    /* Ensure SSID less than 32 bytes is null terminated */
    memset(wifi_cfg->sta.ssid, 0, sizeof(wifi_cfg->sta.ssid));
    memcpy(wifi_cfg->sta.ssid, req_data->ssid, ssid_len);

    /* Using strlcpy allows both max passphrase length (63 bytes) and ensures null termination
     * because size of wifi_cfg->sta.password is 64 bytes (1 extra byte for null character) */
    strlcpy((char *) wifi_cfg->sta.password, req_data->password, sizeof(wifi_cfg->sta.password));

#ifdef CONFIG_NETWORK_PROV_WIFI_STA_ALL_CHANNEL_SCAN
    wifi_cfg->sta.scan_method = WIFI_ALL_CHANNEL_SCAN;
#else /* CONFIG_NETWORK_PROV_WIFI_STA_FAST_SCAN */
    wifi_cfg->sta.scan_method = WIFI_FAST_SCAN;
#endif

    return ESP_OK;
}

static esp_err_t wifi_apply_config_handler(network_prov_ctx_t **ctx)
{
    wifi_config_t *wifi_cfg = get_wifi_config(ctx);
    if (!wifi_cfg) {
        ESP_LOGE(TAG, "Wi-Fi config not set");
        return ESP_ERR_INVALID_STATE;
    }

    esp_err_t ret = network_prov_mgr_configure_wifi_sta(wifi_cfg);
    if (ret == ESP_OK) {
        ESP_LOGD(TAG, "Wi-Fi Credentials Applied");
    } else {
        ESP_LOGE(TAG, "Failed to apply Wi-Fi Credentials");
    }

    free_network_prov_ctx(ctx);
    return ret;
}
#endif // CONFIG_NETWORK_PROV_NETWORK_TYPE_WIFI

#ifdef CONFIG_NETWORK_PROV_NETWORK_TYPE_THREAD
static otOperationalDatasetTlvs *get_thread_dataset(network_prov_ctx_t **ctx)
{
    return (*ctx ? & (*ctx)->thread_dataset : NULL);
}

static otOperationalDatasetTlvs *new_thread_dataset(network_prov_ctx_t **ctx)
{
    free(*ctx);
    (*ctx) = (network_prov_ctx_t *) calloc(1, sizeof(network_prov_ctx_t));
    return get_thread_dataset(ctx);
}

static esp_err_t thread_get_status_handler(network_prov_config_get_thread_data_t *resp_data, network_prov_ctx_t **ctx)
{
    /* Initialize to zero */
    memset(resp_data, 0, sizeof(network_prov_config_get_thread_data_t));

    if (network_prov_mgr_get_thread_state(&resp_data->thread_state) != ESP_OK) {
        ESP_LOGW(TAG, "Network provisioning manager not running");
        return ESP_ERR_INVALID_STATE;
    }

    if (resp_data->thread_state == NETWORK_PROV_THREAD_ATTACHED) {
        ESP_LOGD(TAG, "Got state : attached");
        otOperationalDataset dataset;
        if (otDatasetGetActive(esp_openthread_get_instance(), &dataset) != OT_ERROR_NONE) {
            ESP_LOGE(TAG, "Failed to get Thread dataset");
            return ESP_FAIL;
        }
        resp_data->conn_info.channel = dataset.mChannel;
        memcpy(resp_data->conn_info.ext_pan_id, dataset.mExtendedPanId.m8, sizeof(dataset.mExtendedPanId.m8));
        strncpy(resp_data->conn_info.name, dataset.mNetworkName.m8,
                strnlen(dataset.mNetworkName.m8, sizeof(dataset.mNetworkName.m8)));
        resp_data->conn_info.pan_id = dataset.mPanId;
        /* Tell manager to stop provisioning service */
        network_prov_mgr_done();
    } else if (resp_data->thread_state == NETWORK_PROV_THREAD_DETACHED) {
        ESP_LOGD(TAG, "Got state : disconnected");

        /* If disconnected, convey reason */
        network_prov_mgr_get_thread_detached_reason(&resp_data->fail_reason);
    } else {
        ESP_LOGD(TAG, "Got state : attaching");
    }
    return ESP_OK;
}

static esp_err_t thread_set_config_handler(const network_prov_config_set_thread_data_t *req_data, network_prov_ctx_t **ctx)
{
    otOperationalDatasetTlvs *thread_dataset = get_thread_dataset(ctx);
    if (thread_dataset) {
        free_network_prov_ctx(ctx);
    }

    thread_dataset = new_thread_dataset(ctx);
    if (!thread_dataset) {
        ESP_LOGE(TAG, "Unable to allocate Thread dataset");
        return ESP_ERR_NO_MEM;
    }

    ESP_LOGD(TAG, "Thread Dataset Received");

    thread_dataset->mLength = req_data->length;
    memcpy(thread_dataset->mTlvs, req_data->dataset, thread_dataset->mLength);

    return ESP_OK;
}

static esp_err_t thread_apply_config_handler(network_prov_ctx_t **ctx)
{
    otOperationalDatasetTlvs *thread_dataset = get_thread_dataset(ctx);
    if (!thread_dataset) {
        ESP_LOGE(TAG, "Thread Dataset not set");
        return ESP_ERR_INVALID_STATE;
    }

    esp_err_t ret = network_prov_mgr_configure_thread_dataset(thread_dataset);
    if (ret == ESP_OK) {
        ESP_LOGD(TAG, "Thread Dataset Applied");
    } else {
        ESP_LOGE(TAG, "Failed to apply Thread Dataset");
    }

    free_network_prov_ctx(ctx);
    return ret;
}
#endif // CONFIG_NETWORK_PROV_NETWORK_TYPE_THREAD


esp_err_t get_network_prov_handlers(network_prov_config_handlers_t *ptr)
{
    if (!ptr) {
        return ESP_ERR_INVALID_ARG;
    }
#ifdef CONFIG_NETWORK_PROV_NETWORK_TYPE_WIFI
    ptr->wifi_get_status_handler   = wifi_get_status_handler;
    ptr->wifi_set_config_handler   = wifi_set_config_handler;
    ptr->wifi_apply_config_handler = wifi_apply_config_handler;
#endif // CONFIG_NETWORK_PROV_NETWORK_TYPE_WIFI
#ifdef CONFIG_NETWORK_PROV_NETWORK_TYPE_THREAD
    ptr->thread_get_status_handler = thread_get_status_handler;
    ptr->thread_set_config_handler = thread_set_config_handler;
    ptr->thread_apply_config_handler = thread_apply_config_handler;
#endif // CONFIG_NETWORK_PROV_NETWORK_TYPE_THREAD
    ptr->ctx = NULL;
    return ESP_OK;
}

/*************************************************************************/
#ifdef CONFIG_NETWORK_PROV_NETWORK_TYPE_WIFI
static esp_err_t wifi_scan_start(bool blocking, bool passive,
                                 uint8_t group_channels, uint32_t period_ms,
                                 network_prov_scan_ctx_t **ctx)
{
    return network_prov_mgr_wifi_scan_start(blocking, passive, group_channels, period_ms);
}

static esp_err_t wifi_scan_status(bool *scan_finished,
                                  uint16_t *result_count,
                                  network_prov_scan_ctx_t **ctx)
{
    *scan_finished = network_prov_mgr_wifi_scan_finished();
    *result_count  = network_prov_mgr_wifi_scan_result_count();
    return ESP_OK;
}

static esp_err_t wifi_scan_result(uint16_t result_index,
                                  network_prov_scan_wifi_result_t *result,
                                  network_prov_scan_ctx_t **ctx)
{
    const wifi_ap_record_t *record = network_prov_mgr_wifi_scan_result(result_index);
    if (!record) {
        return ESP_FAIL;
    }

    /* Compile time check ensures memory safety in case SSID length in
     * record / result structure definition changes in future */
    _Static_assert(sizeof(result->ssid) == sizeof(record->ssid),
                   "source and destination should be of same size");
    memcpy(result->ssid, record->ssid, sizeof(record->ssid));
    memcpy(result->bssid, record->bssid, sizeof(record->bssid));
    result->channel = record->primary;
    result->rssi = record->rssi;
    result->auth = record->authmode;
    return ESP_OK;
}
#endif // CONFIG_NETWORK_PROV_NETWORK_TYPE_WIFI

#ifdef CONFIG_NETWORK_PROV_NETWORK_TYPE_THREAD
static esp_err_t thread_scan_start(bool blocking, uint32_t channel_mask, network_prov_scan_ctx_t **ctx)
{
    return network_prov_mgr_thread_scan_start(blocking, channel_mask);
}

static esp_err_t thread_scan_status(bool *scan_finished,
                                    uint16_t *result_count,
                                    network_prov_scan_ctx_t **ctx)
{
    *scan_finished = network_prov_mgr_thread_scan_finished();
    *result_count  = network_prov_mgr_thread_scan_result_count();
    return ESP_OK;
}

static esp_err_t thread_scan_result(uint16_t result_index,
                                    network_prov_scan_thread_result_t *result,
                                    network_prov_scan_ctx_t **ctx)
{
    const otActiveScanResult *record = network_prov_mgr_thread_scan_result(result_index);
    if (!record) {
        return ESP_FAIL;
    }

    result->channel = record->mChannel;
    result->pan_id = record->mPanId;
    result->rssi = record->mRssi;
    result->lqi = record->mLqi;
    memcpy(result->ext_addr, record->mExtAddress.m8, sizeof(record->mExtAddress.m8));

    if (record->mDiscover) {
        memcpy(result->ext_pan_id, record->mExtendedPanId.m8, sizeof(result->ext_pan_id));
        memcpy(result->network_name, record->mNetworkName.m8, sizeof(record->mNetworkName.m8));
    } else {
        memset(result->ext_pan_id, 0, sizeof(result->ext_pan_id));
        memset(result->network_name, 0, sizeof(result->network_name));
    }

    return ESP_OK;
}

#endif // CONFIG_NETWORK_PROV_NETWORK_TYPE_THREAD

esp_err_t get_network_scan_handlers(network_prov_scan_handlers_t *ptr)
{
    if (!ptr) {
        return ESP_ERR_INVALID_ARG;
    }
#ifdef CONFIG_NETWORK_PROV_NETWORK_TYPE_WIFI
    ptr->wifi_scan_start  = wifi_scan_start;
    ptr->wifi_scan_status = wifi_scan_status;
    ptr->wifi_scan_result = wifi_scan_result;
#endif // CONFIG_NETWORK_PROV_NETWORK_TYPE_WIFI
#ifdef CONFIG_NETWORK_PROV_NETWORK_TYPE_THREAD
    ptr->thread_scan_start = thread_scan_start;
    ptr->thread_scan_status = thread_scan_status;
    ptr->thread_scan_result = thread_scan_result;
#endif // CONFIG_NETWORK_PROV_NETWORK_TYPE_THREAD
    ptr->ctx = NULL;
    return ESP_OK;
}

/*************************************************************************/

#ifdef CONFIG_NETWORK_PROV_NETWORK_TYPE_WIFI
static esp_err_t wifi_ctrl_reset(void)
{
    return network_prov_mgr_reset_wifi_sm_state_on_failure();
}

static esp_err_t wifi_ctrl_reprov(void)
{
    return network_prov_mgr_reset_wifi_sm_state_for_reprovision();
}
#endif // CONFIG_NETWORK_PROV_NETWORK_TYPE_WIFI

#ifdef CONFIG_NETWORK_PROV_NETWORK_TYPE_THREAD
static esp_err_t thread_ctrl_reset(void)
{
    return network_prov_mgr_reset_thread_sm_state_on_failure();
}

static esp_err_t thread_ctrl_reprov(void)
{
    return network_prov_mgr_reset_thread_sm_state_for_reprovision();
}
#endif // CONFIG_NETWORK_PROV_NETWORK_TYPE_THREAD

esp_err_t get_network_ctrl_handlers(network_ctrl_handlers_t *ptr)
{
    if (!ptr) {
        return ESP_ERR_INVALID_ARG;
    }
#ifdef CONFIG_NETWORK_PROV_NETWORK_TYPE_WIFI
    ptr->wifi_ctrl_reset  = wifi_ctrl_reset;
    ptr->wifi_ctrl_reprov  = wifi_ctrl_reprov;
#endif
#ifdef CONFIG_NETWORK_PROV_NETWORK_TYPE_THREAD
    ptr->thread_ctrl_reset = thread_ctrl_reset;
    ptr->thread_ctrl_reprov = thread_ctrl_reprov;
#endif
    return ESP_OK;
}
