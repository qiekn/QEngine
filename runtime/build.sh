#!/bin/bash

./parser.sh && premake5 gmake && cd build && make && cd ..
