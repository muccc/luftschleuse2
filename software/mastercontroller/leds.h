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
#ifndef LEDS_H_
#define LEDS_H_
#include <stdint.h>

#define LED_0_PORT              A
#define LED_0_PIN               0

#define LED_1_PORT              A
#define LED_1_PIN               1

#define LED_2_PORT              A
#define LED_2_PIN               2

typedef enum {
    LED_ON,
    LED_OFF,
    LED_BLINK_FAST,
    LED_BLINK_SLOW,
    LED_SHORT_FLASH
} leds_state_t;

typedef enum {
    LED_0,
    LED_1,
    LED_2,
} leds_led_t;

#define LEDS_COUNT  3

void leds_init(void);
uint8_t leds_getState(void);
void leds_set(leds_led_t led, leds_state_t state);
void leds_tick(void);
#endif
