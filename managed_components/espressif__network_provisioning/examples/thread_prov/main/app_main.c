/* Network Provisioning Manager Example for Thread network

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <stdio.h>
#include <string.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/event_groups.h>

#include <esp_log.h>
#include <esp_openthread.h>
#include "esp_openthread_cli.h"
#include "esp_openthread_netif_glue.h"
#include "esp_openthread_types.h"
#include <esp_ot_config.h>
#include <esp_netif.h>
#include <esp_event.h>
#include <esp_mac.h>
#include <esp_vfs_eventfd.h>
#include <nvs_flash.h>

#include <network_provisioning/manager.h>

#include "openthread/cli.h"
#include "openthread/instance.h"
#include "openthread/logging.h"
#include "openthread/tasklet.h"
#include "openthread/thread.h"

#include <network_provisioning/scheme_ble.h>

#include "qrcode.h"

static const char *TAG = "app";

#if CONFIG_EXAMPLE_PROV_SECURITY_VERSION_2
#if CONFIG_EXAMPLE_PROV_SEC2_DEV_MODE
#define EXAMPLE_PROV_SEC2_USERNAME          "threadprov"
#define EXAMPLE_PROV_SEC2_PWD               "abcd1234"

/* This salt,verifier has been generated for username = "threadprov" and password = "abcd1234"
 * IMPORTANT NOTE: For production cases, this must be unique to every device
 * and should come from device manufacturing partition.*/
static const char sec2_salt[] = {
    0x1f, 0xff, 0x29, 0xf5, 0xc7, 0x7e, 0x07, 0x48, 0x02, 0xe9, 0x93, 0x3e, 0xa3, 0xa2, 0x26, 0x73
};

static const char sec2_verifier[] = {
    0xa7, 0x29, 0xe6, 0xa5, 0x4d, 0x20, 0x57, 0x71, 0x7c, 0x9d, 0x78, 0x2d, 0x0a, 0xb0, 0x9f, 0xec,
    0x7e, 0x8b, 0xab, 0xf5, 0xe6, 0xc3, 0x36, 0x41, 0x93, 0xfd, 0xb9, 0x49, 0x67, 0xe7, 0x7f, 0x79,
    0x66, 0x25, 0x2e, 0xac, 0x89, 0x19, 0xb2, 0xb3, 0x14, 0xb1, 0x16, 0xb0, 0xb0, 0xe4, 0x34, 0xd4,
    0x99, 0x40, 0x85, 0xa4, 0x99, 0x2b, 0x84, 0x21, 0xa1, 0xfb, 0x15, 0x48, 0x04, 0x91, 0xf5, 0x74,
    0x95, 0x8a, 0x88, 0xd4, 0x4e, 0x25, 0xf6, 0xf3, 0x8e, 0x5c, 0xf9, 0x3c, 0xda, 0xbb, 0x4f, 0xa2,
    0x47, 0xe1, 0x01, 0x8f, 0x1c, 0xf5, 0xe0, 0x34, 0x41, 0x0c, 0x88, 0x76, 0x46, 0xd0, 0x16, 0xd9,
    0xfa, 0x57, 0x3d, 0x78, 0x46, 0xf1, 0xcb, 0xb1, 0x05, 0x16, 0xab, 0xf7, 0xbf, 0x9d, 0xeb, 0x05,
    0x2e, 0xc1, 0xd5, 0xe1, 0xde, 0x92, 0xe6, 0x20, 0x5f, 0xe4, 0x27, 0xda, 0xe3, 0x59, 0x91, 0x27,
    0x7b, 0x40, 0x83, 0x4c, 0xe8, 0xb5, 0xe0, 0x75, 0xe6, 0xbf, 0x26, 0xa9, 0x67, 0x06, 0xa3, 0x15,
    0x2d, 0x20, 0x81, 0xd5, 0x2a, 0x2e, 0x30, 0x84, 0xdf, 0xa2, 0x82, 0x62, 0xc4, 0x47, 0x25, 0xb6,
    0x93, 0x73, 0x87, 0x3c, 0xa7, 0x57, 0x2a, 0x47, 0x96, 0x1d, 0x89, 0xce, 0x49, 0xc6, 0x9d, 0x4f,
    0x6b, 0x39, 0x38, 0x67, 0xbb, 0x85, 0x24, 0xdf, 0xcd, 0xf5, 0xf1, 0x9f, 0x0a, 0x9e, 0x1c, 0x31,
    0xfa, 0xf1, 0x01, 0xa5, 0x30, 0xf0, 0xcb, 0x5e, 0x1c, 0xd6, 0xa0, 0x11, 0x3f, 0xf8, 0xdd, 0x07,
    0x09, 0x53, 0x62, 0x9f, 0x76, 0x5c, 0x69, 0xd1, 0x5e, 0x5b, 0xc7, 0xb0, 0x0e, 0x53, 0xeb, 0x8c,
    0x67, 0x88, 0xc7, 0x45, 0xc0, 0x26, 0xd9, 0xfa, 0xf8, 0x63, 0x0c, 0x64, 0xcb, 0x9e, 0xf4, 0x1b,
    0xb3, 0xfd, 0x78, 0x0c, 0x47, 0x0f, 0x66, 0xf3, 0xf7, 0xcd, 0xe9, 0xc6, 0x36, 0xa5, 0x58, 0xe5,
    0x9d, 0x31, 0x53, 0xb2, 0xe4, 0x8e, 0xdd, 0xd0, 0x8d, 0x13, 0xe8, 0xc6, 0x96, 0x60, 0x30, 0x50,
    0xbc, 0xef, 0xce, 0xbc, 0x23, 0xe3, 0x60, 0x63, 0x54, 0x11, 0x24, 0xba, 0x68, 0x47, 0x6a, 0xb2,
    0x5e, 0x70, 0xa3, 0xa6, 0xc3, 0xad, 0x58, 0xd1, 0x3b, 0xce, 0xce, 0x90, 0xe9, 0x90, 0x7e, 0x7a,
    0xfb, 0x4f, 0x69, 0xa2, 0x81, 0xdf, 0x15, 0xec, 0xa7, 0x8f, 0xd6, 0x5a, 0xb8, 0x1f, 0x42, 0x18,
    0x0e, 0x4f, 0x3e, 0x45, 0x2d, 0x08, 0xf2, 0xd6, 0x51, 0x90, 0xef, 0x64, 0x77, 0xee, 0xcc, 0x3c,
    0xb4, 0xa6, 0x6f, 0x0b, 0x10, 0xb2, 0xce, 0x31, 0x19, 0x10, 0x8d, 0x75, 0x8f, 0xa8, 0xa2, 0x6e,
    0x7a, 0x00, 0x92, 0x91, 0xe2, 0x16, 0xe3, 0x7a, 0xf9, 0x1d, 0x4e, 0x39, 0xe5, 0xd0, 0xd1, 0x7e,
    0x80, 0x86, 0xf4, 0xd5, 0x08, 0xbc, 0xb0, 0xdd, 0x6b, 0x50, 0xfa, 0xdd, 0x16, 0x10, 0x23, 0x4b
};
#endif

static esp_err_t example_get_sec2_salt(const char **salt, uint16_t *salt_len)
{
#if CONFIG_EXAMPLE_PROV_SEC2_DEV_MODE
    ESP_LOGI(TAG, "Development mode: using hard coded salt");
    *salt = sec2_salt;
    *salt_len = sizeof(sec2_salt);
    return ESP_OK;
#elif CONFIG_EXAMPLE_PROV_SEC2_PROD_MODE
    ESP_LOGE(TAG, "Not implemented!");
    return ESP_FAIL;
#endif
}

static esp_err_t example_get_sec2_verifier(const char **verifier, uint16_t *verifier_len)
{
#if CONFIG_EXAMPLE_PROV_SEC2_DEV_MODE
    ESP_LOGI(TAG, "Development mode: using hard coded verifier");
    *verifier = sec2_verifier;
    *verifier_len = sizeof(sec2_verifier);
    return ESP_OK;
#elif CONFIG_EXAMPLE_PROV_SEC2_PROD_MODE
    /* This code needs to be updated with appropriate implementation to provide verifier */
    ESP_LOGE(TAG, "Not implemented!");
    return ESP_FAIL;
#endif
}
#endif

/* Signal Thread events on this event-group */
const int THREAD_ATTACHED_EVENT = BIT0;
static EventGroupHandle_t thread_event_group;

#define PROV_QR_VERSION         "v1"
#define PROV_TRANSPORT_BLE      "ble"
#define QRCODE_BASE_URL         "https://espressif.github.io/esp-jumpstart/qrcode.html"

/* Event handler for catching system events */
static void event_handler(void *arg, esp_event_base_t event_base,
                          int32_t event_id, void *event_data)
{
    if (event_base == NETWORK_PROV_EVENT) {
        switch (event_id) {
        case NETWORK_PROV_START:
            ESP_LOGI(TAG, "Provisioning started");
            break;
        case NETWORK_PROV_THREAD_DATASET_RECV: {
            // TODO Log thread dataset
            break;
        }
        case NETWORK_PROV_THREAD_DATASET_FAIL: {
            network_prov_thread_fail_reason_t *reason = (network_prov_thread_fail_reason_t *)event_data;
            ESP_LOGE(TAG, "Provisioning failed!\n\tReason : %s"
                     "\n\tPlease reset to factory and retry provisioning",
                     (*reason == NETWORK_PROV_THREAD_DATASET_INVALID) ?
                     "Invalid Thread dataset" : "Thread network not found");
            break;
        }
        case NETWORK_PROV_THREAD_DATASET_SUCCESS:
            ESP_LOGI(TAG, "Provisioning successful");
            break;
        case NETWORK_PROV_END:
            /* De-initialize manager once provisioning is finished */
            network_prov_mgr_deinit();
            break;
        default:
            break;
        }
    } else if (event_base == OPENTHREAD_EVENT && event_id == OPENTHREAD_EVENT_ATTACHED) {
        xEventGroupSetBits(thread_event_group, THREAD_ATTACHED_EVENT);
    } else if (event_base == PROTOCOMM_SECURITY_SESSION_EVENT) {
        switch (event_id) {
        case PROTOCOMM_SECURITY_SESSION_SETUP_OK:
            ESP_LOGI(TAG, "Secured session established!");
            break;
        case PROTOCOMM_SECURITY_SESSION_INVALID_SECURITY_PARAMS:
            ESP_LOGE(TAG, "Received invalid security parameters for establishing secure session!");
            break;
        case PROTOCOMM_SECURITY_SESSION_CREDENTIALS_MISMATCH:
            ESP_LOGE(TAG, "Received incorrect username and/or PoP for establishing secure session!");
            break;
        default:
            break;
        }
    }
}

static void get_device_service_name(char *service_name, size_t max)
{
    uint8_t ieee802154_mac[8];
    const char *ssid_prefix = "PROV_";
    esp_read_mac(ieee802154_mac, ESP_MAC_IEEE802154);
    snprintf(service_name, max, "%s%02X%02X%02X",
             ssid_prefix, ieee802154_mac[5], ieee802154_mac[6], ieee802154_mac[7]);
}

/* Handler for the optional provisioning endpoint registered by the application.
 * The data format can be chosen by applications. Here, we are using plain ascii text.
 * Applications can choose to use other formats like protobuf, JSON, XML, etc.
 */
esp_err_t custom_prov_data_handler(uint32_t session_id, const uint8_t *inbuf, ssize_t inlen,
                                   uint8_t **outbuf, ssize_t *outlen, void *priv_data)
{
    if (inbuf) {
        ESP_LOGI(TAG, "Received data: %.*s", inlen, (char *)inbuf);
    }
    char response[] = "SUCCESS";
    *outbuf = (uint8_t *)strdup(response);
    if (*outbuf == NULL) {
        ESP_LOGE(TAG, "System out of memory");
        return ESP_ERR_NO_MEM;
    }
    *outlen = strlen(response) + 1; /* +1 for NULL terminating byte */

    return ESP_OK;
}

static void network_prov_print_qr(const char *name, const char *username, const char *pop, const char *transport)
{
    if (!name || !transport) {
        ESP_LOGW(TAG, "Cannot generate QR code payload. Data missing.");
        return;
    }
    char payload[150] = {0};
    if (pop) {
#if CONFIG_EXAMPLE_PROV_SECURITY_VERSION_1
        snprintf(payload, sizeof(payload), "{\"ver\":\"%s\",\"name\":\"%s\"" \
                 ",\"pop\":\"%s\",\"transport\":\"%s\"}",
                 PROV_QR_VERSION, name, pop, transport);
#elif CONFIG_EXAMPLE_PROV_SECURITY_VERSION_2
        snprintf(payload, sizeof(payload), "{\"ver\":\"%s\",\"name\":\"%s\"" \
                 ",\"username\":\"%s\",\"pop\":\"%s\",\"transport\":\"%s\"}",
                 PROV_QR_VERSION, name, username, pop, transport);
#endif
    } else {
        snprintf(payload, sizeof(payload), "{\"ver\":\"%s\",\"name\":\"%s\"" \
                 ",\"transport\":\"%s\"}",
                 PROV_QR_VERSION, name, transport);
    }
#ifdef CONFIG_EXAMPLE_PROV_SHOW_QR
    ESP_LOGI(TAG, "Scan this QR code from the provisioning application for Provisioning.");
    esp_qrcode_config_t cfg = ESP_QRCODE_CONFIG_DEFAULT();
    esp_qrcode_generate(&cfg, payload);
    //TODO: Add the network protocol type to the QR code payload
#endif /* CONFIG EXAMPLE_PROV_SHOW_QR */
    ESP_LOGI(TAG, "If QR code is not visible, copy paste the below URL in a browser.\n%s?data=%s", QRCODE_BASE_URL, payload);
}

static esp_netif_t *init_openthread_netif(const esp_openthread_platform_config_t *config)
{
    esp_netif_config_t cfg = ESP_NETIF_DEFAULT_OPENTHREAD();
    esp_netif_t *netif = esp_netif_new(&cfg);
    assert(netif != NULL);
    ESP_ERROR_CHECK(esp_netif_attach(netif, esp_openthread_netif_glue_init(config)));

    return netif;
}

static void ot_task_worker(void *aContext)
{
    esp_openthread_platform_config_t config = {
        .radio_config = ESP_OPENTHREAD_DEFAULT_RADIO_CONFIG(),
        .host_config = ESP_OPENTHREAD_DEFAULT_HOST_CONFIG(),
        .port_config = ESP_OPENTHREAD_DEFAULT_PORT_CONFIG(),
    };

    // Initialize the OpenThread stack
    ESP_ERROR_CHECK(esp_openthread_init(&config));
#if CONFIG_OPENTHREAD_LOG_LEVEL_DYNAMIC
    // The OpenThread log level directly matches ESP log level
    (void)otLoggingSetLevel(CONFIG_LOG_DEFAULT_LEVEL);
#endif
#if CONFIG_OPENTHREAD_CLI
    // Initialize the OpenThread cli
    esp_openthread_cli_init();
#endif
    esp_netif_t *openthread_netif = init_openthread_netif(&config);
    // Initialize the esp_netif bindings
    esp_netif_set_default_netif(openthread_netif);

    // Run the main loop
#if CONFIG_OPENTHREAD_CLI
    esp_openthread_cli_create_task();
#endif
    esp_openthread_launch_mainloop();

    // Clean up
    esp_netif_destroy(openthread_netif);
    esp_openthread_netif_glue_deinit();

    esp_vfs_eventfd_unregister();
    vTaskDelete(NULL);
}

void app_main(void)
{
    /* Initialize NVS partition */
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        /* NVS partition was truncated
         * and needs to be erased */
        ESP_ERROR_CHECK(nvs_flash_erase());

        /* Retry nvs_flash_init */
        ESP_ERROR_CHECK(nvs_flash_init());
    }

    /* Initialize TCP/IP */
    ESP_ERROR_CHECK(esp_netif_init());

    /* Initialize the event loop */
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    thread_event_group = xEventGroupCreate();

    /* Register our event handler for OpenThread and Provisioning related events */
    ESP_ERROR_CHECK(esp_event_handler_register(NETWORK_PROV_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL));
#ifdef CONFIG_EXAMPLE_PROV_TRANSPORT_BLE
    ESP_ERROR_CHECK(esp_event_handler_register(PROTOCOMM_TRANSPORT_BLE_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL));
#endif
    ESP_ERROR_CHECK(esp_event_handler_register(PROTOCOMM_SECURITY_SESSION_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL));
    ESP_ERROR_CHECK(esp_event_handler_register(OPENTHREAD_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL));

    esp_vfs_eventfd_config_t eventfd_config = {
        .max_fds = 3,
    };
    ESP_ERROR_CHECK(esp_vfs_eventfd_register(&eventfd_config));
    xTaskCreate(ot_task_worker, "ot_task", 6144, xTaskGetCurrentTaskHandle(), 5, NULL);

    /* Configuration for the provisioning manager */
    network_prov_mgr_config_t config = {
        /* Use network_prov_scheme_ble as the Provisioning Scheme */
        .scheme = network_prov_scheme_ble,

        /* Any default scheme specific event handler that you would
         * like to choose. Since our example application requires
         * neither BT nor BLE, we can choose to release the associated
         * memory once provisioning is complete, or not needed
         * (in case when device is already provisioned). Choosing
         * appropriate scheme specific event handler allows the manager
         * to take care of this automatically. */
        .scheme_event_handler = NETWORK_PROV_SCHEME_BLE_EVENT_HANDLER_FREE_BLE
    };

    /* Initialize provisioning manager with the
     * configuration parameters set above */
    ESP_ERROR_CHECK(network_prov_mgr_init(config));

    bool provisioned = false;
#ifdef CONFIG_EXAMPLE_RESET_PROVISIONED
    network_prov_mgr_reset_thread_provisioning();
#else
    /* Let's find out if the device is provisioned */
    ESP_ERROR_CHECK(network_prov_mgr_is_thread_provisioned(&provisioned));

#endif
    /* If device is not yet provisioned start provisioning service */
    if (!provisioned) {
        ESP_LOGI(TAG, "Starting provisioning");

        /* What is the Device Service Name that we want
         * This translates to :
         *     - device name when scheme is network_prov_scheme_ble
         */
        char service_name[12];
        get_device_service_name(service_name, sizeof(service_name));

#ifdef CONFIG_EXAMPLE_PROV_SECURITY_VERSION_1
        /* What is the security level that we want (0, 1, 2):
         *      - NETWORK_PROV_SECURITY_0 is simply plain text communication.
         *      - NETWORK_PROV_SECURITY_1 is secure communication which consists of secure handshake
         *          using X25519 key exchange and proof of possession (pop) and AES-CTR
         *          for encryption/decryption of messages.
         *      - NETWORK_PROV_SECURITY_2 SRP6a based authentication and key exchange
         *        + AES-GCM encryption/decryption of messages
         */
        network_prov_security_t security = NETWORK_PROV_SECURITY_1;

        /* Do we want a proof-of-possession (ignored if Security 0 is selected):
         *      - this should be a string with length > 0
         *      - NULL if not used
         */
        const char *pop = "abcd1234";

        /* This is the structure for passing security parameters
         * for the protocomm security 1.
         */
        network_prov_security1_params_t *sec_params = pop;

        const char *username  = NULL;

#elif CONFIG_EXAMPLE_PROV_SECURITY_VERSION_2
        network_prov_security_t security = NETWORK_PROV_SECURITY_2;
        /* The username must be the same one, which has been used in the generation of salt and verifier */

#if CONFIG_EXAMPLE_PROV_SEC2_DEV_MODE
        /* This pop field represents the password that will be used to generate salt and verifier.
         * The field is present here in order to generate the QR code containing password.
         * In production this password field shall not be stored on the device */
        const char *username  = EXAMPLE_PROV_SEC2_USERNAME;
        const char *pop = EXAMPLE_PROV_SEC2_PWD;
#elif CONFIG_EXAMPLE_PROV_SEC2_PROD_MODE
        /* The username and password shall not be embedded in the firmware,
         * they should be provided to the user by other means.
         * e.g. QR code sticker */
        const char *username  = NULL;
        const char *pop = NULL;
#endif
        /* This is the structure for passing security parameters
         * for the protocomm security 2.
         * If dynamically allocated, sec2_params pointer and its content
         * must be valid till WIFI_PROV_END event is triggered.
         */
        network_prov_security2_params_t sec2_params = {};

        ESP_ERROR_CHECK(example_get_sec2_salt(&sec2_params.salt, &sec2_params.salt_len));
        ESP_ERROR_CHECK(example_get_sec2_verifier(&sec2_params.verifier, &sec2_params.verifier_len));

        network_prov_security2_params_t *sec_params = &sec2_params;
#endif
        /* What is the service key (could be NULL)
         * This translates to :
         *     - simply ignored when scheme is network_prov_scheme_ble
         */
        const char *service_key = NULL;

#ifdef CONFIG_EXAMPLE_PROV_TRANSPORT_BLE
        /* This step is only useful when scheme is network_prov_scheme_ble. This will
         * set a custom 128 bit UUID which will be included in the BLE advertisement
         * and will correspond to the primary GATT service that provides provisioning
         * endpoints as GATT characteristics. Each GATT characteristic will be
         * formed using the primary service UUID as base, with different auto assigned
         * 12th and 13th bytes (assume counting starts from 0th byte). The client side
         * applications must identify the endpoints by reading the User Characteristic
         * Description descriptor (0x2901) for each characteristic, which contains the
         * endpoint name of the characteristic */
        uint8_t custom_service_uuid[] = {
            /* LSB <---------------------------------------
             * ---------------------------------------> MSB */
            0xb4, 0xdf, 0x5a, 0x1c, 0x3f, 0x6b, 0xf4, 0xbf,
            0xea, 0x4a, 0x82, 0x03, 0x04, 0x90, 0x1a, 0x02,
        };

        /* If your build fails with linker errors at this point, then you may have
         * forgotten to enable the BT stack or BTDM BLE settings in the SDK (e.g. see
         * the sdkconfig.defaults in the example project) */
        network_prov_scheme_ble_set_service_uuid(custom_service_uuid);
#endif /* CONFIG_EXAMPLE_PROV_TRANSPORT_BLE */

        /* An optional endpoint that applications can create if they expect to
         * get some additional custom data during provisioning workflow.
         * The endpoint name can be anything of your choice.
         * This call must be made before starting the provisioning.
         */
        network_prov_mgr_endpoint_create("custom-data");

        /* Do not stop and de-init provisioning even after success,
         * so that we can restart it later. */
#ifdef CONFIG_EXAMPLE_REPROVISIONING
        network_prov_mgr_disable_auto_stop(1000);
#endif
        /* Start provisioning service */
        ESP_ERROR_CHECK(network_prov_mgr_start_provisioning(security, (const void *) sec_params, service_name, service_key));

        /* The handler for the optional endpoint created above.
         * This call must be made after starting the provisioning, and only if the endpoint
         * has already been created above.
         */
        network_prov_mgr_endpoint_register("custom-data", custom_prov_data_handler, NULL);

        /* Uncomment the following to wait for the provisioning to finish and then release
         * the resources of the manager. Since in this case de-initialization is triggered
         * by the default event loop handler, we don't need to call the following */
        // network_prov_mgr_wait();
        // network_prov_mgr_deinit();
        /* Print QR code for provisioning */
        network_prov_print_qr(service_name, username, pop, PROV_TRANSPORT_BLE);
    } else {
        ESP_LOGI(TAG, "Already provisioned, enabling netif and starting Thread");

        /* We don't need the manager as device is already provisioned,
         * so let's release it's resources */
        network_prov_mgr_deinit();

        otInstance *instance = esp_openthread_get_instance();
        (void)otIp6SetEnabled(instance, true);
        (void)otThreadSetEnabled(instance, true);
    }

    /* Wait for Thread connection */
    xEventGroupWaitBits(thread_event_group, THREAD_ATTACHED_EVENT, true, true, portMAX_DELAY);

    /* Start main application now */
#if CONFIG_EXAMPLE_REPROVISIONING
    while (1) {
        for (int i = 0; i < 10; i++) {
            ESP_LOGI(TAG, "Hello World!");
            vTaskDelay(1000 / portTICK_PERIOD_MS);
        }

        /* Resetting provisioning state machine to enable re-provisioning */
        network_prov_mgr_reset_thread_sm_state_for_reprovision();

        /* Wait for thread connection */
        xEventGroupWaitBits(thread_event_group, THREAD_ATTACHED_EVENT, true, true, portMAX_DELAY);
    }
#else
    while (1) {
        ESP_LOGI(TAG, "Hello World!");
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
#endif

}
