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

#include <sdkconfig.h>
#include <stdint.h>
#include <string.h>
#include <nvs_flash.h>
#include <nvs.h>
#include <esp_rmaker_mqtt_glue.h>
#include <esp_rmaker_factory.h>

#define INSIGHTS_CLIENT_CERT_NVS_KEY     "client_cert"
#define INSIGHTS_CLIENT_KEY_NVS_KEY      "client_key"
#define INSIGHTS_MQTT_HOST_NVS_KEY       "mqtt_host"
#define INSIGHTS_NODE_ID                 "node_id"
#define INSIGHTS_META_CRC_NVS_KEY        "i_meta_crc"
#define INSIGHTS_NVS_NAMESPACE           "nvs"

extern uint8_t mqtt_server_root_ca_pem_start[] asm("_binary_mqtt_server_crt_start");
extern uint8_t mqtt_server_root_ca_pem_end[] asm("_binary_mqtt_server_crt_end");

static char *esp_insights_get_mqtt_host(void)
{
    return esp_rmaker_factory_get(INSIGHTS_MQTT_HOST_NVS_KEY);
}

static char *esp_insights_get_client_cert(void)
{
    return esp_rmaker_factory_get(INSIGHTS_CLIENT_CERT_NVS_KEY);
}

static char *esp_insights_get_client_key(void)
{
    return esp_rmaker_factory_get(INSIGHTS_CLIENT_KEY_NVS_KEY);
}

esp_rmaker_mqtt_conn_params_t *esp_insights_get_mqtt_conn_params(void)
{
    esp_rmaker_mqtt_conn_params_t *mqtt_conn_params = calloc(1, sizeof(esp_rmaker_mqtt_conn_params_t));
    if ((mqtt_conn_params->client_key = esp_insights_get_client_key()) == NULL) {
        goto init_err;
    }
    if ((mqtt_conn_params->client_cert = esp_insights_get_client_cert()) == NULL) {
        goto init_err;
    }
    if ((mqtt_conn_params->mqtt_host = esp_insights_get_mqtt_host()) == NULL) {
        goto init_err;
    }
    mqtt_conn_params->server_cert = (char *)mqtt_server_root_ca_pem_start;
    mqtt_conn_params->client_id = esp_rmaker_factory_get(INSIGHTS_NODE_ID);
    return mqtt_conn_params;
init_err:
    if (mqtt_conn_params->mqtt_host) {
        free(mqtt_conn_params->mqtt_host);
    }
    if (mqtt_conn_params->client_cert) {
        free(mqtt_conn_params->client_cert);
    }
    if (mqtt_conn_params->client_key) {
        free(mqtt_conn_params->client_key);
    }
    free(mqtt_conn_params);
    return NULL;
}

void esp_insights_clean_mqtt_conn_params(esp_rmaker_mqtt_conn_params_t *mqtt_conn_params)
{
    if (mqtt_conn_params) {
        if (mqtt_conn_params->mqtt_host) {
            free(mqtt_conn_params->mqtt_host);
        }
        if (mqtt_conn_params->client_cert) {
            free(mqtt_conn_params->client_cert);
        }
        if (mqtt_conn_params->client_key) {
            free(mqtt_conn_params->client_key);
        }
    }
}

esp_err_t esp_insights_meta_nvs_crc_get(uint32_t *crc)
{
    if (!crc) {
        return ESP_ERR_INVALID_ARG;
    }
    nvs_handle_t handle;
    esp_err_t err = nvs_open(INSIGHTS_NVS_NAMESPACE, NVS_READONLY, &handle);
    if (err != ESP_OK) {
        return err;
    }
    err = nvs_get_u32(handle, INSIGHTS_META_CRC_NVS_KEY, crc);
    if (err != ESP_OK) {
        nvs_close(handle);
        return err;
    }
    nvs_close(handle);
    return err;
}

esp_err_t esp_insights_meta_nvs_crc_set(uint32_t crc)
{
    nvs_handle_t handle;
    esp_err_t err = nvs_open(INSIGHTS_NVS_NAMESPACE, NVS_READWRITE, &handle);
    if (err != ESP_OK) {
        return err;
    }
    err = nvs_set_u32(handle, INSIGHTS_META_CRC_NVS_KEY, crc);
    if (err != ESP_OK) {
        nvs_close(handle);
        return err;
    }
    nvs_commit(handle);
    nvs_close(handle);
    return err;
}
