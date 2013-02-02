#include <avr/io.h>
#include <stdint.h>
//#include <avr/interrupt.h>
#include "config.h"
#include "adc.h"

void adc_init(void)
{
#if ADC_REF_VOLTAGE == 2560 && ADC_REF_SOURCE == INTERNAL
    ADMUX = (1<<REFS1) | (1<<REFS0);    //internal 2.56V
#elif ADC_REF_VOLTAGE == 4096 && ADC_REF_SOURCE == EXTERNAL
    ADMUX = 0;
#else
    #error No suitable reference voltage and source selected
#endif

    //ADC on, single conversation, 150khz
    ADCSRA = (1<<ADEN) | (1<<ADSC) | (1<<ADPS2) | (1<<ADPS1) | (1<<ADPS0);
}


uint16_t adc_getChannel(uint8_t channel)
{
    ADMUX &= 0xF0;
    ADMUX |= channel;
    ADCSRA |= (1<<ADSC);
    while( ADCSRA & (1<<ADSC) );

    uint32_t value = ADC;
    value = value * (uint32_t)ADC_REF_VOLTAGE;
    value = value / (uint32_t)ADC_MAX;
    return (uint16_t)value;
}

