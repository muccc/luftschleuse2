#include <stdint.h>
#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>
#include "config.h"
#include "uart.h"
#include "aes/aes.h"
#include "serial_handler.h"

/* a sample key, key must be located in RAM */
uint8_t key[]  = { 0x01, 0x23, 0x45, 0x67,
                   0x89, 0xAB, 0xCD, 0xEF,
                   0x01, 0x23, 0x45, 0x67,
                   0x89, 0xAB, 0xCD, 0xEF };
/* sample data, you can encrypt what you want but keep in mind that only 128 bits (not less not more) get encrypted*/
uint8_t data[] = { 0x01, 0x02, 0x03, 0x04,
                   0x05, 0x06, 0x07, 0x08,
                   0x09, 0x0A, 0x0B, 0x0C,
                   0x0D, 0x0E, 0x0F, 0x00 };
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

    uart1_init(UART_BAUD_SELECT(UART_BAUDRATE, F_CPU));
    sei();

    while( 1 ){
        uint8_t channel = serial_readFrame();
        if( channel == '0' ){
            PORTA ^= 0x01;
        }
        
        if( channel == '1' ){
            PORTA ^= 0x02;
        }

        //aes128_ctx_t ctx; /* the context where the round keys are stored */
        //aes128_init(key, &ctx); /* generating the round keys from the 128 bit key */
        //aes128_enc(data, &ctx); /* encrypting the data block */
        //aes128_dec(data, &ctx); /* decrypting the data block */
    }

}

