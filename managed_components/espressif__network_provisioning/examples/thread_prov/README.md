| Supported Targets | ESP32-C6 | ESP32-H2 |
| ----------------- | -------- | -------- |

# Network Provisioning Manager Example for Thread Provisioning

(See the README.md file in the upper level 'examples' directory for more information about examples.)

`thread_prov` example demonstrates the usage of `network_provisioning` manager component for building a Thread provisioning application.

For this example, Bluetooth LE is chosen as the mode of transport, over which the provisioning related communication is to take place. NimBLE has been configured as the host.

In the provisioning process the device is configured as a Thread FTD with specified dataset. Once configured, the device will retain the Thread configuration, until a flash erase is performed.

Right after the provisioning is complete, Bluetooth LE is turned off and disabled to free the memory used by the Bluetooth LE stack. Though, that is specific to this example, and the user can choose to keep Bluetooth LE stack intact in their own application.

`thread_prov` uses the following components :
* `network_provisioning` : Provides provisioning manager, data structures and protocomm endpoint handlers for Thread configuration
* `protocomm` : For protocol based communication and secure session establishment
* `protobuf` : Google's protocol buffer library for serialization of protocomm data structures
* `bt` : ESP-IDF's Bluetooth LE stack for transport of protobuf packets

This example can be used, as it is, for adding a provisioning service to any application intended for IoT.

> Note: If you use this example code in your own project, in Bluetooth LE mode, then remember to enable the BT stack and BTDM BLE control settings in your SDK configuration (e.g. by using the `sdkconfig.defaults` file from this project).

## Security Scheme

The `protocomm` component is used for establishing secure communication channel at the time of provisioning. It supports two security schemes for establishing secure communication which are [Security 1 Scheme](https://docs.espressif.com/projects/esp-idf/en/stable/esp32/api-reference/provisioning/provisioning.html#security-1-scheme) and [Security 2 Scheme](https://docs.espressif.com/projects/esp-idf/en/stable/esp32/api-reference/provisioning/provisioning.html#security-2-scheme). The example uses `Security 2 Scheme` (latest) by default.

## How to use example

### Hardware Required

Example should be able to run on any commonly available ESP32-H2/ESP32-C6 development board.

### Script Required

Currently, provisioning script is available for `Linux / Windows / macOS` platforms.

#### Platform : Linux / Windows / macOS

To install the dependency packages needed, please refer to the ESP-IDF examples [README file](https://github.com/espressif/esp-idf/blob/master/examples/README.md#running-test-python-script-ttfw).

`esp_prov` supports Bluetooth LE and SoftAP transport for Linux, MacOS and Windows platforms. For Bluetooth LE, however, if dependencies are not met, the script falls back to console mode and requires another application through which the communication can take place. The `esp_prov` console will guide you through the provisioning process of locating the correct Bluetooth LE GATT services and characteristics, the values to write, and input read values.

### Configure the project

```
idf.py menuconfig
```

### Build and Flash

Build the project and flash it to the board, then run monitor tool to view serial output:

```
idf.py -p PORT flash monitor
```

(To exit the serial monitor, type ``Ctrl-]``.)

See the Getting Started Guide for full steps to configure and use ESP-IDF to build projects.

## Example Output

```
I (445) app: Starting provisioning
I (1035) app: Provisioning started
I (1045) network_prov_mgr: Provisioning started with service name : PROV_F72E6B
```

Make sure to note down the Bluetooth LE device name (starting with `PROV_`) displayed in the serial monitor log (eg. PROV_F72E6B). This will depend on the MAC ID and will be unique for every device.

In a separate terminal run the `esp_prov.py` script under [directory](../../tool/esp_prov/) (make sure to replace `dataset_tlvs` with the dataset of the Thread network to which the device is supposed to connect to after provisioning). Assuming default example configuration, which uses the protocomm security version 2:

```
python esp_prov.py --transport ble --service_name PROV_F72E6B --sec_ver 2 --sec2_username threadprov --sec2_pwd abcd1234 --dataset_tlvs <dataset_tlvs>
```

For security scheme 1 with PoP-based (proof-of-possession) authentication, the following command can be used:
```
python esp_prov.py --transport ble --service_name PROV_F72E6B --sec_ver 1 --pop abcd1234 --dataset_tlvs <dataset_tlvs>
```

Above command will perform the provisioning steps, and the monitor log should display something like this :

```
I(125493) OPENTHREAD:[N] Mle-----------: Role disabled -> detached
I (125503) OT_STATE: netif up
I(125883) OPENTHREAD:[N] Mle-----------: Attach attempt 1, AnyPartition reattaching with Active Dataset
I(126793) OPENTHREAD:[N] Mle-----------: RLOC16 fffe -> a40d
I(126793) OPENTHREAD:[N] Mle-----------: Role detached -> child
I (126813) OT_STATE: Set dns server address: FDE6:8626:404B:2::808:808
I (126823) network_prov_mgr: Thread attached
I (126823) app: Provisioning successful
I (126823) app: Hello World!
I (127833) app: Hello World!
I (128833) app: Hello World!
.
.
.
I (131883) network_prov_mgr: Provisioning stopped
.
.
.
I (52355) app: Hello World!
I (53355) app: Hello World!
I (54355) app: Hello World!
I (55355) app: Hello World!
```

**Note:** For generating the credentials for security version 2 (`SRP6a` salt and verifier) for the device-side, the following example command can be used. The output can then directly be used in this example.

The config option `CONFIG_EXAMPLE_PROV_SEC2_DEV_MODE` should be enabled for the example and in `main/app_main.c`, the macro `EXAMPLE_PROV_SEC2_USERNAME` should be set to the same username used in the salt-verifier generation.

```log
$ python esp_prov.py --transport ble --sec_ver 2 --sec2_gen_cred --sec2_username threadprov --sec2_pwd abcd1234
==== Salt-verifier for security scheme 2 (SRP6a) ====
static const char sec2_salt[] = {
    0x1f, 0xff, 0x29, 0xf5, 0xc7, 0x7e, 0x07, 0x48, 0x02, 0xe9, 0x93, 0x3e, 0xa3, 0xa2, 0x26, 0x73
};

static const char sec2_verifier[] = {
    0xa7, 0x29, 0xe6, 0xa5, 0x4d, 0x20, 0x57, 0x71, 0x7c, 0x9d, 0x78, 0x2d, 0x0a, 0xb0, 0x9f, 0xec,
    0x7e, 0x8b, 0xab, 0xf5, 0xe6, 0xc3, 0x36, 0x41, 0x93, 0xfd, 0xb9, 0x49, 0x67, 0xe7, 0x7f, 0x79,
    0x66, 0x25, 0x2e, 0xac, 0x89, 0x19, 0xb2, 0xb3, 0x14, 0xb1, 0x16, 0xb0, 0xb0, 0xe4, 0x34, 0xd4,
    0x99, 0x40, 0x85, 0xa4, 0x99, 0x2b, 0x84, 0x21, 0xa1, 0xfb, 0x15, 0x48, 0x04, 0x91, 0xf5, 0x74,
    .
    .
    .
    0x80, 0x86, 0xf4, 0xd5, 0x08, 0xbc, 0xb0, 0xdd, 0x6b, 0x50, 0xfa, 0xdd, 0x16, 0x10, 0x23, 0x4b
};

```

### QR Code Scanning

Enabling `CONFIG_EXAMPLE_PROV_SHOW_QR` will display a QR code on the serial terminal, which can be scanned from the ESP Provisioning phone apps to start the Thread provisioning process. The ESP Provisioning phone apps will be released later.

The monitor log should display something like this :

```
I (673) app: Provisioning started
I (673) app: Scan this QR code from the provisioning application for Provisioning.
I (693) QRCODE: Encoding below text with ECC LVL 0 & QR Code Version 10
I (774) QRCODE: {"ver":"v1","name":"PROV_F72E6B","username":"threadprov","pop":"abcd1234","transport":"ble","network":"thread"}
>
  █▀▀▀▀▀█ ▄▄▄█ ██▄▀█▀▀▀▀ ▀▄ ▄█▀█▄▀  █▀▀▀▀▀█
  █ ███ █ ▄▄██ ▀▄▄▄▀▀▀▄▀▄▀▀▄▄▄▄▄█▀▄ █ ███ █
  █ ▀▀▀ █ ▄  █   █▄ ▀ ▄ ▄▀▄ █▄███▀  █ ▀▀▀ █
  ▀▀▀▀▀▀▀ ▀ ▀ ▀ ▀▄▀ ▀ ▀▄▀ █ █▄█ █▄▀ ▀▀▀▀▀▀▀
  ▀▀▀██▄▀██▄▀██▀▀▀ ▀▄▄█▄▀█ ▀▄▀▀▀█▄ █▄▀▄█ ▀▄
  ██▄ ▀ ▀  ▀▄▄█ ▄█▄ ▀  ██  ▀█▀█▀█ █▀ ▀█▄▀█▄
  ▄█▀▀▄▄▀█▄▀▀ ▀▄▄▄▄ ▀▀▄▄▀▄▀▀▀▀▄▀▄▄ ▄▄▄▄ ▀▀▄
  ▀ █▄▄ ▀  ▀▀█▀▀█ ▄ ▀█▄█▄ ▀▀▀▀▄▀█ ▄█▄ ▄▀▀▄▄
  ▀█▄█▀▀▀ ▄ ▀█▀██  █▄█▄▄▀▄▀▄▀ ▀  ▄▄█▄▀▄█▀▀▄
  █ ▀██ ▀▀▄▄▄▄██▀▀ █▄ ▄█▄▄▄▀█▀▄▀█ █▄█▄▄█ ▀
  ▀▀▄ ▄ ▀▄ ▄█ ▄ ▀█▀ ▄ ▀ ▀▄ ▀█▀▄▀▄██ ▄█▄█▀█▄
   █▄▄  ▀▀ ▄▀ ▄  █▀ ▄ ▀██▄█▀ ▀  █▄ ▄▀▄█  ▀▄
  ▄▄ ██▄▀▄█▀█▄▀▀ ▀ ▀ ▄█▄▀▄ ▀ █▄▄ ▄▄█▄█▄ ▀▀█
  █▄█ ▄▄▀▀ ▀  ▄ ▄▀▄ ▀▄ █▀  ▀ ▀█ ▄ ▀▀▄ ▄ ▀█
  █ ▄▀▀▀▀█▄▄  ▀ ▄█▄ ▀ ▄▄ ▄█▀▀  ▀▄▄█ ▄█▄ ▀ ▄
  █ █▀█ ▀█▄ ██▀▀  ▄ ██▄██▄█▀ █▄▀█▄██▄ ▄ ▀ ▄
  ▀ ▀▀  ▀ ▄ ▀█▀█▄ ██ █ ▄ █ █ ▄█ ▄▀█▀▀▀█ ▀▄█
  █▀▀▀▀▀█ ▀▄ ▄██▀▀██▄▄ ▄ ▄█▀▄▀▄  ██ ▀ █ ▀ ▄
  █ ███ █ █▀▀ ▄▄▀▄▀ ▄▀ ▄█▄▄█ ▀ ██ █▀▀▀▀███▄
  █ ▀▀▀ █ ███ ▄▀ █▀▀█ █▄ ▄█ ▀▀ ▀ ▀██▀ ▀  ▄
  ▀▀▀▀▀▀▀ ▀   ▀▀ ▀ ▀ ▀▀   ▀▀ ▀▀  ▀ ▀    ▀

I (1134) app: If QR code is not visible, copy paste the below URL in a browser.
https://espressif.github.io/esp-jumpstart/qrcode.html?data={"ver":"v1","name":"PROV_F72E6B","username":"threadprov","pop":"abcd1234","transport":"ble","network":"thread"}

```

### Thread Scanning

Provisioning manager also supports providing real-time Thread scan results (performed on the device) during provisioning. This allows the client side applications to choose the Thread network for which the device is to be configured. Various information about the visible Thread networks is available, like signal strength (RSSI) and link quality (LQI), etc. Also, the manager now provides capabilities information which can be used by client applications to determine availability of specific features (like `thread_scan`).

When using the scan based provisioning, we don't need to specify the `--dataset_tlvs` fields explicitly:

```
python esp_prov.py --transport ble --service_name PROV_F72E6B --pop abcd1234
```

See below the sample output from `esp_prov` tool on running above command:

```
Connecting...
Connected
Getting Services...
Security scheme determined to be : 1

==== Starting Session ====
==== Session Established ====

==== Scanning Thread Networks ====
++++ Scan process executed in 6.247516632080078 sec
++++ Scan results : 12

++++ Scan finished in 7.349079132080078 sec
==== Thread Scan results ====
S.N. PAN ID   EXT PAN ID         NAME               EXT ADDR           CHN  RSSI LQI
[ 1] 34121    9a6526ce2aaf4383   ST-34EW            e2848e0e9e315357   11   -53  9
[ 2] 14311    7e010e5a22beb040   OpenThread-37e7    02a2ab187a4fb728   21   -47  10
[ 3] 14311    7e010e5a22beb040   OpenThread-37e7    22bfc4ba63cf3bb8   21   -44  9
[ 4] 4660     dead00beef00cafe   OpenThread-13b4    1e4d2bb3a614163f   22   -40  10
[ 5] 48989    516f754f983e50c6   OpenThread-bf5d    be64e2845dc1d21a   22   -46  9
[ 6] 4660     dead00beef00cafe   OpenThread-79c0    3e19ed4f89be20ee   22   -53  10
[ 7] 31233    6c5105b3cb215393   qqqQQQQQQQQ        86ba2d8a2ded00d0   24   -47  8
[ 8] 32231    0458ef52172c21d6   OpenThread-7de7    deca5eddda6da2a7   25   -50  10
[ 9] 32231    0458ef52172c21d6   OpenThread-7de7    0e085d0f1d89db89   25   -45  10
[10] 10299    07f592f684bc4266   MyHome1926416771   b268d0525d81f5c4   25   -59  9
[11] 10299    07f592f684bc4266   MyHome1926416771   6ee0445b8d49d4b8   25   -56  9
[12] 51344    7899e5214acf64db   OpenThread-c890    b22f21bc5ce8a058   26   -40  10
Select Network by number (0 to rescan) : 4
Enter Thread network key string :

==== Sending Thread Dataset to Target ====
==== Thread Dataset sent successfully ====

==== Applying Thread Config to Target ====
==== Apply config sent successfully ====

==== Thread connection state  ====
==== Thread state: Attaching ====

==== Thread connection state  ====
==== Thread state: Attaching ====

==== Thread connection state  ====
==== Thread state: Attached ====
==== Provisioning was successful ====

```

### Interactive Provisioning

`esp_prov` supports interactive provisioning. You can trigger the script with a simplified command and input the necessary details
(`Proof-of-possession` for security scheme 1 and `SRP6a username`, `SRP6a password` for security scheme 2) as the provisioning process advances.

The command `python esp_prov.py --transport ble --sec_ver 2` gives out the following sample output:

```
Discovering...
==== BLE Discovery results ====
S.N. Name                              Address
.
.
.
[35] PROV_F73E7E                       60:55:F9:F7:3E:7E
[36] 62-37-56-08-AA-64                 62:37:56:08:AA:64
.
.
.
Select device by number (0 to rescan) : 35
Connecting...
Getting Services..
Security Scheme 2 - SRP6a Username required: threadprov
Security Scheme 2 - SRP6a Password required:

==== Starting Session ====
==== Session Established ====

==== Scanning Thread Networks ====
++++ Scan process executed in 6.247041702270508 sec
++++ Scan results : 14

++++ Scan finished in 7.40191650390625 sec
==== Thread Scan results ====
S.N. PAN ID   EXT PAN ID         NAME               EXT ADDR           CHN  RSSI LQI
[ 1] 34121    9a6526ce2aaf4383   ST-34EW            e2848e0e9e315357   11   -53  9
[ 2] 14311    7e010e5a22beb040   OpenThread-37e7    02a2ab187a4fb728   21   -45  9
[ 3] 14311    7e010e5a22beb040   OpenThread-37e7    22bfc4ba63cf3bb8   21   -40  10
[ 4] 4660     dead00beef00cafe   OpenThread-79c0    3e19ed4f89be20ee   22   -59  10
[ 5] 4660     dead00beef00cafe   OpenThread-13b4    1e4d2bb3a614163f   22   -43  9
[ 6] 48989    516f754f983e50c6   OpenThread-bf5d    be64e2845dc1d21a   22   -47  10
[ 7] 23427    5b83dead5b83beef   5b83               8a1e30a60615c16c   24   -57  8
[ 8] 31233    6c5105b3cb215393   qqqQQQQQQQQ        86ba2d8a2ded00d0   24   -48  10
[ 9] 10299    07f592f684bc4266   MyHome1926416771   6ee0445b8d49d4b8   25   -55  9
[10] 32231    0458ef52172c21d6   OpenThread-7de7    deca5eddda6da2a7   25   -50  10
[11] 10299    07f592f684bc4266   MyHome1926416771   b268d0525d81f5c4   25   -55  9
[12] 32231    0458ef52172c21d6   OpenThread-7de7    0e085d0f1d89db89   25   -47  10
[13] 10299    07f592f684bc4266   MyHome1926416771   7e06f10b32fe678f   25   -54  10
[14] 51344    7899e5214acf64db   OpenThread-c890    b22f21bc5ce8a058   26   -42  10
Select Network by number (0 to rescan) : 5
Enter Thread network key string :

==== Sending Thread Dataset to Target ====
==== Thread Dataset sent successfully ====

==== Applying Thread Config to Target ====
==== Apply config sent successfully ====

==== Thread connection state  ====
==== Thread state: Attaching ====

==== Thread connection state  ====
==== Thread state: Attaching ====

==== Thread connection state  ====
==== Thread state: Attached ====
==== Provisioning was successful ====
```

### Sending Custom Data

The provisioning manager allows applications to send some custom data during provisioning, which may be
required for some other operations like connecting to some cloud service. This is achieved by creating
and registering additional endpoints using the below APIs

```
network_prov_mgr_endpoint_create();
network_prov_mgr_endpoint_register();
```

In this particular example, we have added an endpoint named "custom-data" which can be tested
by passing the `--custom_data <MyCustomData>` option to the esp\_prov tool. Following output is
expected on success:

```
==== Sending Custom data to esp32 ====
CustomData response: SUCCESS
```

## Troubleshooting

### Provisioning failed

It is possible that the Thread dataset provided were incorrect, or the device was not able to establish connection to the network, in which the the `esp_prov` script will notify failure (with reason). Serial monitor log will display the failure along with disconnect reason :

```
E (367015) app: Provisioning failed!
    Reason : Thread network not found
    Please reset to factory and retry provisioning
```

Once dataset have been applied, even though wrong dataset were provided, the device will no longer go into provisioning mode on subsequent reboots until NVS is erased (see following section).

### Provisioning does not start

If the serial monitor log shows the following :

```
I (465) app: Already provisioned, enabling netif and starting Thread
```

it means either the device has been provisioned earlier with or without success (e.g. scenario covered in above section), or that the Thread dataset was already set by some other application flashed previously onto your device.

To fix this we simple need to erase the NVS partition from flash. First we need to find out its address and size. This can be seen from the monitor log on the top right after reboot.

```
I (47) boot: Partition Table:
I (50) boot: ## Label            Usage          Type ST Offset   Length
I (58) boot:  0 nvs              WiFi data        01 02 00009000 00006000
I (65) boot:  1 phy_init         RF data          01 01 0000f000 00001000
I (73) boot:  2 factory          factory app      00 00 00010000 00124f80
I (80) boot: End of partition table
```

Now erase NVS partition by running the following commands :

```
$IDF_PATH/components/esptool_py/esptool/esptool.py erase_region 0x9000 0x6000
```

### Bluetooth Pairing Request during provisioning

ESP-IDF now has functionality to enforce link encryption requirement while performing GATT write on characteristics of provisioning service. This will however result in a pairing pop-up dialog, if link is not encrypted. This feature is disabled by default. In order to enable this feature, please set `CONFIG_WIFI_PROV_BLE_FORCE_ENCRYPTION=y` in the sdkconfig or select the configuration using "idf.py menuconfig" .

```
Component Config --> Network Provisioning Manager --> Force Link Encryption during Characteristic Read/Write

```
Recompiling the application with above changes should suffice to enable this functionality.

