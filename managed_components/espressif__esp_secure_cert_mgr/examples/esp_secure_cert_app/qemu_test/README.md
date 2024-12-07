# ESP Secure cert partition tests

This folder contains the test setup for testing the different flash formats supported by esp_secure_cert partition. The tests are based on the QEMU emulater.

## Requirements

* **QEMU emulator**:
   	
   	The test is based on qemu and needs the qemu emulator support.
   	
   	* For Linux Distributions, the executables to be downloaded from [qemu/releases](https://github.com/espressif/qemu/releases)
   	 
    * For macOS you need to build the binaries for specific release locally on machine. see [Compiling Qemu](https://github.com/espressif/esp-toolchain-docs/blob/main/qemu/esp32/README.md#compiling-qemu) for more details.

* **Pytest**:
    Install the pytest environment by executing following command
    
    ```pip install -r requirements.txt```
 
## Testing
The script [`build_qemu_images.sh`](./build_qemu_images.sh) can be used to generate the respective qemu images. These images are then provided to pytest framework for execution.


### Test the TLV support on esp32
The TLV support can be tested with following commands:

* ```./build_qemu_images.sh tlv```

* ``` cd ..```

* ``pytest --target esp32``
 
### Test the legacy flash format support on esp32
The TLV support can be tested with following commands:

* ```./build_qemu_images.sh legacy```

* ``` cd ..```

* ``pytest --target esp32``
