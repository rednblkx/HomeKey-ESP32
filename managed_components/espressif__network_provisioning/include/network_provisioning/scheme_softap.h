/*
 * SPDX-FileCopyrightText: 2019-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <protocomm.h>
#include <protocomm_httpd.h>

#include "network_provisioning/manager.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Scheme that can be used by manager for provisioning
 *          over SoftAP transport with HTTP server
 */
extern const network_prov_scheme_t network_prov_scheme_softap;

/**
 *
 * @brief Provide HTTPD Server handle externally.
 *
 * Useful in cases wherein applications need the webserver for some
 * different operations, and do not want the wifi provisioning component
 * to start/stop a new instance.
 *
 * @note This API should be called before network_prov_mgr_start_provisioning()
 *
 * @param[in] handle Handle to HTTPD server instance
 */
void network_prov_scheme_softap_set_httpd_handle(void *handle);
#ifdef __cplusplus
}
#endif
