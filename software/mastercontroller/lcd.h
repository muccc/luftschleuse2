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
#ifndef LCD_H_
#define LCD_H_

#include <stdint.h>
#include <stdbool.h>
/*
#badge    <s> 96x68
#badge    <s> das farbige hat 98x70 
*/

#define RESX 96
#define RESY 68
#define RESY_B 9

#define LCD_INVERTED (1<<0)
#define LCD_MIRRORX  (1<<1)
#define LCD_MIRRORY  (1<<2)

/* Display buffer */
extern uint8_t lcdBuffer[RESX*RESY_B];

void lcd_init(void);
void lcd_fill(uint8_t f);
void lcd_display(void);
void lcd_invert(void);
void lcd_toggleFlag(int flag);
void lcd_setPixel(uint8_t x, uint8_t y, bool f);
bool lcd_getPixel(uint8_t x, uint8_t y);
void lcd_setContrast(uint8_t c);
void lcd_setInvert(uint8_t c);

void lcd_startStream(void);
void lcd_stream(uint8_t r, uint8_t g, uint8_t b);
void lcd_stopStream(void);

#endif
