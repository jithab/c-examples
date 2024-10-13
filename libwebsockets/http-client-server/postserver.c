#include <libwebsockets.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

struct my_user_data
{
    char response_buffer[1024]; // Buffer for received data
    size_t buffer_len;          // Length of received data
};

static int callback_http(struct lws *wsi, enum lws_callback_reasons reason, void *user, void *in, size_t len)
{
    struct my_user_data *data = (struct my_user_data *)user;

    switch (reason)
    {
    case LWS_CALLBACK_HTTP:
    {
        lwsl_warn("LWS_CALLBACK_HTTP: %d\n", reason);
        char method[255]; // Define method as a string buffer
        // Get the HTTP method string from the headers
        if (lws_hdr_copy(wsi, method, sizeof(method), WSI_TOKEN_GET_URI) > 0)
        {
            lwsl_warn("WSI_TOKEN_GET_URI: %s\n", method);
            // Handle GET request
            const char *response = "HTTP/1.1 200 OK\r\n"
                                   "Content-Type: text/html\r\n"
                                   "Content-Length: 32\r\n"
                                   "Connection: close\r\n"
                                   "\r\n"
                                   "<h1>Welcome to HTTP Server!</h1>";
            lws_write(wsi, (unsigned char *)response, strlen(response), LWS_WRITE_HTTP);
            return -1; // Close the connection after sending the response
        }
        else if (lws_hdr_copy(wsi, method, sizeof(method), WSI_TOKEN_POST_URI) > 0)
        {
            lwsl_warn("WSI_TOKEN_POST_URI: %s\n", method);

            // Handle POST request (wait for body data to be received)
            data->buffer_len = 0; // Reset buffer for POST data
            return 0;
        }
        break;
    }

    case LWS_CALLBACK_HTTP_BODY:
    {
        lwsl_warn("LWS_CALLBACK_HTTP_BODY: %d\n", reason);
        // Store the received body data
        if (data->buffer_len + len < sizeof(data->response_buffer) - 1)
        {
            strncat(data->response_buffer, (const char *)in, len);
            data->buffer_len += len;
            data->response_buffer[data->buffer_len] = '\0'; // Null-terminate the string
        }
        break;
    }

    case LWS_CALLBACK_HTTP_BODY_COMPLETION:
    {
        lwsl_warn("LWS_CALLBACK_HTTP_BODY_COMPLETION: %d\n", reason);
        const char *response = "HTTP/1.1 200 OK\r\n"
                               "Content-Type: text/html\r\n"
                               "Content-Length: 32\r\n"
                               "Connection: close\r\n"
                               "\r\n"
                               "<h1>Welcome to POST Server!</h1>";
        lws_write(wsi, (unsigned char *)response, strlen(response), LWS_WRITE_HTTP);
        return 0; // Close the connection after sending the response
    }

    case LWS_CALLBACK_HTTP_WRITEABLE:
    {
        lwsl_warn("LWS_CALLBACK_HTTP_WRITEABLE: %d\n", reason);
        // If you want to handle writable events, you can do so here
        break;
    }

    case LWS_CALLBACK_CLOSED_HTTP:
        lwsl_warn("LWS_CALLBACK_CLOSED_HTTP: %d\n", reason);
        break;

    default:
        lwsl_warn("reason: %d\n", reason);
        break;
    }
    return 0;
}

static struct lws_protocols protocols[] = {
    {
        "http",
        callback_http,
        sizeof(struct my_user_data), // User data size
        0,                           // RX buffer size
    },
    {NULL, NULL, 0, 0} // Terminator
};

int main(void)
{
    struct lws_context_creation_info ctx_info;
    struct lws_context *context;
    // lws_set_log_level(LLL_ERR | LLL_WARN | LLL_USER | LLL_INFO, NULL);
    lws_set_log_level(LLL_ERR | LLL_WARN, NULL);

    // Initialize context
    memset(&ctx_info, 0, sizeof(ctx_info));
    ctx_info.port = 8080; // Change this to your desired port
    ctx_info.protocols = protocols;

    context = lws_create_context(&ctx_info);
    if (context == NULL)
    {
        fprintf(stderr, "Failed to create context\n");
        return -1;
    }

    // Event loop
    while (1)
    {
        lws_service(context, 1000); // Process events
    }

    lws_context_destroy(context); // Clean up
    return 0;
}
