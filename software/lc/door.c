#include "door.h"
#include <avr/io.h>

void door_init(void)
{
    DDRC |= 0xC0;
    PORTC &= ~0xC0;
}

uint8_t door_getState(void)
{
    return 0;
}

void door_cmd(door_cmd_t cmd)
{
    switch(cmd){
        case DOOR_CMD_UNLOCK:
            PORTC |= 0x80;
        break;
        case DOOR_CMD_LOCK:
            PORTC &= ~0x80;
        break;
    }
}
