#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int micro_shell(const char *cmd)
{
    int status;
    fflush(stdout);
    fflush(stderr);
    pid_t pid = fork();
    if (pid == 0)
    {
        execl("/bin/sh", "supershell", "-c", cmd, NULL);
        fprintf(stderr, "exec failed: %s\n", strerror(errno));
        _exit(1);
    }
    if (pid < 0)
    {
        fprintf(stderr, "fork failed: %s\n", strerror(errno));
        return 1;
    }
    waitpid(pid, &status, 0);
    fflush(stdout);
    fflush(stderr);
    if (WIFEXITED(status))
    {
        return WEXITSTATUS(status);
    }
    return 1;
}
