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
#include "io-mock.h"
#include "cli.h"

#include <stdio.h>
#include <string.h>

void io_ddr_config_in(char *pin)
{
    //printf("io_ddr_config_in(%s)\n", pin);
}

void io_ddr_config_out(char *pin)
{
    //printf("io_ddr_config_out(%s)\n", pin);
}

void io_pin_set(char *pin)
{
    if(strncmp(pin, "LED", 3) == 0) {
        cli_set_led(pin, true);
    }
    //printf("io_pin_set(%s)\n", pin);
}

void io_pin_clear(char *pin)
{
    if(strncmp(pin, "LED", 3) == 0) {
        cli_set_led(pin, false);
    }

    //printf("io_pin_clear(%s)\n", pin);
}

void io_pin_toggle(char *pin)
{
    //printf("io_pin_toggle(%s)\n", pin);
}

void io_pin_pulse(char *pin)
{
    //printf("io_pin_pulse(%s)\n", pin);
}

uint8_t io_pin_high_(char *pin)
{
    if(strncmp(pin, "BUTTON", 6) == 0) {
        return 1 - cli_get_button(pin);
    }

    //printf("io_pin_high(%s)\n", pin);
    //perror(pin);
    return 1;
}

