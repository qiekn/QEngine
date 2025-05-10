if(APPLE)
  set(RTTR_PATH "${CMAKE_CURRENT_SOURCE_DIR}/lib/macos/librttr_core.dylib")
elseif(UNIX AND NOT APPLE)
  set(RTTR_PATH "${CMAKE_CURRENT_SOURCE_DIR}/lib/linux/librttr_core.a")
elseif(WIN32)
  set(RTTR_PATH "${CMAKE_CURRENT_SOURCE_DIR}/lib/windows/librttr_core.a")
endif()

# macOS use dynamic library (I try to build static buf failed x_x, use homebrew prebuilds instead)
if(APPLE)
  add_library(rttr SHARED IMPORTED)
elseif (NOT APPLE) # use as static library
  add_library(rttr STATIC IMPORTED)
endif()

set_target_properties(rttr PROPERTIES
  IMPORTED_LOCATION ${RTTR_PATH}
  INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/include"
)
