add_library(libzmq STATIC IMPORTED)

if(APPLE)
  set(LIBZMQ_PATH "${CMAKE_CURRENT_SOURCE_DIR}/lib/macos/libzmq.a")
elseif(UNIX AND NOT APPLE)
  set(LIBZMQ_PATH "${CMAKE_CURRENT_SOURCE_DIR}/lib/linux/libzmq.a")
elseif(WIN32)
  set(LIBZMQ_PATH "${CMAKE_CURRENT_SOURCE_DIR}/lib/windows/libzmq.lib")
endif()

set_target_properties(libzmq PROPERTIES
  IMPORTED_LOCATION "${LIBZMQ_PATH}"
  INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

if(APPLE)
  target_link_libraries(libzmq INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/lib/macos/libsodium.a)
endif()

add_library(cppzmq INTERFACE)
target_include_directories(cppzmq INTERFACE binding)
target_link_libraries(cppzmq INTERFACE libzmq)
