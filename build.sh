#!/bin/bash

std=-std=c++11
libs="-luser32 -lopengl32"
includes="-Ithird_party -Ithird_party/Include"
warnings="-Wno-write-strings -Wno-format-security"
name=gamelearning

if [[ "$OSTYPE" == "win32" || "$OSTYPE" == "win64" ]]; then
    # libs="$libs -lgdi32 -lwinmm -limm32 -lole32 -loleaut32 -lversion -luuid"
    # warning="$warning -Wno-write-strings"
    name="$name.exe"
fi

echo "Building $name..."
g++ $std $includes -g src/main.cpp -o $name $libs $warnings

if [[ $? -ne 0 ]]; then
    echo "Build failed."
    exit 1
fi

echo "Build successful."
echo "Running $name..."
./$name