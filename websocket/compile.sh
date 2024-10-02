#!/usr/bin/bsh

rm -rf build; mkdir build

echo "Install: sudo apt install libwebsockets-dev"

gcc -o build/ws_server ws_server.c -lwebsockets
gcc -o build/ws_client ws_client.c -lwebsockets

echo "Run: build/ws_client"

./build/ws_server