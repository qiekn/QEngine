workspace "Zeytin"
    configurations { "EDITOR_MODE", "STANDALONE", "WEB"}
    location "build"
    targetdir "bin/%{cfg.buildcfg}"
    
    staticruntime "On"

    filter "system:linux"
        toolset "clang"
    filter "system:windows"
        toolset "msc"

    filter {}

    includedirs {
        "include", 
        "3rdparty",
        "3rdparty/raylib",
        "3rdparty/rttr", 
        "3rdparty/tracy",
        "3rdparty/zmq/include"
    }
    
    libdirs { 
        "3rdparty/rttr/lib",
        "3rdparty/raylib/lib",
        "3rdparty/zmq/lib"
    }
    
    filter { "system:linux", "configurations:EDITOR_MODE" }
        links {
            "raylib",
            "m",
            "pthread",
            "dl",
            "rt",
            "X11",
            "asound",
            "stdc++",
            "rttr_core",
            "zmq",
            "dw",     
            "bfd",
            "dwarf",
            "unwind"
        }
    
    filter { "system:linux", "configurations:STANDALONE" }
        links {
            "raylib",
            "m",
            "pthread",
            "dl",
            "rt",
            "X11",
            "asound",
            "stdc++",
            "rttr_core"
        }
    
    filter "system:linux"
        buildoptions {
            "-w",
            "-std=c++17",
            "-static-libstdc++",
        }

    filter { "system:windows", "configurations:EDITOR_MODE" }
        links {
            "raylib",
            "rttr_core",
            "libzmq",
            "winmm",
            "gdi32",
            "user32",
            "shell32"
        }
        buildoptions {
            "/MT" 
        }
    
    filter { "system:windows", "configurations:STANDALONE" }
        links {
            "raylib",
            "rttr_core",
            "winmm",
            "gdi32",
            "user32",
            "shell32"
        }
        buildoptions {
            "/MT"  
        }
    
    filter "system:windows"
        buildoptions { 
            "/std:c++17", 
            "/w",
            "/bigobj",
        }

    filter { "configurations:WEB" }
        kind "ConsoleApp"
        targetextension ".html"
        defines { "PLATFORM_WEB" }
        buildoptions {
            "-std=c++17",
            "-Wall",
            "-Wno-missing-braces",
            "-Wunused-result"
        }
        linkoptions {
            "--preload-file ../shared_resources@/shared_resources",
            "-s USE_GLFW=3",
            "-s ASYNCIFY",
            "-s TOTAL_MEMORY=67108864", 
            "-s FORCE_FILESYSTEM=1",
            "-s WASM=1",
            "-s EXPORTED_FUNCTIONS=[\"_free\",\"_malloc\",\"_main\"]",
            "-s EXPORTED_RUNTIME_METHODS=ccall",
            "-Os" 
        }
        links {
            "raylib"
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
            defines { 
                "TRACY_ENABLE=0" 
            }
            symbols "On"
            optimize "Off"
