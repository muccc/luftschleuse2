#include "serial_process.h"
#include "serial_handler.h"
#include "bus_handler.h"
#include "aes.h"
#include "packet.h"
#include "command_process.h"
#include "display_process.h"

#include <string.h>


void serial_init(void)
{
    serial_handler_init();
}

void serial_tick(void)
{
}

void serial_process(void)
{
    uint8_t serial_channel = serial_readFrame();
    packet_t *packet;

    if( serial_channel == 0 ){
        return;
    }

    if( serial_channel == 0xFF ||
            serial_channel == 0xFE ){
        display_data(serial_channel,
                serial_getMessage(), serial_getMessageLen());
        return;
    }

    if( serial_getMessageLen() == 16 ){
        uint8_t *msg = serial_getMessage();
        if( serial_channel == '0' ){
            packet = (packet_t *) msg;

            if( packet_check_magic(packet) ){
                cmd_new(packet->cmd, packet->data);
            }
        }else if(serial_channel != 0) {
            bus_sendFrame(serial_channel, msg, 16);
        }
    }
}

void serial_sendPacket(packet_t *p)
{
    memcpy(p->magic, PACKET_MAGIC, sizeof(p->magic));
    uint8_t *msg = (uint8_t *)p;
    serial_sendFrame('0', msg, sizeof(*p));
}

