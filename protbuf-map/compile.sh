#!/usr/bin/bash

rm -rf build; mkdir build

protoc --c_out=build map_with_http.proto
gcc -g  -o build/main main.c  build/map_with_http.pb-c.c -lprotobuf-c

./build/main