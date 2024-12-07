idf_component_register(SRCS tls_transport.cpp
                       INCLUDE_DIRS include
                       REQUIRES tcp_transport
                       PRIV_REQUIRES mbedtls_cxx)
