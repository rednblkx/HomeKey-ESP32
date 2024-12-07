set(srcs "src/network_config.c"
        "src/network_scan.c"
        "src/network_ctrl.c"
        "src/manager.c"
        "src/handlers.c"
        "src/scheme_console.c"
        "proto-c/network_config.pb-c.c"
        "proto-c/network_scan.pb-c.c"
        "proto-c/network_ctrl.pb-c.c"
        "proto-c/network_constants.pb-c.c")

if(CONFIG_ESP_WIFI_ENABLED AND CONFIG_ESP_WIFI_SOFTAP_SUPPORT)
    list(APPEND srcs "src/scheme_softap.c")
endif()

if(CONFIG_SOC_BT_SUPPORTED AND CONFIG_BT_ENABLED)
    if(CONFIG_BT_BLUEDROID_ENABLED OR CONFIG_BT_NIMBLE_ENABLED)
        list(APPEND srcs
            "src/scheme_ble.c")
    endif()
endif()

idf_component_get_property(protocomm_dir protocomm COMPONENT_DIR)

idf_component_register(SRCS "${srcs}"
                    INCLUDE_DIRS include
                    PRIV_INCLUDE_DIRS src proto-c ${protocomm_dir}/proto-c
                    REQUIRES lwip protocomm
                    PRIV_REQUIRES protobuf-c bt json esp_timer esp_wifi openthread)
