#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "../src/config/config.h"
#include "../src/http/http.h"
#include "../src/logger/logger.h"
#include "../src/utils/string/string.h"

static char *get_client_ip(struct sockaddr_in *client_addr)
{
    static char ip_str[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &client_addr->sin_addr, ip_str, sizeof(ip_str));
    return ip_str;
}

static void handle_client_with_logging(int client_fd,
                                       struct sockaddr_in *client_addr,
                                       struct logger *logger)
{
    char buffer[4096];
    ssize_t bytes_read = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
    
    if (bytes_read <= 0)
    {
        return;
    }
    
    buffer[bytes_read] = '\0';
    char *client_ip = get_client_ip(client_addr);
    
    struct http_request *request = http_request_parse(buffer, bytes_read);
    
    if (!request || !request->is_valid)
    {
        logger_log_bad_request(logger, client_ip);
        
        struct http_response *response =
            http_response_create(HTTP_STATUS_BAD_REQUEST);
        
        logger_log_bad_response(logger, 400, client_ip);
        
        struct string *response_str = http_response_to_string(response);
        if (response_str)
        {
            send(client_fd, response_str->data, response_str->size, 0);
            string_destroy(response_str);
        }
        
        http_response_destroy(response);
        http_request_destroy(request);
        return;
    }
    
    const char *method_str = http_method_to_string(request->method);
    char *target_str = request->target ? request->target->data : "/";
    
    logger_log_request(logger, method_str, target_str, client_ip);
    
    enum http_status status = HTTP_STATUS_OK;
    struct http_response *response = http_response_create(status);
    
    logger_log_response(logger, status, client_ip, method_str, target_str);
    
    struct string *response_str = http_response_to_string(response);
    if (response_str)
    {
        send(client_fd, response_str->data, response_str->size, 0);
        string_destroy(response_str);
    }
    
    http_response_destroy(response);
    http_request_destroy(request);
}

int main(int argc, char **argv)
{
    struct server_config *config = config_parse(argc, argv);
    if (!config || !config_validate(config))
    {
        config_destroy(config);
        return 2;
    }
    
    struct logger *logger = logger_create(config);
    if (!logger)
    {
        fprintf(stderr, "Failed to create logger\n");
        config_destroy(config);
        return 1;
    }
    
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0)
    {
        perror("socket");
        logger_destroy(logger);
        config_destroy(config);
        return 1;
    }
    
    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    
    struct sockaddr_in server_addr = {0};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(config->port);
    inet_pton(AF_INET, config->ip, &server_addr.sin_addr);
    
    if (bind(server_fd, &server_addr, sizeof(server_addr)) < 0)
    {
        perror("bind");
        close(server_fd);
        logger_destroy(logger);
        config_destroy(config);
        return 1;
    }
    
    if (listen(server_fd, 10) < 0)
    {
        perror("listen");
        close(server_fd);
        logger_destroy(logger);
        config_destroy(config);
        return 1;
    }
    
    printf("Server listening on %s:%d\n", config->ip, config->port);
    printf("Logging %s\n",
           config->log_enabled ? "enabled" : "disabled");
    if (config->log_enabled && config->log_file)
    {
        printf("Logging to file: %s\n", config->log_file);
    }
    else if (config->log_enabled)
    {
        printf("Logging to stdout\n");
    }
    
    while (1)
    {
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);
        
        int client_fd = accept(server_fd, &client_addr, &client_len);
        if (client_fd < 0)
        {
            perror("accept");
            continue;
        }
        
        handle_client_with_logging(client_fd, &client_addr, logger);
        close(client_fd);
    }
    
    close(server_fd);
    logger_destroy(logger);
    config_destroy(config);
    return 0;
}
