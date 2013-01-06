#include "serial_process.h"
#include "serial_handler.h"
#include "bus_handler.h"
#include "aes.h"
#include "packet.h"

#include <string.h>


void serial_init(void)
{
}

void serial_tick(void)
{
}

static uint8_t serial_checkMessage(uint8_t *msg)
{
    if( memcmp("SESAME", msg+10, 6) == 0 )
        return 1;
    return 0;
}

void serial_process(void)
{
    uint8_t serial_channel = serial_readFrame();
    uint8_t serial_len = serial_getMessageLen();
    packet_t packet;

    if( serial_getMessageLen() == 16 ){
        uint8_t *msg = serial_getMessage();
        if( serial_channel == '0' ){
            aes_decrypt(msg);
            //serial_sendFrame(1, msg, 16);
            if( serial_checkMessage(msg) ){
                memcpy(&packet, msg, 16);
                cmd_new(packet.cmd, packet.data);
                //if( packet.seq == state.seq ){    
                //    PORTA ^= 0x01;
                //}
            }
        }else if(serial_channel != 0) {
            bus_sendFrame(serial_channel, msg, 16);
        }
    }
    
}
