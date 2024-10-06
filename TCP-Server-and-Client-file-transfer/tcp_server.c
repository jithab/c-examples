#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <arpa/inet.h>

#define PORT 8080            // The port the server will listen on
#define BUFFER_SIZE 1024     // Buffer size for sending file data
#define FILE_NAME "test.bmp"  // The file to send

int serverSocket;  // Global declaration for serverSocket to use in the signal handler

void handle_sigint(int sig) {
    printf("\nCaught signal %d (SIGINT). Shutting down server...\n", sig);
    close(serverSocket);  // Close the server socket to stop accept() from blocking
    exit(0);              // Exit the program
}

// Function to send a file to the client
void sendFile(int clientSocket) {
    FILE *file;
    char buffer[BUFFER_SIZE];
    int bytesRead;

    // Open the file to read
    file = fopen(FILE_NAME, "rb");
    if (file == NULL) {
        printf("Error: Could not open file %s.\n", FILE_NAME);
        return;
    }

    printf("Sending file '%s' to client...\n", FILE_NAME);

    // Read from file and send the contents to the client
    while ((bytesRead = fread(buffer, sizeof(char), BUFFER_SIZE, file)) > 0) {
        send(clientSocket, buffer, bytesRead, 0);
    }

    printf("File sent successfully!\n");

    // Close the file
    fclose(file);
}

int main() {
    int clientSocket;
    struct sockaddr_in serverAddr, clientAddr;
    socklen_t addrSize;
    char clientIP[INET_ADDRSTRLEN];

    // Create the server socket
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket < 0) {
        perror("Error creating socket");
        exit(EXIT_FAILURE);
    }

    // Configure the server address structure
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    // Bind the socket to the port
    if (bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) {
        perror("Error binding socket");
        close(serverSocket);
        exit(EXIT_FAILURE);
    }
    // Start listening for incoming connections
    if (listen(serverSocket, 5) < 0) {
        perror("Error listening");
        close(serverSocket);
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d...\n", PORT);
    signal(SIGINT, handle_sigint);
    while(1) {

        // Accept a connection from a client
        addrSize = sizeof(clientAddr);
        clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddr, &addrSize);
        if (clientSocket < 0) {
            perror("Error accepting connection");
            close(serverSocket);
            exit(EXIT_FAILURE);
        }

        // Convert and print the client's IP address
        inet_ntop(AF_INET, &clientAddr.sin_addr, clientIP, sizeof(clientIP));
        printf("Connection accepted from %s:%d\n", clientIP, ntohs(clientAddr.sin_port));

        // Send the file to the client
        sendFile(clientSocket);

        // Close the client and server sockets
        close(clientSocket);
    }
    close(serverSocket);

    return 0;
}
