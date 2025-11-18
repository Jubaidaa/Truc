#ifndef HTTP_H
#define HTTP_H

#include <stdbool.h>
#include <stddef.h>

#include "../utils/string/string.h"

#define HTTP_VERSION_1_1 "HTTP/1.1"

enum http_method
{
    HTTP_METHOD_UNKNOWN,
    HTTP_METHOD_GET,
    HTTP_METHOD_HEAD,
    HTTP_METHOD_POST,
    HTTP_METHOD_PUT,
    HTTP_METHOD_DELETE,
    HTTP_METHOD_CONNECT,
    HTTP_METHOD_OPTIONS,
    HTTP_METHOD_TRACE
};

enum http_status
{
    HTTP_STATUS_OK = 200,
    HTTP_STATUS_BAD_REQUEST = 400,
    HTTP_STATUS_FORBIDDEN = 403,
    HTTP_STATUS_NOT_FOUND = 404,
    HTTP_STATUS_METHOD_NOT_ALLOWED = 405,
    HTTP_STATUS_INTERNAL_SERVER_ERROR = 500,
    HTTP_STATUS_NOT_IMPLEMENTED = 501,
    HTTP_STATUS_VERSION_NOT_SUPPORTED = 505
};

struct http_header
{
    struct string *name;
    struct string *value;
    struct http_header *next;
};

struct http_request
{
    enum http_method method;
    struct string *method_str;
    struct string *target;
    struct string *version;
    struct http_header *headers;
    struct string *body;
    bool is_valid;
};

struct http_response
{
    enum http_status status;
    struct string *version;
    struct string *reason_phrase;
    struct http_header *headers;
    struct string *body;
};

enum http_method http_method_from_string(const char *method);
const char *http_method_to_string(enum http_method method);
const char *http_status_reason(enum http_status status);

struct http_header *http_header_create(const char *name, const char *value);
void http_header_destroy(struct http_header *header);
void http_header_add(struct http_header **list, struct http_header *header);
struct http_header *http_header_find(struct http_header *list,
                                     const char *name);

struct http_request *http_request_create(void);
void http_request_destroy(struct http_request *request);
struct http_request *http_request_parse(const char *data, size_t size);

struct http_response *http_response_create(enum http_status status);
void http_response_destroy(struct http_response *response);
struct string *http_response_to_string(const struct http_response *response);

#endif // ! HTTP_H

