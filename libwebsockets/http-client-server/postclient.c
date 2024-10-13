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
        const char *content_type = "application/x-www-form-urlencoded";
        const char *content_length = "9"; // Adjust this as per your POST data length

        if (lws_add_http_header_by_name(wsi, (unsigned char *)"Content-Type:", (unsigned char *)content_type, strlen(content_type), p, end))
        {
            lwsl_warn("Failed Content-Type\n");

            // return -1;
        }
        if (lws_add_http_header_by_name(wsi, (unsigned char *)"Content-Length:", (unsigned char *)content_length, 1, p, end))
        {
            lwsl_warn("Failed Content-Length\n");

            // return -1;
        }
        // if (lws_finalize_http_header(wsi, p, end))
        //{
        //     lwsl_warn("Failed lws_finalize_http_header\n");

        // return -1;
        //}
        lws_client_http_body_pending(wsi, 1);
        // lws_callback_on_writable(wsi);
        //  const char *post_data = "key=value";
        //  lws_write(wsi, (unsigned char *)post_data, strlen(post_data), LWS_WRITE_HTTP_FINAL);

        break;
    }

    case LWS_CALLBACK_CLIENT_HTTP_WRITEABLE:
    {
        lwsl_warn("Connection LWS_CALLBACK_CLIENT_WRITEABLE\n");
        // Send POST data
        const char *post_data = "key=value";
        lws_write(wsi, (unsigned char *)post_data, strlen(post_data), LWS_WRITE_HTTP_FINAL);
        // Notify that there is no more data to send
        lws_client_http_body_pending(wsi, 0);

        // Optionally close connection if all data is sent
        // lws_close_reason(wsi, LWS_CLOSE_STATUS_NORMAL, NULL, 0);
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
    case LWS_CALLBACK_RECEIVE_CLIENT_HTTP:
    {
        lwsl_warn("@#@ LWS_CALLBACK_RECEIVE_CLIENT_HTTP\n");

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
        lwsl_warn("@#@ LWS_CALLBACK_RECEIVE_CLIENT_HTTP_READ\n");
        data->response_length += len;
        data->response_buffer[data->response_length] = '\0';
        printf("Received data: %s\n", data->response_buffer);

        break;
    case LWS_CALLBACK_CLIENT_CONNECTION_ERROR:
        lwsl_warn("Connection error\n");
        break;

    case LWS_CALLBACK_COMPLETED_CLIENT_HTTP:
        lwsl_warn("HTTP POST completed\n");
        lws_cancel_service(lws_get_context(wsi)); // Stop the loop
        break;

    case LWS_CALLBACK_ESTABLISHED_CLIENT_HTTP:
        lwsl_warn("LWS_CALLBACK_ESTABLISHED_CLIENT_HTTP\n");

        data->response_length = 0;
        lws_callback_on_writable(wsi);
        break;
    default:
        lwsl_warn("reason: %d\n", reason);
        if (reason == 46)
            return -1;
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
    lws_set_log_level(LLL_ERR | LLL_WARN, NULL);

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
    // conn_info.userdata = user_data;

    wsi = lws_client_connect_via_info(&conn_info);
    if (wsi == NULL)
    {
        fprintf(stderr, "Connection failed\n");
        lws_context_destroy(context);
        return -1;
    }

    // lws_get_protocol(wsi)->user = user_data;

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
