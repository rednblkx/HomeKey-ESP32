/*
 * SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _PROV_NETWORK_CTRL_H_
#define _PROV_NETWORK_CTRL_H_

#include <esp_err.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Internal handlers for receiving and responding to protocomm
 *          requests from client
 *
 * This is to be passed as priv_data for protocomm request handler
 * (refer to `network_ctrl_handler()`) when calling `protocomm_add_endpoint()`.
 */
typedef struct network_ctrl_handlers {
#ifdef CONFIG_NETWORK_PROV_NETWORK_TYPE_WIFI
    /**
     * Handler functions called when ctrl reset command is received
     */
    esp_err_t (*wifi_ctrl_reset)(void);

    /**
     * Handler functions called when ctrl reprov command is received
     */
    esp_err_t (*wifi_ctrl_reprov)(void);
#endif // CONFIG_NETWORK_PROV_NETWORK_TYPE_WIFI

#ifdef CONFIG_NETWORK_PROV_NETWORK_TYPE_THREAD
    esp_err_t (*thread_ctrl_reset)(void);
    esp_err_t (*thread_ctrl_reprov)(void);
#endif // CONFIG_NETWORK_PROV_NETWORK_TYPE_THREAD
} network_ctrl_handlers_t;

/**
 * @brief   Handler for sending on demand network ctrl results
 *
 * This is to be registered as the `prov-ctrl` endpoint handler
 * (protocomm `protocomm_req_handler_t`) using `protocomm_add_endpoint()`
 */
esp_err_t network_ctrl_handler(uint32_t session_id, const uint8_t *inbuf, ssize_t inlen,
                               uint8_t **outbuf, ssize_t *outlen, void *priv_data);

#ifdef __cplusplus
}
#endif

#endif
