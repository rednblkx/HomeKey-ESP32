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

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>
#include <esp_err.h>
#include <esp_log.h>

#include <esp_rmaker_work_queue.h>

#define ESP_RMAKER_TASK_QUEUE_SIZE           8
#define ESP_RMAKER_TASK_STACK       CONFIG_ESP_RMAKER_WORK_QUEUE_TASK_STACK
#define ESP_RMAKER_TASK_PRIORITY    CONFIG_ESP_RMAKER_WORK_QUEUE_TASK_PRIORITY

static const char *TAG = "esp_rmaker_work_queue";

typedef enum {
    WORK_QUEUE_STATE_DEINIT = 0,
    WORK_QUEUE_STATE_INIT_DONE,
    WORK_QUEUE_STATE_RUNNING,
    WORK_QUEUE_STATE_STOP_REQUESTED,
} esp_rmaker_work_queue_state_t;

typedef struct {
    esp_rmaker_work_fn_t work_fn;
    void *priv_data;
} esp_rmaker_work_queue_entry_t;

static QueueHandle_t work_queue;
static esp_rmaker_work_queue_state_t queue_state;

static void esp_rmaker_handle_work_queue(void)
{
    esp_rmaker_work_queue_entry_t work_queue_entry;
    /* 2 sec delay to prevent spinning */
    BaseType_t ret = xQueueReceive(work_queue, &work_queue_entry, 2000 / portTICK_PERIOD_MS);
    while (ret == pdTRUE) {
        work_queue_entry.work_fn(work_queue_entry.priv_data);
        ret = xQueueReceive(work_queue, &work_queue_entry, 0);
    }
}

static void esp_rmaker_work_queue_task(void *param)
{
    ESP_LOGI(TAG, "RainMaker Work Queue task started.");
    while (queue_state != WORK_QUEUE_STATE_STOP_REQUESTED) {
        esp_rmaker_handle_work_queue();
    }
    ESP_LOGI(TAG, "Stopping Work Queue task");
    queue_state = WORK_QUEUE_STATE_INIT_DONE;
    vTaskDelete(NULL);
}

esp_err_t esp_rmaker_work_queue_add_task(esp_rmaker_work_fn_t work_fn, void *priv_data)
{
    if (!work_queue) {
        ESP_LOGE(TAG, "Cannot enqueue function as Work Queue hasn't been created.");
        return ESP_ERR_INVALID_STATE;
    }
    esp_rmaker_work_queue_entry_t work_queue_entry = {
        .work_fn = work_fn,
        .priv_data = priv_data,
    };
    if (xQueueSend(work_queue, &work_queue_entry, 0) == pdTRUE) {
        return ESP_OK;
    }
    return ESP_FAIL;
}

esp_err_t esp_rmaker_work_queue_init(void)
{
    if (queue_state != WORK_QUEUE_STATE_DEINIT) {
        ESP_LOGW(TAG, "Work Queue already initialiased/started.");
        return ESP_OK;
    }
    work_queue = xQueueCreate(ESP_RMAKER_TASK_QUEUE_SIZE, sizeof(esp_rmaker_work_queue_entry_t));
    if (!work_queue) {
        ESP_LOGE(TAG, "Failed to create Work Queue.");
        return ESP_FAIL;
    }
    ESP_LOGI(TAG, "Work Queue created.");
    queue_state = WORK_QUEUE_STATE_INIT_DONE;
    return ESP_OK;
}

esp_err_t esp_rmaker_work_queue_deinit(void)
{
    if (queue_state != WORK_QUEUE_STATE_STOP_REQUESTED) {
        esp_rmaker_work_queue_stop();
    }

    while (queue_state == WORK_QUEUE_STATE_STOP_REQUESTED) {
        ESP_LOGI(TAG, "Waiting for esp_rmaker_work_queue being stopped...");
        vTaskDelay(2000 / portTICK_PERIOD_MS);
    }

    if (queue_state == WORK_QUEUE_STATE_DEINIT) {
        return ESP_OK;
    } else if (queue_state != WORK_QUEUE_STATE_INIT_DONE) {
        ESP_LOGE(TAG, "Cannot deinitialize Work Queue as the task is still running.");
        return ESP_ERR_INVALID_STATE;
    } else {
        vQueueDelete(work_queue);
        work_queue = NULL;
        queue_state = WORK_QUEUE_STATE_DEINIT;
    }
    ESP_LOGI(TAG, "esp_rmaker_work_queue was successfully deinitialized");
    return ESP_OK;
}

esp_err_t esp_rmaker_work_queue_start(void)
{
    if (queue_state == WORK_QUEUE_STATE_RUNNING) {
        ESP_LOGW(TAG, "Work Queue already started.");
        return ESP_OK;
    }
    if (queue_state != WORK_QUEUE_STATE_INIT_DONE) {
        ESP_LOGE(TAG, "Failed to start Work Queue as it wasn't initialized.");
        return ESP_ERR_INVALID_STATE;
    }
    if (xTaskCreate(&esp_rmaker_work_queue_task, "rmaker_queue_task", ESP_RMAKER_TASK_STACK,
                NULL, ESP_RMAKER_TASK_PRIORITY, NULL) != pdPASS) {
        ESP_LOGE(TAG, "Couldn't create RainMaker work queue task");
        return ESP_FAIL;
    }
    queue_state = WORK_QUEUE_STATE_RUNNING;
    return ESP_OK;
}

esp_err_t esp_rmaker_work_queue_stop(void)
{
    if (queue_state == WORK_QUEUE_STATE_RUNNING) {
        queue_state = WORK_QUEUE_STATE_STOP_REQUESTED;
    }
    return ESP_OK;
}
