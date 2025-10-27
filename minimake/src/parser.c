#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static char *rstrip(char *s)
{
    size_t n = strlen(s);
    while (n > 0)
    {
        char ch = s[n - 1];
        if (ch == ' ' || ch == '\t' || ch == '\r' || ch == '\n')
        {
            s[--n] = '\0';
        }
        else
        {
            break;
        }
    }
    return s;
}

static char *lstrip(char *s)
{
    size_t i = 0;
    while (s[i] == ' ' || s[i] == '\t')
    {
        i++;
    }
    return s + i;
}

static void norm_vars(FILE *out, const char *s)
{
    char name[256];
    for (size_t i = 0; s[i] != '\0';)
    {
        if (s[i] == '$' && s[i + 1] == '$')
        {
            fputc('$', out);
            i += 2;
            continue;
        }
        if (s[i] == '$' && s[i + 1] == '{')
        {
            size_t j = 0;
            i += 2;
            while (s[i] && s[i] != '}' && j + 1 < sizeof(name))
            {
                name[j++] = s[i++];
            }
            name[j] = '\0';
            if (s[i] == '}')
            {
                i++;
            }
            {
                const char *val = getenv(name);
                if (val)
                {
                    fputs(val, out);
                }
                else
                {
                    fprintf(out, "$(%s)", name);
                }
            }
            continue;
        }
        fputc(s[i++], out);
    }
}

static void strip_comment(char *line)
{
    for (int i = 0; line[i]; i++)
    {
        if (line[i] == '#')
        {
            line[i] = '\0';
            return;
        }
    }
}

static void no_tab_n_empty(FILE *out, const char *rhs)
{
    for (size_t i = 0; rhs[i] != '\0';)
    {
        while (rhs[i] == ' ' || rhs[i] == '\t')
        {
            i++;
        }
        if (!rhs[i])
        {
            break;
        }
        {
            size_t start = i;
            while (rhs[i] && rhs[i] != ' ' && rhs[i] != '\t')
            {
                i++;
            }
            fwrite(rhs + start, 1, i - start, out);
            fputc('\n', out);
        }
    }
}

static int assign_or_col(const char *line)
{
    for (int i = 0; line[i]; i++)
    {
        if (line[i] == '=' || line[i] == ':')
        {
            return i;
        }
    }
    return -1;
}

static void recipe(FILE *out, const char *line)
{
    norm_vars(out, line + 1);
    fputc('\n', out);
}

static void assignment(FILE *out, char *lhs, char *rhs)
{
    char *L = rstrip(lstrip(lhs));
    char *R = lstrip(rhs);
    norm_vars(out, L);
    fputs("=\n", out);
    norm_vars(out, R);
    fputc('\n', out);
}

static void target(FILE *out, char *lhs, char *rhs)
{
    char *L = rstrip(lstrip(lhs));
    char *R = lstrip(rhs);
    norm_vars(out, L);
    fputs(":\n", out);
    no_tab_n_empty(out, R);
}

static void process_line(FILE *out, char *raw, int *wrote_blank)
{
    rstrip(raw);
    strip_comment(raw);
    {
        char *line = lstrip(raw);
        if (!line[0])
        {
            return;
        }
        if (*wrote_blank)
        {
            fputc('\n', out);
        }
        *wrote_blank = 1;
        if (line[0] == '\t')
        {
            recipe(out, line);
            return;
        }
        {
            int index = assign_or_col(line);
            if (index >= 0)
            {
                char sym = line[index];
                line[index] = '\0';
                {
                    char *lhs = line;
                    char *rhs = line + index + 1;
                    if (sym == '=')
                    {
                        assignment(out, lhs, rhs);
                    }
                    else
                    {
                        target(out, lhs, rhs);
                    }
                }
                return;
            }
        }
        norm_vars(out, line);
        fputc('\n', out);
    }
}

int parse_file(const char *filename)
{
    FILE *in = fopen(filename, "r");
    if (!in)
    {
        fprintf(stderr, "Cannot open %s: %s\n", filename, strerror(errno));
        return 1;
    }
    FILE *out = fopen("rule.txt", "w");
    if (!out)
    {
        fprintf(stderr, "Cannot create rule.txt: %s\n", strerror(errno));
        fclose(in);
        return 1;
    }
    char buf[4096];
    int wrote_blank = 0;
    while (fgets(buf, sizeof(buf), in))
    {
        process_line(out, buf, &wrote_blank);
    }
    fputc('\n', out);
    fclose(in);
    fclose(out);
    return 0;
}
