#include <pins_arduino.h>

// NFC Module Pins
#define SS_PIN SS // Slave Select Pin for SPI communication
#define MISO_PIN MISO // Master In, Slave Out Pin for SPI communication
#define MOSI_PIN MOSI // Master Out, Slave In Pin for SPI communication
#define SCK_PIN SCK // Serial Clock Pin for SPI communication

// MQTT Broker Settings
#define MQTT_HOST "" //IP adress of mqtt broker
#define MQTT_PORT 1883 //Port of mqtt broker
#define MQTT_CLIENTID "" //client-id to connect to mqtt broker
#define MQTT_USERNAME ""  //username to connect to mqtt broker
#define MQTT_PASSWORD ""  //password to connect to mqtt broker

//MQTT Flags
#define MQTT_CUSTOM_STATE_ENABLED 0 // Flag to enable the use of custom states and relevant MQTT Topics
#define MQTT_DISCOVERY true //Enable or disable discovery for home assistant tags functionality, set to true to enable.
#define MQTT_NO_TAG_PUBLISH false // Disable publishing of tags to MQTT topic

//MQTT SSL/TLS Settings
#define MQTT_USE_SSL false // Enable/disable SSL/TLS for MQTT connection
#define MQTT_CA_CERT "" // CA certificate for server validation (PEM format)
#define MQTT_CLIENT_CERT "" // Client certificate for mutual authentication (PEM format)
#define MQTT_CLIENT_KEY "" // Client private key for mutual authentication (PEM format)
#define MQTT_ALLOW_INSECURE false // Allow connections without proper certificate validation (INSECURE - use only for testing)

// MQTT Topics
#define MQTT_LWT_TOPIC "status"
#define MQTT_CUSTOM_STATE_TOPIC "homekit/custom_state" // MQTT Topic for publishing custom lock state
#define MQTT_CUSTOM_STATE_CTRL_TOPIC "homekit/set_custom_state" // MQTT Control Topic with custom lock state
#define MQTT_AUTH_TOPIC "homekey/auth" // MQTT Topic for publishing HomeKey authentication data or RFID UID
#define MQTT_SET_STATE_TOPIC "homekit/set_state" // MQTT Control Topic for the HomeKit lock state (current and target)
#define MQTT_SET_TARGET_STATE_TOPIC "homekit/set_target_state" // MQTT Control Topic for the HomeKit lock target state
#define MQTT_SET_CURRENT_STATE_TOPIC "homekit/set_current_state" // MQTT Control Topic for the HomeKit lock current state
#define MQTT_STATE_TOPIC "homekit/state" // MQTT Topic for publishing the HomeKit lock target state
#define MQTT_PROX_BAT_TOPIC "homekit/set_battery_lvl" // MQTT Control Topic for setting the battery level to be shown in HomeKit
#define MQTT_HK_ALT_ACTION_TOPIC "alt_action" // MQTT Topic for publishing the Alt Action

// MQTT Custom state
#define C_UNLOCK 0
#define C_LOCK 1
#define C_LOCKED 1
#define C_UNLOCKING 2
#define C_UNLOCKED 3
#define C_LOCKING 4
#define C_JAMMED 254
#define C_UNKNOWN 25

// Miscellaneous
#define HOMEKEY_COLOR TAN
#define SETUP_CODE "46637726"  // HomeKit Setup Code (only for reference, has to be changed during WiFi Configuration or from WebUI)
#define OTA_PWD "homespan-ota" //custom password for ota
#define DEVICE_NAME "HK" //Device name
#define HOMEKEY_ALWAYS_UNLOCK 0 // Flag indicating if a successful Homekey authentication should always set and publish the unlock state
#define HOMEKEY_ALWAYS_LOCK 0  // Flag indicating if a successful Homekey authentication should always set and publish the lock state
#define HK_AUTH_PRECOMPUTE_ENABLED true // Enable HomeKey auth precompute cache (faster taps, higher CPU/RAM)
#define HS_STATUS_LED 255 // HomeSpan Status LED GPIO pin
#define HS_PIN 255 // GPIO Pin for a Configuration Mode button (more info on https://github.com/HomeSpan/HomeSpan/blob/master/docs/UserGuide.md#device-configuration-mode)
#define BTR_PROX_BAT_ENABLED false // Enable or disable battery monitoring
#define BTR_PROX_BAT_LOW_THRESHOLD 10 // Battery low status threshold percentage

// Ethernet Settings
#define ETH_ENABLED false // Enable or disable Ethernet connectivity
// See https://github.com/rednblkx/HomeKey-ESP32/blob/main/main/include/eth_structs.hpp for supported chips and presets
#define ETH_ACTIVE_PRESET 255 // Ethernet preset index (255 for custom pins)
#define ETH_PHY_TYPE 1 // Ethernet PHY type identifier
// Ethernet RMII Configuration
#define ETH_RMII_CONF_PHY_ADDR 0 // PHY Address
#define ETH_RMII_CONF_MDC_PIN 255 // Management Data Clock Pin
#define ETH_RMII_CONF_MDIO_PIN 255 // Management Data Input/Output Pin
#define ETH_RMII_CONF_POWER_PIN 255 // Power Pin
#define ETH_RMII_CONF_RMII_CLOCK_MODE 0 // RMII Clock Mode
// Ethernet SPI Configuration
#define ETH_SPI_CONF_SPI_FREQ_MHZ 20 // SPI Frequency
#define ETH_SPI_CONF_PIN_CS 255 // Chip Select Pin
#define ETH_SPI_CONF_PIN_IRQ 255 // Interrupt Request Pin
#define ETH_SPI_CONF_PIN_RST 255 // Reset Pin
#define ETH_SPI_CONF_PIN_SCK 255 // Serial Clock Pin
#define ETH_SPI_CONF_PIN_MISO 255 // Master In, Slave Out Pin
#define ETH_SPI_CONF_PIN_MOSI 255 // Master Out, Slave In Pin

// Actions
#define NFC_NEOPIXEL_PIN 255 // GPIO Pin used for NeoPixel
// 0="RGB", 1="RBG", 2="BRG", 3="BGR", 4="GBR", 5="GRB"
#define NEOPIXEL_TYPE 5 // NeoPixel Type
#define NEOPIXEL_SUCCESS_R 0 // Color value for Red - Success HK Auth
#define NEOPIXEL_SUCCESS_G 255 // Color value for Green - Success HK Auth
#define NEOPIXEL_SUCCESS_B 0 // Color value for Blue - Success HK Auth
#define NEOPIXEL_FAIL_R 255 // Color value for Red - Fail HK Auth
#define NEOPIXEL_FAIL_G 0 // Color value for Green - Fail HK Auth
#define NEOPIXEL_FAIL_B 0 // Color value for Blue - Fail HK Auth
#define NEOPIXEL_TAG_EVENT_R 0 // Color value for Red - Tag Event
#define NEOPIXEL_TAG_EVENT_G 0 // Color value for Green - Tag Event
#define NEOPIXEL_TAG_EVENT_B 0 // Color value for Blue - Tag Event
#define NEOPIXEL_TAG_EVENT_TIME 1000 // GPIO Delay time in ms - Tag Event
#define NEOPIXEL_SUCCESS_TIME 1000 // GPIO Delay time in ms - Success HK Auth
#define NEOPIXEL_FAIL_TIME 1000 // GPIO Delay time in ms - Success HK Auth
#define NFC_SUCCESS_PIN 255 // GPIO Pin pulled HIGH or LOW (see NFC_SUCCESS_HL) on success HK Auth
#define NFC_SUCCESS_HL 1 // Flag to define if NFC_SUCCESS_PIN should be held High or Low
#define NFC_SUCCESS_TIME 1000 // How long should NFC_SUCCESS_PIN be held High or Low
#define NFC_FAIL_PIN 255  // GPIO Pin pulled HIGH or LOW (see NFC_SUCCESS_HL) on failed HK Auth
#define NFC_FAIL_HL 1 // Flag to define if NFC_FAIL_PIN should be held High or Low
#define NFC_FAIL_TIME 1000 // How long should NFC_FAIL_PIN be held High or Low
#define TAG_EVENT_PIN 255 // GPIO Pin pulled HIGH or LOW (see TAG_EVENT_HL) on tag event
#define TAG_EVENT_TIMEOUT 1000 // How long should TAG_EVENT_PIN be held High or Low
#define TAG_EVENT_HL 1  // Flag to define if TAG_EVENT_PIN should be held High or Low
#define GPIO_ACTION_PIN 255 // GPIO Pin pulled HIGH or LOW (see GPIO_ACTION_HL) on action
#define GPIO_ACTION_LOCK_STATE 0 // GPIO Pin pulled HIGH or LOW (see GPIO_ACTION_HL) on action
#define GPIO_ACTION_UNLOCK_STATE 1 // GPIO Pin pulled HIGH or LOW (see GPIO_ACTION_HL) on action
#define GPIO_ACTION_MOMENTARY_STATE static_cast<uint8_t>(gpioMomentaryStateStatus::M_DISABLED)
#define GPIO_ACTION_MOMENTARY_TIMEOUT 5000 // GPIO Delay time in ms - Action
#define GPIO_HK_ALT_ACTION_INIT_PIN 255 // GPIO Pin pulled HIGH or LOW (see GPIO_HK_ALT_ACTION_INIT_HL) on action
#define GPIO_HK_ALT_ACTION_INIT_TIMEOUT 5000 // GPIO Delay time in ms - Action
#define GPIO_HK_ALT_ACTION_INIT_LED_PIN 255 // GPIO Pin pulled HIGH or LOW (see GPIO_HK_ALT_ACTION_INIT_HL) on action
#define GPIO_HK_ALT_ACTION_PIN 255 // GPIO Pin pulled HIGH or LOW (see GPIO_HK_ALT_ACTION_HL) on action
#define GPIO_HK_ALT_ACTION_TIMEOUT 5000 // GPIO Delay time in ms - Action
#define GPIO_HK_ALT_ACTION_GPIO_STATE 1 // GPIO Pin pulled HIGH or LOW (see GPIO_HK_ALT_ACTION_HL) on action
#define GPIO_HK_CONTROLLED_STATE false // Defines if HomeKey can control the GPIO state
#define HK_DUMB_SWITCH_MODE true // Bypass lock state external validation

// WebUI
#define WEB_AUTH_ENABLED false
#define WEB_AUTH_USERNAME "admin"
#define WEB_AUTH_PASSWORD "password"
