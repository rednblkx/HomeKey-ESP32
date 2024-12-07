# from IDF version 5.0, we need to explicitly specify requirements
if("${IDF_VERSION_MAJOR}.${IDF_VERSION_MINOR}" VERSION_GREATER_EQUAL "5.0")
set(req esp_event esp_hw_support)
endif()

set(srcs "src/esp_diag_data_store.c")

if (CONFIG_DIAG_DATA_STORE_RTC)
list(APPEND srcs "src/rtc_store/rtc_store.c")
set(includes "src/rtc_store")
endif()

idf_component_register(SRCS "${srcs}"
                       INCLUDE_DIRS ${includes} "include"
                       PRIV_REQUIRES nvs_flash app_update
                       REQUIRES ${req})
