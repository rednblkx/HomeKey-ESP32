/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#define RET_ON_ERR_WITH_LOG(x, level, tag, format, ...) {             \
    esp_err_t ret = (x);                                              \
    if (ret != ESP_OK) {                                              \
        ESP_LOG_LEVEL(level, tag, "%s", format, ##__VA_ARGS__);       \
        return ret;                                                   \
    }                                                                 \
}

#define SEC2TICKS(s) ((s * 1000) / portTICK_PERIOD_MS)

#ifdef __cplusplus
}
#endif
