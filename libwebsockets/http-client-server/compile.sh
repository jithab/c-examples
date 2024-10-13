#!/usr/bin/bsh

rm -rf build; mkdir build
export LD_LIBRARY_PATH=/usr/local/lib:$LD_LIBRARY_PATH
echo "Install: sudo apt install libwebsockets-dev"

gcc -g -o build/server postserver.c -lwebsockets
gcc -g -o build/client postclient.c -lwebsockets


#gcc -g -o build/server server.c -lwebsockets
#gcc -g -o build/client client.c -lwebsockets

echo "Run: build/client"

# python3 -m http.server 8080
# curl -X POST http://127.0.0.1:8080 -d "key=value"
# curl -v http://127.0.0.1:8080

./build/server