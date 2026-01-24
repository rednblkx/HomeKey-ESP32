idf_component_register(SRCS "main.cpp" "ConfigManager.cpp" "ReaderDataManager.cpp" 
                    "HardwareManager.cpp" "HomeKitLock.cpp" "LockManager.cpp"
                    "MqttManager.cpp" "HKServices.cpp" "NfcManager.cpp" "WebServerManager.cpp" "WebSocketLogSinker.cpp"
                    "ConsoleLogSinker.cpp"
                    INCLUDE_DIRS "include"
                    PRIV_REQUIRES HomeSpan pn532_hal HK-HomeKit-Lib esp_http_server mqtt libsodium event_bus
                    msgpack-c json loggable loggable_espidf)

include(FetchContent)

FetchContent_Declare(
  fmt
  GIT_REPOSITORY https://github.com/fmtlib/fmt
  GIT_TAG        12.1.0)
FetchContent_MakeAvailable(fmt)
target_link_libraries(${COMPONENT_LIB} PUBLIC fmt::fmt-header-only)
if(NOT DEFINED ENV{CI})
  add_custom_target(webui 
    WORKING_DIRECTORY ${COMPONENT_DIR}/../data 
    COMMAND bun install
    COMMAND bun run build 
    COMMAND sh -c "find build \\( -name '*.html.gz' -o -name '*.css' -o -name '*.br' -o -name '*.js' -o -name '*.json.gz' \\) -delete"
    VERBATIM
  )
  littlefs_create_partition_image(spiffs ../data/build FLASH_IN_PROJECT DEPENDS webui)
else()
  littlefs_create_partition_image(spiffs ../data/build FLASH_IN_PROJECT)
endif()
target_compile_definitions(${COMPONENT_LIB} PUBLIC "-DLOG_LOCAL_LEVEL=ESP_LOG_VERBOSE")
