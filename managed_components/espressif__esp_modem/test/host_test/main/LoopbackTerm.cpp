/*
 * SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#include <memory>
#include <future>
#include <cstring>
#include "LoopbackTerm.h"

void LoopbackTerm::start()
{
    status = status_t::STARTED;
}

void LoopbackTerm::stop()
{
    status = status_t::STOPPED;
}

int LoopbackTerm::write(uint8_t *data, size_t len)
{
    if (inject_by) {    // injection test: ignore what we write, but respond with injected data
        signal.clear(1);
        auto ret = std::async(&LoopbackTerm::batch_read, this);
        async_results.push_back(std::move(ret));
        return len;
    }
    if (len > 2 && (data[len - 1] == '\r' || data[len - 1] == '+') ) { // Simple AT responder
        std::string command((char *)data, len);
        std::string response;
        if (command == "+++") {
            response = "NO CARRIER\r\n";
        } else if (command == "ATE1\r" || command == "ATE0\r") {
            response = "OK\r\n ";
        } else if (command == "ATO\r") {
            response = "ERROR\r\n";
        } else if (command.find("ATD") != std::string::npos) {
            response = "CONNECT\n";
        } else if (command.find("AT+CSQ\r") != std::string::npos) {
            response = "+CSQ: 123,456\n\r\nOK\r\n";
        } else if (command.find("AT+CGMM\r") != std::string::npos) {
            response = "0G Dummy Model\n\r\nOK\r\n";
        } else if (command.find("AT+COPS?\r") != std::string::npos) {
            response = "+COPS: 0,0,\"OperatorName\",5\n\r\nOK\r\n";
        } else if (command.find("AT+CBC\r") != std::string::npos) {
            response = is_bg96 ? "+CBC: 1,20,123456\r\r\n\r\nOK\r\n\n\r\n" :
                       "+CBC: 123.456V\r\r\n\r\nOK\r\n\n\r\n";
        } else if (command.find("AT+CPIN=1234\r") != std::string::npos) {
            response = "OK\r\n";
            pin_ok = true;
        } else if (command.find("AT+CPIN?\r") != std::string::npos) {
            response = pin_ok ? "+CPIN: READY\r\nOK\r\n" : "+CPIN: SIM PIN\r\nOK\r\n";
        } else if (command.find("AT") != std::string::npos) {
            if (command.length() > 4) {
                response = command;
                response[0] = 'O';
                response[1] = 'K';
                response[2] = '\r';
                response[3] = '\n';
            } else {
                response = "OK\r\n";
            }

        }
        if (!response.empty()) {
            data_len = response.length();
            loopback_data.resize(data_len);
            memcpy(&loopback_data[0], &response[0], data_len);
            signal.clear(1);
            auto ret = std::async(on_read, nullptr, data_len);
            return len;
        }
    }
    if (len > 2 && data[0] == 0xf9) { // Simple CMUX responder
        // turn the request into a reply -> implements CMUX loopback
        // Note: This simple CMUX responder only updates CMUX headers and replaces payload.
        // It means that all responses (that we test) must be shorter or equal to the requests
        // For example ATD (dial command): sizeof("ATD*99#") >= sizeof("CONNECT");
        if (data[2] == 0x3f || data[2] == 0x53) {  // SABM command
            data[2] = 0x73;
        } else if (data[2] == 0xef) { // Generic request
            data[2] = 0xff;         // generic reply
        }
    }
    loopback_data.resize(data_len + len);
    memcpy(&loopback_data[data_len], data, len);
    data_len += len;
    signal.clear(1);
    auto ret = std::async(on_read, nullptr, data_len);
    return len;
}

int LoopbackTerm::read(uint8_t *data, size_t len)
{
    size_t read_len = std::min(data_len, len);
    if (inject_by && read_len > inject_by) {
        read_len = inject_by;
    }
    if (read_len) {
        if (loopback_data.capacity() < len) {
            loopback_data.reserve(len);
        }
        memcpy(data, &loopback_data[0], read_len);
        loopback_data.erase(loopback_data.begin(), loopback_data.begin() + read_len);
        data_len -= read_len;
    }
    return read_len;
}

LoopbackTerm::LoopbackTerm(bool is_bg96): loopback_data(), data_len(0), pin_ok(false), is_bg96(is_bg96), inject_by(0)
{
    init_signal();
}

LoopbackTerm::LoopbackTerm(): loopback_data(), data_len(0), pin_ok(false), is_bg96(false), inject_by(0)
{
    init_signal();
}

int LoopbackTerm::inject(uint8_t *data, size_t len, size_t injected_by, size_t delay_before, size_t delay_after)
{
    if (data == nullptr) {
        inject_by = 0;
        return 0;
    }

    loopback_data.resize(len);
    memcpy(&loopback_data[0], data, len);
    data_len = len;
    inject_by = injected_by;
    delay_after_inject = delay_after;
    delay_before_inject = delay_before;
    return len;
}

void LoopbackTerm::batch_read()
{
    while (data_len > 0) {
        Task::Delay(delay_before_inject);
        {
            Scoped<Lock> lock(on_read_guard);
            on_read(nullptr, std::min(inject_by, data_len));
        }
        Task::Delay(delay_after_inject);
    }
    signal.set(1);
}

LoopbackTerm::~LoopbackTerm()
{
    data_len = 0;
    signal.wait(1, INT32_MAX); // wait "very long" to let the std::async() finish
}

void LoopbackTerm::init_signal()
{
    // This indicates, that we can safely exit
    // we clear the signal upon an async operation, so the destructor needs to wait until
    // it's finished
    signal.set(1);
}

void LoopbackTerm::set_read_cb(std::function<bool(uint8_t *, size_t)> f)
{
    user_on_read = std::move(f);
    on_read = [this](uint8_t *data, size_t len) {
        auto ret = user_on_read(data, len);
        signal.set(1);
        return ret;
    };
}
