#include <criterion/criterion.h>
#include <criterion/redirect.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "../src/simple_ls.c"

static void redirect_all_output(void)
{
    cr_redirect_stdout();
    cr_redirect_stderr();
}

static const char *get_stdout_str(void)
{
    static char buffer[8192];
    FILE *f = cr_get_redirected_stdout();
    fflush(f);
    fseek(f, 0, SEEK_SET);
    size_t n = fread(buffer, 1, sizeof(buffer) - 1, f);
    buffer[n] = '\0';
    return buffer;
}

static const char *get_stderr_str(void)
{
    static char buffer[8192];
    FILE *f = cr_get_redirected_stderr();
    fflush(f);
    fseek(f, 0, SEEK_SET);
    size_t n = fread(buffer, 1, sizeof(buffer) - 1, f);
    buffer[n] = '\0';
    return buffer;
}

static int contains(const char *haystack, const char *needle)
{
    return haystack && strstr(haystack, needle);
}

/* ---------- TESTS ---------- */

Test(simple_ls, list_current_dir, .init = redirect_all_output)
{
    const char *tmpdir = "/tmp/test_simple_ls_current";
    mkdir(tmpdir, 0700);

    const char *filepath = "/tmp/test_simple_ls_current/file.txt";
    int fd = open(filepath, O_CREAT | O_WRONLY, 0600);
    cr_assert_neq(fd, -1, "Failed to create test file");
    write(fd, "ok", 2);
    close(fd);

    cr_assert_eq(access(filepath, F_OK), 0, "File was not created");

    DIR *check = opendir(tmpdir);
    cr_assert_not_null(check);
    closedir(check);

    int ret = list_dir(tmpdir);
    cr_assert_eq(ret, 0, "Expected return 0, got %d", ret);

    const char *out = get_stdout_str();
    cr_assert(contains(out, "file.txt"),
              "Expected 'file.txt' to appear in output, got:\n%s", out);

    unlink(filepath);
    rmdir(tmpdir);
}

Test(simple_ls, non_existent_dir, .init = redirect_all_output)
{
    int ret = list_dir("/path/that/does/not/exist");
    cr_assert_eq(ret, 2, "Expected error code 2");

    const char *err = get_stderr_str();
    cr_assert(contains(err, "cannot open directory"),
              "Expected error message in stderr");
}

Test(simple_ls, empty_dir_stream, .init = redirect_all_output)
{
    const char *tmpdir = "/tmp/test_simple_ls_empty";
    mkdir(tmpdir, 0700);

    DIR *dir = opendir(tmpdir);
    cr_assert_not_null(dir, "Could not open tmpdir: %s", strerror(errno));

    int ret = list_dir_stream(dir);
    cr_assert_eq(ret, 0);

    const char *out = get_stdout_str();
    cr_assert(!out || *out == '\0', "Empty directory should produce no output");

    closedir(dir);
    rmdir(tmpdir);
}

Test(simple_ls, skip_hidden_files, .init = redirect_all_output)
{
    const char *tmpdir = "/tmp/test_simple_ls_hidden";
    mkdir(tmpdir, 0700);

    const char *hidden = "/tmp/test_simple_ls_hidden/.hidden";
    const char *visible = "/tmp/test_simple_ls_hidden/visible";

    int fd1 = open(hidden, O_CREAT | O_WRONLY, 0600);
    int fd2 = open(visible, O_CREAT | O_WRONLY, 0600);
    cr_assert_neq(fd1, -1);
    cr_assert_neq(fd2, -1);
    write(fd1, "a", 1);
    write(fd2, "b", 1);
    close(fd1);
    close(fd2);

    cr_assert_eq(access(hidden, F_OK), 0, "Hidden file missing");
    cr_assert_eq(access(visible, F_OK), 0, "Visible file missing");

    DIR *check = opendir(tmpdir);
    cr_assert_not_null(check);
    closedir(check);

    int ret = list_dir(tmpdir);
    cr_assert_eq(ret, 0);

    const char *out = get_stdout_str();
    cr_assert(contains(out, "visible"),
              "Expected visible file to appear, got:\n%s", out);
    cr_assert(!contains(out, ".hidden"),
              "Hidden file should not appear, got:\n%s", out);

    unlink(visible);
    unlink(hidden);
    rmdir(tmpdir);
}
