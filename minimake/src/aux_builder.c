#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "builder.h"

static char *xstrdup(const char *s)
{
    if (!s)
    {
        return NULL;
    }
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

static struct rule *create_rule(char *target_name)
{
    struct rule *r = malloc(sizeof(*r));
    if (!r)
    {
        fprintf(stderr, "malloc failed\n");
        exit(1);
    }
    r->target = target_name;
    r->deps = NULL;
    r->cmds = NULL;
    r->phony = !strcmp(target_name, ".PHONY");
    r->visiting = 0;
    r->built = 0;
    r->next = NULL;
    return r;
}

static void add_rule_to_list(struct rule *r, struct rule **rules,
                             struct rule **rtail)
{
    if (!*rules)
    {
        *rules = r;
    }
    else
    {
        (*rtail)->next = r;
    }
    *rtail = r;
}

static int ends_with(const char *str, char c)
{
    if (!str || !str[0])
    {
        return 0;
    }
    size_t len = strlen(str);
    return str[len - 1] == c;
}

int parse_rule_file(const char *path, struct variable **vars,
                    struct rule **rules)
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
    size_t cmd_count = 0;

    char *line;
    while ((line = read_line(f)))
    {
        if (line[0] == '\0')
        {
            current_rule = NULL;
            cmd_count = 0;
            free(line);
            continue;
        }

        if (ends_with(line, '='))
        {
            line[strlen(line) - 1] = '\0';
            char *value = read_line(f);
            
            struct variable *v = malloc(sizeof(*v));
            if (!v)
            {
                fprintf(stderr, "malloc failed\n");
                exit(1);
            }
            v->name = xstrdup(line);
            v->value = value ? value : xstrdup("");
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
            free(line);
            continue;
        }

        if (ends_with(line, ':'))
        {
            line[strlen(line) - 1] = '\0';
            
            struct rule *r = create_rule(xstrdup(line));
            add_rule_to_list(r, rules, &rtail);
            current_rule = r;
            cmd_count = 0;
            free(line);
            
            size_t dep_count = 0;
            char *dep;
            while ((dep = read_line(f)) && dep[0] != '\0')
            {
                add_dep(&current_rule->deps, &dep_count, dep);
                free(dep);
            }
            free(dep);
            continue;
        }

        if (current_rule && line[0] != '\0')
        {
            add_cmd(&current_rule->cmds, &cmd_count, line);
            free(line);
            continue;
        }

        free(line);
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
