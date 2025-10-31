#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "builder.h"

static char *xstrdup(const char *s)
{
    if (!s)
        return NULL;
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
        return NULL;
    size_t len = strlen(buf);
    if (len > 0 && buf[len - 1] == '\n')
        buf[len - 1] = '\0';
    return xstrdup(buf);
}

static void add_str(char ***arr, size_t *count, const char *val)
{
    *arr = realloc(*arr, (*count + 2) * sizeof(char *));
    if (!*arr)
    {
        fprintf(stderr, "malloc failed\n");
        exit(1);
    }
    (*arr)[*count] = xstrdup(val);
    (*arr)[*count + 1] = NULL;
    (*count)++;
}

static struct rule *create_rule(const char *target)
{
    struct rule *r = malloc(sizeof(*r));
    if (!r)
    {
        fprintf(stderr, "malloc failed\n");
        exit(1);
    }
    r->target = xstrdup(target);
    r->deps = NULL;
    r->cmds = NULL;
    r->phony = !strcmp(target, ".PHONY");
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

static int ends_with(const char *str, char c)
{
    size_t len = strlen(str);
    return len && str[len - 1] == c;
}

struct parse_var_ctx
{
    FILE *f;
    struct variable **vars;
    struct variable **vtail;
};

struct parse_rule_ctx
{
    FILE *f;
    struct rule **rules;
    struct rule **rtail;
};

static void parse_variable_line(char *line, struct parse_var_ctx *ctx)
{
    line[strlen(line) - 1] = '\0';
    char *value = read_line(ctx->f);
    struct variable *v = malloc(sizeof(*v));
    if (!v)
    {
        fprintf(stderr, "malloc failed\n");
        exit(1);
    }
    v->name = xstrdup(line);
    v->value = value ? value : xstrdup("");
    v->next = NULL;
    if (!*ctx->vars)
        *ctx->vars = v;
    else
        (*ctx->vtail)->next = v;
    *ctx->vtail = v;
}

static struct rule *parse_rule_block(char *line, struct parse_rule_ctx *ctx)
{
    line[strlen(line) - 1] = '\0';
    struct rule *r = create_rule(line);
    add_rule_to_list(r, ctx->rules, ctx->rtail);
    size_t dep_count = 0;
    char *dep;
    while ((dep = read_line(ctx->f)) && dep[0] != '\0')
    {
        add_str(&r->deps, &dep_count, dep);
        free(dep);
    }
    free(dep);
    return r;
}

int parse_rule_file(const char *path, struct variable **vars,
                    struct rule **rules)
{
    FILE *f = fopen(path, "r");
    if (!f)
        return 1;
    *vars = NULL;
    *rules = NULL;
    struct variable *vtail = NULL;
    struct rule *rtail = NULL;
    struct rule *current = NULL;
    size_t cmd_count = 0;
    int in_rule = 0;
    struct parse_var_ctx vctx = {f, vars, &vtail};
    struct parse_rule_ctx rctx = {f, rules, &rtail};
    char *line;
    while ((line = read_line(f)))
    {
        if (line[0] == '\0')
        {
            if (!in_rule)
            {
                current = NULL;
                cmd_count = 0;
            }
            free(line);
            continue;
        }
        if (ends_with(line, '='))
        {
            parse_variable_line(line, &vctx);
            current = NULL;
            in_rule = 0;
            free(line);
            continue;
        }
        if (ends_with(line, ':'))
        {
            current = parse_rule_block(line, &rctx);
            cmd_count = 0;
            in_rule = 1;
            free(line);
            continue;
        }
        if (current && line[0] != '\0')
            add_str(&current->cmds, &cmd_count, line);
        free(line);
    }
    fclose(f);
    return 0;
}
