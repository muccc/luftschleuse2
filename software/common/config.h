#ifndef CONFIG_H
#define CONFIG_H

#define SERIAL_BUFFERLEN    64

#define SERIAL_ESCAPE   '\\'
#define SERIAL_END      '9'

#define UART_BAUDRATE   115200

#define BUS_TX_PORT  D
#define BUS_TX_PIN   4

#define BUS_nRX_PORT  D
#define BUS_nRX_PIN   5

// B is the back door
// A is the front door
//#define NODE_ADDRESS    'A'
#define NODE_ADDRESS    'B'

#define DOOR_MODEL_1      0
#define DOOR_MODEL_2      1

// Front door is model 2
// Back door is model 1
#define DOOR_MODEL  DOOR_MODEL_1
//#define DOOR_MODEL  DOOR_MODEL_2
#endif

