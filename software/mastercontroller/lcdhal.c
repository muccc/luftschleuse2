#include "lcdhal.h"
#include "pinutils.h"

#include <util/delay.h>

#include <string.h>
#include <stdint.h>
#include <stdbool.h>

void lcdhal_delayms(uint16_t ms)
{
    while(ms--){
        _delay_ms(1);
    }
}

void lcdhal_select(void)
{
    /* the LCD requires 9-Bit frames */
    PIN_CLEAR(LCD_CS);
}

void lcdhal_deselect(void)
{
    PIN_SET(LCD_CS);
    /* reset the bus to 8-Bit frames that everyone else uses */
}

void lcdhal_write(uint8_t cd, uint8_t data)
{
    uint16_t frame = 0x0;

    frame = cd << 8;
    frame |= data;
    PIN_CLEAR(LCD_SCK);
    PIN_CLEAR(LCD_CS);
    
    uint8_t i;
    for(i = 0; i < 9; i++){
        if( frame & 0x100 ){
            PIN_SET(LCD_SDA);
        }else{
            PIN_CLEAR(LCD_SDA);
        }

        PIN_SET(LCD_SCK);
        PIN_CLEAR(LCD_SCK);
        frame <<= 1;
    }
    PIN_SET(LCD_CS);
}

uint8_t lcdhal_read(uint8_t data)
{
    DDR_CONFIG_OUT(LCD_SCK);

    uint8_t i;

    DDR_CONFIG_OUT(LCD_SDA);
    PIN_CLEAR(LCD_SCK);
    PIN_CLEAR(LCD_CS);
    lcdhal_delayms(1);

    PIN_CLEAR(LCD_SDA);
    PIN_SET(LCD_SCK);
    lcdhal_delayms(1);
    
    for(i=0; i<8; i++){
        PIN_CLEAR(LCD_SCK);
        lcdhal_delayms(1);
        if( data & 0x80 )
            PIN_SET(LCD_SDA);
        else
            PIN_CLEAR(LCD_SDA);
        data <<= 1;
        PIN_SET(LCD_SCK);
        lcdhal_delayms(1);
    }
    uint8_t ret = 0;

    DDR_CONFIG_IN(LCD_SDA);
    for(i=0; i<8; i++){
        PIN_CLEAR(LCD_SCK);
        lcdhal_delayms(1);
        ret <<= 1;
        if( PIN_HIGH(LCD_SDA) ){
            ret |= 1;
        }
        PIN_SET(LCD_SCK);
        lcdhal_delayms(1);
    }
    PIN_CLEAR(LCD_SCK);

    PIN_SET(LCD_CS);
    DDR_CONFIG_OUT(LCD_SDA);
    lcdhal_delayms(1);
    return ret;
}


void lcdhal_init(void)
{
    DDR_CONFIG_OUT(LCD_SCK);
    DDR_CONFIG_OUT(LCD_SDA);
    DDR_CONFIG_OUT(LCD_RST);
    DDR_CONFIG_OUT(LCD_CS);

    PIN_CLEAR(LCD_SCK);
    PIN_SET(LCD_CS);
    PIN_CLEAR(LCD_SDA);
    PIN_SET(LCD_RST);

    lcdhal_delayms(100);
    PIN_CLEAR(LCD_RST);
    lcdhal_delayms(100);
    PIN_SET(LCD_RST);
    lcdhal_delayms(100);
}
