#ifndef TIMER0_H_
#define TIMER0_H_
#include <stdint.h>
extern volatile uint8_t timer0_timebase;
void timer0_init(void);
#endif
