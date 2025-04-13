#!/bin/bash

OUTPUT_FILE="${1:-build_status.json}"
BUILD_LOG="build_status/build_output.log"
BUILD_COMMAND="./build.sh"

mkdir -p build_status

echo "BUILD: Starting build process..."
echo "{\"status\":\"running\",\"message\":\"Build started\",\"timestamp\":\"$(date +%s)\"}" > "$OUTPUT_FILE"

$BUILD_COMMAND 2>&1 | tee "$BUILD_LOG" | while read -r line; do
    echo "BUILD: $line"
done

BUILD_EXIT_CODE=${PIPESTATUS[0]}

if [ $BUILD_EXIT_CODE -eq 0 ]; then
    echo "BUILD: Completed successfully"
    echo "{\"status\":\"success\",\"message\":\"Build completed successfully\",\"timestamp\":\"$(date +%s)\"}" > "$OUTPUT_FILE"
    exit 0
else
    echo "BUILD: Failed with exit code $BUILD_EXIT_CODE"
    
    ERROR_SUMMARY=$(grep -E "error:|failed|Error |make: \*\*\*" "$BUILD_LOG" | head -5 | sed 's/"/\\"/g')
    if [ -z "$ERROR_SUMMARY" ]; then
        ERROR_SUMMARY=$(tail -10 "$BUILD_LOG" | sed 's/"/\\"/g')
    fi
    ERROR_JSON=$(printf "%s\\n" "$ERROR_SUMMARY" | sed ':a;N;$!ba;s/\n/\\n/g')
    
    echo "{\"status\":\"failed\",\"message\":\"Build failed with exit code $BUILD_EXIT_CODE\",\"details\":\"$ERROR_JSON\",\"timestamp\":\"$(date +%s)\"}" > "$OUTPUT_FILE"
    
    # Print error summary to console
    echo "BUILD ERROR SUMMARY:"
    echo "$ERROR_SUMMARY" | sed 's/\\n/\n/g'
    
    exit $BUILD_EXIT_CODE
fi
