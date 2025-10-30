#include <criterion/criterion.h>
#include <criterion/redirect.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "../src/parser.h"

static void redirect_all(void)
{
    cr_redirect_stdout();
    cr_redirect_stderr();
}

static void write_file(const char *path, const char *content)
{
    FILE *f = fopen(path, "w");
    fputs(content, f);
    fclose(f);
}

Test(parser, simple_rule_generation, .init = redirect_all)
{
    const char *mk = "test.mk";
    const char *content = "CC = gcc\nall: main.o\n\t$(CC) -o app main.o\n";
    write_file(mk, content);
    int ret = parse_file(mk);
    cr_assert_eq(ret, 0, "Expected parse_file success");
    FILE *f = fopen("rule.txt", "r");
    cr_assert_not_null(f, "rule.txt not generated");
    char buf[256];
    fread(buf, 1, sizeof(buf) - 1, f);
    fclose(f);
    cr_assert(strstr(buf, "all:") != NULL, "Expected target 'all:' in rule.txt");
    cr_assert(strstr(buf, "$(CC)") != NULL, "Expected variable expansion placeholder");
    unlink(mk);
    unlink("rule.txt");
}

Test(parser, handle_comments_and_spaces, .init = redirect_all)
{
    const char *mk = "test2.mk";
    const char *content = "VAR =  value   # comment\n\nall: $(VAR)\n";
    write_file(mk, content);
    int ret = parse_file(mk);
    cr_assert_eq(ret, 0, "Expected parse success");
    FILE *f = fopen("rule.txt", "r");
    cr_assert_not_null(f, "rule.txt not found");
    char buf[256];
    fread(buf, 1, sizeof(buf) - 1, f);
    fclose(f);
    cr_assert(strstr(buf, "VAR") != NULL, "Expected variable VAR in output");
    unlink(mk);
    unlink("rule.txt");
}

