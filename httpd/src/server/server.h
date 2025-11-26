#ifndef SERVER_H
#define SERVER_H

#include <arpa/inet.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <sys/types.h>

#include "../config/config.h"
#include "aux_socket.h"

#define BACKLOG 128
#define BUFFER_SIZE 8192

struct logger;

struct server
{
    struct server_config *config;
    int socket_fd;
    union socket_addr_union address;
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
