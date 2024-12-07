/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#pragma once
#include "cxx_include/esp_modem_api.hpp"
#include "cxx_include/esp_modem_command_library_utils.hpp"

/**
 * @brief Definition of a custom module based on some already defined module
 *          Here we use GenericModule, but you can use any kind of device
 *          that is closer to your CustomModule.
 */
class SIM7600_WITH_TIME: public GenericModule {
    /**
     * @brief Need to reuse the constructors of our ancestor
     */
    using GenericModule::GenericModule;
public:
    /**
     * @brief New command that is not defined in the GenericModule
     */
    command_result get_time(std::string &time)
    {
        return esp_modem::dce_commands::generic_get_string(dte.get(), "AT+CCLK?\r", time);
    }

    /**
     * @brief This command is already defined in the GenericModule
     *
     * Here we just modify get_signal_quality() to return zeroes
     * for demonstration purpose only, since it's called within this example
     */
    command_result get_signal_quality(int &rssi, int &ber) override
    {
        rssi = ber = 0;
        return esp_modem::command_result::OK;
    }

};

/**
 * @brief esp_modem_create_custom_dce() needs to be defined, as it is called in C-API wrapper when creating esp_modem_dce
 *
 * This uses public factory function for creating a common DCE with our CustomModule. Creating raw DCE pointer is only needed
 * for the C-API wrapper; C++API users would create DCE (any kind of smart pointer) directly with
 * Factory::create_unique_dce_from<CustomModule>(dce_config, std::move(dte), netif);
 */
DCE *esp_modem_create_custom_dce(const esp_modem_dce_config_t *dce_config, std::shared_ptr<DTE> dte, esp_netif_t *netif)
{
    return dce_factory::Factory::create_unique_dce_from<SIM7600_WITH_TIME, DCE *>(dce_config, std::move(dte), netif);
}

/**
 * @brief This API is only needed for extending standard C-API, since we added get_time() method to our CustomModule
 *
 * @note This header is included from esp_modem_c_api.cpp, so it could use CONFIG_ESP_MODEM_C_API_STR_MAX macro
 * indicating maximum C-API string size
 *
 * @note In order to access the newly added API get_time(), we have to static_cast<> the GenericModule from DCE
 * to our CustomModule.
 * Alternatively we could use the modem Factory to build our specific DCE_T<CustomModule>, but in that case
 * we couldn't use our C-API wrappers which expect DCE type, DCE_T<GenericModule> with lib commands (this alternative
 * is cleaner, but more suitable for C++ users)
 */
extern "C" esp_err_t esp_modem_get_time(esp_modem_dce_t *dce_wrap, char *p_time)
{
    if (dce_wrap == nullptr || dce_wrap->dce == nullptr) {
        return ESP_ERR_INVALID_ARG;
    }
    std::string time{CONFIG_ESP_MODEM_C_API_STR_MAX};
    auto ret = command_response_to_esp_err(static_cast<SIM7600_WITH_TIME *>(dce_wrap->dce->get_module())->get_time(time));
    if (ret == ESP_OK && !time.empty()) {
        strlcpy(p_time, time.c_str(), CONFIG_ESP_MODEM_C_API_STR_MAX);
    }
    return ret;
}
