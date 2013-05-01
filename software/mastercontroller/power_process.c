#include "power_process.h"
#include "adc.h"
#include <stdint.h>

static uint16_t power_inputVoltage;

static void power_update(void);

void power_init(void)
{
    power_update();
}

void power_tick(void)
{
    static uint16_t c = 0;
    if( c-- == 0 ){
        c = 500;
        power_update();
    }
}

void power_process(void)
{
}

uint16_t power_getInputVoltage(void)
{
    return power_inputVoltage;
}

static void power_update(void)
{
    power_inputVoltage = adc_getChannel(POWER_INPUT_VOLTAGE_CHANNEL); 
    power_inputVoltage *= 11;
}
