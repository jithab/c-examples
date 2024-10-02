#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <linux/can.h>
#include <linux/can/raw.h>

int main() {
    int s;
    struct sockaddr_can addr;
    struct ifreq ifr;
    struct can_frame frame;

    // Open a socket
    if ((s = socket(PF_CAN, SOCK_RAW, CAN_RAW)) < 0) {
        perror("Socket");
        return 1;
    }

    // Specify the CAN interface
    strcpy(ifr.ifr_name, "vcan0");
    ioctl(s, SIOCGIFINDEX, &ifr);

    // Bind the socket to the CAN interface
    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;
    if (bind(s, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("Bind");
        return 1;
    }

    for(int i=0; i < 4; i++) {

        // Prepare a CAN frame
        frame.can_id = i; // CAN ID
        frame.can_dlc = 8;    // Data length code (number of bytes)
        for (int i = 0; i < frame.can_dlc; i++) {
            frame.data[i] = i;
        }
        
        // Send the CAN frame
        if (write(s, &frame, sizeof(struct can_frame)) != sizeof(struct can_frame)) {
            perror("Write");
            return 1;
        }
    }
    // Close the socket
    close(s);

    return 0;
}
