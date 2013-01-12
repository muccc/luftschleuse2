#ifndef BUTTONS_H_
#define BUTTONS_H_
#include <stdint.h>

uint8_t buttons_getPendingButtons(void);
void buttons_clearButtons(uint8_t buttons);
#endif
