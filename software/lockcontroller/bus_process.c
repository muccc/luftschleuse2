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

#if DEBUG
#include <stdio.h>
void print_packet(uint8_t *packet)
{
    uint8_t i = 0;
    for(i = 0; i < 16; i++) {
        printf("%02X ", packet[i]);
    }
    printf("\n");
}
#else
#define printf(...)
#define print_packet(x)
#endif

#define BUS_LEAKY_BUCKET_MAX        10
#define BUS_LEAKY_BUCKET_PERIOD_MS     250

static uint16_t bus_leaky_bucket;
static uint32_t bus_accepted_packets;
static uint32_t bus_rejected_packets;

void bus_init(void)
{
    bus_leaky_bucket = 0;
    bus_accepted_packets = 0;
    bus_rejected_packets = 0;
}

void bus_tick(void)
{
    static uint16_t leaky_bucket_timer = 0;
    if(leaky_bucket_timer-- == 0) {
        leaky_bucket_timer = BUS_LEAKY_BUCKET_PERIOD_MS;
        if(bus_leaky_bucket) {
            bus_leaky_bucket--;
        }
    }
}

void bus_process(void)
{
    uint8_t channel = bus_readFrame();
    uint8_t len = bus_getMessageLen();


    if( channel == NODE_ADDRESS && len == sizeof(packet_t) ){
        if(bus_leaky_bucket > BUS_LEAKY_BUCKET_MAX) {
            bus_rejected_packets++;
            return;
        }

        printf("Got packet\n");
        
        uint8_t *msg = bus_getMessage();
        aes_decrypt(msg);
        print_packet(msg);
        packet_t *packet = (packet_t *) msg;

        if( packet_check_magic(packet) ){
            /*
             * Do not allow more than one packet every 250 ms
             */
            bus_leaky_bucket++;;
            bus_accepted_packets++;
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
        }else{
            printf("bad magic\n");
        }
    }
}

void bus_sendPacket(packet_t *p)
{
    memcpy(p->magic, PACKET_MAGIC, sizeof(p->magic));
    p->seq = sequence_numbers_get_tx();
    sequence_numbers_increment_tx();
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

uint32_t bus_get_accepted_packets(void)
{
    return bus_accepted_packets;
}

uint32_t bus_get_rejected_packets(void)
{
    return bus_rejected_packets;
}


