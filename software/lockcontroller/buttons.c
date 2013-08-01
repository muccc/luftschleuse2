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
#include "buttons.h"
#include "pinutils.h"
#include "bell_process.h"

#include <stdbool.h>

static uint8_t buttons_red_debounce;
static bool buttons_red_state;
static uint16_t buttons_red_counter;

static uint8_t buttons_pending;
static uint8_t buttons_toggle_state;
static uint8_t buttons_latched_state;

void buttons_init(void)
{
    DDR_CONFIG_IN(BUTTON_0);
    PIN_SET(BUTTON_0);
    buttons_pending = 0;
    buttons_red_state = false;
    buttons_red_debounce = 0;
    buttons_toggle_state = 0;
    buttons_red_counter = 0;
    buttons_latched_state = 0;
}
#if 0
uint8_t buttons_getPendingButtons(void)
{
    return buttons_pending;
}

void buttons_clearPendingButtons(uint8_t buttons)
{
    buttons_pending &= ~(buttons);
}

uint8_t buttons_getButtonsToggleState(void)
{
    return buttons_toggle_state;
}
#endif

uint8_t buttons_getButtonsLatchedState(void)
{
    uint8_t bell = 0;
    if( bell_isAccepted() == true ){
        bell = BUTTON_1;
    }

    return buttons_latched_state | bell;
}

static bool buttons_getButtonState(buttons_button_t button)
{
    switch( button ){
        case BUTTON_0:
            return !PIN_HIGH(BUTTON_0);
        break;
        case BUTTON_1:
            return false;   // "Soft" button handled in buttons_getButtonsLatchedState()
    }
    return false;
}

void buttons_tick(void)
{
    //TODO: generalize for more buttons
    if( buttons_getButtonState(BUTTON_0) ){
        if( buttons_red_state == false ){
            if( buttons_red_debounce++ > 100 ){
                buttons_red_state = true;
                buttons_red_debounce = 0;
                buttons_pending |= BUTTON_0;
                buttons_toggle_state ^= BUTTON_0;
                buttons_latched_state |= BUTTON_0;
                buttons_red_counter = 2000;
            }
        }else{
            buttons_red_debounce = 0;
        }
    }else{
        if( buttons_red_state == true ){
            if( buttons_red_debounce++ > 100 ){
                buttons_red_state = false;
                buttons_red_debounce = 0;
            }
        }else{
            buttons_red_debounce = 0;
        }
    }

    if( buttons_red_counter && --buttons_red_counter == 0 ){
        buttons_latched_state &= ~(BUTTON_0);
    }
}
