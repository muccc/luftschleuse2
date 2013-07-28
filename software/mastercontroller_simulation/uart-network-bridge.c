#include "uart.h"
#include <stdint.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <strings.h>
#include <stdio.h>
#include <stdlib.h>

static int uart_sockfd;
static int group_sockfd;

struct sockaddr_in uart_addr;
struct sockaddr_in group_addr;

struct sockaddr_in uart1_txaddr;
static int uart1_sockfd;

#define MULTICAST_PORT 6000
#define MULTICAST_GROUP "239.0.0.1"

uint8_t uart_busy(void)
{
    return 0;
}

void uart_init(unsigned int baudrate)
{
    struct ip_mreq mreq;

    uart_sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (uart_sockfd < 0) {
        perror("socket");
        exit(1);
    }

    group_sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (group_sockfd < 0) {
        perror("socket");
        exit(1);
    }

    bzero((char *)&uart_addr, sizeof(uart_addr));
    uart_addr.sin_family = AF_INET;
    // uart_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    uart_addr.sin_port = htons(MULTICAST_PORT + 1);
    uart_addr.sin_port = 0;
    uart_addr.sin_addr.s_addr = inet_addr(MULTICAST_GROUP);

    bzero((char *)&group_addr, sizeof(group_addr));
    group_addr.sin_family = AF_INET;
    // uart_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    group_addr.sin_port = htons(MULTICAST_PORT);
    group_addr.sin_addr.s_addr = inet_addr(MULTICAST_GROUP);


    if (bind(uart_sockfd, (struct sockaddr *) &uart_addr, sizeof(uart_addr)) < 0) {        
        perror("bind uart");
        exit(1);
    }
    
    socklen_t uart_addrlen = sizeof(uart_addr);
    getsockname(uart_sockfd, (struct sockaddr *)&uart_addr, &uart_addrlen);
    //printf("local port = %d\n", ntohs(uart_addr.sin_port));


    if (bind(group_sockfd, (struct sockaddr *) &group_addr, sizeof(group_addr)) < 0) {        
        perror("bind group");
        exit(1);
    }

    mreq.imr_multiaddr.s_addr = inet_addr(MULTICAST_GROUP);         
    mreq.imr_interface.s_addr = htonl(INADDR_ANY);         
    if (setsockopt(group_sockfd, IPPROTO_IP, IP_ADD_MEMBERSHIP,
            &mreq, sizeof(mreq)) < 0) {
        perror("setsockopt mreq");
        exit(1);
    }         
}

unsigned int uart_getc(void)
{
    uint8_t msg[10];
    struct sockaddr_in remote_addr;
    bzero((char *)&remote_addr, sizeof(remote_addr));
    socklen_t remote_addrlen = sizeof(remote_addr);
    
    int n = recvfrom(group_sockfd, msg, 1, MSG_DONTWAIT, 
            (struct sockaddr *) &remote_addr, &remote_addrlen);
    
    //int n = recv(group_sockfd, msg, 1, MSG_DONTWAIT);
    if(n > 0) {
        //printf("remote port: %d, local port: %d\n", ntohs(remote_addr.sin_port), ntohs(uart_addr.sin_port));
        if(remote_addr.sin_port != uart_addr.sin_port) {
            //printf("%X := %c := uart_getc()\n", msg[0], msg[0]);
            return msg[0];
        }
    }
    return UART_NO_DATA;
}

void uart_putc(unsigned char data)
{
    sendto(uart_sockfd, &data, 1, 0,
              (struct sockaddr *) &group_addr, sizeof(group_addr));
    //printf("uart_putc(%X := %c)\n", data, data);
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
    struct sockaddr_in rxaddr;

    uart1_sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    bzero(&rxaddr, sizeof(rxaddr));
    rxaddr.sin_family = AF_INET;
    rxaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    rxaddr.sin_port = htons(32000);
    bind(uart1_sockfd, (struct sockaddr *)&rxaddr, sizeof(rxaddr));

    bzero(&uart1_txaddr, sizeof(rxaddr));
    uart1_txaddr.sin_family = AF_INET;
    uart1_txaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    uart1_txaddr.sin_port = htons(31000);
}

unsigned int uart1_getc(void)
{
    uint8_t msg[100];
    int n = recv(uart1_sockfd,msg,1,MSG_DONTWAIT);
    if(n > 0) {
        //printf("%X := %c := uart1_getc()\n", msg[0], msg[0]);
        return msg[0];
    }
    return UART_NO_DATA;
}

void uart1_putc(unsigned char data)
{
    sendto(uart1_sockfd,&data,1,0,
                       (struct sockaddr *)&uart1_txaddr,sizeof(uart1_txaddr));
    //printf("uart1_putc(%X := %c)\n", data, data);
}

void uart1_puts(const char *s )
{
    printf("uart1_puts(%s)\n", s);
}

void uart1_puts_p(const char *progmem_s )
{
    printf("uart1_puts_P(%s)\n", progmem_s);
}
