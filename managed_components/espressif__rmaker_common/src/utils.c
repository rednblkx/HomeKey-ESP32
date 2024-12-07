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
#include <stdint.h>
#include <freertos/FreeRTOS.h>
#include <freertos/timers.h>
#include <esp_system.h>
#include <esp_wifi.h>
#include <nvs_flash.h>
#include <esp_rmaker_common_events.h>

ESP_EVENT_DEFINE_BASE(RMAKER_COMMON_EVENT);

static TimerHandle_t reboot_timer;
static TimerHandle_t reset_timer;

static void esp_rmaker_reboot_cb(TimerHandle_t handle)
{
    xTimerDelete(reboot_timer, 10);
    reboot_timer = NULL;
    esp_restart();
}

esp_err_t esp_rmaker_reboot(int8_t seconds)
{
    /* If specified time is 0, reboot immediately */
    if (seconds == 0) {
        esp_event_post(RMAKER_COMMON_EVENT, RMAKER_EVENT_REBOOT, &seconds, sizeof(seconds), portMAX_DELAY);
        esp_restart();
        return ESP_OK;
    } else if (reboot_timer) {
        /* If reboot timer already exists, it means that a reboot operation is already in progress.
         * So, just return an error from here.
         */
        return ESP_FAIL;
    }
    reboot_timer = xTimerCreate("rmaker_reboot_tm", (seconds * 1000) / portTICK_PERIOD_MS,
                            pdFALSE, NULL, esp_rmaker_reboot_cb);
    if (reboot_timer) {
        if (xTimerStart(reboot_timer, 10) != pdTRUE) {
            xTimerDelete(reboot_timer, 10);
            reboot_timer = NULL;
            return ESP_FAIL;
        }
    } else {
        return ESP_ERR_NO_MEM;
    }
    esp_event_post(RMAKER_COMMON_EVENT, RMAKER_EVENT_REBOOT, &seconds, sizeof(seconds), portMAX_DELAY);
    return ESP_OK;
}

#ifdef CONFIG_ESP_RMAKER_NETWORK_OVER_WIFI
static esp_err_t __esp_rmaker_wifi_reset(int8_t reboot_seconds)
{
    esp_wifi_restore();
    if (reboot_seconds >= 0) {
        esp_rmaker_reboot(reboot_seconds);
    }
    return ESP_OK;
}
#endif /* CONFIG_ESP_RMAKER_NETWORK_OVER_WIFI */

static esp_err_t __esp_rmaker_factory_reset(int8_t reboot_seconds)
{
    nvs_flash_deinit();
    nvs_flash_erase();
    if (reboot_seconds >= 0) {
        esp_rmaker_reboot(reboot_seconds);
    }
    return ESP_OK;
}

#ifdef CONFIG_ESP_RMAKER_NETWORK_OVER_WIFI
static void esp_rmaker_wifi_reset_cb(TimerHandle_t handle)
{
    /* (Hack) Using the timer id as reboot seconds */
    int8_t *reboot_seconds = (int8_t *)pvTimerGetTimerID(handle);
    if (reboot_seconds) {
        __esp_rmaker_wifi_reset((int8_t)*reboot_seconds);
        free(reboot_seconds);
    } else {
        __esp_rmaker_wifi_reset(0);
    }
    xTimerDelete(reset_timer, 10);
    reset_timer = NULL;
}
#endif /* CONFIG_ESP_RMAKER_NETWORK_OVER_WIFI */

static void esp_rmaker_factory_reset_cb(TimerHandle_t handle)
{
    /* (Hack) Using the timer id as reboot seconds */
    int8_t *reboot_seconds = (int8_t *)pvTimerGetTimerID(handle);
    if (reboot_seconds) {
        __esp_rmaker_factory_reset((int8_t)*reboot_seconds);
        free(reboot_seconds);
    } else {
        __esp_rmaker_factory_reset(0);
    }
    xTimerDelete(reset_timer, 10);
    reset_timer = NULL;
}

static esp_err_t esp_rmaker_start_reset_timer(int8_t reset_seconds, int8_t reboot_seconds, TimerCallbackFunction_t cb)
{
    /* If reset timer already exists, it means that a reset operation is already in progress.
     * So, just return an error from here.
     */
    if (reset_timer) {
        return ESP_FAIL;
    }
    /* (Hack) Passing the reboot delay as timer id */
    int8_t *reboot_delay = calloc(1, sizeof(int8_t));
    if (reboot_delay) {
        *reboot_delay = reboot_seconds;
    }
    reset_timer = xTimerCreate("rmaker_reset_tm", (reset_seconds * 1000) / portTICK_PERIOD_MS,
            pdFALSE, (void *)reboot_delay, cb);
    if (reset_timer) {
        if (xTimerStart(reset_timer, 10) != pdTRUE) {
            xTimerDelete(reset_timer, 10);
            reset_timer = NULL;
            return ESP_FAIL;
        }
    } else {
        return ESP_ERR_NO_MEM;
    }
    return ESP_OK;
}

esp_err_t esp_rmaker_wifi_reset(int8_t reset_seconds, int8_t reboot_seconds)
{
#ifdef CONFIG_ESP_RMAKER_NETWORK_OVER_WIFI
    esp_err_t err = ESP_FAIL;
    /* If reset time is 0, just do it right away */
    if (reset_seconds == 0) {
        err = __esp_rmaker_wifi_reset(reboot_seconds);
    } else {
        /* Else start a timer so that the task gets performed after the specified delay */
        err = esp_rmaker_start_reset_timer(reset_seconds, reboot_seconds, esp_rmaker_wifi_reset_cb);
    }
    if (err == ESP_OK) {
        esp_event_post(RMAKER_COMMON_EVENT, RMAKER_EVENT_WIFI_RESET, NULL, 0, portMAX_DELAY);
    }
    return ESP_OK;
#else
    return ESP_ERR_NOT_SUPPORTED;
#endif /* CONFIG_ESP_RMAKER_NETWORK_OVER_WIFI */
}

esp_err_t esp_rmaker_factory_reset(int8_t reset_seconds, int8_t reboot_seconds)
{
    esp_err_t err = ESP_FAIL;
    /* If reset time is 0, just do it right away */
    if (reset_seconds == 0) {
        err = __esp_rmaker_factory_reset(reboot_seconds);
    } else {
        /* Else start a timer so that the task gets performed after the specified delay */
        err = esp_rmaker_start_reset_timer(reset_seconds, reboot_seconds, esp_rmaker_factory_reset_cb);
    }
    if (err == ESP_OK) {
        esp_event_post(RMAKER_COMMON_EVENT, RMAKER_EVENT_FACTORY_RESET, NULL, 0, portMAX_DELAY);
    }
    return ESP_OK;
}
