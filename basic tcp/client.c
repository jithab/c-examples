// client.c
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "build/user.pb-c.h"

#define PORT 8080

int main() {
    int sock = 0;
    struct sockaddr_in serv_addr;

    // Create socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation error");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Convert IPv4 address from text to binary form
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        perror("Invalid address/ Address not supported");
        return -1;
    }

    // Connect to the server
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("Connection Failed");
        return -1;
    }

    void *buf;    // Buffer to store serialized data.
    unsigned len; // Length of serialized data.

    // Prepare the data.
    int32_t id = 123;
    char *name = "MyNameIsProtobuf!";

    // Serialize the data.
    User sendData = USER__INIT;
    sendData.id = id;
    sendData.name = name;
    len = user__get_packed_size(&sendData);
    buf = malloc(len);
    user__pack(&sendData, buf);

    // Send message to server
    send(sock, buf, len, 0);
    printf("Protobuf pMessage sent!\n");

    // Close the socket
    close(sock);

    return 0;
}
