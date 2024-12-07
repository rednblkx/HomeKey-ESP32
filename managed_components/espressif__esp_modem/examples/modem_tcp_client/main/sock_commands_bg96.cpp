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

command_result net_open(CommandableIf *t)
{
    ESP_LOGV(TAG, "%s", __func__ );
    std::string out;
    auto ret = dce_commands::generic_get_string(t, "AT+QISTATE?\r", out, 1000);
    if (ret != command_result::OK) {
        return ret;
    }
    if (out.find("+QISTATE: 0") != std::string::npos) {
        ESP_LOGV(TAG, "%s", out.data() );
        ESP_LOGD(TAG, "Already there");
        return command_result::FAIL;
    } else if (out.empty()) {
        return dce_commands::generic_command(t, "AT+QIACT=1\r", "OK", "ERROR", 150000);
    }
    return command_result::FAIL;
}

command_result net_close(CommandableIf *t)
{
    ESP_LOGV(TAG, "%s", __func__ );
    dce_commands::generic_command(t, "AT+QICLOSE=0\r", "OK", "ERROR", 10000);
    esp_modem::Task::Delay(1000);
    return dce_commands::generic_command(t, "AT+QIDEACT=1\r", "OK", "ERROR", 40000);
}

command_result tcp_open(CommandableIf *t, const std::string &host, int port, int timeout)
{
    ESP_LOGV(TAG, "%s", __func__ );
    std::string ip_open = R"(AT+QIOPEN=1,0,"TCP",")" + host + "\"," + std::to_string(port) + "\r";
    auto ret = dce_commands::generic_command(t, ip_open, "+QIOPEN: 0,0", "ERROR", timeout);
    if (ret != command_result::OK) {
        ESP_LOGE(TAG, "%s Failed", __func__ );
        return ret;
    }
    return command_result::OK;
}

command_result tcp_close(CommandableIf *t)
{
    ESP_LOGV(TAG, "%s", __func__ );
    return dce_commands::generic_command(t, "AT+QICLOSE=0\r", "OK", "ERROR", 10000);
}

command_result tcp_send(CommandableIf *t, uint8_t *data, size_t len)
{
    ESP_LOGV(TAG, "%s", __func__ );
    assert(0);      // Remove when fix done
    return command_result::FAIL;
}

command_result tcp_recv(CommandableIf *t, uint8_t *data, size_t len, size_t &out_len)
{
    ESP_LOGV(TAG, "%s", __func__ );
    assert(0);      // Remove when fix done
    return command_result::FAIL;
}

command_result get_ip(CommandableIf *t, std::string &ip)
{
    ESP_LOGV(TAG, "%s", __func__ );
    std::string out;
    auto ret = dce_commands::generic_get_string(t, "AT+QIACT?\r", out, 5000);
    if (ret != command_result::OK) {
        return ret;
    }
    auto pos = out.find("+QIACT: 1");
    auto property = 0;
    while (pos != std::string::npos) {
        // Looking for: +QIACT: <contextID>,<context_state>,<context_type>,<IP_address>
        if (property++ == 3) {  // ip is after 3rd comma (as a 4rd property of QIACT string)
            ip = out.substr(++pos);
            // strip quotes if present
            auto quote1 = ip.find('"');
            auto quote2 = ip.rfind('"');
            if (quote1 != std::string::npos && quote2 != std::string::npos) {
                ip = ip.substr(quote1 + 1, quote2 - 1);
            }
            return command_result::OK;
        }
        pos = out.find(',', ++pos);
    }
    return command_result::FAIL;
}

} // sock_commands

namespace sock_dce {

void Responder::start_sending(size_t len)
{
    data_to_send = len;
    send_stat = 0;
    send_cmd("AT+QISEND=0," + std::to_string(len) + "\r");
}

void Responder::start_receiving(size_t len)
{
    send_cmd("AT+QIRD=0," + std::to_string(len) + "\r");
}

bool Responder::start_connecting(std::string host, int port)
{
    send_cmd(R"(AT+QIOPEN=1,0,"TCP",")" + host + "\"," + std::to_string(port) + "\r");
    return true;
}

Responder::ret Responder::recv(uint8_t *data, size_t len)
{
    const int MIN_MESSAGE = 6;
    size_t actual_len = 0;
    auto *recv_data = (char *)data;
    if (data_to_recv == 0) {
        const std::string_view head = "+QIRD: ";
        auto head_pos = std::search(recv_data, recv_data + len, head.begin(), head.end());
        if (head_pos == recv_data + len) {
            return ret::FAIL;
        }

        auto next_nl = (char *)memchr(head_pos + head.size(), '\n', MIN_MESSAGE);
        if (next_nl == nullptr) {
            return ret::FAIL;
        }

        if (std::from_chars(head_pos + head.size(), next_nl, actual_len).ec == std::errc::invalid_argument) {
            ESP_LOGE(TAG, "cannot convert");
            return ret::FAIL;
        }

        ESP_LOGD(TAG, "Received: actual len=%d", actual_len);
        if (actual_len == 0) {
            ESP_LOGD(TAG, "no data received");
            return ret::FAIL;
        }

        if (actual_len > buffer_size) {
            ESP_LOGE(TAG, "TOO BIG");
            return ret::FAIL;
        }

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
        std::string_view response((char *)last_pos + 2 /* OK */, (char *)data + len - last_pos);
        check_async_replies(status::RECEIVING, response);
    }
    // check if some other data?
    start_receiving(0);
    data_to_recv = 0;
    return ret::OK;
}


Responder::ret Responder::send(uint8_t *data, size_t len)
{
    if (send_stat < 3) {
        if (memchr(data, '>', len) == NULL) {
            if (send_stat++ < 2) {
                return Responder::ret::NEED_MORE_DATA;
            }
            ESP_LOGE(TAG, "Missed >");
            return ret::FAIL;
        }
        auto written = dte->write(&buffer[0], data_to_send);
        if (written != data_to_send) {
            ESP_LOGE(TAG, "written %d (%d)...", written, len);
            return ret::FAIL;
        }
        data_to_send = 0;
        send_stat = 3;
    }
    return Responder::ret::IN_PROGRESS;
}

Responder::ret Responder::send(std::string_view response)
{
    if (send_stat == 3) {
        if (response.find("SEND OK") != std::string::npos) {
            send_cmd("AT+QISEND=0,0\r");
            send_stat++;
            return ret::IN_PROGRESS;
        } else if (response.find("SEND FAIL") != std::string::npos) {
            ESP_LOGE(TAG, "Sending buffer full");
            return ret::FAIL;
        } else if (response.find("ERROR") != std::string::npos) {
            ESP_LOGE(TAG, "Failed to sent");
            return ret::FAIL;
        }
    } else if (send_stat == 4) {
        constexpr std::string_view head = "+QISEND: ";
        if (response.find(head) != std::string::npos) {
            // Parsing +QISEND: <total_send_length>,<ackedbytes>,<unackedbytes>
            size_t head_pos = response.find(head);
            response = response.substr(head_pos + head.size());
            int pos, property = 0;
            int total = 0, ack = 0, unack = 0;
            while ((pos = response.find(',')) != std::string::npos) {
                auto next_comma = (char *)memchr(response.data(), ',', response.size());

                // extract value
                size_t value;
                if (std::from_chars(response.data(), next_comma, value).ec == std::errc::invalid_argument) {
                    ESP_LOGE(TAG, "cannot convert");
                    return ret::FAIL;
                }

                switch (property++) {
                case 0: total = value;
                    break;
                case 1: ack = value;
                    break;
                default:
                    return ret::FAIL;
                }
                response = response.substr(pos + 1);
            }
            if (std::from_chars(response.data(), response.data() + pos, unack).ec == std::errc::invalid_argument) {
                return ret::FAIL;
            }

            if (ack < total) {
                ESP_LOGD(TAG, "all sending data are not ack (missing %d bytes acked)", (total - ack));
                if (total - ack > 64) {
                    ESP_LOGW(TAG, "Need a pause: missing %d bytes acked", (total - ack));
                    return ret::NEED_MORE_TIME;
                }
            }
            send_stat = 0;
            return ret::OK;
        } else if (response.find("ERROR") != std::string::npos) {
            ESP_LOGE(TAG, "Failed to check sending");
            return ret::FAIL;
        }

    }
    return Responder::ret::IN_PROGRESS;
}

Responder::ret Responder::connect(std::string_view response)
{
    if (response.find("+QIOPEN: 0,0") != std::string::npos) {
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
    if (response.find("+QIURC: \"recv\",0") != std::string::npos) {
        uint64_t data_ready = 1;
        write(data_ready_fd, &data_ready, sizeof(data_ready));
        ESP_LOGD(TAG, "Got data on modem!");
    } else if (response.find("+QIRD: ") != std::string::npos) {
        static constexpr std::string_view head = "+QIRD: ";
        size_t head_pos = response.find(head);
        // Parsing +QIURC: <total_receive_length>,<have_read_length>,<unread_length>
        response = response.substr(head_pos + head.size());
        int next_cr = response.find('\r');
        if (next_cr != std::string::npos) {
            response = response.substr(next_cr - 2, next_cr);
            if (response.find(",0") != std::string::npos) {
                ESP_LOGV(TAG, "Receiving done");
            } else {
                uint64_t data_ready = 1;
                write(data_ready_fd, &data_ready, sizeof(data_ready));
                ESP_LOGD(TAG, "Got data on modem!");
            }
        }
    } else if (response.find("+QIURC: \"closed\",0") != std::string::npos) {
        return ret::FAIL;
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
    send_cmd("AT+QISEND=0,0\r");
    return status::SENDING;
}


} // sock_dce
