/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "sys/socket.h" // for INADDR_ANY
#include "esp_netif.h"
#include "esp_log.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "esp_netif_ppp.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"

#include "esp_console.h"
#include "esp_event.h"
#include "esp_bit_defs.h"
#include "argtable3/argtable3.h"
#include "iperf.h"
#include "sdkconfig.h"
#include "network_dce.h"

static const char *TAG = "pppd_test";
static EventGroupHandle_t event_group = NULL;
static esp_netif_t *s_ppp_netif;
static const int GOTIP_BIT = BIT0;

static void on_modem_event(void *arg, esp_event_base_t event_base,
                           int32_t event_id, void *event_data)
{
    if (event_base == IP_EVENT) {
        ESP_LOGD(TAG, "IP event! %" PRIu32, event_id);
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
            xEventGroupSetBits(event_group, GOTIP_BIT);
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
        esp_netif_t *netif = (esp_netif_t *)event_data;
        ESP_LOGI(TAG, "User interrupted event from netif:%p", netif);
        xEventGroupSetBits(event_group, 2);
    }
}

/* "pppd" command */
static struct {
    struct arg_str *control;
    struct arg_end *end;
} pppd_control_args;

static int pppd_cmd_control(int argc, char **argv)
{
    int nerrors = arg_parse(argc, argv, (void **)&pppd_control_args);
    if (nerrors != 0) {
        arg_print_errors(stderr, pppd_control_args.end, argv[0]);
        return 1;
    }

    if (!strncmp(pppd_control_args.control->sval[0], "info", 4)) {
        esp_netif_ip_info_t ip;
        printf("%s:\n", esp_netif_get_desc(s_ppp_netif));
        esp_netif_get_ip_info(s_ppp_netif, &ip);
        printf("  IP: " IPSTR "\r\n", IP2STR(&ip.ip));
        printf("  MASK: " IPSTR "\r\n", IP2STR(&ip.netmask));
        printf("  GW: " IPSTR "\r\n", IP2STR(&ip.gw));
    }
    return 0;
}

/* "iperf" command */

static struct {
    struct arg_str *ip;
    struct arg_lit *server;
    struct arg_lit *udp;
    struct arg_lit *version;
    struct arg_int *port;
    struct arg_int *length;
    struct arg_int *interval;
    struct arg_int *time;
    struct arg_int *bw_limit;
    struct arg_lit *abort;
    struct arg_end *end;
} iperf_args;

static int ppp_cmd_iperf(int argc, char **argv)
{
    int nerrors = arg_parse(argc, argv, (void **)&iperf_args);
    iperf_cfg_t cfg;

    if (nerrors != 0) {
        arg_print_errors(stderr, iperf_args.end, argv[0]);
        return 0;
    }

    memset(&cfg, 0, sizeof(cfg));

    // ethernet iperf only support IPV4 address
    cfg.type = IPERF_IP_TYPE_IPV4;

    /* iperf -a */
    if (iperf_args.abort->count != 0) {
        iperf_stop();
        return 0;
    }

    if (((iperf_args.ip->count == 0) && (iperf_args.server->count == 0)) ||
            ((iperf_args.ip->count != 0) && (iperf_args.server->count != 0))) {
        ESP_LOGE(__func__, "Wrong mode! ESP32 should run in client or server mode");
        return 0;
    }

    /* iperf -s */
    if (iperf_args.ip->count == 0) {
        cfg.flag |= IPERF_FLAG_SERVER;
    }
    /* iperf -c SERVER_ADDRESS */
    else {
        cfg.destination_ip4 = esp_ip4addr_aton(iperf_args.ip->sval[0]);
        cfg.flag |= IPERF_FLAG_CLIENT;
    }

    if (iperf_args.length->count == 0) {
        cfg.len_send_buf = 0;
    } else {
        cfg.len_send_buf = iperf_args.length->ival[0];
    }

    /* wait for ip, could blocked here */
    xEventGroupWaitBits(event_group, GOTIP_BIT, pdFALSE, pdTRUE, portMAX_DELAY);

    cfg.source_ip4 = INADDR_ANY;

    /* iperf -u */
    if (iperf_args.udp->count == 0) {
        cfg.flag |= IPERF_FLAG_TCP;
    } else {
        cfg.flag |= IPERF_FLAG_UDP;
    }

    /* iperf -p */
    if (iperf_args.port->count == 0) {
        cfg.sport = IPERF_DEFAULT_PORT;
        cfg.dport = IPERF_DEFAULT_PORT;
    } else {
        if (cfg.flag & IPERF_FLAG_SERVER) {
            cfg.sport = iperf_args.port->ival[0];
            cfg.dport = IPERF_DEFAULT_PORT;
        } else {
            cfg.sport = IPERF_DEFAULT_PORT;
            cfg.dport = iperf_args.port->ival[0];
        }
    }

    /* iperf -i */
    if (iperf_args.interval->count == 0) {
        cfg.interval = IPERF_DEFAULT_INTERVAL;
    } else {
        cfg.interval = iperf_args.interval->ival[0];
        if (cfg.interval <= 0) {
            cfg.interval = IPERF_DEFAULT_INTERVAL;
        }
    }

    /* iperf -t */
    if (iperf_args.time->count == 0) {
        cfg.time = IPERF_DEFAULT_TIME;
    } else {
        cfg.time = iperf_args.time->ival[0];
        if (cfg.time <= cfg.interval) {
            cfg.time = cfg.interval;
        }
    }

    /* iperf -b */
    if (iperf_args.bw_limit->count == 0) {
        cfg.bw_lim = IPERF_DEFAULT_NO_BW_LIMIT;
    } else {
        cfg.bw_lim = iperf_args.bw_limit->ival[0];
        if (cfg.bw_lim <= 0) {
            cfg.bw_lim = IPERF_DEFAULT_NO_BW_LIMIT;
        }
    }

    printf("mode=%s-%s sip=" IPSTR ":%" PRIu16 ", dip=%" PRIu32 ".%" PRIu32 ".%" PRIu32 ".%" PRIu32 ":%" PRIu16 ", interval=%" PRIu32 ", time=%" PRIu32 "\r\n",
           cfg.flag & IPERF_FLAG_TCP ? "tcp" : "udp",
           cfg.flag & IPERF_FLAG_SERVER ? "server" : "client",
           (uint16_t) cfg.source_ip4 & 0xFF,
           (uint16_t) (cfg.source_ip4 >> 8) & 0xFF,
           (uint16_t) (cfg.source_ip4 >> 16) & 0xFF,
           (uint16_t) (cfg.source_ip4 >> 24) & 0xFF,
           cfg.sport,
           cfg.destination_ip4 & 0xFF, (cfg.destination_ip4 >> 8) & 0xFF,
           (cfg.destination_ip4 >> 16) & 0xFF, (cfg.destination_ip4 >> 24) & 0xFF, cfg.dport,
           cfg.interval, cfg.time);

    iperf_start(&cfg);
    return 0;
}

void register_pppd(void)
{
    event_group = xEventGroupCreate();
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    ESP_ERROR_CHECK(esp_netif_init());

    esp_netif_config_t ppp_netif_config = ESP_NETIF_DEFAULT_PPP();
    s_ppp_netif = esp_netif_new(&ppp_netif_config);
    assert(s_ppp_netif);
    esp_netif_ppp_config_t ppp_config = { true, true };
    esp_netif_ppp_set_params(s_ppp_netif, &ppp_config);

    ESP_ERROR_CHECK(modem_init_network(s_ppp_netif));
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, ESP_EVENT_ANY_ID, on_modem_event, NULL));
    ESP_ERROR_CHECK(esp_event_handler_register(NETIF_PPP_STATUS, ESP_EVENT_ANY_ID, on_ppp_changed, NULL));

    modem_start_network();

    pppd_control_args.control = arg_str1(NULL, NULL, "<info>", "Get info of pppd");
    pppd_control_args.end = arg_end(1);
    const esp_console_cmd_t cmd = {
        .command = "pppd",
        .help = "PPP interface IO control",
        .hint = NULL,
        .func = pppd_cmd_control,
        .argtable = &pppd_control_args
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&cmd));

    iperf_args.ip = arg_str0("c", "client", "<ip>",
                             "run in client mode, connecting to <host>");
    iperf_args.server = arg_lit0("s", "server", "run in server mode");
    iperf_args.udp = arg_lit0("u", "udp", "use UDP rather than TCP");
    iperf_args.version = arg_lit0("V", "ipv6_domain", "use IPV6 address rather than IPV4");
    iperf_args.port = arg_int0("p", "port", "<port>",
                               "server port to listen on/connect to");
    iperf_args.length = arg_int0("l", "len", "<length>", "set read/write buffer size");
    iperf_args.interval = arg_int0("i", "interval", "<interval>",
                                   "seconds between periodic bandwidth reports");
    iperf_args.time = arg_int0("t", "time", "<time>", "time in seconds to transmit for (default 10 secs)");
    iperf_args.bw_limit = arg_int0("b", "bandwidth", "<bandwidth>", "bandwidth to send at in Mbits/sec");
    iperf_args.abort = arg_lit0("a", "abort", "abort running iperf");
    iperf_args.end = arg_end(1);
    const esp_console_cmd_t iperf_cmd = {
        .command = "iperf",
        .help = "iperf command",
        .hint = NULL,
        .func = &ppp_cmd_iperf,
        .argtable = &iperf_args
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&iperf_cmd));
}
