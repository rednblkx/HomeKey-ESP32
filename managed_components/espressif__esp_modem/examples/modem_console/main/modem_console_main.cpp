/*
 * SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

/*
 * Modem console example
*/

#include <cstdio>
#include <cstring>
#include <map>
#include <vector>
#include "sdkconfig.h"
#include "esp_console.h"
#include "esp_event.h"
#include "nvs_flash.h"
#include "esp_sleep.h"
#include "driver/gpio.h"
#include "cxx_include/esp_modem_dte.hpp"
#include "esp_modem_config.h"
#include "cxx_include/esp_modem_api.hpp"
#if defined(CONFIG_EXAMPLE_SERIAL_CONFIG_USB)
#include "esp_modem_usb_config.h"
#include "cxx_include/esp_modem_usb_api.hpp"
#endif
#include "esp_log.h"
#include "console_helper.hpp"
#include "my_module_dce.hpp"

#if defined(CONFIG_EXAMPLE_FLOW_CONTROL_NONE)
#define EXAMPLE_FLOW_CONTROL ESP_MODEM_FLOW_CONTROL_NONE
#elif defined(CONFIG_EXAMPLE_FLOW_CONTROL_SW)
#define EXAMPLE_FLOW_CONTROL ESP_MODEM_FLOW_CONTROL_SW
#elif defined(CONFIG_EXAMPLE_FLOW_CONTROL_HW)
#define EXAMPLE_FLOW_CONTROL ESP_MODEM_FLOW_CONTROL_HW
#endif

#define CHECK_ERR(cmd, success_action)  do { \
        auto err = cmd; \
        if (err == command_result::OK) {     \
            success_action;                  \
            return 0;                        \
        } else {    \
            ESP_LOGE(TAG, "Failed with %s", err == command_result::TIMEOUT ? "TIMEOUT":"ERROR");  \
            return 1;                            \
        } } while (0)

/**
 * Default APN name is taken from Kconfig (this could be updated runtime)
 */
#define DEFAULT_APN CONFIG_EXAMPLE_MODEM_PPP_APN

#define GPIO_OUTPUT_PWRKEY    (gpio_num_t)CONFIG_EXAMPLE_MODEM_PWRKEY_PIN
#define GPIO_OUTPUT_PIN_SEL  (1ULL<<GPIO_OUTPUT_PWRKEY)

extern "C" void modem_console_register_http(void);
extern "C" void modem_console_register_ping(void);

static const char *TAG = "modem_console";
static esp_console_repl_t *s_repl = nullptr;

using namespace esp_modem;
static SignalGroup exit_signal;


void config_gpio(void)
{
    gpio_config_t io_conf = {};                     //zero-initialize the config structure.

    io_conf.intr_type = GPIO_INTR_DISABLE;          //disable interrupt
    io_conf.mode = GPIO_MODE_OUTPUT;                //set as output mode
    io_conf.pin_bit_mask = GPIO_OUTPUT_PIN_SEL;     //bit mask of the pins that you want to set,e.g.GPIO18/19
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;   //disable pull-down mode
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;       //disable pull-up mode

    gpio_config(&io_conf);                          //configure GPIO with the given settings
}

void wakeup_modem(void)
{
    /* Power on the modem */
    ESP_LOGI(TAG, "Power on the modem");
    gpio_set_level(GPIO_OUTPUT_PWRKEY, 1);
    vTaskDelay(pdMS_TO_TICKS(1000));
    gpio_set_level(GPIO_OUTPUT_PWRKEY, 0);

    vTaskDelay(pdMS_TO_TICKS(2000));
}

#ifdef CONFIG_ESP_MODEM_URC_HANDLER
command_result handle_urc(uint8_t *data, size_t len)
{
    ESP_LOG_BUFFER_HEXDUMP("on_read", data, len, ESP_LOG_INFO);
    return command_result::TIMEOUT;
}
#endif

extern "C" void app_main(void)
{
    static RTC_RODATA_ATTR char apn_rtc[20] = DEFAULT_APN;
    static RTC_DATA_ATTR modem_mode mode_rtc = esp_modem::modem_mode::COMMAND_MODE;

    config_gpio();

    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    // init the netif, DTE and DCE respectively
    esp_modem_dce_config_t dce_config = ESP_MODEM_DCE_DEFAULT_CONFIG(DEFAULT_APN);
    esp_netif_config_t ppp_netif_config = ESP_NETIF_DEFAULT_PPP();
    esp_netif_t *esp_netif = esp_netif_new(&ppp_netif_config);
    assert(esp_netif);

#if defined(CONFIG_EXAMPLE_SERIAL_CONFIG_UART)
    esp_modem_dte_config_t dte_config = ESP_MODEM_DTE_DEFAULT_CONFIG();
    /* setup UART specific configuration based on kconfig options */
    dte_config.uart_config.tx_io_num = CONFIG_EXAMPLE_MODEM_UART_TX_PIN;
    dte_config.uart_config.rx_io_num = CONFIG_EXAMPLE_MODEM_UART_RX_PIN;
    dte_config.uart_config.rts_io_num = CONFIG_EXAMPLE_MODEM_UART_RTS_PIN;
    dte_config.uart_config.cts_io_num = CONFIG_EXAMPLE_MODEM_UART_CTS_PIN;
    dte_config.uart_config.flow_control = EXAMPLE_FLOW_CONTROL;
    dte_config.uart_config.rx_buffer_size = CONFIG_EXAMPLE_MODEM_UART_RX_BUFFER_SIZE;
    dte_config.uart_config.tx_buffer_size = CONFIG_EXAMPLE_MODEM_UART_TX_BUFFER_SIZE;
    dte_config.uart_config.event_queue_size = CONFIG_EXAMPLE_MODEM_UART_EVENT_QUEUE_SIZE;
    dte_config.task_stack_size = CONFIG_EXAMPLE_MODEM_UART_EVENT_TASK_STACK_SIZE;
    dte_config.task_priority = CONFIG_EXAMPLE_MODEM_UART_EVENT_TASK_PRIORITY;
    dte_config.dte_buffer_size = CONFIG_EXAMPLE_MODEM_UART_RX_BUFFER_SIZE / 2;
    auto uart_dte = create_uart_dte(&dte_config);

#if defined(CONFIG_EXAMPLE_MODEM_DEVICE_SHINY)
    ESP_LOGI(TAG, "Initializing esp_modem for the SHINY module...");
    auto dce = create_shiny_dce(&dce_config, uart_dte, esp_netif);
#elif defined(CONFIG_EXAMPLE_MODEM_DEVICE_BG96)
    ESP_LOGI(TAG, "Initializing esp_modem for the BG96 module...");
    auto dce = create_BG96_dce(&dce_config, uart_dte, esp_netif);
#elif defined(CONFIG_EXAMPLE_MODEM_DEVICE_SIM800)
    ESP_LOGI(TAG, "Initializing esp_modem for the SIM800 module...");
    auto dce = create_SIM800_dce(&dce_config, uart_dte, esp_netif);
#elif defined(CONFIG_EXAMPLE_MODEM_DEVICE_SIM7000)
    ESP_LOGI(TAG, "Initializing esp_modem for the SIM7000 module...");
    auto dce = create_SIM7000_dce(&dce_config, uart_dte, esp_netif);
#elif defined(CONFIG_EXAMPLE_MODEM_DEVICE_SIM7070)
    ESP_LOGI(TAG, "Initializing esp_modem for the SIM7070 module...");
    auto dce = create_SIM7070_dce(&dce_config, uart_dte, esp_netif);
#elif CONFIG_EXAMPLE_MODEM_DEVICE_SIM7600 == 1
    ESP_LOGI(TAG, "Initializing esp_modem for the SIM7600 module...");
    auto dce = create_SIM7600_dce(&dce_config, uart_dte, esp_netif);
#else
    ESP_LOGI(TAG, "Initializing esp_modem for a generic module...");
    auto dce = create_generic_dce(&dce_config, uart_dte, esp_netif);
#endif


#elif defined(CONFIG_EXAMPLE_SERIAL_CONFIG_USB)
    while (1) {
        exit_signal.clear(1);
#if CONFIG_EXAMPLE_MODEM_DEVICE_BG96 == 1
        ESP_LOGI(TAG, "Initializing esp_modem for the BG96 module...");
        struct esp_modem_usb_term_config usb_config = ESP_MODEM_BG96_USB_CONFIG();
#elif CONFIG_EXAMPLE_MODEM_DEVICE_EC20 == 1
        ESP_LOGI(TAG, "Initializing esp_modem for the EC20 module...");
        struct esp_modem_usb_term_config usb_config = ESP_MODEM_EC20_USB_CONFIG();
#elif CONFIG_EXAMPLE_MODEM_DEVICE_SIM7600 == 1
        ESP_LOGI(TAG, "Initializing esp_modem for the SIM7600 module...");
        struct esp_modem_usb_term_config usb_config = ESP_MODEM_SIM7600_USB_CONFIG();
#elif CONFIG_EXAMPLE_MODEM_DEVICE_A7670 == 1
        ESP_LOGI(TAG, "Initializing esp_modem for the A7670 module...");
        struct esp_modem_usb_term_config usb_config = ESP_MODEM_A7670_USB_CONFIG();
#else
#error USB modem not selected
#endif
        const esp_modem_dte_config_t dte_config = ESP_MODEM_DTE_DEFAULT_USB_CONFIG(usb_config);
        ESP_LOGI(TAG, "Waiting for USB device connection...");
        auto dte = create_usb_dte(&dte_config);
        dte->set_error_cb([&](terminal_error err) {
            ESP_LOGI(TAG, "error handler %d", (int)err);
            if (err == terminal_error::DEVICE_GONE) {
                exit_signal.set(1);
            }
        });
#if CONFIG_EXAMPLE_MODEM_DEVICE_BG96 == 1
        std::unique_ptr<DCE> dce = create_BG96_dce(&dce_config, dte, esp_netif);
#elif CONFIG_EXAMPLE_MODEM_DEVICE_SIM7600 == 1 || CONFIG_EXAMPLE_MODEM_DEVICE_A7670 == 1 || CONFIG_EXAMPLE_MODEM_DEVICE_EC20 == 1
        std::unique_ptr<DCE> dce = create_SIM7600_dce(&dce_config, dte, esp_netif);
#else
#error USB modem not selected
#endif

#else
#error Invalid serial connection to modem.
#endif

    assert(dce != nullptr);

    if (dte_config.uart_config.flow_control == ESP_MODEM_FLOW_CONTROL_HW) {
        if (command_result::OK != dce->set_flow_control(2, 2)) {
            ESP_LOGE(TAG, "Failed to set the set_flow_control mode");
            return;
        }
        ESP_LOGI(TAG, "set_flow_control OK");
    }

    // init console REPL environment
    esp_console_repl_config_t repl_config = ESP_CONSOLE_REPL_CONFIG_DEFAULT();
    esp_console_dev_uart_config_t uart_config = ESP_CONSOLE_DEV_UART_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_console_new_repl_uart(&uart_config, &repl_config, &s_repl));

    switch (esp_sleep_get_wakeup_cause()) {
    case ESP_SLEEP_WAKEUP_TIMER:
        if (esp_modem::modem_mode::CMUX_MODE == mode_rtc) {
            ESP_LOGI(TAG, "Deep sleep reset\n");

            /* Set APN */
            auto new_pdp = std::unique_ptr<PdpContext>(new PdpContext(apn_rtc));
            dce->get_module()->configure_pdp_context(std::move(new_pdp));

            /* Set CMUX mode */
            if (!dce->set_mode(mode_rtc)) {
                ESP_LOGE(TAG, "Failed to set the desired mode");
            }
        }
        break;
    case ESP_SLEEP_WAKEUP_UNDEFINED:
    default:
        ESP_LOGD(TAG, "Not a deep sleep reset\n");
    }

    modem_console_register_http();
    modem_console_register_ping();
    const struct SetModeArgs {
        SetModeArgs(): mode(STR1, nullptr, nullptr, "<mode>", "PPP, CMD or CMUX") {}
        CommandArgs mode;
    } set_mode_args;
    const ConsoleCommand SetModeParser("set_mode", "sets modem mode", &set_mode_args, sizeof(set_mode_args), [&](ConsoleCommand * c) {
        if (c->get_count_of(&SetModeArgs::mode)) {
            auto mode = c->get_string_of(&SetModeArgs::mode);
            modem_mode dev_mode;
            if (mode == "AUTO") {
                dev_mode = esp_modem::modem_mode::AUTODETECT;
            } else if (mode == "UNDEF") {
                dev_mode = esp_modem::modem_mode::UNDEF;
            } else if (mode == "CMUX1") {
                dev_mode = esp_modem::modem_mode::CMUX_MANUAL_MODE;
            } else if (mode == "CMUX2") {
                dev_mode = esp_modem::modem_mode::CMUX_MANUAL_EXIT;
            } else if (mode == "CMUX3") {
                dev_mode = esp_modem::modem_mode::CMUX_MANUAL_SWAP;
            } else if (mode == "CMUX4") {
                dev_mode = esp_modem::modem_mode::CMUX_MANUAL_DATA;
            } else if (mode == "CMUX5") {
                dev_mode = esp_modem::modem_mode::CMUX_MANUAL_COMMAND;
            } else if (mode == "CMD") {
                dev_mode = esp_modem::modem_mode::COMMAND_MODE;
            } else if (mode == "PPP") {
                dev_mode = esp_modem::modem_mode::DATA_MODE;
            } else if (mode == "CMUX") {
                /* Even if switching to CMUX fails, we make the DTE multiplex terminal.
                   (This is potentially a bug and might be fixed eventually) */
                dev_mode = esp_modem::modem_mode::CMUX_MODE;
                mode_rtc = dev_mode;
            } else {
                ESP_LOGE(TAG, "Unsupported mode: %s", mode.c_str());
                return 1;
            }
            ESP_LOGI(TAG, "Switching to %s name...", mode.c_str());
            if (!dce->set_mode(dev_mode)) {
                ESP_LOGE(TAG, "Failed to set the desired mode");
                return 1;
            }
            mode_rtc = dev_mode;

            ESP_LOGI(TAG, "OK");
        }
        return 0;
    });

    const struct SetPinArgs {
        SetPinArgs(): pin(STR1, nullptr, nullptr, "<pin>", "PIN") {}
        CommandArgs pin;
    } set_pin_args;
    const ConsoleCommand SetPinParser("set_pin", "sets SIM card PIN", &set_pin_args, sizeof(set_pin_args), [&](ConsoleCommand * c) {
        if (c->get_count_of(&SetPinArgs::pin)) {
            auto pin = c->get_string_of(&SetPinArgs::pin);
            ESP_LOGI(TAG, "Setting pin=%s...", pin.c_str());
            auto err = dce->set_pin(pin);
            if (err == command_result::OK) {
                ESP_LOGI(TAG, "OK");
            } else {
                ESP_LOGE(TAG, "Failed %s", err == command_result::TIMEOUT ? "TIMEOUT" : "");
                return 1;
            }
        }
        return 0;
    });

    const std::vector<CommandArgs> no_args;
    const ConsoleCommand ReadPinArgs("read_pin", "checks if SIM is unlocked", no_args, [&](ConsoleCommand * c) {
        bool pin_ok;
        ESP_LOGI(TAG, "Checking pin...");
        auto err = dce->read_pin(pin_ok);
        if (err == command_result::OK) {
            ESP_LOGI(TAG, "OK. Pin status: %s", pin_ok ? "true" : "false");
        } else {
            ESP_LOGE(TAG, "Failed %s", err == command_result::TIMEOUT ? "TIMEOUT" : "");
            return 1;
        }
        return 0;
    });

    const ConsoleCommand GetModuleName("get_module_name", "reads the module name", no_args, [&](ConsoleCommand * c) {
        std::string module_name;
        ESP_LOGI(TAG, "Reading module name...");
        CHECK_ERR(dce->get_module_name(module_name), ESP_LOGI(TAG, "OK. Module name: %s", module_name.c_str()));
    });

    const ConsoleCommand GetOperatorName("get_operator_name", "reads the operator name", no_args, [&](ConsoleCommand * c) {
        std::string operator_name;
        int act;
        ESP_LOGI(TAG, "Reading operator name...");
        CHECK_ERR(dce->get_operator_name(operator_name, act), ESP_LOGI(TAG, "OK. Operator name: %s", operator_name.c_str()));
    });

    const struct GenericCommandArgs {
        GenericCommandArgs():
            cmd(STR1, nullptr, nullptr, "<command>", "AT command to send to the modem"),
            timeout(INT0, "t", "timeout", "<timeout>", "command timeout"),
            pattern(STR0, "p", "pattern", "<pattern>", "command response to wait for"),
            no_cr(LIT0, "n", "no-cr", "do not add trailing CR to the command") {}
        CommandArgs cmd;
        CommandArgs timeout;
        CommandArgs pattern;
        CommandArgs no_cr;
    } send_cmd_args;
    const ConsoleCommand SendCommand("cmd", "sends generic AT command, no_args", &send_cmd_args, sizeof(send_cmd_args), [&](ConsoleCommand * c) {
        auto cmd = c->get_string_of(&GenericCommandArgs::cmd);
        auto timeout = c->get_count_of(&GenericCommandArgs::timeout) ? c->get_int_of(&GenericCommandArgs::timeout)
                       : 1000;
        ESP_LOGI(TAG, "Sending command %s with timeout %d", cmd.c_str(), timeout);
        auto pattern = c->get_string_of(&GenericCommandArgs::pattern);
        if (c->get_count_of(&GenericCommandArgs::no_cr) == 0) {
            cmd += '\r';
        }
        CHECK_ERR(dce->command(cmd, [&](uint8_t *data, size_t len) {
            std::string response((char *) data, len);
            ESP_LOGI(TAG, "%s", response.c_str());
            if (pattern.empty() || response.find(pattern) != std::string::npos) {
                return command_result::OK;
            }
            if (response.find(pattern) != std::string::npos) {
                return command_result::OK;
            }
            return command_result::TIMEOUT;
        }, timeout),);
    });

    const ConsoleCommand GetSignalQuality("get_signal_quality", "Gets signal quality", no_args, [&](ConsoleCommand * c) {
        int rssi, ber;
        CHECK_ERR(dce->get_signal_quality(rssi, ber), ESP_LOGI(TAG, "OK. rssi=%d, ber=%d", rssi, ber));
    });
    const ConsoleCommand GetBatteryStatus("get_battery_status", "Reads voltage/battery status", no_args, [&](ConsoleCommand * c) {
        int volt, bcl, bcs;
        CHECK_ERR(dce->get_battery_status(volt, bcl, bcs), ESP_LOGI(TAG, "OK. volt=%d, bcl=%d, bcs=%d", volt, bcl, bcs));
    });
    const ConsoleCommand PowerDown("power_down", "power down the module", no_args, [&](ConsoleCommand * c) {
        ESP_LOGI(TAG, "Power down the module...");
        CHECK_ERR(dce->power_down(), ESP_LOGI(TAG, "OK"));
    });
    const ConsoleCommand Reset("reset", "reset the module", no_args, [&](ConsoleCommand * c) {
        ESP_LOGI(TAG, "Resetting the module...");
        CHECK_ERR(dce->reset(), ESP_LOGI(TAG, "OK"));
    });
#ifdef CONFIG_ESP_MODEM_URC_HANDLER
    const ConsoleCommand HandleURC("urc", "toggle urc handling", no_args, [&](ConsoleCommand * c) {
        static int cnt = 0;
        if (++cnt % 2) {
            ESP_LOGI(TAG, "Adding URC handler");
            dce->set_urc(handle_urc);
        } else {
            ESP_LOGI(TAG, "URC removed");
            dce->set_urc(nullptr);
        }
        return 0;
    });
#endif
    const ConsoleCommand PauseNetwork("pause_net", "toggle network pause", no_args, [&](ConsoleCommand * c) {
        static int cnt = 0;
        if (++cnt % 2) {
            ESP_LOGI(TAG, "Pausing netif");
            dce->pause_netif(true);
        } else {
            ESP_LOGI(TAG, "Unpausing netif");
            dce->pause_netif(false);
        }
        return 0;
    });

    const struct SetApn {
        SetApn(): apn(STR1, nullptr, nullptr, "<apn>", "APN (Access Point Name)") {}
        CommandArgs apn;
    } set_apn;
    const ConsoleCommand SetApnParser("set_apn", "sets APN", &set_apn, sizeof(set_apn), [&](ConsoleCommand * c) {
        if (c->get_count_of(&SetApn::apn)) {
            auto apn = c->get_string_of(&SetApn::apn);
            ESP_LOGI(TAG, "Setting the APN=%s...", apn.c_str());
            strcpy(apn_rtc, apn.c_str());
            auto new_pdp = std::unique_ptr<PdpContext>(new PdpContext(apn));
            dce->get_module()->configure_pdp_context(std::move(new_pdp));
            ESP_LOGI(TAG, "OK");
        }
        return 0;
    });

    const ConsoleCommand ExitConsole("exit", "exit the console application", no_args, [&](ConsoleCommand * c) {
        ESP_LOGI(TAG, "Exiting...");
        exit_signal.set(1);
        return 0;
    });

    /* Put the esp32 into deep sleep */
    const struct DeepSleepArgs {
        DeepSleepArgs(): timeout(INT1, nullptr, nullptr, "<tout>", "TIMEOUT") {}
        CommandArgs timeout;
    } deep_sleep_args;
    const ConsoleCommand SetDeepSleep("set_deep_sleep", "Put esp32 to deep sleep", &deep_sleep_args, sizeof(deep_sleep_args), [&](ConsoleCommand * c) {
        int tout = c->get_int_of(&DeepSleepArgs::timeout);
        ESP_LOGI(TAG, "Entering deep sleep for %d sec", tout);
        ESP_LOGI(TAG, "Wakeup Cause: %d ", esp_sleep_get_wakeup_cause());
        esp_deep_sleep(tout * 1000000);
        return 0;
    });

    /* Wake up modem */
    const ConsoleCommand WakeupModem("wakeup_modem", "Wakes up the modem from PSM", no_args, [&](ConsoleCommand * c) {
        wakeup_modem();
        ESP_LOGI(TAG, "OK");
        return 0;
    });

    /* Enable PSM modem */
    const ConsoleCommand EnablePSM("enable_psm", "Enables PSM on the modem", no_args, [&](ConsoleCommand * c) {
        std::string out;
        CHECK_ERR(dce->at("AT+CPSMS=1", out, 500), ESP_LOGI(TAG, "OK. %s", out.c_str()));
        return 0;
    });

    /* Disable PSM modem */
    const ConsoleCommand DisablePSM("disable_psm", "Disables PSM on the modem", no_args, [&](ConsoleCommand * c) {
        std::string out;
        CHECK_ERR(dce->at("AT+CPSMS=0", out, 500),   ESP_LOGI(TAG, "OK. %s", out.c_str()));
        return 0;
    });

    /* Get modem PSM cfg */
    const ConsoleCommand GetModemCfg("get_psm_cfg", "Get PSM config", no_args, [&](ConsoleCommand * c) {
        std::string out;
        CHECK_ERR(dce->at("AT+CPSMS?", out, 500),  ESP_LOGI(TAG, "OK. %s", out.c_str()));
        return 0;
    });

    /* Set modem PSM config */
    const struct SetPsmCfgArgs {
        SetPsmCfgArgs():
            periodic_tau(STR1, nullptr, nullptr, "<Requested_Periodic-TAU>", "T3412 Timer in 8-bit format"),
            active_time(STR0, nullptr, nullptr, "<Requested_Active-Time>", "T3324 Timer in 8-bit format") {}
        CommandArgs periodic_tau;
        CommandArgs active_time;
    } set_psm_cfg_args;
    const ConsoleCommand SetPsmCfg("set_psm_cfg", "Set PSM config", &set_psm_cfg_args, sizeof(set_psm_cfg_args), [&](ConsoleCommand * c) {
        std::string out;
        auto periodic_tau = c->get_string_of(&SetPsmCfgArgs::periodic_tau);
        auto active_time = c->get_string_of(&SetPsmCfgArgs::active_time);

        /* Validate input */
        if ((strlen(periodic_tau.c_str()) != strlen("00000000")) ||
                (strlen(active_time.c_str()) != strlen("00000000"))) {
            ESP_LOGE(TAG, "Failed with ERROR: Invalid argument length");
            return 1;
        }

        /* Get PSM config */
        dce->at("AT+CPSMS?", out, 500);

        /* Update PSM config */
        if (out.size() > 8) {
            std::string set_cmd = "AT+CPSMS=" + std::to_string(out.c_str()[8] - '0') + ",,,\"" + periodic_tau.c_str() + "\",\"" + active_time.c_str() + "\"";
            CHECK_ERR(dce->at(set_cmd, out, 500),  ESP_LOGI(TAG, "OK. %s", out.c_str()));
        } else {
            ESP_LOGE(TAG, "Failed with ERROR: Invalid AT+CPSMS? return length");
        }

        return 0;
    });


    // start console REPL
    ESP_ERROR_CHECK(esp_console_start_repl(s_repl));
    // wait for exit
    exit_signal.wait_any(1, UINT32_MAX);
    s_repl->del(s_repl);
    ESP_LOGI(TAG, "Exiting...%" PRIu32, esp_get_free_heap_size());
#if defined(CONFIG_EXAMPLE_SERIAL_CONFIG_USB)
    // USB example runs in a loop to demonstrate hot-plugging and sudden disconnection features.
} // while (1)
#endif
}
