#include "http_error.h"

#include <stdio.h>
#include <time.h>
#include <unistd.h>

#include "../utils/aux_string.h"
#include "../utils/string/string.h"

static void get_date_header_val(char *buffer, size_t size)
{
    time_t now = time(NULL);
    struct tm *gmt = gmtime(&now);

    if (gmt)
    {
        strftime(buffer, size, "%a, %d %b %Y %H:%M:%S GMT", gmt);
    }
    else
    {
        buffer[0] = '\0';
    }
}

struct http_response *http_error_create_response(enum http_status status)
{
    struct http_response *response = http_response_create(status);
    if (!response)
    {
        return NULL;
    }

    char date_buf[64];
    get_date_header_val(date_buf, sizeof(date_buf));
    struct http_header *date = http_header_create("Date", date_buf);
    http_header_add(&response->headers, date);

    struct http_header *conn = http_header_create("Connection", "close");
    http_header_add(&response->headers, conn);

    struct http_header *len = http_header_create("Content-Length", "0");
    http_header_add(&response->headers, len);

    return response;
}

void http_error_send(int client_fd, enum http_status status)
{
    struct http_response *response = http_error_create_response(status);
    if (!response)
    {
        return;
    }

    struct string *response_str = http_response_to_string(response);
    if (response_str)
    {
        write(client_fd, response_str->data, response_str->size);
        string_destroy(response_str);
    }

    http_response_destroy(response);
}
