#define _POSIX_C_SOURCE 200809L

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "builder.h"
#include "parser.h"

struct parse_ctx
{
    struct variable **vars;
    struct variable *vtail;
    struct rule **rules;
    struct rule *rtail;
    struct rule *current_rule;
    size_t cmd_count;
};

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

static char *rstrip(char *s)
{
    size_t n = strlen(s);
    while (n > 0)
    {
        char ch = s[n - 1];
        if (ch == ' ' || ch == '\t' || ch == '\r' || ch == '\n')
        {
            s[--n] = '\0';
        }
        else
        {
            break;
        }
    }
    return s;
}

static char *lstrip(char *s)
{
    size_t i = 0;
    while (s[i] == ' ' || s[i] == '\t')
    {
        i++;
    }
    return s + i;
}

static void strip_comment(char *line)
{
    for (int i = 0; line[i]; i++)
    {
        if (line[i] == '#')
        {
            line[i] = '\0';
            return;
        }
    }
}

static void ensure_capacity(char **res, size_t *cap, size_t needed)
{
    while (needed > *cap)
    {
        *cap *= 2;
        *res = realloc(*res, *cap);
        if (!*res)
        {
            fprintf(stderr, "malloc failed\n");
            exit(1);
        }
    }
}

static void append_char(char **res, size_t *cap, size_t *len, char c)
{
    ensure_capacity(res, cap, *len + 2);
    (*res)[(*len)++] = c;
    (*res)[*len] = '\0';
}

static char *expand_env_vars(const char *s)
{
    size_t cap = 256;
    size_t len = 0;
    char *res = malloc(cap);
    if (!res)
    {
        fprintf(stderr, "malloc failed\n");
        exit(1);
    }
    res[0] = '\0';
    for (size_t i = 0; s[i] != '\0'; i++)
    {
        if (s[i] == '$' && s[i + 1] == '$')
        {
            append_char(&res, &cap, &len, '$');
            i++;
        }
        else if (s[i] == '$' && s[i + 1] == '{')
        {
            size_t j = i + 2;
            while (s[j] && s[j] != '}') j++;
            if (s[j] != '}')
            {
                append_char(&res, &cap, &len, s[i]);
                continue;
            }
            size_t name_len = j - i - 2;
            char *name = malloc(name_len + 1);
            memcpy(name, s + i + 2, name_len);
            name[name_len] = '\0';
            const char *val = getenv(name);
            if (val)
            {
                size_t val_len = strlen(val);
                ensure_capacity(&res, &cap, len + val_len + 1);
                strcpy(res + len, val);
                len += val_len;
            }
            else
            {
                ensure_capacity(&res, &cap, len + name_len + 4);
                len += sprintf(res + len, "$(%s)", name);
            }
            free(name);
            i = j;
        }
        else
        {
            append_char(&res, &cap, &len, s[i]);
        }
    }
    return res;
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

static void add_variable(struct parse_ctx *ctx, const char *name, const char *value)
{
    struct variable *v = malloc(sizeof(*v));
    if (!v)
    {
        fprintf(stderr, "malloc failed\n");
        exit(1);
    }
    v->name = xstrdup(name);
    v->value = xstrdup(value);
    v->next = NULL;

    if (!*ctx->vars)
        *ctx->vars = v;
    else
        ctx->vtail->next = v;
    ctx->vtail = v;
}

static void add_rule(struct parse_ctx *ctx, struct rule *r)
{
    if (!*ctx->rules)
        *ctx->rules = r;
    else
        ctx->rtail->next = r;
    ctx->rtail = r;
}

static void process_assignment(struct parse_ctx *ctx, char *lhs, char *rhs)
{
    char *name = rstrip(lstrip(lhs));
    char *value = lstrip(rhs);
    
    char *expanded_name = expand_env_vars(name);
    char *expanded_value = expand_env_vars(value);
    
    add_variable(ctx, expanded_name, expanded_value);
    
    free(expanded_name);
    free(expanded_value);
}

static void process_target(struct parse_ctx *ctx, char *lhs, char *rhs)
{
    char *target = rstrip(lstrip(lhs));
    char *deps_str = lstrip(rhs);
    
    char *expanded_target = expand_env_vars(target);
    struct rule *r = create_rule(expanded_target);
    add_rule(ctx, r);
    ctx->current_rule = r;
    ctx->cmd_count = 0;
    
    free(expanded_target);

    if (deps_str && *deps_str)
    {
        char *expanded_deps = expand_env_vars(deps_str);
        size_t dep_count = 0;

        char *dep_copy = xstrdup(expanded_deps);
        char *saveptr = NULL;
        char *dep = strtok_r(dep_copy, " \t", &saveptr);
        
        while (dep)
        {
            add_str(&r->deps, &dep_count, dep);
            dep = strtok_r(NULL, " \t", &saveptr);
        }
        
        free(dep_copy);
        free(expanded_deps);
    }
}

static void process_recipe(struct parse_ctx *ctx, const char *line)
{
    if (!ctx->current_rule)
        return;

    const char *cmd = line + 1;
    char *expanded = expand_env_vars(cmd);
    
    add_str(&ctx->current_rule->cmds, &ctx->cmd_count, expanded);
    
    free(expanded);
}

static void process_line(struct parse_ctx *ctx, char *raw)
{
    rstrip(raw);
    strip_comment(raw);
    
    char *line = lstrip(raw);
    if (!line[0])
        return;

    if (raw[0] == '\t')
    {
        process_recipe(ctx, raw);
        return;
    }
    
    int index = -1;
    char sym = '\0';
    for (int i = 0; line[i]; i++)
    {
        if (line[i] == '=' || line[i] == ':')
        {
            index = i;
            sym = line[i];
            break;
        }
    }
    
    if (index >= 0)
    {
        line[index] = '\0';
        char *lhs = line;
        char *rhs = line + index + 1;
        
        if (sym == '=')
        {
            process_assignment(ctx, lhs, rhs);
            ctx->current_rule = NULL; 
        }
        else
        {
            process_target(ctx, lhs, rhs);
        }
    }
}

int parse_file(const char *filename, struct variable **vars, struct rule **rules)
{
    FILE *in = fopen(filename, "r");
    if (!in)
    {
        fprintf(stderr, "Cannot open %s: %s\n", filename, strerror(errno));
        return 1;
    }
    
    *vars = NULL;
    *rules = NULL;
    
    struct parse_ctx ctx = {
        .vars = vars,
        .vtail = NULL,
        .rules = rules,
        .rtail = NULL,
        .current_rule = NULL,
        .cmd_count = 0
    };
    
    char buf[4096];
    while (fgets(buf, sizeof(buf), in))
    {
        process_line(&ctx, buf);
    }
    
    fclose(in);
    return 0;
}