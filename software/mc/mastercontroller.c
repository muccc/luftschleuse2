#include <stdint.h>
#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>
#include "config.h"
#include "uart.h"
#include "aes/aes.h"
#include "serial_handler.h"

/* a sample key, key must be located in RAM */
uint8_t key[]  = { 0x0, 0x0, 0x0, 0x0,
                   0x0, 0x0, 0x0, 0x0,
                   0x0, 0x0, 0x0, 0x0,
                   0x0, 0x0, 0x0, 0x0 };
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

    aes128_ctx_t ctx; /* the context where the round keys are stored */
    aes128_init(key, &ctx); /* generating the round keys from the 128 bit key */
    
    while( 1 ){
        uint8_t channel = serial_readFrame();

        if( channel == '0' ){
            if( serial_getMessageLen() == 16 ){
                uint8_t *msg = serial_getMessage();
                aes128_dec(msg, &ctx); /* decrypting the data block */
                serial_sendFrame(1, msg, 16);
                if( msg[0] == 0 ){    
                    PORTA ^= 0x01;
                }
            }
        }
        
        if( channel == '1' ){
            PORTA ^= 0x02;
        }

        //aes128_enc(data, &ctx); /* encrypting the data block */
    }

}

