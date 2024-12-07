/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#include <vector>
#include "esp_log.h"
#include "mbedtls_wrap.hpp"
#include "esp_transport_tcp.h"

#define TAG "batch-tls"

using namespace idf::mbedtls_cxx;

class TlsTransport: public Tls {
public:
    explicit TlsTransport(esp_transport_handle_t parent):
        Tls(), transport_(parent),  last_timeout(0), read_len(0), offset(0) {}
    int send(const unsigned char *buf, size_t len) override;
    int recv(unsigned char *buf, size_t len) override;
    static bool set_func(esp_transport_handle_t tls_transport);
    int preread(size_t len, int timeout_ms);
    bool prepare_buffer(size_t max_size);
private:
    esp_transport_handle_t transport_{};
    int connect(const char *host, int port, int timeout_ms);
    void delay() override;

    struct transport {
        static int connect(esp_transport_handle_t t, const char *host, int port, int timeout_ms);
        static int read(esp_transport_handle_t t, char *buffer, int len, int timeout_ms);
        static int write(esp_transport_handle_t t, const char *buffer, int len, int timeout_ms);
        static int close(esp_transport_handle_t t);
        static int poll_read(esp_transport_handle_t t, int timeout_ms);
        static int poll_write(esp_transport_handle_t t, int timeout_ms);
        static int destroy(esp_transport_handle_t t);
    };
    int last_timeout;
    std::vector<char> buf;
    size_t read_len;
    size_t offset;
};

esp_transport_handle_t esp_transport_tls_init(esp_transport_handle_t parent)
{
    esp_transport_handle_t transport_handle = esp_transport_init();
    auto *tls_context = new TlsTransport(parent);
    esp_transport_set_context_data(transport_handle, tls_context);
    TlsTransport::set_func(transport_handle);
    return transport_handle;
}

int TlsTransport::send(const unsigned char *buf, size_t len)
{
    int ret = esp_transport_write(transport_, reinterpret_cast<const char *>(buf), len, 0);
    ESP_LOGD(TAG, "writing(len=%d) ret=%d", len, ret);
    return ret;
}

int TlsTransport::recv(unsigned char *buffer, size_t len)
{
    ESP_LOGD(TAG, "recv(len=%d)", len);
    if (read_len != 0) {

        if (read_len > len) {
            memcpy((char *)buffer, buf.data() + offset, len);
            read_len -= len;
            offset += len;
            ESP_LOGD(TAG, "read %d from batch read_len = %d", len, read_len);
            return len;
        } else {
            int remaining = len = read_len;
            if (remaining > 0) {
                memcpy((char *)buffer, buf.data() + offset, remaining);
                read_len = 0;
                offset = 0;
                return remaining;

            }
            read_len = 0;
            offset = 0;
            return ERR_TCP_TRANSPORT_CONNECTION_CLOSED_BY_FIN;
        }
    }
    int ret = esp_transport_read(transport_, reinterpret_cast<char *>(buffer), len, last_timeout);

    if (ret == ERR_TCP_TRANSPORT_CONNECTION_TIMEOUT) {
        return MBEDTLS_ERR_SSL_WANT_READ;
    }
    return ret == ERR_TCP_TRANSPORT_CONNECTION_CLOSED_BY_FIN ? 0 : ret;
}

bool TlsTransport::set_func(esp_transport_handle_t tls_transport)
{
    return esp_transport_set_func(tls_transport, TlsTransport::transport::connect, TlsTransport::transport::read, TlsTransport::transport::write, TlsTransport::transport::close, TlsTransport::transport::poll_read, TlsTransport::transport::poll_write, TlsTransport::transport::destroy) == ESP_OK;
}

int TlsTransport::connect(const char *host, int port, int timeout_ms)
{
    return esp_transport_connect(transport_, host, port, timeout_ms);
}

void TlsTransport::delay()
{
    vTaskDelay(pdMS_TO_TICKS(500));
}

int TlsTransport::transport::connect(esp_transport_handle_t t, const char *host, int port, int timeout_ms)
{
    auto tls = static_cast<TlsTransport *>(esp_transport_get_context_data(t));
    tls->init(is_server{false}, do_verify{true});

    ESP_LOGD(TAG, "TLS-connect");
    auto ret = tls->connect(host, port, timeout_ms);
    if (ret < 0) {
        ESP_LOGI(TAG, "Failed to connect to transport");
        return ret;
    }
    if (tls->is_session_loaded()) {
        tls->set_session();
    }
    ESP_LOGI(TAG, "Before handshake");
    ret = tls->handshake();
    if (ret < 0) {
        ESP_LOGI(TAG, "Failed to handshake");
        return ret;
    }
    if (!tls->get_session()) {
        // we're not able to save session, report an error and continue (next connection will be slower)
        ESP_LOGW(TAG, "Failed to save session");
    }
    ESP_LOGI(TAG, "After handshake");
    return 0;
}

int TlsTransport::transport::read(esp_transport_handle_t t, char *buffer, int len, int timeout_ms)
{
    auto tls = static_cast<TlsTransport *>(esp_transport_get_context_data(t));
    ESP_LOGD(TAG, "available=%d tls->read_len=%d", tls->get_available_bytes(), tls->read_len);
    if (tls->get_available_bytes() <= 0 && tls->read_len == 0) {
        ESP_LOGD(TAG, "red(len=%d, timeout=%d) tls->read_len=%d", len, timeout_ms, tls->read_len);
        tls->last_timeout = timeout_ms;
        int poll = esp_transport_poll_read(t, timeout_ms);
        if (poll == -1) {
            return ERR_TCP_TRANSPORT_CONNECTION_FAILED;
        }
        if (poll == 0) {
            return ERR_TCP_TRANSPORT_CONNECTION_TIMEOUT;
        }
    }

    auto ret =  tls->read(reinterpret_cast<unsigned char *>(buffer), len);
    if (ret == MBEDTLS_ERR_SSL_WANT_READ) {
        ret = ERR_TCP_TRANSPORT_CONNECTION_TIMEOUT;
    }
    ESP_LOGD(TAG, "red(len=%d, timeout=%d) ret=%d", len, timeout_ms, ret);
    return ret;
}

int TlsTransport::transport::write(esp_transport_handle_t t, const char *buffer, int len, int timeout_ms)
{
    int poll;
    if ((poll = esp_transport_poll_write(t, timeout_ms)) <= 0) {
        ESP_LOGW(TAG, "Poll timeout or error timeout_ms=%d", timeout_ms);
        return poll;
    }

    auto tls = static_cast<TlsTransport *>(esp_transport_get_context_data(t));
    int ret = tls->write(reinterpret_cast<const unsigned char *>(buffer), len);
    ESP_LOGD(TAG, "write ret=%d", ret);
    return ret;
}

int TlsTransport::transport::close(esp_transport_handle_t t)
{
    auto tls = static_cast<TlsTransport *>(esp_transport_get_context_data(t));
    int ret = esp_transport_close(tls->transport_);
    tls->deinit();
    return ret;
}

int TlsTransport::transport::poll_read(esp_transport_handle_t t, int timeout_ms)
{
    auto tls = static_cast<TlsTransport *>(esp_transport_get_context_data(t));
    return esp_transport_poll_read(tls->transport_, timeout_ms);
}

int TlsTransport::transport::poll_write(esp_transport_handle_t t, int timeout_ms)
{
    auto tls = static_cast<TlsTransport *>(esp_transport_get_context_data(t));
    return esp_transport_poll_write(tls->transport_, timeout_ms);
}

int TlsTransport::transport::destroy(esp_transport_handle_t t)
{
    auto tls = static_cast<TlsTransport *>(esp_transport_get_context_data(t));
    return esp_transport_destroy(tls->transport_);
}


esp_transport_handle_t esp_transport_batch_tls_init(esp_transport_handle_t parent, const size_t max_buffer_size)
{
    esp_transport_handle_t ssl = esp_transport_init();
    auto *tls = new TlsTransport(parent);
    esp_transport_set_context_data(ssl, tls);
    TlsTransport::set_func(ssl);
    tls->prepare_buffer(max_buffer_size);
    return ssl;
}

bool esp_transport_batch_set_ca_cert(esp_transport_handle_t t, const char *ca_cert, size_t cert_len = 0)
{
    auto tls = static_cast<TlsTransport *>(esp_transport_get_context_data(t));
    const_buf cert((const unsigned char *)ca_cert, cert_len ? cert_len : strlen(ca_cert) + 1);
    return tls->set_ca_cert(cert);
}

bool esp_transport_batch_set_cn(esp_transport_handle_t t, const char *name)
{
    auto tls = static_cast<TlsTransport *>(esp_transport_get_context_data(t));
    return tls->set_hostname(name);
}

int TlsTransport::preread(size_t len, int timeout_ms)
{
    while (len != read_len) {
        int l = esp_transport_read(transport_, buf.data() + read_len, len - read_len, timeout_ms);
        ESP_LOGD(TAG, "need %d read %d already %d", len, l, read_len);
        if ((l == ERR_TCP_TRANSPORT_CONNECTION_CLOSED_BY_FIN || l == ERR_TCP_TRANSPORT_CONNECTION_TIMEOUT ) && read_len > 0) {
            return read_len;
        }
        if (l <= 0) {
            read_len = 0;
            return read_len;
        }
        read_len += l;
    }
    return read_len;
}

bool TlsTransport::prepare_buffer(size_t max_size)
{
    buf.resize(max_size);
    return true;
}

int esp_transport_batch_tls_pre_read(esp_transport_handle_t t, size_t len, int timeout_ms)
{
    auto tls = static_cast<TlsTransport *>(esp_transport_get_context_data(t));
    return tls->preread(len, timeout_ms);
}
