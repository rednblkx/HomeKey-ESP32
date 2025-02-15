// Copyright 2020 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

/********** STANDARD UI TYPES **********/

#define ESP_RMAKER_UI_TOGGLE            "esp.ui.toggle"
#define ESP_RMAKER_UI_SLIDER            "esp.ui.slider"
#define ESP_RMAKER_UI_DROPDOWN          "esp.ui.dropdown"
#define ESP_RMAKER_UI_TEXT              "esp.ui.text"
#define ESP_RMAKER_UI_HUE_SLIDER        "esp.ui.hue-slider"
#define ESP_RMAKER_UI_HUE_CIRCLE        "esp.ui.hue-circle"
#define ESP_RMAKER_UI_PUSHBUTTON        "esp.ui.push-btn-big"
#define ESP_RMAKER_UI_TRIGGER           "esp.ui.trigger"
#define ESP_RMAKER_UI_HIDDEN            "esp.ui.hidden"
#define ESP_RMAKER_UI_QR_SCAN           "esp.ui.qr-scan"

/********** STANDARD PARAM TYPES **********/

#define ESP_RMAKER_PARAM_NAME           "esp.param.name"
#define ESP_RMAKER_PARAM_POWER          "esp.param.power"
#define ESP_RMAKER_PARAM_BRIGHTNESS     "esp.param.brightness"
#define ESP_RMAKER_PARAM_HUE            "esp.param.hue"
#define ESP_RMAKER_PARAM_SATURATION     "esp.param.saturation"
#define ESP_RMAKER_PARAM_INTENSITY      "esp.param.intensity"
#define ESP_RMAKER_PARAM_CCT            "esp.param.cct"
#define ESP_RMAKER_PARAM_SPEED          "esp.param.speed"
#define ESP_RMAKER_PARAM_DIRECTION      "esp.param.direction"
#define ESP_RMAKER_PARAM_TEMPERATURE    "esp.param.temperature"
#define ESP_RMAKER_PARAM_OTA_STATUS     "esp.param.ota_status"
#define ESP_RMAKER_PARAM_OTA_INFO       "esp.param.ota_info"
#define ESP_RMAKER_PARAM_OTA_URL        "esp.param.ota_url"
#define ESP_RMAKER_PARAM_TIMEZONE       "esp.param.tz"
#define ESP_RMAKER_PARAM_TIMEZONE_POSIX       "esp.param.tz_posix"
#define ESP_RMAKER_PARAM_SCHEDULES      "esp.param.schedules"
#define ESP_RMAKER_PARAM_SCENES         "esp.param.scenes"
#define ESP_RMAKER_PARAM_REBOOT         "esp.param.reboot"
#define ESP_RMAKER_PARAM_FACTORY_RESET  "esp.param.factory-reset"
#define ESP_RMAKER_PARAM_WIFI_RESET     "esp.param.wifi-reset"
#define ESP_RMAKER_PARAM_LOCAL_CONTROL_POP      "esp.param.local_control_pop"
#define ESP_RMAKER_PARAM_LOCAL_CONTROL_TYPE     "esp.param.local_control_type"
#define ESP_RMAKER_PARAM_TOGGLE         "esp.param.toggle"
#define ESP_RMAKER_PARAM_RANGE          "esp.param.range"
#define ESP_RMAKER_PARAM_MODE           "esp.param.mode"
#define ESP_RMAKER_PARAM_BLINDS_POSITION     "esp.param.blinds-position"
#define ESP_RMAKER_PARAM_GARAGE_POSITION     "esp.param.garage-position"
#define ESP_RMAKER_PARAM_LIGHT_MODE     "esp.param.light-mode"
#define ESP_RMAKER_PARAM_AC_MODE        "esp.param.ac-mode"
#define ESP_RMAKER_PARAM_ADD_ZIGBEE_DEVICE     "esp.param.add_zigbee_device"


/********** STANDARD DEVICE TYPES **********/

#define ESP_RMAKER_DEVICE_SWITCH        "esp.device.switch"
#define ESP_RMAKER_DEVICE_LIGHTBULB     "esp.device.lightbulb"
#define ESP_RMAKER_DEVICE_FAN           "esp.device.fan"
#define ESP_RMAKER_DEVICE_TEMP_SENSOR   "esp.device.temperature-sensor"
#define ESP_RMAKER_DEVICE_LIGHT         "esp.device.light"
#define ESP_RMAKER_DEVICE_OUTLET        "esp.device.outlet"
#define ESP_RMAKER_DEVICE_PLUG          "esp.device.plug"
#define ESP_RMAKER_DEVICE_SOCKET        "esp.device.socket"
#define ESP_RMAKER_DEVICE_LOCK          "esp.device.lock"
#define ESP_RMAKER_DEVICE_BLINDS_INTERNAL   "esp.device.blinds-internal"
#define ESP_RMAKER_DEVICE_BLINDS_EXTERNAL   "esp.device.blinds-external"
#define ESP_RMAKER_DEVICE_GARAGE_DOOR   "esp.device.garage-door"
#define ESP_RMAKER_DEVICE_GARAGE_LOCK   "esp.device.garage-door-lock"
#define ESP_RMAKER_DEVICE_SPEAKER       "esp.device.speaker"
#define ESP_RMAKER_DEVICE_AIR_CONDITIONER   "esp.device.air-conditioner"
#define ESP_RMAKER_DEVICE_THERMOSTAT    "esp.device.thermostat"
#define ESP_RMAKER_DEVICE_TV            "esp.device.tv"
#define ESP_RMAKER_DEVICE_WASHER        "esp.device.washer"
#define ESP_RMAKER_DEVICE_OTHER         "esp.device.other"
#define ESP_RMAKER_DEVICE_ZIGBEE_GATEWAY   "esp.device.zigbee_gateway"
#define ESP_RMAKER_DEVICE_THREAD_BR        "esp.device.thread-br"

/********** STANDARD SERVICE TYPES **********/
#define ESP_RMAKER_SERVICE_OTA          "esp.service.ota"
#define ESP_RMAKER_SERVICE_TIME         "esp.service.time"
#define ESP_RMAKER_SERVICE_SCHEDULE     "esp.service.schedule"
#define ESP_RMAKER_SERVICE_SCENES       "esp.service.scenes"
#define ESP_RMAKER_SERVICE_SYSTEM       "esp.service.system"
#define ESP_RMAKER_SERVICE_LOCAL_CONTROL    "esp.service.local_control"

#ifdef __cplusplus
}
#endif
