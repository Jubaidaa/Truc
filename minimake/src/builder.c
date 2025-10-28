#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include "builder.h"
#include "micro_shell.h"

static char *xstrdup(const char *s)
{
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

static int newer_than(const char *target, char **deps)
{
    struct stat stt;
    if (stat(target, &stt) != 0)
    {
        return 1;
    }
    for (int i = 0; deps && deps[i]; i++)
    {
        struct stat std;
        if (stat(deps[i], &std) == 0 && std.st_mtime > stt.st_mtime)
        {
            return 1;
        }
    }
    return 0;
}

static int execute_command(const char *cmd)
{
    const char *run = cmd;
    if (cmd[0] == '@')
    {
        run = cmd + 1;
    }
    else
    {
        printf("%s\n", cmd);
    }
    fflush(stdout);
    fflush(stderr);
    int ret = micro_shell(run);
    fflush(stdout);
    fflush(stderr);
    return ret;
}

static char *join_with_space(char **items)
{
    size_t total = 0;
    size_t count = 0;
    if (!items)
    {
        return xstrdup("");
    }
    for (size_t i = 0; items[i]; i++)
    {
        total += strlen(items[i]);
        count++;
    }
    if (count == 0)
    {
        return xstrdup("");
    }
    total += count - 1;
    char *out = malloc(total + 1);
    if (!out)
    {
        fprintf(stderr, "malloc failed\n");
        exit(1);
    }
    out[0] = '\0';
    for (size_t i = 0; items[i]; i++)
    {
        if (i > 0)
        {
            strcat(out, " ");
        }
        strcat(out, items[i]);
    }
    return out;
}

const char *get_variable(const struct variable *vars, const char *name)
{
    for (const struct variable *v = vars; v; v = v->next)
    {
        if (!strcmp(v->name, name))
        {
            return v->value ? v->value : "";
        }
    }
    const char *env = getenv(name);
    return env ? env : "";
}

static char *expand_once(const char *in, const struct variable *vars,
                         const struct rule *ctx)
{
    size_t i = 0;
    size_t cap = strlen(in) + 128;
    char *out = malloc(cap);
    if (!out)
    {
        fprintf(stderr, "malloc failed\n");
        exit(1);
    }
    out[0] = '\0';
    while (in[i])
    {
        if (in[i] == '$')
        {
            if (in[i + 1] == '@' && ctx)
            {
                strcat(out, ctx->target);
                i += 2;
                continue;
            }
            if (in[i + 1] == '<' && ctx && ctx->deps && ctx->deps[0])
            {
                strcat(out, ctx->deps[0]);
                i += 2;
                continue;
            }
            if (in[i + 1] == '^' && ctx)
            {
                char *joined = join_with_space(ctx->deps);
                strcat(out, joined);
                free(joined);
                i += 2;
                continue;
            }
            if (in[i + 1] == '(')
            {
                char name[256];
                size_t j = i + 2;
                size_t k = 0;
                while (in[j] && in[j] != ')' && k + 1 < sizeof(name))
                {
                    name[k++] = in[j++];
                }
                name[k] = '\0';
                if (in[j] == ')')
                {
                    j++;
                }
                const char *val = get_variable(vars, name);
                strcat(out, val);
                i = j;
                continue;
            }
        }
        size_t len = strlen(out);
        if (len + 2 > cap)
        {
            cap += 128;
            out = realloc(out, cap);
        }
        out[len] = in[i];
        out[len + 1] = '\0';
        i++;
    }
    return out;
}

char *expand_variables(const char *line, const struct variable *vars,
                       const struct rule *ctx)
{
    char *prev = strdup(line);
    for (int depth = 0; depth < 10; depth++)
    {
        char *next = expand_once(prev, vars, ctx);
        if (!strcmp(next, prev))
        {
            free(prev);
            return next;
        }
        free(prev);
        prev = next;
    }
    return prev;
}

static int build_rule_inner(struct rule *rules, struct variable *vars,
                            struct rule *r)
{
    if (!r)
    {
        return 1;
    }
    if (r->visiting)
    {
        fprintf(stderr, "cycle detected at target '%s'\n", r->target);
        return 2;
    }
    if (r->built)
    {
        return 0;
    }
    r->visiting = 1;
    if (r->deps)
    {
        for (int i = 0; r->deps[i]; i++)
        {
            struct rule *dr = find_rule(rules, r->deps[i]);
            if (dr)
            {
                int ret = build_rule_inner(rules, vars, dr);
                if (ret)
                {
                    r->visiting = 0;
                    return 2;
                }
            }
            else
            {
                struct stat st;
                if (stat(r->deps[i], &st) != 0)
                {
                    fprintf(stderr,
                            "minimake: *** No rule to make target '%s'. Stop.\n",
                            r->deps[i]);
                    r->visiting = 0;
                    return 2;
                }
            }
        }
    }
    if (!r->phony && !newer_than(r->target, r->deps))
    {
        printf("Nothing to be done for '%s'\n", r->target);
        r->visiting = 0;
        r->built = 1;
        return 0;
    }
    if (r->cmds)
    {
        for (int i = 0; r->cmds[i]; i++)
        {
            char *expanded = expand_variables(r->cmds[i], vars, r);
            int ret = execute_command(expanded);
            free(expanded);
            if (ret)
            {
                r->visiting = 0;
                return 2;
            }
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
