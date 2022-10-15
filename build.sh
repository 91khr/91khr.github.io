#!/bin/env bash

cd $(readlink -f $(dirname $0))

# Check standard support
g++ -std=c++20 -E -x c++ /dev/null >/dev/null 2>&1 && std=c++20 || std=c++17

function runmake() {
    f="$1"; shift
    make -f - <<<"$f: $f.cpp; g++ -std=${std} $f.cpp -o $f ${extargs} $@"
}

# Peek environment and refresh timestamps
if [ "$1" == "deploy" ]; then
    shift
    extargs=
else
    extargs="$extargs -g"
    # Run timestamp updater
    if runmake dev/tmstamp; then
        git status -s --no-renames --untracked-files src | ./dev/tmstamp || exit $?
    else
        exit $?
    fi
fi

# Run build executable
if runmake dev/build -llua; then
    echo End compilation
    mkdir -p out
    cp -r etc/* out
    exec ./dev/build $@
else
    exit $?
fi
