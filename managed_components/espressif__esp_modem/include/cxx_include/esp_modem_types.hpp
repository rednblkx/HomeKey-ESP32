/*
 * SPDX-FileCopyrightText: 2021-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <functional>
#include <string>
#include <cstddef>
#include <cstdint>

namespace esp_modem {

/**
 * @defgroup ESP_MODEM_TYPES
 * @brief Basic type definitions used in esp-modem
 */

/** @addtogroup ESP_MODEM_TYPES
* @{
*/

/**
 * @brief Modem working mode
 */
enum class modem_mode {
    UNDEF,
    COMMAND_MODE, /*!< Command mode -- the modem is supposed to send AT commands in this mode */
    DATA_MODE,    /*!< Data mode -- the modem communicates with network interface on PPP protocol */
    DUAL_MODE,    /*!< Dual mode -- the modem has two real terminals. Data and commands work at the same time */
    CMUX_MODE,    /*!< CMUX (Multiplex mode) -- Simplified CMUX mode, which creates two virtual terminals,
                   *  assigning one solely to command interface and the other  to the data mode */
    CMUX_MANUAL_MODE,    /*!< Enter CMUX mode manually -- just creates two virtual terminals */
    CMUX_MANUAL_EXIT,    /*!< Exits CMUX mode manually -- just destroys two virtual terminals */
    CMUX_MANUAL_DATA,    /*!< Sets the primary terminal to DATA mode in manual CMUX */
    CMUX_MANUAL_COMMAND, /*!< Sets the primary terminal to COMMAND mode in manual CMUX */
    CMUX_MANUAL_SWAP,    /*!< Swaps virtual terminals in manual CMUX mode (primary <-> secondary) */
    RESUME_DATA_MODE,    /*!< This is used when the device is already in DATA mode and we need the modem lib to
                          * enter the mode without switching. On success, we would end up in DATA-mode, UNDEF otherwise */
    RESUME_COMMAND_MODE, /*!< This is used when the device is already in COMMAND mode and we want to resume it
                          * On success, we would end up in DATA-mode, UNDEF otherwise */
    RESUME_CMUX_MANUAL_MODE, /*!< This is used when the device is already in CMUX mode and we need the modem lib to
                              * enter it without switching. On success, we would end up in CMUX_MANUAL-mode, UNDEF otherwise */
    RESUME_CMUX_MANUAL_DATA, /*!< This is used when the device is already in CMUX-DATA mode and we need the modem lib to
                              * enter it without switching. On success, we would end up in CMUX_MANUAL-DATA mode, UNDEF otherwise */
    AUTODETECT,              /*!< Auto-detection command: It tries to send a few packets in order to recognize which mode the
                              * the device currently is and update the modem library mode. On success the modem is updated,
                              * otherwise it's set to UNDEF */
};

/**
 * @brief Module command result
 */
enum class command_result {
    OK,             /*!< The command completed successfully */
    FAIL,           /*!< The command explicitly failed */
    TIMEOUT         /*!< The device didn't respond in the specified timeline */
};

typedef std::function<command_result(uint8_t *data, size_t len)> got_line_cb;

/**
 * @brief PDP context used for configuring and setting the data mode up
 */
struct PdpContext {
    explicit PdpContext(std::string apn) : apn(std::move(apn)) {}
    size_t context_id = 1;
    std::string protocol_type = "IP";
    std::string apn;
};

/**
 * @brief Interface for classes eligible to send AT commands (Modules, DCEs, DTEs)
 */
class CommandableIf {
public:
    CommandableIf() = default;
    CommandableIf(const CommandableIf &) = delete;
    CommandableIf &operator=(const CommandableIf &) = delete;
    CommandableIf(CommandableIf &&) = delete;
    CommandableIf &operator=(CommandableIf &&) = delete;
    virtual ~CommandableIf() = default;
    /**
     * @brief Sends custom AT command
     * @param command Command to be sent
     * @param got_line callback if a line received
     * @param time_ms timeout in milliseconds
     * @param separator Character treated as a line separator, typically '\n'
     * @return OK, FAIL or TIMEOUT
     */
    virtual command_result command(const std::string &command, got_line_cb got_line, uint32_t time_ms, const char separator) = 0;
    virtual command_result command(const std::string &command, got_line_cb got_line, uint32_t time_ms) = 0;

    virtual int write(uint8_t *data, size_t len) = 0;
    virtual void on_read(got_line_cb on_data) = 0;
};

/**
 * @brief Interface for classes implementing a module for the modem
 */
class ModuleIf {
public:
    ModuleIf() = default;
    ModuleIf(const ModuleIf &) = delete;
    ModuleIf &operator=(const ModuleIf &) = delete;
    ModuleIf(ModuleIf &&) = delete;
    ModuleIf &operator=(ModuleIf &&) = delete;
    virtual ~ModuleIf() = default;
    /**
     * @brief Sets the data mode up (provides the necessary configuration to connect to the cellular network)
     * @return true on success
     */
    virtual bool setup_data_mode() = 0;

    /**
     * @brief Sets the operation mode
     * @param mode Desired mode
     * @return true on success
     */
    virtual bool set_mode(modem_mode mode) = 0;
};

/**
 * @}
 */

} // namespace esp_modem
