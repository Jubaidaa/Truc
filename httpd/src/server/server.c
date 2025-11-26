#define _POSIX_C_SOURCE 200809L

#include "server.h"

#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

#include "../http/http.h"
#include "../http/http_error.h"
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
    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGTERM, &sa, NULL);
}

static int server_create_socket(struct server *server,
                                struct server_config *config)
{
    server->socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server->socket_fd == -1)
    {
        return -1;
    }

    int opt = 1;
    if (setsockopt(server->socket_fd, SOL_SOCKET, SO_REUSEADDR, &opt,
                   sizeof(opt))
        < 0)
    {
        close(server->socket_fd);
        return -1;
    }

    struct sockaddr_in sin = { 0 };
    sin.sin_family = AF_INET;
    sin.sin_port = my_htons(config->port);

    if (my_inet_pton_ipv4(config->ip, &sin.sin_addr) <= 0)
    {
        close(server->socket_fd);
        return -1;
    }

    memcpy(&server->address.ss, &sin, sizeof(sin));
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
    return server;
}

int server_bind(struct server *server)
{
    if (!server || server->socket_fd < 0)
    {
        return -1;
    }

    if (bind(server->socket_fd, &server->address.sa, sizeof(struct sockaddr_in))
        < 0)
    {
        return -1;
    }
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
        return -1;
    }
    return 0;
}

static struct string *read_file_content(const char *path)
{
    int fd = open(path, O_RDONLY);
    if (fd == -1)
    {
        return NULL;
    }

    off_t size = lseek(fd, 0, SEEK_END);
    if (size < 0)
    {
        close(fd);
        return NULL;
    }
    lseek(fd, 0, SEEK_SET);

    char *buffer = malloc(size);
    if (!buffer)
    {
        close(fd);
        return NULL;
    }

    ssize_t read_size = read(fd, buffer, size);
    close(fd);

    if (read_size < 0)
    {
        free(buffer);
        return NULL;
    }

    struct string *content = string_create(buffer, read_size);
    free(buffer);
    return content;
}

static struct http_response *create_file_response(const char *filepath,
                                                  enum http_method method)
{
    int fd = open(filepath, O_RDONLY);
    if (fd == -1)
    {
        if (errno == EACCES)
        {
            return http_error_create_response(HTTP_STATUS_FORBIDDEN);
        }
        return http_error_create_response(HTTP_STATUS_NOT_FOUND);
    }
    close(fd);

    struct http_response *resp = http_response_create(HTTP_STATUS_OK);
    if (!resp)
    {
        return NULL;
    }

    struct stat st;
    lstat(filepath, &st);

    char buf[64];
    time_t now = time(NULL);
    strftime(buf, sizeof(buf), "%a, %d %b %Y %H:%M:%S GMT", gmtime(&now));
    http_header_add(&resp->headers, http_header_create("Date", buf));
    http_header_add(&resp->headers,
                    http_header_create("Connection", "close"));

    char len[32];
    snprintf(len, sizeof(len), "%ld", st.st_size);
    http_header_add(&resp->headers, http_header_create("Content-Length", len));

    if (method == HTTP_METHOD_GET)
    {
        resp->body = read_file_content(filepath);
    }
    return resp;
}

static int check_host_header(struct http_request *req,
                             struct server_config *conf)
{
    struct http_header *h = http_header_find(req->headers, "Host");
    if (!h)
    {
        return -1;
    }

    char port_str[16];
    snprintf(port_str, sizeof(port_str), "%d", conf->port);

    struct string *host_ip_port = string_create(conf->ip, strlen(conf->ip));
    string_concat_str(host_ip_port, ":", 1);
    string_concat_str(host_ip_port, port_str, strlen(port_str));

    int valid = 0;
    if (string_compare_str(h->value, conf->server_name) == 0)
    {
        valid = 1;
    }
    else if (string_compare_str(h->value, conf->ip) == 0)
    {
        valid = 1;
    }
    else if (string_compare_str(h->value, host_ip_port->data) == 0)
    {
        valid = 1;
    }

    string_destroy(host_ip_port);
    return valid ? 0 : -1;
}

static void perform_file_response(int client_fd, struct server *server,
                                  struct http_request *req, char *path,
                                  struct log_request_info *info)
{
    struct http_response *resp = create_file_response(path, req->method);
    if (!resp)
    {
        return;
    }

    struct string *s = http_response_to_string(resp);
    if (s)
    {
        write(client_fd, s->data, s->size);
        string_destroy(s);
    }
    logger_log_response(server->logger, resp->status, info);
    http_response_destroy(resp);
}

static void handle_request_logic(int client_fd, struct server *server,
                                 struct http_request *req,
                                 const char *client_ip)
{
    struct log_request_info info = { http_method_to_string(req->method),
                                     req->target->data, client_ip };
    logger_log_request(server->logger, &info);

    if (strncmp(req->version->data, "HTTP/", 5) != 0)
    {
        http_error_send(client_fd, HTTP_STATUS_BAD_REQUEST);
        logger_log_response(server->logger, 400, &info);
        return;
    }
    if (strcmp(req->version->data, "HTTP/1.1") != 0)
    {
        http_error_send(client_fd, HTTP_STATUS_VERSION_NOT_SUPPORTED);
        logger_log_response(server->logger, 505, &info);
        return;
    }
    if (check_host_header(req, server->config) < 0)
    {
        http_error_send(client_fd, HTTP_STATUS_BAD_REQUEST);
        logger_log_response(server->logger, 400, &info);
        return;
    }
    if (req->method != HTTP_METHOD_GET && req->method != HTTP_METHOD_HEAD)
    {
        http_error_send(client_fd, HTTP_STATUS_METHOD_NOT_ALLOWED);
        logger_log_response(server->logger, 405, &info);
        return;
    }
    char path[2048];
    const char *target = req->target->data;
    if (strcmp(target, "/") == 0)
        snprintf(path, sizeof(path), "%s/%s", server->config->root_dir,
                 server->config->default_file);
    else
        snprintf(path, sizeof(path), "%s%s", server->config->root_dir, target);
    perform_file_response(client_fd, server, req, path, &info);
}

void server_run(struct server *server)
{
    server->running = true;
    while (server->running && g_server_running)
    {
        union socket_addr_union addr;
        socklen_t len = sizeof(addr.sa);

        int client_fd = accept(server->socket_fd, &addr.sa, &len);
        if (client_fd < 0)
        {
            continue;
        }

        char client_ip[INET_ADDRSTRLEN];
        struct sockaddr_in client_sin;
        memcpy(&client_sin, &addr.ss, sizeof(client_sin));
        inet_ntop(AF_INET, &client_sin.sin_addr, client_ip, INET_ADDRSTRLEN);

        char buf[BUFFER_SIZE];
        ssize_t n = read(client_fd, buf, BUFFER_SIZE);

        if (n > 0)
        {
            struct http_request *req = http_request_parse(buf, n);
            if (req && req->is_valid)
            {
                handle_request_logic(client_fd, server, req, client_ip);
            }
            else
            {
                logger_log_bad_request(server->logger, client_ip);
                http_error_send(client_fd, HTTP_STATUS_BAD_REQUEST);
                logger_log_bad_response(server->logger, 400, client_ip);
            }
            http_request_destroy(req);
        }
        close(client_fd);
    }
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
