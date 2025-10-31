#ifndef BUILDER_H
#define BUILDER_H

#include <stddef.h>

struct variable
{
    char *name;
    char *value;
    struct variable *next;
};

struct rule
{
    char *target;
    char **deps;
    char **cmds;
    int phony;
    int visiting;
    int built;
    struct rule *next;
};

const char *get_variable(const struct variable *vars, const char *name);
struct rule *find_rule(struct rule *rules, const char *target);
char *expand_variables(const char *line, const struct variable *vars,
                       const struct rule *ctx);
int build_rule(struct rule *rules, struct variable *vars, const char *target);
int build_rule_inner(struct rule *rules, struct variable *vars, struct rule *r);
void free_variables(struct variable *vars);
void free_rules(struct rule *rules);

#endif /* ! BUILDER_H */
