/*
 * SPDX-FileCopyrightText: 2021-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

namespace esp_modem::dce_commands {

/**
 * @brief Generic command that passes on supplied pass_phrase, and fails on fail_phrase
 * @param t Any "Command-able" class that implements "command()" method
 * @param command Command to issue
 * @param pass_phrase Pattern to find in replies to complete the command successfully
 * @param fail_phrase If this pattern found the command fails immediately
 * @param timeout_ms Command timeout in ms
 * @return Generic command return type (OK, FAIL, TIMEOUT)
 */
command_result generic_command(CommandableIf *t, const std::string &command,
                               const std::string &pass_phrase,
                               const std::string &fail_phrase, uint32_t timeout_ms);

/**
 * @brief Utility command to send command and return reply (after DCE says OK)
 * @param t Anything that is "command-able"
 * @param command Command to issue
 * @param output String to return (could be either std::string& or std::string_view&)
 * @param timeout_ms Command timeout in ms
 * @return Generic command return type (OK, FAIL, TIMEOUT)
 */
template <typename T> command_result generic_get_string(CommandableIf *t, const std::string &command, T &output, uint32_t timeout_ms = 500);

/**
 * @brief Generic command that passes on "OK" and fails on "ERROR"
 * @param t Anything that is "command-able"
 * @param command Command to issue
 * @param timeout_ms Command timeout in ms
 * @return Generic command return type (OK, FAIL, TIMEOUT)
 */
command_result generic_command_common(CommandableIf *t, const std::string &command, uint32_t timeout_ms = 500);

} // esp_modem::dce_commands
