idf_component_register(SRCS "main.cpp" "ConfigManager.cpp" "ReaderDataManager.cpp" 
                    "HardwareManager.cpp" "HomeKitLock.cpp" "LockManager.cpp"
                    "MqttManager.cpp" "HKServices.cpp" "NfcManager.cpp" "WebServerManager.cpp" "WebSocketLogSinker.cpp"
                    INCLUDE_DIRS "include"
                    PRIV_REQUIRES HomeSpan PN532 HK-HomeKit-Lib esp_http_server mqtt libsodium
                    msgpack-c json format loggable)
if(NOT DEFINED ENV{CI})
  add_custom_target(webui 
    WORKING_DIRECTORY ${COMPONENT_DIR}/../data 
    COMMAND bun install
    COMMAND bun run build 
    COMMAND sh -c "find build -regex '.*\\.\\(html\\.gz\\|css\\|br\\|js\\|json\\.gz\\)' -delete"
    VERBATIM
  )
endif()
littlefs_create_partition_image(spiffs ../data/build FLASH_IN_PROJECT DEPENDS webui)
target_compile_definitions(${COMPONENT_LIB} PUBLIC "-DLOG_LOCAL_LEVEL=ESP_LOG_VERBOSE")
