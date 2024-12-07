/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once
#ifdef __cplusplus
extern "C" {
#endif

#include "esp_err.h"
#include "esp_zigbee_type.h"
#include "zcl/esp_zigbee_zcl_common.h"

/**
 * @brief The basic zcl information for OTA command
 *
 */
typedef struct esp_zb_ota_zcl_information_s {
    esp_zb_zcl_addr_t src_addr;         /*!< The source address */
    uint16_t dst_short_addr;            /*!< The destination short address */
    uint8_t src_endpoint;               /*!< The source endpoint identifier */
    uint8_t dst_endpoint;               /*!< The destination endpoint identifier */
    uint16_t cluster_id;                /*!< The cluster identifier */
    uint16_t profile_id;                /*!< The profile identifier */
    uint8_t command_id;                 /*!< The command identifier */
    uint16_t manufacturer_specific;     /*!< The manufacturer specific  data */
} esp_zb_ota_zcl_information_t;

/**
 * @brief The ZCL ota upgrade header optional structure
 *
 */
typedef struct esp_zb_ota_file_optional_s {
    uint8_t security_credential_version;        /*!< Indicate security credential version type such as SE1.0 or SE2.0 that the client
                                                     is required to have, before it SHALL install the image. */
    esp_zb_ieee_addr_t upgrade_file_destination;/*!< Indicates that this OTA file contains security credential/certificate data or
                                                     other type of information that is specific to a particular device.*/
    uint16_t minimum_hardware_version;          /*!< Represent the earliest hardware platform version this image SHOULD be used on. */
    uint16_t maximum_hardware_version;          /*!< Represent the latest hardware platform version this image SHOULD be used on. */
} esp_zb_ota_file_optional_t;

/**
 * @brief The Zigbee ZCL OTA file header struct.
 *
 */
typedef struct esp_zb_ota_file_header_s {
    uint16_t manufacturer_code;                /*!< OTA header manufacturer code */
    uint16_t image_type;                       /*!< Image type value to distinguish the products */
    uint32_t file_version;                     /*!< File version represents the release and build number of the image’s application and stack */
    uint32_t image_size;                       /*!< Total image size in bytes transferred from the server to the client */
    uint16_t field_control;                    /*!< Indicate whether additional optional information */
    esp_zb_ota_file_optional_t optional;       /*!< The optional header controlled by the filed contorl, @see esp_zb_ota_file_optional_t */
} esp_zb_ota_file_header_t;

/**
 * @brief A callback for the OTA Server to retrieve the next OTA data
 *
 * @param[in] message The message will provide the basic OTA cluster information, @ref esp_zb_ota_zcl_information_s
 * @param[in] index   The index of the OTA file
 * @param[in] size    The size indicates how many bytes the user needs to allocate for the @p data
 * @param[out] data   The next OTA data that will be transmit to OTA Client by the OTA Server
 *
 */
typedef esp_err_t (*esp_zb_ota_next_data_callback_t)(esp_zb_ota_zcl_information_t message, uint16_t index, uint8_t size, uint8_t **data);

/**
 * @brief The Zigbee ZCL OTA upgrade client variable configuration struct.
 *
 */
typedef struct esp_zb_zcl_ota_upgrade_client_variable_s {
    uint16_t timer_query;  /*!< The field indicates the time of querying OTA image for OTA upgrade client */
    uint16_t hw_version;   /*!< The hardware version */
    uint8_t max_data_size; /*!< The maximum size of OTA data */
} esp_zb_zcl_ota_upgrade_client_variable_t;

/**
 * @brief The Zigbee ZCL OTA upgrade server variable configuration struct.
 *
 */
typedef struct esp_zb_zcl_ota_upgrade_server_variable_s {
    uint8_t query_jitter;  /*!< Query jitter */
    uint32_t current_time; /*!< Current time of OTA server */
    uint8_t file_count;    /*!< The field specifies the maximum number of OTA files for the OTA upgrade server variable. */
} esp_zb_zcl_ota_upgrade_server_variable_t;

/**
 * @brief The Zigbee ZCL OTA upgrade server notification request struct
 *
 */
typedef struct esp_zb_ota_upgrade_server_notify_req_s {
    uint8_t endpoint;                             /*!< The endpoint identifier for ota server cluster */
    uint8_t index;                                /*!< The index of OTA file */
    uint8_t notify_on;                            /*!< The field indicates whether send the notification request directly */
    uint32_t ota_upgrade_time;                    /*!< The time indicates the interval for the OTA file upgrade after the OTA process is completed */
    esp_zb_ota_file_header_t ota_file_header;     /*!< The header is used to register the basic OTA upgrade information */
    esp_zb_ota_next_data_callback_t next_data_cb; /*!< The callback is used to retrieve the next OTA data, which will be transmitted to the OTA client side */
} esp_zb_ota_upgrade_server_notify_req_t;

/********************* Declare functions **************************/
/**
 * @brief Notify the image upgrade event of OTA upgrade server
 *
 * @param[in] req The OTA file information request @ref esp_zb_ota_upgrade_server_notify_req_s
 * @return
 *      - ESP_OK: On success
 *      - ESP_ERR_NOT_FOUND: Not found the variable table in the OTA server side
 *      - ESP_ERR_INVALID_ARG: The input arguments are incorrect or invalid.
 */
esp_err_t esp_zb_ota_upgrade_server_notify_req(esp_zb_ota_upgrade_server_notify_req_t *req);

/**
 * @brief Send the OTA upgrade client query image request
 * 
 * @param[in] server_addr The short address of the OTA upgrade server that the client expect to query 
 * @param[in] server_ep   The endpoint identifier of the OTA upgrade server with OTA image
 * @return
 *      - ESP_OK: On success
 *      - ESP_FAIL: On failed
 */
esp_err_t esp_zb_ota_upgrade_client_query_image_req(uint16_t server_ep, uint8_t server_addr);

/**
 * @brief Set the interval of query for OTA upgrade client 
 * 
 * @param[in] endpoint The endpoint identifier of OTA upgrade client resides
 * @param[in] interval The interval in minute
 * @return
 *      - ESP_OK: On success
 *      - ESP_FAIL: On failed
 */
esp_err_t esp_zb_ota_upgrade_client_query_interval_set(uint8_t endpoint, uint16_t interval);

/**
 * @brief Stop the image query of OTA upgrade client
 *
 * @return
 *      - ESP_OK: On success
 *      - ESP_FAIL: On failed
 */
esp_err_t esp_zb_ota_upgrade_client_query_image_stop(void);

#ifdef __cplusplus
}
#endif
