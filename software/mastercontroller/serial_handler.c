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
#include <stdint.h>
#include <string.h>
#include "config.h"
#include "uart.h"
#include "serial_handler.h"

static uint8_t serial_buffer[SERIAL_BUFFERLEN];
static uint8_t serial_message[SERIAL_BUFFERLEN];
static uint8_t serial_command;
static uint16_t serial_messagelen;

void serial_handler_init(void)
{
    uart1_init(UART_BAUD_SELECT(UART_BAUDRATE, F_CPU));
}

uint16_t serial_getMessageLen(void)
{
    return serial_messagelen;
}

uint8_t * serial_getMessage(void)
{
    return serial_message;
}

 void serial_putcenc(uint8_t c)
{
    if( c == SERIAL_ESCAPE )
        uart1_putc(SERIAL_ESCAPE);
    uart1_putc(c);
}

void serial_putsenc(char * s)
{
    while( *s ){
        if( *s == SERIAL_ESCAPE )
            uart1_putc(SERIAL_ESCAPE);
        uart1_putc(*s++);
    }
}

void serial_putenc(uint8_t *d, uint16_t n)
{
    uint16_t i;
    for(i=0; i<n; i++){
        if( *d == SERIAL_ESCAPE )
            uart1_putc(SERIAL_ESCAPE);
        uart1_putc(*d++);
    }
}

 void serial_putStart(uint8_t command)
{
    uart1_putc(SERIAL_ESCAPE);
    uart1_putc(command);
}

 void serial_putStop(void)
{
    uart1_putc(SERIAL_ESCAPE);
    uart1_putc(SERIAL_END);
}

#ifdef USEDEBUG
void serial_sendFrames(uint8_t command, char *s)
{
    serial_putStart(command);
    serial_putsenc(s);
    serial_putStop();
}
#endif

void serial_sendFramec(uint8_t command, uint8_t s)
{
    serial_putStart(command);
    serial_putcenc(s);
    serial_putStop();
}

void serial_sendFrame(uint8_t command, uint8_t *d, uint16_t n)
{
    serial_putStart(command);
    serial_putenc(d, n);
    serial_putStop();
}

uint8_t serial_readFrame(void)
{
    static uint16_t fill = 0;
    static uint8_t escaped = 0;
    int  i;
    uint8_t data;
    
    i = uart1_getc();
    if ( i & UART_NO_DATA ){
        return 0;
    }
    data = i&0xFF;

    if( data == SERIAL_ESCAPE ){
        if( !escaped ){
            escaped = 1;
            return 0;
        }
        escaped = 0;
    }else if( escaped ){
        escaped = 0;
        if( data != SERIAL_END ){
            fill = 0;
            serial_command = data;
            return 0;
        }else if( data == SERIAL_END ){
            memcpy(serial_message, serial_buffer, fill);
            serial_messagelen = fill;
            return serial_command;
        }
    }
    serial_buffer[fill++] = data;
    if( fill >= SERIAL_BUFFERLEN ){
        fill = 0;
        escaped = 0;
    }
    return 0;
}

