#include "http.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../utils/aux_string.h"

static int string_compare_case_insensitive(const char *s1, const char *s2)
{
    while (*s1 && *s2)
    {
        unsigned char uc1 = *s1;
        unsigned char uc2 = *s2;
        int c1 = tolower(uc1);
        int c2 = tolower(uc2);

        if (c1 != c2)
        {
            return c1 - c2;
        }

        s1++;
        s2++;
    }

    unsigned char uc1 = *s1;
    unsigned char uc2 = *s2;
    return tolower(uc1) - tolower(uc2);
}

static const char *find_crlf(const char *data, size_t size)
{
    for (size_t i = 0; i < size - 1; i++)
    {
        if (data[i] == '\r' && data[i + 1] == '\n')
        {
            return &data[i];
        }
    }
    return NULL;
}

enum http_method http_method_from_string(const char *method)
{
    if (!method)
    {
        return HTTP_METHOD_UNKNOWN;
    }

    if (strcmp(method, "GET") == 0)
    {
        return HTTP_METHOD_GET;
    }
    else if (strcmp(method, "HEAD") == 0)
    {
        return HTTP_METHOD_HEAD;
    }
    else if (strcmp(method, "POST") == 0)
    {
        return HTTP_METHOD_POST;
    }
    else if (strcmp(method, "PUT") == 0)
    {
        return HTTP_METHOD_PUT;
    }
    else if (strcmp(method, "DELETE") == 0)
    {
        return HTTP_METHOD_DELETE;
    }
    else if (strcmp(method, "CONNECT") == 0)
    {
        return HTTP_METHOD_CONNECT;
    }
    else if (strcmp(method, "OPTIONS") == 0)
    {
        return HTTP_METHOD_OPTIONS;
    }
    else if (strcmp(method, "TRACE") == 0)
    {
        return HTTP_METHOD_TRACE;
    }

    return HTTP_METHOD_UNKNOWN;
}

const char *http_method_to_string(enum http_method method)
{
    switch (method)
    {
    case HTTP_METHOD_GET:
        return "GET";
    case HTTP_METHOD_HEAD:
        return "HEAD";
    case HTTP_METHOD_POST:
        return "POST";
    case HTTP_METHOD_PUT:
        return "PUT";
    case HTTP_METHOD_DELETE:
        return "DELETE";
    case HTTP_METHOD_CONNECT:
        return "CONNECT";
    case HTTP_METHOD_OPTIONS:
        return "OPTIONS";
    case HTTP_METHOD_TRACE:
        return "TRACE";
    default:
        return "UNKNOWN";
    }
}

const char *http_status_reason(enum http_status status)
{
    switch (status)
    {
    case HTTP_STATUS_OK:
        return "OK";
    case HTTP_STATUS_BAD_REQUEST:
        return "Bad Request";
    case HTTP_STATUS_FORBIDDEN:
        return "Forbidden";
    case HTTP_STATUS_NOT_FOUND:
        return "Not Found";
    case HTTP_STATUS_METHOD_NOT_ALLOWED:
        return "Method Not Allowed";
    case HTTP_STATUS_INTERNAL_SERVER_ERROR:
        return "Internal Server Error";
    case HTTP_STATUS_NOT_IMPLEMENTED:
        return "Not Implemented";
    case HTTP_STATUS_VERSION_NOT_SUPPORTED:
        return "HTTP Version Not Supported";
    default:
        return "Unknown";
    }
}

struct http_header *http_header_create(const char *name, const char *value)
{
    if (!name || !value)
    {
        return NULL;
    }

    struct http_header *header = calloc(1, sizeof(struct http_header));
    if (!header)
    {
        return NULL;
    }

    header->name = string_create(name, strlen(name));
    header->value = string_create(value, strlen(value));

    if (!header->name || !header->value)
    {
        http_header_destroy(header);
        return NULL;
    }

    return header;
}

void http_header_destroy(struct http_header *header)
{
    while (header)
    {
        struct http_header *next = header->next;
        string_destroy(header->name);
        string_destroy(header->value);
        free(header);
        header = next;
    }
}

void http_header_add(struct http_header **list, struct http_header *header)
{
    if (!list || !header)
    {
        return;
    }

    if (!*list)
    {
        *list = header;
        return;
    }

    struct http_header *current = *list;
    while (current->next)
    {
        current = current->next;
    }
    current->next = header;
}

struct http_header *http_header_find(struct http_header *list, const char *name)
{
    while (list)
    {
        if (string_compare_case_insensitive(list->name->data, name) == 0)
        {
            return list;
        }
        list = list->next;
    }
    return NULL;
}

struct http_request *http_request_create(void)
{
    struct http_request *request = calloc(1, sizeof(struct http_request));
    if (!request)
    {
        return NULL;
    }

    request->method = HTTP_METHOD_UNKNOWN;
    request->is_valid = false;

    return request;
}

void http_request_destroy(struct http_request *request)
{
    if (!request)
    {
        return;
    }

    string_destroy(request->method_str);
    string_destroy(request->target);
    string_destroy(request->version);
    string_destroy(request->body);
    http_header_destroy(request->headers);
    free(request);
}

static bool parse_request_line(struct http_request *request, const char *line,
                               size_t line_len)
{
    const char *space1 = memchr(line, ' ', line_len);
    if (!space1)
    {
        return false;
    }

    size_t method_len = space1 - line;
    request->method_str = string_create(line, method_len);
    if (!request->method_str)
    {
        return false;
    }

    char method_buf[32];
    if (method_len >= sizeof(method_buf))
    {
        return false;
    }
    memcpy(method_buf, line, method_len);
    method_buf[method_len] = '\0';
    request->method = http_method_from_string(method_buf);

    const char *target_start = space1 + 1;
    size_t remaining = line_len - (target_start - line);
    const char *space2 = memchr(target_start, ' ', remaining);
    if (!space2)
    {
        return false;
    }

    size_t target_len = space2 - target_start;
    request->target = string_create(target_start, target_len);
    if (!request->target)
    {
        return false;
    }

    const char *version_start = space2 + 1;
    size_t version_len = line_len - (version_start - line);
    request->version = string_create(version_start, version_len);
    if (!request->version)
    {
        return false;
    }

    return true;
}

static bool parse_header_line(struct http_request *request, const char *line,
                              size_t line_len)
{
    const char *colon = memchr(line, ':', line_len);
    if (!colon)
    {
        return false;
    }

    size_t name_len = colon - line;
    char name_buf[256];
    if (name_len >= sizeof(name_buf))
    {
        return false;
    }
    memcpy(name_buf, line, name_len);
    name_buf[name_len] = '\0';

    const char *value_start = colon + 1;
    size_t remaining = line_len - (value_start - line);
    while (remaining > 0 && (*value_start == ' ' || *value_start == '\t'))
    {
        value_start++;
        remaining--;
    }

    char value_buf[4096];
    if (remaining >= sizeof(value_buf))
    {
        return false;
    }
    memcpy(value_buf, value_start, remaining);
    value_buf[remaining] = '\0';

    struct http_header *header = http_header_create(name_buf, value_buf);
    if (!header)
    {
        return false;
    }

    http_header_add(&request->headers, header);
    return true;
}

struct parse_body_info
{
    const char *body_start;
    size_t body_len;
};

static bool parse_headers(struct http_request *request, const char *data,
                          size_t size, struct parse_body_info *body_info)
{
    const char *current = data;
    size_t remaining = size;

    while (remaining > 0)
    {
        const char *line_end = find_crlf(current, remaining);
        if (!line_end)
        {
            break;
        }

        size_t line_len = line_end - current;

        if (line_len == 0)
        {
            current = line_end + 2;
            body_info->body_start = current;
            body_info->body_len = size - (current - data);
            return true;
        }

        parse_header_line(request, current, line_len);

        current = line_end + 2;
        remaining = size - (current - data);
    }

    body_info->body_start = NULL;
    body_info->body_len = 0;
    return true;
}

struct http_request *http_request_parse(const char *data, size_t size)
{
    if (!data || size == 0)
    {
        return NULL;
    }

    struct http_request *request = http_request_create();
    if (!request)
    {
        return NULL;
    }

    const char *current = data;
    size_t remaining = size;

    const char *line_end = find_crlf(current, remaining);
    if (!line_end)
    {
        http_request_destroy(request);
        return NULL;
    }

    size_t line_len = line_end - current;
    if (!parse_request_line(request, current, line_len))
    {
        http_request_destroy(request);
        return NULL;
    }

    current = line_end + 2;
    remaining = size - (current - data);

    struct parse_body_info body_info;
    parse_headers(request, current, remaining, &body_info);

    if (body_info.body_start && body_info.body_len > 0)
    {
        request->body = string_create(body_info.body_start, body_info.body_len);
    }

    request->is_valid = true;
    return request;
}

struct http_response *http_response_create(enum http_status status)
{
    struct http_response *response = calloc(1, sizeof(struct http_response));
    if (!response)
    {
        return NULL;
    }

    response->status = status;
    response->version =
        string_create(HTTP_VERSION_1_1, strlen(HTTP_VERSION_1_1));
    response->reason_phrase = string_create(http_status_reason(status),
                                            strlen(http_status_reason(status)));

    if (!response->version || !response->reason_phrase)
    {
        http_response_destroy(response);
        return NULL;
    }

    return response;
}

void http_response_destroy(struct http_response *response)
{
    if (!response)
    {
        return;
    }

    string_destroy(response->version);
    string_destroy(response->reason_phrase);
    string_destroy(response->body);
    http_header_destroy(response->headers);
    free(response);
}

struct string *http_response_to_string(const struct http_response *response)
{
    if (!response)
    {
        return NULL;
    }

    struct string *result = string_create_empty(1024);
    if (!result)
    {
        return NULL;
    }

    string_concat_str(result, response->version->data, response->version->size);
    string_concat_str(result, " ", 1);

    char status_str[16];
    int status_len =
        snprintf(status_str, sizeof(status_str), "%d", response->status);
    string_concat_str(result, status_str, status_len);
    string_concat_str(result, " ", 1);

    string_concat_str(result, response->reason_phrase->data,
                      response->reason_phrase->size);
    string_concat_str(result, "\r\n", 2);

    struct http_header *header = response->headers;
    while (header)
    {
        string_concat_str(result, header->name->data, header->name->size);
        string_concat_str(result, ": ", 2);
        string_concat_str(result, header->value->data, header->value->size);
        string_concat_str(result, "\r\n", 2);
        header = header->next;
    }

    string_concat_str(result, "\r\n", 2);

    if (response->body && response->body->size > 0)
    {
        string_concat_str(result, response->body->data, response->body->size);
    }

    return result;
}
