# ESP-Modem Testing

This folder contains automated and manual tests for esp-modem component. Beside these tests, some jobs are executed in CI to exercise standard examples (please refer to the CI definition and CI related sdkconfigs in examples).

List of test projects:

* `host_test` -- esp_modem is build on host (linux), modem's terminal in mocked using Loobpack class which creates simple responders to AT and CMUX mode. This test is executed in CI.
* `target`  -- test executed on target with no modem device, just a pppd running on the test runner. This test is executed in CI.
* `target_ota` -- Manual test which perform OTA over PPP.
* `target_iperf` -- Manual test to measure data throughput via PPP.

## Manual testing

Prior to every `esp_modem` release, these manual tests must be executed and pass
(IDF-9074 to move the manual tests to CI)

### `target_ota`

Make sure that the UART ISR is not in IRAM, so the error messages are expected in the log, but the ESP32 should recover and continue with downloading the image.

Perform the test for these devices
* SIM7600 (CMUX mode)
* BG96 (CMUX mode)
* SIM7000 (PPP mode)
* A7672 (CMUX mode -- the only device with 2 byte CMUX payload), so the test is expected to fail more often if (`CONFIG_ESP_MODEM_CMUX_DEFRAGMENT_PAYLOAD=y` && `CONFIG_ESP_MODEM_USE_INFLATABLE_BUFFER_IF_NEEDED=n` && dte_buffer < device max payload)
* NetworkDCE -- no modem device, pppd (PPP mode)

Perform the test with these configurations:
* CONFIG_TEST_USE_VFS_TERM (y/n)
* CONFIG_ESP_HTTP_CLIENT_ENABLE_CUSTOM_TRANSPORT (y/n)
* CONFIG_ESP_MODEM_CMUX_DEFRAGMENT_PAYLOAD (y/n)
* CONFIG_ESP_MODEM_USE_INFLATABLE_BUFFER_IF_NEEDED (y/n)

**Criteria for passing the test**

The test should complete the download with maximum 1 retry (50% of OTA failure)
In case of CMUX mode, we're trying to exit CMUX at the end of the test. This step might also fail for some devices, as the CMUX-exit is not supported on certain devices (when the final error message appears that the device failed to exit CMUX, we just verify the new image by reseting the ESP32)

### `target_iperf`

Run these 4 `iperf` configurations (either manually or using `pytest`):
* tcp_tx_throughput
* tcp_rx_throughput
* udp_tx_throughput
* udp_rx_throughput

And verify in all four cases the value is about 0.70 Mbps
