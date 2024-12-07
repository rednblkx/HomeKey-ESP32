# Simple example of esp_modem component

(See the README.md file in the upper level 'examples' directory for more information about examples.)

## Overview

This example demonstrates the use of the [esp-modem component](https://components.espressif.com/component/espressif/esp_modem) to connect to a network and send some AT commands.
It uses modem CMUX mode so that commands and network could be used at the same time.

The example uses the following configuration options to demonstrate basic esp-modem capabilities:
* `EXAMPLE_NEED_SIM_PIN`: To unlock the SIM card with a PIN code if needed
* `EXAMPLE_PERFORM_OTA`: To start simple OTA at the end of the example to exercise basic CMUX/modem networking. Please note that the option `CONFIG_UART_ISR_IN_IRAM` is not enabled automatically, so that buffer overflows are expected and CMUX/PPP and networking should recover.
* `EXAMPLE_USE_VFS_TERM`: To demonstrate using an abstract file descriptor to talk to the device (instead of the UART driver directly). This option could be used when implementing a custom VFS driver.
