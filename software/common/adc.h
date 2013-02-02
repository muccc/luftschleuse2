#ifndef ADC_H_
#define ADC_H_
#include <stdint.h>

#define INTERNAL    1

#define ADC_REF_VOLTAGE     2560
#define ADC_REF_SOURCE      INTERNAL

#define ADC_RESOLUTION      10
#define ADC_MAX             ((1<<ADC_RESOLUTION)-1)

void adc_init(void);
uint16_t adc_getChannel(uint8_t channel);
#endif
