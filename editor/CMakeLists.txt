project(editor)

#---------------------------------------------------------------------3
#                               Editor                                |
#---------------------------------------------------------------------3

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# sources
file(GLOB_RECURSE SRCS CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

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
