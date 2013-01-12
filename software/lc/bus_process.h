#ifndef BUS_PROCESS_H_
#define BUS_PROCESS_H_

#include "packet.h" 
void bus_init(void);
void bus_tick(void);
void bus_process(void);
void bus_sendAck(void);
void bus_sendPacket(packet_t *p);
#endif
