#ifndef POWER_PROCESS_H_
#define POWER_PROCESS_H_
#include <stdint.h>

#define POWER_INPUT_VOLTAGE_CHANNEL 6

void power_init(void);
void power_tick(void);
void power_process(void);
uint16_t power_getInputVoltage(void);

#endif
