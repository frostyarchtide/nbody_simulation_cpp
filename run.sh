#!/bin/bash

if [ ! -d "build" ]; then
    meson setup build
fi

meson compile -C build
build/nbody_simulation
