#!/usr/bin/bsh

rm -rf build; mkdir build

gcc -o build/server server.c
gcc -o build/client client.c
