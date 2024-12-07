/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include <esp_heap_caps.h>
#include <freertos/FreeRTOS.h>
#include <freertos/timers.h>

#include <esp_rmaker_work_queue.h>
#include <esp_diagnostics.h>
#include <esp_diagnostics_metrics.h>
#include "esp_diagnostics_internal.h"

#define LOG_TAG            "heap_metrics"
#define METRICS_TAG        "heap"

#define KEY_ALLOC_FAIL     "alloc_fail"
#define KEY_FREE           "free"
#define KEY_MIN_FREE       "min_free_ever"
#define KEY_LFB            "lfb"
#ifdef CONFIG_ESP32_SPIRAM_SUPPORT
#define KEY_EXT_FREE       "ext_free"
#define KEY_EXT_LFB        "ext_lfb"
#define KEY_EXT_MIN_FREE   "ext_min_free_ever"
#endif /* CONFIG_ESP32_SPIRAM_SUPPORT */

#define PATH_HEAP_INTERNAL "heap.internal"
#define PATH_HEAP_EXTERNAL "heap.external"

#define DEFAULT_POLLING_INTERVAL 30 /* 30 seconds */

typedef struct {
    bool init;
    TimerHandle_t handle;
} heap_diag_priv_data_t;

static heap_diag_priv_data_t s_priv_data;

esp_err_t esp_diag_heap_metrics_dump(void)
{
    if (!s_priv_data.init) {
        ESP_LOGW(LOG_TAG, "Heap metrics not initialized");
        return ESP_ERR_INVALID_STATE;
    }
    uint32_t free = heap_caps_get_free_size(MALLOC_CAP_INTERNAL);
    uint32_t lfb = heap_caps_get_largest_free_block(MALLOC_CAP_INTERNAL);
    uint32_t min_free_ever = heap_caps_get_minimum_free_size(MALLOC_CAP_INTERNAL);

    RET_ON_ERR_WITH_LOG(esp_diag_metrics_add_uint(KEY_FREE, free), ESP_LOG_WARN, LOG_TAG,
                        "Failed to add heap metric key:" KEY_FREE);
    RET_ON_ERR_WITH_LOG(esp_diag_metrics_add_uint(KEY_LFB, lfb), ESP_LOG_WARN, LOG_TAG,
                        "Failed to add heap metric key:" KEY_LFB);
    RET_ON_ERR_WITH_LOG(esp_diag_metrics_add_uint(KEY_MIN_FREE, min_free_ever), ESP_LOG_WARN, LOG_TAG,
                        "Failed to add heap metric key:" KEY_MIN_FREE);

    ESP_LOGI(LOG_TAG, KEY_FREE ":0x%" PRIx32 " " KEY_LFB ":0x%" PRIx32 " " KEY_MIN_FREE ":0x%" PRIx32, free, lfb, min_free_ever);
#ifdef CONFIG_ESP32_SPIRAM_SUPPORT
    free = heap_caps_get_free_size(MALLOC_CAP_SPIRAM);
    lfb = heap_caps_get_largest_free_block(MALLOC_CAP_SPIRAM);
    min_free_ever = heap_caps_get_minimum_free_size(MALLOC_CAP_SPIRAM);

    RET_ON_ERR_WITH_LOG(esp_diag_metrics_add_uint(KEY_EXT_FREE, free), ESP_LOG_WARN, LOG_TAG,
                        "Failed to add heap metric key:" KEY_EXT_FREE);
    RET_ON_ERR_WITH_LOG(esp_diag_metrics_add_uint(KEY_EXT_LFB, lfb), ESP_LOG_WARN, LOG_TAG,
                        "Failed to add heap metric key:" KEY_EXT_LFB);
    RET_ON_ERR_WITH_LOG(esp_diag_metrics_add_uint(KEY_EXT_MIN_FREE, min_free_ever), ESP_LOG_WARN, LOG_TAG,
                        "Failed to add heap metric key:" KEY_EXT_MIN_FREE);

    ESP_LOGI(LOG_TAG, KEY_EXT_FREE ":0x%" PRIx32 " " KEY_EXT_LFB ":0x%" PRIx32 " " KEY_EXT_MIN_FREE ":0x%" PRIx32, free, lfb, min_free_ever);
#endif /* CONFIG_ESP32_SPIRAM_SUPPORT */
    return ESP_OK;
}

static void heap_metrics_dump_cb(void *arg)
{
    esp_diag_heap_metrics_dump();
}

static void heap_timer_cb(TimerHandle_t handle)
{
    esp_rmaker_work_queue_add_task(heap_metrics_dump_cb, NULL);
}

#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(4, 2, 0)
static void alloc_failed_hook(size_t size, uint32_t caps, const char *func)
{
    esp_diag_heap_metrics_dump();
    esp_diag_metrics_add_uint(KEY_ALLOC_FAIL, size);
    ESP_DIAG_EVENT(METRICS_TAG, KEY_ALLOC_FAIL " size:0x%x func:%s", size, func);
}
#endif

esp_err_t esp_diag_heap_metrics_init(void)
{
    if (s_priv_data.init) {
        return ESP_ERR_INVALID_STATE;
    }
#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(4, 2, 0)
    esp_err_t err = heap_caps_register_failed_alloc_callback(alloc_failed_hook);
    if (err != ESP_OK) {
        return err;
    }
    esp_diag_metrics_register(METRICS_TAG, KEY_ALLOC_FAIL, "Malloc fail", METRICS_TAG, ESP_DIAG_DATA_TYPE_UINT);
#endif

#ifdef CONFIG_ESP32_SPIRAM_SUPPORT
    esp_diag_metrics_register(METRICS_TAG, KEY_EXT_FREE, "External free heap", PATH_HEAP_EXTERNAL, ESP_DIAG_DATA_TYPE_UINT);
    esp_diag_metrics_register(METRICS_TAG, KEY_EXT_LFB, "External largest free block", PATH_HEAP_EXTERNAL, ESP_DIAG_DATA_TYPE_UINT);
    esp_diag_metrics_register(METRICS_TAG, KEY_EXT_MIN_FREE, "External minimum free size", PATH_HEAP_EXTERNAL, ESP_DIAG_DATA_TYPE_UINT);

#endif /* CONFIG_ESP32_SPIRAM_SUPPORT */

    esp_diag_metrics_register(METRICS_TAG, KEY_FREE, "Free heap", PATH_HEAP_INTERNAL, ESP_DIAG_DATA_TYPE_UINT);
    esp_diag_metrics_register(METRICS_TAG, KEY_LFB, "Largest free block", PATH_HEAP_INTERNAL, ESP_DIAG_DATA_TYPE_UINT);
    esp_diag_metrics_register(METRICS_TAG, KEY_MIN_FREE, "Minimum free size", PATH_HEAP_INTERNAL, ESP_DIAG_DATA_TYPE_UINT);

    s_priv_data.handle = xTimerCreate("heap_metrics", SEC2TICKS(DEFAULT_POLLING_INTERVAL),
                                      pdTRUE, NULL, heap_timer_cb);
    if (s_priv_data.handle) {
        xTimerStart(s_priv_data.handle, 0);
    }
    s_priv_data.init = true;

    // Dump metrics for the first time
    esp_diag_heap_metrics_dump();

    return ESP_OK;
}

esp_err_t esp_diag_heap_metrics_deinit(void)
{
    if (!s_priv_data.init) {
        return ESP_ERR_INVALID_STATE;
    }
    /* Try to delete timer with 10 ticks wait time */
    if (xTimerDelete(s_priv_data.handle, 10) == pdFALSE) {
        ESP_LOGW(LOG_TAG, "Failed to delete heap metric timer");
    }
#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(4, 2, 0)
    esp_diag_metrics_unregister(KEY_ALLOC_FAIL);
#endif
#ifdef CONFIG_ESP32_SPIRAM_SUPPORT
    esp_diag_metrics_unregister(KEY_EXT_FREE);
    esp_diag_metrics_unregister(KEY_EXT_LFB);
    esp_diag_metrics_unregister(KEY_EXT_MIN_FREE);
#endif
    esp_diag_metrics_unregister(KEY_FREE);
    esp_diag_metrics_unregister(KEY_LFB);
    esp_diag_metrics_unregister(KEY_MIN_FREE);
    memset(&s_priv_data, 0, sizeof(s_priv_data));
    return ESP_OK;
}

void esp_diag_heap_metrics_reset_interval(uint32_t period)
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
