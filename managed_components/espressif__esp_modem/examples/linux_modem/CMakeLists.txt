cmake_minimum_required(VERSION 3.5)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)

set(EXTRA_COMPONENT_DIRS ../../port/linux)

set(COMPONENTS main)
project(linux_modem)


idf_component_get_property(esp_modem esp_modem COMPONENT_LIB)
target_compile_definitions(${esp_modem} PRIVATE "-DCONFIG_COMPILER_CXX_EXCEPTIONS")
target_compile_definitions(${esp_modem} PRIVATE "-DCONFIG_IDF_TARGET_LINUX")
