# SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Unlicense OR CC0-1.0
from __future__ import print_function, unicode_literals


def test_cmux_connection(dut):
    """
    steps:
      1. initializes connection with SIM800
      2. checks we get an IP
      3. checks for the MQTT events
    """
    # Get topic and data from Kconfig
    topic = ''
    data = ''
    try:
        topic = dut.app.sdkconfig.get('EXAMPLE_MQTT_TEST_TOPIC')
        data = dut.app.sdkconfig.get('EXAMPLE_MQTT_TEST_DATA')
    except Exception:
        print('ENV_TEST_FAILURE: Cannot find broker url in sdkconfig')
        raise
    # Check the sequence of connecting, publishing, disconnecting
    dut.expect('Modem has correctly entered multiplexed')
    # Check for MQTT connection and the data event
    dut.expect(f'TOPIC: {topic}')
    dut.expect(f'DATA: {data}')
