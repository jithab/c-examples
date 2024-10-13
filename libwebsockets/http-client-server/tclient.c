#include <libwebsockets.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define BUFFER_SIZE 1024 * 100

struct my_user_data
{
    char response_buffer[BUFFER_SIZE + 1]; // Buffer to store the response
    size_t response_length;                // Current length of the response
};
static int callback_http(struct lws *wsi, enum lws_callback_reasons reason,
                         void *user, void *in, size_t len)
{
    struct my_user_data *data = (struct my_user_data *)user;

    switch (reason)
    {
    case LWS_CALLBACK_CLIENT_CONNECTION_ERROR:
        lwsl_warn("Connection error\n");
        break;
    case LWS_CALLBACK_COMPLETED_CLIENT_HTTP:
        lwsl_warn("ccccLWS_CALLBACK_COMPLETED_CLIENT_HTTP\n");
        break;

    case LWS_CALLBACK_RECEIVE_CLIENT_HTTP:
    {
        if (data->response_length >= BUFFER_SIZE)
        {
            data->response_length = 0; // In case of overflow, just overwrite!
        }

        int availableLength = BUFFER_SIZE - data->response_length;
        int n = 0;
        char *buf_ptr = (char *)(&data->response_buffer[data->response_length]);

        n = lws_http_client_read(wsi, &buf_ptr, &availableLength);
        if (n < 0)
        {
            return -1; // Indicate error in case of read failure
        }
    }
    break;
    case LWS_CALLBACK_RECEIVE_CLIENT_HTTP_READ:
        data->response_length += len;
        data->response_buffer[data->response_length] = '\0';
        break;
    default:
        break;
    }
    return 0;
}

// Declare the protocols array
static struct lws_protocols protocols[] = {
    {
        "http",        // Name of the protocol
        callback_http, // Callback function
        0,             // User data per session TODO why not used?
        0,             // RX buffer size
    },
    {NULL, NULL, 0, 0} // Terminator
};

int httpRequest(char *address, int port, struct my_user_data **user_data)
{
    lws_set_log_level(LLL_ERR | LLL_WARN | LLL_USER, NULL);

    struct lws_context_creation_info ctx_info;
    struct lws_client_connect_info conn_info;
    struct lws_context *context;
    struct lws *wsi;
    // Initialize libwebsockets context
    memset(&ctx_info, 0, sizeof(ctx_info));
    ctx_info.port = CONTEXT_PORT_NO_LISTEN;
    ctx_info.protocols = protocols; // Assign the protocols array
    context = lws_create_context(&ctx_info);
    if (context == NULL)
    {
        fprintf(stderr, "Failed to create context\n");
        return -1;
    }

    // Initialize connection info
    memset(&conn_info, 0, sizeof(conn_info));
    conn_info.context = context;
    conn_info.address = address; // Server address
    conn_info.port = port;       // HTTP port
    conn_info.path = "/";        // HTTP GET path
    conn_info.host = lws_canonical_hostname(context);
    conn_info.origin = "origin";
    conn_info.protocol = protocols[0].name;
    conn_info.method = "GET"; // Set method to HTTP GET
    conn_info.ietf_version_or_minus_one = -1;
    conn_info.ssl_connection = 0; // No SSL

    conn_info.userdata = *user_data;

    wsi = lws_client_connect_via_info(&conn_info);
    if (wsi == NULL)
    {
        fprintf(stderr, "Connection failed\n");
        lws_context_destroy(context);
        return -1;
    }

    // Event loop
    while (lws_service(context, 1000) >= 0 && lws_get_socket_fd(wsi) >= 0)
    {
    }

    // Clean up
    lws_context_destroy(context);
    return 0;
}

int main(void)
{

    struct my_user_data *user_data = malloc(sizeof(struct my_user_data));
    user_data->response_length = 0; // Initialize response length
    httpRequest("127.0.0.1", 8080, &user_data);

    printf("Got data: %s.@\n", user_data->response_buffer);
}
