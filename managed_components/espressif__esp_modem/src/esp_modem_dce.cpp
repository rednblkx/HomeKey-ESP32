/*
 * SPDX-FileCopyrightText: 2021-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <list>
#include <unistd.h>
#include <cstring>

#include "cxx_include/esp_modem_dte.hpp"
#include "cxx_include/esp_modem_dce.hpp"
#include "esp_log.h"

namespace esp_modem {

namespace transitions {

static bool exit_data(DTE &dte, ModuleIf &device, Netif &netif)
{
    auto signal = std::make_shared<SignalGroup>();
    std::weak_ptr<SignalGroup> weak_signal = signal;
    dte.set_read_cb([&netif, weak_signal](uint8_t *data, size_t len) -> bool {
        // post the transitioning data to the network layers if it contains PPP SOF marker
        if (memchr(data, 0x7E, len))
        {
            ESP_LOG_BUFFER_HEXDUMP("esp-modem: debug_data (PPP)", data, len, ESP_LOG_DEBUG);
            netif.receive(data, len);
        }
        // treat the transitioning data as a textual message if it contains a newline char
        if (memchr(data, '\n', len))
        {
            ESP_LOG_BUFFER_HEXDUMP("esp-modem: debug_data (CMD)", data, len, ESP_LOG_DEBUG);
            const auto pass = std::list<std::string_view>({"NO CARRIER", "DISCONNECTED", "OK"});
            std::string_view response((char *) data, len);
            for (auto &it : pass)
                if (response.find(it) != std::string::npos) {
                    if (auto signal = weak_signal.lock()) {
                        signal->set(1);
                    }
                    return true;
                }
        }
        return false;
    });
    netif.stop();
    netif.wait_until_ppp_exits();
#ifdef ESP_MODEM_PPP_ESCAPE_BEFORE_EXIT
    std::array<uint8_t, 3> ppp_escape = {'+', '+', '+'};
    dte.write(ppp_escape.data(), ppp_escape.size());
#endif
    if (!signal->wait(1, 2000)) { // wait for any of the disconnection messages
        // if no reply -> set device to command mode
        dte.set_read_cb(nullptr);
        if (!device.set_mode(modem_mode::COMMAND_MODE)) {
            return false;
        }
    }
    dte.set_read_cb(nullptr);
    if (!dte.set_mode(modem_mode::COMMAND_MODE)) {
        return false;
    }
    return true;
}

static bool enter_data(DTE &dte, ModuleIf &device, Netif &netif)
{
    if (!device.setup_data_mode()) {
        return false;
    }
    if (!device.set_mode(modem_mode::DATA_MODE)) {
        return false;
    }
    if (!dte.set_mode(modem_mode::DATA_MODE)) {
        return false;
    }
    netif.start();
    return true;
}

} // namespace transitions

/**
 * Set mode while the entire DTE is locked
*/
bool DCE_Mode::set(DTE *dte, ModuleIf *device, Netif &netif, modem_mode m)
{
    Scoped<DTE> lock(*dte);
    return set_unsafe(dte, device, netif, m);
}

/**
 * state machine:
 *
 * COMMAND_MODE <----> DATA_MODE
 * COMMAND_MODE <----> CMUX_MODE
 *
 * UNDEF <----> any
 */
bool DCE_Mode::set_unsafe(DTE *dte, ModuleIf *device, Netif &netif, modem_mode m)
{
    switch (m) {
    case modem_mode::UNDEF:
        if (!dte->set_mode(m)) {
            return false;
        }
        mode = m;
        return true;
    case modem_mode::DUAL_MODE: // Only DTE can be in Dual mode
        break;
    case modem_mode::AUTODETECT: {
        auto guessed = guess_unsafe(dte, true);
        if (guessed == modem_mode::UNDEF) {
            return false;
        }
        // prepare the undefined mode before to allow all possible transitions
        if (!dte->set_mode(modem_mode::UNDEF)) {
            return false;
        }
        mode = modem_mode::UNDEF;
        ESP_LOGD("DCE mode", "Detected mode: %d", static_cast<int>(guessed));
        if (guessed == modem_mode::DATA_MODE) {
            return set_unsafe(dte, device, netif, esp_modem::modem_mode::RESUME_DATA_MODE);
        } else if (guessed == esp_modem::modem_mode::COMMAND_MODE) {
            return set_unsafe(dte, device, netif, esp_modem::modem_mode::RESUME_COMMAND_MODE);
        } else if (guessed == esp_modem::modem_mode::CMUX_MODE) {
            if (!set_unsafe(dte, device, netif, esp_modem::modem_mode::RESUME_CMUX_MANUAL_MODE)) {
                return false;
            }
            // now we guess the mode for each terminal
            guessed = guess_unsafe(dte, false);
            ESP_LOGD("DCE mode", "Detected mode on primary term: %d", static_cast<int>(guessed));
            // now we need to access the second terminal, so we could simply send a SWAP command
            // (switching to data mode does the swapping internally, so we only swap if we're in CMD mode)
            if (guessed == modem_mode::DATA_MODE) {
                // switch to DATA on the primary terminal and swap terminals
                if (!set_unsafe(dte, device, netif, esp_modem::modem_mode::RESUME_CMUX_MANUAL_DATA)) {
                    return false;
                }
            } else {
                // swap terminals
                if (!set_unsafe(dte, device, netif, esp_modem::modem_mode::CMUX_MANUAL_SWAP)) {
                    return false;
                }
            }
            guessed = guess_unsafe(dte, false);
            ESP_LOGD("DCE mode", "Detected mode on secondary term: %d", static_cast<int>(guessed));
            if (guessed == modem_mode::DATA_MODE) {
                if (!set_unsafe(dte, device, netif, esp_modem::modem_mode::RESUME_CMUX_MANUAL_DATA)) {
                    return false;
                }
            }
        }
        return true;
    }
    case modem_mode::COMMAND_MODE:
        if (mode == modem_mode::COMMAND_MODE || mode >= modem_mode::CMUX_MANUAL_MODE) {
            return false;
        }
        if (mode == modem_mode::CMUX_MODE) {
            netif.stop();
            netif.wait_until_ppp_exits();
            if (!dte->set_mode(modem_mode::COMMAND_MODE)) {
                return false;
            }
            mode = m;
            return true;
        }
        if (!transitions::exit_data(*dte, *device, netif)) {
            mode = modem_mode::UNDEF;
            return false;
        }
        mode = m;
        return true;
    case modem_mode::RESUME_DATA_MODE:
        if (!dte->set_mode(modem_mode::DATA_MODE)) {
            return false;
        }
        netif.start();
        mode = modem_mode::DATA_MODE;
        return true;
    case modem_mode::RESUME_COMMAND_MODE:
        if (!dte->set_mode(modem_mode::COMMAND_MODE)) {
            return false;
        }
        mode = modem_mode::COMMAND_MODE;
        return true;
    case modem_mode::RESUME_CMUX_MANUAL_MODE:
        if (!dte->set_mode(modem_mode::CMUX_MANUAL_MODE)) {
            return false;
        }
        mode = modem_mode::CMUX_MANUAL_MODE;
        return true;
    case modem_mode::RESUME_CMUX_MANUAL_DATA:
        if (!dte->set_mode(modem_mode::CMUX_MANUAL_SWAP)) {
            return false;
        }
        netif.start();
        mode = modem_mode::CMUX_MANUAL_MODE;
        return true;
    case modem_mode::DATA_MODE:
        if (mode == modem_mode::DATA_MODE || mode == modem_mode::CMUX_MODE || mode >= modem_mode::CMUX_MANUAL_MODE) {
            return false;
        }
        if (!transitions::enter_data(*dte, *device, netif)) {
            return false;
        }
        mode = m;
        return true;
    case modem_mode::CMUX_MODE:
        if (mode == modem_mode::DATA_MODE || mode == modem_mode::CMUX_MODE || mode >= modem_mode::CMUX_MANUAL_MODE) {
            return false;
        }
        device->set_mode(modem_mode::CMUX_MODE);    // switch the device into CMUX mode
        usleep(100'000);                            // some devices need a few ms to switch

        if (!dte->set_mode(modem_mode::CMUX_MODE)) {
            return false;
        }
        mode = modem_mode::CMUX_MODE;
        return transitions::enter_data(*dte, *device, netif);
    case modem_mode::CMUX_MANUAL_MODE:
        if (mode != modem_mode::COMMAND_MODE && mode != modem_mode::UNDEF) {
            return false;
        }
        device->set_mode(modem_mode::CMUX_MODE);
        usleep(100'000);

        if (!dte->set_mode(m)) {
            return false;
        }
        mode = modem_mode::CMUX_MANUAL_MODE;
        return true;
    case modem_mode::CMUX_MANUAL_EXIT:
        if (mode != modem_mode::CMUX_MANUAL_MODE && mode != modem_mode::UNDEF) {
            return false;
        }
        if (!dte->set_mode(m)) {
            return false;
        }
        mode = modem_mode::COMMAND_MODE;
        return true;
    case modem_mode::CMUX_MANUAL_SWAP:
        if (mode != modem_mode::CMUX_MANUAL_MODE && mode != modem_mode::UNDEF) {
            return false;
        }
        if (!dte->set_mode(m)) {
            return false;
        }
        return true;
    case modem_mode::CMUX_MANUAL_DATA:
        if (mode != modem_mode::CMUX_MANUAL_MODE && mode != modem_mode::UNDEF) {
            return false;
        }
        return transitions::enter_data(*dte, *device, netif);
    case modem_mode::CMUX_MANUAL_COMMAND:
        if (mode != modem_mode::CMUX_MANUAL_MODE && mode != modem_mode::UNDEF) {
            return false;
        }
        return transitions::exit_data(*dte, *device, netif);
    }
    return false;
}

modem_mode DCE_Mode::get()
{
    return mode;
}

modem_mode DCE_Mode::guess(DTE *dte, bool with_cmux)
{
    Scoped<DTE> lock(*dte);
    return guess_unsafe(dte, with_cmux);
}

/**
 * This namespace contains probe packets and expected replies on 3 different protocols,
 * the modem device could use (as well as timeouts and mode ids for synchronisation)
 */
namespace probe {

namespace ppp {
// Test that we're in the PPP mode by sending an LCP protocol echo request and expecting LCP echo reply
constexpr std::array<uint8_t, 16> lcp_echo_request = {0x7e, 0xff, 0x03, 0xc0, 0x21, 0x09, 0x01, 0x00, 0x08, 0x99, 0xd1, 0x35, 0xc1, 0x8e, 0x2c, 0x7e };
constexpr std::array<uint8_t, 5> lcp_echo_reply_head = {0x7e, 0xff, 0x7d, 0x23, 0xc0};
const size_t mode = 1 << 0;
const int timeout = 200;
}

namespace cmd {
// For command mode, we just send a simple AT command
const char at[] = "\r\nAT\r\n";
const size_t max_at_reply = 16; // account for some whitespaces and/or CMUX encapsulation
const char reply[] = { 'O', 'K' };
const int mode = 1 << 1;
const int timeout = 500;
}

namespace cmux {
// For CMUX mode, we send an SABM on control terminal (0)
const uint8_t sabm0_reqest[] = {0xf9, 0x03, 0x3f, 0x01, 0x1c, 0xf9};
const uint8_t sabm0_reply[] = {0xf9, 0x03, 0x73, 0x01};
const int mode = 1 << 0;
const int timeout = 200;
}
};

modem_mode DCE_Mode::guess_unsafe(DTE *dte, bool with_cmux)
{
    // placeholder for reply and its size, since it could come in pieces, and we have to cache
    // this is captured by the lambda by reference.
    // must make sure the lambda is cleared before exiting this function (done by dte->on_read(nullptr))
    uint8_t reply[std::max(probe::cmd::max_at_reply, std::max(sizeof(probe::ppp::lcp_echo_request), sizeof(probe::cmux::sabm0_reply)))];
    size_t reply_pos = 0;
    auto signal = std::make_shared<SignalGroup>();
    std::weak_ptr<SignalGroup> weak_signal = signal;
    dte->on_read([weak_signal, with_cmux, &reply, &reply_pos](uint8_t *data, size_t len) {
        // storing the response in the `reply` array and de-fragmenting
        if (reply_pos >= sizeof(reply)) {
            return command_result::TIMEOUT;
        }
        auto reply_size = std::min((size_t)sizeof(reply) - reply_pos, len);
        ::memcpy(reply + reply_pos, data, reply_size);
        reply_pos += reply_size;
        ESP_LOG_BUFFER_HEXDUMP("esp-modem: guess mode data:", reply, reply_pos, ESP_LOG_DEBUG);

        // Check whether the response resembles the "golden" reply (for these 3 protocols)
        if (reply_pos >= sizeof(probe::ppp::lcp_echo_reply_head)) {
            // check for initial 2 bytes
            auto *ptr = static_cast<uint8_t *>(memmem(reply, reply_pos, probe::ppp::lcp_echo_reply_head.data(), 2));
            // and check the other two bytes for protocol ID:
            // * either LCP reply
            if (ptr && ptr[3] == probe::ppp::lcp_echo_reply_head[3] && ptr[4] == probe::ppp::lcp_echo_reply_head[4]) {
                if (auto signal = weak_signal.lock()) {
                    signal->set(probe::ppp::mode);
                }
            }
            // * or LCP conf request
            if (ptr && ptr[3] == probe::ppp::lcp_echo_request[3] && ptr[4] == probe::ppp::lcp_echo_request[4]) {
                if (auto signal = weak_signal.lock()) {
                    signal->set(probe::ppp::mode);
                }
            }
        }
        if (reply_pos >= 4 && memmem(reply, reply_pos, probe::cmd::reply, sizeof(probe::cmd::reply))) {
            if (reply[0] != 0xf9) {   // double check that the reply is not wrapped in CMUX headers
                if (auto signal = weak_signal.lock()) {
                    signal->set(probe::cmd::mode);
                }
            }
        }
        if (with_cmux && reply_pos >= sizeof(probe::cmux::sabm0_reply)) {
            // checking the initial 3 bytes
            auto *ptr = static_cast<uint8_t *>(memmem(reply, reply_pos, probe::cmux::sabm0_reply, 3));
            // and checking that DLCI is 0 (control frame)
            if (ptr && (ptr[3] >> 2) == 0) {
                if (auto signal = weak_signal.lock()) {
                    signal->set(probe::cmux::mode);
                }
            }
        }
        return command_result::TIMEOUT;
    });
    auto guessed = modem_mode::UNDEF;
    // Check the PPP mode fist by sending LCP echo request
    dte->send((uint8_t *)probe::ppp::lcp_echo_request.data(), sizeof(probe::ppp::lcp_echo_request), 0);
    if (signal->wait(probe::ppp::mode, probe::ppp::timeout)) {
        guessed = modem_mode::DATA_MODE;
    } else {    // LCP echo timeout
        // now check for AT mode
        reply_pos = 0;
        dte->send((uint8_t *)probe::cmd::at, sizeof(probe::cmd::at), 0);
        if (signal->wait(probe::cmd::mode, probe::cmd::timeout)) {
            guessed = modem_mode::COMMAND_MODE;
        } else if (with_cmux) {     // no AT reply, check for CMUX mode (if requested)
            reply_pos = 0;
            dte->send((uint8_t *) probe::cmux::sabm0_reqest, sizeof(probe::cmux::sabm0_reqest), 0);
            if (signal->wait(probe::cmux::mode, probe::cmux::timeout)) {
                guessed = modem_mode::CMUX_MODE;
            }
        }
    }
    dte->on_read(nullptr);
    return guessed;
}

} // esp_modem
