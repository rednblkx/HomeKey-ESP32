# ESP Insights
set(srcs "src/esp_insights.c"
        "src/esp_insights_transport.c"
        "src/esp_insights_client_data.c"
        "src/esp_insights_encoder.c"
        "src/esp_insights_cbor_decoder.c"
        "src/esp_insights_cbor_encoder.c")

set(priv_req cbor rmaker_common esptool_py espcoredump esp_diag_data_store nvs_flash)

# esp_timer component was introduced in v4.2
if("${IDF_VERSION_MAJOR}.${IDF_VERSION_MINOR}" VERSION_GREATER "4.1")
    list(APPEND priv_req esp_timer)
endif()

# esp_hw_support component was introduced in v4.3
if("${IDF_VERSION_MAJOR}.${IDF_VERSION_MINOR}" VERSION_GREATER "4.2")
    list(APPEND priv_req esp_hw_support)
endif()

# from IDF version 5.0, we need to explicitly specify requirements
if("${IDF_VERSION_MAJOR}.${IDF_VERSION_MINOR}" VERSION_GREATER_EQUAL "5.0")
    list(APPEND priv_req esp_wifi)
endif()

set(pub_req esp_diagnostics)

idf_component_register(SRCS ${srcs}
                       INCLUDE_DIRS "include"
                       PRIV_INCLUDE_DIRS "src"
                       REQUIRES ${pub_req}
                       PRIV_REQUIRES ${priv_req})

if(CONFIG_ESP_INSIGHTS_TRANSPORT_MQTT)
    target_add_binary_data(${COMPONENT_TARGET} "server_certs/mqtt_server.crt" TEXT)
    target_sources(${COMPONENT_LIB} PRIVATE "src/transport/esp_insights_mqtt.c")
else()
    target_add_binary_data(${COMPONENT_TARGET} "server_certs/https_server.crt" TEXT)
    idf_component_get_property(http_client_lib esp_http_client COMPONENT_LIB)
    target_link_libraries(${COMPONENT_LIB} PRIVATE ${http_client_lib})
    target_sources(${COMPONENT_LIB} PRIVATE "src/transport/esp_insights_https.c")
endif()

# Added just to automatically trigger re-runs of CMake
git_describe(ESP_INSIGHTS_VERSION ${COMPONENT_DIR})
message("ESP Insights Project commit: " ${ESP_INSIGHTS_VERSION})
