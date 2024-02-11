#define MQTT_HOST "0.0.0.0" //IP adress of mqtt broker
#define MQTT_PORT 1883 //Port of mqtt broker
#define MQTT_CLIENTID "homekey_mqtt" //client-id to connect to mqtt broker
#define MQTT_USERNAME "username"  //username to connect to mqtt broker
#define MQTT_PASSWORD "password"  //password to connect to mqtt broker
#define MQTT_AUTH_TOPIC "topic/auth"
#define MQTT_SET_STATE_TOPIC "topic/set_state"
#define MQTT_SET_TARGET_STATE_TOPIC "topic/set_target_state"
#define MQTT_SET_CURRENT_STATE_TOPIC "topic/set_current_state"
#define MQTT_STATE_TOPIC "topic/state"
#define HK_CODE "46637726"  //code used for homekit setup
#define LED_PIN 2 //status led pin, built in is pin 2
#define OTA_PWD "homespan-ota" //custom password for ota
#define SSID "ssid" //SSID of your network
#define WIFI_PWD "password" //Password of your network
#define NAME "HK Lock" //Device name
#define DISCOVERY "1" //Enable or disable discovery for home assistant tags functionality, set to "1" to enable.