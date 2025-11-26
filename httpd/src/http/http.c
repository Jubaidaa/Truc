#include "http.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../utils/aux_string.h"

static const char *find_crlf(const char *data, size_t size)
{
    if (size < 2)
    {
        return NULL;
    }

    for (size_t i = 0; i + 1 < size; i++)
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
    if (strcmp(method, "HEAD") == 0)
    {
        return HTTP_METHOD_HEAD;
    }
    if (strcmp(method, "POST") == 0)
    {
        return HTTP_METHOD_POST;
    }
    if (strcmp(method, "PUT") == 0)
    {
        return HTTP_METHOD_PUT;
    }
    if (strcmp(method, "DELETE") == 0)
    {
        return HTTP_METHOD_DELETE;
    }
    if (strcmp(method, "CONNECT") == 0)
    {
        return HTTP_METHOD_CONNECT;
    }
    if (strcmp(method, "OPTIONS") == 0)
    {
        return HTTP_METHOD_OPTIONS;
    }
    if (strcmp(method, "TRACE") == 0)
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

static bool parse_request_line_parts(struct http_request *request,
                                     const char *line, size_t line_len)
{
    char *buf = malloc(line_len + 1);
    if (!buf)
    {
        return false;
    }
    memcpy(buf, line, line_len);
    buf[line_len] = '\0';

    char *space1 = strchr(buf, ' ');
    if (!space1)
    {
        free(buf);
        return false;
    }
    *space1 = '\0';
    request->method_str = string_create(buf, space1 - buf);
    request->method = http_method_from_string(buf);

    char *target = space1 + 1;
    char *space2 = strchr(target, ' ');
    if (!space2)
    {
        free(buf);
        return false;
    }
    *space2 = '\0';
    request->target = string_create(target, space2 - target);
    request->version = string_create(space2 + 1, strlen(space2 + 1));

    free(buf);
    return true;
}

static bool parse_request_line(struct http_request *request, const char *line,
                               size_t line_len)
{
    if (!parse_request_line_parts(request, line, line_len))
    {
        return false;
    }

    if (!request->method_str || !request->target || !request->version)
    {
        return false;
    }

    if (request->target->size == 0 || request->target->data[0] != '/')
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
    char *name_buf = malloc(name_len + 1);
    if (!name_buf)
    {
        return false;
    }
    memcpy(name_buf, line, name_len);
    name_buf[name_len] = '\0';

    for (size_t i = 0; i < name_len; i++)
    {
        if (name_buf[i] == ' ' || name_buf[i] == '\t')
        {
            free(name_buf);
            return false;
        }
    }

    const char *v_start = colon + 1;
    size_t rem = line_len - (v_start - line);
    while (rem > 0 && (*v_start == ' ' || *v_start == '\t'))
    {
        v_start++;
        rem--;
    }

    char *val_buf = malloc(rem + 1);
    if (!val_buf)
    {
        free(name_buf);
        return false;
    }
    memcpy(val_buf, v_start, rem);
    val_buf[rem] = '\0';

    struct http_header *h = http_header_create(name_buf, val_buf);
    free(name_buf);
    free(val_buf);

    if (!h)
    {
        return false;
    }
    http_header_add(&request->headers, h);
    return true;
}

static bool check_null_bytes_in_headers(const char *data, size_t size)
{
    const char *body_start = NULL;

    if (size >= 4)
    {
        for (size_t i = 0; i + 3 < size; i++)
        {
            if (data[i] == '\r' && data[i + 1] == '\n' && data[i + 2] == '\r'
                && data[i + 3] == '\n')
            {
                body_start = &data[i + 4];
                break;
            }
        }
    }

    size_t header_len;
    if (body_start)
    {
        header_len = body_start - data;
    }
    else
    {
        header_len = size;
    }

    for (size_t i = 0; i < header_len; i++)
    {
        if (data[i] == '\0')
        {
            return true;
        }
    }
    return false;
}

static bool validate_content_length(struct http_request *request)
{
    struct http_header *cl = http_header_find(request->headers,
                                              "Content-Length");
    if (!cl || !cl->value || !cl->value->data)
    {
        return true;
    }

    const char *val = cl->value->data;
    if (*val == '\0' || *val == '-')
    {
        return false;
    }

    for (const char *p = val; *p != '\0'; p++)
    {
        if (*p < '0' || *p > '9')
        {
            return false;
        }
    }

    return true;
}

static bool parse_all_headers(struct http_request *req, const char *data,
                              size_t size, const char **cursor)
{
    const char *cur = *cursor;
    size_t rem = size - (cur - data);

    while (rem > 0)
    {
        const char *le = find_crlf(cur, rem);
        if (!le)
        {
            break;
        }

        size_t len = le - cur;
        if (len == 0)
        {
            cur = le + 2;
            size_t used = cur - data;
            if (size > used)
            {
                req->body = string_create(cur, size - used);
            }
            *cursor = cur;
            return true;
        }

        if (!parse_header_line(req, cur, len))
        {
            return false;
        }

        cur = le + 2;
        rem = size - (cur - data);
    }
    *cursor = cur;
    return true;
}

struct http_request *http_request_parse(const char *data, size_t size)
{
    if (!data || size == 0 || check_null_bytes_in_headers(data, size))
    {
        return NULL;
    }

    struct http_request *req = http_request_create();
    if (!req)
    {
        return NULL;
    }

    const char *cur = data;
    const char *le = find_crlf(cur, size);
    if (!le || !parse_request_line(req, cur, le - cur))
    {
        http_request_destroy(req);
        return NULL;
    }

    cur = le + 2;
    if (!parse_all_headers(req, data, size, &cur))
    {
        http_request_destroy(req);
        return NULL;
    }

    if (!validate_content_length(req))
    {
        http_request_destroy(req);
        return NULL;
    }

    req->is_valid = true;
    return req;
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

    struct string *res = string_create_empty(1024);
    if (!res)
    {
        return NULL;
    }

    string_concat_str(res, response->version->data, response->version->size);
    string_concat_str(res, " ", 1);

    char st_str[16];
    int st_len = snprintf(st_str, sizeof(st_str), "%d", response->status);
    string_concat_str(res, st_str, st_len);
    string_concat_str(res, " ", 1);

    string_concat_str(res, response->reason_phrase->data,
                      response->reason_phrase->size);
    string_concat_str(res, "\r\n", 2);

    struct http_header *h = response->headers;
    while (h)
    {
        string_concat_str(res, h->name->data, h->name->size);
        string_concat_str(res, ": ", 2);
        string_concat_str(res, h->value->data, h->value->size);
        string_concat_str(res, "\r\n", 2);
        h = h->next;
    }
    string_concat_str(res, "\r\n", 2);

    if (response->body && response->body->size > 0)
    {
        string_concat_str(res, response->body->data, response->body->size);
    }

    return res;
}
