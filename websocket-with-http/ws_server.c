#include <libwebsockets.h>
#include <string.h>


static int callback_http(struct lws *wsi, enum lws_callback_reasons reason, void *user,
                         void *in, size_t len) {
    switch (reason) {
        case LWS_CALLBACK_RECEIVE_CLIENT_HTTP_READ:
            printf("Received: %.*s\n", (int)len, (char *)in);
            // Echo the received message back to the client            
            lws_write(wsi, (unsigned char *)in, len, LWS_WRITE_TEXT);
            break;
        default:
            break;
    }
    return 0;
}

static int callback_echo(struct lws *wsi, enum lws_callback_reasons reason, void *user,
                         void *in, size_t len) {
    switch (reason) {
        case LWS_CALLBACK_RECEIVE:
            printf("Received: %.*s\n", (int)len, (char *)in);
            // Echo the received message back to the client
            unsigned char *t = strdup("123456789");            
            lws_write(wsi, t, 9, LWS_WRITE_TEXT);
            break;
        default:
            break;
    }
    return 0;
}

static struct lws_protocols protocols[] = {
    {
        "http-protocol",
        callback_http,
        0,
        4096,
    },

    {
        "echo-protocol",
        callback_echo,
        0,
        4096,
    },
    { NULL, NULL, 0, 0 }  // terminator
};

int main(void) {
    struct lws_context_creation_info info;
    memset(&info, 0, sizeof(info));

    info.port = 8080;
    info.protocols = protocols;

    struct lws_context *context = lws_create_context(&info);
    if (!context) {
        fprintf(stderr, "lws init failed\n");
        return -1;
    }

    while (1) {
        lws_service(context, 1000);
    }

    lws_context_destroy(context);

    return 0;
}
