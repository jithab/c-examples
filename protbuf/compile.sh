#!/usr/bin/bash

rm -rf build; mkdir build

protoc --c_out=build user.proto
gcc  -o build/main main.c  build/user.pb-c.c -lprotobuf-c

./build/main