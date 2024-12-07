/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <charconv>
#include <cstring>
#include "sock_commands.hpp"
#include "cxx_include/esp_modem_command_library_utils.hpp"
#include "sock_dce.hpp"

static const char *TAG = "sock_commands";

namespace sock_commands {

using namespace esp_modem;

command_result net_open(CommandableIf *term)
{
    ESP_LOGV(TAG, "%s", __func__ );
    std::string response;
    auto ret = dce_commands::generic_get_string(term, "AT+NETOPEN?\r", response, 1000);
    if (ret != command_result::OK) {
        return ret;
    }
    ESP_LOGV(TAG, "%s", response.data() );
    if (response.find("+NETOPEN: 1") != std::string::npos) {
        ESP_LOGD(TAG, "Already there");
        ret = command_result::OK;
    } else if (response.find("+NETOPEN: 0") != std::string::npos) {
        ESP_LOGD(TAG, "Need to setup");
        ret = dce_commands::generic_command(term, "AT+NETOPEN\r", "+NETOPEN: 1", "+NETOPEN: 0", 10000);
    } else {
        return command_result::FAIL;
    }
    if (ret != command_result::OK) {
        return ret;
    }
    return dce_commands::generic_command(term, "AT+CIPRXGET=1\r", "OK", "ERROR", 5000);
}

command_result net_close(CommandableIf *term)
{
    ESP_LOGV(TAG, "%s", __func__ );
    return dce_commands::generic_command(term, "AT+NETCLOSE\r", "+NETCLOSE:", "ERROR", 30000);
}

command_result tcp_open(CommandableIf *term, const std::string &host, int port, int timeout)
{
    ESP_LOGV(TAG, "%s", __func__ );
    auto ret = dce_commands::generic_command(term, "AT+CIPRXGET=1\r", "OK", "ERROR", 50000);
    if (ret != command_result::OK) {
        ESP_LOGE(TAG, "Setting Rx mode failed!");
        return ret;
    }
    ESP_LOGV(TAG, "%s", __func__ );
    std::string ip_open = R"(AT+CIPOPEN=0,"TCP",")" + host + "\"," + std::to_string(port) + "\r";
    ret = dce_commands::generic_command(term, ip_open, "+CIPOPEN: 0,0", "ERROR", timeout);
    if (ret != command_result::OK) {
        ESP_LOGE(TAG, "%s Failed", __func__ );
        return ret;
    }
    return command_result::OK;
}

command_result tcp_close(CommandableIf *term)
{
    ESP_LOGV(TAG, "%s", __func__ );
    return dce_commands::generic_command(term, "AT+CIPCLOSE=0\r", "+CIPCLOSE:", "ERROR", 10000);
}

command_result tcp_send(CommandableIf *term, uint8_t *data, size_t len)
{
    ESP_LOGV(TAG, "%s", __func__ );
    std::string send = "AT+CIPSEND=0," + std::to_string(len) + "\r";
    auto ret = term->command(send, [&](uint8_t *data, size_t len) {
        std::string_view response((char *)data, len);
        ESP_LOGI(TAG, "CIPSEND response %.*s", static_cast<int>(response.size()), response.data());
        if (response.find('>') != std::string::npos) {
            return command_result::OK;
        }
        return command_result::TIMEOUT;
    }, 50000, '>');
    if (ret != command_result::OK) {
        return ret;
    }
    ret = command_result::TIMEOUT;
    ESP_LOGW(TAG, "Before setting...");
    term->on_read([&ret](uint8_t *cmd_data, size_t cmd_len) {
        std::string_view response((char *)cmd_data, cmd_len);
        ESP_LOGW(TAG, "CIPSEND response %.*s", static_cast<int>(response.size()), response.data());

        if (response.find("+CIPSEND:") != std::string::npos) {
            ret = command_result::OK;
        } else if (response.find("ERROR") != std::string::npos) {
            ret = command_result::FAIL;
        }
        return ret;
    });
    ESP_LOGW(TAG, "Before writing...");
    auto written = term->write(data, len);
    if (written != len) {
        ESP_LOGE(TAG, "written %d (%d)...", written, len);
        return command_result::FAIL;
    }
    uint8_t ctrl_z = '\x1A';
    term->write(&ctrl_z, 1);
    int count = 0;
    while (ret == command_result::TIMEOUT && count++ < 1000 ) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
    term->on_read(nullptr);
    return ret;
}

command_result tcp_recv(CommandableIf *term, uint8_t *data, size_t len, size_t &out_len)
{
    ESP_LOGV(TAG, "%s", __func__ );
    std::string out;
    auto ret = dce_commands::generic_get_string(term, "AT+CIPRXGET=4,0\r", out);
    if (ret != command_result::OK) {
        return ret;
    }
    constexpr std::string_view pattern = "+CIPRXGET: 4,0,";
    if (out.find(pattern) == std::string::npos) {

        return command_result::FAIL;
    }
    size_t data_len;
    if (std::from_chars(out.data() + pattern.size(), out.data() + out.size(), data_len).ec == std::errc::invalid_argument) {
        return command_result::FAIL;
    }
    ESP_LOGD(TAG, "size=%d", data_len);
    if (data_len == 0) {
        out_len = data_len;
        return command_result::OK;
    }
    return term->command("AT+CIPRXGET=2,0,100\r", [&](uint8_t *cmd_data, size_t cmd_len) {
        char pattern[] = "+CIPRXGET: 2,0,";
        ESP_LOG_BUFFER_HEXDUMP(TAG, cmd_data, cmd_len, ESP_LOG_DEBUG);
        char *pos = strstr((char *)cmd_data, pattern);
        if (pos == nullptr) {
            return command_result::FAIL;
        }
        auto p1 = memchr(pos + sizeof(pattern) - 1, ',', 4);
        if (p1 == nullptr)  {
            return command_result::FAIL;
        }
        *(char *)p1 = '\0';
        size_t actual_len = atoi(pos + sizeof(pattern) - 1);
        ESP_LOGD(TAG, "actual len=%d", actual_len);

        pos = strchr((char *)p1 + 1, '\n');
        if (pos == nullptr) {
            ESP_LOGE(TAG, "not found");
            return command_result::FAIL;
        }
        if (actual_len > len) {
            ESP_LOGE(TAG, "TOO BIG");
            return command_result::FAIL;
        }
        out_len = actual_len;
        memcpy(data, pos + 1, actual_len);
        pos = strstr((char *)pos + 1 + actual_len, "OK");
        if (pos == nullptr) {
            ESP_LOGE(TAG, "ok NOT FOUND");
            return command_result::FAIL;
        }
        return command_result::OK;
    }, 50000);
}

command_result get_ip(CommandableIf *term, std::string &ip)
{
    std::string resp;
    auto ret = dce_commands::generic_get_string(term, "AT+IPADDR\r", resp, 5000);
    if (ret != command_result::OK) {
        return ret;
    }
    ip = resp;
    return command_result::OK;
}

command_result set_rx_mode(CommandableIf *term, int mode)
{
    return dce_commands::generic_command(term, "AT+CIPRXGET=" + std::to_string(mode) + "\r", "OK", "ERROR", 5000);
}

} // sock_commands

namespace sock_dce {

void Responder::start_sending(size_t len)
{
    data_to_send = len;
    send_stat = 0;
    send_cmd("AT+CIPSEND=0," + std::to_string(len) + "\r");
}

void Responder::start_receiving(size_t len)
{
    send_cmd("AT+CIPRXGET=2,0," + std::to_string(len) + "\r");
}

bool Responder::start_connecting(std::string host, int port)
{
    send_cmd(R"(AT+CIPOPEN=0,"TCP",")" + host + "\"," + std::to_string(port) + "\r");
    return true;
}

Responder::ret Responder::recv(uint8_t *data, size_t len)
{
    const int MIN_MESSAGE = 6;
    size_t actual_len = 0;
    auto *recv_data = (char *)data;
    if (data_to_recv == 0) {
        static constexpr std::string_view head = "+CIPRXGET: 2,0,";
        auto head_pos = std::search(recv_data, recv_data + len, head.begin(), head.end());
        if (head_pos == recv_data + len) {
            return ret::FAIL;
        }

        if (head_pos - (char *)data > MIN_MESSAGE) {
            // check for async replies before the Recv header
            std::string_view response((char *)data, head_pos - (char *)data);
            check_async_replies(status::RECEIVING, response);
        }

        auto next_comma = (char *)memchr(head_pos + head.size(), ',', MIN_MESSAGE);
        if (next_comma == nullptr)  {
            return ret::FAIL;
        }
        if (std::from_chars(head_pos + head.size(), next_comma, actual_len).ec == std::errc::invalid_argument) {
            ESP_LOGE(TAG, "cannot convert");
            return ret::FAIL;
        }

        auto next_nl = (char *)memchr(next_comma, '\n', 8 /* total_len size (~4) + markers */);
        if (next_nl == nullptr) {
            ESP_LOGE(TAG, "not found");
            return ret::FAIL;
        }
        if (actual_len > buffer_size) {
            ESP_LOGE(TAG, "TOO BIG");
            return ret::FAIL;
        }
        total_len = 0;
        if (std::from_chars(next_comma + 1, next_nl - 1, total_len).ec == std::errc::invalid_argument) {
            ESP_LOGE(TAG, "cannot convert");
            return ret::FAIL;
        }
        read_again = (total_len > 0);
        recv_data = next_nl + 1;
        auto first_data_len = len - (recv_data - (char *)data) /* minus size of the command marker */;
        if (actual_len > first_data_len) {
            on_read(recv_data, first_data_len);
            data_to_recv = actual_len - first_data_len;
            return ret::NEED_MORE_DATA;
        }
        on_read(recv_data, actual_len);
    } else if (data_to_recv > len) {    // continue sending
        on_read(recv_data, len);
        data_to_recv -= len;
        return ret::NEED_MORE_DATA;
    } else if (data_to_recv <= len) {    // last read -> looking for "OK" marker
        on_read(recv_data, data_to_recv);
        actual_len = data_to_recv;
    }

    // "OK" after the data
    char *last_pos = nullptr;
    if (actual_len + 1 + 2 /* OK */  > len) {
        last_pos = (char *)memchr(recv_data + 1 + actual_len, 'O', MIN_MESSAGE);
        if (last_pos == nullptr || last_pos[1] != 'K') {
            data_to_recv = 0;
            return ret::FAIL;
        }
    }
    if (last_pos != nullptr && (char *)data + len - last_pos - 2 > MIN_MESSAGE) {
        // check for async replies after the Recv header
        std::string_view response((char *)last_pos + 2 /* OK */, (char *)data + len - last_pos - 2);
        check_async_replies(status::RECEIVING, response);
    }
    data_to_recv = 0;
    if (read_again) {
        uint64_t data_ready = 1;
        write(data_ready_fd, &data_ready, sizeof(data_ready));
    }
    return ret::OK;
}

Responder::ret Responder::send(uint8_t *data, size_t len)
{
    if (send_stat == 0) {
        if (memchr(data, '>', len) == NULL) {
            ESP_LOGE(TAG, "Missed >");
            return ret::FAIL;
        }
        auto written = dte->write(&buffer[0], data_to_send);
        if (written != data_to_send) {
            ESP_LOGE(TAG, "written %d (%d)...", written, len);
            return ret::FAIL;
        }
        data_to_send = 0;
        uint8_t ctrl_z = '\x1A';
        dte->write(&ctrl_z, 1);
        send_stat++;
        return ret::IN_PROGRESS;
    }
    return Responder::ret::IN_PROGRESS;
}

Responder::ret Responder::send(std::string_view response)
{
    if (send_stat == 1) {
        if (response.find("+CIPSEND:") != std::string::npos) {
            send_stat = 0;
            return ret::OK;
        }
        if (response.find("ERROR") != std::string::npos) {
            ESP_LOGE(TAG, "Failed to sent");
            send_stat = 0;
            return ret::FAIL;
        }
    }
    return Responder::ret::IN_PROGRESS;
}

Responder::ret Responder::connect(std::string_view response)
{
    if (response.find("+CIPOPEN: 0,0") != std::string::npos) {
        ESP_LOGI(TAG, "Connected!");
        return ret::OK;
    }
    if (response.find("ERROR") != std::string::npos) {
        ESP_LOGE(TAG, "Failed to open");
        return ret::FAIL;
    }
    return Responder::ret::IN_PROGRESS;
}

Responder::ret Responder::check_async_replies(status state, std::string_view &response)
{
    ESP_LOGD(TAG, "response %.*s", static_cast<int>(response.size()), response.data());
    if (response.find("+CIPRXGET: 1") != std::string::npos) {
        uint64_t data_ready = 1;
        write(data_ready_fd, &data_ready, sizeof(data_ready));
        ESP_LOGD(TAG, "Got data on modem!");
    }
    if (state == status::SENDING) {
        return send(response);
    } else if (state == status::CONNECTING) {
        return connect(response);
    }
    return ret::IN_PROGRESS;

}

Responder::ret Responder::process_data(status state, uint8_t *data, size_t len)
{
    if (state == status::SENDING) {
        return send(data, len);
    }
    if (state == status::RECEIVING) {
        return recv(data, len);
    }
    return Responder::ret::IN_PROGRESS;
}

status Responder::pending()
{
    return status::PENDING;
}


} // sock_dce
