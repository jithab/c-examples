#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define SERVER_IP "127.0.0.1"  // Server's IP address
#define SERVER_PORT 8080       // Server's port
#define BUFFER_SIZE 1024       // Buffer size for receiving file data
//#define OUTPUT_FILE "received_file.bmp"  // Name of the file to save

// Function to receive a file from the server
void receiveFile(int socket, char *outputfile) {
    FILE *file;
    char buffer[BUFFER_SIZE];
    int bytesReceived;

    // Open a file to write the received data
    file = fopen(outputfile, "wb");
    if (file == NULL) {
        printf("Error: Could not open file to save data.\n");
        return;
    }

    printf("Receiving file...\n");

    // Receive file data from the server and write it to the file
    while ((bytesReceived = recv(socket, buffer, BUFFER_SIZE, 0)) > 0) {
        fwrite(buffer, sizeof(char), bytesReceived, file);
    }

    printf("File received successfully and saved as '%s'.\n", outputfile);

    // Close the file
    fclose(file);
}

int tcp_client(char *outputfile) {
    int clientSocket;
    struct sockaddr_in serverAddr;

    // Create the socket
    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket < 0) {
        perror("Error creating socket");
        exit(EXIT_FAILURE);
    }

    // Set up the server address structure
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(SERVER_PORT);
    serverAddr.sin_addr.s_addr = inet_addr(SERVER_IP);

    // Connect to the server
    if (connect(clientSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) {
        perror("Error connecting to server");
        close(clientSocket);
        exit(EXIT_FAILURE);
    }

    printf("Connected to server.\n");

    // Receive the file from the server
    receiveFile(clientSocket, outputfile);

    // Close the socket
    close(clientSocket);

    return 0;
}
