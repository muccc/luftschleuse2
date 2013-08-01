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
#ifndef BUS_PROCESS_H_
#define BUS_PROCESS_H_

#include "packet.h" 
#include <stdint.h>
#include <stdbool.h>

void bus_init(void);
void bus_tick(void);
void bus_process(void);
void bus_sendAck(bool success);
void bus_sendPacket(packet_t *p);
uint32_t bus_get_accepted_packets(void);
uint32_t bus_get_rejected_packets(void);
#endif
