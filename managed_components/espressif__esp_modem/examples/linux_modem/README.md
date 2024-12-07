# Linux modem example

This is an experimental port of the esp_modem to linux.
It mocks some IDF functionality with `port/linux` layers (used for modem host test suite) and implements `esp_netif`,
which supports `tun` interface and uses lwIP `ppp` implementation to parse or wrap IP packets to be send/receive
over PPPoS, i.e. over the modem serial line.

## Configuration

* Set path to the lwip and lwip_contrib repositories as environmental variables:
  - `LWIP_PATH`: path to the lwip repository
  - `LWIP_CONTRIB_PATH`: path to the lwip_contrib repository
* Create a `tun` interface using `make_tun_netif` script.
* Set SIO dev name directly in the code: This is the serial port which is the modem connected to
* (Set the tun device na interface name in the code: Not needed if the device was created using the script above.)
* Build and run the example (no elevated privileges needed)
* Experiment with the network, after getting the IP from the modem device
    - directly in the code
    - in the system (need to set `tun` interface IP, dns servers, and routing the desired traffic over the tun interface)
