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
#ifndef LCD_HAL_H_
#define LCD_HAL_H_

#include <stdint.h>
#include <stdbool.h>

#define LCD_CS_PORT     B
#define LCD_CS_PIN      1

#define LCD_SCK_PORT    B
#define LCD_SCK_PIN     0

#define LCD_SDA_PORT    B
#define LCD_SDA_PIN     3

#define LCD_RST_PORT    B
#define LCD_RST_PIN     4

void lcdhal_delayms(uint16_t ms);
void lcdhal_select(void);
void lcdhal_deselect(void);
void lcdhal_write(uint8_t cd, uint8_t data);
uint8_t lcdhal_read(uint8_t data);
void lcdhal_init(void);
#endif
