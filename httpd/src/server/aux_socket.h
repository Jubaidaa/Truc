#ifndef AUX_SOCKET_H
#define AUX_SOCKET_H

#include <sys/socket.h>
#include <sys/types.h>

struct sockaddr_in;

union socket_addr_union {
  struct sockaddr sa;
  struct sockaddr_in sin;
};

#endif // ! AUX_SOCKET_H
