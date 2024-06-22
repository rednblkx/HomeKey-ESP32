// MQTT Broker Settings
#define MQTT_HOST "0.0.0.0" //IP adress of mqtt broker
#define MQTT_PORT 1883 //Port of mqtt broker
#define MQTT_CLIENTID "homekey_mqtt" //client-id to connect to mqtt broker
#define MQTT_USERNAME "username"  //username to connect to mqtt broker
#define MQTT_PASSWORD "password"  //password to connect to mqtt broker

//MQTT Flags
#define MQTT_CUSTOM_STATE_ENABLED 0 // Flag to enable the use of custom states and relevant MQTT Topics
#define MQTT_DISCOVERY true //Enable or disable discovery for home assistant tags functionality, set to true to enable.

// MQTT Topics
#define MQTT_CUSTOM_STATE_TOPIC "topic/homekey/custom_state" // MQTT Topic for publishing custom lock state
#define MQTT_CUSTOM_STATE_CTRL_TOPIC "topic/homekey/set_custom_state" // MQTT Control Topic with custom lock state
#define MQTT_AUTH_TOPIC "topic/homekey/auth" // MQTT Topic for publishing HomeKey authentication data or RFID UID
#define MQTT_SET_STATE_TOPIC "topic/homekey/set_state" // MQTT Control Topic for the HomeKit lock state (current and target)
#define MQTT_SET_TARGET_STATE_TOPIC "topic/homekey/set_target_state" // MQTT Control Topic for the HomeKit lock target state
#define MQTT_SET_CURRENT_STATE_TOPIC "topic/homekey/set_current_state" // MQTT Control Topic for the HomeKit lock current state
#define MQTT_STATE_TOPIC "topic/homekey/state" // MQTT Topic for publishing the HomeKit lock target state

//Miscellaneous
#define SETUP_CODE ""  //code used for homekit setup
#define OTA_PWD "homespan-ota" //custom password for ota
#define DEVICE_NAME "HK" //Device name
#define HOMEKEY_ALWAYS_UNLOCK 0 // Flag indicating if a successful Homekey authentication should always set and publish the unlock state
#define HOMEKEY_ALWAYS_LOCK 0  // Flag indicating if a successful Homekey authentication should always set and publish the lock state
#define HS_STATUS_LED 2 // HomeSpan Status LED GPIO pin
#define HS_PIN 255 // GPIO Pin for a Configuration Mode button (more info on https://github.com/HomeSpan/HomeSpan/blob/master/docs/UserGuide.md#device-configuration-mode)
#define NFC_NEOPIXEL_PIN 255 // GPIO Pin used for NeoPixel
#define NFC_SUCCESS_PIN 255 // GPIO Pin pulled HIGH or LOW (see NFC_SUCCESS_HL) on success HK Auth
#define NFC_SUCCESS_HL HIGH // Flag to define if NFC_SUCCESS_PIN should be held High or Low
#define NFC_SUCCESS_TIME 1000 // How long should NFC_SUCCESS_PIN be held High or Low
#define NFC_FAIL_PIN 255  // GPIO Pin pulled HIGH or LOW (see NFC_SUCCESS_HL) on failed HK Auth
#define NFC_FAIL_HL HIGH // Flag to define if NFC_FAIL_PIN should be held High or Low
#define NFC_FAIL_TIME 1000 // How long should NFC_FAIL_PIN be held High or Low
#define GPIO_ACTION_ENABLE false
#define GPIO_ACTION_PIN 255
#define GPIO_ACTION_LOCK_STATE LOW
#define GPIO_ACTION_UNLOCK_STATE HIGH