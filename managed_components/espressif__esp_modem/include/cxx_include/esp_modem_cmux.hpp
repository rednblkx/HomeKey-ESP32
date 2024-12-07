/*
 * SPDX-FileCopyrightText: 2021-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "esp_modem_terminal.hpp"
#include "cxx_include/esp_modem_buffer.hpp"

namespace esp_modem {

constexpr size_t MAX_TERMINALS_NUM = 2;
/**
 * @defgroup ESP_MODEM_CMUX ESP_MODEM CMUX class
 * @brief Definition of CMUX terminal
 */
/** @addtogroup ESP_MODEM_CMUX
* @{
*/

/**
 * @brief CMUX state machine
 */
enum class cmux_state {
    INIT,
    HEADER,
    PAYLOAD,
    FOOTER,
    RECOVER,
};

/**
 * @brief CMUX terminal abstraction
 *
 * This class inherits from Terminal class, as it is a Terminal, but is also composed of another Terminal,
 * which is used to communicate with the modem, i.e. the original Terminal which has been multiplexed.
 *
 * @note Implementation of CMUX protocol is experimental
 */
class CMuxInstance;

/**
 * @brief CMux class which consumes the original terminal and creates multiple virtual terminals from it.
 * This class itself is not usable as a DTE terminal, only via its instances defined in `CMuxInstance`
 */
class CMux {
public:
    explicit CMux(std::shared_ptr<Terminal> t, unique_buffer &&b):
        term(std::move(t)), payload_start(nullptr), total_payload_size(0), buffer(std::move(b))  {}
    ~CMux() = default;

    /**
     * @brief Initializes CMux protocol
     * @return true on success
     */
    [[nodiscard]] bool init();

    /**
     * @brief Closes and deinits CMux protocol
     * @return true on success
     */
    [[nodiscard]] bool deinit();

    /**
     * @brief   Ejects the attached terminal and buffer,
     * so they could be used as traditional command/data DTE's
     * @return  pair of the original terminal and buffer
     */
    std::pair<std::shared_ptr<Terminal>, unique_buffer> detach();

    /**
     * @brief Sets read callback for the appropriate terminal
     * @param inst Index of the terminal
     * @param f function pointer
     */
    void set_read_cb(int inst, std::function<bool(uint8_t *data, size_t len)> f);

    /**
     * @brief Writes to the appropriate terminal
     * @param i Index of the terminal
     * @param data Data to write
     * @param len Data length to write
     * @return The actual written length
     */
    int write(int i, uint8_t *data, size_t len);

    /**
     * @brief Recovers the protocol
     *
     * This restarts the CMUX state machine, which could have been in a wrong state due to communication
     * issue on a lower layer.
     *
     * @return true on success
     */
    bool recover();

private:

    enum class protocol_mismatch_reason {
        MISSED_LEAD_SOF,
        MISSED_TRAIL_SOF,
        WRONG_CRC,
        UNEXPECTED_HEADER,
        UNEXPECTED_DATA,
        READ_BEHIND_BUFFER,
        UNKNOWN
    };

    static uint8_t fcs_crc(const uint8_t frame[6]);     /*!< Utility to calculate FCS CRC */
    bool data_available(uint8_t *data, size_t len);     /*!< Called when valid data available (returns false on unexpected data format) */
    void send_sabm(size_t i);                           /*!< Sending initial SABM */
    void send_disconnect(size_t i);                     /*!< Sending closing request for each virtual or control terminal */
    bool on_cmux_data(uint8_t *data, size_t len);       /*!< Called from terminal layer when raw CMUX protocol data available */

    struct CMuxFrame;                                   /*!< Forward declare the Frame struct, used in protocol decoders */
    /**
     * These methods serve different states of the CMUX protocols
     * @param frame Currently available cmux frame (basically data, size, methods)
     * @return - true if the state processed successfully
     *         - false if more data needed to process the current state
     */
    bool on_recovery(CMuxFrame &frame);
    bool on_init(CMuxFrame &frame);
    bool on_header(CMuxFrame &frame);
    bool on_payload(CMuxFrame &frame);
    bool on_footer(CMuxFrame &frame);
    void recover_protocol(protocol_mismatch_reason reason);

    std::function<bool(uint8_t *data, size_t len)> read_cb[MAX_TERMINALS_NUM];  /*!< Function pointers to read callbacks */
    std::shared_ptr<Terminal> term;                   /*!< The original terminal */
    cmux_state state;                                 /*!< CMux protocol state */

    /**
     * CMux control fields and offsets
     */
    uint8_t dlci;
    uint8_t type;
    size_t payload_len;
    uint8_t frame_header[6];
    size_t frame_header_offset;
    uint8_t *payload_start;
    size_t total_payload_size;
    int sabm_ack;

    /**
     * Processing unique buffer (reused and transferred from it's parent DTE)
     */
    unique_buffer buffer;

    Lock lock;
};

/**
 * @brief This represents a specific instance of a CMUX virtual terminal. This class also implements Terminal interface
 * and as such could be used as a DTE's terminal.
 */
class CMuxInstance: public Terminal {
public:
    explicit CMuxInstance(std::shared_ptr<CMux> parent, int i): cmux(std::move(parent)), instance(i) {}

    int write(uint8_t *data, size_t len) override
    {
        return cmux->write(instance, data, len);
    }
    void set_read_cb(std::function<bool(uint8_t *data, size_t len)> f) override
    {
        return cmux->set_read_cb(instance, std::move(f));
    }
    int read(uint8_t *data, size_t len) override
    {
        return  0;
    }
    void start() override { }
    void stop() override { }
private:
    std::shared_ptr<CMux> cmux;
    size_t instance;
};

/**
 * @}
 */

} // namespace esp_modem
