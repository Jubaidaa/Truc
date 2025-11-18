#include <stdio.h>
#include <stdlib.h>

#include "config/config.h"
#include "server/server.h"

int main(int argc, char **argv)
{
    struct server_config *config = config_parse(argc, argv);
    if (!config)
    {
        fprintf(stderr, "Failed to parse configuration\n");
        return 1;
    }
    
    if (!config_validate(config))
    {
        config_destroy(config);
        return 1;
    }
    
    if (config->daemon_action)
    {
        printf("Daemon functionality not yet implemented\n");
        config_destroy(config);
        return 0;
    }
    
    config_print(config);
    
    struct server *server = server_create(config);
    if (!server)
    {
        fprintf(stderr, "Failed to create server\n");
        config_destroy(config);
        return 1;
    }
    
    if (server_bind(server) < 0)
    {
        server_destroy(server);
        config_destroy(config);
        return 1;
    }
    
    if (server_listen(server) < 0)
    {
        server_destroy(server);
        config_destroy(config);
        return 1;
    }
    
    server_run(server);
    
    server_destroy(server);
    config_destroy(config);
    
    return 0;
}

