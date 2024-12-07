// Copyright 2020 Espressif Systems (Shanghai) PTE LTD
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
#include <string.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>
#include <freertos/event_groups.h>
#include <esp_log.h>
#include <esp_wifi.h>
#include <esp_event.h>
#include <esp_bit_defs.h>

#include <esp_rmaker_factory.h>
#include <esp_rmaker_work_queue.h>
#include <esp_rmaker_common_events.h>
#include <esp_rmaker_utils.h>

#include <esp_rmaker_core.h>
#include <esp_rmaker_user_mapping.h>
#include <esp_rmaker_utils.h>
#include "esp_rmaker_internal.h"
#include "esp_rmaker_mqtt.h"
#include "esp_rmaker_claim.h"
#include "esp_rmaker_client_data.h"

#ifdef CONFIG_OPENTHREAD_ENABLED
#include <esp_openthread.h>
#include <esp_openthread_lock.h>
#include <esp_openthread_dns64.h>
#endif

#if defined(CONFIG_ESP_RMAKER_NETWORK_OVER_WIFI)
static const int WIFI_CONNECTED_EVENT = BIT0;
#elif defined(CONFIG_ESP_RMAKER_NETWORK_OVER_THREAD)
static const int THREAD_SET_DNS_SEVER_EVENT = BIT0;
#endif

#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(5, 1, 0)
#include "esp_mac.h"
#endif

static const int MQTT_CONNECTED_EVENT = BIT1;
static EventGroupHandle_t rmaker_core_event_group;

ESP_EVENT_DEFINE_BASE(RMAKER_EVENT);

static const char *TAG = "esp_rmaker_core";


#if defined(CONFIG_ESP_RMAKER_SELF_CLAIM) || defined(CONFIG_ESP_RMAKER_ASSISTED_CLAIM)
#define ESP_RMAKER_CLAIM_ENABLED
#endif

#define ESP_RMAKER_CHECK_HANDLE(rval) \
{ \
    if (!esp_rmaker_priv_data) {\
        ESP_LOGE(TAG, "ESP RainMaker not initialised"); \
        return rval; \
    } \
}

#define ESP_CLAIM_NODE_ID_SIZE  12

/* Handle to maintain internal information (will move to an internal file) */
typedef struct {
    char *node_id;
    const esp_rmaker_node_t *node;
    bool enable_time_sync;
    esp_rmaker_state_t state;
    bool mqtt_connected;
    esp_rmaker_mqtt_conn_params_t *mqtt_conn_params;
#ifdef ESP_RMAKER_CLAIM_ENABLED
    bool need_claim;
    esp_rmaker_claim_data_t *claim_data;
#endif /* ESP_RMAKER_CLAIM_ENABLED */
} esp_rmaker_priv_data_t;

static esp_rmaker_priv_data_t *esp_rmaker_priv_data;

bool esp_rmaker_is_mqtt_connected()
{
    if (esp_rmaker_priv_data) {        
        return esp_rmaker_priv_data->mqtt_connected;
    }
    return false;
}

esp_rmaker_state_t esp_rmaker_get_state(void)
{
    if (esp_rmaker_priv_data) {
        return esp_rmaker_priv_data->state;
    }
    return ESP_RMAKER_STATE_DEINIT;
}

static void reset_event_handler(void* arg, esp_event_base_t event_base,
                          int32_t event_id, void* event_data)
{
        switch (event_id) {
            case RMAKER_EVENT_WIFI_RESET:
                esp_rmaker_mqtt_disconnect();
                break;
            case RMAKER_EVENT_FACTORY_RESET:
                esp_rmaker_reset_user_node_mapping();
                break;
            default:
                break;
        }
}

static void esp_rmaker_mqtt_event_handler(void* arg, esp_event_base_t event_base,
                          int32_t event_id, void* event_data)
{
    if (!esp_rmaker_priv_data) {
        return;
    }
    if (event_base == RMAKER_COMMON_EVENT && event_id == RMAKER_MQTT_EVENT_CONNECTED) {
        esp_rmaker_priv_data->mqtt_connected = true;
    } else if (event_base == RMAKER_COMMON_EVENT && event_id == RMAKER_MQTT_EVENT_DISCONNECTED) {
        esp_rmaker_priv_data->mqtt_connected = false;
    }
}

#ifdef CONFIG_ESP_RMAKER_READ_NODE_ID_FROM_CERT_CN

#include "mbedtls/x509_crt.h"
#include "mbedtls/oid.h"

static char* esp_rmaker_populate_node_id_from_cert()
{ 
    void *addr = NULL;
    size_t len = 0;
    addr = esp_rmaker_get_client_cert();
    if (addr) {
        len = esp_rmaker_get_client_cert_len();
    } else { 
        ESP_LOGE(TAG, "Failed to get device certificate."); 
        return NULL; 
    }
    mbedtls_x509_crt crt;
    mbedtls_x509_crt_init(&crt);
    char *node_id = NULL;
    int ret = mbedtls_x509_crt_parse(&crt, addr, len); 
    if (ret != 0) {
        ESP_LOGE(TAG, "Parsing of device certificate failed, returned %02X", ret); 
    } else {
        mbedtls_asn1_named_data *cn_data;
        cn_data = mbedtls_asn1_find_named_data(&crt.subject, MBEDTLS_OID_AT_CN, 
                                                MBEDTLS_OID_SIZE(MBEDTLS_OID_AT_CN));
        if (cn_data) {
            node_id = MEM_CALLOC_EXTRAM(1, cn_data->val.len + 1);
            memcpy(node_id, (const char *)cn_data->val.p, cn_data->val.len);
        }
    }
    mbedtls_x509_crt_free(&crt); 
    return node_id; 
}
#endif

static char *esp_rmaker_populate_node_id(bool use_claiming)
{
#ifdef CONFIG_ESP_RMAKER_READ_NODE_ID_FROM_CERT_CN
    char *node_id = esp_rmaker_populate_node_id_from_cert();
#else /* !CONFIG_ESP_RMAKER_READ_NODE_ID_FROM_CERT_CN */
    char *node_id = esp_rmaker_factory_get("node_id");
#endif /* CONFIG_ESP_RMAKER_READ_NODE_ID_FROM_CERT_CN */

#ifdef ESP_RMAKER_CLAIM_ENABLED
    if (!node_id && use_claiming) {
        uint8_t mac_addr[6];
#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(5, 1, 0)
        /* ESP_MAC_BASE was introduced in ESP-IDF v5.1. It is the same as the Wi-Fi Station MAC address
         * for chips supporting Wi-Fi. We can use base MAC address to generate claim init request for both
         * Wi-Fi and Thread devices
         */
        esp_err_t err = esp_read_mac(mac_addr, ESP_MAC_BASE);
#else
        /* Thread was officially supported in ESP-IDF v5.1. Use Wi-Fi Station MAC address to generate claim
         * init request.
         */
        esp_err_t err = esp_wifi_get_mac(WIFI_IF_STA, mac_addr);
#endif

        if (err != ESP_OK) {
            ESP_LOGE(TAG, "Could not fetch MAC address.");
            return NULL;
        }
        node_id = MEM_CALLOC_EXTRAM(1, ESP_CLAIM_NODE_ID_SIZE + 1); /* +1 for NULL terminatation */
        snprintf(node_id, ESP_CLAIM_NODE_ID_SIZE + 1, "%02X%02X%02X%02X%02X%02X",
                mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
    }
#endif /* ESP_RMAKER_CLAIM_ENABLED */
    return node_id;
}

esp_err_t esp_rmaker_change_node_id(char *node_id, size_t len)
{
    if(esp_rmaker_priv_data) {
        char *new_node_id = strndup(node_id, len);
        if (!new_node_id) {
            ESP_LOGE(TAG, "Failed to allocate %lu bytes for new node_id.", (unsigned long) len);
            return ESP_ERR_NO_MEM;
        }
        if (esp_rmaker_priv_data->node_id) {
            free(esp_rmaker_priv_data->node_id);
        }
        esp_rmaker_priv_data->node_id = new_node_id;
        _esp_rmaker_node_t *node = (_esp_rmaker_node_t *)esp_rmaker_get_node();
        node->node_id = new_node_id;
        ESP_LOGI(TAG, "New Node ID ----- %s", new_node_id);
        return ESP_OK;
    }
    return ESP_ERR_INVALID_STATE;
}


/* Event handler for catching system events */
static void esp_rmaker_event_handler(void* arg, esp_event_base_t event_base,
                          int32_t event_id, void* event_data)
{
#if defined(CONFIG_ESP_RMAKER_NETWORK_OVER_WIFI)
    if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
#ifdef CONFIG_ESP_RMAKER_ASSISTED_CLAIM
        if (esp_rmaker_priv_data->claim_data) {
            ESP_LOGE(TAG, "Node connected to Wi-Fi without Assisted claiming. Cannot proceed to MQTT connection.");
            ESP_LOGE(TAG, "Please update your phone apps and repeat Wi-Fi provisioning with BLE transport.");
        }
#endif
        if (rmaker_core_event_group) {
            /* Signal rmaker thread to continue execution */
            xEventGroupSetBits(rmaker_core_event_group, WIFI_CONNECTED_EVENT);
        }
    } else
#elif defined(CONFIG_ESP_RMAKER_NETWORK_OVER_THREAD) /* CONFIG_ESP_RMAKER_NETWORK_OVER_WIFI */
    if (event_base == OPENTHREAD_EVENT && event_id == OPENTHREAD_EVENT_SET_DNS_SERVER) {
#ifdef CONFIG_ESP_RMAKER_ASSISTED_CLAIM
        if (esp_rmaker_priv_data->claim_data) {
            ESP_LOGE(TAG, "Node connected to Thread network without Assisted claiming. Cannot proceed to MQTT connection.");
            ESP_LOGE(TAG, "Please update your phone apps and repeat Wi-Fi provisioning with BLE transport.");
        }
#endif
        if (rmaker_core_event_group) {
            /* Signal rmaker thread to continue execution */
            xEventGroupSetBits(rmaker_core_event_group, THREAD_SET_DNS_SEVER_EVENT);
        }
    } else
#endif /* CONFIG_ESP_RMAKER_NETWORK_OVER_THREAD */
     if (event_base == RMAKER_EVENT &&
            (event_id == RMAKER_EVENT_USER_NODE_MAPPING_DONE ||
            event_id == RMAKER_EVENT_USER_NODE_MAPPING_RESET)) {
        esp_event_handler_unregister(RMAKER_EVENT, event_id, &esp_rmaker_event_handler);
        esp_rmaker_params_mqtt_init();
    } else if (event_base == RMAKER_COMMON_EVENT && event_id == RMAKER_MQTT_EVENT_CONNECTED) {
        if (rmaker_core_event_group) {
            /* Signal rmaker thread to continue execution */
            xEventGroupSetBits(rmaker_core_event_group, MQTT_CONNECTED_EVENT);
        }
    }
}

static esp_err_t esp_rmaker_deinit_priv_data(esp_rmaker_priv_data_t *rmaker_priv_data)
{
    if (!rmaker_priv_data) {
        return ESP_ERR_INVALID_ARG;
    }
    esp_rmaker_work_queue_deinit();
#ifndef CONFIG_ESP_RMAKER_DISABLE_USER_MAPPING_PROV
    esp_rmaker_user_mapping_prov_deinit();
#endif
#ifdef ESP_RMAKER_CLAIM_ENABLED
    if (rmaker_priv_data->claim_data) {
        esp_rmaker_claim_data_free(rmaker_priv_data->claim_data);
    }
#endif
    if (rmaker_priv_data->mqtt_conn_params) {
        esp_rmaker_clean_mqtt_conn_params(rmaker_priv_data->mqtt_conn_params);
        free(rmaker_priv_data->mqtt_conn_params);
    }
    if (rmaker_priv_data->node_id) {
        free(rmaker_priv_data->node_id);
    }
    free(rmaker_priv_data);
    return ESP_OK;
}

esp_err_t esp_rmaker_node_deinit(const esp_rmaker_node_t *node)
{
    if (!esp_rmaker_priv_data) {
        ESP_LOGE(TAG, "ESP RainMaker already de-initialized.");
        return ESP_ERR_INVALID_ARG;
    }

    if (esp_rmaker_priv_data->state != ESP_RMAKER_STATE_INIT_DONE) {
        ESP_LOGE(TAG, "ESP RainMaker is still running. Please stop it first.");
        return ESP_ERR_INVALID_STATE;
    }
    esp_rmaker_node_delete(node);
    esp_rmaker_priv_data->node = NULL;
    esp_rmaker_deinit_priv_data(esp_rmaker_priv_data);
    esp_rmaker_priv_data = NULL;
    return ESP_OK;
}

char *esp_rmaker_get_node_id(void)
{
    if (esp_rmaker_priv_data) {
        return esp_rmaker_priv_data->node_id;
    }
    return NULL;
}

static esp_err_t esp_rmaker_report_node_config_and_state()
{
    if (esp_rmaker_report_node_config() != ESP_OK) {
        ESP_LOGE(TAG, "Report node config failed.");
        return ESP_FAIL;
    }
    if (esp_rmaker_user_node_mapping_get_state() == ESP_RMAKER_USER_MAPPING_DONE) {
        if (esp_rmaker_report_node_state() != ESP_OK) {
            ESP_LOGE(TAG, "Report node state failed.");
            return ESP_FAIL;
        }
    }
    return ESP_OK;
}

static void __esp_rmaker_report_node_config_and_state(void *data)
{
    esp_rmaker_report_node_config_and_state();
}

esp_err_t esp_rmaker_report_node_details()
{
    return esp_rmaker_work_queue_add_task(__esp_rmaker_report_node_config_and_state, NULL);
}


static void esp_rmaker_task(void *data)
{
    ESP_RMAKER_CHECK_HANDLE();
    esp_rmaker_priv_data->state = ESP_RMAKER_STATE_STARTING;
    esp_err_t err = ESP_FAIL;
#if defined(CONFIG_ESP_RMAKER_NETWORK_OVER_WIFI)
    wifi_ap_record_t ap_info;
#elif defined(CONFIG_ESP_RMAKER_NETWORK_OVER_THREAD)
    ip6_addr_t nat64_prefix;
#endif
    esp_rmaker_priv_data->mqtt_connected = false;
    esp_event_handler_register(RMAKER_COMMON_EVENT, RMAKER_MQTT_EVENT_CONNECTED, &esp_rmaker_mqtt_event_handler, NULL);
    esp_event_handler_register(RMAKER_COMMON_EVENT, RMAKER_MQTT_EVENT_DISCONNECTED, &esp_rmaker_mqtt_event_handler, NULL);
    rmaker_core_event_group = xEventGroupCreate();
    if (!rmaker_core_event_group) {
        ESP_LOGE(TAG, "Failed to create event group. Aborting");
        goto rmaker_end;
    }
#if defined(CONFIG_ESP_RMAKER_NETWORK_OVER_WIFI)
    err = esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &esp_rmaker_event_handler, esp_rmaker_priv_data);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to register event handler. Error: %d. Aborting", err);
        goto rmaker_end;
    }
#elif defined(CONFIG_ESP_RMAKER_NETWORK_OVER_THREAD)
    err = esp_event_handler_register(OPENTHREAD_EVENT, OPENTHREAD_EVENT_SET_DNS_SERVER, &esp_rmaker_event_handler, esp_rmaker_priv_data);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to register event handler. Error: %d. Aborting", err);
        goto rmaker_end;
    }
#endif
    err = esp_event_handler_register(RMAKER_COMMON_EVENT, RMAKER_MQTT_EVENT_CONNECTED, &esp_rmaker_event_handler, esp_rmaker_priv_data);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to register event handler. Error: %d. Aborting", err);
        goto rmaker_end;
    }
    /* Assisted claiming needs to be done before Wi-Fi connection */
#ifdef CONFIG_ESP_RMAKER_ASSISTED_CLAIM
    if (esp_rmaker_priv_data->need_claim) {
#if defined(CONFIG_ESP_RMAKER_NETWORK_OVER_WIFI)
        if (esp_wifi_sta_get_ap_info(&ap_info) == ESP_OK) {
            ESP_LOGE(TAG, "Node connected to Wi-Fi without Assisted claiming. Cannot proceed to MQTT connection.");
            ESP_LOGE(TAG, "Please update your phone apps and repeat Wi-Fi provisioning with BLE transport.");
            esp_event_handler_unregister(IP_EVENT, IP_EVENT_STA_GOT_IP, &esp_rmaker_event_handler);
#elif defined(CONFIG_ESP_RMAKER_NETWORK_OVER_THREAD)
        if (esp_openthread_get_nat64_prefix(&nat64_prefix) == ESP_OK) {
            ESP_LOGE(TAG, "Node connected to Thread without Assisted claiming. Cannot proceed to MQTT connection.");
            ESP_LOGE(TAG, "Please update your phone apps and repeat Thread provisioning with BLE transport.");
            esp_event_handler_unregister(OPENTHREAD_EVENT, OPENTHREAD_EVENT_SET_DNS_SERVER, &esp_rmaker_event_handler);
#endif
            err = ESP_FAIL;
            goto rmaker_end;
        }
        esp_rmaker_post_event(RMAKER_EVENT_CLAIM_STARTED, NULL, 0);
        err = esp_rmaker_assisted_claim_perform(esp_rmaker_priv_data->claim_data);
        if (err != ESP_OK) {
            esp_rmaker_post_event(RMAKER_EVENT_CLAIM_FAILED, NULL, 0);
            ESP_LOGE(TAG, "esp_rmaker_self_claim_perform() returned %d. Aborting", err);
#if defined(CONFIG_ESP_RMAKER_NETWORK_OVER_WIFI)
            esp_event_handler_unregister(IP_EVENT, IP_EVENT_STA_GOT_IP, &esp_rmaker_event_handler);
#elif defined(CONFIG_ESP_RMAKER_NETWORK_OVER_THREAD)
            esp_event_handler_unregister(OPENTHREAD_EVENT, OPENTHREAD_EVENT_SET_DNS_SERVER, &esp_rmaker_event_handler);
#endif
            goto rmaker_end;
        }
        esp_rmaker_priv_data->claim_data = NULL;
        esp_rmaker_post_event(RMAKER_EVENT_CLAIM_SUCCESSFUL, NULL, 0);
    }
#endif /* CONFIG_ESP_RMAKER_ASSISTED_CLAIM */
#if defined(CONFIG_ESP_RMAKER_NETWORK_OVER_WIFI)
    /* Check if already connected to Wi-Fi */
    if (esp_wifi_sta_get_ap_info(&ap_info) != ESP_OK) {
        /* Wait for Wi-Fi connection */
        xEventGroupWaitBits(rmaker_core_event_group, WIFI_CONNECTED_EVENT, false, true, portMAX_DELAY);
    }
    esp_event_handler_unregister(IP_EVENT, IP_EVENT_STA_GOT_IP, &esp_rmaker_event_handler);
#elif defined(CONFIG_ESP_RMAKER_NETWORK_OVER_THREAD) /* CONFIG_ESP_RMAKER_NETWORK_OVER_WIFI */
    esp_openthread_lock_acquire(portMAX_DELAY);
    err = esp_openthread_get_nat64_prefix(&nat64_prefix);
    esp_openthread_lock_release();
    /* Check if already get nat64 prefix */
    if (err != ESP_OK) {
        /* Wait for Thread connection */
        xEventGroupWaitBits(rmaker_core_event_group, THREAD_SET_DNS_SEVER_EVENT, false, true, portMAX_DELAY);
        err = ESP_OK;
    }
    esp_event_handler_unregister(OPENTHREAD_EVENT, OPENTHREAD_EVENT_SET_DNS_SERVER, &esp_rmaker_event_handler);
#endif /* CONFIG_ESP_RMAKER_NETWORK_OVER_THREAD */

    if (esp_rmaker_priv_data->enable_time_sync) {
#ifdef CONFIG_MBEDTLS_HAVE_TIME_DATE
        esp_rmaker_time_wait_for_sync(portMAX_DELAY);
#endif
    }
    /* Self claiming can be done only after Wi-Fi connection */
#ifdef CONFIG_ESP_RMAKER_SELF_CLAIM
    if (esp_rmaker_priv_data->need_claim) {
        esp_rmaker_post_event(RMAKER_EVENT_CLAIM_STARTED, NULL, 0);
        err = esp_rmaker_self_claim_perform(esp_rmaker_priv_data->claim_data);
        if (err != ESP_OK) {
            esp_rmaker_post_event(RMAKER_EVENT_CLAIM_FAILED, NULL, 0);
            ESP_LOGE(TAG, "esp_rmaker_self_claim_perform() returned %d. Aborting", err);
            goto rmaker_end;
        }
        esp_rmaker_priv_data->claim_data = NULL;
        esp_rmaker_post_event(RMAKER_EVENT_CLAIM_SUCCESSFUL, NULL, 0);
    }
#endif
#ifdef ESP_RMAKER_CLAIM_ENABLED
    if (esp_rmaker_priv_data->need_claim) {
        esp_rmaker_priv_data->mqtt_conn_params = esp_rmaker_get_mqtt_conn_params();
        if (!esp_rmaker_priv_data->mqtt_conn_params) {
            ESP_LOGE(TAG, "Failed to initialise MQTT Config after claiming. Aborting");
            err = ESP_FAIL;
            goto rmaker_end;
        }
        err = esp_rmaker_mqtt_init(esp_rmaker_priv_data->mqtt_conn_params);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "esp_rmaker_mqtt_init() returned %d. Aborting", err);
            goto rmaker_end;
        }
        esp_rmaker_priv_data->need_claim = false;
    }
#endif /* ESP_RMAKER_CLAIM_ENABLED */
#ifdef CONFIG_ESP_RMAKER_CMD_RESP_ENABLE
        err = esp_rmaker_cmd_response_enable();
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "Failed to enable Command - Response module. Aborting!!!");
            goto rmaker_end;
        }
        esp_rmaker_node_add_attribute(esp_rmaker_get_node(), "cmd-resp", "1");
#else
    ESP_LOGW(TAG, "Command-Response Module not enabled. Set CONFIG_ESP_RMAKER_CMD_RESP_ENABLE=y to use it.");
#endif /* !CONFIG_ESP_RMAKER_CMD_RESP_ENABLE */
#ifdef CONFIG_ESP_RMAKER_LOCAL_CTRL_ENABLE
    err = esp_rmaker_local_ctrl_enable();
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to start local control service. Aborting!!!");
        goto rmaker_end;
    }
#endif /* CONFIG_ESP_RMAKER_LOCAL_CTRL_ENABLE */
    err = esp_rmaker_mqtt_connect();
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "esp_rmaker_mqtt_connect() returned %d. Aborting", err);
        goto rmaker_end;
    }
    ESP_LOGI(TAG, "Waiting for MQTT connection");
    xEventGroupWaitBits(rmaker_core_event_group, MQTT_CONNECTED_EVENT, false, true, portMAX_DELAY);
    esp_event_handler_unregister(RMAKER_COMMON_EVENT, RMAKER_MQTT_EVENT_CONNECTED, &esp_rmaker_event_handler);
    esp_rmaker_priv_data->state = ESP_RMAKER_STATE_STARTED;
    err = esp_rmaker_report_node_config();
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Aborting!!!");
        goto rmaker_end;
    }
    if (esp_rmaker_user_node_mapping_get_state() == ESP_RMAKER_USER_MAPPING_DONE) {
        err = esp_rmaker_params_mqtt_init();
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "Aborting!!!");
            goto rmaker_end;
        }
    } else {
        /* If network is connected without even starting the user-node mapping workflow,
         * it could mean that some incorrect app was used to provision the device. Even
         * if the older user would not be able to update the params, it would be better
         * to completely reset the user permissions by sending a dummy user node mapping
         * request, so that the earlier user won't even see the connectivity and other
         * status.
         */
        if (esp_rmaker_user_node_mapping_get_state() != ESP_RMAKER_USER_MAPPING_STARTED) {
            esp_rmaker_reset_user_node_mapping();
            /* Wait for user reset to finish. */
            err = esp_event_handler_register(RMAKER_EVENT, RMAKER_EVENT_USER_NODE_MAPPING_RESET,
                    &esp_rmaker_event_handler, NULL);
        } else {
            /* Wait for User Node mapping to finish. */
            err = esp_event_handler_register(RMAKER_EVENT, RMAKER_EVENT_USER_NODE_MAPPING_DONE,
                    &esp_rmaker_event_handler, NULL);
        }
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "Aborting!!!");
            goto rmaker_end;
        }
        ESP_LOGI(TAG, "Waiting for User Node Association.");
    }
    err = ESP_OK;

rmaker_end:
    if (rmaker_core_event_group) {
        vEventGroupDelete(rmaker_core_event_group);
    }
    rmaker_core_event_group = NULL;
    if (err == ESP_OK) {
        return;
    }
    if (esp_rmaker_priv_data->mqtt_connected) {
        esp_rmaker_mqtt_disconnect();
    }
    esp_rmaker_priv_data->state = ESP_RMAKER_STATE_INIT_DONE;
}


static esp_err_t esp_rmaker_mqtt_conn_params_init(esp_rmaker_priv_data_t *rmaker_priv_data, bool use_claiming)
{
    rmaker_priv_data->mqtt_conn_params = esp_rmaker_get_mqtt_conn_params();
    if (rmaker_priv_data->mqtt_conn_params) {
        return ESP_OK;
    }
#ifdef ESP_RMAKER_CLAIM_ENABLED
    if (use_claiming) {
#ifdef CONFIG_ESP_RMAKER_SELF_CLAIM
        rmaker_priv_data->claim_data = esp_rmaker_self_claim_init();
#endif
#ifdef CONFIG_ESP_RMAKER_ASSISTED_CLAIM
        rmaker_priv_data->claim_data = esp_rmaker_assisted_claim_init();
#endif
        if (!rmaker_priv_data->claim_data) {
            ESP_LOGE(TAG, "Failed to initialise Claiming.");
            return ESP_FAIL;
        } else {
            rmaker_priv_data->need_claim = true;
            return ESP_OK;
        }
    }
#endif /* ESP_RMAKER_CLAIM_ENABLED */
    return ESP_FAIL;
}
/* Initialize ESP RainMaker */
static esp_err_t esp_rmaker_init(const esp_rmaker_config_t *config, bool use_claiming)
{
    if (esp_rmaker_priv_data) {
        ESP_LOGE(TAG, "ESP RainMaker already initialised");
        return ESP_ERR_INVALID_STATE;
    }
    if (!config) {
        ESP_LOGE(TAG, "RainMaker config missing. Cannot initialise");
        return ESP_ERR_INVALID_ARG;
    }
    if (esp_rmaker_factory_init() != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialise storage");
        return ESP_FAIL;
    }
    esp_rmaker_priv_data = MEM_CALLOC_EXTRAM(1, sizeof(esp_rmaker_priv_data_t));
    if (!esp_rmaker_priv_data) {
        ESP_LOGE(TAG, "Failed to allocate memory");
        return ESP_ERR_NO_MEM;
    }

    esp_rmaker_priv_data->node_id = esp_rmaker_populate_node_id(use_claiming);
    if (!esp_rmaker_priv_data->node_id) {
        esp_rmaker_deinit_priv_data(esp_rmaker_priv_data);
        esp_rmaker_priv_data = NULL;
        ESP_LOGE(TAG, "Failed to initialise Node Id. Please perform \"claiming\" using RainMaker CLI.");
        return ESP_ERR_NO_MEM;
    }

    if (esp_rmaker_work_queue_init() != ESP_OK) {
        esp_rmaker_deinit_priv_data(esp_rmaker_priv_data);
        esp_rmaker_priv_data = NULL;
        ESP_LOGE(TAG, "ESP RainMaker Queue Creation Failed");
        return ESP_ERR_NO_MEM;
    }
#ifndef CONFIG_ESP_RMAKER_DISABLE_USER_MAPPING_PROV
    if (esp_rmaker_user_mapping_prov_init()) {
        esp_rmaker_deinit_priv_data(esp_rmaker_priv_data);
        esp_rmaker_priv_data = NULL;
        ESP_LOGE(TAG, "Could not initialise User-Node mapping.");
        return ESP_FAIL;
    }
#endif /* !CONFIG_ESP_RMAKER_DISABLE_USER_MAPPING_PROV */
    if (esp_rmaker_mqtt_conn_params_init(esp_rmaker_priv_data, use_claiming) != ESP_OK) {
        esp_rmaker_deinit_priv_data(esp_rmaker_priv_data);
        esp_rmaker_priv_data = NULL;
        ESP_LOGE(TAG, "Failed to initialise MQTT Params. Please perform \"claiming\" using RainMaker CLI.");
        return ESP_FAIL;
    } else {
#ifdef ESP_RMAKER_CLAIM_ENABLED
        if (!esp_rmaker_priv_data->need_claim)
#endif /* ESP_RMAKER_CLAIM_ENABLED */
        {
            if (esp_rmaker_mqtt_init(esp_rmaker_priv_data->mqtt_conn_params) != ESP_OK) {
                esp_rmaker_deinit_priv_data(esp_rmaker_priv_data);
                esp_rmaker_priv_data = NULL;
                ESP_LOGE(TAG, "Failed to initialise MQTT");
                return ESP_FAIL;
            }
        }
    }
    esp_rmaker_user_node_mapping_init();
#ifdef CONFIG_ESP_RMAKER_LOCAL_CTRL_ENABLE
    esp_rmaker_init_local_ctrl_service();
#endif
    esp_rmaker_priv_data->enable_time_sync = config->enable_time_sync;
    esp_rmaker_post_event(RMAKER_EVENT_INIT_DONE, NULL, 0);
    esp_rmaker_priv_data->state = ESP_RMAKER_STATE_INIT_DONE;

    /* Adding the RainMaker Task to the queue so that it is will be the first function
     * to be executed when the Work Queue task begins.
     */
    esp_rmaker_work_queue_add_task(esp_rmaker_task, NULL);
    return ESP_OK;
}

static esp_err_t esp_rmaker_register_node(const esp_rmaker_node_t *node)
{
    ESP_RMAKER_CHECK_HANDLE(ESP_ERR_INVALID_STATE);
    if (esp_rmaker_priv_data->node) {
        ESP_LOGE(TAG, "A node has already been registered. Cannot register another.");
        return ESP_ERR_INVALID_STATE;
    }
    if (!node) {
        ESP_LOGE(TAG, "Node handle cannot be NULL.");
        return ESP_ERR_INVALID_ARG;
    }
    esp_rmaker_priv_data->node = node;
    return ESP_OK;
}

esp_rmaker_node_t *esp_rmaker_node_init(const esp_rmaker_config_t *config, const char *name, const char *type)
{
    esp_err_t err = esp_rmaker_init(config, true);
    if (err != ESP_OK) {
        return NULL;
    }
    esp_rmaker_node_t *node = esp_rmaker_node_create(name, type);
    if (!node) {
        ESP_LOGE(TAG, "Failed to create node");
        return NULL;
    }
    err = esp_rmaker_register_node(node);
    if (err != ESP_OK) {
        free(node);
        return NULL;
    }
    return node;
}

const esp_rmaker_node_t *esp_rmaker_get_node()
{
    ESP_RMAKER_CHECK_HANDLE(NULL);
    return esp_rmaker_priv_data->node;
}

/* Start the ESP RainMaker Core Task */
esp_err_t esp_rmaker_start(void)
{
    ESP_RMAKER_CHECK_HANDLE(ESP_ERR_INVALID_STATE);
    if (esp_rmaker_priv_data->enable_time_sync) {
        esp_rmaker_time_sync_init(NULL);
    }
    ESP_LOGI(TAG, "Starting RainMaker Work Queue task");
    if (esp_rmaker_work_queue_start() != ESP_OK) {
        ESP_LOGE(TAG, "Couldn't create RainMaker Work Queue task");
        return ESP_FAIL;
    }
    ESP_ERROR_CHECK(esp_event_handler_register(RMAKER_COMMON_EVENT, ESP_EVENT_ANY_ID, &reset_event_handler, NULL));
    return ESP_OK;
}

esp_err_t esp_rmaker_stop()
{
    ESP_RMAKER_CHECK_HANDLE(ESP_ERR_INVALID_STATE);
    esp_rmaker_priv_data->state = ESP_RMAKER_STATE_STOP_REQUESTED;
    return ESP_OK;
}
