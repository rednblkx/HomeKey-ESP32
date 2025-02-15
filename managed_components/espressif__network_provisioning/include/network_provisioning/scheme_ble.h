/*
 * SPDX-FileCopyrightText: 2019-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <protocomm.h>
#include <protocomm_ble.h>

#include "network_provisioning/manager.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Scheme that can be used by manager for provisioning
 *          over BLE transport with GATT server
 */
extern const network_prov_scheme_t network_prov_scheme_ble;

/* This scheme specific event handler is to be used when application
 * doesn't require BT and BLE after provisioning has finished */
#define NETWORK_PROV_SCHEME_BLE_EVENT_HANDLER_FREE_BTDM {    \
    .event_cb  = network_prov_scheme_ble_event_cb_free_btdm, \
    .user_data = NULL                                     \
}

/* This scheme specific event handler is to be used when application
 * doesn't require BLE to be active after provisioning has finished */
#define NETWORK_PROV_SCHEME_BLE_EVENT_HANDLER_FREE_BLE {    \
    .event_cb  = network_prov_scheme_ble_event_cb_free_ble, \
    .user_data = NULL                                    \
}

/* This scheme specific event handler is to be used when application
 * doesn't require BT to be active after provisioning has finished */
#define NETWORK_PROV_SCHEME_BLE_EVENT_HANDLER_FREE_BT {    \
    .event_cb  = network_prov_scheme_ble_event_cb_free_bt, \
    .user_data = NULL                                   \
}

void network_prov_scheme_ble_event_cb_free_btdm(void *user_data, network_prov_cb_event_t event, void *event_data);
void network_prov_scheme_ble_event_cb_free_ble (void *user_data, network_prov_cb_event_t event, void *event_data);
void network_prov_scheme_ble_event_cb_free_bt  (void *user_data, network_prov_cb_event_t event, void *event_data);

/**
 * @brief   Set the 128 bit GATT service UUID used for provisioning
 *
 * This API is used to override the default 128 bit provisioning
 * service UUID, which is 0000ffff-0000-1000-8000-00805f9b34fb.
 *
 * This must be called before starting provisioning, i.e. before
 * making a call to network_prov_mgr_start_provisioning(), otherwise
 * the default UUID will be used.
 *
 * @note    The data being pointed to by the argument must be valid
 *          atleast till provisioning is started. Upon start, the
 *          manager will store an internal copy of this UUID, and
 *          this data can be freed or invalidated afterwords.
 *
 * @param[in] uuid128  A custom 128 bit UUID
 *
 * @return
 *  - ESP_OK              : Success
 *  - ESP_ERR_INVALID_ARG : Null argument
 */
esp_err_t network_prov_scheme_ble_set_service_uuid(uint8_t *uuid128);

/**
 * @brief   Set manufacturer specific data in scan response
 *
 * This must be called before starting provisioning, i.e. before
 * making a call to network_prov_mgr_start_provisioning().
 *
 * @note    It is important to understand that length of custom manufacturer
 *          data should be within limits. The manufacturer data goes into scan
 *          response along with BLE device name. By default, BLE device name
 *          length is of 11 Bytes, however it can vary as per application use
 *          case. So, one has to honour the scan response data size limits i.e.
 *          (mfg_data_len + 2) < 31 - (device_name_length + 2 ). If the
 *          mfg_data length exceeds this limit, the length will be truncated.
 *
 * @param[in] mfg_data      Custom manufacturer data
 * @param[in] mfg_data_len  Manufacturer data length
 *
 * @return
 *  - ESP_OK              : Success
 *  - ESP_ERR_INVALID_ARG : Null argument
 */
esp_err_t network_prov_scheme_ble_set_mfg_data(uint8_t *mfg_data, ssize_t mfg_data_len);

#ifdef __cplusplus
}
#endif
