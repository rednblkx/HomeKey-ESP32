/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once
#ifdef __cplusplus
extern "C" {
#endif

#include "esp_err.h"
#include "esp_zigbee_type.h"

/** Enum of the Zigbee network device type
 * @anchor esp_zb_nwk_device_type_t
 */
typedef enum {
    ESP_ZB_DEVICE_TYPE_COORDINATOR = 0x0,       /*!<  Device - Coordinator */
    ESP_ZB_DEVICE_TYPE_ROUTER  = 0x1,           /*!<  Device - Router */
    ESP_ZB_DEVICE_TYPE_ED = 0x2,                /*!<  Device - End device */
    ESP_ZB_DEVICE_TYPE_NONE = 0x3,              /*!<  Unknown Device */
} esp_zb_nwk_device_type_t;

#define ESP_ZB_NWK_INFO_ITERATOR_INIT 0         /*!< Initializer for esp_zb_neighbor_info_iterator_t. */
#define ESP_ZB_NWK_INFO_ITERATOR_EOT  0xFFFF    /*!< Indicate the iterator reach the End of Table. */

#define ESP_ZB_NWK_MAX_SOURCE_ROUTE 5

/**
 * @brief Iterator used to iterate through the tables of network informations.
 *
 */
typedef uint16_t esp_zb_nwk_info_iterator_t;

/**
 * @brief Relationship of network neighbor
 * @anchor esp_zb_nwk_relationship_t
 */
typedef enum {
    ESP_ZB_NWK_RELATIONSHIP_PARENT                = 0U, /*!< The peer device is the parent of current device. */
    ESP_ZB_NWK_RELATIONSHIP_CHILD                 = 1U, /*!< The peer device is the child of current device. */
    ESP_ZB_NWK_RELATIONSHIP_SIBLING               = 2U, /*!< The peer device is the sibling of current device. */
    ESP_ZB_NWK_RELATIONSHIP_NONE_OF_THE_ABOVE     = 3U, /*!< The relationship is none of above, means that the peer device is
                                                             currently unknown, and its relationship with the network is in
                                                             the process of being established. */
    ESP_ZB_NWK_RELATIONSHIP_PREVIOUS_CHILD        = 4U, /*!< The peer device is the previous child of current device, meaning it
                                                             has been confirmed to have left the network */
    ESP_ZB_NWK_RELATIONSHIP_UNAUTHENTICATED_CHILD = 5U, /*!< The peer device is the unauthenticated child of current device,
                                                             meaning it is in the process of joining the network but has not yet
                                                             been authenticated. */
} esp_zb_nwk_relationship_t;

/**
 * @brief Information of network neighbor table entry
 *
 */
typedef struct esp_zb_nwk_neighbor_info_s {
    esp_zb_ieee_addr_t ieee_addr; /*!< Long address (EUI64) of the device. */
    uint16_t short_addr;          /*!< Short address (network address) of the device. */
    uint8_t device_type;          /*!< Neighbor device type, refer to @ref esp_zb_nwk_device_type_t */
    uint8_t depth;                /*!< The network depth of this device.*/
    uint8_t rx_on_when_idle;      /*!< Indicates if neighbour receiver enabled during idle periods:*/
    uint8_t relationship;         /*!< The relationship between the neighbour and current device, refer to @ref esp_zb_nwk_relationship_t */
    uint8_t lqi;                  /*!< Link quality. Also used to calculate incoming cost */
    int8_t rssi;                  /*!< Received signal strength indicator */
    uint8_t outgoing_cost;        /*!< The cost of an outgoing link. Got from link status. */
    uint8_t age;                  /*!< Counter value for router aging. The number of nwkLinkStatusPeriod
                                   * intervals since a link status command was received. */
    uint32_t device_timeout;      /*!< Configured end device timeout, in seconds. */
    uint32_t timeout_counter;     /*!< Timeout value ED aging, in milliseconds. */
} esp_zb_nwk_neighbor_info_t;

/**
 * @brief State of the network route
 * @anchor esp_zb_nwk_route_state_t
 */
typedef enum {
    ESP_ZB_NWK_ROUTE_STATE_ACTIVE               = 0U, /*!< The route is active. */
    ESP_ZB_NWK_ROUTE_STATE_DISCOVERY_UNDERWAY   = 1U, /*!< The route is under discovery process. */
    ESP_ZB_NWK_ROUTE_STATE_DISCOVERY_FAILED     = 2U, /*!< The discovery process failed of this route. */
    ESP_ZB_NWK_ROUTE_STATE_INACTIVE             = 3U, /*!< The route is inactive and under validation. */
} esp_zb_nwk_route_state_t;

/**
 * @brief Information of network routing table entry
 *
 */
typedef struct esp_zb_nwk_route_info_s {
    uint16_t dest_addr;     /*!< 16-bit network address of the destination. */
    uint16_t next_hop_addr; /*!< 16-bit network address of the next hop on the way to the destination. */
    struct {
        uint8_t status                : 3;  /*!< The status of the route, bit 0 - 2, refer to esp_zb_nwk_route_state_t */
        uint8_t no_route_cache        : 1;  /*!< Dest does not store source routes, bit 3 */
        uint8_t many_to_one           : 1;  /*!< Dest is the concentrator and many-to-one request was used, bit 4 */
        uint8_t route_record_required : 1;  /*!< Route record command frame should be sent
                                             *   to the dest prior to the next data packet, bit 5 */
        uint8_t group_id              : 1;  /*!< Indicates that dest_addr is a Group ID, bit 6 */
        uint8_t /* reserved */        : 2;  /*!< Reserved, bit 7 */
    } flags;                /*!< Flags in the routing table entry */
    uint8_t expiry;         /*!< Expiration time. */
} esp_zb_nwk_route_info_t;

/**
 * @brief Information of network route record table entry
 *
 */
typedef struct esp_zb_nwk_route_record_info_s {
    uint16_t dest_address;                      /*!< Destination network address of this route record. */
    uint8_t expiry;                             /*!< Expiration time. */
    uint8_t relay_count;                        /*!< The count of relay nodes from concentrator to the destination. */
    uint16_t path[ESP_ZB_NWK_MAX_SOURCE_ROUTE]; /*!< The set of network addresses that represent the route
                                                 *   in order from the concentrator to the destination.*/
} esp_zb_nwk_route_record_info_t;

/**
 * @brief Set the network update id
 *
 * @param[in] id The network update id is expected to be set
 * @return
 *      -   ESP_OK: On success
 *      -   ESP_ERR_INVALID_STATE: Device is already on a network
 */
esp_err_t esp_zb_nwk_set_update_id(uint8_t id);

/**
 * @brief Get the network update id
 *
 * @return The network update id
 */
uint8_t esp_zb_nwk_get_update_id(void);

/**
 * @brief Set the network outgoing frame counter
 *
 * @param[in] frame_counter The network frame counter value is expect to be set
 * @return
 *      -   ESP_OK: On success
 *      -   ESP_ERR_INVALID_STATE: Device is already on a network
 */
esp_err_t esp_zb_nwk_set_frame_counter(uint32_t frame_counter);

/**
 * @brief Get the current network outgoing frame counter
 *
 * @return The network outgoing frame counter
 */
uint32_t esp_zb_nwk_get_frame_counter(void);

/**
 * @brief Set zigbee rx on when idle.
 *
 * @param[in] rx_on enable/disable rx on when idle.
 *
 */
void esp_zb_set_rx_on_when_idle(bool rx_on);

/**
 * @brief Get the current state of RX-ON-When-Idle
 *
 * @return The state of RX-ON-When-Idle
 */
bool esp_zb_get_rx_on_when_idle(void);

/**
 * @brief Set the maximum number of children allowed.
 *
 * The function only takes effect on ZC/ZR.
 *
 * @param[in] max_children Maximum number of children.
 *
 * @return
 *      - ESP_OK: On success
 *      - ESP_ERR_NOT_SUPPORTED: for ZED.
 *
 */
esp_err_t esp_zb_nwk_set_max_children(uint8_t max_children);

/**
 * @brief Get the maximum number of children allowed.
 *
 * @return
 *      - Maximum number of children, for ZC/ZR
 *      - 0, for ZED
 *
 */
uint8_t esp_zb_nwk_get_max_children(void);

/**
 * @brief   Set the Zigbee device long address.
 *
 * @note  Set this function AFTER @ref esp_zb_init called, if user wants to set specific address
 * without reading MAC address from flash refer to tools/mfg_tool or eFUSE.
 *
 * @param[in] addr An 64-bit of IEEE long address, which is presented in little-endian.
 * @return
 *      -   ESP_OK: On success
 *      -   ESP_ERR_INVALID_STATE: Device is already on a network
 */
esp_err_t esp_zb_set_long_address(esp_zb_ieee_addr_t addr);

/**
 * @brief   Get the Zigbee device long address.
 *
 * @note This function will return a pointer to 64-bit of ieee long address.
 *
 * @param[out] addr An 64-bit of IEEE long address, which is presented in little-endian.
 *
 */
void esp_zb_get_long_address(esp_zb_ieee_addr_t addr);

/**
 * @brief   Get the Zigbee device short address.
 *
 * @return 16-bit Zigbee short address
 *
 */
uint16_t esp_zb_get_short_address(void);

/**
 * @brief Set the Zigbee network extended PAN ID.
 *
 * @param ext_pan_id An 64-bit of extended PAN ID, which is presented in little-endian.
 */
void esp_zb_set_extended_pan_id(const esp_zb_ieee_addr_t ext_pan_id);

/**
 * @brief   Get the Zigbee network extended PAN ID.
 *
 * @note This function will return back a pointer to 64-bit of extended PAN ID.
 *
 * @param[out] ext_pan_id An 64-bit of extended PAN ID, which is presented in little-endian.
 *
 */
void esp_zb_get_extended_pan_id(esp_zb_ieee_addr_t ext_pan_id);

/**
 * @brief   Set the Zigbee network PAN ID.
 *
 * @note The PAN ID will be set from the network PIB to the IEEE802154 PIB
 * @param[in] pan_id 16-bit Zigbee network PAN ID
 *
 */
void esp_zb_set_pan_id(uint16_t pan_id);

/**
 * @brief   Get the Zigbee network PAN ID.
 *
 * @return 16-bit Zigbee network PAN ID
 *
 */
uint16_t esp_zb_get_pan_id(void);

/**
 * @brief   Get the currently used channel.
 * @return  8-bit Zigbee network channel number
 */
uint8_t esp_zb_get_current_channel(void);

/**
 * @brief  Get the network short address by the IEEE address
 *
 * @param[in] address An 64-bit for the IEEE address, which is presented in little-endian.
 * @return Network short address
 *
 */
uint16_t esp_zb_address_short_by_ieee(esp_zb_ieee_addr_t address);

/**
 * @brief Get the network IEEE address by the short address
 *
 * @param[in] short_addr The 2-byte address which will been used to search the mapped IEEE address
 * @param[out] ieee_addr The 64-bit of address for Zigbee IEEE address, which is presented in little-endian.
 * @return
 *      - ESP_OK: on success
 *      - ESP_ERR_NOT_FOUND: not found the IEEE address
 */
esp_err_t esp_zb_ieee_address_by_short(uint16_t short_addr, uint8_t *ieee_addr);

/**
 * @brief Set the Zigbee network device role
 *
 * @param[in] role The network device role is expected to be set
 * @return
 *      -   ESP_OK: On success
 *      -   ESP_ERR_INVALID_STATE: Device is already on a network
 *      -   ESP_ERR_INVALID_ARG: Invalid device role
 */
esp_err_t esp_zb_set_network_device_role(esp_zb_nwk_device_type_t role);

/**
 * @brief   Get the Zigbee network device type.
 *
 * @return device type @ref esp_zb_nwk_device_type_t
 *
 */
esp_zb_nwk_device_type_t esp_zb_get_network_device_role(void);

/**
 * @brief  Iterate through the neighbor table and get information about active neighbor
 *
 * @param[in] iterator iterator used to iterate through neighbor table, refer to esp_zb_nwk_info_iterator_t
 * @param[out] nbr_info next neighbor information, @ref esp_zb_nwk_neighbor_info_s
 *
 * @return - ESP_OK on success
 *         - ESP_ERR_NOT_FOUND on finish iteration
 *         - ESP_ERR_INVALID_ARG if arguements are invalid
 *
 */
esp_err_t esp_zb_nwk_get_next_neighbor(esp_zb_nwk_info_iterator_t *iterator, esp_zb_nwk_neighbor_info_t *nbr_info);

/**
 * @brief  Iterate through the routing table and get the information in the entry
 *
 * @param[in] iterator iterator used to iterate through routing table, refer to esp_zb_nwk_info_iterator_t
 * @param[out] route_info next route entry information, @ref esp_zb_nwk_route_info_s
 *
 * @return - ESP_OK on success
 *         - ESP_ERR_NOT_FOUND on finish iteration
 *         - ESP_ERR_INVALID_ARG if arguements are invalid
 *
 */
esp_err_t esp_zb_nwk_get_next_route(esp_zb_nwk_info_iterator_t *iterator, esp_zb_nwk_route_info_t *route_info);

/**
 * @brief  Iterate through the route record table (a.k.a source route table) and get the information in the entry
 *
 * @param[in] iterator iterator used to iterate through routing table, refer to esp_zb_nwk_info_iterator_t
 * @param[out] route_record_info next route record entry information, @ref esp_zb_nwk_route_record_info_s
 *
 * @return - ESP_OK on success
 *         - ESP_ERR_NOT_FOUND on finish iteration
 *         - ESP_ERR_INVALID_ARG if arguements are invalid
 *
 */
esp_err_t esp_zb_nwk_get_next_route_record(esp_zb_nwk_info_iterator_t *iterator, esp_zb_nwk_route_record_info_t *route_record_info);

#ifdef __cplusplus
}
#endif
