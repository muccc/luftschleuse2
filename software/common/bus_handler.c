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
#include "bus_handler.h"
#include "pinutils.h"
#include <util/delay.h>

static uint8_t bus_buffer[SERIAL_BUFFERLEN];
static uint8_t bus_message[SERIAL_BUFFERLEN];
static uint8_t bus_command;
static uint16_t bus_messagelen;
static uint8_t bus_sendlock;

void bus_handler_init(void)
{
    uart_init(UART_BAUD_SELECT(UART_BAUDRATE, F_CPU));
    PIN_CLEAR(BUS_TX);
    PIN_CLEAR(BUS_nRX);
    DDR_CONFIG_OUT(BUS_nRX);
    DDR_CONFIG_OUT(BUS_TX);
    bus_sendlock = 0;
}

void bus_setTX(void)
{
    PIN_SET(BUS_TX);
    PIN_SET(BUS_nRX);
    bus_sendlock++;
}

void bus_setRX(void)
{
    bus_sendlock--;
    if( bus_sendlock == 0 ){
        while( uart_busy() );
        _delay_us(100);
        PIN_CLEAR(BUS_TX);
        PIN_CLEAR(BUS_nRX);
    }
}

uint16_t bus_getMessageLen(void)
{
    return bus_messagelen;
}

uint8_t * bus_getMessage(void)
{
    return bus_message;
}

void bus_putcenc(uint8_t c)
{
    bus_setTX();
    if( c == SERIAL_ESCAPE )
        uart_putc(SERIAL_ESCAPE);
    uart_putc(c);
    bus_setRX();
}

void bus_putsenc(char * s)
{
    bus_setTX();
    while( *s ){
        if( *s == SERIAL_ESCAPE )
            uart_putc(SERIAL_ESCAPE);
        uart_putc(*s++);
    }
    bus_setRX();
}

void bus_putenc(uint8_t *d, uint16_t n)
{
    bus_setTX();
    uint16_t i;
    for(i=0; i<n; i++){
        if( *d == SERIAL_ESCAPE )
            uart_putc(SERIAL_ESCAPE);
        uart_putc(*d++);
    }
    
    bus_setRX();
}

void bus_putStart(uint8_t command)
{
    bus_setTX();
    uart_putc(SERIAL_ESCAPE);
    uart_putc(command);
    bus_setRX();
}

void bus_putStop(void)
{
    bus_setTX();
    uart_putc(SERIAL_ESCAPE);
    uart_putc(SERIAL_END);
    bus_setRX();
}

#ifdef USEDEBUG
void bus_sendFrames(uint8_t command, char *s)
{
    bus_putStart(command);
    bus_putsenc(s);
    bus_putStop();
}
#endif

void bus_sendFramec(uint8_t command, uint8_t s)
{
    bus_setTX();
    bus_putStart(command);
    bus_putcenc(s);
    bus_putStop();
    bus_setRX();
}

void bus_sendFrame(uint8_t command, uint8_t *d, uint16_t n)
{
    bus_setTX();
    bus_putStart(command);
    bus_putenc(d, n);
    bus_putStop();
    bus_setRX();
}

uint8_t bus_readFrame(void)
{
    static uint16_t fill = 0;
    static uint8_t escaped = 0;
    int  i;
    uint8_t data;
    
    i = uart_getc();
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
            bus_command = data;
            return 0;
        }else if( data == SERIAL_END ){
            memcpy(bus_message, bus_buffer, fill);
            bus_messagelen = fill;
            return bus_command;
        }
    }
    bus_buffer[fill++] = data;
    if( fill >= SERIAL_BUFFERLEN ){
        fill = 0;
        escaped = 0;
    }
    return 0;
}

//uint8_t bus_isBusy(void)
//{
//    return bus_busy;
//}

