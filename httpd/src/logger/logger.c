#define _POSIX_C_SOURCE 200809L

#include "logger.h"

#include <stdlib.h>
#include <string.h>
#include <time.h>

static char *string_duplicate(const char *src)
{
    if (!src)
    {
        return NULL;
    }
    
    size_t len = strlen(src);
    char *dst = malloc(len + 1);
    if (dst)
    {
        strcpy(dst, src);
    }
    return dst;
}

static void format_gmt_date(char *buffer, size_t size)
{
    time_t now = time(NULL);
    struct tm *tm_info = gmtime(&now);
    
    if (tm_info)
    {
        strftime(buffer, size, "%a, %d %b %Y %H:%M:%S GMT", tm_info);
    }
    else
    {
        buffer[0] = '\0';
    }
}

static int logger_setup_output(struct logger *logger,
                               const struct server_config *config)
{
    if (config->log_file)
    {
        logger->output = fopen(config->log_file, "a");
        if (!logger->output)
        {
            return -1;
        }
        logger->owns_file = true;
    }
    else
    {
        logger->output = stdout;
        logger->owns_file = false;
    }
    
    return 0;
}

struct logger *logger_create(const struct server_config *config)
{
    if (!config)
    {
        return NULL;
    }
    
    struct logger *logger = calloc(1, sizeof(struct logger));
    if (!logger)
    {
        return NULL;
    }
    
    logger->enabled = config->log_enabled;
    
    if (!logger->enabled)
    {
        logger->output = NULL;
        logger->owns_file = false;
        return logger;
    }
    
    if (config->server_name)
    {
        logger->server_name = string_duplicate(config->server_name);
    }
    else
    {
        logger->server_name = string_duplicate("localhost");
    }
    
    if (!logger->server_name)
    {
        free(logger);
        return NULL;
    }
    
    if (logger_setup_output(logger, config) < 0)
    {
        free(logger->server_name);
        free(logger);
        return NULL;
    }
    
    return logger;
}

void logger_destroy(struct logger *logger)
{
    if (!logger)
    {
        return;
    }
    
    if (logger->owns_file && logger->output)
    {
        fclose(logger->output);
    }
    
    free(logger->server_name);
    free(logger);
}

void logger_log_request(struct logger *logger,
                        const struct log_request_info *info)
{
    if (!logger || !logger->enabled || !logger->output)
    {
        return;
    }
    
    if (!info || !info->request_type || !info->target || !info->client_ip)
    {
        return;
    }
    
    char date_buffer[64];
    format_gmt_date(date_buffer, sizeof(date_buffer));
    
    fprintf(logger->output, "%s [%s] received %s on '%s' from %s\n",
            date_buffer, logger->server_name, info->request_type,
            info->target, info->client_ip);
    
    fflush(logger->output);
}

void logger_log_bad_request(struct logger *logger, const char *client_ip)
{
    if (!logger || !logger->enabled || !logger->output)
    {
        return;
    }
    
    if (!client_ip)
    {
        return;
    }
    
    char date_buffer[64];
    format_gmt_date(date_buffer, sizeof(date_buffer));
    
    fprintf(logger->output, "%s [%s] received Bad Request from %s\n",
            date_buffer, logger->server_name, client_ip);
    
    fflush(logger->output);
}

void logger_log_response(struct logger *logger, int status_code,
                         const struct log_request_info *info)
{
    if (!logger || !logger->enabled || !logger->output)
    {
        return;
    }
    
    if (!info || !info->client_ip || !info->request_type || !info->target)
    {
        return;
    }
    
    char date_buffer[64];
    format_gmt_date(date_buffer, sizeof(date_buffer));
    
    fprintf(logger->output,
            "%s [%s] responding with %d to %s for %s on '%s'\n",
            date_buffer, logger->server_name, status_code, info->client_ip,
            info->request_type, info->target);
    
    fflush(logger->output);
}

void logger_log_bad_response(struct logger *logger, int status_code,
                             const char *client_ip)
{
    if (!logger || !logger->enabled || !logger->output)
    {
        return;
    }
    
    if (!client_ip)
    {
        return;
    }
    
    char date_buffer[64];
    format_gmt_date(date_buffer, sizeof(date_buffer));
    
    fprintf(logger->output, "%s [%s] responding with %d to %s\n",
            date_buffer, logger->server_name, status_code, client_ip);
    
    fflush(logger->output);
}
