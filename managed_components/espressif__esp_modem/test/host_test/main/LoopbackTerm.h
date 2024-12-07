/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#pragma once

#include "cxx_include/esp_modem_api.hpp"
#include "cxx_include/esp_modem_terminal.hpp"

using namespace esp_modem;

class LoopbackTerm : public Terminal {
public:
    explicit LoopbackTerm(bool is_bg96);
    explicit LoopbackTerm();

    ~LoopbackTerm() override;

    /**
     * @brief Inject user data to the terminal, to respond.
     * inject_by defines batch sizes: the read callback is called multiple times
     * with partial data of `inject_by` size
     */
    int inject(uint8_t *data, size_t len, size_t inject_by, size_t delay_before = 0, size_t delay_after = 1);

    void start() override;
    void stop() override;

    int write(uint8_t *data, size_t len) override;

    int read(uint8_t *data, size_t len) override;

    void set_read_cb(std::function<bool(uint8_t *data, size_t len)> f) override;

private:
    enum class status_t {
        STARTED,
        STOPPED
    };
    void batch_read();
    std::function<bool(uint8_t *data, size_t len)> user_on_read;
    status_t status;
    SignalGroup signal;
    void init_signal();
    std::vector<uint8_t> loopback_data;
    size_t data_len;
    bool pin_ok;
    bool is_bg96;
    size_t inject_by;
    size_t delay_before_inject;
    size_t delay_after_inject;
    std::vector<std::future<void>> async_results;
    Lock on_read_guard;

};
