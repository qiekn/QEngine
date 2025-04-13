#!/bin/bash

VERBOSE=""
if [ "$1" == "--verbose" ]; then
    VERBOSE="VERBOSE=1"
fi

./parser.sh && premake5 gmake && cd build && make $VERBOSE 

EXIT_CODE=$?

exit $EXIT_CODE
