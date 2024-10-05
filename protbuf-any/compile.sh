#!/usr/bin/bash

rm -rf build; mkdir build

protoc --proto_path=protobuf-messages --c_out=build protobuf-messages/any_with_http.proto protobuf-messages/google/protobuf/any.proto
gcc -Ibuild -g -o build/main main.c  build/any_with_http.pb-c.c build/google/protobuf/any.pb-c.c -lprotobuf-c

./build/main