/*
 *  This file is part of the luftschleuse2 project.
 *
 *  See https://github.com/muccc/luftschleuse2 for more information.
 *
 *  Copyright (C) 2013 Tobias Schneider <schneider@muc.ccc.de> 
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "bus_process.h"
#include "bus_handler.h"
#include "command_process.h"
#include "packet.h" 
#include "config.h"
#include "aes.h"
#include "door.h"
#include "pinutils.h"
#include "sequence_numbers.h"

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
        // TODO: set a timeout to prevent us getting flooded with
        // messages. Also enforce this timeout at boot-up.
        uint8_t *msg = bus_getMessage();
        aes_decrypt(msg);
        packet_t *packet = (packet_t *) msg;

        if( packet_check_magic(packet) ){
            if( sequence_numbers_check_rx(packet->seq) ){
                cmd_new(packet->cmd, packet->data);
            }else{
                uint32_t sync_seq = sequence_numbers_get_expected_rx();
                packet_t p;
                p.seq = sync_seq;
                memcpy(p.magic, PACKET_SYNC_MAGIC, sizeof(p.magic));
                uint8_t *msg = (uint8_t *)&p;
                aes_encrypt(msg);
                bus_sendFrame(NODE_ADDRESS, msg, sizeof(p));
            }
        }else if( packet_check_sync_magic(packet) ){
            sequence_numbers_set_tx(packet->seq);
        }
    }
}

void bus_sendPacket(packet_t *p)
{
    memcpy(p->magic, PACKET_MAGIC, sizeof(p->magic));
    p->seq = sequence_numbers_get_tx();
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

