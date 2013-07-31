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
#include "uart.h"
#include <stdint.h>
#include <stdio.h>

uint8_t uart_busy(void)
{
    return 0;
}

void uart_init(unsigned int baudrate)
{
}

unsigned int uart_getc(void)
{    
    //printf("uart_getc()\n");
    return 0;
}

void uart_putc(unsigned char data)
{
    printf("uart_putc(%X := %c)\n", data, data);
}

void uart_puts(const char *s )
{
    printf("uart_puts(%s)\n", s);
}

void uart_puts_p(const char *progmem_s )
{
    printf("uart_puts_P(%s)\n", progmem_s);
}

void uart1_init(unsigned int baudrate)
{
}

unsigned int uart1_getc(void)
{    
    //printf("uart1_getc()\n");
    return 0;
}

void uart1_putc(unsigned char data)
{
    printf("uart1_putc(%X := %c)\n", data, data);
}

void uart1_puts(const char *s )
{
    printf("uart1_puts(%s)\n", s);
}

void uart1_puts_p(const char *progmem_s )
{
    printf("uart1_puts_P(%s)\n", progmem_s);
}
