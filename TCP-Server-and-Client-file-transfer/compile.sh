#!/usr/bin/bsh

rm -rf build; mkdir -p build/test_tcp build/test_ws

gcc -g main.c create_bmp_image.c  tcp_client.c
gcc -g tcp_server.c -o build/tcp_server 

gcc -g ws_server.c -o build/ws_server -lwebsockets 
gcc -g ws_client.c -o build/ws_client -lwebsockets 
