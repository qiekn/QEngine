#!/bin/bash

if ! command -v emcc &> /dev/null; then
    echo "Emscripten (emcc) not found in path."
    echo "Please make sure you've sourced the Emscripten environment:"
    echo "  source /path/to/emsdk/emsdk_env.sh"
    exit 1
fi

RAYLIB_PATH="3rdparty/raylib"
SHELL_HTML="web/template.html"
OUTPUT_DIR="bin/WEB"

mkdir -p $OUTPUT_DIR

if [ "$1" == "--direct" ]; then
    echo "Using direct compilation with emcc..."
    
    SOURCE_FILES=$(find source -name "*.cpp" ! -path "source/editor/*" ! -path "source/remote_logger/*")
    
    emcc -o $OUTPUT_DIR/Zeytin.html $SOURCE_FILES \
        -Wall -std=c++17 -D_DEFAULT_SOURCE -Wno-missing-braces -Wunused-result -Os \
        -I include -I 3rdparty -I $RAYLIB_PATH -I 3rdparty/rttr \
        -L $RAYLIB_PATH/lib -L 3rdparty/rttr/lib \
        -s USE_GLFW=3 -s ASYNCIFY -s TOTAL_MEMORY=67108864 -s FORCE_FILESYSTEM=1 \
        --preload-file ../shared_resources@/shared_resources \
        --shell-file $SHELL_HTML \
        $RAYLIB_PATH/lib/libraylib.web.a \
        -DPLATFORM_WEB -DSTANDALONE=1 \
        -s EXPORTED_FUNCTIONS='["_free","_malloc","_main"]' \
        -s EXPORTED_RUNTIME_METHODS=ccall
else
    echo "Using premake for web build..."
    
    premake5 gmake
    
    cd build
    make config=web verbose=1
    cd ..
fi

mkdir -p web_deploy
cp -r $OUTPUT_DIR/* web_deploy/

echo "Build complete! Files are in the web_deploy directory."
echo "To test your game, run a local web server in the web_deploy directory."
echo "For example: python3 -m http.server 8080"
