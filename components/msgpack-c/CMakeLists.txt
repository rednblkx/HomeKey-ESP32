set(BUILD_SHARED_LIBS OFF)
set(MSGPACK_BUILD_EXAMPLES OFF CACHE BOOL "Disable msgpack examples")
set(MSGPACK_BUILD_TESTS OFF CACHE BOOL "Disable msgpack tests")
# Create dummy functions to override the problematic commands in the upstream CMakeLists.txt.
# This is the key to resolving the "not in any export set" errors.
function(install)
    # This empty function will be called instead of the real 'install' command.
endfunction()

function(export)
    # This empty function will be called instead of the real 'export' command.
endfunction()
idf_component_register(INCLUDE_DIRS "upstream/include")
add_subdirectory(upstream)
target_include_directories(${COMPONENT_LIB} INTERFACE "${CMAKE_CURRENT_BINARY_DIR}/upstream/include")
target_include_directories(${COMPONENT_LIB} INTERFACE "${CMAKE_CURRENT_BINARY_DIR}/upstream/include/msgpack")
target_link_libraries(${COMPONENT_LIB} INTERFACE msgpack-c)
