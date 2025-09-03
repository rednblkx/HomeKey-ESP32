SET(MSGPACK_ENDIAN_BIG_BYTE 0)
SET(MSGPACK_ENDIAN_LITTLE_BYTE 1)
CONFIGURE_FILE (
  ${CMAKE_CURRENT_LIST_DIR}/upstream/cmake/sysdep.h.in
  ${CMAKE_CURRENT_BINARY_DIR}/msgpack/sysdep.h
  @ONLY
)

CONFIGURE_FILE (
  ${CMAKE_CURRENT_LIST_DIR}/upstream/cmake/pack_template.h.in
  ${CMAKE_CURRENT_BINARY_DIR}/msgpack/pack_template.h
  @ONLY
)
idf_component_register(SRCS
      "upstream/src/objectc.c"
      "upstream/src/unpack.c"
      "upstream/src/version.c"
      "upstream/src/vrefbuffer.c"
      "upstream/src/zone.c" INCLUDE_DIRS "upstream/include")
target_include_directories(${COMPONENT_LIB} PUBLIC "${CMAKE_CURRENT_BINARY_DIR}/msgpack")
target_include_directories(${COMPONENT_LIB} PUBLIC "${CMAKE_CURRENT_BINARY_DIR}")
