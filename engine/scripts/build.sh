#!/bin/bash

VERBOSE=""
if [ "$1" == "--verbose" ]; then
    VERBOSE="VERBOSE=1"
fi

python3 parser2.py && cd .. && premake5 gmake && cd build && make $VERBOSE 

EXIT_CODE=$?

exit $EXIT_CODE
