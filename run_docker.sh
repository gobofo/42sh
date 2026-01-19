#!/bin/bash

IMAGE_NAME="42sh-env"
CONTAINER_APP_DIR="/app"

echo "--- Building Docker Image ---"
docker build -q -t $IMAGE_NAME . > /dev/null
if [ $? -ne 0 ]; then 
    echo "Error: Docker build failed"
    exit 1
fi

if [ "$1" == "check" ]; then
    echo "--- Running Testsuite ---"
    CMD_TO_RUN="BIN_PATH=\$(pwd)/src/42sh OUTPUT_FILE=\$(pwd)/out make check"
    INTERACTIVE_FLAGS="-i"
elif [ $# -gt 0 ]; then
    echo "--- Running Command: ./src/42sh '$@' ---"
    CMD_TO_RUN="./src/42sh $@"
    INTERACTIVE_FLAGS="-i"
else
    echo "--- Entering Interactive Mode ---"
    CMD_TO_RUN="./src/42sh"
    INTERACTIVE_FLAGS="-it"
fi

docker run $INTERACTIVE_FLAGS --rm -v "$(pwd):$CONTAINER_APP_DIR" $IMAGE_NAME bash -c '
    find . -name ".deps" -type d -exec rm -rf {} + > /dev/null 2>&1
    rm -rf autom4te.cache > /dev/null 2>&1
    
    autoreconf -i > /dev/null 2>&1 && \
    ./configure CFLAGS="-std=c99 -pedantic -Werror -Wall -Wextra -Wvla -g -O0 -fsanitize=address" \
                LDFLAGS="-fsanitize=address" > /dev/null 2>&1 && \

    make -s > /dev/null 2>&1 || exit 1
    
    ./src/42sh "$@"
' -- "$@"

if [ "$1" == "check" ] && [ -f "out" ]; then
    echo "--- Test Results: \$(cat out)% ---"
    rm out
fi
