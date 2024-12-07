/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <condition_variable>
#include <unistd.h>
#include "cxx_include/esp_modem_primitives.hpp"

namespace esp_modem {

struct SignalGroupInternal {
    std::condition_variable notify;
    std::mutex m;
    uint32_t flags{ 0 };
};


SignalGroup::SignalGroup(): event_group(std::make_unique<SignalGroupInternal>())
{
}

void SignalGroup::set(uint32_t bits)
{
    std::unique_lock<std::mutex> lock(event_group->m);
    event_group->flags |= bits;
    event_group->notify.notify_all();
}

void SignalGroup::clear(uint32_t bits)
{
    std::unique_lock<std::mutex> lock(event_group->m);
    event_group->flags &= ~bits;
    event_group->notify.notify_all();
}

bool SignalGroup::wait(uint32_t flags, uint32_t time_ms)
{
    std::unique_lock<std::mutex> lock(event_group->m);
    return event_group->notify.wait_for(lock, std::chrono::milliseconds(time_ms), [&] {
        if ((flags & event_group->flags) == flags)
        {
            event_group->flags &= ~flags;
            return true;
        }
        return false;
    });
//    return ret != std::cv_status::timeout;
//    , [&]{return flags&event_group->flags; });
}

bool SignalGroup::is_any(uint32_t flags)
{
    std::unique_lock<std::mutex> lock(event_group->m);
    return flags & event_group->flags;
}

bool SignalGroup::wait_any(uint32_t flags, uint32_t time_ms)
{
    std::unique_lock<std::mutex> lock(event_group->m);
    return event_group->notify.wait_for(lock, std::chrono::milliseconds(time_ms), [&] { return flags & event_group->flags; });
}

SignalGroup::~SignalGroup() = default;

Task::Task(size_t stack_size, size_t priority, void *task_param, TaskFunction_t task_function)
{
    task_handle = std::thread(task_function, task_param);
}

Task::~Task()
{
    task_handle.join();
}

void Task::Delete() {}

void Task::Relinquish()
{
    usleep(0);
}

void Task::Delay(uint32_t ms)
{
    usleep(ms * 1000);
}

} // namespace esp_modem
