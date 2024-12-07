/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#include "sock_dce.hpp"
#include "esp_log.h"
#include "esp_transport.h"


static int tcp_connect(esp_transport_handle_t t, const char *host, int port, int timeout_ms)
{
    auto *dce = (sock_dce::DCE *)esp_transport_get_context_data(t);
    if (!dce->connect(host, port)) {
        return -1;
    }
    if (dce->wait_to_idle(timeout_ms)) {
        dce->set_idle();
        return 1;
    }
    return -1;
}

static int tcp_read(esp_transport_handle_t t, char *buffer, int len, int timeout_ms)
{
    auto *dce = (sock_dce::DCE *)esp_transport_get_context_data(t);
    auto ret = dce->wait_to_read(1000);
    if (ret > 0) {
        return dce->sync_recv(buffer, len, timeout_ms);
    }
    return ret;
}

static int tcp_write(esp_transport_handle_t t, const char *buffer, int len, int timeout_ms)
{

    auto *dce = (sock_dce::DCE *)esp_transport_get_context_data(t);
    return dce->sync_send(buffer, len, timeout_ms);

}

static int base_close(esp_transport_handle_t t)
{
    auto *dce = (sock_dce::DCE *)esp_transport_get_context_data(t);
    return dce->tcp_close() == esp_modem::command_result::OK ? 0 : -1;
}

static int base_poll_read(esp_transport_handle_t t, int timeout_ms)
{
    auto *dce = (sock_dce::DCE *)esp_transport_get_context_data(t);
    return dce->wait_to_read(timeout_ms);
}

static int base_poll_write(esp_transport_handle_t t, int timeout_ms)
{
    // expect the socket is always writable
    return 1;
}

static int base_destroy(esp_transport_handle_t transport)
{
    return 0;
}

esp_transport_handle_t esp_transport_at_init(sock_dce::DCE *dce)
{
    esp_transport_handle_t at = esp_transport_init();
    if (!at) {
        return nullptr;
    }
    esp_transport_set_context_data(at, (void *)dce);
    esp_transport_set_func(at, tcp_connect, tcp_read, tcp_write, base_close, base_poll_read, base_poll_write, base_destroy);
    return at;
}
