idf_component_register(SRCS "main.cpp" "app_events.cpp" "app_event_loop.cpp" "ConfigManager.cpp" "ReaderDataManager.cpp"
                    "HardwareManager.cpp" "HomeKitLock.cpp" "LockManager.cpp"
                    "MqttManager.cpp" "HKServices.cpp" "NfcManager.cpp" "Pn532Reader.cpp" "Pn7160Reader.cpp" "St25r3916Reader.cpp" "WebServerManager.cpp" "WebSocketLogSinker.cpp"
                    "ConsoleLogSinker.cpp" "GPIOAllocator.cpp" "EthernetDriver.cpp"
                    INCLUDE_DIRS "include"
                    PRIV_REQUIRES HomeSpan pn532_hal pn7160 DigitalDoorKey esp_https_server mqtt libsodium
                    msgpack-c json loggable loggable_espidf esp_wifi dns_server)

include(FetchContent)

FetchContent_Declare(
  fmt
  GIT_REPOSITORY https://github.com/fmtlib/fmt
  GIT_TAG        12.1.0)
FetchContent_MakeAvailable(fmt)
target_compile_options(fmt PUBLIC "-DFMT_THROW(x)=::fmt::assert_fail(__FILE__,__LINE__,\"fmt error\")")
target_compile_definitions(fmt PUBLIC "-DFMT_USE_LOCALE=0")
target_compile_definitions(fmt PRIVATE "-DFMT_OS=0")
target_link_libraries(${COMPONENT_LIB} PRIVATE fmt::fmt)
if(NOT DEFINED ENV{CI})
  add_custom_target(webui 
    WORKING_DIRECTORY ${COMPONENT_DIR}/../data 
    COMMAND bun install
    COMMAND bun run build 
    COMMAND sh -c "find dist \\( -name '*.css' -o -name '*.js' \\) -delete"
    VERBATIM
  )
  littlefs_create_partition_image(spiffs ../data/dist FLASH_IN_PROJECT DEPENDS webui)
else()
  littlefs_create_partition_image(spiffs ../data/dist FLASH_IN_PROJECT)
endif()
target_compile_definitions(${COMPONENT_LIB} PRIVATE "-DLOG_LOCAL_LEVEL=ESP_LOG_DEBUG")
