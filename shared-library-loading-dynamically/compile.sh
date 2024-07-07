#!/usr/bin/bsh

rm -rf build; mkdir build

# Generate a object file with Position-Independent Code
gcc -c -fPIC example.c -o build/example.o

# Link the Object File into a Shared Library
gcc -shared -o build/libexample.so build/example.o

# Compile the Main with libdl library
gcc -o build/main main.c -ldl

# Tell the system where to find the shared library
export LD_LIBRARY_PATH=./build

# Run the main
build/main