#define _POSIX_C_SOURCE 200809L

#include "server.h"

#include <arpa/inet.h>
#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "../config/config.h"

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
    
    server->socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server->socket_fd == -1)
    {
        perror("socket");
        free(server);
        return NULL;
    }
    
    int opt = 1;
    if (setsockopt(server->socket_fd, SOL_SOCKET, SO_REUSEADDR,
                   &opt, sizeof(opt)) < 0)
    {
        perror("setsockopt");
        close(server->socket_fd);
        free(server);
        return NULL;
    }
    
    memset(&server->address, 0, sizeof(server->address));
    server->address.sin_family = AF_INET;
    server->address.sin_port = htons(config->port);
    
    if (inet_pton(AF_INET, config->ip, &server->address.sin_addr) <= 0)
    {
        fprintf(stderr, "Invalid address: %s\n", config->ip);
        close(server->socket_fd);
        free(server);
        return NULL;
    }
    
    setup_signal_handlers();
    
    return server;
}

int server_bind(struct server *server)
{
    if (!server || server->socket_fd < 0)
    {
        return -1;
    }
    
    if (bind(server->socket_fd, (struct sockaddr *)&server->address,
             sizeof(server->address)) < 0)
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

void server_run_echo(struct server *server)
{
    if (!server)
    {
        return;
    }
    
    server->running = true;
    printf("Echo server started. Press Ctrl+C to stop.\n");
    
    while (server->running && g_server_running)
    {
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);
        
        int client_fd = accept(server->socket_fd,
                              (struct sockaddr *)&client_addr,
                              &client_len);
        
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
        inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, INET_ADDRSTRLEN);
        int client_port = ntohs(client_addr.sin_port);
        
        printf("Connection from %s:%d\n", client_ip, client_port);
        
        char buffer[BUFFER_SIZE];
        ssize_t bytes_read;
        
        while ((bytes_read = read(client_fd, buffer, BUFFER_SIZE)) > 0)
        {
            ssize_t total_written = 0;
            while (total_written < bytes_read)
            {
                ssize_t bytes_written = write(client_fd,
                                             buffer + total_written,
                                             bytes_read - total_written);
                if (bytes_written <= 0)
                {
                    break;
                }
                total_written += bytes_written;
            }
            
            if (total_written < bytes_read)
            {
                break;
            }
        }
        
        close(client_fd);
        printf("Connection closed from %s:%d\n", client_ip, client_port);
    }
    
    server->running = false;
    printf("Echo server stopped.\n");
}

void server_run(struct server *server)
{
    server_run_echo(server);
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
    
    free(server);
}

