#ifndef SIMPLE_LS_H
#define SIMPLE_LS_H

#include <dirent.h>

int list_dir_stream(DIR *dir);
int list_dir(const char *dirname);

#endif /* ! SIMPLE_LS_H */
