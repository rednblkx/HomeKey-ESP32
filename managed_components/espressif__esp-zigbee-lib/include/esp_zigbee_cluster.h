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

/**
 * @brief  Create an empty cluster list.
 *
 * @note  This cluster list is ready to add / update cluster refer to esp_zb_cluster_list_add_xxx_cluster() and esp_zb_cluster_list_update_xxx_cluster().
 * @return pointer to  @ref esp_zb_cluster_list_s
 *
 */
esp_zb_cluster_list_t *esp_zb_zcl_cluster_list_create(void);

/**
 * @brief  Get ZCL cluster descriptor.
 *
 * @note  Getting the local cluster from the specific endpoint.
 *
 * @param[in] endpoint The endpoint
 * @param[in] cluster_id The cluster id for attribute list refer to esp_zb_zcl_cluster_id
 * @param[in] cluster_role The cluster role of this cluster, either server or client role refer to esp_zb_zcl_cluster_role_t
 *
 * @return pointer to  @ref esp_zb_zcl_cluster_s
 *
 */
esp_zb_zcl_cluster_t *esp_zb_zcl_get_cluster(uint8_t endpoint, uint16_t cluster_id, uint8_t cluster_role);

/********************************* ZCL HA standard cluster ********************************/

/**
 * @brief  Create a standard basic cluster attribute list.
 *
 * @note  This only contains the mandatory attribute.
 * @param[in] basic_cfg  Configuration parameters for this cluster defined by @ref esp_zb_basic_cluster_cfg_s
 *
 * @return Pointer to attribute list @ref esp_zb_attribute_list_s
 *
 */
esp_zb_attribute_list_t *esp_zb_basic_cluster_create(esp_zb_basic_cluster_cfg_t *basic_cfg);

/**
 * @brief  Create a power configuration cluster attribute list.
 *
 * @note  No mandatory attributes are requested by the ZCL specs, in current implementation main voltage and frequency are added by default.
 * @param[in] power_cfg  Configuration parameters for this cluster defined by @ref esp_zb_power_config_cluster_cfg_s
 *
 * @return Pointer to attribute list @ref esp_zb_attribute_list_s
 *
 */
esp_zb_attribute_list_t *esp_zb_power_config_cluster_create(esp_zb_power_config_cluster_cfg_t *power_cfg);

/**
 * @brief  Create a standard identify cluster attribute list.
 *
 * @note  This only contains the mandatory attribute.
 * @param[in] identify_cfg  Configuration parameters for this cluster defined by @ref esp_zb_identify_cluster_cfg_s
 *
 * @return Pointer to attribute list @ref esp_zb_attribute_list_s
 *
 */
esp_zb_attribute_list_t *esp_zb_identify_cluster_create(esp_zb_identify_cluster_cfg_t *identify_cfg);

/**
 * @brief  Create a standard group cluster attribute list.
 *
 * @note  This only contains the mandatory attribute.
 * @param[in] group_cfg  Configuration parameters for this cluster defined by @ref esp_zb_groups_cluster_cfg_s
 *
 * @return Pointer to attribute list @ref esp_zb_attribute_list_s
 *
 */
esp_zb_attribute_list_t *esp_zb_groups_cluster_create(esp_zb_groups_cluster_cfg_t *group_cfg);

/**
 * @brief  Create a standard scenes cluster attribute list.
 *
 * @note  This only contains the mandatory attribute.
 * @param[in] scene_cfg  Configuration parameters for this cluster defined by @ref esp_zb_scenes_cluster_cfg_s
 *
 * @return Pointer to attribute list @ref esp_zb_attribute_list_s
 *
 */
esp_zb_attribute_list_t *esp_zb_scenes_cluster_create(esp_zb_scenes_cluster_cfg_t *scene_cfg);

/**
 * @brief  Create a standard on-off cluster attribute list.
 *
 * @note  This only contains the mandatory attribute.
 * @param[in] on_off_cfg  Configuration parameters for this cluster defined by @ref esp_zb_on_off_cluster_cfg_s
 *
 * @return Pointer to attribute list @ref esp_zb_attribute_list_s
 *
 */
esp_zb_attribute_list_t *esp_zb_on_off_cluster_create(esp_zb_on_off_cluster_cfg_t *on_off_cfg);

/**
 * @brief  Create a standard on-off switch config cluster attribute list.
 *
 * @note  This only contains the mandatory attribute. Switch type attribute default set to 0. User could change it if necessary.
 * @param[in] on_off_switch_cfg  Configuration parameters for this cluster defined by @ref esp_zb_on_off_switch_cluster_cfg_s
 *
 * @return Pointer to attribute list @ref esp_zb_attribute_list_s
 *
 */
esp_zb_attribute_list_t *esp_zb_on_off_switch_config_cluster_create(esp_zb_on_off_switch_cluster_cfg_t *on_off_switch_cfg);

/**
 * @brief  Create a standard level cluster attribute list.
 *
 * @note  This only contains the mandatory attribute.
 * @param[in] level_cfg  Configuration parameters for this cluster defined by @ref esp_zb_level_cluster_cfg_s
 *
 * @return Pointer to attribute list @ref esp_zb_attribute_list_s
 *
 */
esp_zb_attribute_list_t *esp_zb_level_cluster_create(esp_zb_level_cluster_cfg_t *level_cfg);

/**
 * @brief  Create a standard color cluster attribute list.
 *
 * @note  This only contains the mandatory attribute.
 * @param[in] color_cfg  Configuration parameters for this cluster defined by @ref esp_zb_color_cluster_cfg_s
 *
 * @return Pointer to attribute list @ref esp_zb_attribute_list_s
 *
 */
esp_zb_attribute_list_t *esp_zb_color_control_cluster_create(esp_zb_color_cluster_cfg_t *color_cfg);

/**
 * @brief  Create a standard time cluster attribute list.
 *
 * @note  This only contains the mandatory attribute.
 * @param[in] time_cfg  Configuration parameters for this cluster defined by @ref esp_zb_time_cluster_cfg_s
 *
 * @return Pointer to attribute list @ref esp_zb_attribute_list_s
 *
 */
esp_zb_attribute_list_t *esp_zb_time_cluster_create(esp_zb_time_cluster_cfg_t *time_cfg);

/**
 * @brief  Create a standard shade config cluster attribute list.
 *
 * @note  This only contains the mandatory attribute.
 * @param[in] shade_cfg  Configuration parameters for this cluster defined by @ref esp_zb_shade_config_cluster_cfg_s
 *
 * @return Pointer to attribute list @ref esp_zb_attribute_list_s
 *
 */
esp_zb_attribute_list_t *esp_zb_shade_config_cluster_create(esp_zb_shade_config_cluster_cfg_t *shade_cfg);

/**
 * @brief  Create a standard binary input (basic) cluster attribute list.
 *
 * @note  This only contains the mandatory attribute. Switch type attribute default set to 0. User could change it if necessary.
 * @param[in] binary_input_cfg  Configuration parameters for this cluster defined by @ref esp_zb_binary_input_cluster_cfg_s
 *
 * @return Pointer to attribute list @ref esp_zb_attribute_list_s
 *
 */
esp_zb_attribute_list_t *esp_zb_binary_input_cluster_create(esp_zb_binary_input_cluster_cfg_t *binary_input_cfg);

/**
 * @brief  Create a standard commissioning cluster attribute list.
 *
 * @note  This only contains the mandatory attribute. The ZCL spec 13.2.1.1 strongly recommended that this cluster only
 *        be deplayed on a single device endpoint, so we only supports single cluster instance for time being. Creating
 *        multiple clusters on different endpoints may cause unexpected behaviour.
 * @param[in] commissioning_cfg  Configuration parameters for this cluster defined by @ref esp_zb_commissioning_cluster_cfg_s
 *
 * @return Pointer to attribute list @ref esp_zb_attribute_list_s
 *
 */
esp_zb_attribute_list_t *esp_zb_commissioning_cluster_create(esp_zb_commissioning_cluster_cfg_t *commissioning_cfg);

/**
 * @brief  Create a standard door lock cluster attribute list.
 *
 * @note  This only contains the mandatory attribute.
 * @param[in] lock_cfg  Configuration parameters for this cluster defined by @ref esp_zb_door_lock_cluster_cfg_s
 *
 * @return Pointer to attribute list @ref esp_zb_attribute_list_s
 *
 */
esp_zb_attribute_list_t *esp_zb_door_lock_cluster_create(esp_zb_door_lock_cluster_cfg_t *lock_cfg);

/**
 * @brief  Create a standard IAS zone cluster attribute list.
 *
 * @note  This only contains the mandatory attribute. Switch type attribute default set to 0. User could change it if necessary.
 * @param[in] ias_zone_cfg  Configuration parameters for this cluster defined by @ref esp_zb_ias_zone_cluster_cfg_s
 *
 * @return Pointer to attribute list @ref esp_zb_attribute_list_s
 *
 */
esp_zb_attribute_list_t *esp_zb_ias_zone_cluster_create(esp_zb_ias_zone_cluster_cfg_t *ias_zone_cfg);

/**
 * @brief  Create a standard IAS ACE cluster attribute list.
 *
 * @note  This only contains the mandatory attribute.
 * @param[in] ias_ace_cfg Configuration parameters for this cluster defined by @ref esp_zb_ias_ace_cluster_cfg_s
 *
 * @return Pointer to attribute list @ref esp_zb_attribute_list_s
 *
 */
esp_zb_attribute_list_t *esp_zb_ias_ace_cluster_create(esp_zb_ias_ace_cluster_cfg_t *ias_ace_cfg);

/**
 * @brief  Create a standard IAS WD cluster attribute list.
 *
 * @note  This only contains the mandatory attribute.
 * @param[in] ias_wd_cfg  Configuration parameters for this cluster defined by @ref esp_zb_ias_wd_cluster_cfg_s
 *
 * @return Pointer to attribute list @ref esp_zb_attribute_list_s
 *
 */
esp_zb_attribute_list_t *esp_zb_ias_wd_cluster_create(esp_zb_ias_wd_cluster_cfg_t *ias_wd_cfg);

/**
 * @brief  Create a standard temperature measurement cluster attribute list.
 *
 * @note  This only contains the mandatory attribute.
 * @param[in] temperature_cfg  Configuration parameters for this cluster defined by @ref esp_zb_temperature_meas_cluster_cfg_s
 *
 * @return Pointer to attribute list @ref esp_zb_attribute_list_s
 *
 */
esp_zb_attribute_list_t *esp_zb_temperature_meas_cluster_create(esp_zb_temperature_meas_cluster_cfg_t *temperature_cfg);

/**
 * @brief  Create a standard humidity measurement cluster attribute list.
 *
 * @note  This only contains the mandatory attribute.
 * @param[in] humidity_cfg  Configuration parameters for this cluster defined by @ref esp_zb_humidity_meas_cluster_cfg_s
 *
 * @return Pointer to attribute list @ref esp_zb_attribute_list_s
 *
 */
esp_zb_attribute_list_t *esp_zb_humidity_meas_cluster_create(esp_zb_humidity_meas_cluster_cfg_t *humidity_cfg);

/**
 * @brief Create a standard illuminance measurement cluster attribute list.
 *
 * @param[in] illuminance_cfg Configuration parameters for this cluster defined by @ref esp_zb_illuminance_meas_cluster_cfg_s
 *
 * @return Pointer to attribute list @ref esp_zb_attribute_list_s
 *
 */
esp_zb_attribute_list_t *esp_zb_illuminance_meas_cluster_create(esp_zb_illuminance_meas_cluster_cfg_t *illuminance_cfg);

/**
 * @brief Create a standard pressure measurement cluster attribute list.
 *
 * @param[in] pressure_cfg Configuration parameters for this cluster defined by @ref esp_zb_pressure_meas_cluster_cfg_s
 *
 * @return Pointer to attribute list @ref esp_zb_attribute_list_s
 *
 */
esp_zb_attribute_list_t *esp_zb_pressure_meas_cluster_create(esp_zb_pressure_meas_cluster_cfg_t *pressure_cfg);

/**
 * @brief Create a standard flow measurement cluster attribute list.
 *
 * @param[in] flow_meas_cfg Configuration parameters for this cluster defined by @ref esp_zb_flow_meas_cluster_cfg_s
 *
 * @return Pointer to attribute list @ref esp_zb_attribute_list_s
 *
 */
esp_zb_attribute_list_t *esp_zb_flow_meas_cluster_create(esp_zb_flow_meas_cluster_cfg_t *flow_meas_cfg);

/**
 * @brief Create a standard electrical measurement cluster attribute list.
 *
 * @param[in] electrical_cfg Configuration parameters for this cluster defined by @ref esp_zb_electrical_meas_cluster_cfg_s
 *
 * @return Pointer to attribute list @ref esp_zb_attribute_list_s
 *
 */
esp_zb_attribute_list_t *esp_zb_electrical_meas_cluster_create(esp_zb_electrical_meas_cluster_cfg_t *electrical_cfg);

/**
 * @brief Create a standard occupancy sensor cluster attribute list.
 *
 * @param[in] sensing_cfg Configuration parameters for this cluster defined by @ref esp_zb_occupancy_sensing_cluster_cfg_s
 *
 * @return Pointer to attribute list @ref esp_zb_attribute_list_s
 *
 */
esp_zb_attribute_list_t *esp_zb_occupancy_sensing_cluster_create(esp_zb_occupancy_sensing_cluster_cfg_t *sensing_cfg);

/**
 * @brief Create a standard window covering cluster attribute list.
 *
 * @param[in] covering_cfg Configuration parameters for this cluster defined by @ref esp_zb_window_covering_cluster_cfg_s
 *
 * @return Pointer to attribute list @ref esp_zb_attribute_list_s
 *
 */
esp_zb_attribute_list_t *esp_zb_window_covering_cluster_create(esp_zb_window_covering_cluster_cfg_t *covering_cfg);

/**
 * @brief  Create a standard OTA cluster attribute list
 *
 * @note  This only contains the mandatory attribute
 * @param[in] ota_cfg  Configuration parameters for this cluster defined by @ref esp_zb_ota_cluster_cfg_s
 *
 * @return pointer to attribute list @ref esp_zb_attribute_list_s
 *
 */
esp_zb_attribute_list_t *esp_zb_ota_cluster_create(esp_zb_ota_cluster_cfg_t *ota_cfg);

/**
 * @brief  Create a standard touchlink commissioning cluster attribute list
 *
 * @note  This only contains the `ZB_ZCL_ATTR_GLOBAL_CLUSTER_REVISION_ID` and touchlink commissioning cluster has no attributes.
 *
 * @param[in] tl_cfg  Configuration parameters for this cluster defined by @ref esp_zb_touchlink_commissioning_cfg_s
 *
 * @return pointer to attribute list @ref esp_zb_attribute_list_s
 *
 */
esp_zb_attribute_list_t *esp_zb_touchlink_commissioning_cluster_create(esp_zb_touchlink_commissioning_cfg_t *tl_cfg);

/**
 * @brief  Create a standard thermostat cluster attribute list
 *
 * @param[in] thermostat_cfg  Configuration parameters for this cluster defined by @ref esp_zb_thermostat_cluster_cfg_s
 *
 * @return pointer to attribute list @ref esp_zb_attribute_list_s
 *
 */
esp_zb_attribute_list_t *esp_zb_thermostat_cluster_create(esp_zb_thermostat_cluster_cfg_t *thermostat_cfg);

/**
 * @brief  Create a standard fan control attribute list
 *
 * @param[in] fan_control_cfg  Configuration parameters for this cluster defined by @ref esp_zb_fan_control_cluster_cfg_s
 *
 * @return pointer to attribute list @ref esp_zb_attribute_list_s
 *
 */
esp_zb_attribute_list_t *esp_zb_fan_control_cluster_create(esp_zb_fan_control_cluster_cfg_t *fan_control_cfg);

/**
 * @brief  Create a standard thermostat ui config attribute list
 *
 * @param[in] thermostat_ui_config_cfg  Configuration parameters for this cluster defined by @ref esp_zb_thermostat_ui_config_cluster_cfg_s
 *
 * @return pointer to attribute list @ref esp_zb_attribute_list_s
 *
 */
esp_zb_attribute_list_t *esp_zb_thermostat_ui_config_cluster_create(esp_zb_thermostat_ui_config_cluster_cfg_t *thermostat_ui_config_cfg);

/**
 * @brief  Create a standard analog input attribute list
 *
 * @param[in] analog_input_cfg  Configuration parameters for this cluster defined by @ref esp_zb_analog_input_cluster_cfg_s
 *
 * @return pointer to attribute list @ref esp_zb_attribute_list_s
 *
 */
esp_zb_attribute_list_t *esp_zb_analog_input_cluster_create(esp_zb_analog_input_cluster_cfg_t *analog_input_cfg);

/**
 * @brief  Create a standard analog output attribute list
 *
 * @param[in] analog_output_cfg  Configuration parameters for this cluster defined by @ref esp_zb_analog_output_cluster_cfg_s
 *
 * @return pointer to attribute list @ref esp_zb_attribute_list_s
 *
 */
esp_zb_attribute_list_t *esp_zb_analog_output_cluster_create(esp_zb_analog_output_cluster_cfg_t *analog_output_cfg);

/**
 * @brief  Create a standard analog value attribute list
 *
 * @param[in] analog_value_cfg  Configuration parameters for this cluster defined by @ref esp_zb_analog_value_cluster_cfg_s
 *
 * @return pointer to attribute list @ref esp_zb_attribute_list_s
 *
 */
esp_zb_attribute_list_t *esp_zb_analog_value_cluster_create(esp_zb_analog_value_cluster_cfg_t *analog_value_cfg);

/**
 * @brief  Create a standard electrical conductivity measurement cluster attribute list.
 *
 * @note  This only contains the mandatory attribute.
 * @param[in] ec_measurement_cfg  Configuration parameters for this cluster defined by @ref esp_zb_ec_measurement_cluster_cfg_s
 *
 * @return Pointer to attribute list @ref esp_zb_attribute_list_s
 *
 */
esp_zb_attribute_list_t *esp_zb_ec_measurement_cluster_create(esp_zb_ec_measurement_cluster_cfg_t *ec_measurement_cfg);

/**
 * @brief  Create a standard pH measurement cluster attribute list.
 *
 * @note  This only contains the mandatory attribute.
 * @param[in] ph_measurement_cfg  Configuration parameters for this cluster defined by @ref esp_zb_ph_measurement_cluster_cfg_s
 *
 * @return Pointer to attribute list @ref esp_zb_attribute_list_s
 *
 */
esp_zb_attribute_list_t *esp_zb_ph_measurement_cluster_create(esp_zb_ph_measurement_cluster_cfg_t *ph_measurement_cfg);

/**
 * @brief  Create a standard wind speed measurement cluster attribute list.
 *
 * @note  This only contains the mandatory attribute.
 * @param[in] wind_speed_measurement_cfg  Configuration parameters for this cluster defined by @ref esp_zb_wind_speed_measurement_cluster_cfg_s
 *
 * @return Pointer to attribute list @ref esp_zb_attribute_list_s
 *
 */
esp_zb_attribute_list_t *esp_zb_wind_speed_measurement_cluster_create(esp_zb_wind_speed_measurement_cluster_cfg_t *wind_speed_measurement_cfg);

/**
 * @brief  Create a standard carbon dioxide measurement attribute list
 *
 * @param[in] carbon_dioxide_measurement_cfg  Configuration parameters for this cluster defined by @ref esp_zb_carbon_dioxide_measurement_cluster_cfg_s
 *
 * @return pointer to attribute list @ref esp_zb_attribute_list_s
 *
 */
esp_zb_attribute_list_t *esp_zb_carbon_dioxide_measurement_cluster_create(esp_zb_carbon_dioxide_measurement_cluster_cfg_t *carbon_dioxide_measurement_cfg);

/**
 * @brief  Create a standard pm2.5 measurement attribute list
 *
 * @param[in] pm2_5_measurement_cfg  Configuration parameters for this cluster defined by @ref esp_zb_pm2_5_measurement_cluster_cfg_s
 *
 * @return pointer to attribute list @ref esp_zb_attribute_list_s
 *
 */
esp_zb_attribute_list_t *esp_zb_pm2_5_measurement_cluster_create(esp_zb_pm2_5_measurement_cluster_cfg_t *pm2_5_measurement_cfg);

/**
 * @brief  Create a standard multistate value attribute list
 *
 * @param[in] multistate_value_cfg  Configuration parameters for this cluster defined by @ref esp_zb_multistate_value_cluster_cfg_s
 *
 * @return pointer to attribute list @ref esp_zb_attribute_list_s
 *
 */
esp_zb_attribute_list_t *esp_zb_multistate_value_cluster_create(esp_zb_multistate_value_cluster_cfg_t *multistate_value_cfg);

/**
 * @brief  Create a standard metering attribute list
 *
 * @param[in] metering_cfg  Configuration parameters for this cluster defined by @ref esp_zb_metering_cluster_cfg_s
 *
 * @return pointer to attribute list @ref esp_zb_attribute_list_s
 *
 */
esp_zb_attribute_list_t *esp_zb_metering_cluster_create(esp_zb_metering_cluster_cfg_t *metering_cfg);

/**
 * @brief  Create a standard diagnostics attribute list
 *
 * @param[in] diag_cfg  Configuration parameters for this cluster defined by @ref esp_zb_diagnostics_cluster_cfg_s
 *
 * @return pointer to attribute list @ref esp_zb_attribute_list_s
 *
 */
esp_zb_attribute_list_t *esp_zb_diagnostics_cluster_create(esp_zb_diagnostics_cluster_cfg_t *diag_cfg);

/**
 * @brief  Create a standard metering attribute list
 *
 * @param[in] meter_identification_cfg  Configuration parameters for this cluster defined by @ref esp_zb_meter_identification_cluster_cfg_s
 *
 * @return pointer to attribute list @ref esp_zb_attribute_list_s
 *
 */
esp_zb_attribute_list_t *esp_zb_meter_identification_cluster_create(esp_zb_meter_identification_cluster_cfg_t *meter_identification_cfg);

/**
 * @brief  Create a standard price attribute list
 *
 * @param[in] price_cfg Configuration parameters for this cluster defined by @ref esp_zb_price_cluster_cfg_s
 *
 * @return pointer to attribute list @ref esp_zb_attribute_list_s
 *
 */
esp_zb_attribute_list_t *esp_zb_price_cluster_create(esp_zb_price_cluster_cfg_t *price_cfg);

/**
 * @brief  Create a standard demand response and load control attribute list
 *
 * @param[in] drlc_cfg Configuration parameters for this cluster defined by @ref esp_zb_drlc_cluster_cfg_s
 *
 * @return pointer to attribute list @ref esp_zb_attribute_list_s
 *
 */
esp_zb_attribute_list_t *esp_zb_drlc_cluster_create(esp_zb_drlc_cluster_cfg_t *drlc_cfg);

/**************************************** ADD CLUSTER ***********************************/
/**
 * @brief Add basic cluster (attribute list) in a cluster list.
 *
 * @param[in] cluster_list A pointer to cluster list @ref esp_zb_cluster_list_s
 * @param[in] attr_list  An attribute list which wants to add
 * @param[in] role_mask  A role of server or client for this cluster (attribute list) refer to esp_zb_zcl_cluster_role_t
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_ARG if cluster list not initialized
 *
 */
esp_err_t esp_zb_cluster_list_add_basic_cluster(esp_zb_cluster_list_t *cluster_list, esp_zb_attribute_list_t *attr_list, uint8_t role_mask);

/**
 * @brief Add power configuration cluster (attribute list) in a cluster list.
 *
 * @param[in] cluster_list A pointer to cluster list @ref esp_zb_cluster_list_s
 * @param[in] attr_list  An attribute list which wants to add
 * @param[in] role_mask  A role of server or client for this cluster (attribute list) refer to esp_zb_zcl_cluster_role_t
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_ARG if cluster list not initialized
 *
 */
esp_err_t esp_zb_cluster_list_add_power_config_cluster(esp_zb_cluster_list_t *cluster_list, esp_zb_attribute_list_t *attr_list, uint8_t role_mask);

/**
 * @brief Add identify cluster (attribute list) in a cluster list.
 *
 * @param[in] cluster_list A pointer to cluster list @ref esp_zb_cluster_list_s
 * @param[in] attr_list  An attribute list which wants to add
 * @param[in] role_mask  A role of server or client for this cluster (attribute list) refer to esp_zb_zcl_cluster_role_t
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_ARG if cluster list not initialized
 *
 */
esp_err_t esp_zb_cluster_list_add_identify_cluster(esp_zb_cluster_list_t *cluster_list, esp_zb_attribute_list_t *attr_list, uint8_t role_mask);

/**
 * @brief Add groups cluster (attribute list) in a cluster list.
 *
 * @param[in] cluster_list A pointer to cluster list @ref esp_zb_cluster_list_s
 * @param[in] attr_list  An attribute list which wants to add
 * @param[in] role_mask  A role of server or client for this cluster (attribute list) refer to esp_zb_zcl_cluster_role_t
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_ARG if cluster list not initialized
 *
 */
esp_err_t esp_zb_cluster_list_add_groups_cluster(esp_zb_cluster_list_t *cluster_list, esp_zb_attribute_list_t *attr_list, uint8_t role_mask);

/**
 * @brief Add scenes cluster (attribute list) in a cluster list.
 *
 * @param[in] cluster_list A pointer to cluster list @ref esp_zb_cluster_list_s
 * @param[in] attr_list  An attribute list which wants to add
 * @param[in] role_mask  A role of server or client for this cluster (attribute list) refer to esp_zb_zcl_cluster_role_t
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_ARG if cluster list not initialized
 *
 */
esp_err_t esp_zb_cluster_list_add_scenes_cluster(esp_zb_cluster_list_t *cluster_list, esp_zb_attribute_list_t *attr_list, uint8_t role_mask);

/**
 * @brief Add on_off cluster (attribute list) in a cluster list.
 *
 * @param[in] cluster_list A pointer to cluster list @ref esp_zb_cluster_list_s
 * @param[in] attr_list  An attribute list which wants to add
 * @param[in] role_mask  A role of server or client for this cluster (attribute list) refer to esp_zb_zcl_cluster_role_t
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_ARG if cluster list not initialized
 *
 */
esp_err_t esp_zb_cluster_list_add_on_off_cluster(esp_zb_cluster_list_t *cluster_list, esp_zb_attribute_list_t *attr_list, uint8_t role_mask);

/**
 * @brief Add on_off switch config cluster (attribute list) in a cluster list.
 *
 * @param[in] cluster_list A pointer to cluster list @ref esp_zb_cluster_list_s
 * @param[in] attr_list  An attribute list which wants to add
 * @param[in] role_mask  A role of server or client for this cluster (attribute list) refer to esp_zb_zcl_cluster_role_t
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_ARG if cluster list not initialized
 *
 */
esp_err_t esp_zb_cluster_list_add_on_off_switch_config_cluster(esp_zb_cluster_list_t *cluster_list, esp_zb_attribute_list_t *attr_list, uint8_t role_mask);

/**
 * @brief Add level cluster (attribute list) in a cluster list.
 *
 * @param[in] cluster_list A pointer to cluster list @ref esp_zb_cluster_list_s
 * @param[in] attr_list  An attribute list which wants to add
 * @param[in] role_mask  A role of server or client for this cluster (attribute list) refer to esp_zb_zcl_cluster_role_t
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_ARG if cluster list not initialized
 *
 */
esp_err_t esp_zb_cluster_list_add_level_cluster(esp_zb_cluster_list_t *cluster_list, esp_zb_attribute_list_t *attr_list, uint8_t role_mask);

/**
 * @brief Add color_control cluster (attribute list) in a cluster list.
 *
 * @param[in] cluster_list A pointer to cluster list @ref esp_zb_cluster_list_s
 * @param[in] attr_list  An attribute list which wants to add
 * @param[in] role_mask  A role of server or client for this cluster (attribute list) refer to esp_zb_zcl_cluster_role_t
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_ARG if cluster list not initialized
 *
 */
esp_err_t esp_zb_cluster_list_add_color_control_cluster(esp_zb_cluster_list_t *cluster_list, esp_zb_attribute_list_t *attr_list, uint8_t role_mask);

/**
 * @brief Add Time cluster (attribute list) in a cluster list.
 *
 * @param[in] cluster_list A pointer to cluster list @ref esp_zb_cluster_list_s
 * @param[in] attr_list  An attribute list which wants to add
 * @param[in] role_mask  A role of server or client for this cluster (attribute list) refer to esp_zb_zcl_cluster_role_t
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_ARG if cluster list not initialized
 *
 */
esp_err_t esp_zb_cluster_list_add_time_cluster(esp_zb_cluster_list_t *cluster_list, esp_zb_attribute_list_t *attr_list, uint8_t role_mask);

/**
 * @brief Add Shade Config cluster (attribute list) in a cluster list.
 *
 * @param[in] cluster_list A pointer to cluster list @ref esp_zb_cluster_list_s
 * @param[in] attr_list  An attribute list which wants to add
 * @param[in] role_mask  A role of server or client for this cluster (attribute list) refer to esp_zb_zcl_cluster_role_t
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_ARG if cluster list not initialized
 *
 */
esp_err_t esp_zb_cluster_list_add_shade_config_cluster(esp_zb_cluster_list_t *cluster_list, esp_zb_attribute_list_t *attr_list, uint8_t role_mask);

/**
 * @brief Add binary input (basic) cluster (attribute list) in a cluster list.
 *
 * @param[in] cluster_list A pointer to cluster list @ref esp_zb_cluster_list_s
 * @param[in] attr_list  An attribute list which wants to add
 * @param[in] role_mask  A role of server or client for this cluster (attribute list) refer to esp_zb_zcl_cluster_role_t
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_ARG if cluster list not initialized
 *
 */
esp_err_t esp_zb_cluster_list_add_binary_input_cluster(esp_zb_cluster_list_t *cluster_list, esp_zb_attribute_list_t *attr_list, uint8_t role_mask);

/**
 * @brief Add Commissioning cluster (attribute list) in a cluster list.
 *
 * @param[in] cluster_list A pointer to cluster list @ref esp_zb_cluster_list_s
 * @param[in] attr_list  An attribute list which wants to add
 * @param[in] role_mask  A role of server or client for this cluster (attribute list) refer to esp_zb_zcl_cluster_role_t
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_ARG if cluster list not initialized
 *
 */
esp_err_t esp_zb_cluster_list_add_commissioning_cluster(esp_zb_cluster_list_t *cluster_list, esp_zb_attribute_list_t *attr_list, uint8_t role_mask);

/**
 * @brief Add ias zone cluster (attribute list) in a cluster list.
 *
 * @param[in] cluster_list A pointer to cluster list @ref esp_zb_cluster_list_s
 * @param[in] attr_list  An attribute list which wants to add
 * @param[in] role_mask  A role of server or client for this cluster (attribute list) refer to esp_zb_zcl_cluster_role_t
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_ARG if cluster list not initialized
 *
 */
esp_err_t esp_zb_cluster_list_add_ias_zone_cluster(esp_zb_cluster_list_t *cluster_list, esp_zb_attribute_list_t *attr_list, uint8_t role_mask);

/**
 * @brief Add IAS ACE cluster (attribute list) in a cluster list.
 *
 * @param[in] cluster_list A pointer to cluster list @ref esp_zb_cluster_list_s
 * @param[in] attr_list  An attribute list which wants to add
 * @param[in] role_mask  A role of server or client for this cluster (attribute list) refer to esp_zb_zcl_cluster_role_t
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_ARG if cluster list not initialized
 *
 */
esp_err_t esp_zb_cluster_list_add_ias_ace_cluster(esp_zb_cluster_list_t *cluster_list, esp_zb_attribute_list_t *attr_list, uint8_t role_mask);

/**
 * @brief Add IAS_WD cluster (attribute list) in a cluster list.
 *
 * @param[in] cluster_list A pointer to cluster list @ref esp_zb_cluster_list_s
 * @param[in] attr_list  An attribute list which wants to add
 * @param[in] role_mask  A role of server or client for this cluster (attribute list) refer to esp_zb_zcl_cluster_role_t
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_ARG if cluster list not initialized
 *
 */
esp_err_t esp_zb_cluster_list_add_ias_wd_cluster(esp_zb_cluster_list_t *cluster_list, esp_zb_attribute_list_t *attr_list, uint8_t role_mask);

/**
 * @brief Add Door Lock cluster (attribute list) in a cluster list.
 *
 * @param[in] cluster_list A pointer to cluster list @ref esp_zb_cluster_list_s
 * @param[in] attr_list  An attribute list which wants to add
 * @param[in] role_mask  A role of server or client for this cluster (attribute list) refer to esp_zb_zcl_cluster_role_t
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_ARG if cluster list not initialized
 *
 */
esp_err_t esp_zb_cluster_list_add_door_lock_cluster(esp_zb_cluster_list_t *cluster_list, esp_zb_attribute_list_t *attr_list, uint8_t role_mask);

/**
 * @brief Add Temperature measurement cluster (attribute list) in a cluster list.
 *
 * @param[in] cluster_list A pointer to cluster list @ref esp_zb_cluster_list_s
 * @param[in] attr_list  An attribute list which wants to add
 * @param[in] role_mask  A role of server or client for this cluster (attribute list) refer to esp_zb_zcl_cluster_role_t
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_ARG if cluster list not initialized
 *
 */
esp_err_t esp_zb_cluster_list_add_temperature_meas_cluster(esp_zb_cluster_list_t *cluster_list, esp_zb_attribute_list_t *attr_list, uint8_t role_mask);

/**
 * @brief Add Humidity measurement cluster (attribute list) in a cluster list.
 *
 * @param[in] cluster_list A pointer to cluster list @ref esp_zb_cluster_list_s
 * @param[in] attr_list  An attribute list which wants to add
 * @param[in] role_mask  A role of server or client for this cluster (attribute list) refer to esp_zb_zcl_cluster_role_t
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_ARG if cluster list not initialized
 *
 */
esp_err_t esp_zb_cluster_list_add_humidity_meas_cluster(esp_zb_cluster_list_t *cluster_list, esp_zb_attribute_list_t *attr_list, uint8_t role_mask);


/**
 * @brief Add illuminance measurement cluster (attribute list) in a cluster list.
 *
 * @param[in] cluster_list A pointer to cluster list @ref esp_zb_cluster_list_s
 * @param[in] attr_list  An attribute list which wants to add
 * @param[in] role_mask  A role of server or client for this cluster (attribute list) refer to esp_zb_zcl_cluster_role_t
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_ARG if cluster list not initialized
 *
 */
esp_err_t esp_zb_cluster_list_add_illuminance_meas_cluster(esp_zb_cluster_list_t *cluster_list, esp_zb_attribute_list_t *attr_list, uint8_t role_mask);

/**
 * @brief Add pressure measurement cluster (attribute list) in a cluster list.
 *
 * @param[in] cluster_list A pointer to cluster list @ref esp_zb_cluster_list_s
 * @param[in] attr_list  An attribute list which wants to add
 * @param[in] role_mask  A role of server or client for this cluster (attribute list) refer to esp_zb_zcl_cluster_role_t
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_ARG if cluster list not initialized
 *
 */
esp_err_t esp_zb_cluster_list_add_pressure_meas_cluster(esp_zb_cluster_list_t *cluster_list, esp_zb_attribute_list_t *attr_list, uint8_t role_mask);

/**
 * @brief Add flow measurement cluster (attribute list) in a cluster list.
 *
 * @param[in] cluster_list A pointer to cluster list @ref esp_zb_cluster_list_s
 * @param[in] attr_list  An attribute list which wants to add
 * @param[in] role_mask  A role of server or client for this cluster (attribute list) refer to esp_zb_zcl_cluster_role_t
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_ARG if cluster list not initialized
 *
 */
esp_err_t esp_zb_cluster_list_add_flow_meas_cluster(esp_zb_cluster_list_t *cluster_list, esp_zb_attribute_list_t *attr_list, uint8_t role_mask);

/**
 * @brief Add electrical measurement cluster (attribute list) in a cluster list.
 *
 * @param[in] cluster_list A pointer to cluster list @ref esp_zb_cluster_list_s
 * @param[in] attr_list  An attribute list which wants to add
 * @param[in] role_mask  A role of server or client for this cluster (attribute list) refer to esp_zb_zcl_cluster_role_t
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_ARG if cluster list not initialized
 *
 */
esp_err_t esp_zb_cluster_list_add_electrical_meas_cluster(esp_zb_cluster_list_t *cluster_list, esp_zb_attribute_list_t *attr_list, uint8_t role_mask);

/**
 * @brief Add occupancy sensor cluster (attribute list) in a cluster list.
 *
 * @param[in] cluster_list A pointer to cluster list @ref esp_zb_cluster_list_s
 * @param[in] attr_list  An attribute list which wants to add
 * @param[in] role_mask  A role of server or client for this cluster (attribute list) refer to esp_zb_zcl_cluster_role_t
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_ARG if cluster list not initialized
 *
 */
esp_err_t esp_zb_cluster_list_add_occupancy_sensing_cluster(esp_zb_cluster_list_t *cluster_list, esp_zb_attribute_list_t *attr_list, uint8_t role_mask);

/**
 * @brief Add window covering cluster (attribute list) in a cluster list.
 *
 * @param[in] cluster_list A pointer to cluster list @ref esp_zb_cluster_list_s
 * @param[in] attr_list  An attribute list which wants to add
 * @param[in] role_mask  A role of server or client for this cluster (attribute list) refer to esp_zb_zcl_cluster_role_t
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_ARG if cluster list not initialized
 *
 */
esp_err_t esp_zb_cluster_list_add_window_covering_cluster(esp_zb_cluster_list_t *cluster_list, esp_zb_attribute_list_t *attr_list, uint8_t role_mask);

/**
 * @brief Add OTA cluster (attribute list) in a cluster list.
 *
 * @param[in] cluster_list A pointer to cluster list @ref esp_zb_cluster_list_s
 * @param[in] attr_list  An attribute list which wants to add
 * @param[in] role_mask  A role of server or client for this cluster (attribute list) refer to esp_zb_zcl_cluster_role_t
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_ARG if cluster list not initialized
 *
 */
esp_err_t esp_zb_cluster_list_add_ota_cluster(esp_zb_cluster_list_t *cluster_list, esp_zb_attribute_list_t *attr_list, uint8_t role_mask);

/**
 * @brief Add touchlink commissioning cluster (attribute list) in a cluster list.
 *
 * @param[in] cluster_list A pointer to cluster list @ref esp_zb_cluster_list_s
 * @param[in] attr_list An attribute list which wants to add
 * @param[in] role_mask A role of server or client for this cluster (attribute list) refer to esp_zb_zcl_cluster_role_t
 *
 * @note The touchlink commissioning cluster id should not be same as already existing zcl cluster id refer to zb_zcl_common.h
 *
 * @return
 * - ESP_OK on success
 * - ESP_ERR_INVALID_ARG if cluster list not initialized
 *
 */
esp_err_t esp_zb_cluster_list_add_touchlink_commissioning_cluster(esp_zb_cluster_list_t *cluster_list, esp_zb_attribute_list_t *attr_list, uint8_t role_mask);

/**
 * @brief Add thermostat cluster (attribute list) in a cluster list.
 *
 * @param[in] cluster_list A pointer to cluster list @ref esp_zb_cluster_list_s
 * @param[in] attr_list  An attribute list which wants to add
 * @param[in] role_mask  A role of server or client for this cluster (attribute list) refer to esp_zb_zcl_cluster_role_t
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_ARG if cluster list not initialized
 *
 */
esp_err_t esp_zb_cluster_list_add_thermostat_cluster(esp_zb_cluster_list_t *cluster_list, esp_zb_attribute_list_t *attr_list, uint8_t role_mask);

/**
 * @brief Add fan control cluster (attribute list) in a cluster list.
 *
 * @param[in] cluster_list A pointer to cluster list @ref esp_zb_cluster_list_s
 * @param[in] attr_list  An attribute list which wants to add
 * @param[in] role_mask  A role of server or client for this cluster (attribute list) refer to esp_zb_zcl_cluster_role_t
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_ARG if cluster list not initialized
 *
 */
esp_err_t esp_zb_cluster_list_add_fan_control_cluster(esp_zb_cluster_list_t *cluster_list, esp_zb_attribute_list_t *attr_list, uint8_t role_mask);

/**
 * @brief Add thermostat ui config cluster (attribute list) in a cluster list.
 *
 * @param[in] cluster_list A pointer to cluster list @ref esp_zb_cluster_list_s
 * @param[in] attr_list  An attribute list which wants to add
 * @param[in] role_mask  A role of server or client for this cluster (attribute list) refer to esp_zb_zcl_cluster_role_t
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_ARG if cluster list not initialized
 *
 */
esp_err_t esp_zb_cluster_list_add_thermostat_ui_config_cluster(esp_zb_cluster_list_t *cluster_list, esp_zb_attribute_list_t *attr_list, uint8_t role_mask);

/**
 * @brief Add analog input cluster (attribute list) in a cluster list.
 *
 * @param[in] cluster_list A pointer to cluster list @ref esp_zb_cluster_list_s
 * @param[in] attr_list  An attribute list which wants to add
 * @param[in] role_mask  A role of server or client for this cluster (attribute list) refer to esp_zb_zcl_cluster_role_t
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_ARG if cluster list not initialized
 *
 */
esp_err_t esp_zb_cluster_list_add_analog_input_cluster(esp_zb_cluster_list_t *cluster_list, esp_zb_attribute_list_t *attr_list, uint8_t role_mask);

/**
 * @brief Add analog output cluster (attribute list) in a cluster list.
 *
 * @param[in] cluster_list A pointer to cluster list @ref esp_zb_cluster_list_s
 * @param[in] attr_list  An attribute list which wants to add
 * @param[in] role_mask  A role of server or client for this cluster (attribute list) refer to esp_zb_zcl_cluster_role_t
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_ARG if cluster list not initialized
 *
 */
esp_err_t esp_zb_cluster_list_add_analog_output_cluster(esp_zb_cluster_list_t *cluster_list, esp_zb_attribute_list_t *attr_list, uint8_t role_mask);

/**
 * @brief Add analog value cluster (attribute list) in a cluster list.
 *
 * @param[in] cluster_list A pointer to cluster list @ref esp_zb_cluster_list_s
 * @param[in] attr_list  An attribute list which wants to add
 * @param[in] role_mask  A role of server or client for this cluster (attribute list) refer to esp_zb_zcl_cluster_role_t
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_ARG if cluster list not initialized
 *
 */
esp_err_t esp_zb_cluster_list_add_analog_value_cluster(esp_zb_cluster_list_t *cluster_list, esp_zb_attribute_list_t *attr_list, uint8_t role_mask);

/**
 * @brief Add electrical conductivity measurement cluster (attribute list) in a cluster list.
 *
 * @param[in] cluster_list A pointer to cluster list @ref esp_zb_cluster_list_s
 * @param[in] attr_list  An attribute list which wants to add
 * @param[in] role_mask  A role of server or client for this cluster (attribute list) refer to esp_zb_zcl_cluster_role_t
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_ARG if cluster list not initialized
 *
 */
esp_err_t esp_zb_cluster_list_add_ec_measurement_cluster(esp_zb_cluster_list_t *cluster_list, esp_zb_attribute_list_t *attr_list, uint8_t role_mask);

/**
 * @brief Add pH measurement cluster (attribute list) in a cluster list.
 *
 * @param[in] cluster_list A pointer to cluster list @ref esp_zb_cluster_list_s
 * @param[in] attr_list  An attribute list which wants to add
 * @param[in] role_mask  A role of server or client for this cluster (attribute list) refer to esp_zb_zcl_cluster_role_t
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_ARG if cluster list not initialized
 *
 */
esp_err_t esp_zb_cluster_list_add_ph_measurement_cluster(esp_zb_cluster_list_t *cluster_list, esp_zb_attribute_list_t *attr_list, uint8_t role_mask);

/**
 * @brief Add wind speed measurement cluster (attribute list) in a cluster list.
 *
 * @param[in] cluster_list A pointer to cluster list @ref esp_zb_cluster_list_s
 * @param[in] attr_list  An attribute list which wants to add
 * @param[in] role_mask  A role of server or client for this cluster (attribute list) refer to esp_zb_zcl_cluster_role_t
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_ARG if cluster list not initialized
 *
 */
esp_err_t esp_zb_cluster_list_add_wind_speed_measurement_cluster(esp_zb_cluster_list_t *cluster_list, esp_zb_attribute_list_t *attr_list, uint8_t role_mask);

/**
 * @brief Add carbon dioxide measurement cluster (attribute list) in a cluster list.
 *
 * @param[in] cluster_list A pointer to cluster list @ref esp_zb_cluster_list_s
 * @param[in] attr_list  An attribute list which wants to add
 * @param[in] role_mask  A role of server or client for this cluster (attribute list) refer to esp_zb_zcl_cluster_role_t
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_ARG if cluster list not initialized
 *
 */
esp_err_t esp_zb_cluster_list_add_carbon_dioxide_measurement_cluster(esp_zb_cluster_list_t *cluster_list, esp_zb_attribute_list_t *attr_list, uint8_t role_mask);

/**
 * @brief Add pm2.5 measurement cluster (attribute list) in a cluster list.
 *
 * @param[in] cluster_list A pointer to cluster list @ref esp_zb_cluster_list_s
 * @param[in] attr_list  An attribute list which wants to add
 * @param[in] role_mask  A role of server or client for this cluster (attribute list) refer to esp_zb_zcl_cluster_role_t
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_ARG if cluster list not initialized
 *
 */
esp_err_t esp_zb_cluster_list_add_pm2_5_measurement_cluster(esp_zb_cluster_list_t *cluster_list, esp_zb_attribute_list_t *attr_list, uint8_t role_mask);

/**
 * @brief Add multistate value cluster (attribute list) in a cluster list.
 *
 * @param[in] cluster_list A pointer to cluster list @ref esp_zb_cluster_list_s
 * @param[in] attr_list  An attribute list which wants to add
 * @param[in] role_mask  A role of server or client for this cluster (attribute list) refer to esp_zb_zcl_cluster_role_t
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_ARG if cluster list not initialized
 *
 */
esp_err_t esp_zb_cluster_list_add_multistate_value_cluster(esp_zb_cluster_list_t *cluster_list, esp_zb_attribute_list_t *attr_list, uint8_t role_mask);

/**
 * @brief Add metering cluster (attribute list) in a cluster list.
 *
 * @param[in] cluster_list A pointer to cluster list @ref esp_zb_cluster_list_s
 * @param[in] attr_list  An attribute list which wants to add
 * @param[in] role_mask  A role of server or client for this cluster (attribute list) refer to esp_zb_zcl_cluster_role_t
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_ARG if cluster list not initialized
 *
 */
esp_err_t esp_zb_cluster_list_add_metering_cluster(esp_zb_cluster_list_t *cluster_list, esp_zb_attribute_list_t *attr_list, uint8_t role_mask);

/**
 * @brief Add diagnostics cluster (attribute list) in a cluster list.
 *
 * @param[in] cluster_list A pointer to cluster list @ref esp_zb_cluster_list_s
 * @param[in] attr_list  An attribute list which wants to add
 * @param[in] role_mask  A role of server or client for this cluster (attribute list) refer to esp_zb_zcl_cluster_role_t
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_ARG if cluster list not initialized
 *
 */
esp_err_t esp_zb_cluster_list_add_diagnostics_cluster(esp_zb_cluster_list_t *cluster_list, esp_zb_attribute_list_t *attr_list, uint8_t role_mask);

/**
 * @brief Add meter identification cluster (attribute list) in a cluster list.
 *
 * @param[in] cluster_list A pointer to cluster list @ref esp_zb_cluster_list_s
 * @param[in] attr_list  An attribute list which wants to add
 * @param[in] role_mask  A role of server or client for this cluster (attribute list) refer to esp_zb_zcl_cluster_role_t
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_ARG if cluster list not initialized
 *
 */
esp_err_t esp_zb_cluster_list_add_meter_identification_cluster(esp_zb_cluster_list_t *cluster_list, esp_zb_attribute_list_t *attr_list, uint8_t role_mask);

/**
 * @brief Add price cluster (attribute list) in a cluster list.
 *
 * @param[in] cluster_list A pointer to cluster list @ref esp_zb_cluster_list_s
 * @param[in] attr_list  An attribute list which wants to add
 * @param[in] role_mask  A role of server or client for this cluster (attribute list) refer to esp_zb_zcl_cluster_role_t
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_ARG if cluster list not initialized
 *
 */
esp_err_t esp_zb_cluster_list_add_price_cluster(esp_zb_cluster_list_t *cluster_list, esp_zb_attribute_list_t *attr_list, uint8_t role_mask);

/**
 * @brief Add demand response and load control cluster (attribute list) in a cluster list.
 *
 * @param[in] cluster_list A pointer to cluster list @ref esp_zb_cluster_list_s
 * @param[in] attr_list  An attribute list which wants to add
 * @param[in] role_mask  A role of server or client for this cluster (attribute list) refer to esp_zb_zcl_cluster_role_t
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_ARG if cluster list not initialized
 *
 */
esp_err_t esp_zb_cluster_list_add_drlc_cluster(esp_zb_cluster_list_t *cluster_list, esp_zb_attribute_list_t *attr_list, uint8_t role_mask);

/**
 * @brief Add customized cluster (attribute list) in a cluster list.
 *
 * @param[in] cluster_list A pointer to cluster list @ref esp_zb_cluster_list_s
 * @param[in] attr_list An attribute list which wants to add
 * @param[in] role_mask A role of server or client for this cluster (attribute list) refer to esp_zb_zcl_cluster_role_t
 *
 * @note The customized cluster id should not be same as already existing zcl cluster id refer to zb_zcl_common.h
 *
 * @return
 * - ESP_OK on success
 * - ESP_ERR_INVALID_ARG if cluster list not initialized
 *
 */
esp_err_t esp_zb_cluster_list_add_custom_cluster(esp_zb_cluster_list_t *cluster_list, esp_zb_attribute_list_t *attr_list, uint8_t role_mask);

/**
 * @brief Get cluster (attribute list) from a cluster list.
 *
 * @param[in] cluster_list A pointer to cluster list @ref esp_zb_cluster_list_s
 * @param[in] cluster_id The cluster id for attribute list refer to esp_zb_zcl_cluster_id
 * @param[in] role_mask A role of server or client for this cluster (attribute list) refer to esp_zb_zcl_cluster_role_t
 *
 * @return
 *      - pointer to  @ref esp_zb_attribute_list_s, if the cluster is found in the cluster list
 *      - ``NULL`` if cluster is not found
 *
 */
esp_zb_attribute_list_t *esp_zb_cluster_list_get_cluster(const esp_zb_cluster_list_t *cluster_list, uint16_t cluster_id, uint8_t role_mask);

/**************************************** UPDATE CLUSTER ***********************************/

/**
 * @brief Update cluster (attribute list) in a cluster list.
 *
 * @param[in] cluster_list A pointer to cluster list @ref esp_zb_cluster_list_s
 * @param[in] attr_list  An attribute list which wants to update
 * @param[in] cluster_id The cluster id for attribute list refer to esp_zb_zcl_cluster_id
 * @param[in] role_mask  A role of server or client for this cluster (attribute list) refer to esp_zb_zcl_cluster_role_t
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_ARG if cluster list not initialized
 *
 */
esp_err_t esp_zb_cluster_list_update_cluster(esp_zb_cluster_list_t *cluster_list, esp_zb_attribute_list_t *attr_list,
                                             uint16_t cluster_id, uint8_t role_mask);

/**
 * @brief Update basic cluster (attribute list) in a cluster list.
 * @deprecated This will be removed from the SDK, please use ``esp_zb_cluster_list_update_cluster()`` instead.
 *
 * @param[in] cluster_list A pointer to cluster list @ref esp_zb_cluster_list_s
 * @param[in] attr_list  An attribute list which wants to update
 * @param[in] role_mask  A role of server or client for this cluster (attribute list) refer to esp_zb_zcl_cluster_role_t
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_ARG if cluster list not initialized
 *
 */
ESP_ZB_DEPRECATED
esp_err_t esp_zb_cluster_list_update_basic_cluster(esp_zb_cluster_list_t *cluster_list, esp_zb_attribute_list_t *attr_list, uint8_t role_mask);

/**
 * @brief Update power configuration cluster (attribute list) in a cluster list.
 * @deprecated This will be removed from the SDK, please use ``esp_zb_cluster_list_update_cluster()`` instead.
 *
 * @param[in] cluster_list A pointer to cluster list @ref esp_zb_cluster_list_s
 * @param[in] attr_list  An attribute list which wants to update
 * @param[in] role_mask  A role of server or client for this cluster (attribute list) refer to esp_zb_zcl_cluster_role_t
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_ARG if cluster list not initialized
 *
 */
ESP_ZB_DEPRECATED
esp_err_t esp_zb_cluster_list_update_power_config_cluster(esp_zb_cluster_list_t *cluster_list, esp_zb_attribute_list_t *attr_list, uint8_t role_mask);

/**
 * @brief Update identify cluster (attribute list) in a cluster list.
 * @deprecated This will be removed from the SDK, please use ``esp_zb_cluster_list_update_cluster()`` instead.
 *
 * @param[in] cluster_list A pointer to cluster list @ref esp_zb_cluster_list_s
 * @param[in] attr_list  An attribute list which wants to update
 * @param[in] role_mask  A role of server or client for this cluster (attribute list) refer to esp_zb_zcl_cluster_role_t
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_ARG if cluster list not initialized
 *
 */
ESP_ZB_DEPRECATED
esp_err_t esp_zb_cluster_list_update_identify_cluster(esp_zb_cluster_list_t *cluster_list, esp_zb_attribute_list_t *attr_list, uint8_t role_mask);

/**
 * @brief Update groups cluster (attribute list) in a cluster list.
 * @deprecated This will be removed from the SDK, please use ``esp_zb_cluster_list_update_cluster()`` instead.
 *
 * @param[in] cluster_list A pointer to cluster list @ref esp_zb_cluster_list_s
 * @param[in] attr_list  An attribute list which wants to update
 * @param[in] role_mask  A role of server or client for this cluster (attribute list) refer to esp_zb_zcl_cluster_role_t
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_ARG if cluster list not initialized
 *
 */
ESP_ZB_DEPRECATED
esp_err_t esp_zb_cluster_list_update_groups_cluster(esp_zb_cluster_list_t *cluster_list, esp_zb_attribute_list_t *attr_list, uint8_t role_mask);

/**
 * @brief Update scenes cluster (attribute list) in a cluster list.
 * @deprecated This will be removed from the SDK, please use ``esp_zb_cluster_list_update_cluster()`` instead.
 *
 * @param[in] cluster_list A pointer to cluster list @ref esp_zb_cluster_list_s
 * @param[in] attr_list  An attribute list which wants to update
 * @param[in] role_mask  A role of server or client for this cluster (attribute list) refer to esp_zb_zcl_cluster_role_t
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_ARG if cluster list not initialized
 *
 */
ESP_ZB_DEPRECATED
esp_err_t esp_zb_cluster_list_update_scenes_cluster(esp_zb_cluster_list_t *cluster_list, esp_zb_attribute_list_t *attr_list, uint8_t role_mask);

/**
 * @brief Update on_off cluster (attribute list) in a cluster list.
 * @deprecated This will be removed from the SDK, please use ``esp_zb_cluster_list_update_cluster()`` instead.
 *
 * @param[in] cluster_list A pointer to cluster list @ref esp_zb_cluster_list_s
 * @param[in] attr_list  An attribute list which wants to update
 * @param[in] role_mask  A role of server or client for this cluster (attribute list) refer to esp_zb_zcl_cluster_role_t
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_ARG if cluster list not initialized
 *
 */
ESP_ZB_DEPRECATED
esp_err_t esp_zb_cluster_list_update_on_off_cluster(esp_zb_cluster_list_t *cluster_list, esp_zb_attribute_list_t *attr_list, uint8_t role_mask);

/**
 * @brief Update on_off switch config cluster (attribute list) in a cluster list.
 * @deprecated This will be removed from the SDK, please use ``esp_zb_cluster_list_update_cluster()`` instead.
 *
 * @param[in] cluster_list A pointer to cluster list @ref esp_zb_cluster_list_s
 * @param[in] attr_list  An attribute list which wants to update
 * @param[in] role_mask  A role of server or client for this cluster (attribute list) refer to esp_zb_zcl_cluster_role_t
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_ARG if cluster list not initialized
 *
 */
ESP_ZB_DEPRECATED
esp_err_t esp_zb_cluster_list_update_on_off_switch_config_cluster(esp_zb_cluster_list_t *cluster_list, esp_zb_attribute_list_t *attr_list, uint8_t role_mask);

/**
 * @brief Update level cluster (attribute list) in a cluster list.
 * @deprecated This will be removed from the SDK, please use ``esp_zb_cluster_list_update_cluster()`` instead.
 *
 * @param[in] cluster_list A pointer to cluster list @ref esp_zb_cluster_list_s
 * @param[in] attr_list  An attribute list which wants to update
 * @param[in] role_mask  A role of server or client for this cluster (attribute list) refer to esp_zb_zcl_cluster_role_t
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_ARG if cluster list not initialized
 *
 */
ESP_ZB_DEPRECATED
esp_err_t esp_zb_cluster_list_update_level_cluster(esp_zb_cluster_list_t *cluster_list, esp_zb_attribute_list_t *attr_list, uint8_t role_mask);

/**
 * @brief Update color control cluster (attribute list) in a cluster list.
 * @deprecated This will be removed from the SDK, please use ``esp_zb_cluster_list_update_cluster()`` instead.
 *
 * @param[in] cluster_list A pointer to cluster list @ref esp_zb_cluster_list_s
 * @param[in] attr_list  An attribute list which wants to update
 * @param[in] role_mask  A role of server or client for this cluster (attribute list) refer to esp_zb_zcl_cluster_role_t
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_ARG if cluster list not initialized
 *
 */
ESP_ZB_DEPRECATED
esp_err_t esp_zb_cluster_list_update_color_control_cluster(esp_zb_cluster_list_t *cluster_list, esp_zb_attribute_list_t *attr_list, uint8_t role_mask);

/**
 * @brief Update Time cluster (attribute list) in a cluster list.
 * @deprecated This will be removed from the SDK, please use ``esp_zb_cluster_list_update_cluster()`` instead.
 *
 * @param[in] cluster_list A pointer to cluster list @ref esp_zb_cluster_list_s
 * @param[in] attr_list  An attribute list which wants to update
 * @param[in] role_mask  A role of server or client for this cluster (attribute list) refer to esp_zb_zcl_cluster_role_t
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_ARG if cluster list not initialized
 *
 */
ESP_ZB_DEPRECATED
esp_err_t esp_zb_cluster_list_update_time_cluster(esp_zb_cluster_list_t *cluster_list, esp_zb_attribute_list_t *attr_list, uint8_t role_mask);

/**
 * @brief Update Shade Config cluster (attribute list) in a cluster list.
 * @deprecated This will be removed from the SDK, please use ``esp_zb_cluster_list_update_cluster()`` instead.
 *
 * @param[in] cluster_list A pointer to cluster list @ref esp_zb_cluster_list_s
 * @param[in] attr_list  An attribute list which wants to update
 * @param[in] role_mask  A role of server or client for this cluster (attribute list) refer to esp_zb_zcl_cluster_role_t
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_ARG if cluster list not initialized
 *
 */
ESP_ZB_DEPRECATED
esp_err_t esp_zb_cluster_list_update_shade_config_cluster(esp_zb_cluster_list_t *cluster_list, esp_zb_attribute_list_t *attr_list, uint8_t role_mask);

/**
 * @brief Update binary input (basic) cluster (attribute list) in a cluster list.
 * @deprecated This will be removed from the SDK, please use ``esp_zb_cluster_list_update_cluster()`` instead.
 *
 * @param[in] cluster_list A pointer to cluster list @ref esp_zb_cluster_list_s
 * @param[in] attr_list  An attribute list which wants to add
 * @param[in] role_mask  A role of server or client for this cluster (attribute list) refer to esp_zb_zcl_cluster_role_t
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_ARG if cluster list not initialized
 *
 */
ESP_ZB_DEPRECATED
esp_err_t esp_zb_cluster_list_update_binary_input_cluster(esp_zb_cluster_list_t *cluster_list, esp_zb_attribute_list_t *attr_list, uint8_t role_mask);

/**
 * @brief Update Commissioning cluster (attribute list) in a cluster list.
 * @deprecated This will be removed from the SDK, please use ``esp_zb_cluster_list_update_cluster()`` instead.
 *
 * @param[in] cluster_list A pointer to cluster list @ref esp_zb_cluster_list_s
 * @param[in] attr_list  An attribute list which wants to update
 * @param[in] role_mask  A role of server or client for this cluster (attribute list) refer to esp_zb_zcl_cluster_role_t
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_ARG if cluster list not initialized
 *
 */
ESP_ZB_DEPRECATED
esp_err_t esp_zb_cluster_list_update_commissioning_cluster(esp_zb_cluster_list_t *cluster_list, esp_zb_attribute_list_t *attr_list, uint8_t role_mask);

/**
 * @brief Update ias zone cluster (attribute list) in a cluster list.
 * @deprecated This will be removed from the SDK, please use ``esp_zb_cluster_list_update_cluster()`` instead.
 *
 * @param[in] cluster_list A pointer to cluster list @ref esp_zb_cluster_list_s
 * @param[in] attr_list  An attribute list which wants to add
 * @param[in] role_mask  A role of server or client for this cluster (attribute list) refer to esp_zb_zcl_cluster_role_t
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_ARG if cluster list not initialized
 *
 */
ESP_ZB_DEPRECATED
esp_err_t esp_zb_cluster_list_update_ias_zone_cluster(esp_zb_cluster_list_t *cluster_list, esp_zb_attribute_list_t *attr_list, uint8_t role_mask);

/**
 * @brief Update IAS ACE cluster (attribute list) in a cluster list.
 * @deprecated This will be removed from the SDK, please use ``esp_zb_cluster_list_update_cluster()`` instead.
 *
 * @param[in] cluster_list A pointer to cluster list @ref esp_zb_cluster_list_s
 * @param[in] attr_list  An attribute list which wants to update
 * @param[in] role_mask  A role of server or client for this cluster (attribute list) refer to esp_zb_zcl_cluster_role_t
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_ARG if cluster list not initialized
 *
 */
ESP_ZB_DEPRECATED
esp_err_t esp_zb_cluster_list_update_ias_ace_cluster(esp_zb_cluster_list_t *cluster_list, esp_zb_attribute_list_t *attr_list, uint8_t role_mask);

/**
 * @brief Update IAS_WD cluster (attribute list) in a cluster list.
 * @deprecated This will be removed from the SDK, please use ``esp_zb_cluster_list_update_cluster()`` instead.
 *
 * @param[in] cluster_list A pointer to cluster list @ref esp_zb_cluster_list_s
 * @param[in] attr_list  An attribute list which wants to update
 * @param[in] role_mask  A role of server or client for this cluster (attribute list) refer to esp_zb_zcl_cluster_role_t
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_ARG if cluster list not initialized
 *
 */
ESP_ZB_DEPRECATED
esp_err_t esp_zb_cluster_list_update_ias_wd_cluster(esp_zb_cluster_list_t *cluster_list, esp_zb_attribute_list_t *attr_list, uint8_t role_mask);

/**
 * @brief Update Door Lock cluster (attribute list) in a cluster list.
 * @deprecated This will be removed from the SDK, please use ``esp_zb_cluster_list_update_cluster()`` instead.
 *
 * @param[in] cluster_list A pointer to cluster list @ref esp_zb_cluster_list_s
 * @param[in] attr_list  An attribute list which wants to update
 * @param[in] role_mask  A role of server or client for this cluster (attribute list) refer to esp_zb_zcl_cluster_role_t
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_ARG if cluster list not initialized
 *
 */
ESP_ZB_DEPRECATED
esp_err_t esp_zb_cluster_list_update_door_lock_cluster(esp_zb_cluster_list_t *cluster_list, esp_zb_attribute_list_t *attr_list, uint8_t role_mask);

/**
 * @brief Update temperature measurement cluster (attribute list) in a cluster list.
 * @deprecated This will be removed from the SDK, please use ``esp_zb_cluster_list_update_cluster()`` instead.
 *
 * @param[in] cluster_list A pointer to cluster list @ref esp_zb_cluster_list_s
 * @param[in] attr_list  An attribute list which wants to update
 * @param[in] role_mask  A role of server or client for this cluster (attribute list) refer to esp_zb_zcl_cluster_role_t
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_ARG if cluster list not initialized
 *
 */
ESP_ZB_DEPRECATED
esp_err_t esp_zb_cluster_list_update_temperature_meas_cluster(esp_zb_cluster_list_t *cluster_list, esp_zb_attribute_list_t *attr_list, uint8_t role_mask);

/**
 * @brief Update humidity measurement cluster (attribute list) in a cluster list.
 * @deprecated This will be removed from the SDK, please use ``esp_zb_cluster_list_update_cluster()`` instead.
 *
 * @param[in] cluster_list A pointer to cluster list @ref esp_zb_cluster_list_s
 * @param[in] attr_list  An attribute list which wants to update
 * @param[in] role_mask  A role of server or client for this cluster (attribute list) refer to esp_zb_zcl_cluster_role_t
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_ARG if cluster list not initialized
 *
 */
ESP_ZB_DEPRECATED
esp_err_t esp_zb_cluster_list_update_humidity_meas_cluster(esp_zb_cluster_list_t *cluster_list, esp_zb_attribute_list_t *attr_list, uint8_t role_mask);

/**
 * @brief Update illuminance measurement cluster (attribute list) in a cluster list.
 * @deprecated This will be removed from the SDK, please use ``esp_zb_cluster_list_update_cluster()`` instead.
 *
 * @param[in] cluster_list A pointer to cluster list @ref esp_zb_cluster_list_s
 * @param[in] attr_list  An attribute list which wants to update
 * @param[in] role_mask  A role of server or client for this cluster (attribute list) refer to esp_zb_zcl_cluster_role_t
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_ARG if cluster list not initialized
 *
 */
ESP_ZB_DEPRECATED
esp_err_t esp_zb_cluster_list_update_illuminance_meas_cluster(esp_zb_cluster_list_t *cluster_list, esp_zb_attribute_list_t *attr_list, uint8_t role_mask);

/**
 * @brief Update pressure measurement cluster (attribute list) in a cluster list.
 * @deprecated This will be removed from the SDK, please use ``esp_zb_cluster_list_update_cluster()`` instead.
 *
 * @param[in] cluster_list A pointer to cluster list @ref esp_zb_cluster_list_s
 * @param[in] attr_list  An attribute list which wants to update
 * @param[in] role_mask  A role of server or client for this cluster (attribute list) refer to esp_zb_zcl_cluster_role_t
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_ARG if cluster list not initialized
 *
 */
ESP_ZB_DEPRECATED
esp_err_t esp_zb_cluster_list_update_pressure_meas_cluster(esp_zb_cluster_list_t *cluster_list, esp_zb_attribute_list_t *attr_list, uint8_t role_mask);

/**
 * @brief Update flow measurement cluster (attribute list) in a cluster list.
 * @deprecated This will be removed from the SDK, please use ``esp_zb_cluster_list_update_cluster()`` instead.
 *
 * @param[in] cluster_list A pointer to cluster list @ref esp_zb_cluster_list_s
 * @param[in] attr_list  An attribute list which wants to update
 * @param[in] role_mask  A role of server or client for this cluster (attribute list) refer to esp_zb_zcl_cluster_role_t
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_ARG if cluster list not initialized
 *
 */
ESP_ZB_DEPRECATED
esp_err_t esp_zb_cluster_list_update_flow_meas_cluster(esp_zb_cluster_list_t *cluster_list, esp_zb_attribute_list_t *attr_list, uint8_t role_mask);

/**
 * @brief Update electrical measurement cluster (attribute list) in a cluster list.
 * @deprecated This will be removed from the SDK, please use ``esp_zb_cluster_list_update_cluster()`` instead.
 *
 * @param[in] cluster_list A pointer to cluster list @ref esp_zb_cluster_list_s
 * @param[in] attr_list  An attribute list which wants to update
 * @param[in] role_mask  A role of server or client for this cluster (attribute list) refer to esp_zb_zcl_cluster_role_t
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_ARG if cluster list not initialized
 *
 */
ESP_ZB_DEPRECATED
esp_err_t esp_zb_cluster_list_update_electrical_meas_cluster(esp_zb_cluster_list_t *cluster_list, esp_zb_attribute_list_t *attr_list, uint8_t role_mask);

/**
 * @brief Update occupancy sensor cluster (attribute list) in a cluster list.
 * @deprecated This will be removed from the SDK, please use ``esp_zb_cluster_list_update_cluster()`` instead.
 *
 * @param[in] cluster_list A pointer to cluster list @ref esp_zb_cluster_list_s
 * @param[in] attr_list  An attribute list which wants to update
 * @param[in] role_mask  A role of server or client for this cluster (attribute list) refer to esp_zb_zcl_cluster_role_t
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_ARG if cluster list not initialized
 *
 */
ESP_ZB_DEPRECATED
esp_err_t esp_zb_cluster_list_update_occupancy_sensing_cluster(esp_zb_cluster_list_t *cluster_list, esp_zb_attribute_list_t *attr_list, uint8_t role_mask);

/**
 * @brief Update window covering cluster (attribute list) in a cluster list.
 * @deprecated This will be removed from the SDK, please use ``esp_zb_cluster_list_update_cluster()`` instead.
 *
 * @param[in] cluster_list A pointer to cluster list @ref esp_zb_cluster_list_s
 * @param[in] attr_list  An attribute list which wants to update
 * @param[in] role_mask  A role of server or client for this cluster (attribute list) refer to esp_zb_zcl_cluster_role_t
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_ARG if cluster list not initialized
 *
 */
ESP_ZB_DEPRECATED
esp_err_t esp_zb_cluster_list_update_window_covering_cluster(esp_zb_cluster_list_t *cluster_list, esp_zb_attribute_list_t *attr_list, uint8_t role_mask);

/**
 * @brief Update thermostat cluster (attribute list) in a cluster list.
 * @deprecated This will be removed from the SDK, please use ``esp_zb_cluster_list_update_cluster()`` instead.
 *
 * @param[in] cluster_list A pointer to cluster list @ref esp_zb_cluster_list_s
 * @param[in] attr_list  An attribute list which wants to update
 * @param[in] role_mask  A role of server or client for this cluster (attribute list) refer to esp_zb_zcl_cluster_role_t
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_ARG if cluster list not initialized
 *
 */
ESP_ZB_DEPRECATED
esp_err_t esp_zb_cluster_list_update_thermostat_cluster(esp_zb_cluster_list_t *cluster_list, esp_zb_attribute_list_t *attr_list, uint8_t role_mask);

/**
 * @brief Update fan control cluster (attribute list) in a cluster list.
 * @deprecated This will be removed from the SDK, please use ``esp_zb_cluster_list_update_cluster()`` instead.
 *
 * @param[in] cluster_list A pointer to cluster list @ref esp_zb_cluster_list_s
 * @param[in] attr_list  An attribute list which wants to update
 * @param[in] role_mask  A role of server or client for this cluster (attribute list) refer to esp_zb_zcl_cluster_role_t
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_ARG if cluster list not initialized
 *
 */
ESP_ZB_DEPRECATED
esp_err_t esp_zb_cluster_list_update_fan_control_cluster(esp_zb_cluster_list_t *cluster_list, esp_zb_attribute_list_t *attr_list, uint8_t role_mask);

/**
 * @brief Update thermostat ui config cluster (attribute list) in a cluster list.
 * @deprecated This will be removed from the SDK, please use ``esp_zb_cluster_list_update_cluster()`` instead.
 *
 * @param[in] cluster_list A pointer to cluster list @ref esp_zb_cluster_list_s
 * @param[in] attr_list  An attribute list which wants to update
 * @param[in] role_mask  A role of server or client for this cluster (attribute list) refer to esp_zb_zcl_cluster_role_t
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_ARG if cluster list not initialized
 *
 */
ESP_ZB_DEPRECATED
esp_err_t esp_zb_cluster_list_update_thermostat_ui_config_cluster(esp_zb_cluster_list_t *cluster_list, esp_zb_attribute_list_t *attr_list, uint8_t role_mask);

/**
 * @brief Update analog input cluster (attribute list) in a cluster list.
 * @deprecated This will be removed from the SDK, please use ``esp_zb_cluster_list_update_cluster()`` instead.
 *
 * @param[in] cluster_list A pointer to cluster list @ref esp_zb_cluster_list_s
 * @param[in] attr_list  An attribute list which wants to update
 * @param[in] role_mask  A role of server or client for this cluster (attribute list) refer to esp_zb_zcl_cluster_role_t
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_ARG if cluster list not initialized
 *
 */
ESP_ZB_DEPRECATED
esp_err_t esp_zb_cluster_list_update_analog_input_cluster(esp_zb_cluster_list_t *cluster_list, esp_zb_attribute_list_t *attr_list, uint8_t role_mask);

/**
 * @brief Update analog output cluster (attribute list) in a cluster list.
 * @deprecated This will be removed from the SDK, please use ``esp_zb_cluster_list_update_cluster()`` instead.
 *
 * @param[in] cluster_list A pointer to cluster list @ref esp_zb_cluster_list_s
 * @param[in] attr_list  An attribute list which wants to update
 * @param[in] role_mask  A role of server or client for this cluster (attribute list) refer to esp_zb_zcl_cluster_role_t
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_ARG if cluster list not initialized
 *
 */
ESP_ZB_DEPRECATED
esp_err_t esp_zb_cluster_list_update_analog_output_cluster(esp_zb_cluster_list_t *cluster_list, esp_zb_attribute_list_t *attr_list, uint8_t role_mask);

/**
 * @brief Update analog value cluster (attribute list) in a cluster list.
 * @deprecated This will be removed from the SDK, please use ``esp_zb_cluster_list_update_cluster()`` instead.
 *
 * @param[in] cluster_list A pointer to cluster list @ref esp_zb_cluster_list_s
 * @param[in] attr_list  An attribute list which wants to update
 * @param[in] role_mask  A role of server or client for this cluster (attribute list) refer to esp_zb_zcl_cluster_role_t
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_ARG if cluster list not initialized
 *
 */
ESP_ZB_DEPRECATED
esp_err_t esp_zb_cluster_list_update_analog_value_cluster(esp_zb_cluster_list_t *cluster_list, esp_zb_attribute_list_t *attr_list, uint8_t role_mask);

/**
 * @brief Update carbon dioxide measurement cluster (attribute list) in a cluster list.
 * @deprecated This will be removed from the SDK, please use ``esp_zb_cluster_list_update_cluster()`` instead.
 *
 * @param[in] cluster_list A pointer to cluster list @ref esp_zb_cluster_list_s
 * @param[in] attr_list  An attribute list which wants to update
 * @param[in] role_mask  A role of server or client for this cluster (attribute list) refer to esp_zb_zcl_cluster_role_t
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_ARG if cluster list not initialized
 *
 */
ESP_ZB_DEPRECATED
esp_err_t esp_zb_cluster_list_update_carbon_dioxide_measurement_cluster(esp_zb_cluster_list_t *cluster_list, esp_zb_attribute_list_t *attr_list, uint8_t role_mask);

/**
 * @brief Update pm2.5 measurement cluster (attribute list) in a cluster list.
 * @deprecated This will be removed from the SDK, please use ``esp_zb_cluster_list_update_cluster()`` instead.
 *
 * @param[in] cluster_list A pointer to cluster list @ref esp_zb_cluster_list_s
 * @param[in] attr_list  An attribute list which wants to update
 * @param[in] role_mask  A role of server or client for this cluster (attribute list) refer to esp_zb_zcl_cluster_role_t
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_ARG if cluster list not initialized
 *
 */
ESP_ZB_DEPRECATED
esp_err_t esp_zb_cluster_list_update_pm2_5_measurement_cluster(esp_zb_cluster_list_t *cluster_list, esp_zb_attribute_list_t *attr_list, uint8_t role_mask);

/**
 * @brief Update multistate value cluster (attribute list) in a cluster list.
 * @deprecated This will be removed from the SDK, please use ``esp_zb_cluster_list_update_cluster()`` instead.
 *
 * @param[in] cluster_list A pointer to cluster list @ref esp_zb_cluster_list_s
 * @param[in] attr_list  An attribute list which wants to update
 * @param[in] role_mask  A role of server or client for this cluster (attribute list) refer to esp_zb_zcl_cluster_role_t
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_ARG if cluster list not initialized
 *
 */
ESP_ZB_DEPRECATED
esp_err_t esp_zb_cluster_list_update_multistate_value_cluster(esp_zb_cluster_list_t *cluster_list, esp_zb_attribute_list_t *attr_list, uint8_t role_mask);

/**
 * @brief Update metering cluster (attribute list) in a cluster list.
 * @deprecated This will be removed from the SDK, please use ``esp_zb_cluster_list_update_cluster()`` instead.
 *
 * @param[in] cluster_list A pointer to cluster list @ref esp_zb_cluster_list_s
 * @param[in] attr_list  An attribute list which wants to update
 * @param[in] role_mask  A role of server or client for this cluster (attribute list) refer to esp_zb_zcl_cluster_role_t
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_ARG if cluster list not initialized
 *
 */
ESP_ZB_DEPRECATED
esp_err_t esp_zb_cluster_list_update_metering_cluster(esp_zb_cluster_list_t *cluster_list, esp_zb_attribute_list_t *attr_list, uint8_t role_mask);

/**
 * @brief Update diagnostics cluster (attribute list) in a cluster list.
 * @deprecated This will be removed from the SDK, please use ``esp_zb_cluster_list_update_cluster()`` instead.
 *
 * @param[in] cluster_list A pointer to cluster list @ref esp_zb_cluster_list_s
 * @param[in] attr_list  An attribute list which wants to update
 * @param[in] role_mask  A role of server or client for this cluster (attribute list) refer to esp_zb_zcl_cluster_role_t
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_ARG if cluster list not initialized
 *
 */
ESP_ZB_DEPRECATED
esp_err_t esp_zb_cluster_list_update_diagnostics_cluster(esp_zb_cluster_list_t *cluster_list, esp_zb_attribute_list_t *attr_list, uint8_t role_mask);

/**
 * @brief Update meter identification cluster (attribute list) in a cluster list.
 * @deprecated This will be removed from the SDK, please use ``esp_zb_cluster_list_update_cluster()`` instead.
 *
 * @param[in] cluster_list A pointer to cluster list @ref esp_zb_cluster_list_s
 * @param[in] attr_list  An attribute list which wants to update
 * @param[in] role_mask  A role of server or client for this cluster (attribute list) refer to esp_zb_zcl_cluster_role_t
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_ARG if cluster list not initialized
 *
 */
ESP_ZB_DEPRECATED
esp_err_t esp_zb_cluster_list_update_meter_identification_cluster(esp_zb_cluster_list_t *cluster_list, esp_zb_attribute_list_t *attr_list, uint8_t role_mask);

/**
 * @brief Update price cluster (attribute list) in a cluster list.
 * @deprecated This will be removed from the SDK, please use ``esp_zb_cluster_list_update_cluster()`` instead.
 *
 * @param[in] cluster_list A pointer to cluster list @ref esp_zb_cluster_list_s
 * @param[in] attr_list  An attribute list which wants to update
 * @param[in] role_mask  A role of server or client for this cluster (attribute list) refer to esp_zb_zcl_cluster_role_t
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_ARG if cluster list not initialized
 *
 */
ESP_ZB_DEPRECATED
esp_err_t esp_zb_cluster_list_update_price_cluster(esp_zb_cluster_list_t *cluster_list, esp_zb_attribute_list_t *attr_list, uint8_t role_mask);

/**
 * @brief Update OTA cluster (attribute list) in a cluster list.
 * @deprecated This will be removed from the SDK, please use ``esp_zb_cluster_list_update_cluster()`` instead.
 *
 * @param[in] cluster_list A pointer to cluster list @ref esp_zb_cluster_list_s
 * @param[in] attr_list  An attribute list which wants to update
 * @param[in] role_mask  A role of server or client for this cluster (attribute list) refer to esp_zb_zcl_cluster_role_t
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_ARG if cluster list not initialized
 *
 */
ESP_ZB_DEPRECATED
esp_err_t esp_zb_cluster_list_update_ota_cluster(esp_zb_cluster_list_t *cluster_list, esp_zb_attribute_list_t *attr_list, uint8_t role_mask);

/**
 * @brief Update customized cluster (attribute list) in a cluster list.
 * @deprecated This will be removed from the SDK, please use ``esp_zb_cluster_list_update_cluster()`` instead.
 *
 * @param[in] cluster_list A pointer to cluster list @ref esp_zb_cluster_list_s
 * @param[in] attr_list  An attribute list which wants to update
 * @param[in] role_mask  A role of server or client for this cluster (attribute list) refer to esp_zb_zcl_cluster_role_t
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_ARG if cluster list not initialized
 *
 */
ESP_ZB_DEPRECATED
esp_err_t esp_zb_cluster_list_update_custom_cluster(esp_zb_cluster_list_t *cluster_list, esp_zb_attribute_list_t *attr_list, uint8_t role_mask);

#ifdef __cplusplus
}
#endif
