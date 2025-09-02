idf_component_register(SRCS "main.cpp" "ConfigManager.cpp" "ReaderDataManager.cpp" 
                    "HardwareManager.cpp" "HomeKitLock.cpp" "LockManager.cpp"
                    "MqttManager.cpp" "HKServices.cpp" "NfcManager.cpp" "WebServerManager.cpp"
                    INCLUDE_DIRS "include"
                    PRIV_REQUIRES HomeSpan PN532 HK-HomeKit-Lib ESPAsyncWebServer mqtt libsodium
                    msgpack-c json)
littlefs_create_partition_image(spiffs ../data FLASH_IN_PROJECT)
