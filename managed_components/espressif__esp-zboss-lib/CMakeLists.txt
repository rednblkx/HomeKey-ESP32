cmake_minimum_required(VERSION 3.16)

if(CONFIG_ZB_ENABLED)
    set(include_dirs include)
endif()

idf_component_register(
    INCLUDE_DIRS "${include_dirs}"
)

if(CONFIG_ZB_ENABLED)

    set(ZBOSS_LIBS "")
    if(CONFIG_ZB_ZCZR)
        if(CONFIG_ZB_DEBUG_MODE)
            add_prebuilt_library(zboss_stack_lib "${CMAKE_CURRENT_SOURCE_DIR}/lib/${idf_target}/libzboss_stack.zczr.debug.a")
        else()
            add_prebuilt_library(zboss_stack_lib "${CMAKE_CURRENT_SOURCE_DIR}/lib/${idf_target}/libzboss_stack.zczr.a")
        endif()
    elseif(CONFIG_ZB_ZGPD)
        add_prebuilt_library(zboss_stack_lib "${CMAKE_CURRENT_SOURCE_DIR}/lib/${idf_target}/libzboss_stack.gpd.a")
    elseif(CONFIG_ZB_ZED)
        if(CONFIG_ZB_DEBUG_MODE)
            add_prebuilt_library(zboss_stack_lib "${CMAKE_CURRENT_SOURCE_DIR}/lib/${idf_target}/libzboss_stack.ed.debug.a")
        else()
            add_prebuilt_library(zboss_stack_lib "${CMAKE_CURRENT_SOURCE_DIR}/lib/${idf_target}/libzboss_stack.ed.a")
        endif()
    endif()

    if(CONFIG_ZB_DEBUG_MODE)
        add_prebuilt_library(zboss_port_lib "${CMAKE_CURRENT_SOURCE_DIR}/lib/${idf_target}/libzboss_port.a"
            PRIV_REQUIRES ieee802154 efuse esp_system esp_hw_support
            REQUIRES driver)
    else()
        add_prebuilt_library(zboss_port_lib "${CMAKE_CURRENT_SOURCE_DIR}/lib/${idf_target}/libzboss_port.debug.a"
            PRIV_REQUIRES ieee802154 efuse esp_system esp_hw_support
            REQUIRES driver)
    endif()

    list(APPEND ZBOSS_LIBS zboss_stack_lib zboss_port_lib)

    execute_process(
    COMMAND git rev-parse --short HEAD
    WORKING_DIRECTORY $ENV{IDF_PATH}
    OUTPUT_VARIABLE IDF_VERSION_FOR_ZIGBEE_PACKAGE OUTPUT_STRIP_TRAILING_WHITESPACE)

    execute_process(
    COMMAND git rev-parse --short HEAD
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    OUTPUT_VARIABLE ZIGBEE_SDK_BUILD_VERSION OUTPUT_STRIP_TRAILING_WHITESPACE)

    file(READ "${CMAKE_CURRENT_SOURCE_DIR}/idf_component.yml" component_yml_text)
    string(REGEX MATCH "\nversion: \"?([0-9]+.[0-9]+.[0-9]+)\"?" COMPONENT_VERSION_MATCH "\n${component_yml_text}")
    if(COMPONENT_VERSION_MATCH)
        set(ZBOSS_LIB_VERSION ${CMAKE_MATCH_1})
    else()
        message(WARNING "Cannot parse version of the ZLIB component")
    endif()

    string(TIMESTAMP ZB_BUILD_TIMESTAMP "%Y-%m-%d %H:%M:%S UTC" UTC)
    string(CONCAT RCP_FULL_VERSION_STRING
       "${IDF_VERSION_FOR_ZIGBEE_PACKAGE}-${ZIGBEE_SDK_BUILD_VERSION}-"
       "${ZBOSS_LIB_VERSION}-${ZB_BUILD_TIMESTAMP}")
    message(${RCP_FULL_VERSION_STRING})

    target_compile_options(${COMPONENT_LIB} INTERFACE $<$<COMPILE_LANGUAGE:C>:-Wno-strict-prototypes>)
    target_link_libraries(${COMPONENT_LIB} INTERFACE ${ZBOSS_LIBS})

    if (CONFIG_ZB_RADIO_SPINEL_UART)
        idf_component_get_property(openthread_lib openthread COMPONENT_LIB)
        idf_component_get_property(zigbee_lib espressif__esp-zigbee-lib COMPONENT_LIB)
        target_link_libraries(${COMPONENT_LIB} INTERFACE zboss_port_lib $<TARGET_FILE:${zigbee_lib}> $<TARGET_FILE:${openthread_lib}> zboss_port_lib)
    endif()

endif()
