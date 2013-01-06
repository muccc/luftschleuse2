#include "command_process.h"
#include <avr/io.h>
#include <stdint.h>

void cmd_init(void)
{
}

void cmd_tick(void)
{
}

void cmd_process(void)
{
}

void cmd_new(uint8_t cmd, uint8_t *data)
{
    if( cmd == CMD_SET_LED ){
        uint8_t led = data[0];
        uint8_t state = data[1];
        //DDRC |= 0x80;
        //PORTC ^= 0x80;
    }
}

