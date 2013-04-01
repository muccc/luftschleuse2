#include "command_process.h"
#include "leds.h"
#include "packet.h"
#include "buttons.h"

#include <avr/io.h>
#include <stdint.h>
#include <string.h>

void cmd_init(void)
{
}

void cmd_tick(void)
{
}

void cmd_process(void)
{
}

static void cmd_sendState(void)
{
   packet_t p;
   memset(&p, 0, sizeof(p));

   p.cmd = CMD_SEND_STATE;
   p.data[0] = buttons_getButtonsLatchedState();
   p.data[1] = 0;
   p.data[2] = 0;
   p.data[3] = power_getInputVoltage()/100;
   p.data[4] = 0;

   serial_sendPacket(&p);
}


void cmd_new(cmd_t cmd, uint8_t *data)
{
    uint8_t led, state;
    switch( cmd ){
        case CMD_GET_STATE:
            cmd_sendState();
        break;
        case CMD_SET_LED:
            led = data[0];
            state = data[1];
            leds_set(led, state);
        break;
        case CMD_WRITE_LCD:
        break;
    }
}

