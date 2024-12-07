/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#ifdef CONFIG_COMPILER_CXX_EXCEPTIONS
#define TRY_CATCH_OR_DO(block, action) \
    try { block                   \
    } catch (std::bad_alloc& e) { \
        ESP_LOGE(TAG, "Out of memory"); \
        action; \
    } catch (::esp_modem::esp_err_exception& e) {    \
        esp_err_t err = e.get_err_t();  \
        ESP_LOGE(TAG, "%s: Exception caught with ESP err_code=%d %s", __func__, err, esp_err_to_name(err)); \
        ESP_LOGE(TAG, "%s", e.what());  \
        action; \
    }

#define TRY_CATCH_RET_NULL(block) TRY_CATCH_OR_DO(block, return nullptr)
#else

#define TRY_CATCH_OR_DO(block, action) \
    block

#define TRY_CATCH_RET_NULL(block) \
    block

#endif
