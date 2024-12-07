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
#include <esp_openthread.h>
#include <esp_openthread_lock.h>

#include <openthread/border_agent.h>
#include <openthread/dataset.h>
#include <openthread/dataset_ftd.h>
#include <openthread/error.h>
#include <openthread/logging.h>
#include <openthread/thread.h>

#include "esp_rmaker_thread_br_priv.h"

static const char* TAG = "thread_br";

esp_err_t thread_br_set_thread_enabled(bool enabled)
{
    otInstance *instance = esp_openthread_get_instance();
    if (!instance) {
        ESP_LOGE(TAG, "Thread not initialized");
        return ESP_ERR_INVALID_STATE;
    }
    esp_openthread_lock_acquire(portMAX_DELAY);
    bool is_enabled = (otThreadGetDeviceRole(instance) != OT_DEVICE_ROLE_DISABLED);
    bool is_ip6_enabled = otIp6IsEnabled(instance);
    if (enabled && !is_ip6_enabled) {
        if (otIp6SetEnabled(instance, enabled) != OT_ERROR_NONE) {
            ESP_LOGE(TAG, "Failed to %s netif", enabled ? "enable" : "disable");
            esp_openthread_lock_release();
            return ESP_FAIL;
        }
    }
    if (enabled != is_enabled) {
        if (otThreadSetEnabled(instance, enabled) != OT_ERROR_NONE) {
            ESP_LOGE(TAG, "Failed to %s thread", enabled ? "enable" : "disable");
            esp_openthread_lock_release();
            return ESP_FAIL;
        }
    }
    if (!enabled && is_ip6_enabled) {
        if (otIp6SetEnabled(instance, enabled) != OT_ERROR_NONE) {
            ESP_LOGE(TAG, "Failed to %s netif", enabled ? "enable" : "disable");
            esp_openthread_lock_release();
            return ESP_FAIL;
        }
    }
    esp_openthread_lock_release();
    return ESP_OK;
}

esp_err_t thread_br_set_active_dataset_tlvs(otOperationalDatasetTlvs *dataset_tlvs)
{
    if (!dataset_tlvs) {
        return ESP_ERR_INVALID_ARG;
    }
    otInstance *instance = esp_openthread_get_instance();
    if (!instance) {
        ESP_LOGE(TAG, "Thread not initialized");
        return ESP_ERR_INVALID_STATE;
    }

    esp_err_t err = ESP_OK;
    esp_openthread_lock_acquire(portMAX_DELAY);
    if (otDatasetSetActiveTlvs(instance, dataset_tlvs) != OT_ERROR_NONE) {
        ESP_LOGE(TAG, "Failed to set active Thread DatasetTlvs");
        err = ESP_FAIL;
    }
    esp_openthread_lock_release();
    return err;
}

esp_err_t thread_br_get_active_dataset_tlvs(otOperationalDatasetTlvs *dataset_tlvs)
{
    if (!dataset_tlvs) {
        return ESP_ERR_INVALID_ARG;
    }
    otInstance *instance = esp_openthread_get_instance();
    if (!instance) {
        ESP_LOGE(TAG, "Thread not initialized");
        return ESP_ERR_INVALID_STATE;
    }

    esp_err_t err = ESP_OK;
    esp_openthread_lock_acquire(portMAX_DELAY);
    if (otDatasetGetActiveTlvs(instance, dataset_tlvs) != OT_ERROR_NONE) {
        ESP_LOGI(TAG, "Active Dataset not set");
        err = ESP_FAIL;
    }
    esp_openthread_lock_release();
    return err;
}

esp_err_t thread_br_set_pending_dataset_tlvs(otOperationalDatasetTlvs *dataset_tlvs)
{
    if (!dataset_tlvs) {
        return ESP_ERR_INVALID_ARG;
    }
    otInstance *instance = esp_openthread_get_instance();
    if (!instance) {
        ESP_LOGE(TAG, "Thread not initialized");
        return ESP_ERR_INVALID_STATE;
    }

    esp_err_t err = ESP_OK;
    esp_openthread_lock_acquire(portMAX_DELAY);
    if (otDatasetSetPendingTlvs(instance, dataset_tlvs) != OT_ERROR_NONE) {
        ESP_LOGE(TAG, "Failed to set pending Thread DatasetTlvs");
        err = ESP_FAIL;
    }
    esp_openthread_lock_release();
    return err;
}

esp_err_t thread_br_get_border_agent_id(otBorderAgentId *border_agent_id)
{
    if (!border_agent_id) {
        return ESP_ERR_INVALID_ARG;
    }
    otInstance *instance = esp_openthread_get_instance();
    if (!instance) {
        ESP_LOGE(TAG, "Thread not initialized");
        return ESP_ERR_INVALID_STATE;
    }

    esp_err_t err = ESP_OK;
    esp_openthread_lock_acquire(portMAX_DELAY);
    if (otBorderAgentGetId(instance, border_agent_id) != OT_ERROR_NONE) {
        ESP_LOGE(TAG, "Failed to get Border Agent Id");
        err = ESP_FAIL;
    }
    esp_openthread_lock_release();
    return err;
}

otDeviceRole thread_br_get_device_role()
{
    if (!esp_openthread_get_instance()) {
        return OT_DEVICE_ROLE_DISABLED;
    }
    esp_openthread_lock_acquire(portMAX_DELAY);
    otDeviceRole role = otThreadGetDeviceRole(esp_openthread_get_instance());
    esp_openthread_lock_release();
    return role;
}

esp_err_t thread_br_generate_and_commit_new_dataset()
{
    otInstance *instance = esp_openthread_get_instance();
    if (!instance) {
        ESP_LOGE(TAG, "Thread not initialized");
        return ESP_ERR_INVALID_STATE;
    }
    if (thread_br_get_device_role() != OT_DEVICE_ROLE_DISABLED) {
        ESP_LOGE(TAG, "The device role should be disabled when calling generate_and_commit_new_dataset");
        return ESP_ERR_INVALID_STATE;
    }
    esp_openthread_lock_acquire(portMAX_DELAY);
    otOperationalDataset dataset;
    if (otDatasetCreateNewNetwork(instance, &dataset) != OT_ERROR_NONE) {
        esp_openthread_lock_release();
        return ESP_FAIL;
    }
    if (otDatasetSetActive(instance, &dataset) != OT_ERROR_NONE) {
        esp_openthread_lock_release();
        return ESP_FAIL;
    }
    esp_openthread_lock_release();
    return ESP_OK;
}
