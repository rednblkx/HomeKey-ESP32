/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

/**
 * @file esp_insights_cbor_decoder.h
 * @brief this file contains useful abstractions on cbor
 *
 * @note please keep this file as utility, avoid taking insights decisions here
 */

#include <cbor.h>

/**
 * @brief   decodes a cbor message and prints into json format
 *
 * @param buffer    buffer to decode and print
 * @param len       length of the buffer to decode
 * @return esp_err_t ESP_OK on success, apt error otherwise
 */
esp_err_t esp_insights_cbor_decode_dump(const uint8_t *buffer, int len);
