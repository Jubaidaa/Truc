#include <assert.h>
#include <err.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char **argv)
{
    pid_t pid;
    int wstatus;

    if (argc != 2)
    {
        fprintf(stderr, "faut faire: %s 'command'\n", argv[0]);
        return 1;
    }

    pid = fork();
    if (pid < 0)
    {
        err(1, "fork");
    }

    if (pid == 0)
    {
        execl("/bin/sh", "micro_shell", "-c", argv[1], NULL);
        err(1, "execl");
    }

    if (waitpid(pid, &wstatus, 0) < 0)
    {
        err(1, "waitpid");
    }

    if (WIFEXITED(wstatus))
    {
        printf("process exit status: %d\n", WEXITSTATUS(wstatus));
    }
    else
    {
        fprintf(stderr, "process a fait un truc zarbi\n");
        return 1;
    }

    return 0;
}
