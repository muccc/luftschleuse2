#include "config.h"
#include "uart.h"
#include "serial_process.h"
#include "command_process.h"
#include "bus_handler.h"
#include "bus_process.h"
#include "aes.h"
#include "timer0.h"
#include "buttons.h"
#include "leds.h"

#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <string.h>
#include <stdint.h>


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

    bus_handler_init();
    serial_init();
    bus_init();
    cmd_init();
    buttons_init();
    leds_init();
    timer0_init();
    sei();
    
    leds_set(0,LED_SHORT_FLASH);
    leds_set(1,LED_BLINK_FAST);
    leds_set(2,LED_BLINK_SLOW);

    while( 1 ){
        if( timer0_timebase ){
            timer0_timebase--;
            bus_tick();
            serial_tick();
            buttons_tick();
            leds_tick();
        }
        bus_process();
        serial_process();

        //aes128_enc(data, &ctx); /* encrypting the data block */
    }

}

