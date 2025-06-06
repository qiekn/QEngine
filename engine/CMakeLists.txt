set(ENGINE engine)

#---------------------------------------------------------------------3
#                               Engine                                |
#---------------------------------------------------------------------3

# sources
file(GLOB_RECURSE ENGINE_SRCS CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

add_executable(${ENGINE} ${ENGINE_SRCS})

# headers
target_include_directories(${ENGINE} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

# libraries
target_link_libraries(${ENGINE} PRIVATE
    raylib rlimgui
    rttr
    pthread
    cppzmq
    rapidjson
)


# checks if OSX and links appropriate frameworks (only required on macOS)
if (APPLE)
    target_link_libraries(${ENGINE} PRIVATE
        "-framework IOKit"
        "-framework Cocoa"
        "-framework OpenGL"
    )
endif()
