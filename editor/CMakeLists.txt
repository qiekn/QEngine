cmake_minimum_required(VERSION 3.11) # FetchContent is available in 3.11+
project(editor)

#---------------------------------------------------------------------3
#                            Dependencies                             |
#---------------------------------------------------------------------3

# Raylib
set(RAYLIB_VERSION 5.5)
find_package(raylib ${RAYLIB_VERSION} QUIET) # QUIET or REQUIRED
if (NOT raylib_FOUND) # If there's none, fetch and build raylib
  include(FetchContent)
  FetchContent_Declare(
    raylib
    DOWNLOAD_EXTRACT_TIMESTAMP OFF
    URL https://github.com/raysan5/raylib/archive/refs/tags/${RAYLIB_VERSION}.tar.gz
  )
  FetchContent_GetProperties(raylib)
  if (NOT raylib_POPULATED) # Have we downloaded raylib yet?
    set(FETCHCONTENT_QUIET NO)
    FetchContent_MakeAvailable(raylib)
    set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE) # don't build the supplied examples
  endif()
endif()

add_subdirectory(3rdparty/imgui)
add_subdirectory(3rdparty/rlimgui)
#add_subdirectory(3rdparty/entt)
#add_subdirectory(3rdparty/json)
#add_subdirectory(3rdparty/magic-enum)
add_subdirectory(3rdparty/rapidjson)
add_subdirectory(3rdparty/zmq)

#---------------------------------------------------------------------3
#                             Our Project                             |
#---------------------------------------------------------------------3

# generate compile_commands.json
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 20)

# sources
file(GLOB_RECURSE SRCS CONFIGURE_DEPENDS
    "${CMAKE_SOURCE_DIR}/src/*.cpp"
)

# targets
add_executable(${PROJECT_NAME} ${SRCS})

# headers
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/include)

# libraries
target_link_libraries(${PROJECT_NAME}
    raylib rlimgui
    rapidjson
    cppzmq
)

# checks if OSX and links appropriate frameworks (only required on macOS)
if (APPLE)
    target_link_libraries(${PROJECT_NAME} "-framework IOKit")
    target_link_libraries(${PROJECT_NAME} "-framework Cocoa")
    target_link_libraries(${PROJECT_NAME} "-framework OpenGL")
endif()

# format
file(GLOB_RECURSE HDRS CONFIGURE_DEPENDS
    "${CMAKE_SOURCE_DIR}/include/*.h"
)

add_custom_target(format 
    COMMAND clang-format -i ${SRCS} ${HDRS}
    COMMENT "Running clang-format on all header & cpp files"
)
