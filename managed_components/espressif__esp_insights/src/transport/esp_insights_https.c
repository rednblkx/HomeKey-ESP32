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
#include <esp_err.h>
#include <esp_log.h>
#include <esp_event.h>
#include <esp_http_client.h>
#include <esp_insights.h>
#include <esp_insights_internal.h>

typedef struct {
    const char *auth_key;
    const char *node_id;
    const char *url;
} https_data_t;

static https_data_t s_https_data;

static const char *TAG = "tport_https";
#define HTTPS_URL_PATH "prod/node/data"

extern const uint8_t insights_https_server_crt_start[] asm("_binary_https_server_crt_start");
extern const uint8_t insights_https_server_crt_end[] asm("_binary_https_server_crt_end");

static esp_err_t esp_insights_https_init(void *userdata)
{
    if (sizeof(CONFIG_ESP_INSIGHTS_TRANSPORT_HTTPS_HOST) <= 1) {
        ESP_LOGE(TAG, "Insights HTTPS host is missing");
        return ESP_ERR_INVALID_ARG;
    }
    s_https_data.url = CONFIG_ESP_INSIGHTS_TRANSPORT_HTTPS_HOST "/" HTTPS_URL_PATH;
    /* In default https transport, auth_key is sent as userdata */
    s_https_data.auth_key = (const char *)userdata;
    s_https_data.node_id = esp_insights_get_node_id();
    return ESP_OK;
}

static void esp_insights_https_deinit(void)
{
    memset(&s_https_data, 0, sizeof(s_https_data));
}

static esp_err_t http_event_handle(esp_http_client_event_t *evt)
{
    switch(evt->event_id) {
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
            ESP_LOGD(TAG, "HTTP_EVENT_ON_HEADER %s:%s", evt->header_key, evt->header_value);
            break;
        case HTTP_EVENT_ON_DATA:
            ESP_LOGD(TAG, "HTTP_EVENT_ON_DATA, len=%d", evt->data_len);
            if (!esp_http_client_is_chunked_response(evt->client)) {
                ESP_LOGD(TAG, "%.*s\n", evt->data_len, (char*)evt->data);
            }
            break;
        case HTTP_EVENT_ON_FINISH:
            ESP_LOGD(TAG, "HTTP_EVENT_ON_FINISH");
            break;
        case HTTP_EVENT_DISCONNECTED:
            ESP_LOGD(TAG, "HTTP_EVENT_DISCONNECTED");
            break;
        default:
            break;
    }
    return ESP_OK;
}

static int esp_insights_https_data_send(void *data, size_t len)
{
    if (!data) {
        return ESP_ERR_INVALID_ARG;
    }
    if (!s_https_data.auth_key) {
        ESP_LOGE(TAG, "Transport not initialized");
        return ESP_ERR_INVALID_STATE;
    }

    int msg_id = -1;
    char url[256];
    memset(url, 0, sizeof(url));
    snprintf(url, sizeof(url), "%s?node_id=%s", s_https_data.url, s_https_data.node_id);

    esp_http_client_config_t client_config = {
        .url = url,
        .method = HTTP_METHOD_POST,
        .transport_type = HTTP_TRANSPORT_OVER_SSL,
        .buffer_size_tx = 1024,
        .event_handler = http_event_handle,
        .cert_pem = (const char *)insights_https_server_crt_start,
    };
    esp_http_client_handle_t client = esp_http_client_init(&client_config);
    if (!client) {
        ESP_LOGE(TAG, "Failed to initialize esp_http_client");
        return msg_id;
    }
    esp_err_t err = esp_http_client_set_header(client, "Authorization", s_https_data.auth_key);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to set Authorization header err:0x%x", err);
        goto cleanup;
    }
    err = esp_http_client_set_header(client, "Content-type", "application/octet-stream");
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to set content type err:0x%x", err);
        goto cleanup;
    }
    err = esp_http_client_set_post_field(client, data, len);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "esp_http_client_set_post_field failed err:0x%x", err);
        goto cleanup;
    }
    err = esp_http_client_perform(client);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "esp_http_client_perform failed err:0x%x", err);
    } else {
        int status = esp_http_client_get_status_code(client);
        if (status == HttpStatus_Ok) {
            msg_id = 0;
        } else {
            ESP_LOGE(TAG, "API response status = %d", status);
        }
    }
    if (msg_id == 0) {
        esp_event_post(INSIGHTS_EVENT, INSIGHTS_EVENT_TRANSPORT_SEND_SUCCESS, NULL, 0, portMAX_DELAY);
    } else {
        esp_event_post(INSIGHTS_EVENT, INSIGHTS_EVENT_TRANSPORT_SEND_FAILED, NULL, 0, portMAX_DELAY);
    }
cleanup:
    esp_http_client_cleanup(client);
    return msg_id;
}

esp_insights_transport_config_t g_default_insights_transport_https = {
    .callbacks = {
        .init = esp_insights_https_init,
        .deinit = esp_insights_https_deinit,
        .data_send = esp_insights_https_data_send,
    }
};
