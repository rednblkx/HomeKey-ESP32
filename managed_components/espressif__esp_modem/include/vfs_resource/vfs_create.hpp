/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#define ESP_MODEM_VFS_DEFAULT_UART_CONFIG(name)  {  \
        .dev_name = (name), \
        .uart = {               \
            .port_num = UART_NUM_1,                 \
            .data_bits = UART_DATA_8_BITS,          \
            .stop_bits = UART_STOP_BITS_1,          \
            .parity = UART_PARITY_DISABLE,          \
            .flow_control = ESP_MODEM_FLOW_CONTROL_NONE,\
            .source_clk = ESP_MODEM_DEFAULT_UART_CLK,   \
            .baud_rate = 115200,                    \
            .tx_io_num = 25,                        \
            .rx_io_num = 26,                        \
            .rts_io_num = 27,                       \
            .cts_io_num = 23,                       \
            .rx_buffer_size = 4096,                 \
            .tx_buffer_size = 512,                  \
            .event_queue_size = 0,                  \
       },                                           \
}

/**
 * @brief UART init struct for VFS
 */
struct esp_modem_vfs_uart_creator {
    const char *dev_name;                       /*!< VFS device name, e.g. /dev/uart/n */
    struct esp_modem_uart_term_config uart;     /*!< UART driver init struct */
};

/**
 * @brief UART init struct for VFS
 */
struct esp_modem_vfs_socket_creator {
    const char *host_name;                    /*!< VFS socket: host name (or IP address) */
    int port;                                 /*!< VFS socket: port number */
};

/**
 * @brief Creates a socket VFS and configures the DTE struct
 *
 * @param config Socket config option, basically host + port
 * @param created_config reference to the VFS portion of the DTE config to be set up
 * @return true on success
 */
bool vfs_create_socket(struct esp_modem_vfs_socket_creator *config, struct esp_modem_vfs_term_config *created_config);

/**
 * @brief Creates a uart VFS and configures the DTE struct
 *
 * @param config Uart config option, basically file name and console options
 * @param created_config reference to the VFS portion of the DTE config to be set up
 * @return true on success
 */
bool vfs_create_uart(struct esp_modem_vfs_uart_creator *config, struct esp_modem_vfs_term_config *created_config);
