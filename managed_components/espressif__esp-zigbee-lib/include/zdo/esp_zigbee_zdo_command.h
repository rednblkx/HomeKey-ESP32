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
#include "esp_zigbee_zdo_common.h"

/* MATCH DESC REQ ZCL configuration */
#define ESP_ZB_MATCH_DESC_REQ_TIMEOUT              (5 * ESP_ZB_TIME_ONE_SECOND)            /* timeout for finding */
#define ESP_ZB_MATCH_DESC_REQ_ROLE                 ESP_ZB_NWK_BROADCAST_RX_ON_WHEN_IDLE    /* find non-sleep Zigbee device, 0xFFFD */

#define ESP_ZB_IEEE_ADDR_REQ_TIMEOUT                (5 * ESP_ZB_TIME_ONE_SECOND)            /* timeout for ieee address request */
#define ESP_ZB_NWK_ADDR_REQ_TIMEOUT                 (5 * ESP_ZB_TIME_ONE_SECOND)            /* timeout for nwk address request */
#define ESP_ZB_NODE_DESC_REQ_TIMEOUT                (5 * ESP_ZB_TIME_ONE_SECOND)            /* timeout for node descriptor request */
#define ESP_ZB_BIND_DEVICE_REQ_TIMEOUT              (5 * ESP_ZB_TIME_ONE_SECOND)            /* timeout for binding request */
#define ESP_ZB_ACTIVE_EP_REQ_TIMEOUT                (5 * ESP_ZB_TIME_ONE_SECOND)            /* timeout for active endpoint request */
#define ESP_ZB_SIMPLE_DESC_REQ_TIMEOUT              (5 * ESP_ZB_TIME_ONE_SECOND)            /* timeout for simple descriptor request */
#define ESP_ZB_PERMIT_JOIN_REQ_TIMEOUT              (5 * ESP_ZB_TIME_ONE_SECOND)            /* timeout for permit join request */
#define ESP_ZB_DEVICE_LEAVE_REQ_TIMEOUT             (5 * ESP_ZB_TIME_ONE_SECOND)            /* timeout for device leave request */
#define ESP_ZB_DEVICE_BIND_TABLE_REQ_TIMEOUT        (5 * ESP_ZB_TIME_ONE_SECOND)            /* timeout for device bind table request */
#define ESP_ZB_DEVICE_MGMT_LQI_REQ_TIMEOUT          (5 * ESP_ZB_TIME_ONE_SECOND)            /* timeout for zdo mgmt lqi request */

/**
 * @brief The network address list of assocaited devices.
 *
 */
typedef struct esp_zb_zdo_nwk_addr_list_s {
    uint8_t start_index;        /*!< Starting index into the list of associated devices for this report. */
    uint8_t total;              /*!< Count of the number of 16-bit short addresses to follow.*/
    uint8_t count;              /*!< Number of short addresses in the list. */
    uint16_t *nwk_addresses;    /*!< Array of network address. */
} esp_zb_zdo_nwk_addr_list_t;

/**
 * @brief The Zigbee ZDO nwk_addr response struct.
 *
 */
typedef struct esp_zb_zdo_nwk_addr_rsp_s {
    esp_zb_ieee_addr_t ieee_addr;           /*!< 64-bit address for the Remote Device. */
    uint16_t nwk_addr;                      /*!< 16-bit address for the Remote Device. */
    esp_zb_zdo_nwk_addr_list_t *ext_resp;   /*!< Extended response: network address of assosicated devices.
                                                 This field only existed when the request was sent with RequestType = 1. */
} esp_zb_zdo_nwk_addr_rsp_t;

/**
 * @brief The Zigbee ZDO ieee_addr response struct.
 *
 * @anchor esp_zb_zdo_ieee_addr_rsp_t
 */
typedef esp_zb_zdo_nwk_addr_rsp_t esp_zb_zdo_ieee_addr_rsp_t;

/** Find device callback
 *
 * @brief A ZDO match desc request callback for user to get response info.
 *
 * @note User's callback get response from the remote device that local node wants to find a particular device on endpoint.
 *
 * @param[in] zdo_status  The ZDO response status, refer to `esp_zb_zdp_status`
 * @param[in] addr  A short address of the device response, 0xFFFF - invalid address
 * @param[in] endpoint An endpoint of the device response, 0xFF - invalid endpoint
 * @param[in] user_ctx  User information context, set in `esp_zb_zdo_find_xxx()`
 *
 */
typedef void (*esp_zb_zdo_match_desc_callback_t)(esp_zb_zdp_status_t zdo_status, uint16_t addr, uint8_t endpoint, void *user_ctx);

/** IEEE address request callback
 *
 * @brief A ZDO ieee address request callback for user to get response info.
 *
 * @note User's callback get response from the remote device that local node wants to get ieee address.
 *
 * @param[in] zdo_status The ZDO response status, refer to `esp_zb_zdp_status`
 * @param[in] resp       The response of ieee address request, see @ref esp_zb_zdo_ieee_addr_rsp_t.
 * @param[in] user_ctx   User information context, set in `esp_zb_zdo_ieee_addr_req()`
 *
 */
typedef void (*esp_zb_zdo_ieee_addr_callback_t)(esp_zb_zdp_status_t zdo_status, esp_zb_zdo_ieee_addr_rsp_t *resp, void *user_ctx);

/** Network address request callback
 *
 * @brief A ZDO network address request callback for user to get response info.
 *
 * @note User's callback gets response from the remote device that local node wants to get network address.
 *
 * @param[in] zdo_status The ZDO response status, refer to `esp_zb_zdp_status`
 * @param[in] resp       The response of network address request, see @ref esp_zb_zdo_nwk_addr_rsp_s.
 * @param[in] user_ctx   User information context, set in `esp_zb_zdo_nwk_addr_req()`
 *
 */
typedef void (*esp_zb_zdo_nwk_addr_callback_t)(esp_zb_zdp_status_t zdo_status, esp_zb_zdo_nwk_addr_rsp_t *resp, void *user_ctx);

/** Node descriptor callback
 *
 * @brief A ZDO Node descriptor request callback for user to get node desc info.
 *
 * @note User's callback get response from the remote device that local node wants to get node descriptor response.
 *
 * @param[in] zdo_status    The ZDO response status, refer to `esp_zb_zdp_status`
 * @param[in] addr          A short address of the device response, 0xFFFF - invalid address
 * @param[in] node_desc     A pointer to the node desc @ref esp_zb_af_node_desc_s
 * @param[in] user_ctx  User information context, set in `esp_zb_zdo_node_desc_req()`
 *
 */
typedef void (*esp_zb_zdo_node_desc_callback_t)(esp_zb_zdp_status_t zdo_status, uint16_t addr, esp_zb_af_node_desc_t *node_desc, void *user_ctx);

/** Bind request callback
 *
 * @brief A ZDO bind request callback for user to get response info.
 *
 * @note user's callback get response from the remote device that local node wants to bind.
 *
 * @param[in] zdo_status The ZDO response status, refer to `esp_zb_zdp_status`
 * @param[in] user_ctx  User information context, set in `esp_zb_zdo_device_bind_req()`
 *
 */
typedef void (*esp_zb_zdo_bind_callback_t)(esp_zb_zdp_status_t zdo_status, void *user_ctx);

/** Active endpoint callback
 *
 * @brief A ZDO active endpoint request callback for user to get response info.
 *
 * @note User's callback get response from the remote device that local node wants to get active endpoint.
 *
 * @param[in] zdo_status The ZDO response status, refer to `esp_zb_zdp_status`
 * @param[in] ep_count  A number of active endpoint
 * @param[in] ep_id_list A pointer of the endpoint id list
 * @param[in] user_ctx  User information context, set in `esp_zb_zdo_active_ep_req()`
 *
 */
typedef void (*esp_zb_zdo_active_ep_callback_t)(esp_zb_zdp_status_t zdo_status, uint8_t ep_count, uint8_t *ep_id_list, void *user_ctx);

/** Simple descriptor callback
 *
 * @brief A ZDO simple descriptor request callback for user to get simple desc info.
 *
 * @note User's callback get response from the remote device that local node wants to get simple desc.
 *
 * @param[in] zdo_status The ZDO response status, refer to `esp_zb_zdp_status`
 * @param[in] simple_desc  A pointer to the simple desc @ref esp_zb_af_simple_desc_1_1_s
 * @param[in] user_ctx  User information context, set in `esp_zb_zdo_simple_desc_req()`
 *
 */
typedef void (*esp_zb_zdo_simple_desc_callback_t)(esp_zb_zdp_status_t zdo_status, esp_zb_af_simple_desc_1_1_t *simple_desc, void *user_ctx);

/** Permit join request callback
 *
 * @brief A ZDO permit join request callback for user to get permit join response info.
 *
 * @note User's callback get response from the node requested to permit join.
 *
 * @param[in] zdo_status The ZDO response status, refer to `esp_zb_zdp_status`
 * @param[in] user_ctx  User information context, set in `esp_zb_zdo_permit_joining_req()`
 *
 */
typedef void (*esp_zb_zdo_permit_join_callback_t)(esp_zb_zdp_status_t zdo_status, void *user_ctx);

/** Leave request callback
 *
 * @brief A ZDO leave request callback for user to get leave status.
 *
 * @note User's callback get response from the device that wants to leave.
 *
 * @param[in] zdo_status The ZDO response status, refer to `esp_zb_zdp_status`
 * @param[in] user_ctx  User information context, set in `esp_zb_zdo_device_leave_req()`
 *
 */
typedef void (*esp_zb_zdo_leave_callback_t)(esp_zb_zdp_status_t zdo_status, void *user_ctx);

/**
 * @brief The Zigbee ZDO bind command struct
 *
 * @note Current implementation of the API ONLY supports 64 bit extended address's address mode. Other address mode will support later.
 *
 * @note Be aware of the one req_dst_addr is address that command send to, while dst_address is the destination of the binding entry.
 *
 * @note NOW the dst_addr_mode is default by ZB_BIND_DST_ADDR_MODE_64_BIT_EXTENDED. Later SDK will support other address mode.
 *
 */
typedef struct esp_zb_zdo_bind_req_param_s {
    esp_zb_ieee_addr_t src_address;                     /*!< The IEEE address for the source */
    uint8_t src_endp;                                   /*!< The source endpoint for the binding entry */
    uint16_t cluster_id;                                /*!< The identifier of the cluster on the source device that is bound to the destination */
    uint8_t dst_addr_mode;                              /*!< The destination address mode ref to esp_zb_zdo_bind_dst_addr_mode_t*/
    esp_zb_addr_u dst_address_u;                        /*!< The destination address for the binding entry */
    uint8_t dst_endp;                                   /*!< This field shall be present only if the
                                                         * DstAddrMode field has a value of
                                                         * refer to ZB_BIND_DST_ADDR_MODE_64_BIT_EXTENDED refer to bind_dst_addr_mode
                                                         * and, if present, shall be the
                                                         * destination endpoint for the binding entry. */
    uint16_t req_dst_addr;                              /*!< Destination address of the request send to */
} esp_zb_zdo_bind_req_param_t;

/**
 * @brief The Zigbee ZDO match descriptor command struct
 */
typedef struct esp_zb_zdo_match_desc_req_param_s {
    uint16_t dst_nwk_addr;              /*!< NWK address that request sent to */
    uint16_t addr_of_interest;          /*!< NWK address of interest */
    uint16_t profile_id;                /*!< Profile ID to be match at the destination which refers to esp_zb_af_profile_id_t */
    uint8_t num_in_clusters;            /*!< The number of input clusters provided for matching cluster server */
    uint8_t num_out_clusters;           /*!< The number of output clusters provided for matching cluster client */
    uint16_t *cluster_list;             /*!< The pointer MUST point the uint16_t object with a size equal to num_in_clusters + num_out_clusters,
                                         * which will be used to match device. */
} esp_zb_zdo_match_desc_req_param_t;

/**
 * @brief The Zigbee ZDO ieee_addr request command struct
 *
 */
typedef struct esp_zb_zdo_ieee_addr_req_param_s {
    uint16_t   dst_nwk_addr;                            /*!< NWK address that request sent to */
    uint16_t   addr_of_interest;                        /*!< NWK address of interest */
    uint8_t    request_type;                            /*!< Request type for this command: 0x00  Single device response 0x01 Extended response  */
    uint8_t    start_index;                             /*!< If the Request type for this command is Extended response, the StartIndex provides the
                                                         * starting index for the requested elements of the associated devices list */
} esp_zb_zdo_ieee_addr_req_param_t;

/**
 * @brief The Zigbee ZDO nwk_addr request command struct
 *
 */
typedef struct esp_zb_zdo_nwk_addr_req_param_s {
    uint16_t   dst_nwk_addr;                            /*!< NWK address that request sent to */
    esp_zb_ieee_addr_t ieee_addr_of_interest;           /*!< IEEE address to be matched by the remote device */
    uint8_t    request_type;                            /*!< Request type for this command: 0x00  Single device response 0x01 Extended response  */
    uint8_t    start_index;                             /*!< If the Request type for this command is Extended response, the StartIndex provides the
                                                         * starting index for the requested elements of the associated devices list */
} esp_zb_zdo_nwk_addr_req_param_t;

/**
 * @brief The Zigbee ZDO node descriptor command struct
 *
 */
typedef struct esp_zb_zdo_node_desc_req_param_s {
    uint16_t    dst_nwk_addr;                           /*!< NWK address that is used for IEEE address mapping.  */
} esp_zb_zdo_node_desc_req_param_t;

/**
 * @brief The Zigbee ZDO simple descriptor command struct
 *
 */
typedef struct esp_zb_zdo_simple_desc_req_param_s {
    uint16_t   addr_of_interest;                        /*!< NWK address of interest */
    uint8_t    endpoint;                                /*!< Endpoint of interest */
} esp_zb_zdo_simple_desc_req_param_t;

/**
 * @brief The Zigbee ZDO active endpoint command struct
 *
 */
typedef struct esp_zb_zdo_active_ep_req_param_s {
    uint16_t   addr_of_interest;                        /*!< NWK address of interest */
} esp_zb_zdo_active_ep_req_param_t;

/**
 * @brief The Zigbee ZDO permit join command struct
 *
 */
typedef struct esp_zb_zdo_permit_joining_req_param_s {
    uint16_t   dst_nwk_addr;                            /*!< NWK address that request sent to */
    uint8_t permit_duration;                            /*!< The length of time in seconds. 0x00 and 0xff indicate that permission is disabled or enabled */
    uint8_t tc_significance;                            /*!< If this is set to 0x01 and the remote device is the Trust Center, the command affects the Trust Center
                                                         * authentication policy as described in the sub-clauses below; If this is set to 0x00, there is no effect
                                                         * on the Trust Center */
} esp_zb_zdo_permit_joining_req_param_t;

/**
 * @brief The Zigbee ZDO leave command struct
 *
 */
typedef struct esp_zb_zdo_mgmt_leave_req_param_s {
    esp_zb_ieee_addr_t device_address;                  /*!< 64-bit IEEE address on device */
    uint16_t   dst_nwk_addr;                            /*!< NWK address that request sent to */
    unsigned int reserved: 6;                           /*!< Reserved */
    unsigned int remove_children: 1;                    /*!< Bitfield of remove children or not */
    unsigned int rejoin: 1;                             /*!< Bitfield of rejoin or not */
} esp_zb_zdo_mgmt_leave_req_param_t;

/**
 * @brief The Zigbee ZDO binding table request struct
 *
 */
typedef struct esp_zb_zdo_mgmt_bind_param_s {
    uint8_t start_index;                /*!< The starting index for the requested elements of the Binding Table */
    uint16_t dst_addr;                  /*!< The destination address */
} esp_zb_zdo_mgmt_bind_param_t;

/**
 * @brief The Zigbee ZDO binding table record struct
 *
 */
typedef struct esp_zb_zdo_binding_table_record_s {
    esp_zb_ieee_addr_t src_address;                     /*!< The source IEEE address for the binding entry. */
    uint8_t src_endp;                                   /*!< The source endpoint for the binding entry. */
    uint16_t cluster_id;                                /*!< The identifier of the cluster on the source device that is bound to the destination device. */
    uint8_t dst_addr_mode;                              /*!< Destination address mode @ref esp_zb_zdo_bind_dst_addr_mode_t */
    esp_zb_addr_u dst_address;                          /*!< The destination address for the binding entry.16 or 64 bit. As specified by the dst_addr_mode field.*/
    uint8_t dst_endp;                                   /*!< The destination endpoint for binding entry, this field shall be present only if the DstAddrMode field has a value of 0x03
                                                            refer to esp_zb_zdo_bind_dst_addr_mode_t */
    struct esp_zb_zdo_binding_table_record_s *next;     /*!< The next binding table record */
} esp_zb_zdo_binding_table_record_t;

/**
 * @brief The Zigbee ZDO binding table information struct for users
 *
 */
typedef struct esp_zb_zdo_binding_table_info_s {
    uint8_t status;                             /*!< The status of binding table information refer to esp_zb_zdp_status */
    uint8_t index;                              /*!< The index of binding table record */
    uint8_t total;                              /*!< The total number of records in the binding table for requests */
    uint8_t count;                              /*!< The number of binding table records in the response */
    esp_zb_zdo_binding_table_record_t *record;  /*!< The binding table record list */
} esp_zb_zdo_binding_table_info_t;

/**
* @brief Struture of network descriptor request of active scan response
*/
typedef struct esp_zb_network_descriptor_s{
    uint16_t short_pan_id;                      /*!< PAN id */
    bool     permit_joining;                    /*!< Indicates that at least one router / coordinator on the network currently permits joining */
    esp_zb_ieee_addr_t extended_pan_id;         /*!< Extended PAN id, the MAC address which forms the network */
    uint8_t logic_channel;                      /*!< The current logical channel occupied by the network. */
    bool router_capacity;                       /*!< This value is set to true if the device is capable of accepting join requests from router-capable
                                                     devices and set to FALSE otherwise. */
    bool end_device_capacity;                   /*!< This value is set to true if the device is capable of accepting join requests from end devices
                                                     and set to FALSE otherwise.*/
} esp_zb_network_descriptor_t;

/**
 * @brief Channel information of Energy Detect
 *
 */
typedef struct esp_zb_energy_detect_channel_info_s {
    uint8_t channel_number;     /*!< The channel of energy detect */
    int8_t energy_detected;     /*!< The energy value of channel in dbm */
} esp_zb_energy_detect_channel_info_t;

/**
 * @brief Structure of Zigbee ZDO Mgmt_Lqi_req command
 *
 */
typedef struct esp_zb_zdo_mgmt_lqi_req_param_s {
    uint8_t start_index;                /*!< Starting Index for the requested elements of the Neighbor Table */
    uint16_t dst_addr;                  /*!< The destination network short address of request */
} esp_zb_zdo_mgmt_lqi_req_param_t;

/**
 * @brief Structure of neighbor table list record for Zigbee ZDO Mgmt_Lqi_rsp
 *
 */
typedef struct esp_zb_zdo_neighbor_table_list_record_s {
  esp_zb_ext_pan_id_t extended_pan_id;  /*!< 64-bit extended pan id of the neighboring device */
  esp_zb_ieee_addr_t  extended_addr;    /*!< 64-bit IEEE address that is unique to every device */
  uint16_t network_addr;                /*!< The 16-bit network address of the neighboring device */
  uint8_t device_type:2;                /*!< The type of the neighbor device @see esp_zb_nwk_device_type_t */
  uint8_t rx_when_idle:2;               /*!< Indicates if neighbor's receiver is enabled during idle portions of the CAP
                                             0x00 = Receiver is off
                                             0x01 = Receiver is on
                                             0x02 = Unknown */
  uint8_t relationship:3;               /*!< The relationship between the neighbor and the current device,
                                            @see esp_zb_nwk_relationship_t */
  uint8_t reserved:1;                   /*!< This reserved bit shall be set to 0 */
  uint8_t permit_join;                  /*!< An indication of whether the neighbor device is accepting join requests */
  uint8_t depth;                        /*!< The tree depth of the neighbor device. A value of 0x00 indicates that the
                                             device is the ZigBee coordinator for the network */
  uint8_t lqi;                          /*!< The estimated link quality for RF transmissions from this device */
} esp_zb_zdo_neighbor_table_list_record_t;

/**
 * @brief Structure of Zigbee ZDO Mgmt_Lqi_rsp
 *
 */
typedef struct esp_zb_zdo_mgmt_lqi_rsp_s {
    uint8_t status;                                               /*!< The status of the Mgmt_Lqi_req command */
    uint8_t neighbor_table_entries;                               /*!< Total number of Neighbor Table entries within the Remote Device. */
    uint8_t start_index;                                          /*!< Starting index within the Neighbor Table to begin reporting for the
                                                                       NeighborTableList.*/
    uint8_t neighbor_table_list_count;                            /*!< Number of Neighbor Table entries included within NeighborTableList. */
    esp_zb_zdo_neighbor_table_list_record_t *neighbor_table_list; /*!< A list of descriptors, beginning with the StartIndex element and
                                                                       continuing for neighbor_table_list_count, of the elements in the
                                                                       Remote Device's Neighbor Table */
} esp_zb_zdo_mgmt_lqi_rsp_t;

/** Active scan network callback
 *
 * @brief A ZDO active scan request callback for user to get scan list status.
 *
 * @note User's callback get response from the device that found in network.
 *
 * @param[in] zdo_status The ZDO response status, refer to esp_zb_zdp_status_t
 * @param[in] count     Number of discovered networks @p nwk_descriptor
 * @param[in] nwk_descriptor The pointer to all discovered networks see refer to esp_zb_network_descriptor_t
 *
 */
typedef void (*esp_zb_zdo_scan_complete_callback_t)(esp_zb_zdp_status_t zdo_status, uint8_t count,
                                                    esp_zb_network_descriptor_t *nwk_descriptor);

/**
 * @brief ZDO energy detect callback
 *
 * @param[in] status  The status of callback, refer to esp_zb_zdp_status_t
 * @param[in] count   The size of energy detect list
 * @param[in] ed_list The list of energy detect information, refer to esp_zb_energy_detect_channel_info_t
 *
 */
typedef void (*esp_zb_zdo_energy_detect_callback_t)(esp_zb_zdp_status_t status, uint16_t count,
                                                    esp_zb_energy_detect_channel_info_t *channel_info);

/** Binding table request callback
 *
 * @brief A ZDO binding table request callback for user to get the binding table record of remote device.
 *
 * @param[in] table_info The binding table record which is only accessed by read, refer to esp_zb_zdo_binding_table_info_s
 * @param[in] user_ctx   User information context, set in `esp_zb_zdo_binding_table_req()`
 *
 */
typedef void (*esp_zb_zdo_binding_table_callback_t)(const esp_zb_zdo_binding_table_info_t *table_info, void *user_ctx);

/** Management LQI Response Callback
 *
 * @brief A ZDO Mgmt_Lqi_rsp callback for user to get the mgmt lqi record of remote device.
 *
 * @param[in] rsp The response structure of ZDO mgmt lqi, refer to esp_zb_zdo_mgmt_lqi_rsp_t
 * @param[in] user_ctx User information context, set in esp_zb_zdo_mgmt_lqi_req()
 */
typedef void (*esp_zb_zdo_mgmt_lqi_rsp_callback_t)(const esp_zb_zdo_mgmt_lqi_rsp_t *rsp, void *user_ctx);

/********************* Declare functions **************************/
/* ZDO command list, more ZDO command will be supported later like node_desc, power_desc */

/**
 * @brief   Active scan available network.
 *
 * Network discovery service for scanning available network
 *
 * @param[in] channel_mask Valid channel mask is from 0x00000800 (only channel 11) to 0x07FFF800 (all channels from 11 to 26)
 * @param[in] scan_duration Time spent scanning each channel, in units of ((1 << scan_duration) + 1) * a beacon time.
 * @param[in] user_cb   A user callback to get the active scan result please refer to esp_zb_zdo_scan_complete_callback_t
 */
void esp_zb_zdo_active_scan_request(uint32_t channel_mask, uint8_t scan_duration, esp_zb_zdo_scan_complete_callback_t user_cb);

/**
 * @brief Energy detect request
 *
 * @param[in] channel_mask The channel mask that will trigger the energy detection, with a range from 0x00000800 to 0x07FFF800.
 * @param[in] duration The detection duration on each channel, in units of ((1 << scan_duration) + 1) * a beacon time.
 * @param[in] cb A user callback to receive the energy detection result, see esp_zb_zdo_energy_detect_callback_t.
 */
void esp_zb_zdo_energy_detect_request(uint32_t channel_mask, uint8_t duration, esp_zb_zdo_energy_detect_callback_t cb);

/**
 * @brief   Send bind device request command
 *
 * @param[in] cmd_req  Pointer to the bind request command @ref esp_zb_zdo_bind_req_param_s
 * @param[in] user_cb  A user callback that will be called if received bind response refer to esp_zb_zdo_bind_callback_t
 * @param[in] user_ctx A void pointer that contains the user defines additional information when callback trigger
 *
 */
void esp_zb_zdo_device_bind_req(esp_zb_zdo_bind_req_param_t *cmd_req, esp_zb_zdo_bind_callback_t user_cb, void *user_ctx);

/**
 * @brief   Send unbind device request command
 *
 * @param[in] cmd_req  Pointer to the bind request command @ref esp_zb_zdo_bind_req_param_s
 * @param[in] user_cb  A user callback that will be called if received bind response refer to esp_zb_zdo_bind_callback_t
 * @param[in] user_ctx A void pointer that contains the user defines additional information when callback trigger
 *
 */
void esp_zb_zdo_device_unbind_req(esp_zb_zdo_bind_req_param_t *cmd_req, esp_zb_zdo_bind_callback_t user_cb, void *user_ctx);

/**
 * @brief   Send find on-off device request command
 *
 * @param[in] cmd_req  Pointer to the find request command @ref esp_zb_zdo_match_desc_req_param_s
 * @param[in] user_cb  A user callback that will be called if received find response refer to esp_zb_zdo_match_desc_callback_t
 * @param[in] user_ctx A void pointer that contains the user defines additional information when callback trigger
 *
 */
void esp_zb_zdo_find_on_off_light(esp_zb_zdo_match_desc_req_param_t *cmd_req, esp_zb_zdo_match_desc_callback_t user_cb, void *user_ctx);

/**
 * @brief   Send find color_dimmable device request command
 *
 * @param[in] cmd_req  Pointer to the find request command @ref esp_zb_zdo_match_desc_req_param_s
 * @param[in] user_cb  A user callback that will be called if received find response refer to esp_zb_zdo_match_desc_callback_t
 * @param[in] user_ctx A void pointer that contains the user defines additional information when callback trigger
 *
 */
void esp_zb_zdo_find_color_dimmable_light(esp_zb_zdo_match_desc_req_param_t *cmd_req, esp_zb_zdo_match_desc_callback_t user_cb, void *user_ctx);

/**
 * @brief Send match desc request to find matched Zigbee device
 *
 * @param[in] param    Pointer to @ref esp_zb_zdo_match_desc_req_param_s that will be used to match device
 * @param[in] user_cb  A user callback that will be called if received find response refer to esp_zb_zdo_match_desc_callback_t
 * @param[in] user_ctx A void pointer that contains the user defines additional information when callback trigger
 * @return
 *          - ESP_OK: Success in send match desc request
 *          - ESP_ERR_NO_MEM: Failed to allocate the memory for the reqeust
 *          - ESP_ERR_INVALID_SIZE: The size of cluster list is wrong in @p param
 */
esp_err_t esp_zb_zdo_match_cluster(esp_zb_zdo_match_desc_req_param_t *param, esp_zb_zdo_match_desc_callback_t user_cb,
                                     void *user_ctx);

/**
 * @brief   Send ieee_addr request command
 *
 * @param[in] cmd_req  Pointer to the ieee address request command @ref esp_zb_zdo_ieee_addr_req_param_s
 * @param[in] user_cb  A user callback that will be called if received ieee address response refer to esp_zb_zdo_ieee_addr_callback_t
 * @param[in] user_ctx A void pointer that contains the user defines additional information when callback trigger
 *
 */
void esp_zb_zdo_ieee_addr_req(esp_zb_zdo_ieee_addr_req_param_t *cmd_req, esp_zb_zdo_ieee_addr_callback_t user_cb, void *user_ctx);

/**
 * @brief   Send nwk_addr request command
 *
 * @param[in] cmd_req  Pointer to the nwk address request command @ref esp_zb_zdo_nwk_addr_req_param_s
 * @param[in] user_cb  A user callback that will be called if received nwk address response refer to esp_zb_zdo_nwk_addr_callback_t
 * @param[in] user_ctx A void pointer that contains the user defines additional information when callback trigger
 *
 */
void esp_zb_zdo_nwk_addr_req(esp_zb_zdo_nwk_addr_req_param_t *cmd_req, esp_zb_zdo_nwk_addr_callback_t user_cb, void *user_ctx);

/**
 * @brief   Send node descriptor request command
 *
 * @param[in] cmd_req  Pointer to the node descriptor request command @ref esp_zb_zdo_node_desc_req_param_s
 * @param[in] user_cb  A user callback that will be called if received node desc response refer to esp_zb_zdo_node_desc_callback_t
 * @param[in] user_ctx A void pointer that contains the user defines additional information when callback trigger
 *
 */
void esp_zb_zdo_node_desc_req(esp_zb_zdo_node_desc_req_param_t *cmd_req, esp_zb_zdo_node_desc_callback_t user_cb, void *user_ctx);

/**
 * @brief   Send simple descriptor request command
 *
 * @param[in] cmd_req  Pointer to the simple descriptor request command @ref esp_zb_zdo_simple_desc_req_param_s
 * @param[in] user_cb  A user callback that will be called if received simple desc response refer to esp_zb_zdo_simple_desc_callback_t
 * @param[in] user_ctx A void pointer that contains the user defines additional information when callback trigger
 *
 */
void esp_zb_zdo_simple_desc_req(esp_zb_zdo_simple_desc_req_param_t *cmd_req, esp_zb_zdo_simple_desc_callback_t user_cb, void *user_ctx);

/**
 * @brief   Send active endpoint request command
 *
 * @param[in] cmd_req  Pointer to the active endpoint request command @ref esp_zb_zdo_active_ep_req_param_s
 * @param[in] user_cb  A user callback that will be called if received active ep response refer to esp_zb_zdo_active_ep_callback_t
 * @param[in] user_ctx A void pointer that contains the user defines additional information when callback trigger
 *
 */
void esp_zb_zdo_active_ep_req(esp_zb_zdo_active_ep_req_param_t *cmd_req, esp_zb_zdo_active_ep_callback_t user_cb, void *user_ctx);

/**
 * @brief   Send leave request command
 *
 * @param[in] cmd_req  Pointer to the leave request command @ref esp_zb_zdo_mgmt_leave_req_param_s
 * @param[in] user_cb  A user callback that will be called if received leave response refer to esp_zb_zdo_leave_callback_t
 * @param[in] user_ctx A void pointer that contains the user defines additional information when callback trigger
 *
 */
void esp_zb_zdo_device_leave_req(esp_zb_zdo_mgmt_leave_req_param_t *cmd_req, esp_zb_zdo_leave_callback_t user_cb, void *user_ctx);

/**
 * @brief   Send permit join request command
 *
 * @param[in] cmd_req  Pointer to the permit join request command @ref esp_zb_zdo_permit_joining_req_param_s
 * @param[in] user_cb  A user callback that will be called if received permit join response refer to esp_zb_zdo_permit_join_callback_t
 * @param[in] user_ctx A void pointer that contains the user defines additional information when callback trigger
 *
 */
void esp_zb_zdo_permit_joining_req(esp_zb_zdo_permit_joining_req_param_t *cmd_req, esp_zb_zdo_permit_join_callback_t user_cb, void *user_ctx);

/**
 * @brief  Send binding table request command
 *
 * @param[in] cmd_req  Pointer to the zdo mgmt bind request command @ref esp_zb_zdo_mgmt_bind_param_s
 * @param[in] user_cb  A user callback that will be called if received binding table response refer to esp_zb_zdo_binding_table_callback_t
 * @param[in] user_ctx A void pointer that contains the user defines additional information when callback trigger
 *
 */
void esp_zb_zdo_binding_table_req(esp_zb_zdo_mgmt_bind_param_t *cmd_req, esp_zb_zdo_binding_table_callback_t user_cb, void *user_ctx);

/**
 * @brief Send device announcement command
 *
 */
void esp_zb_zdo_device_announcement_req(void);

/**
 * @brief Send ZDO management lqi request command
 *
 * @param[in] cmd_req A pointer indicates the fields of the Mgmt_Lqi_req command, @ref esp_zb_zdo_mgmt_lqi_req_param_s
 * @param[in] user_cb A user callback that will be called if received Mgmt_Lqi_req response refer to esp_zb_zdo_mgmt_lqi_rsp_callback_t
 * @param[in] user_ctx A void pointer that contains the user defines additional information when user_cb is triggered
 */
void esp_zb_zdo_mgmt_lqi_req(esp_zb_zdo_mgmt_lqi_req_param_t *cmd_req, esp_zb_zdo_mgmt_lqi_rsp_callback_t user_cb, void* user_ctx);

/**
 * @brief Stringify the Zigbee Device Object signal
 *
 * @param[in] signal A @ref esp_zb_app_signal_type_t object that expect to stringified
 * @return A string pointer of esp_zb_app_signal_type_t
 */
const char *esp_zb_zdo_signal_to_string(esp_zb_app_signal_type_t signal);
#ifdef __cplusplus
}
#endif
