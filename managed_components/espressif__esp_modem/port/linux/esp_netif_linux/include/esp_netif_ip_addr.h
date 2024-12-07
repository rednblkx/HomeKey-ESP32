// Copyright 2015-2019 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

#include <endian.h>

#ifdef __cplusplus
extern "C" {
#endif

#if BYTE_ORDER == BIG_ENDIAN
#define esp_netif_htonl(x) ((uint32_t)(x))
#else
#define esp_netif_htonl(x) ((((x) & (uint32_t)0x000000ffUL) << 24) | \
                      (((x) & (uint32_t)0x0000ff00UL) <<  8) | \
                      (((x) & (uint32_t)0x00ff0000UL) >>  8) | \
                      (((x) & (uint32_t)0xff000000UL) >> 24))
#endif

#define esp_netif_ip4_makeu32(a,b,c,d) (((uint32_t)((a) & 0xff) << 24) | \
                               ((uint32_t)((b) & 0xff) << 16) | \
                               ((uint32_t)((c) & 0xff) << 8)  | \
                                (uint32_t)((d) & 0xff))

// Access address in 16-bit block
#define ESP_IP6_ADDR_BLOCK1(ip6addr) ((uint16_t)((esp_netif_htonl((ip6addr)->addr[0]) >> 16) & 0xffff))
#define ESP_IP6_ADDR_BLOCK2(ip6addr) ((uint16_t)((esp_netif_htonl((ip6addr)->addr[0])) & 0xffff))
#define ESP_IP6_ADDR_BLOCK3(ip6addr) ((uint16_t)((esp_netif_htonl((ip6addr)->addr[1]) >> 16) & 0xffff))
#define ESP_IP6_ADDR_BLOCK4(ip6addr) ((uint16_t)((esp_netif_htonl((ip6addr)->addr[1])) & 0xffff))
#define ESP_IP6_ADDR_BLOCK5(ip6addr) ((uint16_t)((esp_netif_htonl((ip6addr)->addr[2]) >> 16) & 0xffff))
#define ESP_IP6_ADDR_BLOCK6(ip6addr) ((uint16_t)((esp_netif_htonl((ip6addr)->addr[2])) & 0xffff))
#define ESP_IP6_ADDR_BLOCK7(ip6addr) ((uint16_t)((esp_netif_htonl((ip6addr)->addr[3]) >> 16) & 0xffff))
#define ESP_IP6_ADDR_BLOCK8(ip6addr) ((uint16_t)((esp_netif_htonl((ip6addr)->addr[3])) & 0xffff))

#define IPSTR "%d.%d.%d.%d"
#define esp_ip4_addr_get_byte(ipaddr, idx) (((const uint8_t*)(&(ipaddr)->addr))[idx])
#define esp_ip4_addr1(ipaddr) esp_ip4_addr_get_byte(ipaddr, 0)
#define esp_ip4_addr2(ipaddr) esp_ip4_addr_get_byte(ipaddr, 1)
#define esp_ip4_addr3(ipaddr) esp_ip4_addr_get_byte(ipaddr, 2)
#define esp_ip4_addr4(ipaddr) esp_ip4_addr_get_byte(ipaddr, 3)


#define esp_ip4_addr1_16(ipaddr) ((uint16_t)esp_ip4_addr1(ipaddr))
#define esp_ip4_addr2_16(ipaddr) ((uint16_t)esp_ip4_addr2(ipaddr))
#define esp_ip4_addr3_16(ipaddr) ((uint16_t)esp_ip4_addr3(ipaddr))
#define esp_ip4_addr4_16(ipaddr) ((uint16_t)esp_ip4_addr4(ipaddr))

#define IP2STR(ipaddr) esp_ip4_addr1_16(ipaddr), \
    esp_ip4_addr2_16(ipaddr), \
    esp_ip4_addr3_16(ipaddr), \
    esp_ip4_addr4_16(ipaddr)

#define IPV6STR "%04x:%04x:%04x:%04x:%04x:%04x:%04x:%04x"

#define IPV62STR(ipaddr) ESP_IP6_ADDR_BLOCK1(&(ipaddr)),     \
    ESP_IP6_ADDR_BLOCK2(&(ipaddr)),     \
    ESP_IP6_ADDR_BLOCK3(&(ipaddr)),     \
    ESP_IP6_ADDR_BLOCK4(&(ipaddr)),     \
    ESP_IP6_ADDR_BLOCK5(&(ipaddr)),     \
    ESP_IP6_ADDR_BLOCK6(&(ipaddr)),     \
    ESP_IP6_ADDR_BLOCK7(&(ipaddr)),     \
    ESP_IP6_ADDR_BLOCK8(&(ipaddr))

#define ESP_IPADDR_TYPE_V4                0U
#define ESP_IPADDR_TYPE_V6                6U
#define ESP_IPADDR_TYPE_ANY               46U

#define ESP_IP4TOUINT32(a,b,c,d) (((uint32_t)((a) & 0xffU) << 24) | \
                               ((uint32_t)((b) & 0xffU) << 16) | \
                               ((uint32_t)((c) & 0xffU) << 8)  | \
                                (uint32_t)((d) & 0xffU))

#define ESP_IP4TOADDR(a,b,c,d) esp_netif_htonl(ESP_IP4TOUINT32(a, b, c, d))

// new definitions
#define ESP_IPADDR4_INIT(ipaddr, a,b,c,d)  (ipaddr)->addr = ESP_IP4TOADDR(a,b,c,d)
#define ESP_IP6TOADDR(a, b, c, d)      { { { { a, b, c, d } , 0 } }, ESP_IPADDR_TYPE_V6 }

// TODO: use esp-netif instead of lwip API
#define ip_2_ip6(ipaddr)   (&((ipaddr)->u_addr.ip6))
#define ip_2_ip4(ipaddr)   (&((ipaddr)->u_addr.ip4))
#define IP_SET_TYPE_VAL(ipaddr, iptype) do { (ipaddr).type = (iptype); }while(0)
#define IP_GET_TYPE(ipaddr)           ((ipaddr)->type)

#define IP6_NO_ZONE 0
#define ip6_addr_clear_zone(ip6addr) ((ip6addr)->zone = IP6_NO_ZONE)

#define inet6_addr_from_ip6addr(target_in6addr, source_ip6addr) {(target_in6addr)->s6_addr32[0] = (source_ip6addr)->addr[0]; \
                                                                 (target_in6addr)->s6_addr32[1] = (source_ip6addr)->addr[1]; \
                                                                 (target_in6addr)->s6_addr32[2] = (source_ip6addr)->addr[2]; \
                                                                 (target_in6addr)->s6_addr32[3] = (source_ip6addr)->addr[3];}

#define inet6_addr_to_ip6addr(target_ip6addr, source_in6addr)   {(target_ip6addr)->addr[0] = (source_in6addr)->s6_addr32[0]; \
                                                                 (target_ip6addr)->addr[1] = (source_in6addr)->s6_addr32[1]; \
                                                                 (target_ip6addr)->addr[2] = (source_in6addr)->s6_addr32[2]; \
                                                                 (target_ip6addr)->addr[3] = (source_in6addr)->s6_addr32[3]; \
                                                                 ip6_addr_clear_zone(target_ip6addr);}


struct esp_ip6_addr {
    uint32_t addr[4];
    uint8_t zone;
};



struct esp_ip4_addr {
    uint32_t addr;
};

typedef struct esp_ip4_addr esp_ip4_addr_t;

typedef struct esp_ip6_addr esp_ip6_addr_t;

typedef struct _ip_addr {
    union {
        esp_ip6_addr_t ip6;
        esp_ip4_addr_t ip4;
    } u_addr;
    uint8_t type;
} esp_ip_addr_t;

typedef enum {
    ESP_IP6_ADDR_IS_UNKNOWN,
    ESP_IP6_ADDR_IS_GLOBAL,
    ESP_IP6_ADDR_IS_LINK_LOCAL,
    ESP_IP6_ADDR_IS_SITE_LOCAL,
    ESP_IP6_ADDR_IS_UNIQUE_LOCAL,
    ESP_IP6_ADDR_IS_IPV4_MAPPED_IPV6
} esp_ip6_addr_type_t;

typedef struct {
    esp_ip4_addr_t ip;      /**< Interface IPV4 address */
    esp_ip4_addr_t netmask; /**< Interface IPV4 netmask */
    esp_ip4_addr_t gw;      /**< Interface IPV4 gateway address */
} esp_netif_ip_info_t;

typedef struct {
    esp_ip6_addr_t ip; /**< Interface IPV6 address */
} esp_netif_ip6_info_t;

/**
 * @brief  Get the IPv6 address type
 *
 * @param[in]  ip6_addr IPv6 type
 *
 * @return IPv6 type in form of enum esp_ip6_addr_type_t
 */
esp_ip6_addr_type_t esp_netif_ip6_get_addr_type(esp_ip6_addr_t *ip6_addr);

#ifdef __cplusplus
}
#endif
