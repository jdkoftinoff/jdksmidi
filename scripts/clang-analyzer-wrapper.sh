#!/bin/bash
#
# Clang Static Analyzer wrapper script for jdksmidi
# This script runs the clang static analyzer on source files during compilation
#

# Check if we have the right number of arguments
if [ $# -eq 0 ]; then
    echo "Usage: $0 <compiler> [compiler-args...]"
    exit 1
fi

# Get the compiler and its arguments
COMPILER="$1"
shift
ARGS="$@"

# Check if clang static analyzer is available
if ! command -v clang >/dev/null 2>&1; then
    echo "Warning: clang not found, running normal compilation"
    exec "$COMPILER" $ARGS
fi

# Extract source files from arguments
SOURCE_FILES=""
for arg in $ARGS; do
    case "$arg" in
        *.cpp|*.cc|*.cxx|*.c)
            SOURCE_FILES="$SOURCE_FILES $arg"
            ;;
    esac
done

# Run static analyzer on source files
if [ -n "$SOURCE_FILES" ]; then
    echo "Running clang static analyzer on:$SOURCE_FILES"
    clang --analyze -Xanalyzer -analyzer-output=text $ARGS 2>&1 | grep -v "note: "
fi

# Always run the normal compilation
exec "$COMPILER" $ARGS