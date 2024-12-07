/*
 * SPDX-FileCopyrightText: 2021-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <cstring>
#include <cassert>
#include "cxx_include/esp_modem_dte.hpp"
#include "uart_terminal.hpp"
#include "esp_log.h"
#include "cxx_include/esp_modem_api.hpp"
#include "cxx_include/esp_modem_dce_factory.hpp"
#include "esp_modem_c_api_types.h"
#include "esp_modem_config.h"
#include "exception_stub.hpp"
#include "esp_private/c_api_wrapper.hpp"

#ifndef HAVE_STRLCPY
size_t strlcpy(char *dest, const char *src, size_t len);
#endif

#ifdef CONFIG_ESP_MODEM_ADD_CUSTOM_MODULE
#include CONFIG_ESP_MODEM_CUSTOM_MODULE_HEADER
#endif

//
// C API definitions
using namespace esp_modem;

extern "C" esp_modem_dce_t *esp_modem_new_dev(esp_modem_dce_device_t module, const esp_modem_dte_config_t *dte_config, const esp_modem_dce_config_t *dce_config, esp_netif_t *netif)
{
    auto dce_wrap = new (std::nothrow) esp_modem_dce_wrap;
    if (dce_wrap == nullptr) {
        return nullptr;
    }
    auto dte = create_uart_dte(dte_config);
    if (dte == nullptr) {
        delete dce_wrap;
        return nullptr;
    }
    dce_wrap->dte = dte;
#ifdef CONFIG_ESP_MODEM_ADD_CUSTOM_MODULE
    if (module == ESP_MODEM_DCE_CUSTOM) {
        dce_wrap->dce = esp_modem_create_custom_dce(dce_config, dte, netif);
    } else
#endif
    {
        dce_factory::Factory f(convert_modem_enum(module));
        dce_wrap->dce = f.build(dce_config, std::move(dte), netif);
    }
    if (dce_wrap->dce == nullptr) {
        delete dce_wrap;
        return nullptr;
    }
    dce_wrap->modem_type = convert_modem_enum(module);
    dce_wrap->dte_type = esp_modem_dce_wrap::modem_wrap_dte_type::UART;
    return dce_wrap;
}

extern "C" esp_modem_dce_t *esp_modem_new(const esp_modem_dte_config_t *dte_config, const esp_modem_dce_config_t *dce_config, esp_netif_t *netif)
{
    return esp_modem_new_dev(ESP_MODEM_DCE_GENERIC, dte_config, dce_config, netif);
}

extern "C" void esp_modem_destroy(esp_modem_dce_t *dce_wrap)
{
    if (dce_wrap) {
        delete dce_wrap->dce;
        delete dce_wrap;
    }
}

extern "C" esp_err_t esp_modem_set_error_cb(esp_modem_dce_t *dce_wrap, esp_modem_terminal_error_cbt err_cb)
{
    if (dce_wrap == nullptr || dce_wrap->dce == nullptr || dce_wrap->dte == nullptr) {
        return ESP_ERR_INVALID_ARG;
    }

    if (err_cb) {
        dce_wrap->dte->set_error_cb([err_cb](terminal_error err) {
            err_cb(convert_terminal_error_enum(err));
        });
    } else {
        dce_wrap->dte->set_error_cb(nullptr);
    }
    return ESP_OK;
}

extern "C" esp_err_t esp_modem_sync(esp_modem_dce_t *dce_wrap)
{
    if (dce_wrap == nullptr || dce_wrap->dce == nullptr) {
        return ESP_ERR_INVALID_ARG;
    }
    return command_response_to_esp_err(dce_wrap->dce->sync());
}

extern "C" esp_modem_dce_mode_t esp_modem_get_mode(esp_modem_dce_t *dce_wrap)
{
    if (dce_wrap == nullptr || dce_wrap->dce == nullptr) {
        return ESP_MODEM_MODE_UNDEF;
    }
    auto mode = dce_wrap->dce->get_mode();
    switch (mode) {
    default:
    case modem_mode::UNDEF:
        return ESP_MODEM_MODE_UNDEF;
    case modem_mode::COMMAND_MODE:
        return ESP_MODEM_MODE_COMMAND;
    case modem_mode::DATA_MODE:
        return ESP_MODEM_MODE_DATA;
    case modem_mode::CMUX_MODE:
        return ESP_MODEM_MODE_CMUX;
    case modem_mode::CMUX_MANUAL_MODE:
        return ESP_MODEM_MODE_CMUX_MANUAL;
    case modem_mode::CMUX_MANUAL_EXIT:
        return ESP_MODEM_MODE_CMUX_MANUAL_EXIT;
    case modem_mode::CMUX_MANUAL_DATA:
        return ESP_MODEM_MODE_CMUX_MANUAL_DATA;
    case modem_mode::CMUX_MANUAL_COMMAND:
        return ESP_MODEM_MODE_CMUX_MANUAL_COMMAND;
    case modem_mode::CMUX_MANUAL_SWAP:
        return ESP_MODEM_MODE_CMUX_MANUAL_SWAP;
    }
}

extern "C" esp_err_t esp_modem_set_mode(esp_modem_dce_t *dce_wrap, esp_modem_dce_mode_t mode)
{
    if (dce_wrap == nullptr || dce_wrap->dce == nullptr) {
        return ESP_ERR_INVALID_ARG;
    }
    switch (mode) {
    case ESP_MODEM_MODE_UNDEF:
        return dce_wrap->dce->set_mode(modem_mode::UNDEF) ? ESP_OK : ESP_FAIL;
    case ESP_MODEM_MODE_DATA:
        return dce_wrap->dce->set_mode(modem_mode::DATA_MODE) ? ESP_OK : ESP_FAIL;
    case ESP_MODEM_MODE_COMMAND:
        return dce_wrap->dce->set_mode(modem_mode::COMMAND_MODE) ? ESP_OK : ESP_FAIL;
    case ESP_MODEM_MODE_CMUX:
        return dce_wrap->dce->set_mode(modem_mode::CMUX_MODE) ? ESP_OK : ESP_FAIL;
    case ESP_MODEM_MODE_CMUX_MANUAL:
        return dce_wrap->dce->set_mode(modem_mode::CMUX_MANUAL_MODE) ? ESP_OK : ESP_FAIL;
    case ESP_MODEM_MODE_CMUX_MANUAL_EXIT:
        return dce_wrap->dce->set_mode(modem_mode::CMUX_MANUAL_EXIT) ? ESP_OK : ESP_FAIL;
    case ESP_MODEM_MODE_CMUX_MANUAL_SWAP:
        return dce_wrap->dce->set_mode(modem_mode::CMUX_MANUAL_SWAP) ? ESP_OK : ESP_FAIL;
    case ESP_MODEM_MODE_CMUX_MANUAL_DATA:
        return dce_wrap->dce->set_mode(modem_mode::CMUX_MANUAL_DATA) ? ESP_OK : ESP_FAIL;
    case ESP_MODEM_MODE_CMUX_MANUAL_COMMAND:
        return dce_wrap->dce->set_mode(modem_mode::CMUX_MANUAL_COMMAND) ? ESP_OK : ESP_FAIL;
    case ESP_MODEM_MODE_DETECT:
        return dce_wrap->dce->set_mode(modem_mode::AUTODETECT) ? ESP_OK : ESP_FAIL;
    }
    return ESP_ERR_NOT_SUPPORTED;
}

extern "C" esp_err_t esp_modem_read_pin(esp_modem_dce_t *dce_wrap, bool *pin)
{
    if (dce_wrap == nullptr || dce_wrap->dce == nullptr) {
        return ESP_ERR_INVALID_ARG;
    }

    return command_response_to_esp_err(dce_wrap->dce->read_pin(*pin));
}

extern "C" esp_err_t esp_modem_sms_txt_mode(esp_modem_dce_t *dce_wrap, bool txt)
{
    if (dce_wrap == nullptr || dce_wrap->dce == nullptr) {
        return ESP_ERR_INVALID_ARG;
    }

    return command_response_to_esp_err(dce_wrap->dce->sms_txt_mode(txt));
}

extern "C" esp_err_t esp_modem_send_sms(esp_modem_dce_t *dce_wrap, const char *number, const char *message)
{
    if (dce_wrap == nullptr || dce_wrap->dce == nullptr) {
        return ESP_ERR_INVALID_ARG;
    }
    std::string number_str(number);
    std::string message_str(message);
    return command_response_to_esp_err(dce_wrap->dce->send_sms(number_str, message_str));
}

extern "C" esp_err_t esp_modem_sms_character_set(esp_modem_dce_t *dce_wrap)
{
    if (dce_wrap == nullptr || dce_wrap->dce == nullptr) {
        return ESP_ERR_INVALID_ARG;
    }

    return command_response_to_esp_err(dce_wrap->dce->sms_character_set());
}

extern "C" esp_err_t esp_modem_set_pin(esp_modem_dce_t *dce_wrap, const char *pin)
{
    if (dce_wrap == nullptr || dce_wrap->dce == nullptr) {
        return ESP_ERR_INVALID_ARG;
    }
    std::string pin_str(pin);
    return command_response_to_esp_err(dce_wrap->dce->set_pin(pin_str));
}

extern "C" esp_err_t esp_modem_at(esp_modem_dce_t *dce_wrap, const char *at, char *p_out, int timeout)
{
    if (dce_wrap == nullptr || dce_wrap->dce == nullptr) {
        return ESP_ERR_INVALID_ARG;
    }
    std::string out;
    std::string at_str(at);
    auto ret = command_response_to_esp_err(dce_wrap->dce->at(at_str, out, timeout));
    if ((p_out != NULL) && (!out.empty())) {
        strlcpy(p_out, out.c_str(), CONFIG_ESP_MODEM_C_API_STR_MAX);
    }
    return ret;
}

extern "C" esp_err_t esp_modem_get_signal_quality(esp_modem_dce_t *dce_wrap, int *rssi, int *ber)
{
    if (dce_wrap == nullptr || dce_wrap->dce == nullptr) {
        return ESP_ERR_INVALID_ARG;
    }
    return command_response_to_esp_err(dce_wrap->dce->get_signal_quality(*rssi, *ber));
}

extern "C" esp_err_t esp_modem_get_imsi(esp_modem_dce_t *dce_wrap, char *p_imsi)
{
    if (dce_wrap == nullptr || dce_wrap->dce == nullptr) {
        return ESP_ERR_INVALID_ARG;
    }
    std::string imsi;
    auto ret = command_response_to_esp_err(dce_wrap->dce->get_imsi(imsi));
    if (ret == ESP_OK && !imsi.empty()) {
        strlcpy(p_imsi, imsi.c_str(), CONFIG_ESP_MODEM_C_API_STR_MAX);
    }
    return ret;
}

extern "C" esp_err_t esp_modem_at_raw(esp_modem_dce_t *dce_wrap, const char *cmd, char *p_out, const char *pass, const char *fail, int timeout)
{
    if (dce_wrap == nullptr || dce_wrap->dce == nullptr) {
        return ESP_ERR_INVALID_ARG;
    }
    std::string out;
    auto ret = command_response_to_esp_err(dce_wrap->dce->at_raw(cmd, out, pass, fail, timeout));
    if ((p_out != NULL) && (!out.empty())) {
        strlcpy(p_out, out.c_str(), CONFIG_ESP_MODEM_C_API_STR_MAX);
    }
    return ret;
}


extern "C" esp_err_t esp_modem_set_flow_control(esp_modem_dce_t *dce_wrap, int dce_flow, int dte_flow)
{
    if (dce_wrap == nullptr || dce_wrap->dce == nullptr) {
        return ESP_ERR_INVALID_ARG;
    }
    return command_response_to_esp_err(dce_wrap->dce->set_flow_control(dce_flow, dte_flow));
}

extern "C" esp_err_t esp_modem_store_profile(esp_modem_dce_t *dce_wrap)
{
    if (dce_wrap == nullptr || dce_wrap->dce == nullptr) {
        return ESP_ERR_INVALID_ARG;
    }
    return command_response_to_esp_err(dce_wrap->dce->store_profile());
}

extern "C" esp_err_t esp_modem_get_imei(esp_modem_dce_t *dce_wrap, char *p_imei)
{
    if (dce_wrap == nullptr || dce_wrap->dce == nullptr) {
        return ESP_ERR_INVALID_ARG;
    }
    std::string imei;
    auto ret = command_response_to_esp_err(dce_wrap->dce->get_imei(imei));
    if (ret == ESP_OK && !imei.empty()) {
        strlcpy(p_imei, imei.c_str(), CONFIG_ESP_MODEM_C_API_STR_MAX);
    }
    return ret;
}

extern "C" esp_err_t esp_modem_get_operator_name(esp_modem_dce_t *dce_wrap, char *p_name, int *p_act)
{
    if (dce_wrap == nullptr || dce_wrap->dce == nullptr || p_name == nullptr || p_act == nullptr) {
        return ESP_ERR_INVALID_ARG;
    }
    std::string name;
    int act;
    auto ret = command_response_to_esp_err(dce_wrap->dce->get_operator_name(name, act));
    if (ret == ESP_OK && !name.empty()) {
        strlcpy(p_name, name.c_str(), CONFIG_ESP_MODEM_C_API_STR_MAX);
        *p_act = act;
    }
    return ret;
}

extern "C" esp_err_t esp_modem_get_module_name(esp_modem_dce_t *dce_wrap, char *p_name)
{
    if (dce_wrap == nullptr || dce_wrap->dce == nullptr) {
        return ESP_ERR_INVALID_ARG;
    }
    std::string name;
    auto ret = command_response_to_esp_err(dce_wrap->dce->get_module_name(name));
    if (ret == ESP_OK && !name.empty()) {
        strlcpy(p_name, name.c_str(), CONFIG_ESP_MODEM_C_API_STR_MAX);
    }
    return ret;
}

extern "C" esp_err_t esp_modem_get_battery_status(esp_modem_dce_t *dce_wrap, int *p_volt, int *p_bcs, int *p_bcl)
{
    if (dce_wrap == nullptr || dce_wrap->dce == nullptr || p_bcs == nullptr || p_bcl == nullptr || p_volt == nullptr) {
        return ESP_ERR_INVALID_ARG;
    }
    int bcs, bcl, volt;
    auto ret = command_response_to_esp_err(dce_wrap->dce->get_battery_status(volt, bcs, bcl));
    if (ret == ESP_OK) {
        *p_volt = volt;
        *p_bcs = bcs;
        *p_bcl = bcl;
    }
    return ret;
}

extern "C" esp_err_t esp_modem_power_down(esp_modem_dce_t *dce_wrap)
{
    if (dce_wrap == nullptr || dce_wrap->dce == nullptr) {
        return ESP_ERR_INVALID_ARG;
    }
    return command_response_to_esp_err(dce_wrap->dce->power_down());
}

extern "C" esp_err_t esp_modem_set_operator(esp_modem_dce_t *dce_wrap, int mode, int format, const char *oper)
{
    if (dce_wrap == nullptr || dce_wrap->dce == nullptr) {
        return ESP_ERR_INVALID_ARG;
    }
    std::string operator_str(oper);
    return command_response_to_esp_err(dce_wrap->dce->set_operator(mode, format, operator_str));
}

extern "C" esp_err_t esp_modem_set_network_attachment_state(esp_modem_dce_t *dce_wrap, int state)
{
    if (dce_wrap == nullptr || dce_wrap->dce == nullptr) {
        return ESP_ERR_INVALID_ARG;
    }
    return command_response_to_esp_err(dce_wrap->dce->set_network_attachment_state(state));
}

extern "C" esp_err_t esp_modem_get_network_attachment_state(esp_modem_dce_t *dce_wrap, int *p_state)
{
    if (dce_wrap == nullptr || dce_wrap->dce == nullptr) {
        return ESP_ERR_INVALID_ARG;
    }
    int state;
    auto ret = command_response_to_esp_err(dce_wrap->dce->get_network_attachment_state(state));
    if (ret == ESP_OK) {
        *p_state = state;
    }
    return ret;
}

extern "C" esp_err_t esp_modem_set_radio_state(esp_modem_dce_t *dce_wrap, int state)
{
    if (dce_wrap == nullptr || dce_wrap->dce == nullptr) {
        return ESP_ERR_INVALID_ARG;
    }
    return command_response_to_esp_err(dce_wrap->dce->set_radio_state(state));
}

extern "C" esp_err_t esp_modem_get_radio_state(esp_modem_dce_t *dce_wrap, int *p_state)
{
    if (dce_wrap == nullptr || dce_wrap->dce == nullptr) {
        return ESP_ERR_INVALID_ARG;
    }
    int state;
    auto ret = command_response_to_esp_err(dce_wrap->dce->get_radio_state(state));
    if (ret == ESP_OK) {
        *p_state = state;
    }
    return ret;
}

extern "C" esp_err_t esp_modem_set_network_mode(esp_modem_dce_t *dce_wrap, int mode)
{
    if (dce_wrap == nullptr || dce_wrap->dce == nullptr) {
        return ESP_ERR_INVALID_ARG;
    }
    return command_response_to_esp_err(dce_wrap->dce->set_network_mode(mode));
}

extern "C" esp_err_t esp_modem_set_preferred_mode(esp_modem_dce_t *dce_wrap, int mode)
{
    if (dce_wrap == nullptr || dce_wrap->dce == nullptr) {
        return ESP_ERR_INVALID_ARG;
    }
    return command_response_to_esp_err(dce_wrap->dce->set_preferred_mode(mode));
}

extern "C" esp_err_t esp_modem_set_network_bands(esp_modem_dce_t *dce_wrap, const char *mode, const int *bands, int size)
{
    if (dce_wrap == nullptr || dce_wrap->dce == nullptr) {
        return ESP_ERR_INVALID_ARG;
    }
    std::string mode_str(mode);
    return command_response_to_esp_err(dce_wrap->dce->set_network_bands(mode, bands, size));
}

extern "C" esp_err_t esp_modem_get_network_system_mode(esp_modem_dce_t *dce_wrap, int *p_mode)
{
    if (dce_wrap == nullptr || dce_wrap->dce == nullptr) {
        return ESP_ERR_INVALID_ARG;
    }
    int mode;
    auto ret = command_response_to_esp_err(dce_wrap->dce->get_network_system_mode(mode));
    if (ret == ESP_OK) {
        *p_mode = mode;
    }
    return ret;
}

extern "C" esp_err_t esp_modem_set_gnss_power_mode(esp_modem_dce_t *dce_wrap, int mode)
{
    if (dce_wrap == nullptr || dce_wrap->dce == nullptr) {
        return ESP_ERR_INVALID_ARG;
    }
    return command_response_to_esp_err(dce_wrap->dce->set_gnss_power_mode(mode));
}

extern "C" esp_err_t esp_modem_get_gnss_power_mode(esp_modem_dce_t *dce_wrap, int *p_mode)
{
    if (dce_wrap == nullptr || dce_wrap->dce == nullptr) {
        return ESP_ERR_INVALID_ARG;
    }
    int mode;
    auto ret = command_response_to_esp_err(dce_wrap->dce->get_gnss_power_mode(mode));
    if (ret == ESP_OK) {
        *p_mode = mode;
    }
    return ret;
}

extern "C" esp_err_t esp_modem_reset(esp_modem_dce_t *dce_wrap)
{
    return command_response_to_esp_err(dce_wrap->dce->reset());
}

extern "C" esp_err_t esp_modem_set_pdp_context(esp_modem_dce_t *dce_wrap, esp_modem_PdpContext_t *c_api_pdp)
{
    if (dce_wrap == nullptr || dce_wrap->dce == nullptr) {
        return ESP_ERR_INVALID_ARG;
    }
    esp_modem::PdpContext pdp{c_api_pdp->apn};
    pdp.context_id = c_api_pdp->context_id;
    pdp.protocol_type = c_api_pdp->protocol_type;
    return command_response_to_esp_err(dce_wrap->dce->set_pdp_context(pdp));
}

extern "C" esp_err_t esp_modem_command(esp_modem_dce_t *dce_wrap, const char *command, esp_err_t(*got_line_fn)(uint8_t *data, size_t len), uint32_t timeout_ms)
{
    if (dce_wrap == nullptr || dce_wrap->dce == nullptr || command == nullptr || got_line_fn == nullptr) {
        return ESP_ERR_INVALID_ARG;
    }
    std::string cmd(command);
    return command_response_to_esp_err(dce_wrap->dce->command(cmd, [got_line_fn](uint8_t *data, size_t len) {
        switch (got_line_fn(data, len)) {
        case ESP_OK:
            return command_result::OK;
        case ESP_FAIL:
            return command_result::FAIL;
        default:
            return command_result::TIMEOUT;
        }
    }, timeout_ms));
}

extern "C" esp_err_t esp_modem_set_baud(esp_modem_dce_t *dce_wrap, int baud)
{
    return command_response_to_esp_err(dce_wrap->dce->set_baud(baud));
}

extern "C" esp_err_t esp_modem_set_apn(esp_modem_dce_t *dce_wrap, const char *apn)
{
    auto new_pdp = std::unique_ptr<PdpContext>(new PdpContext(apn));
    dce_wrap->dce->get_module()->configure_pdp_context(std::move(new_pdp));
    return ESP_OK;
}

#ifdef CONFIG_ESP_MODEM_URC_HANDLER
extern "C" esp_err_t esp_modem_set_urc(esp_modem_dce_t *dce_wrap, esp_err_t(*got_line_fn)(uint8_t *data, size_t len))
{
    if (dce_wrap == nullptr || dce_wrap->dce == nullptr) {
        return ESP_ERR_INVALID_ARG;
    }
    if (got_line_fn == nullptr) {
        dce_wrap->dce->set_urc(nullptr);
        return ESP_OK;
    }
    dce_wrap->dce->set_urc([got_line_fn](uint8_t *data, size_t len) {
        switch (got_line_fn(data, len)) {
        case ESP_OK:
            return command_result::OK;
        case ESP_FAIL:
            return command_result::FAIL;
        default:
            return command_result::TIMEOUT;
        }
    });
    return ESP_OK;
}
#endif

extern "C" esp_err_t esp_modem_pause_net(esp_modem_dce_t *dce_wrap, bool pause)
{
    if (dce_wrap == nullptr || dce_wrap->dce == nullptr) {
        return ESP_ERR_INVALID_ARG;
    }
    return command_response_to_esp_err(dce_wrap->dce->pause_netif(pause));
}

extern "C" esp_err_t esp_modem_hang_up(esp_modem_dce_t *dce_wrap)
{
    if (dce_wrap == nullptr || dce_wrap->dce == nullptr) {
        return ESP_ERR_INVALID_ARG;
    }
    return command_response_to_esp_err(dce_wrap->dce->hang_up());
}
