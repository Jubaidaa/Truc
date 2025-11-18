#ifndef SERVER_H
#define SERVER_H

#include <netinet/in.h>
#include <stdbool.h>

#include "../config/config.h"

#define BACKLOG 128
#define BUFFER_SIZE 8192

struct server
{
    struct server_config *config;
    int socket_fd;
    struct sockaddr_in address;
    bool running;
};

struct server *server_create(struct server_config *config);
int server_bind(struct server *server);
int server_listen(struct server *server);
void server_run(struct server *server);
void server_run_echo(struct server *server);
void server_stop(struct server *server);
void server_destroy(struct server *server);

#endif // ! SERVER_H

