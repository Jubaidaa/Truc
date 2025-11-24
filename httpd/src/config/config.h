#ifndef CONFIG_H
#define CONFIG_H

#include <stdbool.h>

struct server_config {
  char *log_file;
  bool log_enabled;
  char *pid_file;
  char *daemon_action;
  char *server_name;
  int port;
  char *ip;
  char *root_dir;
  char *default_file;
};

struct server_config *config_parse(int argc, char **argv);
void config_destroy(struct server_config *config);
void config_print(const struct server_config *config);
bool config_validate(const struct server_config *config);

#endif // ! CONFIG_H
