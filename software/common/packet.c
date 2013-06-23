#include "packet.h"
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

bool packet_check_magic(packet_t *p)
{
    if( memcmp(PACKET_MAGIC, p->magic, sizeof(PACKET_MAGIC)) == 0 ){
        return true;
    }
    return false;
}

bool packet_check_sync_magic(packet_t *p)
{
    if( memcmp(PACKET_SYNC_MAGIC, p->magic,
            sizeof(PACKET_SYNC_MAGIC)) == 0 ){
        return true;
    }
    return false;
}

