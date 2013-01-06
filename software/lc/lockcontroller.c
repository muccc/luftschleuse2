#include "config.h"
#include "bus_handler.h"
#include "bus_process.h"
#include "command_process.h"
#include "aes.h"
#include "timer0.h"

#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <string.h>
#include <stdint.h>

typedef struct{
    uint32_t seq;
    int locked:1;
}__attribute__((packed)) state_t;

state_t state_ee EEMEM = {.seq=3, .locked=1};
state_t state;

int main(void)
{
    wdt_disable();
    /* Clear WDRF in MCUSR */
    MCUSR &= ~(1<<WDRF);
    /* Write logical one to WDCE and WDE */
    /* Keep old prescaler setting to prevent unintentional time-out */
    WDTCSR |= (1<<WDCE) | (1<<WDE);
    /* Turn off WDT */
    WDTCSR = 0x00;
    DDRA |= 0x07;
    PORTA &= ~7;
    DDRC |= 0x07;

    eeprom_read_block(&state, &state_ee, sizeof(state));
    aes_handler_init();
    bus_handler_init();
    bus_init();
    cmd_init();
    timer0_init();
    sei();
    uint16_t foo = 0;   
    while( 1 ){
        if( timebase ){
            timebase--;
            bus_tick();
            if( foo++ == 1000 ){
                foo = 0;
            }

        }
        bus_process();
        //aes128_enc(data, &ctx); /* encrypting the data block */
    }

}

