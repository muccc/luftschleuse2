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
#ifndef PACKET_H_
#define PACKET_H_
#include <stdint.h>
#include <stdbool.h>

typedef struct{
    uint32_t seq;
    uint8_t  cmd;
    uint8_t  data[5];
    uint8_t  magic[6];
}__attribute__((packed)) packet_t;

#define PACKET_MAGIC        "SESAME"
#define PACKET_SYNC_MAGIC   "SYNCME"

bool packet_check_magic(packet_t *p);
bool packet_check_sync_magic(packet_t *p);

#endif
