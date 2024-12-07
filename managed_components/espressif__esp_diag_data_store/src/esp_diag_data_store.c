/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include <stdbool.h>
#include <esp_err.h>
#include <esp_diag_data_store.h>
#include <rtc_store.h>

ESP_EVENT_DEFINE_BASE(ESP_DIAG_DATA_STORE_EVENT);

/* Callback type to initialize the store */
typedef esp_err_t (*init_cb_t) (void);
/* Callback type to deinitialize the store */
typedef void (*deinit_cb_t) (void);
/* Callback type to write data */
typedef esp_err_t (*write_cb_t) (void *data, size_t len);
/* Callback type to write non_critical data */
typedef esp_err_t (*nc_write_cb_t) (const char *dg, void *data, size_t len);
/* Callback type to read data */
typedef int (*read_cb_t) (uint8_t *buf, size_t size);
/* Callback type to release the data */
typedef esp_err_t (*release_cb_t) (size_t size);
/* Callback type to get CRC of data store configuration.
This crc will be used to discard data from data store if its value is changed */
typedef uint32_t (*crc_cb_t) ();
/* Callback type to discard data from data store. */
typedef esp_err_t (*discard_data_cb_t) ();

typedef struct {
    init_cb_t init;
    deinit_cb_t deinit;
    write_cb_t critical_write;
    nc_write_cb_t non_critical_write;
    read_cb_t critical_read;
    read_cb_t non_critical_read;
    release_cb_t critical_release;
    release_cb_t non_critical_release;
    crc_cb_t data_store_crc;
    discard_data_cb_t discard_data;
} data_store_cbs_t;

typedef struct {
    bool init;
    data_store_cbs_t cbs;
} priv_data_t;

static priv_data_t s_priv_data;

#define CHECK_STORE_INIT(ret) \
{ \
    if (!s_priv_data.init) { \
        return ret; \
    } \
}

static void set_diag_store_cbs(void)
{
    s_priv_data.cbs.init = rtc_store_init;
    s_priv_data.cbs.deinit = rtc_store_deinit;
    s_priv_data.cbs.critical_write = rtc_store_critical_data_write;
    s_priv_data.cbs.non_critical_write = rtc_store_non_critical_data_write;
    s_priv_data.cbs.critical_read = rtc_store_critical_data_read;
    s_priv_data.cbs.non_critical_read = rtc_store_non_critical_data_read;
    s_priv_data.cbs.critical_release = rtc_store_critical_data_release;
    s_priv_data.cbs.non_critical_release = rtc_store_non_critical_data_release;
    s_priv_data.cbs.data_store_crc = rtc_store_get_crc;
    s_priv_data.cbs.discard_data = rtc_store_discard_data;
}

static void unset_diag_store_cbs(void)
{
    s_priv_data.cbs.init = NULL;
    s_priv_data.cbs.deinit = NULL;
    s_priv_data.cbs.critical_write = NULL;
    s_priv_data.cbs.non_critical_write = NULL;
    s_priv_data.cbs.critical_read = NULL;
    s_priv_data.cbs.non_critical_read = NULL;
    s_priv_data.cbs.critical_release = NULL;
    s_priv_data.cbs.non_critical_release = NULL;
    s_priv_data.cbs.data_store_crc = NULL;
    s_priv_data.cbs.discard_data = NULL;
}

esp_err_t esp_diag_data_store_critical_write(void *data, size_t len)
{
    CHECK_STORE_INIT(ESP_ERR_INVALID_STATE);
    return s_priv_data.cbs.critical_write(data, len);
}

esp_err_t esp_diag_data_store_non_critical_write(const char *dg, void *data, size_t len)
{
    CHECK_STORE_INIT(ESP_ERR_INVALID_STATE);
    return s_priv_data.cbs.non_critical_write(dg, data, len);
}

int esp_diag_data_store_critical_read(uint8_t *buf, size_t size)
{
    CHECK_STORE_INIT(-1);
    return s_priv_data.cbs.critical_read(buf, size);
}

int esp_diag_data_store_non_critical_read(uint8_t *buf, size_t size)
{
    CHECK_STORE_INIT(-1);
    return s_priv_data.cbs.non_critical_read(buf, size);
}

esp_err_t esp_diag_data_store_critical_release(size_t size)
{
    CHECK_STORE_INIT(ESP_ERR_INVALID_STATE);
    return s_priv_data.cbs.critical_release(size);
}

esp_err_t esp_diag_data_store_non_critical_release(size_t size)
{
    CHECK_STORE_INIT(ESP_ERR_INVALID_STATE);
    return s_priv_data.cbs.non_critical_release(size);
}

esp_err_t esp_diag_data_store_init(void)
{
    set_diag_store_cbs();
    esp_err_t err = s_priv_data.cbs.init();
    if (err != ESP_OK) {
        return err;
    }
    s_priv_data.init = true;
    return ESP_OK;
}

void esp_diag_data_store_deinit(void)
{
    CHECK_STORE_INIT();
    s_priv_data.cbs.deinit();
    unset_diag_store_cbs();
    s_priv_data.init = false;
}

uint32_t esp_diag_data_store_get_crc(void)
{
    CHECK_STORE_INIT(ESP_ERR_INVALID_STATE);
    return s_priv_data.cbs.data_store_crc();
}

esp_err_t esp_diag_data_discard_data(void)
{
    CHECK_STORE_INIT(ESP_ERR_INVALID_STATE);
    return s_priv_data.cbs.discard_data();
}