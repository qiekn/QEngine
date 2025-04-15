workspace "Zeytin"
    configurations {"EDITOR_MODE", "STANDALONE"}
    location "build"
    toolset "clang"
    targetdir "bin/%{cfg.buildcfg}"

    includedirs {
        "include", 
        "3rdparty/raylib/src", 
        "3rdparty/rttr/build/install/include", 
        "3rdparty/rapidjson/include",
        "3rdparty/tracy",
    }

    libdirs { 
        "3rdparty/raylib/src", 
        "3rdparty/rttr/build/install/lib",
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
        "rttr_core",
        "zmq"
    }


    buildoptions { 
        "-w",              
        "-std=c++17",     
        "-stdlib=libstdc++", 
        "-g3",            
        "-fno-omit-frame-pointer",
        "-rdynamic"
    }

    filter "action:gmake"
        buildoptions { "-fPIC" }  

    project "Zeytin"
        kind "ConsoleApp"
        language "C++"
        files { "source/**.cpp", "3rdparty/backward-cpp/backward.cpp", "3rdparty/tracy/TracyClient.cpp",}

        filter "configurations:EDITOR_MODE"
            defines {"DEBUG=1", "EDITOR_MODE=1", "TRACY_ENABLE=1"}
            symbols "On"
            optimize "Off"
