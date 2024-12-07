/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file c_api_wrapper.hpp
 * @brief Collection of C API wrappers
 *
 * This file is located in include/esp_private because it is not intended for users,
 * but rather for esp_modem C extension developers.
 *
 * The C extension API must provide a 'factory function' that returns initialized pointer to esp_modem_dce_wrap.
 * Helper functions provided below, can be used for conversion between C++ enum classes and standard C enums.
 */

#pragma once

#include "cxx_include/esp_modem_dce_factory.hpp"
#include "esp_modem_c_api_types.h"

using namespace esp_modem;

struct esp_modem_dce_wrap { // need to mimic the polymorphic dispatch as CPP uses templated dispatch
    enum class modem_wrap_dte_type { UART, VFS, USB } dte_type;
    dce_factory::ModemType modem_type;
    DCE *dce;
    std::shared_ptr<DTE> dte;
    esp_modem_dce_wrap() : dce(nullptr), dte(nullptr) {}
};

inline dce_factory::ModemType convert_modem_enum(esp_modem_dce_device_t module)
{
    switch (module) {
    case ESP_MODEM_DCE_SIM7600:
        return esp_modem::dce_factory::ModemType::SIM7600;
    case ESP_MODEM_DCE_SIM7070:
        return esp_modem::dce_factory::ModemType::SIM7070;
    case ESP_MODEM_DCE_SIM7000:
        return esp_modem::dce_factory::ModemType::SIM7000;
    case ESP_MODEM_DCE_BG96:
        return esp_modem::dce_factory::ModemType::BG96;
    case ESP_MODEM_DCE_SIM800:
        return esp_modem::dce_factory::ModemType::SIM800;
    default:
    case ESP_MODEM_DCE_GENERIC:
        return esp_modem::dce_factory::ModemType::GenericModule;
    }
}

inline esp_modem_terminal_error_t convert_terminal_error_enum(terminal_error err)
{
    switch (err) {
    case terminal_error::BUFFER_OVERFLOW:
        return ESP_MODEM_TERMINAL_BUFFER_OVERFLOW;
    case terminal_error::CHECKSUM_ERROR:
        return ESP_MODEM_TERMINAL_CHECKSUM_ERROR;
    case terminal_error::UNEXPECTED_CONTROL_FLOW:
        return ESP_MODEM_TERMINAL_UNEXPECTED_CONTROL_FLOW;
    case terminal_error::DEVICE_GONE:
        return ESP_MODEM_TERMINAL_DEVICE_GONE;
    default:
        return ESP_MODEM_TERMINAL_UNKNOWN_ERROR;
    }
}

inline esp_err_t command_response_to_esp_err(command_result res)
{
    switch (res) {
    case command_result::OK:
        return ESP_OK;
    case command_result::FAIL:
        return ESP_FAIL;
    case command_result::TIMEOUT:
        return ESP_ERR_TIMEOUT;
    }
    return ESP_ERR_INVALID_ARG;
}
