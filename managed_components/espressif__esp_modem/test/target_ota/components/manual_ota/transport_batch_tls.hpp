/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#pragma once

/**
 * @brief Creates batch transport
 *
 * @param parent tcp-transport handle to the parent transport
 * @param max_buffer_size maximum size of one batch
 * @return created transport handle
 */
esp_transport_handle_t esp_transport_batch_tls_init(esp_transport_handle_t parent, const size_t max_buffer_size);

/**
 * @brief Performs batch read operation from the underlying transport
 *
 * @param t Transport handle
 * @param len Batch size
 * @param timeout_ms Timeout in ms
 * @return true If read from the parent transport completed successfully
 */
bool esp_transport_batch_tls_pre_read(esp_transport_handle_t t, size_t len, int timeout_ms);

/**
 * @brief Set the CA Certificate to verify the server
 *
 * @param ca_cert Pointer to the CA Cert data
 * @param cert_len CA Cert data len (set to 0 if null terminated string, i.e. PEM format)
 * @return true on success
 */
bool esp_transport_batch_set_ca_cert(esp_transport_handle_t t, const char *ca_cert, size_t cert_len);

/**
 * @brief Set comman name
 * @param t
 * @param ca_cert
 * @param cert_len
 * @return
 */
bool esp_transport_batch_set_cn(esp_transport_handle_t t, const char *name);
