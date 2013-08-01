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
#include "display_process.h"
#include "lcd.h"
#include "leds.h"
#include "lcdhal.h"

#include <stdint.h>

void display_init(void)
{
    lcd_init();
#if 0
    static uint8_t r = 0;
    while(1){
        uint16_t x = 5000;
        if( r == 0 )
            r = 0xf<<4;
        else
            r = 0;

        lcd_startStream();

        while(x--)
            lcd_stream(r,0,0);
        lcd_stopStream();
    }
#endif
}

void display_tick(void)
{
}

void display_process(void)
{
}

void display_data(uint8_t channel, uint8_t *data, uint8_t len)
{
    uint16_t x,r,g,b;
    if( channel == 0xFE ){
        if( len > 0 ){
            switch( data[0] ){
                case 0x01:
                    lcd_startStream();
                break;
                case 0x02:
                    lcd_stopStream();
                break;
                case 0x03:
                    if( len == 6 ){
                        x = data[0] << 8;
                        x |= data[1];
                        r = data[2];
                        g = data[3];
                        b = data[4];
                        while( x-- ){
                            lcd_stream(r,g,b);
                        }
                    }
                break;
                case 0x04:
                    lcd_init();
                break;
                case 0x05:
                    lcdhal_write(data[1], data[2]);
                break;
                case 0x06:
                    lcdhal_select();
                break;
                case 0x07:
                    lcdhal_deselect();
                break;
                case 0x08:
                    lcdhal_init();
                break;

            };
        }
    }else if( channel == 0xFF ){
        while( len >= 2 ){
             uint8_t count = data[0] >> 4;
             uint8_t r = data[0] << 4;
             uint8_t g = data[1] & 0xF0;
             uint8_t b = data[1] << 4;
             do {
                lcd_stream(r,g,b);
             }while(count--);
             data += 2;
             len -= 2;
        }
    }
}

