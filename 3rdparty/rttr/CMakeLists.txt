add_library(librttr STATIC IMPORTED)

if(WIN32)
  set(RTTR_PATH "${CMAKE_CURRENT_SOURCE_DIR}/lib/windows/rttr_core.lib")
elseif(UNIX AND NOT APPLE)
  set(RTTR_PATH "${CMAKE_CURRENT_SOURCE_DIR}/lib/linux/librttr_core.a")
elseif(APPLE)
  set(RTTR_PATH "${CMAKE_CURRENT_SOURCE_DIR}/lib/macos/librttr_core.a")
endif()

set_target_properties(librttr PROPERTIES
  IMPORTED_LOCATION ${RTTR_PATH}
  INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/include"
)


add_library(rttr INTERFACE)
target_include_directories(rttr INTERFACE include)
target_link_libraries(rttr INTERFACE librttr)
