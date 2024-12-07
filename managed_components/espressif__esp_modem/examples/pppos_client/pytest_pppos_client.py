# SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Unlicense OR CC0-1.0
from __future__ import print_function, unicode_literals


def test_pppos_connect(dut):
    """
    steps:
      1. initializes connection with SIM800
      2. checks we get an IP
      3. checks for the MQTT events
      4. checks that the client cleanly disconnects
    """
    # Check the sequence of connecting, publishing, disconnecting
    dut.expect('Modem Connect to PPP Server', timeout=90)
    # Check for MQTT connection and the data event
    dut.expect('MQTT_EVENT_CONNECTED')
    dut.expect('MQTT_EVENT_DATA')
    dut.expect('TOPIC=/ci/esp-modem/pppos-client')
    dut.expect('DATA=esp32-pppos')
    # Check that we have disconnected
    dut.expect('User interrupted event')
    # And can use commands again
    dut.expect('IMSI=[0-9]+')
