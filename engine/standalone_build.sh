#!/bin/bash

#if [ -d "bin" ]; then
#    rm -r bin
#fi
#
#if [ -d "build" ]; then
#    rm -r build
#fi

premake5 gmake && cd build && make config=standalone && cd ..
