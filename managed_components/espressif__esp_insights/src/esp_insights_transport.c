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
#include <stdbool.h>
#include <esp_insights.h>

static const char *TAG = "insights_transport";

typedef struct {
    bool init;
    esp_insights_transport_config_t config;
} priv_data_t;

static priv_data_t s_priv_data;

#define CHECK_TRANSPORT_INIT(ret) do {                       \
    if (!s_priv_data.init) {                                 \
        ESP_LOGE(TAG, "Transport callbacks not registered"); \
        return ret;                                          \
    }                                                        \
} while (0)

esp_err_t esp_insights_transport_register(esp_insights_transport_config_t *config)
{
    if (s_priv_data.init) {
        ESP_LOGW(TAG, "Already initialized");
        return ESP_ERR_INVALID_STATE;
    }
    /* We at least need data_send callback */
   if (!(config && config->callbacks.data_send)) {
        ESP_LOGE(TAG, "Failed to init transport, Please set at least data_send callback");
        return ESP_ERR_INVALID_ARG;
    }
    memcpy(&s_priv_data.config, config, sizeof(s_priv_data.config));
    s_priv_data.init = true;

    /* Call transport init */
    if (s_priv_data.config.callbacks.init) {
        s_priv_data.config.callbacks.init(config->userdata);
    }
    return ESP_OK;
}

void esp_insights_transport_unregister(void)
{
    if (!s_priv_data.init) {
        ESP_LOGE(TAG, "Not initialized");
        return;
    }
    /* Call transport deinit */
    if (s_priv_data.config.callbacks.deinit) {
        s_priv_data.config.callbacks.deinit();
    }
    memset(&s_priv_data, 0, sizeof(s_priv_data));
}

esp_err_t esp_insights_transport_connect(void)
{
    CHECK_TRANSPORT_INIT(ESP_ERR_INVALID_STATE);
    if (s_priv_data.config.callbacks.connect) {
        return s_priv_data.config.callbacks.connect();
    }
    ESP_LOGW(TAG, "connect callback not set");
    return ESP_OK;
}

void esp_insights_transport_disconnect(void)
{
    CHECK_TRANSPORT_INIT();
    if (s_priv_data.config.callbacks.disconnect) {
        return s_priv_data.config.callbacks.disconnect();
    }
    ESP_LOGW(TAG, "disconnect callback not set");
}

int esp_insights_transport_data_send(void *data, size_t len)
{
    CHECK_TRANSPORT_INIT(-1);
    if (s_priv_data.config.callbacks.data_send) {
        return s_priv_data.config.callbacks.data_send(data, len);
    }
    ESP_LOGW(TAG, "data send callback not set");
    return -1;
}
