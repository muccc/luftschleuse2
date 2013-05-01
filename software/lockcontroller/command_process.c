#include "command_process.h"
#include "power_process.h"
#include "bus_process.h"
#include "packet.h"
#include "door.h"
#include "leds.h"
#include "buttons.h"
#include "power_process.h"
#include <avr/io.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
static uint16_t cmd_desiredDoorStateTimeout;
static uint8_t cmd_desiredDoorState;

void cmd_init(void)
{
    cmd_desiredDoorStateTimeout = 0;
    cmd_desiredDoorState = DOOR_LOCK_LOCKED;
}

void cmd_tick(void)
{
    if( cmd_desiredDoorStateTimeout-- == 0 ){
        cmd_desiredDoorStateTimeout = 0;
        cmd_desiredDoorState = DOOR_LOCK_LOCKED;
    }

    door_setDesiredState(cmd_desiredDoorState);

    if( door_getState() & DOOR_LOCK_LOCKED ){
        leds_set(LED_OPEN, LED_SHORT_FLASH);
    }else if( door_getState() & DOOR_LOCK_UNLOCKED ){
        leds_set(LED_OPEN, LED_ON);
    }
}

void cmd_process(void)
{
}

void cmd_setDesiredDoorState(uint8_t desiredState)
{
    cmd_desiredDoorState = desiredState;
    cmd_desiredDoorStateTimeout = 5000;
}

static void cmd_sendState(void)
{
   packet_t p;
   memset(&p, 0, sizeof(p));

   p.cmd = CMD_SEND_STATE;
   p.data[0] = buttons_getButtonsLatchedState();
   p.data[1] = door_getState();
   p.data[2] = 0;
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
    //}else if( cmd == CMD_SEND_STATE ){
        //cmd_sendState();
    }else if( cmd == CMD_CLEAR_BUTTONS ){
        uint8_t buttons = data[0];
        buttons_clearPendingButtons(buttons);
        bus_sendAck(true);
    }else if( cmd == CMD_DOOR_STATE ){
        uint8_t door_state = data[0];
        cmd_setDesiredDoorState(door_state);
        //bool success = door_cmd(cmd);
        cmd_sendState();
        //bus_sendAck(success);
    }
}

