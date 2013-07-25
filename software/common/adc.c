/*
 *  This file is part of the luftschleuse2 project.
 *
 *  See https://github.com/muccc/luftschleuse2 for more information.
 *
 *  Copyright (C) 2013 Tobias Schneider <schneider@muc.ccc.de> 
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
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

