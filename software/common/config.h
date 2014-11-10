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
#ifndef CONFIG_H
#define CONFIG_H

#define SERIAL_BUFFERLEN    128

#define SERIAL_ESCAPE   '\\'
#define SERIAL_END      '9'

#define UART_BAUDRATE   115200

#define BUS_TX_PORT  D
#define BUS_TX_PIN   4

#define BUS_nRX_PORT  D
#define BUS_nRX_PIN   5

#define DOOR_MODEL_1      0
#define DOOR_MODEL_2      1
#define DOOR_MODEL_3      2
#define DOOR_MODEL_MOCK   3

#endif

