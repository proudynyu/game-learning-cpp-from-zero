#!/bin/bash

libs=-luser32
warnings="-Wno-writable-strings -Wno-format-security"
name=gamelearning

if [[ "$OSTYPE" == "win32" || "$OSTYPE" == "win64" ]]; then
    # libs="$libs -lgdi32 -lwinmm -limm32 -lole32 -loleaut32 -lversion -luuid"
    # warning="$warning -Wno-write-strings"
    name="$name.exe"
fi

g++ -g src/main.cpp -o $name $libs $warning
