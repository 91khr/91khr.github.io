#!/bin/env bash

cd $(readlink -f $(dirname $0))

# Check standard support
g++ -std=c++20 -E -x c++ /dev/null >/dev/null 2>&1 && std=c++20 || std=c++17

function runmake() {
    make -f - <<<"$1: $1.cpp; g++ -std=${std} $1.cpp -o $1 ${extargs}"
}

# Peek environment and refresh timestamps
if [ "$1" == "deploy" ]; then
    shift
    extargs=
else
    extargs=-g
    # Run timestamp updater
    if runmake dev/tmstamp; then
        git status -s --no-renames src | ./dev/tmstamp || exit $?
    else
        exit $?
    fi
fi

# Run build executable
if runmake dev/build; then
    echo End compilation
    mkdir -p out
    cp -r etc/* out
    exec ./dev/build $@
else
    exit $?
fi
