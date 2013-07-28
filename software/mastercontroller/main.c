#include "timer0.h"
#include "mastercontroller.h"

#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>

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
    
    mastercontroller_init();
    timer0_init();
    sei();
    
    while( 1 ){
        if( timer0_timebase ){
            timer0_timebase--;
            mastercontroller_tick();
        }
        mastercontroller_process();
    }
}

