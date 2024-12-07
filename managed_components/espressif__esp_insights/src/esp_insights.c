// Copyright 2021 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
#include <string.h>
#include <esp_log.h>
#include <esp_wifi.h>
#include <esp_core_dump.h>
#include <freertos/FreeRTOS.h>
#include <freertos/timers.h>
#include <nvs.h>
#include <esp_crc.h>
#include <freertos/semphr.h>
#include <esp_diag_data_store.h>
#include <esp_diagnostics.h>
#include <esp_diagnostics_metrics.h>
#include <esp_diagnostics_variables.h>
#include <esp_diagnostics_system_metrics.h>
#include <esp_diagnostics_network_variables.h>
#include <esp_rmaker_utils.h>
#include <esp_rmaker_factory.h>
#include <esp_rmaker_work_queue.h>
#include <esp_insights.h>
#include <esp_insights_internal.h>

#include "esp_insights_client_data.h"
#include "esp_insights_encoder.h"
#include "esp_insights_cbor_decoder.h"

#include <esp_idf_version.h>
#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(5, 0, 0)
#include <esp_mac.h>
#endif

#define INSIGHTS_DEBUG_ENABLED      CONFIG_ESP_INSIGHTS_DEBUG_ENABLED
#define APP_ELF_SHA256_LEN          (CONFIG_APP_RETRIEVE_LEN_ELF_SHA + 1)

#if CONFIG_ESP_INSIGHTS_CLOUD_POST_MIN_INTERVAL_SEC > CONFIG_ESP_INSIGHTS_CLOUD_POST_MAX_INTERVAL_SEC
#error "CONFIG_ESP_INSIGHTS_CLOUD_POST_MIN_INTERVAL_SEC must be less than or equal to CONFIG_ESP_INSIGHTS_CLOUD_POST_MAX_INTERVAL_SEC"
#endif

#define CLOUD_REPORTING_PERIOD_MIN_SEC    CONFIG_ESP_INSIGHTS_CLOUD_POST_MIN_INTERVAL_SEC
#define CLOUD_REPORTING_PERIOD_MAX_SEC    CONFIG_ESP_INSIGHTS_CLOUD_POST_MAX_INTERVAL_SEC
#define CLOUD_REPORTING_TIMEOUT_TICKS     ((30 * 1000) / portTICK_PERIOD_MS)

#ifdef CONFIG_DIAG_DATA_STORE_RTC
#if CONFIG_RTC_STORE_DATA_SIZE > (1024 * 4)
#define INSIGHTS_DATA_MAX_SIZE  (CONFIG_RTC_STORE_DATA_SIZE - 1024)
#else
#define INSIGHTS_DATA_MAX_SIZE  CONFIG_RTC_STORE_DATA_SIZE
#endif
#else
/* TODO: Hardcoding it to 6K but needs to think about clearer way to define this */
#define INSIGHTS_DATA_MAX_SIZE (1024 * 6)
#endif

#define INSIGHTS_READ_BUF_SIZE  (1024)  // read this much data from data store in one go

#define SEND_INSIGHTS_META (CONFIG_DIAG_ENABLE_METRICS || CONFIG_DIAG_ENABLE_VARIABLES)
#define KEY_LOG_WR_FAIL    "log_wr_fail"

#define DIAG_DATA_STORE_CRC_KEY "rtc_buf_sha"
#define INSIGHTS_NVS_NAMESPACE "storage"

ESP_EVENT_DEFINE_BASE(INSIGHTS_EVENT);

typedef struct esp_insights_entry {
    esp_rmaker_work_fn_t work_fn;
    TimerHandle_t timer;
    uint32_t min_seconds;
    uint32_t max_seconds;
    uint32_t cur_seconds;
    void *priv_data;
} esp_insights_entry_t;

typedef struct {
    uint8_t *scratch_buf;
    uint8_t *read_buf;      // buffer to hold data read from RTC buf
    int data_msg_id;
    uint32_t data_msg_len;
    SemaphoreHandle_t data_lock;
    char app_sha256[APP_ELF_SHA256_LEN];
    bool data_sent;
#if SEND_INSIGHTS_META
    bool meta_msg_pending;
    uint32_t meta_msg_id;
    uint32_t meta_crc;
#endif /* SEND_INSIGHTS_META */
    bool data_send_inprogress;
    uint32_t log_write_fail_cnt; /* Count of failed log write */
    TimerHandle_t data_send_timer; /* timer to reset data_send_inprogress flag on timeout */
    char *node_id;
    int boot_msg_id;   /* To track whether first message is sent or not, -1:failed, 0:success, >0:inprogress */
    bool init_done;     /* insights init done */
    bool enabled;       /* insights enable is done */
} esp_insights_data_t;

#ifdef CONFIG_ESP_INSIGHTS_ENABLED

static const char *TAG = "esp_insights";
static esp_insights_data_t s_insights_data;
static esp_insights_entry_t *s_periodic_insights_entry;

static void esp_insights_first_call(void *priv_data)
{
    if (!priv_data) {
        return;
    }
    esp_insights_entry_t *entry = (esp_insights_entry_t *)priv_data;
    esp_rmaker_work_queue_add_task(entry->work_fn, entry->priv_data);
    /* Start timer here so that the function is called periodically */
    ESP_LOGI(TAG, "Scheduling Insights timer for %" PRIu32 " seconds.", entry->cur_seconds);
    xTimerStart(entry->timer, 0);
}

/* Returns true if wifi is connected and insights is enabled, false otherwise */
static bool is_insights_active(void)
{
    wifi_ap_record_t ap_info;
    bool wifi_connected = esp_wifi_sta_get_ap_info(&ap_info) == ESP_OK;
    return wifi_connected && s_insights_data.enabled;
}

/* This executes in the context of timer task.
 *
 * There is a dynamic logic to decide the next instance when the insights
 * data will be reported. Depending on whether data was sent or not during
 * the previous timeout, we double or halve the time period.
 * This ensures that data generally gets reported quick enough,
 * but if there's very frequent data being generated, it wont result
 * into too frquent publishes.
 * The period will keep changing between CLOUD_REPORTING_PERIOD_MIN_SEC and
 * CLOUD_REPORTING_PERIOD_MAX_SEC
 */
static void esp_insights_common_cb(TimerHandle_t handle)
{
    esp_insights_entry_t *entry = (esp_insights_entry_t *)pvTimerGetTimerID(handle);
    /* Check if any data was sent during the previous time out */
    xSemaphoreTake(s_insights_data.data_lock, portMAX_DELAY);
    bool l_data_sent = s_insights_data.data_sent;
    s_insights_data.data_sent = false;
    xSemaphoreGive(s_insights_data.data_lock);

    if (entry) {
        if (is_insights_active() == true) {
            esp_rmaker_work_queue_add_task(entry->work_fn, entry->priv_data);
        }
        /* If data was sent during previous timer interval, double the period */
        if (l_data_sent) {
            entry->cur_seconds <<= 1; /* Double the period */
            if (entry->cur_seconds > entry->max_seconds) {
                entry->cur_seconds = entry->max_seconds;
            }
        } else {
            /* If no data was sent during previous timer interval, halve the period */
            entry->cur_seconds >>= 1; /* Halve the period */
            if (entry->cur_seconds < entry->min_seconds) {
                entry->cur_seconds = entry->min_seconds;
            }
        }
        xTimerChangePeriod(handle, (entry->cur_seconds * 1000)/ portTICK_PERIOD_MS, 100);
        xTimerStart(handle, 0);
    }
}

static esp_err_t esp_insights_unregister_periodic_handler(void)
{
    if (s_periodic_insights_entry) {
        if (s_periodic_insights_entry->timer) {
            ESP_LOGI(TAG, "Deleting the periodic timer");
            if (xTimerDelete(s_periodic_insights_entry->timer, 10) != pdPASS) {
                ESP_LOGE(TAG, "Failed to delete the periodic timer");
            }
        }

        if (s_periodic_insights_entry) {
            free(s_periodic_insights_entry);
            s_periodic_insights_entry = NULL;
        }
    }

    return ESP_OK;
}

static esp_err_t esp_insights_register_periodic_handler(esp_rmaker_work_fn_t work_fn,
                                                        uint32_t min_seconds, uint32_t max_seconds,
                                                        void *priv_data)
{
    if (s_periodic_insights_entry) {
        ESP_LOGI(TAG, "s_periodic_insights_entry already registered");
        return ESP_OK;
    }

    if (!work_fn || (min_seconds == 0) || (max_seconds == 0)) {
        ESP_LOGI(TAG, "invalid params register_periodic_handler");
        return ESP_ERR_INVALID_ARG;
    }

    s_periodic_insights_entry = calloc (1, sizeof(esp_insights_entry_t));
    if (!s_periodic_insights_entry) {
        ESP_LOGI(TAG, "allocation failed, line %d", __LINE__);
        return ESP_ERR_NO_MEM;
    }
    s_periodic_insights_entry->work_fn = work_fn;
    s_periodic_insights_entry->priv_data = priv_data;
    s_periodic_insights_entry->min_seconds = min_seconds;
    s_periodic_insights_entry->max_seconds = max_seconds;
    s_periodic_insights_entry->cur_seconds = min_seconds;
    s_periodic_insights_entry->timer = xTimerCreate("test", (s_periodic_insights_entry->cur_seconds * 1000)/ portTICK_PERIOD_MS,
                                                    pdFALSE, (void *)s_periodic_insights_entry, esp_insights_common_cb);
    if (!s_periodic_insights_entry->timer) {
        ESP_LOGI(TAG, "timer creation failed, line %d", __LINE__);
        free(s_periodic_insights_entry);
        return ESP_FAIL;
    }
    /* Rainmaker work queue execution start after MQTT connection is established,
     * esp_insights_first_call() will be executed after MQTT connection is established.
     * It add the work_fn to the queue and start the periodic timer.
     */
    esp_err_t ret = esp_rmaker_work_queue_add_task(esp_insights_first_call, s_periodic_insights_entry);
    if (ret != ESP_OK) {
        ESP_LOGI(TAG, "failed to enqueue insights_first_call, line %d", __LINE__);
    }
    return ret;
}

static void data_send_timeout_cb(TimerHandle_t handle)
{
    xSemaphoreTake(s_insights_data.data_lock, portMAX_DELAY);
    s_insights_data.data_send_inprogress = false;
    if (s_insights_data.boot_msg_id > 0) {
        s_insights_data.boot_msg_id = -1;
    }
    xSemaphoreGive(s_insights_data.data_lock);
}

/* This executes in the context of default event loop task */
static void insights_event_handler(void* arg, esp_event_base_t event_base,
                                   int32_t event_id, void* event_data)
{
    esp_insights_transport_event_data_t *data = event_data;
    if (event_base != INSIGHTS_EVENT) {
        return;
    }
    switch(event_id) {
        case INSIGHTS_EVENT_TRANSPORT_SEND_SUCCESS:
#if INSIGHTS_DEBUG_ENABLED
            ESP_LOGI(TAG, "Data send success, msg_id:%d.", data ? data->msg_id : 0);
#endif
            if (data && data->msg_id) {
                xSemaphoreTake(s_insights_data.data_lock, portMAX_DELAY);
                if (xTimerIsTimerActive(s_insights_data.data_send_timer) == pdTRUE) {
                    xTimerStop(s_insights_data.data_send_timer, portMAX_DELAY);
                }
                if (data->msg_id == s_insights_data.data_msg_id) {
                    esp_diag_data_store_critical_release(s_insights_data.data_msg_len);
                    s_insights_data.data_sent = true;
                    s_insights_data.data_send_inprogress = false;
#if SEND_INSIGHTS_META
                } else if (s_insights_data.meta_msg_pending && data->msg_id == s_insights_data.meta_msg_id) {
                    esp_insights_meta_nvs_crc_set(s_insights_data.meta_crc);
                    s_insights_data.meta_msg_pending = false;
                    s_insights_data.data_sent = true;
#endif /* SEND_INSIGHTS_META */
                } else if (s_insights_data.boot_msg_id > 0 && s_insights_data.boot_msg_id == data->msg_id) {
#if CONFIG_ESP_INSIGHTS_COREDUMP_ENABLE
                    esp_core_dump_image_erase();
#endif // CONFIG_ESP_INSIGHTS_COREDUMP_ENABLE
                    s_insights_data.boot_msg_id = 0;
                }
                xSemaphoreGive(s_insights_data.data_lock);
            }
            break;
        case INSIGHTS_EVENT_TRANSPORT_SEND_FAILED:
            xSemaphoreTake(s_insights_data.data_lock, portMAX_DELAY);
            if (xTimerIsTimerActive(s_insights_data.data_send_timer) == pdTRUE) {
                xTimerStop(s_insights_data.data_send_timer, portMAX_DELAY);
            }
            s_insights_data.data_send_inprogress = false;
            if (s_insights_data.boot_msg_id > 0 && data->msg_id == s_insights_data.boot_msg_id) {
                s_insights_data.boot_msg_id = -1;
            }
            xSemaphoreGive(s_insights_data.data_lock);
            break;
        default:
            break;
    }
}

#if INSIGHTS_DEBUG_ENABLED
#ifndef CONFIG_ESP_INSIGHTS_DEBUG_PRINT_JSON
static void hex_dump(uint8_t *data, uint32_t len)
{
    int index;
    for (index = 0; index < len; index++) {
        if ((index % 16) == 0) {
            printf("\n");
        }
        printf("0x%02x ", s_insights_data.scratch_buf[index]);
    }
    printf("\n");
}
#endif

static void insights_dbg_dump(uint8_t *data, uint32_t len)
{
#if CONFIG_ESP_INSIGHTS_DEBUG_PRINT_JSON
    esp_insights_cbor_decode_dump((const uint8_t *) (s_insights_data.scratch_buf + 3), len - 3);
#else
    hex_dump(data, len);
#endif
}
#endif /* INSIGHTS_DEBUG_ENABLED */

static void send_boottime_data(void)
{
    uint16_t len = 0;
    esp_insights_encode_data_begin(s_insights_data.scratch_buf, INSIGHTS_DATA_MAX_SIZE);
    esp_insights_encode_boottime_data();
    len = esp_insights_encode_data_end(s_insights_data.scratch_buf);
    if (len == 0) {
        ESP_LOGE(TAG, "No boottime data to send");
        s_insights_data.boot_msg_id = 0; // mark it sent
    }
#if INSIGHTS_DEBUG_ENABLED
    ESP_LOGI(TAG, "Sending boottime data of length: %d", len);
    insights_dbg_dump(s_insights_data.scratch_buf, len);
#endif
    int msg_id = esp_insights_transport_data_send(s_insights_data.scratch_buf, len);
    s_insights_data.boot_msg_id = msg_id;
    if (msg_id > 0) {
        return;
    } else if (msg_id == 0) {
#if CONFIG_ESP_INSIGHTS_COREDUMP_ENABLE
        esp_core_dump_image_erase();
#endif // CONFIG_ESP_INSIGHTS_COREDUMP_ENABLE
    } else {
#if INSIGHTS_DEBUG_ENABLED
        ESP_LOGI(TAG, "boottime_data message send failed");
#endif
    }
}

#if SEND_INSIGHTS_META
/* Returns true if ESP Insights metadata CRC is changed */
static bool insights_meta_changed(void)
{
    uint32_t nvs_crc;
    uint32_t meta_crc = esp_diag_meta_crc_get();
    esp_err_t err = esp_insights_meta_nvs_crc_get(&nvs_crc);
    if (err == ESP_OK && nvs_crc == meta_crc) {
        /* crc found and matched, no need to send insights meta */
        return false;
    }
    ESP_LOGI(TAG, "Insights metrics metadata changed");
    s_insights_data.meta_crc = meta_crc;
    return true;
}

static void send_insights_meta(void)
{
    uint16_t len = 0;

    memset(s_insights_data.scratch_buf, 0, INSIGHTS_DATA_MAX_SIZE);
    len = esp_insights_encode_meta(s_insights_data.scratch_buf, INSIGHTS_DATA_MAX_SIZE, s_insights_data.app_sha256);
    if (len == 0) {
#if INSIGHTS_DEBUG_ENABLED
        ESP_LOGI(TAG, "No metadata to send");
#endif
        return;
    }
#if INSIGHTS_DEBUG_ENABLED
    ESP_LOGI(TAG, "Insights meta data length %d", len);
    insights_dbg_dump(s_insights_data.scratch_buf, len);
#endif
    int msg_id = esp_insights_transport_data_send(s_insights_data.scratch_buf, len);
    if (msg_id > 0) {
        xSemaphoreTake(s_insights_data.data_lock, portMAX_DELAY);
        s_insights_data.meta_msg_pending = true;
        s_insights_data.meta_msg_id = msg_id;
        xSemaphoreGive(s_insights_data.data_lock);
    } else if (msg_id == 0) {
        esp_insights_meta_nvs_crc_set(s_insights_data.meta_crc);
    } else {
#if INSIGHTS_DEBUG_ENABLED
        ESP_LOGI(TAG, "meta message send failed");
#endif
    }
}
#endif /* SEND_INSIGHTS_META */

/* Consider 100 bytes are published and received on cloud but RMAKER_MQTT_EVENT_PUBLISHED
 * event is not received for 100 bytes. In a mean time 50 bytes are added to the buffer.
 * When the next time timer expires then old 100 bytes plus new 50 bytes will be published
 * and if RMAKER_MQTT_EVENT_PUBLISHED event is recieve for the new message then 150 bytes
 * will be removed from the buffers.
 *
 * In short, there is the possibility of data duplication, so cloud should be able to handle it.
 */

/* This encodes and sends insights data */
static void send_insights_data(void)
{
    uint16_t len = 0;
    size_t critical_data_size = 0;
    size_t non_critical_data_size = 0;
    size_t critical_consumed = 0;
    size_t non_critical_consumed = 0;

    memset(s_insights_data.scratch_buf, 0, INSIGHTS_DATA_MAX_SIZE);

#if CONFIG_DIAG_ENABLE_VARIABLES
    static uint32_t prev_log_write_fail_cnt = 0;
    if (s_insights_data.log_write_fail_cnt > prev_log_write_fail_cnt) {
        prev_log_write_fail_cnt = s_insights_data.log_write_fail_cnt;
        esp_diag_variable_add_uint(KEY_LOG_WR_FAIL, prev_log_write_fail_cnt);
    }
#endif /* CONFIG_DIAG_ENABLE_VARIABLES */

    esp_insights_encode_data_begin(s_insights_data.scratch_buf, INSIGHTS_DATA_MAX_SIZE);

    critical_data_size = esp_diag_data_store_critical_read(s_insights_data.read_buf, INSIGHTS_READ_BUF_SIZE);
    if (critical_data_size > 0) {
        critical_consumed = esp_insights_encode_critical_data(s_insights_data.read_buf, critical_data_size);
    }

    non_critical_data_size = esp_diag_data_store_non_critical_read(s_insights_data.read_buf, INSIGHTS_READ_BUF_SIZE);
    if (non_critical_data_size > 0) {
        non_critical_consumed = esp_insights_encode_non_critical_data(s_insights_data.read_buf, non_critical_data_size);
        esp_diag_data_store_non_critical_release(non_critical_consumed);
    }
    len = esp_insights_encode_data_end(s_insights_data.scratch_buf);
    if (!critical_consumed && !non_critical_consumed) {
        len = 0; // just ignore the encoded data
    }

    if (len == 0) {
#if INSIGHTS_DEBUG_ENABLED
        ESP_LOGI(TAG, "No data to send");
#endif
        goto data_send_end;
    }
#if INSIGHTS_DEBUG_ENABLED
    ESP_LOGI(TAG, "Sending data of length: %d", len);
    insights_dbg_dump(s_insights_data.scratch_buf, len);
#endif
    int msg_id = esp_insights_transport_data_send(s_insights_data.scratch_buf, len);
    if (msg_id > 0) {
        xSemaphoreTake(s_insights_data.data_lock, portMAX_DELAY);
        s_insights_data.data_msg_len = critical_consumed;
        s_insights_data.data_msg_id = msg_id;
        xTimerReset(s_insights_data.data_send_timer, portMAX_DELAY);
        xSemaphoreGive(s_insights_data.data_lock);
        return;
    } else if (msg_id == 0) {
        esp_diag_data_store_critical_release(critical_consumed);
        s_insights_data.data_sent = true;
    } else {
#if INSIGHTS_DEBUG_ENABLED
        ESP_LOGI(TAG, "insights_data message send failed");
#endif
    }
data_send_end:
    xSemaphoreTake(s_insights_data.data_lock, portMAX_DELAY);
    s_insights_data.data_send_inprogress = false;
    xSemaphoreGive(s_insights_data.data_lock);
}

static void insights_periodic_handler(void *priv_data)
{
    xSemaphoreTake(s_insights_data.data_lock, portMAX_DELAY);
    /* Return if wifi is disconnected */
    if (is_insights_active() == false) {
        s_insights_data.data_send_inprogress = false;
        xSemaphoreGive(s_insights_data.data_lock);
        return;
    }
    if (s_insights_data.data_send_inprogress) {
        xSemaphoreGive(s_insights_data.data_lock);
        return;
    }
    s_insights_data.data_send_inprogress = true;
    xSemaphoreGive(s_insights_data.data_lock);
#if SEND_INSIGHTS_META
    if (insights_meta_changed()) {
        send_insights_meta();
    }
#endif /* SEND_INSIGHTS_META */
    if (s_insights_data.boot_msg_id == -1) {
        send_boottime_data();
    }
    send_insights_data();
}

esp_err_t esp_insights_send_data(void)
{
    if (is_insights_active() == true) {
        ESP_LOGI(TAG, "Sending data to cloud");
        return esp_rmaker_work_queue_add_task(insights_periodic_handler, NULL);
    }
    ESP_LOGW(TAG, "Wi-Fi not in connected state");
    return ESP_FAIL;
}

static void data_store_event_handler(void* arg, esp_event_base_t event_base,
                                    int32_t event_id, void* event_data)
{
    if (event_base != ESP_DIAG_DATA_STORE_EVENT) {
        return;
    }
    switch(event_id) {
        case ESP_DIAG_DATA_STORE_EVENT_CRITICAL_DATA_LOW_MEM:
        case ESP_DIAG_DATA_STORE_EVENT_NON_CRITICAL_DATA_LOW_MEM:
        {
#if INSIGHTS_DEBUG_ENABLED
            ESP_LOGI(TAG, "ESP_DIAG_DATA_STORE_EVENT_%sCRITICAL_DATA_LOW_MEM",
                    event_id == ESP_DIAG_DATA_STORE_EVENT_CRITICAL_DATA_LOW_MEM ? "" : "NON_");
#endif
            if (is_insights_active() == true) {
                esp_rmaker_work_queue_add_task(insights_periodic_handler, NULL);
            }
            break;
        }

        case ESP_DIAG_DATA_STORE_EVENT_CRITICAL_DATA_WRITE_FAIL:
            s_insights_data.log_write_fail_cnt++;
#if INSIGHTS_DEBUG_ENABLED
            ESP_LOGI(TAG, "Log write fail count: %" PRIu32, s_insights_data.log_write_fail_cnt);
#endif
            break;

        case ESP_DIAG_DATA_STORE_EVENT_NON_CRITICAL_DATA_WRITE_FAIL:
#if INSIGHTS_DEBUG_ENABLED
            ESP_LOGI(TAG, "Non critical data write failed");
#endif
            break;

        default:
            break;
    }
}

static esp_err_t log_write_cb(void *data, size_t len, void *priv_data)
{
    esp_err_t ret_val = esp_diag_data_store_critical_write(data, len);
#if INSIGHTS_DEBUG_ENABLED
    if (ret_val != ESP_OK) {
        ESP_LOGI(TAG, "esp_diag_data_store_critical_write failed len %d, err 0x%04x", len, ret_val);
    }
#endif
    return ret_val;
}

#if CONFIG_DIAG_ENABLE_METRICS
static esp_err_t metrics_write_cb(const char *group, void *data, size_t len, void *cb_arg)
{
    esp_err_t ret_val = esp_diag_data_store_non_critical_write(group, data, len);
#if INSIGHTS_DEBUG_ENABLED
    if (ret_val != ESP_OK) {
        ESP_LOGI(TAG, "esp_diag_data_store_non_critical_write failed group %s, len %d, err 0x%04x", group, len, ret_val);
    }
#endif
    return ret_val;
}

static void metrics_init(void)
{
    /* Initialize and enable metrics */
    esp_diag_metrics_config_t metrics_config = {
        .write_cb = metrics_write_cb,
        .cb_arg = NULL,
    };
    esp_err_t ret = esp_diag_metrics_init(&metrics_config);
    if (ret == ESP_OK) {
#if CONFIG_DIAG_ENABLE_HEAP_METRICS
        ret = esp_diag_heap_metrics_init();
        if (ret != ESP_OK) {
            ESP_LOGW(TAG, "Failed to initialize heap metrics");
        }
#endif /* CONFIG_DIAG_ENABLE_HEAP_METRICS */
#if CONFIG_DIAG_ENABLE_WIFI_METRICS
        ret = esp_diag_wifi_metrics_init();
        if (ret != ESP_OK) {
            ESP_LOGW(TAG, "Failed to initialize wifi metrics");
        }
#endif /* CONFIG_DIAG_ENABLE_WIFI_METRICS */
        return;
    }
    ESP_LOGE(TAG, "Failed to initialize metrics.");
}

static void metrics_deinit(void)
{
#if CONFIG_DIAG_ENABLE_HEAP_METRICS
    esp_diag_heap_metrics_deinit();
#endif
#if CONFIG_DIAG_ENABLE_WIFI_METRICS
    esp_diag_wifi_metrics_deinit();
#endif
    esp_diag_metrics_deinit();
}
#endif /* CONFIG_DIAG_ENABLE_METRICS */

#if CONFIG_DIAG_ENABLE_VARIABLES
static esp_err_t variables_write_cb(const char *group, void *data, size_t len, void *cb_arg)
{
    return esp_diag_data_store_non_critical_write(group, data, len);
}

static void variables_init(void)
{
    /* Initialize and enable variables */
    esp_diag_variable_config_t variable_config = {
        .write_cb = variables_write_cb,
        .cb_arg = NULL,
    };
    esp_err_t ret = esp_diag_variable_init(&variable_config);
    if (ret == ESP_OK) {
#if CONFIG_DIAG_ENABLE_NETWORK_VARIABLES
        ret = esp_diag_network_variables_init();
        if (ret != ESP_OK) {
            ESP_LOGW(TAG, "Failed to initialize network variables");
        }
#endif /* CONFIG_DIAG_ENABLE_NETWORK_VARIABLES */
        esp_diag_variable_register("diag", KEY_LOG_WR_FAIL, "Log write fail count", "Diagnostics.Log", ESP_DIAG_DATA_TYPE_UINT);
        return;
    }
    ESP_LOGE(TAG, "Failed to initialize param-values.");
}

static void variables_deinit(void)
{
#if CONFIG_DIAG_ENABLE_NETWORK_VARIABLES
    esp_diag_network_variables_deinit();
#endif
    esp_diag_variables_deinit();
}
#endif /* CONFIG_DIAG_ENABLE_VARIABLES */

void esp_insights_disable(void)
{
    s_insights_data.enabled = false;

    esp_insights_unregister_periodic_handler();
#ifdef CONFIG_DIAG_ENABLE_VARIABLES
    variables_deinit();
#endif
#ifdef CONFIG_DIAG_ENABLE_METRICS
    metrics_deinit();
#endif
    esp_diag_log_hook_disable(ESP_DIAG_LOG_TYPE_ERROR | ESP_DIAG_LOG_TYPE_WARNING | ESP_DIAG_LOG_TYPE_EVENT);
    esp_diag_data_store_deinit();
    esp_event_handler_unregister(INSIGHTS_EVENT, ESP_EVENT_ANY_ID, insights_event_handler);
    esp_event_handler_unregister(ESP_DIAG_DATA_STORE_EVENT, ESP_EVENT_ANY_ID, data_store_event_handler);
    if (s_insights_data.data_lock) {
        vSemaphoreDelete(s_insights_data.data_lock);
        s_insights_data.data_lock = NULL;
    }
    if (s_insights_data.scratch_buf) {
        free(s_insights_data.scratch_buf);
        s_insights_data.scratch_buf = NULL;
    }
    if (s_insights_data.data_send_timer) {
        xTimerDelete(s_insights_data.data_send_timer, portMAX_DELAY);
        s_insights_data.data_send_timer = NULL;
    }
    if (s_insights_data.node_id) {
        free(s_insights_data.node_id);
        s_insights_data.node_id = NULL;
    }
}

void esp_insights_deinit(void)
{
    esp_insights_transport_disconnect();
    esp_insights_disable();
    esp_insights_transport_unregister();
    esp_rmaker_work_queue_deinit();
    s_insights_data.init_done = false;
}

/* Use the node id provided by user, if it is NULL and transport is set to MQTT then
 * try to find node id in factory partition. If not found in factory partition or in case
 * of HTTPS transport generate one using mac address
 */
static esp_err_t esp_insights_set_node_id(const char *node_id)
{
    if (node_id) {
        s_insights_data.node_id = strdup(node_id);
        if (!s_insights_data.node_id) {
            return ESP_ERR_NO_MEM;
        }
        return ESP_OK;
    }
#ifdef CONFIG_ESP_INSIGHTS_TRANSPORT_MQTT
    if (esp_rmaker_factory_init() == ESP_OK) {
        s_insights_data.node_id = esp_rmaker_factory_get("node_id");
    }
#endif
    if (!s_insights_data.node_id) {
        uint8_t eth_mac[6];
        if (esp_read_mac(eth_mac, ESP_MAC_WIFI_STA) != ESP_OK) {
            ESP_LOGE(TAG, "Could not fetch MAC address.");
            return ESP_FAIL;
        }
        s_insights_data.node_id = calloc(1, 13); /* 12 bytes for mac + 1 for NULL terminatation */
        if (!s_insights_data.node_id) {
            return ESP_ERR_NO_MEM;
        }
        snprintf(s_insights_data.node_id, 13, "%02X%02X%02X%02X%02X%02X",
                  eth_mac[0], eth_mac[1], eth_mac[2], eth_mac[3], eth_mac[4], eth_mac[5]);
    }
    return ESP_OK;
}

const char *esp_insights_get_node_id(void)
{
    return s_insights_data.node_id;
}

static esp_err_t esp_insights_read_diag_data_store_crc_from_nvs(uint32_t *crc)
{
    if (!crc) {
        return ESP_ERR_INVALID_ARG;
    }
    nvs_handle_t handle;
    esp_err_t err = nvs_open(INSIGHTS_NVS_NAMESPACE, NVS_READONLY, &handle);
    if (err != ESP_OK) {
        return err;
    }
    err = nvs_get_u32(handle, DIAG_DATA_STORE_CRC_KEY, crc);
    nvs_close(handle);
    return err;
}

static esp_err_t esp_insights_set_rtc_crc_in_nvs(uint32_t crc)
{
    if (!crc) {
        return ESP_ERR_INVALID_ARG;
    }
    nvs_handle_t handle;
    esp_err_t err = nvs_open(INSIGHTS_NVS_NAMESPACE, NVS_READWRITE, &handle);
    if (err != ESP_OK) {
        return err;
    }
err = nvs_set_u32(handle, DIAG_DATA_STORE_CRC_KEY, crc);
    nvs_commit(handle);
    nvs_close(handle);
    return err;
}

esp_err_t esp_insights_enable(esp_insights_config_t *config)
{
    esp_err_t err = ESP_OK;
    if (!config) {
        return ESP_ERR_INVALID_ARG;
    }
    if (s_insights_data.data_lock) {
        ESP_LOGW(TAG, "Insights already enabled");
        return ESP_OK;
    }
    s_insights_data.data_lock = xSemaphoreCreateMutex();
    if (!s_insights_data.data_lock) {
        ESP_LOGE(TAG, "Failed to create data lock.");
        return ESP_ERR_NO_MEM;
    }
    err = s_insights_data.node_id ? ESP_OK : esp_insights_set_node_id(config->node_id);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to set node id");
        goto enable_err;
    }
    if (config->alloc_ext_ram) {
        s_insights_data.scratch_buf = MEM_ALLOC_EXTRAM(INSIGHTS_DATA_MAX_SIZE);
        s_insights_data.read_buf = MEM_ALLOC_EXTRAM(INSIGHTS_READ_BUF_SIZE);
    } else {
        s_insights_data.scratch_buf = malloc(INSIGHTS_DATA_MAX_SIZE);
        s_insights_data.read_buf = malloc(INSIGHTS_READ_BUF_SIZE);
    }
    if (!s_insights_data.scratch_buf) {
        ESP_LOGE(TAG, "Failed to allocate memory for scratch buffer.");
        err = ESP_ERR_NO_MEM;
        goto enable_err;
    }
    if (!s_insights_data.read_buf) {
        ESP_LOGE(TAG, "Failed to allocate memory for read_buf");
        free(s_insights_data.scratch_buf);
        err = ESP_ERR_NO_MEM;
        goto enable_err;
    }

    /* Get sha256 */
    esp_diag_device_info_t device_info;
    memset(&device_info, 0, sizeof(device_info));
    err = esp_diag_device_info_get(&device_info);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to get device info");
        goto enable_err;
    }
    memcpy(s_insights_data.app_sha256, device_info.app_elf_sha256, sizeof(s_insights_data.app_sha256));
    err = esp_event_handler_register(INSIGHTS_EVENT, ESP_EVENT_ANY_ID, insights_event_handler, NULL);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to register event handler for INSIGHTS_EVENTS");
        goto enable_err;
    }
    /* Register event handler for data store events */
    err = esp_event_handler_register(ESP_DIAG_DATA_STORE_EVENT, ESP_EVENT_ANY_ID, data_store_event_handler, NULL);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to register event handler for DIAG_DATA_STORE_EVENT");
        goto enable_err;
    }
    err = esp_diag_data_store_init();
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialise RTC store.");
        goto enable_err;
    }
    uint32_t new_crc, previous_crc;
    new_crc = esp_diag_data_store_get_crc();
    new_crc = esp_crc32_le(esp_diag_data_size_get_crc(), (const unsigned char *)&new_crc, sizeof(new_crc));
    err = esp_insights_read_diag_data_store_crc_from_nvs(&previous_crc);
    if ((err != ESP_OK) || (new_crc != previous_crc)) {
        ESP_LOGI(TAG, "RTC Store configuration changed. Discarding previous data from RTC buffers");
        esp_diag_data_discard_data();
        err = esp_insights_set_rtc_crc_in_nvs(new_crc);
        if (err != ESP_OK) {
            ESP_LOGI(TAG, "Failed to set RTC Store CRC in nvs.");
        }
    }
    esp_diag_log_config_t log_config = {
        .write_cb = log_write_cb,
        .cb_arg = NULL,
    };
    err = esp_diag_log_hook_init(&log_config);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialise Log hook.");
        goto enable_err;
    }
    esp_diag_log_hook_enable(config->log_type);

#if CONFIG_DIAG_ENABLE_METRICS
    metrics_init();
#endif /* CONFIG_DIAG_ENABLE_METRICS */
#if CONFIG_DIAG_ENABLE_VARIABLES
    variables_init();
#endif /* CONFIG_DIAG_ENABLE_VARIABLES */

    s_insights_data.boot_msg_id = -1;
    s_insights_data.data_send_timer = xTimerCreate("data_send_timer", CLOUD_REPORTING_TIMEOUT_TICKS,
                                                   pdFALSE, NULL, data_send_timeout_cb);
    if (!s_insights_data.data_send_timer) {
        ESP_LOGE(TAG, "Failed to create data_send_timer.");
        err = ESP_ERR_NO_MEM;
        goto enable_err;
    }

    err = esp_insights_register_periodic_handler(insights_periodic_handler,
                CLOUD_REPORTING_PERIOD_MIN_SEC, CLOUD_REPORTING_PERIOD_MAX_SEC, NULL);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to register insights_periodic_handler.");
        goto enable_err;
    }

    s_insights_data.enabled = true;

    ESP_LOGI(TAG, "=========================================");
    ESP_LOGI(TAG, "Insights enabled for Node ID %s", s_insights_data.node_id);
    ESP_LOGI(TAG, "=========================================");
    s_insights_data.init_done = true;
    return ESP_OK;
enable_err:
    esp_insights_disable();
    return err;
}

esp_err_t esp_insights_init(esp_insights_config_t *config)
{
    esp_err_t err;
    if (s_insights_data.data_lock) {
        ESP_LOGW(TAG, "ESP Insights already initialized");
        return ESP_OK;
    }
    if (!config) {
        return ESP_ERR_INVALID_ARG;
    }
    /* set node id */
    err = esp_insights_set_node_id(config->node_id);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to set node id");
        return err;
    }
#ifdef CONFIG_ESP_INSIGHTS_TRANSPORT_MQTT
    err = esp_insights_transport_register(&g_default_insights_transport_mqtt);
#else
    g_default_insights_transport_https.userdata = (void *)config->auth_key;
    err = esp_insights_transport_register(&g_default_insights_transport_https);
#endif
    if (err != ESP_OK && err != ESP_ERR_INVALID_STATE) {
        ESP_LOGE(TAG, "Failed to register transport");
        goto init_err;
    }
    err = esp_rmaker_work_queue_init();
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialise Work Queue.");
        goto init_err;
    }
    err = esp_insights_enable(config);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to enable ESP Insights.");
        goto init_err;
    }
    err = esp_insights_transport_connect();
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to connect to transport.");
        goto init_err;
    }
    err = esp_rmaker_work_queue_start();
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to start Work Queue.");
        goto init_err;
    }

    s_insights_data.init_done = true;
    return ESP_OK;
init_err:
    if (s_insights_data.node_id) {
        free(s_insights_data.node_id);
        s_insights_data.node_id = NULL;
    }
    esp_insights_deinit();
    return err;
}

#endif /* CONFIG_ESP_INSIGHTS_ENABLED */
