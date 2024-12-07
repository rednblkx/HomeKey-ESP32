/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "esp_event.h"
#include "esp_modem_exception.hpp"

#if defined(CONFIG_IDF_TARGET_LINUX)
#include <mutex>
#include <thread>

#else
#include "freertos/event_groups.h"
// forward declarations of FreeRTOS primitives
struct QueueDefinition;
#endif


namespace esp_modem {

// Forward declaration for both linux/FreeRTOS targets
//
using TaskFunction_t = void (*)(void *);
#if !defined(CONFIG_IDF_TARGET_LINUX)
struct Lock {
    using MutexT = QueueHandle_t;
    explicit Lock();
    ~Lock();
    void lock();
    void unlock();
private:
    MutexT m{};
};
using TaskT = TaskHandle_t;
using SignalT = EventGroupHandle_t;
#else
using Lock = std::recursive_mutex;
struct SignalGroupInternal;
using SignalT = std::unique_ptr<SignalGroupInternal>;
using TaskT = std::thread;
static constexpr uint32_t portMAX_DELAY = UINT32_MAX;
#endif

template<class T>
class Scoped {
public:
    explicit Scoped(T &l): lock(l)
    {
        lock.lock();
    }
    ~Scoped()
    {
        lock.unlock();
    }

private:
    T &lock;
};

class Task {
public:
    explicit Task(size_t stack_size, size_t priority, void *task_param, TaskFunction_t task_function);
    ~Task();

    static void Delete();
    static void Relinquish();
    static void Delay(uint32_t delay);
private:
    TaskT task_handle;
};


class SignalGroup {
public:
    static constexpr size_t bit0 = 1 << 0;
    static constexpr size_t bit1 = 1 << 1;
    static constexpr size_t bit2 = 1 << 2;
    static constexpr size_t bit3 = 1 << 3;

    explicit SignalGroup();

    void set(uint32_t bits);

    void clear(uint32_t bits);

    // waiting for all and clearing if set
    bool wait(uint32_t flags, uint32_t time_ms);

    bool is_any(uint32_t flags);

    // waiting for any bit, not clearing them
    bool wait_any(uint32_t flags, uint32_t time_ms);

    ~SignalGroup();

private:
    SignalT event_group;
};

} // namespace esp_modem
