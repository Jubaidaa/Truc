#define _GNU_SOURCE

#include "config.h"

#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static char *string_duplicate_c(const char *src)
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

static void print_usage(const char *program_name)
{
    fprintf(stderr, "Usage: %s [OPTIONS]\n", program_name);
    fprintf(stderr, "Options:\n");
    fprintf(stderr, "  --log-file FILE      Path to log file\n");
    fprintf(stderr, "  --log true/false     Enable/disable logging\n");
    fprintf(stderr, "  --pid-file FILE      Path to PID file\n");
    fprintf(stderr, "  --daemon ACTION      Daemon action (start/stop/restart)\n");
    fprintf(stderr, "  --server-name NAME   Server name\n");
    fprintf(stderr, "  --port PORT         Port number\n");
    fprintf(stderr, "  --ip IP             IP address to bind\n");
    fprintf(stderr, "  --root-dir DIR      Document root directory\n");
    fprintf(stderr, "  --default-file FILE Default file (default: index.html)\n");
    fprintf(stderr, "  -h, --help          Show this help\n");
}

static struct server_config *config_create_defaults(void)
{
    struct server_config *config = calloc(1, sizeof(struct server_config));
    if (!config)
    {
        return NULL;
    }
    
    config->port = 8080;
    config->ip = string_duplicate_c("127.0.0.1");
    config->default_file = string_duplicate_c("index.html");
    config->log_enabled = true;
    
    return config;
}

static void handle_log_option(struct server_config *config, const char *optarg)
{
    if (strcmp(optarg, "true") == 0)
    {
        config->log_enabled = true;
    }
    else if (strcmp(optarg, "false") == 0)
    {
        config->log_enabled = false;
    }
}

static void parse_option_group1(struct server_config *config, int c,
                                const char *optarg)
{
    if (c == 'l')
    {
        free(config->log_file);
        config->log_file = string_duplicate_c(optarg);
    }
    else if (c == 'L')
    {
        handle_log_option(config, optarg);
    }
    else if (c == 'p')
    {
        free(config->pid_file);
        config->pid_file = string_duplicate_c(optarg);
    }
    else if (c == 'd')
    {
        free(config->daemon_action);
        config->daemon_action = string_duplicate_c(optarg);
    }
}

static void parse_option_group2(struct server_config *config, int c,
                                const char *optarg)
{
    if (c == 's')
    {
        free(config->server_name);
        config->server_name = string_duplicate_c(optarg);
    }
    else if (c == 'P')
    {
        config->port = atoi(optarg);
    }
    else if (c == 'i')
    {
        free(config->ip);
        config->ip = string_duplicate_c(optarg);
    }
    else if (c == 'r')
    {
        free(config->root_dir);
        config->root_dir = string_duplicate_c(optarg);
    }
    else if (c == 'f')
    {
        free(config->default_file);
        config->default_file = string_duplicate_c(optarg);
    }
}

static int parse_option(struct server_config *config, int c,
                       const char *optarg, char **argv)
{
    if (c == 'h')
    {
        print_usage(argv[0]);
        config_destroy(config);
        exit(0);
    }
    else if (c == 'l' || c == 'L' || c == 'p' || c == 'd')
    {
        parse_option_group1(config, c, optarg);
    }
    else if (c == 's' || c == 'P' || c == 'i' || c == 'r' || c == 'f')
    {
        parse_option_group2(config, c, optarg);
    }
    else
    {
        print_usage(argv[0]);
        config_destroy(config);
        return -1;
    }
    
    return 0;
}

struct server_config *config_parse(int argc, char **argv)
{
    struct server_config *config = config_create_defaults();
    if (!config)
    {
        return NULL;
    }
    
    static struct option long_options[] = {
        {"log-file", required_argument, 0, 'l'},
        {"log", required_argument, 0, 'L'},
        {"pid-file", required_argument, 0, 'p'},
        {"daemon", required_argument, 0, 'd'},
        {"server-name", required_argument, 0, 's'},
        {"port", required_argument, 0, 'P'},
        {"ip", required_argument, 0, 'i'},
        {"root-dir", required_argument, 0, 'r'},
        {"default-file", required_argument, 0, 'f'},
        {"help", no_argument, 0, 'h'},
        {0, 0, 0, 0}
    };
    
    int option_index = 0;
    int c;
    
    while ((c = getopt_long(argc, argv, "h", long_options,
                            &option_index)) != -1)
    {
        if (parse_option(config, c, optarg, argv) < 0)
        {
            return NULL;
        }
    }
    
    return config;
}

void config_destroy(struct server_config *config)
{
    if (!config)
    {
        return;
    }
    
    free(config->log_file);
    free(config->pid_file);
    free(config->daemon_action);
    free(config->server_name);
    free(config->ip);
    free(config->root_dir);
    free(config->default_file);
    free(config);
}

void config_print(const struct server_config *config)
{
    if (!config)
    {
        return;
    }
    
    printf("Configuration:\n");
    printf("  Log file: %s\n", config->log_file ? config->log_file : "none");
    printf("  Logging: %s\n", config->log_enabled ? "enabled" : "disabled");
    printf("  PID file: %s\n", config->pid_file ? config->pid_file : "none");
    printf("  Daemon: %s\n",
           config->daemon_action ? config->daemon_action : "none");
    printf("  Server name: %s\n",
           config->server_name ? config->server_name : "none");
    printf("  Port: %d\n", config->port);
    printf("  IP: %s\n", config->ip);
    printf("  Root dir: %s\n",
           config->root_dir ? config->root_dir : "none");
    printf("  Default file: %s\n", config->default_file);
}

bool config_validate(const struct server_config *config)
{
    if (!config)
    {
        return false;
    }
    
    if (config->port < 1 || config->port > 65535)
    {
        fprintf(stderr, "Error: Invalid port number\n");
        return false;
    }
    
    if (!config->ip)
    {
        fprintf(stderr, "Error: No IP address specified\n");
        return false;
    }
    
    if (config->daemon_action)
    {
        if (strcmp(config->daemon_action, "start") != 0
            && strcmp(config->daemon_action, "stop") != 0
            && strcmp(config->daemon_action, "restart") != 0)
        {
            fprintf(stderr, "Error: Invalid daemon action\n");
            return false;
        }
    }
    
    return true;
}
