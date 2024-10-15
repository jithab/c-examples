#!/usr/bin/bsh

rm -rf build; mkdir build

gcc -o build/main main.c -lzlog

./build/main

exit

# Install zlog into system
git clone https://github.com/HardySimpson/zlog.git
cd zlog
make
sudo make install
sudo ldconfig