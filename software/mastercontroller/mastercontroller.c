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
#include "uart.h"
#include "serial_process.h"
#include "command_process.h"
#include "bus_handler.h"
#include "bus_process.h"
#include "aes.h"
#include "timer0.h"
#include "buttons.h"
#include "leds.h"
#include "display_process.h"

#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <string.h>
#include <stdint.h>


int main(void)
{
    wdt_disable();
    /* Clear WDRF in MCUSR */
    MCUSR &= ~(1<<WDRF);
    /* Write logical one to WDCE and WDE */
    /* Keep old prescaler setting to prevent unintentional time-out */
    WDTCSR |= (1<<WDCE) | (1<<WDE);
    /* Turn off WDT */
    WDTCSR = 0x00;

    bus_handler_init();
    serial_init();
    bus_init();
    cmd_init();
    buttons_init();
    leds_init();
    display_init();
    timer0_init();
    sei();
    
    leds_set(0,LED_SHORT_FLASH);
    leds_set(1,LED_BLINK_FAST);
    leds_set(2,LED_BLINK_SLOW);

    while( 1 ){
        if( timer0_timebase ){
            timer0_timebase--;
            bus_tick();
            serial_tick();
            buttons_tick();
            leds_tick();
            display_tick();
        }
        bus_process();
        serial_process();
        display_process();
    }
}

