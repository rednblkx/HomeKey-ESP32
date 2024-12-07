/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#pragma once
#include <vector>
#include "esp_http_client.h"
#include "esp_partition.h"
#include "esp_transport_tcp.h"
#include "esp_ota_ops.h"

class manual_ota {
public:
    /**
     * @brief Set the preferred mode
     */
    enum class mode {
        BATCH,      /**< Read data chunk from TCP and pass it to SSL, restore session on reconnection */
        NORMAL      /**< Use standard partial download, continuously passing data from TCP to mbedTLS */
    } mode_ {mode::BATCH};

    /**
     * @brief Set the OTA batch size in kB
     *
     * This would allocate two big buffers:
     * - one for reading from TCP socket and
     * - one for passing to mbedTLS for description
     */
    size_t size_{32};

    /**
     * @brief Set timeout in seconds
     *
     * This is the network timeout, so if less data than the batch size received
     * the timeout (and no EOF) we should proceed with passing the data to mbedtls
     */
    int timeout_{2};

    /**
     * @brief Set common name of the server to verify
     */
    const char *common_name_{};
    /**
     * @brief Wrapper around the http client -- Please set the http config
     */
    class http_client {
        friend class manual_ota;
        ~http_client();
        bool init();
        esp_http_client_handle_t handle_{nullptr};
        bool handle_redirects();
        bool set_range(size_t from, size_t to);
        bool is_data_complete();
        int64_t get_image_len();
    public:
        esp_http_client_config_t config_{};     /**< Configure the http connection parameters */
    } http_;

    /**
     * @brief Construct a new manual ota object
     */
    explicit manual_ota() {}

    ~manual_ota();

    /**
     * @brief Start the manual OTA process
     *
     * @return true if started successfully
     */
    bool begin();

    /**
     * @brief Performs one read-write OTA iteration
     *
     * @return true if the process is in progress
     * @return false if the process finished, call end() to get OTA result
     */
    bool perform();

    /**
     * @brief Finishes an OTA update
     *
     * @return true if the OTA update completed successfully
     */
    bool end();

private:
    enum class state {
        UNDEF,
        INIT,
        IMAGE_CHECK,
        START,
        END,
        FAIL,
    };
    int64_t image_length_;
    size_t file_length_;
    size_t max_buffer_size_{size_ * 1024};
    const esp_partition_t *partition_{nullptr};
    state status{state::UNDEF};
    std::vector<char> buffer_{};
    int reconnect_attempts_;
    const int max_reconnect_attempts_{3};
    esp_transport_handle_t ssl_;
    esp_ota_handle_t update_handle_{0};
    bool ota_begin;

    bool prepare_reconnect();
    bool fail();
};
