# Custom transports

This component is a placeholder of custom transports. It contains mbedTLS cxx wrapper which could be used to create a custom TLS layer on any kind of IO function.

# List of Transports

## TLS Transport

TLS layer on top of any custom transport. Very similar to `ssl_transport` (standard IDF transport), but this is customizable and could work on any kind of transport, not only  the BSD socket based tcp transport (like `ssl_transport`).
