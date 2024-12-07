/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <optional>
#include <unistd.h>
#include <sys/fcntl.h>
#include "esp_log.h"
#include "esp_modem_config.h"
#include "cxx_include/esp_modem_exception.hpp"
#include "exception_stub.hpp"
#include "uart_resource.hpp"
#include "vfs_resource/vfs_create.hpp"

[[maybe_unused]] constexpr const char *TAG = "vfs_uart_creator";


struct esp_modem_vfs_resource {
    explicit esp_modem_vfs_resource(const esp_modem_uart_term_config *config, int fd)
        : internal(config, nullptr, fd) {}

    esp_modem::uart_resource internal;
};


static void vfs_destroy_uart(int fd, struct esp_modem_vfs_resource *resource)
{
    if (fd >= 0) {
        close(fd);
    }
    delete resource;
}

bool vfs_create_uart(struct esp_modem_vfs_uart_creator *config, struct esp_modem_vfs_term_config *created_config)
{
    if (!config->dev_name || created_config == nullptr) {
        return false;
    }
    TRY_CATCH_OR_DO(
        int fd = open(config->dev_name, O_RDWR);
        ESP_MODEM_THROW_IF_FALSE(fd >= 0, "Cannot open the fd");

        created_config->resource = new esp_modem_vfs_resource(&config->uart, fd);
        created_config->fd = fd;
        created_config->deleter = vfs_destroy_uart;

        // Set the FD to non-blocking mode
        int flags = fcntl(fd, F_GETFL, nullptr) | O_NONBLOCK;
        fcntl(fd, F_SETFL, flags);

        , return false)

    return true;
}
