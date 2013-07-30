/*
 *  This file is part of the luftschleuse2 project.
 *
 *  See https://github.com/muccc/luftschleuse2 for more information.
 *
 *  Copyright (C) 2013 Tobias Schneider <schneider@muc.ccc.de> 
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "command_process.h"
#include "leds.h"
#include "packet.h"
#include "buttons.h"
#include "power_process.h"
#include "serial_process.h"

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

