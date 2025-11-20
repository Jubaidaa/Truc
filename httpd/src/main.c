#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "config/config.h"
#include "daemon/daemon.h"
#include "server/server.h"

static int run_server(struct server_config *config)
{
    struct server *server = server_create(config);
    if (!server)
    {
        fprintf(stderr, "Failed to create server\n");
        return 1;
    }

    if (server_bind(server) < 0)
    {
        server_destroy(server);
        return 1;
    }

    if (server_listen(server) < 0)
    {
        server_destroy(server);
        return 1;
    }

    server_run(server);

    server_destroy(server);
    return 0;
}

static int handle_daemon_action(struct server_config *config)
{
    int result = 0;

    if (strcmp(config->daemon_action, "start") == 0)
    {
        result = daemon_start(config);
        if (result >= 0)
        {
            return result;
        }
    }
    else if (strcmp(config->daemon_action, "stop") == 0)
    {
        result = daemon_stop(config);
        return result;
    }
    else if (strcmp(config->daemon_action, "restart") == 0)
    {
        result = daemon_restart(config);
        if (result >= 0)
        {
            return result;
        }
    }

    return -1;
}

int main(int argc, char **argv)
{
    struct server_config *config = config_parse(argc, argv);
    if (!config)
    {
        fprintf(stderr, "Failed to parse configuration\n");
        return 2;
    }

    if (!config_validate(config))
    {
        config_destroy(config);
        return 2;
    }

    if (config->daemon_action)
    {
        int result = handle_daemon_action(config);
        if (result >= 0)
        {
            config_destroy(config);
            return result;
        }
    }

    config_print(config);

    int ret = run_server(config);
    config_destroy(config);

    return ret;
}
