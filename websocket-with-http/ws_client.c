#include <libwebsockets.h>
#include <string.h>
#define PORT 8080
#define BUFFER_SIZE 1024
static int callback_client(struct lws *wsi, enum lws_callback_reasons reason, void *user,
                           void *in, size_t len) {
    switch (reason) {
        case LWS_CALLBACK_CLIENT_ESTABLISHED:
            lws_callback_on_writable(wsi);
            break;
        case LWS_CALLBACK_CLIENT_WRITEABLE: {
            char *msg = "Hello, Server!";
            unsigned char buf[LWS_PRE + strlen(msg)];
            memcpy(&buf[LWS_PRE], msg, strlen(msg));
            lws_write(wsi, &buf[LWS_PRE], strlen(msg), LWS_WRITE_TEXT);
            break;
        }
        case  LWS_CALLBACK_CLIENT_RECEIVE:
            printf("Received: %.*s\n", (int)len, (char *)in);
            lws_close_reason(wsi, LWS_CLOSE_STATUS_NORMAL, NULL, 0);
            return -1;
        default:
            break;
    }
    return 0;
}

static struct lws_protocols protocols[] = {
    {
        "echo-protocol",
        callback_client,
        0,
        4096,
    },
    { NULL, NULL, 0, 0 }  // terminator
};

int websocket() {
    struct lws_context_creation_info info;
    struct lws_client_connect_info ccinfo = {0};
    struct lws_context *context;
    struct lws *wsi;

    memset(&info, 0, sizeof(info));
    info.port = CONTEXT_PORT_NO_LISTEN;
    info.protocols = protocols;

    context = lws_create_context(&info);
    if (!context) {
        fprintf(stderr, "lws init failed\n");
        return -1;
    }

    ccinfo.context = context;
    ccinfo.address = "localhost";
    ccinfo.port = PORT;
    ccinfo.path = "/";
    ccinfo.host = lws_canonical_hostname(context);
    ccinfo.origin = "origin";
    ccinfo.protocol = protocols[0].name;

    wsi = lws_client_connect_via_info(&ccinfo);
    if (!wsi) {
        fprintf(stderr, "Client connection failed\n");
        lws_context_destroy(context);
        return -1;
    }

    while (1) {
        lws_service(context, 1000);
    }

    lws_context_destroy(context);

    return 0;
}


int tcp() {
    int sock = 0;
    struct sockaddr_in serv_addr;
    char buffer[BUFFER_SIZE] = {0};
    const char *hello = "Hello from client";

    // Create socket file descriptor
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Socket creation error \n");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Convert IPv4 and IPv6 addresses from text to binary form
    if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }

    // Connect to the server
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("\nConnection Failed \n");
        return -1;
    }

    // Send message to server
    send(sock, hello, strlen(hello), 0);
    printf("Hello message sent\n");

    // Read response from server
    int valread = read(sock, buffer, BUFFER_SIZE);
    printf("Server: %s\n", buffer);

    // Close the socket
    close(sock);
    return 0;
}

int main(void) {
    tcp();
    websocket();

}
