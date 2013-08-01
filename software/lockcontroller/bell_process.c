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
#include "bell_process.h"
#include "pinutils.h"
#include <stdint.h>
#include <stdbool.h>

#define MIN_PRESS   50
#define MAX_PRESS   2500
#define MIN_BREAK   50
#define MAX_BREAK   2500

//static const int16_t times[] = {-200, -200, 500, -200, -200};
static const int16_t times[] = {-400, -400, 800, -400, -400};
static uint8_t step;
static uint8_t debounce;
static bool pressed;
static int16_t press_timer;
static int16_t break_timer;
static bool accepted;
static uint16_t c = 0;

void bell_init(void)
{
    DDR_CONFIG_IN(BELL);
    PIN_SET(BELL);
    step = 0;
    debounce = 0;
    pressed = false;
    press_timer = -1;
    break_timer = -1;
    accepted = false;
    c = 0;
}

static inline bool isPressed(void)
{
    if( !PIN_HIGH(BELL) ){
        return true;

    }
    return false;
}

void bell_tick(void)
{

    if( c && --c==0 ){
        accepted = false;
    }

    if( debounce == 0 && !pressed && isPressed() ){
        debounce = 50;
        pressed = true;
        press_timer = 0;
    }

    if( debounce == 0 && pressed && !isPressed() ){
        debounce = 50;
        pressed = false;
        break_timer = 0;
    }

    if( debounce ){
        debounce--;
    }

    if( pressed && (press_timer >= 0) ){
        press_timer++;
    }

    if( !pressed && (break_timer >= 0) ){
        break_timer++;
    }

 
    if( !pressed && press_timer > 0 ){
        if( times[step] > 0 ){
            if( press_timer > MIN_PRESS &&
                press_timer > times[step] ){
                step++;
            }else{
                step = 0;
            }
        }else{
            if( press_timer > MIN_PRESS &&
                press_timer < -times[step] ){
                step++;
            }else{
                step = 0;
            }
        }
        press_timer = -1;

        if( step * sizeof(times[0]) == sizeof(times) ){
            accepted = true;
            c = 2000;
        }
    }

    if( press_timer > MAX_PRESS ){
        press_timer = -1;
        step = 0;
    }

    if( break_timer > MAX_BREAK ){
        break_timer = -1;
        step = 0;
    }

}

bool bell_isAccepted(void)
{
    return accepted;
}

void bell_process(void)
{
}

