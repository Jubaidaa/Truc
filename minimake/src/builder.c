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

struct append_ctx
{
    char **res;
    size_t *len;
    size_t *cap;
};

struct expand_ctx
{
    const char *line;
    const struct variable *vars;
    const struct rule *ctx;
};

const char *get_variable(const struct variable *vars, const char *name)
{
    for (const struct variable *v = vars; v; v = v->next)
        if (strcmp(v->name, name) == 0)
            return v->value;
    return NULL;
}

static void append_str(struct append_ctx *a, const char *s)
{
    size_t slen = strlen(s);
    while (*a->len + slen + 1 > *a->cap)
    {
        *a->cap *= 2;
        *a->res = realloc(*a->res, *a->cap);
        if (!*a->res)
        {
            fprintf(stderr, "malloc failed\n");
            exit(1);
        }
    }
    strcpy(*a->res + *a->len, s);
    *a->len += slen;
}

static char *build_deps_string(char **deps)
{
    size_t total = 0;
    for (size_t i = 0; deps && deps[i]; i++)
        total += strlen(deps[i]) + 1;
    char *res = malloc(total + 1);
    if (!res)
    {
        fprintf(stderr, "malloc failed\n");
        exit(1);
    }
    res[0] = '\0';
    for (size_t i = 0; deps && deps[i]; i++)
    {
        if (i)
            strcat(res, " ");
        strcat(res, deps[i]);
    }
    return res;
}

static char *expand_auto(const char *name, const struct rule *ctx)
{
    if (!ctx)
        return strdup("");
    if (!strcmp(name, "@"))
        return strdup(ctx->target ? ctx->target : "");
    if (!strcmp(name, "<"))
        return ctx->deps && ctx->deps[0] ? strdup(ctx->deps[0]) : strdup("");
    if (!strcmp(name, "^"))
        return build_deps_string(ctx->deps);
    return strdup("");
}

static size_t handle_paren(struct append_ctx *a, struct expand_ctx *e, size_t i)
{
    const char *line = e->line;
    size_t j = i + 2;
    while (line[j] && line[j] != ')')
        j++;
    if (line[j] != ')')
        return i;
    size_t len = j - i - 2;
    char *name = malloc(len + 1);
    memcpy(name, line + i + 2, len);
    name[len] = '\0';
    const char *val = get_variable(e->vars, name);
    append_str(a, val ? val : "");
    free(name);
    return j + 1;
}

static size_t handle_auto(struct append_ctx *a, struct expand_ctx *e, size_t i)
{
    char c = e->line[i + 1];
    if (c != '@' && c != '<' && c != '^')
        return i;
    char name[2] = {c, '\0'};
    char *val = expand_auto(name, e->ctx);
    append_str(a, val);
    free(val);
    return i + 2;
}

char *expand_variables(const char *line, const struct variable *vars,
                       const struct rule *ctx)
{
    struct expand_ctx e = {line, vars, ctx};
    size_t cap = 256, len = 0;
    char *res = malloc(cap);
    if (!res)
    {
        fprintf(stderr, "malloc failed\n");
        exit(1);
    }
    res[0] = '\0';
    struct append_ctx a = {&res, &len, &cap};

    for (size_t i = 0; line && line[i];)
    {
        if (line[i] == '$' && line[i + 1] == '(')
        {
            i = handle_paren(&a, &e, i);
            continue;
        }
        if (line[i] == '$')
        {
            size_t new_i = handle_auto(&a, &e, i);
            if (new_i != i)
            {
                i = new_i;
                continue;
            }
        }
        append_str(&a, (char[]){line[i], '\0'});
        i++;
    }
    return res;
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
        if (!strcmp(r->target, target))
            return r;
    return NULL;
}

static int exec_cmd(const char *cmd)
{
    int silent = (cmd[0] == '@');
    const char *act = silent ? cmd + 1 : cmd;
    if (!silent)
        printf("%s\n", act);
    return micro_shell(act);
}

static char **split_deps(const char *str)
{
    if (!str || !str[0])
        return NULL;
    size_t count = 1;
    for (size_t i = 0; str[i]; i++)
        if (str[i] == ' ' && str[i + 1] && str[i + 1] != ' ')
            count++;
    char **result = malloc((count + 1) * sizeof(char *));
    if (!result)
    {
        fprintf(stderr, "malloc failed\n");
        exit(1);
    }
    size_t idx = 0, start = 0;
    for (size_t i = 0; str[i]; i++)
    {
        if (str[i] == ' ' || str[i + 1] == '\0')
        {
            size_t end = (str[i] == ' ') ? i : i + 1;
            if (end > start)
            {
                size_t len = end - start;
                result[idx] = malloc(len + 1);
                if (!result[idx])
                {
                    fprintf(stderr, "malloc failed\n");
                    exit(1);
                }
                memcpy(result[idx], str + start, len);
                result[idx][len] = '\0';
                idx++;
            }
            start = i + 1;
        }
    }
    result[idx] = NULL;
    return result;
}

static int check_dep_time(const char *dep, time_t t, const struct variable *v)
{
    char *expanded = expand_variables(dep, v, NULL);
    char **files = split_deps(expanded);
    int needs = 0;
    if (files)
    {
        for (size_t i = 0; files[i]; i++)
        {
            time_t d;
            if (file_mtime(files[i], &d) || d > t)
            {
                needs = 1;
            }
            free(files[i]);
        }
        free(files);
    }
    free(expanded);
    return needs;
}

static int needs_rebuild_check(struct rule *r, const struct variable *v)
{
    if (!r->target || r->phony)
        return 1;
    char *target_exp = expand_variables(r->target, v, NULL);
    time_t t;
    int stat_ret = file_mtime(target_exp, &t);
    if (stat_ret)
    {
        free(target_exp);
        return 1;
    }
    for (size_t i = 0; r->deps && r->deps[i]; i++)
    {
        if (check_dep_time(r->deps[i], t, v))
        {
            free(target_exp);
            return 1;
        }
    }
    free(target_exp);
    return 0;
}

int build_rule_inner(struct rule *rules, struct variable *vars, struct rule *r)
{
    if (!r)
        return 1;
    if (r->visiting)
        return 1;
    if (r->built)
        return 0;
    r->visiting = 1;
    for (size_t i = 0; r->deps && r->deps[i]; i++)
    {
        struct rule *d = find_rule(rules, r->deps[i]);
        if (d && build_rule_inner(rules, vars, d))
        {
            r->visiting = 0;
            return 1;
        }
    }
    if (!needs_rebuild_check(r, vars))
    {
        char *target_exp = expand_variables(r->target, vars, NULL);
        if (!r->cmds || !r->cmds[0])
            printf("minimake: Nothing to be done for '%s'.\n", target_exp);
        else
            printf("minimake: '%s' is up to date.\n", target_exp);
        free(target_exp);
        r->visiting = 0;
        r->built = 1;
        return 0;
    }
    for (size_t j = 0; r->cmds && r->cmds[j]; j++)
    {
        char *exp = expand_variables(r->cmds[j], vars, r);
        int ret = exec_cmd(exp);
        free(exp);
        if (ret)
        {
            r->visiting = 0;
            return ret;
        }
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
        for (size_t i = 0; rules->deps && rules->deps[i]; i++)
            free(rules->deps[i]);
        free(rules->deps);
        for (size_t j = 0; rules->cmds && rules->cmds[j]; j++)
            free(rules->cmds[j]);
        free(rules->cmds);
        free(rules->target);
        free(rules);
        rules = n;
    }
}
