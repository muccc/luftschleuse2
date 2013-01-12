#include "bus_process.h"
#include "bus_handler.h"
#include "command_process.h"
#include "packet.h" 
#include "config.h"
#include <util/delay.h>
#include <string.h>

void bus_init(void)
{
}

void bus_tick(void)
{
}

void bus_process(void)
{
    uint8_t channel = bus_readFrame();
    uint8_t len = bus_getMessageLen();
    if( channel == NODE_ADDRESS){
        cmd_new(CMD_SEND_STATE, NULL);
    }
}

void bus_sendPacket(packet_t *p)
{
    memcpy(p->magic, "SESAME", 6);
    //TODO: increment sequence number
    p->seq = 0;
    _delay_ms(1);
    bus_sendFrame(NODE_ADDRESS, (uint8_t *)p, sizeof(*p));

}
