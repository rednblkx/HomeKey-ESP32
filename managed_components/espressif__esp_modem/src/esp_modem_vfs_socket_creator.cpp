/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <cstring>
#include <unistd.h>
#include <sys/fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include "esp_log.h"
#include "esp_modem_config.h"
#include "cxx_include/esp_modem_exception.hpp"
#include "exception_stub.hpp"
#include "uart_resource.hpp"
#include "vfs_resource/vfs_create.hpp"


constexpr const char *TAG = "vfs_socket_creator";

/**
 * @brief socket VFS
 * @note: Remote command:
 * socat TCP-L:2222 GOPEN:/dev/ttyS0,ispeed=115200,ospeed=1152000,b115200,raw,echo=0
 */
static esp_err_t hostname_to_fd(const char *host, int port, int *fd)
{
    struct sockaddr_storage address = {};
    struct addrinfo *address_info;
    struct addrinfo hints = {};
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    int res = getaddrinfo(host, nullptr, &hints, &address_info);
    if (res != 0 || address_info == nullptr) {
        ESP_LOGE(TAG, "couldn't get hostname for :%s: "
                 "getaddrinfo() returns %d, addrinfo=%p", host, res, address_info);
        return ESP_FAIL;
    }
    *fd = socket(address_info->ai_family, address_info->ai_socktype, address_info->ai_protocol);
    if (*fd < 0) {
        ESP_LOGE(TAG, "Failed to create socket (family %d socktype %d protocol %d)", address_info->ai_family, address_info->ai_socktype, address_info->ai_protocol);
        freeaddrinfo(address_info);
        return ESP_FAIL;
    }

    if (address_info->ai_family == AF_INET) {
        auto *p = reinterpret_cast<struct sockaddr_in *>(address_info->ai_addr);
        p->sin_port = htons(port);
        ESP_LOGI(TAG, "[sock=%d] Resolved IPv4 address: %s", *fd, inet_ntoa(p->sin_addr));
        memcpy(&address, p, sizeof(struct sockaddr ));
    } else {
        ESP_LOGE(TAG, "Unsupported protocol family %d", address_info->ai_family);
        close(*fd);
        freeaddrinfo(address_info);
        return ESP_FAIL;
    }

    freeaddrinfo(address_info);
    if (connect(*fd, (struct sockaddr *)&address, sizeof(struct sockaddr)) < 0) {
        ESP_LOGE(TAG, "[sock=%d] Failed to connect", *fd);
        return ESP_FAIL;
    }
    return ESP_OK;
}


static void vfs_destroy_socket(int fd, struct esp_modem_vfs_resource *resource)
{
    if (fd >= 0) {
        close(fd);
    }
}

bool vfs_create_socket(struct esp_modem_vfs_socket_creator *config, struct esp_modem_vfs_term_config *created_config)
{
    if (config == nullptr || created_config == nullptr) {
        return false;
    }
    TRY_CATCH_OR_DO(
        int fd = -1;
        ESP_MODEM_THROW_IF_ERROR(hostname_to_fd(config->host_name, config->port, &fd));

        // Set the FD to non-blocking mode
        int flags = fcntl(fd, F_GETFL, nullptr) | O_NONBLOCK;
        fcntl(fd, F_SETFL, flags);

        created_config->fd = fd;
        created_config->deleter = vfs_destroy_socket;
        , return false)
    return true;
}
