#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "builder.h"

int parse_file(const char *filename);

static void print_help(void)
{
    printf("Usage: minimake [TARGETS...] [-f FILE] [-p] [-h]\n");
    printf("Options:\n");
    printf("  -f FILE   Use FILE as makefile (default: Makefile or makefile)\n");
    printf("  -p        Print the file\n");
    printf("  -h        Display this help message\n");
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

static void pretty_print(void)
{
    FILE *f = fopen("rule.txt", "r");
    if (!f)
    {
        fprintf(stderr, "cannot open rule.txt\n");
        exit(1);
    }
    char buf[4096];
    while (fgets(buf, sizeof(buf), f))
        fputs(buf, stdout);
    fclose(f);
}

static int build_targets(struct rule *rules, struct variable *vars,
                         char **argv, int start, int argc)
{
    int ret = 0;
    if (start == -1)
    {
        if (rules)
            return build_rule(rules, vars, rules->target);
        fprintf(stderr, "No default target found.\n");
        return 1;
    }
    for (int i = start; i < argc; i++)
    {
        if (argv[i][0] == '-')
            continue;
        ret = build_rule(rules, vars, argv[i]);
        if (ret)
            break;
    }
    return ret;
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
                fprintf(stderr, "-f requires a file\n");
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
        fprintf(stderr, "Cannot open %s: %s\n", makefile, strerror(errno));
        return 2;
    }

    if (parse_file(makefile))
    {
        if (access(makefile, F_OK) != 0)
            return 2;
        return 1;
    }

    if (parse_only)
    {
        pretty_print();
        return 0;
    }

    struct variable *vars = NULL;
    struct rule *rules = NULL;

    if (parse_rule_file("rule.txt", &vars, &rules))
    {
        fprintf(stderr, "cannot parse rule.txt\n");
        free_variables(vars);
        free_rules(rules);
        return 1;
    }

    int ret = build_targets(rules, vars, argv, first_target, argc);

    if (rules && !rules->visiting)
    {
        free_variables(vars);
        free_rules(rules);
    }

    return ret;
}
