#ifndef AUX_BUILDER_H
#define AUX_BUILDER_H

#include "builder.h"

int parse_rule_file(const char *path, struct variable **vars, struct rule **rules);
char *xstrdup(const char *s);
char *rstrip(char *s);
int is_blank(const char *s);

#endif /* ! AUX_BUILDER_H */

