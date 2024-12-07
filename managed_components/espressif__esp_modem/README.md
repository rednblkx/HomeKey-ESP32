# ESP MODEM

[![Component Registry](https://components.espressif.com/components/espressif/esp_modem/badge.svg)](https://components.espressif.com/components/espressif/esp_modem)

The `esp-modem` component is a managed component for `esp-idf` that is used for communication with GSM/LTE modems
that support AT commands and PPP protocol as a network interface.

## Examples

Get started with one of the examples:
* `examples/pppos_client` -- simple client implemented in C
* `examples/modem_console` -- C++ example implementing simple terminal console with DCE commands
* `examples/ap_to_pppos` -- this example focuses on the network connectivity of the esp-modem and provides a WiFi AP
  that forwards packets (and uses NAT) to and from the PPPoS connection.
* `examples/simple_cmux_client` -- this example sets up the CMUX mode to talk to the modem using AT commands
  with simultaneously maintaining active network connection.

## Documentation

* Continue with esp-modem [brief overview](https://github.com/espressif/esp-protocols/tree/master/docs/esp_modem/en/README.rst)
* View the full [html documentation](https://docs.espressif.com/projects/esp-protocols/esp_modem/docs/latest/index.html)
