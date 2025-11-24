#ifndef AUX_SOCKET_H
#define AUX_SOCKET_H

#include <sys/socket.h>
#include <sys/types.h>

union socket_addr_union
{
    struct sockaddr sa;
    struct sockaddr_storage ss;
};

#endif // ! AUX_SOCKET_H
