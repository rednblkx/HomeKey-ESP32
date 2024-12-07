/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

/*
 * Power Saving Mode(PSM) in Sim70XX modems along with
 * Deep sleep and Light Sleep modes in esp32.
*/
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_log.h"
#include "driver/uart.h"
#include "string.h"
#include "driver/gpio.h"
#include "esp_sleep.h"
#include "driver/rtc_io.h"
#include "esp_modem_api.h"
#include "esp_event.h"
#include "sdkconfig.h"


#define BUF_SIZE 1024

#define TXD_PIN             ((gpio_num_t)CONFIG_EXAMPLE_MODEM_UART_TX_PIN)
#define RXD_PIN             ((gpio_num_t)CONFIG_EXAMPLE_MODEM_UART_RX_PIN)
#define GPIO_INPUT_STATUS   ((gpio_num_t)CONFIG_EXAMPLE_MODEM_STATUS_PIN)
#define GPIO_OUTPUT_PWRKEY  ((gpio_num_t)CONFIG_EXAMPLE_MODEM_PWRKEY_PIN)
#define GPIO_OUTPUT_PIN_SEL (1ULL<<GPIO_OUTPUT_PWRKEY)

#define CHECK_ERR(cmd, success_action)  do {    \
        esp_err_t ret = cmd;                    \
        if (ret == ESP_OK) {                    \
            success_action;                     \
        } else {                                \
            ESP_LOGE(TAG, "Failed with %s", ret == ESP_ERR_TIMEOUT ? "TIMEOUT":"ERROR");  \
        } } while (0)


const char *TAG = "modem_psm";
RTC_DATA_ATTR static int boot_count = 0;


void dce_init(esp_modem_dce_t **dce, esp_netif_t **esp_netif)
{
    esp_modem_dce_config_t dce_config = ESP_MODEM_DCE_DEFAULT_CONFIG(CONFIG_EXAMPLE_MODEM_PPP_APN);
    esp_modem_dte_config_t dte_config = ESP_MODEM_DTE_DEFAULT_CONFIG();
    esp_netif_config_t netif_ppp_config = ESP_NETIF_DEFAULT_PPP();
    *esp_netif = esp_netif_new(&netif_ppp_config);
    assert(*esp_netif);

    /* setup UART specific configuration based on kconfig options */
    dte_config.uart_config.tx_io_num = CONFIG_EXAMPLE_MODEM_UART_TX_PIN;
    dte_config.uart_config.rx_io_num = CONFIG_EXAMPLE_MODEM_UART_RX_PIN;
    dte_config.uart_config.rx_buffer_size = CONFIG_EXAMPLE_MODEM_UART_RX_BUFFER_SIZE;
    dte_config.uart_config.tx_buffer_size = CONFIG_EXAMPLE_MODEM_UART_TX_BUFFER_SIZE;
    dte_config.uart_config.event_queue_size = CONFIG_EXAMPLE_MODEM_UART_EVENT_QUEUE_SIZE;
    dte_config.task_stack_size = CONFIG_EXAMPLE_MODEM_UART_EVENT_TASK_STACK_SIZE;
    dte_config.task_priority = CONFIG_EXAMPLE_MODEM_UART_EVENT_TASK_PRIORITY;
    dte_config.dte_buffer_size = CONFIG_EXAMPLE_MODEM_UART_RX_BUFFER_SIZE / 2;

    ESP_LOGI(TAG, "Initializing esp_modem for a generic module...");
    *dce = esp_modem_new(&dte_config, &dce_config, *esp_netif);
    assert(*dce);
}

void config_pwrkey_gpio(void)
{
    gpio_config_t io_conf = {};                     //zero-initialize the config structure.

    io_conf.intr_type = GPIO_INTR_DISABLE;          //disable interrupt
    io_conf.mode = GPIO_MODE_OUTPUT;                //set as output mode
    io_conf.pin_bit_mask = GPIO_OUTPUT_PIN_SEL;     //bit mask of the pins that you want to set,e.g.GPIO18/19
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;   //disable pull-down mode
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;       //disable pull-up mode

    gpio_config(&io_conf);                          //configure GPIO with the given settings
}

void power_on_modem(esp_modem_dce_t *dce)
{
    rtc_gpio_hold_dis(GPIO_OUTPUT_PWRKEY);

    /* Power on the modem */
    ESP_LOGI(TAG, "Power on the modem");
    gpio_set_level(GPIO_OUTPUT_PWRKEY, 1);
    vTaskDelay(pdMS_TO_TICKS(500));
    gpio_set_level(GPIO_OUTPUT_PWRKEY, 0);

    rtc_gpio_hold_en(GPIO_OUTPUT_PWRKEY);
    vTaskDelay(pdMS_TO_TICKS(2000));

    CHECK_ERR(esp_modem_sync(dce), ESP_LOGI(TAG, "OK"));
}

void power_down_modem(esp_modem_dce_t *dce)
{
    char data[BUF_SIZE];

    ESP_LOGI(TAG, "Power down the modem");

    /* Power down the modem by AT command */
    CHECK_ERR(esp_modem_at(dce, "AT+CPOWD=1", data, 500), ESP_LOGI(TAG, "OK. %s", data));
}

void run_at(esp_modem_dce_t *dce, uint8_t count)
{
    for (int i = 0; i < count; i++) {
        CHECK_ERR(esp_modem_sync(dce), ESP_LOGI(TAG, "OK"));
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void run_at_ping(esp_modem_dce_t *dce)
{
    char data[BUF_SIZE];
    int rssi, ber;
    CHECK_ERR(esp_modem_get_signal_quality(dce, &rssi, &ber), ESP_LOGI(TAG, "OK. rssi=%d, ber=%d", rssi, ber));
    vTaskDelay(pdMS_TO_TICKS(1000));

    CHECK_ERR(esp_modem_at(dce, "AT+CPSMS?", data, 500), ESP_LOGI(TAG, "OK. %s", data));
    vTaskDelay(pdMS_TO_TICKS(1000));

    CHECK_ERR(esp_modem_at(dce, "AT+CPSI?", data, 500), ESP_LOGI(TAG, "OK. %s", data));             // Inquiring UE system information
    CHECK_ERR(esp_modem_at(dce, "AT+CNACT=0,1", data, 500), ESP_LOGI(TAG, "OK. %s", data));         // Activate the APP network
    CHECK_ERR(esp_modem_at(dce, "AT+SNPDPID=0", data, 500), ESP_LOGI(TAG, "OK. %s", data));         // Select PDP index for PING
    CHECK_ERR(esp_modem_at(dce, "AT+SNPING4=\"8.8.8.8\",3,16,1000", data, 500), ESP_LOGI(TAG, "OK. %s", data));     // Send IPv4 PING
    CHECK_ERR(esp_modem_at(dce, "AT+CNACT=0,0", data, 500), ESP_LOGI(TAG, "OK. %s", data));         // Deactivate the APP network
}


void app_main(void)
{
    char *cmd;
    char data[BUF_SIZE];
    esp_modem_dce_t *dce = NULL;
    esp_netif_t *esp_netif = NULL;
    esp_sleep_wakeup_cause_t wakeup_cause = esp_sleep_get_wakeup_cause();
    ESP_LOGI(TAG, "Deep sleep Wake Up Cause: %d\n", wakeup_cause);

    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    dce_init(&dce, &esp_netif);

    switch (wakeup_cause) {
    case ESP_SLEEP_WAKEUP_UNDEFINED:
        /* Power on the modem */
        config_pwrkey_gpio();
        power_on_modem(dce);

        run_at(dce, 3);

        /* Configure Power Saving Mode in the modem */
        asprintf(&cmd, "AT+CPSMS=1,,,\"%s\",\"%s\"\r\n", CONFIG_EXAMPLE_MODEM_T3412_PERIODIC_TAU,
                 CONFIG_EXAMPLE_MODEM_T3324_ACTIVE_TIME);
        CHECK_ERR(esp_modem_at(dce, cmd, data, 500), ESP_LOGI(TAG, "OK. %s", data));
        free(cmd);
        vTaskDelay(pdMS_TO_TICKS(1000));

        /* Perform Communication tasks */
        run_at_ping(dce);
        vTaskDelay(pdMS_TO_TICKS(1000));
        break;

    case ESP_SLEEP_WAKEUP_EXT0:
        run_at(dce, 3);

        /* Perform Communication tasks */
        run_at_ping(dce);
        vTaskDelay(pdMS_TO_TICKS(1000));
        break;

    default:
        ESP_LOGI(TAG, "Not a deep sleep reset: %d\n", wakeup_cause);
    }

    ESP_LOGI(TAG, "Light sleep before going to Deep sleep\n\n");

    /* Enter light sleep mode as long as the modem is awake */
    esp_sleep_enable_timer_wakeup(CONFIG_EXAMPLE_MODEM_LIGHT_SLEEP_DURATION * 1000000);
    esp_light_sleep_start();
    esp_sleep_disable_wakeup_source(ESP_SLEEP_WAKEUP_TIMER);    // Disabling timer before going to deep sleep.
    wakeup_cause = esp_sleep_get_wakeup_cause();
    ESP_LOGI(TAG, "Light sleep wakeup cause: %d\n", wakeup_cause);

    /* DCE and netif clean-up */
    /* In Deep-sleep mode, the CPUs, most of the RAM, and all digital peripherals are powered off.
     * So dce and netif would be lost anyway if they are not explicitly destroyed.
     * But since dce is also linked with UART communication with the modem it's better to exit gracefully before going to deep sleep.
     */
    esp_modem_destroy(dce);
    esp_netif_destroy(esp_netif);

    /* Enable wakeup if status pin is high */
    esp_sleep_enable_ext0_wakeup(GPIO_INPUT_STATUS, 1);

    ESP_LOGI(TAG, "Entering deep sleep: %d\n", boot_count++);
    esp_deep_sleep_start();
}
