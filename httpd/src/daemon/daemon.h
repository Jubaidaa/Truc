#ifndef DAEMON_H
#define DAEMON_H

#include <sys/types.h>

#include "../config/config.h"

int daemon_start(struct server_config *config);
int daemon_stop(struct server_config *config);
int daemon_restart(struct server_config *config);
int daemon_check_running(const char *pid_file, pid_t *pid);
int daemon_write_pid(const char *pid_file);
int daemon_remove_pid(const char *pid_file);

#endif // ! DAEMON_H
