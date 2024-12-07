/*
 * SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

/*
 *  pppd test
*/

#include <cstring>
#include "esp_system.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "esp_netif_ppp.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"

#define CATCH_CONFIG_MAIN
#include "catch2/catch_test_macros.hpp"
#include "catch2/catch_session.hpp"

static const char *TAG = "pppd_test";
static EventGroupHandle_t event_group = NULL;

static void on_modem_event(void *arg, esp_event_base_t event_base,
                           int32_t event_id, void *event_data)
{
    if (event_base == IP_EVENT) {
        ESP_LOGD(TAG, "IP event! %" PRId32, event_id);
        if (event_id == IP_EVENT_PPP_GOT_IP) {
            esp_netif_dns_info_t dns_info;


            ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
            esp_netif_t *netif = event->esp_netif;

            ESP_LOGI(TAG, "Modem Connect to PPP Server");
            ESP_LOGI(TAG, "~~~~~~~~~~~~~~");
            ESP_LOGI(TAG, "IP          : " IPSTR, IP2STR(&event->ip_info.ip));
            ESP_LOGI(TAG, "Netmask     : " IPSTR, IP2STR(&event->ip_info.netmask));
            ESP_LOGI(TAG, "Gateway     : " IPSTR, IP2STR(&event->ip_info.gw));
            esp_netif_get_dns_info(netif, ESP_NETIF_DNS_MAIN, &dns_info);
            ESP_LOGI(TAG, "Name Server1: " IPSTR, IP2STR(&dns_info.ip.u_addr.ip4));
            esp_netif_get_dns_info(netif, ESP_NETIF_DNS_BACKUP, &dns_info);
            ESP_LOGI(TAG, "Name Server2: " IPSTR, IP2STR(&dns_info.ip.u_addr.ip4));
            ESP_LOGI(TAG, "~~~~~~~~~~~~~~");

            ESP_LOGI(TAG, "GOT ip event!!!");
            xEventGroupSetBits(event_group, 1);
        } else if (event_id == IP_EVENT_PPP_LOST_IP) {
            ESP_LOGI(TAG, "Modem Disconnect from PPP Server");
        } else if (event_id == IP_EVENT_GOT_IP6) {
            ESP_LOGI(TAG, "GOT IPv6 event!");
            ip_event_got_ip6_t *event = (ip_event_got_ip6_t *)event_data;
            ESP_LOGI(TAG, "Got IPv6 address " IPV6STR, IPV62STR(event->ip6_info.ip));
        }
    }
}

static void on_ppp_changed(void *arg, esp_event_base_t event_base,
                           int32_t event_id, void *event_data)
{
    ESP_LOGI(TAG, "PPP state changed event %" PRId32, event_id);
    if (event_id == NETIF_PPP_ERRORUSER) {
        /* User interrupted event from esp-netif */
        auto p_netif = static_cast<esp_netif_t **>(event_data);
        ESP_LOGI(TAG, "User interrupted event from netif:%p", *p_netif);
        xEventGroupSetBits(event_group, 2);
    }
}



esp_err_t modem_init_network(esp_netif_t *netif);
void modem_start_network();
void modem_stop_network();

extern "C" void app_main(void)
{

    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    event_group = xEventGroupCreate();

    // init the DTE
    esp_netif_config_t ppp_netif_config = ESP_NETIF_DEFAULT_PPP();
    esp_netif_t *ppp_netif = esp_netif_new(&ppp_netif_config);
    assert(ppp_netif);
    esp_netif_ppp_config_t ppp_config = { true, true };
    esp_netif_ppp_set_params(ppp_netif, &ppp_config);

    ESP_ERROR_CHECK(modem_init_network(ppp_netif));
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, ESP_EVENT_ANY_ID, on_modem_event, nullptr));
    ESP_ERROR_CHECK(esp_event_handler_register(NETIF_PPP_STATUS, ESP_EVENT_ANY_ID, &on_ppp_changed, nullptr));

#if CONFIG_TEST_APP_AUTH
    esp_netif_ppp_set_auth(ppp_netif, NETIF_PPP_AUTHTYPE_CHAP, CONFIG_TEST_APP_AUTH_USERNAME, CONFIG_TEST_APP_AUTH_PASSWORD);
#endif

    modem_start_network();
    Catch::Session session;
    int numFailed = session.run();
    if (numFailed > 0) {
        ESP_LOGE(TAG, "Test FAILED!");
    } else {
        ESP_LOGI(TAG, "Test passed!");
    }

}

TEST_CASE("Connect test", "[esp_modem]")
{
    EventBits_t b = xEventGroupWaitBits(event_group, 1, pdTRUE, pdFALSE, pdMS_TO_TICKS(15000));
    CHECK(b == 1);
}

TEST_CASE("Disconnection test", "[esp_modem]")
{
    modem_stop_network();
    EventBits_t b = xEventGroupWaitBits(event_group, 2, pdTRUE, pdFALSE, pdMS_TO_TICKS(15000));
    CHECK(b == 2);
}


extern "C" {

    static void handle(int nr)
    {
        ESP_LOGE(TAG, "Signal handler %d", nr);
    }

    _sig_func_ptr signal (int nr, _sig_func_ptr)
    {
        return handle;
    }


}
