workspace "ZeytinEditor"
    configurations { "Debug" }
    location "build"
    targetdir "bin/%{cfg.buildcfg}"
    toolset "clang"

   
    includedirs {
        "include",
        "include/imgui",
        "3rdparty/raylib/src",
        "3rdparty/rapidjson/include"
    }

    
    libdirs {
        "3rdparty/raylib/src"
    }

    links {
        "raylib",
        "m",
        "pthread",
        "dl",
        "rt",
        "X11",
        "asound",
        "stdc++",
        "zmq"
    }

  
    buildoptions {
        "-w",
        "-std=c++17",
        "-stdlib=libstdc++",
        "-ferror-limit=0"
    }

    filter "action:gmake"
        buildoptions { "-fPIC" }

   
    project "ZeytinEditor"
        kind "ConsoleApp"
        language "C++"
        symbols "On"
        optimize "Off"

        
        files {
            "3rdparty/rlimgui/**.cpp",
            "3rdparty/imgui/**.cpp",
            "3rdparty/imgui_test_engine/imgui_test_engine/**.cpp",
            "3rdparty/imgui_test_engine/imgui_test_engine/**.h",
            "3rdparty/backward-cpp/backward.cpp",
            "source/**.cpp"
        }

        filter "configurations:Debug"
            defines {
                "DEBUG=1",
                "IMGUI_ENABLE_TEST_ENGINE",
                "IMGUI_TEST_ENGINE_ENABLE_COROUTINE_STDTHREAD_IMPL=1"
            }

            includedirs {
                "3rdparty/imgui_test_engine"
            }
