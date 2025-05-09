add_library(zmq INTERFACE)

target_include_directories(zmq INTERFACE
  ${CMAKE_CURRENT_SOURCE_DIR}/binding
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)

if(APPLE)
  target_link_libraries(zmq INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/lib/macos/libzmq.a)
elseif(UNIX)
  target_link_libraries(zmq INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/lib/linux/libzmq.a)
elseif(WIN32)
  target_link_libraries(zmq INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/lib/windows/libzmq.lib)
endif()
