# This file contains the list of changes across different versions

## v2.5.0
* Assigned 2 project specific entries for Matter project in the list of TLV entries

## v2.4.1
* Added a new API `esp_secure_cert_get_tlv_info` for obtaining TLV information
* Added `esp_secure_cert_free_tlv_info` API for freeing TLV information.
* Added `esp_secure_cert_iterate_to_next_tlv` API for iterating the TLV entries
* Updated the API documentation for available `esp_secure_cert_get_*` APIs

## v2.4.0 (yanked)
* Added support for multiple entries of the same type by adding a new field called subtype.
* Fixed API for obtaining CA cert for the legacy flash formats (9b091ee)

### Yank explanation
This version was later yanked due to following reason.

* The API `esp_secure_cert_tlv_get_addr` which was made public in this version has incorrect documentation and the respective free API was not present.
* The changes in this version also modifiy the behaviour of existing APIs to obtain the TLV entry of latest subtype. While no current users shall be affected, this may cause inconsistency in the available API usage going forward.

Please note that the yanked version does not affect any of existing users. The yanking is done due to future API usage considerations and to avoid any possible inconsistencies.

## v2.3.1
* Make esp_secure_cert_get_key_type API available for DS peripheral case as well.

## v2.3.0
* Added support to obtain the priv key type
* Added support for getting the efuse key id for priv key

## v2.2.0
* tools: Support DER encoded private keys when creating secure cert partition on host
### Breaking changes in v2.2.0
* Updated the order of arguments for esp_pbkdf2_hmac_sha256 API to match it with corresponding mbedTLS API

## v2.1.0
* Added support for HMAC based ECDSA key derivation with PBKDF2-HMAC-SHA256
* Fixed build failure when example is setup through component manager

## v2.0.8
* Fix for supporting IDF v4.3

## v2.0.7
* Updated documentation regarding TLV format
* Fixed priv_key free API when HMAC based encryption scheme is enabled.

## v2.0.6
* Added HMAC based encryption scheme to protect private key data
* Added support for private key validation in the esp_secure_cert_app
* Added support of configurable esp_secure_cert partition offset in for configure_esp_secure_cert.py utility

## v2.0.5
* Fixed targets in Kconfig to reflect DS Peripheral compatibility

## v2.0.4
* Add implementation of `esp_secure_cert_free_*` APIs for TLV configuration.

## v2.0.3
* Added C linkage so that C++ code can find the definitions for secure cert APIs.
* Minor documentation fixes.

## v2.0.2
* Updated reference to the new esp_partition component (IDFv5.0)

## v2.0.1
* Added fixes for build failures with `-Wstrict-prototypes` CFLAG.
* Added fix for build failure with toolchain change in IDFv4.x and IDFv5.x

## v2.0.0
* Added esp-secure-cert-tool to PyPi.
* Restructure esp-secure-cert-tool
### Breaking changes in v2.0.0
* Added the support for TLV format for storing data in esp_secure_cert partition.
* Make the TLV `cust_flash_tlv` as the default flash format.
* Marked all the supported flash formats before TLV as legacy: `cust_flash`, `nvs`.
* esp_secure_cert_app: Updated the partition table for the example

## v1.0.3
* esp_secure_cert API now Dynamically identify the type of partitionand access the data accordingly
* esp_secure_cert_app: Enable support for target esp32
* Added tests based on qemu
* Added priv_key functionality to the configure_esp_secure_cert.py script.
### Breaking changes in v1.0.3
* Removed all the configuration options related to selecting the type of `esp_secure_cert` partition
* Remove `esp_secure_cert_get_*_addr` API, the contents can now be obtained through `esp_secure_cert_get_*` API.
* Remove APIs to obain the contents of the DS contexts e.g. efuse key id, ciphertext, iv etc. The contents can be accesed from inside the DS context which can be obtained through respective API.
* Breaking change in the `esp_secure_cert_get_*` API:
The API now accepts `char **buffer` instead of `char *buffer`. It will allocate the required memory dynamically and directly if necessary and provide the respective pointer.
