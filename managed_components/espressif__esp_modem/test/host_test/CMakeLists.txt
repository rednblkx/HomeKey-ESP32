cmake_minimum_required(VERSION 3.5)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)

set(EXTRA_COMPONENT_DIRS    # Add esp_modem component and linux port components
        ../..
        ../../port/linux)

set(COMPONENTS main)
project(host_modem_test)

idf_component_get_property(esp_modem esp_modem COMPONENT_LIB)
target_compile_definitions(${esp_modem} PRIVATE "-DCONFIG_COMPILER_CXX_EXCEPTIONS")
target_compile_definitions(${esp_modem} PRIVATE "-DCONFIG_IDF_TARGET_LINUX")
target_link_options(${esp_modem} INTERFACE -fsanitize=address -fsanitize=undefined)
