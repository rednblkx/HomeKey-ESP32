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
#include <esp_log.h>
#include <esp_rmaker_mqtt_glue.h>
#include <esp_rmaker_factory.h>
#include <esp_insights_client_data.h>
#include <esp_rmaker_common_events.h>
#include <esp_insights.h>
#include <esp_insights_internal.h>

static const char *TAG = "tport_mqtt";

#define INSIGHTS_TOPIC_SUFFIX       "diagnostics/from-node"

typedef struct {
    const char *node_id;
    esp_rmaker_mqtt_config_t mqtt_config;
    esp_rmaker_mqtt_conn_params_t *conn_params;
    bool connected;
} mqtt_data_t;

static mqtt_data_t s_mqtt_data;

static esp_rmaker_mqtt_conn_params_t *get_mqtt_conn_params(void)
{
    if (s_mqtt_data.mqtt_config.get_conn_params) {
        return s_mqtt_data.mqtt_config.get_conn_params();
    } else {
        return esp_insights_get_mqtt_conn_params();
    }
}

static void clean_mqtt_conn_params(void)
{
    if (s_mqtt_data.conn_params) {
        esp_insights_clean_mqtt_conn_params(s_mqtt_data.conn_params);
        free(s_mqtt_data.conn_params);
        s_mqtt_data.conn_params = NULL;
    }
}

static void rmaker_common_event_handler(void* arg, esp_event_base_t event_base,
                                        int32_t event_id, void* event_data)
{
    if (event_base != RMAKER_COMMON_EVENT) {
        return;
    }
    esp_insights_transport_event_data_t data;
    switch(event_id) {
        case RMAKER_MQTT_EVENT_DISCONNECTED:
            s_mqtt_data.connected = false;
            break;
        case RMAKER_MQTT_EVENT_CONNECTED:
            s_mqtt_data.connected = true;
            break;
        case RMAKER_MQTT_EVENT_PUBLISHED:
            memset(&data, 0, sizeof(data));
            data.msg_id = *(int *)event_data;
            esp_event_post(INSIGHTS_EVENT, INSIGHTS_EVENT_TRANSPORT_SEND_SUCCESS, &data, sizeof(data), portMAX_DELAY);
            break;
#ifdef CONFIG_MQTT_REPORT_DELETED_MESSAGES
        case RMAKER_MQTT_EVENT_MSG_DELETED:
            memset(&data, 0, sizeof(data));
            data.msg_id = *(int *)event_data;
            esp_event_post(INSIGHTS_EVENT, INSIGHTS_EVENT_TRANSPORT_SEND_FAILED, &data, sizeof(data), portMAX_DELAY);
            break;
#endif /* CONFIG_MQTT_REPORT_DELETED_MESSAGES */
        default:
            break;
    }
}

static esp_err_t esp_insights_mqtt_init(void *userdata)
{
    if (!s_mqtt_data.mqtt_config.setup_done) {
        esp_rmaker_mqtt_glue_setup(&s_mqtt_data.mqtt_config);
    }
    esp_err_t err = esp_event_handler_register(RMAKER_COMMON_EVENT, ESP_EVENT_ANY_ID, rmaker_common_event_handler, NULL);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to register event handler for RMAKER_COMMON_EVENT");
        return err;
    }
    err = esp_rmaker_factory_init();
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize factory partition");
        return err;
    }
    s_mqtt_data.conn_params = get_mqtt_conn_params();
    if (!s_mqtt_data.conn_params) {
        ESP_LOGE(TAG, "Failed to get MQTT connection parameters.");
        err = ESP_FAIL;
        goto init_err;
    }
    s_mqtt_data.node_id = esp_insights_get_node_id();
    if (!s_mqtt_data.node_id) {
        ESP_LOGE(TAG, "Failed to get node id");
        err = ESP_FAIL;
        goto init_err;
    }
    if (!s_mqtt_data.mqtt_config.init) {
        ESP_LOGW(TAG, "esp_insights_mqtt_init not registered");
        return ESP_OK;
    }
    err = s_mqtt_data.mqtt_config.init(s_mqtt_data.conn_params);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize MQTT connection");
        goto init_err;
    }
    return ESP_OK;
init_err:
    clean_mqtt_conn_params();
    esp_event_handler_unregister(RMAKER_COMMON_EVENT, ESP_EVENT_ANY_ID, rmaker_common_event_handler);
    return err;
}

static void esp_insights_mqtt_deinit(void)
{
    if (s_mqtt_data.mqtt_config.deinit) {
        s_mqtt_data.mqtt_config.deinit();
    }
    if (s_mqtt_data.node_id) {
        s_mqtt_data.node_id = NULL;
    }
    clean_mqtt_conn_params();
    esp_event_handler_unregister(RMAKER_COMMON_EVENT, ESP_EVENT_ANY_ID, rmaker_common_event_handler);
}

static esp_err_t esp_insights_mqtt_connect(void)
{
    if (s_mqtt_data.mqtt_config.connect) {
        return s_mqtt_data.mqtt_config.connect();
    }
    ESP_LOGW(TAG, "esp_insights_mqtt_connect not registered");
    return ESP_ERR_NOT_FOUND;
}

static void esp_insights_mqtt_disconnect(void)
{
    if (s_mqtt_data.mqtt_config.disconnect) {
        s_mqtt_data.mqtt_config.disconnect();
    } else {
        ESP_LOGW(TAG, "esp_insights_mqtt_disconnect not registered");
    }
}

static esp_err_t esp_insights_mqtt_publish(const char *topic, void *data, size_t data_len, uint8_t qos, int *msg_id)
{
    if (s_mqtt_data.mqtt_config.publish) {
        return s_mqtt_data.mqtt_config.publish(topic, data, data_len, qos, msg_id);
    }
    ESP_LOGW(TAG, "esp_insights_mqtt_publish not registered");
    return ESP_ERR_NOT_FOUND;
}

static int esp_insights_mqtt_data_send(void *data, size_t len)
{
    char topic[128];
    int msg_id = -1;
    if (!data) {
        return 0;
    }
    if (!s_mqtt_data.node_id) {
        return -1;
    }
	snprintf(topic, sizeof(topic), "node/%s/%s", s_mqtt_data.node_id, INSIGHTS_TOPIC_SUFFIX);
	esp_insights_mqtt_publish(topic, data, len, 1, &msg_id);
    return msg_id;
}

esp_insights_transport_config_t g_default_insights_transport_mqtt = {
    .callbacks = {
        .init = esp_insights_mqtt_init,
        .deinit = esp_insights_mqtt_deinit,
        .connect = esp_insights_mqtt_connect,
        .disconnect = esp_insights_mqtt_disconnect,
        .data_send = esp_insights_mqtt_data_send,
    }
};
