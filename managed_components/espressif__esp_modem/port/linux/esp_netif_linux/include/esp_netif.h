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
#include <stdlib.h>
#include <stdbool.h>
#include "esp_netif_ip_addr.h"
#include "esp_err.h"
#include "esp_event_base.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct esp_netif_obj esp_netif_t;

typedef struct esp_netif_driver_base_s {
    esp_err_t (*post_attach)(esp_netif_t *netif, void *h);
    esp_netif_t *netif;
} esp_netif_driver_base_t;

struct esp_netif_driver_ifconfig {
    void *handle;
    esp_err_t (*transmit)(void *h, void *buffer, size_t len);
    esp_err_t (*transmit_wrap)(void *h, void *buffer, size_t len, void *netstack_buffer);
    void (*driver_free_rx_buffer)(void *h, void *buffer);
};

struct esp_netif_config {
    const char *dev_name;  /**< Name of the file device */
    const char *if_name;   /**< Network interface name */
};

struct esp_netif_obj {
    uint8_t *in_buf;
    uint8_t *out_buf;
    int fd;
    esp_err_t (*transmit)(void *h, void *buffer, size_t len);
    void *ctx;
};

int esp_netif_receive(esp_netif_t *netif, uint8_t *data, size_t len);

typedef struct esp_netif_config esp_netif_config_t;

esp_netif_t *esp_netif_new(const esp_netif_config_t *esp_netif_config);

void esp_netif_destroy(esp_netif_t *esp_netif);

#ifdef __cplusplus
}
#endif
