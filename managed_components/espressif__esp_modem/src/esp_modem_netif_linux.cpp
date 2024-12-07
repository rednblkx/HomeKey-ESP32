/*
 * SPDX-FileCopyrightText: 2021-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <thread>
#include "cxx_include/esp_modem_netif.hpp"
#include "cxx_include/esp_modem_dte.hpp"

namespace esp_modem {

void Netif::on_ppp_changed(void *arg, esp_event_base_t event_base,
                           int32_t event_id, void *event_data)
{
}

esp_err_t Netif::esp_modem_dte_transmit(void *h, void *buffer, size_t len)
{
    auto *this_netif = static_cast<Netif *>(h);
    this_netif->ppp_dte->write((uint8_t *) buffer, len);
    return len;
}

esp_err_t Netif::esp_modem_post_attach(esp_netif_t *esp_netif, void *args)
{
    return ESP_OK;
}

void Netif::receive(uint8_t *data, size_t len)
{
    esp_netif_receive(driver.base.netif, data, len);
}

Netif::Netif(std::shared_ptr<DTE> e, esp_netif_t *ppp_netif) :
    ppp_dte(std::move(e))
{
    driver.base.netif = ppp_netif;
    driver.ppp = this;
}

void Netif::start()
{
    ppp_dte->set_read_cb([this](uint8_t *data, size_t len) -> bool {
        receive(data, len);
        return true;
    });
    driver.base.netif->transmit = esp_modem_dte_transmit;
    driver.base.netif->ctx = (void *)this;
    signal.set(PPP_STARTED);
}

void Netif::stop()
{
    signal.clear(PPP_STARTED);
}

Netif::~Netif() = default;

void Netif::wait_until_ppp_exits()
{
}

} // namespace esp_modem
