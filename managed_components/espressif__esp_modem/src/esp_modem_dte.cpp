/*
 * SPDX-FileCopyrightText: 2021-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <cstring>
#include "esp_log.h"
#include "cxx_include/esp_modem_dte.hpp"
#include "cxx_include/esp_modem_cmux.hpp"
#include "esp_modem_config.h"

using namespace esp_modem;

static const size_t dte_default_buffer_size = 1000;

DTE::DTE(const esp_modem_dte_config *config, std::unique_ptr<Terminal> terminal):
    buffer(config->dte_buffer_size),
    cmux_term(nullptr), primary_term(std::move(terminal)), secondary_term(primary_term),
    mode(modem_mode::UNDEF)
{
    set_command_callbacks();
}

DTE::DTE(std::unique_ptr<Terminal> terminal):
    buffer(dte_default_buffer_size),
    cmux_term(nullptr), primary_term(std::move(terminal)), secondary_term(primary_term),
    mode(modem_mode::UNDEF)
{
    set_command_callbacks();
}

DTE::DTE(const esp_modem_dte_config *config, std::unique_ptr<Terminal> t, std::unique_ptr<Terminal> s):
    buffer(config->dte_buffer_size),
    cmux_term(nullptr), primary_term(std::move(t)), secondary_term(std::move(s)),
    mode(modem_mode::DUAL_MODE)
{
    set_command_callbacks();
}

DTE::DTE(std::unique_ptr<Terminal> t, std::unique_ptr<Terminal> s):
    buffer(dte_default_buffer_size),
    cmux_term(nullptr), primary_term(std::move(t)), secondary_term(std::move(s)),
    mode(modem_mode::DUAL_MODE)
{
    set_command_callbacks();
}

void DTE::set_command_callbacks()
{
    primary_term->set_read_cb([this](uint8_t *data, size_t len) {
        Scoped<Lock> l(command_cb.line_lock);
#ifndef CONFIG_ESP_MODEM_URC_HANDLER
        if (command_cb.got_line == nullptr || command_cb.result != command_result::TIMEOUT) {
            return false;   // this line has been processed already (got OK or FAIL previously)
        }
#endif
        if (data) {
            // For terminals which post data directly with the callback (CMUX)
            // we cannot defragment unless we allocate, but
            // we'll try to process the data on the actual buffer
#ifdef CONFIG_ESP_MODEM_USE_INFLATABLE_BUFFER_IF_NEEDED
            if (inflatable.consumed != 0) {
                inflatable.grow(inflatable.consumed + len);
                std::memcpy(inflatable.current(), data, len);
                data = inflatable.begin();
            }
            if (command_cb.process_line(data, inflatable.consumed, len)) {
                return true;
            }
            // at this point we're sure that the data processing hasn't finished,
            // and we have to grow the inflatable buffer (if enabled) or give up
            if (inflatable.consumed == 0) {
                inflatable.grow(len);
                std::memcpy(inflatable.begin(), data, len);
            }
            inflatable.consumed += len;
            return false;
#else
            if (command_cb.process_line(data, 0, len)) {
                return true;
            }
            // cannot inflate and the processing hasn't finishes in the first iteration, but continue
            // (will post next fragments to the parser, since we might be just missing a last token or OK
            return false;
#endif
        }
        // data == nullptr: Terminals which request users to read current data
        // we're able to use DTE's buffer to defragment it; as long as we consume less that the buffer size
        if (buffer.size > buffer.consumed) {
            data = buffer.get();
            len = primary_term->read(data + buffer.consumed, buffer.size - buffer.consumed);
            if (command_cb.process_line(data, buffer.consumed, len)) {
                return true;
            }
            buffer.consumed += len;
            return false;
        }
        // we have used the entire DTE's buffer, need to use the inflatable buffer to continue
#ifdef CONFIG_ESP_MODEM_USE_INFLATABLE_BUFFER_IF_NEEDED
        if (inflatable.consumed == 0) {
            inflatable.grow(buffer.size + len);
            std::memcpy(inflatable.begin(), buffer.get(), buffer.size);
            inflatable.consumed = buffer.size;
        } else {
            inflatable.grow(inflatable.consumed + len);
        }
        len = primary_term->read(inflatable.current(), len);
        if (command_cb.process_line(inflatable.begin(), inflatable.consumed, len)) {
            return true;
        }
        inflatable.consumed += len;
        return false;
#else
        // cannot inflate -> report a failure
        command_cb.give_up();
        return true;
#endif
    });
    primary_term->set_error_cb([this](terminal_error err) {
        if (user_error_cb) {
            user_error_cb(err);
        }
        handle_error(err);
    });
    secondary_term->set_error_cb([this](terminal_error err) {
        if (user_error_cb) {
            user_error_cb(err);
        }
        handle_error(err);
    });

}

command_result DTE::command(const std::string &command, got_line_cb got_line, uint32_t time_ms, const char separator)
{
    Scoped<Lock> l1(internal_lock);
    command_cb.set(got_line, separator);
    primary_term->write((uint8_t *)command.c_str(), command.length());
    command_cb.wait_for_line(time_ms);
    command_cb.set(nullptr);
    buffer.consumed = 0;
#ifdef CONFIG_ESP_MODEM_USE_INFLATABLE_BUFFER_IF_NEEDED
    inflatable.deflate();
#endif
    return command_cb.result;
}

command_result DTE::command(const std::string &cmd, got_line_cb got_line, uint32_t time_ms)
{
    return command(cmd, got_line, time_ms, '\n');
}

bool DTE::exit_cmux()
{
    if (!cmux_term) {
        return false;
    }
    const bool success = cmux_term->deinit();
    exit_cmux_internal();
    cmux_term.reset();
    return success;
}

void DTE::exit_cmux_internal()
{
    if (!cmux_term) {
        return;
    }

    auto ejected = cmux_term->detach();
    // return the ejected terminal and buffer back to this DTE
    primary_term = std::move(ejected.first);
    buffer = std::move(ejected.second);
    secondary_term = primary_term;
    set_command_callbacks();
}

bool DTE::setup_cmux()
{
    if (cmux_term) {
        ESP_LOGE("esp_modem_dte", "Cannot setup_cmux(), cmux_term already exists");
        return false;
    }
    cmux_term = std::make_shared<CMux>(primary_term, std::move(buffer));
    if (cmux_term == nullptr) {
        return false;
    }

    if (!cmux_term->init()) {
        exit_cmux_internal();
        cmux_term = nullptr;
        return false;
    }

    primary_term   = std::make_unique<CMuxInstance>(cmux_term, 0);
    secondary_term = std::make_unique<CMuxInstance>(cmux_term, 1);
    if (primary_term == nullptr || secondary_term == nullptr) {
        exit_cmux_internal();
        cmux_term = nullptr;
        return false;
    }
    set_command_callbacks();
    return true;
}

bool DTE::set_mode(modem_mode m)
{
    // transitions (any) -> UNDEF
    if (m == modem_mode::UNDEF) {
        mode = m;
        return true;
    }
    // transitions (COMMAND|UNDEF) -> CMUX
    if (m == modem_mode::CMUX_MODE) {
        if (mode == modem_mode::UNDEF || mode == modem_mode::COMMAND_MODE) {
            if (setup_cmux()) {
                mode = m;
                return true;
            }
            mode = modem_mode::UNDEF;
            return false;
        }
    }
    // transitions (COMMAND|DUAL|CMUX|UNDEF) -> DATA
    if (m == modem_mode::DATA_MODE || m == modem_mode::RESUME_DATA_MODE) {
        if (mode == modem_mode::CMUX_MODE || mode == modem_mode::CMUX_MANUAL_MODE || mode == modem_mode::DUAL_MODE) {
            // mode stays the same, but need to swap terminals (as command has been switched)
            secondary_term.swap(primary_term);
            set_command_callbacks();
        } else {
            mode = modem_mode::DATA_MODE;
        }
        return true;
    }
    // transitions (DATA|DUAL|CMUX|UNDEF) -> COMMAND
    if (m == modem_mode::COMMAND_MODE) {
        if (mode == modem_mode::CMUX_MODE) {
            if (exit_cmux()) {
                mode = m;
                return true;
            }
            mode = modem_mode::UNDEF;
            return false;
        } if (mode == modem_mode::CMUX_MANUAL_MODE || mode == modem_mode::DUAL_MODE) {
            return true;
        } else {
            mode = m;
            return true;
        }
    }
    // manual CMUX transitions: Enter CMUX
    if (m == modem_mode::CMUX_MANUAL_MODE) {
        if (setup_cmux()) {
            mode = m;
            return true;
        }
        mode = modem_mode::UNDEF;
        return false;
    }
    // manual CMUX transitions: Exit CMUX
    if (m == modem_mode::CMUX_MANUAL_EXIT && (mode == modem_mode::CMUX_MANUAL_MODE || mode == modem_mode::UNDEF))  {
        if (exit_cmux()) {
            mode = modem_mode::COMMAND_MODE;
            return true;
        }
        mode = modem_mode::UNDEF;
        return false;
    }
    // manual CMUX transitions: Swap terminals
    if (m == modem_mode::CMUX_MANUAL_SWAP && (mode == modem_mode::CMUX_MANUAL_MODE || mode == modem_mode::UNDEF)) {
        secondary_term.swap(primary_term);
        set_command_callbacks();
        return true;
    }
    mode = modem_mode::UNDEF;
    return false;
}

void DTE::set_read_cb(std::function<bool(uint8_t *, size_t)> f)
{
    if (f == nullptr) {
        set_command_callbacks();
        return;
    }
    on_data = std::move(f);
    secondary_term->set_read_cb([this](uint8_t *data, size_t len) {
        if (!data) { // if no data available from terminal callback -> need to explicitly read some
            data = buffer.get();
            len = secondary_term->read(buffer.get(), buffer.size);
        }
        if (on_data) {
            return on_data(data, len);
        }
        return false;
    });
}

void DTE::set_error_cb(std::function<void(terminal_error err)> f)
{
    user_error_cb = std::move(f);
    set_command_callbacks();
}

int DTE::read(uint8_t **d, size_t len)
{
    auto data_to_read = std::min(len, buffer.size);
    auto data = buffer.get();
    auto actual_len = secondary_term->read(data, data_to_read);
    *d = data;
    return actual_len;
}

int DTE::write(uint8_t *data, size_t len)
{
    return secondary_term->write(data, len);
}

int DTE::send(uint8_t *data, size_t len, int term_id)
{
    Terminal *term = term_id == 0 ? primary_term.get() : secondary_term.get();
    return term->write(data, len);
}

int DTE::write(DTE_Command command)
{
    return primary_term->write(command.data, command.len);
}

void DTE::on_read(got_line_cb on_read_cb)
{
    if (on_read_cb == nullptr) {
        primary_term->set_read_cb(nullptr);
        internal_lock.unlock();
        set_command_callbacks();
        return;
    }
    internal_lock.lock();
    primary_term->set_read_cb([this, on_read_cb](uint8_t *data, size_t len) {
        if (!data) {
            data = buffer.get();
            len = primary_term->read(data, buffer.size);
        }
        auto res = on_read_cb(data, len);
        if (res == command_result::OK || res == command_result::FAIL) {
            primary_term->set_read_cb(nullptr);
            internal_lock.unlock();
            return true;
        }
        return false;
    });
}

bool DTE::command_cb::process_line(uint8_t *data, size_t consumed, size_t len)
{
#ifdef CONFIG_ESP_MODEM_URC_HANDLER
    if (urc_handler) {
        urc_handler(data, consumed + len);
    }
    if (result != command_result::TIMEOUT || got_line == nullptr) {
        return false;   // this line has been processed already (got OK or FAIL previously)
    }
#endif
    if (memchr(data + consumed, separator, len)) {
        result = got_line(data, consumed + len);
        if (result == command_result::OK || result == command_result::FAIL) {
            signal.set(GOT_LINE);
            return true;
        }
    }
    return false;
}

bool DTE::recover()
{
    if (mode == modem_mode::CMUX_MODE || mode == modem_mode::CMUX_MANUAL_MODE || mode == modem_mode::DUAL_MODE) {
        return cmux_term->recover();
    }
    return false;
}

void DTE::handle_error(terminal_error err)
{
    if (err == terminal_error::BUFFER_OVERFLOW ||
            err == terminal_error::CHECKSUM_ERROR ||
            err == terminal_error::UNEXPECTED_CONTROL_FLOW) {
        recover();
    }
}

#ifdef CONFIG_ESP_MODEM_USE_INFLATABLE_BUFFER_IF_NEEDED
void DTE::extra_buffer::grow(size_t need_size)
{
    if (need_size == 0) {
        delete buffer;
        buffer = nullptr;
    } else if (buffer == nullptr) {
        buffer = new std::vector<uint8_t>(need_size);
    } else {
        buffer->resize(need_size);
    }
}
#endif

/**
 * Implemented here to keep all headers C++11 compliant
 */
unique_buffer::unique_buffer(size_t size):
    data(std::make_unique<uint8_t[]>(size)), size(size), consumed(0) {}
