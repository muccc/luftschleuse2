#include "bus_process.h"
#include "bus_handler.h"
#include "serial_handler.h"
#include "config.h"

static uint8_t bus_nextQueryTarget;
static uint16_t bus_nextQueryTimer;

void bus_init(void)
{
    bus_nextQueryTarget = BUS_QUERY_START;
    bus_nextQueryTimer = 0;
}

void bus_query(uint8_t address)
{
    bus_sendFrame(address, NULL, 0);
}

void bus_tick(void)
{
    if( bus_nextQueryTimer++ == (BUS_QUERY_INTERVAL/BUS_QUERY_AMOUNT) ){
        bus_query(bus_nextQueryTarget++);
        if( bus_nextQueryTarget == BUS_QUERY_START + BUS_QUERY_AMOUNT ){
            bus_nextQueryTarget = BUS_QUERY_START;
        }
        bus_nextQueryTimer = 0;
    }
}

void bus_process(void)
{
    uint8_t channel = bus_readFrame();
    uint8_t len = bus_getMessageLen();
    if( len && channel ){
        serial_sendFrame(channel, bus_getMessage(), len);
    }
}

void bus_holdoff(uint8_t holdoff)
{
    bus_nextQueryTimer += holdoff;
}
