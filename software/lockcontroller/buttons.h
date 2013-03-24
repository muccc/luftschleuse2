#ifndef BUTTONS_H_
#define BUTTONS_H_
#include <stdint.h>
#include <stdbool.h>

#define BUTTON_RED_PORT              C
#define BUTTON_RED_PIN               1

typedef enum {
    BUTTON_RED = 1
} buttons_button_t;

void buttons_init(void);
void buttons_tick(void);
uint8_t buttons_getPendingButtons(void);
void buttons_clearPendingButtons(uint8_t buttons);
bool buttons_getButtonState(buttons_button_t button);
uint8_t buttons_getButtonsToggleState(void);

#endif
