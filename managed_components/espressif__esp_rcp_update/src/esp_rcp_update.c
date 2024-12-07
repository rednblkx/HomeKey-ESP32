/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "esp_rcp_update.h"

#include <ctype.h>
#include <stdio.h>
#include <string.h>

#include "esp32_port.h"
#include "esp_br_firmware.h"
#include "esp_check.h"
#include "esp_err.h"
#include "esp_loader.h"
#include "esp_log.h"
#include "nvs.h"
#include "nvs_flash.h"
#include "driver/gpio.h"
#include "driver/uart.h"

#define RCP_UPDATE_MAX_RETRY 3
#define RCP_VERIFIED_FLAG (1 << 5)
#define RCP_SEQ_KEY "rcp_seq"
#define TAG "RCP_UPDATE"

typedef struct esp_rcp_update_handle {
    nvs_handle_t nvs_handle;
    int8_t update_seq;
    bool verified;
    esp_rcp_update_config_t update_config;
} esp_rcp_update_handle;

struct rcp_flash_arg_t {
    uint32_t tag;
    uint32_t offset;
} __attribute__((packed));

typedef struct rcp_flash_arg_t rcp_flash_arg_t;

static esp_rcp_update_handle s_handle;

static esp_loader_error_t connect_to_target(target_chip_t target_chip, uint32_t higher_baudrate)
{
    esp_loader_connect_args_t connect_config = ESP_LOADER_CONNECT_DEFAULT();

    esp_loader_error_t err = esp_loader_connect(&connect_config);
    if (err != ESP_LOADER_SUCCESS) {
        return err;
    }
    if (esp_loader_get_target() != target_chip) {
        return ESP_LOADER_ERROR_UNSUPPORTED_CHIP;
    }

    if (higher_baudrate) {
        ESP_RETURN_ON_ERROR(esp_loader_change_transmission_rate(higher_baudrate), TAG,
                            "Failed to change bootloader baudrate");
        ESP_RETURN_ON_ERROR(loader_port_change_transmission_rate(higher_baudrate), TAG,
                            "Failed to change local port baudrate");
    }
    return ESP_LOADER_SUCCESS;
}

static esp_err_t seek_to_subfile(FILE *fp, esp_br_filetag_t tag, esp_br_subfile_info_t *found_info)
{
    if (fseek(fp, 0, SEEK_SET) != 0) {
        return ESP_FAIL;
    }
    esp_br_subfile_info_t subfile_info;
    if (fread(&subfile_info, 1, sizeof(subfile_info), fp) != sizeof(subfile_info)) {
        return ESP_FAIL;
    }
    if (subfile_info.tag != FILETAG_IMAGE_HEADER || subfile_info.size % sizeof(subfile_info) != 0) {
        return ESP_FAIL;
    }

    int num_subfiles = subfile_info.size / sizeof(subfile_info);
    for (int i = 1; i < num_subfiles; i++) {
        if (fread(&subfile_info, 1, sizeof(subfile_info), fp) != sizeof(subfile_info)) {
            return ESP_FAIL;
        }
        if (subfile_info.tag == tag) {
            *found_info = subfile_info;
            return fseek(fp, subfile_info.offset, SEEK_SET) == 0 ? ESP_OK : ESP_FAIL;
        }
    }
    return ESP_ERR_NOT_FOUND;
}

esp_err_t esp_rcp_load_version_in_storage(char *version_str, size_t size)
{
    char fullpath[RCP_FILENAME_MAX_SIZE];
    int8_t update_seq = esp_rcp_get_update_seq();

    sprintf(fullpath, "%s_%d/" ESP_BR_RCP_IMAGE_FILENAME, s_handle.update_config.firmware_dir, update_seq);
    FILE *fp = fopen(fullpath, "r");
    if (fp == NULL) {
        return ESP_ERR_NOT_FOUND;
    }
    esp_br_subfile_info_t version_info;
    ESP_RETURN_ON_ERROR(seek_to_subfile(fp, FILETAG_RCP_VERSION, &version_info), TAG, "Failed to find version subfile");
    memset(version_str, 0, size);
    int read_size = size < version_info.size ? size : version_info.size;
    fread(version_str, 1, read_size, fp);
    fclose(fp);
    return ESP_OK;
}

static esp_loader_error_t flash_binary(FILE *firmware, size_t size, size_t address)
{
    esp_loader_error_t err;
    static uint8_t payload[1024];

    ESP_LOGI(TAG, "Erasing flash (this may take a while)...");
    err = esp_loader_flash_start(address, size, sizeof(payload));
    if (err != ESP_LOADER_SUCCESS) {
        ESP_LOGE(TAG, "Erasing flash failed with error %d.", err);
        return err;
    }
    ESP_LOGI(TAG, "Start programming");

    size_t binary_size = size;
    size_t written = 0;

    ESP_LOGI(TAG, "binary_size %u", binary_size);
    while (size > 0) {
        size_t to_read = size < sizeof(payload) ? size : sizeof(payload);
        fread(payload, 1, to_read, firmware);

        err = esp_loader_flash_write(payload, to_read);
        if (err != ESP_LOADER_SUCCESS) {
            ESP_LOGE(TAG, "Packet could not be written! Error %d.", err);
            return err;
        }

        size -= to_read;
        written += to_read;
        ESP_LOGI(TAG, "left size %u, written %u", size, written);

        int progress = (int)(((float)written / binary_size) * 100);
        ESP_LOGI(TAG, "Progress: %d %%", progress);
        fflush(stdout);
    };

    ESP_LOGI(TAG, "Finished programming");

    err = esp_loader_flash_verify();
    if (err != ESP_LOADER_SUCCESS) {
        ESP_LOGE(TAG, "MD5 does not match. err: %d", err);
        return err;
    }
    ESP_LOGI(TAG, "Flash verified");

    return ESP_LOADER_SUCCESS;
}

static void load_rcp_update_seq(esp_rcp_update_handle *handle)
{
    int8_t seq = 0;
    bool verified;
    esp_err_t err = nvs_get_i8(handle->nvs_handle, RCP_SEQ_KEY, &seq);

    if (err != ESP_OK) {
        seq = 0;
        verified = 1;
    } else {
        verified = (seq & RCP_VERIFIED_FLAG);
        seq = (seq & ~RCP_VERIFIED_FLAG);
    }
    handle->update_seq = seq;
    handle->verified = verified;
}

const char *esp_rcp_get_firmware_dir(void)
{
    return s_handle.update_config.firmware_dir;
}

int8_t esp_rcp_get_update_seq(void)
{
    return s_handle.verified ? (s_handle.update_seq) : (1 - s_handle.update_seq);
}

int8_t esp_rcp_get_next_update_seq(void)
{
    return 1 - esp_rcp_get_update_seq();
}

esp_err_t esp_rcp_submit_new_image()
{
    ESP_RETURN_ON_FALSE(s_handle.update_config.rcp_type != RCP_TYPE_INVALID, ESP_ERR_INVALID_STATE, TAG,
                        "RCP update not initialized");
    s_handle.update_seq = esp_rcp_get_next_update_seq();
    s_handle.verified = true;

    int8_t new_seq = s_handle.update_seq | RCP_VERIFIED_FLAG;
    esp_err_t error = nvs_set_i8(s_handle.nvs_handle, RCP_SEQ_KEY, new_seq);
    if (error == ESP_OK) {
        return nvs_commit(s_handle.nvs_handle);
    } else {
        return error;
    }
}

esp_err_t esp_rcp_update_init(const esp_rcp_update_config_t *update_config)
{
    ESP_RETURN_ON_ERROR(nvs_open("storage", NVS_READWRITE, &s_handle.nvs_handle), "TAG", "Failed to open nvs");
    ESP_RETURN_ON_FALSE(update_config->rcp_type == RCP_TYPE_ESP32H2_UART, ESP_ERR_INVALID_ARG, TAG,
                        "Unsupported RCP type");

    s_handle.update_config = *update_config;
    load_rcp_update_seq(&s_handle);
    ESP_LOGI(TAG, "RCP: using update sequence %d", s_handle.update_seq);
    return ESP_OK;
}

#if CONFIG_OPENTHREAD_RADIO_SPINEL_SPI
static esp_err_t esp_rcp_boot_pin_mux(void)
{
    gpio_config_t io_conf;
    memset(&io_conf, 0, sizeof(io_conf));
    io_conf.intr_type = GPIO_INTR_NEGEDGE;
    io_conf.pin_bit_mask = (1ULL << s_handle.update_config.boot_pin);
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pull_up_en = GPIO_PULLUP_ENABLE;
    ESP_RETURN_ON_ERROR(gpio_config(&io_conf), TAG, "Failed to config GPIO[%d]", s_handle.update_config.boot_pin);
    return ESP_OK;
}
#endif

esp_err_t esp_rcp_update(void)
{
    ESP_RETURN_ON_FALSE(s_handle.update_config.rcp_type != RCP_TYPE_INVALID, ESP_ERR_INVALID_STATE, TAG,
                        "RCP update not initialized");

    loader_esp32_config_t loader_config = {
        .baud_rate = s_handle.update_config.uart_baudrate,
        .uart_port = s_handle.update_config.uart_port,
        .uart_rx_pin = s_handle.update_config.uart_rx_pin,
        .uart_tx_pin = s_handle.update_config.uart_tx_pin,
        .reset_trigger_pin = s_handle.update_config.reset_pin,
        .gpio0_trigger_pin = s_handle.update_config.boot_pin,
    };
    ESP_RETURN_ON_ERROR(loader_port_esp32_init(&loader_config), TAG, "Failed to initialize UART port");
    ESP_RETURN_ON_ERROR(connect_to_target(s_handle.update_config.target_chip, s_handle.update_config.update_baudrate),
                        TAG, "Failed to connect to RCP");

    char fullpath[RCP_FILENAME_MAX_SIZE];
    int update_seq = esp_rcp_get_update_seq();
    sprintf(fullpath, "%s_%d/" ESP_BR_RCP_IMAGE_FILENAME, s_handle.update_config.firmware_dir, update_seq);
    FILE *fp = fopen(fullpath, "r");
    ESP_RETURN_ON_FALSE(fp != NULL, ESP_ERR_NOT_FOUND, TAG, "Cannot find rcp image");
    esp_br_subfile_info_t subfile;
    seek_to_subfile(fp, FILETAG_RCP_FLASH_ARGS, &subfile);
    int num_flash_binaries = subfile.size / sizeof(rcp_flash_arg_t);

    for (int i = 0; i < num_flash_binaries; i++) {
        rcp_flash_arg_t flash_args;
        fread(&flash_args, 1, sizeof(flash_args), fp);
        int num_retry = 0;
        long current = ftell(fp);
        if (seek_to_subfile(fp, flash_args.tag, &subfile) != ESP_OK) {
            ESP_LOGE(TAG, "Failed to seek to subfile with tag %lu", flash_args.tag);
            abort();
        }
        while (flash_binary(fp, subfile.size, flash_args.offset) != ESP_LOADER_SUCCESS) {
            ESP_LOGW(TAG, "Failed to flash %s, retrying...", fullpath);
            num_retry++;
            if (num_retry > RCP_UPDATE_MAX_RETRY) {
                ESP_LOGE(TAG, "Failed to update RCP, abort and reboot");
                abort();
            }
            seek_to_subfile(fp, flash_args.tag, &subfile);
        }
        fseek(fp, current, SEEK_SET);
    }
    fclose(fp);
    esp_loader_reset_target();
    loader_port_esp32_deinit();

#if CONFIG_OPENTHREAD_RADIO_SPINEL_SPI
    ESP_RETURN_ON_ERROR(esp_rcp_boot_pin_mux(), TAG, "Failed to multiplex boot pin");
#endif
    return ESP_OK;
}

void esp_rcp_update_deinit(void)
{
    nvs_close(s_handle.nvs_handle);
}

esp_err_t esp_rcp_mark_image_verified(bool verified)
{
    ESP_RETURN_ON_FALSE(s_handle.update_config.rcp_type != RCP_TYPE_INVALID, ESP_ERR_INVALID_STATE, TAG,
                        "RCP update not initialized");
    int8_t val;
    if (!verified) {
        val = esp_rcp_get_update_seq();
    } else {
        val = esp_rcp_get_update_seq() | RCP_VERIFIED_FLAG;
    }
    s_handle.verified = verified;
    s_handle.update_seq = (val & ~RCP_VERIFIED_FLAG);
    esp_err_t error = nvs_set_i8(s_handle.nvs_handle, RCP_SEQ_KEY, val);
    if (error == ESP_OK) {
        return nvs_commit(s_handle.nvs_handle);
    } else {
        return error;
    }
}

esp_err_t esp_rcp_mark_image_unusable(void)
{
    ESP_RETURN_ON_FALSE(s_handle.update_config.rcp_type != RCP_TYPE_INVALID, ESP_ERR_INVALID_STATE, TAG,
                        "RCP update not initialized");

    s_handle.verified = 0;
    int8_t val = s_handle.update_seq & ~RCP_VERIFIED_FLAG;
    esp_err_t error = nvs_set_i8(s_handle.nvs_handle, RCP_SEQ_KEY, val);
    if (error == ESP_OK) {
        return nvs_commit(s_handle.nvs_handle);
    } else {
        return error;
    }
}

void esp_rcp_reset(void)
{
    gpio_config_t io_conf = {};
    uint8_t reset_pin = s_handle.update_config.reset_pin;
    io_conf.pin_bit_mask = ((1ULL << reset_pin));
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_conf.intr_type = GPIO_INTR_DISABLE;
    gpio_config(&io_conf);
    gpio_set_level(reset_pin, 0);
    vTaskDelay(pdMS_TO_TICKS(150));
    gpio_set_level(reset_pin, 1);
    vTaskDelay(pdMS_TO_TICKS(850));
}
