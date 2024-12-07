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

#include <stdint.h>
#include <string.h>
#include <soc/soc_memory_layout.h>
#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>
#include <esp_system.h>
#include <esp_log.h>
#include <nvs_flash.h>
#include <esp_ota_ops.h>

#include <esp_diag_data_store.h>
#include "rtc_store.h"
#include <esp_crc.h>
#include <inttypes.h>

#if __has_include("esp_idf_version.h")
#include "esp_idf_version.h"
#endif

#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(5, 0, 0)
#include <esp_random.h> // esp_system.h does not provice esp_random() API from IDF v5.0
#include <esp_app_desc.h> // for `esp_app_get_elf_sha256` API
#endif

#define TAG "RTC_STORE"
#define INSIGHTS_NVS_NAMESPACE "storage"
/**
 * @brief Manages RTC store for critical and non_critical data
 *
 * @attention there are some prints in this file, (not logs). this is to avoid logging them in Insights,
 *    which may get stuck in recursive mutex etc. Please be careful if you are using logs...
 */

#if CONFIG_ESP_INSIGHTS_DEBUG_ENABLED
#define RTC_STORE_DBG_PRINTS 1
#endif

#define DIAG_CRITICAL_BUF_SIZE        CONFIG_RTC_STORE_CRITICAL_DATA_SIZE
#define NON_CRITICAL_DATA_SIZE        (CONFIG_RTC_STORE_DATA_SIZE - DIAG_CRITICAL_BUF_SIZE)

/* If data is perfectly aligned then buffers get wrapped and we have to perform two read
 * operation to get all the data, +1 ensures that data will be moved to the start of buffer
 * when there is not enough space at the end of buffer.
 */
#if ((NON_CRITICAL_DATA_SIZE % 4) == 0)
#define DIAG_NON_CRITICAL_BUF_SIZE    (NON_CRITICAL_DATA_SIZE + 1)
#else
#define DIAG_NON_CRITICAL_BUF_SIZE    NON_CRITICAL_DATA_SIZE
#endif

/* When buffer is filled beyond configured capacity then we post an event.
 * In case of failure in sending data over the network, new critical data is dropped and
 * non-critical data is overwritten.
 */

/* When current free size of buffer drops below (100 - reporting_watermark)% then we post an event */
#define DIAG_CRITICAL_DATA_REPORTING_WATERMARK \
    ((DIAG_CRITICAL_BUF_SIZE * (100 - CONFIG_DIAG_DATA_STORE_REPORTING_WATERMARK_PERCENT)) / 100)
#define DIAG_NON_CRITICAL_DATA_REPORTING_WATERMARK \
    ((DIAG_NON_CRITICAL_BUF_SIZE * (100 - CONFIG_DIAG_DATA_STORE_REPORTING_WATERMARK_PERCENT)) / 100)

/* non critical data is stored in Length - Value format */
#define SIZE_OF_DATA_LEN    sizeof(size_t)

// Assumption is RTC memory size will never exeed UINT16_MAX
typedef union {
    struct {
        uint16_t read_offset;
        uint16_t filled;
    };
    uint32_t value;
} data_store_info_t;

typedef struct {
    uint8_t *buf;
    size_t size;
    data_store_info_t info;
} data_store_t;

typedef struct {
    SemaphoreHandle_t lock;     // critical lock
    data_store_t *store;        // pointer to rtc data store
    size_t wrap_cnt;            // keep track of no. of times wrapping happened
} rbuf_data_t;

typedef struct {
    bool init;
    rbuf_data_t critical;
    rbuf_data_t non_critical;
    rtc_store_meta_header_t *meta_hdr;
    char sha_sum[2 * SHA_SIZE + 1];
} rtc_store_priv_data_t;

// have a strategy to invalidate data beyond this
//
#define RTC_STORE_MAX_META_RECORDS  (10) // Max master records possible

// each data record must have an identifier to point a meta

typedef struct {
    struct {
        data_store_t store;
        uint8_t buf[DIAG_CRITICAL_BUF_SIZE];
    } critical;
    struct {
        data_store_t store;
        uint8_t buf[DIAG_NON_CRITICAL_BUF_SIZE];
    } non_critical;
    rtc_store_meta_header_t meta[RTC_STORE_MAX_META_RECORDS];
    uint8_t meta_hdr_idx;
} rtc_store_t;

typedef struct {
    uint8_t *critical_buf;
    uint8_t *non_critical_buf;
    rtc_store_t *rtc_store;
    size_t critical_buf_size;
    size_t non_critical_buf_size;
} rtc_store_meta_info_t;

static rtc_store_priv_data_t s_priv_data;
RTC_NOINIT_ATTR static rtc_store_t s_rtc_store;

static inline size_t data_store_get_size(data_store_t *store)
{
    return store->size;
}

static inline size_t data_store_get_free_at_end(data_store_t *store)
{
    data_store_info_t *info = (data_store_info_t *) &store->info;
    size_t free_at_end = 0;
    if (info->read_offset + info->filled < store->size) {
        free_at_end = store->size - (info->filled + info->read_offset);
    }
    return free_at_end;
}

static inline size_t data_store_get_free(data_store_t *store)
{
    data_store_info_t *info = (data_store_info_t *) &store->info;
    return store->size - info->filled;
}

static inline size_t data_store_get_filled(data_store_t *store)
{
    data_store_info_t *info = (data_store_info_t *) &store->info;
    return info->filled;
}

static void rtc_store_read_complete(rbuf_data_t *rbuf_data, size_t len)
{
    data_store_info_t info =  {
        .value = rbuf_data->store->info.value,
    };
#if RTC_STORE_DBG_PRINTS
    ESP_LOGI(TAG, "to free %u, size %u", len, rbuf_data->store->size);
#endif
    // modify new pointers
    info.filled -= len;
    info.read_offset += len;

    if (((data_store_info_t *) &info)->read_offset > rbuf_data->store->size) {
        ((data_store_info_t *) &info)->read_offset -= rbuf_data->store->size;
        rbuf_data->wrap_cnt++; // wrap around count
    }

    // commit modifications
    rbuf_data->store->info.value = info.value;
}

static void rtc_store_write_complete(rbuf_data_t *rbuf_data, size_t len)
{
    data_store_info_t *info = (data_store_info_t *) &rbuf_data->store->info;
#if RTC_STORE_DBG_PRINTS
    ESP_LOGI(TAG, "before write_complete, filled %" PRIu16 ", size %u, read_offset %" PRIu16 ", len %u",
             info->filled, rbuf_data->store->size, info->read_offset, len);
#endif

    info->filled += len;

#if RTC_STORE_DBG_PRINTS
    ESP_LOGI(TAG, "after write_complete, filled %" PRIu16 ", size %u, read_offset %" PRIu16 ", len %u",
             info->filled, rbuf_data->store->size, info->read_offset, len);
#endif
}

// Caller has made sure that enough memory is free...
static size_t rtc_store_write_at_offset(rbuf_data_t *rbuf_data, void *data, size_t len, size_t offset)
{
    data_store_info_t *info = (data_store_info_t *) &rbuf_data->store->info;
#if RTC_STORE_DBG_PRINTS
    ESP_LOGI(TAG, "(write_at_offset): size %u, available: %u, filled %u, read_ptr %" PRIu16 ", to_write %u",
             rbuf_data->store->size, data_store_get_free(rbuf_data->store),
             data_store_get_filled(rbuf_data->store), info->read_offset, len);
#endif

    uint16_t write_offset = info->filled + info->read_offset;
    if (write_offset >= rbuf_data->store->size) { // wrap around
        write_offset -= rbuf_data->store->size;
    }

    size_t free_at_end = data_store_get_free_at_end(rbuf_data->store);
    if (free_at_end && (free_at_end < offset)) {
        offset -= free_at_end;
        memcpy(rbuf_data->store->buf + offset, data, len);
    } else if (free_at_end && (free_at_end < offset + len)) {
        free_at_end -= offset;
        uint8_t *write_ptr = (uint8_t *) (rbuf_data->store->buf + write_offset + offset);
        memcpy(write_ptr, data, free_at_end);
        memcpy(rbuf_data->store->buf, data + free_at_end, len - free_at_end);
    } else {
        uint8_t *write_ptr = (uint8_t *) (rbuf_data->store->buf + write_offset + offset);
        memcpy(write_ptr, data, len);
    }
    return len;
}

static size_t rtc_store_write(rbuf_data_t *rbuf_data, void *data, size_t len)
{
    return rtc_store_write_at_offset(rbuf_data, data, len, 0);
}

esp_err_t rtc_store_critical_data_write(void *data, size_t len)
{
    esp_err_t ret = ESP_OK;

    if (!data || !len) {
        return ESP_ERR_INVALID_ARG;
    }
    if (!s_priv_data.init) {
        printf("rtc_store init not done! skipping critical_data_write...\n");
        return ESP_ERR_INVALID_STATE;
    }

    size_t len_real = len + 1; // 1 byte to store meta_index
    if (len_real > DIAG_CRITICAL_BUF_SIZE) {
        printf("rtc_store_critical_data_write: len too large %d, size %d\n",
                len_real, DIAG_CRITICAL_BUF_SIZE);
        return ESP_FAIL;
    }
    xSemaphoreTake(s_priv_data.critical.lock, portMAX_DELAY);

    size_t curr_free = data_store_get_free(s_priv_data.critical.store);
    // size_t free_at_end = data_store_get_free_at_end(s_priv_data.critical.store);
    // If no space available... Raise write fail event
    if (curr_free < len_real) {
        esp_event_post(ESP_DIAG_DATA_STORE_EVENT, ESP_DIAG_DATA_STORE_EVENT_CRITICAL_DATA_WRITE_FAIL, data, len_real, 0);
#if RTC_STORE_DBG_PRINTS
        printf("%s, curr_free %d, req_free %d\n", TAG, curr_free, len_real);
#endif
        ret = ESP_ERR_NO_MEM;
    } else { // we have enough space of (len + 1)
        rtc_store_write(&s_priv_data.critical, &s_rtc_store.meta_hdr_idx, 1);
        rtc_store_write_at_offset(&s_priv_data.critical, data, len, 1);
        rtc_store_write_complete(&s_priv_data.critical, len + 1);
        curr_free = data_store_get_free(s_priv_data.critical.store);
    }
    xSemaphoreGive(s_priv_data.critical.lock);

    if (curr_free < DIAG_CRITICAL_DATA_REPORTING_WATERMARK) {
        esp_event_post(ESP_DIAG_DATA_STORE_EVENT, ESP_DIAG_DATA_STORE_EVENT_CRITICAL_DATA_LOW_MEM, NULL, 0, 0);
    }
    return ret;
}

static int rtc_store_data_read_unsafe(rbuf_data_t *rbuf_data, uint8_t *buf, size_t size);

esp_err_t rtc_store_non_critical_data_write(const char *dg, void *data, size_t len)
{
    if (!dg || !len || !data) {
        return ESP_ERR_INVALID_ARG;
    }
    if (!esp_ptr_in_drom(dg)) {
        return ESP_ERR_INVALID_ARG;
    }
    if (!s_priv_data.init) {
        printf("rtc_store init not done! skipping non_critical_data_write...\n");
        return ESP_ERR_INVALID_STATE;
    }
    rtc_store_non_critical_data_hdr_t header;
    size_t req_free = sizeof(header) + len + 1; // 1 byte for meta index
    size_t curr_free;

    if (req_free > DIAG_NON_CRITICAL_BUF_SIZE) {
        printf("rtc_store_non_critical_data_write: len too large %d, size %d\n",
                req_free, DIAG_NON_CRITICAL_BUF_SIZE);
        return ESP_FAIL;
    }

    if (xSemaphoreTake(s_priv_data.non_critical.lock, 0) == pdFALSE) {
        return ESP_FAIL;
    }

#if CONFIG_RTC_STORE_OVERWRITE_NON_CRITICAL_DATA
    /* Make enough room for the item */
    while (data_store_get_free(s_priv_data.non_critical.store) < req_free) {
        uint8_t tmp_buf[sizeof(header) + 1];
        rtc_store_data_read_unsafe(&s_priv_data.non_critical, tmp_buf, sizeof(tmp_buf));
        memcpy(&header, tmp_buf + 1, sizeof(header)); // because 1 byte is meta_hdr idx
        size_t to_free = sizeof(tmp_buf) + header.len;
        rtc_store_read_complete(&s_priv_data.non_critical, to_free);
    }
#else // just check if we have enough space to write the item
    curr_free = data_store_get_free(s_priv_data.non_critical.store);
    if (curr_free < req_free) {
        xSemaphoreGive(s_priv_data.non_critical.lock);
        esp_event_post(ESP_DIAG_DATA_STORE_EVENT, ESP_DIAG_DATA_STORE_EVENT_NON_CRITICAL_DATA_LOW_MEM, NULL, 0, 0);
        return ESP_ERR_NO_MEM;
    }
#endif
    memset(&header, 0, sizeof(header));
    header.dg = dg;
    header.len = len;

    // we have made sure of free size at this point, write index byte, data header and then actual data
    rtc_store_write(&s_priv_data.non_critical, &s_rtc_store.meta_hdr_idx, 1);
    rtc_store_write_at_offset(&s_priv_data.non_critical, &header, sizeof(header), 1);
    rtc_store_write_at_offset(&s_priv_data.non_critical, data, len, 1 + sizeof(header));
    rtc_store_write_complete(&s_priv_data.non_critical, req_free);

    curr_free = data_store_get_free(s_priv_data.non_critical.store);
    xSemaphoreGive(s_priv_data.non_critical.lock);

    // Post low memory event even if data overwrite is enabled.
    if (curr_free < DIAG_NON_CRITICAL_DATA_REPORTING_WATERMARK) {
        esp_event_post(ESP_DIAG_DATA_STORE_EVENT, ESP_DIAG_DATA_STORE_EVENT_NON_CRITICAL_DATA_LOW_MEM, NULL, 0, 0);
    }
    return ESP_OK;
}

static int rtc_store_data_read_unsafe(rbuf_data_t *rbuf_data, uint8_t *buf, size_t size)
{
    data_store_info_t *info = (data_store_info_t *) &rbuf_data->store->info;

    if (info->filled < size) {
        size = info->filled;
    }

    size_t data_at_end = rbuf_data->store->size - info->read_offset;
    if (data_at_end < size) {
        // data is wrapped, read data in 2 parts
        memcpy(buf, rbuf_data->store->buf + info->read_offset, data_at_end);
        memcpy(buf + data_at_end, rbuf_data->store->buf, size - data_at_end);
    } else {
        // single memcpy
        memcpy(buf, rbuf_data->store->buf + info->read_offset, size);
    }
    return size;
}

static int rtc_store_data_read(rbuf_data_t *rbuf_data, uint8_t *buf, size_t size)
{
    if (!size) {
        return -1;
    }
    if (!s_priv_data.init) {
        return -1;
    }

    xSemaphoreTake(rbuf_data->lock, portMAX_DELAY);
    size = rtc_store_data_read_unsafe(rbuf_data, buf, size);
    xSemaphoreGive(rbuf_data->lock);
    return size;
}

static esp_err_t rtc_store_data_release(rbuf_data_t *rbuf_data, size_t size)
{
    if (!s_priv_data.init) {
        return ESP_ERR_INVALID_STATE;
    }
    xSemaphoreTake(rbuf_data->lock, portMAX_DELAY);
    data_store_info_t *info = (data_store_info_t *) &rbuf_data->store->info;
    if (info->filled < size) {
        xSemaphoreGive(rbuf_data->lock);
        return ESP_FAIL;
    }
    rtc_store_read_complete(rbuf_data, size);
    xSemaphoreGive(rbuf_data->lock);
    return ESP_OK;
}

int rtc_store_critical_data_read(uint8_t *buf, size_t size)
{
    return rtc_store_data_read(&s_priv_data.critical, buf, size);
}

int rtc_store_critical_data_read_and_release(uint8_t *buf, size_t size)
{
    int data_read = rtc_store_data_read(&s_priv_data.critical, buf, size);
    if (data_read > 0) {
        rtc_store_data_release(&s_priv_data.critical, size);
    }
    return data_read;
}

int rtc_store_non_critical_data_read(uint8_t *buf, size_t size)
{
    return rtc_store_data_read(&s_priv_data.non_critical, buf, size);
}

int rtc_store_non_critical_data_read_and_release(uint8_t *buf, size_t size)
{
    int data_read = rtc_store_data_read(&s_priv_data.non_critical, buf, size);
    if (data_read > 0) {
        rtc_store_data_release(&s_priv_data.non_critical, data_read);
    }
    return data_read;
}

esp_err_t rtc_store_critical_data_release(size_t size)
{
    return rtc_store_data_release(&s_priv_data.critical, size);
}

esp_err_t rtc_store_non_critical_data_release(size_t size)
{
    return rtc_store_data_release(&s_priv_data.non_critical, size);
}

static void rtc_store_rbuf_deinit(rbuf_data_t *rbuf_data)
{
    if (rbuf_data->lock) {
        vSemaphoreDelete(rbuf_data->lock);
        rbuf_data->lock = NULL;
    }
}

void rtc_store_deinit(void)
{
    rtc_store_rbuf_deinit(&s_priv_data.critical);
    rtc_store_rbuf_deinit(&s_priv_data.non_critical);
    s_priv_data.init = false;
}

static bool rtc_store_integrity_check(data_store_t *store)
{
    data_store_info_t *info = (data_store_info_t *) &store->info;
    if (info->filled > store->size ||
            info->read_offset > store->size) {
        return false;
    }
    return true;
}

static esp_err_t rtc_store_rbuf_init(rbuf_data_t *rbuf_data,
                                     data_store_t *rtc_store,
                                     uint8_t *rtc_buf,
                                     size_t rtc_buf_size)
{
    esp_reset_reason_t reset_reason = esp_reset_reason();

    rbuf_data->lock = xSemaphoreCreateMutex();
    if (!rbuf_data->lock) {
#if RTC_STORE_DBG_PRINTS
        printf("rtc_store_rbuf_init: lock creation failed\n");
#endif
        return ESP_ERR_NO_MEM;
    }

    /* Check for stale data */
    if (reset_reason == ESP_RST_UNKNOWN ||
            reset_reason == ESP_RST_POWERON ||
            reset_reason == ESP_RST_BROWNOUT) {
        // TODO: also check if hash is changed
        rtc_store->info.value = 0;
    }

    /* Point priv_data to actual RTC data */
    rbuf_data->store = rtc_store;
    rbuf_data->store->buf = rtc_buf;
    rbuf_data->store->size = rtc_buf_size;

    if (rtc_store_integrity_check(rtc_store) == false) {
        // discard all the existing data
        printf("%s: intergrity_check failed, discarding old data...\n", TAG);
        rtc_store->info.value = 0;
    }
    return ESP_OK;
}

rtc_store_meta_header_t *rtc_store_get_meta_record_by_index(uint8_t idx)
{
    if (idx >= RTC_STORE_MAX_META_RECORDS) {
        printf("%s: meta index out of range [0, %d], index %d\n",
               TAG, RTC_STORE_MAX_META_RECORDS - 1, idx);
        return NULL;
    }
    return &s_rtc_store.meta[idx];
}

rtc_store_meta_header_t *rtc_store_get_meta_record_current()
{
    return &s_rtc_store.meta[s_rtc_store.meta_hdr_idx];
}

static inline uint8_t to_int_digit(unsigned val)
{
    return (val <= '9') ? (val - '0') : (val - 'a' + 10);
}

static void hex_to_bytes(uint8_t *src, uint8_t *dst, int out_len)
{
    for (int i = 0; i < out_len; i++) {
        uint8_t val0 = to_int_digit(src[2 * i]);
        uint8_t val1 = to_int_digit(src[2 * i + 1]);
        dst[i] = (val0 << 4) | (val1);
    }
}

static esp_err_t rtc_store_meta_hdr_init()
{
    uint8_t gen_id = 0, boot_cnt = 0;
    // Initialize NVS
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        err = nvs_flash_erase();
        if (err != ESP_OK) {
            printf("%s: NVS erase failed!\n", TAG);
            goto skip_nvs_read_write;
        }
        err = nvs_flash_init();
    }
    if (err != ESP_OK) {
        printf("%s: NVS init failed!\n", TAG);
        goto skip_nvs_read_write;
    }

    nvs_handle_t nvs_handle;
    // Open NVS and read our values
    err = nvs_open(INSIGHTS_NVS_NAMESPACE, NVS_READWRITE, &nvs_handle);
    if (err != ESP_OK) {
        printf("%s: Error (%s) opening NVS handle!\n", TAG, esp_err_to_name(err));
        goto skip_nvs_read_write;
    }
    err = nvs_get_u8(nvs_handle, "gen_id", &gen_id) ||
            nvs_get_u8(nvs_handle, "boot_cnt", &boot_cnt);

    boot_cnt += 1;

    if (err != ESP_OK) { // gen_id not found in NVS, hard reset case
        gen_id = esp_random();
        boot_cnt = 0;
        nvs_set_u8(nvs_handle, "gen_id", gen_id);
    }
    nvs_set_u8(nvs_handle, "boot_cnt", boot_cnt);
skip_nvs_read_write:

    s_rtc_store.meta_hdr_idx = (s_rtc_store.meta_hdr_idx + 1) % RTC_STORE_MAX_META_RECORDS;
    s_priv_data.meta_hdr = &s_rtc_store.meta[s_rtc_store.meta_hdr_idx];

    // populate meta header
#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(5, 0, 0)
    esp_app_get_elf_sha256(s_priv_data.sha_sum, sizeof(s_priv_data.sha_sum));
#else
    esp_ota_get_app_elf_sha256(s_priv_data.sha_sum, sizeof(s_priv_data.sha_sum));
#endif
    hex_to_bytes((uint8_t *) s_priv_data.sha_sum, (uint8_t *) s_priv_data.meta_hdr->sha_sum, SHA_SIZE);

    s_priv_data.meta_hdr->gen_id = gen_id;
    s_priv_data.meta_hdr->boot_cnt = boot_cnt;

    return ESP_OK;
}

esp_err_t rtc_store_discard_data(void)
{
    if (!s_priv_data.init) {
        ESP_LOGW(TAG, "RTC Store not initialized yet. Cannot discard data.");
        return ESP_ERR_INVALID_STATE;
    }
    xSemaphoreTake(s_priv_data.critical.lock, portMAX_DELAY);
    s_rtc_store.critical.store.info.value = 0;
    xSemaphoreGive(s_priv_data.critical.lock);
    xSemaphoreTake(s_priv_data.non_critical.lock, portMAX_DELAY);
    s_rtc_store.non_critical.store.info.value = 0;
    xSemaphoreGive(s_priv_data.non_critical.lock);
    return ESP_OK;
}

uint32_t rtc_store_get_crc()
{
    rtc_store_meta_info_t rtc_meta_info = {
        .critical_buf = s_rtc_store.critical.buf,
        .non_critical_buf = s_rtc_store.non_critical.buf,
        .rtc_store = &s_rtc_store,
        .critical_buf_size = DIAG_CRITICAL_BUF_SIZE,
        .non_critical_buf_size = DIAG_NON_CRITICAL_BUF_SIZE
    };
    uint32_t crc = 0;
    crc = esp_crc32_le(crc, (const unsigned char *)&rtc_meta_info, sizeof(rtc_meta_info));
    return crc;
}

esp_err_t rtc_store_init(void)
{
    esp_err_t err;
    if (s_priv_data.init) {
        return ESP_ERR_INVALID_STATE;
    }
    /* Initialize critical RTC rbuf */
    err = rtc_store_rbuf_init(&s_priv_data.critical,
                              &s_rtc_store.critical.store,
                              s_rtc_store.critical.buf,
                              DIAG_CRITICAL_BUF_SIZE);
    if (err != ESP_OK) {
#if RTC_STORE_DBG_PRINTS
        printf("rtc_store_rbuf_init(critical) failed\n");
#endif
        return err;
    }
    /* Initialize non critical RTC rbuf */
    err = rtc_store_rbuf_init(&s_priv_data.non_critical,
                              &s_rtc_store.non_critical.store,
                              s_rtc_store.non_critical.buf,
                              DIAG_NON_CRITICAL_BUF_SIZE);
    if (err != ESP_OK) {
#if RTC_STORE_DBG_PRINTS
        printf("rtc_store_rbuf_init(non_critical) failed\n");
#endif
        rtc_store_rbuf_deinit(&s_priv_data.critical);
        return err;
    }

    esp_reset_reason_t reset_reason = esp_reset_reason();

    if (reset_reason == ESP_RST_UNKNOWN ||
            reset_reason == ESP_RST_POWERON ||
            reset_reason == ESP_RST_BROWNOUT) {
        // TODO: also check if hash is changed
        s_rtc_store.meta_hdr_idx = -1;
    }
    rtc_store_meta_hdr_init();

    s_priv_data.init = true;
    return ESP_OK;
}
