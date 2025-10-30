#include <criterion/criterion.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../src/builder.h"

Test(aux_builder, test_parse_simple_variable)
{
    FILE *f = fopen("test_rule.txt", "w");
    cr_assert_not_null(f, "Failed to create test file");

    fprintf(f, "CC\n=\ngcc\n\n");
    fclose(f);

    struct variable *vars = NULL;
    struct rule *rules = NULL;

    int ret = parse_rule_file("test_rule.txt", &vars, &rules);
    cr_assert_eq(ret, 0, "Expected successful parse");
    cr_assert_not_null(vars, "Expected variables to be parsed");
    cr_assert_str_eq(vars->name, "CC", "Expected variable name CC");
    cr_assert_str_eq(vars->value, "gcc", "Expected variable value gcc");

    free_variables(vars);
    free_rules(rules);
    remove("test_rule.txt");
}

Test(aux_builder, test_parse_simple_rule)
{
    FILE *f = fopen("test_rule.txt", "w");
    cr_assert_not_null(f, "Failed to create test file");

    fprintf(f, "all\n:\nprogram\n\n");
    fclose(f);

    struct variable *vars = NULL;
    struct rule *rules = NULL;

    int ret = parse_rule_file("test_rule.txt", &vars, &rules);
    cr_assert_eq(ret, 0, "Expected successful parse");
    cr_assert_not_null(rules, "Expected rules to be parsed");
    cr_assert_str_eq(rules->target, "all", "Expected target all");
    cr_assert_not_null(rules->deps, "Expected dependencies");
    cr_assert_str_eq(rules->deps[0], "program", "Expected dep program");

    free_variables(vars);
    free_rules(rules);
    remove("test_rule.txt");
}

Test(aux_builder, test_parse_rule_with_command)
{
    FILE *f = fopen("test_rule.txt", "w");
    cr_assert_not_null(f, "Failed to create test file");

    fprintf(f, "program\n:\nfile.c\n\ngcc file.c -o program\n\n");
    fclose(f);

    struct variable *vars = NULL;
    struct rule *rules = NULL;

    int ret = parse_rule_file("test_rule.txt", &vars, &rules);
    cr_assert_eq(ret, 0, "Expected successful parse");
    cr_assert_not_null(rules, "Expected rules to be parsed");
    cr_assert_str_eq(rules->target, "program", "Expected target program");
    cr_assert_not_null(rules->cmds, "Expected commands");
    cr_assert_str_eq(rules->cmds[0], "gcc file.c -o program",
                     "Expected gcc command");

    free_variables(vars);
    free_rules(rules);
    remove("test_rule.txt");
}

Test(aux_builder, test_parse_phony_target)
{
    FILE *f = fopen("test_rule.txt", "w");
    cr_assert_not_null(f, "Failed to create test file");

    fprintf(f, ".PHONY\n:\nclean\n\n");
    fclose(f);

    struct variable *vars = NULL;
    struct rule *rules = NULL;

    int ret = parse_rule_file("test_rule.txt", &vars, &rules);
    cr_assert_eq(ret, 0, "Expected successful parse");
    cr_assert_not_null(rules, "Expected rules to be parsed");
    cr_assert_eq(rules->phony, 1, "Expected phony flag to be set");

    free_variables(vars);
    free_rules(rules);
    remove("test_rule.txt");
}

Test(aux_builder, test_parse_multiple_variables)
{
    FILE *f = fopen("test_rule.txt", "w");
    cr_assert_not_null(f, "Failed to create test file");

    fprintf(f, "CC\n=\ngcc\n\nCFLAGS\n=\n-Wall\n\n");
    fclose(f);

    struct variable *vars = NULL;
    struct rule *rules = NULL;

    int ret = parse_rule_file("test_rule.txt", &vars, &rules);
    cr_assert_eq(ret, 0, "Expected successful parse");
    cr_assert_not_null(vars, "Expected variables to be parsed");
    cr_assert_str_eq(vars->name, "CC", "Expected first var CC");
    cr_assert_not_null(vars->next, "Expected second variable");
    cr_assert_str_eq(vars->next->name, "CFLAGS", "Expected second var CFLAGS");

    free_variables(vars);
    free_rules(rules);
    remove("test_rule.txt");
}
