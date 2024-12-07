/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/timers.h>
#include <esp_wifi.h>

#include <esp_rmaker_work_queue.h>
#include <esp_diagnostics_metrics.h>
#include "esp_diagnostics_internal.h"

#define LOG_TAG            "wifi_metrics"
#define METRICS_TAG        "wifi"
#define KEY_RSSI           "rssi"
#define KEY_MIN_RSSI       "min_rssi_ever"
#define KEY_STATUS         "conn_status"

#define PATH_WIFI_STATION  "Wi-Fi.Station"

#define DEFAULT_POLLING_INTERVAL 30   /* 30 seconds */
/* start reporting minimum ever rssi when rssi reaches -50 dbm */
#define WIFI_RSSI_THRESHOLD      -50

typedef struct {
    bool init;
    bool wifi_connected;
    bool status_sent;
    uint32_t period;
    TimerHandle_t handle;
    int32_t prev_rssi;
    int32_t min_rssi;
} wifi_diag_priv_data_t;

static wifi_diag_priv_data_t s_priv_data;

static void update_min_rssi(int32_t rssi)
{
    if (rssi < s_priv_data.min_rssi) {
        s_priv_data.min_rssi = rssi;
        esp_diag_metrics_add_int(KEY_MIN_RSSI, rssi);
        ESP_LOGI(LOG_TAG, "Wi-Fi RSSI crossed threshold %" PRIi32, rssi);
#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(4, 3, 0)
        esp_wifi_set_rssi_threshold(rssi);
#endif
    }
}

static void wifi_evt_handler(void *arg, esp_event_base_t evt_base, int32_t evt_id, void *evt_data)
{
    switch (evt_id) {
#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(4, 3, 0)
        case WIFI_EVENT_STA_BSS_RSSI_LOW:
        {
            wifi_event_bss_rssi_low_t *data = evt_data;
            update_min_rssi(data->rssi);
        }
        break;
#endif /* ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(4, 3, 0)  */
        case WIFI_EVENT_STA_CONNECTED:
        {
            s_priv_data.wifi_connected = true;
            s_priv_data.status_sent = false;
            if (esp_diag_metrics_add_bool(KEY_STATUS, 1) == ESP_OK) {
                s_priv_data.status_sent = true;
            }
            break;
        }
        case WIFI_EVENT_STA_DISCONNECTED:
        {
            if (s_priv_data.wifi_connected) {
                s_priv_data.wifi_connected = false;
                s_priv_data.status_sent = false;
                if (esp_diag_metrics_add_bool(KEY_STATUS, 0) == ESP_OK) {
                    s_priv_data.status_sent = true;
                }
            }
            break;
        }
        default:
        break;
    }
}

/* This function returns 1 on failure otherwise valid RSSI */
static int32_t get_rssi(void)
{
    wifi_ap_record_t ap_info;
    if (esp_wifi_sta_get_ap_info(&ap_info) == ESP_OK) {
        return ap_info.rssi;
    }
    return 1;
}

esp_err_t esp_diag_wifi_metrics_dump(void)
{
    if (!s_priv_data.init) {
        ESP_LOGW(LOG_TAG, "Wi-Fi metrics not initialized");
        return ESP_ERR_INVALID_STATE;
    }
    int32_t rssi = get_rssi();
    if (rssi != 1) {
        update_min_rssi(rssi);
        RET_ON_ERR_WITH_LOG(esp_diag_metrics_add_int(KEY_RSSI, rssi), ESP_LOG_WARN, LOG_TAG,
                            "Failed to add Wi-Fi metrics key:" KEY_RSSI);
        RET_ON_ERR_WITH_LOG(esp_diag_metrics_add_int(KEY_MIN_RSSI, s_priv_data.min_rssi), ESP_LOG_WARN, LOG_TAG,
                            "Failed to add Wi-Fi metrics key:" KEY_MIN_RSSI);
        s_priv_data.prev_rssi = rssi;
        ESP_LOGI(LOG_TAG, "%s:%" PRIi32 " %s:%" PRIi32, KEY_RSSI, rssi, KEY_MIN_RSSI, s_priv_data.min_rssi);
    }
    if (!s_priv_data.status_sent) {
        // if for some reason we were not able to add the status, try again
        if (esp_diag_metrics_add_bool(KEY_STATUS, s_priv_data.wifi_connected) == ESP_OK) {
            s_priv_data.status_sent = true;
        }
    }
    return ESP_OK;
}

static void wifi_metrics_dump_cb(void *arg)
{
    esp_diag_wifi_metrics_dump();
}

static void wifi_timer_cb(TimerHandle_t handle)
{
    esp_rmaker_work_queue_add_task(wifi_metrics_dump_cb, NULL);
}

esp_err_t esp_diag_wifi_metrics_init(void)
{
    if (s_priv_data.init) {
        return ESP_ERR_INVALID_STATE;
    }
    /* Register the event handler for wifi events */
    esp_err_t err = esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, wifi_evt_handler, NULL);
    if (err != ESP_OK) {
        return err;
    }
#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(4, 3, 0)
    err = esp_wifi_set_rssi_threshold(WIFI_RSSI_THRESHOLD);
    if (err != ESP_OK) {
        ESP_LOGW(LOG_TAG, "Failed to set rssi threshold value");
    }
#endif
    esp_diag_metrics_register(METRICS_TAG, KEY_RSSI, "Wi-Fi RSSI", PATH_WIFI_STATION, ESP_DIAG_DATA_TYPE_INT);
    esp_diag_metrics_register(METRICS_TAG, KEY_MIN_RSSI, "Minimum ever Wi-Fi RSSI", PATH_WIFI_STATION, ESP_DIAG_DATA_TYPE_INT);
    esp_diag_metrics_register(METRICS_TAG, KEY_STATUS, "Wi-Fi connect status", PATH_WIFI_STATION, ESP_DIAG_DATA_TYPE_BOOL);

    s_priv_data.min_rssi = WIFI_RSSI_THRESHOLD;
    s_priv_data.handle = xTimerCreate("wifi_metrics", SEC2TICKS(DEFAULT_POLLING_INTERVAL),
                                      pdTRUE, NULL, wifi_timer_cb);
    if (s_priv_data.handle) {
        xTimerStart(s_priv_data.handle, 0);
    }
    s_priv_data.init = true;
    /* Record RSSI at start */
    esp_diag_wifi_metrics_dump();
    return ESP_OK;
}

esp_err_t esp_diag_wifi_metrics_deinit(void)
{
    if (!s_priv_data.init) {
        return ESP_ERR_INVALID_STATE;
    }
    esp_event_handler_unregister(WIFI_EVENT, ESP_EVENT_ANY_ID, wifi_evt_handler);
    /* Try to delete timer with 10 ticks wait time */
    if (xTimerDelete(s_priv_data.handle, 10) == pdFALSE) {
        ESP_LOGW(LOG_TAG, "Failed to delete heap metric timer");
    }
    esp_diag_metrics_unregister(KEY_RSSI);
    esp_diag_metrics_unregister(KEY_MIN_RSSI);
    esp_diag_metrics_unregister(KEY_STATUS);
    memset(&s_priv_data, 0, sizeof(s_priv_data));
    return ESP_OK;
}

void esp_diag_wifi_metrics_reset_interval(uint32_t period)
{
    if (!s_priv_data.init) {
        return;
    }
    if (period == 0) {
        xTimerStop(s_priv_data.handle, 0);
        return;
    }
    xTimerChangePeriod(s_priv_data.handle, SEC2TICKS(period), 0);
}
