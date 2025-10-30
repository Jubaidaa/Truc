#include <criterion/criterion.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../src/builder.h"

Test(builder, test_get_variable)
{
    struct variable v1 = {.name = "CC", .value = "gcc", .next = NULL};
    struct variable v2 = {.name = "CFLAGS", .value = "-Wall", .next = &v1};

    const char *cc = get_variable(&v2, "CC");
    cr_assert_str_eq(cc, "gcc", "Expected CC to be gcc");

    const char *cflags = get_variable(&v2, "CFLAGS");
    cr_assert_str_eq(cflags, "-Wall", "Expected CFLAGS to be -Wall");

    const char *unknown = get_variable(&v2, "UNKNOWN");
    cr_assert_str_eq(unknown, "", "Expected empty string for unknown var");
}

Test(builder, test_expand_variables_simple)
{
    struct variable v = {.name = "CC", .value = "gcc", .next = NULL};

    char *result = expand_variables("$(CC) -c file.c", &v, NULL);
    cr_assert_str_eq(result, "gcc -c file.c", "Expected variable expansion");
    free(result);
}

Test(builder, test_expand_variables_automatic)
{
    char *deps[] = {"input.c", NULL};
    struct rule r = {
        .target = "output.o",
        .deps = deps,
        .cmds = NULL,
        .phony = 0,
        .visiting = 0,
        .built = 0,
        .next = NULL
    };

    char *result1 = expand_variables("Building $@", NULL, &r);
    cr_assert_str_eq(result1, "Building output.o", "Expected $@ expansion");
    free(result1);

    char *result2 = expand_variables("From $<", NULL, &r);
    cr_assert_str_eq(result2, "From input.c", "Expected $< expansion");
    free(result2);
}

Test(builder, test_expand_variables_all_deps)
{
    char *deps[] = {"file1.o", "file2.o", "file3.o", NULL};
    struct rule r = {
        .target = "program",
        .deps = deps,
        .cmds = NULL,
        .phony = 0,
        .visiting = 0,
        .built = 0,
        .next = NULL
    };

    char *result = expand_variables("gcc $^ -o $@", NULL, &r);
    cr_assert_str_eq(result, "gcc file1.o file2.o file3.o -o program",
                     "Expected $^ expansion");
    free(result);
}

Test(builder, test_find_rule)
{
    struct rule r1 = {.target = "all", .deps = NULL, .cmds = NULL,
                      .phony = 0, .visiting = 0, .built = 0, .next = NULL};
    struct rule r2 = {.target = "clean", .deps = NULL, .cmds = NULL,
                      .phony = 1, .visiting = 0, .built = 0, .next = &r1};

    struct rule *found = find_rule(&r2, "clean");
    cr_assert_not_null(found, "Expected to find clean rule");
    cr_assert_str_eq(found->target, "clean", "Expected clean target");

    found = find_rule(&r2, "all");
    cr_assert_not_null(found, "Expected to find all rule");
    cr_assert_str_eq(found->target, "all", "Expected all target");

    found = find_rule(&r2, "nonexistent");
    cr_assert_null(found, "Expected NULL for nonexistent rule");
}
