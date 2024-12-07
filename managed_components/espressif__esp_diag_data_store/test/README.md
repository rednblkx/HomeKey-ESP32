# Diagnostics data store unit tests

Please take a look at how to build, flash, and run [esp-idf unit tests](https://github.com/espressif/esp-idf/tree/master/tools/unit-test-app#unit-test-app).

Follow the steps mentioned below to unit test the diagnostics data store

* Change to the unit test app directory
```
cd $IDF_PATH/tools/unit-test-app
```

* Append `/path/to/esp-insights/components` directory to `EXTRA_COMPONENT_DIRS` in `CMakeLists.txt`

### Required configuration to unit test RTC store
* Let's add the config option for RTC store in sdkconfig.defaults.
```
echo CONFIG_DIAG_DATA_STORE_RTC=y >> $IDF_PATH/tools/unit-test-app/sdkconfig.defaults
```

### Required configuration to unit test flash store
* Let's add the config option for Flash store in sdkconfig.defaults.
```
echo CONFIG_DIAG_DATA_STORE_FLASH=y >> $IDF_PATH/tools/unit-test-app/sdkconfig.defaults
```
* Flash store requires the partition table entry of at lease 16KB, include the below line at the end of partition table csv file
```
diag_data, data, nvs, , 16K,
```

## Build, flash and run tests
```
# Clean any previous configuration and builds
rm -r sdkconfig build

# Set the target
idf.py set-target esp32

# Building the firmware
idf.py -T esp_diag_data_store build

# Flash and run the test cases
idf.py -p <serial-port> -T esp_diag_data_store flash monitor
```
