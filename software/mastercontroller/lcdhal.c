#include "lcdhal.h"
#include "pinutils.h"

#include <util/delay.h>

#include <string.h>
#include <stdint.h>
#include <stdbool.h>

void lcdhal_delayms(uint8_t ms)
{
    while(ms--){
        __delay_us(1000);
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

    //while ((SSP_SSP0SR & (SSP_SSP0SR_TNF_NOTFULL | SSP_SSP0SR_BSY_BUSY)) != SSP_SSP0SR_TNF_NOTFULL);
    //SSP_SSP0DR = frame;
    //while ((SSP_SSP0SR & (SSP_SSP0SR_BSY_BUSY|SSP_SSP0SR_RNE_NOTEMPTY)) != SSP_SSP0SR_RNE_NOTEMPTY);
    /* clear the FIFO */
    //frame = SSP_SSP0DR;
}

uint8_t lcdhal_read(uint8_t data)
{
#if 0
    uint32_t op211cache=IOCON_PIO2_11;
    uint32_t op09cache=IOCON_PIO0_9;
    uint32_t dircache=GPIO_GPIO2DIR;
    IOCON_PIO2_11=IOCON_PIO2_11_FUNC_GPIO|IOCON_PIO2_11_MODE_PULLUP;
    IOCON_PIO0_9=IOCON_PIO0_9_FUNC_GPIO|IOCON_PIO0_9_MODE_PULLUP;
    gpioSetDir(SCK, 1);

    uint8_t i;

    gpioSetDir(SDA, 1);
    gpioSetValue(SCK, 0);
    gpioSetValue(CS, 0);
    delayms(1);

    gpioSetValue(SDA, 0);
    gpioSetValue(SCK, 1);
    delayms(1);
    
    for(i=0; i<8; i++){
        gpioSetValue(SCK, 0);
        delayms(1);
        if( data & 0x80 )
            gpioSetValue(SDA, 1);
        else
            gpioSetValue(SDA, 0);
        data <<= 1;
        gpioSetValue(SCK, 1);
        delayms(1);
    }
    uint8_t ret = 0;

    gpioSetDir(SDA, 0);
    for(i=0; i<8; i++){
        gpioSetValue(SCK, 0);
        delayms(1);
        ret <<= 1;
        ret |= gpioGetValue(SDA);
        gpioSetValue(SCK, 1);
        delayms(1);
    }
    gpioSetValue(SCK, 0);

    gpioSetValue(CS, 1);
    gpioSetDir(SDA, 1);
    IOCON_PIO2_11=op211cache;
    IOCON_PIO0_9=op09cache;
    GPIO_GPIO2DIR=dircache;
    delayms(1);
    return ret;
#endif
    return 0;
}


void lcdhal_init(void)
{

    //sspInit(0, sspClockPolarity_Low, sspClockPhase_RisingEdge);

    //gpioSetValue(RB_LCD_CS, 1);
    //gpioSetValue(RB_LCD_RST, 1);

    //gpioSetDir(RB_LCD_CS, gpioDirection_Output);
    //gpioSetDir(RB_LCD_RST, gpioDirection_Output);

    lcdhal_delayms(100);
    //gpioSetValue(RB_LCD_RST, 0);
    lcdhal_delayms(100);
    //gpioSetValue(RB_LCD_RST, 1);
    lcdhal_delayms(100);
}
