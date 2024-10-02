#!/usr/bin/bsh

rm -rf build; mkdir build

gcc -o build/can_send can_send.c
gcc -o build/can_receive can_receive.c

# Setting up virtual can
sudo modprobe vcan
sudo ip link add dev vcan0 type vcan
sudo ip link set up vcan0

echo -e "\n\n\nRun can sender in another shell: build/can_send"
# Run receiver
sudo build/can_receive
