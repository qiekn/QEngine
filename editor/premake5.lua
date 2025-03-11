workspace "ZeytinEditor"
    configurations { "Debug" }

    location "build"
    toolset "clang"
    targetdir "bin/%{cfg.buildcfg}"

    includedirs {
        "include", 
        "include/imgui",
        "3rdparty/raylib/src", 
        "3rdparty/rapidjson/include", 
    }

    libdirs { 
        "3rdparty/raylib/src", 
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
    }

    filter "action:gmake"
        buildoptions { "-fPIC" }  

    project "ZeytinEditor"
        kind "ConsoleApp"
        language "C++"

        files { "source/**.cpp" }

        filter "configurations:Debug"
            defines {"DEBUG=1"}
            symbols "On"

