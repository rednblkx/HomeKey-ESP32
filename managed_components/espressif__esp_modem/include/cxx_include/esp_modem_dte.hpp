/*
 * SPDX-FileCopyrightText: 2021-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <memory>
#include <utility>
#include <cstddef>
#include <cstdint>
#include "cxx_include/esp_modem_primitives.hpp"
#include "cxx_include/esp_modem_terminal.hpp"
#include "cxx_include/esp_modem_types.hpp"
#include "cxx_include/esp_modem_buffer.hpp"

struct esp_modem_dte_config;

namespace esp_modem {

class CMux;

/**
 * @defgroup ESP_MODEM_DTE
 * @brief Definition of DTE and related classes
 */
/** @addtogroup ESP_MODEM_DTE
* @{
*/

struct DTE_Command {
    DTE_Command(const std::string &cmd): data((uint8_t *)cmd.c_str()), len(cmd.length()) {}

    uint8_t *data;
    size_t len;
};

/**
 * DTE (Data Terminal Equipment) class
 */
class DTE : public CommandableIf {
public:

    /**
     * @brief Creates a DTE instance from the terminal
     * @param config DTE config structure
     * @param t unique-ptr to Terminal
     * @param s unique-ptr to secondary Terminal
     */
    explicit DTE(const esp_modem_dte_config *config, std::unique_ptr<Terminal> t);
    explicit DTE(std::unique_ptr<Terminal> t);
    explicit DTE(const esp_modem_dte_config *config, std::unique_ptr<Terminal> t, std::unique_ptr<Terminal> s);
    explicit DTE(std::unique_ptr<Terminal> t, std::unique_ptr<Terminal> s);

    ~DTE() = default;

    /**
     * @brief Writing to the underlying terminal
     * @param data Data pointer to write
     * @param len Data len to write
     * @return number of bytes written
     */
    int write(uint8_t *data, size_t len) override;

    int write(DTE_Command command);

    /**
     * @brief send data to the selected terminal, by default (without term_id argument)
     * this API works the same as write: sends data to the secondary terminal, which is
     * typically used as data terminal (for networking).
     *
     * @param data Data pointer to write
     * @param len Data len to write
     * @param term_id Terminal id: Primary if id==0, Secondary if id==1
     * @return number of bytes written
     */
    int send(uint8_t *data, size_t len, int term_id = 1);

    /**
     * @brief Reading from the underlying terminal
     * @param d Returning the data pointer of the received payload
     * @param len Length of the data payload
     * @return number of bytes read
     */
    int read(uint8_t **d, size_t len);

    /**
     * @brief Sets read callback with valid data and length
     * @param f Function to be called on data available
     */
    void set_read_cb(std::function<bool(uint8_t *data, size_t len)> f);

    /**
     * @brief Sets read callback for manual command processing
     * Note that this API also locks the command API, which can only be used
     * after you remove the callback by dte->on_read(nullptr)
     *
     * @param on_data Function to be called when a command response is available
     */
    void on_read(got_line_cb on_data) override;

    /**
     * @brief Sets DTE error callback
     * @param f Function to be called on DTE error
     */
    void set_error_cb(std::function<void(terminal_error err)> f);

#ifdef CONFIG_ESP_MODEM_URC_HANDLER
    /**
     * @brief Allow setting a line callback for all incoming data
     * @param line_cb
     */
    void set_urc_cb(got_line_cb line_cb)
    {
        command_cb.urc_handler = std::move(line_cb);
    }
#endif

    /**
     * @brief Sets the DTE to desired mode (Command/Data/Cmux)
     * @param m Desired operation mode
     * @return true on success
     */
    [[nodiscard]] bool set_mode(modem_mode m);

    /**
     * @brief Sends command and provides callback with responding line
     * @param command String parameter representing command
     * @param got_line Function to be called after line available as a response
     * @param time_ms Time in ms to wait for the answer
     * @return OK, FAIL, TIMEOUT
     */
    command_result command(const std::string &command, got_line_cb got_line, uint32_t time_ms) override;

    /**
     * @brief Sends the command (same as above) but with a specific separator
     */
    command_result command(const std::string &command, got_line_cb got_line, uint32_t time_ms, char separator) override;

    /**
     * @brief Allows this DTE to recover from a generic connection issue
     *
     * @return true if success
     */
    bool recover();

    /**
     * @brief Set internal command callbacks to the underlying terminal.
     * Here we capture command replies to be processed by supplied command callbacks in  struct command_cb.
     */
    void set_command_callbacks();

protected:
    /**
     * @brief Allows for locking the DTE
     */
    void lock()
    {
        internal_lock.lock();
    }
    void unlock()
    {
        internal_lock.unlock();
    }
    friend class Scoped<DTE>;                               /*!< Declaring "Scoped<DTE> lock(dte)" locks this instance */
private:

    void handle_error(terminal_error err);                  /*!< Performs internal error handling */
    [[nodiscard]] bool setup_cmux();                        /*!< Internal setup of CMUX mode */
    [[nodiscard]] bool exit_cmux();                         /*!< Exit of CMUX mode and cleanup  */
    void exit_cmux_internal();                              /*!< Cleanup CMUX */

    Lock internal_lock{};                                   /*!< Locks DTE operations */
    unique_buffer buffer;                                   /*!< DTE buffer */
    std::shared_ptr<CMux> cmux_term;                        /*!< Primary terminal for this DTE */
    std::shared_ptr<Terminal> primary_term;                 /*!< Reference to the primary terminal (mostly for sending commands) */
    std::shared_ptr<Terminal> secondary_term;               /*!< Secondary terminal for this DTE */
    modem_mode mode;                                        /*!< DTE operation mode */
    std::function<bool(uint8_t *data, size_t len)> on_data; /*!< on data callback for current terminal */
    std::function<void(terminal_error err)> user_error_cb;  /*!< user callback on error event from attached terminals */

#ifdef CONFIG_ESP_MODEM_USE_INFLATABLE_BUFFER_IF_NEEDED
    /**
     * @brief Implements an extra buffer that is used to capture partial reads from underlying terminals
     * when we run out of the standard buffer
     */
    struct extra_buffer {
        extra_buffer(): buffer(nullptr) {}
        ~extra_buffer()
        {
            delete buffer;
        }
        std::vector<uint8_t> *buffer;
        size_t consumed{0};
        void grow(size_t need_size);
        void deflate()
        {
            grow(0);
            consumed = 0;
        }
        [[nodiscard]] uint8_t *begin() const
        {
            return &buffer->at(0);
        }
        [[nodiscard]] uint8_t *current() const
        {
            return &buffer->at(0) + consumed;
        }
    } inflatable;
#endif // CONFIG_ESP_MODEM_USE_INFLATABLE_BUFFER_IF_NEEDED

    /**
     * @brief This abstracts command callback processing and implements its locking, signaling of completion and timeouts.
     */
    struct command_cb {
#ifdef CONFIG_ESP_MODEM_URC_HANDLER
        got_line_cb urc_handler {};                             /*!< URC callback if enabled */
#endif
        static const size_t GOT_LINE = SignalGroup::bit0;       /*!< Bit indicating response available */
        got_line_cb got_line;                                   /*!< Supplied command callback */
        Lock line_lock{};                                       /*!< Command callback locking mechanism */
        char separator{};                                       /*!< Command reply separator (end of line/processing unit) */
        command_result result{};                                /*!< Command return code */
        SignalGroup signal;                                     /*!< Event group used to signal request-response operations */
        bool process_line(uint8_t *data, size_t consumed, size_t len);  /*!< Lets the processing callback handle one line (processing unit) */
        bool wait_for_line(uint32_t time_ms)                    /*!< Waiting for command processing */
        {
            return signal.wait_any(command_cb::GOT_LINE, time_ms);
        }
        void set(got_line_cb l, char s = '\n')                  /*!< Sets the command callback atomically */
        {
            Scoped<Lock> lock(line_lock);
            if (l) {
                // if we set the line callback, we have to reset the signal and the result
                signal.clear(GOT_LINE);
                result = command_result::TIMEOUT;
            } else {
                // if we clear the line callback, we check consistency (since we've locked the line processing)
                if (signal.is_any(command_cb::GOT_LINE) && result == command_result::TIMEOUT) {
                    ESP_MODEM_THROW_IF_ERROR(ESP_ERR_INVALID_STATE);
                }
            }
            got_line = std::move(l);
            separator = s;
        }
        void give_up()                                          /*!< Reports other than timeout error when processing replies (out of buffer) */
        {
            result = command_result::FAIL;
            signal.set(GOT_LINE);
        }
    } command_cb;                                               /*!< Command callback utility class */
};

/**
 * @}
 */

} // namespace esp_modem
