cmake_minimum_required(VERSION 3.16)

if (CONFIG_ZB_ENABLED)
    set(src_dirs "")
    set(include_dirs include)
endif()

if(CONFIG_ZB_RADIO_SPINEL_UART)
    list(APPEND src_dirs src/radio_spinel)
    list(APPEND include_dirs include/radio_spinel)
endif()

idf_component_register(SRC_DIRS "${src_dirs}"
                       INCLUDE_DIRS "${include_dirs}"
                       REQUIRES driver vfs ieee802154 openthread espressif__esp-zboss-lib
)

if(CONFIG_ZB_ENABLED)

    set(ESP_ZIGBEE_API_LIBS "")

    if(CONFIG_ZB_ZCZR)
        if(CONFIG_ZB_DEBUG_MODE)
            add_prebuilt_library(esp_zigbee_api_lib "${CMAKE_CURRENT_SOURCE_DIR}/lib/${idf_target}/libesp_zb_api_zczr.debug.a" REQUIRES espressif__esp-zboss-lib)
        else()
            add_prebuilt_library(esp_zigbee_api_lib "${CMAKE_CURRENT_SOURCE_DIR}/lib/${idf_target}/libesp_zb_api_zczr.a" REQUIRES espressif__esp-zboss-lib)
        endif()
    elseif(CONFIG_ZB_ZED)
        if(CONFIG_ZB_DEBUG_MODE)
            add_prebuilt_library(esp_zigbee_api_lib "${CMAKE_CURRENT_SOURCE_DIR}/lib/${idf_target}/libesp_zb_api_ed.debug.a" REQUIRES espressif__esp-zboss-lib)
        else()
            add_prebuilt_library(esp_zigbee_api_lib "${CMAKE_CURRENT_SOURCE_DIR}/lib/${idf_target}/libesp_zb_api_ed.a" REQUIRES espressif__esp-zboss-lib)
        endif()
    elseif(CONFIG_ZB_ZGPD)
        add_prebuilt_library(esp_zigbee_api_lib "${CMAKE_CURRENT_SOURCE_DIR}/lib/${idf_target}/libesp_zb_api_gpd.a" REQUIRES espressif__esp-zboss-lib)
    endif()

    list(APPEND ESP_ZIGBEE_API_LIBS esp_zigbee_api_lib)

    target_link_libraries(${COMPONENT_LIB} INTERFACE ${ESP_ZIGBEE_API_LIBS})
    target_compile_options(${COMPONENT_LIB} INTERFACE $<$<COMPILE_LANGUAGE:C>:-Wno-strict-prototypes>)
endif()
