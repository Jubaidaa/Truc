#define _POSIX_C_SOURCE 200809L

#include "server.h"

#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include "../http/http.h"
#include "../logger/logger.h"
#include "../utils/aux_string.h"
#include "aux_network.h"

static volatile sig_atomic_t g_server_running = 1;

static void signal_handler(int sig)
{
    if (sig == SIGINT || sig == SIGTERM)
    {
        g_server_running = 0;
    }
}

static void setup_signal_handlers(void)
{
    struct sigaction sa;

    signal(SIGPIPE, SIG_IGN);

    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = signal_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;

    if (sigaction(SIGINT, &sa, NULL) == -1)
    {
        perror("sigaction SIGINT");
    }

    if (sigaction(SIGTERM, &sa, NULL) == -1)
    {
        perror("sigaction SIGTERM");
    }
}

static int server_create_socket(struct server *server,
                                struct server_config *config)
{
    server->socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server->socket_fd == -1)
    {
        perror("socket");
        return -1;
    }

    int opt = 1;
    if (setsockopt(server->socket_fd, SOL_SOCKET, SO_REUSEADDR, &opt,
                   sizeof(opt))
        < 0)
    {
        perror("setsockopt");
        close(server->socket_fd);
        return -1;
    }

    memset(&server->address, 0, sizeof(server->address));
    server->address.sin.sin_family = AF_INET;
    server->address.sin.sin_port = my_htons(config->port);

    if (my_inet_pton_ipv4(config->ip, &server->address.sin.sin_addr) <= 0)
    {
        fprintf(stderr, "Invalid address: %s\n", config->ip);
        close(server->socket_fd);
        return -1;
    }

    return 0;
}

struct server *server_create(struct server_config *config)
{
    if (!config)
    {
        return NULL;
    }

    struct server *server = calloc(1, sizeof(struct server));
    if (!server)
    {
        return NULL;
    }

    server->config = config;
    server->socket_fd = -1;
    server->running = false;

    if (server_create_socket(server, config) < 0)
    {
        free(server);
        return NULL;
    }

    setup_signal_handlers();

    server->logger = logger_create(config);
    if (!server->logger && config->log_enabled)
    {
        fprintf(stderr, "Warning: Failed to create logger\n");
    }

    return server;
}

int server_bind(struct server *server)
{
    if (!server || server->socket_fd < 0)
    {
        return -1;
    }

    if (bind(server->socket_fd, &server->address.sa,
             sizeof(server->address.sin))
        < 0)
    {
        perror("bind");
        return -1;
    }

    printf("Server bound to %s:%d\n", server->config->ip, server->config->port);
    return 0;
}

int server_listen(struct server *server)
{
    if (!server || server->socket_fd < 0)
    {
        return -1;
    }

    if (listen(server->socket_fd, BACKLOG) < 0)
    {
        perror("listen");
        return -1;
    }

    printf("Server listening (backlog: %d)\n", BACKLOG);
    return 0;
}

static const char *get_mime_type(const char *path)
{
    const char *ext = strrchr(path, '.');
    if (!ext)
    {
        return "application/octet-stream";
    }

    if (strcmp(ext, ".html") == 0 || strcmp(ext, ".htm") == 0)
    {
        return "text/html";
    }
    if (strcmp(ext, ".txt") == 0)
    {
        return "text/plain";
    }
    if (strcmp(ext, ".css") == 0)
    {
        return "text/css";
    }
    if (strcmp(ext, ".js") == 0)
    {
        return "application/javascript";
    }
    if (strcmp(ext, ".json") == 0)
    {
        return "application/json";
    }
    if (strcmp(ext, ".png") == 0)
    {
        return "image/png";
    }
    if (strcmp(ext, ".jpg") == 0 || strcmp(ext, ".jpeg") == 0)
    {
        return "image/jpeg";
    }
    if (strcmp(ext, ".gif") == 0)
    {
        return "image/gif";
    }

    return "application/octet-stream";
}

static struct string *read_file(const char *path)
{
    FILE *f = fopen(path, "rb");
    if (!f)
    {
        return NULL;
    }

    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fseek(f, 0, SEEK_SET);

    if (size < 0)
    {
        fclose(f);
        return NULL;
    }

    char *buffer = malloc(size);
    if (!buffer)
    {
        fclose(f);
        return NULL;
    }

    size_t read_size = fread(buffer, 1, size, f);
    fclose(f);

    struct string *content = string_create(buffer, read_size);
    free(buffer);

    return content;
}

static void send_error_response(int client_fd, enum http_status status)
{
    struct http_response *response = http_response_create(status);
    struct string *response_str = http_response_to_string(response);

    if (response_str)
    {
        write(client_fd, response_str->data, response_str->size);
        string_destroy(response_str);
    }

    http_response_destroy(response);
}

static void build_filepath(char *filepath, size_t size,
                           struct server_config *config,
                           struct http_request *request)
{
    if (strcmp(request->target->data, "/") == 0)
    {
        snprintf(filepath, size, "%s/%s", config->root_dir,
                 config->default_file);
    }
    else
    {
        snprintf(filepath, size, "%s%s", config->root_dir,
                 request->target->data);
    }
}

static struct http_response *create_file_response(const char *filepath,
                                                  enum http_method method)
{
    struct stat st;
    if (lstat(filepath, &st) < 0 || !S_ISREG(st.st_mode))
    {
        return NULL;
    }

    struct http_response *response = http_response_create(HTTP_STATUS_OK);

    const char *mime = get_mime_type(filepath);
    struct http_header *content_type = http_header_create("Content-Type", mime);
    http_header_add(&response->headers, content_type);

    char content_len[32];
    snprintf(content_len, sizeof(content_len), "%ld", st.st_size);
    struct http_header *length =
        http_header_create("Content-Length", content_len);
    http_header_add(&response->headers, length);

    if (method == HTTP_METHOD_GET)
    {
        response->body = read_file(filepath);
    }

    return response;
}

static int handle_invalid_request(int client_fd, struct server *server,
                                   const char *client_ip)
{
    logger_log_bad_request(server->logger, client_ip);
    send_error_response(client_fd, HTTP_STATUS_BAD_REQUEST);
    logger_log_bad_response(server->logger, 400, client_ip);
    return -1;
}

static int send_file_response(int client_fd, struct server *server,
                               const struct log_request_info *info,
                               struct http_request *request)
{
    char filepath[2048];
    build_filepath(filepath, sizeof(filepath), server->config, request);

    struct http_response *response =
        create_file_response(filepath, request->method);
    if (!response)
    {
        send_error_response(client_fd, HTTP_STATUS_NOT_FOUND);
        logger_log_response(server->logger, 404, info);
        return -1;
    }

    struct string *response_str = http_response_to_string(response);
    if (response_str)
    {
        write(client_fd, response_str->data, response_str->size);
        string_destroy(response_str);
    }

    logger_log_response(server->logger, 200, info);
    http_response_destroy(response);
    return 0;
}

static int validate_request(struct http_request *request, int client_fd,
                            struct server *server,
                            const struct log_request_info *info)
{
    if (strcmp(request->version->data, "HTTP/1.1") != 0)
    {
        send_error_response(client_fd, HTTP_STATUS_VERSION_NOT_SUPPORTED);
        logger_log_response(server->logger, 505, info);
        return -1;
    }

    if (request->method != HTTP_METHOD_GET
        && request->method != HTTP_METHOD_HEAD)
    {
        send_error_response(client_fd, HTTP_STATUS_METHOD_NOT_ALLOWED);
        logger_log_response(server->logger, 405, info);
        return -1;
    }

    return 0;
}

static void handle_http_request(int client_fd, struct server *server,
                                const char *client_ip)
{
    char buffer[BUFFER_SIZE];
    ssize_t bytes_read = read(client_fd, buffer, BUFFER_SIZE - 1);

    if (bytes_read <= 0)
    {
        return;
    }

    buffer[bytes_read] = '\0';

    struct http_request *request = http_request_parse(buffer, bytes_read);
    if (!request || !request->is_valid)
    {
        handle_invalid_request(client_fd, server, client_ip);
        http_request_destroy(request);
        return;
    }

    struct log_request_info info;
    info.request_type = http_method_to_string(request->method);
    info.target = request->target->data;
    info.client_ip = client_ip;

    logger_log_request(server->logger, &info);

    if (validate_request(request, client_fd, server, &info) < 0)
    {
        http_request_destroy(request);
        return;
    }

    if (!server->config->root_dir)
    {
        http_request_destroy(request);
        return;
    }

    send_file_response(client_fd, server, &info, request);
    http_request_destroy(request);
}

void server_run_http(struct server *server)
{
    if (!server)
    {
        return;
    }

    server->running = true;
    printf("HTTP server started. Press Ctrl+C to stop.\n");

    while (server->running && g_server_running)
    {
        union socket_addr_union client_addr;
        socklen_t client_len = sizeof(client_addr.sin);

        int client_fd = accept(server->socket_fd, &client_addr.sa, &client_len);

        if (client_fd < 0)
        {
            if (errno == EINTR)
            {
                continue;
            }
            perror("accept");
            continue;
        }

        char client_ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &client_addr.sin.sin_addr, client_ip,
                  INET_ADDRSTRLEN);

        handle_http_request(client_fd, server, client_ip);

        close(client_fd);
    }

    server->running = false;
    printf("HTTP server stopped.\n");
}

void server_run(struct server *server)
{
    server_run_http(server);
}

void server_stop(struct server *server)
{
    if (server)
    {
        server->running = false;
        g_server_running = 0;
    }
}

void server_destroy(struct server *server)
{
    if (!server)
    {
        return;
    }

    if (server->socket_fd >= 0)
    {
        close(server->socket_fd);
    }

    logger_destroy(server->logger);

    free(server);
}
