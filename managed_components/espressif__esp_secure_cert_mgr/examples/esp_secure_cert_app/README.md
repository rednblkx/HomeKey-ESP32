# ESP Secure Certificate Application

The sample app demonstrates the use of APIs from *esp_secure_cert_mgr* to retrieve the contents of the *esp_secure_cert* partition. The example can also be used to verify the validity of the contents from the *esp_secure_cert* partition.

## Requirements
* The device must be pre-provisioned and have an *esp_secure_cert* partition.

## How to use the example
Before project configuration and build, be sure to set the correct chip target using `idf.py set-target <chip_name>`.
### Configure the project

* The *esp_secure_cert* partition needs to be generated and flashed first with help of [configure_esp_secure_cert.py](https://github.com/espressif/esp_secure_cert_mgr/blob/main/tools/configure_esp_secure_cert.py) script. See [tools/README.md](https://github.com/espressif/esp_secure_cert_mgr/blob/main/tools/README.md) for more details.

* Please ensure that appropriate type of esp_secure_cert partition has been set in your projects `partitions.csv` file. Please refer the "esp_secure_cert partition" section in the [component README](https://github.com/espressif/esp_secure_cert_mgr#readme) for more details.

### Build and Flash

Build the project and flash it to the board, then run the monitor tool to view the serial output:

```
idf.py -p PORT flash monitor
```

(Replace PORT with the name of the serial port to use.)

(To exit the serial monitor, type ``Ctrl-]``.)

See the Getting Started Guide for full steps to configure and use ESP-IDF to build projects.

### Example Output
```
I (331) sample_app: Device Cert:
Length: 1233
-----BEGIN CERTIFICATE-----
.
.
-----END CERTIFICATE-----

I (441) sample_app: CA Cert:
Length: 1285
-----BEGIN CERTIFICATE-----
.
.
-----END CERTIFICATE-----

I (561) sample_app: Successfuly obtained ciphertext, ciphertext length is 1200
I (571) sample_app: Successfuly obtained initialization vector, iv length is 16
I (571) sample_app: RSA length is 2048
I (581) sample_app: Efuse key id 1
I (581) sample_app: Successfully obtained the ds context
I (831) sample_app: Ciphertext validated succcessfully
```

## Additional configurations for `pre_prov` partition
Few of the modules which were pre-provisioned initially had the name of the pre-provisioning partition as `pre_prov`. For the modules which have pre-provisioning partition of name `esp_secure_cert` this part can be ignored.

* For modules with `pre_prov` partition of type *cust_flash*, please update the line refering to `esp_secure_cert` partition in the partitions.csv with following: 
```
pre_prov,         0x3F,          ,    0xD000,     0x6000,
```
* No change is necessary for `pre_prov` partition of type *nvs*.
