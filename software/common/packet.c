#include "packet.h"
#include <stdint.h>
#include <string.h>

uint8_t packet_check(packet_t *p)
{
    if( memcmp(PACKET_MAGIC, p->magic, sizeof(PACKET_MAGIC)) == 0 )
        return 1;
    return 0;
}

