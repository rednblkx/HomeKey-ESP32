# Modem Power Saving Mode, sleep modes example


## Overview

This example demonstrates Power Saving Mode(PSM) in Sim70XX modems along with Deep sleep and Light Sleep modes in esp32.
This example enables Power saving mode in Sim70XX modem and tries to synchronise the sleep cycle of the modem with esp32.
When the modem wakes up from PSM sleep it wakes up the esp32. While the modem is awake esp32 goes to light sleep.

This example is tested on a SIM7080 modem.

## Supported devices

This example is supported only on ESP32, ESP32-S2 and ESP32-S3.

## Configuration

* EXAMPLE_MODEM_UART_TX_PIN: Connect this pin to the UART Tx pin of the modem.
* EXAMPLE_MODEM_UART_RX_PIN: Connect this pin to the UART Rx pin of the modem.
* EXAMPLE_MODEM_PWRKEY_PIN: Connect this pin to the PWRKEY pin of the modem.
* EXAMPLE_MODEM_STATUS_PIN: Connect this pin to the Status pin of the modem.

    The status pin might not be exposed on a modem. It needs to tapped out from the LED indicator marked as STS.

* EXAMPLE_MODEM_LIGHT_SLEEP_DURATION: Duration for which the esp32 will go into light sleep, while the modem is awake.

    If the esp32 goes into deep sleep while the modem is awake, esp32 will be awaken by the STATUS pin. So it's recommended to go to light sleep while the modem is awake.

* EXAMPLE_MODEM_T3412_PERIODIC_TAU: T3412 timer is the duration of one awake and sleep cycle of the modem in PSM.
* EXAMPLE_MODEM_T3324_ACTIVE_TIME: T3324 timer is the duration for which the modem stays PSM.

    The timeout information is coded in bit format for T3412 and T3324 timer.
