/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#include <cstring>
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "esp_netif.h"
#include "esp_netif_ppp.h"
#include "esp_log.h"
#include "esp_event.h"
#include "cxx_include/esp_modem_dte.hpp"
#include "esp_modem_config.h"
#include "cxx_include/esp_modem_api.hpp"
#include "esp_vfs_dev.h"        // For optional VFS support
#include "vfs_resource/vfs_create.hpp"
#include "network_dce.hpp"
#include "manual_ota.hpp"
#include "mqtt_client.h"

using namespace esp_modem;

static const char *TAG = "ota_test";


// Wrap event handlers to destruct correctly on returning from main
class StatusHandler {
public:
    static constexpr auto IP_Event      = SignalGroup::bit0;
    static constexpr auto MQTT_Connect  = SignalGroup::bit1;
    static constexpr auto MQTT_Data     = SignalGroup::bit2;

    StatusHandler()
    {
        ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, ESP_EVENT_ANY_ID, on_event, this));
    }

    ~StatusHandler()
    {
        esp_event_handler_unregister(IP_EVENT, ESP_EVENT_ANY_ID, on_event);
    }

    void handle_mqtt(esp_mqtt_client_handle_t client)
    {
        mqtt = client;
        esp_mqtt_client_register_event(client, MQTT_EVENT_ANY, on_event, this);
    }

    void remove_mqtt()
    {
#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(5, 1, 0)
        esp_mqtt_client_unregister_event(mqtt, MQTT_EVENT_ANY, on_event);
#endif
        mqtt = nullptr;
    }

    esp_err_t wait_for(decltype(IP_Event) event, int milliseconds)
    {
        return signal.wait_any(event, milliseconds);
    }

    ip_event_t get_ip_event_type()
    {
        return ip_event_type;
    }

private:
    static void on_event(void *arg, esp_event_base_t base, int32_t event, void *data)
    {
        auto *handler = static_cast<StatusHandler *>(arg);
        if (base == IP_EVENT) {
            handler->ip_event(event, data);
        } else {
            handler->mqtt_event(event, data);
        }
    }

    void ip_event(int32_t id, void *data)
    {
        if (id == IP_EVENT_PPP_GOT_IP) {
            auto *event = (ip_event_got_ip_t *)data;
            ESP_LOGI(TAG, "IP          : " IPSTR, IP2STR(&event->ip_info.ip));
            ESP_LOGI(TAG, "Netmask     : " IPSTR, IP2STR(&event->ip_info.netmask));
            ESP_LOGI(TAG, "Gateway     : " IPSTR, IP2STR(&event->ip_info.gw));
            signal.set(IP_Event);
        } else if (id == IP_EVENT_PPP_LOST_IP) {
            signal.set(IP_Event);
        }
        ip_event_type = static_cast<ip_event_t>(id);
    }

    void mqtt_event(int32_t event, void *data)
    {
        if (mqtt && event == MQTT_EVENT_CONNECTED) {
            signal.set(MQTT_Connect);
        } else if (mqtt && event == MQTT_EVENT_DATA) {
            auto event_data = static_cast<esp_mqtt_event_handle_t>(data);
            ESP_LOGI(TAG, " TOPIC: %.*s", event_data->topic_len, event_data->topic);
            ESP_LOGI(TAG, " DATA: %.*s", event_data->data_len, event_data->data);
            signal.set(MQTT_Data);
        }
    }

    esp_modem::SignalGroup signal{};
    esp_mqtt_client_handle_t mqtt{};
    ip_event_t ip_event_type{};
};

// Wrap MQTT operations to destroy everything on returning from main
struct PublishOnce {
    esp_mqtt_client_handle_t mqtt_;
    StatusHandler *events_;

    PublishOnce(StatusHandler *events)
    {
        esp_mqtt_client_config_t config = { };
        config.broker.address.uri = CONFIG_BROKER_URI;
        mqtt_ = esp_mqtt_client_init(&config);
        events_ = events;
        events->handle_mqtt(mqtt_);
    }

    bool Connect()
    {
        return esp_mqtt_client_start(mqtt_) == ESP_OK;
    }

    bool SubscribePublish()
    {
        return  esp_mqtt_client_subscribe(mqtt_, "/topic/esp-modem", 0) >= 0 &&
                esp_mqtt_client_publish(mqtt_, "/topic/esp-modem", "Hello modem", 0, 0, 0) >= 0;
    }

    ~PublishOnce()
    {
        events_->remove_mqtt();
        esp_mqtt_client_destroy(mqtt_);
    }
};


// OTA related
static constexpr auto OTA_OK      = SignalGroup::bit0;
static constexpr auto OTA_FAILED  = SignalGroup::bit1;

void ota_task(void *ctx)
{
    static const char *ca_cert_pem = "-----BEGIN CERTIFICATE-----\n" CONFIG_TEST_OTA_CA_CERT "\n-----END CERTIFICATE-----";
    auto ota_done = static_cast<esp_modem::SignalGroup *>(ctx);
    manual_ota ota;
    ota.http_.config_.url = CONFIG_TEST_OTA_URI;
    ota.http_.config_.cert_pem = ca_cert_pem;
    ota.size_ = 32;
    ota.common_name_ = CONFIG_TEST_OTA_CN;
#ifndef CONFIG_ESP_HTTP_CLIENT_ENABLE_CUSTOM_TRANSPORT
    // will have to use NORMAL mode, before custom transport is supported in IDF
    ota.mode_ = manual_ota::mode::NORMAL;
#endif

    ota.begin();
    while (true) {
        if (!ota.perform()) {
            break;
        }
    }
    auto ret = ota.end();
    ota_done->set(ret ? OTA_OK : OTA_FAILED);
    vTaskDelete(nullptr);
}


// App related
extern "C" void app_main(void)
{
    esp_log_level_set("*", ESP_LOG_INFO);
    esp_log_level_set("ota_test", ESP_LOG_DEBUG);

    // Initialize system functions
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    ESP_ERROR_CHECK(esp_netif_init());

    // Initialize DTE
    esp_modem_dte_config_t dte_config = ESP_MODEM_DTE_DEFAULT_CONFIG();
#ifdef CONFIG_TEST_USE_VFS_TERM
    // To code-cover the vfs layers
    struct esp_modem_vfs_uart_creator uart_config = ESP_MODEM_VFS_DEFAULT_UART_CONFIG("/dev/uart/1");
    assert(vfs_create_uart(&uart_config, &dte_config.vfs_config) == true);

    auto dte = create_vfs_dte(&dte_config);
    esp_vfs_dev_uart_use_driver(uart_config.uart.port_num);
#else
    auto dte = create_uart_dte(&dte_config);
#endif // CONFIG_TEST_USE_VFS_TERM
    assert(dte);
    dte->set_error_cb([](terminal_error err) {
        ESP_LOGE(TAG, "DTE reported terminal error: %d", static_cast<int>(err));
    });

    // Initialize PPP netif
    esp_netif_config_t netif_ppp_config = ESP_NETIF_DEFAULT_PPP();
    esp_netif_t *esp_netif = esp_netif_new(&netif_ppp_config);
    assert(esp_netif);

    // Initialize DCE
#ifdef CONFIG_TEST_DEVICE_PPPD_SERVER
    auto dce = create(dte, esp_netif);
#else
    esp_modem_dce_config_t dce_config = ESP_MODEM_DCE_DEFAULT_CONFIG(CONFIG_TEST_MODEM_APN);
    auto dce = create_generic_dce(&dce_config, dte, esp_netif);
    assert(dce);
#endif

    StatusHandler handler;

#ifndef CONFIG_TEST_DEVICE_PPPD_SERVER
    if (dce->set_mode(esp_modem::modem_mode::CMUX_MANUAL_MODE) &&
            dce->set_mode(esp_modem::modem_mode::CMUX_MANUAL_SWAP) &&
            dce->set_mode(esp_modem::modem_mode::CMUX_MANUAL_DATA)) {
#else
    if (dce->set_mode(esp_modem::modem_mode::DATA_MODE)) {
#endif
        ESP_LOGI(TAG, "Modem has correctly entered the desired mode (CMUX/DATA/Manual CMUX)");
    } else {
        ESP_LOGE(TAG, "Failed to configure multiplexed command mode... exiting");
        return;
    }

    if (!handler.wait_for(StatusHandler::IP_Event, 60000)) {
        ESP_LOGE(TAG, "Cannot get IP within specified timeout... exiting");
        return;
    } else if (handler.get_ip_event_type() == IP_EVENT_PPP_GOT_IP) {
        ESP_LOGI(TAG, "Got IP address");

        /* When connected to network, subscribe and publish some MQTT data */
        PublishOnce publish(&handler);
        if (!publish.Connect()) {
            ESP_LOGE(TAG, "Failed to connect to mqtt server");
            return;
        }
        if (!handler.wait_for(StatusHandler::MQTT_Connect, 60000)) {
            ESP_LOGE(TAG, "Cannot connect to %s within specified timeout... exiting", CONFIG_BROKER_URI);
            return;
        }
        ESP_LOGI(TAG, "Connected");

        if (!publish.SubscribePublish()) {
            ESP_LOGE(TAG, "Failed to subscribe and publish to mqtt server");
            return;
        }

        if (!handler.wait_for(StatusHandler::MQTT_Data, 60000)) {
            ESP_LOGE(TAG, "Didn't receive published data within specified timeout... exiting");
            return;
        }
        ESP_LOGI(TAG, "Received MQTT data");

    } else if (handler.get_ip_event_type() == IP_EVENT_PPP_LOST_IP) {
        ESP_LOGE(TAG, "PPP client has lost connection... exiting");
        return;
    }


    esp_modem::SignalGroup ota_done{};
#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(5, 2, 0)
    // now stop the LCP keepalive before performing OTA
    esp_netif_ppp_config_t cfg;
    ESP_ERROR_CHECK(esp_netif_ppp_get_params(esp_netif, &cfg));
    cfg.ppp_lcp_echo_disabled = true;
    ESP_ERROR_CHECK(esp_netif_ppp_set_params(esp_netif, &cfg));
#endif

    // Run the OTA in a separate task to keep sending commands to the modem at the same time
    xTaskCreate(ota_task, "ota_task", 8192, &ota_done, 5, nullptr);

#ifndef CONFIG_TEST_DEVICE_PPPD_SERVER
    while (true) {
        std::string str;
        if (dce->get_imsi(str) == esp_modem::command_result::OK) {
            ESP_LOGI(TAG, "Modem IMSI number: %s", str.c_str());
        }
        if (ota_done.wait_any(OTA_OK | OTA_FAILED, 100)) {
            break;
        }
    }
#else
    ota_done.wait_any(OTA_OK | OTA_FAILED, portMAX_DELAY);
#endif // CONFIG_TEST_DEVICE_PPPD_SERVER

#ifndef CONFIG_TEST_DEVICE_PPPD_SERVER
    if (dce->set_mode(esp_modem::modem_mode::CMUX_MANUAL_EXIT)) {
#else
    if (dce->set_mode(esp_modem::modem_mode::COMMAND_MODE)) {
#endif
        ESP_LOGI(TAG, "Modem CMUX/DATA mode exit");
    } else {
        ESP_LOGE(TAG, "Failed to configure desired mode... exiting");
        return;
    }

    if (ota_done.is_any(OTA_OK)) {
        ESP_LOGI(TAG, "Prepare to restart system!");
        esp_restart();
    }

}
