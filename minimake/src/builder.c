#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include "builder.h"
#include "micro_shell.h"
#include "aux_builder.h"
#include "utils.h"

static int file_mtime(const char *path, time_t *t)
{
    struct stat st;
    if (stat(path, &st) == 0)
    {
        *t = st.st_mtime;
        return 0;
    }
    return -1;
}

struct rule *find_rule(struct rule *rules, const char *target)
{
    for (struct rule *r = rules; r; r = r->next)
    {
        if (strcmp(r->target, target) == 0)
            return r;
    }
    return NULL;
}

static int execute_command(const char *cmd)
{
    int silent = (cmd[0] == '@');
    const char *actual = silent ? cmd + 1 : cmd;
    if (!silent)
        printf("%s\n", actual);
    return micro_shell(actual);
}

static int needs_rebuild(struct rule *r)
{
    if (!r->target || r->phony)
        return 1;

    time_t tgt_time;
    if (file_mtime(r->target, &tgt_time) != 0)
        return 1;

    if (r->deps)
    {
        for (size_t i = 0; r->deps[i]; i++)
        {
            time_t dep_time;
            if (file_mtime(r->deps[i], &dep_time) == 0)
            {
                if (dep_time > tgt_time)
                    return 1;
            }
            else
            {
                /* Dependency missing */
                return 1;
            }
        }
    }
    return 0;
}

static int build_rule_inner(struct rule *rules, struct variable *vars, struct rule *r)
{
    if (!r)
        return 1;

    if (r->visiting)
    {
        fprintf(stderr, "cycle detected at target '%s'\n", r->target);
        return 1;
    }
    if (r->built)
        return 0;

    r->visiting = 1;

    if (r->deps)
    {
        for (size_t i = 0; r->deps[i]; i++)
        {
            struct rule *dep_rule = find_rule(rules, r->deps[i]);
            if (dep_rule)
            {
                if (build_rule_inner(rules, vars, dep_rule))
                {
                    r->visiting = 0;
                    return 1;
                }
            }
        }
    }

    int need = needs_rebuild(r);
    if (!need)
    {
        if (!r->cmds || !r->cmds[0])
            printf("minimake: '%s' is up to date.\n", r->target);
        else
            printf("minimake: Nothing to be done for '%s'.\n", r->target);
        r->visiting = 0;
        r->built = 1;
        return 0;
    }

    if (r->cmds)
    {
        for (size_t j = 0; r->cmds[j]; j++)
        {
            char *expanded = expand_variables(r->cmds[j], vars, r);
            int ret = execute_command(expanded);
            free(expanded);
            if (ret)
            {
                r->visiting = 0;
                return ret;
            }
        }
    }
    else if (!r->phony)
    {
        /* No commands and must rebuild → invalid */
        printf("minimake: Nothing to be done for '%s'.\n", r->target);
    }

    r->visiting = 0;
    r->built = 1;
    return 0;
}

int build_rule(struct rule *rules, struct variable *vars, const char *target)
{
    struct rule *r = find_rule(rules, target);
    if (!r)
    {
        fprintf(stderr, "Unknown target: %s\n", target);
        return 1;
    }
    return build_rule_inner(rules, vars, r);
}

void free_variables(struct variable *vars)
{
    while (vars)
    {
        struct variable *n = vars->next;
        free(vars->name);
        free(vars->value);
        free(vars);
        vars = n;
    }
}

void free_rules(struct rule *rules)
{
    while (rules)
    {
        struct rule *n = rules->next;
        if (rules->deps)
        {
            for (size_t i = 0; rules->deps[i]; i++)
                free(rules->deps[i]);
            free(rules->deps);
        }
        if (rules->cmds)
        {
            for (size_t j = 0; rules->cmds[j]; j++)
                free(rules->cmds[j]);
            free(rules->cmds);
        }
        free(rules->target);
        free(rules);
        rules = n;
    }
}
