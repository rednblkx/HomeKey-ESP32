/*
 * SPDX-FileCopyrightText: 2018-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <string.h>
#include <esp_err.h>
#include <esp_log.h>

#include "network_constants.pb-c.h"
#include "network_config.pb-c.h"

#include <network_provisioning/network_config.h>

static const char *TAG = "NetworkProvConfig";

typedef struct network_prov_config_cmd {
    int cmd_num;
    esp_err_t (*command_handler)(NetworkConfigPayload *req,
                                 NetworkConfigPayload *resp, void *priv_data);
} network_prov_config_cmd_t;

static esp_err_t cmd_get_status_handler(NetworkConfigPayload *req,
                                        NetworkConfigPayload *resp, void *priv_data);

static esp_err_t cmd_set_config_handler(NetworkConfigPayload *req,
                                        NetworkConfigPayload *resp, void *priv_data);

static esp_err_t cmd_apply_config_handler(NetworkConfigPayload *req,
        NetworkConfigPayload *resp, void *priv_data);

static network_prov_config_cmd_t cmd_table[] = {
    {
        .cmd_num = NETWORK_CONFIG_MSG_TYPE__TypeCmdGetWifiStatus,
        .command_handler = cmd_get_status_handler
    },
    {
        .cmd_num = NETWORK_CONFIG_MSG_TYPE__TypeCmdSetWifiConfig,
        .command_handler = cmd_set_config_handler
    },
    {
        .cmd_num = NETWORK_CONFIG_MSG_TYPE__TypeCmdApplyWifiConfig,
        .command_handler = cmd_apply_config_handler
    },
    {
        .cmd_num = NETWORK_CONFIG_MSG_TYPE__TypeCmdGetThreadStatus,
        .command_handler = cmd_get_status_handler
    },
    {
        .cmd_num = NETWORK_CONFIG_MSG_TYPE__TypeCmdSetThreadConfig,
        .command_handler = cmd_set_config_handler
    },
    {
        .cmd_num = NETWORK_CONFIG_MSG_TYPE__TypeCmdApplyThreadConfig,
        .command_handler = cmd_apply_config_handler
    }
};

static esp_err_t cmd_get_status_handler(NetworkConfigPayload *req,
                                        NetworkConfigPayload *resp, void *priv_data)
{
    ESP_LOGD(TAG, "Enter cmd_get_status_handler");
    network_prov_config_handlers_t *h = (network_prov_config_handlers_t *) priv_data;
    if (!h) {
        ESP_LOGE(TAG, "Command invoked without handlers");
        return ESP_ERR_INVALID_STATE;
    }

    if (req->msg == NETWORK_CONFIG_MSG_TYPE__TypeCmdGetWifiStatus) {
        RespGetWifiStatus *resp_payload = (RespGetWifiStatus *) malloc(sizeof(RespGetWifiStatus));
        if (!resp_payload) {
            ESP_LOGE(TAG, "Error allocating memory");
            return ESP_ERR_NO_MEM;
        }
        resp_get_wifi_status__init(resp_payload);
#ifdef CONFIG_NETWORK_PROV_NETWORK_TYPE_WIFI
        network_prov_config_get_wifi_data_t resp_data;
        if (h->wifi_get_status_handler) {
            if (h->wifi_get_status_handler(&resp_data, &h->ctx) == ESP_OK) {
                if (resp_data.wifi_state == NETWORK_PROV_WIFI_STA_CONNECTING) {
                    resp_payload->wifi_sta_state = WIFI_STATION_STATE__Connecting;
                    resp_payload->state_case = RESP_GET_WIFI_STATUS__STATE_WIFI_CONNECTED;
                } else if (resp_data.wifi_state == NETWORK_PROV_WIFI_STA_CONNECTED) {
                    resp_payload->wifi_sta_state  = WIFI_STATION_STATE__Connected;
                    resp_payload->state_case = RESP_GET_WIFI_STATUS__STATE_WIFI_CONNECTED;
                    WifiConnectedState *connected = (WifiConnectedState *)(
                                                        malloc(sizeof(WifiConnectedState)));
                    if (!connected) {
                        free(resp_payload);
                        ESP_LOGE(TAG, "Error allocating memory");
                        return ESP_ERR_NO_MEM;
                    }
                    resp_payload->wifi_connected  = connected;
                    wifi_connected_state__init(connected);

                    connected->ip4_addr = strdup(resp_data.conn_info.ip_addr);
                    if (connected->ip4_addr == NULL) {
                        free(connected);
                        free(resp_payload);
                        return ESP_ERR_NO_MEM;
                    }

                    connected->bssid.len  = sizeof(resp_data.conn_info.bssid);
                    connected->bssid.data = (uint8_t *) strndup(resp_data.conn_info.bssid,
                                            sizeof(resp_data.conn_info.bssid));
                    if (connected->bssid.data == NULL) {
                        free(connected->ip4_addr);
                        free(connected);
                        free(resp_payload);
                        return ESP_ERR_NO_MEM;
                    }

                    connected->ssid.len   = strlen(resp_data.conn_info.ssid);
                    connected->ssid.data  = (uint8_t *) strdup(resp_data.conn_info.ssid);
                    if (connected->ssid.data == NULL) {
                        free(connected->bssid.data);
                        free(connected->ip4_addr);
                        free(connected);
                        free(resp_payload);
                        return ESP_ERR_NO_MEM;
                    }

                    connected->channel    = resp_data.conn_info.channel;
                    connected->auth_mode  = resp_data.conn_info.auth_mode;
                } else if (resp_data.wifi_state == NETWORK_PROV_WIFI_STA_DISCONNECTED) {
                    resp_payload->wifi_sta_state = WIFI_STATION_STATE__ConnectionFailed;
                    resp_payload->state_case = RESP_GET_WIFI_STATUS__STATE_WIFI_FAIL_REASON;

                    if (resp_data.fail_reason == NETWORK_PROV_WIFI_STA_AUTH_ERROR) {
                        resp_payload->wifi_fail_reason = WIFI_CONNECT_FAILED_REASON__AuthError;
                    } else if (resp_data.fail_reason == NETWORK_PROV_WIFI_STA_AP_NOT_FOUND) {
                        resp_payload->wifi_fail_reason = WIFI_CONNECT_FAILED_REASON__WifiNetworkNotFound;
                    }
                }
                resp_payload->status = STATUS__Success;
            } else {
                resp_payload->status = STATUS__InternalError;
            }
        } else {
            resp_payload->status = STATUS__InternalError;
        }
#else // CONFIG_NETWORK_PROV_NETWORK_TYPE_WIFI
        resp_payload->status = STATUS__InvalidArgument;
#endif // !CONFIG_NETWORK_PROV_NETWORK_TYPE_WIFI
        resp->payload_case = NETWORK_CONFIG_PAYLOAD__PAYLOAD_RESP_GET_WIFI_STATUS;
        resp->resp_get_wifi_status = resp_payload;
    } else if (req->msg == NETWORK_CONFIG_MSG_TYPE__TypeCmdGetThreadStatus) {
        RespGetThreadStatus *resp_payload = (RespGetThreadStatus *) malloc(sizeof(RespGetThreadStatus));
        if (!resp_payload) {
            ESP_LOGE(TAG, "Error allocating memory");
            return ESP_ERR_NO_MEM;
        }
        resp_get_thread_status__init(resp_payload);
#ifdef CONFIG_NETWORK_PROV_NETWORK_TYPE_THREAD
        network_prov_config_get_thread_data_t resp_data;
        if (h->thread_get_status_handler) {
            if (h->thread_get_status_handler(&resp_data, &h->ctx) == ESP_OK) {
                if (resp_data.thread_state == NETWORK_PROV_THREAD_ATTACHING) {
                    resp_payload->thread_state = THREAD_NETWORK_STATE__Attaching;
                    resp_payload->state_case = RESP_GET_THREAD_STATUS__STATE_THREAD_ATTACHED;
                } else if (resp_data.thread_state == NETWORK_PROV_THREAD_ATTACHED) {
                    resp_payload->thread_state  = THREAD_NETWORK_STATE__Attached;
                    resp_payload->state_case = RESP_GET_THREAD_STATUS__STATE_THREAD_ATTACHED;
                    ThreadAttachState *attached = (ThreadAttachState *)malloc(sizeof(ThreadAttachState));
                    if (!attached) {
                        free(resp_payload);
                        ESP_LOGE(TAG, "Error allocating memory");
                        return ESP_ERR_NO_MEM;
                    }
                    resp_payload->thread_attached  = attached;
                    thread_attach_state__init(attached);
                    attached->channel = resp_data.conn_info.channel;
                    attached->ext_pan_id.len = sizeof(resp_data.conn_info.ext_pan_id);
                    attached->ext_pan_id.data = (uint8_t *)malloc(attached->ext_pan_id.len);
                    if (!attached->ext_pan_id.data) {
                        free(attached);
                        free(resp_payload);
                        return ESP_ERR_NO_MEM;
                    }
                    memcpy(attached->ext_pan_id.data, resp_data.conn_info.ext_pan_id, sizeof(resp_data.conn_info.ext_pan_id));
                    attached->pan_id = resp_data.conn_info.pan_id;

                    attached->name = (char *)malloc(sizeof(resp_data.conn_info.name));
                    if (!attached->name) {
                        free(attached->ext_pan_id.data);
                        free(attached);
                        free(resp_payload);
                        return ESP_ERR_NO_MEM;
                    }
                    memcpy(attached->name, resp_data.conn_info.name, sizeof(resp_data.conn_info.name));
                } else if (resp_data.thread_state == NETWORK_PROV_THREAD_DETACHED) {
                    resp_payload->thread_state = THREAD_NETWORK_STATE__AttachingFailed;
                    resp_payload->state_case = RESP_GET_THREAD_STATUS__STATE_THREAD_FAIL_REASON;

                    if (resp_data.fail_reason == NETWORK_PROV_THREAD_DATASET_INVALID) {
                        resp_payload->thread_fail_reason = THREAD_ATTACH_FAILED_REASON__DatasetInvalid;
                    } else if (resp_data.fail_reason == NETWORK_PROV_THREAD_NETWORK_NOT_FOUND) {
                        resp_payload->thread_fail_reason = THREAD_ATTACH_FAILED_REASON__ThreadNetworkNotFound;
                    }
                }
                resp_payload->status = STATUS__Success;
            } else {
                resp_payload->status = STATUS__InternalError;
            }
        } else {
            resp_payload->status = STATUS__InternalError;
        }
#else // CONFIG_NETWORK_PROV_NETWORK_TYPE_THREAD
        resp_payload->status = STATUS__InvalidArgument;
#endif // !CONFIG_NETWORK_PROV_NETWORK_TYPE_THREAD
        resp->payload_case = NETWORK_CONFIG_PAYLOAD__PAYLOAD_RESP_GET_THREAD_STATUS;
        resp->resp_get_thread_status = resp_payload;
    }
    return ESP_OK;
}

static esp_err_t cmd_set_config_handler(NetworkConfigPayload *req,
                                        NetworkConfigPayload *resp, void  *priv_data)
{
    ESP_LOGD(TAG, "Enter cmd_set_config_handler");
    network_prov_config_handlers_t *h = (network_prov_config_handlers_t *) priv_data;
    if (!h) {
        ESP_LOGE(TAG, "Command invoked without handlers");
        return ESP_ERR_INVALID_STATE;
    }

    if (req->msg == NETWORK_CONFIG_MSG_TYPE__TypeCmdSetWifiConfig) {
        RespSetWifiConfig *resp_payload = (RespSetWifiConfig *) malloc(sizeof(RespSetWifiConfig));
        if (resp_payload == NULL) {
            ESP_LOGE(TAG, "Error allocating memory");
            return ESP_ERR_NO_MEM;
        }
        resp_set_wifi_config__init(resp_payload);
#ifdef CONFIG_NETWORK_PROV_NETWORK_TYPE_WIFI
        network_prov_config_set_wifi_data_t req_data;
        memset(&req_data, 0, sizeof(req_data));

        /* Check arguments provided in protobuf packet:
         * - SSID / Passphrase string length must be within the standard limits
         * - BSSID must either be NULL or have length equal to that imposed by the standard
         * If any of these conditions are not satisfied, don't invoke the handler and
         * send error status without closing connection */
        resp_payload->status = STATUS__InvalidArgument;
        if (req->cmd_set_wifi_config->bssid.len != 0 &&
                req->cmd_set_wifi_config->bssid.len != sizeof(req_data.bssid)) {
            ESP_LOGD(TAG, "Received invalid BSSID");
        } else if (req->cmd_set_wifi_config->ssid.len >= sizeof(req_data.ssid)) {
            ESP_LOGD(TAG, "Received invalid SSID");
        } else if (req->cmd_set_wifi_config->passphrase.len >= sizeof(req_data.password)) {
            ESP_LOGD(TAG, "Received invalid Passphrase");
        } else {
            /* The received SSID and Passphrase are not NULL terminated so
             * we memcpy over zeroed out arrays. Above length checks ensure
             * that there is atleast 1 extra byte for null termination */
            memcpy(req_data.ssid, req->cmd_set_wifi_config->ssid.data,
                   req->cmd_set_wifi_config->ssid.len);
            memcpy(req_data.password, req->cmd_set_wifi_config->passphrase.data,
                   req->cmd_set_wifi_config->passphrase.len);
            memcpy(req_data.bssid, req->cmd_set_wifi_config->bssid.data,
                   req->cmd_set_wifi_config->bssid.len);
            req_data.channel = req->cmd_set_wifi_config->channel;
            if (h->wifi_set_config_handler(&req_data, &h->ctx) == ESP_OK) {
                resp_payload->status = STATUS__Success;
            } else {
                resp_payload->status = STATUS__InternalError;
            }
        }
#else // CONFIG_NETWORK_PROV_NETWORK_TYPE_WIFI
        resp_payload->status = STATUS__InvalidArgument;
#endif // !CONFIG_NETWORK_PROV_NETWORK_TYPE_WIFI
        resp->payload_case = NETWORK_CONFIG_PAYLOAD__PAYLOAD_RESP_SET_WIFI_CONFIG;
        resp->resp_set_wifi_config = resp_payload;
    } else if (req->msg == NETWORK_CONFIG_MSG_TYPE__TypeCmdSetThreadConfig) {
        RespSetThreadConfig *resp_payload = (RespSetThreadConfig *) malloc(sizeof(RespSetThreadConfig));
        if (resp_payload == NULL) {
            ESP_LOGE(TAG, "Error allocating memory");
            return ESP_ERR_NO_MEM;
        }
        resp_set_thread_config__init(resp_payload);
#ifdef CONFIG_NETWORK_PROV_NETWORK_TYPE_THREAD
        network_prov_config_set_thread_data_t req_data;
        memset(&req_data, 0, sizeof(req_data));
        resp_payload->status = STATUS__InvalidArgument;
        if (req->cmd_set_thread_config->dataset.len > sizeof(req_data.dataset)) {
            ESP_LOGD(TAG, "Received invalid dataset");
        }
        memcpy(req_data.dataset, req->cmd_set_thread_config->dataset.data,
               req->cmd_set_thread_config->dataset.len);
        req_data.length = req->cmd_set_thread_config->dataset.len;
        if (h->thread_set_config_handler(&req_data, &h->ctx) == ESP_OK) {
            resp_payload->status = STATUS__Success;
        } else {
            resp_payload->status = STATUS__InternalError;
        }
#else // CONFIG_NETWORK_PROV_NETWORK_TYPE_THREAD
        resp_payload->status = STATUS__InvalidArgument;
#endif // !CONFIG_NETWORK_PROV_NETWORK_TYPE_THREAD
        resp->payload_case = NETWORK_CONFIG_PAYLOAD__PAYLOAD_RESP_SET_THREAD_CONFIG;
        resp->resp_set_thread_config = resp_payload;
    }
    return ESP_OK;
}

static esp_err_t cmd_apply_config_handler(NetworkConfigPayload *req,
        NetworkConfigPayload *resp, void  *priv_data)
{
    ESP_LOGD(TAG, "Enter cmd_apply_config_handler");
    network_prov_config_handlers_t *h = (network_prov_config_handlers_t *) priv_data;
    if (!h) {
        ESP_LOGE(TAG, "Command invoked without handlers");
        return ESP_ERR_INVALID_STATE;
    }
    if (req->msg == NETWORK_CONFIG_MSG_TYPE__TypeCmdApplyWifiConfig) {
        RespApplyWifiConfig *resp_payload = (RespApplyWifiConfig *) malloc(sizeof(RespApplyWifiConfig));
        if (!resp_payload) {
            ESP_LOGE(TAG, "Error allocating memory");
            return ESP_ERR_NO_MEM;
        }
        resp_apply_wifi_config__init(resp_payload);
#ifdef CONFIG_NETWORK_PROV_NETWORK_TYPE_WIFI
        if (h->wifi_apply_config_handler && h->wifi_apply_config_handler(&h->ctx) == ESP_OK) {
            resp_payload->status = STATUS__Success;
        } else {
            resp_payload->status = STATUS__InternalError;
        }
#else // CONFIG_NETWORK_PROV_NETWORK_TYPE_WIFI
        resp_payload->status = STATUS__InvalidArgument;
#endif // !CONFIG_NETWORK_PROV_NETWORK_TYPE_WIFI
        resp->payload_case = NETWORK_CONFIG_PAYLOAD__PAYLOAD_RESP_APPLY_WIFI_CONFIG;
        resp->resp_apply_wifi_config = resp_payload;
    } else if (req->msg == NETWORK_CONFIG_MSG_TYPE__TypeCmdApplyThreadConfig) {
        RespApplyThreadConfig *resp_payload = (RespApplyThreadConfig *) malloc(sizeof(RespApplyThreadConfig));
        if (!resp_payload) {
            ESP_LOGE(TAG, "Error allocating memory");
            return ESP_ERR_NO_MEM;
        }
        resp_apply_thread_config__init(resp_payload);
#ifdef CONFIG_NETWORK_PROV_NETWORK_TYPE_THREAD
        if (h->thread_apply_config_handler && h->thread_apply_config_handler(&h->ctx) == ESP_OK) {
            resp_payload->status = STATUS__Success;
        } else {
            resp_payload->status = STATUS__InternalError;
        }
#else // CONFIG_NETWORK_PROV_NETWORK_TYPE_THREAD
        resp_payload->status = STATUS__InvalidArgument;
#endif // !CONFIG_NETWORK_PROV_NETWORK_TYPE_THREAD
        resp->payload_case = NETWORK_CONFIG_PAYLOAD__PAYLOAD_RESP_APPLY_THREAD_CONFIG;
        resp->resp_apply_thread_config = resp_payload;
    }
    return ESP_OK;
}

static int lookup_cmd_handler(int cmd_id)
{
    for (size_t i = 0; i < sizeof(cmd_table) / sizeof(network_prov_config_cmd_t); i++) {
        if (cmd_table[i].cmd_num == cmd_id) {
            return i;
        }
    }

    return -1;
}
static void network_prov_config_command_cleanup(NetworkConfigPayload *resp, void *priv_data)
{
    if (!resp) {
        return;
    }

    switch (resp->msg) {
    case NETWORK_CONFIG_MSG_TYPE__TypeRespGetWifiStatus: {
#ifdef CONFIG_NETWORK_PROV_NETWORK_TYPE_WIFI
        switch (resp->resp_get_wifi_status->wifi_sta_state) {
        case WIFI_STATION_STATE__Connecting:
            break;
        case WIFI_STATION_STATE__Connected:
            if (resp->resp_get_wifi_status->wifi_connected) {
                if (resp->resp_get_wifi_status->wifi_connected->ip4_addr) {
                    free(resp->resp_get_wifi_status->wifi_connected->ip4_addr);
                }
                if (resp->resp_get_wifi_status->wifi_connected->bssid.data) {
                    free(resp->resp_get_wifi_status->wifi_connected->bssid.data);
                }
                if (resp->resp_get_wifi_status->wifi_connected->ssid.data) {
                    free(resp->resp_get_wifi_status->wifi_connected->ssid.data);
                }
                free(resp->resp_get_wifi_status->wifi_connected);
            }
            break;
        case WIFI_STATION_STATE__ConnectionFailed:
            break;
        default:
            break;
        }
#endif // CONFIG_NETWORK_PROV_NETWORK_TYPE_WIFI
        free(resp->resp_get_wifi_status);
    }
    break;
    case NETWORK_CONFIG_MSG_TYPE__TypeRespGetThreadStatus: {
#ifdef CONFIG_NETWORK_PROV_NETWORK_TYPE_THREAD
        switch (resp->resp_get_thread_status->thread_state) {
        case THREAD_NETWORK_STATE__Attaching:
            break;
        case THREAD_NETWORK_STATE__Attached:
            if (resp->resp_get_thread_status->thread_attached) {
                if (resp->resp_get_thread_status->thread_attached->name) {
                    free(resp->resp_get_thread_status->thread_attached->name);
                }
                free(resp->resp_get_thread_status->thread_attached);
            }
            break;
        case THREAD_NETWORK_STATE__AttachingFailed:
            break;
        default:
            break;
        }
#endif // CONFIG_NETWORK_PROV_NETWORK_TYPE_THREAD
        free(resp->resp_get_thread_status);
    }
    break;
    case NETWORK_CONFIG_MSG_TYPE__TypeRespSetWifiConfig: {
        free(resp->resp_set_wifi_config);
    }
    break;
    case NETWORK_CONFIG_MSG_TYPE__TypeRespSetThreadConfig: {
        free(resp->resp_set_thread_config);
    }
    break;
    case NETWORK_CONFIG_MSG_TYPE__TypeRespApplyWifiConfig: {
        free(resp->resp_apply_wifi_config);
    }
    break;
    case NETWORK_CONFIG_MSG_TYPE__TypeRespApplyThreadConfig: {
        free(resp->resp_apply_thread_config);
    }
    break;
    default:
        ESP_LOGE(TAG, "Unsupported response type in cleanup_handler");
        break;
    }
    return;
}

static esp_err_t network_prov_config_command_dispatcher(NetworkConfigPayload *req,
        NetworkConfigPayload *resp, void *priv_data)
{
    esp_err_t ret;

    ESP_LOGD(TAG, "In network_prov_config_command_dispatcher Cmd=%d", req->msg);

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

esp_err_t network_prov_config_data_handler(uint32_t session_id, const uint8_t *inbuf, ssize_t inlen,
        uint8_t **outbuf, ssize_t *outlen, void *priv_data)
{
    NetworkConfigPayload *req;
    NetworkConfigPayload resp;
    esp_err_t ret;

    req = network_config_payload__unpack(NULL, inlen, inbuf);
    if (!req) {
        ESP_LOGE(TAG, "Unable to unpack config data");
        return ESP_ERR_INVALID_ARG;
    }

    network_config_payload__init(&resp);
    ret = network_prov_config_command_dispatcher(req, &resp, priv_data);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Proto command dispatcher error %d", ret);
        return ESP_FAIL;
    }

    resp.msg = req->msg + 1; /* Response is request + 1 */
    network_config_payload__free_unpacked(req, NULL);

    *outlen = network_config_payload__get_packed_size(&resp);
    if (*outlen <= 0) {
        ESP_LOGE(TAG, "Invalid encoding for response");
        return ESP_FAIL;
    }

    *outbuf = (uint8_t *) malloc(*outlen);
    if (!*outbuf) {
        ESP_LOGE(TAG, "System out of memory");
        return ESP_ERR_NO_MEM;
    }
    network_config_payload__pack(&resp, *outbuf);
    network_prov_config_command_cleanup(&resp, priv_data);

    return ESP_OK;
}
