idf_build_get_property(python PYTHON)
idf_build_get_property(idf_path IDF_PATH)
idf_build_get_property(build_dir BUILD_DIR)

set(PROJ_BUILD_CONFIG_FILE project_build_config.json)
set(ARCHIVE_NAME ${CMAKE_PROJECT_NAME}-v${PROJECT_VER})

if(CONFIG_ESP_INSIGHTS_ENABLED)
# This will create the archive tar
add_custom_command(
        TARGET app
        POST_BUILD
        COMMAND ${python} ${CMAKE_CURRENT_LIST_DIR}/scripts/get_projbuild_gitconfig.py ${PROJECT_DIR} ${CMAKE_PROJECT_NAME} ${PROJECT_VER} ${build_dir}/${PROJ_BUILD_CONFIG_FILE} ${idf_path} ${_CMAKE_TOOLCHAIN_PREFIX}
        COMMAND ${CMAKE_COMMAND}
        -D BUILD_DIR=${build_dir}
        -D PROJECT_DIR=${PROJECT_DIR}
        -D PROJECT_NAME=${CMAKE_PROJECT_NAME}
        -D PROJECT_VER=${PROJECT_VER}
        -D ARCHIVE_DIR=${ARCHIVE_NAME}
        -D PROJ_CONFIG_FILE=${PROJ_BUILD_CONFIG_FILE}
        -D PARTITION_CSV_FILE=${CONFIG_PARTITION_TABLE_CUSTOM_FILENAME}
        -P ${CMAKE_CURRENT_LIST_DIR}/scripts/gen_tar_dir.cmake
        COMMAND ${CMAKE_COMMAND} -E echo "===================== Generating insights firmware package build/${ARCHIVE_NAME}.zip ======================"
        COMMAND ${CMAKE_COMMAND} -E tar cfv ${ARCHIVE_NAME}.zip ${ARCHIVE_NAME} --format=zip
        COMMAND ${CMAKE_COMMAND} -E remove_directory ${ARCHIVE_NAME}
        COMMAND ${CMAKE_COMMAND} -E remove ${PROJ_BUILD_CONFIG_FILE}
        VERBATIM
)
endif()
