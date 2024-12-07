/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

/*
 * PPPoS Client Example
*/
#pragma once

#include <string>
#include <memory>

struct MqttClientHandle;

/**
 * @brief Simple MQTT client wrapper
 */
class MqttClient {
public:
    enum class Event {
        CONNECT,
        DATA,
    };

    explicit MqttClient(const std::string &uri);
    ~MqttClient();

    /**
     * @brief Start the mqtt-client
     */
    void connect();

    /**
     * @brief Publish to topic
     * @param topic Topic to publish
     * @param data Data to publish
     * @param qos QoS (0 by default)
     * @return message id
     */
    int publish(const std::string &topic, const std::string &data, int qos = 0);

    /**
     * @brief Subscribe to a topic
     * @param topic Topic to subscribe
     * @param qos QoS (0 by default)
     * @return message id
     */
    int subscribe(const std::string &topic, int qos = 0);

    /**
     * @brief Get topic from event data
     * @return String topic
     */
    std::string get_topic(void *);

    /**
     * @brief Get published data from event
     * @return String representation of the data
     */
    std::string get_data(void *);

    /**
     * @brief Register MQTT event
     * @param id Event id
     * @param event_handler Event handler
     * @param event_handler_arg Event handler parameters
     */
    void register_handler(int32_t id, esp_event_handler_t event_handler, void *event_handler_arg);

    /**
     * @brief Convert internal MQTT event to standard ESPEvent
     * @param ev internal mqtt event
     * @return corresponding esp_event id
     */
    static int32_t get_event(Event ev);

private:
    std::unique_ptr<MqttClientHandle> h;
};
