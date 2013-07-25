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
#include "lcd.h"
#include "lcdhal.h"
#include <string.h>
#include <stdint.h>
#include <stdbool.h>


#define DISPLAY_N1200 0
#define DISPLAY_N1600 1

#define MODE 16 /* 8 or 16 */

#if MODE == 8
#define putpix(x) _helper_pixel8(x)
#define px_INIT_MODE 2
#define px_PACK(r,g,b) COLORPACK_RGB332(r,g,b)
#define px_type uint8_t
#else
 #if MODE == 12
 #define putpix(x) _helper_pixel12(x)
 #define px_INIT_MODE 3
 #define px_PACK(r,g,b) COLORPACK_RGB444(r,g,b)
 #define px_type uint16_t
 #else
 #define putpix(x) _helper_pixel16(x)
 #define px_INIT_MODE 5
 #define px_PACK(r,g,b) COLORPACK_RGB565(r,g,b)
 #define px_type uint16_t
 #endif
#endif

/**************************************************************************/
/* Utility routines to manage nokia display */
/**************************************************************************/

uint8_t lcdBuffer[RESX*RESY_B];
uint8_t displayType;
static bool lcdmirror;
static bool lcdinvert;

#define TYPE_CMD    0
#define TYPE_DATA   1

void lcd_init(void)
{
    uint8_t id;

    lcdhal_init();

    //id=lcdhal_read(220); // ID3
    
    //if(id==14)
        displayType=DISPLAY_N1600;
    //else /* ID3 == 48 */
    //    displayType=DISPLAY_N1200;
    
/* Small Nokia 1200 LCD docs:
 *           clear/ set
 *  on       0xae / 0xaf
 *  invert   0xa6 / 0xa7
 *  mirror-x 0xA0 / 0xA1
 *  mirror-y 0xc7 / 0xc8
 *
 *  0x20+x contrast (0=black - 0x2e)
 *  0x40+x offset in rows from top (-0x7f)
 *  0x80+x contrast? (0=black -0x9f?)
 *  0xd0+x black lines from top? (-0xdf?)
 *
 */
    lcdhal_select();

    if(displayType==DISPLAY_N1200){
    	/* Decoded:
    	 * E2: Internal reset
    	 * AF: Display on/off: DON = 1
    	 * A1: undefined?
    	 * A4: all on/normal: DAL = 0
    	 * 2F: charge pump on/off: PC = 1
    	 * B0: set y address: Y[0-3] = 0
    	 * 10: set x address (upper bits): X[6-4] = 0
    	 */
        static uint8_t initseq[]= { 0xE2,0xAF, // Display ON
                             0xA1, // Mirror-X
                             0xA4, 0x2F, 0xB0, 0x10};
        uint8_t i = 0;
        while(i<sizeof(initseq)){
            lcdhal_write(TYPE_CMD,initseq[i++]);
            lcdhal_delayms(5); // actually only needed after the first
        }
    }else{ /* displayType==DISPLAY_N1600 */
        static uint8_t initseq_d[] = {
        		/* The controller is a PCF8833 -
                   documentation can be found online.
        		 * CMD 01: Soft-reset
        		 * CMD 11: Sleep-out
        		 * CMD 29: Display ON
        		 * CMD 03: Booster voltage ON
        		 * CMD 13: Normal display mode
        		 * CMD 3A: interface pixel format 
        		 *   DAT 02  | 02:  8 bit/pixel (3:3:2)
        		 *           | 03: 12 bit/pixel (4:4:4)
        		 *           | 05: 16 bit/pixel (5:6:5)
        		 * CMD 2A: column address set
        		 *   DAT 1    : xs
        		 *   DAT 98-2 : xe
        		 * CMD 2B: page address set
        		 *   DAT 1    : ys
        		 *   DAT 70-2 : ye
        		 */
            0x11, 
            0x29, 
            0x03, 
            0x13, 
            0x3A, px_INIT_MODE, 
//            0x2A, 1, 98-2, 
            0x2A, 0, 97, 
//            0x2B, 1, 70-2
            0x2B, 0, 69
        };
        uint16_t initseq_c = ~  (  /* comands: 1, data: 0 */
                (1<<0) |
                (1<<1) |
                (1<<2) |
                (1<<3) |
                (1<<4) | (0<< 5) |
                (1<<6) | (0<< 7) | (0<< 8) |
                (1<<9) | (0<<10) | (0<<11) |
                0);
        uint8_t i = 0;

        lcdhal_write(0, 0x01); /* most color displays need the pause */
        lcdhal_delayms(10);

        while(i<sizeof(initseq_d)){
            lcdhal_write(initseq_c&1, initseq_d[i++]);
            initseq_c = initseq_c >> 1;
        }
    }
    lcdhal_deselect();
}

void lcd_fill(uint8_t f)
{
    memset(lcdBuffer,f,RESX*RESY_B);
}

void lcd_setPixel(uint8_t x, uint8_t y, bool f)
{
    if (x<0 || x> RESX || y<0 || y > RESY)
        return;
    uint8_t y_byte = (RESY-(y+1)) / 8;
    uint8_t y_off = (RESY-(y+1)) % 8;
    uint8_t byte = lcdBuffer[y_byte*RESX+(RESX-(x+1))];
    if (f) {
        byte |= (1 << y_off);
    } else {
        byte &= ~(1 << y_off);
    }
    lcdBuffer[y_byte*RESX+(RESX-(x+1))] = byte;
}

bool lcd_getPixel(uint8_t x, uint8_t y)
{
    uint8_t y_byte = (RESY-(y+1)) / 8;
    uint8_t y_off = (RESY-(y+1)) % 8;
    uint8_t byte = lcdBuffer[y_byte*RESX+(RESX-(x+1))];
    return byte & (1 << y_off);
}

// Color display helper functions
static inline void _helper_pixel8(uint8_t color1)
{
    lcdhal_write(TYPE_DATA, color1);
}

static void _helper_pixel12(uint16_t color)
{
    static uint8_t odd=0;
    static uint8_t rest;
    if(odd){
        lcdhal_write(TYPE_DATA,(rest<<4) | (color>>8));
        lcdhal_write(TYPE_DATA,color&0xff);
    }else{
        lcdhal_write(TYPE_DATA,(color>>4)&0xff);
        rest=(color&0x0f);
    }
    odd=1-odd;
}

static void _helper_pixel16(uint16_t color)
{
    lcdhal_write(TYPE_DATA,color>>8);
    lcdhal_write(TYPE_DATA,color&0xFF);
}

#define COLORPACK_RGB565(r,g,b) (((r&0xF8) << 8) | ((g&0xFC)<<3) | ((b&0xF8) >> 3))
#define COLORPACK_RGB444(r,g,b) ( ((r&0xF0)<<4) | (g&0xF0) | ((b&0xF0)>>4) )
#define COLORPACK_RGB332(r,g,b) ( (((r>>5)&0x7)<<5) | (((g>>5)&0x7)<<2) | ((b>>6)&0x3) )

static const px_type COLOR_FG =   px_PACK(0xff, 0x00, 0x00);
static const px_type COLOR_BG =   px_PACK(0x00, 0x00, 0x00);

void lcd_display(void)
{
    uint8_t byte;
    lcdhal_select();

    if(displayType==DISPLAY_N1200){
        lcdhal_write(TYPE_CMD,0xB0);
        lcdhal_write(TYPE_CMD,0x10);
        lcdhal_write(TYPE_CMD,0x00);
        uint16_t i,page;
        for(page=0; page<RESY_B;page++) {
            for(i=0; i<RESX; i++) {
                if (lcdmirror)
                    byte=lcdBuffer[page*RESX+RESX-1-(i)];
                else
                    byte=lcdBuffer[page*RESX+(i)];
  
                if (lcdinvert)
                    byte=~byte;
      
                lcdhal_write(TYPE_DATA,byte);
            }
        }
    } else { /* displayType==DISPLAY_N1600 */
        uint16_t x,y;
        bool px;
 
        lcdhal_write(TYPE_CMD,0x2C);
  
        for(y=RESY;y>0;y--){
            for(x=RESX;x>0;x--){
                if(lcdmirror)
                    px=lcd_getPixel(RESX-x,y-1);
                else
                    px=lcd_getPixel(x-1,y-1);

                if((!px)^(!lcdinvert)) {
                    putpix(COLOR_FG); /* foreground */
                } else {
                    putpix(COLOR_BG); /* background */
                }
            }
        }
    }
    lcdhal_deselect();
}

void lcd_startStream(void)
{
    lcdhal_select();
    lcdhal_write(TYPE_CMD,0x2C);
}

void lcd_stream(uint8_t r, uint8_t g, uint8_t b)
{
    putpix(px_PACK(r, g, b));
}

void lcd_stopStream(void)
{
    lcdhal_deselect();
}

void lcdRefresh(void) __attribute__ ((weak, alias ("lcd_display")));

inline void lcd_invert(void)
{
    lcdinvert=!lcdinvert;
}

void lcd_setContrast(uint8_t c)
{
    lcdhal_select();
    if(displayType==DISPLAY_N1200){
        if(c<0x1F)
            lcdhal_write(TYPE_CMD,0x80+c);
    }else{ /* displayType==DISPLAY_N1600 */
        if(c<0x40) {
            lcdhal_write(TYPE_CMD,0x25);
            lcdhal_write(TYPE_DATA,4*c);
        }
    }
    lcdhal_deselect();
}

void lcd_setInvert(uint8_t c)
{
    lcdhal_select();
     /* it doesn't harm N1600, save space */
//  if(displayType==DISPLAY_N1200)
        lcdhal_write(TYPE_CMD,(c&1)+0xa6);
    lcdhal_deselect();
}

