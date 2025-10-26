#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int list_dir_stream(DIR *dir)
{
    struct dirent *entry;

    while ((entry = readdir(dir)) != NULL)
    {
        if (entry->d_name[0] == '.')
        {
            continue;
        }
        printf("%s\n", entry->d_name);
        fflush(stdout);
    }
    return 0;
}

int list_dir(const char *dirname)
{
    DIR *dir = opendir(dirname);

    if (!dir)
    {
        fprintf(stderr, "simple_ls: cannot open directory '%s': %s\n", dirname,
                strerror(errno));
        return 2;
    }

    if (list_dir_stream(dir) != 0)
    {
        closedir(dir);
        return 2;
    }

    if (closedir(dir) != 0)
    {
        fprintf(stderr, "simple_ls: failed to close directory '%s': %s\n",
                dirname, strerror(errno));
        return 2;
    }
    return 0;
}
