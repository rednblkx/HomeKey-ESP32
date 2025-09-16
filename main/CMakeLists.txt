idf_component_register(SRCS "main.cpp" "ConfigManager.cpp" "ReaderDataManager.cpp" 
                    "HardwareManager.cpp" "HomeKitLock.cpp" "LockManager.cpp"
                    "MqttManager.cpp" "HKServices.cpp" "NfcManager.cpp" "WebServerManager.cpp"
                    INCLUDE_DIRS "include"
                    PRIV_REQUIRES HomeSpan PN532 HK-HomeKit-Lib esp_http_server mqtt libsodium
                    msgpack-c json format)
add_custom_target(webui WORKING_DIRECTORY ${COMPONENT_DIR}/../data COMMAND npm run build VERBATIM)
littlefs_create_partition_image(spiffs ../data/dist FLASH_IN_PROJECT DEPENDS webui)
