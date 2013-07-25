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
#include "timer0.h"

#include <avr/interrupt.h>
#include <avr/io.h>

volatile uint8_t timer0_timebase = 0;

ISR(TIMER0_OVF_vect)
{
    timer0_timebase++;
}

void timer0_init(void)
{
    //18e6/256/64 ~ 1099
#if defined(__AVR_ATmega324P__) ||  defined(__AVR_ATmega644P__)
    TCCR0B = (1<<CS00) | (1<<CS01);           //divide by 64
    TIMSK0 |= 1<<TOIE0;         //enable timer interrupt
#else
    TCCR0 = (1<<CS00) | (1<<CS01);           //divide by 64
    TIMSK |= 1<<TOIE0;          //enable timer interrupt
#endif
}
