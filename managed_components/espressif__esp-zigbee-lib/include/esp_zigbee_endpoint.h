/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once
#ifdef __cplusplus
extern "C" {
#endif
#include "esp_zigbee_type.h"

/**
 * @brief  Create an empty endpoint list.
 *
 * @note This endpoint list is ready to add endpoint refer @ref esp_zb_ep_list_add_ep.
 * @return pointer to  @ref esp_zb_ep_list_s
 *
 */
esp_zb_ep_list_t *esp_zb_ep_list_create(void);

/**
 * @brief Add an endpoint (which includes cluster list) in a endpoint list.
 *
 * @param[in] ep_list A pointer to endpoint list @ref esp_zb_ep_list_s
 * @param[in] cluster_list  An cluster list which wants to add to endpoint
 * @param[in] endpoint_config  A specific endpoint config @ref esp_zb_endpoint_config_s
 * @anchor esp_zb_ep_list_add_ep
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_ARG if endpoint list not initialized
 *
 */
esp_err_t esp_zb_ep_list_add_ep(esp_zb_ep_list_t *ep_list, esp_zb_cluster_list_t *cluster_list, esp_zb_endpoint_config_t endpoint_config);

/**
 * @brief Add a gateway endpoint to the endpoint list.
 * @param[in] ep_list         A pointer to the endpoint list where the @p cluster_list will be added, @ref esp_zb_ep_list_s.
 * @param[in] cluster_list    A pointer to @ref esp_zb_cluster_list_s indicating the gateway's clusters.
 * @param[in] endpoint_config The specific endpoint configuration, @ref esp_zb_endpoint_config_s.
 *
 * @return
 *      - ESP_OK on success.
 *      - ESP_ERR_NO_MEM if the number of gateways reaches the limit.
 *      - ESP_ERR_INVALID_ARG if the endpoint list is not initialized.
 */
esp_err_t esp_zb_ep_list_add_gateway_ep(esp_zb_ep_list_t *ep_list, esp_zb_cluster_list_t *cluster_list, esp_zb_endpoint_config_t endpoint_config);

/**
 * @brief Get endpoint (cluster list) from a endpoint list.
 *
 * @param[in] ep_list A pointer to endpoint list @ref esp_zb_ep_list_s
 * @param[in] ep_id The endpoint id for cluster list
 *
 * @return
 *      - pointer to  @ref esp_zb_cluster_list_s, if the endpoint is found in the endpoint list
 *      - ``NULL`` if endpoint is not found
 *
 */
esp_zb_cluster_list_t *esp_zb_ep_list_get_ep(const esp_zb_ep_list_t *ep_list, uint8_t ep_id);

#ifdef __cplusplus
}
#endif
