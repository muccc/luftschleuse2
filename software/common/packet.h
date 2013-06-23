#ifndef PACKET_H_
#define PACKET_H_
#include <stdint.h>
#include <stdbool.h>

typedef struct{
    uint32_t seq;
    uint8_t  cmd;
    uint8_t  data[5];
    uint8_t  magic[6];
}__attribute__((packed)) packet_t;

#define PACKET_MAGIC        "SESAME"
#define PACKET_SYNC_MAGIC   "SYNCME"

bool packet_check_magic(packet_t *p);
bool packet_check_sync_magic(packet_t *p);

#endif
