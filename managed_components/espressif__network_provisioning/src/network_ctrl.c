/*
 * SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <esp_log.h>
#include <string.h>
#include <esp_err.h>

#include "network_ctrl.pb-c.h"

#include "network_ctrl.h"

static const char *TAG = "proto_network_ctrl";

typedef struct network_ctrl_cmd {
    int cmd_id;
    esp_err_t (*command_handler)(NetworkCtrlPayload *req,
                                 NetworkCtrlPayload *resp, void *priv_data);
} network_ctrl_cmd_t;

static esp_err_t cmd_ctrl_reset_handler(NetworkCtrlPayload *req,
                                        NetworkCtrlPayload *resp,
                                        void *priv_data);

static esp_err_t cmd_ctrl_reprov_handler(NetworkCtrlPayload *req,
        NetworkCtrlPayload *resp,
        void *priv_data);

static network_ctrl_cmd_t cmd_table[] = {
    {
        .cmd_id = NETWORK_CTRL_MSG_TYPE__TypeCmdCtrlWifiReset,
        .command_handler = cmd_ctrl_reset_handler
    },
    {
        .cmd_id = NETWORK_CTRL_MSG_TYPE__TypeCmdCtrlThreadReset,
        .command_handler = cmd_ctrl_reset_handler
    },
    {
        .cmd_id = NETWORK_CTRL_MSG_TYPE__TypeCmdCtrlWifiReprov,
        .command_handler = cmd_ctrl_reprov_handler
    },
    {
        .cmd_id = NETWORK_CTRL_MSG_TYPE__TypeCmdCtrlThreadReprov,
        .command_handler = cmd_ctrl_reprov_handler
    },
};

static esp_err_t cmd_ctrl_reset_handler(NetworkCtrlPayload *req,
                                        NetworkCtrlPayload *resp, void *priv_data)
{
    network_ctrl_handlers_t *h = (network_ctrl_handlers_t *) priv_data;
    if (!h) {
        ESP_LOGE(TAG, "Command invoked without handlers");
        return ESP_ERR_INVALID_STATE;
    }
    if (req->msg == NETWORK_CTRL_MSG_TYPE__TypeCmdCtrlWifiReset) {
        RespCtrlWifiReset *resp_payload = (RespCtrlWifiReset *) malloc(sizeof(RespCtrlWifiReset));
        if (!resp_payload) {
            ESP_LOGE(TAG, "Error allocating memory");
            return ESP_ERR_NO_MEM;
        }
        resp_ctrl_wifi_reset__init(resp_payload);
#ifdef CONFIG_NETWORK_PROV_NETWORK_TYPE_WIFI
        if (h->wifi_ctrl_reset) {
            resp->status = (h->wifi_ctrl_reset() == ESP_OK ? STATUS__Success : STATUS__InternalError);
        } else {
            resp->status = STATUS__InternalError;
        }
#else
        resp->status = STATUS__InvalidArgument;
#endif
        resp->payload_case = NETWORK_CTRL_PAYLOAD__PAYLOAD_RESP_CTRL_WIFI_RESET;
        resp->resp_ctrl_wifi_reset = resp_payload;
    } else if (req->msg == NETWORK_CTRL_MSG_TYPE__TypeCmdCtrlThreadReset) {
        RespCtrlThreadReset *resp_payload = (RespCtrlThreadReset *) malloc(sizeof(RespCtrlThreadReset));
        if (!resp_payload) {
            ESP_LOGE(TAG, "Error allocating memory");
            return ESP_ERR_NO_MEM;
        }
        resp_ctrl_thread_reset__init(resp_payload);
#ifdef CONFIG_NETWORK_PROV_NETWORK_TYPE_THREAD
        if (h->thread_ctrl_reset) {
            resp->status = (h->thread_ctrl_reset() == ESP_OK ? STATUS__Success : STATUS__InternalError);
        } else {
            resp->status = STATUS__InternalError;
        }
#else
        resp->status = STATUS__InvalidArgument;
#endif
        resp->payload_case = NETWORK_CTRL_PAYLOAD__PAYLOAD_RESP_CTRL_THREAD_RESET;
        resp->resp_ctrl_thread_reset = resp_payload;
    }
    return ESP_OK;
}

static esp_err_t cmd_ctrl_reprov_handler(NetworkCtrlPayload *req,
        NetworkCtrlPayload *resp, void *priv_data)
{
    network_ctrl_handlers_t *h = (network_ctrl_handlers_t *) priv_data;
    if (!h) {
        ESP_LOGE(TAG, "Command invoked without handlers");
        return ESP_ERR_INVALID_STATE;
    }
    if (req->msg == NETWORK_CTRL_MSG_TYPE__TypeCmdCtrlWifiReprov) {
        RespCtrlWifiReprov *resp_payload = (RespCtrlWifiReprov *) malloc(sizeof(RespCtrlWifiReprov));
        if (!resp_payload) {
            ESP_LOGE(TAG, "Error allocating memory");
            return ESP_ERR_NO_MEM;
        }
        resp_ctrl_wifi_reprov__init(resp_payload);
#ifdef CONFIG_NETWORK_PROV_NETWORK_TYPE_WIFI
        if (h->wifi_ctrl_reprov) {
            resp->status = (h->wifi_ctrl_reprov() == ESP_OK ? STATUS__Success : STATUS__InternalError);
        } else {
            resp->status = STATUS__InternalError;
        }
#else
        resp->status = STATUS__InvalidArgument;
#endif
        resp->payload_case = NETWORK_CTRL_PAYLOAD__PAYLOAD_RESP_CTRL_WIFI_REPROV;
        resp->resp_ctrl_wifi_reprov = resp_payload;
    } else if (req->msg == NETWORK_CTRL_MSG_TYPE__TypeCmdCtrlThreadReprov) {
        RespCtrlThreadReprov *resp_payload = (RespCtrlThreadReprov *) malloc(sizeof(RespCtrlThreadReprov));
        if (!resp_payload) {
            ESP_LOGE(TAG, "Error allocating memory");
            return ESP_ERR_NO_MEM;
        }
        resp_ctrl_thread_reprov__init(resp_payload);
#ifdef CONFIG_NETWORK_PROV_NETWORK_TYPE_THREAD
        if (h->thread_ctrl_reprov) {
            resp->status = (h->thread_ctrl_reprov() == ESP_OK ? STATUS__Success : STATUS__InternalError);
        } else {
            resp->status = STATUS__InternalError;
        }
#else
        resp->status = STATUS__InvalidArgument;
#endif
        resp->payload_case = NETWORK_CTRL_PAYLOAD__PAYLOAD_RESP_CTRL_THREAD_REPROV;
        resp->resp_ctrl_thread_reprov = resp_payload;
    }
    return ESP_OK;
}

static int lookup_cmd_handler(int cmd_id)
{
    for (size_t i = 0; i < sizeof(cmd_table) / sizeof(network_ctrl_cmd_t); i++) {
        if (cmd_table[i].cmd_id == cmd_id) {
            return i;
        }
    }

    return -1;
}

static void network_ctrl_cmd_cleanup(NetworkCtrlPayload *resp, void *priv_data)
{
    switch (resp->msg) {
    case NETWORK_CTRL_MSG_TYPE__TypeRespCtrlWifiReset: {
        free(resp->resp_ctrl_wifi_reset);
    }
    break;
    case NETWORK_CTRL_MSG_TYPE__TypeRespCtrlWifiReprov: {
        free(resp->resp_ctrl_wifi_reprov);
    }
    break;
    case NETWORK_CTRL_MSG_TYPE__TypeRespCtrlThreadReset: {
        free(resp->resp_ctrl_thread_reset);
    }
    break;
    case NETWORK_CTRL_MSG_TYPE__TypeRespCtrlThreadReprov: {
        free(resp->resp_ctrl_thread_reprov);
    }
    break;
    default:
        ESP_LOGE(TAG, "Unsupported response type in cleanup_handler");
        break;
    }
    return;
}

static esp_err_t network_ctrl_cmd_dispatcher(NetworkCtrlPayload *req,
        NetworkCtrlPayload *resp, void *priv_data)
{
    esp_err_t ret;

    ESP_LOGD(TAG, "In network_ctrl_cmd_dispatcher Cmd=%d", req->msg);

    int cmd_index = lookup_cmd_handler(req->msg);
    if (cmd_index < 0) {
        ESP_LOGE(TAG, "Failed to find cmd with ID = %d in the command table", req->msg);
        return ESP_FAIL;
    }

    ret = cmd_table[cmd_index].command_handler(req, resp, priv_data);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Error executing command handler");
    }

    return ret;
}

esp_err_t network_ctrl_handler(uint32_t session_id, const uint8_t *inbuf, ssize_t inlen,
                               uint8_t **outbuf, ssize_t *outlen, void *priv_data)
{
    NetworkCtrlPayload *req;
    NetworkCtrlPayload resp;
    esp_err_t ret = ESP_OK;

    req = network_ctrl_payload__unpack(NULL, inlen, inbuf);
    if (!req) {
        ESP_LOGE(TAG, "Unable to unpack ctrl message");
        return ESP_ERR_INVALID_ARG;
    }

    network_ctrl_payload__init(&resp);
    ret = network_ctrl_cmd_dispatcher(req, &resp, priv_data);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Command dispatcher error %02X", ret);
        ret = ESP_FAIL;
        goto exit;
    }

    resp.msg = req->msg + 1; /* Response is request + 1 */
    *outlen = network_ctrl_payload__get_packed_size(&resp);
    if (*outlen <= 0) {
        ESP_LOGE(TAG, "Invalid encoding for response");
        ret = ESP_FAIL;
        goto exit;
    }

    *outbuf = (uint8_t *) malloc(*outlen);
    if (!*outbuf) {
        ESP_LOGE(TAG, "Failed to allocate memory for the output buffer");
        ret = ESP_ERR_NO_MEM;
        goto exit;
    }
    network_ctrl_payload__pack(&resp, *outbuf);
    ESP_LOGD(TAG, "Response packet size : %d", *outlen);
exit:

    network_ctrl_payload__free_unpacked(req, NULL);
    network_ctrl_cmd_cleanup(&resp, priv_data);
    return ret;
}
