#include "aux_network.h"

#include <string.h>

static int is_little_endian(void)
{
    uint16_t test = 1;
    return *((uint8_t *)&test) == 1;
}

uint16_t my_htons(uint16_t hostshort)
{
    if (is_little_endian())
    {
        return ((hostshort & 0xFF00) >> 8) | ((hostshort & 0x00FF) << 8);
    }
    return hostshort;
}

static int parse_ipv4_octet(const char **str)
{
    int value = 0;
    int digits = 0;
    
    while (**str >= '0' && **str <= '9')
    {
        value = value * 10 + (**str - '0');
        (*str)++;
        digits++;
        
        if (value > 255 || digits > 3)
        {
            return -1;
        }
    }
    
    if (digits == 0)
    {
        return -1;
    }
    
    return value;
}

int my_inet_pton_ipv4(const char *src, void *dst)
{
    if (!src || !dst)
    {
        return 0;
    }
    
    uint8_t *addr = (uint8_t *)dst;
    const char *ptr = src;
    
    for (int i = 0; i < 4; i++)
    {
        int octet = parse_ipv4_octet(&ptr);
        if (octet < 0)
        {
            return 0;
        }
        
        addr[i] = (uint8_t)octet;
        
        if (i < 3)
        {
            if (*ptr != '.')
            {
                return 0;
            }
            ptr++;
        }
    }
    
    if (*ptr != '\0')
    {
        return 0;
    }
    
    return 1;
}
