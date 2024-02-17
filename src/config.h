#define MQTT_HOST "0.0.0.0" //IP adress of mqtt broker
#define MQTT_PORT 1883 //Port of mqtt broker
#define MQTT_CLIENTID "homekey_mqtt" //client-id to connect to mqtt broker
#define MQTT_USERNAME "username"  //username to connect to mqtt broker
#define MQTT_PASSWORD "password"  //password to connect to mqtt broker
#define MQTT_AUTH_TOPIC "topic/auth" // MQTT Topic for publishing HomeKey authentication data or RFID UID
#define MQTT_SET_STATE_TOPIC "topic/set_state" // MQTT Control Topic for the HomeKit lock state (current and target)
#define MQTT_SET_TARGET_STATE_TOPIC "topic/set_target_state" // MQTT Control Topic for the HomeKit lock target state
#define MQTT_SET_CURRENT_STATE_TOPIC "topic/set_current_state" // MQTT Control Topic for the HomeKit lock current state
#define MQTT_STATE_TOPIC "topic/state" // MQTT Topic for publishing the HomeKit lock target state
#define HK_CODE "46637726"  //code used for homekit setup
#define LED_PIN 2 //status led pin, built in is pin 2
#define OTA_PWD "homespan-ota" //custom password for ota
#define NAME "HK Lock" //Device name
#define DISCOVERY "1" //Enable or disable discovery for home assistant tags functionality, set to "1" to enable.
#define CONTROL_PIN 26 // GPIO Pin for a Configuration Mode button (more info on https://github.com/HomeSpan/HomeSpan/blob/master/docs/UserGuide.md#device-configuration-mode)