FILE(GLOB_RECURSE app_sources ${CMAKE_CURRENT_SOURCE_DIR}/upstream/src/*.cpp ${CMAKE_CURRENT_SOURCE_DIR}/upstream/src/src/extras/*.cpp)

idf_component_register(SRCS ${app_sources}
                      INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/upstream/src ${CMAKE_CURRENT_SOURCE_DIR}/upstream/src/src/extras
                      REQUIRES arduino-esp32 libsodium app_update nvs_flash)
component_compile_options(-Wno-error=format= -Wno-format)