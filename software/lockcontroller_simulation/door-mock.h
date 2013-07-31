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
#ifndef DOOR_H_
#define DOOR_H_
#include <stdbool.h>
#include <stdint.h>

#define DOOR_DOOR_CLOSED            (1<<0)
#define DOOR_LOCK_LOCKED            (1<<1)
#define DOOR_LOCK_UNLOCKED          (1<<2)
#define DOOR_LOCK_LOCKING           (1<<3)
#define DOOR_LOCK_UNLOCKING         (1<<4)
#define DOOR_HANDLE_PRESSED         (1<<5)
#define DOOR_LOCK_MANUAL_UNLOCKED   (1<<6)

typedef enum {
    DOOR_CMD_LOCK=0,
    DOOR_CMD_UNLOCK=1,
    DOOR_CMD_UNLOCK_PERM=2,
    DOOR_CMD_NONE
} door_cmd_t;

void door_init(void);
void door_tick(void);
void door_process(void);
uint8_t door_getState(void);
void door_setDesiredState(uint8_t desiredState);

#endif

