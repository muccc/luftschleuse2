#ifndef BUTTONS_H_
#define BUTTONS_H_
#include <stdint.h>
#include <stdbool.h>

#define BUTTON_0_PORT              C
#define BUTTON_0_PIN               1

typedef enum {
    BUTTON_0 = 1,
    BUTTON_1 = 2        // "Soft" button of the bell code...
} buttons_button_t;

void buttons_init(void);
void buttons_tick(void);
#if 0
uint8_t buttons_getPendingButtons(void);
void buttons_clearPendingButtons(uint8_t buttons);
uint8_t buttons_getButtonsToggleState(void);
#endif

//bool buttons_getButtonState(buttons_button_t button);
uint8_t buttons_getButtonsLatchedState(void);

#endif
