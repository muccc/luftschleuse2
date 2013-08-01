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
#ifndef BUTTONS_H_
#define BUTTONS_H_
#include <stdint.h>
#include <stdbool.h>

#define BUTTON_0_PORT              C
#define BUTTON_0_PIN               1

typedef enum {
    BUTTON_0 = 1,
    BUTTON_1 = 2        // "Soft" button of the bell code...
} buttons_button_t;

void buttons_init(void);
void buttons_tick(void);
#if 0
uint8_t buttons_getPendingButtons(void);
void buttons_clearPendingButtons(uint8_t buttons);
uint8_t buttons_getButtonsToggleState(void);
#endif

//bool buttons_getButtonState(buttons_button_t button);
uint8_t buttons_getButtonsLatchedState(void);

#endif
