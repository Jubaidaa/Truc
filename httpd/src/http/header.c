#include "../utils/aux_string.h"
#include "http.h"

#include <ctype.h>
#include <stdlib.h>
#include <string.h>

struct http_header *http_header_create(const char *name, const char *value) {
  if (!name || !value) {
    return NULL;
  }

  struct http_header *header = calloc(1, sizeof(struct http_header));
  if (!header) {
    return NULL;
  }

  header->name = string_create(name, strlen(name));
  header->value = string_create(value, strlen(value));

  if (!header->name || !header->value) {
    http_header_destroy(header);
    return NULL;
  }

  return header;
}

void http_header_destroy(struct http_header *header) {
  while (header) {
    struct http_header *next = header->next;
    string_destroy(header->name);
    string_destroy(header->value);
    free(header);
    header = next;
  }
}

void http_header_add(struct http_header **list, struct http_header *header) {
  if (!list || !header) {
    return;
  }

  if (!*list) {
    *list = header;
    return;
  }

  struct http_header *current = *list;
  while (current->next) {
    current = current->next;
  }
  current->next = header;
}

static int string_compare_case_insensitive(const char *s1, const char *s2) {
  while (*s1 && *s2) {
    unsigned char uc1 = *s1;
    unsigned char uc2 = *s2;
    int c1 = tolower(uc1);
    int c2 = tolower(uc2);

    if (c1 != c2) {
      return c1 - c2;
    }

    s1++;
    s2++;
  }

  unsigned char uc1 = *s1;
  unsigned char uc2 = *s2;
  return tolower(uc1) - tolower(uc2);
}

struct http_header *http_header_find(struct http_header *list,
                                     const char *name) {
  while (list) {
    if (string_compare_case_insensitive(list->name->data, name) == 0) {
      return list;
    }
    list = list->next;
  }
  return NULL;
}
