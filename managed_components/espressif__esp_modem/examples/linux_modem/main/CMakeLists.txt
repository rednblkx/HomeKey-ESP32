idf_component_register(SRCS "modem_main.cpp"
                       REQUIRES esp_modem)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
target_link_libraries(${COMPONENT_LIB}  PRIVATE Threads::Threads)

set_target_properties(${COMPONENT_LIB} PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS ON
)
target_compile_definitions(${COMPONENT_LIB} PRIVATE "-DCONFIG_IDF_TARGET_LINUX")
