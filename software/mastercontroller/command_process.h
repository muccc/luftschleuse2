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
#ifndef CMD_PROCESS_H_
#define CMD_PROCESS_H_

#include <stdint.h>


typedef enum {
    CMD_SET_LED  = 'L',
    CMD_WRITE_LCD = 'W',
    CMD_GET_STATE = 'S',
    CMD_SEND_STATE = 'S'
} cmd_t;

void cmd_init(void);
void cmd_tick(void);
void cmd_process(void);
void cmd_new(cmd_t cmd, uint8_t *data);

#endif
