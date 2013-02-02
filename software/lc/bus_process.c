#include "bus_process.h"
#include "bus_handler.h"
#include "command_process.h"
#include "packet.h" 
#include "config.h"
#include "aes.h"
#include "door.h"
#include "pinutils.h"

#include <util/delay.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

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

    if( channel == NODE_ADDRESS && len == 0 ){
        cmd_new(CMD_SEND_STATE, NULL);
    }else if( channel == NODE_ADDRESS && len == sizeof(packet_t) ){
        uint8_t *msg = bus_getMessage();
        aes_decrypt(msg);
        packet_t *packet = (packet_t *) msg;
        if( packet_check(packet) ){
            //TODO: check sequence number
            cmd_new(packet->cmd, packet->data);
        }
    }
}

void bus_sendPacket(packet_t *p)
{
    memcpy(p->magic, PACKET_MAGIC, sizeof(p->magic));
    //TODO: increment sequence number
    p->seq = 0;
    uint8_t *msg = (uint8_t *)p;
    aes_encrypt(msg);
    bus_sendFrame(NODE_ADDRESS, msg, sizeof(*p));
}

void bus_sendAck(bool success)
{
    packet_t p;
    memset(&p, 0, sizeof(p));

    p.cmd = CMD_ACK;
    p.data[0] = 0;

    if( success ){
        p.data[0] = 1;
    }

    bus_sendPacket(&p);
}

