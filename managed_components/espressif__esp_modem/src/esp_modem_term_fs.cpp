/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <optional>
#include <unistd.h>
#include "cxx_include/esp_modem_dte.hpp"
#include "esp_log.h"
#include "esp_modem_config.h"
#include "exception_stub.hpp"

static const char *TAG = "fs_terminal";

namespace esp_modem {


struct File {
    explicit File(const esp_modem_dte_config *config):
        fd(config->vfs_config.fd), deleter(config->vfs_config.deleter), resource(config->vfs_config.resource)
    {}

    ~File()
    {
        if (deleter) {
            deleter(fd, resource);
        }
    }
    int fd;
    void (*deleter)(int fd, struct esp_modem_vfs_resource *res);
    struct esp_modem_vfs_resource *resource;
};

class FdTerminal : public Terminal {
public:
    explicit FdTerminal(const esp_modem_dte_config *config);

    ~FdTerminal() override;

    void start() override
    {
        signal.set(TASK_START);
    }

    void stop() override
    {
        signal.clear(TASK_START);
    }

    int write(uint8_t *data, size_t len) override;

    int read(uint8_t *data, size_t len) override;

    void set_read_cb(std::function<bool(uint8_t *data, size_t len)> f) override
    {
        on_read = std::move(f);
        signal.set(TASK_PARAMS);
    }

private:
    void task();

    static const size_t TASK_INIT = SignalGroup::bit0;
    static const size_t TASK_START = SignalGroup::bit1;
    static const size_t TASK_STOP = SignalGroup::bit2;
    static const size_t TASK_PARAMS = SignalGroup::bit3;

    File f;
    SignalGroup signal;
    Task task_handle;
};

std::unique_ptr<Terminal> create_vfs_terminal(const esp_modem_dte_config *config)
{
    TRY_CATCH_RET_NULL(
        auto term = std::make_unique<FdTerminal>(config);
        term->start();
        return term;
    )
}

FdTerminal::FdTerminal(const esp_modem_dte_config *config) :
    f(config), signal(),
    task_handle(config->task_stack_size, config->task_priority, this, [](void *p)
{
    auto t = static_cast<FdTerminal *>(p);
    t->task();
    Task::Delete();
})
{}

void FdTerminal::task()
{
    std::function<bool(uint8_t *data, size_t len)> on_read_priv = nullptr;
    signal.set(TASK_INIT);
    signal.wait_any(TASK_START | TASK_STOP, portMAX_DELAY);
    if (signal.is_any(TASK_STOP)) {
        return; // exits to the static method where the task gets deleted
    }

    while (signal.is_any(TASK_START)) {
        int s;
        fd_set rfds;
        struct timeval tv = {
            .tv_sec = 1,
            .tv_usec = 0,
        };
        FD_ZERO(&rfds);
        FD_SET(f.fd, &rfds);

        s = select(f.fd + 1, &rfds, nullptr, nullptr, &tv);
        if (signal.is_any(TASK_PARAMS)) {
            on_read_priv = on_read;
            signal.clear(TASK_PARAMS);
        }

        if (s < 0) {
            break;
        } else if (s == 0) {
//            ESP_LOGV(TAG, "Select exited with timeout");
        } else {
            if (FD_ISSET(f.fd, &rfds)) {
                if (on_read_priv) {
                    on_read_priv(nullptr, 0);
                }
            }
        }
        Task::Relinquish();
    }
}

int FdTerminal::read(uint8_t *data, size_t len)
{
    int size = ::read(f.fd, data, len);
    if (size < 0) {
        if (errno != EAGAIN) {
            ESP_LOGE(TAG, "Error occurred during read: %d", errno);
        }
        return 0;
    }

    return size;
}

int FdTerminal::write(uint8_t *data, size_t len)
{
    int size = ::write(f.fd, data, len);
    if (size < 0) {
        ESP_LOGE(TAG, "Error occurred during read: %d", errno);
        return 0;
    }
    return size;
}

FdTerminal::~FdTerminal()
{
    FdTerminal::stop();
}

} // namespace esp_modem
