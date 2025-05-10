cmake_minimum_required(VERSION 3.11) # FetchContent is available in 3.11+
project(editor)

#---------------------------------------------------------------------3
#                             Our Project                             |
#---------------------------------------------------------------------3

# generate compile_commands.json
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 20)

# sources
file(GLOB_RECURSE SRCS CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

# targets
add_executable(${PROJECT_NAME} ${SRCS})

# headers
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

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
    "${CMAKE_CURRENT_SOURCE_DIR}/include/*.h"
)

add_custom_target(format 
    COMMAND clang-format -i ${SRCS} ${HDRS}
    COMMENT "Running clang-format on all header & cpp files"
)
