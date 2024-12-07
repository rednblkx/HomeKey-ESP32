idf_component_register(SRCS "SIM7070_gnss.cpp"
                    INCLUDE_DIRS "."
                    PRIV_REQUIRES esp_modem)

set_target_properties(${COMPONENT_LIB} PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS ON
)
