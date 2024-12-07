/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "driver/uart.h"
#include "esp_modem_dce_config.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup ESP_MODEM_CONFIG
 * @brief Configuration structures for DTE and DCE
 */

/** @addtogroup ESP_MODEM_CONFIG
 * @{
 */

/**
 * @brief Modem flow control type
 *
 */
typedef enum {
    ESP_MODEM_FLOW_CONTROL_NONE = 0,
    ESP_MODEM_FLOW_CONTROL_SW,
    ESP_MODEM_FLOW_CONTROL_HW
} esp_modem_flow_ctrl_t;

/**
 * @brief UART configuration structure
 *
 */
struct esp_modem_uart_term_config {
    uart_port_t port_num;           /*!< UART port number */
    uart_word_length_t data_bits;   /*!< Data bits of UART */
    uart_stop_bits_t stop_bits;     /*!< Stop bits of UART */
    uart_parity_t parity;           /*!< Parity type */
    esp_modem_flow_ctrl_t flow_control; /*!< Flow control type */
    uart_sclk_t source_clk;         /*!< UART source clock selection */
    int baud_rate;                  /*!< Communication baud rate */
    int tx_io_num;                  /*!< TXD Pin Number */
    int rx_io_num;                  /*!< RXD Pin Number */
    int rts_io_num;                 /*!< RTS Pin Number */
    int cts_io_num;                 /*!< CTS Pin Number */
    int rx_buffer_size;             /*!< UART RX Buffer Size */
    int tx_buffer_size;             /*!< UART TX Buffer Size */
    int event_queue_size;           /*!< UART Event Queue Size, set to 0 if no event queue needed */
};

// Forward declare the resource struct
struct esp_modem_vfs_resource;

/**
 * @brief VFS configuration structure
 *
 */
struct esp_modem_vfs_term_config {
    int fd;                                     /*!< Already created file descriptor */
    void (*deleter)(int, struct esp_modem_vfs_resource *);            /*!< Custom close function for the fd */
    struct esp_modem_vfs_resource *resource;    /*!< Resource attached to the VFS (need for clenaup) */
};

/**
 * @brief Complete DTE configuration structure
 *
 * Note that the generic part is common for DTE and its SW resources
 * The following portions for config is dedicated to the chosen HW resource
 * used as a communication terminal for this DTE
 */
struct esp_modem_dte_config {
    size_t dte_buffer_size;                             /*!< DTE buffer size */
    uint32_t task_stack_size;                           /*!< Terminal task stack size */
    unsigned task_priority;                             /*!< Terminal task priority */
    union {
        struct esp_modem_uart_term_config uart_config;      /*!< Configuration for UART Terminal */
        struct esp_modem_vfs_term_config vfs_config;        /*!< Configuration for VFS Terminal */
        void *extension_config;                             /*!< Configuration for app specific Terminal */
    };
};

#if ESP_IDF_VERSION_MAJOR >= 5
#define ESP_MODEM_DEFAULT_UART_CLK      UART_SCLK_DEFAULT
#else
#define ESP_MODEM_DEFAULT_UART_CLK      UART_SCLK_APB
#endif

/**
 * @brief ESP Modem DTE Default Configuration
 *
 */
#define ESP_MODEM_DTE_DEFAULT_CONFIG() \
    {                                  \
        .dte_buffer_size = 512,        \
        .task_stack_size = 4096,       \
        .task_priority = 5,            \
        .uart_config = {               \
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
            .event_queue_size = 30,                 \
       },                                           \
    }

typedef struct esp_modem_dte_config esp_modem_dte_config_t;

/**
 * @}
 */

#ifdef __cplusplus
}
#endif
