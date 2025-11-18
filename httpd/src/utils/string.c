#include "string/string.h"

#include <stdlib.h>
#include <string.h>

#include "aux_string.h"

struct string *string_create(const char *str, size_t size)
{
    struct string *new_str = malloc(sizeof(struct string));
    if (!new_str)
    {
        return NULL;
    }

    new_str->size = size;
    new_str->data = malloc(size + 1);
    if (!new_str->data)
    {
        free(new_str);
        return NULL;
    }

    if (str)
    {
        memcpy(new_str->data, str, size);
    }
    new_str->data[size] = '\0';

    return new_str;
}

struct string *string_create_empty(size_t initial_capacity)
{
    struct string *new_str = malloc(sizeof(struct string));
    if (!new_str)
    {
        return NULL;
    }

    new_str->size = 0;
    new_str->data = malloc(initial_capacity + 1);
    if (!new_str->data)
    {
        free(new_str);
        return NULL;
    }
    new_str->data[0] = '\0';

    return new_str;
}

int string_compare_n_str(const struct string *str1, const char *str2, size_t n)
{
    if (!str1 || !str1->data || !str2)
    {
        return -1;
    }

    size_t cmp_size = str1->size < n ? str1->size : n;
    int result = memcmp(str1->data, str2, cmp_size);
    
    if (result != 0)
    {
        return result;
    }
    
    if (str1->size < n)
    {
        return -1;
    }
    else if (str1->size > n && n < strlen(str2))
    {
        return 1;
    }
    
    return 0;
}

int string_compare_str(const struct string *str1, const char *str2)
{
    if (!str1 || !str1->data || !str2)
    {
        return -1;
    }
    
    size_t str2_len = strlen(str2);
    if (str1->size != str2_len)
    {
        return str1->size < str2_len ? -1 : 1;
    }
    
    return memcmp(str1->data, str2, str1->size);
}

void string_concat_str(struct string *str, const char *to_concat, size_t size)
{
    if (!str || !to_concat || size == 0)
    {
        return;
    }

    size_t new_size = str->size + size;
    char *new_data = realloc(str->data, new_size + 1);
    if (!new_data)
    {
        return;
    }

    memcpy(new_data + str->size, to_concat, size);
    new_data[new_size] = '\0';
    
    str->data = new_data;
    str->size = new_size;
}

void string_clear(struct string *str)
{
    if (str && str->data)
    {
        str->data[0] = '\0';
        str->size = 0;
    }
}

struct string *string_duplicate(const struct string *src)
{
    if (!src)
    {
        return NULL;
    }
    
    return string_create(src->data, src->size);
}

void string_destroy(struct string *str)
{
    if (str)
    {
        free(str->data);
        free(str);
    }
}

