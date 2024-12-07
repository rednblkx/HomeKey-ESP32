#!/bin/bash
ESP_SECURE_CERT_APP=$PWD/../examples/esp_secure_cert_app

# This option causes the script to fail if any command fails in the main body of the script
# Note: this does not apply to the internal functions
# We need to check the exit status of each command individually
set -e

clean() {
    rm -r sdkconfig 2>&1
    rm -r build 2>&1
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

# Build the default config with ds support enabled
# $1 = IDF_TARGET
build_ds_config() {
    idf_target=$1
    cd $ESP_SECURE_CERT_APP
    clean
    idf.py set-target $idf_target || exit $?
    idf.py build || exit $?
    clean
}

# Build the firmware with the config option for legacy flash formats enabled
# $1 = IDF_TARGET
build_ds_config_legacy_flash_format() {
    idf_target=$1
    cd $ESP_SECURE_CERT_APP
    clean
    echo "CONFIG_ESP_SECURE_CERT_SUPPORT_LEGACY_FORMATS=y" >> sdkconfig.defaults
    idf.py set-target $idf_target || exit $?
    idf.py build || exit $?
    clean
}

# Build the firmware with ds support disabled
build_no_ds_config() {
    idf_target=$1
    if [[ $idf_target == "esp32" ]]; then
        echo "No ds config is same as default config for esp32"
        echo "Exiting"
        exit 0
    fi
    cd $ESP_SECURE_CERT_APP
    clean
    echo "CONFIG_ESP_SECURE_CERT_DS_PERIPHERAL=n" >> sdkconfig.defaults
    idf.py set-target $idf_target || exit $?
    idf.py build || exit $?
    clean
}

# $1 = IDF_TARGET
# $2 = CI_PARALLEL_COUNT
# This function assigns tests based on the parallel count value
assign_test() {
    idf_target=$1
    ci_parallel_count=$2
    if [[ $ci_parallel_count == "1" ]]; then
        echo "Building the default config"
        build_ds_config $idf_target || exit 1
    fi
    if [[ $ci_parallel_count == "2" ]]; then
        echo "Building the legacy flash format config"
        build_ds_config_legacy_flash_format $idf_target || exit 1
    fi
    if [[ $ci_parallel_count == "3" ]]; then
        echo "Building with ds support disabled"
        build_no_ds_config  $idf_target || exit 1
    fi
}

IDF_TARGET=$1
CI_PARALLEL_COUNT=$2
echo "Building for target "$IDF_TARGET
echo "CI Parallel count: "$CI_PARALLEL_COUNT
idf_path_verify || exit 1
assign_test $IDF_TARGET $CI_PARALLEL_COUNT
