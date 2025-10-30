#include "utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *xstrdup(const char *s)
{
    if (!s)
    {
        return NULL;
    }
    size_t n = strlen(s) + 1;
    char *p = malloc(n);
    if (!p)
    {
        fprintf(stderr, "malloc failed\n");
        exit(1);
    }
    memcpy(p, s, n);
    return p;
}
