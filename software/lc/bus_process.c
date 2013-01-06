#include "bus_process.h"
#include "bus_handler.h"
#include "config.h"

void bus_init(void)
{
}

void bus_tick(void)
{
}

void bus_process(void)
{
    uint8_t channel = bus_readFrame();
    uint8_t len = bus_getMessageLen();
    if( channel == 'A'){
        //DDRC |= 0xC0;
        //PORTC ^= 0xC0;
    }
}

