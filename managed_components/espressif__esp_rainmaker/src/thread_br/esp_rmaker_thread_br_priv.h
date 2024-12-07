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

#pragma once

#include "esp_rcp_update.h"
#include <esp_rmaker_core.h>
#include <esp_openthread.h>
#include <openthread/border_agent.h>

/********** THREAD BR PARAMS NAME ***********/
#define ESP_RMAKER_DEF_BORDER_AGENT_ID      "BorderAgentId"
#define ESP_RMAKER_DEF_ACTIVE_DATASET       "ActiveDataset"
#define ESP_RMAKER_DEF_PENDING_DATASET      "PendingDataset"
#define ESP_RMAKER_DEF_DEVICE_ROLE          "DeviceRole"
#define ESP_RMAKER_DEF_THREAD_BR_CMD        "ThreadCmd"

/********** THREAD BR SERVICE NAME ***********/
#define ESP_RMAKER_DEF_THREAD_BR_SERVICE    "TBRService"

/********** THREAD BR PARAM TYPES ***********/
#define ESP_RMAKER_PARAM_TBR_BORDER_AGENT_ID   "esp.param.tbr-border-agent-id"
#define ESP_RMAKER_PARAM_TBR_ACTIVE_DATASET    "esp.param.tbr-active-dataset"
#define ESP_RMAKER_PARAM_TBR_PENDING_DATASET   "esp.param.tbr-pending-dataset"
#define ESP_RMAKER_PARAM_TBR_DEVICE_ROLE       "esp.param.tbr-device-role"
#define ESP_RMAKER_PARAM_TBR_CMD               "esp.param.tbr-cmd"

/********** THREAD BR SERVICE TYPE ***********/
#define ESP_RMAKER_SERVICE_THREAD_BR        "esp.service.thread-br"

/********** THREAD BR CMD TYPE ***********/
#define ESP_RMAKER_THREAD_BR_CMD_GEN_DATASET 1

esp_rmaker_device_t *esp_rmaker_thread_br_service_create(const char *serv_name, esp_rmaker_device_write_cb_t write_cb,
                                                         esp_rmaker_device_read_cb_t read_cb, void *priv_data);

esp_err_t esp_rmaker_thread_br_report_device_role(void);

esp_err_t esp_rmaker_thread_br_report_border_agent_id(void);

esp_err_t thread_br_set_thread_enabled(bool enabled);

esp_err_t thread_br_set_active_dataset_tlvs(otOperationalDatasetTlvs *dataset_tlvs);

esp_err_t thread_br_get_active_dataset_tlvs(otOperationalDatasetTlvs *dataset_tlvs);

esp_err_t thread_br_set_pending_dataset_tlvs(otOperationalDatasetTlvs *dataset_tlvs);

esp_err_t thread_br_get_border_agent_id(otBorderAgentId *border_agent_id);

otDeviceRole thread_br_get_device_role(void);

esp_err_t thread_br_generate_and_commit_new_dataset(void);

esp_err_t thread_border_router_start(const esp_openthread_platform_config_t *platform_config,
                                     const esp_rcp_update_config_t *rcp_update_config);
