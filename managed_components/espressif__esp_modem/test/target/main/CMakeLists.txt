idf_component_register(SRCS "pppd_test.cpp"
                            "NetworkDCE.cpp"
                       REQUIRES esp_modem catch2)

set_target_properties(${COMPONENT_LIB} PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS ON
)
