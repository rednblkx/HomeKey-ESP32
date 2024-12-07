/*
 * SPDX-FileCopyrightText: 2021-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <cstring>
#include <unistd.h>
#include <cxx_include/esp_modem_cmux.hpp>
#include "cxx_include/esp_modem_dte.hpp"
#include "esp_log.h"
#include "sdkconfig.h"

using namespace esp_modem;

#ifdef CONFIG_IDF_TARGET_LINUX
#define PRIsize_t "lu"
#else
#define PRIsize_t "u"
#endif

#ifdef CONFIG_ESP_MODEM_CMUX_DEFRAGMENT_PAYLOAD
/**
 * @brief Define this to defragment partially received data of CMUX payload
 *        This is useful if upper layers expect the entire payload available
 *        for parsing.
 */
#define DEFRAGMENT_CMUX_PAYLOAD
#endif

#define EA 0x01  /* Extension bit      */
#define CR 0x02  /* Command / Response */
#define PF 0x10  /* Poll / Final       */

/* Frame types */
#define FT_RR      0x01  /* Receive Ready                            */
#define FT_UI      0x03  /* Unnumbered Information                   */
#define FT_RNR     0x05  /* Receive Not Ready                        */
#define FT_REJ     0x09  /* Reject                                   */
#define FT_DM      0x0F  /* Disconnected Mode                        */
#define FT_SABM    0x2F  /* Set Asynchronous Balanced Mode           */
#define FT_DISC    0x43  /* Disconnect                               */
#define FT_UA      0x63  /* Unnumbered Acknowledgement               */
#define FT_UIH     0xEF  /* Unnumbered Information with Header check */

/* Control channel commands */
#define CMD_NSC    0x08  /* Non Supported Command Response           */
#define CMD_TEST   0x10  /* Test Command                             */
#define CMD_PSC    0x20  /* Power Saving Control                     */
#define CMD_RLS    0x28  /* Remote Line Status Command               */
#define CMD_FCOFF  0x30  /* Flow Control Off Command                 */
#define CMD_PN     0x40  /* DLC parameter negotiation                */
#define CMD_RPN    0x48  /* Remote Port Negotiation Command          */
#define CMD_FCON   0x50  /* Flow Control On Command                  */
#define CMD_CLD    0x60  /* Multiplexer close down                   */
#define CMD_SNC    0x68  /* Service Negotiation Command              */
#define CMD_MSC    0x70  /* Modem Status Command                     */

/* Flag sequence field between messages (start of frame) */
#define SOF_MARKER 0xF9

uint8_t CMux::fcs_crc(const uint8_t frame[6])
{
    //    #define FCS_GOOD_VALUE 0xCF
    uint8_t crc = 0xFF; // FCS_INIT_VALUE

    for (int i = 1; i < 4; i++) {
        crc ^= frame[i];

        for (int j = 0; j < 8; j++) {
            if (crc & 0x01) {
                crc = (crc >> 1) ^ 0xe0; // FCS_POLYNOMIAL
            } else {
                crc >>= 1;
            }
        }
    }

    return crc;
}

void CMux::send_disconnect(size_t i)
{
    if (i == 0) {   // control terminal
        uint8_t frame[] = {
            SOF_MARKER, 0x3, 0xEF, 0x5, 0xC3, 0x1, 0xF2, SOF_MARKER
        };
        term->write(frame, 8);
    } else {        // separate virtual terminal
        uint8_t frame[] = {
            SOF_MARKER, 0x3, FT_DISC | PF, 0x1, 0, SOF_MARKER
        };
        frame[1] |= i << 2;
        frame[4] = 0xFF - fcs_crc(frame);
        term->write(frame, sizeof(frame));
    }
}

void CMux::send_sabm(size_t i)
{
    uint8_t frame[6];
    frame[0] = SOF_MARKER;
    frame[1] = (i << 2) | 0x3;
    frame[2] = FT_SABM | PF;
    frame[3] = 1;
    frame[4] = 0xFF - fcs_crc(frame);
    frame[5] = SOF_MARKER;
    term->write(frame, 6);
}


struct CMux::CMuxFrame {
    uint8_t *ptr;     /*!< pointer to the currently processing byte of the CMUX frame */
    size_t len;       /*!< length of available data in the current CMUX frame */
    void advance(int size = 1)
    {
        ptr += size;
        len -= size;
    }
};

bool CMux::data_available(uint8_t *data, size_t len)
{
    if (data && (type & FT_UIH) == FT_UIH && len > 0 && dlci > 0) { // valid payload on a virtual term
        int virtual_term = dlci - 1;
        if (virtual_term < MAX_TERMINALS_NUM) {
            if (read_cb[virtual_term] == nullptr) {
                // ignore all virtual terminal's data before we completely establish CMUX
                ESP_LOG_BUFFER_HEXDUMP("CMUX Rx before init", data, len, ESP_LOG_DEBUG);
                return true;
            }
            // Post partial data (or defragment to post on CMUX footer)
#ifdef DEFRAGMENT_CMUX_PAYLOAD
            if (payload_start == nullptr) {
                payload_start = data;
                total_payload_size = 0;
            }
            total_payload_size += len;
#else
            read_cb[virtual_term](data, len);
#endif
        } else {
            return false;
        }
    } else if (data == nullptr && type == (FT_UA | PF) && len == 0) { // notify the initial SABM command
        Scoped<Lock> l(lock);
        sabm_ack = dlci;
    } else if (data == nullptr && dlci > 0) {
        int virtual_term = dlci - 1;
        if (virtual_term < MAX_TERMINALS_NUM) {
            if (read_cb[virtual_term] == nullptr) {
                // silently ignore this CMUX frame (not finished entering CMUX, yet)
                return true;
            }
#ifdef DEFRAGMENT_CMUX_PAYLOAD
            read_cb[virtual_term](payload_start, total_payload_size);
#endif
        } else {
            return false;
        }
    } else if ((type & FT_UIH) == FT_UIH && dlci == 0) { // notify the internal DISC command
        if ((data == nullptr) || (len > 0 && (data[0] & 0xE1) == 0xE1)) {
            // Not a DISC, ignore (MSC frame)
            return true;
        }
        Scoped<Lock> l(lock);
        sabm_ack = dlci;
    } else {
        return false;
    }
    return true;
}

bool CMux::on_init(CMuxFrame &frame)
{
    if (frame.ptr[0] != SOF_MARKER) {
        recover_protocol(protocol_mismatch_reason::MISSED_LEAD_SOF);
        return true;
    }
    if (frame.len > 1 && frame.ptr[1] == SOF_MARKER) {
        // empty frame
        frame.advance();
        return true;
    }
    state = cmux_state::HEADER;
    frame.advance();
    frame_header_offset = 1;
    return true;
}

bool CMux::on_recovery(CMuxFrame &frame)
{
    uint8_t *recover_ptr;
    if (frame.ptr[0] == SOF_MARKER) {
        // already init state
        state = cmux_state::INIT;
        return true;
    }
    recover_ptr = static_cast<uint8_t *>(memchr(frame.ptr, SOF_MARKER, frame.len));
    if (recover_ptr && frame.len > recover_ptr - frame.ptr) {
        frame.len -= (recover_ptr - frame.ptr);
        frame.ptr = recover_ptr;
        state = cmux_state::INIT;
        ESP_LOGI("CMUX", "Protocol recovered");
        if (frame.len > 1 && frame.ptr[1] == SOF_MARKER) {
            // empty frame
            frame.advance();
        }
        return true;
    }
    // marker not found, continue with recovery
    return false;
}


bool CMux::on_header(CMuxFrame &frame)
{
    if (frame.len > 0 && frame_header_offset == 1 && frame.ptr[0] == SOF_MARKER) {
        // Previously trailing SOF interpreted as heading SOF, remove it and restart HEADER
        frame.advance();
        return true;
    }
    if (frame.len + frame_header_offset < 4) {
        memcpy(frame_header + frame_header_offset, frame.ptr, frame.len);
        frame_header_offset += frame.len;
        return false; // need read more
    }
    size_t payload_offset = std::min(frame.len, 4 - frame_header_offset);
    memcpy(frame_header + frame_header_offset, frame.ptr, payload_offset);
#ifndef ESP_MODEM_CMUX_USE_SHORT_PAYLOADS_ONLY
    if ((frame_header[3] & 1) == 0) {
        if (frame_header_offset + frame.len <= 4) {
            frame_header_offset += frame.len;
            return false; // need read more
        }
        payload_offset = std::min(frame.len, 5 - frame_header_offset);
        memcpy(frame_header + frame_header_offset, frame.ptr, payload_offset);
        payload_len = frame_header[4] << 7;
        frame_header_offset += payload_offset - 1; // rewind frame_header back to hold only 6 bytes size
    } else
#endif // ! ESP_MODEM_CMUX_USE_SHORT_PAYLOADS_ONLY
    {
        payload_len = 0;
        frame_header_offset += payload_offset;
    }
    dlci = frame_header[1] >> 2;
    type = frame_header[2];
    // Sanity check for expected values of DLCI and type,
    // since CRC could be evaluated after the frame payload gets received
    if (dlci > MAX_TERMINALS_NUM || (frame_header[1] & 0x01) == 0 ||
            (((type & FT_UIH) != FT_UIH) &&  type != (FT_UA | PF) ) ) {
        recover_protocol(protocol_mismatch_reason::UNEXPECTED_HEADER);
        return true;
    }
    payload_len += (frame_header[3] >> 1);
    frame.advance(payload_offset);
    state = cmux_state::PAYLOAD;
    return true;
}

bool CMux::on_payload(CMuxFrame &frame)
{
    ESP_LOGD("CMUX", "Payload frame: dlci:%02x type:%02x payload:%" PRIsize_t " available:%" PRIsize_t, dlci, type, payload_len, frame.len);
    if (frame.len < payload_len) { // payload
        state = cmux_state::PAYLOAD;
        if (!data_available(frame.ptr, frame.len)) { // partial read
            recover_protocol(protocol_mismatch_reason::UNEXPECTED_DATA);
            return true;
        }
        payload_len -= frame.len;
        return false;
    } else { // complete
        if (payload_len > 0) {
            if (!data_available(&frame.ptr[0], payload_len)) { // rest read
                recover_protocol(protocol_mismatch_reason::UNEXPECTED_DATA);
                return true;
            }
        }
        frame.advance((payload_len));
        state = cmux_state::FOOTER;
        payload_len = 0;
    }
    return true;
}

bool CMux::on_footer(CMuxFrame &frame)
{
    size_t footer_offset = 0;
    if (frame.len + frame_header_offset < 6) {
        memcpy(frame_header + frame_header_offset, frame.ptr, frame.len);
        frame_header_offset += frame.len;
        return false; // need read more
    } else {
        footer_offset = std::min(frame.len, 6 - frame_header_offset);
        memcpy(frame_header + frame_header_offset, frame.ptr, footer_offset);
        if (frame_header[5] != SOF_MARKER) {
            recover_protocol(protocol_mismatch_reason::MISSED_TRAIL_SOF);
            return true;
        }
#ifdef ESP_MODEM_CMUX_USE_SHORT_PAYLOADS_ONLY
        uint8_t crc = 0xFF - fcs_crc(frame_header);
        if (crc != frame_header[4]) {
            recover_protocol(protocol_mismatch_reason::WRONG_CRC);
            return true;
        }
#endif
        frame.advance(footer_offset);
        state = cmux_state::INIT;
        frame_header_offset = 0;
        if (!data_available(nullptr, 0)) {
            recover_protocol(protocol_mismatch_reason::UNEXPECTED_DATA);
            return true;
        }
        payload_start = nullptr;
        total_payload_size = 0;
    }
    return true;
}

bool CMux::on_cmux_data(uint8_t *data, size_t actual_len)
{
    if (!data) {
#ifdef DEFRAGMENT_CMUX_PAYLOAD
        auto data_to_read = buffer.size - 128; // keep 128 (max CMUX payload) backup buffer)
        if (payload_start) {
            data = payload_start + total_payload_size;
            auto data_end = buffer.get() + buffer.size;
            data_to_read = payload_len + 2; // 2 -- CMUX protocol footer
            if (data + data_to_read >= data_end) {
                ESP_LOGW("CUMX", "Failed to defragment longer payload (payload=%" PRIsize_t ")", payload_len);
                // If you experience this error, your device uses longer payloads while
                // the configured buffer is too small to defragment the payload properly.
                // To resolve this issue you can:
                // * Either increase `dte_buffer_size`
                // * Or disable `ESP_MODEM_CMUX_DEFRAGMENT_PAYLOAD` in menuconfig

                // Attempts to process the data accumulated so far (rely on upper layers to process correctly)
                data_available(nullptr, 0);
                if (payload_len > total_payload_size) {
                    payload_start = nullptr;
                    total_payload_size = 0;
                } else {
                    // cannot continue with this payload, give-up and recover the protocol
                    recover_protocol(protocol_mismatch_reason::READ_BEHIND_BUFFER);
                }
                data_to_read = buffer.size;
                data = buffer.get();
            }
        } else {
            data = buffer.get();
        }
        actual_len = term->read(data, data_to_read);
#else
        data = buffer.get();
        actual_len = term->read(data, buffer.size);
#endif
    }
    if (data == nullptr) {
        return false;
    }
    ESP_LOG_BUFFER_HEXDUMP("CMUX Received", data, actual_len, ESP_LOG_VERBOSE);
    CMuxFrame frame = { .ptr = data, .len = actual_len };
    while (frame.len > 0) {
        switch (state) {
        case cmux_state::RECOVER:
            if (!on_recovery(frame)) {
                return false;
            }
            break;
        case cmux_state::INIT:
            if (!on_init(frame)) {
                return false;
            }
            break;
        case cmux_state::HEADER:
            if (!on_header(frame)) {
                return false;
            }
            break;
        case cmux_state::PAYLOAD:
            if (!on_payload(frame)) {
                return false;
            }
            break;
        case cmux_state::FOOTER:
            if (!on_footer(frame)) {
                return false;
            }
            break;
        }
    }
    return true;
}

bool CMux::deinit()
{
    int timeout;
    sabm_ack = -1;
    // First disconnect all (2) virtual terminals
    for (size_t i = 1; i < 3; i++) {
        send_disconnect(i);
        timeout = 0;
        while (true) {
            usleep(10'000);
            Scoped<Lock> l(lock);
            if (sabm_ack == i) {
                sabm_ack = -1;
                break;
            }
            if (timeout++ > 100) {
                return false;
            }
        }
    }
    sabm_ack = -1;
    // Then disconnect the control terminal
    send_disconnect(0);
    timeout = 0;
    while (true) {
        usleep(10'000);
        Scoped<Lock> l(lock);
        if (sabm_ack == 0) {
            break;
        }
        if (timeout++ > 100) {
            return false;
        }
    }
    term->set_read_cb(nullptr);
    return true;
}

bool CMux::init()
{
    frame_header_offset = 0;
    state = cmux_state::INIT;
    term->set_read_cb([this](uint8_t *data, size_t len) {
        this->on_cmux_data(data, len);
        return false;
    });

    sabm_ack = -1;
    for (size_t i = 0; i < 3; i++) {
        int timeout = 0;
        send_sabm(i);
        while (true) {
            usleep(10'000);
            Scoped<Lock> l(lock);
            if (sabm_ack == i) {
                sabm_ack = -1;
                break;
            }
            if (timeout++ > 100) {
                return false;
            }
        }
        if (i > 1) {    // wait for each virtual terminal to settle MSC (no need for control term, DLCI=0)
            usleep(CONFIG_ESP_MODEM_CMUX_DELAY_AFTER_DLCI_SETUP * 1'000);
        }
    }
    return true;
}

int CMux::write(int virtual_term, uint8_t *data, size_t len)
{
    const size_t cmux_max_len = 127;
    Scoped<Lock> l(lock);
    int i = virtual_term + 1;
    size_t need_write = len;
    while (need_write > 0) {
        size_t batch_len = need_write;
        if (batch_len > cmux_max_len) {
            batch_len = cmux_max_len;
        }
        uint8_t frame[6];
        frame[0] = SOF_MARKER;
        frame[1] = (i << 2) + 1;
        frame[2] = FT_UIH;
        frame[3] = (batch_len << 1) + 1;
        frame[4] = 0xFF - fcs_crc(frame);
        frame[5] = SOF_MARKER;

        term->write(frame, 4);
        term->write(data, batch_len);
        term->write(frame + 4, 2);
        ESP_LOG_BUFFER_HEXDUMP("Send", frame, 4, ESP_LOG_VERBOSE);
        ESP_LOG_BUFFER_HEXDUMP("Send", data, batch_len, ESP_LOG_VERBOSE);
        ESP_LOG_BUFFER_HEXDUMP("Send", frame + 4, 2, ESP_LOG_VERBOSE);
        need_write -= batch_len;
        data += batch_len;
    }
    return len;
}

void CMux::set_read_cb(int inst, std::function<bool(uint8_t *, size_t)> f)
{
    if (inst < MAX_TERMINALS_NUM) {
        read_cb[inst] = std::move(f);
    }
}

std::pair<std::shared_ptr<Terminal>, unique_buffer> CMux::detach()
{
    return std::make_pair(std::move(term), std::move(buffer));
}

void esp_modem::CMux::recover_protocol(protocol_mismatch_reason reason)
{
    ESP_LOGW("CMUX", "Restarting CMUX state machine (reason: %d)", static_cast<int>(reason));
    payload_start = nullptr;
    total_payload_size = 0;
    frame_header_offset = 0;
    state = cmux_state::RECOVER;
}

bool CMux::recover()
{
    Scoped<Lock> l(lock);
    recover_protocol(protocol_mismatch_reason::UNKNOWN);
    return true;
}
