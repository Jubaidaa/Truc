#ifndef AUX_NETWORK_H
#define AUX_NETWORK_H

#include <stdint.h>

uint16_t my_htons(uint16_t hostshort);
int my_inet_pton_ipv4(const char *src, void *dst);

#endif // ! AUX_NETWORK_H
