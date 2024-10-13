#include <libwebsockets.h>
#include <string.h>
#define FILE_NAME "test.bmp" // File to be sent
char buffer[1024*1024];
#define MAX_BUF_SIZE 1024*1024


struct per_session_data_http {
    FILE *file; // File pointer
    char buffer[MAX_BUF_SIZE]; // Buffer to hold file content
    size_t buffer_len; // Length of data in buffer
};

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
    }
                lwsl_warn("xxx LWS reason: %d\n", reason);
    return 0;
}


// HTTP callback function
static int callback_http(struct lws *wsi, enum lws_callback_reasons reason,
                         void *user, void *in, size_t len) {
    struct per_session_data_http *pss = (struct per_session_data_http *)user;
    
    switch (reason) {

    case LWS_CALLBACK_FILTER_NETWORK_CONNECTION:
    case LWS_CALLBACK_WSI_CREATE:
    case LWS_CALLBACK_CHANGE_MODE_POLL_FD:
    case LWS_CALLBACK_LOCK_POLL:
    case LWS_CALLBACK_UNLOCK_POLL:
    case LWS_CALLBACK_ADD_POLL_FD:
    case LWS_CALLBACK_SERVER_NEW_CLIENT_INSTANTIATED:
    case LWS_CALLBACK_DEL_POLL_FD:
    case LWS_CALLBACK_HTTP_BIND_PROTOCOL:
    case LWS_CALLBACK_EVENT_WAIT_CANCELLED:
    case LWS_CALLBACK_FILTER_HTTP_CONNECTION:
    case LWS_CALLBACK_HTTP_DROP_PROTOCOL:
        return 0;
    case LWS_CALLBACK_HTTP: {
                lwsl_warn("STATUS: LWS_CALLBACK_HTTP\n");
            lwsl_user("HTTP request received, serving file: %s\n", FILE_NAME);

            pss->file = fopen(FILE_NAME, "rb"); // Open the file
            if (!pss->file) {
                lwsl_err("Failed to open file: %s\n", FILE_NAME);
                lws_return_http_status(wsi, HTTP_STATUS_INTERNAL_SERVER_ERROR, NULL);
                return -1;
            }

            lws_callback_on_writable(wsi); // Make socket writable to start sending file
            break;
        break;
    }

    case LWS_CALLBACK_HTTP_WRITEABLE: {
        lwsl_warn("STATUS: LWS_CALLBACK_HTTP_WRITEABLE\n");
            if (pss->file) {
                // Read chunk from file
                pss->buffer_len = fread(pss->buffer, 1, sizeof(pss->buffer), pss->file);

                // If no more data, close the file and connection
                if (pss->buffer_len == 0) {
                    fclose(pss->file);
                    pss->file = NULL;
                    lws_callback_on_writable(wsi);
                    return -1; // Close the connection after sending the file
                }

                // Send the chunk to the client
                lws_write(wsi, (unsigned char *)pss->buffer, pss->buffer_len, LWS_WRITE_HTTP);
                lws_callback_on_writable(wsi); // Continue writing until the file is done
            }
        break;
    }

    case LWS_CALLBACK_CLOSED_HTTP: {
                lwsl_warn("STATUS: LWS_CALLBACK_CLOSED_HTTP\n");
            if (pss->file) {
                fclose(pss->file);
                pss->file = NULL;
            }
            break;
    }

    case LWS_CALLBACK_WSI_DESTROY:
        lwsl_warn("STATUS: LWS_CALLBACK_WSI_DESTROY\n");
            break;
        default:
            lwsl_warn("HTTP LWS reason: %d\n", reason);
            break;
    }
       
    return 0;
}


static struct lws_protocols protocols[] = {
    {
        "http-only",   // HTTP protocol
        callback_http, // Callback function for HTTP
        sizeof(struct per_session_data_http), // Size of per-session data for HTTP
        MAX_BUF_SIZE,
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
    info.options = LWS_SERVER_OPTION_HTTP_HEADERS_SECURITY_BEST_PRACTICES_ENFORCE;

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
