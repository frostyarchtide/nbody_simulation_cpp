#!/bin/bash

if [ ! -d "build" ]; then
    meson setup build --buildtype=debugoptimized
fi

meson compile -C build
build/nbody_simulation
