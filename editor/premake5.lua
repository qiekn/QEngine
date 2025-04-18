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
    
    filter "system:windows"
        defines { "_WINDOWS", "_CRT_SECURE_NO_WARNINGS" }
        links {
            "raylib",
            "winmm",
            "gdi32",
            "user32",
            "shell32",
            "zmq"  
        }
        
    filter "system:linux"
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
        buildoptions { "-fPIC" }
    filter {}

    buildoptions {
        "-w",
        "-std=c++17",
        "-stdlib=libstdc++",
        "-ferror-limit=0"
    }

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
