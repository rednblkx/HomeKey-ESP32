idf_component_register(SRCS "esp_err_to_name.c"
                        INCLUDE_DIRS include
                        REQUIRES esp_netif_linux esp_event_mock)
