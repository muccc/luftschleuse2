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
#include "leds.h"
#include "pinutils.h"

static leds_state_t leds_open_state;
static uint16_t leds_counter;
void leds_init(void)
{
    DDR_CONFIG_OUT(LED_OPEN_PIN);
    PIN_SET(LED_OPEN_PIN);
    leds_open_state = LED_OFF;
    leds_counter = 0;
}

uint8_t leds_getState(void)
{
    return 0;
}

void leds_set(leds_led_t led, leds_state_t state)
{
    if( led == LED_OPEN ){
        leds_open_state = state;
    }
}

void leds_tick(void)
{
    leds_counter++;
    if( leds_open_state == LED_ON){
        PIN_SET(LED_OPEN_PIN);
    }else if( leds_open_state == LED_OFF ){
        PIN_CLEAR(LED_OPEN_PIN);
    }else if( leds_open_state == LED_SHORT_FLASH ){
        if( (leds_counter % 1024) < 50 ){
            PIN_SET(LED_OPEN_PIN);
        }else{
            PIN_CLEAR(LED_OPEN_PIN);
        }
    }

}
