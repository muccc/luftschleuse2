#include "config.h"
#include "bus_handler.h"
#include "bus_process.h"
#include "command_process.h"
#include "aes.h"
#include "timer0.h"
#include "door.h"
#include "adc.h"
#include "power_process.h"
#include "leds.h"
#include "buttons.h"

#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <util/delay.h>
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
    DDRC |= 0xC0;

    eeprom_read_block(&state, &state_ee, sizeof(state));
    
    leds_init();
    buttons_init();
    adc_init();
    power_init();
    door_init();
    aes_handler_init();
    bus_handler_init();
    bus_init();
    cmd_init();
    timer0_init();
    sei();
    
    while( 1 ){
        if( timer0_timebase ){
            timer0_timebase--;
            bus_tick();
            door_tick();
            power_tick();
            cmd_tick();
            buttons_tick();
            leds_tick();
        }
        bus_process();
        door_process();
        power_process();
    }

}

