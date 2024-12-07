// Copyright 2021 Espressif Systems (Shanghai) PTE LTD
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
#if CONFIG_ESP_INSIGHTS_COREDUMP_ENABLE
#include <esp_core_dump.h>
#endif

size_t esp_insights_encode_meta(uint8_t *out_data, size_t out_data_size, char *sha256);

esp_err_t esp_insights_encode_data_begin(uint8_t *out_data, size_t out_data_size);
void esp_insights_encode_boottime_data(void);

/**
 * @brief encode critical data
 *
 * @param critical_data pointer to critical data
 * @param critical_data_size size of critical data
 * @return size_t length of data consumed
 */
size_t esp_insights_encode_critical_data(const void *critical_data, size_t critical_data_size);

/**
 * @brief encode non_critical data
 *
 * @param critical_data pointer to non_critical data
 * @param critical_data_size size of non_critical data
 * @return size_t length of data consumed
 */
size_t esp_insights_encode_non_critical_data(const void *non_critical_data, size_t non_critical_data_size);

/**
 * @brief finish encoding message
 *
 * @param out_data encoded data pointer
 * @return size_t size of the data encoded
 */
size_t esp_insights_encode_data_end(uint8_t *out_data);
