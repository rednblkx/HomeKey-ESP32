/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include <esp_event.h>
#include <esp_wifi.h>
#include <esp_netif_ip_addr.h>
#include <esp_diagnostics_variables.h>

#define TAG_IP             "ip"
#define TAG_WIFI           "wifi"

/* Wifi keys */
#define KEY_SSID           "ssid"
#define KEY_BSSID          "bssid"
#define KEY_CHANNEL        "channel"
#define KEY_AUTHMODE       "auth"
#define KEY_REASON         "reason"
#define KEY_DISC_CNT       "disconn_cnt"

/* Optional WiFi var keys */
#define KEY_PROTOCOL       "protocol"
#define KEY_BANDWIDTH      "bandwidth"
#define KEY_PROTOCOL_AP    "protocol_ap"
#define KEY_BANDWIDTH_AP   "bandwidth_ap"
#define KEY_POWER_SAVE     "power_save"
#define KEY_SECOND_CH      "second_ch"

/* IP keys */
#define KEY_IPv4           "ipv4"
#define KEY_NETMASK        "netmask"
#define KEY_GATEWAY        "gw"

#define PATH_WIFI_STATION  "Wi-Fi.Station"
#define PATH_IP_STATION    "IP.Station"
#define PATH_WIFI_AP       "Wi-Fi.AP"

typedef struct {
    wifi_event_sta_connected_t prev_sta_data;
    bool wifi_connected;
    int32_t disconn_cnt;
    bool init;
} priv_data_t;

static priv_data_t s_priv_data;

static bool bssid_matched(uint8_t *bssid1, uint8_t *bssid2)
{
    uint8_t i;
    for (i = 0; i < 6; i++) {
        if (bssid1[i] != bssid2[i]) {
            return false;
        }
    }
    return true;
}

#if CONFIG_DIAG_MORE_NETWORK_VARS
static void diag_add_more_wifi_vars()
{
    uint8_t wifi_protocol = 0;
    wifi_bandwidth_t wifi_bw = WIFI_BW_HT20;
    wifi_ps_type_t wifi_ps_type = WIFI_PS_NONE;
    esp_wifi_get_protocol(WIFI_IF_STA, &wifi_protocol);
    esp_wifi_get_bandwidth(WIFI_IF_STA, &wifi_bw);
    esp_wifi_get_ps(&wifi_ps_type);
    esp_diag_variable_add_uint(KEY_PROTOCOL, wifi_protocol);
    esp_diag_variable_add_uint(KEY_BANDWIDTH, wifi_bw);
    esp_diag_variable_add_uint(KEY_POWER_SAVE, wifi_ps_type);
    if (wifi_bw == WIFI_BW_HT40) {
        // not very useful for now, but will be useful for 5GHz band
        uint8_t primary_ch = 0;
        wifi_second_chan_t secondary_ch = WIFI_SECOND_CHAN_NONE;
        esp_wifi_get_channel(&primary_ch, &secondary_ch);
        esp_diag_variable_add_uint(KEY_SECOND_CH, secondary_ch);
    }
    esp_wifi_get_protocol(WIFI_IF_AP, &wifi_protocol);
    esp_wifi_get_bandwidth(WIFI_IF_AP, &wifi_bw);
    esp_diag_variable_add_uint(KEY_PROTOCOL_AP, wifi_protocol);
    esp_diag_variable_add_uint(KEY_BANDWIDTH_AP, wifi_bw);
}
#endif

static void diag_register_wifi_vars()
{
    esp_diag_variable_register(TAG_WIFI, KEY_SSID, "SSID", PATH_WIFI_STATION, ESP_DIAG_DATA_TYPE_STR);
    esp_diag_variable_register(TAG_WIFI, KEY_BSSID, "BSSID", PATH_WIFI_STATION, ESP_DIAG_DATA_TYPE_MAC);
    esp_diag_variable_register(TAG_WIFI, KEY_CHANNEL, "Channel", PATH_WIFI_STATION, ESP_DIAG_DATA_TYPE_INT);
    esp_diag_variable_register(TAG_WIFI, KEY_AUTHMODE, "Auth Mode", PATH_WIFI_STATION, ESP_DIAG_DATA_TYPE_UINT);
    esp_diag_variable_register(TAG_WIFI, KEY_DISC_CNT, "Disconnect count since last reboot", PATH_WIFI_STATION, ESP_DIAG_DATA_TYPE_INT);
    esp_diag_variable_register(TAG_WIFI, KEY_REASON, "Last Wi-Fi disconnect reason", PATH_WIFI_STATION, ESP_DIAG_DATA_TYPE_INT);
}

#if CONFIG_DIAG_MORE_NETWORK_VARS
static void diag_register_more_wifi_vars()
{
    esp_diag_variable_register(TAG_WIFI, KEY_PROTOCOL, "Protocol", PATH_WIFI_STATION, ESP_DIAG_DATA_TYPE_UINT);
    esp_diag_variable_register(TAG_WIFI, KEY_BANDWIDTH, "Bandwidth", PATH_WIFI_STATION, ESP_DIAG_DATA_TYPE_UINT);
    esp_diag_variable_register(TAG_WIFI, KEY_POWER_SAVE, "Power Save", PATH_WIFI_STATION, ESP_DIAG_DATA_TYPE_UINT);
    esp_diag_variable_register(TAG_WIFI, KEY_SECOND_CH, "Secondary Channel", PATH_WIFI_STATION, ESP_DIAG_DATA_TYPE_UINT);

    esp_diag_variable_register(TAG_WIFI, KEY_PROTOCOL_AP, "Protocol", PATH_WIFI_AP, ESP_DIAG_DATA_TYPE_UINT);
    esp_diag_variable_register(TAG_WIFI, KEY_BANDWIDTH_AP, "Bandwidth", PATH_WIFI_AP, ESP_DIAG_DATA_TYPE_UINT);
}
#endif

static void evt_handler(void *arg, esp_event_base_t evt_base, int32_t evt_id, void *evt_data)
{
    if (evt_base == WIFI_EVENT) {
        switch (evt_id) {
            case WIFI_EVENT_STA_CONNECTED:
            {
                s_priv_data.wifi_connected = true;
                wifi_event_sta_connected_t *data = evt_data;

                if (strncmp((char *)s_priv_data.prev_sta_data.ssid, (char *)data->ssid, data->ssid_len) != 0) {
                    esp_diag_variable_add_str(KEY_SSID, (char *)data->ssid);
                }
                if (!bssid_matched(s_priv_data.prev_sta_data.bssid, data->bssid)) {
                    esp_diag_variable_add_mac(KEY_BSSID, data->bssid);
                }
                if (s_priv_data.prev_sta_data.channel != data->channel) {
                    esp_diag_variable_add_int(KEY_CHANNEL, data->channel);
                }
                if (s_priv_data.prev_sta_data.authmode != data->authmode) {
                    esp_diag_variable_add_uint(KEY_AUTHMODE, data->authmode);
                }
                memcpy(&s_priv_data.prev_sta_data, data, sizeof(s_priv_data.prev_sta_data));
                break;
            }
            case WIFI_EVENT_STA_DISCONNECTED:
            {
                if (s_priv_data.wifi_connected) {
                    s_priv_data.wifi_connected = false;
                    wifi_event_sta_disconnected_t *data = evt_data;
                    s_priv_data.disconn_cnt++;
                    esp_diag_variable_add_int(KEY_REASON, data->reason);
                    esp_diag_variable_add_int(KEY_DISC_CNT, s_priv_data.disconn_cnt);
                }
                break;
            }
            case WIFI_EVENT_STA_AUTHMODE_CHANGE:
            {
                wifi_event_sta_authmode_change_t *data = evt_data;
                esp_diag_variable_add_uint(KEY_AUTHMODE, data->new_mode);
                s_priv_data.prev_sta_data.authmode = data->new_mode;
                break;
            }
            default:
                break;
        }
    } else if (evt_base == IP_EVENT) {
        switch(evt_id) {
            case IP_EVENT_STA_GOT_IP:
            {
                ip_event_got_ip_t *data = evt_data;
                esp_diag_variable_add_ipv4(KEY_IPv4, data->ip_info.ip.addr);
                esp_diag_variable_add_ipv4(KEY_NETMASK, data->ip_info.netmask.addr);
                esp_diag_variable_add_ipv4(KEY_GATEWAY, data->ip_info.gw.addr);
#if CONFIG_DIAG_MORE_NETWORK_VARS
                diag_add_more_wifi_vars();
#endif
                break;
            }
            case IP_EVENT_STA_LOST_IP:
            {
                uint32_t ip = 0x0;
                esp_diag_variable_add_ipv4(KEY_IPv4, ip);
                break;
            }
            default:
                break;
        }
    }
}

esp_err_t esp_diag_network_variables_init(void)
{
    wifi_ap_record_t ap_info;
    esp_netif_ip_info_t ip_info;

    if (s_priv_data.init) {
        return ESP_ERR_INVALID_STATE;
    }
    /* Register the event handler for wifi events */
    esp_err_t err = esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, evt_handler, NULL);
    if (err != ESP_OK) {
        return err;
    }
    /* Register the event handler for ip events */
    err = esp_event_handler_register(IP_EVENT, ESP_EVENT_ANY_ID, evt_handler, NULL);
    if (err != ESP_OK) {
        return err;
    }

    /* wifi variables */
    diag_register_wifi_vars();
#if CONFIG_DIAG_MORE_NETWORK_VARS
    diag_register_more_wifi_vars();
#endif

    /* IP address variables */
    esp_diag_variable_register(TAG_IP, KEY_IPv4, "IPv4", PATH_IP_STATION, ESP_DIAG_DATA_TYPE_IPv4);
    esp_diag_variable_register(TAG_IP, KEY_NETMASK, "Netmask", PATH_IP_STATION, ESP_DIAG_DATA_TYPE_IPv4);
    esp_diag_variable_register(TAG_IP, KEY_GATEWAY, "Gateway", PATH_IP_STATION, ESP_DIAG_DATA_TYPE_IPv4);

    memset(&s_priv_data.prev_sta_data, 0, sizeof(s_priv_data.prev_sta_data));
    /* If wifi is not connected then wifi details are recorded in event handler */
    if (esp_wifi_sta_get_ap_info(&ap_info) == ESP_OK) {
        /* If wifi is connected then record the wifi details */
        memcpy(s_priv_data.prev_sta_data.ssid, ap_info.ssid, sizeof(s_priv_data.prev_sta_data.ssid));
        memcpy(s_priv_data.prev_sta_data.bssid, ap_info.bssid, sizeof(s_priv_data.prev_sta_data.bssid));
        s_priv_data.prev_sta_data.channel = ap_info.primary;
        s_priv_data.prev_sta_data.authmode = ap_info.authmode;
        s_priv_data.wifi_connected = true;

        esp_diag_variable_add_str(KEY_SSID, (char *)ap_info.ssid);
        esp_diag_variable_add_mac(KEY_BSSID, ap_info.bssid);
        esp_diag_variable_add_int(KEY_CHANNEL, ap_info.primary);
        esp_diag_variable_add_uint(KEY_AUTHMODE, ap_info.authmode);
    }

    memset(&ip_info, 0, sizeof(ip_info));
    /* If wifi interface is up and running then record the details */
    if (esp_netif_is_netif_up(esp_netif_get_handle_from_ifkey("WIFI_STA_DEF"))
        && esp_netif_get_ip_info(esp_netif_get_handle_from_ifkey("WIFI_STA_DEF"), &ip_info) == ESP_OK) {
        esp_diag_variable_add_ipv4(KEY_IPv4, ip_info.ip.addr);
        esp_diag_variable_add_ipv4(KEY_NETMASK, ip_info.netmask.addr);
        esp_diag_variable_add_ipv4(KEY_GATEWAY, ip_info.gw.addr);
    }
    esp_diag_variable_add_int(KEY_DISC_CNT, s_priv_data.disconn_cnt);
    s_priv_data.init = true;
    return ESP_OK;
}

esp_err_t esp_diag_network_variables_deinit(void)
{
    if (!s_priv_data.init) {
        return ESP_ERR_INVALID_STATE;
    }
    esp_event_handler_unregister(WIFI_EVENT, ESP_EVENT_ANY_ID, evt_handler);
    esp_event_handler_unregister(IP_EVENT, ESP_EVENT_ANY_ID, evt_handler);
    esp_diag_variable_unregister(KEY_SSID);
    esp_diag_variable_unregister(KEY_BSSID);
    esp_diag_variable_unregister(KEY_CHANNEL);
    esp_diag_variable_unregister(KEY_AUTHMODE);
    esp_diag_variable_unregister(KEY_REASON);
    esp_diag_variable_unregister(KEY_DISC_CNT);
    esp_diag_variable_unregister(KEY_IPv4);
    esp_diag_variable_unregister(KEY_NETMASK);
    esp_diag_variable_unregister(KEY_GATEWAY);
#if CONFIG_DIAG_MORE_NETWORK_VARS
    esp_diag_variable_unregister(KEY_PROTOCOL);
    esp_diag_variable_unregister(KEY_BANDWIDTH);
    esp_diag_variable_unregister(KEY_POWER_SAVE);
    esp_diag_variable_unregister(KEY_SECOND_CH);
    esp_diag_variable_unregister(KEY_PROTOCOL_AP);
    esp_diag_variable_unregister(KEY_BANDWIDTH_AP);
#endif
    memset(&s_priv_data, 0, sizeof(s_priv_data));
    return ESP_OK;
}
