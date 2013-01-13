#ifndef DOOR_H_
#define DOOR_H_
#include <stdint.h>

#define DOOR_STATE_CLOSED       (1<<0)
#define DOOR_STATE_LOCKED       (1<<1)
#define DOOR_STATE_LOCKING      (1<<2)

typedef enum {
    DOOR_CMD_LOCK=0,
    DOOR_CMD_UNLOCK=1,
    DOOR_CMD_UNLOCK_PERM=2
} door_cmd_t;

void door_init(void);
uint8_t door_getState(void);
void door_cmd(door_cmd_t cmd);

#endif
