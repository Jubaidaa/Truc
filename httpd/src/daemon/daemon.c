#define _POSIX_C_SOURCE 200809L

#include "daemon.h"

#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

int daemon_check_running(const char *pid_file, pid_t *pid)
{
    if (!pid_file)
    {
        return 0;
    }

    FILE *f = fopen(pid_file, "r");
    if (!f)
    {
        return 0;
    }

    if (fscanf(f, "%d", pid) != 1)
    {
        fclose(f);
        return 0;
    }
    fclose(f);

    if (kill(*pid, 0) == 0)
    {
        return 1;
    }

    return 0;
}

int daemon_write_pid(const char *pid_file)
{
    if (!pid_file)
    {
        return -1;
    }

    FILE *f = fopen(pid_file, "w");
    if (!f)
    {
        perror("fopen");
        return -1;
    }

    fprintf(f, "%d\n", getpid());
    fclose(f);
    return 0;
}

int daemon_remove_pid(const char *pid_file)
{
    if (!pid_file)
    {
        return -1;
    }

    if (remove(pid_file) < 0 && errno != ENOENT)
    {
        perror("remove");
        return -1;
    }

    return 0;
}

static void daemonize_process(void)
{
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);

    int dev_null = open("/dev/null", O_RDWR);
    if (dev_null >= 0)
    {
        dup2(dev_null, STDIN_FILENO);
        dup2(dev_null, STDOUT_FILENO);
        dup2(dev_null, STDERR_FILENO);
        if (dev_null > 2)
        {
            close(dev_null);
        }
    }
}

int daemon_start(struct server_config *config)
{
    if (!config || !config->pid_file)
    {
        fprintf(stderr, "Error: PID file is required for daemon mode\n");
        return 2;
    }

    pid_t existing_pid;
    if (daemon_check_running(config->pid_file, &existing_pid))
    {
        fprintf(stderr, "Error: Daemon already running with PID %d\n",
                existing_pid);
        return 1;
    }

    pid_t pid = fork();
    if (pid < 0)
    {
        perror("fork");
        return 1;
    }

    if (pid > 0)
    {
        printf("%d\n", pid);
        return 0;
    }

    daemonize_process();

    if (daemon_write_pid(config->pid_file) < 0)
    {
        exit(1);
    }

    return -1;
}

int daemon_stop(struct server_config *config)
{
    if (!config || !config->pid_file)
    {
        fprintf(stderr, "Error: PID file is required for daemon mode\n");
        return 2;
    }

    pid_t pid;
    if (!daemon_check_running(config->pid_file, &pid))
    {
        fprintf(stderr, "Error: No daemon running\n");
        return 1;
    }

    if (kill(pid, SIGTERM) < 0)
    {
        perror("kill");
        return 1;
    }

    daemon_remove_pid(config->pid_file);
    return 0;
}

int daemon_restart(struct server_config *config)
{
    if (!config || !config->pid_file)
    {
        fprintf(stderr, "Error: PID file is required for daemon mode\n");
        return 2;
    }

    pid_t pid;
    if (daemon_check_running(config->pid_file, &pid))
    {
        if (kill(pid, SIGTERM) < 0)
        {
            perror("kill");
            return 1;
        }
    }

    daemon_remove_pid(config->pid_file);

    return daemon_start(config);
}
