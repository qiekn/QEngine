#!/bin/bash

#if [ -d "bin" ]; then
#    rm -r bin
#fi
#
#if [ -d "build" ]; then
#    rm -r build
#fi

python3 parser2.py && cd .. && premake5 gmake && cd build && make config=standalone
