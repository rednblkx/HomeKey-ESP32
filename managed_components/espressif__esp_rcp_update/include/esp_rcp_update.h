/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "esp_loader.h"
#include "nvs.h"

#ifdef __cplusplus
extern "C" {
#endif

#define RCP_FIRMWARE_DIR_SIZE 20
#define RCP_FILENAME_MAX_SIZE 100
#define RCP_URL_MAX_SIZE 100

typedef enum {
    RCP_TYPE_INVALID = 0,  /* Invalid type */
    RCP_TYPE_ESP32H2_UART, /* ESP32H2 connected via UART */
} esp_rcp_type_t;

/**
 * @brief The RCP update config for OpenThread.
 *
 */
typedef struct {
    esp_rcp_type_t rcp_type;                  /*!< RCP type */
    int uart_rx_pin;                          /*!< UART rx pin */
    int uart_tx_pin;                          /*!< UART tx pin */
    int uart_port;                            /*!< UART port */
    int uart_baudrate;                        /*!< UART baudrate */
    int reset_pin;                            /*!< RESET pin */
    int boot_pin;                             /*!< Boot mode select pin */
    uint32_t update_baudrate;                 /*!< Baudrate when flashing the firmware */
    char firmware_dir[RCP_FIRMWARE_DIR_SIZE]; /*!< The directory storing the RCP firmware */
    target_chip_t target_chip;                /*!< The target chip type */
} esp_rcp_update_config_t;

/**
 * @brief This function initializes the RCP update process
 *
 * @param[in] update_config     The RCP update specific config
 *
 * @return
 *  - ESP_OK
 *  - ESP_FAIL
 *  - ESP_ERR_INVALID_ARG   If the RCP type is not supported.
 *
 */
esp_err_t esp_rcp_update_init(const esp_rcp_update_config_t *update_config);

/**
 * @brief This function triggers an RCP firmware update.
 *
 * @return
 *  - ESP_OK
 *  - ESP_FAIL
 *  - ESP_ERR_INVALID_STASTE    If the RCP update is not initialized.
 *  - ESP_ERR_NOT_FOUND         RCP firmware not found in storage.
 *
 */
esp_err_t esp_rcp_update(void);

/**
 * @brief This function acquires the RCP image base directory.
 *
 * @note The real RCP image directory should be suffixed the update sequence.
 *
 */
const char *esp_rcp_get_firmware_dir(void);

/**
 * @brief This function retrieves the update image sequence.
 *
 * The current update image sequence will be used to update the RCP.
 *
 */
int8_t esp_rcp_get_update_seq(void);

/**
 * @brief This function retrieves the next update image sequence.
 *
 * The next update image sequence will be used for the downloaded image.
 *
 */
int8_t esp_rcp_get_next_update_seq(void);

/**
 * @brief This function resets the RCP.
 *
 */
void esp_rcp_reset(void);

/**
 * @brief This function marks the downloaded image as valid.
 *
 * The image in the next update image sequence will then be used for RCP update.
 *
 * @return
 *  - ESP_OK
 *  - ESP_ERR_INVALID_STASTE    If the RCP update is not initialized.
 *
 */
esp_err_t esp_rcp_submit_new_image(void);

/**
 * @brief This function marks previously downloaded image as valid.
 *
 * @return
 *  - ESP_OK
 *  - ESP_ERR_INVALID_STASTE    If the RCP update is not initialized.
 *
 */
esp_err_t esp_rcp_mark_image_verified(bool verified);

/**
 * @brief This function marks previously downloaded image as unusable.
 *
 * @return
 *  - ESP_OK
 *  - ESP_ERR_INVALID_STASTE    If the RCP update is not initialized.
 *
 */
esp_err_t esp_rcp_mark_image_unusable(void);

/**
 * @brief This function loads the RCP version in the current update image.
 *
 * @param[out] version_str      The RCP version string output.
 * @param[in]  size             Size of version_str.
 *
 * @return
 *  - ESP_OK
 *  - ESP_ERR_INVALID_STASTE    If the RCP update is not initialized.
 *  - ESP_ERR_NOT_FOUND         RCP version not found in update image.
 *
 */
esp_err_t esp_rcp_load_version_in_storage(char *version_str, size_t size);

/**
 * @brief This function deinitializes the RCP update process.
 *
 */
void esp_rcp_update_deinit(void);

#ifdef __cplusplus
}
#endif
