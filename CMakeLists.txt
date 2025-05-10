cmake_minimum_required(VERSION 3.15)
project(game)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(3rdparty)

add_subdirectory(editor)
add_subdirectory(engine)


# format
file(GLOB_RECURSE ALL_HDRS CONFIGURE_DEPENDS
    "${CMAKE_SOURCE_DIR}/editor/include/*.h"
)

file(GLOB_RECURSE ALL_SRCS CONFIGURE_DEPENDS
    "${CMAKE_SOURCE_DIR}/editor/include/*.h"
)

add_custom_target(format 
    COMMAND clang-format -i ${ALL_SRCS} ${ALL_HDRS}
    COMMENT "Running clang-format on all header & cpp files"
)
