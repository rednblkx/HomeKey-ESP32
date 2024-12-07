# A CMake script to create tar package for ESP Insights
# It is created for the esp_rainmaker component

cmake_minimum_required(VERSION 3.5)

# Set file paths
# Set app elf file path
set(elf_file_path ${BUILD_DIR}/${PROJECT_NAME}.elf)
# Set app binary file path
set(bin_file_path ${BUILD_DIR}/${PROJECT_NAME}.bin)
# Set app map file path
set(map_file_path ${BUILD_DIR}/${PROJECT_NAME}.map)
# Set project git config json file path
set(proj_config_file_path ${BUILD_DIR}/${PROJ_CONFIG_FILE})
# Set sdkconfig file path
set(sdkconfig_file_path ${PROJECT_DIR}/sdkconfig)
# Set partition table binary file path
set(partition_bin_file_path ${BUILD_DIR}/partition_table/partition-table.bin)
# Set partition table csv file path
set(partition_csv_file_path ${PROJECT_DIR}/${PARTITION_CSV_FILE})
# Set bootloader binary file path
set(bootloader_bin_file_path ${BUILD_DIR}/bootloader/bootloader.bin)
# Set ota data initial binary file path
set(ota_data_bin_file_path ${BUILD_DIR}/ota_data_initial.bin)
# Set flash args file path
set(flash_args_file_path ${BUILD_DIR}/flash_args)
# Set project build config file path
set(proj_desc_file_path ${BUILD_DIR}/project_description.json)
# Set custom project build config file path
set(custom_proj_desc_file_path ${BUILD_DIR}/project_description_custom.json)

# Create archive directory
file(MAKE_DIRECTORY ${BUILD_DIR}/${ARCHIVE_DIR})
# Copy project git config file generated to archive directory
file(COPY ${proj_config_file_path} DESTINATION ${BUILD_DIR}/${ARCHIVE_DIR})

# Copy elf file to archive directory
if(EXISTS ${elf_file_path})
    file(COPY ${elf_file_path} DESTINATION ${BUILD_DIR}/${ARCHIVE_DIR})
endif()

# Copy bin file to archive directory
if(EXISTS ${bin_file_path})
    file(COPY ${bin_file_path} DESTINATION ${BUILD_DIR}/${ARCHIVE_DIR})
endif()

# Copy map file to archive directory
if(EXISTS ${map_file_path})
    file(COPY ${map_file_path} DESTINATION ${BUILD_DIR}/${ARCHIVE_DIR})
endif()

# Copy sdkconfig file to archive directory
if(EXISTS ${sdkconfig_file_path})
    file(COPY ${sdkconfig_file_path} DESTINATION ${BUILD_DIR}/${ARCHIVE_DIR})
endif()

# Copy partition csv file to archive directory
if(EXISTS ${partition_csv_file_path})
    file(COPY ${partition_csv_file_path} DESTINATION ${BUILD_DIR}/${ARCHIVE_DIR})
endif()

# Copy partition bin file to archive directory
if(EXISTS ${partition_bin_file_path})
    # Create partition_table sub-dir in archive directory
    file(MAKE_DIRECTORY ${BUILD_DIR}/${ARCHIVE_DIR}/partition_table)
    file(COPY ${partition_bin_file_path} DESTINATION ${BUILD_DIR}/${ARCHIVE_DIR}/partition_table)
endif()

# Copy bootloader bin file to archive directory
if(EXISTS ${bootloader_bin_file_path})
    # Create bootloader sub-dir in archive directory
    file(MAKE_DIRECTORY ${BUILD_DIR}/${ARCHIVE_DIR}/bootloader)
    file(COPY ${bootloader_bin_file_path} DESTINATION ${BUILD_DIR}/${ARCHIVE_DIR}/bootloader)
endif()

# Copy ota_data_initial bin file to archive directory
if(EXISTS ${ota_data_bin_file_path})
    file(COPY ${ota_data_bin_file_path} DESTINATION ${BUILD_DIR}/${ARCHIVE_DIR})
endif()

# Copy flash args file to archive directory
if(EXISTS ${flash_args_file_path})
    file(COPY ${flash_args_file_path} DESTINATION ${BUILD_DIR}/${ARCHIVE_DIR})
endif()

# Copy project description json file to archive directory
if (EXISTS ${proj_desc_file_path})
    file(COPY ${proj_desc_file_path} DESTINATION ${BUILD_DIR}/${ARCHIVE_DIR}/)
endif()

# Copy custom project description json file to archive directory
if (EXISTS ${custom_proj_desc_file_path})
    file(COPY ${custom_proj_desc_file_path} DESTINATION ${BUILD_DIR}/${ARCHIVE_DIR}/)
endif()
