#include "config.h"
#include "uart.h"
#include "serial_handler.h"
#include "serial_process.h"
#include "command_process.h"
#include "bus_handler.h"
#include "bus_process.h"
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

volatile uint8_t tick = 0;



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
    DDRC |= 0xC0;

    eeprom_read_block(&state, &state_ee, sizeof(state));
    serial_handler_init();
    aes_handler_init();
    bus_handler_init();
    serial_init();
    bus_init();
    cmd_init();
    timer0_init();
    sei();
    
    uint16_t foo = 0;
    while( 1 ){
        if( timebase ){
            timebase--;
            bus_tick();
            serial_tick();
            if( foo++ == 1000 ){
                //PORTC ^= 0xC0;
                foo = 0;
            }
        }
        bus_process();
        serial_process();

        //aes128_enc(data, &ctx); /* encrypting the data block */
    }

}

