#!/bin/bash

if [ ! -d "build" ]; then
    meson setup build --buildtype=debugoptimized
fi

if [ "$1" == "configure" ]; then
    if [ -z "$2" ]; then
        meson configure build -Dglfw:display-api=auto
    else
        meson configure build -Dglfw:display-api=$2
    fi
fi

meson compile -C build && build/nbody_simulation
