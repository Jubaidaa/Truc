#define _POSIX_C_SOURCE 200809L
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

const char *get_variable(const struct variable *vars, const char *name)
{
    if (!name)
    {
        return NULL;
    }
    for (const struct variable *v = vars; v; v = v->next)
    {
        if (strcmp(v->name, name) == 0)
        {
            return v->value;
        }
    }
    return NULL;
}

static char *build_deps_string(char **deps)
{
    size_t total = 0;
    for (size_t i = 0; deps[i]; i++)
    {
        total += strlen(deps[i]) + 1;
    }
    char *result = malloc(total + 1);
    if (!result)
    {
        fprintf(stderr, "malloc failed\n");
        exit(1);
    }
    result[0] = '\0';
    for (size_t i = 0; deps[i]; i++)
    {
        if (i > 0)
        {
            strcat(result, " ");
        }
        strcat(result, deps[i]);
    }
    return result;
}

static char *expand_automatic(const char *var, const struct rule *ctx)
{
    if (!ctx)
    {
        return strdup("");
    }
    if (strcmp(var, "@") == 0)
    {
        return strdup(ctx->target ? ctx->target : "");
    }
    if (strcmp(var, "<") == 0)
    {
        if (ctx->deps && ctx->deps[0])
        {
            return strdup(ctx->deps[0]);
        }
        return strdup("");
    }
    if (strcmp(var, "^") == 0 && ctx->deps)
    {
        return build_deps_string(ctx->deps);
    }
    return strdup("");
}

static void append_string(char **result, size_t *len, size_t *cap,
                          const char *str)
{
    size_t slen = strlen(str);
    while (*len + slen + 1 > *cap)
    {
        *cap *= 2;
        *result = realloc(*result, *cap);
        if (!*result)
        {
            fprintf(stderr, "malloc failed\n");
            exit(1);
        }
    }
    strcpy(*result + *len, str);
    *len += slen;
}

static size_t handle_paren_var(const char *line, size_t i, char **result,
                                size_t *len, size_t *cap,
                                const struct variable *vars)
{
    size_t j = i + 2;
    while (line[j] && line[j] != ')')
    {
        j++;
    }
    if (line[j] == ')')
    {
        size_t name_len = j - i - 2;
        char *name = malloc(name_len + 1);
        if (!name)
        {
            fprintf(stderr, "malloc failed\n");
            exit(1);
        }
        memcpy(name, line + i + 2, name_len);
        name[name_len] = '\0';
        const char *val = get_variable(vars, name);
        append_string(result, len, cap, val ? val : "");
        free(name);
        return j + 1;
    }
    return i;
}

static size_t handle_auto_var(const char *line, size_t i, char **result,
                               size_t *len, size_t *cap,
                               const struct rule *ctx)
{
    char c = line[i + 1];
    if (c == '@' || c == '<' || c == '^')
    {
        char var_name[2];
        var_name[0] = c;
        var_name[1] = '\0';
        char *val = expand_automatic(var_name, ctx);
        append_string(result, len, cap, val);
        free(val);
        return i + 2;
    }
    return i;
}

char *expand_variables(const char *line, const struct variable *vars,
                       const struct rule *ctx)
{
    if (!line)
    {
        return NULL;
    }
    size_t cap = 256;
    char *result = malloc(cap);
    if (!result)
    {
        fprintf(stderr, "malloc failed\n");
        exit(1);
    }
    size_t len = 0;
    result[0] = '\0';

    for (size_t i = 0; line[i];)
    {
        if (line[i] == '$' && line[i + 1] == '(')
        {
            i = handle_paren_var(line, i, &result, &len, &cap, vars);
            continue;
        }
        if (line[i] == '$' && line[i + 1])
        {
            size_t new_i = handle_auto_var(line, i, &result, &len,
                                           &cap, ctx);
            if (new_i != i)
            {
                i = new_i;
                continue;
            }
        }
        while (len + 2 > cap)
        {
            cap *= 2;
            result = realloc(result, cap);
            if (!result)
            {
                fprintf(stderr, "malloc failed\n");
                exit(1);
            }
        }
        result[len++] = line[i++];
        result[len] = '\0';
    }
    return result;
}

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

int build_rule_inner(struct rule *rules, struct variable *vars, struct rule *r)
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
            printf("minimake: Nothing to be done for '%s'.\n", r->target);
        else
            printf("minimake: '%s' is up to date.\n", r->target);
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
