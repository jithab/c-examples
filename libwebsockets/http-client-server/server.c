#include <libwebsockets.h>
#include <string.h>

static int callback_http(struct lws *wsi, enum lws_callback_reasons reason,
                         void *user, void *in, size_t len)
{
    switch (reason)
    {
    case LWS_CALLBACK_HTTP:
        const char *response_body = "<h1>Hello from HTTP</h1>";
        const char *headers = "HTTP/1.1 200 OK\r\n"
                              "Content-Type: text/html\r\n"
                              "Content-Length: 24\r\n"
                              "Connection: close\r\n\r\n";

        // Send HTTP headers
        lws_write(wsi, (unsigned char *)headers, strlen(headers), LWS_WRITE_HTTP_HEADERS);

        // Send HTTP body
        lws_write(wsi, (unsigned char *)response_body, strlen(response_body), LWS_WRITE_HTTP_FINAL);

        return -1; // Close the connection after the response is sent

        break;
    default:

        break;
    }
    return 0;
}

static struct lws_protocols protocols[] = {
    {"http-only", callback_http, 0, 0, 0, NULL, 0},
    {NULL, NULL, 0, 0}};

int main(void)
{
    lws_set_log_level(LLL_ERR | LLL_WARN, NULL);

    struct lws_context_creation_info info;
    memset(&info, 0, sizeof(info));

    info.port = 8080;
    info.protocols = protocols;
    info.gid = -1;
    info.uid = -1;

    struct lws_context *context = lws_create_context(&info);
    if (!context)
    {
        lwsl_err("lws init failed\n");
        return -1;
    }

    while (1)
    {
        lws_service(context, 1000);
    }

    lws_context_destroy(context);
    return 0;
}
