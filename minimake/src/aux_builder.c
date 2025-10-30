#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "builder.h"
#include "utils.h"

static char *read_line(FILE *f)
{
    char buf[4096];
    if (!fgets(buf, sizeof(buf), f))
        return NULL;
    size_t len = strlen(buf);
    if (len > 0 && buf[len - 1] == '\n')
        buf[len - 1] = '\0';
    return xstrdup(buf);
}

struct rule *find_rule(struct rule *rules, const char *target)
{
    for (struct rule *r = rules; r; r = r->next)
        if (!strcmp(r->target, target))
            return r;
    return NULL;
}

static void add_dep(char ***arr, size_t *count, const char *dep)
{
    *arr = realloc(*arr, (*count + 2) * sizeof(char *));
    if (!*arr)
    {
        fprintf(stderr, "malloc failed\n");
        exit(1);
    }
    (*arr)[*count] = xstrdup(dep);
    (*arr)[*count + 1] = NULL;
    (*count)++;
}

static void add_cmd(char ***arr, size_t *count, const char *cmd)
{
    *arr = realloc(*arr, (*count + 2) * sizeof(char *));
    if (!*arr)
    {
        fprintf(stderr, "malloc failed\n");
        exit(1);
    }
    (*arr)[*count] = xstrdup(cmd);
    (*arr)[*count + 1] = NULL;
    (*count)++;
}

static struct rule *create_rule(char *line)
{
    size_t len = strlen(line);
    if (len > 0 && line[len - 1] == ':')
        line[len - 1] = '\0';

    struct rule *r = malloc(sizeof(*r));
    if (!r)
    {
        fprintf(stderr, "malloc failed\n");
        exit(1);
    }
    r->target = xstrdup(line);
    r->deps = NULL;
    r->cmds = NULL;
    r->phony = !strcmp(line, ".PHONY");
    r->visiting = 0;
    r->built = 0;
    r->next = NULL;
    return r;
}

static void add_rule_to_list(struct rule *r, struct rule **rules,
                             struct rule **tail)
{
    if (!*rules)
        *rules = r;
    else
        (*tail)->next = r;
    *tail = r;
}

int parse_rule_file(const char *path, struct variable **vars,
                    struct rule **rules)
{
    FILE *f = fopen(path, "r");
    if (!f)
        return 1;

    *vars = NULL;
    *rules = NULL;
    struct rule *tail = NULL;
    struct rule *current = NULL;
    char *line;
    size_t cmd_count = 0, dep_count = 0;

    while ((line = read_line(f)))
    {
        if (line[0] == '\0')
        {
            current = NULL;
            free(line);
            continue;
        }
        if (strchr(line, '='))
        {
            char *name = strtok(line, "=");
            char *value = strtok(NULL, "");
            if (!value)
                value = "";
            struct variable *v = malloc(sizeof(*v));
            v->name = xstrdup(name);
            v->value = xstrdup(value);
            v->next = *vars;
            *vars = v;
        }
        else if (line[strlen(line) - 1] == ':')
        {
            struct rule *r = create_rule(line);
            add_rule_to_list(r, rules, &tail);
            current = r;
            dep_count = 0;
            cmd_count = 0;
        }
        else if (current && line[0] == '\t')
        {
            add_cmd(&current->cmds, &cmd_count, line + 1);
        }
        else if (current)
        {
            add_dep(&current->deps, &dep_count, line);
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
                free(rules->deps[i]);
            free(rules->deps);
        }
        if (rules->cmds)
        {
            for (int i = 0; rules->cmds[i]; i++)
                free(rules->cmds[i]);
            free(rules->cmds);
        }
        free(rules);
        rules = next;
    }
}

