#ifndef LOGGER_H
#define LOGGER_H

#include <stdbool.h>
#include <stdio.h>

#include "../config/config.h"
#include "../http/http.h"

struct logger
{
    FILE *output;
    bool enabled;
    bool owns_file;
    char *server_name;
};

struct log_request_info
{
    const char *request_type;
    const char *target;
    const char *client_ip;
};

struct logger *logger_create(const struct server_config *config);
void logger_destroy(struct logger *logger);

void logger_log_request(struct logger *logger,
                        const struct log_request_info *info);
void logger_log_bad_request(struct logger *logger, const char *client_ip);

void logger_log_response(struct logger *logger, int status_code,
                         const struct log_request_info *info);
void logger_log_bad_response(struct logger *logger, int status_code,
                             const char *client_ip);

#endif // ! LOGGER_H
