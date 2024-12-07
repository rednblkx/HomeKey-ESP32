set(srcs "srcs/esp_secure_cert_tlv_read.c" "srcs/esp_secure_cert_crypto.c")

if(CONFIG_ESP_SECURE_CERT_SUPPORT_LEGACY_FORMATS)
    list(APPEND srcs "srcs/esp_secure_cert_read.c")
endif()

set(priv reqs "")
idf_build_get_property(build_components BUILD_COMPONENTS)
if(esp_partition IN_LIST build_components)
    list(APPEND priv_reqs esp_partition)
endif()

idf_component_register(SRCS ${srcs}
                    INCLUDE_DIRS "include"
                    PRIV_INCLUDE_DIRS "private_include"
                    REQUIRES spi_flash mbedtls nvs_flash efuse
                    PRIV_REQUIRES ${priv_reqs})
