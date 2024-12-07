/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <cassert>
#include "esp_log.h"
#include "cxx_include/esp_modem_dte.hpp"
#include "uart_terminal.hpp"
#include "vfs_termial.hpp"
#include "cxx_include/esp_modem_api.hpp"
#include "cxx_include/esp_modem_dce_factory.hpp"
#include "esp_modem_config.h"
#include "exception_stub.hpp"

namespace esp_modem {

struct PdpContext;

#ifdef CONFIG_COMPILER_CXX_EXCEPTIONS
static const char *TAG = "modem_api";
#endif


std::shared_ptr<DTE> create_vfs_dte(const dte_config *config)
{
    TRY_CATCH_RET_NULL(
        auto term = create_vfs_terminal(config);
        return std::make_shared<DTE>(config, std::move(term));
    )
}


static inline std::unique_ptr<DCE>
create_modem_dce(dce_factory::ModemType m, const dce_config *config, std::shared_ptr<DTE> dte, esp_netif_t *netif)
{
    dce_factory::Factory f(m);
    TRY_CATCH_RET_NULL(
        return f.build_unique(config, std::move(dte), netif);
    )
}

std::unique_ptr<DCE> create_SIM7600_dce(const dce_config *config, std::shared_ptr<DTE> dte, esp_netif_t *netif)
{
    return create_modem_dce(dce_factory::ModemType::SIM7600, config, std::move(dte), netif);
}

std::unique_ptr<DCE> create_SIM7070_dce(const dce_config *config, std::shared_ptr<DTE> dte, esp_netif_t *netif)
{
    return create_modem_dce(dce_factory::ModemType::SIM7070, config, std::move(dte), netif);
}

std::unique_ptr<DCE> create_SIM7000_dce(const dce_config *config, std::shared_ptr<DTE> dte, esp_netif_t *netif)
{
    return create_modem_dce(dce_factory::ModemType::SIM7000, config, std::move(dte), netif);
}

std::unique_ptr<DCE> create_SIM800_dce(const dce_config *config, std::shared_ptr<DTE> dte, esp_netif_t *netif)
{
    return create_modem_dce(dce_factory::ModemType::SIM800, config, std::move(dte), netif);
}

std::unique_ptr<DCE> create_BG96_dce(const dce_config *config, std::shared_ptr<DTE> dte, esp_netif_t *netif)
{
    return create_modem_dce(dce_factory::ModemType::BG96, config, std::move(dte), netif);
}

std::unique_ptr<DCE> create_generic_dce(const dce_config *config, std::shared_ptr<DTE> dte, esp_netif_t *netif)
{
    return create_modem_dce(dce_factory::ModemType::GenericModule, config, std::move(dte), netif);
}

} // namespace esp_modem
