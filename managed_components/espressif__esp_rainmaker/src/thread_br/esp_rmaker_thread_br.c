// Copyright 2024 Espressif Systems (Shanghai) PTE LTD
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

#include <esp_check.h>
#include <esp_err.h>
#include <esp_log.h>
#include <esp_rmaker_thread_br.h>
#include <esp_rmaker_core.h>
#include <esp_rmaker_utils.h>
#include <stdint.h>
#include <string.h>

#include "esp_rmaker_thread_br_priv.h"

static const char *TAG = "thread_br";
static const esp_rmaker_device_t *thread_br_service = NULL;

static int char_to_num(char ch)
{
    if (ch >= '0' && ch <= '9') {
        return ch - '0';
    } else if (ch >= 'A' && ch <= 'F') {
        return ch - 'A' + 10;
    } else if (ch >= 'a' && ch <= 'f') {
        return ch - 'a' + 10;
    }
    return -1;
}

static int parse_bytes(uint8_t *buffer, size_t buffer_size, const char *str)
{
    if (!str || strlen(str) % 2 != 0 || strlen(str) / 2 > buffer_size) {
        return -1;
    }
    for (size_t i = 0; i < strlen(str) / 2; ++i) {
        int byte_h = char_to_num(str[2 * i]);
        int byte_l = char_to_num(str[2 * i + 1]);
        if (byte_h < 0 || byte_l < 0) {
            return -1;
        }
        buffer[i] = (byte_h << 4) + byte_l;
    }
    return strlen(str) / 2;
}

static bool convert_bytes_to_str(const uint8_t *bytes, size_t bytes_size, char *str, size_t str_size)
{
    if (str_size < bytes_size * 2 + 1) {
        return false;
    }
    for (size_t i = 0; i < bytes_size; ++i) {
        uint8_t byte_h = bytes[i] >> 4;
        uint8_t byte_l = bytes[i] & 0xF;
        str[2 * i] = byte_h > 9 ? (byte_h - 0xA + 'A') : (byte_h + '0');
        str[2 * i + 1] = byte_l > 9 ? (byte_l - 0xA + 'A') : (byte_l + '0');
    }
    str[2 * bytes_size] = 0;
    return true;
}

static esp_err_t write_cb(const esp_rmaker_device_t *device, const esp_rmaker_param_t *param,
            const esp_rmaker_param_val_t val, void *priv_data, esp_rmaker_write_ctx_t *ctx)
{
    if (ctx->src == ESP_RMAKER_REQ_SRC_INIT) {
        return ESP_OK;
    }
    if (strcmp(esp_rmaker_param_get_type(param), ESP_RMAKER_PARAM_TBR_ACTIVE_DATASET) == 0) {
        if (val.type != RMAKER_VAL_TYPE_STRING || !val.val.s) {
            return ESP_ERR_INVALID_ARG;
        }
        otOperationalDatasetTlvs active_dataset;
        otOperationalDatasetTlvs current_active_dataset;
        int len = parse_bytes(active_dataset.mTlvs, sizeof(active_dataset.mTlvs), val.val.s);
        if (len < 0) {
            return ESP_ERR_INVALID_ARG;
        }
        active_dataset.mLength = len;
        if (thread_br_get_active_dataset_tlvs(&current_active_dataset) != ESP_OK ||
            current_active_dataset.mLength != active_dataset.mLength ||
            memcmp(current_active_dataset.mTlvs, active_dataset.mTlvs, len)) {
            ESP_RETURN_ON_ERROR(thread_br_set_thread_enabled(false), TAG, "Failed to disable Thread");
            ESP_RETURN_ON_ERROR(thread_br_set_active_dataset_tlvs(&active_dataset), TAG, "Failed to set active dataset");
        }
        ESP_RETURN_ON_ERROR(thread_br_set_thread_enabled(true), TAG, "Failed to enable Thread");
        ESP_RETURN_ON_ERROR(esp_rmaker_param_update_and_report(param, val), TAG,
                            "Failed to update and report active dataset");
    } else if (strcmp(esp_rmaker_param_get_type(param), ESP_RMAKER_PARAM_TBR_PENDING_DATASET) == 0) {
        if (val.type != RMAKER_VAL_TYPE_STRING || !val.val.s) {
            return ESP_ERR_INVALID_ARG;
        }
        otOperationalDatasetTlvs pending_dataset;
        int len = parse_bytes(pending_dataset.mTlvs, sizeof(pending_dataset.mTlvs), val.val.s);
        if (len < 0) {
            return ESP_ERR_INVALID_ARG;
        }
        pending_dataset.mLength = len;
        ESP_RETURN_ON_ERROR(thread_br_set_pending_dataset_tlvs(&pending_dataset), TAG, "Failed to set pending dataset");
        ESP_RETURN_ON_ERROR(esp_rmaker_param_update_and_report(param, val), TAG,
                            "Failed to update and report active dataset");
    } else if (strcmp(esp_rmaker_param_get_type(param), ESP_RMAKER_PARAM_TBR_CMD) == 0) {
        if (val.type != RMAKER_VAL_TYPE_INTEGER) {
            return ESP_ERR_INVALID_ARG;
        }
        if (val.val.i == ESP_RMAKER_THREAD_BR_CMD_GEN_DATASET) {
            ESP_RETURN_ON_ERROR(thread_br_set_thread_enabled(false), TAG, "Failed to disable Thread");
            ESP_RETURN_ON_ERROR(thread_br_generate_and_commit_new_dataset(), TAG, "Failed to generate a new dataset");
            ESP_RETURN_ON_ERROR(thread_br_set_thread_enabled(true), TAG, "Failed to enable Thread");
            // We need to report the generated active dataset.
            otOperationalDatasetTlvs active_dataset;
            ESP_RETURN_ON_ERROR(thread_br_get_active_dataset_tlvs(&active_dataset), TAG, "Failed to get active dataset");
            char *dataset_str = (char *)MEM_CALLOC_EXTRAM(2 * active_dataset.mLength + 1, 1);
            if (!dataset_str) {
                return ESP_ERR_NO_MEM;
            }
            convert_bytes_to_str(active_dataset.mTlvs, active_dataset.mLength, dataset_str, 2 * active_dataset.mLength + 1);
            esp_rmaker_param_val_t report_val = esp_rmaker_str(dataset_str);
            esp_rmaker_param_t *report_param = esp_rmaker_device_get_param_by_type(device, ESP_RMAKER_PARAM_TBR_ACTIVE_DATASET);
            esp_err_t err = esp_rmaker_param_update_and_report(report_param, report_val);
            free(dataset_str);
            if (err != ESP_OK) {
                ESP_LOGE(TAG, "Failed to update and report active dataset");
                return err;
            }
        }
    }
    return ESP_OK;
}

esp_err_t esp_rmaker_thread_br_enable(const esp_openthread_platform_config_t *platform_config,
                                      const esp_rcp_update_config_t *rcp_update_config)
{
    if (thread_br_service) {
        return ESP_ERR_INVALID_STATE;
    }
    thread_br_service = esp_rmaker_thread_br_service_create(ESP_RMAKER_DEF_THREAD_BR_SERVICE, write_cb, NULL, NULL);
    if (!thread_br_service) {
        ESP_LOGE(TAG, "Failed to create thread br service");
        return ESP_FAIL;
    }
    esp_err_t err = esp_rmaker_node_add_device(esp_rmaker_get_node(), thread_br_service);
    if (err != ESP_OK) {
        esp_rmaker_device_delete(thread_br_service);
        thread_br_service = NULL;
    }
    thread_border_router_start(platform_config, rcp_update_config);
    return ESP_OK;
}

esp_err_t esp_rmaker_thread_br_report_device_role(void)
{
    if (!thread_br_service) {
        return ESP_ERR_INVALID_STATE;
    }
    esp_rmaker_param_t *param = esp_rmaker_device_get_param_by_type(thread_br_service,
                                                                    ESP_RMAKER_PARAM_TBR_DEVICE_ROLE);
    if (!param) {
        return ESP_ERR_INVALID_STATE;
    }
    otDeviceRole role = thread_br_get_device_role();
    esp_rmaker_param_val_t val = esp_rmaker_int(role);
    ESP_RETURN_ON_ERROR(esp_rmaker_param_update_and_report(param, val), TAG,
                        "Failed to update and report Thread device role");
    return ESP_OK;
}

esp_err_t esp_rmaker_thread_br_report_border_agent_id(void)
{
    if (!thread_br_service) {
        return ESP_ERR_INVALID_STATE;
    }
    esp_rmaker_param_t *param = esp_rmaker_device_get_param_by_type(thread_br_service,
                                                                    ESP_RMAKER_PARAM_TBR_BORDER_AGENT_ID);
    if (!param) {
        return ESP_ERR_INVALID_STATE;
    }
    otBorderAgentId border_agent_id;
    ESP_RETURN_ON_ERROR(thread_br_get_border_agent_id(&border_agent_id), TAG, "Failed to get Border Agent ID");
    char border_agent_id_str[sizeof(border_agent_id.mId) * 2 + 1];
    convert_bytes_to_str(border_agent_id.mId, sizeof(border_agent_id.mId), border_agent_id_str,
                         sizeof(border_agent_id_str));
    esp_rmaker_param_val_t report_val = esp_rmaker_str(border_agent_id_str);
    ESP_RETURN_ON_ERROR(esp_rmaker_param_update_and_report(param, report_val), TAG,
                        "Failed to update and report border agent id");
    return ESP_OK;
}
