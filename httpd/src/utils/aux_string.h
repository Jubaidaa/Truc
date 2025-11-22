#ifndef AUX_STRING_H
#define AUX_STRING_H

#include <stddef.h>

#include "string/string.h"

struct string *string_create_empty(size_t initial_capacity);
int string_compare_str(const struct string *str1, const char *str2);
void string_clear(struct string *str);
struct string *string_duplicate(const struct string *src);

#endif // ! AUX_STRING_H
