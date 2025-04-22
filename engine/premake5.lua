workspace "Zeytin"
    configurations {"EDITOR_MODE", "STANDALONE"}
    location "build"
    targetdir "bin/%{cfg.buildcfg}"
    
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
        "3rdparty/rapidjson/include",
        "3rdparty/tracy",
    }
    
    libdirs { 
        "3rdparty/rttr/lib",
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
            "rttr_core",
            "dw",     
            "bfd",
            "dwarf",
            "unwind"
        }
    
    filter "system:linux"
        buildoptions {
            "-w",
            "-std=c++17",
            "-stdlib=libstdc++",
            "-g3",
            "-fno-omit-frame-pointer",
            "-rdynamic",
            "-funwind-tables",
            "-fasynchronous-unwind-tables",
            "-fPIC"
        }
        postbuildcommands {
            "{COPY} %{wks.location}/../3rdparty/raylib/lib/libraylib.so* %{cfg.targetdir}/"
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
    
    filter { "system:windows", "configurations:STANDALONE" }
        links {
            "raylib",
            "rttr_core",
            "winmm",
            "gdi32",
            "user32",
            "shell32"
        }
    
    filter "system:windows"
        buildoptions { "/std:c++17", "/w" }
        
        postbuildcommands {
            "{COPY} %{wks.location}/../3rdparty/raylib/lib/*.dll %{cfg.targetdir}"
        }
    
    filter {}
    
    project "Zeytin"
        kind "ConsoleApp"
        language "C++"
        files { "source/**.cpp", "3rdparty/backward-cpp/backward.cpp", "3rdparty/tracy/TracyClient.cpp"}
        
        filter "configurations:EDITOR_MODE"
            defines {"DEBUG=1", "EDITOR_MODE=1", "TRACY_ENABLE=1"}
            symbols "On"
            optimize "Off"
            
        filter "configurations:STANDALONE"
            defines {"TRACY_ENABLE=1"}
            symbols "On"
            optimize "Off"
