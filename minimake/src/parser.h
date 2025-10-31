#ifndef PARSER_H
#define PARSER_H

#include "builder.h"

int parse_file(const char *filename, struct variable **vars, struct rule **rules);

#endif /* ! PARSER_H */
