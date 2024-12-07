/*
 * SPDX-FileCopyrightText: 2018-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _NETWORK_PROV_CONFIG_H_
#define _NETWORK_PROV_CONFIG_H_

#include "esp_netif_ip_addr.h"
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef CONFIG_NETWORK_PROV_NETWORK_TYPE_WIFI
/**
 * @brief   WiFi STA status for conveying back to the provisioning master
 */
typedef enum {
    NETWORK_PROV_WIFI_STA_CONNECTING,
    NETWORK_PROV_WIFI_STA_CONNECTED,
    NETWORK_PROV_WIFI_STA_DISCONNECTED
} network_prov_wifi_sta_state_t;

/**
 * @brief   WiFi STA connection fail reason
 */
typedef enum {
    NETWORK_PROV_WIFI_STA_AUTH_ERROR,
    NETWORK_PROV_WIFI_STA_AP_NOT_FOUND
} network_prov_wifi_sta_fail_reason_t;

/**
 * @brief   WiFi STA connected status information
 */
typedef struct {
    /**
     * IP Address received by station
     */
    char    ip_addr[IP4ADDR_STRLEN_MAX];

    char    bssid[6];   /*!< BSSID of the AP to which connection was estalished */
    char    ssid[33];   /*!< SSID of the to which connection was estalished */
    uint8_t channel;    /*!< Channel of the AP */
    uint8_t auth_mode;  /*!< Authorization mode of the AP */
} network_prov_wifi_sta_conn_info_t;

/**
 * @brief   WiFi status data to be sent in response to `get_status` request from master
 */
typedef struct {
    network_prov_wifi_sta_state_t wifi_state;    /*!< WiFi state of the station */
    union {
        /**
         * Reason for disconnection (valid only when `wifi_state` is `WIFI_STATION_DISCONNECTED`)
         */
        network_prov_wifi_sta_fail_reason_t fail_reason;

        /**
         * Connection information (valid only when `wifi_state` is `WIFI_STATION_CONNECTED`)
         */
        network_prov_wifi_sta_conn_info_t   conn_info;
    };
} network_prov_config_get_wifi_data_t;

/**
 * @brief   WiFi config data received by slave during `set_config` request from master
 */
typedef struct {
    char    ssid[33];       /*!< SSID of the AP to which the slave is to be connected */
    char    password[64];   /*!< Password of the AP */
    char    bssid[6];       /*!< BSSID of the AP */
    uint8_t channel;        /*!< Channel of the AP */
} network_prov_config_set_wifi_data_t;
#endif // CONFIG_NETWORK_PROV_NETWORK_TYPE_WIFI

#ifdef CONFIG_NETWORK_PROV_NETWORK_TYPE_THREAD
/**
 * @brief   Thread status for conveying back to the provisioning master
 */
typedef enum {
    NETWORK_PROV_THREAD_ATTACHING,
    NETWORK_PROV_THREAD_ATTACHED,
    NETWORK_PROV_THREAD_DETACHED,
} network_prov_thread_state_t;

/**
 * @brief   Thread attaching fail reason
 */
typedef enum {
    NETWORK_PROV_THREAD_DATASET_INVALID,
    NETWORK_PROV_THREAD_NETWORK_NOT_FOUND,
} network_prov_thread_fail_reason_t;

/**
 * @brief   Thread attached status information
 */
typedef struct {
    uint32_t pan_id; /*!< PAN ID */
    uint16_t channel; /*!< Channel */
    char name[17]; /*!< Network Name */
    uint8_t ext_pan_id[8]; /*!< Extended PAN ID */
} network_prov_thread_conn_info_t;

/**
 * @brief   Thread status data to be sent in response to `get_status` request from master
 */
typedef struct {
    network_prov_thread_state_t thread_state;
    union {
        /**
         * Reason for disconnection (valid only when `thread_state` is `THREAD_DETACHED`)
         */
        network_prov_thread_fail_reason_t fail_reason;

        /**
         * Connection information (valid only when `thread_state` is `THREAD_ATTACHED`)
         */
        network_prov_thread_conn_info_t conn_info;
    };
} network_prov_config_get_thread_data_t;

/**
 * @brief   Thread config data received by slave during `set_config` request from master
 */
typedef struct {
    uint8_t dataset[254];
    uint8_t length;
} network_prov_config_set_thread_data_t;
#endif // CONFIG_NETWORK_PROV_NETWORK_TYPE_THREAD

/**
 * @brief   Type of context data passed to each get/set/apply handler
 *           function set in `network_prov_config_handlers` structure.
 *
 * This is passed as an opaque pointer, thereby allowing it be defined
 * later in application code as per requirements.
 */
typedef struct network_prov_ctx network_prov_ctx_t;

/**
 * @brief   Internal handlers for receiving and responding to protocomm
 *          requests from master
 *
 * This is to be passed as priv_data for protocomm request handler
 * (refer to `network_prov_config_data_handler()`) when calling `protocomm_add_endpoint()`.
 */
typedef struct network_prov_config_handlers {
#ifdef CONFIG_NETWORK_PROV_NETWORK_TYPE_WIFI
    /**
     * Handler function called when connection status
     * of the slave (in WiFi station mode) is requested
     */
    esp_err_t (*wifi_get_status_handler)(network_prov_config_get_wifi_data_t *resp_data,
                                         network_prov_ctx_t **ctx);

    /**
     * Handler function called when WiFi connection configuration
     * (eg. AP SSID, password, etc.) of the slave (in WiFi station mode)
     * is to be set to user provided values
     */
    esp_err_t (*wifi_set_config_handler)(const network_prov_config_set_wifi_data_t *req_data,
                                         network_prov_ctx_t **ctx);

    /**
     * Handler function for applying the configuration that was set in
     * `set_config_handler`. After applying the station may get connected to
     * the AP or may fail to connect. The slave must be ready to convey the
     * updated connection status information when `get_status_handler` is
     * invoked again by the master.
     */
    esp_err_t (*wifi_apply_config_handler)(network_prov_ctx_t **ctx);
#endif // CONFIG_NETWORK_PROV_NETWORK_TYPE_WIFI

#ifdef CONFIG_NETWORK_PROV_NETWORK_TYPE_THREAD
    esp_err_t (*thread_get_status_handler)(network_prov_config_get_thread_data_t *resp_data,
                                           network_prov_ctx_t **ctx);

    esp_err_t (*thread_set_config_handler)(const network_prov_config_set_thread_data_t *req_data,
                                           network_prov_ctx_t **ctx);

    esp_err_t (*thread_apply_config_handler)(network_prov_ctx_t **ctx);
#endif // CONFIG_NETWORK_PROV_NETWORK_TYPE_THREAD

    /**
     * Context pointer to be passed to above handler functions upon invocation
     */
    network_prov_ctx_t *ctx;
} network_prov_config_handlers_t;

/**
 * @brief   Handler for receiving and responding to requests from master
 *
 * This is to be registered as the `network_config` endpoint handler
 * (protocomm `protocomm_req_handler_t`) using `protocomm_add_endpoint()`
 */
esp_err_t network_prov_config_data_handler(uint32_t session_id, const uint8_t *inbuf, ssize_t inlen,
        uint8_t **outbuf, ssize_t *outlen, void *priv_data);

#ifdef __cplusplus
}
#endif

#endif
