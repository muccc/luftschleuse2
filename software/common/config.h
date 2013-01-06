#ifndef CONFIG_H
#define CONFIG_H

#define SERIAL_BUFFERLEN    64

#define SERIAL_ESCAPE   '\\'
#define SERIAL_END      '9'

#define UART_BAUDRATE   115200
#define UART_USE_0

#define BUS_TX_PORT  D
#define BUS_TX_PIN   4

#define BUS_nRX_PORT  D
#define BUS_nRX_PIN   5

#define BUS_QUERY_INTERVAL      500
#define BUS_QUERY_START         'A'
#define BUS_QUERY_AMOUNT        3

#endif

