/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <esp_secure_boot.h>
#include <esp_efuse.h>

#if CONFIG_IDF_TARGET_ESP32
#include "esp32/rom/secure_boot.h"
#elif CONFIG_IDF_TARGET_ESP32S2
#include "esp32s2/rom/secure_boot.h"
#elif CONFIG_IDF_TARGET_ESP32C3
#include "esp32c3/rom/secure_boot.h"
#elif CONFIG_IDF_TARGET_ESP32S3
#include "esp32s3/rom/secure_boot.h"
#elif CONFIG_IDF_TARGET_ESP32C2
#include "esp32c2/rom/secure_boot.h"
#elif CONFIG_IDF_TARGET_ESP32C6
#include "esp32c6/rom/secure_boot.h"
#elif CONFIG_IDF_TARGET_ESP32H2
#include "esp32h2/rom/secure_boot.h"
#endif

/**
 * @brief Get secure boot digest
 *
 * @return 2D pointer with secure boot digest array
 * @note the memory allocated gets freed with \ref `esp_rmaker_secure_boot_digest_free` API
 */
char** esp_rmaker_get_secure_boot_digest();

/**
 * @brief free secure boot digest buffer
 */
esp_err_t esp_rmaker_secure_boot_digest_free(char **digest);
