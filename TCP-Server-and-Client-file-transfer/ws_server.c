#include <libwebsockets.h>
#include <string.h>

#define FILE_NAME "test.bmp" // File to be sent

static int callback_echo(struct lws *wsi, enum lws_callback_reasons reason, void *user,
                         void *in, size_t len) {
    static int sent_file = 0;
    FILE *file = NULL;

    switch (reason) {
        case LWS_CALLBACK_SERVER_WRITEABLE: 
            lwsl_warn("status: LWS_CALLBACK_SERVER_WRITEABLE.\n");
            // Check if the file has been sent already
            if (!sent_file) {
                file = fopen(FILE_NAME, "rb");
                if (!file) {
                    lwsl_err("Failed to open file for reading\n");
                    return -1;
                }

                // Read and send the file content
                char buffer[512];
                size_t bytesRead;
                int i=0;
                while ((bytesRead = fread(buffer, 1, sizeof(buffer), file)) > 0) {
                    lwsl_warn("count: %d\n", i++);
                    lws_write(wsi, (unsigned char *)buffer, bytesRead, LWS_WRITE_BINARY);
                }
                
                fclose(file);
                sent_file = 1; // Mark the file as sent
                lws_close_reason(wsi, LWS_CLOSE_STATUS_NORMAL, "123", 3); // Close connection
            }
            break;

        case LWS_CALLBACK_CLOSED:
            lwsl_warn("Client disconnected\n");
            break;

        default:
            break;
    }
        char str[30];  // Buffer to hold the string representation of the integer

    // Convert integer to string
    sprintf(str, "SXXX: ");//   , reason);
                lwsl_warn("SSSS: %d\n", reason);
    return 0;
}


static struct lws_protocols protocols[] = {
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

    info.port = 8081;
    info.protocols = protocols;

    lws_set_log_level(LLL_ERR | LLL_WARN , NULL);
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
