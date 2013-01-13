#include "command_process.h"
#include "bus_process.h"
#include "packet.h"
#include "door.h"
#include "leds.h"
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
   p.data[0] = buttons_getPendingButtons();
   p.data[1] = door_getState();
   p.data[2] = leds_getState();
   p.data[3] = 0;
   p.data[4] = 0;

   bus_sendPacket(&p);
}

void cmd_new(uint8_t cmd, uint8_t *data)
{
    uint8_t reply = 0;

    if( cmd == CMD_SET_LED ){
        uint8_t led = data[0];
        uint8_t state = data[1];
        leds_set(led, state);
    }else if( cmd == CMD_SEND_STATE ){
        cmd_sendState();
        reply = 1;
    }else if( cmd == CMD_CLEAR_BUTTONS ){
        uint8_t buttons = data[0];
        buttons_clearButtons(buttons);
    }else if( cmd == CMD_DOOR_CMD ){
        uint8_t cmd = data[0];
        door_cmd(cmd);
    }
    
    if( !reply ){
        bus_sendAck();
    }
}

