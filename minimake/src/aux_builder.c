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
    size_t dep_count = 0;
    size_t cmd_count = 0;

    char *line;
    while ((line = read_line(f)))
    {
        fprintf(stderr, "[DEBUG] Read line: '%s'\n", line);
        
        if (line[0] == '\0')
        {
            fprintf(stderr, "[DEBUG] Empty line - resetting current_rule\n");
            current_rule = NULL;
            dep_count = 0;
            cmd_count = 0;
            free(line);
            continue;
        }

        char *next = read_line(f);
        if (next)
        {
            fprintf(stderr, "[DEBUG] Next line: '%s'\n", next);
        }
        
        if (next && !strcmp(next, "="))
        {
            char *value = read_line(f);
            fprintf(stderr, "[DEBUG] Variable: %s = %s\n", line, value ? value : "(null)");
            
            struct variable *v = malloc(sizeof(*v));
            if (!v)
            {
                fprintf(stderr, "malloc failed\n");
                exit(1);
            }
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
        
        if (next && !strcmp(next, ":"))
        {
            fprintf(stderr, "[DEBUG] Target: %s\n", line);
            struct rule *r = create_rule(line);
            add_rule_to_list(r, rules, &rtail);
            current_rule = r;
            dep_count = 0;
            cmd_count = 0;
            free(next);
            
            while ((line = read_line(f)) && line[0] != '\0')
            {
                fprintf(stderr, "[DEBUG]   Dependency: %s\n", line);
                add_dep(&current_rule->deps, &dep_count, line);
                free(line);
            }
            free(line);
            continue;
        }
        
        if (current_rule && line[0] != '\0')
        {
            fprintf(stderr, "[DEBUG]   Command: %s\n", line);
            add_cmd(&current_rule->cmds, &cmd_count, line);
        }
        
        free(line);
        if (next)
        {
            free(next);
        }
    }

    fprintf(stderr, "[DEBUG] === Parsed rules ===\n");
    for (struct rule *r = *rules; r; r = r->next)
    {
        fprintf(stderr, "[DEBUG] Rule: %s\n", r->target);
        if (r->deps)
        {
            for (int i = 0; r->deps[i]; i++)
            {
                fprintf(stderr, "[DEBUG]   dep: %s\n", r->deps[i]);
            }
        }
        if (r->cmds)
        {
            for (int i = 0; r->cmds[i]; i++)
            {
                fprintf(stderr, "[DEBUG]   cmd: %s\n", r->cmds[i]);
            }
        }
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
