#ifndef LEDS_H_
#define LEDS_H_
#include <stdint.h>

uint8_t leds_getState(void);
void leds_set(uint8_t led, uint8_t state);

#endif
