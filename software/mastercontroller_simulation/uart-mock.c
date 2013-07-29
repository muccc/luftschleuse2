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
