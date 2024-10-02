#!/usr/bin/bash

rm -rf build; mkdir build

protoc --c_out=build user.proto
gcc server.c -o build/server build/user.pb-c.c -lprotobuf-c
gcc client.c -o build/client build/user.pb-c.c -lprotobuf-c


echo "Start client your self. Command: ./build/client"
./build/server