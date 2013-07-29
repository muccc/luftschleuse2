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
#if DOOR_MODEL == DOOR_MODEL_MOCK
#include "door-1.h"
#include "cli.h"

#include "pinutils.h"
#include <stdbool.h>
#include <stdint.h>

void door_init(void)
{
}

void door_tick(void)
{
}

void door_process(void)
{
}

uint8_t door_getState(void)
{
    return cli_getState();
}

void door_setDesiredState(uint8_t desiredState)
{
    cli_updateDesiredState(desiredState);
}

#endif
 
