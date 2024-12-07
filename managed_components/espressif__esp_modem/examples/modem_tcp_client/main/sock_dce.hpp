/*
 * SPDX-FileCopyrightText: 2021-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "esp_modem_config.h"
#include "cxx_include/esp_modem_api.hpp"
#include <cxx_include/esp_modem_dce_factory.hpp>
#include "socket_commands.inc"
#include "sock_commands.hpp"
#include <sys/socket.h>

#pragma  once

namespace sock_dce {


enum class status {
    IDLE,
    CONNECTING,
    SENDING,
    RECEIVING,
    FAILED,
    PENDING
};

class Responder {
public:
    enum class ret {
        OK, FAIL, IN_PROGRESS, NEED_MORE_DATA, NEED_MORE_TIME
    };
    Responder(int &s, int &ready_fd, std::shared_ptr<esp_modem::DTE> &dte_arg):
        sock(s), data_ready_fd(ready_fd), dte(dte_arg) {}
    ret process_data(status state, uint8_t *data, size_t len);
    ret check_async_replies(status state, std::string_view &response);

    void start_sending(size_t len);
    void start_receiving(size_t len);
    bool start_connecting(std::string host, int port);
    status pending();
    uint8_t *get_buf()
    {
        return &buffer[0];
    }
    size_t get_buf_len()
    {
        return buffer_size;
    }

    void clear_offsets()
    {
        actual_read = 0;
    }

    size_t data_available()
    {
        return actual_read;
    }

    size_t has_data()
    {
        return total_len;
    }

private:
    static constexpr size_t buffer_size = 512;

    bool on_read(char *data, size_t len)
    {
#ifndef CONFIG_EXAMPLE_CUSTOM_TCP_TRANSPORT
        ::send(sock, data, len, 0);
#else
        ::memcpy(&buffer[actual_read], data, len);
        actual_read += len;
#endif
        return true;
    }

    ret recv(uint8_t *data, size_t len);
    ret send(uint8_t *data, size_t len);
    ret send(std::string_view response);
    ret connect(std::string_view response);
    void send_cmd(std::string_view command)
    {
        dte->write((uint8_t *) command.begin(), command.size());
    }
    std::array<uint8_t, buffer_size> buffer;
    size_t data_to_recv = 0;
    size_t actual_read = 0;
    size_t total_len = 0;

    bool read_again = false;
    int &sock;
    int &data_ready_fd;
    int send_stat = 0;
    size_t data_to_send = 0;
    std::shared_ptr<esp_modem::DTE> &dte;
};

class DCE : public ::esp_modem::GenericModule {
    using esp_modem::GenericModule::GenericModule;
public:

#define ESP_MODEM_DECLARE_DCE_COMMAND(name, return_type, num, ...) \
esp_modem::return_type name(__VA_ARGS__);

    DECLARE_SOCK_COMMANDS(declare name(Commandable *p, ...);)

#undef ESP_MODEM_DECLARE_DCE_COMMAND

    bool init();
    bool connect(std::string host, int port);

    void start_listening(int port);

    bool perform_sock();

    void set_idle()
    {
        signal.set(IDLE);
    }

    bool wait_to_idle(uint32_t ms)
    {
        if (!signal.wait(IDLE, ms)) {
            ESP_LOGE("dce", "Failed to get idle");
            return false;
        }
        if (state != status::IDLE) {
            ESP_LOGE("dce", "Unexpected state %d", static_cast<int>(state));
            return false;
        }
        return true;
    }

    int sync_recv(char *buffer, int len, int timeout_ms)
    {
        if (!wait_to_idle(timeout_ms)) {
            return 0;
        }
        at.clear_offsets();
        state = status::RECEIVING;
        uint64_t data;
        read(data_ready_fd, &data, sizeof(data));
        int max_len = std::min(len, (int)at.get_buf_len());
        at.start_receiving(max_len);
        if (!signal.wait(IDLE, 500 + timeout_ms)) {
            return 0;
        }
        int ret = at.data_available();
        if (ret > 0) {
            memcpy(buffer, at.get_buf(), ret);
        }
        set_idle();
        return ret;
    }

    int sync_send(const char *buffer, size_t len, int timeout_ms)
    {
        int len_to_send = std::min(len, at.get_buf_len());
        if (!wait_to_idle(timeout_ms)) {
            return -1;
        }
        state = status::SENDING;
        memcpy(at.get_buf(), buffer, len_to_send);
        ESP_LOG_BUFFER_HEXDUMP("dce", at.get_buf(), len, ESP_LOG_VERBOSE);
        at.start_sending(len_to_send);
        if (!signal.wait(IDLE, timeout_ms + 1000)) {
            if (state == status::PENDING) {
                state = status::IDLE;
            } else {
                return -1;
            }
        }
        set_idle();
        return len_to_send;
    }

    int wait_to_read(uint32_t ms)
    {
        if (at.has_data() > 0) {
            ESP_LOGD("dce",  "Data buffered in modem (len=%d)", at.has_data());
            return 1;
        }
        struct timeval tv = {
            .tv_sec = static_cast<time_t>(ms / 1000),
            .tv_usec = 0,
        };
        fd_set fdset;
        FD_ZERO(&fdset);
        FD_SET(data_ready_fd, &fdset);
        int s = select(data_ready_fd + 1, &fdset, nullptr, nullptr, &tv);
        if (s == 0) {
            return 0;
        } else if (s < 0) {
            ESP_LOGE("dce",  "select error %d", errno);
            return -1;
        }
        if (FD_ISSET(data_ready_fd, &fdset)) {
            ESP_LOGD("dce", "select read: modem data available");
            return 1;
        }
        return -1;
    }

private:
    esp_modem::SignalGroup signal;

    void close_sock();
    bool accept_sock();
    bool sock_to_at();
    bool at_to_sock();

    void perform_at(uint8_t *data, size_t len);

    status state{status::IDLE};
    static constexpr uint8_t IDLE = 1;
    Responder at{sock, data_ready_fd, dte};
    int sock {-1};
    int listen_sock {-1};
    int data_ready_fd {-1};
};

std::unique_ptr<DCE> create(const esp_modem::dce_config *config, std::shared_ptr<esp_modem::DTE> dte);

}
