cmake_minimum_required(VERSION 3.15)
project(game)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory(3rdparty)

add_subdirectory(editor)
add_subdirectory(engine)


# format
file(GLOB_RECURSE ALL_HDRS CONFIGURE_DEPENDS
    "${CMAKE_SOURCE_DIR}/editor/include/*.h"
    "${CMAKE_SOURCE_DIR}/engine/include/*.h"
)

file(GLOB_RECURSE ALL_SRCS CONFIGURE_DEPENDS
    "${CMAKE_SOURCE_DIR}/editor/include/*.h"
    "${CMAKE_SOURCE_DIR}/engine/include/*.h"
)

add_custom_target(format 
    COMMAND clang-format -i ${ALL_SRCS} ${ALL_HDRS}
    COMMENT "Running clang-format on all header & cpp files"
)
