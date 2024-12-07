/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <fcntl.h>
#include "esp_err.h"
#include "esp_check.h"
#include "esp_log.h"
#include "esp_vfs_dev.h"
#include "driver/uart.h"
#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(5, 3, 0)
#include "driver/uart_vfs.h"
#endif
#include "esp_radio_spinel.h"
#include "esp_zigbee_radio_spinel_uart.h"

static const char *TAG = "ESP_ZIGBEE_RADIO_SPINEL_UART";

static esp_err_t zb_uart_deinit(const esp_radio_spinel_uart_config_t *config)
{
    return uart_driver_delete(config->port);
}

static esp_err_t zb_uart_init_port(const esp_radio_spinel_uart_config_t *config)
{
    ESP_RETURN_ON_ERROR(uart_param_config(config->port, &(config->uart_config)), TAG,
                        "Failed to config uart parameters");
    ESP_RETURN_ON_ERROR(
        uart_set_pin(config->port, config->tx_pin, config->rx_pin, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE), TAG,
        "uart_set_pin failed");
    ESP_RETURN_ON_ERROR(uart_driver_install(config->port, 1024, 0, 0, NULL, 0), TAG, "Failed to install uart driver");
#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(5, 3, 0)
    uart_vfs_dev_use_driver(config->port);
#else
    esp_vfs_dev_uart_use_driver(config->port);
#endif
    return ESP_OK;
}

static esp_err_t zb_uart_init(const esp_radio_spinel_uart_config_t *config, esp_line_endings_t tx_mode,
                              esp_line_endings_t rx_mode)
{
    esp_err_t ret = ESP_OK;

    ESP_GOTO_ON_ERROR(zb_uart_init_port(config), exit, TAG, "Failed to initialize uart port");
#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(5, 3, 0)
    uart_vfs_dev_port_set_rx_line_endings(config->port, rx_mode);
    uart_vfs_dev_port_set_tx_line_endings(config->port, tx_mode);
#else
    esp_vfs_dev_uart_port_set_rx_line_endings(config->port, rx_mode);
    esp_vfs_dev_uart_port_set_tx_line_endings(config->port, tx_mode);
#endif
exit:
    if (ret != ESP_OK) {
        zb_uart_deinit(config);
    }
    return ret;
}

static esp_err_t zb_radio_spinel_init_uart(const esp_radio_spinel_uart_config_t *uart_config, int *uart_fd)
{
    char uart_path[16];
    ESP_RETURN_ON_ERROR(zb_uart_init(uart_config, ESP_LINE_ENDINGS_LF, ESP_LINE_ENDINGS_LF), TAG,
                        "Failed to initialize uart");

    snprintf(uart_path, sizeof(uart_path), "/dev/uart/%d", uart_config->port);
    *uart_fd = open(uart_path, O_RDWR | O_NONBLOCK);

    return *uart_fd >= 0 ? ESP_OK : ESP_FAIL;
}

static esp_err_t zb_radio_spinel_deinit_uart(const esp_radio_spinel_uart_config_t *uart_config, int *uart_fd)
{
    if (*uart_fd != -1) {
        close(*uart_fd);
        *uart_fd = -1;
        return zb_uart_deinit(uart_config);
    } else {
        return ESP_ERR_INVALID_STATE;
    }
}

esp_err_t esp_zb_radio_spinel_config_uart(const esp_zb_uart_config_t *uart_config)
{
    const esp_radio_spinel_uart_config_t radio_config = {
        .port = uart_config->port,
        .uart_config = uart_config->uart_config,
        .rx_pin = uart_config->rx_pin,
        .tx_pin = uart_config->tx_pin,
    };

    if (esp_radio_spinel_uart_interface_enable(&radio_config, zb_radio_spinel_init_uart, zb_radio_spinel_deinit_uart,
                                               ESP_RADIO_SPINEL_ZIGBEE) != ESP_OK) {
        ESP_LOGE(TAG, "Spinel UART interface enable failed");
    } else {
        ESP_LOGI(TAG, "Spinel UART interface enable successfully");
    }
    return ESP_OK;
}
