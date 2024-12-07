/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <charconv>
#include <sys/socket.h>
#include "esp_vfs.h"
#include "esp_vfs_eventfd.h"

#include "sock_dce.hpp"

namespace sock_dce {

constexpr auto const *TAG = "sock_dce";


bool DCE::perform_sock()
{
    if (listen_sock == -1) {
        ESP_LOGE(TAG, "Listening socket not ready");
        close_sock();
        return false;
    }
    if (sock == -1) {   // no active socket, need to accept one first
        return accept_sock();
    }

    // we have a socket, let's check the status
    struct timeval tv = {
        .tv_sec = 0,
        .tv_usec = 500000,
    };
    if (state == status::PENDING) {
        vTaskDelay(pdMS_TO_TICKS(500));
        state = at.pending();
        return true;
    }
    fd_set fdset;
    FD_ZERO(&fdset);
    FD_SET(sock, &fdset);
    FD_SET(data_ready_fd, &fdset);
    int s = select(std::max(sock, data_ready_fd) + 1, &fdset, nullptr, nullptr, &tv);
    if (s == 0) {
        ESP_LOGV(TAG, "perform select timeout...");
        return true;
    } else if (s < 0) {
        ESP_LOGE(TAG,  "select error %d", errno);
        close_sock();
        return false;
    }
    if (FD_ISSET(sock, &fdset) && !sock_to_at()) {
        return false;
    }
    if (FD_ISSET(data_ready_fd, &fdset) && !at_to_sock()) {
        return false;
    }
    return true;
}

void DCE::perform_at(uint8_t *data, size_t len)
{
    ESP_LOG_BUFFER_HEXDUMP(TAG, data, len, ESP_LOG_VERBOSE);
    switch (at.process_data(state, data, len)) {
    case Responder::ret::OK:
        state = status::IDLE;
        signal.set(IDLE);
        return;
    case Responder::ret::FAIL:
        state = status::FAILED;
        signal.set(IDLE);
        return;
    case Responder::ret::NEED_MORE_DATA:
        return;
    case Responder::ret::IN_PROGRESS:
        break;
    case Responder::ret::NEED_MORE_TIME:
        state = status::PENDING;
        return;
    }
    std::string_view response((char *)data, len);
    switch (at.check_async_replies(state, response)) {
    case Responder::ret::OK:
        state = status::IDLE;
        signal.set(IDLE);
        return;
    case Responder::ret::FAIL:
        state = status::FAILED;
        signal.set(IDLE);
        return;
    case Responder::ret::NEED_MORE_TIME:
        state = status::PENDING;
        return;
    case Responder::ret::NEED_MORE_DATA:
    case Responder::ret::IN_PROGRESS:
        break;
    }
}

void DCE::close_sock()
{
    if (sock > 0) {
        close(sock);
        sock = -1;
    }
    dte->on_read(nullptr);
    const int retries = 5;
    int i = 0;
    while (net_close() != esp_modem::command_result::OK) {
        if (i++ > retries) {
            ESP_LOGE(TAG, "Failed to close network");
            return;
        }
        esp_modem::Task::Delay(1000);
    }
}

bool DCE::at_to_sock()
{
    uint64_t data;
    read(data_ready_fd, &data, sizeof(data));
    ESP_LOGD(TAG, "select read: modem data available %" PRIu64, data);
    if (!signal.wait(IDLE, 1000)) {
        ESP_LOGE(TAG, "Failed to get idle");
        close_sock();
        return false;
    }
    if (state != status::IDLE) {
        ESP_LOGE(TAG, "Unexpected state %d", static_cast<int>(state));
        close_sock();
        return false;
    }
    state = status::RECEIVING;
    at.start_receiving(at.get_buf_len());
    return true;
}

bool DCE::sock_to_at()
{
    ESP_LOGD(TAG,  "socket read: data available");
    if (!signal.wait(IDLE, 1000)) {
        ESP_LOGE(TAG,  "Failed to get idle");
        close_sock();
        return false;
    }
    if (state != status::IDLE) {
        ESP_LOGE(TAG,  "Unexpected state %d", static_cast<int>(state));
        close_sock();
        return false;
    }
    state = status::SENDING;
    int len = ::recv(sock, at.get_buf(), at.get_buf_len(), 0);
    if (len < 0) {
        ESP_LOGE(TAG,  "read error %d", errno);
        close_sock();
        return false;
    } else if (len == 0) {
        ESP_LOGE(TAG,  "EOF %d", errno);
        close_sock();
        return false;
    }
    ESP_LOG_BUFFER_HEXDUMP(TAG, at.get_buf(), len, ESP_LOG_VERBOSE);
    at.start_sending(len);
    return true;
}

bool DCE::accept_sock()
{
    struct timeval tv = {
        .tv_sec = 0,
        .tv_usec = 500000,
    };
    fd_set fdset;
    FD_ZERO(&fdset);
    FD_SET(listen_sock, &fdset);
    int s = select(listen_sock + 1, &fdset, nullptr, nullptr, &tv);
    if (s > 0 && FD_ISSET(listen_sock, &fdset)) {
        struct sockaddr_in source_addr = {};
        socklen_t addr_len = sizeof(source_addr);
        sock = accept(listen_sock, (struct sockaddr *)&source_addr, &addr_len);
        if (sock < 0) {
            ESP_LOGE(TAG, "Unable to accept connection: errno %d", errno);
            return false;
        }
        ESP_LOGD(TAG, "Socket accepted!");
        FD_ZERO(&fdset);
        return true;
    } else if (s == 0) {
        return true;
    }
    return false;
}

void DCE::start_listening(int port)
{
    listen_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
    if (listen_sock < 0) {
        ESP_LOGE(TAG, "Unable to create socket: errno %d", errno);
        return;
    }
    int opt = 1;
    setsockopt(listen_sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    ESP_LOGI(TAG, "Socket created");
    struct sockaddr_in addr = { };
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
//    inet_aton("127.0.0.1", &addr.sin_addr);

    int err = bind(listen_sock, (struct sockaddr *)&addr, sizeof(addr));
    if (err != 0) {
        ESP_LOGE(TAG, "Socket unable to bind: errno %d", errno);
        return;
    }
    ESP_LOGI(TAG, "Socket bound, port %d", 1883);
    err = listen(listen_sock, 1);
    if (err != 0) {
        ESP_LOGE(TAG, "Error occurred during listen: errno %d", errno);
        return;
    }

}

bool DCE::connect(std::string host, int port)
{
    dte->on_read(nullptr);
    tcp_close();
    dte->on_read([this](uint8_t *data, size_t len) {
        this->perform_at(data, len);
        return esp_modem::command_result::TIMEOUT;
    });
    if (!at.start_connecting(host, port)) {
        ESP_LOGE(TAG, "Unable to start connecting");
        dte->on_read(nullptr);
        return false;
    }
    state = status::CONNECTING;
    return true;
}

bool DCE::init()
{
    esp_vfs_eventfd_config_t config = ESP_VFS_EVENTD_CONFIG_DEFAULT();
    esp_vfs_eventfd_register(&config);

    data_ready_fd = eventfd(0, EFD_SUPPORT_ISR);
    assert(data_ready_fd > 0);

    dte->on_read(nullptr);
    const int retries = 5;
    int i = 0;
    while (sync() != esp_modem::command_result::OK) {
        if (i++ > retries) {
            ESP_LOGE(TAG, "Failed to sync up");
            return false;
        }
        esp_modem::Task::Delay(1000);
    }
    ESP_LOGD(TAG, "Modem in sync");
    i = 0;
    while (setup_data_mode() != true) {
        if (i++ > retries) {
            ESP_LOGE(TAG, "Failed to setup pdp/data");
            return false;
        }
        esp_modem::Task::Delay(1000);
    }
    ESP_LOGD(TAG, "PDP configured");
    i = 0;
    while (net_open() != esp_modem::command_result::OK) {
        if (i++ > retries) {
            ESP_LOGE(TAG, "Failed to open network");
            return false;
        }
        net_close();
        esp_modem::Task::Delay(1000);
    }
    ESP_LOGD(TAG, "Network opened");
    i = 0;
    std::string ip_addr;
    while (get_ip(ip_addr) != esp_modem::command_result::OK) {
        if (i++ > retries) {
            ESP_LOGE(TAG, "Failed obtain an IP address");
            return false;
        }
        esp_modem::Task::Delay(5000);
    }
    ESP_LOGI(TAG, "Got IP %s", ip_addr.c_str());
    return true;
}


class Factory: public ::esp_modem::dce_factory::Factory {
public:
    static std::unique_ptr<DCE> create(const esp_modem::dce_config *config, std::shared_ptr<esp_modem::DTE> dte)
    {
        return esp_modem::dce_factory::Factory::build_module_T<DCE, std::unique_ptr<DCE>>(config, std::move(dte));
    }
};

std::unique_ptr<DCE> create(const esp_modem::dce_config *config, std::shared_ptr<esp_modem::DTE> dte)
{
    return Factory::create(config, std::move(dte));
}

// Helper macros to handle multiple arguments of declared API
#define ARGS0
#define ARGS1 , p1
#define ARGS2 , p1 , p2
#define ARGS3 , p1 , p2 , p3

#define EXPAND_ARGS(x)  ARGS ## x
#define ARGS(x)  EXPAND_ARGS(x)

//
// Repeat all declarations and forward to the AT commands defined in ::sock_commands namespace
//
#define ESP_MODEM_DECLARE_DCE_COMMAND(name, return_type, arg_nr, ...) \
     esp_modem::return_type DCE::name(__VA_ARGS__) { return sock_commands::name(dte.get() ARGS(arg_nr) ); }

DECLARE_SOCK_COMMANDS(return_type name(...) )

#undef ESP_MODEM_DECLARE_DCE_COMMAND

} // namespace sock_dce
