set(EDITOR editor)

#---------------------------------------------------------------------3
#                               Editor                                |
#---------------------------------------------------------------------3

# sources
file(GLOB_RECURSE EDITOR_SRCS CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

add_executable(${EDITOR} ${EDITOR_SRCS})

# headers
target_include_directories(${EDITOR} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

# libraries
target_link_libraries(${EDITOR}
    raylib rlimgui
    rapidjson
    cppzmq
)

# checks if OSX and links appropriate frameworks (only required on macOS)
if (APPLE)
    target_link_libraries(${EDITOR} "-framework IOKit")
    target_link_libraries(${EDITOR} "-framework Cocoa")
    target_link_libraries(${EDITOR} "-framework OpenGL")
endif()
