// Copyright 2024 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

#include <esp_err.h>
#include <esp_openthread.h>
#include <esp_rcp_update.h>

/** Enable Thread Border Router
 *
 * This API enables the Thread Border Router service for the node. For more information,
 * check [here](https://openthread.io/guides/border-router/espressif-esp32)
 *
 * @note This API should be called after esp_rmaker_node_init() but before esp_rmaker_start().
 *
 * @param[in] platform_config Platform config for OpenThread
 * @param[in] rcp_update_config RCP update config
 *
 * @return ESP_OK on success.
 * @return error in case of failure.
 */
esp_err_t esp_rmaker_thread_br_enable(const esp_openthread_platform_config_t *platform_config,
                                      const esp_rcp_update_config_t *rcp_update_config);

#ifdef __cplusplus
}
#endif
