#include "door.h"
#include <avr/io.h>

uint8_t door_init(void)
{
    DDRC |= 0xC0;
    PORTC &= ~0xC0;
}

uint8_t door_getState(void)
{
    return 0;
}

uint8_t door_setState(uint8_t state)
{
    if( state ){
        PORTC |= 0x80;
    }else{
        PORTC &= ~0x80;
    }
}
