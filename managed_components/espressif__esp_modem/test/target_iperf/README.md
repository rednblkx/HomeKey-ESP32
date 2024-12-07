# Target test for measuring TCP/UDP network performance

## Overview

The aim of this test is to run `iperf` application to measure network throughput of the esp_modem library.

### Configure PPP server

This test uses a network DCE device, which only needs a PPP server. To run the PPP server, use this command:
```
sudo pppd /dev/ttyUSB1 115200 192.168.11.1:192.168.11.2 ms-dns 8.8.8.8 modem local noauth debug nocrtscts nodetach +ipv6
```

### Running using pytest

For checking the performance, you only need to execute the pytest, which will measure the network throughput automatically and report the resultant values. For running the pytest, you need to:
* install IDF pytest packages by running: `./install.sh --enable-pytest`
* add IDF internal packages to python path: `export PYTHONPATH=${DIF_PATH}/tools/ci/python_packages/`
* run the pytest as **superuser**

It's useful to note that when running the test multiple times, you can use `pytest --skip-autoflash y` so the pytest wouldn't have to always reprogram the DUT.

### Performance summary

Here's an example of the resultant summary logged by the pytest
```
2023-11-29 18:28:25 INFO [Performance][tcp_tx_throughput]: 0.75 Mbps
2023-11-29 18:28:25 INFO [Performance][tcp_rx_throughput]: 0.70 Mbps
2023-11-29 18:28:25 INFO [Performance][udp_tx_throughput]: 0.73 Mbps
2023-11-29 18:28:25 INFO [Performance][udp_rx_throughput]: 0.70 Mbps
```

### Running the iperf manually

Execute `idf.py flash monitor` in one terminal and after connecting to the PPP server (after getting an IP address), you can use standard `iperf` commands.
In another terminal, you need to execute the iperf counterpart.
For example running for checking UDP performance, and running server on ESP32, please run:
* iperf -u -s -i 3 (in ESP32 terminal)
* iperf -u -c SERVER_IP -t 60 -i 3 (on the host side)

Note that command `pppd info` will print actual IP addresses:
```
iperf> pppd info
ppp:
  IP: 192.168.11.2
  MASK: 255.255.255.255
  GW: 192.168.11.3
```
