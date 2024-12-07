/*
 * SPDX-FileCopyrightText: 2019-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <esp_log.h>
#include <string.h>
#include <esp_err.h>
#include <esp_wifi.h>

#include "network_scan.pb-c.h"

#include <network_provisioning/network_scan.h>

static const char *TAG = "proto_network_scan";

typedef struct network_prov_scan_cmd {
    int cmd_num;
    esp_err_t (*command_handler)(NetworkScanPayload *req,
                                 NetworkScanPayload *resp, void *priv_data);
} network_prov_scan_cmd_t;

static esp_err_t cmd_scan_start_handler(NetworkScanPayload *req,
                                        NetworkScanPayload *resp,
                                        void *priv_data);

static esp_err_t cmd_scan_status_handler(NetworkScanPayload *req,
        NetworkScanPayload *resp,
        void *priv_data);

static esp_err_t cmd_scan_result_handler(NetworkScanPayload *req,
        NetworkScanPayload *resp,
        void *priv_data);

static network_prov_scan_cmd_t cmd_table[] = {
    {
        .cmd_num = NETWORK_SCAN_MSG_TYPE__TypeCmdScanWifiStart,
        .command_handler = cmd_scan_start_handler
    },
    {
        .cmd_num = NETWORK_SCAN_MSG_TYPE__TypeCmdScanWifiStatus,
        .command_handler = cmd_scan_status_handler
    },
    {
        .cmd_num = NETWORK_SCAN_MSG_TYPE__TypeCmdScanWifiResult,
        .command_handler = cmd_scan_result_handler
    },
    {
        .cmd_num = NETWORK_SCAN_MSG_TYPE__TypeCmdScanThreadStart,
        .command_handler = cmd_scan_start_handler
    },
    {
        .cmd_num = NETWORK_SCAN_MSG_TYPE__TypeCmdScanThreadStatus,
        .command_handler = cmd_scan_status_handler
    },
    {
        .cmd_num = NETWORK_SCAN_MSG_TYPE__TypeCmdScanThreadResult,
        .command_handler = cmd_scan_result_handler
    }
};

static esp_err_t cmd_scan_start_handler(NetworkScanPayload *req,
                                        NetworkScanPayload *resp, void *priv_data)
{
    network_prov_scan_handlers_t *h = (network_prov_scan_handlers_t *) priv_data;
    if (!priv_data) {
        ESP_LOGE(TAG, "Command invoked without handlers");
        return ESP_ERR_INVALID_STATE;
    }
    if (req->msg == NETWORK_SCAN_MSG_TYPE__TypeCmdScanWifiStart) {
        RespScanWifiStart *resp_payload = (RespScanWifiStart *) malloc(sizeof(RespScanWifiStart));
        if (!resp_payload) {
            ESP_LOGE(TAG, "Error allocating memory");
            return ESP_ERR_NO_MEM;
        }
        resp_scan_wifi_start__init(resp_payload);
#ifdef CONFIG_NETWORK_PROV_NETWORK_TYPE_WIFI
        if (h->wifi_scan_start) {
            resp->status = (h->wifi_scan_start(req->cmd_scan_wifi_start->blocking,
                                               req->cmd_scan_wifi_start->passive,
                                               req->cmd_scan_wifi_start->group_channels,
                                               req->cmd_scan_wifi_start->period_ms,
                                               &h->ctx) == ESP_OK ? STATUS__Success : STATUS__InternalError);
        } else {
            resp->status = STATUS__InternalError;
        }
#else
        resp->status = STATUS__InvalidArgument;
#endif
        resp->payload_case = NETWORK_SCAN_PAYLOAD__PAYLOAD_RESP_SCAN_WIFI_START;
        resp->resp_scan_wifi_start = resp_payload;
    } else if (req->msg == NETWORK_SCAN_MSG_TYPE__TypeCmdScanThreadStart) {
        RespScanThreadStart *resp_payload = (RespScanThreadStart *) malloc(sizeof(RespScanThreadStart));
        if (!resp_payload) {
            ESP_LOGE(TAG, "Error allocating memory");
            return ESP_ERR_NO_MEM;
        }
        resp_scan_thread_start__init(resp_payload);
#ifdef CONFIG_NETWORK_PROV_NETWORK_TYPE_THREAD
        if (h->thread_scan_start) {
            resp->status = (h->thread_scan_start(req->cmd_scan_thread_start->blocking,
                                                 req->cmd_scan_thread_start->channel_mask,
                                                 &h->ctx) == ESP_OK ? STATUS__Success : STATUS__InternalError);
        } else {
            resp->status = STATUS__InternalError;
        }
#else
        resp->status = STATUS__InvalidArgument;
#endif
        resp->payload_case = NETWORK_SCAN_PAYLOAD__PAYLOAD_RESP_SCAN_THREAD_START;
        resp->resp_scan_thread_start = resp_payload;
    }
    return ESP_OK;
}

static esp_err_t cmd_scan_status_handler(NetworkScanPayload *req,
        NetworkScanPayload *resp, void *priv_data)
{
    bool scan_finished = false;
    uint16_t result_count = 0;

    network_prov_scan_handlers_t *h = (network_prov_scan_handlers_t *) priv_data;
    if (!h) {
        ESP_LOGE(TAG, "Command invoked without handlers");
        return ESP_ERR_INVALID_STATE;
    }
    if (req->msg == NETWORK_SCAN_MSG_TYPE__TypeCmdScanWifiStatus) {
        RespScanWifiStatus *resp_payload = (RespScanWifiStatus *) malloc(sizeof(RespScanWifiStatus));
        if (!resp_payload) {
            ESP_LOGE(TAG, "Error allocating memory");
            return ESP_ERR_NO_MEM;
        }
        resp_scan_wifi_status__init(resp_payload);
#ifdef CONFIG_NETWORK_PROV_NETWORK_TYPE_WIFI
        if (h->wifi_scan_status) {
            resp->status = (h->wifi_scan_status(&scan_finished, &result_count, &h->ctx) == ESP_OK ?
                            STATUS__Success : STATUS__InternalError);
        } else {
            resp->status = STATUS__InternalError;
        }
#else
        resp->status = STATUS__InvalidArgument;
#endif
        resp_payload->scan_finished = scan_finished;
        resp_payload->result_count = result_count;
        resp->payload_case = NETWORK_SCAN_PAYLOAD__PAYLOAD_RESP_SCAN_WIFI_STATUS;
        resp->resp_scan_wifi_status = resp_payload;
    } else if (req->msg == NETWORK_SCAN_MSG_TYPE__TypeCmdScanThreadStatus) {
        RespScanThreadStatus *resp_payload = (RespScanThreadStatus *) malloc(sizeof(RespScanThreadStatus));
        if (!resp_payload) {
            ESP_LOGE(TAG, "Error allocating memory");
            return ESP_ERR_NO_MEM;
        }
        resp_scan_thread_status__init(resp_payload);
#ifdef CONFIG_NETWORK_PROV_NETWORK_TYPE_THREAD
        if (h->thread_scan_status) {
            resp->status = (h->thread_scan_status(&scan_finished, &result_count, &h->ctx) == ESP_OK ?
                            STATUS__Success : STATUS__InternalError);
        } else {
            resp->status = STATUS__InternalError;
        }
#else
        resp->status = STATUS__InvalidArgument;
#endif
        resp_payload->scan_finished = scan_finished;
        resp_payload->result_count = result_count;
        resp->payload_case = NETWORK_SCAN_PAYLOAD__PAYLOAD_RESP_SCAN_THREAD_STATUS;
        resp->resp_scan_thread_status = resp_payload;
    }
    return ESP_OK;
}

static esp_err_t cmd_scan_result_handler(NetworkScanPayload *req,
        NetworkScanPayload *resp, void *priv_data)
{
    esp_err_t err;
    network_prov_scan_handlers_t *h = (network_prov_scan_handlers_t *) priv_data;
    if (!priv_data) {
        ESP_LOGE(TAG, "Command invoked without handlers");
        return ESP_ERR_INVALID_STATE;
    }
    if (req->msg == NETWORK_SCAN_MSG_TYPE__TypeCmdScanWifiResult) {
        RespScanWifiResult *resp_payload = (RespScanWifiResult *) malloc(sizeof(RespScanWifiResult));
        if (!resp_payload) {
            ESP_LOGE(TAG, "Error allocating memory");
            return ESP_ERR_NO_MEM;
        }
        resp_scan_wifi_result__init(resp_payload);
        resp->status = STATUS__Success;
        resp->payload_case = NETWORK_SCAN_PAYLOAD__PAYLOAD_RESP_SCAN_WIFI_RESULT;
        resp->resp_scan_wifi_result = resp_payload;
#ifdef CONFIG_NETWORK_PROV_NETWORK_TYPE_WIFI
        network_prov_scan_wifi_result_t scan_result = {{0}, {0}, 0, 0, 0};
        WiFiScanResult **results = NULL;

        /* Allocate memory only if there are non-zero scan results */
        if (req->cmd_scan_wifi_result->count) {
            results = (WiFiScanResult **) calloc(req->cmd_scan_wifi_result->count,
                                                 sizeof(WiFiScanResult *));
            if (!results) {
                ESP_LOGE(TAG, "Failed to allocate memory for results array");
                return ESP_ERR_NO_MEM;
            }
        }
        resp_payload->entries = results;
        resp_payload->n_entries = req->cmd_scan_wifi_result->count;

        /* If req->cmd_scan_wifi_result->count is 0, the below loop will
        * be skipped.
        */
        for (uint16_t i = 0; i < req->cmd_scan_wifi_result->count; i++) {
            if (!h->wifi_scan_result) {
                resp->status = STATUS__InternalError;
                break;
            }
            err = h->wifi_scan_result(i + req->cmd_scan_wifi_result->start_index,
                                      &scan_result, &h->ctx);
            if (err != ESP_OK) {
                resp->status = STATUS__InternalError;
                break;
            }

            results[i] = (WiFiScanResult *) malloc(sizeof(WiFiScanResult));
            if (!results[i]) {
                ESP_LOGE(TAG, "Failed to allocate memory for result entry");
                return ESP_ERR_NO_MEM;
            }
            wi_fi_scan_result__init(results[i]);

            results[i]->ssid.len = strnlen(scan_result.ssid, 32);
            results[i]->ssid.data = (uint8_t *) strndup(scan_result.ssid, 32);
            if (!results[i]->ssid.data) {
                ESP_LOGE(TAG, "Failed to allocate memory for scan result entry SSID");
                return ESP_ERR_NO_MEM;
            }

            results[i]->channel = scan_result.channel;
            results[i]->rssi = scan_result.rssi;
            results[i]->auth = scan_result.auth;

            results[i]->bssid.len = sizeof(scan_result.bssid);
            results[i]->bssid.data = malloc(results[i]->bssid.len);
            if (!results[i]->bssid.data) {
                ESP_LOGE(TAG, "Failed to allocate memory for scan result entry BSSID");
                return ESP_ERR_NO_MEM;
            }
            memcpy(results[i]->bssid.data, scan_result.bssid, results[i]->bssid.len);
        }
#else // CONFIG_NETWORK_PROV_NETWORK_TYPE_WIFI
        resp->status = STATUS__InvalidArgument;
#endif // !CONFIG_NETWORK_PROV_NETWORK_TYPE_WIFI
    } else if (req->msg == NETWORK_SCAN_MSG_TYPE__TypeCmdScanThreadResult) {
        RespScanThreadResult *resp_payload = (RespScanThreadResult *) malloc(sizeof(RespScanThreadResult));
        if (!resp_payload) {
            ESP_LOGE(TAG, "Error allocating memory");
            return ESP_ERR_NO_MEM;
        }
        resp_scan_thread_result__init(resp_payload);
        resp->status = STATUS__Success;
        resp->payload_case = NETWORK_SCAN_PAYLOAD__PAYLOAD_RESP_SCAN_THREAD_RESULT;
        resp->resp_scan_thread_result = resp_payload;
#ifdef CONFIG_NETWORK_PROV_NETWORK_TYPE_THREAD
        network_prov_scan_thread_result_t scan_result;
        memset(&scan_result, 0, sizeof(scan_result));
        ThreadScanResult **results = NULL;

        /* Allocate memory only if there are non-zero scan results */
        if (req->cmd_scan_thread_result->count) {
            results = (ThreadScanResult **) calloc(req->cmd_scan_thread_result->count,
                                                   sizeof(ThreadScanResult *));
            if (!results) {
                ESP_LOGE(TAG, "Failed to allocate memory for results array");
                return ESP_ERR_NO_MEM;
            }
        }
        resp_payload->entries = results;
        resp_payload->n_entries = req->cmd_scan_thread_result->count;

        /* If req->cmd_scan_result->count is 0, the below loop will
        * be skipped.
        */
        for (uint16_t i = 0; i < req->cmd_scan_thread_result->count; i++) {
            if (!h->thread_scan_result) {
                resp->status = STATUS__InternalError;
                break;
            }
            err = h->thread_scan_result(i + req->cmd_scan_thread_result->start_index,
                                        &scan_result, &h->ctx);
            if (err != ESP_OK) {
                resp->status = STATUS__InternalError;
                break;
            }

            results[i] = (ThreadScanResult *) malloc(sizeof(ThreadScanResult));
            if (!results[i]) {
                ESP_LOGE(TAG, "Failed to allocate memory for result entry");
                return ESP_ERR_NO_MEM;
            }
            thread_scan_result__init(results[i]);
            results[i]->pan_id = scan_result.pan_id;
            results[i]->channel = scan_result.channel;
            results[i]->rssi = scan_result.rssi;
            results[i]->lqi = scan_result.lqi;

            results[i]->ext_addr.len = sizeof(scan_result.ext_addr);
            results[i]->ext_addr.data = (uint8_t *)malloc(results[i]->ext_addr.len);
            if (!results[i]->ext_addr.data) {
                ESP_LOGE(TAG, "Failed to allocate memory for scan result entry extended address");
                return ESP_ERR_NO_MEM;
            }
            memcpy(results[i]->ext_addr.data, scan_result.ext_addr, results[i]->ext_addr.len);

            results[i]->ext_pan_id.len = sizeof(scan_result.ext_pan_id);
            results[i]->ext_pan_id.data = (uint8_t *)malloc(results[i]->ext_pan_id.len);
            if (!results[i]->ext_pan_id.data) {
                ESP_LOGE(TAG, "Failed to allocate memory for scan result entry extended PAN ID");
                return ESP_ERR_NO_MEM;
            }
            memcpy(results[i]->ext_pan_id.data, scan_result.ext_pan_id, results[i]->ext_pan_id.len);

            results[i]->network_name = (char *)malloc(sizeof(scan_result.network_name));
            if (!results[i]->network_name) {
                ESP_LOGE(TAG, "Failed to allocate memory for scan result entry networkname");
            }
            memcpy(results[i]->network_name, scan_result.network_name, sizeof(scan_result.network_name));
        }
#else // CONFIG_NETWORK_PROV_NETWORK_TYPE_THREAD
        resp->status = STATUS__InvalidArgument;
#endif // !CONFIG_NETWORK_PROV_NETWORK_TYPE_THREAD
    }
    return ESP_OK;
}


static int lookup_cmd_handler(int cmd_id)
{
    for (size_t i = 0; i < sizeof(cmd_table) / sizeof(network_prov_scan_cmd_t); i++) {
        if (cmd_table[i].cmd_num == cmd_id) {
            return i;
        }
    }

    return -1;
}

static void network_prov_scan_cmd_cleanup(NetworkScanPayload *resp, void *priv_data)
{
    switch (resp->msg) {
    case NETWORK_SCAN_MSG_TYPE__TypeRespScanWifiStart: {
        free(resp->resp_scan_wifi_start);
    }
    break;
    case NETWORK_SCAN_MSG_TYPE__TypeRespScanThreadStart: {
        free(resp->resp_scan_thread_start);
    }
    break;
    case NETWORK_SCAN_MSG_TYPE__TypeRespScanWifiStatus: {
        free(resp->resp_scan_wifi_status);
    }
    break;
    case NETWORK_SCAN_MSG_TYPE__TypeRespScanThreadStatus: {
        free(resp->resp_scan_thread_status);
    }
    break;

    case NETWORK_SCAN_MSG_TYPE__TypeRespScanWifiResult: {
        if (!resp->resp_scan_wifi_result) {
            return;
        }
#ifdef CONFIG_NETWORK_PROV_NETWORK_TYPE_WIFI
        if (resp->resp_scan_wifi_result->entries) {
            for (uint16_t i = 0; i < resp->resp_scan_wifi_result->n_entries; i++) {
                if (!resp->resp_scan_wifi_result->entries[i]) {
                    continue;
                }
                free(resp->resp_scan_wifi_result->entries[i]->ssid.data);
                free(resp->resp_scan_wifi_result->entries[i]->bssid.data);
            }
            free(resp->resp_scan_wifi_result->entries);
        }
#endif // CONFIG_NETWORK_PROV_NETWORK_TYPE_WIFI
        free(resp->resp_scan_wifi_result);
    }
    break;
    case NETWORK_SCAN_MSG_TYPE__TypeRespScanThreadResult: {
        if (!resp->resp_scan_thread_result) {
            return;
        }
#ifdef CONFIG_NETWORK_PROV_NETWORK_TYPE_THREAD
        if (resp->resp_scan_thread_result->entries) {
            for (uint16_t i = 0; i < resp->resp_scan_thread_result->n_entries; i++) {
                if (!resp->resp_scan_thread_result->entries[i]) {
                    continue;
                }
                free(resp->resp_scan_thread_result->entries[i]->ext_addr.data);
                free(resp->resp_scan_thread_result->entries[i]->ext_pan_id.data);
                free(resp->resp_scan_thread_result->entries[i]->network_name);
                free(resp->resp_scan_thread_result->entries[i]);
            }
            free(resp->resp_scan_thread_result->entries);
        }
#endif // CONFIG_NETWORK_PROV_NETWORK_TYPE_THREAD
        free(resp->resp_scan_thread_result);
    }
    break;
    default:
        ESP_LOGE(TAG, "Unsupported response type in cleanup_handler");
        break;
    }
    return;
}

static esp_err_t network_prov_scan_cmd_dispatcher(NetworkScanPayload *req,
        NetworkScanPayload *resp, void *priv_data)
{
    esp_err_t ret;

    ESP_LOGD(TAG, "In network_prov_scan_cmd_dispatcher Cmd=%d", req->msg);

    int cmd_index = lookup_cmd_handler(req->msg);
    if (cmd_index < 0) {
        ESP_LOGE(TAG, "Invalid command handler lookup");
        return ESP_FAIL;
    }

    ret = cmd_table[cmd_index].command_handler(req, resp, priv_data);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Error executing command handler");
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t network_prov_scan_handler(uint32_t session_id, const uint8_t *inbuf, ssize_t inlen,
                                    uint8_t **outbuf, ssize_t *outlen, void *priv_data)
{
    NetworkScanPayload *req;
    NetworkScanPayload resp;
    esp_err_t ret = ESP_OK;

    req = network_scan_payload__unpack(NULL, inlen, inbuf);
    if (!req) {
        ESP_LOGE(TAG, "Unable to unpack scan message");
        return ESP_ERR_INVALID_ARG;
    }

    network_scan_payload__init(&resp);
    ret = network_prov_scan_cmd_dispatcher(req, &resp, priv_data);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Command dispatcher error %d", ret);
        ret = ESP_FAIL;
        goto exit;
    }

    resp.msg = req->msg + 1; /* Response is request + 1 */
    *outlen = network_scan_payload__get_packed_size(&resp);
    if (*outlen <= 0) {
        ESP_LOGE(TAG, "Invalid encoding for response");
        ret = ESP_FAIL;
        goto exit;
    }

    *outbuf = (uint8_t *) malloc(*outlen);
    if (!*outbuf) {
        ESP_LOGE(TAG, "System out of memory");
        ret = ESP_ERR_NO_MEM;
        goto exit;
    }
    network_scan_payload__pack(&resp, *outbuf);
    ESP_LOGD(TAG, "Response packet size : %d", *outlen);
exit:

    network_scan_payload__free_unpacked(req, NULL);
    network_prov_scan_cmd_cleanup(&resp, priv_data);
    return ret;
}
