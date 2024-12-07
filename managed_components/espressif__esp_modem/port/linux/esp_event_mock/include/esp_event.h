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

#include <stdint.h>
#include "esp_netif.h"
#include "esp_err.h"


typedef void *system_event_t;

extern const char *WIFI_EVENT;
extern const char *IP_EVENT;

#define ESP_EVENT_ANY_BASE     NULL             /**< register handler for any event base */
#define ESP_EVENT_ANY_ID       -1               /**< register handler for any event id */

typedef struct {
    int if_index;                    /*!< Interface index for which the event is received (left for legacy compilation) */
    esp_netif_t *esp_netif;          /*!< Pointer to corresponding esp-netif object */
    esp_netif_ip6_info_t ip6_info;   /*!< IPv6 address of the interface */
    int ip_index;                    /*!< IPv6 address index */
} ip_event_got_ip6_t;

esp_err_t esp_event_handler_register(const char *event_base, int32_t event_id, void *event_handler, void *event_handler_arg);

esp_err_t esp_event_handler_unregister(const char *event_base, int32_t event_id, void *event_handler);

typedef void *QueueHandle_t;
