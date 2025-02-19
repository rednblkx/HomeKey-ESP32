idf_component_register(SRCS "main.cpp"
                    INCLUDE_DIRS "include"
                    REQUIRES HomeSpan PN532 HK-HomeKit-Lib ESPAsyncWebServer mqtt libsodium)
littlefs_create_partition_image(spiffs ../data FLASH_IN_PROJECT)