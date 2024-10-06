#include <libwebsockets.h>
#include <string.h>

#define FILE_NAME "received_file.bmp" // File to be received

// Client state
struct per_session_data__client {
    struct lws *wsi;
    FILE *file;
};

static int callback_client(struct lws *wsi, enum lws_callback_reasons reason, void *user,
                           void *in, size_t len) {

    struct per_session_data__client *pss = (struct per_session_data__client *)user;

    switch (reason) {
        case LWS_CALLBACK_CLIENT_ESTABLISHED:
            lwsl_warn("status: LWS_CALLBACK_CLIENT_ESTABLISHED\n");
            
            // Connection established
            pss->file = fopen(FILE_NAME, "wb");
            if (!pss->file) {
                lwsl_err("Failed to open file for writing\n");
                return -1;
            }
            break;

        case LWS_CALLBACK_CLIENT_WRITEABLE: {
            lwsl_warn("status: LWS_CALLBACK_CLIENT_WRITEABLE");
            char *msg = "Hello, Server!";
            unsigned char buf[LWS_PRE + strlen(msg)];
            memcpy(&buf[LWS_PRE], msg, strlen(msg));
            lws_write(wsi, &buf[LWS_PRE], strlen(msg), LWS_WRITE_TEXT);
            break;
        }
        case LWS_CALLBACK_CLIENT_RECEIVE:
            // Receive data from the server and write to the file
            if (pss->file) {
                lwsl_warn("Got something to write.\n");
                fwrite(in, 1, len, pss->file);
            }
            break;

        case LWS_CALLBACK_CLIENT_CLOSED:
            // Close file when done
            if (pss->file) {
                fclose(pss->file);
                pss->file = NULL;
            }
            lwsl_warn("Connection closed\n");
            break;

        default:
            break;
    }
    return 0;
}
static struct lws_protocols protocols[] = {
    {
        "echo-protocol",
        callback_client,
        sizeof(struct per_session_data__client) ,
        4096,
    },
    { NULL, NULL, 0, 0 }  // terminator
};

int main(void) {
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
    lws_set_log_level(LLL_ERR | LLL_WARN , NULL);

    //
    ccinfo.context = context;
    ccinfo.address = "localhost";
    ccinfo.port = 8081;
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
