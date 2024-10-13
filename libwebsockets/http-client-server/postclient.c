#include <libwebsockets.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define BUFFER_SIZE 1024 * 100

struct my_user_data
{
    char response_buffer[BUFFER_SIZE + 1]; // Buffer to store the response
    size_t response_length;                // Current length of the response
    const char *post_data;                 // POST data to be sent
};

static int callback_http(struct lws *wsi, enum lws_callback_reasons reason, void *user, void *in, size_t len)
{
    struct my_user_data *data = (struct my_user_data *)user;

    switch (reason)
    {
    case LWS_CALLBACK_CLIENT_APPEND_HANDSHAKE_HEADER:
    {
        lwsl_warn("Connection LWS_CALLBACK_CLIENT_APPEND_HANDSHAKE_HEADER\n");

        // Prepare POST headers
        unsigned char **p = (unsigned char **)in;
        unsigned char *end = (*p) + len;
        const char *content_type = "Content-Type: application/x-www-form-urlencoded\r\n";
        const char *content_length = "Content-Length: 13\r\n"; // Adjust this as per your POST data length

        if (lws_add_http_header_by_name(wsi, (unsigned char *)"Content-Type:", (unsigned char *)content_type, strlen(content_type), p, end))
        {
            return -1;
        }
        if (lws_add_http_header_by_name(wsi, (unsigned char *)"Content-Length:", (unsigned char *)content_length, strlen(content_length), p, end))
        {
            return -1;
        }
        break;
    }

    case LWS_CALLBACK_CLIENT_WRITEABLE:
    {
        lwsl_warn("Connection LWS_CALLBACK_CLIENT_WRITEABLE\n");
        // Send POST data
        const char *post_data = "key=value";
        lws_write(wsi, (unsigned char *)post_data, strlen(post_data), LWS_WRITE_HTTP_FINAL);
        break;
    }

    case LWS_CALLBACK_CLIENT_RECEIVE:
    {
        lwsl_warn("Connection LWS_CALLBACK_CLIENT_RECEIVE\n");
        // Receive the response
        strncat(data->response_buffer, (const char *)in, len);
        data->response_length += len;
        break;
    }

    case LWS_CALLBACK_CLIENT_CONNECTION_ERROR:
        lwsl_warn("Connection error\n");
        break;

    case LWS_CALLBACK_COMPLETED_CLIENT_HTTP:
        lwsl_info("HTTP POST completed\n");
        lws_cancel_service(lws_get_context(wsi)); // Stop the loop
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

int httpPostRequest(const char *address, int port, struct my_user_data *user_data)
{
    lws_set_log_level(LLL_ERR | LLL_WARN | LLL_USER, NULL);

    struct lws_context_creation_info ctx_info;
    struct lws_client_connect_info conn_info;
    struct lws_context *context;
    struct lws *wsi;

    // Initialize context
    memset(&ctx_info, 0, sizeof(ctx_info));
    ctx_info.port = CONTEXT_PORT_NO_LISTEN;
    ctx_info.protocols = protocols;

    context = lws_create_context(&ctx_info);
    if (context == NULL)
    {
        fprintf(stderr, "Failed to create context\n");
        return -1;
    }

    // Initialize connection info
    memset(&conn_info, 0, sizeof(conn_info));
    conn_info.context = context;
    conn_info.address = address;
    conn_info.port = port;
    conn_info.path = "/";
    conn_info.host = lws_canonical_hostname(context);
    conn_info.origin = "origin";
    conn_info.protocol = protocols[0].name;
    conn_info.method = "POST"; // Set method to POST
    conn_info.ietf_version_or_minus_one = -1;
    conn_info.ssl_connection = 0; // No SSL
    conn_info.userdata = user_data;

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
    memset(user_data->response_buffer, 0, sizeof(user_data->response_buffer));
    user_data->response_length = 0;
    user_data->post_data = "key=value"; // The data you want to post

    httpPostRequest("127.0.0.1", 8080, user_data);
    printf("Received data: %s\n", user_data->response_buffer);

    free(user_data);
    return 0;
}
