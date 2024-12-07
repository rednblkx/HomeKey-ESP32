idf_component_register(SRC_DIRS src
                       INCLUDE_DIRS include
                       REQUIRES driver esp-serial-flasher nvs_flash)

idf_build_get_property(python PYTHON)
if(CONFIG_AUTO_UPDATE_RCP)
add_custom_target(rcp_image_generation ALL
    COMMAND ${python} ${CMAKE_CURRENT_SOURCE_DIR}/create_ota_image.py
    --rcp-build-dir ${CONFIG_RCP_SRC_DIR}
    --target-file ${CMAKE_CURRENT_BINARY_DIR}/spiffs_image/ot_rcp_0/rcp_image
    )

spiffs_create_partition_image(${CONFIG_RCP_PARTITION_NAME} ${CMAKE_CURRENT_BINARY_DIR}/spiffs_image FLASH_IN_PROJECT
                                DEPENDS rcp_image_generation)
endif()

if(CONFIG_CREATE_OTA_IMAGE_WITH_RCP_FW)
    add_custom_command(OUTPUT ${build_dir}/ota_with_rcp_image
        COMMAND ${python} ${CMAKE_CURRENT_SOURCE_DIR}/create_ota_image.py
        --rcp-build-dir ${CONFIG_RCP_SRC_DIR}
        --target-file ${build_dir}/ota_with_rcp_image
        --br-firmware "${build_dir}/${elf_name}.bin"
        DEPENDS "${build_dir}/.bin_timestamp"
        )

    add_custom_target(gen_ota_image ALL DEPENDS ${build_dir}/ota_with_rcp_image)
    add_dependencies(gen_ota_image gen_project_binary)
endif()
