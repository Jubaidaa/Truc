#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "builder.h"
#include "aux_builder.h"
#include "parser.h"

int main(int argc, char **argv)
{
    const char *makefile = NULL;
    const char *target = NULL;
    int print_only = 0;
    int help = 0;

    if (argc < 2)
    {
        fprintf(stderr, "Usage: %s [-f file] [-p] [-h] [target]\n", argv[0]);
        return 1;
    }

    for (int i = 1; i < argc; i++)
    {
        if (argv[i][0] == '\0')
        {
            fprintf(stderr, "minimake: *** empty string invalid as argument.  Stop.\n");
            return 2;
        }
    }

    for (int i = 1; i < argc; i++)
    {
        if (!strcmp(argv[i], "-f"))
        {
            if (i + 1 < argc)
            {
                makefile = argv[++i];
            }
            else
            {
                fprintf(stderr, "Usage: %s [-f file] [-p] [-h] [target]\n", argv[0]);
                return 1;
            }
        }
        else if (!strcmp(argv[i], "-p"))
        {
            print_only = 1;
        }
        else if (!strcmp(argv[i], "-h"))
        {
            help = 1;
        }
        else
        {
            target = argv[i];
        }
    }

    if (help)
    {
        printf("Usage: %s [-f file] [-p] [-h] [target]\n", argv[0]);
        printf("  -f <file> : specify a Makefile (default: Makefile or makefile)\n");
        printf("  -p        : print rules and variables after parsing\n");
        printf("  -h        : show this help message\n");
        return 0;
    }

    if (!makefile)
    {
        if (access("Makefile", F_OK) == 0)
        {
            makefile = "Makefile";
        }
        else if (access("makefile", F_OK) == 0)
        {
            makefile = "makefile";
        }
        else
        {
            fprintf(stderr, "Cannot open Makefile: No such file or directory\n");
            return 2;
        }
    }

    struct variable *vars = NULL;
    struct rule *rules = NULL;

    if (parse_file(makefile))
    {
        if (access(makefile, F_OK) != 0)
        {
            return 2;
        }
        fprintf(stderr, "Parser failed on %s\n", makefile);
        return 1;
    }

    if (parse_rule_file("rule.txt", &vars, &rules))
    {
        fprintf(stderr, "Failed to read rule.txt\n");
        free_variables(vars);
        free_rules(rules);
        return 1;
    }

    if (print_only)
    {
        struct variable *v = vars;
        struct rule *r = rules;
        printf("Variables:\n");
        while (v)
        {
            printf("  %s = %s\n", v->name, v->value);
            v = v->next;
        }
        printf("\nRules:\n");
        while (r)
        {
            printf("  %s:\n", r->target);
            if (r->deps)
            {
                for (size_t i = 0; r->deps[i]; i++)
                {
                    printf("    dep: %s\n", r->deps[i]);
                }
            }
            if (r->cmds)
            {
                for (size_t j = 0; r->cmds[j]; j++)
                {
                    printf("    cmd: %s\n", r->cmds[j]);
                }
            }
            printf("\n");
            r = r->next;
        }
        free_variables(vars);
        free_rules(rules);
        return 0;
    }

    if (!target)
    {
        target = "all";
    }

    int ret = build_rule(rules, vars, target);
    free_variables(vars);
    free_rules(rules);
    return ret;
}
