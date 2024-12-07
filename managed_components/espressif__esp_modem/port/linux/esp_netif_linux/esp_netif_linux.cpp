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

#include <thread>
#include <atomic>
#include <stdexcept>
#include <cstring>
#include <net/if.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/if_tun.h>
#include "esp_netif.h"
#include "esp_err.h"
#include "esp_log.h"

#define BUF_SIZE 1518

static const char *TAG = "esp_netif_linux";

extern "C" int ppp_netif_init(esp_netif_t *netif);
extern "C" int tun_read(void);

class NetifStorage;

void read_task(NetifStorage *netif);

class NetifStorage: public esp_netif_obj {
public:
    explicit NetifStorage(const esp_netif_config_t *config) : esp_netif_obj(), exit(false)
    {
        if ((fd = open(config->dev_name, O_RDWR)) == -1) {
            ESP_LOGE(TAG, "Cannot open %s", config->dev_name);
            throw std::runtime_error("Failed to open device");
        }
        struct ifreq ifr = { };
        ifr.ifr_flags = IFF_TUN;
        strncpy(ifr.ifr_name, config->if_name, IFNAMSIZ);

        if (ioctl(fd, TUNSETIFF, (void *)&ifr) == -1) {
            ESP_LOGE(TAG, "Cannot set ioctl TUNSETIFF %m");
            throw std::runtime_error("Failed to set tun device interface name");
        }
        ioctl(fd, TUNSETNOCSUM, 1);

        in_buf = new uint8_t[BUF_SIZE];
        out_buf = new uint8_t[BUF_SIZE];

        if (!ppp_netif_init(this)) {
            ESP_LOGE(TAG, "Cannot initialize pppos lwip netif %m");
            throw std::runtime_error("Failed setup ppp interface");
        }

        task = std::thread(read_task, this);
    }

    ~NetifStorage()
    {
        exit = true;
        task.join();
        close(fd);
        delete[] in_buf;
        delete[] out_buf;
    }

    std::thread task;
    std::atomic<bool> exit;

    static void read_task(NetifStorage *netif)
    {
        while (!netif->exit.load()) {
            tun_read();
        }
    }
};




extern "C" esp_netif_t *esp_netif_new(const esp_netif_config_t *config)
{
    return new NetifStorage(config);
}

void esp_netif_destroy(esp_netif_t *netif)
{
    delete static_cast<NetifStorage *>(netif);
}
