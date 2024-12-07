# SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Unlicense OR CC0-1.0
"""
Test case for iperf example.

This test case might have problem running on Windows:

- use `sudo killall iperf` to force kill iperf, didn't implement windows version

"""
from __future__ import division, unicode_literals

import logging
import os
import subprocess

import pytest
from common_test_methods import get_host_ip4_by_dest_ip
from idf_iperf_test_util import IperfUtility
from pytest_embedded import Dut

try:
    from typing import Any, Callable, Tuple
except ImportError:
    # Only used for type annotations
    pass

NO_BANDWIDTH_LIMIT = -1  # iperf send bandwidth is not limited


class IperfTestUtilityEth(IperfUtility.IperfTestUtility):
    """ iperf test implementation """
    def __init__(self, dut: str, config_name: str, pc_nic_ip: str, pc_iperf_log_file: str, test_result:Any=None) -> None:
        IperfUtility.IperfTestUtility.__init__(self, dut, config_name, 'None', 'None', pc_nic_ip, pc_iperf_log_file, test_result)

    def setup(self) -> Tuple[str,int]:
        """
        setup iperf test:

        1. kill current iperf process
        2. reboot DUT (currently iperf is not very robust, need to reboot DUT)
        """
        try:
            subprocess.check_output('sudo killall iperf 2>&1 > /dev/null', shell=True)
        except subprocess.CalledProcessError:
            pass
        self.dut.write('restart')
        dut_ip = self.dut.expect(r'pppd_test: IP          : (\d+\.\d+\.\d+\.\d+)').group(1)
        self.dut.expect("Type 'help' to get the list of commands.")
        self.dut.expect('iperf>')
        rssi = 0
        return dut_ip, rssi


@pytest.fixture
def log_performance(record_property: Callable[[str, object], None]) -> Callable[[str, str], None]:
    """
    log performance item with pre-defined format to the console
    and record it under the ``properties`` tag in the junit report if available.
    """

    def real_func(item: str, value: str) -> None:
        """
        :param item: performance item name
        :param value: performance value
        """
        logging.info('[Performance][%s]: %s', item, value)

    return real_func


def test_esp_pppd_iperf(
    dut: Dut,
    log_performance: Callable[[str, object], None],
    # check_performance: Callable[[str, float, str], None],
) -> None:
    """
    steps: |
      1. test TCP tx rx and UDP tx rx throughput
      2. compare with the pre-defined pass standard
    """

    # 1. preparing
    pc_iperf_log_file = os.path.join(dut.logdir, 'pc_iperf_log.md')
    # 2. wait for DUT
    dut_ip = dut.expect(r'pppd_test: IP          : (\d+\.\d+\.\d+\.\d+)').group(1)

    pc_nic_ip = get_host_ip4_by_dest_ip(dut_ip)
    test_result = {
        'tcp_tx': IperfUtility.TestResult('tcp', 'tx', 'ppp_client'),
        'tcp_rx': IperfUtility.TestResult('tcp', 'rx', 'ppp_client'),
        'udp_tx': IperfUtility.TestResult('udp', 'tx', 'ppp_client'),
        'udp_rx': IperfUtility.TestResult('udp', 'rx', 'ppp_client'),
    }
    test_utility = IperfTestUtilityEth(dut, 'ppp', pc_nic_ip, pc_iperf_log_file, test_result)

    # 3. run test for TCP Tx, Rx and UDP Tx, Rx
    test_utility.run_test('tcp', 'tx', 0, 5)
    test_utility.run_test('tcp', 'rx', 0, 5)
    test_utility.run_test('udp', 'tx', 0, 5)
    test_utility.run_test('udp', 'rx', 0, 5)

    # 4. print out performance details
    for throughput_type in test_result:
        print('{}_throughput'.format(throughput_type))
        print(test_result[throughput_type])
        print('---------------------------')

    # 5. log performance summary
    for throughput_type in test_result:
        log_performance('{}_throughput'.format(throughput_type),
                        '{:.02f} Mbps'.format(test_result[throughput_type].get_best_throughput()))
