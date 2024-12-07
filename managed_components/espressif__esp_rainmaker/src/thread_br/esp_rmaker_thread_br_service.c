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


#include <esp_err.h>
#include <esp_rmaker_core.h>
#include <esp_rmaker_standard_types.h>
#include <esp_rmaker_thread_br_priv.h>

static esp_rmaker_param_t *esp_rmaker_thread_br_border_agent_id_param_create(const char *param_name)
{
    esp_rmaker_param_t *param = esp_rmaker_param_create(param_name, ESP_RMAKER_PARAM_TBR_BORDER_AGENT_ID,
                                                        esp_rmaker_str(""), PROP_FLAG_READ);
    return param;
}

static esp_rmaker_param_t *esp_rmaker_thread_br_active_dataset_param_create(const char *param_name)
{
    esp_rmaker_param_t *param = esp_rmaker_param_create(param_name, ESP_RMAKER_PARAM_TBR_ACTIVE_DATASET,
                                                        esp_rmaker_str(""),
                                                        PROP_FLAG_READ | PROP_FLAG_WRITE | PROP_FLAG_PERSIST);
    return param;
}

static esp_rmaker_param_t *esp_rmaker_thread_br_pending_dataset_param_create(const char *param_name)
{
    esp_rmaker_param_t *param = esp_rmaker_param_create(param_name, ESP_RMAKER_PARAM_TBR_PENDING_DATASET,
                                                        esp_rmaker_str(""),
                                                        PROP_FLAG_READ | PROP_FLAG_WRITE | PROP_FLAG_PERSIST);
    return param;
}

static esp_rmaker_param_t *esp_rmaker_thread_br_thread_role_param_create(const char *param_name)
{
    esp_rmaker_param_t *param = esp_rmaker_param_create(param_name, ESP_RMAKER_PARAM_TBR_DEVICE_ROLE,
                                                        esp_rmaker_int(0), PROP_FLAG_READ);
    return param;
}

static esp_rmaker_param_t *esp_rmaker_thread_br_cmd_param_create(const char *param_name)
{
    esp_rmaker_param_t *param = esp_rmaker_param_create(param_name, ESP_RMAKER_PARAM_TBR_CMD,
                                                        esp_rmaker_int(0), PROP_FLAG_WRITE);
    return param;
}

esp_rmaker_device_t *esp_rmaker_thread_br_service_create(const char *serv_name, esp_rmaker_device_write_cb_t write_cb,
                                                         esp_rmaker_device_read_cb_t read_cb, void *priv_data)
{
    esp_rmaker_device_t *service = esp_rmaker_service_create(serv_name, ESP_RMAKER_SERVICE_THREAD_BR, priv_data);
    if (service) {
        esp_rmaker_device_add_cb(service, write_cb, read_cb);
        esp_rmaker_device_add_param(
            service, esp_rmaker_thread_br_border_agent_id_param_create(ESP_RMAKER_DEF_BORDER_AGENT_ID));
        esp_rmaker_device_add_param(
            service, esp_rmaker_thread_br_active_dataset_param_create(ESP_RMAKER_DEF_ACTIVE_DATASET));
        esp_rmaker_device_add_param(
            service, esp_rmaker_thread_br_pending_dataset_param_create(ESP_RMAKER_DEF_PENDING_DATASET));
        esp_rmaker_device_add_param(service, esp_rmaker_thread_br_thread_role_param_create(ESP_RMAKER_DEF_DEVICE_ROLE));
        esp_rmaker_device_add_param(service, esp_rmaker_thread_br_cmd_param_create(ESP_RMAKER_DEF_THREAD_BR_CMD));
    }
    return service;
}
