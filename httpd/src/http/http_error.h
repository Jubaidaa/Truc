#ifndef HTTP_ERROR_H
#define HTTP_ERROR_H

#include "http.h"

struct http_response *http_error_create_response(enum http_status status);
void http_error_send(int client_fd, enum http_status status);

#endif // ! HTTP_ERROR_H
