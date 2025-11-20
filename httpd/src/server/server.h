#ifndef SERVER_H
#define SERVER_H

#include <stdbool.h>
#include <stdint.h>
#include <sys/socket.h>
#include <sys/types.h>

#include "../config/config.h"

#define BACKLOG 128
#define BUFFER_SIZE 8192
#define INET_ADDRSTRLEN 16

#ifndef AF_INET
#define AF_INET 2
#endif

struct in_addr
{
    uint32_t s_addr;
};

struct sockaddr_in
{
    sa_family_t sin_family;
    uint16_t sin_port;
    struct in_addr sin_addr;
    unsigned char sin_zero[8];
};

struct logger;

struct server
{
    struct server_config *config;
    int socket_fd;
    struct sockaddr_in address;
    bool running;
    struct logger *logger;
};

struct server *server_create(struct server_config *config);
int server_bind(struct server *server);
int server_listen(struct server *server);
void server_run(struct server *server);
void server_run_echo(struct server *server);
void server_stop(struct server *server);
void server_destroy(struct server *server);

#endif // ! SERVER_H
