#ifndef BUS_HANDLER
#define BUS_HANDLER

#include <stdint.h>
#include <avr/io.h>
#include "config.h"
#include "uart.h"

extern void bus_handler_init(void);
extern uint16_t bus_getMessageLen(void);
extern uint8_t * bus_getMessage(void);
extern  void bus_putcenc(uint8_t c);
extern void bus_putsenc(char * s);
extern void bus_putenc(uint8_t *d, uint16_t n);
extern  void bus_putStart(uint8_t command);
extern  void bus_putStop(void);
#ifdef USEDEBUG
extern void bus_sendFrames(uint8_t command, char *s);
#else
#define bus_sendFrames(x) {}
#endif
extern void bus_sendFramec(uint8_t command, uint8_t s);
extern void bus_sendFrame(uint8_t command, uint8_t *d, uint16_t n);
extern uint8_t bus_readFrame(void);

#endif
