#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include "builder.h"
#include "micro_shell.h"

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

static int check_in_directory(const char *dirname, const char *filename)
{
    DIR *dir = opendir(dirname);
    if (!dir)
    {
        return 0;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL)
    {
        if (entry->d_name[0] == '.')
        {
            continue;
        }
        if (!strcmp(entry->d_name, filename))
        {
            closedir(dir);
            return 1;
        }
    }
    closedir(dir);
    return 0;
}

static int check_with_path(const char *path)
{
    char *slash = strrchr(path, '/');
    if (!slash)
    {
        return 0;
    }

    size_t dirlen = slash - path;
    char dirname[1024];
    if (dirlen >= sizeof(dirname))
    {
        return 0;
    }
    memcpy(dirname, path, dirlen);
    dirname[dirlen] = '\0';
    const char *filename = slash + 1;

    return check_in_directory(dirname, filename);
}

static int file_exists(const char *path)
{
    struct stat st;
    if (stat(path, &st) == 0)
    {
        return 1;
    }

    if (check_with_path(path))
    {
        return 1;
    }

    return check_in_directory(".", path);
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

static char *join_with_space(char **items)
{
    if (!items)
    {
        return xstrdup("");
    }

    size_t total = 0;
    size_t count = 0;
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

static void handle_auto_var(const char *in, size_t *i, char **out,
                            size_t *cap, const struct rule *ctx)
{
    const char *val = "";
    if (in[*i + 1] == '@')
    {
        val = ctx ? ctx->target : "";
    }
    else if (in[*i + 1] == '<')
    {
        val = (ctx && ctx->deps && ctx->deps[0]) ? ctx->deps[0] : "";
    }
    else if (in[*i + 1] == '^')
    {
        char *joined = ctx ? join_with_space(ctx->deps) : xstrdup("");
        size_t need = strlen(*out) + strlen(joined) + 1;
        if (need > *cap)
        {
            *cap = need + 64;
            *out = realloc(*out, *cap);
        }
        strcat(*out, joined);
        free(joined);
        *i += 2;
        return;
    }
    size_t need = strlen(*out) + strlen(val) + 1;
    if (need > *cap)
    {
        *cap = need + 64;
        *out = realloc(*out, *cap);
    }
    strcat(*out, val);
    *i += 2;
}

static void handle_var_ref(const char *in, size_t *i, char **out,
                           size_t *cap, const struct variable *vars)
{
    size_t j = *i + 2;
    char name[256];
    size_t k = 0;
    while (in[j] && in[j] != ')' && k + 1 < sizeof(name))
    {
        name[k++] = in[j++];
    }
    name[k] = '\0';
    if (in[j] == ')')
    {
        const char *val = get_variable(vars, name);
        size_t need = strlen(*out) + strlen(val) + 1;
        if (need > *cap)
        {
            *cap = need + 64;
            *out = realloc(*out, *cap);
        }
        strcat(*out, val);
        *i = j + 1;
    }
}

char *expand_variables(const char *in, const struct variable *vars,
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
            if (in[i + 1] == '@' || in[i + 1] == '<' || in[i + 1] == '^')
            {
                handle_auto_var(in, &i, &out, &cap, ctx);
                continue;
            }
            if (in[i + 1] == '(')
            {
                handle_var_ref(in, &i, &out, &cap, vars);
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

static int execute_command(const char *cmd)
{
    const char *run = cmd;
    if (cmd[0] == '@')
    {
        run = cmd + 1;
        while (*run == ' ' || *run == '\t')
        {
            run++;
        }
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

static int is_variable_reference(const char *str)
{
    if (!str || str[0] != '$')
    {
        return 0;
    }
    if (str[1] == '(')
    {
        return 1;
    }
    return 0;
}

static int check_dependencies(struct rule *rules, struct variable *vars,
                              struct rule *r)
{
    if (!r->deps)
    {
        return 0;
    }

    for (size_t i = 0; r->deps[i]; i++)
    {
        char *expanded = expand_variables(r->deps[i], vars, r);
        
        if (is_variable_reference(r->deps[i]))
        {
            char *token = strtok(expanded, " \t\n");
            while (token)
            {
                struct rule *dr = find_rule(rules, token);
                if (dr)
                {
                    int ret = build_rule_inner(rules, vars, dr);
                    if (ret)
                    {
                        free(expanded);
                        return ret;
                    }
                }
                else if (!file_exists(token))
                {
                    fprintf(stderr,
                            "minimake: *** No rule to make target '%s'. Stop.\n",
                            token);
                    free(expanded);
                    return 2;
                }
                token = strtok(NULL, " \t\n");
            }
        }
        else
        {
            struct rule *dr = find_rule(rules, expanded);
            if (dr)
            {
                int ret = build_rule_inner(rules, vars, dr);
                free(expanded);
                if (ret)
                {
                    return ret;
                }
            }
            else
            {
                if (!file_exists(expanded))
                {
                    fprintf(stderr,
                            "minimake: *** No rule to make target '%s'. Stop.\n",
                            expanded);
                    free(expanded);
                    return 2;
                }
                free(expanded);
            }
        }
        
        if (is_variable_reference(r->deps[i]))
        {
            free(expanded);
        }
        else
        {
            free(expanded);
        }
    }
    return 0;
}

static int execute_commands(struct rule *r, struct variable *vars)
{
    if (!r->cmds)
    {
        return 0;
    }

    for (size_t i = 0; r->cmds[i]; i++)
    {
        char *expanded = expand_variables(r->cmds[i], vars, r);
        int ret = execute_command(expanded);
        free(expanded);
        if (ret)
        {
            return ret;
        }
    }
    return 0;
}

int build_rule_inner(struct rule *rules, struct variable *vars,
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

    int ret = check_dependencies(rules, vars, r);
    if (ret)
    {
        r->visiting = 0;
        return ret;
    }

    if (!r->phony && !newer_than(r->target, r->deps))
    {
        printf("Nothing to be done for '%s'\n", r->target);
        r->visiting = 0;
        r->built = 1;
        return 0;
    }

    ret = execute_commands(r, vars);
    r->visiting = 0;
    if (ret)
    {
        return ret;
    }

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
