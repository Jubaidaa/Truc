#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "builder.h"
#include "utils.h"

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

static void parse_variable(FILE *f, char *line, struct variable **vars,
                           struct variable **vtail)
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
        (*vtail)->next = v;
    }
    *vtail = v;
}

static void parse_rule_deps(FILE *f, struct rule *r)
{
    size_t dep_count = 0;
    char *dep;
    while ((dep = read_line(f)) && dep[0] != '\0')
    {
        add_dep(&r->deps, &dep_count, dep);
        free(dep);
    }
    free(dep);
}

static struct rule *create_rule(char *line)
{
    size_t len = strlen(line);
    if (len > 0 && line[len - 1] == ':')
    {
        line[len - 1] = '\0';
    }

    struct rule *r = malloc(sizeof(*r));
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

static struct rule *parse_target(FILE *f, char *line, struct rule **rules,
                                 struct rule **rtail)
{
    struct rule *r = create_rule(line);
    parse_rule_deps(f, r);
    add_rule_to_list(r, rules, rtail);
    return r;
}

static void parse_command(char *line, struct rule *current_rule)
{
    if (!current_rule)
    {
        return;
    }

    size_t cmd_count = 0;
    if (current_rule->cmds)
    {
        while (current_rule->cmds[cmd_count])
        {
            cmd_count++;
        }
    }
    add_cmd(&current_rule->cmds, &cmd_count, line);
}

static int process_line_pair(FILE *f, char *line, char *next,
                             struct variable **vars, struct variable **vtail,
                             struct rule **rules, struct rule **rtail,
                             struct rule **current_rule)
{
    if (!strcmp(next, "="))
    {
        parse_variable(f, line, vars, vtail);
        free(next);
        return 1;
    }

    if (!strcmp(next, ":"))
    {
        *current_rule = parse_target(f, line, rules, rtail);
        free(next);
        return 1;
    }

    parse_command(line, *current_rule);
    free(line);
    free(next);
    return 0;
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

        process_line_pair(f, line, next, vars, &vtail, rules, &rtail,
                          &current_rule);
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

