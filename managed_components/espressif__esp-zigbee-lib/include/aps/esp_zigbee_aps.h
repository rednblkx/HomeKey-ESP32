/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "esp_err.h"
#include "esp_zigbee_type.h"

/**
 * @brief Enumeration for APSDE-DATA address mode
 *
 */
typedef enum {
    ESP_ZB_APS_ADDR_MODE_DST_ADDR_ENDP_NOT_PRESENT   =   0x0,  /*!< DstAddress and DstEndpoint not present,
                                                                    only for APSDE-DATA request and confirm  */
    ESP_ZB_APS_ADDR_MODE_16_GROUP_ENDP_NOT_PRESENT   =   0x1,  /*!< 16-bit group address for DstAddress; DstEndpoint not present */
    ESP_ZB_APS_ADDR_MODE_16_ENDP_PRESENT             =   0x2,  /*!< 16-bit address for DstAddress and DstEndpoint present */
    ESP_ZB_APS_ADDR_MODE_64_ENDP_PRESENT             =   0x3,  /*!< 64-bit extended address for DstAddress and DstEndpoint present */
    ESP_ZB_APS_ADDR_MODE_64_PRESENT_ENDP_NOT_PRESENT =   0x4,  /*!< 64-bit extended address for DstAddress, but DstEndpoint NOT present,
                                                                    only for APSDE indication */
} esp_zb_aps_address_mode_t;

/**
 * @brief Enumeration for APSDE-DATA Request TX options
 *
 */
typedef enum esp_zb_apsde_tx_opt_e {
    ESP_ZB_APSDE_TX_OPT_SECURITY_ENABLED         = 0x01U, /*!< Security enabled transmission */
    ESP_ZB_APSDE_TX_OPT_USE_NWK_KEY_R21OBSOLETE  = 0x02U, /*!< Use NWK key (obsolete) */
    ESP_ZB_APSDE_TX_OPT_NO_LONG_ADDR             = 0x02U, /*!< Extension: do not include long src/dst addresses into NWK hdr  */
    ESP_ZB_APSDE_TX_OPT_ACK_TX                   = 0x04U, /*!< Acknowledged transmission */
    ESP_ZB_APSDE_TX_OPT_FRAG_PERMITTED           = 0x08U, /*!< Fragmentation permitted */
    ESP_ZB_APSDE_TX_OPT_INC_EXT_NONCE            = 0x10U, /*!< Include extended nonce in APS security frame */
} esp_zb_apsde_tx_opt_t;

/**
 * @brief APSDE-DATA.request Parameters
 *
 */
typedef struct esp_zb_apsde_data_req_s {
    uint8_t dst_addr_mode;      /*!< The addressing mode for the destination address used in this primitive and of the APDU to be transferred,
                                     refer to esp_zb_aps_address_mode_t */
    esp_zb_addr_u dst_addr;     /*!< The individual device address or group address of the entity to which the ASDU is being transferred*/
    uint8_t dst_endpoint;       /*!< The number of the individual endpoint of the entity to which the ASDU is being transferred or the broadcast endpoint (0xff).*/
    uint16_t profile_id;        /*!< The identifier of the profile for which this frame is intended. */
    uint16_t cluster_id;        /*!< The identifier of the object for which this frame is intended. */
    uint8_t src_endpoint;       /*!< The individual endpoint of the entity from which the ASDU is being transferred.*/
    uint32_t asdu_length;       /*!< The number of octets comprising the ASDU to be transferred */
    uint8_t *asdu;              /*!< The set of octets comprising the ASDU to be transferred. */
    uint8_t tx_options;         /*!< The transmission options for the ASDU to be transferred, refer to esp_zb_apsde_tx_opt_t */
    bool use_alias;             /*!< The next higher layer may use the UseAlias parameter to request alias usage by NWK layer for the current frame.*/
    uint16_t alias_src_addr;    /*!< The source address to be used for this NSDU. If the use_alias is true */
    int alias_seq_num;          /*!< The sequence number to be used for this NSDU. If the use_alias is true */
    uint8_t radius;             /*!< The distance, in hops, that a transmitted frame will be allowed to travel through the network.*/
} esp_zb_apsde_data_req_t;

/**
 * @brief APSDE-DATA.confirm Parameters
 *
 */
typedef struct esp_zb_apsde_data_confirm_s {
    uint8_t status;           /*!< The status of data confirm. 0: success, otherwise failed */
    uint8_t dst_addr_mode;    /*!< The addressing mode for the destination address used in this primitive and of the APDU to be transferred,
                                   refer to esp_zb_aps_address_mode_t */
    esp_zb_addr_u dst_addr;   /*!< The individual device address or group address of the entity to which the ASDU is being transferred.*/
    uint8_t dst_endpoint;     /*!< The number of the individual endpoint of the entity to which the ASDU is being transferred or the broadcast endpoint (0xff).*/
    uint8_t src_endpoint;     /*!< The individual endpoint of the entity from which the ASDU is being transferred.*/
    int tx_time;              /*!< Reserved */
    uint32_t asdu_length;     /*!< The length of ASDU*/
    uint8_t *asdu;            /*!< Payload */
} esp_zb_apsde_data_confirm_t;

/**
 * @brief APSDE-DATA.indication Parameters
 * 
 */
typedef struct esp_zb_apsde_data_ind_s {
    uint8_t status;             /*!< The status of the incoming frame processing, 0: on success */
    uint8_t dst_addr_mode;      /*!< The addressing mode for the destination address used in this primitive and of the APDU that has been received,
                                     refer to esp_zb_aps_address_mode_t */
    uint16_t dst_short_addr;    /*!< The individual device address or group address to which the ASDU is directed.*/
    uint8_t dst_endpoint;       /*!< The target endpoint on the local entity to which the ASDU is directed.*/
    uint8_t src_addr_mode;      /*!< Reserved, The addressing mode for the source address used in this primitive and of the APDU that has been received.*/
    uint16_t src_short_addr;    /*!< The individual device address of the entity from which the ASDU has been received.*/
    uint8_t src_endpoint;       /*!< The number of the individual endpoint of the entity from which the ASDU has been received.*/
    uint16_t profile_id;        /*!< The identifier of the profile from which this frame originated.*/
    uint16_t cluster_id;        /*!< The identifier of the received object.*/
    uint32_t asdu_length;       /*!< The number of octets comprising the ASDU being indicated by the APSDE.*/
    uint8_t *asdu;              /*!< The set of octets comprising the ASDU being indicated by the APSDE. */
    uint8_t security_status;    /*!< UNSECURED if the ASDU was received without any security. SECURED_NWK_KEY if the received ASDU was secured with the NWK key.*/
    int lqi;                    /*!< The link quality indication delivered by the NLDE.*/
    int rx_time;                /*!< Reserved, a time indication for the received packet based on the local clock */
} esp_zb_apsde_data_ind_t;

/**
 * @brief APSDE data indication application callback
 *
 * @param[in] ind APSDE-DATA.indication
 * @return
 *      - true: The indication has already been handled
 *      - false: The indication has not been handled; it will be processed by the stack.
 *
 */
typedef bool (* esp_zb_apsde_data_indication_callback_t)(esp_zb_apsde_data_ind_t ind);

/**
 * @brief APSDE data confirm application callback
 *
 * @param[in] confirm APSDE-DATA.confirm
 */
typedef void (* esp_zb_apsde_data_confirm_callback_t)(esp_zb_apsde_data_confirm_t confirm);

/**
 * @brief Register the callback for retrieving the aps data indication
 *
 * @param[in] cb A function pointer for esp_zb_apsde_data_indication_callback_t
 */
void esp_zb_aps_data_indication_handler_register(esp_zb_apsde_data_indication_callback_t cb);

/**
 * @brief APS data request
 *
 * @param[in] req A pointer for apsde data request, @ref esp_zb_apsde_data_req_s
 * @return
 *      - ESP_OK: on success
 *      - ESP_ERR_NO_MEM: not memory
 *      - ESP_FAIL: on failed
 */
esp_err_t esp_zb_aps_data_request(esp_zb_apsde_data_req_t *req);

/**
 * @brief Register the callback for retrieving the aps data confirm
 *
 * @note If the callback is registered by the application, the application is responsible for handling APSDE confirm.
 * @param[in] cb A function pointer for esp_zb_apsde_data_confirm_callback_t
 */
void esp_zb_aps_data_confirm_handler_register(esp_zb_apsde_data_confirm_callback_t cb);

/**
 * @brief Set the APS trust center address
 *
 * @param[in] address A 64-bit value is expected to be set to trust center address
 * @return
 *      -   ESP_OK: On success
 *      -   ESP_ERR_INVALID_STATE: Device is already on a network
 */
esp_err_t esp_zb_aps_set_trust_center_address(esp_zb_ieee_addr_t address);

/**
 * @brief Get the APS trust center address
 *
 * @param[out] address A 64-bit value will be assigned from the trust center address
 */
void esp_zb_aps_get_trust_center_address(esp_zb_ieee_addr_t address);

#ifdef __cplusplus
}
#endif
