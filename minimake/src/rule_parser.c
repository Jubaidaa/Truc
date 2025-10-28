#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "builder.h"

static char *xstrdup(const char *s)
{
    if (!s) return NULL;
    size_t n = strlen(s) + 1;
    char *p = malloc(n);
    if (!p)
    {
        fprintf(stderr, "malloc failed\n");
        exit(1);
    }
    memcpy(p, s, n);
    return p;
}

static char *read_line(FILE *f)
{
    char buf[4096];
    if (!fgets(buf, sizeof(buf), f))
    {
        return NULL;
    }
    size_t len = strlen(buf);
    if (len > 0 && buf[len - 1] == '\n')
    {
        buf[len - 1] = '\0';
    }
    return xstrdup(buf);
}

struct rule *find_rule(struct rule *rules, const char *target)
{
    for (struct rule *r = rules; r; r = r->next)
    {
        if (!strcmp(r->target, target))
        {
            return r;
        }
    }
    return NULL;
}

static void add_dep(char ***deps, size_t *count, const char *dep)
{
    *deps = realloc(*deps, (*count + 2) * sizeof(char *));
    if (!*deps)
    {
        fprintf(stderr, "malloc failed\n");
        exit(1);
    }
    (*deps)[*count] = xstrdup(dep);
    (*deps)[*count + 1] = NULL;
    (*count)++;
}

static void add_cmd(char ***cmds, size_t *count, const char *cmd)
{
    *cmds = realloc(*cmds, (*count + 2) * sizeof(char *));
    if (!*cmds)
    {
        fprintf(stderr, "malloc failed\n");
        exit(1);
    }
    (*cmds)[*count] = xstrdup(cmd);
    (*cmds)[*count + 1] = NULL;
    (*count)++;
}

int parse_rule_file(const char *path, struct variable **vars, struct rule **rules)
{
    FILE *f = fopen(path, "r");
    if (!f)
    {
        return 1;
    }

    *vars = NULL;
    *rules = NULL;
    struct variable *vtail = NULL;
    struct rule *rtail = NULL;
    struct rule *current_rule = NULL;

    char *line;
    while ((line = read_line(f)))
    {
        if (line[0] == '\0')
        {
            current_rule = NULL;
            free(line);
            continue;
        }

        char *next = read_line(f);
        if (!next || next[0] == '\0')
        {
            free(line);
            free(next);
            current_rule = NULL;
            continue;
        }

        if (!strcmp(next, "="))
        {
            char *value = read_line(f);
            struct variable *v = malloc(sizeof(*v));
            v->name = line;
            v->value = value;
            v->next = NULL;

            if (!*vars)
            {
                *vars = v;
            }
            else
            {
                vtail->next = v;
            }
            vtail = v;
            free(next);
            continue;
        }

        if (!strcmp(next, ":"))
        {
            struct rule *r = malloc(sizeof(*r));
            r->target = line;
            r->deps = NULL;
            r->cmds = NULL;
            r->phony = !strcmp(line, ".PHONY");
            r->visiting = 0;
            r->built = 0;
            r->next = NULL;

            size_t dep_count = 0;
            char *dep;
            while ((dep = read_line(f)) && dep[0] != '\0')
            {
                add_dep(&r->deps, &dep_count, dep);
                free(dep);
            }
            free(dep);

            if (!*rules)
            {
                *rules = r;
            }
            else
            {
                rtail->next = r;
            }
            rtail = r;
            current_rule = r;
            free(next);
            continue;
        }

        if (current_rule)
        {
            size_t cmd_count = 0;
            if (!current_rule->cmds)
            {
                current_rule->cmds = NULL;
            }
            else
            {
                while (current_rule->cmds[cmd_count])
                {
                    cmd_count++;
                }
            }
            add_cmd(&current_rule->cmds, &cmd_count, line);
        }

        free(line);
        free(next);
    }

    fclose(f);
    return 0;
}

void free_variables(struct variable *vars)
{
    while (vars)
    {
        struct variable *next = vars->next;
        free(vars->name);
        free(vars->value);
        free(vars);
        vars = next;
    }
}

void free_rules(struct rule *rules)
{
    while (rules)
    {
        struct rule *next = rules->next;
        free(rules->target);
        if (rules->deps)
        {
            for (int i = 0; rules->deps[i]; i++)
            {
                free(rules->deps[i]);
            }
            free(rules->deps);
        }
        if (rules->cmds)
        {
            for (int i = 0; rules->cmds[i]; i++)
            {
                free(rules->cmds[i]);
            }
            free(rules->cmds);
        }
        free(rules);
        rules = next;
    }
}
