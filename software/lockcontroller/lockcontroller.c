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
#include "config.h"
#include "bus_handler.h"
#include "bus_process.h"
#include "command_process.h"
#include "aes.h"
#include "timer0.h"
#include "door.h"
#include "adc.h"
#include "power_process.h"
#include "leds.h"
#include "buttons.h"
#include "bell_process.h"
#include "sequence_numbers.h"

#include <util/delay.h>
#include <stdbool.h>
#include <string.h>
#include <stdint.h>


void lockcontroller_init(void)
{
    uint16_t i;
    for(i=0; i < 2000; i++) {
        _delay_ms(1);
    }

    leds_init();
    buttons_init();
    adc_init();
    power_init();
    door_init();
    sequence_numbers_init();
    aes_handler_init();
    bus_handler_init();
    bus_init();
    cmd_init();
    bell_init();
}

void lockcontroller_tick(void)
{
    bus_tick();
    door_tick();
    power_tick();
    cmd_tick();
    buttons_tick();
    leds_tick();
    bell_tick();
}

void lockcontroller_process(void)
{
    bus_process();
    door_process();
    power_process();
}

