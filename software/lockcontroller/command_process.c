#include "command_process.h"
#include "power_process.h"
#include "bus_process.h"
#include "packet.h"
#include "door.h"
#include "leds.h"
#include "buttons.h"
#include <avr/io.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

void cmd_init(void)
{
}

void cmd_tick(void)
{
    static uint16_t ticks = 0;
    static bool busy = false; 
#if 0
    if( ticks++ == 1000 ){
        ticks = 0;
        if( buttons_getButtonState(BUTTON_RED) ){
            if( !busy ){
                busy = true;
                if( door_getState() & DOOR_LOCK_PERM_UNLOCKED ){
                    door_cmd(DOOR_CMD_LOCK);
                }else{
                    door_cmd(DOOR_CMD_UNLOCK_PERM);
                }
            }
        }else{
            busy = false;
        }
    }
#else    
//    if( ticks++ == 1000 ){
//    ticks = 0;
    if( buttons_getPendingButtons() == BUTTON_RED ){
        if( door_getState() & DOOR_LOCK_PERM_UNLOCKED ){
        //if( door_getState() & DOOR_LOCK_UNLOCKED ){
            door_cmd(DOOR_CMD_LOCK);
        }else{
            door_cmd(DOOR_CMD_UNLOCK_PERM);
        }
        buttons_clearPendingButtons(BUTTON_RED);
    }
//    }
#endif
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
   p.data[3] = power_getInputVoltage()/100;
   p.data[4] = 0;

   bus_sendPacket(&p);
}

void cmd_new(uint8_t cmd, uint8_t *data)
{
    if( cmd == CMD_SET_LED ){
        uint8_t led = data[0];
        uint8_t state = data[1];
        leds_set(led, state);
        bus_sendAck(true);
    }else if( cmd == CMD_SEND_STATE ){
        cmd_sendState();
    }else if( cmd == CMD_CLEAR_BUTTONS ){
        uint8_t buttons = data[0];
        buttons_clearPendingButtons(buttons);
        bus_sendAck(true);
    }else if( cmd == CMD_DOOR_CMD ){
        uint8_t cmd = data[0];
        bool success = door_cmd(cmd);
        bus_sendAck(success);
    }
}

