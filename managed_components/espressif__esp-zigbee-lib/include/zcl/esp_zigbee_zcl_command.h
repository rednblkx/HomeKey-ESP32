/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once
#include "esp_err.h"
#include "esp_zigbee_ota.h"
#include <stdint.h>
#include "aps/esp_zigbee_aps.h"
#ifdef __cplusplus
extern "C" {
#endif
#include "esp_zigbee_type.h"
#include "esp_zigbee_zcl_common.h"

#ifdef CONFIG_ZB_GP_ENABLED
#include "zgp/esp_zigbee_zgp.h"
#endif /* CONFIG_ZB_GP_ENABLED */

/** Defined the ZCL command of address_mode */
typedef esp_zb_aps_address_mode_t esp_zb_zcl_address_mode_t;

/**
 * @brief ZCL command direction enum
 * @anchor esp_zb_zcl_cmd_direction
 */
typedef enum {
    ESP_ZB_ZCL_CMD_DIRECTION_TO_SRV = 0x00U, /*!< Command for cluster server side */
    ESP_ZB_ZCL_CMD_DIRECTION_TO_CLI = 0x01U, /*!< Command for cluster client side */
} esp_zb_zcl_cmd_direction_t;

/**
 * @brief ZCL report direction enum of attribute
 * @anchor esp_zb_zcl_report_direction_t
 */
typedef enum {
    ESP_ZB_ZCL_REPORT_DIRECTION_SEND = 0x00U, /**< Report should be sent by a cluster. */
    ESP_ZB_ZCL_REPORT_DIRECTION_RECV = 0x01U, /**< Report should be received by a cluster. */
} esp_zb_zcl_report_direction_t;

/**
 * @brief The application message of ZCL command send status message
 *
 */
typedef struct esp_zb_zcl_command_send_status_s {
    esp_err_t status;           /*!< command send status */
    uint8_t tsn;                /*!< Transaction sequence number */
    uint8_t dst_endpoint;       /*!< Destination endpoint */
    uint8_t src_endpoint;       /*!< Source endpoint */
    esp_zb_zcl_addr_t dst_addr; /*!< Destination address */
} esp_zb_zcl_command_send_status_message_t;

/**
 * @brief The callback of Zigbee zcl command send status
 *
 */
typedef void (*esp_zb_zcl_command_send_status_callback_t)(esp_zb_zcl_command_send_status_message_t message);

/**
 * @brief The Zigbee zcl cluster attribute value struct
 *
 */
typedef struct esp_zb_zcl_attribute_data_s {
    esp_zb_zcl_attr_type_t type; /*!< The type of attribute, which can refer to esp_zb_zcl_attr_type_t */
    uint16_t size;               /*!< The value size of attribute  */
    void *value;                 /*!< The value of attribute, Note that if the type is string/array, the first byte of value indicates the string length */
} ESP_ZB_PACKED_STRUCT esp_zb_zcl_attribute_data_t;

/**
 * @brief The Zigbee zcl cluster attribute struct
 *
 */
typedef struct esp_zb_zcl_attribute_s {
    uint16_t id;                      /*!< The identify of attribute */
    esp_zb_zcl_attribute_data_t data; /*!< The data fo attribute */
} esp_zb_zcl_attribute_t;

/**
 * @brief The Zigbee zcl custom cluster handlers struct
 *
 */
typedef struct esp_zb_zcl_custom_cluster_handlers_s {
    uint16_t cluster_id;                                        /*!< Cluster identifier */
    uint8_t cluster_role;                                       /*!< Cluster role */
    esp_zb_zcl_cluster_check_value_callback_t check_value_cb;   /*!< The callback for validating the value from ZCL command */
    esp_zb_zcl_cluster_write_attr_callback_t write_attr_cb;     /*!< The callback for validating the ZCL write attribute */
} esp_zb_zcl_custom_cluster_handlers_t;

/**
 * @brief The Zigbee zcl cluster command properties struct
 *
 */
typedef struct esp_zb_zcl_command_s {
    uint8_t id;        /*!< The command id */
    uint8_t direction; /*!< The command direction */
    uint8_t is_common; /*!< The command is common type */
} esp_zb_zcl_command_t;

/**
 * @brief The Zigbee ZCL basic command info
 *
 */
typedef struct esp_zb_zcl_basic_cmd_s {
    esp_zb_addr_u dst_addr_u;                   /*!< Single short address or group address */
    uint8_t  dst_endpoint;                      /*!< Destination endpoint */
    uint8_t  src_endpoint;                      /*!< Source endpoint */
} esp_zb_zcl_basic_cmd_t;

/**
 * @brief The Zigbee ZCL command common struct, no command specific payload
 *
 */
typedef struct esp_zb_zcl_common_cmd_s {
    esp_zb_zcl_basic_cmd_t zcl_basic_cmd;           /*!< Basic command info */
    esp_zb_zcl_address_mode_t address_mode;         /*!< APS addressing mode constants refer to esp_zb_zcl_address_mode_t */
} esp_zb_zcl_common_cmd_t;

/**
 * @brief The Zigbee ZCL read attribute command struct
 *
 */
typedef struct esp_zb_zcl_read_attr_cmd_s {
    esp_zb_zcl_basic_cmd_t zcl_basic_cmd;           /*!< Basic command info */
    esp_zb_zcl_address_mode_t address_mode;         /*!< APS addressing mode constants refer to esp_zb_zcl_address_mode_t */
    uint16_t clusterID;                             /*!< Cluster ID to read */
    struct {
        uint8_t manuf_specific   : 2;               /*!< Sent as manufacturer extension with code. */
        uint8_t direction        : 1;               /*!< The command direction, refer to esp_zb_zcl_cmd_direction_t */
        uint8_t dis_defalut_resp : 1;               /*!< Disable default response for this command. */
    };
    uint16_t manuf_code;                            /*!< The manufacturer code sent with the command. */
    uint8_t attr_number;                            /*!< Number of attribute in the attr_field */
    uint16_t *attr_field;                           /*!< Attribute identifier field to read */
} esp_zb_zcl_read_attr_cmd_t;

/**
 * @brief The Zigbee ZCL write attribute command struct
 *
 */
typedef struct esp_zb_zcl_write_attr_cmd_s {
    esp_zb_zcl_basic_cmd_t zcl_basic_cmd;           /*!< Basic command info */
    esp_zb_zcl_address_mode_t address_mode;         /*!< APS addressing mode constants refer to esp_zb_zcl_address_mode_t */
    uint16_t clusterID;                             /*!< Cluster ID to write */
    struct {
        uint8_t manuf_specific   : 2;               /*!< Sent as manufacturer extension with code. */
        uint8_t direction        : 1;               /*!< The command direction, refer to esp_zb_zcl_cmd_direction_t */
        uint8_t dis_defalut_resp : 1;               /*!< Disable default response for this command. */
    };
    uint16_t manuf_code;                            /*!< The manufacturer code sent with the command. */
    uint8_t attr_number;                            /*!< Number of attribute in the attr_field  */
    esp_zb_zcl_attribute_t *attr_field;             /*!< Attributes which will be writed, @ref esp_zb_zcl_attribute_s */
} esp_zb_zcl_write_attr_cmd_t;

/**
 * @brief The Zigbee zcl configure report record struct
 *
 */
typedef struct esp_zb_zcl_config_report_record_s {
    esp_zb_zcl_report_direction_t direction; /*!< Direction field specifies whether values of the attribute are to be reported, or whether reports of the
                                                  attribute are to be received.*/
    uint16_t attributeID;                    /*!< Attribute ID to report */
    union {
        struct {
            uint8_t attrType;                /*!< Attribute type to report refer to zb_zcl_common.h zcl_attr_type */
            uint16_t min_interval;           /*!< Minimum reporting interval */
            uint16_t max_interval;           /*!< Maximum reporting interval */
            void *reportable_change;         /*!< Minimum change to attribute will result in report */
        };                                   /*!< Configurations to report sender. This is presented when the direction is ESP_ZB_ZCL_REPORT_DIRECTION_SEND,
                                              *   when the receiver is configuring the sender to report the attributes.
                                              */
        struct {
            uint16_t timeout;                /*!< Timeout period */
        };                                   /*!< Configurations to report receiver. This is presented when the direction is ESP_ZB_ZCL_REPORT_DIRECTION_RECV,
                                              *   when the sender is configuring the receiver to receive to attributes report.
                                              */
    };
} esp_zb_zcl_config_report_record_t;

/**
 * @brief The Zigbee ZCL configure report command struct
 *
 */
typedef struct esp_zb_zcl_config_report_cmd_s {
    esp_zb_zcl_basic_cmd_t zcl_basic_cmd;               /*!< Basic command info */
    esp_zb_zcl_address_mode_t address_mode;             /*!< APS addressing mode constants refer to esp_zb_zcl_address_mode_t */
    uint16_t clusterID;                                 /*!< Cluster ID to report */
    struct {
        uint8_t manuf_specific   : 2;                   /*!< Sent as manufacturer extension with code. */
        uint8_t direction        : 1;                   /*!< The command direction, refer to esp_zb_zcl_cmd_direction_t */
        uint8_t dis_defalut_resp : 1;                   /*!< Disable default response for this command. */
    };
    uint16_t manuf_code;                                /*!< The manufacturer code sent with the command. */
    uint16_t record_number;                             /*!< Number of report configuration record in the record_field */
    esp_zb_zcl_config_report_record_t *record_field;    /*!< Report configuration records, @ref esp_zb_zcl_config_report_record_s */
} esp_zb_zcl_config_report_cmd_t;

/**
 * @brief The Zigbee ZCL Attribute record struct
 *
 */
typedef struct esp_zb_zcl_attribute_record_s {
    uint8_t report_direction;   /*!< Report direction of this attribute, @ref esp_zb_zcl_report_direction_t */
    uint16_t attributeID;       /*!< Attribute ID to report */
} esp_zb_zcl_attribute_record_t;

/**
 * @brief The Zigbee ZCL read report configuration command struct
 *
 */
typedef struct esp_zb_zcl_read_report_config_cmd_s {
    esp_zb_zcl_basic_cmd_t zcl_basic_cmd;               /*!< Basic command info */
    esp_zb_zcl_address_mode_t address_mode;             /*!< APS addressing mode constants refer to esp_zb_zcl_address_mode_t */
    uint16_t clusterID;                                 /*!< Cluster ID to report */
    struct {
        uint8_t manuf_specific   : 2;                   /*!< Sent as manufacturer extension with code. */
        uint8_t direction        : 1;                   /*!< The command direction, refer to esp_zb_zcl_cmd_direction_t */
        uint8_t dis_defalut_resp : 1;                   /*!< Disable default response for this command. */
    };
    uint16_t manuf_code;                                /*!< The manufacturer code sent with the command. */
    uint16_t record_number;                             /*!< Number of attribute records in the record_field */
    esp_zb_zcl_attribute_record_t *record_field;        /*!< Attribute records, @ref esp_zb_zcl_attribute_record_s */
} esp_zb_zcl_read_report_config_cmd_t;

/**
 * @brief The Zigbee ZCL report attribute command struct
 *
 */
typedef struct esp_zb_zcl_report_attr_cmd_s {
    esp_zb_zcl_basic_cmd_t zcl_basic_cmd;           /*!< Basic command info */
    esp_zb_zcl_address_mode_t address_mode;         /*!< APS addressing mode constants refer to esp_zb_zcl_address_mode_t */
    uint16_t clusterID;                             /*!< Cluster ID to report */
    struct {
        uint8_t manuf_specific   : 2;               /*!< Sent as manufacturer extension with code. */
        uint8_t direction        : 1;               /*!< The command direction, refer to esp_zb_zcl_cmd_direction_t */
        uint8_t dis_defalut_resp : 1;               /*!< Disable default response for this command. */
    };
    uint16_t manuf_code;                            /*!< The manufacturer code of the attribute to report */
    uint16_t attributeID;                           /*!< Attribute ID to report */
} esp_zb_zcl_report_attr_cmd_t;

/* ZCL basic cluster */

/**
 * @brief The Zigbee ZCL configure report command struct
 *
 */
typedef struct esp_zb_zcl_disc_attr_cmd_s {
    esp_zb_zcl_basic_cmd_t zcl_basic_cmd;   /*!< Basic command info */
    esp_zb_zcl_address_mode_t address_mode; /*!< APS addressing mode constants refer to esp_zb_zcl_address_mode_t */
    uint16_t cluster_id;                    /*!< The cluster identifier for which the attribute is discovered. */
    struct {
        uint8_t manuf_specific   : 2;       /*!< Sent as manufacturer extension with code. */
        uint8_t direction        : 1;       /*!< The command direction, refer to esp_zb_zcl_cmd_direction_t */
        uint8_t dis_defalut_resp : 1;       /*!< Disable default response for this command. */
    };
    uint16_t manuf_code;                    /*!< The manufacturer code sent with the command. */
    uint16_t start_attr_id;                 /*!< The attribute identifier at which to begin the attribute discover */
    uint8_t max_attr_number;                /*!< The maximum number of attribute identifiers that are to be returned in the resulting Discover Attributes Response command*/
} esp_zb_zcl_disc_attr_cmd_t;

/**
 * @brief The Zigbee ZCL basic reset factory default command struct
 *
 */
typedef struct esp_zb_zcl_basic_fact_reset_cmd_s {
    esp_zb_zcl_basic_cmd_t zcl_basic_cmd;           /*!< Basic command info */
    esp_zb_zcl_address_mode_t address_mode;         /*!< APS addressing mode constants refer to esp_zb_zcl_address_mode_t */
} esp_zb_zcl_basic_fact_reset_cmd_t;

/* ZCL on/off cluster */

/**
 * @brief The Zigbee ZCL on-off command struct
 *
 */
typedef struct esp_zb_zcl_on_off_cmd_s {
    esp_zb_zcl_basic_cmd_t zcl_basic_cmd;           /*!< Basic command info */
    esp_zb_zcl_address_mode_t address_mode;         /*!< APS addressing mode constants refer to esp_zb_zcl_address_mode_t */
    uint8_t  on_off_cmd_id;                         /*!< command id for the on-off cluster command */
} esp_zb_zcl_on_off_cmd_t;

/**
 * @brief The Zigbee ZCL on-off off with effect command struct
 *
 */
typedef struct esp_zb_zcl_on_off_off_with_effect_cmd_s {
    esp_zb_zcl_basic_cmd_t zcl_basic_cmd;   /*!< Basic command info */
    esp_zb_zcl_address_mode_t address_mode; /*!< APS addressing mode constants refer to esp_zb_zcl_address_mode_t */
    uint8_t effect_id;                      /*!< The field specifies the fading effect to use when switching the device off */
    uint8_t effect_variant;                 /*!< The field is used to indicate which variant of the effect, indicated in the Effect Identifier field, SHOULD be triggered. */
} esp_zb_zcl_on_off_off_with_effect_cmd_t;


/**
 * @brief The Zigbee ZCL on-off on with recall global scene command struct
 *
 */
typedef struct esp_zb_zcl_on_off_on_with_recall_global_scene_cmd_s {
    esp_zb_zcl_basic_cmd_t zcl_basic_cmd;   /*!< Basic command info */
    esp_zb_zcl_address_mode_t address_mode; /*!< APS addressing mode constants refer to esp_zb_zcl_address_mode_t */
} esp_zb_zcl_on_off_on_with_recall_global_scene_cmd_t;

/**
 * @brief The Zigbee ZCL on-off on with timed off command struct
 *
 */
typedef struct esp_zb_zcl_on_off_on_with_timed_off_cmd_s {
    esp_zb_zcl_basic_cmd_t zcl_basic_cmd;   /*!< Basic command info */
    esp_zb_zcl_address_mode_t address_mode; /*!< APS addressing mode constants refer to esp_zb_zcl_address_mode_t */
    uint8_t on_off_control;                 /*!< The field contains information on how the device is to be operated */
    uint16_t on_time;                       /*!< The field specifies the length of time (in 1/10ths second) that the device is to remain "on", i.e.,
                                                 with its OnOff attribute equal to 0x01, before automatically turning "off".*/
    uint16_t off_wait_time;                 /*!< The field specifies the length of time (in 1/10ths second) that the device SHALL remain "off", i.e.,
                                                 with its OnOff attribute equal to 0x00, and guarded to prevent an on command turning the device back "on" */
} esp_zb_zcl_on_off_on_with_timed_off_cmd_t;

/* ZCL identify cluster */

/**
 * @brief The Zigbee ZCL identify command struct
 *
 */
typedef struct esp_zb_zcl_identify_cmd_s {
    esp_zb_zcl_basic_cmd_t zcl_basic_cmd;           /*!< Basic command info */
    esp_zb_zcl_address_mode_t address_mode;         /*!< APS addressing mode constants refer to esp_zb_zcl_address_mode_t */
    uint16_t identify_time;                         /*!< identify itself for specific time */
} esp_zb_zcl_identify_cmd_t;

/**
 * @brief The Zigbee ZCL identify trigger effect command strcut
 *
 */
typedef struct esp_zb_zcl_identify_trigger_effect_cmd_s {
    esp_zb_zcl_basic_cmd_t zcl_basic_cmd;   /*!< Basic command info */
    esp_zb_zcl_address_mode_t address_mode; /*!< APS addressing mode constants refer to esp_zb_zcl_address_mode_t */
    uint8_t effect_id;                      /*!< The field specifies the identify effect to use, refer to esp_zb_zcl_identify_trigger_effect_s */
    uint8_t effect_variant;                 /*!< The field is used to indicate which variant of the effect, indicated in the effect identifier field, SHOULD be triggered */
} esp_zb_zcl_identify_trigger_effect_cmd_t;

/**
 * @brief The Zigbee ZCL identify query command struct
 *
 */
typedef struct esp_zb_zcl_identify_query_cmd_s {
    esp_zb_zcl_basic_cmd_t zcl_basic_cmd;           /*!< Basic command info */
    esp_zb_zcl_address_mode_t address_mode;         /*!< APS addressing mode constants refer to esp_zb_zcl_address_mode_t */
} esp_zb_zcl_identify_query_cmd_t;

/* ZCL commisssioning cluster */

/**
 * @brief The Zigbee ZCL commisssioning restart device command struct
 *
 */
typedef struct esp_zb_zcl_comm_restart_device_cmd_s {
    esp_zb_zcl_basic_cmd_t zcl_basic_cmd;                        /*!< Basic command info */
    esp_zb_zcl_address_mode_t address_mode;                      /*!< APS addressing mode constants refer to esp_zb_zcl_address_mode_t */
    esp_zb_zcl_commissioning_restart_device_options_t options;   /*!< Restart device options */
    uint8_t delay;                                               /*!< Seconds of delay before starting the restart procedure */
    uint8_t jitter;                                              /*!< Parameter to calculate the additional milliseconds should be added to delay */
} esp_zb_zcl_comm_restart_device_cmd_t;

/**
 * @brief The Zigbee ZCL commisssioning save startup parameters command struct
 *
 */
typedef struct esp_zb_zcl_comm_save_startup_params_cmd_s {
    esp_zb_zcl_basic_cmd_t zcl_basic_cmd;               /*!< Basic command info */
    esp_zb_zcl_address_mode_t address_mode;             /*!< APS addressing mode constants refer to esp_zb_zcl_address_mode_t */
    uint8_t index;                                      /*!< Index to save the parameter set */
} esp_zb_zcl_comm_save_startup_params_cmd_t;

/**
 * @brief The Zigbee ZCL commisssioning restore startup parameters command struct
 *
 */
typedef struct esp_zb_zcl_comm_restore_startup_params_cmd_s {
    esp_zb_zcl_basic_cmd_t zcl_basic_cmd;             /*!< Basic command info */
    esp_zb_zcl_address_mode_t address_mode;           /*!< APS addressing mode constants refer to esp_zb_zcl_address_mode_t */
    uint8_t index;                                    /*!< Index of the saved the parameter set to be restored */
} esp_zb_zcl_comm_restore_startup_params_cmd_t;

/**
 * @brief The Zigbee ZCL commisssioning reset startup parameters command struct
 *
 */
typedef struct esp_zb_zcl_comm_reset_startup_params_cmd_s {
    esp_zb_zcl_basic_cmd_t zcl_basic_cmd;                             /*!< Basic command info */
    esp_zb_zcl_address_mode_t address_mode;                           /*!< APS addressing mode constants refer to esp_zb_zcl_address_mode_t */
    esp_zb_zcl_commissioning_reset_startup_param_options_t options;   /*!< Reset startup parameter options */
    uint8_t index;                                                    /*!< Index of the saved parameters to be erased */
} esp_zb_zcl_comm_reset_startup_params_cmd_t;

/* ZCL level cluster */

/**
 * @brief The Zigbee ZCL level move to level command struct
 *
 */
typedef struct esp_zb_zcl_move_to_level_cmd_s {
    esp_zb_zcl_basic_cmd_t zcl_basic_cmd;               /*!< Basic command info */
    esp_zb_zcl_address_mode_t address_mode;             /*!< APS addressing mode constants refer to esp_zb_zcl_address_mode_t */
    uint8_t level;                                      /*!< level wants to move to */
    uint16_t transition_time;                           /*!< time wants to transition tenths of a second */
} esp_zb_zcl_move_to_level_cmd_t;

/**
 * @brief The Zigbee ZCL level move command struct
 *
 */
typedef struct esp_zb_zcl_level_move_cmd_s {
    esp_zb_zcl_basic_cmd_t zcl_basic_cmd;               /*!< Basic command info */
    esp_zb_zcl_address_mode_t address_mode;             /*!< APS addressing mode constants refer to esp_zb_zcl_address_mode_t */
    uint8_t move_mode;                                  /*!< move mode either up or down */
    uint8_t rate;                                       /*!< move rate wants to movement in units per second */
} esp_zb_zcl_level_move_cmd_t;

/**
 * @brief The Zigbee ZCL level step command struct
 *
 */
typedef struct esp_zb_zcl_level_step_cmd_s {
    esp_zb_zcl_basic_cmd_t zcl_basic_cmd;               /*!< Basic command info */
    esp_zb_zcl_address_mode_t address_mode;             /*!< APS addressing mode constants refer to esp_zb_zcl_address_mode_t */
    uint8_t step_mode;                                  /*!< step mode either up or down */
    uint8_t step_size;                                  /*!< step size wants to change*/
    uint16_t transition_time;                           /*!< time wants to transition tenths of a second */
} esp_zb_zcl_level_step_cmd_t;

/**
 * @brief The Zigbee ZCL level stop command struct
 *
 */
typedef struct esp_zb_zcl_level_stop_cmd_s {
    esp_zb_zcl_basic_cmd_t zcl_basic_cmd;               /*!< Basic command info */
    esp_zb_zcl_address_mode_t address_mode;             /*!< APS addressing mode constants refer to esp_zb_zcl_address_mode_t */
} esp_zb_zcl_level_stop_cmd_t;

/* ZCL color cluster */

/**
 * @brief The Zigbee ZCL color move to hue command struct
 *
 */
typedef struct esp_zb_zcl_color_move_to_hue_cmd_s {
    esp_zb_zcl_basic_cmd_t zcl_basic_cmd;               /*!< Basic command info */
    esp_zb_zcl_address_mode_t address_mode;             /*!< APS addressing mode constants refer to esp_zb_zcl_address_mode_t */
    uint8_t hue;                                        /*!< current value of hue */
    uint8_t direction;                                  /*!< direction */
    uint16_t transition_time;                           /*!< time wants to transition tenths of a second */
} esp_zb_zcl_color_move_to_hue_cmd_t;

/**
 * @brief The Zigbee ZCL color move hue command struct
 *
 */
typedef struct esp_zb_zcl_color_move_hue_cmd_s {
    esp_zb_zcl_basic_cmd_t zcl_basic_cmd;               /*!< Basic command info */
    esp_zb_zcl_address_mode_t address_mode;             /*!< APS addressing mode constants refer to esp_zb_zcl_address_mode_t */
    uint8_t move_mode;                                  /*!< move mode */
    uint8_t rate;                                       /*!< rate */
} esp_zb_zcl_color_move_hue_cmd_t;

/**
 * @brief The Zigbee ZCL color step hue command struct
 *
 */
typedef struct esp_zb_zcl_color_step_hue_cmd_s {
    esp_zb_zcl_basic_cmd_t zcl_basic_cmd;               /*!< Basic command info */
    esp_zb_zcl_address_mode_t address_mode;             /*!< APS addressing mode constants refer to esp_zb_zcl_address_mode_t */
    uint8_t step_mode;                                  /*!< step mode */
    uint8_t step_size;                                  /*!< step size */
    uint16_t transition_time;                           /*!< time wants to transition tenths of a second */
} esp_zb_zcl_color_step_hue_cmd_t;

/**
 * @brief The Zigbee ZCL color move to saturation command struct
 *
 */
typedef struct esp_zb_zcl_color_move_to_saturation_cmd_s {
    esp_zb_zcl_basic_cmd_t zcl_basic_cmd;               /*!< Basic command info */
    esp_zb_zcl_address_mode_t address_mode;             /*!< APS addressing mode constants refer to esp_zb_zcl_address_mode_t */
    uint8_t saturation;                                 /*!< current value of saturation */
    uint16_t transition_time;                           /*!< time wants to transition tenths of a second */
} esp_zb_zcl_color_move_to_saturation_cmd_t;

/**
 * @brief The Zigbee ZCL color move saturation command struct
 *
 */
typedef struct esp_zb_zcl_color_move_saturation_cmd_s {
    esp_zb_zcl_basic_cmd_t zcl_basic_cmd;               /*!< Basic command info */
    esp_zb_zcl_address_mode_t address_mode;             /*!< APS addressing mode constants refer to esp_zb_zcl_address_mode_t */
    uint8_t move_mode;                                  /*!< move mode */
    uint8_t rate;                                       /*!< rate */
} esp_zb_zcl_color_move_saturation_cmd_t;

/**
 * @brief The Zigbee ZCL color step saturation command struct
 *
 */
typedef struct esp_zb_zcl_color_step_saturation_cmd_s {
    esp_zb_zcl_basic_cmd_t zcl_basic_cmd;               /*!< Basic command info */
    esp_zb_zcl_address_mode_t address_mode;             /*!< APS addressing mode constants refer to esp_zb_zcl_address_mode_t */
    uint8_t step_mode;                                  /*!< step mode */
    uint8_t step_size;                                  /*!< step size */
    uint16_t transition_time;                           /*!< time wants to transition tenths of a second */
} esp_zb_zcl_color_step_saturation_cmd_t;

/**
 * @brief The Zigbee ZCL color move to hue and saturation command struct
 *
 */
typedef struct esp_zb_color_move_to_hue_saturation_cmd_s {
    esp_zb_zcl_basic_cmd_t zcl_basic_cmd;               /*!< Basic command info */
    esp_zb_zcl_address_mode_t address_mode;             /*!< APS addressing mode constants refer to esp_zb_zcl_address_mode_t */
    uint8_t hue;                                        /*!< current value of hue */
    uint8_t saturation;                                 /*!< current value of saturation */
    uint16_t transition_time;                           /*!< time wants to transition tenths of a second */
} esp_zb_color_move_to_hue_saturation_cmd_t;

/**
 * @brief The Zigbee ZCL color move to color command struct
 *
 */
typedef struct esp_zb_zcl_color_move_to_color_cmd_s {
    esp_zb_zcl_basic_cmd_t zcl_basic_cmd;               /*!< Basic command info */
    esp_zb_zcl_address_mode_t address_mode;             /*!< APS addressing mode constants refer to esp_zb_zcl_address_mode_t */
    uint16_t color_x;                                   /*!< current value of chromaticity value x from (0 ~ 1) to (0 ~ 65535)*/
    uint16_t color_y;                                   /*!< current value of chromaticity value y from (0 ~ 1) to (0 ~ 65535)*/
    uint16_t transition_time;                           /*!< time wants to transition tenths of a second */
} esp_zb_zcl_color_move_to_color_cmd_t;

/**
 * @brief The Zigbee ZCL color move color command struct
 *
 */
typedef struct esp_zb_zcl_color_move_color_cmd_s {
    esp_zb_zcl_basic_cmd_t zcl_basic_cmd;               /*!< Basic command info */
    esp_zb_zcl_address_mode_t address_mode;             /*!< APS addressing mode constants refer to esp_zb_zcl_address_mode_t */
    int16_t rate_x;                                     /*!< specifies the rate of movement in steps per second of color x */
    int16_t rate_y;                                     /*!< specifies the rate of movement in steps per second of color y */
} esp_zb_zcl_color_move_color_cmd_t;

/**
 * @brief The Zigbee ZCL color step color command struct
 *
 */
typedef struct esp_zb_zcl_color_step_color_cmd_s {
    esp_zb_zcl_basic_cmd_t zcl_basic_cmd;               /*!< Basic command info */
    esp_zb_zcl_address_mode_t address_mode;             /*!< APS addressing mode constants refer to esp_zb_zcl_address_mode_t */
    int16_t step_x;                                     /*!< specifies the change to be added to color x */
    int16_t step_y;                                     /*!< specifies the change to be added to color x */
    uint16_t transition_time;                           /*!< time wants to transition tenths of a second */
} esp_zb_zcl_color_step_color_cmd_t;

/**
 * @brief The Zigbee ZCL color stop command struct
 *
 */
typedef struct esp_zb_zcl_color_stop_move_step_cmd_s {
    esp_zb_zcl_basic_cmd_t zcl_basic_cmd;                   /*!< Basic command info */
    esp_zb_zcl_address_mode_t address_mode;                 /*!< APS addressing mode constants refer to esp_zb_zcl_address_mode_t */
} esp_zb_zcl_color_stop_move_step_cmd_t;

/**
 * @brief The Zigbee ZCL color move to color temperature command struct
 *
 */
typedef struct esp_zb_zcl_color_move_to_color_temperature_cmd_s {
    esp_zb_zcl_basic_cmd_t zcl_basic_cmd;   /*!< Basic command info */
    esp_zb_zcl_address_mode_t address_mode; /*!< APS addressing mode constants refer to esp_zb_zcl_address_mode_t */
    uint16_t color_temperature;             /*!< The field indicates the  color-temperature value */
    uint16_t transition_time;               /*!< The time wants to transition tenths of a second */
} esp_zb_zcl_color_move_to_color_temperature_cmd_t;

/**
 * @brief The Zigbee ZCL color enhanced move to hue command struct
 *
 */
typedef struct esp_zb_zcl_color_enhanced_move_to_hue_cmd_s {
    esp_zb_zcl_basic_cmd_t zcl_basic_cmd;   /*!< Basic command info */
    esp_zb_zcl_address_mode_t address_mode; /*!< APS addressing mode constants refer to esp_zb_zcl_address_mode_t */
    uint16_t enhanced_hue;                  /*!< The field specifies the target enhanced hue for the lamp */
    uint8_t direction;                      /*!< The direction */
    uint16_t transition_time;               /*!< The time wants to transition tenths of a second */
} esp_zb_zcl_color_enhanced_move_to_hue_cmd_t;

/**
 * @brief The Zigbee ZCL color enhanced move hue
 *
 */
typedef struct esp_zb_zcl_color_enhanced_move_hue_cmd_s {
    esp_zb_zcl_basic_cmd_t zcl_basic_cmd;   /*!< Basic command info */
    esp_zb_zcl_address_mode_t address_mode; /*!< APS addressing mode constants refer to esp_zb_zcl_address_mode_t */
    uint8_t move_mode;                      /*!< The Move Mode, If the Move Mode field is equal to 0x00 (Stop), the Rate field SHALL be ignored */
    uint16_t rate;                          /*!< The field specifies the rate of movement in steps per second */
} esp_zb_zcl_color_enhanced_move_hue_cmd_t;

/**
 * @brief The Zigbee ZCL color enhanced step hue command struct
 *
 */
typedef struct esp_zb_zcl_color_enhanced_step_hue_cmd_s {
    esp_zb_zcl_basic_cmd_t zcl_basic_cmd;   /*!< Basic command info */
    esp_zb_zcl_address_mode_t address_mode; /*!< APS addressing mode constants refer to esp_zb_zcl_address_mode_t */
    uint8_t step_mode;                      /*!< The Step Mode */
    uint16_t step_size;                     /*!< The Step Size specifies the change to be added to the current value of the device’s enhanced hue.*/
    uint16_t transition_time;               /*!< The time wants to transition tenths of a second  */
} esp_zb_zcl_color_enhanced_step_hue_cmd_t;

/**
 * @brief The Zigbee ZCL color enhanced move to hue saturation command struct
 *
 */
typedef struct esp_zb_zcl_color_enhanced_move_to_hue_saturation_cmd_s {
    esp_zb_zcl_basic_cmd_t zcl_basic_cmd;   /*!< Basic command info */
    esp_zb_zcl_address_mode_t address_mode; /*!< APS addressing mode constants refer to esp_zb_zcl_address_mode_t */
    uint16_t enhanced_hue;                  /*!< The Enhanced Hue specifies the target extended hue for the lamp */
    uint8_t saturation;                     /*!< The value of Saturation */
    uint16_t transition_time;               /*!< The time wants to transition tenths of a second */
} esp_zb_zcl_color_enhanced_move_to_hue_saturation_cmd_t;

/**
 * @brief The Zigbee ZCL color color loop set command struct
 *
 */
typedef struct esp_zb_zcl_color_color_loop_set_cmd_s {
    esp_zb_zcl_basic_cmd_t zcl_basic_cmd;   /*!< Basic command info */
    esp_zb_zcl_address_mode_t address_mode; /*!< APS addressing mode constants refer to esp_zb_zcl_address_mode_t */
    uint8_t update_flags;                   /*!< The Update Flags field specifies which color loop attributes to update before the color loop is started */
    uint8_t action;                         /*!< The Action field specifies the action to take for the color loop,
                                                 if the Update Action sub-field of the Update Flags field is set to 1. */
    uint8_t direction;                      /*!< The Direction field of the color loop set command */
    uint16_t time;                          /*!< The Time field specifies the number of seconds over which to perform a full color loop,
                                                 if the Update Time field of the Update Flags field is set to 1. */
    uint16_t start_hue;                     /*!< The field specifies the starting hue to use for the color loop if the Update Start Hue field of the Update Flags field is set to 1 */
} esp_zb_zcl_color_color_loop_set_cmd_t;

/**
 * @brief The Zigbee ZCL color move color temperature command struct
 *
 */
typedef struct esp_zb_zcl_color_move_color_temperature_cmd_s {
    esp_zb_zcl_basic_cmd_t zcl_basic_cmd;   /*!< Basic command info */
    esp_zb_zcl_address_mode_t address_mode; /*!< APS addressing mode constants refer to esp_zb_zcl_address_mode_t */
    uint8_t move_mode;                      /*!< The Move Mode field of the Move Hue command, if the Move Mode field is equal to 0x00, the Rate field SHALL be ignored. */
    uint16_t rate;                          /*!< The Rate field specifies the rate of movement in steps per second */
    uint16_t color_temperature_minimum;     /*!< The field specifies a lower bound on the Color-Temperature attribute */
    uint16_t color_temperature_maximum;     /*!< The field specifies a upper bound on the Color-Temperature attribute */
} esp_zb_zcl_color_move_color_temperature_cmd_t;

/**
 * @brief The Zigbee ZCL color step color temperature command struct
 *
 */
typedef struct esp_zb_zcl_color_step_color_temperature_cmd_s {
    esp_zb_zcl_basic_cmd_t zcl_basic_cmd;   /*!< Basic command info */
    esp_zb_zcl_address_mode_t address_mode; /*!< APS addressing mode constants refer to esp_zb_zcl_address_mode_t */
    uint8_t move_mode;                      /*!< The Move Mode field of the Step Hue command, If the Move Mode field is equal to 0x00, the Rate field SHALL be ignored. */
    uint16_t step_size;                     /*!< The Step Size field specifies the change to be added to (or subtracted from) the current
                                                 value of the device’s color temperature.*/
    uint16_t transition_time;               /*!< The time wants to transition tenths of a second  */
    uint16_t color_temperature_minimum;     /*!< The field specifies a lower bound on the Color-Temperature attribute*/
    uint16_t color_temperature_maximum;     /*!< The field specifies a upper bound on the Color-Temperature attribute*/
} esp_zb_zcl_color_step_color_temperature_cmd_t;

/**
 * @brief The Zigbee ZCL lock/unlock door command struct
 *
 */
typedef struct esp_zb_zcl_lock_unlock_door_cmd_s {
    esp_zb_zcl_basic_cmd_t zcl_basic_cmd;                   /*!< Basic command info */
    esp_zb_zcl_address_mode_t address_mode;                 /*!< APS addressing mode constants refer to esp_zb_zcl_address_mode_t */
} esp_zb_zcl_lock_unlock_door_cmd_t;

/**
 * @brief The Zigbee ZCL groups add group command struct
 *
 * @note Group name currently is not supported, put empty string, Support of group names is optional, @@see ZCL specification, subclause  3.6.2.2.2
 */
typedef struct esp_zb_zcl_groups_add_group_cmd_s {
    esp_zb_zcl_basic_cmd_t zcl_basic_cmd;                  /*!< Basic command info */
    esp_zb_zcl_address_mode_t address_mode;                /*!< APS addressing mode constants, refer to esp_zb_zcl_address_mode_t */
    uint16_t group_id;                                     /*!< Group id */
} esp_zb_zcl_groups_add_group_cmd_t;

/**
 * @brief The Zigbee ZCL groups remove all groups command struct
 *
 */
typedef struct esp_zb_zcl_groups_remove_all_groups_cmd_s {
    esp_zb_zcl_basic_cmd_t zcl_basic_cmd;                  /*!< Basic command info */
    esp_zb_zcl_address_mode_t address_mode;                /*!< APS addressing mode constants, refer to esp_zb_zcl_address_mode_t */
} esp_zb_zcl_groups_remove_all_groups_cmd_t;

/**
 * @brief The Zigbee ZCL groups get group membership command struct
 *
 * @note Get group membership will set enable ZCL response by default, later will support this feature
 *
 */
typedef struct esp_zb_zcl_groups_get_group_membership_cmd_s {
    esp_zb_zcl_basic_cmd_t zcl_basic_cmd;                  /*!< Basic command info */
    esp_zb_zcl_address_mode_t address_mode;                /*!< APS addressing mode constants, refer to esp_zb_zcl_address_mode_t */
    uint8_t group_count;                                   /*!< Total group count */
    uint16_t *group_list;                                  /*!< Maximum group list */
} esp_zb_zcl_groups_get_group_membership_cmd_t;

/**
 * @brief The Zigbee ZCL scenes extension field struct
 *
 */
typedef struct esp_zb_zcl_scenes_extension_field_s {
    uint16_t cluster_id;                                   /*!< Cluster id */
    uint8_t length;                                        /*!< Length of scenes_extension_field */
    uint8_t *extension_field_attribute_value_list;         /*!< Extension field attribute value list */
    struct esp_zb_zcl_scenes_extension_field_s *next;      /*!< A pointer to next scenes extension field */
} esp_zb_zcl_scenes_extension_field_t;

/**
 * @brief The Zigbee ZCL scenes add scene command struct
 *
 * The maximum number of scenes capable of being stored in the table is 10.
 */
typedef struct esp_zb_zcl_scenes_add_scene_cmd_s {
    esp_zb_zcl_basic_cmd_t zcl_basic_cmd;                        /*!< Basic command info */
    uint16_t group_id;                                           /*!< Group id */
    uint8_t scene_id;                                            /*!< Scene id */
    uint16_t transition_time;                                    /*!< Time wants to transition tenths of a second */
    esp_zb_zcl_scenes_extension_field_t *extension_field;        /*!< The extension field list, please use 'NULL' as the end of list */
} esp_zb_zcl_scenes_add_scene_cmd_t;

/**
 * @brief The Zigbee ZCL scenes remove scene command struct
 *
 */
typedef struct esp_zb_zcl_scenes_remove_scene_cmd_s {
    esp_zb_zcl_basic_cmd_t zcl_basic_cmd;               /*!< Basic command info */
    esp_zb_zcl_address_mode_t address_mode;             /*!< APS addressing mode constants refer to esp_zb_zcl_address_mode_t */
    uint16_t group_id;                                  /*!< Group id */
    uint8_t scene_id;                                   /*!< Scene id */
} esp_zb_zcl_scenes_remove_scene_cmd_t;

/**
 * @brief The Zigbee ZCL scenes remove all scenes command struct
 *
 */
typedef struct esp_zb_zcl_scenes_remove_all_scenes_cmd_s {
    esp_zb_zcl_basic_cmd_t zcl_basic_cmd;               /*!< Basic command info */
    esp_zb_zcl_address_mode_t address_mode;             /*!< APS addressing mode constants refer to esp_zb_zcl_address_mode_t */
    uint16_t group_id;                                  /*!< Group id */
} esp_zb_zcl_scenes_remove_all_scenes_cmd_t;

/**
 * @brief The Zigbee ZCL scenes view scene command struct
 *
 */
typedef struct esp_zb_zcl_scenes_view_scene_cmd_s {
    esp_zb_zcl_basic_cmd_t zcl_basic_cmd;               /*!< Basic command info */
    uint16_t group_id;                                  /*!< Group id */
    uint8_t scene_id;                                   /*!< Scene id */
} esp_zb_zcl_scenes_view_scene_cmd_t;

/**
 * @brief The Zigbee ZCL scenes store scene command struct
 *
 */
typedef struct esp_zb_zcl_scenes_store_scene_cmd_s {
    esp_zb_zcl_basic_cmd_t zcl_basic_cmd;                      /*!< Basic command info */
    esp_zb_zcl_address_mode_t address_mode;                    /*!< APS addressing mode constants refer to esp_zb_zcl_address_mode_t */
    uint16_t group_id;                                         /*!< Group id */
    uint8_t scene_id;                                          /*!< Scene id */
} esp_zb_zcl_scenes_store_scene_cmd_t;

/**
 * @brief The Zigbee ZCL scenes recall scene command struct
 *
 */
typedef struct esp_zb_zcl_scenes_recall_scene_cmd_s {
    esp_zb_zcl_basic_cmd_t zcl_basic_cmd;                 /*!< Basic command info */
    esp_zb_zcl_address_mode_t address_mode;               /*!< APS addressing mode constants refer to esp_zb_zcl_address_mode_t */
    uint16_t group_id;                                    /*!< Group id */
    uint8_t scene_id;                                     /*!< Scene id */
} esp_zb_zcl_scenes_recall_scene_cmd_t;

/**
 * @brief The Zigbee ZCL scenes get scene membership command struct
 *
 */
typedef struct esp_zb_zcl_scenes_get_scene_membership_cmd_s {
    esp_zb_zcl_basic_cmd_t zcl_basic_cmd;               /*!< Basic command info */
    esp_zb_zcl_address_mode_t address_mode;             /*!< APS addressing mode constants refer to esp_zb_zcl_address_mode_t */
    uint16_t group_id;                                  /*!< Group id */
} esp_zb_zcl_scenes_get_scene_membership_cmd_t;

/**
 * @brief The Zigbee ZCL IAS zone enroll response command struct
 */
typedef struct esp_zb_zcl_ias_zone_enroll_response_cmd_s {
    esp_zb_zcl_basic_cmd_t zcl_basic_cmd;                      /*!< Basic command info */
    esp_zb_zcl_address_mode_t address_mode;                    /*!< APS addressing mode constants refer to esp_zb_zcl_address_mode_t */
    uint8_t enroll_rsp_code;                                   /*!< The enroll response code refer to `esp_zb_zcl_ias_zone_enroll_response_code_t` */
    uint8_t zone_id;                                           /*!< Zone ID is the index of table */
} esp_zb_zcl_ias_zone_enroll_response_cmd_t;

/**
 * @brief The Zigbee ZCL IAS zone Change Notification command struct
 */
typedef struct esp_zb_zcl_ias_zone_status_change_notif_cmd_s {
    esp_zb_zcl_basic_cmd_t zcl_basic_cmd;                       /*!< Basic command info */
    esp_zb_zcl_address_mode_t address_mode;                     /*!< APS addressing mode constants refer to esp_zb_zcl_address_mode_t */
    uint16_t zone_status;                                       /*!< Zone status shall be current value of the zone status attribute */
    uint8_t  extend_status;                                     /*!< Extended status for additional info */
    uint8_t  zone_id;                                           /*!< Zone ID is the index of table */
    uint16_t delay;                                             /*!< Delay in quarter-seconds */
} esp_zb_zcl_ias_zone_status_change_notif_cmd_t;

/**
 * @brief The Zigbee ZCL IAS zone enroll request command struct
 */
typedef struct esp_zb_zcl_ias_zone_enroll_request_cmd_s {
    esp_zb_zcl_basic_cmd_t zcl_basic_cmd;                      /*!< Basic command info */
    esp_zb_zcl_address_mode_t address_mode;                    /*!< APS addressing mode constants refer to esp_zb_zcl_address_mode_t */
    uint16_t zone_type;                                        /*!< Zone type */
    uint16_t manuf_code;                                       /*!< Manufacturer code */
} esp_zb_zcl_ias_zone_enroll_request_cmd_t;

/**
 * @brief The Zigbee ZCL IAS_ACE Arm command struct
 */
typedef struct esp_zb_zcl_ias_ace_arm_cmd_s {
    esp_zb_zcl_basic_cmd_t    zcl_basic_cmd;                   /*!< Basic command info */
    esp_zb_zcl_address_mode_t address_mode;                    /*!< APS addressing mode constants refer to esp_zb_zcl_address_mode_t */
    esp_zb_zcl_ias_ace_arm_t  payload;                         /*!< Arm command payload */
} esp_zb_zcl_ias_ace_arm_cmd_t;

/**
 * @brief The Zigbee ZCL IAS_ACE Bypass command struct
 */
typedef struct esp_zb_zcl_ias_ace_bypass_cmd_s {
    esp_zb_zcl_basic_cmd_t zcl_basic_cmd;                       /*!< Basic command info */
    esp_zb_zcl_address_mode_t address_mode;                     /*!< APS addressing mode constants refer to esp_zb_zcl_address_mode_t */
    esp_zb_zcl_ias_ace_bypass_t payload;                        /*!< Bypass command payload */
} esp_zb_zcl_ias_ace_bypass_cmd_t;

/**
 * @brief The Zigbee ZCL IAS_WD Start Warning command struct
 */
typedef struct esp_zb_zcl_ias_wd_start_warning_cmd_s {
    esp_zb_zcl_basic_cmd_t            zcl_basic_cmd;  /*!< Basic command info */
    esp_zb_zcl_address_mode_t         address_mode;   /*!< APS addressing mode constants refer to esp_zb_zcl_address_mode_t */
    esp_zb_zcl_ias_wd_start_warning_t payload;        /*!< Start Warning command payload */
} esp_zb_zcl_ias_wd_start_warning_cmd_t;

/**
 * @brief The Zigbee ZCL IAS_WD Squawk command struct
 */
typedef struct esp_zb_zcl_ias_wd_squawk_cmd_s {
    esp_zb_zcl_basic_cmd_t     zcl_basic_cmd;  /*!< Basic command info */
    esp_zb_zcl_address_mode_t  address_mode;   /*!< APS addressing mode constants refer to esp_zb_zcl_address_mode_t */
    esp_zb_zcl_ias_wd_squawk_t payload;        /*!< Squawk command payload */
} esp_zb_zcl_ias_wd_squawk_cmd_t;

/**
 * @brief The Zigbee ZCL IAS_ACE Emergency command struct
 */
typedef esp_zb_zcl_common_cmd_t esp_zb_zcl_ias_ace_emergency_cmd_t;

/**
 * @brief The Zigbee ZCL IAS_ACE Fire command struct
 */
typedef esp_zb_zcl_common_cmd_t esp_zb_zcl_ias_ace_fire_cmd_t;

/**
 * @brief The Zigbee ZCL IAS_ACE Panic command struct
 */
typedef esp_zb_zcl_common_cmd_t esp_zb_zcl_ias_ace_panic_cmd_t;

/**
 * @brief The Zigbee ZCL IAS_ACE Get Zone ID Map command struct
 */
typedef esp_zb_zcl_common_cmd_t esp_zb_zcl_ias_ace_get_zone_id_map_cmd_t;

/**
 * @brief The Zigbee ZCL IAS_ACE Get Zone Information command struct
 */
typedef struct esp_zb_zcl_ias_ace_get_zone_info_cmd_s {
    esp_zb_zcl_basic_cmd_t zcl_basic_cmd;                       /*!< Basic command info */
    esp_zb_zcl_address_mode_t address_mode;                     /*!< APS addressing mode constants refer to esp_zb_zcl_address_mode_t */
    uint8_t zone_id;                                            /*!< ID of the Zone to get the information */
} esp_zb_zcl_ias_ace_get_zone_info_cmd_t;

/**
 * @brief The Zigbee ZCL IAS_ACE Get Panel Status command struct
 */
typedef esp_zb_zcl_common_cmd_t esp_zb_zcl_ias_ace_get_panel_status_cmd_t;

/**
 * @brief The Zigbee ZCL IAS_ACE Get Bypassed Zone List command struct
 */
typedef esp_zb_zcl_common_cmd_t esp_zb_zcl_ias_ace_get_bypassed_zone_list_cmd_t;

/**
 * @brief The Zigbee ZCL IAS_ACE Get Zone Status command struct
 */
typedef struct esp_zb_zcl_ias_ace_get_zone_status_cmd_s {
    esp_zb_zcl_basic_cmd_t zcl_basic_cmd;                       /*!< Basic command info */
    esp_zb_zcl_address_mode_t address_mode;                     /*!< APS addressing mode constants refer to esp_zb_zcl_address_mode_t */
    esp_zb_zcl_ias_ace_get_zone_status_t payload;               /*!< Get Zone Status command payload */
} esp_zb_zcl_ias_ace_get_zone_status_cmd_t;

/**
 * @brief The Zigbee ZCL IAS_ACE Zone Status Changed command struct
 */
typedef struct esp_zb_zcl_ias_ace_zone_status_changed_cmd_s {
    esp_zb_zcl_basic_cmd_t zcl_basic_cmd;                       /*!< Basic command info */
    esp_zb_zcl_address_mode_t address_mode;                     /*!< APS addressing mode constants refer to esp_zb_zcl_address_mode_t */
    esp_zb_zcl_ias_ace_zone_status_changed_t payload;           /*!< Zone Status Changed command payload */
} esp_zb_zcl_ias_ace_zone_status_changed_cmd_t;

/**
 * @brief The Zigbee ZCL IAS_ACE Panel Status Changed command struct
 */
typedef struct esp_zb_zcl_ias_ace_panel_status_changed_cmd_s {
    esp_zb_zcl_basic_cmd_t zcl_basic_cmd;                       /*!< Basic command info */
    esp_zb_zcl_address_mode_t address_mode;                     /*!< APS addressing mode constants refer to esp_zb_zcl_address_mode_t */
    esp_zb_zcl_ias_ace_panel_status_changed_t payload;          /*!< Panel Status Changed command payload */
} esp_zb_zcl_ias_ace_panel_status_changed_cmd_t;

/**
 * @brief The Zigbee ZCL window covering send command struct
 *
 * @note value only support uint8_t, uint16_t data types for the Lift/Tilt value/percentage payload. If not, set to NULL
 *
 */
typedef struct esp_zb_zcl_window_covering_cluster_send_cmd_req_s {
    esp_zb_zcl_basic_cmd_t zcl_basic_cmd;                       /*!< Basic command info */
    esp_zb_zcl_address_mode_t address_mode;                     /*!< APS addressing mode constants refer to esp_zb_zcl_address_mode_t */
    void *value;                                                /*!< Pointer to payload value of window covering command */
    uint8_t cmd_id;                                             /*!< Command id, refer to esp_zb_zcl_window_covering_cmd_t */
} esp_zb_zcl_window_covering_cluster_send_cmd_req_t;

/**
 * @brief The Zigbee ZCL electrical profile information response command struct
 */
typedef struct esp_zb_zcl_electrical_profile_info_cmd_resp_s {
    esp_zb_zcl_basic_cmd_t zcl_basic_cmd;                       /*!< Basic command info */
    esp_zb_zcl_address_mode_t address_mode;                     /*!< APS addressing mode constants refer to esp_zb_zcl_address_mode_t */
    esp_zb_electrical_measurement_profile_info_t profile_info;  /*!< Electrical profile info response command */
} esp_zb_zcl_electrical_profile_info_cmd_resp_t;

/**
 * @brief The Zigbee ZCL electrical profile response command struct
 */
typedef struct esp_zb_zcl_electrical_measurement_profile_cmd_resp_s {
    esp_zb_zcl_basic_cmd_t zcl_basic_cmd;                        /*!< Basic command info */
    esp_zb_zcl_address_mode_t address_mode;                      /*!< APS addressing mode constants refer to esp_zb_zcl_address_mode_t */
    esp_zb_electrical_measurement_profile_t profile;             /*!< Electrical profile response command */
} esp_zb_zcl_electrical_measurement_profile_cmd_resp_t;

/**
 * @brief The Zigbee ZCL thermostat setpoint raise lower request command struct
 */
typedef struct esp_zb_zcl_thermostat_setpoint_raise_lower_request_cmd_s {
    esp_zb_zcl_basic_cmd_t zcl_basic_cmd;                      /*!< Basic command info */
    esp_zb_zcl_address_mode_t address_mode;                    /*!< APS addressing mode constants refer to esp_zb_zcl_address_mode_t */
    uint8_t mode;                                              /*!< Mode field SHALL be set to Heat(0x00), Cool(0x01) and Both(0x02). It specifies which setpoint is to be configured */
    int8_t amount;                                             /*!< Amount field specifies the setpoint(s) are to be increased (or decreased) by, in steps of 0.1 degree Celsius */
} esp_zb_zcl_thermostat_setpoint_raise_lower_request_cmd_t;

/**
 * @brief The Zigbee ZCL thermostat set weekly schedule request command struct
 */
typedef struct esp_zb_zcl_thermostat_set_weekly_schedule_request_cmd_s {
    esp_zb_zcl_basic_cmd_t zcl_basic_cmd;                      /*!< Basic command info */
    esp_zb_zcl_address_mode_t address_mode;                    /*!< APS addressing mode constants refer to esp_zb_zcl_address_mode_t */
    uint8_t num_of_transitions;                                /*!< Number of transitions for sequence field indicates how many individual transitions to expect for this sequence of commands */
    uint8_t day_of_week;                                       /*!< Day of week for sequence field represents the day of the week at which all the transitions within the payload of the command SHOULD be associated to */
    uint8_t mode_for_seq;                                      /*!< Mode for sequence field determines how the application SHALL decode the Set Point Fields of each transition for the remaining of the command */
    uint16_t transition_time;                                  /*!< Transition time field represents the start time of the schedule transition during the associated day */
    uint16_t heat_set_point;                                   /*!< Heat set point field represents the heat setpoint(0.01 degree Celsius resolution) to be applied, if the heat bit is enabled in the Mode For Sequence byte */
    uint16_t cool_set_point;                                   /*!< Cool set point field represents the cool setpoint(0.01 degree Celsius resolution) to be applied, if the cool bit is enabled in the Mode For Sequence byte */
} esp_zb_zcl_thermostat_set_weekly_schedule_request_cmd_t;

/**
 * @brief The Zigbee ZCL thermostat get weekly schedule request command struct
 */
typedef struct esp_zb_zcl_thermostat_get_weekly_schedule_request_cmd_s {
    esp_zb_zcl_basic_cmd_t zcl_basic_cmd;                      /*!< Basic command info */
    esp_zb_zcl_address_mode_t address_mode;                    /*!< APS addressing mode constants refer to esp_zb_zcl_address_mode_t */
    uint8_t days_to_return;                                    /*!< Days to return field indicates the number of days the client would like to return the set point values for */
    uint8_t mode_to_return;                                    /*!< Mode to return field indicates the mode(heat only, cool only or heat & cool) the client would like to return the set point values for */
} esp_zb_zcl_thermostat_get_weekly_schedule_request_cmd_t;

/**
 * @brief The Zigbee ZCL thermostat clear weekly schedule request command struct
 */
typedef struct esp_zb_thermostat_clear_weekly_schedule_cmd_s {
    esp_zb_zcl_basic_cmd_t zcl_basic_cmd;                   /*!< Basic command info */
    esp_zb_zcl_address_mode_t address_mode;                 /*!< APS addressing mode constants refer to esp_zb_zcl_address_mode_t */
} esp_zb_thermostat_clear_weekly_schedule_cmd_t;

/**
 * @brief The Zigbee ZCL thermostat get relay status log request command struct
 */
typedef struct esp_zb_thermostat_get_relay_status_log_cmd_s {
    esp_zb_zcl_basic_cmd_t zcl_basic_cmd;                   /*!< Basic command info */
    esp_zb_zcl_address_mode_t address_mode;                 /*!< APS addressing mode constants refer to esp_zb_zcl_address_mode_t */
} esp_zb_thermostat_get_relay_status_log_cmd_t;

/**
 * @brief The Zigbee ZCL metering get profile request command struct
 */
typedef struct esp_zb_metering_get_profile_cmd_s {
    esp_zb_zcl_basic_cmd_t zcl_basic_cmd;                    /*!< Basic command info */
    esp_zb_zcl_address_mode_t address_mode;                  /*!< APS addressing mode constants refer to esp_zb_zcl_address_mode_t */
    esp_zb_zcl_metering_interval_channel_t interval_channel; /*!< Interval channel is used to select the quantity of interest by the 'GetProfileResponse' command */
    uint32_t end_time;                                       /*!< End time is a 32-bit value (in UTC) used to select an Intervals block from all the Intervals blocks available */
    uint8_t number_of_periods;                               /*!< Number of periods represents the number of intervals being requested, which cannot exceed MaxNumberOfPeriodsDelivered */
} esp_zb_metering_get_profile_cmd_t;

/**
 * @brief The Zigbee ZCL metering request fast poll mode command struct
 */
typedef struct esp_zb_metering_request_fast_poll_mode_cmd_s {
    esp_zb_zcl_basic_cmd_t zcl_basic_cmd;   /*!< Basic command info */
    esp_zb_zcl_address_mode_t address_mode; /*!< APS addressing mode constants refer to esp_zb_zcl_address_mode_t */
    uint8_t fast_poll_update_period;        /*!< Desired fast poll period (seconds) not to be less than the FastPollUpdatePeriod attribute */
    uint8_t duration;                       /*!< Desired duration (minutes) for the server to remain in fast poll mode not to exceed 15 minutes */
} esp_zb_metering_request_fast_poll_mode_cmd_t;

/**
 * @brief The Zigbee ZCL metering get snapshot command struct
 */
typedef struct esp_zb_metering_get_snapshot_cmd_s {
    esp_zb_zcl_basic_cmd_t zcl_basic_cmd;                /*!< Basic command info */
    esp_zb_zcl_address_mode_t address_mode;              /*!< APS addressing mode constants refer to esp_zb_zcl_address_mode_t */
    uint32_t earliest_start_time;                        /*!< A UTC Timestamp indicating the earliest time of a snapshot to be returned by a corresponding Publish Snapshot command */
    uint32_t latest_end_time;                            /*!< A UTC Timestamp indicating the latest time of a snapshot to be returned by a corresponding Publish Snapshot command */
    uint8_t snapshot_offset;                             /*!< This field identifies the individual snapshot to be returned, where multiple snapshots satisfy the selection criteria specified by the other fields in this command */
    esp_zb_zcl_metering_snapshot_cause_t snapshot_cause; /*!< This field is used to select only snapshots that were taken due to a specific cause,
                                                              setting 0xFFFFFFFF indicates that all snapshots should be selected, irrespective of the cause */
} esp_zb_metering_get_snapshot_cmd_t;

/**
 * @brief The Zigbee ZCL metering get sampled data command struct
 */
typedef struct esp_zb_metering_get_sampled_data_cmd_s {
    esp_zb_zcl_basic_cmd_t zcl_basic_cmd;          /*!< Basic command info */
    esp_zb_zcl_address_mode_t address_mode;        /*!< APS addressing mode constants refer to esp_zb_zcl_address_mode_t */
    uint16_t sample_id;                            /*!< Unique identifier allocated to this Sampling session. This field allows devices to match response data with the appropriate request */
    uint32_t earliest_sample_time;                 /*!< A UTC Timestamp indicating the earliest time of a sample to be returned. Samples with a timestamp equal to or greater than the specified EarliestSampleTime shall be returned */
    esp_zb_zcl_metering_sample_type_t sample_type; /*!< Sample_type identifies the required type of sampled data */
    uint16_t number_of_samples;                    /*!< This filed represents the number of samples being requested. This value cannot exceed the size stipulated in the MaxNumberOfSamples field.
                                                        If fewer samples are available for the time period, only those available are returned */
} esp_zb_metering_get_sampled_data_cmd_t;

/**
 * @brief The Zigbee ZCL price get current price command struct
 */
typedef struct esp_zb_zcl_price_get_current_price_cmd_s {
    esp_zb_zcl_basic_cmd_t zcl_basic_cmd;     /*!< Basic command info */
    esp_zb_zcl_address_mode_t address_mode;   /*!< APS addressing mode constants refer to esp_zb_zcl_address_mode_t */
    uint8_t command_options;                  /*!< Zero bit is Requestor Rx On When Idle, others are reserved. */
} esp_zb_zcl_price_get_current_price_cmd_t;

/**
 * @brief The Zigbee ZCL price get scheduled prices command struct
 */
typedef struct esp_zb_zcl_price_get_scheduled_prices_cmd_s {
    esp_zb_zcl_basic_cmd_t zcl_basic_cmd;       /*!< Basic command info */
    esp_zb_zcl_address_mode_t address_mode;     /*!< APS addressing mode constants refer to esp_zb_zcl_address_mode_t */
    uint32_t start_time;                        /*!< UTC timestamp represents the minimum ending time for any scheduled or
                                                 *   currently active pricing events to be resent.
                                                 *   @note Value 0x00000000 will be replaced with the current time stamp.
                                                 */
    uint8_t  number_of_events;                  /*!< NumberOfEvents represents the maximum number of events to be sent.
                                                 *   @note Value 0 indicates no maximum limit.
                                                 */
} esp_zb_zcl_price_get_scheduled_prices_cmd_t;

/**
 * @brief The Zigbee ZCL price get tier labels command struct
 */
typedef struct esp_zb_zcl_price_get_tier_labels_cmd_s {
    esp_zb_zcl_basic_cmd_t zcl_basic_cmd;       /*!< Basic command info */
    esp_zb_zcl_address_mode_t address_mode;     /*!< APS addressing mode constants refer to esp_zb_zcl_address_mode_t */
    uint32_t issuer_tariff_id;                  /*!< Unique identifier generated by the commodity supplier.
                                                 *   This is used to identify the tariff that the labels apply to.
                                                 */
} esp_zb_zcl_price_get_tier_labels_cmd_t;

/**
 * @brief The Zigbee ZCL DRLC LoadControlEvent command struct
 */
typedef struct esp_zb_zcl_drlc_load_control_event_req_s {
    esp_zb_zcl_basic_cmd_t zcl_basic_cmd;                   /*!< Basic command info */
    esp_zb_zcl_address_mode_t address_mode;                 /*!< APS addressing mode constants refer to esp_zb_zcl_address_mode_t */
    esp_zb_zcl_drlc_load_control_event_payload_t payload;   /*!< The payload of LoadControlEvent, @see esp_zb_zcl_drlc_load_control_event_payload_t */
} esp_zb_zcl_drlc_load_control_event_req_t;

/**
 * @brief The Zigbee ZCL DRLC CancelLoadControlEvent command struct
 */
typedef struct esp_zb_zcl_drlc_cancel_load_control_event_req_s {
    esp_zb_zcl_basic_cmd_t zcl_basic_cmd;                       /*!< Basic command info */
    esp_zb_zcl_address_mode_t address_mode;                     /*!< APS addressing mode constants refer to esp_zb_zcl_address_mode_t */
    esp_zb_zcl_drlc_cancel_load_control_event_payload_t payload;/*!< The payload of CancelLoadControlEvent, @see esp_zb_zcl_drlc_cancel_load_control_event_payload_t */
} esp_zb_zcl_drlc_cancel_load_control_event_req_t;

/**
 * @brief The Zigbee ZCL DRLC CancelAllLoadControlEvents command struct
 */
typedef struct esp_zb_zcl_drlc_cancel_all_load_control_events_req_s {
    esp_zb_zcl_basic_cmd_t zcl_basic_cmd;                               /*!< Basic command info */
    esp_zb_zcl_address_mode_t address_mode;                             /*!< APS addressing mode constants refer to esp_zb_zcl_address_mode_t */
    esp_zb_zcl_drlc_cancel_all_load_control_events_payload_t  payload;  /*!< The payload of CancelAllLoadControlEvents, @see esp_zb_zcl_drlc_cancel_all_load_control_events_payload_t */
} esp_zb_zcl_drlc_cancel_all_load_control_events_req_t;

/**
 * @brief The Zigbee ZCL DRLC ReportEventStatus command struct
 */
typedef struct esp_zb_zcl_drlc_report_event_status_req_s {
    esp_zb_zcl_basic_cmd_t zcl_basic_cmd;                   /*!< Basic command info */
    esp_zb_zcl_address_mode_t address_mode;                 /*!< APS addressing mode constants refer to esp_zb_zcl_address_mode_t */
    esp_zb_zcl_drlc_report_event_status_payload_t payload;  /*!< The payload of ReportEventStatus, @see esp_zb_zcl_drlc_report_event_status_payload_t */
} esp_zb_zcl_drlc_report_event_status_req_t;

/**
 * @brief The Zigbee ZCL DRLC GetScheduledEvents command struct
 */
typedef struct esp_zb_zcl_drlc_get_scheduled_events_req_s {
    esp_zb_zcl_basic_cmd_t zcl_basic_cmd;                   /*!< Basic command info */
    esp_zb_zcl_address_mode_t address_mode;                 /*!< APS addressing mode constants refer to esp_zb_zcl_address_mode_t */
    esp_zb_zcl_drlc_get_scheduled_events_payload_t payload; /*!< The payload of GetScheduledEvents, @see esp_zb_zcl_drlc_get_scheduled_events_payload_t */
} esp_zb_zcl_drlc_get_scheduled_events_req_t;

/**
 * @brief The Zigbee ZCL custom cluster command struct
 *
 * @note For string data type, the first byte should be the length of string.
 *       For array, array16, array32, and long string data types, the first 2 bytes should represent the number of elements in the array.
 *       The size of data is only required when the type is ESP_ZB_ZCL_ATTR_TYPE_SET, ESP_ZB_ZCL_ATTR_TYPE_BAG, or CUSTOM_TYPE.
 *
 */
typedef struct esp_zb_zcl_custom_cluster_cmd_s {
    esp_zb_zcl_basic_cmd_t zcl_basic_cmd;                   /*!< Basic command info */
    esp_zb_zcl_address_mode_t address_mode;                 /*!< APS addressing mode constants refer to esp_zb_zcl_address_mode_t */
    uint16_t profile_id;                                    /*!< Profile id */
    uint16_t cluster_id;                                    /*!< Cluster id */
    struct {
        uint8_t manuf_specific   : 2;                       /*!< Sent as manufacturer extension with code. */
        uint8_t direction        : 1;                       /*!< The command direction, refer to esp_zb_zcl_cmd_direction_t */
        uint8_t dis_defalut_resp : 1;                       /*!< Disable default response for this command. */
    };
    uint16_t manuf_code;                                    /*!< The manufacturer code sent with the command. */
    uint16_t custom_cmd_id;                                 /*!< Custom command id */
    struct {
        esp_zb_zcl_attr_type_t type;                        /*!< The type of custom data, refer to esp_zb_zcl_attr_type_t */
        uint16_t size;                                      /*!< The size of custom data */
        void *value;                                        /*!< The value of custom data */
    } data;                                                 /*!< The custom data */
} esp_zb_zcl_custom_cluster_cmd_t;

#ifdef CONFIG_ZB_GP_ENABLED
/**
 * @brief The request structure for GPP commissioning enter
 *
 */
typedef struct esp_zgp_zcl_proxy_commissioning_enter_s {
    uint8_t exit_mode;              /*!< Exit mode, refer to esp_zgp_commissioning_exit_mode_t */
    uint16_t commissioning_window;  /*!< Commissioning window */
} esp_zgp_zcl_proxy_commissioning_enter_req_t;

/**
 * @brief The request structure for GPP commissioning
 *
 */
typedef struct esp_zgp_zcl_proxy_commissioning_s {
    uint8_t options;                /*!< Option: Exit mode(3rd bit), Channel present(4th bit), Reserved(5-7th bit) */
    uint8_t channel;                /*!< The devices should stay on the operational channel.*/
    uint16_t commissioning_window;  /*!< Flag of the Exit mode sub-field is set to 0b1 */
} esp_zgp_zcl_proxy_commissioning_req_t;

/**
 * @brief The request structure for managing pairing information by GPS
 *
 */
typedef struct esp_zgp_zcl_pairing_req_s {
    esp_zb_zcl_basic_cmd_t zcl_basic_cmd;   /*!< Basic command info */
    esp_zb_zcl_address_mode_t address_mode; /*!< APS addressing mode constants refer to esp_zb_zcl_address_mode_t */
    uint32_t options;                       /*!< 24-bit indicates the properties of request */
    esp_zb_zgpd_addr_t zgpd_id;             /*!< The value of the GPD identifier */
    uint8_t endpoint;                       /*!< Endpoint of ZGPD if APP_ID eq 0010 */
    uint8_t dev_id;                         /*!< Device identifier */
    uint8_t *key;                           /*!< The security key */
    uint8_t forward_radius;                 /*!< Radius in the groupcast forwarding of the GPDF packet */
    uint16_t assigned_alias;                /*!< Assigned alias in the groupcast forwarding of the GPDF packet */
    uint16_t group_id;                      /*!< Group identifier */
    uint32_t sec_frame_counter;             /*!< Security frame counter of ZGPD */
} esp_zgp_zcl_pairing_req_t;

/**
 * @brief The request structure for sink table information
 *
 */
typedef struct esp_zgp_zcl_gp_sink_table_req_s {
    esp_zb_zcl_basic_cmd_t zcl_basic_cmd;   /*!< Basic command info */
    esp_zb_zcl_address_mode_t address_mode; /*!< APS addressing mode constants refer to esp_zb_zcl_address_mode_t */
    uint8_t index;                          /*!< The index of request sink table */
    uint8_t options;                        /*!< Option: app_id(1-3rd bit), refer to esp_zb_zgp_app_id_t. req_type(4th bit), refer to esp_zgp_table_request_entries_type_t */
    esp_zb_zgpd_id_t *zgpd_id;              /*!< The green power device identifier */
} esp_zgp_zcl_gp_sink_table_req_t;

/**
 * @brief The request structure for proxy table information
 *
 */
typedef esp_zgp_zcl_gp_sink_table_req_t esp_zgp_zcl_gp_proxy_table_req_t;

/**
 * @brief The request structure for configuring the Sink Table of a GPS
 *
 */
typedef struct esp_zgp_zcl_pairing_configuration_req_s {
    esp_zb_zcl_basic_cmd_t zcl_basic_cmd;           /*!< Basic command info */
    esp_zb_zcl_address_mode_t address_mode;         /*!< APS addressing mode constants refer to esp_zb_zcl_address_mode_t */
    esp_zb_zgp_sink_tbl_ent_t *ent;                 /*!< Sink table entry */
    uint8_t actions;                                /*!< The configuration action, refer to esp_zgp_pairing_config_action_t */
    uint8_t num_paired_endpoints;                   /*!< The number of endpoints listed in the Paired endpoints field */
    uint8_t *paired_endpoints;                      /*!< The paired endpoints */
    uint8_t app_info;                               /*!< Application infomation */
    uint8_t num_gpd_commands;                       /*!< The number of gpd command listed in the gpd_command field */
    uint8_t *gpd_commands;                          /*!< The green power command list */
} esp_zgp_zcl_pairing_configuration_req_t;
#endif /* CONFIG_ZB_GP_ENABLED */

/**
 * @brief The Zigbee ZCL custom cluster request command struct
 *
 */
typedef esp_zb_zcl_custom_cluster_cmd_t esp_zb_zcl_custom_cluster_cmd_req_t;

/**
 * @brief The Zigbee ZCL custom cluster response command struct
 *
 */
typedef esp_zb_zcl_custom_cluster_cmd_t esp_zb_zcl_custom_cluster_cmd_resp_t;

/*********************** User Message *****************************/
/**
 * @brief The Zigbee zcl cluster device callback common information
 *
 */
typedef struct esp_zb_device_cb_common_info_s {
    esp_zb_zcl_status_t status;                 /*!< The operation status of ZCL, refer to esp_zb_zcl_status_t */
    uint8_t dst_endpoint;                       /*!< The destination endpoint id of the ZCL indication */
    uint16_t cluster;                           /*!< The cluster id of the ZCL indication */
} esp_zb_device_cb_common_info_t;

/**
 * @brief The Zigbee zcl set attribute value device callback message struct
 *
 */
typedef struct esp_zb_zcl_set_attr_value_message_s {
    esp_zb_device_cb_common_info_t info;        /*!< The common information for Zigbee device callback */
    esp_zb_zcl_attribute_t attribute;           /*!< The attribute entry whose value is set */
} esp_zb_zcl_set_attr_value_message_t;

/**
 * @brief The Zigbee zcl scene cluster store scene device callback message struct
 *
 */
typedef struct esp_zb_zcl_store_scene_message_s {
    esp_zb_device_cb_common_info_t info;        /*!< The common information for Zigbee device callback */
    esp_zb_zcl_command_t command;               /*!< The ZCL indication for command */
    uint16_t group_id;                          /*!< The group id of Zigbee scenes cluster */
    uint8_t scene_id;                           /*!< The scene id of Zigbee scenes cluster */
} esp_zb_zcl_store_scene_message_t;

/**
 * @brief The Zigbee zcl scene cluster recall scene device callback message struct
 *
 */
typedef struct esp_zb_zcl_recall_scene_message_s {
    esp_zb_device_cb_common_info_t info;            /*!< The common information for Zigbee device callback */
    esp_zb_zcl_command_t command;                   /*!< The ZCL indication for command */
    uint16_t group_id;                              /*!< The group id of Zigbee scenes cluster */
    uint8_t scene_id;                               /*!< The scene id of Zigbee scenes cluster */
    uint16_t transition_time;                       /*!< The recall transition time of Zigbee scenes cluster */
    esp_zb_zcl_scenes_extension_field_t *field_set; /*!< The extension field of Zigbee scenes cluster,{{cluster_id, length, value},..., {cluster_id,
                                                       length, value}}, note that the `NULL` is the end of field */
} esp_zb_zcl_recall_scene_message_t;

/**
 * @brief The Zigbee zcl ias zone cluster enroll response device callback message struct
 *
 */
typedef struct esp_zb_zcl_ias_zone_enroll_response_message_s {
    esp_zb_device_cb_common_info_t info;        /*!< The common information for Zigbee device callback */
    uint8_t response_code;                      /*!< The response code of Zigbee ias zone cluster */
    uint8_t zone_id;                            /*!< The id of Zigbee ias zone cluster, refer to esp_zb_zcl_ias_zone_enroll_response_code_t  */
} esp_zb_zcl_ias_zone_enroll_response_message_t;

/**
 * @brief The Zigbee ZCL IAS_ACE Arm Response message struct
 *
 */
typedef struct esp_zb_zcl_ias_ace_arm_response_message_s {
    esp_zb_device_cb_common_info_t info;            /*!< The common information for Zigbee device callback */
    const esp_zb_zcl_ias_ace_arm_resp_t *msg_in;    /*!< Received Arm Response command payload */
} esp_zb_zcl_ias_ace_arm_response_message_t;

/**
 * @brief The Zigbee ZCL IAS_ACE Get Zone ID Map Response message struct
 *
 */
typedef struct esp_zb_zcl_ias_ace_get_zone_id_map_response_message_s {
    esp_zb_device_cb_common_info_t info;                        /*!< The common information for Zigbee device callback */
    const esp_zb_zcl_ias_ace_get_zone_id_map_resp_t *msg_in;    /*!< Received Get Zone ID Map Response command payload */
} esp_zb_zcl_ias_ace_get_zone_id_map_response_message_t;

/**
 * @brief The Zigbee ZCL IAS_ACE Get Zone Information Response message struct
 *
 */
typedef struct esp_zb_zcl_ias_ace_get_zone_info_response_message_s {
    esp_zb_device_cb_common_info_t info;                        /*!< The common information for Zigbee device callback */
    const esp_zb_zcl_ias_ace_get_zone_info_resp_t *msg_in;      /*!< Received Get Zone Information Response command payload */
} esp_zb_zcl_ias_ace_get_zone_info_response_message_t;

/**
 * @brief The Zigbee ZCL IAS_ACE Zone Status Changed message struct
 *
 */
typedef struct esp_zb_zcl_ias_ace_zone_status_changed_message_s {
    esp_zb_device_cb_common_info_t info;                        /*!< The common information for Zigbee device callback */
    const esp_zb_zcl_ias_ace_zone_status_changed_t *msg_in;     /*!< Received Zone Status Changed command payload */
} esp_zb_zcl_ias_ace_zone_status_changed_message_t;

/**
 * @brief The Zigbee ZCL IAS_ACE Panel Status Changed message struct
 *
 */
typedef struct esp_zb_zcl_ias_ace_panel_status_changed_message_s {
    esp_zb_device_cb_common_info_t info;                        /*!< The common information for Zigbee device callback */
    const esp_zb_zcl_ias_ace_panel_status_changed_t *msg_in;    /*!< Received Panel Status Changed command payload */
} esp_zb_zcl_ias_ace_panel_status_changed_message_t;

/**
 * @brief The Zigbee ZCL IAS_ACE Get Panel Status Response message struct
 *
 */
typedef struct esp_zb_zcl_ias_ace_get_panel_status_response_message_s {
    esp_zb_device_cb_common_info_t info;                        /*!< The common information for Zigbee device callback */
    const esp_zb_zcl_ias_ace_get_panel_status_resp_t *msg_in;   /*!< Received Get Panel Status Response command payload */
} esp_zb_zcl_ias_ace_get_panel_status_response_message_t;

/**
 * @brief The Zigbee ZCL IAS_ACE Set Bypassed Zone List message struct
 *
 */
typedef struct esp_zb_zcl_ias_ace_set_bypassed_zone_list_message_s {
    esp_zb_device_cb_common_info_t info;                        /*!< The common information for Zigbee device callback */
    const esp_zb_zcl_ias_ace_set_bypassed_zone_list_t *msg_in;  /*!< Received Set Bypassed Zone List command payload */
} esp_zb_zcl_ias_ace_set_bypassed_zone_list_message_t;

/**
 * @brief The Zigbee ZCL IAS_ACE Bypass Response message struct
 *
 */
typedef struct esp_zb_zcl_ias_ace_bypass_response_message_s {
    esp_zb_device_cb_common_info_t info;                /*!< The common information for Zigbee device callback */
    const esp_zb_zcl_ias_ace_bypass_resp_t *msg_in;     /*!< Received Bypass Response command payload */
} esp_zb_zcl_ias_ace_bypass_response_message_t;

/**
 * @brief The Zigbee ZCL IAS_ACE Get Zone Status Response message struct
 *
 */
typedef struct esp_zb_zcl_ias_ace_get_zone_status_response_message_s {
    esp_zb_device_cb_common_info_t info;                        /*!< The common information for Zigbee device callback */
    const esp_zb_zcl_ias_ace_get_zone_status_resp_t *msg_in;    /*!< Received Get Zone Status Response command payload */
} esp_zb_zcl_ias_ace_get_zone_status_response_message_t;

/**
 * @brief The Zigbee ZCL IAS_ACE Arm message struct
 *
 */
typedef struct esp_zb_zcl_ias_ace_arm_message_s {
    esp_zb_device_cb_common_info_t info;    /*!< The common information for Zigbee device callback */
    const esp_zb_zcl_ias_ace_arm_t *msg_in; /*!< Received Arm command payload */
    esp_zb_zcl_ias_ace_arm_resp_t *msg_out; /*!< Response of Arm command to be sent */
} esp_zb_zcl_ias_ace_arm_message_t;

/**
 * @brief The Zigbee ZCL IAS_ACE Bypass message struct
 *
 */
typedef struct esp_zb_zcl_ias_ace_bypass_message_s {
    esp_zb_device_cb_common_info_t info;        /*!< The common information for Zigbee device callback */
    const esp_zb_zcl_ias_ace_bypass_t *msg_in;  /*!< Received Bypass command payload */
    esp_zb_zcl_ias_ace_bypass_resp_t *msg_out;  /*!< Response of Bypass command to be sent */
} esp_zb_zcl_ias_ace_bypass_message_t;

/**
 * @brief The Zigbee ZCL IAS_ACE Emergency message struct
 *
 */
typedef struct esp_zb_zcl_ias_ace_emergency_message_s {
    esp_zb_device_cb_common_info_t info;        /*!< The common information for Zigbee device callback */
} esp_zb_zcl_ias_ace_emergency_message_t;

/**
 * @brief The Zigbee ZCL IAS_ACE Fire message struct
 *
 */
typedef esp_zb_zcl_ias_ace_emergency_message_t esp_zb_zcl_ias_ace_fire_message_t;

/**
 * @brief The Zigbee ZCL IAS_ACE Panic message struct
 *
 */
typedef esp_zb_zcl_ias_ace_emergency_message_t esp_zb_zcl_ias_ace_panic_message_t;

/**
 * @brief The Zigbee ZCL IAS_ACE Get Panel Status message struct
 *
 */
typedef struct esp_zb_zcl_ias_ace_get_panel_status_message_s {
    esp_zb_device_cb_common_info_t info;                    /*!< The common information for Zigbee device callback */
    esp_zb_zcl_ias_ace_get_panel_status_resp_t *msg_out;    /*!< Response of Get Panel Status command to be sent */
} esp_zb_zcl_ias_ace_get_panel_status_message_t;

/**
 * @brief The Zigbee ZCL IAS_ACE Get Bypassed Zone List message struct
 *
 */
typedef struct esp_zb_zcl_ias_ace_get_bypassed_zone_list_message_s {
    esp_zb_device_cb_common_info_t info;                            /*!< The common information for Zigbee device callback */
    esp_zb_zcl_ias_ace_set_bypassed_zone_list_message_t *msg_out;   /*!< Response of Get Bypassed Zone List command to be sent */
} esp_zb_zcl_ias_ace_get_bypassed_zone_list_message_t;

/**
 * @brief The Zigbee ZCL IAS_ACE Get Zone Status message struct
 *
 */
typedef struct esp_zb_zcl_ias_ace_get_zone_status_message_s {
    esp_zb_device_cb_common_info_t info;                    /*!< The common information for Zigbee device callback */
    const esp_zb_zcl_ias_ace_get_zone_status_t *msg_in;     /*!< Received Get Zone Status command payload */
    esp_zb_zcl_ias_ace_get_zone_status_resp_t *msg_out;     /*!< Response of Get Zone Status command to be sent */
} esp_zb_zcl_ias_ace_get_zone_status_message_t;

/**
 * @brief The Zigbee ZCL IAS_WD Start Warning message struct
 *
 */
typedef struct esp_zb_zcl_ias_wd_start_warning_message_s {
    esp_zb_device_cb_common_info_t info;                /**< The common information for Zigbee device callback */
    const esp_zb_zcl_ias_wd_start_warning_t *msg_in;    /**< Received Start Warning command, refer to esp_zb_zcl_ias_wd_start_warning_t */
} esp_zb_zcl_ias_wd_start_warning_message_t;

/**
 * @brief The Zigbee ZCL IAS_WD Squawk message struct
 *
 */
typedef struct esp_zb_zcl_ias_wd_squawk_message_s {
    esp_zb_device_cb_common_info_t info;        /**< The common information for Zigbee device callback */
    const esp_zb_zcl_ias_wd_squawk_t *msg_in;   /**< Received Squawk command, refer to esp_zb_zcl_ias_wd_squawk_t */
} esp_zb_zcl_ias_wd_squawk_message_t;

/**
 * @brief The Zigbee zcl ota upgrade value device callback message struct
 *
 */
typedef struct esp_zb_zcl_ota_upgrade_value_message_s {
    esp_zb_device_cb_common_info_t info;             /*!< The common information for Zigbee device callback */
    esp_zb_zcl_ota_upgrade_status_t upgrade_status;  /*!< The update status for Zigbee ota update */
    esp_zb_ota_file_header_t ota_header;             /*!< The header indicates the basic OTA upgrade information */
    uint16_t payload_size;                           /*!< The OTA payload size */
    uint8_t *payload;                                /*!< The OTA payload */
} esp_zb_zcl_ota_upgrade_value_message_t;

/**
 * @brief The Zigbee zcl ota upgrade client query image response message struct
 *
 */
typedef struct esp_zb_zcl_ota_upgrade_query_image_resp_message_s {
    esp_zb_device_cb_common_info_t info;    /*!< The common information for Zigbee device callback */
    esp_zb_zcl_addr_t server_addr;          /*!< Server address */
    uint8_t server_endpoint;                /*!< Server endpoint */
    uint8_t query_status;                   /*!< Status, see esp_zb_zcl_status_t */
    uint16_t manufacturer_code;             /*!< Manufacturer code */
    uint16_t image_type;                    /*!< Image type */
    uint32_t file_version;                  /*!< File version */
    uint32_t image_size;                    /*!< Image size */
} esp_zb_zcl_ota_upgrade_query_image_resp_message_t;

/**
 * @brief The Zigbee zcl ota upgrade server status message struct
 *
 */
typedef struct esp_zb_zcl_ota_upgrade_server_status_message_s {
    esp_zb_device_cb_common_info_t info;              /*!< The common information for Zigbee device callback */
    esp_zb_zcl_addr_t zcl_addr;                       /*!< The address information is sourced from the OTA upgrade client */
    esp_zb_ota_upgrade_server_status_t server_status; /*!< The status of OTA upgrade server, which can refer to esp_zb_ota_upgrade_server_status_t */
    uint16_t image_type;                              /*!< The image type of OTA file */
    uint32_t version;                                 /*!< The version of OTA file */
    uint32_t *upgrade_time;                           /*!< The upgrade time of OTA file, which indicates the interval time when the received OTA image will be updated */
} esp_zb_zcl_ota_upgrade_server_status_message_t;

/**
 * @brief The Zigbee zcl ota upgrade server query image message struct
 *
 */
typedef struct esp_zb_zcl_ota_upgrade_server_query_image_message_s {
    esp_zb_device_cb_common_info_t info; /*!< The common information for Zigbee device callback */
    esp_zb_zcl_addr_t zcl_addr;          /*!< The address information is sourced from the OTA upgrade client */
    uint16_t image_type;                 /*!< The image type of OTA file */
    uint16_t manufacturer_code;          /*!< The manufacturer code of OTA file */
    uint32_t version;                    /*!< The version code of OTA file */
    uint8_t *table_idx;                  /*!< The pointer for the index of variable table */
} esp_zb_zcl_ota_upgrade_server_query_image_message_t;

/**
 * @brief Structure for Thermostat Weekly Schedule Set message
 *
 */
typedef struct esp_zb_zcl_thermostat_weekly_schedule_set_message_s {
    esp_zb_device_cb_common_info_t info;                        /*!< The common information for Zigbee device callback */
    uint32_t trans_status;                                      /*!< The status of the schedule transition, refer to esp_err_t */
    uint8_t day_of_week;                                        /*!< This field is a bitmap represents the day of the week at which all
                                                                     the transitions within the payload of the command SHOULD be associated to,
                                                                     refer to esp_zb_zcl_thermostat_day_of_week_t */
    uint8_t mode_for_req;                                       /*!< This field is a bitmap  determines how the application SHALL decode the Set
                                                                     Point Fields of each transition. @see esp_zb_zcl_thermostat_weekly_schedule_mode_for_seq_t  */
    esp_zb_zcl_thermostat_weekly_schedule_transition_t trans;   /*!< Transition field, refer to esp_zb_zcl_thermostat_weekly_schedule_transition_t */
} esp_zb_zcl_thermostat_weekly_schedule_set_message_t;

/**
 * @brief The Zigbee zcl thermostat value callback message struct
 *
 */
typedef struct esp_zb_zcl_thermostat_value_message_s {
    esp_zb_device_cb_common_info_t info; /*!< The common information for Zigbee device callback */
    uint8_t mode;                        /*!< Mode for Sequence, Heat(0x00), Cool(0x01) and Both(0x02) */
    uint16_t heat_setpoint;              /*!< Heat Set Point */
    uint16_t cool_setpoint;              /*!< Cool Set Point */
} esp_zb_zcl_thermostat_value_message_t;

/**
 * @brief The frame header of Zigbee zcl command struct
 *
 * @note frame control field:
 * |----1 bit---|---------1 bit---------|---1 bit---|----------1 bit-----------|---4 bit---|
 * | Frame type | Manufacturer specific | Direction | Disable Default Response | Reserved  |
 *
 */
typedef struct esp_zb_zcl_frame_header_s {
    uint8_t fc;          /*!< A 8-bit Frame control */
    uint16_t manuf_code; /*!< Manufacturer code */
    uint8_t tsn;         /*!< Transaction sequence number */
    int8_t rssi;         /*!< Signal strength */
} esp_zb_zcl_frame_header_t;

/**
 * @brief The Zigbee zcl metering get profile response info offered by user struct
 *
 */
typedef struct esp_zb_zcl_metering_get_profile_resp_info_offered_s {
    uint32_t end_time;                                                     /*!< It is 32-bit value (in UTC) representing the end time of the most chronologically recent interval being requested */
    esp_zb_zcl_metering_status_field_t status;                             /*!< Status of 'GetProfile' command */
    esp_zb_zcl_metering_profile_interval_period_t profile_interval_period; /*!< Represents the interval or time frame used to capture metered Energy, Gas, and Water consumption for profiling purposes */
    uint8_t number_of_periods_delivered;                                   /*!< Number of periods represents the number of intervals being requested, it cannot exceed the MaxNumberOfPeriodsDelivered attribute.
                                                                                If fewer intervals are available for the time period, only those available are returned */
    esp_zb_uint24_t *intervals;                                            /*!< Series of interval data captured using the period specified by the 'ProfileIntervalPeriod' attribute.
                                                                                The content of the interval data depends of the type of information requested using the Channel field in the 'GetProfile' command,
                                                                                and will represent the change in that information since the previous interval */
} esp_zb_zcl_metering_get_profile_resp_info_offered_t;

/**
 * @brief The Zigbee zcl metering get profile callback message struct
 *
 */
typedef struct esp_zb_zcl_metering_get_profile_message_s {
    esp_zb_device_cb_common_info_t info;                                   /*!< The common information for Zigbee device callback */
    esp_zb_zcl_metering_interval_channel_t interval_channel;               /*!< Interval channel is used to select the quantity of interest by the 'GetProfileResponse' command */
    uint32_t end_time;                                                     /*!< End time is a 32-bit value (in UTC) used to select an Intervals block from all the Intervals blocks available */
    uint8_t number_of_periods;                                             /*!< Number of periods represents the number of intervals being requested */
    esp_zb_zcl_metering_get_profile_resp_info_offered_t resp_info_offered; /*!< The info used for 'GetProfileResponse' command, to response 'GetProfile' command.
                                                                                The info SHOULD be offered by user, otherwise, the response has no sense */
} esp_zb_zcl_metering_get_profile_message_t;

/**
 * @brief The Zigbee zcl metering get profile response callback message struct
 *
 */
typedef struct esp_zb_zcl_metering_get_profile_resp_message_s {
    esp_zb_device_cb_common_info_t info;                                   /*!< The common information for Zigbee device callback */
    uint32_t end_time;                                                     /*!< Represents the end time of the most chronologically recent interval being requested */
    esp_zb_zcl_metering_status_field_t status;                             /*!< Status of GetProfile command */
    esp_zb_zcl_metering_profile_interval_period_t profile_interval_period; /*!< Represents the interval or time frame used to capture metered Energy, Gas, and Water consumption for profiling purposes */
    uint8_t number_of_periods_delivered;                                   /*!< Represents the number of intervals the device returned */
    esp_zb_uint24_t *intervals;                                            /*!< Series of interval data captured using the period specified by the ProfileIntervalPeriod field. The content of the interval data depends of
                                                                                the type of information requested using the Channel field in GetProfile command, and will represent the change in that information since the previous interval */
} esp_zb_zcl_metering_get_profile_resp_message_t;

/**
 * @brief The Zigbee zcl metering get profile response info offered by user struct
 *
 */
typedef struct esp_zb_zcl_metering_request_fast_poll_mode_resp_info_offered_s {
    uint8_t applied_update_period_in_seconds; /*!< The period at which metering data shall be updated, shall be greater than or equal to the minimum FastPollUpdatePeriod Attribute
                                                   and less than or equal to the FastPollUpdatePeriod in RequestFastPollMode command */
    uint32_t fast_poll_mode_end_time;         /*!< UTC time that indicates when the metering server will terminate fast poll mode
                                                   and resume updating at the rate specified by DefaultUpdatePeriod */
} esp_zb_zcl_metering_request_fast_poll_mode_resp_info_offered_t;

/**
 * @brief The Zigbee zcl metering request fast poll mode callback message struct
 *
 */
typedef struct esp_zb_zcl_metering_request_fast_poll_mode_message_s {
    esp_zb_device_cb_common_info_t info;                                              /*!< The common information for Zigbee device callback */
    uint8_t fast_poll_update_period;                                                  /*!< Desired fast poll period (seconds) not to be less than the FastPollUpdatePeriod attribute */
    uint8_t duration;                                                                 /*!< Desired duration (minutes) for the server to remain in fast poll mode not to exceed 15 minutes */
    esp_zb_zcl_metering_request_fast_poll_mode_resp_info_offered_t resp_info_offered; /*!< The info used for 'RequestFastPollModeResponse' command, to response 'RequestFastPollMode' command.
                                                                                           The info SHOULD be offered by user, otherwise, the response has no sense */
} esp_zb_zcl_metering_request_fast_poll_mode_message_t;

/**
 * @brief The Zigbee zcl metering request fast poll mode response callback message struct
 *
 */
typedef struct esp_zb_zcl_metering_request_fast_poll_mode_resp_message_s {
    esp_zb_device_cb_common_info_t info;      /*!< The common information for Zigbee device callback */
    uint8_t applied_update_period_in_seconds; /*!< The period at which metering data shall be updated, shall be greater than or equal to the minimum FastPollUpdatePeriod Attribute
                                                   and less than or equal to the FastPollUpdatePeriod in 'RequestFastPollMode' command */
    uint32_t fast_poll_mode_end_time;         /*!< UTC time that indicates when the metering server will terminate fast poll mode
                                                   and resume updating at the rate specified by DefaultUpdatePeriod */
} esp_zb_zcl_metering_request_fast_poll_mode_resp_message_t;

/**
 * @brief The Zigbee zcl metering snapshot tou delivered sub-payload struct
 *
 */
typedef struct esp_zb_zcl_metering_tou_delivered_payload_s {
    esp_zb_uint48_t current_summation_delivered;  /*!< An unsigned 48-bit integer that returns the value of the CurrentSummationDelivered attribute at the stated snapshot timestamp */
    uint32_t bill_to_date_delivered;              /*!< An unsigned 32-bit integer that provides a value for the costs in the current billing period */
    uint32_t bill_to_date_time_stamp_delivered;   /*!< A UTC timestamp that indicates when the value of the associated BillToDateDelivered parameter was last updated */
    uint32_t projected_bill_delivered;            /*!< An unsigned 32-bit integer that provides a value indicating what the estimated state of the account will be at the end of the billing period based on past consumption */
    uint32_t projected_bill_time_stamp_delivered; /*!< A UTC timestamp that indicates when the associated ProjectedBillDelivered parameter was last updated */
    uint8_t bill_delivered_trailing_digit;        /*!< An 8-bit BitMap used to determine where the decimal point is located in the BillToDateDelivered and ProjectedBillDelivered fields */
    uint8_t number_of_tiers_in_use;               /*!< An 8-bit integer representing the number of tiers in use at the time the snapshot was taken */
    esp_zb_uint48_t *tier_summation;              /*!< The Publish Snapshot command contains N elements of CurrentTierNSummationDelivered attributes from the TOU Information Set.
                                                       The Metering server shall send only the number of tiers in use, as stated in this command */
} ESP_ZB_PACKED_STRUCT esp_zb_zcl_metering_tou_delivered_payload_t;

/**
 * @brief The Zigbee zcl metering snapshot tou received sub-payload struct
 *
 */
typedef struct esp_zb_zcl_metering_tou_received_payload_s {
    esp_zb_uint48_t current_summation_received;  /*!< An unsigned 48-bit integer that returns the value of the CurrentSummationReceived attribute at the stated snapshot timestamp */
    uint32_t bill_to_date_received;              /*!< An unsigned 32-bit integer that provides a value for the costs in the current billing period */
    uint32_t bill_to_date_time_stamp_received;   /*!< A UTC timestamp that indicates when the value of the associated BillToDateReceived parameter was last updated */
    uint32_t projected_bill_received;            /*!< An unsigned 32-bit integer that provides a value indicating what the estimated state of the account will be at the end of the billing period based on past generation */
    uint32_t projected_bill_time_stamp_received; /*!< A UTC timestamp that indicates when the associated ProjectedBillReceived parameter was last updated */
    uint8_t bill_received_trailing_digit;        /*!< An 8-bit BitMap used to determine where the decimal point is located in the BillToDateReceived and ProjectedBillReceived fields */
    uint8_t number_of_tiers_in_use;              /*!< An 8-bit integer representing the number of tiers in use at the time the snapshot was taken */
    esp_zb_uint48_t *tier_summation;             /*!< The Publish Snapshot command contains N elements of CurrentTierNSummationReceived attributes from the TOU Information Set.
                                                      The Metering server shall send only the number of tiers in use, as stated in this command */
} ESP_ZB_PACKED_STRUCT esp_zb_zcl_metering_tou_received_payload_t;

/**
 * @brief The Zigbee zcl metering snapshot block delivered sub-payload struct
 *
 */
typedef struct esp_zb_zcl_metering_block_tier_delivered_payload_s {
    esp_zb_uint48_t current_summation_delivered;         /*!< An unsigned 48-bit integer that returns the value of the CurrentSummationDelivered attribute at the stated snapshot timestamp */
    uint32_t bill_to_date_delivered;                     /*!< An unsigned 32-bit integer that provides a value for the costs in the current billing period */
    uint32_t bill_to_date_time_stamp_delivered;          /*!< A UTC timestamp that indicates when the value of the associated BillToDateDelivered parameter was last updated */
    uint32_t projected_bill_delivered;                   /*!< An unsigned 32-bit integer that provides a value indicating what the estimated state of the account will be at the end of the billing period based on past consumption */
    uint32_t projected_bill_time_stamp_delivered;        /*!< A UTC timestamp that indicates when the associated ProjectedBillDelivered parameter was last updated */
    uint8_t bill_delivered_trailing_digit;               /*!< An 8-bit BitMap used to determine where the decimal point is located in the BillToDateDelivered and ProjectedBillDelivered fields */
    uint8_t number_of_tiers_in_use;                      /*!< An 8-bit integer representing the number of tiers in use at the time the snapshot was taken */
    esp_zb_uint48_t *tier_summation;                     /*!< The Publish Snapshot command contains N elements of CurrentTierNSummationDelivered attributes from the TOU Information Set */
    uint8_t number_of_tiers_and_block_thresholds_in_use; /*!< An 8-bit BitMap representing the number of tiers and block thresholds in use at the time the snapshot was taken */
    esp_zb_uint48_t *tier_block_summation;               /*!< The Publish Snapshot command contains N elements of the Block Information Attribute Set (Delivered).
                                                              The metering server shall send only the number of Tiers and Blocks in use as stated in this command */
} ESP_ZB_PACKED_STRUCT esp_zb_zcl_metering_block_tier_delivered_payload_t;

/**
 * @brief The Zigbee zcl metering snapshot block received sub-payload struct
 *
 */
typedef struct esp_zb_zcl_metering_block_tier_received_payload_s {
    esp_zb_uint48_t current_summation_received;          /*!< An unsigned 48-bit integer that returns the value of the CurrentSummationReceived attribute at the stated snapshot timestamp */
    uint32_t bill_to_date_received;                      /*!< An unsigned 32-bit integer that provides a value for the costs in the current billing period */
    uint32_t bill_to_date_time_stamp_received;           /*!< A UTC timestamp that indicates when the value of the associated BillToDateReceived parameter was last updated */
    uint32_t projected_bill_received;                    /*!< An unsigned 32-bit integer that provides a value indicating what the estimated state of the account will be at the end of the billing period based on past generation */
    uint32_t projected_bill_time_stamp_received;         /*!< A UTC timestamp that indicates when the associated ProjectedBillReceived parameter was last updated */
    uint8_t bill_received_trailing_digit;                /*!< An 8-bit BitMap used to determine where the decimal point is located in the BillToDateReceived and ProjectedBillReceived fields */
    uint8_t number_of_tiers_in_use;                      /*!< An 8-bit integer representing the number of tiers in use at the time the snapshot was taken */
    esp_zb_uint48_t *tier_summation;                     /*!< The Publish Snapshot command contains N elements of CurrentTierNSummationReceived attributes from the TOU Information Set */
    uint8_t number_of_tiers_and_block_thresholds_in_use; /*!< An 8-bit BitMap representing the number of tiers and block thresholds in use at the time the snapshot was taken */
    esp_zb_uint48_t *tier_block_summation;               /*!< The Publish Snapshot command contains N elements of the Block Information Attribute Set (Received).
                                                              The metering server shall send only the number of Tiers and Blocks in use as stated in this command */
} ESP_ZB_PACKED_STRUCT esp_zb_zcl_metering_block_tier_received_payload_t;

/**
 * @brief The Zigbee zcl metering snapshot tou delivered no billing sub-payload struct
 *
 */
typedef struct esp_zb_zcl_metering_tou_delivered_no_billing_payload_s {
    esp_zb_uint48_t current_summation_delivered; /*!< An unsigned 48-bit integer that returns the value of the CurrentSummationDelivered attribute at the stated snapshot timestamp */
    uint8_t number_of_tiers_in_use;              /*!< An 8-bit integer representing the number of tiers in use at the time the snapshot was taken */
    esp_zb_uint48_t *tier_summation;             /*!< The Publish Snapshot command contains N elements of CurrentTierNSummationDelivered attributes from the TOU Information Set.
                                                      The metering server shall send only the number of Tiers in use as stated in this command */
} ESP_ZB_PACKED_STRUCT esp_zb_zcl_metering_tou_delivered_no_billing_payload_t;

/**
 * @brief The Zigbee zcl metering snapshot tou received no billing sub-payload struct
 *
 */
typedef struct esp_zb_zcl_metering_tou_received_no_billing_payload_s {
    esp_zb_uint48_t current_summation_received; /*!< An unsigned 48-bit integer that returns the value of the CurrentSummationReceived attribute at the stated snapshot timestamp */
    uint8_t number_of_tiers_in_use;             /*!< An 8-bit integer representing the number of tiers in use at the time the snapshot was taken */
    esp_zb_uint48_t *tier_summation;            /*!< The Publish Snapshot command contains N elements of CurrentTierNSummationReceived attributes from the TOU Information Set.
                                                     The metering server shall send only the number of Tiers in use as stated in this command */
} ESP_ZB_PACKED_STRUCT esp_zb_zcl_metering_tou_received_no_billing_payload_t;

/**
 * @brief The Zigbee zcl metering snapshot block tier delivered no billing sub-payload struct
 *
 */
typedef struct esp_zb_zcl_metering_block_tier_delivered_no_billing_payload_s {

    esp_zb_uint48_t current_summation_delivered;         /*!< An unsigned 48-bit integer that returns the value of the CurrentSummationDelivered attribute at the stated snapshot timestamp */
    uint8_t number_of_tiers_in_use;                      /*!< An 8-bit integer representing the number of tiers in use at the time the snapshot was taken */
    esp_zb_uint48_t *tier_summation;                     /*!< The Publish Snapshot command contains N elements of CurrentTierNSummationDelivered attributes from the TOU Information Set */
    uint8_t number_of_tiers_and_block_thresholds_in_use; /*!< An 8-bit BitMap representing the number of tiers and block thresholds in use at the time the snapshot was taken */
    esp_zb_uint48_t *tier_block_summation;               /*!< The Publish Snapshot command contains N elements of the Block Information Attribute Set (Delivered).
                                                              The metering server shall send only the number of Tiers and Blocks in use as stated in this command */
} ESP_ZB_PACKED_STRUCT esp_zb_zcl_metering_block_tier_delivered_no_billing_payload_t;

/**
 * @brief The Zigbee zcl metering snapshot block tier received no billing sub-payload struct
 *
 */
typedef struct esp_zb_zcl_metering_block_tier_received_no_billing_payload_s {
    esp_zb_uint48_t current_summation_received;          /*!< An unsigned 48-bit integer that returns the value of the CurrentSummationReceived attribute at the stated snapshot timestamp */
    uint8_t number_of_tiers_in_use;                      /*!< An 8-bit integer representing the number of tiers in use at the time the snapshot was taken */
    esp_zb_uint48_t *tier_summation;                     /*!< The Publish Snapshot command contains N elements of CurrentTierNSummationReceived attributes from the TOU Information Set */
    uint8_t number_of_tiers_and_block_thresholds_in_use; /*!< An 8-bit BitMap representing the number of tiers and block thresholds in use at the time the snapshot was taken */
    esp_zb_uint48_t *tier_block_summation;               /*!< The Publish Snapshot command contains N elements of the Block Information Attribute Set (Received).
                                                              The metering server shall send only the number of Tiers and Blocks in use as stated in this command */
} ESP_ZB_PACKED_STRUCT esp_zb_zcl_metering_block_tier_received_no_billing_payload_t;

/**
 * @brief The Zigbee zcl metering snapshot sub-payload struct
 *
 */
typedef union esp_zb_zcl_metering_snapshot_sub_payload_s{
    esp_zb_zcl_metering_tou_delivered_payload_t tou_delivered;                                     /*!< @ref esp_zb_zcl_metering_tou_delivered_payload_s */
    esp_zb_zcl_metering_tou_received_payload_t tou_received;                                       /*!< @ref esp_zb_zcl_metering_tou_received_payload_s */
    esp_zb_zcl_metering_block_tier_delivered_payload_t block_tier_delivered;                       /*!< @ref esp_zb_zcl_metering_block_tier_delivered_payload_s */
    esp_zb_zcl_metering_block_tier_received_payload_t block_tier_received;                         /*!< @ref esp_zb_zcl_metering_block_tier_received_payload_s */
    esp_zb_zcl_metering_tou_delivered_no_billing_payload_t tou_delivered_no_billing;               /*!< @ref esp_zb_zcl_metering_tou_delivered_no_billing_payload_s */
    esp_zb_zcl_metering_tou_received_no_billing_payload_t tou_received_no_billing;                 /*!< @ref esp_zb_zcl_metering_tou_received_no_billing_payload_s */
    esp_zb_zcl_metering_block_tier_delivered_no_billing_payload_t block_tier_delivered_no_billing; /*!< @ref esp_zb_zcl_metering_block_tier_delivered_no_billing_payload_s */
    esp_zb_zcl_metering_block_tier_received_no_billing_payload_t block_tier_received_no_billing;   /*!< @ref esp_zb_zcl_metering_block_tier_received_no_billing_payload_s */
} ESP_ZB_PACKED_STRUCT esp_zb_zcl_metering_snapshot_sub_payload_t;

/**
 * @brief The Zigbee zcl metering snapshot struct
 *
 */
typedef struct esp_zb_zcl_metering_snapshot_s {
    uint32_t snapshot_id;                                              /*!< Unique identifier allocated by the device creating the snapshot. */
    uint32_t snapshot_time;                                            /*!< This is a 32-bit value (in UTC Time) representing the time at which the data snapshot was taken. */
    uint8_t total_snapshots_found;                                     /*!< An 8-bit Integer indicating the number of snapshots found, based on the search criteria defined in the associated GetSnapshot command */
    esp_zb_zcl_metering_snapshot_cause_t snapshot_cause;               /*!< A 32-bit BitMap indicating the cause of the snapshot */
    esp_zb_zcl_metering_snapshot_payload_type_t snapshot_payload_type; /*!< The SnapshotPayloadType is an 8-bit enumerator defining the format of the Snapshot Sub-Payload in this message */
    esp_zb_zcl_metering_snapshot_sub_payload_t snapshot_sub_payload;   /*!< @ref esp_zb_zcl_metering_snapshot_sub_payload_s */
} ESP_ZB_PACKED_STRUCT esp_zb_zcl_metering_snapshot_t;

/**
 * @brief The Zigbee zcl metering get snapshot callback message struct
 *
 */
typedef struct esp_zb_zcl_metering_get_snapshot_message_s {
    esp_zb_device_cb_common_info_t info;                 /*!< The common information for Zigbee device callback */
    uint32_t earliest_start_time;                        /*!< A UTC Timestamp indicating the earliest time of a snapshot to be returned by a corresponding Publish Snapshot command */
    uint32_t latest_end_time;                            /*!< A UTC Timestamp indicating the latest time of a snapshot to be returned by a corresponding Publish Snapshot command */
    uint8_t snapshot_offset;                             /*!< This field identifies the individual snapshot to be returned, where multiple snapshots satisfy the selection criteria specified by the other fields in this command */
    esp_zb_zcl_metering_snapshot_cause_t snapshot_cause; /*!< This field is used to select only snapshots that were taken due to a specific cause,
                                                              setting 0xFFFFFFFF indicates that all snapshots should be selected, irrespective of the cause */
    esp_zb_zcl_metering_snapshot_t resp_info_offered;    /*!< The info used for 'PublishSnapshot' command, to response 'GetSnapshot' command.
                                                              The info SHOULD be offered by user, otherwise, the response has no sense */
} esp_zb_zcl_metering_get_snapshot_message_t;

/**
 * @brief The Zigbee zcl metering publish snapshot struct
 *
 */
typedef struct esp_zb_zcl_metering_publish_snapshot_message_s {
    esp_zb_device_cb_common_info_t info;                               /*!< The common information for Zigbee device callback */
    uint32_t snapshot_id;                                              /*!< Unique identifier allocated by the device creating the snapshot. */
    uint32_t snapshot_time;                                            /*!< This is a 32-bit value (in UTC Time) representing the time at which the data snapshot was taken. */
    uint8_t total_snapshots_found;                                     /*!< An 8-bit Integer indicating the number of snapshots found, based on the search criteria defined in the associated GetSnapshot command */
    uint8_t command_index;                                             /*!< The CommandIndex is used to count the payload fragments in the case where the entire payload (snapshot) does not fit into one message */
    uint8_t total_number_of_commands;                                  /*!< In the case where the entire payload (snapshot) does not fit into one message, the
                                                                            Total Number of Commands field indicates the total number of sub-commands that will be returned */
    esp_zb_zcl_metering_snapshot_cause_t snapshot_cause;               /*!< A 32-bit BitMap indicating the cause of the snapshot */
    esp_zb_zcl_metering_snapshot_payload_type_t snapshot_payload_type; /*!< The SnapshotPayloadType is an 8-bit enumerator defining the format of the Snapshot Sub-Payload in this message */
    esp_zb_zcl_metering_snapshot_sub_payload_t snapshot_sub_payload;   /*!< @ref esp_zb_zcl_metering_snapshot_sub_payload_s */
} esp_zb_zcl_metering_publish_snapshot_message_t;

/**
 * @brief The Zigbee zcl metering get sampled data response info offered by user struct
 *
 */
typedef struct esp_zb_zcl_metering_get_sampled_data_resp_info_offered_s {
    uint16_t sample_id;                            /*!< Unique identifier allocated to this Sampling session */
    uint32_t sample_start_time;                    /*!< A UTC Time field to denote the time of the first sample returned */
    esp_zb_zcl_metering_sample_type_t sample_type; /*!< An 8 bit enumeration that identifies the type of data being sampled */
    uint16_t sample_request_interval;              /*!< An unsigned 16-bit field representing the interval or time in seconds between samples */
    uint16_t number_of_samples;                    /*!< Represents the number of samples being requested, cannot exceed MaxNumberOfSamples.
                                                        And if fewer samples are available for the time period, only those available shall be returned */
    esp_zb_uint24_t *samples;                      /*!< Series of data samples captured using the interval specified by the Sample RequestInterval field in the StartSampling command.
                                                        Invalid samples should be marked as 0xFFFFFF */
} esp_zb_zcl_metering_get_sampled_data_resp_info_offered_t;

/**
 * @brief The Zigbee zcl metering get sampled data callback message struct
 *
 */
typedef struct esp_zb_zcl_metering_get_sampled_data_message_s {
    esp_zb_device_cb_common_info_t info;                                        /*!< The common information for Zigbee device callback */
    uint16_t sample_id;                                                         /*!< Unique identifier allocated to this Sampling session */
    uint32_t earliest_sample_time;                                              /*!< A UTC Timestamp indicating the earliest time of a sample to be returned */
    esp_zb_zcl_metering_sample_type_t sample_type;                              /*!< An 8 bit enumeration that identifies the type of data being sampled */
    uint16_t number_of_samples;                                                 /*!< Represents the number of samples being requested, This value cannot exceed the size stipulated in the MaxNumberOfSamples field in the StartSampling command */
    esp_zb_zcl_metering_get_sampled_data_resp_info_offered_t resp_info_offered; /*!< The info used for 'GetSampledDataResponse' command, to response 'GetSampledData' command.
                                                                                     The info SHOULD be offered by user, otherwise, the response has no sense */
} esp_zb_zcl_metering_get_sampled_data_message_t;

/**
 * @brief The Zigbee zcl metering get sampled data response callback message struct
 *
 */
typedef struct esp_zb_zcl_metering_get_sampled_data_resp_message_s {
    esp_zb_device_cb_common_info_t info;           /*!< The common information for Zigbee device callback */
    uint16_t sample_id;                            /*!< Unique identifier allocated to this Sampling session */
    uint32_t sample_start_time;                    /*!< A UTC Time field to denote the time of the first sample returned */
    esp_zb_zcl_metering_sample_type_t sample_type; /*!< An 8 bit enumeration that identifies the type of data being sampled */
    uint16_t sample_request_interval;              /*!< An unsigned 16-bit field representing the interval or time in seconds between samples */
    uint16_t number_of_samples;                    /*!< Represents the number of samples being requested, cannot exceed MaxNumberOfSamples.
                                                        And if fewer samples are available for the time period, only those available shall be returned */
    esp_zb_uint24_t *samples;                      /*!< Series of data samples captured using the interval specified by the Sample RequestInterval field in the StartSampling command.
                                                        Invalid samples should be marked as 0xFFFFFF */
} esp_zb_zcl_metering_get_sampled_data_resp_message_t;

/**
 * @brief The Zigbee zcl price publish price struct
 */
typedef struct esp_zb_zcl_price_publish_price_s {
    /* Mandatory fields. */
    uint32_t provider_id;                     /*!< Unique identifier for the commodity provider. */
    uint8_t  rate_label[13];                  /*!< Commodity provider-specific information regarding the current billing rate. */
    uint32_t issuer_event_id;                 /*!< Unique identifier generated by the commodity provider allows devices to determine which pricing information is newer. */
    uint32_t current_time;                    /*!< UTC Time field containing the current time as determined by the device. */
    uint8_t  unit_of_measure;                 /*!< Field identifying the commodity as well as this base units of measure. */
    uint16_t currency;                        /*!< Field identifying information concerning the local unit of currency used in the price field. */
    uint32_t price_trailing_digit : 4;        /*!< Determines where the decimal point is located in the price field */
    uint32_t price_tier : 4;                  /*!< Indicates the current price tier as chosen by the commodity provider */
    uint32_t number_of_price_tiers : 4;       /*!< Represents the maximum number of price tiers available. */
    uint32_t register_tier : 4;               /*!< Indicates the register tier used with the current Price Tier. */
    uint32_t start_time;                      /*!< Denotes the time at which the signal becomes valid. Value 0x00000000 is a special time denoting "now". */
    uint16_t duration_in_minutes;             /*!< Denotes amount of time in minutes after Start Time during which the price signal is valid. */
    uint32_t price;                           /*!< Contains the price of the commodity measured in base unit of Currency per Unit of Measure with the decimal point located as indicated by the Price Trailing Digit field when the commodity is delivered to premises. */
    /* Optional fields. */
    uint8_t  price_ratio;                     /*!< Gives the ratio of the price denoted in the Price field to the "normal" price, chosen by the commodity provider. */
    uint32_t generation_price;                /*!< Contains the price of the commodity measured in base unit of Currency per Unit of Measure with the decimal point located as indicated by the Price Trailing Digit field when the commodity is received from premises. */
    uint8_t  generation_price_ratio;          /*!< Gives the ratio of the price denoted in the Generation Price field to the "normal" price, chosen by the commodity provider. */
    uint32_t alternate_cost_delivered;        /*!< Provides mechanism to describe an alternative measure of the cost of the energy consumed. */
    uint8_t  alternate_cost_unit;             /*!< Identifies the unit for Alternate Cost Delivered field */
    uint8_t  alternate_cost_trailing_digit;   /*!< Determines where the decimal point is located in the alternate cost field. */
    uint8_t  number_of_block_thresholds;      /*!< Indicates the number of block thresholds available. */
    uint8_t  price_control;                   /*!< Identifies additional control options (PriceAcknowledgement policy and total number of tier flag). */
    uint8_t  number_of_generation_tiers;      /*!< Specifies the total number of generation tiers applicable in the current tariff. */
    uint8_t  generation_tier;                 /*!< Specifies the current generation tier. */
    uint8_t  extended_number_of_price_tiers;  /*!< Indicates a maximum number of tier available. */
    uint8_t  extended_price_tier;             /*!< Indicates the current price tier. */
    uint8_t  extended_register_tier;          /*!< Indicates the register tier used with the current Price Tier. */
} ESP_ZB_PACKED_STRUCT esp_zb_zcl_price_publish_price_t;

/**
 * @brief The Zigbee zcl price get current price callback message struct
 *
 */
typedef struct esp_zb_zcl_price_get_current_price_message_s {
    esp_zb_device_cb_common_info_t info;                    /*!< The common information for Zigbee device callback */
    uint8_t command_options;                                /*!< Zero bit is Requestor Rx On When Idle, others are reserved. */
    uint8_t resp_count;                                     /*!< Number of prices in `resp_info_offered` to be responded. */
    esp_zb_zcl_price_publish_price_t *resp_info_offered;    /*!< The info used for 'PublishPrice' command, to response 'GetCurrent' command.
                                                             *   The info SHOULD be offered by user, otherwise, the response has no sense */
} esp_zb_zcl_price_get_current_price_message_t;

/**
 * @brief The Zigbee zcl price get scheduled prices callback message struct
 *
 */
typedef struct esp_zb_zcl_price_get_scheduled_prices_message_s {
    esp_zb_device_cb_common_info_t info;                    /*!< The common information for Zigbee device callback */
    uint32_t start_time;                                    /*!< The minimum ending time for any scheduled or currently active pricing events to be resent.
                                                             *   @note Value  0x00000000 will be replaced with the current time stamp.
                                                             */
    uint8_t number_of_events;                               /*!< The maximum number of events to be sent. Value 0 indicates no maximum limit. */
    uint8_t resp_count;                                     /*!< Number of prices in `resp_info_offered` to be responded. */
    esp_zb_zcl_price_publish_price_t *resp_info_offered;    /*!< The info used for 'PublishPrice' command, to response 'GetCurrent' command.
                                                             *   The info SHOULD be offered by user, otherwise, the response has no sense.
                                                             */
} esp_zb_zcl_price_get_scheduled_prices_message_t;

/**
 * @brief The Zigbee zcl price publish price callback message struct
 *
 */
typedef struct esp_zb_zcl_price_publish_price_message_s {
    esp_zb_device_cb_common_info_t info;                    /*!< The common information for Zigbee device callback */
    esp_zb_zcl_price_publish_price_t price;                 /*!< Recived publish price information */
} esp_zb_zcl_price_publish_price_message_t;

/**
 * @brief The Zigbee zcl price acknowledgement callback message struct
 *
 */
typedef struct esp_zb_zcl_price_ack_message_s {
    esp_zb_device_cb_common_info_t info;                    /*!< The common information for Zigbee device callback */
    uint32_t provider_id;                                   /*!< Unique identifier for the commodity provider. */
    uint32_t issuer_event_id;                               /*!< Unique identifier generated by the commodity provider. */
    uint32_t current_time;                                  /*!< This field containing the current time as determined by the device. */
    uint8_t price_control;                                  /*!< Identifies additional control options (PriceAcknowledgement policy and
                                                             *   total number of tier flag).
                                                             */
} esp_zb_zcl_price_ack_message_t;

/**
 * @brief The Zigbee ZCL price tier label entry struct
 */
typedef struct esp_zb_zcl_price_tier_label_entry_s {
    uint8_t tier_id;                                        /*!< The tier number that the associated TierLabel applies to. */
    uint8_t tier_label[13];                                 /*!< Octet String field capable of storing a 12 character string (the first
                                                             *   character indicates the string length, represented in hexadecimal format)
                                                             *   encoded in the UTF-8 format.
                                                             */
} ESP_ZB_PACKED_STRUCT esp_zb_zcl_price_tier_label_entry_t;

/**
 * @brief The Zigbee ZCL price publish tier labels information struct
 */
typedef struct esp_zb_price_publish_tier_labels_info_s {
    uint32_t provider_id;                                   /*!< An unsigned 32-bit field containing a unique identifier for the commodity provider. */
    uint32_t issuer_event_id;                               /*!< Unique identifier generated by the commodity provider. */
    uint32_t issuer_tariff_id;                              /*!< Unique identifier generated by the commodity supplier. */
    uint8_t number_of_labels;                               /*!< The number of TierID/Tier Label sets contained within the command. */
    esp_zb_zcl_price_tier_label_entry_t *tier_labels;       /*!< Tier labels need to be published in the command */
} esp_zb_price_publish_tier_labels_info_t;

/**
 * @brief The Zigbee ZCL price publish tier labels callback message struct
 */
typedef struct esp_zb_zcl_price_get_tier_labels_message_s {
    esp_zb_device_cb_common_info_t info;                            /*!< The common information for Zigbee device callback */
    uint32_t issuer_tariff_id;                                      /*!< Unique identifier generated by the commodity supplier. */
    esp_zb_price_publish_tier_labels_info_t resp_info_offered;      /*!< Information of the response */
} esp_zb_zcl_price_get_tier_labels_message_t;

/**
 * @brief The Zigbee ZCL price publish tier labels callback message struct
 */
typedef struct esp_zb_zcl_price_publish_tier_labels_message_s {
    esp_zb_device_cb_common_info_t info;                    /*!< The common information for Zigbee device callback */
    uint32_t provider_id;                                   /*!< An unsigned 32-bit field containing a unique identifier for the commodity provider. */
    uint32_t issuer_event_id;                               /*!< Unique identifier generated by the commodity provider. */
    uint32_t issuer_tariff_id;                              /*!< Unique identifier generated by the commodity supplier. */
    uint8_t command_index;                                  /*!< This is used to count the payload fragments in the case where the entire payload does not fit into one message. */
    uint8_t total_number_of_commands;                       /*!< In the case where the entire payload does not fit into one message,
                                                             *   this field indicates the total number of sub-commands in the message.
                                                             */
    uint8_t number_of_labels;                               /*!< The number of TierID/Tier Label sets contained within the command. */
    esp_zb_zcl_price_tier_label_entry_t *tier_labels;       /*!< Tier labels published in the command */
} esp_zb_zcl_price_publish_tier_labels_message_t;

/**
 * @brief Structure for DRLC LoadControlEvent request callback message
 */
typedef struct esp_zb_zcl_drlc_load_control_event_message_s {
    esp_zb_device_cb_common_info_t info;                    /*!< The common information for Zigbee device callback */
    esp_zb_zcl_drlc_load_control_event_payload_t payload;   /*!< The payload of LoadControlEvent request, @see esp_zb_zcl_drlc_load_control_event_payload_t */
    esp_zb_zcl_drlc_report_event_status_payload_t *response;/*!< An output pointer allows the user to pass it as a parameter in response to this request */
} esp_zb_zcl_drlc_load_control_event_message_t;

/**
 * @brief Structure for DRLC CancelLoadControlEvent request callback message
 */
typedef struct esp_zb_zcl_drlc_cancel_load_control_event_message_s {
    esp_zb_device_cb_common_info_t info;                            /*!< The common information for Zigbee device callback */
    esp_zb_zcl_drlc_cancel_load_control_event_payload_t payload;    /*!< The payload of CancelLoadControlEvent request, @see esp_zb_zcl_drlc_cancel_load_control_event_payload_t */
    esp_zb_zcl_drlc_report_event_status_payload_t *response;        /*!< An output pointer allows the user to pass it as a parameter in response to this request */
} esp_zb_zcl_drlc_cancel_load_control_event_message_t;

/**
 * @brief Structure for DRLC CancelAllLoadControlEvents request callback message
 */
typedef struct esp_zb_zcl_drlc_cancel_all_load_control_events_message_s {
    esp_zb_device_cb_common_info_t info;                                /*!< The common information for Zigbee device callback */
    esp_zb_zcl_drlc_cancel_all_load_control_events_payload_t payload;   /*!< The payload of CancelAllLoadControlEvents */
} esp_zb_zcl_drlc_cancel_all_load_control_events_message_t;

/**
 * @brief Structure for DRLC ReportEventStatus request callback message
 */
typedef struct esp_zb_zcl_drlc_report_event_status_message_s {
    esp_zb_device_cb_common_info_t info;                    /*!< The common information for Zigbee device callback */
    esp_zb_zcl_drlc_report_event_status_payload_t payload;  /*!< the payload of ReportEventStatus, @see esp_zb_zcl_drlc_report_event_status_payload_t */
} esp_zb_zcl_drlc_report_event_status_message_t;

/**
 * @brief Structure for DRLC GetScheduledEvents request callback message
 */
typedef struct esp_zb_zcl_drlc_get_scheduled_events_message_s {
    esp_zb_device_cb_common_info_t info;                      /*!< The common information for Zigbee device callback */
    esp_zb_zcl_drlc_get_scheduled_events_payload_t payload;   /*!< The payload of GetScheduledEvents, @see esp_zb_zcl_drlc_get_scheduled_events_payload_t */
    esp_zb_zcl_drlc_load_control_event_payload_t *response;   /*!< An output pointer allows the user to pass it as a parameter in response to this request */
} esp_zb_zcl_drlc_get_scheduled_events_message_t;

/**
 * @brief The Zigbee ZCL commissioning restart device callback message struct
 */
typedef struct esp_zigbee_zcl_commissioning_restart_device_message_s {
    esp_zb_device_cb_common_info_t info;                        /*!< The common information for Zigbee device callback */
    esp_zb_zcl_commissioning_restart_device_payload_t msg_in;   /*!< Received restart device payload */
} esp_zigbee_zcl_commissioning_restart_device_message_t;

/**
 * @brief The Zigbee ZCL commissioning startup parameters operation struct
 */
typedef enum {
    ESP_ZB_ZCL_COMMISSIONING_STARTUP_PARAMETERS_OPERATION_SAVE,       /*!< Indicates to save startup parameter set */
    ESP_ZB_ZCL_COMMISSIONING_STARTUP_PARAMETERS_OPERATION_RESTORE,    /*!< Indicates to restore startup parameter set */
    ESP_ZB_ZCL_COMMISSIONING_STARTUP_PARAMETERS_OPERATION_ERASE,      /*!< Indicates to erase startup parameter set */
    ESP_ZB_ZCL_COMMISSIONING_STARTUP_PARAMETERS_OPERATION_RESET,      /*!< Indicates to reset startup parameter set */
    ESP_ZB_ZCL_COMMISSIONING_STARTUP_PARAMETERS_OPERATION_RESET_ALL,  /*!< Indicates to reset all startup parameter set */
} esp_zb_zcl_commissioning_startup_parameters_operation_t;

/**
 * @brief The Zigbee ZCL commissioning startup parameters operation callback message struct
 */
typedef struct esp_zigbee_zcl_commissioning_operate_startup_parameters_message_s {
    esp_zb_device_cb_common_info_t info;                                 /*!< The common information for Zigbee device callback */
    esp_zb_zcl_commissioning_startup_parameters_operation_t operation;   /*!< Operation on startup parameter set */
    uint8_t index;                                                       /*!< Index of the startup parameter set to be operated on */
    esp_zb_zcl_status_t status;                                          /*!< Result of the operation, will be sent in the response */
} esp_zigbee_zcl_commissioning_operate_startup_parameters_message_t;

/**
 * @brief The Zigbee ZCL commissioning startup parameters operation callback message struct
 */
typedef struct esp_zigbee_zcl_commissioning_command_response_message_s {
    esp_zb_device_cb_common_info_t info;                    /*!< The common information for Zigbee device callback */
    esp_zb_zcl_status_t status;                             /*!< Status of the received response */
} esp_zigbee_zcl_commissioning_command_response_message_t;

/**
 * @brief The Zigbee zcl door lock callback message struct
 *
 */
typedef struct esp_zb_zcl_door_lock_lock_door_message_s {
    esp_zb_device_cb_common_info_t info;  /*!< The common information for Zigbee device callback */
    esp_zb_zcl_door_lock_cmd_id_t cmd_id; /*!< The lock or unlock command identifier */
} esp_zb_zcl_door_lock_lock_door_message_t;

/**
 * @brief The Zigbee zcl door lock response callback message struct
 *
 */
typedef struct esp_zb_zcl_door_lock_lock_door_resp_message_s {
    esp_zb_device_cb_common_info_t info;    /*!< The common information for Zigbee device callback */
    esp_zb_zcl_door_lock_cmd_resp_t cmd_id; /*!< The lock or unlock response command identifier */
    esp_zb_zcl_status_t req_status;         /*!< The status of request command indicates if the message has received successfully. */
} esp_zb_zcl_door_lock_lock_door_resp_message_t;

/**
 * @brief The Zigbee zcl identify cluster calls identify effect command message struct
 *
 */
typedef struct esp_zb_zcl_identify_effect_message_s {
    esp_zb_device_cb_common_info_t info; /*!< The common information for Zigbee device callback */
    uint8_t effect_id;                   /*!< The field specifies the identify effect to use, refer to esp_zb_zcl_identify_trigger_effect_s */
    uint8_t effect_variant;              /*!< The field is used to indicate which variant of the effect, indicated in the effect identifier field, SHOULD be triggered */
} esp_zb_zcl_identify_effect_message_t;

/**
 * @brief The Zigbee zcl basic cluster reset to factory default message
 *
 */
typedef struct esp_zb_zcl_basic_reset_factory_default_message_s {
    esp_zb_device_cb_common_info_t info; /*!< The common information for Zigbee device callback */
} esp_zb_zcl_basic_reset_factory_default_message_t;

/**
 * @brief ZCL window covering movement message
 *
 */
typedef struct esp_zb_zcl_window_covering_movement_message_s {
    esp_zb_device_cb_common_info_t info; /*!< The common information for Zigbee device callback */
    uint16_t command;                    /*!< The movement command, refer to esp_zb_zcl_window_covering_cmd_t */
    union {
        uint8_t percentage_lift_value;  /*!< The payload for ESP_ZB_ZCL_CMD_WINDOW_COVERING_GO_TO_LIFT_PERCENTAGE */
        uint8_t percentage_tilt_value;  /*!< The payload for ESP_ZB_ZCL_CMD_WINDOW_COVERING_GO_TO_TILT_PERCENTAGE */
        uint16_t lift_value;            /*!< The payload for ESP_ZB_ZCL_CMD_WINDOW_COVERING_GO_TO_LIFT_VALUE */
        uint16_t tilt_value;            /*!< The payload for ESP_ZB_ZCL_CMD_WINDOW_COVERING_GO_TO_TILT_VALUE */
    } payload;                          /*!< Command payload */
} esp_zb_zcl_window_covering_movement_message_t;

/**
 * @brief The Zigbee zcl command basic application information struct
 *
 */
typedef struct esp_zb_zcl_cmd_info_s {
    esp_zb_zcl_status_t status;       /*!< The status of command, which can refer to  esp_zb_zcl_status_t */
    esp_zb_zcl_frame_header_t header; /*!< The command frame properties, which can refer to esp_zb_zcl_frame_field_t */
    esp_zb_zcl_addr_t src_address;    /*!< The struct of address contains short and ieee address, which can refer to esp_zb_zcl_addr_s */
    uint16_t dst_address;             /*!< The destination short address of command */
    uint8_t src_endpoint;             /*!< The source endpoint of command */
    uint8_t dst_endpoint;             /*!< The destination endpoint of command */
    uint16_t cluster;                 /*!< The cluster id for command */
    uint16_t profile;                 /*!< The application profile identifier*/
    esp_zb_zcl_command_t command;     /*!< The properties of command */
} esp_zb_zcl_cmd_info_t;

/**
 * @brief The Zigbee zcl attribute report message struct
 *
 */
typedef struct esp_zb_zcl_report_attr_message_s {
    esp_zb_zcl_status_t status;       /*!< The status of the report attribute response, which can refer to esp_zb_zcl_status_t */
    esp_zb_zcl_addr_t src_address;    /*!< The struct of address contains short and ieee address, which can refer to esp_zb_zcl_addr_s */
    uint8_t src_endpoint;             /*!< The endpoint id which comes from report device */
    uint8_t dst_endpoint;             /*!< The destination endpoint id */
    uint16_t cluster;                 /*!< The cluster id that reported */
    esp_zb_zcl_attribute_t attribute; /*!< The attribute entry of report response */
} esp_zb_zcl_report_attr_message_t;

/**
 * @brief The variable of Zigbee zcl read attribute response
 *
 */
typedef struct esp_zb_zcl_read_attr_resp_variable_s {
    esp_zb_zcl_status_t status;                        /*!< The field specifies the status of the read operation on this attribute */
    esp_zb_zcl_attribute_t attribute;                  /*!< The field contain the current value of this attribute, @ref esp_zb_zcl_attribute_s */
    struct esp_zb_zcl_read_attr_resp_variable_s *next; /*!< Next variable */
} esp_zb_zcl_read_attr_resp_variable_t;

/**
 * @brief The Zigbee zcl read attribute response struct
 *
 */
typedef struct esp_zb_zcl_cmd_read_attr_resp_message_s {
    esp_zb_zcl_cmd_info_t info;                      /*!< The basic information of reading attribute response message that refers to esp_zb_zcl_cmd_info_t */
    esp_zb_zcl_read_attr_resp_variable_t *variables; /*!< The variable items, @ref esp_zb_zcl_read_attr_resp_variable_s */
} esp_zb_zcl_cmd_read_attr_resp_message_t;

/**
 * @brief The variable of Zigbee zcl write attribute response
 *
 */
typedef struct esp_zb_zcl_write_attr_resp_variable_s {
    esp_zb_zcl_status_t status;                         /*!< The field specifies the status of the write operation on this attribute */
    uint16_t attribute_id;                              /*!< The attribute id of the write attribute response, please note that when info.status does not equal
                                                            ESP_ZB_ZCL_STATUS_SUCCESS, the attribute_id is reported; otherwise, it is an invalid value (0xFFFF) */
    struct esp_zb_zcl_write_attr_resp_variable_s *next; /*!< Next variable */
} esp_zb_zcl_write_attr_resp_variable_t;
/**
 * @brief The Zigbee zcl response struct for writing attribute
 *
 */
typedef struct esp_zb_zcl_cmd_write_attr_resp_message_s {
    esp_zb_zcl_cmd_info_t info;                       /*!< The basic information of the write attribute response message that refers to esp_zb_zcl_cmd_info_t */
    esp_zb_zcl_write_attr_resp_variable_t *variables; /*!< The variable items, @ref esp_zb_zcl_write_attr_resp_variable_s */
} esp_zb_zcl_cmd_write_attr_resp_message_t;

/**
 * @brief The variable of Zigbee zcl configures report attribute response
 *
 */
typedef struct esp_zb_zcl_config_report_resp_variable_s {
    esp_zb_zcl_status_t status;                            /*!< The field specifies the status of the Configure Reporting operation attempted on this attribute */
    uint8_t direction;                                     /*!< The direction field specifies whether values of the attribute are reported (0x00),
                                                               or whether reports of the attribute are received (0x01).*/
    uint16_t attribute_id;                                 /*!< The The attribute id of configuring report response, please note that when info.status does not equal
                                                               ESP_ZB_ZCL_STATUS_SUCCESS, the attribute_id is reported; otherwise, it is an invalid value (0xFFFF). */
    struct esp_zb_zcl_config_report_resp_variable_s *next; /*!< Next variable */
} esp_zb_zcl_config_report_resp_variable_t;

/**
 * @brief The Zigbee zcl configure report response struct
 *
 */
typedef struct esp_zb_zcl_cmd_config_report_resp_message_s {
    esp_zb_zcl_cmd_info_t info;                          /*!< The basic information of configuring report response message, @ref esp_zb_zcl_cmd_info_s */
    esp_zb_zcl_config_report_resp_variable_t *variables; /*!< The variable items, @ref esp_zb_zcl_config_report_resp_variable_s */
} esp_zb_zcl_cmd_config_report_resp_message_t;

/**
 * @brief The variable of Zigbee zcl read report configuration response
 *
 */
typedef struct esp_zb_zcl_read_report_config_resp_variable_s {
    esp_zb_zcl_status_t status; /*!< The field specifies the status of the read report config operation on this attribute, refer to esp_zb_zcl_status_t */
    uint8_t report_direction;   /*!< Direction: report is client or server */
    uint16_t attribute_id;      /*!< The attribute id, please note that when info.status does not equal ESP_ZB_ZCL_STATUS_SUCCESS,
                                   the attribute_id is reported; otherwise, it is an invalid value (0xFFFF). */
    union {
        struct {
            uint8_t attr_type;     /*!< Attribute type */
            uint16_t min_interval; /*!< Minimum interval time */
            uint16_t max_interval; /*!< Maximum interval time */
            uint8_t delta[1];      /*!< Actual reportable change */
        } client;                  /*!< Describes how attribute should be reported */
        struct {
            uint16_t timeout;      /*!< Timeout period */
        } server;                  /*!< Describes how attribute report is received  */
    };
    struct esp_zb_zcl_read_report_config_resp_variable_s *next; /*!< Next variable */
} esp_zb_zcl_read_report_config_resp_variable_t;

/**
 * @brief The Zigbee zcl reading report configuration response struct
 *
 */
typedef struct esp_zb_zcl_cmd_read_report_config_resp_message_s {
    esp_zb_zcl_cmd_info_t info; /*!< The basic information of reading report configuration response message that refers to esp_zb_zcl_cmd_info_t */
    esp_zb_zcl_read_report_config_resp_variable_t *variables; /*!< The variable items, @ref esp_zb_zcl_read_report_config_resp_variable_s */
} esp_zb_zcl_cmd_read_report_config_resp_message_t;

/**
 * @brief Attribute information field for discovering attributes response struct
 *
 */
typedef struct esp_zb_zcl_disc_attr_variable_s {
    uint16_t attr_id;                             /*!< The field contain the identifier of a discovered attribute */
    esp_zb_zcl_attr_type_t data_type;             /*!< The field contain the data type of the attribute in the same attribute report field */
    struct esp_zb_zcl_disc_attr_variable_s *next; /*!< Next variable */
} esp_zb_zcl_disc_attr_variable_t;

/**
 * @brief The Zigbee zcl discover attribute response struct
 *
 */
typedef struct esp_zb_zcl_cmd_discover_attributes_resp_message_s {
    esp_zb_zcl_cmd_info_t info;                 /*!< The basic information of configuring report response message that refers to esp_zb_zcl_cmd_info_t */
    uint8_t is_completed;                       /*!< A value of 0 indicates that there are more attributes to be discovered, otherwise, it is completed */
    esp_zb_zcl_disc_attr_variable_t *variables; /*!< The variable items, which can refer to esp_zb_zcl_attr_info_field_t */
} esp_zb_zcl_cmd_discover_attributes_resp_message_t;

/**
 * @brief The Zigbee command default response struct
 *
 */
typedef struct esp_zb_zcl_cmd_default_resp_message_s {
    esp_zb_zcl_cmd_info_t info;      /*!< The basic information of configuring report response message that refers to esp_zb_zcl_cmd_info_t */
    uint8_t resp_to_cmd;             /*!< The field specifies the identifier of the received command to which this command is a response */
    esp_zb_zcl_status_t status_code; /*!< The field specifies the nature of the error that was detected in the received command, refer to esp_zb_zcl_status_t */
} esp_zb_zcl_cmd_default_resp_message_t;
/**
 * @brief The Zigbee zcl group operation response struct
 *
 * @note Operation: add or remove
 */
typedef struct esp_zb_zcl_groups_operate_group_resp_message_s {
    esp_zb_zcl_cmd_info_t info; /*!< The basic information of group cluster response that refers to esp_zb_zcl_cmd_info_t */
    uint16_t group_id;          /*!< The Group id of adding group response */
} esp_zb_zcl_groups_operate_group_resp_message_t;

/**
 * @brief The Zigbee zcl group view response struct
 *
 */
typedef struct esp_zb_zcl_groups_view_group_resp_message_s {
    esp_zb_zcl_cmd_info_t info; /*!< The basic information of group cluster response that refers to esp_zb_zcl_cmd_info_t */
    uint16_t group_id;          /*!< The group id of adding group response */
    uint8_t *group_name;        /*!< The group name */
} esp_zb_zcl_groups_view_group_resp_message_t;

/**
 * @brief The Zigbee zcl group get membership response struct
 *
 */
typedef struct esp_zb_zcl_groups_get_group_membership_resp_message_s {
    esp_zb_zcl_cmd_info_t info; /*!< The basic information of group cluster response that refers to esp_zb_zcl_cmd_info_t */
    uint8_t capacity;           /*!< The Capacity of group table */
    uint8_t group_count;        /*!< The Group  count */
    uint16_t *group_id;         /*!< The Group id list */
} esp_zb_zcl_groups_get_group_membership_resp_message_t;

/**
 * @brief The Zigbee ZCL scenes operate response struct
 *
 * @note Operation: add or remove or remove_all or store, refer to esp_zb_zcl_scenes_cmd_resp_id_t
 */
typedef struct esp_zb_zcl_scenes_operate_scene_resp_message_s {
    esp_zb_zcl_cmd_info_t info; /*!< The basic information of scene cluster response that refers to esp_zb_zcl_cmd_info_t */
    uint16_t group_id;          /*!< The Scene group identifier */
    uint8_t scene_id;           /*!< The Scene identifier */
} esp_zb_zcl_scenes_operate_scene_resp_message_t;

/**
 * @brief The Zigbee ZCL scenes view scene response struct
 *
 */
typedef struct esp_zb_zcl_scenes_view_scene_resp_message_s {
    esp_zb_zcl_cmd_info_t info;                      /*!< The basic information of scene cluster response that refers to esp_zb_zcl_cmd_info_t */
    uint16_t group_id;                               /*!< The Scene group identifier */
    uint8_t scene_id;                                /*!< The Scene identifier */
    uint16_t transition_time;                        /*!< The Scene transition time Valid if status is refers to ESP_ZB_ZCL_STATUS_SUCCESS only */
    esp_zb_zcl_scenes_extension_field_t *field_set;  /*!< Extension field, {{cluster_id, length, value}, ... , {cluster_id, length, value}} */
} esp_zb_zcl_scenes_view_scene_resp_message_t;

/**
 * @brief The Zigbee ZCL scenes get scene membership response struct
 *
 */
typedef struct esp_zb_zcl_scenes_get_scene_membership_resp_message_s {
    esp_zb_zcl_cmd_info_t info; /*!< The basic information of scene cluster response that refers to esp_zb_zcl_cmd_info_t */
    uint8_t capacity;           /*!< The Scene table capacity(Mandatory) */
    uint16_t group_id;          /*!< The Group identifier(Mandatory) */
    uint8_t scene_count;        /*!< The Number of scenes(Optional) */
    uint8_t *scene_list;        /*!< The Array of scenes corresponding to the group identifier(Optional) */
} esp_zb_zcl_scenes_get_scene_membership_resp_message_t;

/**
 * @brief The Zigbee ZCL IAS Zone enroll request message struct
 *
 */
typedef struct esp_zb_zcl_ias_zone_enroll_request_message_s {
    esp_zb_zcl_cmd_info_t info;                 /*!< The basic information of ias zone message that refers to esp_zb_zcl_cmd_info_t */
    uint16_t zone_type;                         /*!< The zone type, refer to esp_zb_zcl_ias_zone_zonetype_t */
    uint16_t manufacturer_code;                 /*!< The manufacturer code */
} esp_zb_zcl_ias_zone_enroll_request_message_t;

/**
 * @brief The Zigbee ZCL IAS Zone status change notification response message struct
 *
 */
typedef struct esp_zb_zcl_ias_zone_status_change_notification_message_s {
    esp_zb_zcl_cmd_info_t info;             /*!< The basic information of ias zone message that refers to esp_zb_zcl_cmd_info_t */
    uint16_t zone_status;                   /*!< The zone status attribute, which can refer esp_zb_zcl_ias_zone_zonestatus_t */
    uint8_t extended_status;                /*!< Reserved for additional status information and SHALL be set to zero */
    uint8_t zone_id;                        /*!< The Zone ID is the index of the Zone in the CIE's zone table */
    uint16_t delay;                         /*!< The amount of time, in quarter-seconds, from the moment when a change takes
                                                 place in one or more bits of the Zone Status attribute */
} esp_zb_zcl_ias_zone_status_change_notification_message_t;

/**
 * @brief The Zigbee zcl privilege command message struct
 *
 */
typedef struct esp_zb_zcl_privilege_command_message_s {
    esp_zb_zcl_cmd_info_t info; /*!< The basic information of privilege command message that refers to esp_zb_zcl_cmd_info_t */
    uint16_t size;              /*!< The size of data */
    void *data;                 /*!< The privilege command data */
} esp_zb_zcl_privilege_command_message_t;

/**
 * @brief The Zigbee zcl customized cluster message struct
 *
 * @note For string data type, the first byte should be the length of string.
 *       For array, array16, array32, and long string data types, the first 2 bytes should represent the number of elements in the array.
 */
typedef struct esp_zb_zcl_custom_cluster_command_message_s {
    esp_zb_zcl_cmd_info_t info; /*!< The basic information of customized cluster command message that refers to esp_zb_zcl_cmd_info_t */
    struct {
        uint16_t size; /*!< The size of custom data */
        void *value;   /*!< The value of custom data */
    } data;            /*!< The custom data */
} esp_zb_zcl_custom_cluster_command_message_t;

#ifdef CONFIG_ZB_GP_ENABLED
/**
 * @brief The message for receiving the green power command
 *
 */
typedef struct esp_zb_zcl_cmd_green_power_recv_message_s {
    esp_zb_zcl_cmd_info_t info;                 /*!< The basic information of  message that refers to esp_zb_zcl_cmd_info_t */
} esp_zb_zcl_cmd_green_power_recv_message_t;
#endif /* CONFIG_ZB_GP_ENABLED */

/**
 * @brief The Zigbee touchlink group information record struct
 *
 */
typedef struct esp_zb_touchlink_get_group_info_record_s {
    uint16_t group_id;   /*!< The identifier of the group described by this record */
    uint8_t group_type;  /*!< The group type shall indicate the meaning of a group in the user interface. In the current version of this specification, this value shall be set to 0x00 */
    struct esp_zb_touchlink_get_group_info_record_s *next; /*!< Next variable */
} esp_zb_touchlink_get_group_info_record_t;

/**
 * @brief The Zigbee ZCL touchlink commissioning get group identifiers response struct
 *
 */
typedef struct esp_zb_zcl_touchlink_get_group_identifiers_resp_message_s {
    esp_zb_zcl_cmd_info_t info; /*!< The basic information of touclink get group identifiers response that refers to esp_zb_zcl_cmd_info_t */
    uint8_t total;           /*!< The total number of group identifiers supported by the device */
    uint8_t start_idx;       /*!< The internal starting index from which the following group identifiers are taken */
    uint8_t count;           /*!< The number of entries in the group information record list field */
    esp_zb_touchlink_get_group_info_record_t *group_info_record_list;  /*!< Group information record list , {{Group identifier, Group type}, ... , {Group identifier, Group type}} */
} esp_zb_touchlink_get_group_identifiers_resp_message_t;

/**
 * @brief The Zigbee touchlink endpoint list record struct
 *
 */
typedef struct esp_zb_touchlink_get_endpoint_list_record_s {
    uint16_t addr_short; /*!< The short network address of the device specified by the current endpoint information record */
    uint8_t endpoint;    /*!< The identifier of the endpoint on the device specified by the network address field */
    uint16_t profile_id; /*!< The identifier of the profile supported on the endpoint */
    uint16_t device_id;  /*!< The identifier of the device description supported on the endpoint */
    uint8_t version;     /*!< Specifies the version of the device description supported by the sub-device on the endpoint */
    struct esp_zb_touchlink_get_endpoint_list_record_s *next; /*!< Next variable */
} esp_zb_touchlink_get_endpoint_list_record_t;

/**
 * @brief The Zigbee ZCL touchlink commissioning get endpoint list response struct
 *
 */
typedef struct esp_zb_zcl_touchlink_get_endpoint_list_resp_message_s {
    esp_zb_zcl_cmd_info_t info; /*!< The basic information of touchlink get endpoint list response that refers to esp_zb_zcl_cmd_info_t */
    uint8_t total;           /*!< The total number of endpoints supported by the device */
    uint8_t start_idx;       /*!< The internal starting index from which the following list of endpoints are taken */
    uint8_t count;           /*!< The number of entries in the endpoint information record list field */
    esp_zb_touchlink_get_endpoint_list_record_t *endpoint_record_list;  /*!< Endpoint record list , {{Network address, Endpoint identifier, Profile identifier, Device identifier, Version},
                                                                        ... , {Network address, Endpoint identifier, Profile identifier, Device identifier, Version}} */
} esp_zb_zcl_touchlink_get_endpoint_list_resp_message_t;

/**
 * @brief Structure for Thermostat Get Weekly Schedule command response
 *
 */
typedef struct esp_zb_zcl_thermostat_get_weekly_schedule_resp_message_s {
    esp_zb_zcl_cmd_info_t info;                                 /*!< The basic information of response from the peer device */
    uint8_t num_of_trans;                                       /*!< The field indicates how many individual transitions are included for
                                                                     this sequence of response */
    uint8_t day_of_week;                                        /*!< This field is a bitmap represents the day of the week at which all
                                                                     the transitions within the payload of the command SHOULD be associated to,
                                                                     refer to esp_zb_zcl_thermostat_day_of_week_t */
    uint8_t mode_for_req;                                       /*!< This field is a bitmap  determines how the application SHALL decode the Set
                                                                     Point Fields of each transition. @see esp_zb_zcl_thermostat_weekly_schedule_mode_for_seq_t  */
    esp_zb_zcl_thermostat_weekly_schedule_transition_t *trans;  /*!< Transition field, refer to esp_zb_zcl_thermostat_weekly_schedule_transition_t */
} esp_zb_zcl_thermostat_get_weekly_schedule_resp_message_t;

/**
 * @brief The Zigbee ZCL touchlink commissioning endpoint information struct
 *
 */
typedef struct esp_zb_touchlink_endpoint_information_record_s {
    esp_zb_ieee_addr_t addr_long;         /*!< The IEEE address of the local device */
    uint16_t addr_short;                  /**< The short network address of the local device */
    uint8_t endpoint_id;                  /*!< The identifier of the local endpoint */
    uint16_t profile_id;                  /*!< The identifier of the profile supported on the endpoint specified in the endpoint identifier field */
    uint16_t device_id;                   /*!< The identifier of the device description supported on the endpoint specified in the endpoint identifier field */
    uint8_t device_version;               /*!< The version of the device description supported by the sub-device on the endpoint specified by the endpoint identifier field */
    bool is_record;                       /*!< The parameter indicates whether the endpoint information is recorded in the record list */
} esp_zb_touchlink_endpoint_information_record_t;

/**
 * @brief The Zigbee touchlink get group identifiers request command struct
 *
 */
typedef struct esp_zb_touchlink_send_get_group_identifiers_cmd_s {
    esp_zb_zcl_basic_cmd_t zcl_basic_cmd; /*!< Basic command info */
    uint8_t start_index;                  /*!< The index (starting from 0) at which to start returning group identifiers. */
} esp_zb_touchlink_send_get_group_identifiers_cmd_t;

/**
 * @brief The Zigbee touchlink get endpoint list request command struct
 *
 */
typedef struct esp_zb_touchlink_send_get_endpoint_list_cmd_s {
    esp_zb_zcl_basic_cmd_t zcl_basic_cmd; /*!< Basic command info */
    uint8_t start_index;                  /*!< The index (starting from 0) at which to start returning endpoint identifiers. */
} esp_zb_touchlink_send_get_endpoint_list_cmd_t;

/**
 * @brief The Zigbee touchlink endpoint information request command struct
 *
 */
typedef struct esp_zb_touchlink_send_endpoint_information_cmd_s {
    esp_zb_zcl_basic_cmd_t zcl_basic_cmd; /*!< Basic command info */
    uint8_t endpoint_id;                  /*!< The identifier of the local endpoint */
    uint16_t profile_id;                  /*!< The identifier of the profile supported on the endpoint specified in the endpoint identifier field. */
    uint16_t device_id;                   /*!< The identifier of the device description supported on the endpoint specified in the endpoint identifier field */
    uint8_t device_version;               /*!< Device version;the most significant 4 bits shall be set to 0x0 */
} esp_zb_touchlink_send_endpoint_information_cmd_t;

/* @brief   Send touchlink get group identifiers request command
 *
 * @param[in] cmd_req Pointer to the get group identifiers request command, refer to esp_zb_touchlink_send_get_group_identifiers_cmd_t
 *
 */
void esp_zb_touchlink_send_get_group_identifiers_cmd_req(esp_zb_touchlink_send_get_group_identifiers_cmd_t *cmd_req);

/* @brief   Send touchlink get endpoint list request command
 *
 * @param[in] cmd_req Pointer to the get endpoint list request command, refer to esp_zb_touchlink_send_get_endpoint_list_cmd_t
 *
 */
void esp_zb_touchlink_send_get_endpoint_list_cmd_req(esp_zb_touchlink_send_get_endpoint_list_cmd_t *cmd_req);

/* @brief   Send touchlink endpoint information request command
 *
 * @param[in] cmd_req Pointer to the endpoint information request command, refer to esp_zb_touchlink_send_endpoint_information_cmd_t
 *
 */
void esp_zb_touchlink_send_ep_info_cmd_req(esp_zb_touchlink_send_endpoint_information_cmd_t *cmd_req);

/* read attribute, write attribute, config report and more general command will support later */

/**
 * @brief   Send read attribute command
 *
 * @param[in]  cmd_req  pointer to the read_attribute command @ref esp_zb_zcl_read_attr_cmd_s
 *
 * @return The transaction sequence number
 */
uint8_t esp_zb_zcl_read_attr_cmd_req(esp_zb_zcl_read_attr_cmd_t *cmd_req);

/**
 * @brief   Send write attribute command
 *
 * @param[in]  cmd_req  pointer to the write attribute command @ref esp_zb_zcl_write_attr_cmd_s
 *
 * @return The transaction sequence number
 */
uint8_t esp_zb_zcl_write_attr_cmd_req(esp_zb_zcl_write_attr_cmd_t *cmd_req);

/**
 * @brief   Send report attribute command
 *
 * @param[in]  cmd_req  pointer to the report attribute command @ref esp_zb_zcl_report_attr_cmd_s
 * @note This function just does one-shot report ignoring the reporting configuration.
 * @return - ESP_OK on success
 *
 */
esp_err_t esp_zb_zcl_report_attr_cmd_req(esp_zb_zcl_report_attr_cmd_t *cmd_req);

/**
 * @brief   Send config report command
 *
 * @param[in]  cmd_req  pointer to the config report command @ref esp_zb_zcl_config_report_cmd_s
 *
 * @return The transaction sequence number
 */
uint8_t esp_zb_zcl_config_report_cmd_req(esp_zb_zcl_config_report_cmd_t *cmd_req);

/**
 * @brief   Send read reporting configuration command
 *
 * @param[in]  cmd_req  pointer to the read report config command, @ref esp_zb_zcl_read_report_config_cmd_s
 *
 * @return The transaction sequence number
 */
uint8_t esp_zb_zcl_read_report_config_cmd_req(esp_zb_zcl_read_report_config_cmd_t *cmd_req);

/**
 * @brief Send discover attributes command
 *
 * @param[in] cmd_req pointer to the discover attributes command @ref esp_zb_zcl_disc_attr_cmd_s
 *
 * @return The transaction sequence number
 */
uint8_t esp_zb_zcl_disc_attr_cmd_req(esp_zb_zcl_disc_attr_cmd_t *cmd_req);

/* ZCL basic cluster list command */

/**
 * @brief   Send ZCL basic reset to factory default command
 *
 * @param[in]  cmd_req  pointer to the basic command @ref esp_zb_zcl_basic_fact_reset_cmd_s
 *
 * @return The transaction sequence number
 */
uint8_t esp_zb_zcl_basic_factory_reset_cmd_req(esp_zb_zcl_basic_fact_reset_cmd_t *cmd_req);

/* ZCL on off cluster list command */

/**
 * @brief   Send on-off command
 *
 * @param[in]  cmd_req  pointer to the on-off command @ref esp_zb_zcl_on_off_cmd_s
 *
 * @return The transaction sequence number
 */
uint8_t esp_zb_zcl_on_off_cmd_req(esp_zb_zcl_on_off_cmd_t *cmd_req);

/**
 * @brief Send on-off Off With Effect command
 *
 * @note The Off With Effect command allows devices to be turned off using enhanced ways of fading
 * @param[in] cmd_req pointer to the on-off off with effect command @ref esp_zb_zcl_on_off_off_with_effect_cmd_s
 *
 * @return The transaction sequence number
 */
uint8_t esp_zb_zcl_on_off_off_with_effect_cmd_req(esp_zb_zcl_on_off_off_with_effect_cmd_t *cmd_req);

/**
 * @brief Send on-off On With Recall Global Scene command
 *
 * @note The On With Recall Global Scene command allows the recall of the settings when the device was turned off.
 * @param[in] cmd_req pointer to the on-off on with recall global scene command @ref esp_zb_zcl_on_off_on_with_recall_global_scene_cmd_s
 *
 * @return The transaction sequence number
 */
uint8_t esp_zb_zcl_on_off_on_with_recall_global_scene_cmd_req(esp_zb_zcl_on_off_on_with_recall_global_scene_cmd_t *cmd_req);

/**
 * @brief Send on-off On With Timed Off command
 *
 * @note The On With Timed Off command allows devices to be turned on for a specific duration with a guarded off
         duration so that SHOULD the device be subsequently switched off.
 * @param[in] cmd_req pointer to the on-off on with timed off command @ref esp_zb_zcl_on_off_on_with_timed_off_cmd_s
 *
 * @return The transaction sequence number
 */
uint8_t esp_zb_zcl_on_off_on_with_timed_off_cmd_req(esp_zb_zcl_on_off_on_with_timed_off_cmd_t *cmd_req);

/* ZCL identify cluster list command */

/**
 * @brief   Send identify command
 *
 * @param[in]  cmd_req  pointer to the identify command @ref esp_zb_zcl_identify_cmd_s
 *
 * @return The transaction sequence number
 */
uint8_t esp_zb_zcl_identify_cmd_req(esp_zb_zcl_identify_cmd_t *cmd_req);

/**
 * @brief Send identify trigger effect command
 *
 * @param[in] cmd_req pointer to the identify trigger variant command refer to esp_zb_zcl_identify_trigger_variant_cmd_s
 *
 * @return The transaction sequence number
 */
uint8_t esp_zb_zcl_identify_trigger_effect_cmd_req(esp_zb_zcl_identify_trigger_effect_cmd_t *cmd_req);

/**
 * @brief   Send identify query command
 *
 * @param[in]  cmd_req  pointer to the identify query command @ref esp_zb_zcl_identify_query_cmd_s
 *
 * @return The transaction sequence number
 */
uint8_t esp_zb_zcl_identify_query_cmd_req(esp_zb_zcl_identify_query_cmd_t *cmd_req);

/* ZCL commissioning cluster list command */

/**
 * @brief   Send commissioning restart device command
 *
 * @param[in]  cmd_req  pointer to the commissioning restart device command @ref esp_zb_zcl_comm_restart_device_cmd_s
 *
 * @return The transaction sequence number
 */
uint8_t esp_zb_zcl_comm_restart_device_cmd_req(esp_zb_zcl_comm_restart_device_cmd_t *cmd_req);

/**
 * @brief   Send commissioning save startup parameters command
 *
 * @param[in]  cmd_req  pointer to the commissioning save startup parameters command @ref esp_zb_zcl_comm_save_startup_params_cmd_s
 *
 * @return The transaction sequence number
 */
uint8_t esp_zb_zcl_comm_save_startup_params_cmd_req(esp_zb_zcl_comm_save_startup_params_cmd_t *cmd_req);

/**
 * @brief   Send commissioning restore startup parameters command
 *
 * @param[in]  cmd_req  pointer to the commissioning restore startup parameters command @ref esp_zb_zcl_comm_restore_startup_params_cmd_s
 *
 * @return The transaction sequence number
 */
uint8_t esp_zb_zcl_comm_restore_startup_params_cmd_req(esp_zb_zcl_comm_restore_startup_params_cmd_t *cmd_req);

/**
 * @brief   Send commissioning reset startup parameters command
 *
 * @param[in]  cmd_req  pointer to the commissioning reset startup parameters command @ref esp_zb_zcl_comm_reset_startup_params_cmd_s
 *
 * @return The transaction sequence number
 */
uint8_t esp_zb_zcl_comm_reset_startup_params_cmd_req(esp_zb_zcl_comm_reset_startup_params_cmd_t *cmd_req);

/* ZCL level control cluster list command */

/**
 * @brief   Send move to level command
 *
 * @param[in]  cmd_req  pointer to the move to level command @ref esp_zb_zcl_move_to_level_cmd_s
 *
 * @return The transaction sequence number
 */
uint8_t esp_zb_zcl_level_move_to_level_cmd_req(esp_zb_zcl_move_to_level_cmd_t *cmd_req);

/**
 * @brief   Send move to level with on/off effect command
 *
 * @param[in]  cmd_req  pointer to the move to level command @ref esp_zb_zcl_move_to_level_cmd_s
 *
 * @return The transaction sequence number
 */
uint8_t esp_zb_zcl_level_move_to_level_with_onoff_cmd_req(esp_zb_zcl_move_to_level_cmd_t *cmd_req);

/**
 * @brief   Send move level command
 *
 * @param[in]  cmd_req  pointer to the move level command @ref esp_zb_zcl_level_move_cmd_s
 *
 * @return The transaction sequence number
 */
uint8_t esp_zb_zcl_level_move_cmd_req(esp_zb_zcl_level_move_cmd_t *cmd_req);

/**
 * @brief   Send move level with on/off effect command
 *
 * @param[in]  cmd_req  pointer to the move level command @ref esp_zb_zcl_level_move_cmd_s
 *
 * @return The transaction sequence number
 */
uint8_t esp_zb_zcl_level_move_with_onoff_cmd_req(esp_zb_zcl_level_move_cmd_t *cmd_req);

/**
 * @brief   Send step level command
 *
 * @param[in]  cmd_req  pointer to the step level command @ref esp_zb_zcl_level_step_cmd_s
 *
 * @return The transaction sequence number
 */
uint8_t esp_zb_zcl_level_step_cmd_req(esp_zb_zcl_level_step_cmd_t *cmd_req);

/**
 * @brief   Send step level with on/off effect command
 *
 * @param[in]  cmd_req  pointer to the step level command @ref esp_zb_zcl_level_step_cmd_s
 *
 * @return The transaction sequence number
 */
uint8_t esp_zb_zcl_level_step_with_onoff_cmd_req(esp_zb_zcl_level_step_cmd_t *cmd_req);

/**
 * @brief   Send stop level command
 *
 * @param[in]  cmd_req  pointer to the stop level command @ref esp_zb_zcl_level_stop_cmd_s
 *
 * @return The transaction sequence number
 */
uint8_t esp_zb_zcl_level_stop_cmd_req(esp_zb_zcl_level_stop_cmd_t *cmd_req);

/* ZCL color control cluster list command */

/**
 * @brief   Send color move to hue command
 *
 * @param[in]  cmd_req  pointer to the move to hue command @ref esp_zb_zcl_color_move_to_hue_cmd_s
 *
 * @return The transaction sequence number
 */
uint8_t esp_zb_zcl_color_move_to_hue_cmd_req(esp_zb_zcl_color_move_to_hue_cmd_t *cmd_req);

/**
 * @brief   Send color move hue command
 *
 * @param[in]  cmd_req  pointer to the move hue command @ref esp_zb_zcl_color_move_hue_cmd_s
 *
 * @return The transaction sequence number
 */
uint8_t esp_zb_zcl_color_move_hue_cmd_req(esp_zb_zcl_color_move_hue_cmd_t *cmd_req);

/**
 * @brief   Send color step hue command
 *
 * @param[in]  cmd_req  pointer to the step hue command @ref esp_zb_zcl_color_step_hue_cmd_s
 *
 * @return The transaction sequence number
 */
uint8_t esp_zb_zcl_color_step_hue_cmd_req(esp_zb_zcl_color_step_hue_cmd_t *cmd_req);

/**
 * @brief   Send color move to saturation command
 *
 * @param[in]  cmd_req  pointer to the move to saturation command @ref esp_zb_zcl_color_move_to_saturation_cmd_s
 *
 * @return The transaction sequence number
 */
uint8_t esp_zb_zcl_color_move_to_saturation_cmd_req(esp_zb_zcl_color_move_to_saturation_cmd_t *cmd_req);

/**
 * @brief   Send color move saturation command
 *
 * @param[in]  cmd_req  pointer to the move saturation command @ref esp_zb_zcl_color_move_saturation_cmd_s
 *
 * @return The transaction sequence number
 */
uint8_t esp_zb_zcl_color_move_saturation_cmd_req(esp_zb_zcl_color_move_saturation_cmd_t *cmd_req);

/**
 * @brief   Send color step saturation command
 *
 * @param[in]  cmd_req  pointer to the step saturation command @ref esp_zb_zcl_color_step_saturation_cmd_s
 *
 * @return The transaction sequence number
 */
uint8_t esp_zb_zcl_color_step_saturation_cmd_req(esp_zb_zcl_color_step_saturation_cmd_t *cmd_req);

/**
 * @brief   Send color move to hue and saturation command
 *
 * @param[in]  cmd_req  pointer to the move to hue and saturation command @ref esp_zb_color_move_to_hue_saturation_cmd_s
 *
 * @return The transaction sequence number
 */
uint8_t esp_zb_zcl_color_move_to_hue_and_saturation_cmd_req(esp_zb_color_move_to_hue_saturation_cmd_t *cmd_req);

/**
 * @brief   Send color move to color command
 *
 * @param[in]  cmd_req  pointer to the move to color command @ref esp_zb_zcl_color_move_to_color_cmd_s
 *
 * @return The transaction sequence number
 */
uint8_t esp_zb_zcl_color_move_to_color_cmd_req(esp_zb_zcl_color_move_to_color_cmd_t *cmd_req);

/**
 * @brief   Send color move color command
 *
 * @param[in]  cmd_req  pointer to the move color command @ref esp_zb_zcl_color_move_color_cmd_s
 *
 * @return The transaction sequence number
 */
uint8_t esp_zb_zcl_color_move_color_cmd_req(esp_zb_zcl_color_move_color_cmd_t *cmd_req);

/**
 * @brief   Send color step color command
 *
 * @param[in]  cmd_req  pointer to the step color command @ref esp_zb_zcl_color_step_color_cmd_s
 *
 * @return The transaction sequence number
 */
uint8_t esp_zb_zcl_color_step_color_cmd_req(esp_zb_zcl_color_step_color_cmd_t *cmd_req);

/**
 * @brief   Send color stop color command
 *
 * @param[in]  cmd_req  pointer to the stop color command @ref esp_zb_zcl_color_stop_move_step_cmd_s
 *
 * @return The transaction sequence number
 */
uint8_t esp_zb_zcl_color_stop_move_step_cmd_req(esp_zb_zcl_color_stop_move_step_cmd_t *cmd_req);

/**
 * @brief   Send color control move to color temperature command(0x0a)
 *
 * @param[in]  cmd_req  pointer to the move to color temperature command @ref esp_zb_zcl_color_move_to_color_temperature_cmd_s
 *
 * @return The transaction sequence number
 */
uint8_t esp_zb_zcl_color_move_to_color_temperature_cmd_req(esp_zb_zcl_color_move_to_color_temperature_cmd_t *cmd_req);

/**
 * @brief   Send color control enhanced move to hue command(0x40)
 *
 * @param[in]  cmd_req  pointer to the enhanced move to hue command @ref esp_zb_zcl_color_enhanced_move_to_hue_cmd_s
 *
 * @return The transaction sequence number
 */
uint8_t esp_zb_zcl_color_enhanced_move_to_hue_cmd_req(esp_zb_zcl_color_enhanced_move_to_hue_cmd_t *cmd_req);

/**
 * @brief   Send color control enhanced move hue command(0x41)
 *
 * @param[in]  cmd_req  pointer to the enhanced move hue command @ref esp_zb_zcl_color_enhanced_move_hue_cmd_s
 *
 * @return The transaction sequence number
 */
uint8_t esp_zb_zcl_color_enhanced_move_hue_cmd_req(esp_zb_zcl_color_enhanced_move_hue_cmd_t *cmd_req);

/**
 * @brief   Send color control enhanced step hue command(0x42)
 *
 * @param[in]  cmd_req  pointer to the enhanced step hue command @ref esp_zb_zcl_color_enhanced_step_hue_cmd_s
 *
 * @return The transaction sequence number
 */
uint8_t esp_zb_zcl_color_enhanced_step_hue_cmd_req(esp_zb_zcl_color_enhanced_step_hue_cmd_t *cmd_req);

/**
 * @brief   Send color control move to hue and saturation command(0x43)
 *
 * @param[in]  cmd_req  pointer to the enhanced move to hue saturation command @ref esp_zb_zcl_color_enhanced_move_to_hue_saturation_cmd_s
 *
 * @return The transaction sequence number
 */
uint8_t esp_zb_zcl_color_enhanced_move_to_hue_saturation_cmd_req(esp_zb_zcl_color_enhanced_move_to_hue_saturation_cmd_t *cmd_req);

/**
 * @brief   Send color control color loop set command(0x44)
 *
 * @param[in]  cmd_req  pointer to the color loop set command @ref esp_zb_zcl_color_color_loop_set_cmd_s
 *
 * @return The transaction sequence number
 */
uint8_t esp_zb_zcl_color_color_loop_set_cmd_req(esp_zb_zcl_color_color_loop_set_cmd_t *cmd_req);

/**
 * @brief   Send color control move color temperature command(0x4b)
 *
 * @param[in]  cmd_req  pointer to the move color temperature command @ref esp_zb_zcl_color_move_color_temperature_cmd_s
 *
 * @return The transaction sequence number
 */
uint8_t esp_zb_zcl_color_move_color_temperature_cmd_req(esp_zb_zcl_color_move_color_temperature_cmd_t *cmd_req);

/**
 * @brief   Send color control step color temperature command(0x4c)
 *
 * @param[in]  cmd_req  pointer to the step color temperature command @ref esp_zb_zcl_color_step_color_temperature_cmd_s
 *
 * @return The transaction sequence number
 */
uint8_t esp_zb_zcl_color_step_color_temperature_cmd_req(esp_zb_zcl_color_step_color_temperature_cmd_t *cmd_req);

/**
 * @brief   Send lock door command
 *
 * @param[in]  cmd_req  pointer to the unlock door command @ref esp_zb_zcl_lock_unlock_door_cmd_s
 *
 * @return The transaction sequence number
 */
uint8_t esp_zb_zcl_lock_door_cmd_req(esp_zb_zcl_lock_unlock_door_cmd_t *cmd_req);

/**
 * @brief   Send unlock door command
 *
 * @param[in]  cmd_req  pointer to the unlock door command @ref esp_zb_zcl_lock_unlock_door_cmd_s
 *
 * @return The transaction sequence number
 */
uint8_t esp_zb_zcl_unlock_door_cmd_req(esp_zb_zcl_lock_unlock_door_cmd_t *cmd_req);

/**
 * @brief   Send add group command
 *
 * @param[in]  cmd_req  pointer to the add group command @ref esp_zb_zcl_groups_add_group_cmd_s
 *
 * @return The transaction sequence number
 */
uint8_t esp_zb_zcl_groups_add_group_cmd_req(esp_zb_zcl_groups_add_group_cmd_t *cmd_req);

/**
 * @brief   Send remove group command
 *
 * @param[in]  cmd_req  pointer to the add group command @ref esp_zb_zcl_groups_add_group_cmd_s
 *
 * @return The transaction sequence number
 */
uint8_t esp_zb_zcl_groups_remove_group_cmd_req(esp_zb_zcl_groups_add_group_cmd_t *cmd_req);

/**
 * @brief   Send remove all groups command
 *
 * @param[in]  cmd_req  pointer to the remove all group command @ref esp_zb_zcl_groups_remove_all_groups_cmd_s
 *
 * @return The transaction sequence number
 */
uint8_t esp_zb_zcl_groups_remove_all_groups_cmd_req(esp_zb_zcl_groups_remove_all_groups_cmd_t *cmd_req);

/**
 * @brief   Send view group command
 *
 * @param[in]  cmd_req  pointer to the add group command @ref esp_zb_zcl_groups_add_group_cmd_s
 *
 * @return The transaction sequence number
 */
uint8_t esp_zb_zcl_groups_view_group_cmd_req(esp_zb_zcl_groups_add_group_cmd_t *cmd_req);

/**
 * @brief   Send get group membership command
 *
 * @param[in]  cmd_req  pointer to the get group membership command @ref esp_zb_zcl_groups_get_group_membership_cmd_s
 *
 * @return The transaction sequence number
 */
uint8_t esp_zb_zcl_groups_get_group_membership_cmd_req(esp_zb_zcl_groups_get_group_membership_cmd_t *cmd_req);

/**
 * @brief   Send add scene command
 *
 * @param[in]  cmd_req  pointer to the add scene command  @ref esp_zb_zcl_scenes_add_scene_cmd_s
 *
 * @return The transaction sequence number
 */
uint8_t esp_zb_zcl_scenes_add_scene_cmd_req(esp_zb_zcl_scenes_add_scene_cmd_t *cmd_req);

/**
 * @brief   Send remove scene command
 *
 * @param[in]  cmd_req  pointer to the remove scene command  @ref esp_zb_zcl_scenes_remove_scene_cmd_s
 *
 * @return The transaction sequence number
 */
uint8_t esp_zb_zcl_scenes_remove_scene_cmd_req(esp_zb_zcl_scenes_remove_scene_cmd_t *cmd_req);

/**
 * @brief   Send remove all scenes command
 *
 * @param[in]  cmd_req  pointer to the add scenes command  @ref esp_zb_zcl_scenes_remove_all_scenes_cmd_s
 *
 * @return The transaction sequence number
 */
uint8_t esp_zb_zcl_scenes_remove_all_scenes_cmd_req(esp_zb_zcl_scenes_remove_all_scenes_cmd_t *cmd_req);

/**
 * @brief   Send view scene command
 *
 * @param[in]  cmd_req  pointer to the view scene command  @ref esp_zb_zcl_scenes_view_scene_cmd_s
 *
 * @return The transaction sequence number
 */
uint8_t esp_zb_zcl_scenes_view_scene_cmd_req(esp_zb_zcl_scenes_view_scene_cmd_t *cmd_req);

/**
 * @brief   Send store scene command
 *
 * @param[in]  cmd_req  pointer to the store scene command  @ref esp_zb_zcl_scenes_store_scene_cmd_s
 *
 * @return The transaction sequence number
 */
uint8_t esp_zb_zcl_scenes_store_scene_cmd_req(esp_zb_zcl_scenes_store_scene_cmd_t *cmd_req);

/**
 * @brief   Send recall scene command
 *
 * @param[in]  cmd_req  pointer to the recall scene command  @ref esp_zb_zcl_scenes_recall_scene_cmd_s
 *
 * @return The transaction sequence number
 */
uint8_t esp_zb_zcl_scenes_recall_scene_cmd_req(esp_zb_zcl_scenes_recall_scene_cmd_t *cmd_req);

/**
 * @brief   Send get scene membership command
 *
 * @param[in]  cmd_req  pointer to the get scene membership command  @ref esp_zb_zcl_scenes_get_scene_membership_cmd_s
 *
 * @return The transaction sequence number
 */
uint8_t esp_zb_zcl_scenes_get_scene_membership_cmd_req(esp_zb_zcl_scenes_get_scene_membership_cmd_t *cmd_req);

/**
 * @brief   Send IAS zone enroll response command
 * @note Type 2 cluster from client to server
 * @param[in]  cmd_resp  pointer to the zone enroll response command  @ref esp_zb_zcl_ias_zone_enroll_response_cmd_s
 *
 * @return The transaction sequence number
 */
uint8_t esp_zb_zcl_ias_zone_enroll_cmd_resp(esp_zb_zcl_ias_zone_enroll_response_cmd_t *cmd_resp);

/**
 * @brief   Send IAS zone status change notification command
 * @note Type 2 cluster from server to client
 * @param[in]  cmd_req  pointer to the ias zone status change notification command  @ref esp_zb_zcl_ias_zone_status_change_notif_cmd_s
 *
 * @return The transaction sequence number
 */
uint8_t esp_zb_zcl_ias_zone_status_change_notif_cmd_req(esp_zb_zcl_ias_zone_status_change_notif_cmd_t *cmd_req);

/**
 * @brief   Send IAS zone enroll request command
 * @note Type 2 cluster from server to client
 * @param[in]  cmd_req  pointer to the ias zone enroll request command @ref esp_zb_zcl_ias_zone_enroll_request_cmd_s
 *
 * @return The transaction sequence number
 */
uint8_t esp_zb_zcl_ias_zone_enroll_cmd_req(esp_zb_zcl_ias_zone_enroll_request_cmd_t *cmd_req);

/**
 * @brief   Send IAS_ACE Arm command
 *
 * @param[in]  cmd_req  pointer to the Arm command  @ref esp_zb_zcl_ias_ace_arm_cmd_s
 *
 * @return The transaction sequence number
 */
uint8_t esp_zb_zcl_ias_ace_arm_cmd_req(esp_zb_zcl_ias_ace_arm_cmd_t *cmd_req);

/**
 * @brief   Send IAS_ACE Bypass command
 *
 * @param[in]  cmd_req  pointer to the Bypass command  @ref esp_zb_zcl_ias_ace_bypass_cmd_s
 *
 * @return The transaction sequence number
 */
uint8_t esp_zb_zcl_ias_ace_bypass_cmd_req(esp_zb_zcl_ias_ace_bypass_cmd_t *cmd_req);

/**
 * @brief   Send IAS_ACE Emergency command
 *
 * @param[in]  cmd_req  pointer to the Emergency command, refer to esp_zb_zcl_ias_ace_emergency_cmd_t
 *
 * @return The transaction sequence number
 */
uint8_t esp_zb_zcl_ias_ace_emergency_cmd_req(esp_zb_zcl_ias_ace_emergency_cmd_t *cmd_req);

/**
 * @brief   Send IAS_ACE Fire command
 *
 * @param[in]  cmd_req  pointer to the Fire command, refer to esp_zb_zcl_ias_ace_fire_cmd_t
 *
 * @return The transaction sequence number
 */
uint8_t esp_zb_zcl_ias_ace_fire_cmd_req(esp_zb_zcl_ias_ace_fire_cmd_t *cmd_req);

/**
 * @brief   Send IAS_ACE Panic command
 *
 * @param[in]  cmd_req  pointer to the Panic command, refer to esp_zb_zcl_ias_ace_panic_cmd_t
 *
 * @return The transaction sequence number
 */
uint8_t esp_zb_zcl_ias_ace_panic_cmd_req(esp_zb_zcl_ias_ace_panic_cmd_t *cmd_req);

/**
 * @brief   Send IAS_ACE Get Zone ID Map command
 *
 * @param[in]  cmd_req  pointer to the Get Zone ID Map command, refer to esp_zb_zcl_ias_ace_get_zone_id_map_cmd_t
 *
 * @return The transaction sequence number
 */
uint8_t esp_zb_zcl_ias_ace_get_zone_id_map_cmd_req(esp_zb_zcl_ias_ace_get_zone_id_map_cmd_t *cmd_req);

/**
 * @brief   Send IAS_ACE Get Zone Information command
 *
 * @param[in]  cmd_req  pointer to the Get Zone Information command, refer to esp_zb_zcl_ias_ace_get_zone_info_cmd_t
 *
 * @return The transaction sequence number
 */
uint8_t esp_zb_zcl_ias_ace_get_zone_information_cmd_req(esp_zb_zcl_ias_ace_get_zone_info_cmd_t *cmd_req);

/**
 * @brief   Send IAS_ACE Get Panel Status command
 *
 * @param[in]  cmd_req  pointer to the Get Panel Status command, refer to esp_zb_zcl_ias_ace_get_panel_status_cmd_s
 *
 * @return The transaction sequence number
 */
uint8_t esp_zb_zcl_ias_ace_get_panel_status_cmd_req(esp_zb_zcl_ias_ace_get_panel_status_cmd_t *cmd_req);

/**
 * @brief   Send IAS_ACE Get Bypassed Zone List command
 *
 * @param[in]  cmd_req  pointer to the Get Bypassed Zone List command, refer to esp_zb_zcl_ias_ace_get_bypassed_zone_list_cmd_t
 *
 * @return The transaction sequence number
 */
uint8_t esp_zb_zcl_ias_ace_get_bypassed_zone_list_cmd_req(esp_zb_zcl_ias_ace_get_bypassed_zone_list_cmd_t *cmd_req);

/**
 * @brief   Send IAS_ACE Get Zone Status command
 *
 * @param[in]  cmd_req  pointer to the Get Zone Status command, refer to esp_zb_zcl_ias_ace_get_zone_status_cmd_t
 *
 * @return The transaction sequence number
 */
uint8_t esp_zb_zcl_ias_ace_get_zone_status_cmd_req(esp_zb_zcl_ias_ace_get_zone_status_cmd_t *cmd_req);

/**
 * @brief   Send IAS_ACE Zone Status Changed command
 *
 * @param[in]  cmd_req  pointer to the Zone Status Changed command  @ref esp_zb_zcl_ias_ace_zone_status_changed_cmd_s
 *
 * @return The transaction sequence number
 */
uint8_t esp_zb_zcl_ias_ace_zone_status_changed_cmd_req(esp_zb_zcl_ias_ace_zone_status_changed_cmd_t *cmd_req);

/**
 * @brief   Send IAS_ACE Panel Status Changed command
 *
 * @param[in]  cmd_req  pointer to the Panel Status Changed command  @ref esp_zb_zcl_ias_ace_panel_status_changed_cmd_s
 *
 * @return The transaction sequence number
 */
uint8_t esp_zb_zcl_ias_ace_panel_status_changed_cmd_req(esp_zb_zcl_ias_ace_panel_status_changed_cmd_t *cmd_req);

/**
 * @brief   Send IAS_WD Start Warning command
 *
 * @param[in]  cmd_req  pointer to the Start Warning command  @ref esp_zb_zcl_ias_wd_start_warning_cmd_s
 *
 * @return The transaction sequence number
 */
uint8_t esp_zb_zcl_ias_wd_start_warning_cmd_req(esp_zb_zcl_ias_wd_start_warning_cmd_t *cmd_req);

/**
 * @brief   Send IAS_WD Squawk command
 *
 * @param[in]  cmd_req  pointer to the Squawk command  @ref esp_zb_zcl_ias_wd_squawk_cmd_s
 *
 * @return The transaction sequence number
 */
uint8_t esp_zb_zcl_ias_wd_squawk_cmd_req(esp_zb_zcl_ias_wd_squawk_cmd_t *cmd_req);

/**
 * @brief   Send window covering cluster command request
 *
 * @param[in]  cmd_req  pointer to the send custom cluster command request @ref esp_zb_zcl_window_covering_cluster_send_cmd_req_s
 *
 * @return The transaction sequence number
 */
uint8_t esp_zb_zcl_window_covering_cluster_send_cmd_req(esp_zb_zcl_window_covering_cluster_send_cmd_req_t *cmd_req);

/**
 * @brief   Get electrical measurement cluster profile info response
 *
 * @param[in]  cmd_req  pointer to the send custom cluster command response @ref esp_zb_zcl_electrical_profile_info_cmd_resp_s
 *
 * @return The transaction sequence number
 */
uint8_t esp_zb_zcl_electrical_measurement_cluster_get_profile_info_resp(esp_zb_zcl_electrical_profile_info_cmd_resp_t *cmd_req);

/**
 * @brief   Get electrical measurement cluster measurement profile response
 *
 * @param[in]  cmd_req  pointer to the send custom cluster command response @ref esp_zb_zcl_electrical_measurement_profile_cmd_resp_s
 *
 * @return The transaction sequence number
 */
uint8_t esp_zb_zcl_electrical_measurement_cluster_get_measurement_profile_resp(esp_zb_zcl_electrical_measurement_profile_cmd_resp_t *cmd_req);

/**
 * @brief   Send thermostat setpoint raise or lower command request
 *
 * @param[in]  cmd_req  pointer to the setpoint raise or lower command @ref esp_zb_zcl_thermostat_setpoint_raise_lower_request_cmd_s
 *
 * @return The transaction sequence number
 */
uint8_t esp_zb_zcl_thermostat_setpoint_raise_lower_cmd_req(esp_zb_zcl_thermostat_setpoint_raise_lower_request_cmd_t *cmd_req);

/**
 * @brief   Send thermostat set weekly schedule command request
 *
 * @param[in]  cmd_req  pointer to the set weekly schedule command @ref esp_zb_zcl_thermostat_set_weekly_schedule_request_cmd_s
 *
 * @return The transaction sequence number
 */
uint8_t esp_zb_zcl_thermostat_set_weekly_schedule_cmd_req(esp_zb_zcl_thermostat_set_weekly_schedule_request_cmd_t *cmd_req);

/**
 * @brief   Send thermostat get weekly schedule command request
 *
 * @param[in]  cmd_req  pointer to the get weekly schedule command @ref esp_zb_zcl_thermostat_get_weekly_schedule_request_cmd_s
 *
 * @return The transaction sequence number
 */
uint8_t esp_zb_zcl_thermostat_get_weekly_schedule_cmd_req(esp_zb_zcl_thermostat_get_weekly_schedule_request_cmd_t *cmd_req);

/**
 * @brief   Send thermostat clear weekly schedule command request
 *
 * @param[in]  cmd_req  pointer to the clear weekly schedule command @ref esp_zb_thermostat_clear_weekly_schedule_cmd_s
 *
 * @return The transaction sequence number
 */
uint8_t esp_zb_zcl_thermostat_clear_weekly_schedule_cmd_req(esp_zb_thermostat_clear_weekly_schedule_cmd_t *cmd_req);

/**
 * @brief   Send thermostat get relay status log command request
 *
 * @param[in]  cmd_req  pointer to the get relay status log command @ref esp_zb_thermostat_get_relay_status_log_cmd_s
 *
 * @return The transaction sequence number
 */
uint8_t esp_zb_zcl_thermostat_get_relay_status_log_cmd_req(esp_zb_thermostat_get_relay_status_log_cmd_t *cmd_req);

/**
 * @brief   Send metering get profile command request
 *
 * @param[in]  cmd_req  pointer to the get profile command @ref esp_zb_metering_get_profile_cmd_s
 *
 * @return The transaction sequence number
 */
uint8_t esp_zb_zcl_metering_get_profile_cmd_req(esp_zb_metering_get_profile_cmd_t *cmd_req);

/**
 * @brief   Send metering request fast poll mode command request
 *
 * @param[in]  cmd_req  pointer to the request fast poll mode command @ref esp_zb_metering_request_fast_poll_mode_cmd_s
 *
 * @return The transaction sequence number
 */
uint8_t esp_zb_zcl_metering_request_fast_poll_mode_cmd_req(esp_zb_metering_request_fast_poll_mode_cmd_t *cmd_req);

/**
 * @brief   Send metering get snapshot command request
 *
 * @param[in]  cmd_req  pointer to the get snapshot command @ref esp_zb_metering_get_snapshot_cmd_s
 *
 * @return The transaction sequence number
 */
uint8_t esp_zb_zcl_metering_get_snapshot_cmd_req(esp_zb_metering_get_snapshot_cmd_t *cmd_req);

/**
 * @brief   Send metering get sampled data command request
 *
 * @param[in]  cmd_req  pointer to the get sampled data command @ref esp_zb_metering_get_sampled_data_cmd_s
 *
 * @return The transaction sequence number
 */
uint8_t esp_zb_zcl_metering_get_sampled_data_cmd_req(esp_zb_metering_get_sampled_data_cmd_t *cmd_req);

/**
 * @brief   Send price get current price command request
 *
 * @param[in]  cmd_req  pointer to the get current price command @ref esp_zb_zcl_price_get_current_price_cmd_s
 *
 * @return The transaction sequence number
 */
uint8_t esp_zb_zcl_price_get_current_price_cmd_req(esp_zb_zcl_price_get_current_price_cmd_t *cmd_req);

/**
 * @brief   Send price get scheduled prices command request
 *
 * @param[in]  cmd_req  pointer to the get scheduled prices command @ref esp_zb_zcl_price_get_scheduled_prices_cmd_s
 *
 * @return The transaction sequence number
 */
uint8_t esp_zb_zcl_price_get_scheduled_prices_cmd_req(esp_zb_zcl_price_get_scheduled_prices_cmd_t *cmd_req);

/**
 * @brief   Send price get tier labels command request
 *
 * @param[in]  cmd_req  pointer to the get tier labels command @ref esp_zb_zcl_price_get_tier_labels_cmd_s
 *
 * @return The transaction sequence number
 */
uint8_t esp_zb_zcl_price_get_tier_labels_cmd_req(esp_zb_zcl_price_get_tier_labels_cmd_t *cmd_req);

/**
 * @brief   Send DRLC LoadControlEvent command request
 *
 * @param[in]  cmd_req  pointer to the LoadControlEvent command @ref esp_zb_zcl_drlc_load_control_event_req_s
 *
 * @return The transaction sequence number
 */
uint8_t esp_zb_zcl_drlc_load_control_event_req(esp_zb_zcl_drlc_load_control_event_req_t *cmd_req);

/**
 * @brief   Send DRLC CancelLoadControlEvent command request
 *
 * @param[in]  cmd_req  pointer to the CancelLoadControlEvent command @ref esp_zb_zcl_drlc_cancel_load_control_event_req_s
 *
 * @return The transaction sequence number
 */
uint8_t esp_zb_zcl_drlc_cancel_load_control_event_req(esp_zb_zcl_drlc_cancel_load_control_event_req_t *cmd_req);

/**
 * @brief   Send DRLC CancelAllLoadControlEvent command request
 *
 * @param[in]  cmd_req  pointer to the CancelAllLoadControlEvent command @ref esp_zb_zcl_drlc_cancel_all_load_control_events_req_s
 *
 * @return The transaction sequence number
 */
uint8_t esp_zb_zcl_drlc_cancel_all_load_control_events_req(esp_zb_zcl_drlc_cancel_all_load_control_events_req_t *cmd_req);

/**
 * @brief   Send DRLC ReportEventStatus command request
 *
 * @param[in]  cmd_req  pointer to the ReportEventStatus command @ref esp_zb_zcl_drlc_report_event_status_req_s
 *
 * @return The transaction sequence number
 */
uint8_t esp_zb_zcl_drlc_report_event_status_req(esp_zb_zcl_drlc_report_event_status_req_t *cmd_req);

/**
 * @brief   Send DRLC GetScheduledEvents command request
 *
 * @param[in]  cmd_req  pointer to the GetScheduledEvents command @ref esp_zb_zcl_drlc_get_scheduled_events_req_s
 *
 * @return The transaction sequence number
 */
uint8_t esp_zb_zcl_drlc_get_scheduled_events_req(esp_zb_zcl_drlc_get_scheduled_events_req_t *cmd_req);

#ifdef CONFIG_ZB_GP_ENABLED
/**
 * @brief Perform Proxy Commissioning mode enter request
 *
 * @param[in] req The parameter of request, refer to  esp_zgp_zcl_proxy_commissioning_enter_req_t
 *
 * @return The transaction sequence number
 */
uint8_t esp_zgp_zcl_proxy_commissioning_mode_enter_cmd_req(esp_zgp_zcl_proxy_commissioning_enter_req_t *req);

/**
 * @brief Perform Proxy Commissioning mode level request
 *
 * @return The transaction sequence number
 */
uint8_t esp_zgp_zcl_proxy_commissioning_mode_leave_cmd_req(void);

/**
 * @brief Perform Proxy Commissioning mode request
 *
 * @param[in] req The parameter of request, refer to esp_zgp_zcl_proxy_commissioning_req_t
 *
 * @return The transaction sequence number
 */
uint8_t esp_zgp_zcl_proxy_commissioning_mode_cmd_req(esp_zgp_zcl_proxy_commissioning_req_t *req);

/**
 * @brief Perform GP Pairing request
 *
 * @param[in] req The parameter of request, refer to esp_zgp_zcl_pairing_req_t
 *
 * @return The transaction sequence number
 */
uint8_t esp_zgp_zcl_pairing_cmd_req(esp_zgp_zcl_pairing_req_t *req);

/**
 * @brief Send zcl gp sink table request
 *
 * @param[in] req The parameter of request, refer to esp_zgp_zcl_gp_sink_table_req_t
 *
 * @return The transaction sequence number
 */
uint8_t esp_zgp_zcl_gp_sink_table_cmd_req(esp_zgp_zcl_gp_sink_table_req_t *req);

/**
 * @brief Send zcl gp proxy table request
 *
 * @param[in] req The parameter of request, refer to esp_zgp_zcl_gp_proxy_table_req_t
 *
 * @return The transaction sequence number
 */
uint8_t esp_zgp_zcl_gp_proxy_table_cmd_req(esp_zgp_zcl_gp_proxy_table_req_t *req);

/**
 * @brief Send zcl gp pairing configuration command
 *
 * @param[in] req The parameter of request, refer to esp_zgp_zcl_pairing_configuration_req_t
 *
 * @return The transaction sequence number
 */
uint8_t esp_zgp_zcl_pairing_configuration_cmd_req(esp_zgp_zcl_pairing_configuration_req_t *req);
#endif /* CONFIG_ZB_GP_ENABLED */

/**
 * @brief   Send custom cluster command request
 *
 * @param[in]  cmd_req  pointer to the send custom cluster command request, refer to esp_zb_zcl_custom_cluster_cmd_req_t
 *
 * @return The transaction sequence number
 */
uint8_t esp_zb_zcl_custom_cluster_cmd_req(esp_zb_zcl_custom_cluster_cmd_req_t *cmd_req);

/**
 * @brief   Send custom cluster command response
 *
 * @note The custom response should align in the same direction as the custom request when providing a reply.
 *
 * @param[in]  cmd_req  pointer to the send custom cluster command request, refer to esp_zb_zcl_custom_cluster_cmd_resp_t
 *
 * @return The transaction sequence number
 */
uint8_t esp_zb_zcl_custom_cluster_cmd_resp(esp_zb_zcl_custom_cluster_cmd_resp_t *cmd_req);

/**
 * @brief Update Zigbee ZCL custom cluster handlers
 *
 * @param[in] obj The object of cluster handlers will be updated in the stack if the cluster ID exists; otherwise, it will be added.
 *
 * @return
 *      - ESP_OK: On success
 *      - ESP_ERR_NO_MEM: Out of memory
 *      - ESP_ERR_INVALID_ARG: Invalid argument
 */
esp_err_t esp_zb_zcl_custom_cluster_handlers_update(esp_zb_zcl_custom_cluster_handlers_t obj);

/**
 * @brief Start and enable the attribute reporting.
 *
 * @param attr_info The location information of attribute which will be enabled to report
 * @return
 *      - ESP_OK: On success
 *      - ESP_FAIL: The reporting was not set up with @p attr_info
 */
esp_err_t esp_zb_zcl_start_attr_reporting(esp_zb_zcl_attr_location_info_t attr_info);

/**
 * @brief Stop and disable the attribute reporting.
 *
 * @param attr_info The location information of attribute which will be disabled to report
 * @return
 *      - ESP_OK: On success
 *      - ESP_FAIL: The reporting was not set down with @p attr_info
 */
esp_err_t esp_zb_zcl_stop_attr_reporting(esp_zb_zcl_attr_location_info_t attr_info);

/**
 * @brief Find the attribute reporting information from the device
 *
 * @param attr_info The location information of attribute, which will be utilized for looking up
 * @return
 *      - The result of finding the attribute reporting information
 */
esp_zb_zcl_reporting_info_t *esp_zb_zcl_find_reporting_info(esp_zb_zcl_attr_location_info_t attr_info);

/**
 * @brief Update the attribute report information
 *
 * @param report_info The attribute reporting information that will be updated for the related attribute
 * @return
 *      - ESP_OK: On success
 *      - ESP_ERR_NOT_FOUND: Not found the attribute
 *      - ESP_ERR_NO_MEM: Not memory
 *      - ESP_ERR_INVALID_ARG: Invalid argument
 *      - ESP_FAIL: On failure
 */
esp_err_t esp_zb_zcl_update_reporting_info(esp_zb_zcl_reporting_info_t *report_info);

/**
 * @brief Reset all reporting information of attributes
 *
 */
void esp_zb_zcl_reset_all_reporting_info(void);

#ifdef __cplusplus
}
#endif
