set(srcs "src/work_queue.c" "src/factory.c" "src/time.c" "src/timezone.c" "src/utils.c"
         "src/cmd_resp.c" "src/console/rmaker_common_cmds.c" "src/console/rmaker_console.c")

set(priv_req mqtt nvs_flash console nvs_flash esp_wifi driver)
set(requires esp_event)

# esp_timer component was introduced in v4.2
if("${IDF_VERSION_MAJOR}.${IDF_VERSION_MINOR}" VERSION_GREATER "4.1")
    list(APPEND priv_req esp_timer)
endif()

#if(CONFIG_ESP_RMAKER_LIB_ESP_MQTT)
    list(APPEND srcs "src/esp-mqtt/esp-mqtt-glue.c")
#endif()
if(CONFIG_ESP_RMAKER_MQTT_SEND_USERNAME)
    list(APPEND srcs "src/create_APN3_PPI_string.c")
endif()

idf_component_register(SRCS ${srcs}
                       INCLUDE_DIRS "include"
                       PRIV_INCLUDE_DIRS
                       REQUIRES ${requires}
                       PRIV_REQUIRES ${priv_req})
