/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#include <stdio.h>
#include <string.h>
#include "sdkconfig.h"
#include "esp_log.h"
#include "esp_console.h"

void register_pppd(void);

void app_main(void)
{
    esp_console_repl_t *repl = NULL;
    esp_console_repl_config_t repl_config = ESP_CONSOLE_REPL_CONFIG_DEFAULT();
    esp_console_dev_uart_config_t uart_config = ESP_CONSOLE_DEV_UART_CONFIG_DEFAULT();
    repl_config.prompt = "iperf>";
    // init console REPL environment
    ESP_ERROR_CHECK(esp_console_new_repl_uart(&uart_config, &repl_config, &repl));

    /* Register commands */
    register_pppd();

    printf("\n =======================================================\n");
    printf(" |       Steps to Test PPP Client Bandwidth            |\n");
    printf(" |                                                     |\n");
    printf(" |  1. Enter 'help', check all supported commands      |\n");
    printf(" |  2. Start PPP server on host system                 |\n");
    printf(" |     - pppd /dev/ttyUSB1 115200 192.168.11.1:192.168.11.2 modem local noauth debug nocrtscts nodetach +ipv6\n");
    printf(" |  3. Wait ESP32 to get IP from PPP server            |\n");
    printf(" |  4. Enter 'pppd info' (optional)                    |\n");
    printf(" |  5. Server: 'iperf -u -s -i 3'                      |\n");
    printf(" |  6. Client: 'iperf -u -c SERVER_IP -t 60 -i 3'      |\n");
    printf(" |                                                     |\n");
    printf(" =======================================================\n\n");

    // start console REPL
    ESP_ERROR_CHECK(esp_console_start_repl(repl));
}
