# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "")
  file(REMOVE_RECURSE
  "HomeKey-ESP32.map"
  "esp-idf/esptool_py/flasher_args.json.in"
  "esp-idf/mbedtls/x509_crt_bundle"
  "flash_app_args"
  "flash_bootloader_args"
  "flasher_args.json"
  "https_server.crt.S"
  "ldgen_libraries"
  "ldgen_libraries.in"
  "littlefs_py_venv"
  "project_elf_src_esp32c3.c"
  "rmaker_claim_service_server.crt.S"
  "rmaker_mqtt_server.crt.S"
  "rmaker_ota_server.crt.S"
  "x509_crt_bundle.S"
  )
endif()
