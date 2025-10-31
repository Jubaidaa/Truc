#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "builder.h"
#include "parser.h"

static void print_help(void)
{
    printf("Usage: minimake [TARGETS...] [-f FILE] [-p] [-h]\n");
    printf("Options:\n");
    printf("  -f FILE   (default: Makefile or makefile)\n");
    printf("  -p        Print the file\n");
    printf("  -h        Display help message\n");
}

static const char *detect_makefile(void)
{
    if (access("Makefile", F_OK) == 0)
        return "Makefile";
    if (access("makefile", F_OK) == 0)
        return "makefile";
    fprintf(stderr, "Cannot open Makefile: No such file or directory\n");
    exit(2);
}

static void print_variable(struct variable *v)
{
    printf("%s=\n%s\n", v->name, v->value);
}

static void print_rule(struct rule *r)
{
    printf("%s:\n", r->target);
    for (size_t i = 0; r->deps && r->deps[i]; i++)
        printf("%s\n", r->deps[i]);
    if (r->deps || r->cmds)
        printf("\n");
    for (size_t i = 0; r->cmds && r->cmds[i]; i++)
        printf("\t%s\n", r->cmds[i]);
}

static void pretty_print(struct variable *vars, struct rule *rules)
{
    // Print variables first
    for (struct variable *v = vars; v; v = v->next)
    {
        print_variable(v);
        if (v->next || rules)
            printf("\n");
    }
    
    // Print rules
    for (struct rule *r = rules; r; r = r->next)
    {
        print_rule(r);
        if (r->next)
            printf("\n");
    }
    printf("\n");
}

struct build_ctx
{
    struct rule *rules;
    struct variable *vars;
    char **argv;
    int start;
    int argc;
};

static int build_targets(struct build_ctx *ctx)
{
    int ret = 0;
    int any_rebuilt = 0;
    if (ctx->start == -1)
    {
        if (ctx->rules)
            return build_rule(ctx->rules, ctx->vars, ctx->rules->target);
        fprintf(stderr, "No default target found.\n");
        return 1;
    }
    for (int i = ctx->start; i < ctx->argc; i++)
    {
        if (ctx->argv[i][0] == '-')
            continue;
        ret = build_rule(ctx->rules, ctx->vars, ctx->argv[i]);
        if (ret == 0)
            any_rebuilt = 1;
        if (ret != 0 && ret != 1)
            break;
    }
    if (ret != 0 && ret != 1)
        return ret;
    return any_rebuilt ? 0 : 1;
}

int help_print(int n, const char *makefile)
{
    fprintf(stderr, "Cannot open %s: %s\n", makefile, strerror(errno));
    return n;
}

int main(int argc, char **argv)
{
    const char *makefile = NULL;
    int parse_only = 0;
    int first_target = -1;

    for (int i = 1; i < argc; i++)
    {
        if (!strcmp(argv[i], "-h"))
        {
            print_help();
            return 0;
        }
        if (!strcmp(argv[i], "-p"))
        {
            parse_only = 1;
            continue;
        }
        if (!strcmp(argv[i], "-f"))
        {
            if (i + 1 >= argc)
            {
                return 1;
            }
            makefile = argv[++i];
            continue;
        }
        if (first_target == -1)
            first_target = i;
    }

    if (!makefile)
        makefile = detect_makefile();

    if (access(makefile, F_OK) != 0)
    {
        return help_print(2, makefile);
    }

    struct variable *vars = NULL;
    struct rule *rules = NULL;

    if (parse_file(makefile, &vars, &rules))
    {
        free_variables(vars);
        free_rules(rules);
        if (access(makefile, F_OK) != 0)
            return 2;
        return 1;
    }

    if (parse_only)
    {
        pretty_print(vars, rules);
        free_variables(vars);
        free_rules(rules);
        return 0;
    }

    struct build_ctx ctx = { rules, vars, argv, first_target, argc };
    int ret = build_targets(&ctx);

    if (rules && !rules->visiting)
    {
        free_variables(vars);
        free_rules(rules);
    }

    return ret;
}
