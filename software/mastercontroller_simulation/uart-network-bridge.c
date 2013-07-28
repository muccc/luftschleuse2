#include "uart.h"
#include <stdint.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <strings.h>

static int uart1_sockfd;

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
    return UART_NO_DATA;
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
    struct sockaddr_in servaddr;

    uart1_sockfd=socket(AF_INET,SOCK_DGRAM,0);

    bzero(&servaddr,sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr=htonl(INADDR_ANY);
    servaddr.sin_port=htons(32000);
    bind(uart1_sockfd,(struct sockaddr *)&servaddr,sizeof(servaddr));

}

unsigned int uart1_getc(void)
{
    uint8_t msg[100];
    int n = recv(uart1_sockfd,msg,1,MSG_DONTWAIT);
    if(n > 0) {
        printf("%X := %c := uart1_getc()\n", msg[0], msg[0]);
        return msg[0];
    }
    return UART_NO_DATA;
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
