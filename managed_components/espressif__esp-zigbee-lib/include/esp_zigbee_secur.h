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

/** Enum of the Zigbee secure install code type
 * @note It is formed with 6/8/12/16 bytes IC (install code) + 2 bytes of CRC
 * @anchor esp_zb_secur_ic_type_t
 */
typedef enum {
    ESP_ZB_IC_TYPE_48 =     0x0,                    /*!< Install code total of 8 bytes length */
    ESP_ZB_IC_TYPE_64  =    0x1,                    /*!< Install code total of 10 bytes length */
    ESP_ZB_IC_TYPE_96 =     0x2,                    /*!< Install code total of 14 bytes length */
    ESP_ZB_IC_TYPE_128 =    0x3,                    /*!< Install code total of 18 bytes length */
    ESP_ZB_IC_TYPE_MAX =    0x4                     /*!< Unknown type */
} esp_zb_secur_ic_type_t;
/********************* Declare functions **************************/

/**
 * @brief   Get the Zigbee install code from storage
 *
 * @warning  Only for the non-Trust Center device (non Zigbee coordinator)!
 * @note     Called only after esp_zb_start(), otherwise no install code has been set from
 * production config refer to tools/mfg_tool
 *
 * @param[out]  ic_type  A pointer of install code type @ref esp_zb_secur_ic_type_t
 *
 * @return A pointer to the install code
 *
 */
uint8_t *esp_zb_secur_ic_get(uint8_t *ic_type);

/**
 * @brief   Add the Zigbee install code from remote device
 *
 * @warning  Only for the Trust Center device (Zigbee coordinator)!
 *
 * @param[in]  address  A 64-bit ieee address of the remote device composed by array of zb_uint8_t
 * @param[in]  ic_str  A string of the install code
 *
 * @return -ESP_OK on success
 */
esp_err_t esp_zb_secur_ic_str_add(esp_zb_ieee_addr_t address, char *ic_str);

/**
 * @brief   Set the Zigbee install code from character string
 *
 * @note if user wants to manually set install code instead of from production config refer to esp-zigbee-sdk/tools/mfg_tool
 *
 * @warning  Only for the non-Trust Center device (non Zigbee coordinator)!
 *
 * @param[in]  ic_str  A string of the install code
 *
 * @return - ESP_OK on success
 *
 */
esp_err_t esp_zb_secur_ic_str_set(char *ic_str);

/**
 * @brief   Add the Zigbee install code of a remote device with type
 *
 * @warning  Only for the Trust Center device (Zigbee coordinator)!
 *
 * @param[in]  address  A 64-bit ieee address of the remote device composed by array of zb_uint8_t
 * @param[in]  ic_type A install code type @ref esp_zb_secur_ic_type_t
 * @param[in]  ic  A pointer of data array for install code
 *
 */
esp_err_t esp_zb_secur_ic_add(esp_zb_ieee_addr_t address, uint8_t ic_type, uint8_t *ic);

/**
 * @brief   Set the Zigbee install code for a Zigbee device with type
 *
 * @note if user wants to set install code with type manually instead of from production config check esp-zigbee-sdk/tools/mfg_tool
 *
 * @warning  Only for the non-Trust Center device (non Zigbee coordinator)!
 *
 * @param[in]  ic_type A install code type @ref esp_zb_secur_ic_type_t
 * @param[in]  ic  A pointer of data array for install code
 *
 * @return - ESP_OK on success
 *
 */
esp_err_t esp_zb_secur_ic_set(uint8_t ic_type, uint8_t *ic);

/**
 * @brief   remove the Zigbee install code of a specific device
 *
 * @warning  Only for the Trust Center device (Zigbee coordinator)!
 *
 * @param[in]  address device ieee addressing to remove ic
 * @return - ESP_OK on success
 */
esp_err_t esp_zb_secur_ic_remove_req(esp_zb_ieee_addr_t address);

/**
 * @brief   remove the Zigbee ALL install code
 *
 * @warning  Only for the Trust Center device (Zigbee coordinator)!
 *
 * @return - ESP_OK on success
 */
esp_err_t esp_zb_secur_ic_remove_all_req(void);

/**
 * @brief Set the Trust Center standard distributed key
 *
 * @param key A pointer to standard distributed key of Trust Center that will be set to
 */
 void esp_zb_secur_TC_standard_distributed_key_set(uint8_t *key);

/**
 * @brief Set the Trust Center pre-configured security key
 *
 * @param key A pointer to the pre-configured key of Trust Center that will be set to
 */
void esp_zb_secur_TC_standard_preconfigure_key_set(uint8_t *key);

/**
 * @brief Set the link key exchange requirement for a ZigBee node.
 * 
 * @note The ZigBee end device with @p disabled will not request key from the ZigBee coordinator.
 *       The ZigBee coordinator with @p disabled allows the end device to stay in network without a TC-link key exchange.
 * @param[in] enable Enable or Disable
 */
void esp_zb_secur_link_key_exchange_required_set(bool enable);

/**
 * @brief Enable/Disable the network layer security
 *
 * @param[in] enabled A boolean indicating whether network layer security will be enabled
 * @return
 *      - ESP_OK: On success
 *      - ESP_ERR_INVALID_STATE: Invalid setting when the device is in network state
 *      - Otherwise: On failure
 */
esp_err_t esp_zb_secur_network_security_enable(bool enabled);

/**
 * @brief Get the network security enabled state
 *
 * @return  Whether the network layer security is enabled or not
 */
bool esp_zb_secur_network_security_is_enabled(void);

/**
 * @brief Get the primary security network key
 *
 * @note The network key can only be obtained after the Zigbee device is the joined state.
 * @param[out] key The 16-byte network key
 *
 * @return
 *      - ESP_OK: on success
 *      - ESP_ERR_INVALID_STATE: invalid network state.
 */
esp_err_t esp_zb_secur_primary_network_key_get(uint8_t *key);

/**
 * @brief Set the Zigbee security network key
 *
 * @param key A 16-byte security network key that will be used
 * @return
 *      - ESP_OK: on success
 *      - ESP_ERR_INVALID_STATE: invalid network state.
 */
esp_err_t esp_zb_secur_network_key_set(uint8_t *key);

/**
 * @brief Get the minimum LQI values for device joining the network
 *
 * @return The minimum LQI values
 */
uint8_t esp_zb_secur_network_min_join_lqi_get(void);

/**
 * @brief Set the minimum LQI value for device joining the network
 *
 * @param[in] lqi The LQI values
 */
void esp_zb_secur_network_min_join_lqi_set(uint8_t lqi);

/**
 * @brief Add the specified pre-configured TC standard link key for the device
 *
 * @param[in] key A pointer to 16-byte pre-configured link key to be set
 * @return
 *      - ESP_OK: Preconfigured TC standard link key added successfully
 *      - ESP_FAIL: Failed to add preconfigured TC standard link key
 */
esp_err_t esp_zb_secur_multi_TC_standard_preconfigure_key_add(uint8_t *key);

/**
 * @brief Remove the specified pre-configured TC standard link key from the device
 *
 * @param[in] key A pointer to 16-byte the pre-configured link key to be removed
 * @return
 *      - ESP_OK: Preconfigured TC standard link key removed successfully
 *      - ESP_FAIL: Failed to remove preconfigured TC standard link key
 */
esp_err_t esp_zb_secur_multi_TC_standard_preconfigure_key_remove(uint8_t *key);

/**
 * @brief Add the specified pre-configured distributed link key for the device
 *
 * @param[in] key A pointer to 16-byte pre-configured distributed link key to be set
 * @return
 *      - ESP_OK: Preconfigured distributed link key added successfully
 *      - ESP_FAIL: Failed to add preconfigured distributed link key
 */
esp_err_t esp_zb_secur_multi_standard_distributed_key_add(uint8_t *key);

/**
 * @brief Remove the specified pre-configured distributed link key from the device
 *
 * @param[in] key A pointer to 16-byte pre-configured distributed link key to be removed
 * @return
 *      - ESP_OK: Preconfigured distributed link key removed successfully
 *      - ESP_FAIL: Failed to remove preconfigured distributed link key
 */
esp_err_t esp_zb_secur_multi_standard_distributed_key_remove(uint8_t *key);

#ifdef __cplusplus
}
#endif
