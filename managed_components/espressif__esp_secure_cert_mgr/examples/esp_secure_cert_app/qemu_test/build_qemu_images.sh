#!/bin/bash
ESP_SECURE_CERT_APP=$PWD/../

if [ -z "$1" ]; then
    echo "Usage: build_qemu_image.sh image_type
          image type can be tlv, legacy_format"
    exit 1
fi

image_type=$1

# This option causes the script to fail if any command fails in the main body of the script
# Note: this does not apply to the internal functions
# We need to check the exit status of each command individually
set -e

save_defaults() {
    cp sdkconfig.defaults sdkconfig_defaults_clean
}

restore_defaults() {
    mv sdkconfig_defaults_clean sdkconfig.defaults
}

clean() {
    rm -rf sdkconfig 2>&1
    rm -rf build 2>&1
}

idf_path_verify() {
    if [[ -z "$IDF_PATH" ]]; then
        echo "IDF_PATH not set. Please set IDF_PATH "
        exit 1
    else
        echo '--------####--------'
        echo 'IDF_PATH & Branch'
        echo $IDF_PATH;(cd $IDF_PATH;git branch --show-current)
        echo '--------####--------'
    fi
}

build_tlv_image() {
    cd $ESP_SECURE_CERT_APP
    save_defaults
    clean
    cat sdkconfig.ci.tlv >> sdkconfig.defaults
    idf.py build
    cd qemu_test
    rm -rf qemu_test_images
    mkdir qemu_test_images
    ./make-qemu-img.sh cust_flash_tlv/cust_flash_tlv.bin cust_flash_tlv/partition-table.bin ../ esp_secure_cert_app qemu_test_images/cust_flash_tlv_qemu.img
    cd $ESP_SECURE_CERT_APP
    restore_defaults
}

build_legacy_format_images() {
    cd $ESP_SECURE_CERT_APP
    save_defaults
    clean
    cat sdkconfig.ci.legacy >> sdkconfig.defaults
    idf.py build
    cd qemu_test
    rm -rf qemu_test_images
    mkdir qemu_test_images
    ./make-qemu-img.sh cust_flash/cust_flash.bin cust_flash/partition-table.bin ../ esp_secure_cert_app qemu_test_images/cust_flash_qemu.img
    ./make-qemu-img.sh cust_flash_legacy/cust_flash_legacy.bin cust_flash_legacy/partition-table.bin ../ esp_secure_cert_app qemu_test_images/cust_flash_legacy_qemu.img
    ./make-qemu-img.sh nvs/nvs.bin nvs/partition-table.bin ../ esp_secure_cert_app qemu_test_images/nvs_qemu.img
    ./make-qemu-img.sh nvs_legacy/nvs_legacy.bin nvs/partition-table.bin ../ esp_secure_cert_app qemu_test_images/nvs_legacy_qemu.img
    ./make-qemu-img.sh nvs_legacy/nvs_legacy.bin nvs_legacy/partition-table.bin ../ esp_secure_cert_app qemu_test_images/nvs_legacy_qemu_new_part_name.img
    cd $ESP_SECURE_CERT_APP
    restore_defaults
}

if [ $1 == "tlv" ]; then
    # Build the TLV format image
    build_tlv_image
fi

if [ $1 == "legacy_format" ]; then
    # Build the TLV format image
    build_legacy_format_images
fi
