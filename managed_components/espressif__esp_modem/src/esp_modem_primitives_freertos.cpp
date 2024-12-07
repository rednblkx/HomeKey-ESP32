/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "cxx_include/esp_modem_primitives.hpp"

#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/semphr.h"

namespace esp_modem {

void Lock::unlock()
{
    xSemaphoreGiveRecursive(m);
}

Lock::Lock(): m(nullptr)
{
    m = xSemaphoreCreateRecursiveMutex();
    ESP_MODEM_THROW_IF_FALSE(m != nullptr, "create signal event group failed");
}

Lock::~Lock()
{
    vSemaphoreDelete(m);
}

void Lock::lock()
{
    xSemaphoreTakeRecursive(m, portMAX_DELAY);
}


SignalGroup::SignalGroup(): event_group(nullptr)
{
    event_group = xEventGroupCreate();
    ESP_MODEM_THROW_IF_FALSE(event_group != nullptr, "create signal event group failed");
}

void SignalGroup::set(uint32_t bits)
{
    xEventGroupSetBits(event_group, bits);
}

void SignalGroup::clear(uint32_t bits)
{
    xEventGroupClearBits(event_group, bits);
}

bool SignalGroup::wait(uint32_t flags, uint32_t time_ms)
{
    EventBits_t bits = xEventGroupWaitBits(event_group, flags, pdTRUE, pdTRUE, pdMS_TO_TICKS(time_ms));
    return bits & flags;
}

bool SignalGroup::is_any(uint32_t flags)
{
    return xEventGroupGetBits(event_group) & flags;
}

bool SignalGroup::wait_any(uint32_t flags, uint32_t time_ms)
{
    EventBits_t bits = xEventGroupWaitBits(event_group, flags, pdFALSE, pdFALSE, pdMS_TO_TICKS(time_ms));
    return bits & flags;
}

SignalGroup::~SignalGroup()
{
    if (event_group) {
        vEventGroupDelete(event_group);
    }
}

Task::Task(size_t stack_size, size_t priority, void *task_param, TaskFunction_t task_function)
    : task_handle(nullptr)
{
    BaseType_t ret = xTaskCreate(task_function, "vfs_task", stack_size, task_param, priority, &task_handle);
    ESP_MODEM_THROW_IF_FALSE(ret == pdTRUE, "create vfs task failed");
}

Task::~Task()
{
    if (task_handle) {
        vTaskDelete(task_handle);
    }
}

void Task::Delete()
{
    vTaskDelete(nullptr);
}

void Task::Relinquish()
{
    vTaskDelay(1);
}

void Task::Delay(uint32_t ms)
{
    vTaskDelay(pdMS_TO_TICKS(ms));
}

} // namespace esp_modem
