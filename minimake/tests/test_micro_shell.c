#include <criterion/criterion.h>
#include <criterion/redirect.h>
#include <unistd.h>
#include "../src/micro_shell.h"

static void redirect_all(void)
{
    cr_redirect_stdout();
    cr_redirect_stderr();
}

Test(micro_shell, simple_command, .init = redirect_all)
{
    int ret = micro_shell("echo hello");
    cr_assert_eq(ret, 0, "Expected return 0, got %d", ret);
    fflush(stdout);
    fflush(stderr);
    FILE *f = cr_get_redirected_stdout();
    fseek(f, 0, SEEK_SET);
    char buf[64];
    fgets(buf, sizeof(buf), f);
    cr_assert(strstr(buf, "hello") != NULL, "Expected output to contain 'hello'");
}

Test(micro_shell, failing_command, .init = redirect_all)
{
    int ret = micro_shell("false");
    cr_assert_neq(ret, 0, "Expected non-zero return on failure");
}

Test(micro_shell, invalid_command, .init = redirect_all)
{
    int ret = micro_shell("nonexistent_command_zzz");
    cr_assert_neq(ret, 0, "Expected failure for nonexistent command");
}

