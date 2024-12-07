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

#ifdef CONFIG_COMPILER_CXX_EXCEPTIONS
static const char *TAG = "modem_api_target";
#endif

std::shared_ptr<DTE> create_uart_dte(const dte_config *config)
{
    TRY_CATCH_RET_NULL(
        auto term = create_uart_terminal(config);
        return std::make_shared<DTE>(config, std::move(term));
    )
}

} // namespace esp_modem
