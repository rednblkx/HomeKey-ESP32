/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#include "cxx_include/esp_modem_dte.hpp"
#include "esp_modem_config.h"
#include "cxx_include/esp_modem_api.hpp"
#include "cxx_include/esp_modem_dce_factory.hpp"
#include <memory>
#include <utility>

class NetModule;

/**
 * @brief Custom factory which can build and create a DCE using a custom module
 */
class NetDCE_Factory: public esp_modem::dce_factory::Factory {
public:
    template <typename T, typename ...Args>
    static auto create(const esp_modem::dce_factory::config *cfg, Args &&... args) -> std::shared_ptr<esp_modem::DCE_T<T>>
    {
        return build_generic_DCE<T, esp_modem::DCE_T<T>, std::shared_ptr<esp_modem::DCE_T<T>>>(cfg, std::forward<Args>(args)...);
    }
};

/**
 * @brief This is a null-module, doesn't define any AT commands, just passes everything to pppd
 */
class NetModule: public esp_modem::ModuleIf {
public:
    explicit NetModule(std::shared_ptr<esp_modem::DTE> dte, const esp_modem_dce_config *cfg):
        dte(std::move(dte)) {}

    bool setup_data_mode() override
    {
        return true;
    }

    bool set_mode(esp_modem::modem_mode mode) override
    {
        return true;
    }

private:
    std::shared_ptr<esp_modem::DTE> dte;
};


std::shared_ptr<esp_modem::DCE_T<NetModule>> create(std::shared_ptr<esp_modem::DTE> dte, esp_netif_t *netif)
{
    const esp_modem::dce_config config = {};
    return NetDCE_Factory::create<NetModule>(&config, dte, netif);
}
