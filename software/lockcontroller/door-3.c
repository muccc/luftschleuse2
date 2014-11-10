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
#include "door-config.h"
#if DOOR_MODEL == DOOR_MODEL_3
#include "door-3.h"
#include "pinutils.h"
#include <stdbool.h>
#include <stdint.h>
#include <util/delay.h>
#include <stdbool.h>

static uint8_t door_desiredState;
static uint8_t door_doorstate;

static bool locking;
static bool unlocking;
static bool locked;
static bool unlocked;

static void door_turnRight(void);
static void door_turnLeft(void);
static void door_stop(void);

void door_init(void)
{
    door_doorstate = 0;

    PIN_CLEAR(DOOR_HBRIDGE_ENABLE);
    PIN_CLEAR(DOOR_HBRIDGE_IN1);
    PIN_CLEAR(DOOR_HBRIDGE_IN2);
   
    DDR_CONFIG_OUT(DOOR_HBRIDGE_ENABLE);
    DDR_CONFIG_OUT(DOOR_HBRIDGE_IN1);
    DDR_CONFIG_OUT(DOOR_HBRIDGE_IN2);

    locking = false;
    unlocking = false;
#if 0    
    int i;
    while(1){
        door_turnLeft();
        for(i=0; i<25; i++) _delay_ms(1);
        while(1){
            for(i=0; i<5; i++) _delay_ms(1);
            if(adc_getChannel(7) > 1000) {
                break;
            }
        }
        door_stop();
        for(i=0; i<1000; i++) _delay_ms(1);
        door_turnRight();
        for(i=0; i<25; i++) _delay_ms(1);
        while(1){
            for(i=0; i<5; i++) _delay_ms(1);
            if(adc_getChannel(7) > 1000) {
                break;
            }
        }
        door_stop();
        for(i=0; i<1000; i++) _delay_ms(1);
    }
#endif
}

static void door_turnRight(void)
{
    PIN_SET(DOOR_HBRIDGE_IN1);
    PIN_CLEAR(DOOR_HBRIDGE_IN2);
    PIN_SET(DOOR_HBRIDGE_ENABLE);
}

static void door_turnLeft(void)
{
    PIN_CLEAR(DOOR_HBRIDGE_IN1);
    PIN_SET(DOOR_HBRIDGE_IN2);
    PIN_SET(DOOR_HBRIDGE_ENABLE);
}

static void door_stop(void)
{
    PIN_CLEAR(DOOR_HBRIDGE_IN1);
    PIN_CLEAR(DOOR_HBRIDGE_IN2);
    PIN_CLEAR(DOOR_HBRIDGE_ENABLE);
}

static void door_startLock(void)
{
    int i;
    door_turnRight();
    for(i=0; i<30; i++) _delay_ms(1);
    if(adc_getChannel(7) > 1000) {
        door_stop();
        return;
    }
    locking = true;
    unlocking = false;
    unlocked = false;
    locked = false;
}

static void door_startUnlock(void)
{
    int i;
    door_turnLeft();
    for(i=0; i<30; i++) _delay_ms(1);
    if(adc_getChannel(7) > 1000) {
        door_stop();
        return;
    }
    unlocking = true;
    locking = false;
    locked = false;
    unlocked = false;
}

static void door_update_inputs(void)
{
    door_doorstate &= ~(DOOR_DOOR_CLOSED | DOOR_LOCK_LOCKED | DOOR_HANDLE_PRESSED | DOOR_LOCK_UNLOCKED| DOOR_LOCK_MANUAL_UNLOCKED);

    if(locked) {
        door_doorstate = DOOR_DOOR_CLOSED | DOOR_LOCK_LOCKED;
    }
    if(unlocked) {
        door_doorstate = DOOR_DOOR_CLOSED | DOOR_LOCK_UNLOCKED;
    }
}


void door_tick(void)
{
    door_update_inputs();
    if( door_desiredState == DOOR_LOCK_LOCKED && !locking && !locked ){
        door_startLock();
    }else if( door_desiredState == DOOR_LOCK_UNLOCKED && !unlocking && !unlocked){
        door_startUnlock();
    }

    if(adc_getChannel(7) > 1000) {
        door_stop();
        if(locking) {
            locking = false;
            locked = true;
            unlocked = false;
        }
        if(unlocking) {
            unlocking = false;
            unlocked = true;
            locked = false;
        }
    }
}

void door_process(void)
{
}

uint8_t door_getState(void)
{
    return door_doorstate;
}

void door_setDesiredState(uint8_t desiredState)
{
    door_desiredState = desiredState;
}

#endif
