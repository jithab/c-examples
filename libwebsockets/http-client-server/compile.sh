#!/usr/bin/bsh

rm -rf build; mkdir build
export LD_LIBRARY_PATH=/usr/local/lib:$LD_LIBRARY_PATH
echo "Install: sudo apt install libwebsockets-dev"

gcc -g -o build/server postserver.c -lwebsockets
gcc -g -o build/client postclient.c -lwebsockets

echo "Run: build/client"

#python3 -m http.server 8080
./build/server