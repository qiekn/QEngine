workspace "Zeytin"
    configurations { "EDITOR_MODE", "STANDALONE" }
    location "build"
    targetdir "bin/%{cfg.buildcfg}"
    staticruntime "On"

    filter "system:linux"
        toolset "clang"

    filter {}

    includedirs {
        "include",
        "3rdparty",
        "3rdparty/raylib",
        "3rdparty/rttr",
        "3rdparty/tracy",
        "3rdparty/zmq/include"
    }

    filter "system:windows"
        libdirs {
            "3rdparty/raylib/lib/windows",
            "3rdparty/rttr/lib/windows",
        }
    filter "system:linux"
        libdirs {
            "3rdparty/raylib/lib/linux",
            "3rdparty/rttr/lib/linux",
        }

    filter { "system:linux", "configurations:EDITOR_MODE" }
        links {
            "raylib", "m", "pthread", "dl", "rt", "X11", "asound", "stdc++",
            "rttr_core", "zmq", "dw", "bfd", "dwarf", "unwind"
        }
        libdirs {
            "3rdparty/zmq/lib/linux", 
        }

    filter { "system:linux", "configurations:STANDALONE" }
        links {
            "raylib", "m", "pthread", "dl", "rt", "X11", "asound", "stdc++",
            "rttr_core"
        }

    filter "system:linux"
        buildoptions {
            "-w",
            "-std=c++17",
            "-static-libstdc++"
        }

    filter { "system:windows", "toolset:gcc" }
        buildoptions {
            "-std=c++17",
            "-w",
            "-static-libgcc",
            "-static-libstdc++"
        }

    filter { "system:windows", "toolset:gcc", "configurations:STANDALONE" }
        links {
            "raylib", "rttr_core",
            "winmm", "gdi32", "user32", "shell32"
        }

    filter {}

    project "Zeytin"
        kind "ConsoleApp"
        language "C++"
        files {
            "source/**.cpp"
        }

        filter "configurations:EDITOR_MODE"
            files {
                "3rdparty/backward-cpp/backward.cpp",
                "3rdparty/tracy/TracyClient.cpp"
            }
            defines {
                "DEBUG=1",
                "EDITOR_MODE=1",
                "TRACY_ENABLE=1"
            }
            symbols "On"
            optimize "Off"

        filter "configurations:STANDALONE"
            defines { "TRACY_ENABLE=0" }
            symbols "On"
            optimize "Off"
