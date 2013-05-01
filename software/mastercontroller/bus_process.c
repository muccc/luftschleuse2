#include "bus_process.h"
#include "bus_handler.h"
#include "serial_handler.h"
#include "config.h"

// This is now done by lockd
#define BUS_AUTO_QUERY  0


#if BUS_AUTO_QUERY
static uint8_t bus_nextQueryTarget;
static uint16_t bus_nextQueryTimer;
#endif

void bus_init(void)
{
#if BUS_AUTO_QUERY
    bus_nextQueryTarget = BUS_QUERY_START;
    bus_nextQueryTimer = 0;
#endif
}

#if BUS_AUTO_QUERY
void bus_query(uint8_t address)
{
    bus_sendFrame(address, NULL, 0);
}
#endif

void bus_tick(void)
{
#if BUS_AUTO_QUERY
    if( bus_nextQueryTimer++ == (BUS_QUERY_INTERVAL/BUS_QUERY_AMOUNT) ){
        bus_query(bus_nextQueryTarget++);
        if( bus_nextQueryTarget == BUS_QUERY_START + BUS_QUERY_AMOUNT ){
            bus_nextQueryTarget = BUS_QUERY_START;
        }
        bus_nextQueryTimer = 0;
    }
#endif
}

void bus_process(void)
{
    uint8_t channel = bus_readFrame();
    uint8_t len = bus_getMessageLen();
    if( len && channel ){
        serial_sendFrame(channel, bus_getMessage(), len);
    }
}

#if BUS_AUTO_QUERY
void bus_holdoff(uint8_t holdoff)
{
    bus_nextQueryTimer += holdoff;
}
#endif

