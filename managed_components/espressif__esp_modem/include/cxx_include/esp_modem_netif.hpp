/*
 * SPDX-FileCopyrightText: 2021-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <memory>
#include <cstddef>
#include "esp_netif.h"
#include "cxx_include/esp_modem_primitives.hpp"

namespace esp_modem {

class DTE;
class Netif;

struct ppp_netif_driver {
    esp_netif_driver_base_t base;
    Netif *ppp;
};

/**
 * @defgroup ESP_MODEM_NETIF
 * @brief Network interface layer of the esp-modem
 */

/** @addtogroup ESP_MODEM_NETIF
* @{
*/

/**
 * @brief Network interface class responsible to glue the esp-netif to the modem's DCE
 */
class Netif {
public:
    explicit Netif(std::shared_ptr<DTE> e, esp_netif_t *netif);

    ~Netif();

    /**
     * @brief Start the network interface
     */
    void start();

    /**
     * @brief Blocks until the network interface closes
     */
    void wait_until_ppp_exits();

    /**
     * @brief Stop the network interface
     */
    void stop();

    /**
     * @brief Pause the network interface
     */
    void pause();

    /**
     * @brief Resume the network interface
     */
    void resume();

    void receive(uint8_t *data, size_t len);

private:

    static esp_err_t esp_modem_dte_transmit(void *h, void *buffer, size_t len);

    static esp_err_t esp_modem_post_attach(esp_netif_t *esp_netif, void *args);

    static void on_ppp_changed(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data);

    std::shared_ptr<DTE> ppp_dte;
    struct ppp_netif_driver driver {};
    SignalGroup signal;
    static const size_t PPP_STARTED = SignalGroup::bit0;
    static const size_t PPP_EXIT = SignalGroup::bit1;
};

/**
 * @}
 */

} // namespace esp_modem
