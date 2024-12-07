# AP to PPPoS example

## Overview

This example focuses on the networking part, enables forwarding packets between network interfaces. It creates a WiFi soft AP, which uses NAT to forward packets to and from the PPP network
interface.

### Specific network DCE

Note, that this example could use of a minimal Network DCE, defined in the `network_dce.cpp` file.
Please set `EXAMPLE_USE_MINIMAL_DCE` to `y` in order to demonstrate this functionality.

By default, this example simply connects to the PPP server using a supported device with C-API modem interface.

This example however, doesn't rely on sending specific AT commands, just the bare minimum to setup the cellular network.
Thus, if the `EXAMPLE_USE_MINIMAL_DCE` option is enabled, we directly inherit from the `ModuleIf` and implement only the basic commands.
Also, to demonstrate the dce_factory functionality, a new `NetDCE_Factory` is implemented for creating the network module and the DCE.
