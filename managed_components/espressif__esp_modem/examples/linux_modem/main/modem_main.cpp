/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#include <memory>
#include <cassert>
#include <unistd.h>
#include <esp_log.h>
#include "cxx_include/esp_modem_terminal.hpp"
#include "cxx_include/esp_modem_api.hpp"
#include "cxx_include/esp_modem_dte.hpp"
#include "esp_modem_config.h"
#include "esp_netif.h"
#include "vfs_resource/vfs_create.hpp"


#define CONFIG_EXAMPLE_SIM_PIN "1234"
#define CONFIG_USE_VFS_UART     1

using namespace esp_modem;

[[maybe_unused]] constexpr auto TAG = "linux_modem_main";

extern "C" void app_main(void)
{
    // init the DTE
    esp_modem_dte_config_t dte_config = {
        .dte_buffer_size = 512,
        .task_stack_size = 1024,
        .task_priority = 10,
        .vfs_config = {}
    };
#if CONFIG_USE_VFS_UART == 1
    struct esp_modem_vfs_uart_creator uart_config = {
        .dev_name = "/dev/ttyUSB0",
        .uart = {}
    };
    assert(vfs_create_uart(&uart_config, &dte_config.vfs_config) == true);
#else
    /**
     * @note: It is possible to setup a serial to socket bridge, running a this on a remote host which connects `/dev/ttyS0` to the modem
     * socat TCP-L:2222 GOPEN:/dev/ttyS0,ispeed=115200,ospeed=1152000,b115200,raw,echo=0
     */
    struct esp_modem_vfs_socket_creator socket_config = {
        .host_name = "raspberrypi.local",
        .port = 2222
    };
    assert(vfs_create_socket(&socket_config, &dte_config.vfs_config) == true);
#endif
    auto dte = create_vfs_dte(&dte_config);

    esp_netif_config_t netif_config = {
        .dev_name = "/dev/net/tun",
        .if_name = "tun0"
    };
    esp_netif_t *tun_netif = esp_netif_new(&netif_config);

    esp_modem_dce_config_t dce_config = ESP_MODEM_DCE_DEFAULT_CONFIG("internet");

    auto dce = create_SIM7600_dce(&dce_config, dte, tun_netif);
    assert(dce != nullptr);

    dce->set_command_mode();

    bool pin_ok = true;
    if (dce->read_pin(pin_ok) == command_result::OK && !pin_ok) {
        ESP_MODEM_THROW_IF_FALSE(dce->set_pin(CONFIG_EXAMPLE_SIM_PIN) == command_result::OK, "Cannot set PIN!");
        usleep(1000000);
    }
    std::string str;
//    dce->set_mode(esp_modem::modem_mode::CMUX_MODE);
    dce->get_imsi(str);
    ESP_LOGI(TAG, "Modem IMSI number: %s", str.c_str());
    dce->get_imei(str);
    ESP_LOGI(TAG, "Modem IMEI number: %s", str.c_str());
    while (command_result::OK != dce->get_operator_name(str)) {
        printf(".\n");
    }
    ESP_LOGI(TAG, "Operator name: %s", str.c_str());

    dce->set_mode(esp_modem::modem_mode::DATA_MODE);

    usleep(15'000'000);
    esp_netif_destroy(tun_netif);
}
