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
#include "power_process.h"
#include "adc.h"
#include <stdint.h>

static uint16_t power_inputVoltage;

static void power_update(void);

void power_init(void)
{
    power_update();
}

void power_tick(void)
{
    static uint16_t c = 0;
    if( c-- == 0 ){
        c = 500;
        power_update();
    }
}

void power_process(void)
{
}

uint16_t power_getInputVoltage(void)
{
    return power_inputVoltage;
}

static void power_update(void)
{
    power_inputVoltage = adc_getChannel(POWER_INPUT_VOLTAGE_CHANNEL); 
    power_inputVoltage *= 11;
}
