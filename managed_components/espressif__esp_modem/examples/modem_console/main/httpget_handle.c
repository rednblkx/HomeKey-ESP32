/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

/*
 * Modem console example
 *
*/

#include <stdio.h>
#include <string.h>
#include "sdkconfig.h"
#include "esp_console.h"
#include "argtable3/argtable3.h"
#include "esp_log.h"
#include "esp_http_client.h"

static const char *TAG = "modem_console_httpget";

static esp_err_t http_event_handler(esp_http_client_event_t *evt)
{
    switch (evt->event_id) {
    case HTTP_EVENT_ERROR:
        ESP_LOGD(TAG, "HTTP_EVENT_ERROR");
        break;
    case HTTP_EVENT_ON_CONNECTED:
        ESP_LOGD(TAG, "HTTP_EVENT_ON_CONNECTED");
        break;
    case HTTP_EVENT_HEADER_SENT:
        ESP_LOGD(TAG, "HTTP_EVENT_HEADER_SENT");
        break;
    case HTTP_EVENT_ON_HEADER:
        ESP_LOGD(TAG, "HTTP_EVENT_ON_HEADER, key=%s, value=%s", evt->header_key, evt->header_value);
        break;
    case HTTP_EVENT_ON_DATA:
        ESP_LOGD(TAG, "HTTP_EVENT_ON_DATA, len=%d", evt->data_len);
        if ((bool)evt->user_data &&
                !esp_http_client_is_chunked_response(evt->client)) {
            ESP_LOG_BUFFER_HEXDUMP(TAG, evt->data, evt->data_len, ESP_LOG_INFO);
        }

        break;
    case HTTP_EVENT_ON_FINISH:
        ESP_LOGD(TAG, "HTTP_EVENT_ON_FINISH");
        break;
    case HTTP_EVENT_DISCONNECTED:
        ESP_LOGI(TAG, "HTTP_EVENT_DISCONNECTED");
        break;
    default: break;
    }
    return ESP_OK;
}

static struct {
    struct arg_str *host;
    struct arg_lit *hex;
    struct arg_end *end;
} http_args;

static int do_http_client(int argc, char **argv)
{
    int nerrors = arg_parse(argc, argv, (void **)&http_args);
    if (nerrors != 0) {
        arg_print_errors(stderr, http_args.end, argv[0]);
        return 1;
    }
    esp_http_client_config_t config = {
        .event_handler = http_event_handler,
    };

    if (http_args.host->count > 0) {
        config.url = http_args.host->sval[0];
    } else {
        config.url = "http://httpbin.org/get";
    }

    if (http_args.hex->count > 0) {
        // show hex data from http-get
        config.user_data = (void *)true;
    }


    esp_http_client_handle_t client = esp_http_client_init(&config);

    esp_err_t err = esp_http_client_perform(client);
    if (err == ESP_OK) {
        uint64_t content_length = esp_http_client_get_content_length(client);
        ESP_LOGI(TAG, "HTTP GET Status = %d, content_length = %lld",
                 esp_http_client_get_status_code(client), content_length);
        return 0;
    }
    ESP_LOGE(TAG, "HTTP GET request failed: %s", esp_err_to_name(err));
    return 1;
}

void modem_console_register_http(void)
{
    http_args.host = arg_str0(NULL, NULL, "<host>", "address or host-name to send GET request (defaults to http://httpbin.org/get)");
    http_args.hex = arg_litn("p", "print-hex", 0, 1, "print hex output"),
    http_args.end = arg_end(1);
    const esp_console_cmd_t http_cmd = {
        .command = "httpget",
        .help = "http get command to test data mode",
        .hint = NULL,
        .func = &do_http_client,
        .argtable = &http_args
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&http_cmd));
}
