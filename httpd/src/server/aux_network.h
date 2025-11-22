#ifndef AUX_NETWORK_H
#define AUX_NETWORK_H

#include "../utils/aux_types.h"

U16 my_htons(U16 hostshort);
int my_inet_pton_ipv4(const char *src, void *dst);

#endif // ! AUX_NETWORK_H
