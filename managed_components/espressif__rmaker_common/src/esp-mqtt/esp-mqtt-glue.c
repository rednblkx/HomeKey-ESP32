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

#include <stdio.h>
#include <string.h>
#include <sdkconfig.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_log.h>
#include <mqtt_client.h>
#include <esp_event.h>
#include <esp_rmaker_common_events.h>
#include <esp_rmaker_mqtt_glue.h>
#include <esp_idf_version.h>
#include <esp_rmaker_utils.h>
#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(4, 1, 0)
// Features supported in 4.1+

#ifdef CONFIG_ESP_RMAKER_MQTT_PORT_443
#define ESP_RMAKER_MQTT_USE_PORT_443
#endif

#else

#ifdef CONFIG_ESP_RMAKER_MQTT_PORT_443
#warning "Certificate Bundle not supported below IDF v4.4. Using provided certificate instead."
#endif

#endif /* !IDF4.1 */

#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(4, 4, 0)
// Features supported in 4.4+

#ifdef CONFIG_ESP_RMAKER_MQTT_USE_CERT_BUNDLE
#define ESP_RMAKER_MQTT_USE_CERT_BUNDLE
#include <esp_crt_bundle.h>
#endif

#else

#ifdef CONFIG_ESP_RMAKER_MQTT_USE_CERT_BUNDLE
#warning "Certificate Bundle not supported below 4.4. Using provided certificate instead."
#endif

#endif /* !IDF4.4 */

static const char *TAG = "esp_mqtt_glue";

#define MAX_MQTT_SUBSCRIPTIONS      CONFIG_ESP_RMAKER_MAX_MQTT_SUBSCRIPTIONS

typedef struct {
    char *topic;
    esp_rmaker_mqtt_subscribe_cb_t cb;
    void *priv;
} esp_mqtt_glue_subscription_t;

typedef struct {
    esp_mqtt_client_handle_t mqtt_client;
    esp_rmaker_mqtt_conn_params_t *conn_params;
    esp_mqtt_glue_subscription_t *subscriptions[MAX_MQTT_SUBSCRIPTIONS];
} esp_mqtt_glue_data_t;
esp_mqtt_glue_data_t *mqtt_data;

typedef struct {
    char *data;
    char *topic;
} esp_mqtt_glue_long_data_t;

static void esp_mqtt_glue_deinit(void);

static void esp_mqtt_glue_subscribe_callback(const char *topic, int topic_len, const char *data, int data_len)
{
    esp_mqtt_glue_subscription_t **subscriptions = mqtt_data->subscriptions;
    int i;
    for (i = 0; i < MAX_MQTT_SUBSCRIPTIONS; i++) {
        if (subscriptions[i]) {
            if ((strncmp(topic, subscriptions[i]->topic, topic_len) == 0)
                    && (topic_len == strlen(subscriptions[i]->topic))) {
                subscriptions[i]->cb(subscriptions[i]->topic, (void *)data, data_len, subscriptions[i]->priv);
            }
        }
    }
}

static esp_err_t esp_mqtt_glue_subscribe(const char *topic, esp_rmaker_mqtt_subscribe_cb_t cb, uint8_t qos, void *priv_data)
{
    if ( !mqtt_data || !topic || !cb) {
        return ESP_FAIL;
    }
    int i;
    for (i = 0; i < MAX_MQTT_SUBSCRIPTIONS; i++) {
        if (!mqtt_data->subscriptions[i]) {
            esp_mqtt_glue_subscription_t *subscription = calloc(1, sizeof(esp_mqtt_glue_subscription_t));
            if (!subscription) {
                return ESP_FAIL;
            }
            subscription->topic = strdup(topic);
            if (!subscription->topic) {
                free(subscription);
                return ESP_FAIL;
            }
            int ret = esp_mqtt_client_subscribe(mqtt_data->mqtt_client, subscription->topic, qos);
            if (ret < 0) {
                free(subscription->topic);
                free(subscription);
                return ESP_FAIL;
            }
            subscription->priv = priv_data;
            subscription->cb = cb;
            mqtt_data->subscriptions[i] = subscription;
            ESP_LOGD(TAG, "Subscribed to topic: %s", topic);
            return ESP_OK;
        }
    }
    return ESP_FAIL;
}

static void unsubscribe_helper(esp_mqtt_glue_subscription_t **subscription)
{
    if (subscription && *subscription) {
        if (esp_mqtt_client_unsubscribe(mqtt_data->mqtt_client, (*subscription)->topic) < 0) {
            ESP_LOGW(TAG, "Could not unsubscribe from topic: %s", (*subscription)->topic);
        }
        free((*subscription)->topic);
        free(*subscription);
        *subscription = NULL;
    }
}

static esp_err_t esp_mqtt_glue_unsubscribe(const char *topic)
{
    if (!mqtt_data || !topic) {
        return ESP_FAIL;
    }
    esp_mqtt_glue_subscription_t **subscriptions = mqtt_data->subscriptions;
    int i;
    for (i = 0; i < MAX_MQTT_SUBSCRIPTIONS; i++) {
        if (subscriptions[i]) {
            if (strncmp(topic, subscriptions[i]->topic, strlen(topic)) == 0) {
                unsubscribe_helper(&subscriptions[i]);
                return ESP_OK;
            }
        }
    }
    return ESP_FAIL;
}

static esp_err_t esp_mqtt_glue_publish(const char *topic, void *data, size_t data_len, uint8_t qos, int *msg_id)
{
    if (!mqtt_data || !topic || !data) {
        return ESP_FAIL;
    }
    ESP_LOGD(TAG, "Publishing to %s", topic);
    int ret = esp_mqtt_client_publish(mqtt_data->mqtt_client, topic, data, data_len, qos, 0);
    if (ret < 0) {
        ESP_LOGE(TAG, "MQTT Publish failed");
        return ESP_FAIL;
    }
    if (msg_id) {
        *msg_id = ret;
    }
    return ESP_OK;
}

static esp_mqtt_glue_long_data_t *esp_mqtt_glue_free_long_data(esp_mqtt_glue_long_data_t *long_data)
{
    if (long_data) {
        if (long_data->topic) {
            free(long_data->topic);
        }
        if (long_data->data) {
            free(long_data->data);
        }
        free(long_data);
    }
    return NULL;
}

static esp_mqtt_glue_long_data_t *esp_mqtt_glue_manage_long_data(esp_mqtt_glue_long_data_t *long_data,
        esp_mqtt_event_handle_t event)
{
    if (event->topic) {
        /* This is new data. Free any earlier data, if present. */
        esp_mqtt_glue_free_long_data(long_data);
        long_data = calloc(1, sizeof(esp_mqtt_glue_long_data_t));
        if (!long_data) {
            ESP_LOGE(TAG, "Could not allocate memory for esp_mqtt_glue_long_data_t");
            return NULL;
        }
        long_data->data = MEM_CALLOC_EXTRAM(1, event->total_data_len);
        if (!long_data->data) {
            ESP_LOGE(TAG, "Could not allocate %d bytes for received data.", event->total_data_len);
            return esp_mqtt_glue_free_long_data(long_data);
        }
        long_data->topic = strndup(event->topic, event->topic_len);
        if (!long_data->topic) {
            ESP_LOGE(TAG, "Could not allocate %d bytes for received topic.", event->topic_len);
            return esp_mqtt_glue_free_long_data(long_data);
        }
    }
    if (long_data) {
        memcpy(long_data->data + event->current_data_offset, event->data, event->data_len);

        if ((event->current_data_offset + event->data_len) == event->total_data_len) {
            esp_mqtt_glue_subscribe_callback(long_data->topic, strlen(long_data->topic),
                        long_data->data, event->total_data_len);
            return esp_mqtt_glue_free_long_data(long_data);
        }
    }
    return long_data;
}

#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(5, 0, 0)
static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
#else
static esp_err_t mqtt_event_handler(esp_mqtt_event_handle_t event)
#endif
{
#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(5, 0, 0)
    esp_mqtt_event_handle_t event = event_data;
#else
    uint32_t event_id = event->event_id;
#endif

    switch (event_id) {
        case MQTT_EVENT_CONNECTED:
            ESP_LOGI(TAG, "MQTT Connected");
            /* Resubscribe to all topics after reconnection */
            for (int i = 0; i < MAX_MQTT_SUBSCRIPTIONS; i++) {
                if (mqtt_data->subscriptions[i]) {
                    esp_mqtt_client_subscribe(event->client, mqtt_data->subscriptions[i]->topic, 1);
                }
            }
            esp_event_post(RMAKER_COMMON_EVENT, RMAKER_MQTT_EVENT_CONNECTED, NULL, 0, portMAX_DELAY);
            break;
        case MQTT_EVENT_DISCONNECTED:
            ESP_LOGW(TAG, "MQTT Disconnected. Will try reconnecting in a while...");
            esp_event_post(RMAKER_COMMON_EVENT, RMAKER_MQTT_EVENT_DISCONNECTED, NULL, 0, portMAX_DELAY);
            break;

        case MQTT_EVENT_SUBSCRIBED:
            ESP_LOGD(TAG, "MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);
            break;
        case MQTT_EVENT_UNSUBSCRIBED:
            ESP_LOGD(TAG, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
            break;
        case MQTT_EVENT_PUBLISHED:
            ESP_LOGD(TAG, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
            esp_event_post(RMAKER_COMMON_EVENT, RMAKER_MQTT_EVENT_PUBLISHED, &event->msg_id, sizeof(event->msg_id), portMAX_DELAY);
            break;
#ifdef CONFIG_MQTT_REPORT_DELETED_MESSAGES
        case MQTT_EVENT_DELETED:
            ESP_LOGD(TAG, "MQTT_EVENT_DELETED, msg_id=%d", event->msg_id);
            esp_event_post(RMAKER_COMMON_EVENT, RMAKER_MQTT_EVENT_MSG_DELETED, &event->msg_id, sizeof(event->msg_id), portMAX_DELAY);
            break;
#endif /* CONFIG_MQTT_REPORT_DELETED_MESSAGES */
        case MQTT_EVENT_DATA: {
            ESP_LOGD(TAG, "MQTT_EVENT_DATA");
            static esp_mqtt_glue_long_data_t *long_data;
            /* Topic can be NULL, for data longer than the MQTT buffer */
            if (event->topic) {
                ESP_LOGD(TAG, "TOPIC=%.*s\r\n", event->topic_len, event->topic);
            }
            ESP_LOGD(TAG, "DATA=%.*s\r\n", event->data_len, event->data);
            if (event->data_len == event->total_data_len) {
                /* If long_data still exists, it means there was some issue getting the
                 * long data, and so, it needs to be freed up.
                 */
                if (long_data) {
                    long_data = esp_mqtt_glue_free_long_data(long_data);
                }
                esp_mqtt_glue_subscribe_callback(event->topic, event->topic_len, event->data, event->data_len);
            } else {
                long_data = esp_mqtt_glue_manage_long_data(long_data, event);
            }
            break;
        }
        case MQTT_EVENT_ERROR:
            ESP_LOGE(TAG, "MQTT_EVENT_ERROR");
            break;
        default:
            ESP_LOGD(TAG, "Other event id:%d", event->event_id);
            break;
    }
#if ESP_IDF_VERSION < ESP_IDF_VERSION_VAL(5, 0, 0)
    return ESP_OK;
#endif
}

static esp_err_t esp_mqtt_glue_connect(void)
{
    if (!mqtt_data) {
        return ESP_FAIL;
    }
    ESP_LOGI(TAG, "Connecting to %s", mqtt_data->conn_params->mqtt_host);
    esp_err_t ret = esp_mqtt_client_start(mqtt_data->mqtt_client);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "esp_mqtt_client_start() failed with err = %d", ret);
        return ret;
    }
    return ESP_OK;
}

static void esp_mqtt_glue_unsubscribe_all(void)
{
    if (!mqtt_data) {
        return;
    }
    int i;
    for (i = 0; i < MAX_MQTT_SUBSCRIPTIONS; i++) {
        if (mqtt_data->subscriptions[i]) {
            unsubscribe_helper(&(mqtt_data->subscriptions[i]));
        }
    }
}

static esp_err_t esp_mqtt_glue_disconnect(void)
{
    if (!mqtt_data) {
        return ESP_FAIL;
    }
    esp_mqtt_glue_unsubscribe_all();
    esp_err_t err = esp_mqtt_client_stop(mqtt_data->mqtt_client);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to disconnect from MQTT");
    } else {
        ESP_LOGI(TAG, "MQTT Disconnected.");
    }
    return err;
}
#ifdef ESP_RMAKER_MQTT_USE_PORT_443
static const char *alpn_protocols[] = { "x-amzn-mqtt-ca", NULL };
#endif /* ESP_RMAKER_MQTT_USE_PORT_443 */
static esp_err_t esp_mqtt_glue_init(esp_rmaker_mqtt_conn_params_t *conn_params)
{
#ifdef CONFIG_ESP_RMAKER_MQTT_SEND_USERNAME
    const char *username = esp_get_aws_ppi();
    ESP_LOGI(TAG, "AWS PPI: %s", username);
#endif
    if (mqtt_data) {
        ESP_LOGE(TAG, "MQTT already initialized");
        return ESP_OK;
    }
    if (!conn_params) {
        ESP_LOGE(TAG, "Connection params are mandatory for esp_mqtt_glue_init");
        return ESP_FAIL;
    }
    ESP_LOGI(TAG, "Initialising MQTT");
    mqtt_data = calloc(1, sizeof(esp_mqtt_glue_data_t));
    if (!mqtt_data) {
        ESP_LOGE(TAG, "Failed to allocate memory for esp_mqtt_glue_data_t");
        return ESP_ERR_NO_MEM;
    }
    mqtt_data->conn_params = conn_params;

#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(5, 0, 0)
    const esp_mqtt_client_config_t mqtt_client_cfg = {
        .broker = {
            .address = {
                .hostname = conn_params->mqtt_host,
#ifdef ESP_RMAKER_MQTT_USE_PORT_443
                .port = 443,
#else
                .port = 8883,
#endif
                .transport = MQTT_TRANSPORT_OVER_SSL,
            },
            .verification = {
#ifdef ESP_RMAKER_MQTT_USE_PORT_443
                .alpn_protos = alpn_protocols,
#endif
#ifdef ESP_RMAKER_MQTT_USE_CERT_BUNDLE
                .crt_bundle_attach = esp_crt_bundle_attach,
#else
                .certificate = (const char *)conn_params->server_cert,
                .certificate_len = conn_params->server_cert_len,
#endif
            }
        },
        .credentials = {
#ifdef CONFIG_ESP_RMAKER_MQTT_SEND_USERNAME
            .username = username,
#endif
            .client_id = (const char *)conn_params->client_id,
            .authentication = {
                .certificate = (const char *)conn_params->client_cert,
                .certificate_len = conn_params->client_cert_len,
                .key = (const char *)conn_params->client_key,
                .key_len = conn_params->client_key_len,
                .ds_data = conn_params->ds_data
            },
        },
        .session = {
            .keepalive = CONFIG_ESP_RMAKER_MQTT_KEEP_ALIVE_INTERVAL,
#ifdef CONFIG_ESP_RMAKER_MQTT_PERSISTENT_SESSION
            .disable_clean_session = 1,
#endif /* CONFIG_ESP_RMAKER_MQTT_PERSISTENT_SESSION */
        },
    };
#else
    const esp_mqtt_client_config_t mqtt_client_cfg = {
        .host = conn_params->mqtt_host,
#ifdef ESP_RMAKER_MQTT_USE_PORT_443
        .port = 443,
        .alpn_protos = alpn_protocols,
#else
        .port = 8883,
#endif /* !ESP_RMAKER_MQTT_USE_PORT_443 */
#ifdef ESP_RMAKER_MQTT_USE_CERT_BUNDLE
        .crt_bundle_attach = esp_crt_bundle_attach,
#else
        .cert_pem = (const char *)conn_params->server_cert,
        .cert_len = conn_params->server_cert_len,
#endif
        .client_cert_pem = (const char *)conn_params->client_cert,
        .client_cert_len = conn_params->client_cert_len,
        .client_key_pem = (const char *)conn_params->client_key,
        .client_key_len = conn_params->client_key_len,
        .client_id = (const char *)conn_params->client_id,
        .keepalive = CONFIG_ESP_RMAKER_MQTT_KEEP_ALIVE_INTERVAL,
        .event_handle = mqtt_event_handler,
        .transport = MQTT_TRANSPORT_OVER_SSL,
#ifdef CONFIG_ESP_RMAKER_MQTT_PERSISTENT_SESSION
        .disable_clean_session = 1,
#endif /* CONFIG_ESP_RMAKER_MQTT_PERSISTENT_SESSION */
#ifdef CONFIG_ESP_RMAKER_MQTT_SEND_USERNAME
        .username = username,
#endif
    };
#endif
    mqtt_data->mqtt_client = esp_mqtt_client_init(&mqtt_client_cfg);
    if (!mqtt_data->mqtt_client) {
        ESP_LOGE(TAG, "esp_mqtt_client_init failed");
        esp_mqtt_glue_deinit();
        return ESP_FAIL;
    }
#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(5, 0, 0)
    esp_mqtt_client_register_event(mqtt_data->mqtt_client , ESP_EVENT_ANY_ID, mqtt_event_handler, NULL);
#endif
    return ESP_OK;
}

static void esp_mqtt_glue_deinit(void)
{
    esp_mqtt_glue_unsubscribe_all();
    if (mqtt_data && mqtt_data->mqtt_client) {
        esp_mqtt_client_destroy(mqtt_data->mqtt_client);
    }
    if (mqtt_data) {
        free(mqtt_data);
        mqtt_data = NULL;
    }
}

esp_err_t esp_rmaker_mqtt_glue_setup(esp_rmaker_mqtt_config_t *mqtt_config)
{
    mqtt_config->init           = esp_mqtt_glue_init;
    mqtt_config->deinit         = esp_mqtt_glue_deinit;
    mqtt_config->connect        = esp_mqtt_glue_connect;
    mqtt_config->disconnect     = esp_mqtt_glue_disconnect;
    mqtt_config->publish        = esp_mqtt_glue_publish;
    mqtt_config->subscribe      = esp_mqtt_glue_subscribe;
    mqtt_config->unsubscribe    = esp_mqtt_glue_unsubscribe;
    mqtt_config->setup_done     = true;
    return ESP_OK;
}
